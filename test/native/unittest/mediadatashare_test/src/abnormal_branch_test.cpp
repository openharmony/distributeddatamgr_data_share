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
 * @tc.precon: None
 * @tc.step:
    1. Create DataShareBlockWriterImpl instance
    2. Call various write operations (AllocRow, Write with different types)
    3. Check return values of all operations
 * @tc.expect:
    1. All operations return E_ERROR
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
 * @tc.precon: None
 * @tc.step:
    1. Create ISharedResultSetStub instance with null parameter
    2. Call CreateStub method with null result and valid parcel
    3. Check returned ISharedResultSet pointer
 * @tc.expect:
    1. CreateStub returns nullptr
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
 * @tc.precon: None
 * @tc.step:
    1. Create DataShareKvServiceProxy with null parameter
    2. Call RegisterClientDeathObserver with empty appId and null observer
    3. Check return value
 * @tc.expect:
    1. Method returns -1 (failure)
 */
HWTEST_F(AbnormalBranchTest, AbnormalBranchTest_RegisterClientDeathObserverNull_Test_001, TestSize.Level0)
{
    LOG_INFO("AbnormalBranchTest_RegisterClientDeathObserverNull_Test_001::Start");
    DataShareKvServiceProxy proxy(nullptr);
    std::string appId;
    uint32_t result = proxy.RegisterClientDeathObserver(appId, nullptr);
    EXPECT_EQ(result, -1);
    LOG_INFO("AbnormalBranchTest_RegisterClientDeathObserverNull_Test_001::End");
}

/**
 * @tc.name: AbnormalBranchTest_mReadOnlyInvalid_Test_001
 * @tc.desc: Verify invalid operations on read-only SharedBlock
 * @tc.type: FUNC
 * @tc.precon: None
 * @tc.step:
    1. Create read-only SharedBlock instance
    2. Attempt modification operations (Clear, SetColumnNum, AllocRow, etc.)
    3. Check return values of all operations
 * @tc.expect:
    1. All modification operations return SHARED_BLOCK_INVALID_OPERATION
    2. Init operation succeeds
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
 * @tc.precon: None
 * @tc.step:
    1. Prepare CreateOptions with null token
    2. Call DataShareHelper::Creator with empty URI and prepared options
    3. Check returned DataShareHelper pointer
 * @tc.expect:
    1. Creator returns nullptr (failure to create helper)
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
 * @tc.precon: None
 * @tc.step:
    1. Prepare null proxy and empty URIs list
    2. Call AddObservers with null subscriber, proxy and empty URIs
    3. Check size of returned results
 * @tc.expect:
    1. Results size equals URIs size (0 in this case)
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
 * @tc.precon: None
 * @tc.step:
    1. Prepare null proxy and empty URIs list
    2. Call AddObservers with null subscriber, proxy and empty URIs
    3. Check size of returned results
 * @tc.expect:
    1. Results size equals URIs size (0 in this case)
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
 * @tc.precon: None
 * @tc.step:
    1. Prepare null proxy and empty URIs list
    2. Call DelObservers with null subscriber and proxy (both overloads)
    3. Check size of returned results
 * @tc.expect:
    1. Results size equals URIs size (0 in this case) for both overloads
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
 * @tc.precon: None
 * @tc.step:
    1. Prepare null proxy and empty URIs list
    2. Call DelObservers with null subscriber and proxy (both overloads)
    3. Check size of returned results
 * @tc.expect:
    1. Results size equals URIs size (0 in this case) for both overloads
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
 * @tc.precon: None
 * @tc.step:
    1. Prepare null proxy and empty URIs list
    2. Call EnableObservers with null subscriber, proxy and empty URIs
    3. Check size of returned results
 * @tc.expect:
    1. Results size equals URIs size (0 in this case)
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
 * @tc.precon: None
 * @tc.step:
    1. Prepare null proxy and empty URIs list
    2. Call EnableObservers with null subscriber, proxy and empty URIs
    3. Check size of returned results
 * @tc.expect:
    1. Results size equals URIs size (0 in this case)
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
 * @tc.precon: None
 * @tc.step:
    1. Prepare null proxy and empty URIs list
    2. Call DisableObservers with null subscriber, proxy and empty URIs
    3. Check size of returned results
 * @tc.expect:
    1. Results size equals URIs size (0 in this case)
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
 * @tc.precon: None
 * @tc.step:
    1. Prepare null proxy and empty URIs list
    2. Call DisableObservers with null subscriber, proxy and empty URIs
    3. Check size of returned results
 * @tc.expect:
    1. Results size equals URIs size (0 in this case)
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
 * @tc.precon: None
 * @tc.step:
    1. Prepare null subscriber and valid proxy
    2. Call DisableObservers with null proxy and valid URIs
    3. Call DelObservers with valid proxy and URIs
    4. Call RecoverObservers with null proxy
    5. Check results sizes
 * @tc.expect:
    1. DisableObservers with null proxy returns 0 results
    2. DelObservers with valid proxy returns results matching URIs size
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
 * @tc.precon: None
 * @tc.step:
    1. Create two PublishedDataObserver instances with identical callbacks
    2. Compare them using != and == operators
    3. Check comparison results
 * @tc.expect:
    1. != operator returns true (observers are distinct)
    2. == operator returns false (observers are distinct)
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
 * @tc.precon: None
 * @tc.step:
    1. Prepare null subscriber and valid proxy
    2. Call DelObservers with valid proxy and URIs
    3. Call RecoverObservers with null proxy
    4. Check results size
 * @tc.expect:
    1. DelObservers returns results matching URIs size
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
 * @tc.precon: None
 * @tc.step:
    1. Prepare null subscriber and valid proxy
    2. Call DisableObservers with null proxy and valid URIs
    3. Call DisableObservers with valid proxy and URIs
    4. Call RecoverObservers with null proxy
    5. Check results sizes
 * @tc.expect:
    1. DisableObservers with null proxy returns 0 results
    2. DisableObservers with valid proxy returns results matching URIs size
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
 * @tc.name: RdbSubscriberManagerOperatorTest001
 * @tc.desc: Verify RdbObserver comparison operators
 * @tc.type: FUNC
 * @tc.precon: None
 * @tc.step:
    1. Create two RdbObserver instances with identical callbacks
    2. Compare them using != and == operators
    3. Check comparison results
 * @tc.expect:
    1. != operator returns true (observers are distinct)
    2. == operator returns false (observers are distinct)
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
 * @tc.precon: None
 * @tc.step:
    1. Create DataShareManagerImpl instance with null service and empty bundle name
    2. Call RegisterClientDeathObserver
    3. Set valid service proxy and call again
    4. Set valid bundle name and call again
    5. Check clientDeathObserverPtr_ state
 * @tc.expect:
    1. Observer remains null with empty bundle name
    2. Observer is created when valid bundle name is provided
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
    datashareManager->RegisterClientDeathObserver();
    EXPECT_EQ(datashareManager->clientDeathObserverPtr_, nullptr);
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
* @tc.precon: None
* @tc.expect: Successfully register client death observer
* @tc.step:  1. Create a DataShareManagerImpl instance;
             2. Try to use the instance to get datashare service proxy;
             3. Check whether all member functions of the object are successfully constructed.
* @tc.require: issueIBX9HL
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
* @tc.precon: None
* @tc.expect: Successfully process OnRemoteDied
* @tc.step:  1. Create a DataShareManagerImpl instance;
             2. Call the OnRemoteDied() function;
             3. Check whether datashareManager is successfully deconstructed.
* @tc.require: issueIBX9HL
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
* @tc.precon: None
* @tc.expect: Successfully process SetRegisterCallback
* @tc.step:  1. Create a DataShareManagerImpl instance;
             2. Call the SetRegisterCallback() function;
             3. Check whether datashareManager is successfully deconstructed.
* @tc.require: issueIBX9HL
*/
HWTEST_F(AbnormalBranchTest, SetRegisterCallbackTest001, TestSize.Level0)
{
    LOG_INFO("SetRegisterCallbackTest001::Start");
    auto datashareManager = DataShareManagerImpl::GetInstance();
    std::function<void()> callback = [](){};
    datashareManager->SetRegisterCallback(nullptr, callback);
    LOG_INFO("SetRegisterCallbackTest001::End");
}

/**
 * @tc.name: AmsMgrProxyOnProxyDiedTest001
 * @tc.desc: Verify AmsMgrProxy behavior when proxy dies
 * @tc.type: FUNC
 * @tc.precon: None
 * @tc.step:
    1. Create AmsMgrProxy with null service and proxy
    2. Call OnProxyDied and verify state
    3. Create new proxy, connect to SA, verify initialization
    4. Call OnProxyDied again and verify cleanup
 * @tc.expect:
    1. OnProxyDied handles null pointers gracefully
    2. Proxy initializes properly after ConnectSA
    3. OnProxyDied cleans up resources
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
* @tc.precon: None
* @tc.expect: Successfully process SetRegisterCallback
* @tc.step:  1. Create a AmsMgrProxy instance;
             2. Clear the proxy;
             3. After clear, try to connect SA;
             4. Check if this proxy can be connected successfully.
* @tc.require: issueIBX9HL
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
 * @tc.desc: Verify DataShareServiceProxy subscription behavior after proxy death
 * @tc.type: FUNC
 * @tc.precon: None
 * @tc.step:
    1. Create AmsMgrProxy with null service and proxy
    2. Call OnProxyDied and verify cleanup
    3. Create new proxy, connect to SA, verify initialization
    4. Call OnProxyDied again and verify cleanup
 * @tc.expect:
    1. Proxy handles null states gracefully
    2. Proxy initializes properly after ConnectSA
    3. Resources are cleaned up after OnProxyDied
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
 * @tc.precon: None
 * @tc.step:
    1. Prepare empty URIs list and null observer
    2. Call SubscribeRdbData with valid proxy
    3. Check size of returned result set
 * @tc.expect:
    1. Result set size is 0 (matches empty URIs list)
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
 * @tc.precon: None
 * @tc.step:
    1. Prepare empty URIs list and null observer
    2. Call SubscribePublishedData with valid proxy
    3. Check size of returned result set
 * @tc.expect:
    1. Result set size is 0 (matches empty URIs list)
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
* @tc.desc: Fill the branch DataShareManagerImpl::GetServiceProxy() == nullptr
* @tc.type: FUNC
* @tc.precon: Mock the DataShareManagerImpl::GetServiceProxy() return nullptr
* @tc.expect: Enter the DataShareManagerImpl::GetServiceProxy() branch in the code and return an DATA_SHARE_ERROR.
* @tc.step:  1. Mock GetServiceProxy() to return nullptr;
             2. Call Insert() in GeneralControllerServiceImpl;
             3. Check if the function return DATA_SHARE_ERROR.
* @tc.require: issueIBX9HL
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
* @tc.desc: Fill the branch DataShareManagerImpl::GetServiceProxy() == nullptr
* @tc.type: FUNC
* @tc.precon: Mock the DataShareManagerImpl::GetServiceProxy() return nullptr
* @tc.expect: Enter the DataShareManagerImpl::GetServiceProxy() branch in the code and return an DATA_SHARE_ERROR.
* @tc.step:  1. Mock GetServiceProxy() to return nullptr;
             2. Call Update() in GeneralControllerServiceImpl;
             3. Check if the function return DATA_SHARE_ERROR.
* @tc.require: issueIBX9HL
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
* @tc.desc: Fill the branch DataShareManagerImpl::GetServiceProxy() == nullptr
* @tc.type: FUNC
* @tc.precon: Mock the DataShareManagerImpl::GetServiceProxy() return nullptr
* @tc.expect: Enter the DataShareManagerImpl::GetServiceProxy() branch in the code and return an DATA_SHARE_ERROR.
* @tc.step:  1. Mock GetServiceProxy() to return nullptr;
             2. Call Update() in GeneralControllerServiceImpl;
             3. Check if the function return DATA_SHARE_ERROR.
* @tc.require: issueIBX9HL
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
* @tc.desc: Fill the branch DataShareManagerImpl::GetServiceProxy() == nullptr
* @tc.type: FUNC
* @tc.precon: Mock the DataShareManagerImpl::GetServiceProxy() return nullptr
* @tc.expect: Enter the DataShareManagerImpl::GetServiceProxy() branch in the code
              and return pair of DATA_SHARE_ERROR and 0.
* @tc.step:  1. Mock GetServiceProxy() to return nullptr;
             2. Call InsertEx() in GeneralControllerServiceImpl;
             3. Check if the function return pair of DATA_SHARE_ERROR and 0.
* @tc.require: issueIBX9HL
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
* @tc.desc: Fill the branch DataShareManagerImpl::GetServiceProxy() == nullptr
* @tc.type: FUNC
* @tc.precon: Mock the DataShareManagerImpl::GetServiceProxy() return nullptr
* @tc.expect: Enter the DataShareManagerImpl::GetServiceProxy() branch in the code
              and return pair of DATA_SHARE_ERROR and 0.
* @tc.step:  1. Mock GetServiceProxy() to return nullptr;
             2. Call UpdateEx() in GeneralControllerServiceImpl;
             3. Check if the function return pair of DATA_SHARE_ERROR and 0.
* @tc.require: issueIBX9HL
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
* @tc.desc: Fill the branch DataShareManagerImpl::GetServiceProxy() == nullptr
* @tc.type: FUNC
* @tc.precon: Mock the DataShareManagerImpl::GetServiceProxy() return nullptr
* @tc.expect: Enter the DataShareManagerImpl::GetServiceProxy() branch in the code
              and return pair of DATA_SHARE_ERROR and 0.
* @tc.step:  1. Mock GetServiceProxy() to return nullptr;
             2. Call DeleteEx() in GeneralControllerServiceImpl;
             3. Check if the function return pair of DATA_SHARE_ERROR and 0.
* @tc.require: issueIBX9HL
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
* @tc.desc: Fill the branch DataShareManagerImpl::GetServiceProxy() == nullptr
* @tc.type: FUNC
* @tc.precon: Mock the DataShareManagerImpl::GetServiceProxy() return nullptr
* @tc.expect: Enter the DataShareManagerImpl::GetServiceProxy() branch in the code and return a nullptr.
* @tc.step:  1. Mock GetServiceProxy() to return nullptr;
             2. Call Query() in GeneralControllerServiceImpl;
             3. Check if the function return nullptr.
* @tc.require: issueIBX9HL
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
* @tc.desc: Fill the branch DataShareManagerImpl::GetServiceProxy() == nullptr
* @tc.type: FUNC
* @tc.precon: Mock the DataShareManagerImpl::GetServiceProxy() return nullptr
* @tc.expect: Enter the DataShareManagerImpl::GetServiceProxy() branch in the code.
* @tc.step:  1. Mock GetServiceProxy() to return nullptr;
             2. Call NotifyChange() in GeneralControllerServiceImpl;
             3. Check if the function return.
* @tc.require: issueIBX9HL
*/
HWTEST_F(AbnormalBranchTest, GeneralControllerServiceImplNotifyChangeTest001, TestSize.Level1)
{
    LOG_INFO("GeneralControllerServiceImplNotifyChangeTest001::Start");
    DataShareManagerImplHelper();
    std::string proxyUri = "datashareproxy://com.acts.ohos.data.datasharetest/test";
    Uri uri(proxyUri);
    auto generalCtl = std::make_shared<GeneralControllerServiceImpl>(proxyUri);
    generalCtl->NotifyChange(uri);
    LOG_INFO("GeneralControllerServiceImplNotifyChangeTest001::End");
}

/**
* @tc.name: GeneralControllerServiceImplSetRegisterCallbackTest001
* @tc.desc: Check the main process
* @tc.type: FUNC
* @tc.precon: None
* @tc.expect: Enter the DataShareManagerImpl::GetInstance() branch in the code.
* @tc.step:  1. Call SetRegisterCallback() in GeneralControllerServiceImpl;
             2. Check if the function return.
* @tc.require: issueIBX9HL
*/
HWTEST_F(AbnormalBranchTest, GeneralControllerServiceImplSetRegisterCallbackTest001, TestSize.Level1)
{
    LOG_INFO("GeneralControllerServiceImplSetRegisterCallbackTest001::Start");
    std::string proxyUri = "datashareproxy://com.acts.ohos.data.datasharetest/test";
    auto generalCtl = std::make_shared<GeneralControllerServiceImpl>(proxyUri);
    generalCtl->SetRegisterCallback();
    LOG_INFO("GeneralControllerServiceImplSetRegisterCallbackTest001::End");
}

/**
* @tc.name: PersistentDataControllerAddQueryTemplateTest001
* @tc.desc: Fill the branch DataShareManagerImpl::GetServiceProxy() == nullptr
* @tc.type: FUNC
* @tc.precon: Mock the DataShareManagerImpl::GetServiceProxy() return nullptr
* @tc.expect: Enter the DataShareManagerImpl::GetServiceProxy() branch in the code and return INVALID_VALUE
* @tc.step:  1. Mock GetServiceProxy() to return nullptr;
             2. Call AddQueryTemplate() in PersistentDataController;
             3. Check if the function return INVALID_VALUE.
* @tc.require: issueIBX9HL
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
* @tc.desc: Fill the branch DataShareManagerImpl::GetServiceProxy() == nullptr
* @tc.type: FUNC
* @tc.precon: Mock the DataShareManagerImpl::GetServiceProxy() return nullptr
* @tc.expect: Enter the DataShareManagerImpl::GetServiceProxy() branch in the code and return INVALID_VALUE
* @tc.step:  1. Mock GetServiceProxy() to return nullptr;
             2. Call DelQueryTemplate() in PersistentDataController;
             3. Check if the function return INVALID_VALUE.
* @tc.require: issueIBX9HL
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
* @tc.desc: Fill the branch DataShareManagerImpl::GetServiceProxy() == nullptr
* @tc.type: FUNC
* @tc.precon: Mock the DataShareManagerImpl::GetServiceProxy() return nullptr
* @tc.expect: Enter the DataShareManagerImpl::GetServiceProxy() branch in the code and return empty vector
* @tc.step:  1. Mock GetServiceProxy() to return nullptr;
             2. Call SubscribeRdbData() in PersistentDataController;
             3. Check if the function return empty vector.
* @tc.require: issueIBX9HL
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
* @tc.desc: Fill the branch DataShareManagerImpl::GetServiceProxy() == nullptr
* @tc.type: FUNC
* @tc.precon: Mock the DataShareManagerImpl::GetServiceProxy() return nullptr
* @tc.expect: Enter the DataShareManagerImpl::GetServiceProxy() branch in the code and return empty vector
* @tc.step:  1. Mock GetServiceProxy() to return nullptr;
             2. Call UnSubscribeRdbData() in PersistentDataController;
             3. Check if the function return empty vector.
* @tc.require: issueIBX9HL
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
* @tc.desc: Fill the branch DataShareManagerImpl::GetServiceProxy() == nullptr
* @tc.type: FUNC
* @tc.precon: Mock the DataShareManagerImpl::GetServiceProxy() return nullptr
* @tc.expect: Enter the DataShareManagerImpl::GetServiceProxy() branch in the code and return empty vector
* @tc.step:  1. Mock GetServiceProxy() to return nullptr;
             2. Call EnableSubscribeRdbData() in PersistentDataController;
             3. Check if the function return empty vector.
* @tc.require: issueIBX9HL
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
* @tc.desc: Fill the branch DataShareManagerImpl::GetServiceProxy() == nullptr
* @tc.type: FUNC
* @tc.precon: Mock the DataShareManagerImpl::GetServiceProxy() return nullptr
* @tc.expect: Enter the DataShareManagerImpl::GetServiceProxy() branch in the code and return empty vector
* @tc.step:  1. Mock GetServiceProxy() to return nullptr;
             2. Call DisableSubscribeRdbData() in PersistentDataController;
             3. Check if the function return empty vector.
* @tc.require: issueIBX9HL
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
* @tc.desc: Fill the branch DataShareManagerImpl::GetServiceProxy() == nullptr
* @tc.type: FUNC
* @tc.precon: Mock the DataShareManagerImpl::GetServiceProxy() return nullptr
* @tc.expect: Enter the DataShareManagerImpl::GetServiceProxy() branch in the code and return empty vector
* @tc.step:  1. Mock GetServiceProxy() to return nullptr;
             2. Call Publish() in PublishedDataController;
             3. Check if the function return empty vector.
* @tc.require: issueIBX9HL
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
* @tc.desc: Fill the branch DataShareManagerImpl::GetServiceProxy() == nullptr
* @tc.type: FUNC
* @tc.precon: Mock the DataShareManagerImpl::GetServiceProxy() return nullptr
* @tc.expect: Enter the DataShareManagerImpl::GetServiceProxy() branch in the code
              and return empty datas_ vector in Data.
* @tc.step:  1. Mock GetServiceProxy() to return nullptr;
             2. Call GetPublishedData() in PublishedDataController;
             3. Check if the function return empty datas_ vector in Data.
* @tc.require: issueIBX9HL
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
* @tc.desc: Fill the branch DataShareManagerImpl::GetServiceProxy() == nullptr
* @tc.type: FUNC
* @tc.precon: Mock the DataShareManagerImpl::GetServiceProxy() return nullptr
* @tc.expect: Enter the DataShareManagerImpl::GetServiceProxy() branch in the code and return empty vector
* @tc.step:  1. Mock GetServiceProxy() to return nullptr;
             2. Call SubscribePublishedData() in PublishedDataController;
             3. Check if the function return empty vector.
* @tc.require: issueIBX9HL
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
* @tc.desc: Fill the branch DataShareManagerImpl::GetServiceProxy() == nullptr
* @tc.type: FUNC
* @tc.precon: Mock the DataShareManagerImpl::GetServiceProxy() return nullptr
* @tc.expect: Enter the DataShareManagerImpl::GetServiceProxy() branch in the code and return empty vector
* @tc.step:  1. Mock GetServiceProxy() to return nullptr;
             2. Call UnSubscribePublishedData() in PublishedDataController;
             3. Check if the function return empty vector.
* @tc.require: issueIBX9HL
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
* @tc.desc: Fill the branch DataShareManagerImpl::GetServiceProxy() == nullptr
* @tc.type: FUNC
* @tc.precon: Mock the DataShareManagerImpl::GetServiceProxy() return nullptr
* @tc.expect: Enter the DataShareManagerImpl::GetServiceProxy() branch in the code and return empty vector
* @tc.step:  1. Mock GetServiceProxy() to return nullptr;
             2. Call EnableSubscribePublishedData() in PublishedDataController;
             3. Check if the function return empty vector.
* @tc.require: issueIBX9HL
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
* @tc.desc: Fill the branch DataShareManagerImpl::GetServiceProxy() == nullptr
* @tc.type: FUNC
* @tc.precon: Mock the DataShareManagerImpl::GetServiceProxy() return nullptr
* @tc.expect: Enter the DataShareManagerImpl::GetServiceProxy() branch in the code and return empty vector
* @tc.step:  1. Mock GetServiceProxy() to return nullptr;
             2. Call DisableSubscribePublishedData() in PublishedDataController;
             3. Check if the function return empty vector.
* @tc.require: issueIBX9HL
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