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
#include <chrono>
#include "datashare_log.h"
#include "event_runner.h"

namespace OHOS {
namespace DataShare {
using namespace std::chrono;
constexpr int WAIT_TIME = 3;
constexpr int SLEEP_TIME = 1;
constexpr int TRY_TIMES = 2000;
DataShareUvQueue::DataShareUvQueue(napi_env env)
    : naipEnv_(env)
{
    napi_get_uv_event_loop(env, &loop_);
}

void DataShareUvQueue::LambdaForWork(TaskEntry* taskEntry)
{
    if (taskEntry == nullptr) {
        LOG_ERROR("invalid taskEntry.");
        return;
    }
    {
        std::unique_lock<std::mutex> lock(taskEntry->mutex);
        if (taskEntry->func) {
            taskEntry->func();
        }
        taskEntry->done = true;
        taskEntry->condition.notify_all();
    }
    if (taskEntry->count.fetch_sub(1) == 1) {
        delete taskEntry;
        taskEntry = nullptr;
    }
}

void DataShareUvQueue::JsSyncCall(VoidFunc func, BoolFunc retFunc)
{
    auto *taskEntry = new TaskEntry {std::move(func), false, {}, {}, std::atomic<int>(1)};
    {
        std::unique_lock<std::mutex> lock(taskEntry->mutex);
        taskEntry->count.fetch_add(1);
        auto task = [taskEntry]() {
            DataShareUvQueue::LambdaForWork(taskEntry);
        };
        if (napi_status::napi_ok != napi_send_event(naipEnv_, task, napi_eprio_immediate)) {
            LOG_ERROR("napi_send_event task failed");
            delete taskEntry;
            taskEntry = nullptr;
            return;
        }
        if (taskEntry->condition.wait_for(lock, std::chrono::seconds(WAIT_TIME),
            [taskEntry] { return taskEntry->done; })) {
            auto time = static_cast<uint64_t>(duration_cast<milliseconds>(
                system_clock::now().time_since_epoch()).count());
            LOG_INFO("function ended successfully. times %{public}" PRIu64 ".", time);
        }
    }
    CheckFuncAndExec(retFunc);
    if (taskEntry->count.fetch_sub(1) == 1) {
        delete taskEntry;
        taskEntry = nullptr;
    }
}

void DataShareUvQueue::StsSyncCall(VoidFunc func, BoolFunc retFunc)
{
    // todo
}

void DataShareUvQueue::CheckFuncAndExec(BoolFunc retFunc)
{
    if (retFunc) {
        int tryTimes = TRY_TIMES;
        while (retFunc() != true && tryTimes > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME));
            tryTimes--;
        }
        if (tryTimes <= 0) {
            LOG_ERROR("function execute timeout.");
        }
    }
}
} // namespace DataShare
} // namespace OHOS