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
#include "ishared_result_set_stub.h"
#include "message_parcel.h"
#include "ikvstore_data_service.h"
#include "shared_block.h"
#include "uri.h"
#include "datashare_connection.h"

namespace OHOS {
namespace DataShare {
using namespace testing::ext;
using namespace OHOS::Security::AccessToken;
std::string DATA_SHARE_PROXY_URI = "datashareproxy://com.acts.ohos.data.datasharetest/test";
std::shared_ptr<DataShare::DataShareHelper> dataShareHelper;
std::string TBL_NAME0 = "name0";
std::string TBL_NAME1 = "name1";
constexpr int SUBSCRIBER_ID = 100;
std::atomic_int g_callbackTimes = 0;

class ProxyDatasTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

using namespace OHOS::DataShare;
using Uri = OHOS::Uri;
void ProxyDatasTest::SetUpTestCase(void)
{
    LOG_INFO("SetUpTestCase invoked");
    int sleepTime = 1;
    sleep(sleepTime);

    HapInfoParams info = { .userID = 100,
        .bundleName = "ohos.datashareproxyclienttest.demo",
        .instIndex = 0,
        .appIDDesc = "ohos.datashareproxyclienttest.demo" };
    HapPolicyParams policy = { .apl = APL_SYSTEM_BASIC,
        .domain = "test.domain",
        .permList = { { .permissionName = "ohos.permission.GET_BUNDLE_INFO",
            .bundleName = "ohos.datashareproxyclienttest.demo",
            .grantMode = 1,
            .availableLevel = APL_SYSTEM_BASIC,
            .label = "label",
            .labelId = 1,
            .description = "ohos.datashareproxyclienttest.demo",
            .descriptionId = 1 } },
        .permStateList = { { .permissionName = "ohos.permission.GET_BUNDLE_INFO",
            .isGeneral = true,
            .resDeviceID = { "local" },
            .grantStatus = { PermissionState::PERMISSION_GRANTED },
            .grantFlags = { 1 } } } };
    AccessTokenKit::AllocHapToken(info, policy);
    auto testTokenId =
        Security::AccessToken::AccessTokenKit::GetHapTokenID(info.userID, info.bundleName, info.instIndex);
    SetSelfTokenID(testTokenId);

    CreateOptions options;
    options.enabled_ = true;
    dataShareHelper = DataShare::DataShareHelper::Creator(DATA_SHARE_PROXY_URI, options);
    ASSERT_TRUE(dataShareHelper != nullptr);
    LOG_INFO("SetUpTestCase end");
}

void ProxyDatasTest::TearDownTestCase(void)
{
    auto tokenId = AccessTokenKit::GetHapTokenID(100, "ohos.datashareclienttest.demo", 0);
    AccessTokenKit::DeleteToken(tokenId);
    dataShareHelper = nullptr;
}

void ProxyDatasTest::SetUp(void)
{
}
void ProxyDatasTest::TearDown(void)
{
}

HWTEST_F(ProxyDatasTest, ProxyDatasTest_Insert_Test_001, TestSize.Level0)
{
    LOG_INFO("ProxyDatasTest_Insert_Test_001::Start");
    auto helper = dataShareHelper;
    Uri uri(DATA_SHARE_PROXY_URI);
    DataShare::DataShareValuesBucket valuesBucket;
    std::string name0 = "wang";
    valuesBucket.Put(TBL_NAME0, name0);
    std::string name1 = "wu";
    valuesBucket.Put(TBL_NAME1, name1);

    int retVal = helper->Insert(uri, valuesBucket);
    EXPECT_EQ((retVal > 0), true);
    LOG_INFO("ProxyDatasTest_Insert_Test_001::End");
}

HWTEST_F(ProxyDatasTest, ProxyDatasTest_QUERY_Test_001, TestSize.Level0)
{
    LOG_INFO("ProxyDatasTest_QUERY_Test_001::Start");
    auto helper = dataShareHelper;
    Uri uri(DATA_SHARE_PROXY_URI);
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(TBL_NAME0, "wang");
    std::vector<string> columns;
    auto resultSet = helper->Query(uri, predicates, columns);
    EXPECT_NE(resultSet, nullptr);
    int result = 0;
    resultSet->GetRowCount(result);
    EXPECT_EQ(result, 1);
    LOG_INFO("ProxyDatasTest_QUERY_Test_001::End");
}

HWTEST_F(ProxyDatasTest, ProxyDatasTest_ResultSet_Test_001, TestSize.Level0)
{
    LOG_INFO("ProxyDatasTest_ResultSet_Test_001::Start");
    auto helper = dataShareHelper;
    Uri uri(DATA_SHARE_PROXY_URI);
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(TBL_NAME0, "wang");
    std::vector<string> columns;
    auto resultSet = helper->Query(uri, predicates, columns);
    EXPECT_NE(resultSet, nullptr);

    int result = 0;
    resultSet->GetRowCount(result);
    EXPECT_EQ(result, 1);

    AppDataFwk::SharedBlock *block = nullptr;
    ASSERT_TRUE(resultSet != nullptr);
    bool hasBlock = resultSet->HasBlock();
    EXPECT_EQ(hasBlock, true);
    block = resultSet->GetBlock();
    EXPECT_NE(block, nullptr);
    
    std::vector<uint8_t> blob;
    int err = resultSet->GetBlob(-1, blob);
    EXPECT_EQ(err, E_INVALID_COLUMN_INDEX);
    resultSet->SetBlock(nullptr);
    EXPECT_EQ(nullptr, resultSet->GetBlock());
    std::string stringValue;
    result = resultSet->GetString(0, stringValue);
    EXPECT_EQ(result, E_ERROR);
    int intValue;
    result = resultSet->GetInt(0, intValue);
    EXPECT_EQ(result, E_ERROR);
    LOG_INFO("ProxyDatasTest_ResultSet_Test_001::End");
}

HWTEST_F(ProxyDatasTest, ProxyDatasTest_Template_Test_001, TestSize.Level0)
{
    LOG_INFO("ProxyDatasTest_Template_Test_001::Start");
    auto helper = dataShareHelper;
    PredicateTemplateNode node1("p1", "select name0 as name from TBL00");
    PredicateTemplateNode node2("p2", "select name1 as name from TBL00");
    std::vector<PredicateTemplateNode> nodes;
    nodes.emplace_back(node1);
    nodes.emplace_back(node2);
    Template tpl(nodes, "select name1 as name from TBL00");

    auto result = helper->AddQueryTemplate(DATA_SHARE_PROXY_URI, SUBSCRIBER_ID, tpl);
    EXPECT_EQ(result, 0);
    result = helper->DelQueryTemplate(DATA_SHARE_PROXY_URI, SUBSCRIBER_ID);
    EXPECT_EQ(result, 0);
    LOG_INFO("ProxyDatasTest_Template_Test_001::End");
}

HWTEST_F(ProxyDatasTest, ProxyDatasTest_Template_Test_002, TestSize.Level0)
{
    LOG_INFO("ProxyDatasTest_Template_Test_002::Start");
    auto helper = dataShareHelper;
    PredicateTemplateNode node1("p1", "select name0 as name from TBL00");
    PredicateTemplateNode node2("p2", "select name1 as name from TBL00");
    std::vector<PredicateTemplateNode> nodes;
    nodes.emplace_back(node1);
    nodes.emplace_back(node2);
    Template tpl(nodes, "select name1 as name from TBL00");

    std::string errorUri = "datashareproxy://com.acts.ohos.data.datasharetest";
    auto result = helper->AddQueryTemplate(errorUri, SUBSCRIBER_ID, tpl);
    EXPECT_EQ(result, E_URI_NOT_EXIST);
    result = helper->DelQueryTemplate(errorUri, SUBSCRIBER_ID);
    EXPECT_EQ(result, E_URI_NOT_EXIST);
    LOG_INFO("ProxyDatasTest_Template_Test_002::End");
}

HWTEST_F(ProxyDatasTest, ProxyDatasTest_Publish_Test_001, TestSize.Level0)
{
    LOG_INFO("ProxyDatasTest_Publish_Test_001::Start");
    auto helper = dataShareHelper;
    std::string bundleName = "com.acts.ohos.data.datasharetest";
    Data data;
    data.datas_.emplace_back("datashareproxy://com.acts.ohos.data.datasharetest/test", SUBSCRIBER_ID, "value1");
    std::vector<OperationResult> results = helper->Publish(data, bundleName);
    EXPECT_EQ(results.size(), data.datas_.size());
    for (auto const &result : results) {
        EXPECT_EQ(result.errCode_, 0);
    }

    int errCode = 0;
    auto getData = helper->GetPublishedData(bundleName, errCode);
    EXPECT_EQ(errCode, 0);
    EXPECT_EQ(getData.datas_.size(), data.datas_.size());
    for (auto &publishedDataItem : getData.datas_) {
        EXPECT_EQ(publishedDataItem.subscriberId_, SUBSCRIBER_ID);
        bool isString = publishedDataItem.IsString();
        EXPECT_EQ(isString, true);
        EXPECT_EQ(publishedDataItem.key_, "datashareproxy://com.acts.ohos.data.datasharetest/test");
        auto value = publishedDataItem.GetData();
        EXPECT_EQ(std::get<std::string>(value), "value1");
    }
    LOG_INFO("ProxyDatasTest_Publish_Test_001::End");
}

HWTEST_F(ProxyDatasTest, ProxyDatasTest_Publish_Test_002, TestSize.Level0)
{
    LOG_INFO("ProxyDatasTest_Publish_Test_002::Start");
    auto helper = dataShareHelper;
    std::string bundleName = "com.acts.ohos.error";
    Data data;
    data.datas_.emplace_back("datashareproxy://com.acts.ohos.error", SUBSCRIBER_ID, "value1");
    std::vector<OperationResult> results = helper->Publish(data, bundleName);
    EXPECT_EQ(results.size(), data.datas_.size());
    for (auto const &result : results) {
        EXPECT_EQ(result.errCode_, E_BUNDLE_NAME_NOT_EXIST);
    }

    int errCode = 0;
    auto getData = helper->GetPublishedData(bundleName, errCode);
    EXPECT_EQ(errCode, E_BUNDLE_NAME_NOT_EXIST);
    LOG_INFO("ProxyDatasTest_Publish_Test_002::End");
}

HWTEST_F(ProxyDatasTest, ProxyDatasTest_CombinationRdbData_Test_001, TestSize.Level0)
{
    auto helper = dataShareHelper;
    PredicateTemplateNode node("p1", "select name0 as name from TBL00");
    std::vector<PredicateTemplateNode> nodes;
    nodes.emplace_back(node);
    Template tpl(nodes, "select name1 as name from TBL00");
    auto result = helper->AddQueryTemplate(DATA_SHARE_PROXY_URI, SUBSCRIBER_ID, tpl);
    EXPECT_EQ(result, 0);
    std::vector<std::string> uris;
    uris.emplace_back(DATA_SHARE_PROXY_URI);
    TemplateId tplId;
    tplId.subscriberId_ = SUBSCRIBER_ID;
    tplId.bundleName_ = "ohos.datashareproxyclienttest.demo";
    std::vector<OperationResult> results1 =
        helper->SubscribeRdbData(uris, tplId, [&tplId](const RdbChangeNode &changeNode) {
            EXPECT_EQ(changeNode.uri_, DATA_SHARE_PROXY_URI);
            EXPECT_EQ(changeNode.templateId_.bundleName_, tplId.bundleName_);
            EXPECT_EQ(changeNode.templateId_.subscriberId_, tplId.subscriberId_);
            g_callbackTimes++;
        });
    EXPECT_EQ(results1.size(), uris.size());
    for (auto const &operationResult : results1) {
        EXPECT_EQ(operationResult.errCode_, 0);
    }
    std::vector<OperationResult> results2 = helper->DisableRdbSubs(uris, tplId);
    for (auto const &operationResult : results2) {
        EXPECT_EQ(operationResult.errCode_, 0);
    }
    std::vector<OperationResult> results3 = helper->EnableRdbSubs(uris, tplId);
    for (auto const &operationResult : results3) {
        EXPECT_EQ(operationResult.errCode_, 0);
    }
    Uri uri(DATA_SHARE_PROXY_URI);
    DataShare::DataShareValuesBucket valuesBucket1, valuesBucket2;
    std::string name1 = "wu";
    std::string name2 = "liu";
    valuesBucket1.Put(TBL_NAME1, name1);
    int retVal1 = helper->Insert(uri, valuesBucket1);
    EXPECT_EQ((retVal1 > 0), true);
    EXPECT_EQ(g_callbackTimes, 2);
    std::vector<OperationResult> results4 = helper->UnsubscribeRdbData(uris, tplId);
    EXPECT_EQ(results4.size(), uris.size());
    for (auto const &operationResult : results4) {
        EXPECT_EQ(operationResult.errCode_, 0);
    }
    valuesBucket2.Put(TBL_NAME1, name2);
    int retVal2 = helper->Insert(uri, valuesBucket2);
    EXPECT_EQ((retVal2 > 0), true);
    EXPECT_EQ(g_callbackTimes, 2);
}

HWTEST_F(ProxyDatasTest, ProxyDatasTest_SubscribePublishedData_Test_001, TestSize.Level0)
{
    LOG_INFO("ProxyDatasTest_SubscribePublishedData_Test_001::Start");
    auto helper = dataShareHelper;
    std::vector<std::string> uris;
    uris.emplace_back(DATA_SHARE_PROXY_URI);
    std::vector<OperationResult> results =
        helper->SubscribePublishedData(uris, SUBSCRIBER_ID, [](const PublishedDataChangeNode &changeNode) {
            EXPECT_EQ(changeNode.ownerBundleName_, "ohos.datashareproxyclienttest.demo");
        });
    EXPECT_EQ(results.size(), uris.size());
    for (auto const &operationResult : results) {
        EXPECT_EQ(operationResult.errCode_, 0);
    }
    LOG_INFO("ProxyDatasTest_SubscribePublishedData_Test_001::End");
}

HWTEST_F(ProxyDatasTest, ProxyDatasTest_DisablePubSubs_Test_001, TestSize.Level0)
{
    LOG_INFO("ProxyDatasTest_DisablePubSubs_Test_001::Start");
    auto helper = dataShareHelper;
    std::vector<std::string> uris;
    uris.emplace_back(DATA_SHARE_PROXY_URI);
    std::vector<OperationResult> results = helper->DisablePubSubs(uris, SUBSCRIBER_ID);
    for (auto const &operationResult : results) {
        EXPECT_EQ(operationResult.errCode_, 0);
    }
    LOG_INFO("ProxyDatasTest_DisablePubSubs_Test_001::End");
}

HWTEST_F(ProxyDatasTest, ProxyDatasTest_EnablePubSubs_Test_001, TestSize.Level0)
{
    LOG_INFO("ProxyDatasTest_EnablePubSubs_Test_001::Start");
    auto helper = dataShareHelper;
    std::vector<std::string> uris;
    uris.emplace_back(DATA_SHARE_PROXY_URI);
    std::vector<OperationResult> results = helper->EnablePubSubs(uris, SUBSCRIBER_ID);
    for (auto const &operationResult : results) {
        EXPECT_EQ(operationResult.errCode_, 0);
    }
    LOG_INFO("ProxyDatasTest_EnablePubSubs_Test_001::End");
}

HWTEST_F(ProxyDatasTest, ProxyDatasTest_UnsubscribePublishedData_Test_001, TestSize.Level0)
{
    LOG_INFO("ProxyDatasTest_UnsubscribePublishedData_Test_001::Start");
    auto helper = dataShareHelper;
    std::vector<std::string> uris;
    uris.emplace_back(DATA_SHARE_PROXY_URI);
    std::vector<OperationResult> results = helper->UnsubscribePublishedData(uris, SUBSCRIBER_ID);
    EXPECT_EQ(results.size(), uris.size());
    for (auto const &operationResult : results) {
        EXPECT_EQ(operationResult.errCode_, 0);
    }
    LOG_INFO("ProxyDatasTest_UnsubscribePublishedData_Test_001::End");
}

HWTEST_F(ProxyDatasTest, ProxyDatasTest_AddObserversProxyNull_Test_001, TestSize.Level0)
{
    LOG_INFO("ProxyDatasTest_AddObserversProxyNull_Test_001::Start");
    void *subscriber = nullptr;
    std::shared_ptr<DataShareServiceProxy> proxy = nullptr;
    const std::vector<std::string> uris = {};
    int64_t subscriberId = 0;
    const PublishedDataCallback callback = [](const PublishedDataChangeNode &changeNode){};
    std::vector<OperationResult> results = PublishedDataSubscriberManager::GetInstance().AddObservers(subscriber,
        proxy, uris, subscriberId, callback);
    EXPECT_EQ(results.size(), uris.size());
    LOG_INFO("ProxyDatasTest_AddObserversProxyNull_Test_001::End");
}

HWTEST_F(ProxyDatasTest, ProxyDatasTest_shareBlock_Null_Test_001, TestSize.Level0)
{
    LOG_INFO("ProxyDatasTest_shareBlock_Null_Test_001::Start");
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
    LOG_INFO("ProxyDatasTest_shareBlock_Null_Test_001::End");
}

HWTEST_F(ProxyDatasTest, ProxyDatasTest_ResultSetStubNull_Test_001, TestSize.Level0)
{
    LOG_INFO("ProxyDatasTest_ResultSetStubNull_Test_001::Start");
    ISharedResultSetStub stub(nullptr);
    std::shared_ptr<DataShareResultSet> result = nullptr;
    OHOS::MessageParcel parcel;
    sptr<ISharedResultSet> resultSet = stub.CreateStub(result, parcel);
    EXPECT_EQ(resultSet, nullptr);
    LOG_INFO("ProxyDatasTest_ResultSetStubNull_Test_001::End");
}

HWTEST_F(ProxyDatasTest, ProxyDatasTest_RegisterClientDeathObserverNull_Test_001, TestSize.Level0)
{
    LOG_INFO("ProxyDatasTest_RegisterClientDeathObserverNull_Test_001::Start");
    DataShareKvServiceProxy proxy(nullptr);
    std::string appId;
    uint32_t result = proxy.RegisterClientDeathObserver(appId, nullptr);
    EXPECT_EQ(result, -1);
    LOG_INFO("ProxyDatasTest_RegisterClientDeathObserverNull_Test_001::End");
}

HWTEST_F(ProxyDatasTest, ProxyDatasTest_mReadOnlyInvalid_Test_001, TestSize.Level0)
{
    LOG_INFO("ProxyDatasTest_mReadOnlyInvalid_Test_001::Start");
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
    LOG_INFO("ProxyDatasTest_mReadOnlyInvalid_Test_001::End");
}

HWTEST_F(ProxyDatasTest, ProxyDatasTest_CreatorPossibleNull_Test_001, TestSize.Level0)
{
    LOG_INFO("ProxyDatasTest_CreatorPossibleNull_Test_001::Start");
    std::string strUri;
    CreateOptions options;
    options.token_ = nullptr;
    std::string bundleName;
    std::shared_ptr<DataShareHelper> dataHelper = DataShare::DataShareHelper::Creator(strUri, options, bundleName);
    EXPECT_EQ(dataHelper, nullptr);
    LOG_INFO("ProxyDatasTest_CreatorPossibleNull_Test_001::End");
}

HWTEST_F(ProxyDatasTest, ProxyDatasTest_CreatorPossibleNull_Test_002, TestSize.Level0)
{
    LOG_INFO("ProxyDatasTest_CreatorPossibleNull_Test_002::Start");
    std::string strUri;
    CreateOptions options;
    options.token_ = nullptr;
    options.isProxy_ = false;
    std::string bundleName;
    std::shared_ptr<DataShareHelper> dataHelper = DataShare::DataShareHelper::Creator(strUri, options, bundleName);
    EXPECT_EQ(dataHelper, nullptr);
    LOG_INFO("ProxyDatasTest_CreatorPossibleNull_Test_002::End");
}

HWTEST_F(ProxyDatasTest, ProxyDatasTest_extSpCtl_Null_Test_001, TestSize.Level0)
{
    LOG_INFO("ProxyDatasTest_extSpCtl_Null_Test_001::Start");
    auto helper = dataShareHelper;
    bool ret = helper->Release();
    EXPECT_EQ(ret, true);
    Uri uri("");
    std::string str;
    std::vector<std::string> result = helper->GetFileTypes(uri, str);
    EXPECT_EQ(result.size(), 0);
    int err = helper->OpenFile(uri, str);
    EXPECT_EQ(err, -1);
    err = helper->OpenRawFile(uri, str);
    EXPECT_EQ(err, -1);
    LOG_INFO("ProxyDatasTest_extSpCtl_Null_Test_001::End");
}

HWTEST_F(ProxyDatasTest, ProxyDatasTest_extSpCtl_Null_Test_002, TestSize.Level0)
{
    LOG_INFO("ProxyDatasTest_extSpCtl_Null_Test_002::Start");
    auto helper = dataShareHelper;
    bool ret = helper->Release();
    EXPECT_EQ(ret, true);
    Uri uri("");
    Uri uriResult = helper->NormalizeUri(uri);
    EXPECT_EQ(uriResult, uri);
    uriResult = helper->DenormalizeUri(uri);
    EXPECT_EQ(uriResult, uri);
    LOG_INFO("ProxyDatasTest_extSpCtl_Null_Test_002::End");
}
} // namespace DataShare
} // namespace OHOS