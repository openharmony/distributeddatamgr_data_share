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

/**
 * @tc.name: ProxyDatasTest_Insert_Test_001
 * @tc.desc: Verify the functionality of DataShareHelper successfully inserting data into the data share proxy
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: None
 * @tc.step:
    1. Obtain the DataShareHelper instance
    2. Create a Uri object using the data share proxy URI
    3. Prepare a DataShareValuesBucket and add string data to it
    4. Call the Insert method of DataShareHelper with the Uri and ValuesBucket
 * @tc.expect:
    1. The return value of the Insert method is greater than 0, indicating successful data insertion
 */
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

/**
* @tc.name: ProxyDatasTest_QUERY_Test_001
* @tc.desc: Verify basic query functionality with specific equality condition
* @tc.type: FUNC
* @tc.require: issueIC8OCN
* @tc.precon: Test data containing record with TBL_NAME0 value "wang" exists
* @tc.step:
    1. Obtain the DataShareHelper instance
    2. Create URI for proxy data access using DATA_SHARE_PROXY_URI
    3. Create DataSharePredicates with condition: TBL_NAME0 equals "wang"
    4. Execute query with empty columns list
    5. Check the returned result set and its row count
* @tc.expect:
    1. Query returns a non-null result set
    2. Result set contains exactly 1 record (row count = 1)
*/
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

/**
* @tc.name: ProxyDatasTest_QueryTimout_Test_001
* @tc.desc: Verify query timeout functionality with zero timeout configuration
* @tc.type: FUNC
* @tc.require: issueIC8OCN
* @tc.precon: Test data containing record with TBL_NAME0 value "wang" exists
* @tc.step:
    1. Obtain the DataShareHelper instance
    2. Create URI for proxy data access using DATA_SHARE_PROXY_URI
    3. Create DataSharePredicates with condition: TBL_NAME0 equals "wang"
    4. Configure DataShareOption with timeout set to 0
    5. Execute QueryTimeout with specified parameters and business error pointer
    6. Verify result set and error code
* @tc.expect:
    1. Query returns a non-null result set
    2. Business error code is E_OK (success status)
*/
HWTEST_F(ProxyDatasTest, ProxyDatasTest_QueryTimeout_Test_001, TestSize.Level1)
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
    auto resultSet = helper->Query(uri, predicates, columns, option, &businessError);
    EXPECT_NE(resultSet, nullptr);
    EXPECT_EQ(businessError.GetCode(), E_OK);
    LOG_INFO("ProxyDatasTest_QueryTimeout_Test_001::End");
}

/**
* @tc.name: ProxyDatasTest_QueryTimeout_Test_002
* @tc.desc: Verify query timeout stability and performance with multiple executions
* @tc.type: FUNC
* @tc.require: issueIC8OCN
* @tc.precon: Test data containing record with TBL_NAME0 value "wang" exists
* @tc.step:
    1. Obtain the DataShareHelper instance
    2. Create URI for proxy data access using DATA_SHARE_PROXY_URI
    3. Create DataSharePredicates with condition: TBL_NAME0 equals "wang"
    4. Configure DataShareOption with 4000ms timeout
    5. Execute QueryTimeout 100 times in sequence
    6. Measure execution time for each query
    7. Verify result set, error code and row count for each execution
* @tc.expect:
    1. Each query returns a non-null result set
    2. Each query completes within 1000ms
    3. Each query returns business error code E_OK
    4. Each result set contains exactly 1 record
*/
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
        auto resultSet = helper->Query(uri, predicates, columns, option, &businessError);
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

/**
* @tc.name: ProxyDatasTest_QueryTimeout_Test_003
* @tc.desc: Verify query timeout stability under multi-threaded concurrent access
* @tc.type: FUNC
* @tc.require: issueIC8OCN
* @tc.precon: Test data containing record with TBL_NAME0 value "wang" exists
* @tc.step:
    1. Obtain the DataShareHelper instance
    2. Create URI for proxy data access using DATA_SHARE_PROXY_URI
    3. Create DataSharePredicates with condition: TBL_NAME0 equals "wang"
    4. Configure DataShareOption with 4000ms timeout
    5. Define query function that executes QueryTimeout 100 times
    6. Create 10 threads to run the query function concurrently
    7. Wait for all threads to complete execution
    8. Verify each query's execution time, result set and error code
* @tc.expect:
    1. All threads complete without execution errors
    2. Each query returns a non-null result set
    3. Each query completes within 1000ms
    4. Each query returns business error code E_OK
    5. Each result set contains exactly 1 record
*/
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
            auto resultSet = helper->Query(uri, predicates, columns, option, &businessError);
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

/**
* @tc.name: ProxyDatasTest_QueryTimeout_Test_004
* @tc.require: issueIC8OCN
* @tc.desc: Verify query timeout behavior when threshold is intentionally exceeded
* @tc.type: FUNC
* @tc.precon: Data store has sufficient capacity for test data insertion
* @tc.step:
    1. Obtain the DataShareHelper instance
    2. Create URI for proxy data access using DATA_SHARE_PROXY_URI
    3. Insert 500 test records with TBL_NAME0 values "query0" to "query499"
    4. Create DataSharePredicates with LIKE condition for TBL_NAME0 containing "query"
    5. Configure DataShareOption with 1ms timeout (intentionally short)
    6. Execute QueryTimeout 10 times and count timeout errors
    7. Delete all inserted test records using cleanup predicate
    8. Verify insertion success, timeout occurrences and cleanup success
* @tc.expect:
    1. All 500 test records are inserted successfully
    2. At least one query execution results in E_TIMEOUT_ERROR
    3. Cleanup delete operation removes all inserted records successfully
*/
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
        auto resultSet = helper->Query(uri, predicates, columns, option, &businessError);
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
    auto resultSet = helper->DataShareHelper::Query(uri, predicates, columns, option, &businessError);
    EXPECT_EQ(resultSet, nullptr);
    LOG_INFO("ProxyDatasTest_QueryTimeout_Test_005::End");
}

/**
* @tc.name: ProxyDatasTest_ResultSet_Test_001
* @tc.desc: Verify result set functionality and error handling for invalid operations
* @tc.type: FUNC
* @tc.require: issueIC8OCN
* @tc.precon: Test data containing record with TBL_NAME0 value "wang" exists
* @tc.step:
    1. Obtain the DataShareHelper instance
    2. Create URI for proxy data access using DATA_SHARE_PROXY_URI
    3. Execute query to get result set for TBL_NAME0 equals "wang"
    4. Verify basic result set properties and row count
    5. Check block management functionality (HasBlock, GetBlock)
    6. Test error handling with invalid column index for GetBlob
    7. Nullify block and verify subsequent value retrieval errors
* @tc.expect:
    1. Query returns non-null result set with row count = 1
    2. Result set reports HasBlock = true and non-null block
    3. GetBlob with invalid column index returns E_INVALID_COLUMN_INDEX
    4. After SetBlock(nullptr), GetBlock returns null
    5. GetString and GetInt operations after nullifying block return E_ERROR
*/
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

/**
* @tc.name: ProxyDatasTest_ResultSet_Test_002
* @tc.desc: Verify result set functionality and error handling for invalid operations
* @tc.type: FUNC
* @tc.precon: Test query fail while result set is full
* @tc.step:
    1. Obtain the DataShareHelper instance
    2. Create URI for proxy data access using DATA_SHARE_PROXY_URI
    3. Execute query to get result set for TBL_NAME0 equals "wang" for 32 times and success
    4. Query for 33rd times
    5. Close all the result set
* @tc.expect:
    1. Query returns non-null result set in the early 32 times
    2. Query returns null result set in the 33rd time and errCode is E_RESULTSET_BUSY
*/
HWTEST_F(ProxyDatasTest, ProxyDatasTest_ResultSet_Test_002, TestSize.Level1)
{
    LOG_INFO("ProxyDatasTest_ResultSet_Test_002::Start");
    auto helper = dataShareHelper;
    Uri uri(DATA_SHARE_PROXY_URI);
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(TBL_NAME0, "wang");
    std::vector<string> columns;
    std::vector<std::shared_ptr<DataShareResultSet>> results;
    for (int i = 0; i < 32; ++i) {
        auto resultSet = helper->Query(uri, predicates, columns);
        EXPECT_NE(resultSet, nullptr);
        results.push_back(resultSet);
    }
    DatashareBusinessError errorCode;
    auto resultSet = helper->Query(uri, predicates, columns, &errorCode);
    EXPECT_EQ(resultSet, nullptr);
    EXPECT_NE(&errorCode, nullptr);
    auto code = errorCode.GetCode();
    EXPECT_EQ(code, E_RESULTSET_BUSY);

    for (const auto &result : results) {
        auto ret = result->Close();
        EXPECT_EQ(ret, E_OK);
    }
    LOG_INFO("ProxyDatasTest_ResultSet_Test_002::End");
}

/**
 * @tc.name: ProxyDatasTest_Template_Test_001
 * @tc.desc: Verify the functionality of adding and deleting query templates in the data share proxy
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: None
 * @tc.step:
    1. Obtain the DataShareHelper instance
    2. Create PredicateTemplateNode objects with SQL queries
    3. Assemble the nodes into a Template object
    4. Call AddQueryTemplate to register the template with the proxy URI and subscriber ID
    5. Call DelQueryTemplate to remove the registered template
 * @tc.expect:
    1. AddQueryTemplate returns 0, indicating successful template registration
    2. DelQueryTemplate returns 0, indicating successful template deletion
 */
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

/**
 * @tc.name: ProxyDatasTest_Template_Test_002
 * @tc.desc: Verify that adding and deleting query templates with an invalid URI fails as expected
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: None
 * @tc.step:
    1. Obtain the DataShareHelper instance
    2. Create PredicateTemplateNode objects and assemble them into a Template
    3. Call AddQueryTemplate with an invalid URI and subscriber ID
    4. Call DelQueryTemplate with the same invalid URI and subscriber ID
 * @tc.expect:
    1. AddQueryTemplate returns E_URI_NOT_EXIST, indicating failure due to invalid URI
    2. DelQueryTemplate returns E_URI_NOT_EXIST, indicating failure due to invalid URI
 */
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
 * @tc.desc: Verify the update functionality of query templates in the data share proxy
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: None
 * @tc.step:
    1. Create a Template with an update SQL statement and register it via AddQueryTemplate
    2. Subscribe to RDB data changes using the template ID
    3. Insert new data that triggers the template's update condition
    4. Query the data to verify the update was applied
    5. Unsubscribe from RDB data changes
 * @tc.expect:
    1. Template registration returns 0 (success)
    2. Subscription and unsubscription operations return 0 (success)
    3. Insert operation succeeds (return value > 0)
    4. Query returns a result set with 1 row, confirming the update was applied
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
 * @tc.desc: Verify adding and deleting query templates with parameterized update functions
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: None
 * @tc.step:
    1. Create a Template with predicate nodes and register it via AddQueryTemplate
    2. Delete the registered template via DelQueryTemplate
    3. Create another Template with an explicit update SQL statement and register it
    4. Delete the second template
 * @tc.expect:
    1. All AddQueryTemplate calls return 0 (success)
    2. All DelQueryTemplate calls return 0 (success)
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
 * @tc.desc: Verify that adding a template with an invalid parameterized update function (non-update SQL) fails
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: None
 * @tc.step:
    1. Create a Template with an insert SQL statement as the update function
    2. Attempt to register the template via AddQueryTemplate
    3. Attempt to delete the (unregistered) template via DelQueryTemplate
 * @tc.expect:
    1. AddQueryTemplate returns -1 (failure) due to invalid update SQL
    2. DelQueryTemplate returns 0 (success) (no error if template does not exist)
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
 * @tc.desc: Verify adding and deleting query templates using a URI with a user ID parameter
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: None
 * @tc.step:
    1. Create a URI by appending "?user=100" to the base data share proxy URI
    2. Create a Template with predicate nodes and register it using the new URI
    3. Delete the registered template using the same URI
 * @tc.expect:
    1. AddQueryTemplate returns E_OK (success)
    2. DelQueryTemplate returns E_OK (success)
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

/**
 * @tc.name: ProxyDatasTest_Publish_Test_001
 * @tc.desc: Verify the functionality of publishing string data via the data share proxy and retrieving it
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: None
 * @tc.step:
    1. Create a Data object containing a URI, subscriber ID, and string value
    2. Publish the data using Publish method with the target bundle name
    3. Retrieve the published data using GetPublishedData
    4. Verify the retrieved data matches the published data
 * @tc.expect:
    1. Publish returns OperationResult with errCode_ 0 (success)
    2. GetPublishedData returns errCode_ 0 (success)
    3. Retrieved data has the same size, subscriber ID, key, and value as published data
 */
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

/**
 * @tc.name: ProxyDatasTest_Publish_Test_002
 * @tc.desc: Verify that publishing data with a non-existent bundle name fails as expected
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: None
 * @tc.step:
    1. Create a Data object with a URI, subscriber ID, and string value
    2. Attempt to publish the data using a non-existent bundle name
    3. Attempt to retrieve published data using the same invalid bundle name
 * @tc.expect:
    1. Publish returns OperationResult with errCode_ E_BUNDLE_NAME_NOT_EXIST (failure)
    2. GetPublishedData returns errCode_ E_BUNDLE_NAME_NOT_EXIST (failure)
 */
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

/**
 * @tc.name: ProxyDatasTest_Publish_Test_003
 * @tc.desc: Verify the functionality of publishing binary data (ashmem) via the data share proxy and retrieving it
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: None
 * @tc.step:
    1. Create a Data object containing a URI, subscriber ID, and binary buffer
    2. Publish the data using Publish method with the target bundle name
    3. Retrieve the published data using GetPublishedData
    4. Verify the retrieved binary data matches the published data
 * @tc.expect:
    1. Publish returns OperationResult with errCode_ 0 (success)
    2. GetPublishedData returns errCode_ 0 (success)
    3. Retrieved data has the same size, subscriber ID, and binary content as published data
 */
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

/**
 * @tc.name: ProxyDatasTest_CombinationRdbData_Test_001
 * @tc.desc: Verify combination functionality of RDB data subscription, enabling/disabling, and unsubscription
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: None
 * @tc.step:
    1. Register a query template and subscribe to RDB data changes with a callback
    2. Enable RDB subscriptions
    3. Insert data to trigger the subscription callback
    4. Verify the callback is invoked the expected number of times
    5. Unsubscribe from RDB data changes and insert more data
 * @tc.expect:
    1. Template registration, subscription, and enabling return 0 (success)
    2. Insert operations return values > 0 (success)
    3. Callback is invoked 2 times before unsubscription
    4. No additional callback invocations after unsubscription
 */
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
 * @tc.desc: Verify combination functionality of RDB data subscription, multiple disable operations, and re-enable
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: None
 * @tc.step:
    1. Register a template and subscribe to RDB data changes with two helpers
    2. Disable subscriptions, insert data, and disable again
    3. Re-enable subscriptions and verify callback behavior
    4. Unsubscribe from both helpers
 * @tc.expect:
    1. Template registration, subscriptions, disable/enable return 0 (success)
    2. Insert operation returns value > 0 (success)
    3. Callback is invoked 2 times total (1 before disable, 1 after re-enable)
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
 * @tc.desc: Verify combination functionality of RDB data subscription, disable, re-enable, and unsubscription
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: None
 * @tc.step:
    1. Register a template and subscribe to RDB data changes with a callback
    2. Disable subscriptions, then re-enable them
    3. Unsubscribe from RDB data changes
 * @tc.expect:
    1. Template registration, subscription, disable/enable, and unsubscription return 0 (success)
    2. Callback is invoked 1 time (initial subscription)
    3. No additional callback invocations after disable/re-enable
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
 * @tc.desc: Verify combination functionality of published data subscription, disable, re-enable, and unsubscription
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: None
 * @tc.step:
    1. Publish data and subscribe to published data changes with two helpers
    2. Disable subscriptions, republish data, then disable again
    3. Re-enable subscriptions and verify callback behavior
    4. Unsubscribe from both helpers
 * @tc.expect:
    1. Publish, subscription, disable/enable return 0 (success)
    2. Callback is invoked 2 times total (1 before disable, 1 after re-enable)
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

/**
 * @tc.name: ProxyDatasTest_SubscribePublishedData_Test_001
 * @tc.desc: Verify the functionality of subscribing to published data changes
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: None
 * @tc.step:
    1. Create a list of URIs to subscribe to
    2. Call SubscribePublishedData with the URIs, subscriber ID, and a verification callback
 * @tc.expect:
    1. Subscription returns OperationResult with errCode_ 0 (success)
    2. Callback verifies the owner bundle name when triggered
 */
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

/**
 * @tc.name: ProxyDatasTest_DisablePubSubs_Test_001
 * @tc.desc: Verify the functionality of disabling published data subscriptions
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: None
 * @tc.step:
    1. Create a list of URIs with active subscriptions
    2. Call DisablePubSubs with the URIs and subscriber ID
 * @tc.expect:
    1. DisablePubSubs returns OperationResult with errCode_ 0 (success)
 */
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

/**
 * @tc.name: ProxyDatasTest_EnablePubSubs_Test_001
 * @tc.desc: Verify the functionality of enabling published data subscriptions
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: None
 * @tc.step:
    1. Create a list of URIs with disabled subscriptions
    2. Call EnablePubSubs with the URIs and subscriber ID
 * @tc.expect:
    1. EnablePubSubs returns OperationResult with errCode_ 0 (success)
 */
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

/**
 * @tc.name: ProxyDatasTest_UnsubscribePublishedData_Test_001
 * @tc.desc: Verify the functionality of unsubscribing from published data changes
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: None
 * @tc.step:
    1. Create a list of URIs with active subscriptions
    2. Call UnsubscribePublishedData with the URIs and subscriber ID
 * @tc.expect:
    1. UnsubscribePublishedData returns OperationResult with errCode_ 0 (success)
 */
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

/**
 * @tc.name: ProxyDatasTest_extSpCtl_Null_Test_001
 * @tc.desc: Verify extended special control operations after releasing the helper
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: None
 * @tc.step:
    1. Release the DataShareHelper instance
    2. Call GetFileTypes with an empty URI and string
    3. Call OpenFile and OpenRawFile with an empty URI and string
 * @tc.expect:
    1. Release returns true (success)
    2. GetFileTypes returns an empty list
    3. OpenFile and OpenRawFile return -1 (failure)
 */
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

/**
 * @tc.name: ProxyDatasTest_extSpCtl_Null_Test_002
 * @tc.desc: Verify NormalizeUri and DenormalizeUri operations after releasing the helper
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: None
 * @tc.step:
    1. Release the DataShareHelper instance
    2. Call NormalizeUri and DenormalizeUri with an empty URI
 * @tc.expect:
    1. Release returns true (success)
    2. NormalizeUri and DenormalizeUri return the input empty URI unchanged
 */
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