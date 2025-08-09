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

namespace OHOS {
namespace DataShare {
using namespace testing::ext;
using namespace OHOS::Security::AccessToken;
std::string DATA_SHARE_PROXY_URI = "datashareproxy://com.acts.ohos.data.datasharetest/test";
std::shared_ptr<DataShare::DataShareHelper> dataShareHelper;
std::shared_ptr<DataShare::DataShareHelper> dataShareHelper2;  // for another subscriber
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

void ProxyDatasTest::SetUpTestCase(void)
{
    LOG_INFO("SetUpTestCase invoked");
    int sleepTime = 1;
    sleep(sleepTime);

    HapInfoParams info = { .userID = 100,
        .bundleName = "ohos.datashareproxyclienttest.demo",
        .instIndex = 0,
        .appIDDesc = "ohos.datashareproxyclienttest.demo",
        .isSystemApp = true };
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
    auto testTokenId = Security::AccessToken::AccessTokenKit::GetHapTokenIDEx(
        info.userID, info.bundleName, info.instIndex);
    SetSelfTokenID(testTokenId.tokenIDEx);

    CreateOptions options;
    options.enabled_ = true;
    dataShareHelper = DataShare::DataShareHelper::Creator(DATA_SHARE_PROXY_URI, options);
    ASSERT_TRUE(dataShareHelper != nullptr);
    dataShareHelper2 = DataShare::DataShareHelper::Creator(DATA_SHARE_PROXY_URI, options);
    ASSERT_TRUE(dataShareHelper2 != nullptr);
    LOG_INFO("SetUpTestCase end");
}

void ProxyDatasTest::TearDownTestCase(void)
{
    auto tokenId = AccessTokenKit::GetHapTokenID(100, "ohos.datashareclienttest.demo", 0);
    AccessTokenKit::DeleteToken(tokenId);
    dataShareHelper = nullptr;
    dataShareHelper2 = nullptr;
}

void ProxyDatasTest::SetUp(void)
{
}
void ProxyDatasTest::TearDown(void)
{
}

HWTEST_F(ProxyDatasTest, ProxyDatasTest_Insert_Test_001, TestSize.Level1)
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

HWTEST_F(ProxyDatasTest, ProxyDatasTest_QUERY_Test_001, TestSize.Level1)
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

HWTEST_F(ProxyDatasTest, ProxyDatasTest_QueryTimout_Test_001, TestSize.Level1)
{
    LOG_INFO("ProxyDatasTest_QueryTimeout_Test_001::Start");
    auto helper = dataShareHelper;
    Uri uri(DATA_SHARE_PROXY_URI);
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(TBL_NAME0, "wang");
    std::vector<string> columns;
    DataShareOption option;
    option.timeout = 0;
    DatashareBusinessError businessError;
    auto resultSet = helper->QueryTimeout(uri, predicates, columns, option, &businessError);
    EXPECT_NE(resultSet, nullptr);
    EXPECT_EQ(businessError.GetCode(), E_OK);
    LOG_INFO("ProxyDatasTest_QueryTimeout_Test_001::End");
}

HWTEST_F(ProxyDatasTest, ProxyDatasTest_QueryTimeout_Test_002, TestSize.Level1)
{
    LOG_INFO("ProxyDatasTest_QueryTimeout_Test_002::Start");
    auto helper = dataShareHelper;
    Uri uri(DATA_SHARE_PROXY_URI);
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(TBL_NAME0, "wang");
    std::vector<string> columns;
    DataShareOption option;
    option.timeout = 4000; // 4000 is the query timeout time.
    auto limitTime = 1000; // 1000 is used to detect whether the query timeout waiting logic is abnormal.
    int repeatTimes = 100; // 100 is the number of times the query is executed.
    DatashareBusinessError businessError;
    for (int i = 0; i < repeatTimes; i++) {
        auto start = std::chrono::steady_clock::now();
        auto resultSet = helper->QueryTimeout(uri, predicates, columns, option, &businessError);
        auto finish = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
        EXPECT_TRUE(duration < std::chrono::milliseconds(limitTime));
        EXPECT_NE(resultSet, nullptr);
        EXPECT_EQ(businessError.GetCode(), E_OK);
        int result = 0;
        resultSet->GetRowCount(result);
        EXPECT_EQ(result, 1);
    }
    LOG_INFO("ProxyDatasTest_QueryTimeout_Test_002::End");
}

HWTEST_F(ProxyDatasTest, ProxyDatasTest_QueryTimeout_Test_003, TestSize.Level1)
{
    LOG_INFO("ProxyDatasTest_QueryTimeout_Test_003::Start");
    auto helper = dataShareHelper;
    Uri uri(DATA_SHARE_PROXY_URI);
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(TBL_NAME0, "wang");
    std::vector<string> columns;
    DataShareOption option;
    option.timeout = 4000; // 4000 is the query timeout time.
    auto limitTime = 1000; // 1000 is used to detect whether the query timeout waiting logic is abnormal.
    int repeatTimes = 100; // 100 is the number of times the query is executed.
    DatashareBusinessError businessError;
    
    std::function<void()> func = [&option, &helper, &uri, &predicates, &columns, &businessError,
        &limitTime, &repeatTimes]() {
        for (int i = 0; i < repeatTimes; i++) {
            auto start = std::chrono::steady_clock::now();
            auto resultSet = helper->QueryTimeout(uri, predicates, columns, option, &businessError);
            auto finish = std::chrono::steady_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
            EXPECT_TRUE(duration < std::chrono::milliseconds(limitTime));
            EXPECT_NE(resultSet, nullptr);
            EXPECT_EQ(businessError.GetCode(), E_OK);
            int result = 0;
            resultSet->GetRowCount(result);
            EXPECT_EQ(result, 1);
        }
    };
    int threadNum = 10; // 10 is the number of threads.
    std::thread threads[threadNum];
    for (int i = 0; i < threadNum; ++i) {
        threads[i] = std::thread(func);
    }

    for (int i = 0; i < threadNum; ++i) {
        threads[i].join();
    }
    LOG_INFO("ProxyDatasTest_QueryTimeout_Test_003::End");
}

HWTEST_F(ProxyDatasTest, ProxyDatasTest_QueryTimeout_Test_004, TestSize.Level1)
{
    LOG_INFO("ProxyDatasTest_QueryTimeout_Test_004::Start");
    auto helper = dataShareHelper;
    Uri uri(DATA_SHARE_PROXY_URI);
    std::string name = "timeout";
    int retVal = 0;
    int insertTimes = 500; // 500 is the number of times the insert is executed.
    for (int i = 0; i < insertTimes; i++) {
        DataShare::DataShareValuesBucket valuesBucket;
        std::string name0 = "query" + std::to_string(i);
        valuesBucket.Put(TBL_NAME0, name0);
        valuesBucket.Put(TBL_NAME1, name);
        retVal = helper->Insert(uri, valuesBucket);
        EXPECT_EQ((retVal > 0), true);
    }

    DataShare::DataSharePredicates predicates;
    predicates.Like(TBL_NAME0, "query");
    std::vector<string> columns;
    DataShareOption option;
    option.timeout = 1; // 1 is the query timeout time.
    int count = 0;
    int queryTimes = 10; // 10 is the number of times the query is executed.
    for (int i = 0; i < queryTimes; i++) {
        DatashareBusinessError businessError;
        auto resultSet = helper->QueryTimeout(uri, predicates, columns, option, &businessError);
        if (businessError.GetCode() == E_TIMEOUT_ERROR) {
            count++;
        }
    }
    LOG_INFO("ProxyDatasTest_QueryTimeout_Test_004 Query Timeout %{public}d times", count);
    EXPECT_TRUE(count > 0);
    DataShare::DataSharePredicates delPredicates;
    delPredicates.EqualTo(TBL_NAME1, name);
    retVal = helper->Delete(uri, delPredicates);
    EXPECT_EQ((retVal > 0), true);
    LOG_INFO("ProxyDatasTest_QueryTimeout_Test_004::End");
}


/**
* @tc.name: ProxyDatasTest_QueryTimeout_Test_005
* @tc.desc: test QueryTimeout function
* @tc.type: FUNC
* @tc.require: issueICS05H
* @tc.precon: None
* @tc.step:
    1.Creat a DataShareHelper class
    2.call QueryTimeout function
* @tc.experct: QueryTimeout return nullptr when The function of the parent class was called
*/
HWTEST_F(ProxyDatasTest, ProxyDatasTest_QueryTimeout_Test_005, TestSize.Level1)
{
    LOG_INFO("ProxyDatasTest_QueryTimeout_Test_005::Start");
    auto helper = dataShareHelper;
    Uri uri(DATA_SHARE_PROXY_URI);
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(TBL_NAME0, "wang");
    std::vector<string> columns;
    DataShareOption option;
    option.timeout = 0; // The function has no specific implementation, timeoutis meaningless
    DatashareBusinessError businessError;
    auto resultSet = helper->DataShareHelper::QueryTimeout(uri, predicates, columns, option, &businessError);
    EXPECT_EQ(resultSet, nullptr);
    LOG_INFO("ProxyDatasTest_QueryTimeout_Test_005::End");
}

HWTEST_F(ProxyDatasTest, ProxyDatasTest_ResultSet_Test_001, TestSize.Level1)
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

    bool hasBlock = resultSet->HasBlock();
    EXPECT_EQ(hasBlock, true);
    EXPECT_NE(resultSet->GetBlock(), nullptr);

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

HWTEST_F(ProxyDatasTest, ProxyDatasTest_Template_Test_001, TestSize.Level1)
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

HWTEST_F(ProxyDatasTest, ProxyDatasTest_Template_Test_002, TestSize.Level1)
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

/**
* @tc.name: ProxyDatasTest_Template_Test_003
* @tc.desc: test Template update function
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(ProxyDatasTest, ProxyDatasTest_Template_Test_003, TestSize.Level1)
{
    LOG_INFO("ProxyDatasTest_Template_Test_003::Start");
    auto helper = dataShareHelper;
    PredicateTemplateNode node1("p1", "select name0 as name from TBL00");
    std::vector<PredicateTemplateNode> nodes;
    nodes.emplace_back(node1);
    Template tpl(nodes, "select name0 as name from TBL00");
    tpl.update_ = "update TBL00 set name0 = 'updatetest' where name0 = 'name00'";
    auto result = helper->AddQueryTemplate(DATA_SHARE_PROXY_URI, SUBSCRIBER_ID, tpl);
    EXPECT_EQ(result, 0);

    std::vector<std::string> uris;
    uris.emplace_back(DATA_SHARE_PROXY_URI);
    TemplateId tplId;
    tplId.subscriberId_ = SUBSCRIBER_ID;
    tplId.bundleName_ = "ohos.datashareproxyclienttest.demo";
    std::string data1;
    std::vector<OperationResult> results1 =
        helper->SubscribeRdbData(uris, tplId, [&data1](const RdbChangeNode &changeNode) {
            data1 = changeNode.data_[0];
        });
    for (auto const &operationResult : results1) {
        EXPECT_EQ(operationResult.errCode_, 0);
    }

    Uri uri(DATA_SHARE_PROXY_URI);
    DataShare::DataShareValuesBucket valuesBucket;
    std::string name0 = "name00";
    valuesBucket.Put(TBL_NAME0, name0);
    int retVal = helper->Insert(uri, valuesBucket);
    EXPECT_GT(retVal, 0);

    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(TBL_NAME0, "updatetest");
    std::vector<string> columns;
    auto resultSet = helper->Query(uri, predicates, columns);
    EXPECT_NE(resultSet, nullptr);
    int queryResult = 0;
    resultSet->GetRowCount(queryResult);
    EXPECT_EQ(queryResult, 1);

    std::vector<OperationResult> results2 = helper->UnsubscribeRdbData(uris, tplId);
    EXPECT_EQ(results2.size(), uris.size());
    for (auto const &operationResult : results2) {
        EXPECT_EQ(operationResult.errCode_, 0);
    }
    LOG_INFO("ProxyDatasTest_Template_Test_003::End");
}

/**
* @tc.name: ProxyDatasTest_Template_Test_004
* @tc.desc: test add template with parameter update function
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(ProxyDatasTest, ProxyDatasTest_Template_Test_004, TestSize.Level1)
{
    LOG_INFO("ProxyDatasTest_Template_Test_004::Start");
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

    Template tpl2("update TBL00 set name0 = 'update'", nodes, "select name1 as name from TBL00");
    result = helper->AddQueryTemplate(DATA_SHARE_PROXY_URI, SUBSCRIBER_ID, tpl2);
    EXPECT_EQ(result, 0);
    result = helper->DelQueryTemplate(DATA_SHARE_PROXY_URI, SUBSCRIBER_ID);
    EXPECT_EQ(result, 0);
    LOG_INFO("ProxyDatasTest_Template_Test_004::End");
}

/**
* @tc.name: ProxyDatasTest_Template_Test_005
* @tc.desc: test add template with wrong parameter update function
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(ProxyDatasTest, ProxyDatasTest_Template_Test_005, TestSize.Level1)
{
    LOG_INFO("ProxyDatasTest_Template_Test_005::Start");
    auto helper = dataShareHelper;
    PredicateTemplateNode node1("p1", "select name0 as name from TBL00");
    PredicateTemplateNode node2("p2", "select name1 as name from TBL00");
    std::vector<PredicateTemplateNode> nodes;
    nodes.emplace_back(node1);
    nodes.emplace_back(node2);
    Template tpl2("insert into TBL00 (name0) values ('test')", nodes, "select name1 as name from TBL00");

    auto result = helper->AddQueryTemplate(DATA_SHARE_PROXY_URI, SUBSCRIBER_ID, tpl2);
    EXPECT_EQ(result, -1);
    result = helper->DelQueryTemplate(DATA_SHARE_PROXY_URI, SUBSCRIBER_ID);
    EXPECT_EQ(result, 0);
    LOG_INFO("ProxyDatasTest_Template_Test_005::End");
}

/**
* @tc.name: ProxyDatasTest_Template_Test_006
* @tc.desc: test use uri with userId to add template
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(ProxyDatasTest, ProxyDatasTest_Template_Test_006, TestSize.Level1)
{
    LOG_INFO("ProxyDatasTest_Template_Test_006::Start");
    auto helper = dataShareHelper;
    PredicateTemplateNode node1("p1", "select name0 as name from TBL00");
    PredicateTemplateNode node2("p2", "select name1 as name from TBL00");
    std::vector<PredicateTemplateNode> nodes;
    nodes.emplace_back(node1);
    nodes.emplace_back(node2);
    Template tpl(nodes, "select name1 as name from TBL00");

    std::string uri = DATA_SHARE_PROXY_URI + "?user=100";
    auto result = helper->AddQueryTemplate(uri, SUBSCRIBER_ID, tpl);
    EXPECT_EQ(result, E_OK);
    result = helper->DelQueryTemplate(uri, SUBSCRIBER_ID);
    EXPECT_EQ(result, E_OK);
    LOG_INFO("ProxyDatasTest_Template_Test_006::End");
}

HWTEST_F(ProxyDatasTest, ProxyDatasTest_Publish_Test_001, TestSize.Level1)
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

HWTEST_F(ProxyDatasTest, ProxyDatasTest_Publish_Test_002, TestSize.Level1)
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

HWTEST_F(ProxyDatasTest, ProxyDatasTest_Publish_Test_003, TestSize.Level1)
{
    LOG_INFO("ProxyDatasTest_Publish_Test_003::Start");
    auto helper = dataShareHelper;
    Data data;
    std::vector<uint8_t> buffer= {10, 20, 30};
    data.datas_.emplace_back("datashareproxy://com.acts.ohos.data.datasharetest/test", SUBSCRIBER_ID, buffer);
    std::string bundleName = "com.acts.ohos.data.datasharetest";
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
        bool isAshmem = publishedDataItem.IsAshmem();
        EXPECT_TRUE(isAshmem);
        auto value = publishedDataItem.GetData();
        EXPECT_EQ(std::get<std::vector<uint8_t>>(value)[0], buffer[0]);
    }
    LOG_INFO("ProxyDatasTest_Publish_Test_003::End");
}

HWTEST_F(ProxyDatasTest, ProxyDatasTest_CombinationRdbData_Test_001, TestSize.Level1)
{
    LOG_INFO("ProxyDatasTest_CombinationRdbData_Test_001::Start");
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
    LOG_INFO("ProxyDatasTest_CombinationRdbData_Test_001::End");
}

/**
* @tc.name: ProxyDatasTest_CombinationRdbData_Test_002
* @tc.desc: combination test for persistent data updated between two constant disable
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(ProxyDatasTest, ProxyDatasTest_CombinationRdbData_Test_002, TestSize.Level1)
{
    auto helper = dataShareHelper;
    auto helper2 = dataShareHelper2;
    std::vector<PredicateTemplateNode> nodes;
    Template tpl(nodes, "select name1 as name from TBL00");
    auto result = helper->AddQueryTemplate(DATA_SHARE_PROXY_URI, SUBSCRIBER_ID, tpl);
    EXPECT_EQ(result, 0);

    std::vector<std::string> uris = {DATA_SHARE_PROXY_URI};
    TemplateId tplId;
    tplId.subscriberId_ = SUBSCRIBER_ID;
    tplId.bundleName_ = "ohos.datashareproxyclienttest.demo";
    std::atomic_int callbackTimes = 0;
    std::mutex mutex;
    std::condition_variable cv;
    auto timeout = std::chrono::seconds(2);
    std::vector<OperationResult> results =
        helper->SubscribeRdbData(uris, tplId, [&callbackTimes, &mutex, &cv](const RdbChangeNode &changeNode) {
            std::lock_guard<std::mutex> lock(mutex);
            callbackTimes++;
            cv.notify_all();
        });
    EXPECT_EQ(results.size(), uris.size());
    for (auto const &result : results) EXPECT_EQ(result.errCode_, 0);
    // if there is only one subscriber in a key, the subscriber can't be disabled twice
    results = helper2->SubscribeRdbData(uris, tplId, [](const RdbChangeNode &changeNode) {
        });

    std::unique_lock<std::mutex> lock(mutex);
    cv.wait_for(lock, timeout);
    EXPECT_EQ(callbackTimes, 1);
    lock.unlock();
    results = helper->DisableRdbSubs(uris, tplId);
    EXPECT_EQ(results.size(), uris.size());
    for (auto const &result : results) EXPECT_EQ(result.errCode_, 0);

    Uri uri(DATA_SHARE_PROXY_URI);
    DataShare::DataShareValuesBucket valuesBucket;
    valuesBucket.Put(TBL_NAME1, 1);
    int retVal = helper->Insert(uri, valuesBucket);
    EXPECT_GT(retVal, 0);

    results = helper->DisableRdbSubs(uris, tplId);
    EXPECT_EQ(results.size(), uris.size());
    for (auto const &result : results) EXPECT_EQ(result.errCode_, 0);
    results = helper->EnableRdbSubs(uris, tplId);
    for (auto const &result : results) EXPECT_EQ(result.errCode_, 0);
    EXPECT_EQ(callbackTimes, 2);
    helper->UnsubscribeRdbData(uris, tplId);
    helper2->UnsubscribeRdbData(uris, tplId);
}

/**
* @tc.name: ProxyDatasTest_CombinationRdbData_Test_003
* @tc.desc: combination test for persistent data updated between two constant disable
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(ProxyDatasTest, ProxyDatasTest_CombinationRdbData_Test_003, TestSize.Level1)
{
    LOG_INFO("ProxyDatasTest_CombinationRdbData_Test_003::Start");
    auto helper = dataShareHelper;
    std::vector<PredicateTemplateNode> nodes;
    Template tpl(nodes, "select name1 as name from TBL00");
    auto result = helper->AddQueryTemplate(DATA_SHARE_PROXY_URI, SUBSCRIBER_ID, tpl);
    EXPECT_EQ(result, E_OK);

    std::vector<std::string> uris = {DATA_SHARE_PROXY_URI};
    TemplateId tplId;
    tplId.subscriberId_ = SUBSCRIBER_ID;
    tplId.bundleName_ = "ohos.datashareproxyclienttest.demo";
    std::atomic_int callbackTimes = 0;
    std::mutex mutex;
    std::condition_variable cv;
    auto timeout = std::chrono::seconds(2);
    std::vector<OperationResult> results =
        helper->SubscribeRdbData(uris, tplId, [&callbackTimes, &mutex, &cv](const RdbChangeNode &changeNode) {
            std::lock_guard<std::mutex> lock(mutex);
            callbackTimes++;
            cv.notify_all();
        });
    EXPECT_EQ(results.size(), uris.size());
    for (auto const &result : results) {
        EXPECT_EQ(result.errCode_, E_OK);
    }

    std::unique_lock<std::mutex> lock(mutex);
    cv.wait_for(lock, timeout);
    EXPECT_EQ(callbackTimes, 1);
    lock.unlock();
    results = helper->DisableRdbSubs(uris, tplId);
    EXPECT_EQ(results.size(), uris.size());
    for (auto const &result : results) {
        EXPECT_EQ(result.errCode_, 0);
    }
    results = helper->EnableRdbSubs(uris, tplId);
    for (auto const &result : results) {
        EXPECT_EQ(result.errCode_, 0);
    }
    EXPECT_EQ(callbackTimes, 1);
    results = helper->UnsubscribeRdbData(uris, tplId);
    EXPECT_EQ(results.size(), uris.size());
    for (auto const &result : results) {
        EXPECT_EQ(result.errCode_, E_OK);
    }
    LOG_INFO("ProxyDatasTest_CombinationRdbData_Test_003::End");
}

/**
* @tc.name: ProxyDatasTest_CombinationPublishedData_Test_001
* @tc.desc: combination test for published data updated between two constant disable
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(ProxyDatasTest, ProxyDatasTest_CombinationPublishedData_Test_001, TestSize.Level1)
{
    LOG_INFO("ProxyDatasTest_CombinationPublishedData_Test_001::Start");
    auto helper = dataShareHelper;
    auto helper2 = dataShareHelper2;
    std::string bundleName = "com.acts.ohos.data.datasharetest";
    Data data;
    data.datas_.emplace_back(DATA_SHARE_PROXY_URI, SUBSCRIBER_ID, "value1");
    std::vector<OperationResult> results = helper->Publish(data, bundleName);
    EXPECT_EQ(results.size(), data.datas_.size());
    for (auto const &result : results) EXPECT_EQ(result.errCode_, 0);
    std::vector<std::string> uris = {DATA_SHARE_PROXY_URI};
    std::atomic_int callbackTimes = 0;
    std::mutex mutex;
    std::condition_variable cv;
    auto timeout = std::chrono::seconds(2);
    results = helper->SubscribePublishedData(uris, SUBSCRIBER_ID,
            [&callbackTimes, &mutex, &cv](const PublishedDataChangeNode &changeNode) {
            std::lock_guard<std::mutex> lock(mutex);
            callbackTimes++;
            cv.notify_all();
        });
    EXPECT_EQ(results.size(), uris.size());
    for (auto const &operationResult : results) EXPECT_EQ(operationResult.errCode_, 0);
    // if there is only one subscriber in a key, the subscriber can't be disabled twice
    results = helper2->SubscribePublishedData(uris, SUBSCRIBER_ID, [](const PublishedDataChangeNode &changeNode) {});
    EXPECT_EQ(results.size(), uris.size());
    for (auto const &operationResult : results) {
        EXPECT_EQ(operationResult.errCode_, 0);
    }
    std::unique_lock<std::mutex> lock(mutex);
    cv.wait_for(lock, timeout);
    EXPECT_EQ(callbackTimes, 1);
    lock.unlock();
    results = helper->DisablePubSubs(uris, SUBSCRIBER_ID);
    EXPECT_EQ(results.size(), uris.size());
    for (auto const &operationResult : results) EXPECT_EQ(operationResult.errCode_, 0);

    results = helper->Publish(data, bundleName);
    EXPECT_EQ(results.size(), data.datas_.size());
    for (auto const &result : results) EXPECT_EQ(result.errCode_, 0);
    results = helper->DisablePubSubs(uris, SUBSCRIBER_ID);
    for (auto const &operationResult : results) EXPECT_EQ(operationResult.errCode_, 0);
    results = helper->EnablePubSubs(uris, SUBSCRIBER_ID);
    for (auto const &operationResult : results) EXPECT_EQ(operationResult.errCode_, 0);
    EXPECT_EQ(callbackTimes, 2);
    helper->UnsubscribePublishedData(uris, SUBSCRIBER_ID);
    helper2->UnsubscribePublishedData(uris, SUBSCRIBER_ID);
    LOG_INFO("ProxyDatasTest_CombinationPublishedData_Test_001::End");
}

HWTEST_F(ProxyDatasTest, ProxyDatasTest_SubscribePublishedData_Test_001, TestSize.Level1)
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

HWTEST_F(ProxyDatasTest, ProxyDatasTest_DisablePubSubs_Test_001, TestSize.Level1)
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

HWTEST_F(ProxyDatasTest, ProxyDatasTest_EnablePubSubs_Test_001, TestSize.Level1)
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

HWTEST_F(ProxyDatasTest, ProxyDatasTest_UnsubscribePublishedData_Test_001, TestSize.Level1)
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

HWTEST_F(ProxyDatasTest, ProxyDatasTest_extSpCtl_Null_Test_001, TestSize.Level1)
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

HWTEST_F(ProxyDatasTest, ProxyDatasTest_extSpCtl_Null_Test_002, TestSize.Level1)
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