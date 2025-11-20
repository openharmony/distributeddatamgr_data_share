/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#define LOG_TAG "abnormal_branch_test"

#include <gtest/gtest.h>
#include <unistd.h>
#include <memory>
#include "accesstoken_kit.h"
#include "ams_mgr_proxy.h"
#include "data_share_manager_impl.h"
#include "data_share_service_proxy.h"
#include "datashare_helper.h"
#include "datashare_log.h"
#include "datashare_template.h"
#include "hap_token_info.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "token_setproc.h"
#include "datashare_errno.h"
#include "published_data_subscriber_manager.h"
#include "rdb_subscriber_manager.h"
#include "ishared_result_set_stub.h"
#include "message_parcel.h"
#include "ikvstore_data_service.h"
#include "shared_block.h"
#include "datashare_block_writer_impl.h"
#include "datashare_connection.h"
#include "ikvstore_data_service_mock.h"
#include "general_controller_service_impl.h"
#include "persistent_data_controller.h"
#include "published_data_controller.h"

namespace OHOS {
namespace DataShare {
using namespace testing::ext;
using namespace OHOS::Security::AccessToken;
constexpr int INVALID_VALUE = -1;

class AbnormalBranchTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void AbnormalBranchTest::SetUpTestCase(void)
{
}
void AbnormalBranchTest::TearDownTestCase(void)
{
}
void AbnormalBranchTest::SetUp(void)
{
}
void AbnormalBranchTest::TearDown(void)
{
}

// Used for mock DataShareKvServiceProxy in order to
// cover the situation that DataShareManagerImpl::GetServiceProxy() == nullptr;
void DataShareManagerImplHelper()
{
    auto helper = DataShareManagerImpl::GetInstance();
    helper->dataShareService_ = nullptr;
    auto manager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    auto remoteObject = manager->CheckSystemAbility(DISTRIBUTED_KV_DATA_SERVICE_ABILITY_ID);
    sptr<MockDataShareKvServiceProxy> mockProxy = sptr<MockDataShareKvServiceProxy>
        (new MockDataShareKvServiceProxy(remoteObject));
    EXPECT_CALL(*mockProxy, GetFeatureInterface(testing::_))
        .WillOnce(testing::Return(nullptr));
    
    helper->dataMgrService_ = (sptr<DataShareKvServiceProxy>)mockProxy;
}

/**
 * @tc.name: AbnormalBranchTest_shareBlock_Null_Test_001
 * @tc.desc: Verify operations on DataShareBlockWriterImpl when share block is null
 * @tc.type: FUNC
 * @tc.precon:
    1. DataShareBlockWriterImpl class is implemented
    2. E_ERROR error code is predefined for operation failures
    3. DataShareBlockWriterImpl supports AllocRow and multiple Write method overloads
 * @tc.step:
    1. Create a DataShareBlockWriterImpl instance (share block is null by default)
    2. Call AllocRow method and record the return value
    3. Call Write(int) method and record the return value
    4. Call Write(int, int64_t), Write(int, double) and other Write overloads
    5. Check the return value of each called operation
 * @tc.expect:
    1. All called operations (AllocRow, all Write overloads) return E_ERROR
 */
HWTEST_F(AbnormalBranchTest, AbnormalBranchTest_shareBlock_Null_Test_001, TestSize.Level0)
{
    LOG_INFO("AbnormalBranchTest_shareBlock_Null_Test_001::Start");
    DataShareBlockWriterImpl impl;
    int result = impl.AllocRow();
    EXPECT_EQ(result, E_ERROR);
    result = impl.Write(1);
    EXPECT_EQ(result, E_ERROR);
    int64_t intValue = 0;
    result = impl.Write(1, intValue);
    EXPECT_EQ(result, E_ERROR);
    double doubleValue = 0.0;
    result = impl.Write(1, doubleValue);
    EXPECT_EQ(result, E_ERROR);
    uint8_t *unitValue = nullptr;
    result = impl.Write(1, unitValue, 0);
    EXPECT_EQ(result, E_ERROR);
    char *charValue = nullptr;
    result = impl.Write(1, charValue, 0);
    EXPECT_EQ(result, E_ERROR);
    LOG_INFO("AbnormalBranchTest_shareBlock_Null_Test_001::End");
}

/**
 * @tc.name: AbnormalBranchTest_ResultSetStubNull_Test_001
 * @tc.desc: Verify ISharedResultSetStub behavior when input parameters are null
 * @tc.type: FUNC
 * @tc.precon:
    1. ISharedResultSetStub class is implemented, supporting null-parameter constructor
    2. CreateStub method of ISharedResultSetStub accepts DataShareResultSet pointer and MessageParcel
    3. MessageParcel class is implemented and can be initialized as a valid instance
 * @tc.step:
    1. Create ISharedResultSetStub instance with null constructor parameter
    2. Initialize a null std::shared_ptr<DataShareResultSet> (named result)
    3. Create a valid MessageParcel instance (named parcel)
    4. Call stub.CreateStub(result, parcel) and get the returned ISharedResultSet pointer
 * @tc.expect:
    1. The returned ISharedResultSet pointer from CreateStub is nullptr
 */
HWTEST_F(AbnormalBranchTest, AbnormalBranchTest_ResultSetStubNull_Test_001, TestSize.Level0)
{
    LOG_INFO("AbnormalBranchTest_ResultSetStubNull_Test_001::Start");
    ISharedResultSetStub stub(nullptr);
    std::shared_ptr<DataShareResultSet> result = nullptr;
    OHOS::MessageParcel parcel;
    sptr<ISharedResultSet> resultSet = stub.CreateStub(result, parcel);
    EXPECT_EQ(resultSet, nullptr);
    LOG_INFO("AbnormalBranchTest_ResultSetStubNull_Test_001::End");
}

/**
 * @tc.name: AbnormalBranchTest_RegisterClientDeathObserverNull_Test_001
 * @tc.desc: Verify RegisterClientDeathObserver with null observer
 * @tc.type: FUNC
 * @tc.precon:
    1. DataShareKvServiceProxy class is implemented, supporting null-parameter constructor
    2. RegisterClientDeathObserver method accepts std::string appId and null observer
    3. Method return value -1 is predefined for registration failure
 * @tc.step:
    1. Create DataShareKvServiceProxy instance with null constructor parameter
    2. Initialize an empty std::string appId
    3. Call proxy.RegisterClientDeathObserver(appId, nullptr) and record the return value
 * @tc.expect:
    1. The return value of RegisterClientDeathObserver is -1 (failure)
 */
HWTEST_F(AbnormalBranchTest, AbnormalBranchTest_RegisterClientDeathObserverNull_Test_001, TestSize.Level0)
{
    LOG_INFO("AbnormalBranchTest_RegisterClientDeathObserverNull_Test_001::Start");
    DataShareKvServiceProxy proxy(nullptr);
    std::string appId;
    int32_t result = proxy.RegisterClientDeathObserver(appId, nullptr);
    EXPECT_EQ(result, -1);
    LOG_INFO("AbnormalBranchTest_RegisterClientDeathObserverNull_Test_001::End");
}

/**
 * @tc.name: AbnormalBranchTest_mReadOnlyInvalid_Test_001
 * @tc.desc: Verify invalid operations on read-only SharedBlock
 * @tc.type: FUNC
 * @tc.precon:
    1. SharedBlock class supports read-only initialization via constructor (bool readOnly parameter)
    2. SHARED_BLOCK_INVALID_OPERATION error code is predefined for read-only modification failures
    3. Ashmem class is implemented, supporting CreateAshmem and MapReadAndWriteAshmem methods
    4. SharedBlock::Init() returns bool (true for success) and supports modification methods (Clear, SetColumnNum, etc.)
 * @tc.step:
    1. Create a valid Ashmem instance with name "Test Shared" and size 1024, map it to read-write
    2. Create SharedBlock instance (temp) with readOnly=true
    3. Call temp.Init() and check the return value
    4. Call modification methods (Clear, SetColumnNum, AllocRow, etc.) on temp
    5. Check the return value of each modification method
 * @tc.expect:
    1. SharedBlock::Init() returns true (initialization success)
    2. All modification methods return SHARED_BLOCK_INVALID_OPERATION
 */
HWTEST_F(AbnormalBranchTest, AbnormalBranchTest_mReadOnlyInvalid_Test_001, TestSize.Level0)
{
    LOG_INFO("AbnormalBranchTest_mReadOnlyInvalid_Test_001::Start");
    std::string name = "Test Shared\0";
    bool readOnly = true;
    int32_t size = 1024;
    sptr<Ashmem> ashmem = Ashmem::CreateAshmem(name.c_str(), size);
    ashmem->MapReadAndWriteAshmem();
    AppDataFwk::SharedBlock temp(name, ashmem, size, readOnly);
    int result = temp.Init();
    EXPECT_TRUE(result);
    result = temp.Clear();
    EXPECT_EQ(result, AppDataFwk::SharedBlock::SHARED_BLOCK_INVALID_OPERATION);
    result = temp.SetColumnNum(1);
    EXPECT_EQ(result, AppDataFwk::SharedBlock::SHARED_BLOCK_INVALID_OPERATION);
    result = temp.AllocRow();
    EXPECT_EQ(result, AppDataFwk::SharedBlock::SHARED_BLOCK_INVALID_OPERATION);
    result = temp.FreeLastRow();
    EXPECT_EQ(result, AppDataFwk::SharedBlock::SHARED_BLOCK_INVALID_OPERATION);
    int64_t intValue = 0;
    result = temp.PutLong(1, 1, intValue);
    EXPECT_EQ(result, AppDataFwk::SharedBlock::SHARED_BLOCK_INVALID_OPERATION);
    double doubleValue = 0.0;
    result = temp.PutDouble(1, 1, doubleValue);
    EXPECT_EQ(result, AppDataFwk::SharedBlock::SHARED_BLOCK_INVALID_OPERATION);
    result = temp.PutNull(1, 1);
    EXPECT_EQ(result, AppDataFwk::SharedBlock::SHARED_BLOCK_INVALID_OPERATION);
    result = temp.SetRawData(nullptr, 0);
    EXPECT_EQ(result, AppDataFwk::SharedBlock::SHARED_BLOCK_INVALID_OPERATION);
    LOG_INFO("AbnormalBranchTest_mReadOnlyInvalid_Test_001::End");
}

/**
 * @tc.name: AbnormalBranchTest_CreatorPossibleNull_Test_002
 * @tc.desc: Verify DataShareHelper::Creator with invalid parameters
 * @tc.type: FUNC
 * @tc.precon:
    1. DataShareHelper::Creator is a static method accepting std::string URI, CreateOptions, and std::string bundleName
    2. CreateOptions struct is defined with token_ (pointer) and isProxy_ (bool) members
    3. Creator returns nullptr when parameters are invalid (e.g., null token_)
 * @tc.step:
    1. Initialize an empty std::string strUri (invalid URI)
    2. Create CreateOptions instance: set token_=nullptr, isProxy_=false
    3. Initialize an empty std::string bundleName
    4. Call DataShare::DataShareHelper::Creator(strUri, options, bundleName)
    5. Check the returned std::shared_ptr<DataShareHelper>
 * @tc.expect:
    1. The returned DataShareHelper pointer is nullptr (creation failure)
 */
HWTEST_F(AbnormalBranchTest, AbnormalBranchTest_CreatorPossibleNull_Test_002, TestSize.Level0)
{
    LOG_INFO("AbnormalBranchTest_CreatorPossibleNull_Test_002::Start");
    std::string strUri;
    CreateOptions options;
    options.token_ = nullptr;
    options.isProxy_ = false;
    std::string bundleName;
    std::shared_ptr<DataShareHelper> dataHelper = DataShare::DataShareHelper::Creator(strUri, options, bundleName);
    EXPECT_EQ(dataHelper, nullptr);
    LOG_INFO("AbnormalBranchTest_CreatorPossibleNull_Test_002::End");
}

/**
 * @tc.name: AbnormalBranchTest_AddObserversProxyNull_Test_001
 * @tc.desc: Verify PublishedDataSubscriberManager::AddObservers with null proxy
 * @tc.type: FUNC
 * @tc.precon:
    1. PublishedDataSubscriberManager is a singleton (GetInstance() returns instance)
    2. AddObservers method accepts void* subscriber, std::shared_ptr<DataShareServiceProxy> proxy,
       std::vector<std::string> uris, int64_t subscriberId, and PublishedDataCallback
    3. OperationResult struct is defined, and AddObservers returns std::vector<OperationResult>
 * @tc.step:
    1. Get PublishedDataSubscriberManager instance via GetInstance()
    2. Initialize null void* subscriber and null std::shared_ptr<DataShareServiceProxy> proxy
    3. Create empty std::vector<std::string> uris and set int64_t subscriberId=0
    4. Define an empty PublishedDataCallback (no logic inside)
    5. Call AddObservers with the above parameters, get results vector
    6. Compare results.size() with uris.size()
 * @tc.expect:
    1. The size of returned results vector equals uris.size() (0 in this case)
 */
HWTEST_F(AbnormalBranchTest, AbnormalBranchTest_AddObserversProxyNull_Test_001, TestSize.Level0)
{
    LOG_INFO("AbnormalBranchTest_AddObserversProxyNull_Test_001::Start");
    void *subscriber = nullptr;
    std::shared_ptr<DataShareServiceProxy> proxy = nullptr;
    const std::vector<std::string> uris = {};
    int64_t subscriberId = 0;
    const PublishedDataCallback callback = [](const PublishedDataChangeNode &changeNode){};
    std::vector<OperationResult> results = PublishedDataSubscriberManager::GetInstance().AddObservers(subscriber,
        proxy, uris, subscriberId, callback);
    EXPECT_EQ(results.size(), uris.size());
    LOG_INFO("AbnormalBranchTest_AddObserversProxyNull_Test_001::End");
}

/**
 * @tc.name: AbnormalBranchTest_AddObserversProxyNull_Test_002
 * @tc.desc: Verify RdbSubscriberManager::AddObservers with null proxy
 * @tc.type: FUNC
 * @tc.precon:
    1. RdbSubscriberManager is a singleton (GetInstance() returns instance)
    2. AddObservers method accepts void* subscriber, std::shared_ptr<DataShareServiceProxy> proxy,
       std::vector<std::string> uris, TemplateId templateId, and RdbCallback
    3. OperationResult struct is defined, and AddObservers returns std::vector<OperationResult>
 * @tc.step:
    1. Get RdbSubscriberManager instance via GetInstance()
    2. Initialize null void* subscriber and null std::shared_ptr<DataShareServiceProxy> proxy
    3. Create empty std::vector<std::string> uris and default-initialized TemplateId templateId
    4. Define an empty RdbCallback (no logic inside)
    5. Call AddObservers with the above parameters, get results vector
    6. Compare results.size() with uris.size()
 * @tc.expect:
    1. The size of returned results vector equals uris.size() (0 in this case)
 */
HWTEST_F(AbnormalBranchTest, AbnormalBranchTest_AddObserversProxyNull_Test_002, TestSize.Level0)
{
    LOG_INFO("AbnormalBranchTest_AddObserversProxyNull_Test_002::Start");
    void *subscriber = nullptr;
    std::shared_ptr<DataShareServiceProxy> proxy = nullptr;
    const std::vector<std::string> uris = {};
    TemplateId templateId;
    const RdbCallback callback = [](const RdbChangeNode &changeNode){};
    std::vector<OperationResult> results = RdbSubscriberManager::GetInstance().AddObservers(subscriber, proxy, uris,
        templateId, callback);
    EXPECT_EQ(results.size(), uris.size());
    LOG_INFO("AbnormalBranchTest_AddObserversProxyNull_Test_002::End");
}

/**
 * @tc.name: AbnormalBranchTest_DelObserversProxyNull_Test_001
 * @tc.desc: Verify PublishedDataSubscriberManager::DelObservers with null proxy
 * @tc.type: FUNC
 * @tc.precon:
    1. PublishedDataSubscriberManager is a singleton, supporting two DelObservers overloads
    2. Overload 1: accepts void* subscriber, std::shared_ptr<DataShareServiceProxy> proxy
    3. Overload 2: accepts void* subscriber, proxy, std::vector<std::string> uris, int64_t subscriberId
    4. Both overloads return std::vector<OperationResult>
 * @tc.step:
    1. Get PublishedDataSubscriberManager instance via GetInstance()
    2. Initialize null void* subscriber and null std::shared_ptr<DataShareServiceProxy> proxy
    3. Create empty std::vector<std::string> uris and set int64_t subscriberId=0
    4. Call Overload 1 (subscriber, proxy) and get results1 vector
    5. Call Overload 2 (subscriber, proxy, uris, subscriberId) and get results2 vector
    6. Compare results1.size() and results2.size() with uris.size()
 * @tc.expect:
    1. results1.size() equals uris.size() (0)
    2. results2.size() equals uris.size() (0)
 */
HWTEST_F(AbnormalBranchTest, AbnormalBranchTest_DelObserversProxyNull_Test_001, TestSize.Level0)
{
    LOG_INFO("AbnormalBranchTest_DelObserversProxyNull_Test_001::Start");
    void *subscriber = nullptr;
    std::shared_ptr<DataShareServiceProxy> proxy = nullptr;
    const std::vector<std::string> uris = {};
    int64_t subscriberId = 0;
    std::vector<OperationResult> results = PublishedDataSubscriberManager::GetInstance().DelObservers(subscriber,
        proxy);
    EXPECT_EQ(results.size(), uris.size());
    results = PublishedDataSubscriberManager::GetInstance().DelObservers(subscriber, proxy, uris, subscriberId);
    EXPECT_EQ(results.size(), uris.size());
    LOG_INFO("AbnormalBranchTest_DelObserversProxyNull_Test_001::End");
}

/**
 * @tc.name: AbnormalBranchTest_DelObserversProxyNull_Test_002
 * @tc.desc: Verify RdbSubscriberManager::DelObservers with null proxy
 * @tc.type: FUNC
 * @tc.precon:
    1. RdbSubscriberManager is a singleton, supporting two DelObservers overloads
    2. Overload 1: accepts void* subscriber, std::shared_ptr<DataShareServiceProxy> proxy,
       std::vector<std::string> uris, TemplateId templateId
    3. Overload 2: accepts void* subscriber, std::shared_ptr<DataShareServiceProxy> proxy
    4. Both overloads return std::vector<OperationResult>
 * @tc.step:
    1. Get RdbSubscriberManager instance via GetInstance()
    2. Initialize null void* subscriber and null std::shared_ptr<DataShareServiceProxy> proxy
    3. Create empty std::vector<std::string> uris and default-initialized TemplateId templateId
    4. Call Overload 1 (subscriber, proxy, uris, templateId) and get results1 vector
    5. Call Overload 2 (subscriber, proxy) and get results2 vector
    6. Compare results1.size() and results2.size() with uris.size()
 * @tc.expect:
    1. results1.size() equals uris.size() (0)
    2. results2.size() equals uris.size() (0)
 */
HWTEST_F(AbnormalBranchTest, AbnormalBranchTest_DelObserversProxyNull_Test_002, TestSize.Level0)
{
    LOG_INFO("AbnormalBranchTest_DelObserversProxyNull_Test_002::Start");
    void *subscriber = nullptr;
    std::shared_ptr<DataShareServiceProxy> proxy = nullptr;
    const std::vector<std::string> uris = {};
    TemplateId templateId;
    std::vector<OperationResult> results = RdbSubscriberManager::GetInstance().DelObservers(subscriber, proxy, uris,
        templateId);
    EXPECT_EQ(results.size(), uris.size());
    results = RdbSubscriberManager::GetInstance().DelObservers(subscriber, proxy);
    EXPECT_EQ(results.size(), uris.size());
    LOG_INFO("AbnormalBranchTest_DelObserversProxyNull_Test_002::End");
}

/**
 * @tc.name: AbnormalBranchTest_EnableObserversProxyNull_Test_001
 * @tc.desc: Verify PublishedDataSubscriberManager::EnableObservers with null proxy
 * @tc.type: FUNC
 * @tc.precon:
    1. PublishedDataSubscriberManager is a singleton (GetInstance() returns instance)
    2. EnableObservers method accepts void* subscriber, std::shared_ptr<DataShareServiceProxy> proxy,
       std::vector<std::string> uris, int64_t subscriberId
    3. Method returns std::vector<OperationResult>
 * @tc.step:
    1. Get PublishedDataSubscriberManager instance via GetInstance()
    2. Initialize null void* subscriber and null std::shared_ptr<DataShareServiceProxy> proxy
    3. Create empty std::vector<std::string> uris and set int64_t subscriberId=0
    4. Call EnableObservers with the above parameters, get results vector
    5. Compare results.size() with uris.size()
 * @tc.expect:
    1. The size of returned results vector equals uris.size() (0 in this case)
 */
HWTEST_F(AbnormalBranchTest, AbnormalBranchTest_EnableObserversProxyNull_Test_001, TestSize.Level0)
{
    LOG_INFO("AbnormalBranchTest_EnableObserversProxyNull_Test_001::Start");
    void *subscriber = nullptr;
    std::shared_ptr<DataShareServiceProxy> proxy = nullptr;
    const std::vector<std::string> uris = {};
    int64_t subscriberId = 0;
    std::vector<OperationResult> results = PublishedDataSubscriberManager::GetInstance().EnableObservers(subscriber,
        proxy, uris, subscriberId);
    EXPECT_EQ(results.size(), uris.size());
    LOG_INFO("AbnormalBranchTest_EnableObserversProxyNull_Test_001::End");
}

/**
 * @tc.name: AbnormalBranchTest_EnableObserversProxyNull_Test_002
 * @tc.desc: Verify RdbSubscriberManager::EnableObservers with null proxy
 * @tc.type: FUNC
 * @tc.precon:
    1. RdbSubscriberManager is a singleton (GetInstance() returns instance)
    2. EnableObservers method accepts void* subscriber, std::shared_ptr<DataShareServiceProxy> proxy,
       std::vector<std::string> uris, TemplateId templateId
    3. Method returns std::vector<OperationResult>
 * @tc.step:
    1. Get RdbSubscriberManager instance via GetInstance()
    2. Initialize null void* subscriber and null std::shared_ptr<DataShareServiceProxy> proxy
    3. Create empty std::vector<std::string> uris and default-initialized TemplateId templateId
    4. Call EnableObservers with the above parameters, get results vector
    5. Compare results.size() with uris.size()
 * @tc.expect:
    1. The size of returned results vector equals uris.size() (0 in this case)
 */
HWTEST_F(AbnormalBranchTest, AbnormalBranchTest_EnableObserversProxyNull_Test_002, TestSize.Level0)
{
    LOG_INFO("AbnormalBranchTest_EnableObserversProxyNull_Test_002::Start");
    void *subscriber = nullptr;
    std::shared_ptr<DataShareServiceProxy> proxy = nullptr;
    const std::vector<std::string> uris = {};
    TemplateId templateId;
    std::vector<OperationResult> results = RdbSubscriberManager::GetInstance().EnableObservers(subscriber, proxy,
        uris, templateId);
    EXPECT_EQ(results.size(), uris.size());
    LOG_INFO("AbnormalBranchTest_EnableObserversProxyNull_Test_002::End");
}

/**
 * @tc.name: AbnormalBranchTest_DisableObserversProxyNull_Test_001
 * @tc.desc: Verify PublishedDataSubscriberManager::DisableObservers with null proxy
 * @tc.type: FUNC
 * @tc.precon:
    1. PublishedDataSubscriberManager is a singleton (GetInstance() returns instance)
    2. DisableObservers method accepts void* subscriber, std::shared_ptr<DataShareServiceProxy> proxy,
       std::vector<std::string> uris, int64_t subscriberId
    3. Method returns std::vector<OperationResult>
 * @tc.step:
    1. Get PublishedDataSubscriberManager instance via GetInstance()
    2. Initialize null void* subscriber and null std::shared_ptr<DataShareServiceProxy> proxy
    3. Create empty std::vector<std::string> uris and set int64_t subscriberId=0
    4. Call DisableObservers with the above parameters, get results vector
    5. Compare results.size() with uris.size()
 * @tc.expect:
    1. The size of returned results vector equals uris.size() (0 in this case)
 */
HWTEST_F(AbnormalBranchTest, AbnormalBranchTest_DisableObserversProxyNull_Test_001, TestSize.Level0)
{
    LOG_INFO("AbnormalBranchTest_DisableObserversProxyNull_Test_001::Start");
    void *subscriber = nullptr;
    std::shared_ptr<DataShareServiceProxy> proxy = nullptr;
    const std::vector<std::string> uris = {};
    int64_t subscriberId = 0;
    std::vector<OperationResult> results = PublishedDataSubscriberManager::GetInstance().DisableObservers(subscriber,
        proxy, uris, subscriberId);
    EXPECT_EQ(results.size(), uris.size());
    LOG_INFO("AbnormalBranchTest_DisableObserversProxyNull_Test_001::End");
}

/**
 * @tc.name: AbnormalBranchTest_DisableObserversProxyNull_Test_002
 * @tc.desc: Verify RdbSubscriberManager::DisableObservers with null proxy
 * @tc.type: FUNC
 * @tc.precon:
    1. RdbSubscriberManager is a singleton (GetInstance() returns instance)
    2. DisableObservers method accepts void* subscriber, std::shared_ptr<DataShareServiceProxy> proxy,
       std::vector<std::string> uris, TemplateId templateId
    3. Method returns std::vector<OperationResult>
 * @tc.step:
    1. Get RdbSubscriberManager instance via GetInstance()
    2. Initialize null void* subscriber and null std::shared_ptr<DataShareServiceProxy> proxy
    3. Create empty std::vector<std::string> uris and default-initialized TemplateId templateId
    4. Call DisableObservers with the above parameters, get results vector
    5. Compare results.size() with uris.size()
 * @tc.expect:
    1. The size of returned results vector equals uris.size() (0 in this case)
 */
HWTEST_F(AbnormalBranchTest, AbnormalBranchTest_DisableObserversProxyNull_Test_002, TestSize.Level0)
{
    LOG_INFO("AbnormalBranchTest_DisableObserversProxyNull_Test_002::Start");
    void *subscriber = nullptr;
    std::shared_ptr<DataShareServiceProxy> proxy = nullptr;
    const std::vector<std::string> uris = {};
    TemplateId templateId;
    std::vector<OperationResult> results = RdbSubscriberManager::GetInstance().DisableObservers(subscriber, proxy,
        uris, templateId);
    EXPECT_EQ(results.size(), uris.size());
    LOG_INFO("AbnormalBranchTest_DisableObserversProxyNull_Test_002::End");
}

/**
 * @tc.name: PublishDelObserversTest001
 * @tc.desc: Verify PublishedDataSubscriberManager observer operations with null proxy
 * @tc.type: FUNC
 * @tc.precon:
    1. PublishedDataSubscriberManager is a singleton, accessible via GetInstance()
    2. DataShareManagerImpl::GetServiceProxy() can return a valid DataShareServiceProxy instance
    3. OperationResult struct is defined to store observer operation results
    4. Test URI "datashare:///com.test/db0/tbl0" is predefined and conforms to DataShare URI format
    5. PublishedDataSubscriberManager supports DisableObservers, DelObservers, and RecoverObservers methods
    6. int64_t subscriberId is a valid parameter type for observer methods
 * @tc.step:
    1. Initialize void* subscriber as nullptr, and get a valid proxy via DataShareManagerImpl::GetServiceProxy()
    2. Create an empty std::vector<std::string> uris, and add the predefined test URI to uris
    3. Set int64_t subscriberId to 0
    4. Call PublishedDataSubscriberManager::GetInstance().DisableObservers(subscriber, nullptr, uris, subscriberId),
       and record the returned results
    5. Call the same singleton's DelObservers(subscriber, proxy, uris, subscriberId), and update results
    6. Set proxy to nullptr, then call RecoverObservers(proxy) on the singleton
    7. Check the size of results from steps 4 and 5 respectively
 * @tc.expect:
    1. Results from step 4 (DisableObservers with null proxy) have a size of 0
    2. Results from step 5 (DelObservers with valid proxy) have a size equal to uris.size()
 */
HWTEST_F(AbnormalBranchTest, PublishDelObserversTest001, TestSize.Level0)
{
    LOG_INFO("PublishDelObserversTest001::Start");
    void *subscriber = nullptr;
    auto proxy = DataShareManagerImpl::GetServiceProxy();
    std::vector<std::string> uris = {};
    std::string uri = "datashare:///com.test/db0/tbl0";
    uris.push_back(uri);
    int64_t subscriberId = 0;
    std::vector<OperationResult> results = PublishedDataSubscriberManager::GetInstance().DisableObservers(subscriber,
        nullptr, uris, subscriberId);
    EXPECT_EQ(results.size(), 0);
    results = PublishedDataSubscriberManager::GetInstance().DelObservers(subscriber,
        proxy, uris, subscriberId);
    EXPECT_EQ(results.size(), uris.size());
    proxy = nullptr;
    PublishedDataSubscriberManager::GetInstance().RecoverObservers(proxy);
    LOG_INFO("PublishDelObserversTest001::End");
}

/**
 * @tc.name: PublishedDataSubscriberManagerOperatorTest001
 * @tc.desc: Verify PublishedDataObserver comparison operators
 * @tc.type: FUNC
 * @tc.precon:
    1. PublishedDataObserver class is implemented with overloaded != and == operators
    2. PublishedDataCallback is a predefined function type for observer callbacks
    3. Two PublishedDataObserver instances with identical callbacks are considered distinct instances
    4. The comparison operators of PublishedDataObserver compare instance identity rather than callback content
 * @tc.step:
    1. Define an empty PublishedDataCallback (no business logic inside the lambda)
    2. Create the first PublishedDataObserver instance ob, passing the defined callback
    3. Create the second PublishedDataObserver instance obCompare, passing the same callback
    4. Use the != operator to compare ob and obCompare, and record the result
    5. Use the == operator to compare ob and obCompare, and record the result
 * @tc.expect:
    1. The != operator comparison between ob and obCompare returns true
    2. The == operator comparison between ob and obCompare returns false
 */
HWTEST_F(AbnormalBranchTest, PublishedDataSubscriberManagerOperatorTest001, TestSize.Level0)
{
    LOG_INFO("PublishedDataSubscriberManagerOperatorTest001::Start");
    const PublishedDataCallback callback = [](const PublishedDataChangeNode &changeNode){};
    PublishedDataObserver ob(callback);
    PublishedDataObserver obCompare(callback);
    EXPECT_TRUE(ob != obCompare);
    EXPECT_FALSE(ob == obCompare);
    LOG_INFO("PublishedDataSubscriberManagerOperatorTest001::End");
}

/**
 * @tc.name: RdbDelObserversTest001
 * @tc.desc: Verify RdbSubscriberManager::DelObservers with valid and null proxy
 * @tc.type: FUNC
 * @tc.precon:
    1. RdbSubscriberManager is a singleton, accessible via GetInstance()
    2. DataShareManagerImpl::GetServiceProxy() can return a valid DataShareServiceProxy instance
    3. TemplateId struct has a default constructor (supports no-parameter initialization)
    4. OperationResult struct is defined to store the result of DelObservers
    5. Test URI "datashare:///com.test/db0/tbl0" is predefined for Rdb observer operations
    6. RdbSubscriberManager::DelObservers accepts void* subscriber, proxy, URIs, and TemplateId
 * @tc.step:
    1. Initialize void* subscriber as nullptr, and get a valid proxy via DataShareManagerImpl::GetServiceProxy()
    2. Create an empty std::vector<std::string> uris, and add the predefined test URI to uris
    3. Default-initialize a TemplateId instance templateId
    4. Call RdbSubscriberManager::GetInstance().DelObservers(subscriber, proxy, uris, templateId),
       and record the returned results
    5. Set proxy to nullptr, then call PublishedDataSubscriberManager::GetInstance().RecoverObservers(proxy)
    6. Check the size of results from step 4
 * @tc.expect:
    1. The size of results from DelObservers is equal to uris.size()
 */
HWTEST_F(AbnormalBranchTest, RdbDelObserversTest001, TestSize.Level0)
{
    LOG_INFO("RdbDelObserversTest001::Start");
    void *subscriber = nullptr;
    auto proxy = DataShareManagerImpl::GetServiceProxy();
    std::vector<std::string> uris = {};
    std::string uri = "datashare:///com.test/db0/tbl0";
    uris.push_back(uri);
    TemplateId templateId;
    std::vector<OperationResult> results = RdbSubscriberManager::GetInstance().DelObservers(subscriber,
        proxy, uris, templateId);
    EXPECT_EQ(results.size(), uris.size());
    proxy = nullptr;
    PublishedDataSubscriberManager::GetInstance().RecoverObservers(proxy);
    LOG_INFO("RdbDelObserversTest001::End");
}

/**
 * @tc.name: RdbDisableObserversTest001
 * @tc.desc: Verify RdbSubscriberManager::DisableObservers with valid and null proxy
 * @tc.type: FUNC
 * @tc.precon:
    1. RdbSubscriberManager is a singleton, accessible via GetInstance()
    2. DataShareManagerImpl::GetServiceProxy() can return a valid DataShareServiceProxy instance
    3. TemplateId struct supports default initialization
    4. OperationResult struct is defined to store DisableObservers operation results
    5. Test URI "datashare:///com.test/db0/tbl0" is predefined and valid for Rdb operations
    6. RdbSubscriberManager::DisableObservers handles null proxy gracefully (no crash)
 * @tc.step:
    1. Initialize void* subscriber as nullptr, and get a valid proxy via DataShareManagerImpl::GetServiceProxy()
    2. Create an empty std::vector<std::string> uris, and add the predefined test URI to uris
    3. Default-initialize a TemplateId instance templateId
    4. Call RdbSubscriberManager::GetInstance().DisableObservers(subscriber, nullptr, uris, templateId),
       record results1
    5. Call the same singleton's DisableObservers(subscriber, proxy, uris, templateId), record results2
    6. Set proxy to nullptr, call RdbSubscriberManager::GetInstance().RecoverObservers(proxy)
    7. Check the sizes of results1 and results2 respectively
 * @tc.expect:
    1. results1 (DisableObservers with null proxy) has a size of 0
    2. results2 (DisableObservers with valid proxy) has a size equal to uris.size()
 */
HWTEST_F(AbnormalBranchTest, RdbDisableObserversTest001, TestSize.Level0)
{
    LOG_INFO("RdbDisableObserversTest001::Start");
    void *subscriber = nullptr;
    auto proxy = DataShareManagerImpl::GetServiceProxy();
    std::vector<std::string> uris = {};
    std::string uri = "datashare:///com.test/db0/tbl0";
    uris.push_back(uri);
    TemplateId templateId;
    std::vector<OperationResult> results = RdbSubscriberManager::GetInstance().DisableObservers(subscriber,
        nullptr, uris, templateId);
    EXPECT_EQ(results.size(), 0);
    results = RdbSubscriberManager::GetInstance().DisableObservers(subscriber,
        proxy, uris, templateId);
    EXPECT_EQ(results.size(), uris.size());
    proxy = nullptr;
    RdbSubscriberManager::GetInstance().RecoverObservers(proxy);
    LOG_INFO("RdbDisableObserversTest001::End");
}

/**
* @tc.name: GeneralControllerServiceImplDeleteTest001
* @tc.desc: Fill the branch DataShareManagerImpl::GetServiceProxy() == nullptr
* @tc.type: FUNC
* @tc.precon:
    1. DataShareManagerImplHelper() mocks GetServiceProxy() to return nullptr
    2. GeneralControllerServiceImpl is instantiable with proxyUri
    3. Delete(uri, predicates) returns DATA_SHARE_ERROR when proxy is null
* @tc.require: issueIBX9HL
* @tc.step:
    1. Call DataShareManagerImplHelper() to mock null proxy
    2. Create DataSharePredicates predicates (default)
    3. Define proxyUri: "datashareproxy://com.acts.ohos.data.datasharetest/test"
    4. Create Uri uri(proxyUri)
    5. Create GeneralControllerServiceImpl instance with proxyUri
    6. Call Delete(uri, predicates); get ret
 * @tc.expect:
    1. ret == DATA_SHARE_ERROR (null proxy leads to error)
*/
HWTEST_F(AbnormalBranchTest, RdbSubscriberManagerOperatorTest001, TestSize.Level0)
{
    LOG_INFO("RdbSubscriberManagerOperatorTest001::Start");
    const RdbCallback callback = [](const RdbChangeNode &changeNode){};
    RdbObserver ob(callback);
    RdbObserver obCompare(callback);
    EXPECT_TRUE(ob != obCompare);
    EXPECT_FALSE(ob == obCompare);
    LOG_INFO("RdbSubscriberManagerOperatorTest001::End");
}

/**
 * @tc.name: RegisterClientDeathObserverTest001
 * @tc.desc: Verify DataShareManagerImpl::RegisterClientDeathObserver under various conditions
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. DataShareManagerImpl class can be instantiated via the new operator
    2. DataShareManagerImpl has member variables: dataMgrService_ (pointer to service), bundleName_ (std::string),
       and clientDeathObserverPtr_ (pointer to death observer, initially null)
    3. DataShareManagerImpl::GetDistributedDataManager() can return a valid non-null service instance
    4. RegisterClientDeathObserver() is a member function of DataShareManagerImpl that creates clientDeathObserverPtr_
       only when bundleName_ is non-empty and dataMgrService_ is valid
 * @tc.step:
    1. Create a DataShareManagerImpl instance via new, set dataMgrService_ to nullptr, bundleName_ to empty string,
       and clientDeathObserverPtr_ to nullptr
    2. Call RegisterClientDeathObserver() on the instance
    3. Set dataMgrService_ to the result of GetDistributedDataManager() (valid service), then call
       RegisterClientDeathObserver() again
    4. Set bundleName_ to "com.testbundlename" (valid non-empty name), then call RegisterClientDeathObserver() again
    5. Check the state of clientDeathObserverPtr_ after each call
 * @tc.expect:
    1. After step 2 and step 3 (with empty bundle name), clientDeathObserverPtr_ remains null
    2. After step 4 (with valid bundle name), clientDeathObserverPtr_ is non-null
 */
HWTEST_F(AbnormalBranchTest, RegisterClientDeathObserverTest001, TestSize.Level0)
{
    LOG_INFO("RegisterClientDeathObserverTest001::Start");
    auto datashareManager = new DataShareManagerImpl();
    datashareManager->dataMgrService_ = nullptr;
    datashareManager->bundleName_ = "";
    datashareManager->clientDeathObserverPtr_ = nullptr;
    datashareManager->RegisterClientDeathObserver();
    datashareManager->dataMgrService_ = datashareManager->GetDistributedDataManager();
    EXPECT_NE(datashareManager->dataMgrService_, nullptr);
    datashareManager->bundleName_ = "com.testbundlename";
    datashareManager->RegisterClientDeathObserver();
    EXPECT_NE(datashareManager->clientDeathObserverPtr_, nullptr);
    datashareManager->RegisterClientDeathObserver();
    LOG_INFO("RegisterClientDeathObserverTest001::End");
}

/**
* @tc.name: RegisterClientDeathObserverTest002
* @tc.desc: Check the main process of RegisterClientDeathObserver
* @tc.type: FUNC
* @tc.precon:
    1. DataShareManagerImpl::GetInstance() returns a singleton instance
    2. DataShareManagerImpl::SetBundleName() accepts a non-empty std::string and sets bundleName_
    3. DataShareManagerImpl::GetDataShareServiceProxy() initializes dataShareService_ and dataMgrService_
    4. RegisterClientDeathObserver() is called internally during service proxy initialization
    5. clientDeathObserverPtr_ is non-null when bundleName_ is valid and service proxies are initialized
* @tc.require: issueIBX9HL
* @tc.step:
    1. Get DataShareManagerImpl singleton instance via GetInstance()
    2. Call SetBundleName("com.testbundlename") to set a valid bundle name
    3. Call GetDataShareServiceProxy() to initialize service proxies
    4. Check the states of dataMgrService_, bundleName_, and clientDeathObserverPtr_
* @tc.expect:
    1. dataMgrService_ is not nullptr
    2. bundleName_ is not an empty string
    3. clientDeathObserverPtr_ is not nullptr
*/
HWTEST_F(AbnormalBranchTest, RegisterClientDeathObserverTest002, TestSize.Level0)
{
    LOG_INFO("RegisterClientDeathObserverTest002::Start");
    auto datashareManager = DataShareManagerImpl::GetInstance();
    datashareManager->SetBundleName("com.testbundlename");
    datashareManager->GetDataShareServiceProxy();
    EXPECT_NE(datashareManager->dataMgrService_, nullptr);
    EXPECT_NE(datashareManager->bundleName_, "");
    EXPECT_NE(datashareManager->clientDeathObserverPtr_, nullptr);
    LOG_INFO("RegisterClientDeathObserverTest002::End");
}

/**
* @tc.name: OnRemoteDiedTest001
* @tc.desc: Check the main process of OnRemoteDied and the reset process ResetServiceHandle
* @tc.type: FUNC
* @tc.precon:
    1. DataShareManagerImpl::GetInstance() returns a singleton instance
    2. SetBundleName() sets a valid non-empty bundle name
    3. GetDataShareServiceProxy() initializes dataMgrService_ and dataShareService_ to non-null
    4. OnRemoteDied() calls ResetServiceHandle(), which sets dataMgrService_ and dataShareService_ to null
    5. clientDeathObserverPtr_ is non-null after service proxy initialization
* @tc.require: issueIBX9HL
* @tc.step:
    1. Get DataShareManagerImpl singleton via GetInstance()
    2. Call SetBundleName("com.testbundlename") to set a valid bundle name
    3. Call GetDataShareServiceProxy() to initialize service proxies
    4. Verify initial states of dataMgrService_, bundleName_, clientDeathObserverPtr_
    5. Call OnRemoteDied() to trigger service handle reset
    6. Check the states of dataMgrService_ and dataShareService_ after reset
* @tc.expect:
    1. Before OnRemoteDied(): dataMgrService_ != nullptr, bundleName_ != "", clientDeathObserverPtr_ != nullptr
    2. After OnRemoteDied(): dataMgrService_ == nullptr, dataShareService_ == nullptr
*/
HWTEST_F(AbnormalBranchTest, OnRemoteDiedTest001, TestSize.Level0)
{
    LOG_INFO("OnRemoteDiedTest001::Start");
    auto datashareManager = DataShareManagerImpl::GetInstance();
    datashareManager->SetBundleName("com.testbundlename");
    datashareManager->GetDataShareServiceProxy();
    EXPECT_NE(datashareManager->dataMgrService_, nullptr);
    EXPECT_NE(datashareManager->bundleName_, "");
    EXPECT_NE(datashareManager->clientDeathObserverPtr_, nullptr);

    datashareManager->OnRemoteDied();
    EXPECT_EQ(datashareManager->dataMgrService_, nullptr);
    EXPECT_EQ(datashareManager->dataShareService_, nullptr);
    LOG_INFO("OnRemoteDiedTest001::End");
}

/**
* @tc.name: SetRegisterCallbackTest001
* @tc.desc: Check the main process of SetRegisterCallback
* @tc.type: FUNC
* @tc.precon:
    1. DataShareManagerImpl::GetInstance() returns a singleton instance
    2. DataShareManagerImpl has a member variable observers_ (Vector type) to store callbacks
    3. SetRegisterCallback() accepts a void* pointer and a std::function<void()> callback,
       and adds the callback to observers_
    4. observers_.Size() returns the number of stored callbacks (initial size is non-negative)
    5. Empty std::function<void()> is a valid callback parameter
* @tc.require: issueIBX9HL
* @tc.step:
    1. Get DataShareManagerImpl singleton via GetInstance()
    2. Record the initial size of observers_ via observers_.Size()
    3. Verify the singleton instance is not nullptr
    4. Define an empty std::function<void()> callback (no logic in lambda)
    5. Call SetRegisterCallback(nullptr, callback) to add the callback
    6. Check the new size of observers_
* @tc.expect:
    1. The new size of observers_ is equal to the initial size + 1
*/
HWTEST_F(AbnormalBranchTest, SetRegisterCallbackTest001, TestSize.Level0)
{
    LOG_INFO("SetRegisterCallbackTest001::Start");
    auto datashareManager = DataShareManagerImpl::GetInstance();
    size_t obsSize = datashareManager->observers_.Size();
    ASSERT_NE(datashareManager, nullptr);
    std::function<void()> callback = [](){};
    datashareManager->SetRegisterCallback(nullptr, callback);
    // observers_ size increased by 1
    EXPECT_EQ(datashareManager->observers_.Size(), obsSize + 1);
    LOG_INFO("SetRegisterCallbackTest001::End");
}

/**
 * @tc.name: AmsMgrProxyOnProxyDiedTest001
 * @tc.desc: Verify AmsMgrProxy behavior when proxy dies
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. AmsMgrProxy class can be instantiated using the new operator and destroyed with delete
    2. AmsMgrProxy has member variables: sa_ (service pointer), proxy_ (proxy pointer), and deathRecipient_ (death
       recipient pointer)
    3. ConnectSA() is a member function of AmsMgrProxy that initializes sa_, proxy_, and deathRecipient_ to non-null
       values upon successful connection
    4. OnProxyDied() is a member function of AmsMgrProxy that sets sa_, proxy_, and deathRecipient_ to nullptr
       to clean up resources
    5. AmsMgrProxy can handle null values for sa_ and proxy_ without crashing when OnProxyDied() is called
 * @tc.step:
    1. Create an AmsMgrProxy instance using new, set its sa_ and proxy_ to nullptr
    2. Call OnProxyDied() on the instance, then delete the instance to verify null pointer handling
    3. Create a new AmsMgrProxy instance using new, set its sa_ and proxy_ to nullptr
    4. Call ConnectSA() on the new instance to initialize resources
    5. Verify that sa_, proxy_, and deathRecipient_ are non-null after initialization
    6. Call OnProxyDied() on the instance to trigger resource cleanup
    7. Delete the instance to complete the test
 * @tc.expect:
    1. Step 2: No crash occurs when OnProxyDied() is called with null sa_ and proxy_
    2. Step 5: sa_, proxy_, and deathRecipient_ are all non-null after ConnectSA()
    3. Step 6: OnProxyDied() successfully cleans up resources (no crash during cleanup)
 */
HWTEST_F(AbnormalBranchTest, AmsMgrProxyOnProxyDiedTest001, TestSize.Level0)
{
    LOG_INFO("AmsMgrProxyOnProxyDiedTest001::Start");
    AmsMgrProxy* proxy = new AmsMgrProxy();
    proxy->sa_ = nullptr;
    proxy->proxy_ = nullptr;
    proxy->OnProxyDied();
    delete proxy;
    proxy = new AmsMgrProxy();
    proxy->sa_ = nullptr;
    proxy->proxy_ = nullptr;
    proxy->ConnectSA();
    EXPECT_NE(proxy->sa_, nullptr);
    EXPECT_NE(proxy->proxy_, nullptr);
    EXPECT_NE(proxy->deathRecipient_, nullptr);
    proxy->OnProxyDied();
    delete proxy;
    LOG_INFO("AmsMgrProxyOnProxyDiedTest001::End");
}

/**
* @tc.name: AmsMgrProxyOnProxyDiedTest002
* @tc.desc: Test sa_ with nullptr and destructor of AmsMgrProxy
* @tc.type: FUNC
* @tc.precon:
    1. AmsMgrProxy::GetInstance() returns a singleton instance of AmsMgrProxy
    2. OnProxyDied() sets sa_ to null even if it was initially null
    3. ConnectSA() can re-initialize sa_, proxy_, and deathRecipient_ to non-null after OnProxyDied()
    4. AmsMgrProxy's destructor handles non-null member variables gracefully
* @tc.require: issueIBX9HL
* @tc.step:
    1. Get AmsMgrProxy singleton instance via GetInstance()
    2. Call OnProxyDied() to set sa_ to null
    3. Call ConnectSA() to re-establish SA connection
    4. Check the states of sa_, proxy_, and deathRecipient_ after ConnectSA()
* @tc.expect:
    1. After ConnectSA(): sa_ != nullptr, proxy_ != nullptr, deathRecipient_ != nullptr
*/
HWTEST_F(AbnormalBranchTest, AmsMgrProxyOnProxyDiedTest002, TestSize.Level0)
{
    LOG_INFO("AmsMgrProxyOnProxyDiedTest002::Start");
    AmsMgrProxy* proxy = AmsMgrProxy::GetInstance();
    proxy->OnProxyDied();
    proxy->ConnectSA();
    EXPECT_NE(proxy->sa_, nullptr);
    EXPECT_NE(proxy->proxy_, nullptr);
    EXPECT_NE(proxy->deathRecipient_, nullptr);
    LOG_INFO("AmsMgrProxyOnProxyDiedTest002::End");
}

/**
 * @tc.name: DataShareServiceProxySubscribeRdbDataTest001
 * @tc.desc: Verify AmsMgrProxy's behavior when proxy dies (creation, initialization, cleanup)
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. AmsMgrProxy class supports instantiation via new and destruction via delete
    2. AmsMgrProxy has member variables: sa_ (service pointer), proxy_, deathRecipient_ (death recipient pointer)
    3. AmsMgrProxy::ConnectSA() initializes sa_, proxy_, and deathRecipient_ to non-null on success
    4. AmsMgrProxy::OnProxyDied() sets sa_, proxy_, and deathRecipient_ to nullptr for resource cleanup
    5. AmsMgrProxy handles null values for sa_ and proxy_ gracefully (no crash when calling OnProxyDied())
 * @tc.step:
    1. Create an AmsMgrProxy instance via new, set its sa_ and proxy_ to nullptr
    2. Call OnProxyDied() on the instance, then delete the instance to test null handling
    3. Create a new AmsMgrProxy instance via new, set its sa_ and proxy_ to nullptr
    4. Call ConnectSA() on the new instance to initialize resources
    5. Verify sa_, proxy_, and deathRecipient_ are non-null after initialization
    6. Call OnProxyDied() on the instance to trigger cleanup
    7. Delete the instance to complete the test
 * @tc.expect:
    1. Step 2: No crash occurs when OnProxyDied() is called with null sa_ and proxy_
    2. Step 5: sa_, proxy_, and deathRecipient_ are all non-null (initialization success)
    3. Step 6: No crash occurs during resource cleanup (OnProxyDied() works)
 */
HWTEST_F(AbnormalBranchTest, DataShareServiceProxySubscribeRdbDataTest001, TestSize.Level0)
{
    LOG_INFO("DataShareServiceProxySubscribeRdbDataTest001::Start");
    AmsMgrProxy* proxy = new AmsMgrProxy();
    proxy->sa_ = nullptr;
    proxy->proxy_ = nullptr;
    proxy->OnProxyDied();
    delete proxy;
    proxy = new AmsMgrProxy();
    proxy->sa_ = nullptr;
    proxy->proxy_ = nullptr;
    proxy->ConnectSA();
    EXPECT_NE(proxy->sa_, nullptr);
    EXPECT_NE(proxy->proxy_, nullptr);
    EXPECT_NE(proxy->deathRecipient_, nullptr);
    proxy->OnProxyDied();
    delete proxy;
    LOG_INFO("DataShareServiceProxySubscribeRdbDataTest001::End");
}

/**
 * @tc.name: SubscribeRdbDataTest001
 * @tc.desc: Verify SubscribeRdbData with empty URIs and null observer
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. DataShareManagerImpl::GetServiceProxy() returns a valid DataShareServiceProxy instance
    2. DataShareServiceProxy::SubscribeRdbData() accepts parameters: std::vector<std::string> uris,
       TemplateId templateId, sptr<IDataProxyRdbObserver> observer, and returns std::vector<OperationResult>
    3. TemplateId supports default initialization (no-parameter constructor)
    4. sptr<IDataProxyRdbObserver>(nullptr) is a valid null observer parameter
    5. Empty std::vector<std::string> is a supported input for uris (no crash)
 * @tc.step:
    1. Initialize empty std::vector<std::string> uris (no elements)
    2. Default-initialize TemplateId templateId
    3. Create null observer: sptr<IDataProxyRdbObserver>(nullptr)
    4. Get valid DataShareServiceProxy via DataShareManagerImpl::GetServiceProxy()
    5. Call proxy->SubscribeRdbData(uris, templateId, observer) to get resultset
    6. Check the size of the returned resultset
 * @tc.expect:
    1. DataShareManagerImpl::GetServiceProxy() returns a non-null proxy
    2. The size of the returned resultset is 0 (matches empty uris)
    3. No crash occurs during the SubscribeRdbData() call (valid parameter handling)
 */
HWTEST_F(AbnormalBranchTest, SubscribeRdbDataTest001, TestSize.Level0)
{
    LOG_INFO("EnableSubscribePublishedDataTest001::Start");
    std::vector<std::string> uris = {};
    TemplateId templateId;
    sptr<IDataProxyRdbObserver> observer = OHOS::sptr<IDataProxyRdbObserver>(nullptr);
    auto proxy = DataShareManagerImpl::GetServiceProxy();
    auto resultset = proxy->SubscribeRdbData(uris, templateId, observer);
    EXPECT_EQ(resultset.size(), 0);
    LOG_INFO("EnableSubscribePublishedDataTest001::End");
}

/**
 * @tc.name: SubscribePublishedDataTest001
 * @tc.desc: Verify SubscribePublishedData with empty URIs and null observer
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. DataShareManagerImpl::GetServiceProxy() returns a valid DataShareServiceProxy instance
    2. DataShareServiceProxy::SubscribePublishedData() accepts parameters: std::vector<std::string> uris,
       int64_t subscriberId, sptr<IDataProxyPublishedDataObserver> observer, and returns std::vector<OperationResult>
    3. sptr<IDataProxyPublishedDataObserver>(nullptr) is a valid null observer parameter
    4. Empty std::vector<std::string> is a supported input for uris
    5. int64_t subscriberId = 1 is a valid parameter value
 * @tc.step:
    1. Initialize empty std::vector<std::string> uris (no elements)
    2. Set int64_t subscriberId = 1
    3. Create null observer: sptr<IDataProxyPublishedDataObserver>(nullptr)
    4. Get valid DataShareServiceProxy via DataShareManagerImpl::GetServiceProxy()
    5. Call proxy->SubscribePublishedData(uris, subscriberId, observer) to get resultset
    6. Check the size of the returned resultset
 * @tc.expect:
    1. DataShareManagerImpl::GetServiceProxy() returns a non-null proxy
    2. The size of the returned resultset is 0 (matches empty uris)
    3. No crash occurs during the SubscribePublishedData() call
 */
HWTEST_F(AbnormalBranchTest, SubscribePublishedDataTest001, TestSize.Level0)
{
    LOG_INFO("SubscribePublishedDataTest001::Start");
    std::vector<std::string> uris = {};
    int64_t subscriberId = 1;
    sptr<IDataProxyPublishedDataObserver> observer = OHOS::sptr<IDataProxyPublishedDataObserver>(nullptr);
    auto proxy = DataShareManagerImpl::GetServiceProxy();
    auto resultset = proxy->SubscribePublishedData(uris, subscriberId, observer);
    EXPECT_EQ(resultset.size(), 0);
    LOG_INFO("SubscribePublishedDataTest001::End");
}

/**
 * @tc.name: GeneralControllerServiceImplInsertTest001
 * @tc.desc: Fill the branch DataShareManagerImpl::GetServiceProxy() == nullptr (Insert method)
 * @tc.type: FUNC
 * @tc.require: issueIBX9HL
 * @tc.precon:
    1. DataShareManagerImplHelper() is a mock function that sets GetServiceProxy() to return nullptr
    2. GeneralControllerServiceImpl can be instantiated via std::make_shared, requiring a proxyUri parameter
    3. GeneralControllerServiceImpl::Insert() accepts Uri and DataShareValuesBucket, returns int
    4. DataShareValuesBucket supports default initialization
    5. Uri can be initialized with a valid proxyUri (e.g., "datashareproxy://com.acts.ohos.data.datasharetest/test")
    6. DATA_SHARE_ERROR is a predefined integer error code
 * @tc.step:
    1. Call DataShareManagerImplHelper() to mock GetServiceProxy() returning nullptr
    2. Default-initialize DataShare::DataShareValuesBucket valuesBucket
    3. Define proxyUri: "datashareproxy://com.acts.ohos.data.datasharetest/test"
    4. Create Uri instance using proxyUri
    5. Create GeneralControllerServiceImpl shared pointer via std::make_shared(proxyUri)
    6. Call Insert(uri, valuesBucket) and record the return value ret
    7. Check if ret equals DATA_SHARE_ERROR
 * @tc.expect:
    1. GeneralControllerServiceImpl instance is non-null (creation success)
    2. The return value ret of Insert() is DATA_SHARE_ERROR (null proxy branch triggered)
 */
HWTEST_F(AbnormalBranchTest, GeneralControllerServiceImplInsertTest001, TestSize.Level1)
{
    LOG_INFO("GeneralControllerServiceImplInsertTest001::Start");
    DataShareManagerImplHelper();
    DataShare::DataShareValuesBucket valuesBucket;
    std::string proxyUri = "datashareproxy://com.acts.ohos.data.datasharetest/test";
    Uri uri(proxyUri);
    auto generalCtl = std::make_shared<GeneralControllerServiceImpl>(proxyUri);
    int ret = generalCtl->Insert(uri, valuesBucket);
    EXPECT_EQ(ret, DATA_SHARE_ERROR);
    LOG_INFO("GeneralControllerServiceImplInsertTest001::End");
}

/**
 * @tc.name: GeneralControllerServiceImplUpdateTest001
 * @tc.desc: Fill the branch DataShareManagerImpl::GetServiceProxy() == nullptr (Update method)
 * @tc.type: FUNC
 * @tc.require: issueIBX9HL
 * @tc.precon:
    1. DataShareManagerImplHelper() mocks DataShareManagerImpl::GetServiceProxy() to return nullptr
    2. GeneralControllerServiceImpl can be instantiated via std::make_shared with a proxyUri
    3. GeneralControllerServiceImpl::Update() accepts Uri, DataSharePredicates, DataShareValuesBucket, returns int
    4. DataSharePredicates and DataShareValuesBucket support default initialization
    5. Valid proxyUri is predefined, and Uri can be initialized with it
    6. DATA_SHARE_ERROR is the predefined error code for null proxy
 * @tc.step:
    1. Call DataShareManagerImplHelper() to mock null proxy
    2. Default-initialize DataShare::DataSharePredicates predicates and DataShareValuesBucket valuesBucket
    3. Define proxyUri: "datashareproxy://com.acts.ohos.data.datasharetest/test"
    4. Create Uri instance with proxyUri
    5. Create GeneralControllerServiceImpl shared pointer via std::make_shared(proxyUri)
    6. Call Update(uri, predicates, valuesBucket) and get return value ret
    7. Check if ret == DATA_SHARE_ERROR
 * @tc.expect:
    1. GeneralControllerServiceImpl instance is non-null
    2. Update() returns DATA_SHARE_ERROR (null proxy branch entered)
 */
HWTEST_F(AbnormalBranchTest, GeneralControllerServiceImplUpdateTest001, TestSize.Level1)
{
    LOG_INFO("GeneralControllerServiceImplUpdateTest001::Start");
    DataShareManagerImplHelper();
    DataShare::DataSharePredicates predicates;
    DataShare::DataShareValuesBucket valuesBucket;
    std::string proxyUri = "datashareproxy://com.acts.ohos.data.datasharetest/test";
    Uri uri(proxyUri);
    auto generalCtl = std::make_shared<GeneralControllerServiceImpl>(proxyUri);
    int ret = generalCtl->Update(uri, predicates, valuesBucket);
    EXPECT_EQ(ret, DATA_SHARE_ERROR);
    LOG_INFO("GeneralControllerServiceImplUpdateTest001::End");
}

/**
 * @tc.name: GeneralControllerServiceImplDeleteTest001
 * @tc.desc: Fill the branch DataShareManagerImpl::GetServiceProxy() == nullptr (Delete method)
 * @tc.type: FUNC
 * @tc.require: issueIBX9HL
 * @tc.precon:
    1. DataShareManagerImplHelper() mocks DataShareManagerImpl::GetServiceProxy() to return nullptr
    2. GeneralControllerServiceImpl can be instantiated via std::make_shared with a proxyUri
    3. GeneralControllerServiceImpl::Delete() accepts Uri and DataSharePredicates, returns int
    4. DataSharePredicates supports default initialization
    5. Valid proxyUri is predefined, and Uri can be initialized with it
    6. DATA_SHARE_ERROR is the predefined error code for null proxy
 * @tc.step:
    1. Call DataShareManagerImplHelper() to mock null proxy
    2. Default-initialize DataShare::DataSharePredicates predicates
    3. Define proxyUri: "datashareproxy://com.acts.ohos.data.datasharetest/test"
    4. Create Uri instance with proxyUri
    5. Create GeneralControllerServiceImpl shared pointer via std::make_shared(proxyUri)
    6. Call Delete(uri, predicates) and get return value ret
    7. Check if ret == DATA_SHARE_ERROR
 * @tc.expect:
    1. GeneralControllerServiceImpl instance is non-null
    2. Delete() returns DATA_SHARE_ERROR (null proxy branch entered)
 */
HWTEST_F(AbnormalBranchTest, GeneralControllerServiceImplDeleteTest001, TestSize.Level1)
{
    LOG_INFO("GeneralControllerServiceImplDeleteTest001::Start");
    DataShareManagerImplHelper();
    DataShare::DataSharePredicates predicates;
    std::string proxyUri = "datashareproxy://com.acts.ohos.data.datasharetest/test";
    Uri uri(proxyUri);
    auto generalCtl = std::make_shared<GeneralControllerServiceImpl>(proxyUri);
    int ret = generalCtl->Delete(uri, predicates);
    EXPECT_EQ(ret, DATA_SHARE_ERROR);
    LOG_INFO("GeneralControllerServiceImplDeleteTest001::End");
}

/**
 * @tc.name: GeneralControllerServiceImplInsertExTest001
 * @tc.desc: Fill the branch DataShareManagerImpl::GetServiceProxy() == nullptr (InsertEx method)
 * @tc.type: FUNC
 * @tc.require: issueIBX9HL
 * @tc.precon:
    1. DataShareManagerImplHelper() mocks DataShareManagerImpl::GetServiceProxy() to return nullptr
    2. GeneralControllerServiceImpl can be instantiated via std::make_shared with a proxyUri
    3. GeneralControllerServiceImpl::InsertEx() returns std::pair<int, int64_t> (DATA_SHARE_ERROR and 0 on null proxy)
    4. DataShareValuesBucket supports default initialization, Uri supports proxyUri initialization
    5. DATA_SHARE_ERROR is predefined, and the expected pair (DATA_SHARE_ERROR, 0) is valid
 * @tc.step:
    1. Call DataShareManagerImplHelper() to mock null proxy
    2. Default-initialize DataShare::DataShareValuesBucket valuesBucket
    3. Define proxyUri and create Uri instance with it
    4. Create GeneralControllerServiceImpl shared pointer via std::make_shared(proxyUri)
    5. Call InsertEx(uri, valuesBucket) and get return pair ret
    6. Check if ret equals std::make_pair(DATA_SHARE_ERROR, 0)
 * @tc.expect:
    1. GeneralControllerServiceImpl instance is non-null
    2. InsertEx() returns std::pair(DATA_SHARE_ERROR, 0) (null proxy branch triggered)
 */
HWTEST_F(AbnormalBranchTest, GeneralControllerServiceImplInsertExTest001, TestSize.Level1)
{
    LOG_INFO("GeneralControllerServiceImplInsertExTest001::Start");
    DataShareManagerImplHelper();
    DataShare::DataShareValuesBucket valuesBucket;
    std::string proxyUri = "datashareproxy://com.acts.ohos.data.datasharetest/test";
    Uri uri(proxyUri);
    auto generalCtl = std::make_shared<GeneralControllerServiceImpl>(proxyUri);
    auto ret = generalCtl->InsertEx(uri, valuesBucket);
    EXPECT_EQ(ret, std::make_pair(DATA_SHARE_ERROR, 0));
    LOG_INFO("GeneralControllerServiceImplInsertExTest001::End");
}

/**
 * @tc.name: GeneralControllerServiceImplUpdateExTest001
 * @tc.desc: Fill the branch DataShareManagerImpl::GetServiceProxy() == nullptr (UpdateEx method)
 * @tc.type: FUNC
 * @tc.require: issueIBX9HL
 * @tc.precon:
    1. DataShareManagerImplHelper() mocks DataShareManagerImpl::GetServiceProxy() to return nullptr
    2. GeneralControllerServiceImpl can be instantiated via std::make_shared with a proxyUri
    3. GeneralControllerServiceImpl::UpdateEx() returns std::pair<int, int64_t>
    4. DataSharePredicates and DataShareValuesBucket support default initialization
    5. Uri supports proxyUri initialization, DATA_SHARE_ERROR is predefined
 * @tc.step:
    1. Call DataShareManagerImplHelper() to mock null proxy
    2. Default-initialize predicates and valuesBucket
    3. Define proxyUri and create Uri instance
    4. Create GeneralControllerServiceImpl shared pointer
    5. Call UpdateEx(uri, predicates, valuesBucket) and get ret
    6. Check if ret == std::make_pair(DATA_SHARE_ERROR, 0)
 * @tc.expect:
    1. GeneralControllerServiceImpl instance is non-null
    2. UpdateEx() returns std::pair(DATA_SHARE_ERROR, 0)
 */
HWTEST_F(AbnormalBranchTest, GeneralControllerServiceImplUpdateExTest001, TestSize.Level1)
{
    LOG_INFO("GeneralControllerServiceImplUpdateExTest001::Start");
    DataShareManagerImplHelper();
    DataShare::DataSharePredicates predicates;
    DataShare::DataShareValuesBucket valuesBucket;
    std::string proxyUri = "datashareproxy://com.acts.ohos.data.datasharetest/test";
    Uri uri(proxyUri);
    auto generalCtl = std::make_shared<GeneralControllerServiceImpl>(proxyUri);
    auto ret = generalCtl->UpdateEx(uri, predicates, valuesBucket);
    EXPECT_EQ(ret, std::make_pair(DATA_SHARE_ERROR, 0));
    LOG_INFO("GeneralControllerServiceImplUpdateExTest001::End");
}

/**
 * @tc.name: GeneralControllerServiceImplDeleteExTest001
 * @tc.desc: Fill the branch DataShareManagerImpl::GetServiceProxy() == nullptr (DeleteEx method)
 * @tc.type: FUNC
 * @tc.require: issueIBX9HL
 * @tc.precon:
    1. DataShareManagerImplHelper() mocks DataShareManagerImpl::GetServiceProxy() to return nullptr
    2. GeneralControllerServiceImpl can be instantiated via std::make_shared with a proxyUri
    3. GeneralControllerServiceImpl::DeleteEx() returns std::pair<int, int64_t>
    4. DataSharePredicates supports default initialization, Uri supports proxyUri initialization
    5. DATA_SHARE_ERROR is predefined, expected pair is (DATA_SHARE_ERROR, 0)
 * @tc.step:
    1. Call DataShareManagerImplHelper() to mock null proxy
    2. Default-initialize DataShare::DataSharePredicates predicates
    3. Define proxyUri and create Uri instance
    4. Create GeneralControllerServiceImpl shared pointer
    5. Call DeleteEx(uri, predicates) and get ret
    6. Check if ret == std::make_pair(DATA_SHARE_ERROR, 0)
 * @tc.expect:
    1. GeneralControllerServiceImpl instance is non-null
    2. DeleteEx() returns std::pair(DATA_SHARE_ERROR, 0)
 */
HWTEST_F(AbnormalBranchTest, GeneralControllerServiceImplDeleteExTest001, TestSize.Level1)
{
    LOG_INFO("GeneralControllerServiceImplDeleteExTest001::Start");
    DataShareManagerImplHelper();
    DataShare::DataSharePredicates predicates;
    std::string proxyUri = "datashareproxy://com.acts.ohos.data.datasharetest/test";
    Uri uri(proxyUri);
    auto generalCtl = std::make_shared<GeneralControllerServiceImpl>(proxyUri);
    auto ret = generalCtl->DeleteEx(uri, predicates);
    EXPECT_EQ(ret, std::make_pair(DATA_SHARE_ERROR, 0));
    LOG_INFO("GeneralControllerServiceImplDeleteExTest001::End");
}

/**
 * @tc.name: GeneralControllerServiceImplQueryTest001
 * @tc.desc: Fill the branch DataShareManagerImpl::GetServiceProxy() == nullptr (Query method)
 * @tc.type: FUNC
 * @tc.require: issueIBX9HL
 * @tc.precon:
    1. DataShareManagerImplHelper() mocks DataShareManagerImpl::GetServiceProxy() to return nullptr
    2. GeneralControllerServiceImpl can be instantiated via std::make_shared with a proxyUri
    3. GeneralControllerServiceImpl::Query() returns a pointer type (nullptr on null proxy)
    4. DataSharePredicates, std::vector<string> columns, DatashareBusinessError, DataShareOption support initialization
    5. Uri supports proxyUri initialization
 * @tc.step:
    1. Call DataShareManagerImplHelper() to mock null proxy
    2. Default-initialize predicates, columns, error, DataShareOption option
    3. Define proxyUri and create Uri instance
    4. Create GeneralControllerServiceImpl shared pointer
    5. Call Query(uri, predicates, columns, error, option) and get ret
    6. Check if ret == nullptr
 * @tc.expect:
    1. GeneralControllerServiceImpl instance is non-null
    2. Query() returns nullptr (null proxy branch entered)
 */
HWTEST_F(AbnormalBranchTest, GeneralControllerServiceImplQueryTest001, TestSize.Level1)
{
    LOG_INFO("GeneralControllerServiceImplQueryTest001::Start");
    DataShareManagerImplHelper();
    DataShare::DataSharePredicates predicates;
    std::vector<string> columns;
    DatashareBusinessError error;
    std::string proxyUri = "datashareproxy://com.acts.ohos.data.datasharetest/test";
    Uri uri(proxyUri);
    auto generalCtl = std::make_shared<GeneralControllerServiceImpl>(proxyUri);
    DataShareOption option;
    auto ret = generalCtl->Query(uri, predicates, columns, error, option);
    EXPECT_EQ(ret, nullptr);
    LOG_INFO("GeneralControllerServiceImplQueryTest001::End");
}

/**
 * @tc.name: GeneralControllerServiceImplNotifyChangeTest001
 * @tc.desc: Fill the branch DataShareManagerImpl::GetServiceProxy() == nullptr (NotifyChange method)
 * @tc.type: FUNC
 * @tc.require: issueIBX9HL
 * @tc.precon:
    1. DataShareManagerImplHelper() mocks DataShareManagerImpl::GetServiceProxy() to return nullptr
    2. GeneralControllerServiceImpl can be instantiated via std::make_shared with a proxyUri
    3. GeneralControllerServiceImpl::NotifyChange() accepts Uri and has no return value
    4. Uri supports proxyUri initialization
    5. NotifyChange() handles null proxy gracefully (no crash)
 * @tc.step:
    1. Call DataShareManagerImplHelper() to mock null proxy
    2. Define proxyUri and create Uri instance
    3. Create GeneralControllerServiceImpl shared pointer via std::make_shared(proxyUri)
    4. Verify the instance is non-null
    5. Call NotifyChange(uri)
 * @tc.expect:
    1. GeneralControllerServiceImpl instance is non-null (creation success)
    2. No crash occurs when calling NotifyChange() (null proxy handled)
    3. The null proxy branch in NotifyChange() is entered
 */
HWTEST_F(AbnormalBranchTest, GeneralControllerServiceImplNotifyChangeTest001, TestSize.Level1)
{
    LOG_INFO("GeneralControllerServiceImplNotifyChangeTest001::Start");
    DataShareManagerImplHelper();
    std::string proxyUri = "datashareproxy://com.acts.ohos.data.datasharetest/test";
    Uri uri(proxyUri);
    auto generalCtl = std::make_shared<GeneralControllerServiceImpl>(proxyUri);
    ASSERT_NE(generalCtl, nullptr);
    generalCtl->NotifyChange(uri);
    LOG_INFO("GeneralControllerServiceImplNotifyChangeTest001::End");
}

/**
 * @tc.name: GeneralControllerServiceImplSetRegisterCallbackTest001
 * @tc.desc: Verify SetRegisterCallback() increases the count of observers_ in DataShareManagerImpl
 * @tc.type: FUNC
 * @tc.require: issueIBX9HL
 * @tc.precon:
    1. GeneralControllerServiceImpl can be instantiated via std::make_shared with a valid proxyUri parameter
    2. DataShareManagerImpl::GetInstance() returns a singleton instance of DataShareManagerImpl
    3. DataShareManagerImpl contains a member variable observers_ (of Vector type) to store registered callbacks
    4. GeneralControllerServiceImpl::SetRegisterCallback() adds a callback to DataShareManagerImpl::observers_
    5. The method observers_.Size() returns the current number of stored callbacks (initial size is non-negative)
    6. The proxyUri "datashareproxy://com.acts.ohos.data.datasharetest/test" is valid for instantiation
 * @tc.step:
    1. Define proxyUri as "datashareproxy://com.acts.ohos.data.datasharetest/test"
    2. Create a shared_ptr<GeneralControllerServiceImpl> instance generalCtl via std::make_shared(proxyUri)
    3. Assert that generalCtl is not nullptr (verify successful instantiation)
    4. Get the singleton instance of DataShareManagerImpl via GetInstance(), record as datashareManager
    5. Record the initial size of observers_ via datashareManager->observers_.Size(), store as obsSize
    6. Call generalCtl->SetRegisterCallback() to register the callback
    7. Check the new size of datashareManager->observers_
 * @tc.expect:
    1. generalCtl is not nullptr (instance created successfully)
    2. obsSize is a non-negative integer (valid initial count of observers_)
    3. After calling SetRegisterCallback(), datashareManager->observers_.Size() equals obsSize + 1 (callback added)
 */
HWTEST_F(AbnormalBranchTest, GeneralControllerServiceImplSetRegisterCallbackTest001, TestSize.Level1)
{
    LOG_INFO("GeneralControllerServiceImplSetRegisterCallbackTest001::Start");
    std::string proxyUri = "datashareproxy://com.acts.ohos.data.datasharetest/test";
    auto generalCtl = std::make_shared<GeneralControllerServiceImpl>(proxyUri);
    ASSERT_NE(generalCtl, nullptr);
    auto datashareManager = DataShareManagerImpl::GetInstance();
    size_t obsSize = datashareManager->observers_.Size();
    generalCtl->SetRegisterCallback();
    // size of dataShareManagerImpl observers_ increased by 1
    EXPECT_EQ(datashareManager->observers_.Size(), obsSize + 1);
    LOG_INFO("GeneralControllerServiceImplSetRegisterCallbackTest001::End");
}

/**
 * @tc.name: PersistentDataControllerAddQueryTemplateTest001
 * @tc.desc: Fill the branch DataShareManagerImpl::GetServiceProxy() == nullptr (AddQueryTemplate method)
 * @tc.type: FUNC
 * @tc.require: issueIBX9HL
 * @tc.precon:
    1. DataShareManagerImplHelper() mocks DataShareManagerImpl::GetServiceProxy() to return nullptr
    2. PersistentDataController supports default initialization
    3. PersistentDataController::AddQueryTemplate() accepts proxyUri, templateId (int), Template, returns int
    4. Template can be initialized with std::vector<PredicateTemplateNode> and sql string
    5. INVALID_VALUE is a predefined integer error code (returned on null proxy)
 * @tc.step:
    1. Call DataShareManagerImplHelper() to mock null proxy
    2. Define proxyUri: "datashareproxy://com.acts.ohos.data.datasharetest/test"
    3. Initialize empty std::vector<PredicateTemplateNode> nodes and sql: "select name1 as name from TBL00"
    4. Create Template instance tpl with nodes and sql
    5. Default-initialize PersistentDataController controller
    6. Call AddQueryTemplate(proxyUri, 0, tpl) and get ret
    7. Check if ret == INVALID_VALUE
 * @tc.expect:
    1. PersistentDataController instance is properly initialized
    2. AddQueryTemplate() returns INVALID_VALUE (null proxy branch triggered)
 */
HWTEST_F(AbnormalBranchTest, PersistentDataControllerAddQueryTemplateTest001, TestSize.Level1)
{
    LOG_INFO("PersistentDataControllerAddQueryTemplateTest001::Start");
    DataShareManagerImplHelper();
    std::string proxyUri = "datashareproxy://com.acts.ohos.data.datasharetest/test";
    std::vector<PredicateTemplateNode> nodes;
    Template tpl(nodes, "select name1 as name from TBL00");
    auto controller = PersistentDataController();
    auto ret = controller.AddQueryTemplate(proxyUri, 0, tpl);
    EXPECT_EQ(ret, INVALID_VALUE);
    LOG_INFO("PersistentDataControllerAddQueryTemplateTest001::End");
}

/**
 * @tc.name: PersistentDataControllerDelQueryTemplateTest001
 * @tc.desc: Fill the branch DataShareManagerImpl::GetServiceProxy() == nullptr (DelQueryTemplate method)
 * @tc.type: FUNC
 * @tc.require: issueIBX9HL
 * @tc.precon:
    1. DataShareManagerImplHelper() mocks DataShareManagerImpl::GetServiceProxy() to return nullptr
    2. PersistentDataController supports default initialization
    3. PersistentDataController::DelQueryTemplate() accepts proxyUri and templateId (int), returns int
    4. Valid proxyUri is predefined
    5. INVALID_VALUE is the predefined error code for null proxy
 * @tc.step:
    1. Call DataShareManagerImplHelper() to mock null proxy
    2. Define proxyUri: "datashareproxy://com.acts.ohos.data.datasharetest/test"
    3. Default-initialize PersistentDataController controller
    4. Call DelQueryTemplate(proxyUri, 0) and get ret
    5. Check if ret == INVALID_VALUE
 * @tc.expect:
    1. PersistentDataController instance is properly initialized
    2. DelQueryTemplate() returns INVALID_VALUE (null proxy branch entered)
 */
HWTEST_F(AbnormalBranchTest, PersistentDataControllerDelQueryTemplateTest001, TestSize.Level1)
{
    LOG_INFO("PersistentDataControllerDelQueryTemplateTest001::Start");
    DataShareManagerImplHelper();
    std::string proxyUri = "datashareproxy://com.acts.ohos.data.datasharetest/test";
    auto controller = PersistentDataController();
    auto ret = controller.DelQueryTemplate(proxyUri, 0);
    EXPECT_EQ(ret, INVALID_VALUE);
    LOG_INFO("PersistentDataControllerDelQueryTemplateTest001::End");
}

/**
 * @tc.name: PersistentDataControllerSubscribeRdbDataTest001
 * @tc.desc: Fill the branch DataShareManagerImpl::GetServiceProxy() == nullptr (SubscribeRdbData method)
 * @tc.type: FUNC
 * @tc.require: issueIBX9HL
 * @tc.precon:
    1. DataShareManagerImplHelper() mocks DataShareManagerImpl::GetServiceProxy() to return nullptr
    2. PersistentDataController supports default initialization
    3. PersistentDataController::SubscribeRdbData() accepts subscriber (void*), uris, TemplateId,
       std::function<void(const RdbChangeNode&)>, returns std::vector<OperationResult>
    4. TemplateId supports default initialization, empty vector is returned on null proxy
    5. Valid proxyUri is predefined, and uris can be initialized with it
 * @tc.step:
    1. Call DataShareManagerImplHelper() to mock null proxy
    2. Define proxyUri, create std::vector<std::string> uris = {proxyUri}
    3. Default-initialize TemplateId tplId and empty callback function
    4. Default-initialize PersistentDataController controller
    5. Call SubscribeRdbData(nullptr, uris, tplId, callback) and get ret
    6. Check the size of ret
 * @tc.expect:
    1. PersistentDataController instance is properly initialized
    2. The size of the returned ret vector is 0 (null proxy branch triggered)
 */
HWTEST_F(AbnormalBranchTest, PersistentDataControllerSubscribeRdbDataTest001, TestSize.Level1)
{
    LOG_INFO("PersistentDataControllerSubscribeRdbDataTest001::Start");
    DataShareManagerImplHelper();
    std::string proxyUri = "datashareproxy://com.acts.ohos.data.datasharetest/test";
    std::vector<std::string> uris = {proxyUri};
    TemplateId tplId;
    auto callback = std::function<void(const RdbChangeNode &)>();
    auto controller = PersistentDataController();
    auto ret = controller.SubscribeRdbData(nullptr, uris, tplId, callback);
    EXPECT_EQ(ret.size(), 0);
    LOG_INFO("PersistentDataControllerSubscribeRdbDataTest001::End");
}

/**
 * @tc.name: PersistentDataControllerUnSubscribeRdbDataTest001
 * @tc.desc: Verify UnSubscribeRdbData returns empty vector when GetServiceProxy() is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBX9HL
 * @tc.precon:
    1. DataShareManagerImplHelper() is a mock function that sets GetServiceProxy() to return nullptr
    2. PersistentDataController supports default initialization (no-parameter constructor)
    3. UnSubscribeRdbData() accepts parameters: void* subscriber, std::vector<std::string> uris,
       TemplateId tplId, and returns std::vector<OperationResult>
    4. TemplateId supports default initialization (no-parameter constructor)
    5. The proxyUri "datashareproxy://com.acts.ohos.data.datasharetest/test" is valid and can be added to uris
    6. Empty std::vector<OperationResult> is the expected return value when proxy is nullptr
 * @tc.step:
    1. Call DataShareManagerImplHelper() to mock DataShareManagerImpl::GetServiceProxy() returning nullptr
    2. Define valid proxyUri: "datashareproxy://com.acts.ohos.data.datasharetest/test"
    3. Create std::vector<std::string> uris and add proxyUri to it
    4. Default-initialize TemplateId tplId
    5. Default-initialize PersistentDataController controller
    6. Call controller.UnSubscribeRdbData(nullptr, uris, tplId) and get return value ret
    7. Check the size of ret
 * @tc.expect:
    1. PersistentDataController is initialized successfully (no crash)
    2. The size of the returned ret vector is 0 (empty vector as expected)
 */
HWTEST_F(AbnormalBranchTest, PersistentDataControllerUnSubscribeRdbDataTest001, TestSize.Level1)
{
    LOG_INFO("PersistentDataControllerUnSubscribeRdbDataTest001::Start");
    DataShareManagerImplHelper();
    std::string proxyUri = "datashareproxy://com.acts.ohos.data.datasharetest/test";
    std::vector<std::string> uris = {proxyUri};
    TemplateId tplId;
    auto controller = PersistentDataController();
    auto ret = controller.UnSubscribeRdbData(nullptr, uris, tplId);
    EXPECT_EQ(ret.size(), 0);
    LOG_INFO("PersistentDataControllerUnSubscribeRdbDataTest001::End");
}

/**
 * @tc.name: PersistentDataControllerEnableSubscribeRdbDataTest001
 * @tc.desc: Verify EnableSubscribeRdbData returns empty vector when GetServiceProxy() is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBX9HL
 * @tc.precon:
    1. DataShareManagerImplHelper() mocks DataShareManagerImpl::GetServiceProxy() to return nullptr
    2. PersistentDataController can be default-initialized
    3. PersistentDataController::EnableSubscribeRdbData() accepts void* subscriber, std::vector<std::string> uris,
       TemplateId tplId, and returns std::vector<OperationResult>
    4. TemplateId supports default initialization
    5. ProxyUri "datashareproxy://com.acts.ohos.data.datasharetest/test" is valid for uris
 * @tc.step:
    1. Call DataShareManagerImplHelper() to mock null proxy
    2. Define proxyUri and create uris vector containing proxyUri
    3. Default-initialize TemplateId tplId
    4. Default-initialize PersistentDataController controller
    5. Call controller.EnableSubscribeRdbData(nullptr, uris, tplId) to get ret
    6. Check ret.size()
 * @tc.expect:
    1. PersistentDataController initializes without error
    2. ret.size() == 0 (empty vector returned)
 */
HWTEST_F(AbnormalBranchTest, PersistentDataControllerEnableSubscribeRdbDataTest001, TestSize.Level1)
{
    LOG_INFO("PersistentDataControllerEnableSubscribeRdbDataTest001::Start");
    DataShareManagerImplHelper();
    std::string proxyUri = "datashareproxy://com.acts.ohos.data.datasharetest/test";
    std::vector<std::string> uris = {proxyUri};
    TemplateId tplId;
    auto controller = PersistentDataController();
    auto ret = controller.EnableSubscribeRdbData(nullptr, uris, tplId);
    EXPECT_EQ(ret.size(), 0);
    LOG_INFO("PersistentDataControllerEnableSubscribeRdbDataTest001::End");
}

/**
 * @tc.name: PersistentDataControllerDisableSubscribeRdbDataTest001
 * @tc.desc: Verify DisableSubscribeRdbData returns empty vector when GetServiceProxy() is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBX9HL
 * @tc.precon:
    1. DataShareManagerImplHelper() mocks DataShareManagerImpl::GetServiceProxy() to return nullptr
    2. PersistentDataController supports default initialization
    3. PersistentDataController::DisableSubscribeRdbData() accepts void* subscriber, std::vector<std::string> uris,
       TemplateId tplId, and returns std::vector<OperationResult>
    4. TemplateId can be default-initialized
    5. ProxyUri "datashareproxy://com.acts.ohos.data.datasharetest/test" is valid for uris
 * @tc.step:
    1. Call DataShareManagerImplHelper() to mock null proxy
    2. Define proxyUri and create uris vector with proxyUri
    3. Default-initialize TemplateId tplId
    4. Default-initialize PersistentDataController controller
    5. Call controller.DisableSubscribeRdbData(nullptr, uris, tplId) to get ret
    6. Check ret.size()
 * @tc.expect:
    1. PersistentDataController initializes successfully
    2. ret.size() == 0 (empty vector returned)
 */
HWTEST_F(AbnormalBranchTest, PersistentDataControllerDisableSubscribeRdbDataTest001, TestSize.Level1)
{
    LOG_INFO("PersistentDataControllerDisableSubscribeRdbDataTest001::Start");
    DataShareManagerImplHelper();
    std::string proxyUri = "datashareproxy://com.acts.ohos.data.datasharetest/test";
    std::vector<std::string> uris = {proxyUri};
    TemplateId tplId;
    auto controller = PersistentDataController();
    auto ret = controller.DisableSubscribeRdbData(nullptr, uris, tplId);
    EXPECT_EQ(ret.size(), 0);
    LOG_INFO("PersistentDataControllerDisableSubscribeRdbDataTest001::End");
}

/**
 * @tc.name: PublishedDataControllerPublishTest001
 * @tc.desc: Verify Publish returns empty vector when GetServiceProxy() is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBX9HL
 * @tc.precon:
    1. DataShareManagerImplHelper() mocks DataShareManagerImpl::GetServiceProxy() to return nullptr
    2. PublishedDataController supports default initialization (no-parameter constructor)
    3. PublishedDataController::Publish() accepts parameters: Data data, std::string bundleName,
       and returns std::vector<OperationResult>
    4. Data class supports default initialization (no-parameter constructor)
    5. The bundleName "com.acts.ohos.data.datasharetest" is a valid input parameter
    6. When DataShareManagerImpl::GetServiceProxy() returns nullptr, Publish() is expected to return an empty vector
 * @tc.step:
    1. Call DataShareManagerImplHelper() to mock DataShareManagerImpl::GetServiceProxy() returning nullptr
    2. Default-initialize Data data
    3. Define bundleName as "com.acts.ohos.data.datasharetest"
    4. Default-initialize PublishedDataController controller
    5. Call controller.Publish(data, bundleName) and record the return value ret
    6. Check the size of ret
 * @tc.expect:
    1. PublishedDataController is initialized successfully (no crash during instantiation)
    2. The size of the returned ret vector is 0 (empty vector as expected)
 */
HWTEST_F(AbnormalBranchTest, PublishedDataControllerPublishTest001, TestSize.Level1)
{
    LOG_INFO("PublishedDataControllerPublishTest001::Start");
    DataShareManagerImplHelper();
    Data data;
    std::string bundleName = "com.acts.ohos.data.datasharetest";
    auto controller = PublishedDataController();
    auto ret = controller.Publish(data, bundleName);
    EXPECT_EQ(ret.size(), 0);
    LOG_INFO("PublishedDataControllerPublishTest001::End");
}

/**
 * @tc.name: PublishedDataControllerGetPublishedDataTest001
 * @tc.desc: Verify GetPublishedData returns Data with empty datas_ vector when GetServiceProxy() is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBX9HL
 * @tc.precon:
    1. DataShareManagerImplHelper() mocks DataShareManagerImpl::GetServiceProxy() to return nullptr
    2. PublishedDataController supports default initialization (no-parameter constructor)
    3. PublishedDataController::GetPublishedData() accepts parameters: std::string bundleName, int& errCode,
       and returns a Data object
    4. Data class contains a member variable datas_ (of vector type) to store data entries
    5. The bundleName "com.acts.ohos.data.datasharetest" is a valid input parameter
    6. When GetServiceProxy() returns nullptr, GetPublishedData() is expected to return Data with empty datas_
 * @tc.step:
    1. Call DataShareManagerImplHelper() to mock DataShareManagerImpl::GetServiceProxy() returning nullptr
    2. Define bundleName as "com.acts.ohos.data.datasharetest"
    3. Initialize int errCode to 0
    4. Default-initialize PublishedDataController controller
    5. Call controller.GetPublishedData(bundleName, errCode) and record the return value ret
    6. Check the size of ret.datas_
 * @tc.expect:
    1. PublishedDataController is initialized successfully (no crash during instantiation)
    2. The size of ret.datas_ is 0 (empty vector as expected)
 */
HWTEST_F(AbnormalBranchTest, PublishedDataControllerGetPublishedDataTest001, TestSize.Level1)
{
    LOG_INFO("PublishedDataControllerGetPublishedDataTest001::Start");
    DataShareManagerImplHelper();
    std::string bundleName = "com.acts.ohos.data.datasharetest";
    int errCode = 0;
    auto controller = PublishedDataController();
    auto ret = controller.GetPublishedData(bundleName, errCode);
    EXPECT_EQ(ret.datas_.size(), 0);
    LOG_INFO("PublishedDataControllerGetPublishedDataTest001::End");
}

/**
 * @tc.name: PublishedDataControllerSubscribePublishedDataTest001
 * @tc.desc: Verify SubscribePublishedData returns empty vector when GetServiceProxy is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBX9HL
 * @tc.precon:
    1. DataShareManagerImplHelper() mocks DataShareManagerImpl::GetServiceProxy() to return nullptr
    2. PublishedDataController supports default initialization (no-parameter constructor)
    3. PublishedDataController::SubscribePublishedData() accepts parameters: void* subscriber,
       std::vector<std::string> uris, int64_t subscriberId, std::function<void(const PublishedDataChangeNode&)>,
       and returns std::vector<OperationResult>
    4. The proxyUri is valid and can be added to the uris vector
    5. std::function<void(const PublishedDataChangeNode&)> supports empty initialization
    6. When GetServiceProxy returns nullptr, SubscribePublishedData is expected to return an empty vector
 * @tc.step:
    1. Call DataShareManagerImplHelper() to mock DataShareManagerImpl::GetServiceProxy() returning nullptr
    2. Define proxyUri as "datashareproxy://com.acts.ohos.data.datasharetest/test"
    3. Create a std::vector<std::string> uris and add proxyUri to it
    4. Set int64_t subscriberId to 0, and initialize an empty callback function
    5. Default-initialize PublishedDataController controller
    6. Call controller.SubscribePublishedData(nullptr, uris, subscriberId, callback) and record the return value ret
    7. Check the size of ret
 * @tc.expect:
    1. PublishedDataController is initialized successfully (no crash during instantiation)
    2. The size of the returned ret vector is 0 (empty vector as expected)
 */
HWTEST_F(AbnormalBranchTest, PublishedDataControllerSubscribePublishedDataTest001, TestSize.Level1)
{
    LOG_INFO("PublishedDataControllerSubscribePublishedDataTest001::Start");
    DataShareManagerImplHelper();
    std::string proxyUri = "datashareproxy://com.acts.ohos.data.datasharetest/test";
    std::vector<std::string> uris = {proxyUri};
    auto callback = std::function<void(const PublishedDataChangeNode &changeNode)>();
    auto controller = PublishedDataController();
    auto ret = controller.SubscribePublishedData(nullptr, uris, 0, callback);
    EXPECT_EQ(ret.size(), 0);
    LOG_INFO("PublishedDataControllerSubscribePublishedDataTest001::End");
}

/**
 * @tc.name: PublishedDataControllerSubscribeUnSubscribePublishedDataTest001
 * @tc.desc: Verify UnSubscribePublishedData returns empty vector when GetServiceProxy is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBX9HL
 * @tc.precon:
    1. DataShareManagerImplHelper() mocks DataShareManagerImpl::GetServiceProxy() to return nullptr
    2. PublishedDataController supports default initialization (no-parameter constructor)
    3. PublishedDataController::UnSubscribePublishedData() accepts parameters: void* subscriber,
       std::vector<std::string> uris, int64_t subscriberId, and returns std::vector<OperationResult>
    4. The proxyUri is valid and can be added to the uris vector
    5. When GetServiceProxy returns nullptr, UnSubscribePublishedData is expected to return an empty vector
 * @tc.step:
    1. Call DataShareManagerImplHelper() to mock DataShareManagerImpl::GetServiceProxy() returning nullptr
    2. Define proxyUri as "datashareproxy://com.acts.ohos.data.datasharetest/test"
    3. Create a std::vector<std::string> uris and add proxyUri to it
    4. Set int64_t subscriberId to 0
    5. Default-initialize PublishedDataController controller
    6. Call controller.UnSubscribePublishedData(nullptr, uris, subscriberId) and record the return value ret
    7. Check the size of ret
 * @tc.expect:
    1. PublishedDataController is initialized successfully (no crash during instantiation)
    2. The size of the returned ret vector is 0 (empty vector as expected)
 */
HWTEST_F(AbnormalBranchTest, PublishedDataControllerSubscribeUnSubscribePublishedDataTest001, TestSize.Level1)
{
    LOG_INFO("PublishedDataControllerSubscribeUnSubscribePublishedDataTest001::Start");
    DataShareManagerImplHelper();
    std::string proxyUri = "datashareproxy://com.acts.ohos.data.datasharetest/test";
    std::vector<std::string> uris = {proxyUri};
    auto controller = PublishedDataController();
    auto ret = controller.UnSubscribePublishedData(nullptr, uris, 0);
    EXPECT_EQ(ret.size(), 0);
    LOG_INFO("PublishedDataControllerSubscribeUnSubscribePublishedDataTest001::End");
}

/**
 * @tc.name: PublishedDataControllerSubscribeEnableSubscribePublishedDataTest001
 * @tc.desc: Verify EnableSubscribePublishedData returns empty vector when GetServiceProxy is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBX9HL
 * @tc.precon:
    1. DataShareManagerImplHelper() mocks DataShareManagerImpl::GetServiceProxy() to return nullptr
    2. PublishedDataController supports default initialization (no-parameter constructor)
    3. PublishedDataController::EnableSubscribePublishedData() accepts parameters: void* subscriber,
       std::vector<std::string> uris, int64_t subscriberId, and returns std::vector<OperationResult>
    4. The proxyUri is valid and can be added to the uris vector
    5. When GetServiceProxy returns nullptr, EnableSubscribePublishedData is expected to return an empty vector
 * @tc.step:
    1. Call DataShareManagerImplHelper() to mock DataShareManagerImpl::GetServiceProxy() returning nullptr
    2. Define proxyUri as "datashareproxy://com.acts.ohos.data.datasharetest/test"
    3. Create a std::vector<std::string> uris and add proxyUri to it
    4. Set int64_t subscriberId to 0
    5. Default-initialize PublishedDataController controller
    6. Call controller.EnableSubscribePublishedData(nullptr, uris, subscriberId) and record the return value ret
    7. Check the size of ret
 * @tc.expect:
    1. PublishedDataController is initialized successfully (no crash during instantiation)
    2. The size of the returned ret vector is 0 (empty vector as expected)
 */
HWTEST_F(AbnormalBranchTest, PublishedDataControllerSubscribeEnableSubscribePublishedDataTest001, TestSize.Level1)
{
    LOG_INFO("PublishedDataControllerSubscribeEnableSubscribePublishedDataTest001::Start");
    DataShareManagerImplHelper();
    std::string proxyUri = "datashareproxy://com.acts.ohos.data.datasharetest/test";
    std::vector<std::string> uris = {proxyUri};
    auto controller = PublishedDataController();
    auto ret = controller.EnableSubscribePublishedData(nullptr, uris, 0);
    EXPECT_EQ(ret.size(), 0);
    LOG_INFO("PublishedDataControllerSubscribeEnableSubscribePublishedDataTest001::End");
}

/**
 * @tc.name: PublishedDataControllerSubscribeDisableSubscribePublishedDataTest001
 * @tc.desc: Verify DisableSubscribePublishedData returns empty vector when GetServiceProxy is nullptr
 * @tc.type: FUNC
 * @tc.require: issueIBX9HL
 * @tc.precon:
    1. DataShareManagerImplHelper() mocks DataShareManagerImpl::GetServiceProxy() to return nullptr
    2. PublishedDataController supports default initialization (no-parameter constructor)
    3. PublishedDataController::DisableSubscribePublishedData() accepts parameters: void* subscriber,
       std::vector<std::string> uris, int64_t subscriberId, and returns std::vector<OperationResult>
    4. The proxyUri is valid and can be added to the uris vector
    5. When GetServiceProxy() returns nullptr, DisableSubscribePublishedData is expected to return an empty vector
 * @tc.step:
    1. Call DataShareManagerImplHelper() to mock DataShareManagerImpl::GetServiceProxy() returning nullptr
    2. Define proxyUri as "datashareproxy://com.acts.ohos.data.datasharetest/test"
    3. Create a std::vector<std::string> uris and add proxyUri to it
    4. Set int64_t subscriberId to 0
    5. Default-initialize PublishedDataController controller
    6. Call controller.DisableSubscribePublishedData(nullptr, uris, subscriberId) and record the return value ret
    7. Check the size of ret
 * @tc.expect:
    1. PublishedDataController is initialized successfully (no crash during instantiation)
    2. The size of the returned ret vector is 0 (empty vector as expected)
 */
HWTEST_F(AbnormalBranchTest, PublishedDataControllerSubscribeDisableSubscribePublishedDataTest001, TestSize.Level1)
{
    LOG_INFO("PublishedDataControllerSubscribeDisableSubscribePublishedDataTest001::Start");
    DataShareManagerImplHelper();
    std::string proxyUri = "datashareproxy://com.acts.ohos.data.datasharetest/test";
    std::vector<std::string> uris = {proxyUri};
    auto controller = PublishedDataController();
    auto ret = controller.DisableSubscribePublishedData(nullptr, uris, 0);
    EXPECT_EQ(ret.size(), 0);
    LOG_INFO("PublishedDataControllerSubscribeDisableSubscribePublishedDataTest001::End");
}
} // namespace DataShare
} // namespace OHOS