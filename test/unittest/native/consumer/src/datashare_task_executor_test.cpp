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

#define LOG_TAG "datashare_task_executor_test"

#include "datashare_task_executor.h"

#include <gtest/gtest.h>

#include <chrono>

#include "block_data.h"
#include "datashare_common.h"
#include "datashare_connection.h"
#include "datashare_log.h"
#include "datashare_sa_connection.h"
#include "uri.h"

namespace OHOS {
namespace DataShare {
using namespace testing::ext;
std::string g_dataShareUri = "datashare:///com.acts.datasharetest";
constexpr int32_t TEST_SA_ID = 1001;
constexpr int32_t TEST_WAIT_TIME = 1;
constexpr int32_t CONNECTION_COUNT = 3;
constexpr int32_t SA_CONNECTION_COUNT = 2;

class DataShareTaskExecutorTest : public testing::Test {
public:
    static void SetUpTestCase(void){};
    static void TearDownTestCase(void){};
    void SetUp(){};
    void TearDown(){};
};

class RemoteObjectTest : public IRemoteObject {
public:
    explicit RemoteObjectTest(std::u16string descriptor) : IRemoteObject(descriptor)
    {
    }
    ~RemoteObjectTest()
    {
    }

    int32_t GetObjectRefCount()
    {
        return 0;
    }
    int SendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
    {
        return 0;
    }
    bool AddDeathRecipient(const sptr<DeathRecipient> &recipient)
    {
        return true;
    }
    bool RemoveDeathRecipient(const sptr<DeathRecipient> &recipient)
    {
        return true;
    }
    int Dump(int fd, const std::vector<std::u16string> &args)
    {
        return 0;
    }
};

/**
 * @tc.name: DataShareTaskExecutor_GetInstance_Identity_001
 * @tc.desc: Verify that DataShareTaskExecutor is a process-wide singleton whose GetExecutor lazily
 *           creates one executor pool and keeps returning the same instance on later calls.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. DataShareTaskExecutor can be accessed through GetInstance in the test process.
    2. The expected executor thread name DATASHARE_EXECUTOR_NAME is predefined.
 * @tc.step:
    1. Get the DataShareTaskExecutor instance twice and compare their addresses.
    2. Call GetExecutor twice and compare the returned executors.
    3. Check the thread name of the executor pool.
 * @tc.expect:
    1. Both GetInstance calls return the same instance.
    2. Both GetExecutor calls return the same non-null executor.
    3. The executor pool thread name is DATASHARE_EXECUTOR_NAME.
 */
HWTEST_F(DataShareTaskExecutorTest, DataShareTaskExecutor_GetInstance_Identity_001, TestSize.Level0)
{
    LOG_INFO("DataShareTaskExecutor_GetInstance_Identity_001::Start");
    auto &instance = DataShareTaskExecutor::GetInstance();
    EXPECT_EQ(&instance, &DataShareTaskExecutor::GetInstance());
    auto executorFirst = instance.GetExecutor();
    ASSERT_NE(executorFirst, nullptr);
    auto executorSecond = instance.GetExecutor();
    EXPECT_EQ(executorFirst, executorSecond);
    EXPECT_EQ(executorFirst->pool_.threadName_, DATASHARE_EXECUTOR_NAME);
    LOG_INFO("DataShareTaskExecutor_GetInstance_Identity_001::End");
}

/**
 * @tc.name: DataShareTaskExecutor_SetExecutor_ReplaceAndRestore_002
 * @tc.desc: Verify that SetExecutor replaces the current executor pool and that the original pool
 *           can be restored afterwards.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. DataShareTaskExecutor can be accessed through GetInstance in the test process.
    2. An ExecutorPool can be created for injection.
 * @tc.step:
    1. Save the current executor pool through GetExecutor.
    2. Create a test ExecutorPool and inject it with SetExecutor.
    3. Call GetExecutor and compare it with the injected pool.
    4. Restore the original pool with SetExecutor and call GetExecutor again.
 * @tc.expect:
    1. GetExecutor returns the injected pool after SetExecutor.
    2. GetExecutor returns the original pool after it is restored.
 */
HWTEST_F(DataShareTaskExecutorTest, DataShareTaskExecutor_SetExecutor_ReplaceAndRestore_002, TestSize.Level0)
{
    LOG_INFO("DataShareTaskExecutor_SetExecutor_ReplaceAndRestore_002::Start");
    auto &instance = DataShareTaskExecutor::GetInstance();
    auto originalExecutor = instance.GetExecutor();
    ASSERT_NE(originalExecutor, nullptr);
    auto testExecutor = std::make_shared<ExecutorPool>(1, 0, "DsTaskExecTest");
    instance.SetExecutor(testExecutor);
    EXPECT_EQ(instance.GetExecutor(), testExecutor);
    instance.SetExecutor(originalExecutor);
    EXPECT_EQ(instance.GetExecutor(), originalExecutor);
    LOG_INFO("DataShareTaskExecutor_SetExecutor_ReplaceAndRestore_003::End");
}

/**
 * @tc.name: DataShareTaskExecutor_Execute_TaskRuns_003
 * @tc.desc: Verify that a task submitted to the shared executor pool is actually executed.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. DataShareTaskExecutor can be accessed through GetInstance in the test process.
    2. BlockData can be used to wait for the task result.
 * @tc.step:
    1. Get the shared executor pool through GetExecutor.
    2. Submit a task that sets a BlockData value.
    3. Wait for the BlockData value.
 * @tc.expect:
    1. Execute returns a valid task id.
    2. The task runs on the shared pool and the BlockData value is set within the wait time.
 */
HWTEST_F(DataShareTaskExecutorTest, DataShareTaskExecutor_Execute_TaskRuns_003, TestSize.Level0)
{
    LOG_INFO("DataShareTaskExecutor_Execute_TaskRuns_003::Start");
    auto &instance = DataShareTaskExecutor::GetInstance();
    auto executor = instance.GetExecutor();
    ASSERT_NE(executor, nullptr);
    auto result = std::make_shared<OHOS::BlockData<bool, std::chrono::seconds>>(TEST_WAIT_TIME, false);
    auto taskId = executor->Execute([result]() { result->SetValue(true); });
    EXPECT_NE(taskId, ExecutorPool::INVALID_TASK_ID);
    EXPECT_TRUE(result->GetValue());
    LOG_INFO("DataShareTaskExecutor_Execute_TaskRuns_003::End");
}

/**
 * @tc.name: DataShareTaskExecutor_MultipleConnections_SharedExecutor_004
 * @tc.desc: Verify that multiple DataShareConnection and DataShareSAConnection instances all use the
 *           single executor pool shared through DataShareTaskExecutor instead of creating their own.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. DataShareTaskExecutor can be accessed through GetInstance in the test process.
    2. DataShareConnection and DataShareSAConnection can be instantiated in the test environment and
       their pool-using paths fail fast without a real provider.
 * @tc.step:
    1. Save the shared executor pool through GetExecutor.
    2. Create several DataShareConnection instances and trigger OnAbilityDisconnectDone on each of them.
    3. Create several DataShareSAConnection instances and call GetDataShareProxy on each of them.
    4. Call GetExecutor again and compare it with the saved executor.
 * @tc.expect:
    1. All connection activities keep using the same executor pool instance, whose thread name stays
       DATASHARE_EXECUTOR_NAME.
 */
HWTEST_F(DataShareTaskExecutorTest, DataShareTaskExecutor_MultipleConnections_SharedExecutor_004, TestSize.Level0)
{
    LOG_INFO("DataShareTaskExecutor_MultipleConnections_SharedExecutor_004::Start");
    auto &instance = DataShareTaskExecutor::GetInstance();
    auto sharedExecutor = instance.GetExecutor();
    ASSERT_NE(sharedExecutor, nullptr);
    Uri uri(g_dataShareUri);
    std::u16string tokenString = u"OHOS.DataShare.IDataShare";
    sptr<IRemoteObject> token = new (std::nothrow) RemoteObjectTest(tokenString);
    ASSERT_NE(token, nullptr);
    AppExecFwk::ElementName element("deviceId", "bundleName", "abilityName");
    for (int32_t i = 0; i < CONNECTION_COUNT; ++i) {
        auto connection = std::make_shared<DataShareConnection>(uri, token);
        ASSERT_NE(connection, nullptr);
        ASSERT_TRUE(connection->Init());
        connection->OnAbilityDisconnectDone(element, 0);
    }
    for (int32_t i = 0; i < SA_CONNECTION_COUNT; ++i) {
        DataShareSAConnection saConnection(uri, TEST_SA_ID, TEST_WAIT_TIME);
        auto proxy = saConnection.GetDataShareProxy(uri, token);
        EXPECT_EQ(proxy, nullptr);
    }
    EXPECT_EQ(instance.GetExecutor(), sharedExecutor);
    EXPECT_EQ(instance.GetExecutor()->pool_.threadName_, DATASHARE_EXECUTOR_NAME);
    LOG_INFO("DataShareTaskExecutor_MultipleConnections_SharedExecutor_004::End");
}
} // namespace DataShare
} // namespace OHOS
