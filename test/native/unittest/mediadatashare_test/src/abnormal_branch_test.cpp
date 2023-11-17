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
#include <gtest/gtest.h>
#include <unistd.h>
#include "accesstoken_kit.h"
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

namespace OHOS {
namespace DataShare {
using namespace testing::ext;
using namespace OHOS::Security::AccessToken;

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

HWTEST_F(AbnormalBranchTest, AbnormalBranchTest_RegisterClientDeathObserverNull_Test_001, TestSize.Level0)
{
    LOG_INFO("AbnormalBranchTest_RegisterClientDeathObserverNull_Test_001::Start");
    DataShareKvServiceProxy proxy(nullptr);
    std::string appId;
    uint32_t result = proxy.RegisterClientDeathObserver(appId, nullptr);
    EXPECT_EQ(result, -1);
    LOG_INFO("AbnormalBranchTest_RegisterClientDeathObserverNull_Test_001::End");
}

HWTEST_F(AbnormalBranchTest, AbnormalBranchTest_mReadOnlyInvalid_Test_001, TestSize.Level0)
{
    LOG_INFO("AbnormalBranchTest_mReadOnlyInvalid_Test_001::Start");
    std::string name;
    size_t size = 0;
    bool readOnly = true;
    AppDataFwk::SharedBlock temp(name, nullptr, size, readOnly);
    int result = temp.Clear();
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
    result = temp.SetRawData(nullptr, size);
    EXPECT_EQ(result, AppDataFwk::SharedBlock::SHARED_BLOCK_INVALID_OPERATION);
    LOG_INFO("AbnormalBranchTest_mReadOnlyInvalid_Test_001::End");
}

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
} // namespace DataShare
} // namespace OHOS