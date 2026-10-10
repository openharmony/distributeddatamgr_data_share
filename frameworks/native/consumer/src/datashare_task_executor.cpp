/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#define LOG_TAG "datashare_task_executor"

#include "datashare_task_executor.h"

#include "datashare_common.h"
#include "datashare_log.h"

namespace OHOS::DataShare {
DataShareTaskExecutor &DataShareTaskExecutor::GetInstance()
{
    static DataShareTaskExecutor instance;
    return instance;
}

DataShareTaskExecutor::DataShareTaskExecutor()
{
}

DataShareTaskExecutor::~DataShareTaskExecutor()
{
    std::lock_guard<std::mutex> lock(mutex_);
    pool_ = nullptr;
}

std::shared_ptr<ExecutorPool> DataShareTaskExecutor::GetExecutor()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (pool_ == nullptr) {
        pool_ = std::make_shared<ExecutorPool>(MAX_THREADS, MIN_THREADS, DATASHARE_EXECUTOR_NAME);
    }
    return pool_;
}

void DataShareTaskExecutor::SetExecutor(std::shared_ptr<ExecutorPool> executor)
{
    std::lock_guard<std::mutex> lock(mutex_);
    pool_ = executor;
}
} // namespace OHOS::DataShare
