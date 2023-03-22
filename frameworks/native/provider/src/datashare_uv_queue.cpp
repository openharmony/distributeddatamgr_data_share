/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "datashare_uv_queue.h"
#include <thread>
#include "datashare_log.h"

namespace OHOS {
namespace DataShare {
constexpr int WAIT_TIME = 3;
constexpr int SLEEP_TIME = 100;
constexpr int TRY_TIMES = 20;
DataShareUvQueue::DataShareUvQueue(napi_env env)
    : env_(env)
{
    napi_get_uv_event_loop(env, &loop_);
}

void DataShareUvQueue::SyncCall(NapiVoidFunc func, NapiBoolFunc retFunc)
{
    uv_work_t* work = new (std::nothrow) uv_work_t;
    if (work == nullptr) {
        return;
    }
    work->data = new UvEntry {env_, std::move(func), false, false, {}, {}, std::move(retFunc)};
    auto status = uv_queue_work(
        loop_, work, [](uv_work_t* work) {},
        [](uv_work_t* work, int uvstatus) {
            if (work == nullptr || work->data == nullptr) {
                LOG_ERROR("invalid work or work->data.");
                return;
            }
            auto *entry = static_cast<UvEntry*>(work->data);
            std::unique_lock<std::mutex> lock(entry->mutex);
            if (entry->func) {
                entry->func();
            }
            entry->done = true;
            if (entry->purge) {
                DataShareUvQueue::Purge(work);
            } else {
                entry->condition.notify_all();
            }
        });
    if (status != napi_ok) {
        LOG_ERROR("queue work failed");
        DataShareUvQueue::Purge(work);
        return;
    }

    bool noNeedPurge = false;
    auto *uvEntry = static_cast<UvEntry*>(work->data);
    {
        if (uvEntry == nullptr) {
            LOG_ERROR("invalid uvEntry.");
            return;
        }
        std::unique_lock<std::mutex> lock(uvEntry->mutex);
        if (uvEntry->condition.wait_for(lock, std::chrono::seconds(WAIT_TIME), [uvEntry] { return uvEntry->done; })) {
            LOG_INFO("function ended successfully");
        }
        CheckFuncAndExec(uvEntry->retFunc);
        if (!uvEntry->done && !uv_cancel((uv_req_t*)&work)) {
            LOG_ERROR("uv_cancel failed.");
            uvEntry->purge = true;
            noNeedPurge = true;
        }
    }

    if (!noNeedPurge) {
        DataShareUvQueue::Purge(work);
    }
}

void DataShareUvQueue::Purge(uv_work_t* work)
{
    if (work == nullptr) {
        LOG_ERROR("invalid work");
        return;
    }
    if (work->data == nullptr) {
        LOG_ERROR("invalid work->data");
        delete work;
        return;
    }

    auto *entry = static_cast<UvEntry*>(work->data);

    delete entry;
    entry = nullptr;

    delete work;
    work = nullptr;
}

void DataShareUvQueue::CheckFuncAndExec(NapiBoolFunc retFunc)
{
    if (retFunc) {
        int tryTimes = TRY_TIMES;
        while (retFunc() != true && tryTimes > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME));
            tryTimes--;
        }
    }
}
} // namespace DataShare
} // namespace OHOS