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

namespace OHOS {
namespace DataShare {
using namespace testing::ext;
using namespace OHOS::Security::AccessToken;
std::string DATA_SHARE_PROXY_URI = "datashareproxy://com.acts.ohos.data.datasharetest/test";
std::shared_ptr<DataShare::DataShareHelper> dataShareHelper;
std::string TBL_NAME0 = "name0";
std::string TBL_NAME1 = "name1";
constexpr int SUBSCRIBER_ID = 100;

class ProxyDatasTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

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
    LOG_INFO("ErrorCodeTest_QUERY_Test_001::Start");
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
    LOG_INFO("ErrorCodeTest_QUERY_Test_001::End");
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

HWTEST_F(ProxyDatasTest, ProxyDatasTest_Publish_Test_001, TestSize.Level0)
{
    LOG_INFO("ProxyDatasTest_Publish_Test_001::Start");
    auto helper = dataShareHelper;
    std::string bundleName = "ohos.datashareproxyclienttest.demo";
    // PublishedDataItem item1("key1", SUBSCRIBER_ID, "value1");
    std::vector<PublishedDataItem> items;
    items.emplace_back(PublishedDataItem("key1", SUBSCRIBER_ID, "value1"));
    Data data;
    data.datas_ = items;
    std::vector<OperationResult> results = helper->Publish(data, bundleName);
    EXPECT_EQ(results.size(), items.size());
    for (auto const &result : results) {
        EXPECT_EQ(result.errCode_, 0);
    }

    auto getData = helper->GetPublishedData(bundleName);
    EXPECT_EQ(getData.datas_.size(), items.size());
    for (auto &publishedDataItem : getData.datas_) {
        EXPECT_EQ(publishedDataItem.subscriberId_, SUBSCRIBER_ID);
        bool isString = publishedDataItem.IsString();
        EXPECT_EQ(isString, true);
        EXPECT_EQ(publishedDataItem.key_, "key1");
        auto value = publishedDataItem.GetData();
        EXPECT_EQ(std::get<std::string>(value), "value1");
    }
    LOG_INFO("ProxyDatasTest_Publish_Test_001::End");
}

HWTEST_F(ProxyDatasTest, ProxyDatasTest_SubscribeRdbData_Test_001, TestSize.Level0)
{
    LOG_INFO("ProxyDatasTest_SubscribeRdbData_Test_001::Start");
    auto helper = dataShareHelper;
    PredicateTemplateNode node1("p1", "select name0 as name from TBL00");
    PredicateTemplateNode node2("p2", "select name1 as name from TBL00");
    std::vector<PredicateTemplateNode> nodes;
    nodes.emplace_back(node1);
    nodes.emplace_back(node2);
    Template tpl(nodes, "select name1 as name from TBL00");
    auto result = helper->AddQueryTemplate(DATA_SHARE_PROXY_URI, SUBSCRIBER_ID, tpl);
    EXPECT_EQ(result, 0);

    std::vector<std::string> uris;
    uris.emplace_back(DATA_SHARE_PROXY_URI);
    TemplateId tplId;
    tplId.subscriberId_ = SUBSCRIBER_ID;
    tplId.bundleName_ = "ohos.datashareproxyclienttest.demo";
    std::vector<OperationResult> results =
        helper->SubscribeRdbData(uris, tplId, [&tplId](const RdbChangeNode &changeNode) {
            EXPECT_EQ(changeNode.uri_, DATA_SHARE_PROXY_URI);
            EXPECT_EQ(changeNode.templateId_.bundleName_, tplId.bundleName_);
            EXPECT_EQ(changeNode.templateId_.subscriberId_, tplId.subscriberId_);
        });
    EXPECT_EQ(results.size(), uris.size());
    for (auto const &operationResult : results) {
        EXPECT_EQ(operationResult.errCode_, 0);
    }
    LOG_INFO("ProxyDatasTest_SubscribeRdbData_Test_001::End");
}

HWTEST_F(ProxyDatasTest, ProxyDatasTest_DisableRdbSubs_Test_001, TestSize.Level0)
{
    LOG_INFO("ProxyDatasTest_DisableRdbSubs_Test_001::Start");
    auto helper = dataShareHelper;
    std::vector<std::string> uris;
    uris.emplace_back(DATA_SHARE_PROXY_URI);
    TemplateId tplId;
    tplId.subscriberId_ = SUBSCRIBER_ID;
    tplId.bundleName_ = "ohos.datashareproxyclienttest.demo";
    std::vector<OperationResult> results = helper->DisableRdbSubs(uris, tplId);
    EXPECT_EQ(results.size(), uris.size());
    for (auto const &operationResult : results) {
        EXPECT_EQ(operationResult.errCode_, 0);
    }
    LOG_INFO("ProxyDatasTest_DisableRdbSubs_Test_001::End");
}

HWTEST_F(ProxyDatasTest, ProxyDatasTest_EnableRdbSubs_Test_001, TestSize.Level0)
{
    LOG_INFO("ProxyDatasTest_EnableRdbSubs_Test_001::Start");
    auto helper = dataShareHelper;
    std::vector<std::string> uris;
    uris.emplace_back(DATA_SHARE_PROXY_URI);
    TemplateId tplId;
    tplId.subscriberId_ = SUBSCRIBER_ID;
    tplId.bundleName_ = "ohos.datashareproxyclienttest.demo";
    std::vector<OperationResult> results = helper->EnableRdbSubs(uris, tplId);
    EXPECT_EQ(results.size(), uris.size());
    for (auto const &operationResult : results) {
        EXPECT_EQ(operationResult.errCode_, 0);
    }
    LOG_INFO("ProxyDatasTest_EnableRdbSubs_Test_001::End");
}

HWTEST_F(ProxyDatasTest, ProxyDatasTest_UnsubscribeRdbData_Test_001, TestSize.Level0)
{
    LOG_INFO("ProxyDatasTest_UnSubscribeRdbData_Test_001::Start");
    auto helper = dataShareHelper;
    std::vector<std::string> uris;
    uris.emplace_back(DATA_SHARE_PROXY_URI);
    TemplateId tplId;
    tplId.subscriberId_ = SUBSCRIBER_ID;
    tplId.bundleName_ = "ohos.datashareproxyclienttest.demo";
    std::vector<OperationResult> results = helper->UnsubscribeRdbData(uris, tplId);
    EXPECT_EQ(results.size(), uris.size());
    for (auto const &operationResult : results) {
        EXPECT_EQ(operationResult.errCode_, 0);
    }
    LOG_INFO("ProxyDatasTest_UnSubscribeRdbData_Test_001::End");
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
    EXPECT_EQ(results.size(), uris.size());
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
    EXPECT_EQ(results.size(), uris.size());
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
} // namespace DataShare
} // namespace OHOS