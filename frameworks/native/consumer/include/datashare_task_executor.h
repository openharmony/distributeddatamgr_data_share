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

#ifndef DATASHARE_TASK_EXECUTOR_H
#define DATASHARE_TASK_EXECUTOR_H

#include <chrono>
#include <functional>
#include <memory>
#include <mutex>

#include "executor_pool.h"

namespace OHOS::DataShare {
/**
 * @brief Process-wide executor pool shared by all DataShare components.
 * Components such as DataShareConnection, DataShareSAConnection and
 * GeneralControllerServiceImpl must get the executor from this singleton
 * instead of creating their own ExecutorPool, otherwise every component
 * instance would hold a separate pool and multiply the threads of the
 * process.
 */
class DataShareTaskExecutor {
public:
    using TaskId = ExecutorPool::TaskId;
    using Task = std::function<void()>;
    using Duration = std::chrono::steady_clock::duration;
    static constexpr TaskId INVALID_TASK_ID = ExecutorPool::INVALID_TASK_ID;

    static DataShareTaskExecutor &GetInstance();
    void SetExecutor(std::shared_ptr<ExecutorPool> executor);
    std::shared_ptr<ExecutorPool> GetExecutor();

private:
    DataShareTaskExecutor();
    ~DataShareTaskExecutor();
    std::mutex mutex_;
    std::shared_ptr<ExecutorPool> pool_;
};
} // namespace OHOS::DataShare
#endif // DATASHARE_TASK_EXECUTOR_H
