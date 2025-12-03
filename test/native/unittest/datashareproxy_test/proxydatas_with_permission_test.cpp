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

#define LOG_TAG "proxydatas_with_permission_test"

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
 * @tc.desc: Verify that the DataShareHelper can successfully insert string-type data into the data share proxy
 *           by calling the Insert method, focusing on return value validation.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The pre-initialized DataShareHelper instance (dataShareHelper) is available and non-null.
    2. The DATA_SHARE_PROXY_URI constant is a valid URI for accessing the data share proxy.
    3. TBL_NAME0 and TBL_NAME1 are valid column names in the data source pointed to by DATA_SHARE_PROXY_URI.
 * @tc.step:
    1. Obtain the pre-initialized DataShareHelper instance by assigning dataShareHelper to a local variable (helper).
    2. Create a Uri object using the DATA_SHARE_PROXY_URI constant.
    3. Create a DataShareValuesBucket object, then call Put to add two key-value pairs: TBL_NAME0 = "wang" and
       TBL_NAME1 = "wu".
    4. Call the Insert method of the helper with the created Uri and DataShareValuesBucket, and record the return
       value.
    5. Check whether the return value of the Insert method is greater than 0.
 * @tc.expect:
    1. The return value of the DataShareHelper::Insert method is greater than 0, indicating successful data insertion.
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
 * @tc.desc: Verify the basic query functionality of the DataShareHelper for the data share proxy, using an equality
 *           condition (TBL_NAME0 = "wang") to check the validity of the returned result set.
 * @tc.type: FUNC
 * @tc.require: issueIC8OCN
 * @tc.precon:
    1. The pre-initialized DataShareHelper instance (dataShareHelper) is available and non-null.
    2. The DATA_SHARE_PROXY_URI constant is a valid URI for the data share proxy.
    3. The test data source contains exactly one record where the value of TBL_NAME0 is "wang".
    4. The DataShareResultSet supports the GetRowCount method to obtain the number of rows.
 * @tc.step:
    1. Obtain the DataShareHelper instance by assigning dataShareHelper to a local variable (helper).
    2. Create a Uri object using DATA_SHARE_PROXY_URI.
    3. Create a DataSharePredicates object and call the EqualTo method to set the condition: TBL_NAME0 equals "wang".
    4. Initialize an empty vector<string> for query columns, then call helper->Query with the Uri, predicates, and
       columns.
    5. Check whether the returned DataShareResultSet is non-null.
    6. Call GetRowCount on the result set to get the number of rows and verify the count.
 * @tc.expect:
    1. The Query method returns a non-null DataShareResultSet.
    2. The number of rows obtained via GetRowCount is exactly 1.
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
 * @tc.name: ProxyDatasTest_QueryTimeout_Test_001
 * @tc.desc: Verify the query timeout functionality of the DataShareHelper for the data share proxy when the timeout
 *           is configured to 0ms, checking the result set and business error code.
 * @tc.type: FUNC
 * @tc.require: issueIC8OCN
 * @tc.precon:
    1. The pre-initialized DataShareHelper instance (dataShareHelper) is available and non-null.
    2. DATA_SHARE_PROXY_URI is a valid URI for the data share proxy.
    3. The test data source contains at least one record where TBL_NAME0 equals "wang".
    4. The DatashareBusinessError class supports the GetCode method to retrieve the error code (E_OK is predefined).
 * @tc.step:
    1. Obtain the DataShareHelper instance by assigning dataShareHelper to a local variable (helper).
    2. Create a Uri object using DATA_SHARE_PROXY_URI.
    3. Create a DataSharePredicates object and set the condition: TBL_NAME0 equals "wang" via EqualTo.
    4. Initialize an empty vector<string> for columns; create a DataShareOption object and set its timeout to 0ms.
    5. Declare a DatashareBusinessError object, then call helper->Query with Uri, predicates, columns, option, and
       the error object.
    6. Check the result set and the error code from the DatashareBusinessError object.
 * @tc.expect:
    1. The Query method returns a non-null DataShareResultSet.
    2. The error code obtained via DatashareBusinessError::GetCode is E_OK (success status).
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
 * @tc.desc: Verify the stability and performance of the DataShareHelper's query timeout function for the data share
 *           proxy by executing 100 sequential queries with a 4000ms timeout, checking execution time and results.
 * @tc.type: FUNC
 * @tc.require: issueIC8OCN
 * @tc.precon:
    1. The pre-initialized DataShareHelper instance (dataShareHelper) is available and non-null.
    2. DATA_SHARE_PROXY_URI is a valid URI for the data share proxy.
    3. The test data source has exactly one record where TBL_NAME0 equals "wang".
    4. The test environment supports std::chrono for measuring execution time (milliseconds).
 * @tc.step:
    1. Obtain the DataShareHelper instance (helper) from dataShareHelper; create a Uri via DATA_SHARE_PROXY_URI.
    2. Create a DataSharePredicates object and set TBL_NAME0 = "wang" using EqualTo; initialize an empty column vector.
    3. Create a DataShareOption object and set its timeout to 4000ms; define limitTime (1000ms) and repeatTimes (100).
    4. Loop 100 times:
        a. Record the start time using std::chrono::steady_clock.
        b. Call helper->Query with Uri, predicates, columns, option, and a DatashareBusinessError object.
        c. Record the end time and calculate the execution duration.
        d. Check the result set, execution duration, error code, and row count.
 * @tc.expect:
    1. Each query returns a non-null DataShareResultSet.
    2. Each query’s execution duration is less than 1000ms.
    3. Each query’s DatashareBusinessError code is E_OK.
    4. Each result set’s row count (via GetRowCount) is 1.
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
 * @tc.desc: Verify the stability of the DataShareHelper's query timeout function for the data share proxy under
 *           multi-threaded concurrent access (10 threads, 100 queries each), checking consistency of results and time.
 * @tc.type: FUNC
 * @tc.require: issueIC8OCN
 * @tc.precon:
    1. The pre-initialized DataShareHelper instance (dataShareHelper) is thread-safe for concurrent queries.
    2. DATA_SHARE_PROXY_URI is a valid URI for the data share proxy; test data has one record with TBL_NAME0 = "wang".
    3. The test environment supports std::thread for multi-threading and std::chrono for time measurement.
    4. DatashareBusinessError and DataShareResultSet work normally in concurrent scenarios.
 * @tc.step:
    1. Obtain helper from dataShareHelper; create Uri (DATA_SHARE_PROXY_URI), predicates (TBL_NAME0 = "wang"), and
       empty columns.
    2. Configure DataShareOption (timeout = 4000ms), limitTime (1000ms), repeatTimes (100), and threadNum (10).
    3. Define a lambda function: loop 100 times, execute query, measure time, and verify result set/error/row count.
    4. Create 10 std::thread objects, each running the lambda function.
    5. Call join() on all threads to wait for their completion.
 * @tc.expect:
    1. All threads complete execution without crashes or exceptions.
    2. Every query returns a non-null DataShareResultSet.
    3. Every query’s execution time is less than 1000ms.
    4. Every query’s DatashareBusinessError code is E_OK.
    5. Every result set’s row count is 1.
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
 * @tc.desc: Verify the behavior of the Query function (inherited from the parent class of DataShareHelper) when
 *           called directly, focusing on whether it returns nullptr as expected.
 * @tc.type: FUNC
 * @tc.require: issueICS05H
 * @tc.precon:
    1. The pre-initialized DataShareHelper instance (dataShareHelper) is available and non-null.
    2. DATA_SHARE_PROXY_URI is a valid URI for the data share proxy.
    3. The parent class of DataShareHelper has a Query method that returns nullptr when called directly.
    4. DataShareOption and DatashareBusinessError can be instantiated normally.
 * @tc.step:
    1. Obtain the DataShareHelper instance (helper) by assigning dataShareHelper to a local variable.
    2. Create a Uri object using DATA_SHARE_PROXY_URI.
    3. Create a DataSharePredicates object and set TBL_NAME0 = "wang" via EqualTo; initialize an empty column vector.
    4. Create a DataShareOption object and set its timeout to 0ms (timeout is meaningless for the parent class method).
    5. Declare a DatashareBusinessError object, then call the parent class’s Query method directly:
       helper->DataShareHelper::Query.
    6. Check whether the returned DataShareResultSet is nullptr.
 * @tc.expect:
    1. The parent class’s Query method (called directly) returns a nullptr DataShareResultSet.
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
 * @tc.desc: Verify that the data share proxy returns a null result set with E_RESULTSET_BUSY when the maximum number
 *           of concurrent result sets (32) is exceeded, including result set cleanup.
 * @tc.type: FUNC
 * @tc.precon:
    1. The pre-initialized DataShareHelper instance (dataShareHelper) is available and non-null.
    2. DATA_SHARE_PROXY_URI is valid; test data has one record with TBL_NAME0 = "wang".
    3. The data share proxy limits concurrent result sets to 32; E_RESULTSET_BUSY is a predefined error code.
    4. The ResultSet’s Close method returns E_OK on success.
 * @tc.step:
    1. Obtain helper from dataShareHelper; create Uri (DATA_SHARE_PROXY_URI) and predicates (TBL_NAME0 = "wang").
    2. Initialize a vector to store 32 result sets: loop 32 times, call Query, verify each result set is non-null,
       and save it.
    3. Execute the 33rd Query with a DatashareBusinessError object; check if the result set is null.
    4. Retrieve the error code from the DatashareBusinessError object and verify it is E_RESULTSET_BUSY.
    5. Loop through the vector of 32 result sets, call Close on each, and verify Close returns E_OK.
 * @tc.expect:
    1. The first 32 queries return non-null result sets.
    2. The 33rd query returns a null result set.
    3. The 33rd query’s DatashareBusinessError code is E_RESULTSET_BUSY.
    4. All 32 result sets return E_OK when Close is called.
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
 * @tc.desc: Verify the functionality of adding and deleting query templates in the data share proxy, focusing on
 *           the success of template registration and removal via AddQueryTemplate and DelQueryTemplate.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The pre-initialized DataShareHelper instance (dataShareHelper) is available and non-null.
    2. The DATA_SHARE_PROXY_URI constant is a valid URI for the data share proxy; SUBSCRIBER_ID is a valid
       subscriber ID.
    3. The PredicateTemplateNode and Template classes can be instantiated normally with SQL query parameters.
 * @tc.step:
    1. Obtain the DataShareHelper instance by assigning dataShareHelper to a local variable (helper).
    2. Create two PredicateTemplateNode objects: node1 ("p1", "select name0 as name from TBL00") and
       node2 ("p2", "select name1 as name from TBL00").
    3. Assemble the nodes into a vector<PredicateTemplateNode>, then create a Template object with this vector and
       the SQL "select name1 as name from TBL00".
    4. Call helper->AddQueryTemplate with DATA_SHARE_PROXY_URI, SUBSCRIBER_ID, and the Template, record the return
       value.
    5. Call helper->DelQueryTemplate with the same URI and SUBSCRIBER_ID, record the return value.
 * @tc.expect:
    1. The return value of AddQueryTemplate is 0, indicating successful template registration.
    2. The return value of DelQueryTemplate is 0, indicating successful template deletion.
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
 * @tc.desc: Verify that adding and deleting query templates with an invalid URI in the data share proxy fails as
 *           expected, returning the E_URI_NOT_EXIST error code.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The pre-initialized DataShareHelper instance (dataShareHelper) is available and non-null.
    2. SUBSCRIBER_ID is a valid subscriber ID; E_URI_NOT_EXIST is a predefined invalid URI error code.
    3. PredicateTemplateNode and Template can be instantiated normally with valid SQL queries.
 * @tc.step:
    1. Obtain the DataShareHelper instance (helper) from dataShareHelper.
    2. Create two PredicateTemplateNode objects (node1: "p1", "select name0 as name from TBL00"; node2: "p2",
       "select name1 as name from TBL00"), assemble them into a vector, and create a Template object.
    3. Define an invalid URI (errorUri = "datashareproxy://com.acts.ohos.data.datasharetest").
    4. Call helper->AddQueryTemplate with errorUri, SUBSCRIBER_ID, and the Template, check the return value.
    5. Call helper->DelQueryTemplate with errorUri and SUBSCRIBER_ID, check the return value.
 * @tc.expect:
    1. The return value of AddQueryTemplate is E_URI_NOT_EXIST, indicating failure due to invalid URI.
    2. The return value of DelQueryTemplate is E_URI_NOT_EXIST, indicating failure due to invalid URI.
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
 * @tc.desc: Verify the update functionality of query templates in the data share proxy, including template
 *           registration, RDB data subscription, update triggering via data insertion, and result verification.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The pre-initialized DataShareHelper instance (dataShareHelper) is available and non-null.
    2. DATA_SHARE_PROXY_URI is valid; TBL_NAME0 is a valid column in the data source.
    3. The SubscribeRdbData and UnsubscribeRdbData methods support template-based data change listening.
    4. E_OK is a predefined success error code.
 * @tc.step:
    1. Create a PredicateTemplateNode ("p1", "select name0 as name from TBL00"), assemble it into a vector, then
       create a Template with an update SQL: "update TBL00 set name0 = 'updatetest' where name0 = 'name00'".
    2. Call AddQueryTemplate with DATA_SHARE_PROXY_URI, SUBSCRIBER_ID, and the Template; verify return value is 0.
    3. Create a URI vector (containing DATA_SHARE_PROXY_URI) and a TemplateId (subscriberId_ = SUBSCRIBER_ID,
       bundleName_ = "ohos.datashareproxyclienttest.demo"), call SubscribeRdbData and check errCode_ is E_OK.
    4. Insert a record (TBL_NAME0 = "name00") via Insert, verify return value > 0.
    5. Query with predicates (TBL_NAME0 = "updatetest") to check if the result set has 1 row.
    6. Call UnsubscribeRdbData with the URI vector and TemplateId, verify errCode_ is E_OK.
 * @tc.expect:
    1. AddQueryTemplate returns 0 (successful template registration).
    2. SubscribeRdbData and UnsubscribeRdbData return errCode_ = E_OK (successful subscription/unsubscription).
    3. The Insert method returns a value > 0 (successful data insertion).
    4. The query result set has 1 row (update applied successfully).
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
 * @tc.desc: Verify the functionality of adding and deleting query templates with parameterized update functions in
 *           the data share proxy, including two rounds of template registration and removal.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The pre-initialized DataShareHelper instance (dataShareHelper) is available and non-null.
    2. DATA_SHARE_PROXY_URI is a valid URI for the data share proxy; SUBSCRIBER_ID is a valid subscriber ID.
    3. The Template class supports instantiation with and without explicit update SQL parameters.
 * @tc.step:
    1. Create two PredicateTemplateNodes: node1 ("p1", "select name0 as name from TBL00"), node2 ("p2",
       "select name1 as name from TBL00"), assemble them into a vector.
    2. Create Template1 with the node vector and SQL "select name1 as name from TBL00"; call AddQueryTemplate
       and DelQueryTemplate, verify both return 0.
    3. Create Template2 with an explicit update SQL ("update TBL00 set name0 = 'update'"), the same node vector,
       and SQL "select name1 as name from TBL00".
    4. Call AddQueryTemplate and DelQueryTemplate with Template2, verify both return 0.
 * @tc.expect:
    1. All AddQueryTemplate calls return 0 (successful template registration).
    2. All DelQueryTemplate calls return 0 (successful template deletion).
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
 * @tc.desc: Verify that adding a query template with an invalid parameterized update function (insert SQL instead of
 *           update SQL) fails, and deleting the unregistered template succeeds in the data share proxy.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The pre-initialized DataShareHelper instance (dataShareHelper) is available and non-null.
    2. DATA_SHARE_PROXY_URI and SUBSCRIBER_ID are valid; the Template class rejects non-update SQL as update functions.
    3. DelQueryTemplate returns 0 even if the template does not exist (no error on deletion of non-existent entry).
 * @tc.step:
    1. Create two PredicateTemplateNodes (node1: "p1", "select name0 as name from TBL00"; node2: "p2",
       "select name1 as name from TBL00"), assemble them into a vector.
    2. Create a Template with an invalid update SQL (insert: "insert into TBL00 (name0) values ('test')"), the node
       vector, and SQL "select name1 as name from TBL00".
    3. Call helper->AddQueryTemplate with DATA_SHARE_PROXY_URI, SUBSCRIBER_ID, and the Template; check return value.
    4. Call helper->DelQueryTemplate with the same URI and SUBSCRIBER_ID; check return value.
 * @tc.expect:
    1. AddQueryTemplate returns -1 (failure due to invalid update SQL).
    2. DelQueryTemplate returns 0 (success, no error for unregistered template).
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
 * @tc.desc: Verify the functionality of adding and deleting query templates using a URI with a user ID parameter
 *           ("?user=100") in the data share proxy.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The pre-initialized DataShareHelper instance (dataShareHelper) is available and non-null.
    2. DATA_SHARE_PROXY_URI is a valid base URI; appending "?user=100" forms a valid parameterized URI.
    3. SUBSCRIBER_ID is a valid subscriber ID; E_OK is a predefined success error code.
 * @tc.step:
    1. Create two PredicateTemplateNodes: node1 ("p1", "select name0 as name from TBL00"), node2 ("p2",
       "select name1 as name from TBL00"), assemble them into a vector, then create a Template with the vector
       and SQL "select name1 as name from TBL00".
    2. Create a parameterized URI by appending "?user=100" to DATA_SHARE_PROXY_URI.
    3. Call helper->AddQueryTemplate with the parameterized URI, SUBSCRIBER_ID, and the Template; check return value.
    4. Call helper->DelQueryTemplate with the same parameterized URI and SUBSCRIBER_ID; check return value.
 * @tc.expect:
    1. AddQueryTemplate returns E_OK (successful template registration with parameterized URI).
    2. DelQueryTemplate returns E_OK (successful template deletion with parameterized URI).
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
 * @tc.desc: Verify the functionality of publishing string-type data via the data share proxy and retrieving the
 *           published data using GetPublishedData, ensuring data consistency.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The pre-initialized DataShareHelper instance (dataShareHelper) is available and non-null.
    2. The Data class supports storing data entries (URI, subscriber ID, string value); OperationResult returns
       errCode_ for publish status.
    3. The target bundle name ("com.acts.ohos.data.datasharetest") is valid and exists.
 * @tc.step:
    1. Define a valid bundle name: "com.acts.ohos.data.datasharetest".
    2. Create a Data object, add a data entry to its datas_: URI
       ("datashareproxy://com.acts.ohos.data.datasharetest/test"), SUBSCRIBER_ID, string value ("value1").
    3. Call helper->Publish with the Data object and bundle name; check each OperationResult.errCode_ is 0.
    4. Call helper->GetPublishedData with the bundle name, record the errCode_ and retrieved Data (getData).
    5. Verify the size, subscriber ID, key (URI), and string value of getData.datas_ match the published data.
 * @tc.expect:
    1. The Publish method returns OperationResult with errCode_ = 0 (successful data publishing).
    2. The GetPublishedData method returns errCode_ = 0 (successful data retrieval).
    3. The retrieved data has the same size as published data, and each entry matches in subscriber ID, key (URI),
       and string value ("value1").
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
 * @tc.desc: Verify that publishing data to a non-existent bundle name and retrieving data from the same invalid
 *           bundle name fail, returning E_BUNDLE_NAME_NOT_EXIST in the data share proxy.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The pre-initialized DataShareHelper instance (dataShareHelper) is available and non-null.
    2. The non-existent bundle name ("com.acts.ohos.error") is predefined; E_BUNDLE_NAME_NOT_EXIST is a predefined
       error code for non-existent bundles.
    3. The Data class can be instantiated with a data entry (URI, subscriber ID, string value).
 * @tc.step:
    1. Define a non-existent bundle name: "com.acts.ohos.error".
    2. Create a Data object, add a data entry to its datas_: URI ("datashareproxy://com.acts.ohos.error"),
       SUBSCRIBER_ID, string value ("value1").
    3. Call helper->Publish with the Data object and non-existent bundle name; check each OperationResult.errCode_.
    4. Call helper->GetPublishedData with the same non-existent bundle name, record the errCode_.
 * @tc.expect:
    1. The Publish method returns OperationResult with errCode_ = E_BUNDLE_NAME_NOT_EXIST.
    2. The GetPublishedData method returns errCode_ = E_BUNDLE_NAME_NOT_EXIST (failure due to invalid bundle).
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
 * @tc.desc: Verify the functionality of publishing binary data (vector<uint8_t>) via the data share proxy and
 *           retrieving it using GetPublishedData, ensuring binary content consistency.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The pre-initialized DataShareHelper instance (dataShareHelper) is available and non-null.
    2. The Data class supports storing binary data entries; the IsAshmem method identifies binary data, and GetData
       retrieves the binary content.
    3. The target bundle name ("com.acts.ohos.data.datasharetest") is valid and exists.
 * @tc.step:
    1. Define a valid bundle name: "com.acts.ohos.data.datasharetest".
    2. Create a binary buffer (vector<uint8_t> buffer = {10, 20, 30}).
    3. Create a Data object, add a data entry to its datas_: URI
       ("datashareproxy://com.acts.ohos.data.datasharetest/test"), SUBSCRIBER_ID, and the binary buffer.
    4. Call helper->Publish with the Data object and bundle name; check each OperationResult.errCode_ is 0.
    5. Call helper->GetPublishedData with the bundle name, record errCode_ and retrieved Data (getData).
    6. Verify getData.datas_ matches the published data in size, subscriber ID, and binary content.
 * @tc.expect:
    1. The Publish method returns OperationResult with errCode_ = 0 (successful binary data publishing).
    2. The GetPublishedData method returns errCode_ = 0 (successful binary data retrieval).
    3. The retrieved data has the same size as published data; each entry’s binary content matches.
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
 * @tc.desc: Verify the combination functionality of RDB data subscription, enabling/disabling subscriptions, and
 *           unsubscription in the data share proxy, including callback triggering verification via data insertion.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The pre-initialized DataShareHelper instance (dataShareHelper) is available and non-null.
    2. DATA_SHARE_PROXY_URI is a valid URI; SUBSCRIBER_ID is a valid subscriber ID for RDB subscriptions.
    3. The global callback counter (g_callbackTimes) is initialized to 0 before the test starts.
    4. TBL_NAME1 is a valid column in the data source pointed to by DATA_SHARE_PROXY_URI.
 * @tc.step:
    1. Create a PredicateTemplateNode ("p1", "select name0 as name from TBL00"), assemble it into a vector, then
       create a Template object with the vector and SQL "select name1 as name from TBL00".
    2. Call helper->AddQueryTemplate with DATA_SHARE_PROXY_URI, SUBSCRIBER_ID, and the Template; verify return value
       is 0.
    3. Create a URI vector (containing DATA_SHARE_PROXY_URI) and a TemplateId (subscriberId_ = SUBSCRIBER_ID,
       bundleName_ = "ohos.datashareproxyclienttest.demo"), then call SubscribeRdbData with a callback that increments
       g_callbackTimes; check each OperationResult.errCode_ is 0.
    4. Call helper->EnableRdbSubs with the URI vector and TemplateId; verify each errCode_ is 0.
    5. Insert two records (TBL_NAME1 = "wu" and "liu") via Insert; check each return value > 0 and g_callbackTimes = 2.
    6. Call helper->UnsubscribeRdbData with the URI vector and TemplateId; verify each errCode_ is 0.
    7. Insert another record; verify g_callbackTimes remains 2 (no additional callback).
 * @tc.expect:
    1. AddQueryTemplate, SubscribeRdbData, EnableRdbSubs, and UnsubscribeRdbData return errCode_ = 0 (success).
    2. All Insert operations return values > 0 (successful data insertion).
    3. The callback is invoked 2 times before unsubscription (g_callbackTimes = 2).
    4. No additional callback invocations occur after unsubscription (g_callbackTimes remains 2).
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
 * @tc.desc: Verify the combination functionality of RDB data subscription (with two helpers), multiple disable
 *           operations, re-enable, and unsubscription in the data share proxy, focusing on callback count consistency.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. Two pre-initialized DataShareHelper instances (dataShareHelper and dataShareHelper2) are available and non-null.
    2. DATA_SHARE_PROXY_URI and SUBSCRIBER_ID are valid; the data source supports concurrent subscriptions from
       multiple helpers.
    3. TBL_NAME1 is a valid column; std::mutex and std::condition_variable work for thread-safe callback counting.
 * @tc.step:
    1. Create an empty vector<PredicateTemplateNode>, then create a Template with the vector and SQL
       "select name1 as name from TBL00".
    2. Call dataShareHelper->AddQueryTemplate with DATA_SHARE_PROXY_URI, SUBSCRIBER_ID, and the Template;
       verify return value is 0.
    3. Create a URI vector (containing DATA_SHARE_PROXY_URI) and a TemplateId (subscriberId_ = SUBSCRIBER_ID,
       bundleName_ = "ohos.datashareproxyclienttest.demo").
    4. Subscribe with dataShareHelper (callback increments atomic counter) and dataShareHelper2; verify
       all OperationResult.errCode_ are 0.
    5. Wait for initial callback (counter = 1), then call DisableRdbSubs with dataShareHelper; verify errCode_ = 0.
    6. Insert a record (TBL_NAME1 = 1) via Insert; check return value > 0.
    7. Call DisableRdbSubs again (no error), then EnableRdbSubs; verify errCode_ = 0 and counter = 2.
    8. Unsubscribe from RDB data changes using both helpers.
 * @tc.expect:
    1. AddQueryTemplate, subscriptions, disable/enable, and unsubscription return errCode_ = 0 (success).
    2. The Insert operation returns a value > 0 (successful data insertion).
    3. The callback is invoked 2 times total (1 before disable, 1 after re-enable).
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
 * @tc.desc: Verify the combination functionality of RDB data subscription, disable, re-enable, and unsubscription
 *           in the data share proxy, ensuring no unexpected callback invocations after disable/re-enable.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The pre-initialized DataShareHelper instance (dataShareHelper) is available and non-null.
    2. DATA_SHARE_PROXY_URI, SUBSCRIBER_ID, and E_OK (predefined success code) are valid.
    3. An atomic callback counter (callbackTimes) and thread synchronization tools (mutex, condition_variable) are
       initialized for safe callback counting.
 * @tc.step:
    1. Create an empty vector<PredicateTemplateNode>, then create a Template with the vector and SQL
       "select name1 as name from TBL00".
    2. Call helper->AddQueryTemplate with DATA_SHARE_PROXY_URI, SUBSCRIBER_ID, and the Template; verify return value
       is E_OK.
    3. Create a URI vector (containing DATA_SHARE_PROXY_URI) and a TemplateId (subscriberId_ = SUBSCRIBER_ID,
       bundleName_ = "ohos.datashareproxyclienttest.demo").
    4. Call SubscribeRdbData with a callback that increments callbackTimes; verify each OperationResult.errCode_
       is E_OK.
    5. Wait for the initial callback (callbackTimes = 1) using condition_variable with a 2-second timeout.
    6. Call DisableRdbSubs, then EnableRdbSubs with the URI vector and TemplateId; verify each errCode_ is 0.
    7. Verify callbackTimes remains 1 (no unexpected invocations), then call UnsubscribeRdbData; check each errCode_
       is E_OK.
 * @tc.expect:
    1. AddQueryTemplate, SubscribeRdbData, DisableRdbSubs, EnableRdbSubs, and UnsubscribeRdbData return success.
    2. The callback is invoked 1 time (only initial subscription, no invocations after disable/re-enable).
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
 * @tc.desc: Verify the combination functionality of published data subscription (two helpers), disable, re-enable,
 *           and unsubscription in the data share proxy, including republish verification.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. Two pre-initialized DataShareHelper instances (dataShareHelper and dataShareHelper2) are available and non-null.
    2. The target bundle name ("com.acts.ohos.data.datasharetest") and DATA_SHARE_PROXY_URI are valid.
    3. The Data class supports storing published data entries; an atomic callback counter and thread synchronization
       tools are initialized.
 * @tc.step:
    1. Create a Data object, add a data entry (DATA_SHARE_PROXY_URI, SUBSCRIBER_ID, "value1") to its datas_; call
       helper->Publish with the Data and bundle name; verify each OperationResult.errCode_ is 0.
    2. Create a URI vector (containing DATA_SHARE_PROXY_URI); subscribe to published data with dataShareHelper
       (callback increments counter) and dataShareHelper2 (empty callback); check all errCode_ are 0.
    3. Wait for initial callback (counter = 1) with a 2-second timeout, then call helper->DisablePubSubs; verify
       errCode_ = 0.
    4. Republish the same Data object; verify Publish returns errCode_ = 0 (no callback during disable).
    5. Call DisablePubSubs again (no error), then EnablePubSubs; verify errCode_ = 0 and counter = 2.
    6. Unsubscribe from published data using both helpers.
 * @tc.expect:
    1. Publish, SubscribePublishedData, DisablePubSubs, EnablePubSubs, and unsubscription return errCode_ = 0.
    2. The callback is invoked 2 times total (1 before disable, 1 after re-enable).
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
 * @tc.desc: Verify the basic functionality of subscribing to published data changes in the data share proxy,
 *           focusing on subscription success and callback verification.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The pre-initialized DataShareHelper instance (dataShareHelper) is available and non-null.
    2. DATA_SHARE_PROXY_URI and SUBSCRIBER_ID are valid for published data subscriptions.
    3. The expected owner bundle name ("ohos.datashareproxyclienttest.demo") is predefined and matches the callback’s
       verification logic.
 * @tc.step:
    1. Create a vector<std::string> (uris) and add DATA_SHARE_PROXY_URI to it.
    2. Call helper->SubscribePublishedData with uris, SUBSCRIBER_ID, and a callback that verifies
       changeNode.ownerBundleName_ equals "ohos.datashareproxyclienttest.demo".
    3. Check the size of the returned OperationResult vector matches the size of uris.
    4. Verify each OperationResult.errCode_ in the vector is 0.
 * @tc.expect:
    1. The size of the OperationResult vector is equal to the size of the uris vector.
    2. Each OperationResult.errCode_ is 0 (successful subscription).
    3. When triggered, the callback correctly verifies that ownerBundleName_ matches the expected value.
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
 * @tc.desc: Verify the basic functionality of disabling active published data subscriptions in the data share proxy.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The pre-initialized DataShareHelper instance (dataShareHelper) is available and non-null.
    2. DATA_SHARE_PROXY_URI and SUBSCRIBER_ID are valid, and there are active published data subscriptions
       for the specified uris and subscriber ID.
    3. The DisablePubSubs method returns OperationResult objects to indicate disable status.
 * @tc.step:
    1. Create a vector<std::string> (uris) and add DATA_SHARE_PROXY_URI (with active subscriptions) to it.
    2. Call helper->DisablePubSubs with the uris vector and SUBSCRIBER_ID.
    3. Iterate through the returned vector of OperationResult objects.
    4. Check the errCode_ of each OperationResult to verify disable success.
 * @tc.expect:
    1. Each OperationResult.errCode_ in the returned vector is 0 (successful disabling of subscriptions).
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
 * @tc.desc: Verify the basic functionality of enabling disabled published data subscriptions in the data share proxy.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The pre-initialized DataShareHelper instance (dataShareHelper) is available and non-null.
    2. DATA_SHARE_PROXY_URI and SUBSCRIBER_ID are valid, and the published data subscriptions for the specified
       uris and subscriber ID are currently disabled.
    3. The EnablePubSubs method returns OperationResult objects to indicate enable status.
 * @tc.step:
    1. Create a vector<std::string> (uris) and add DATA_SHARE_PROXY_URI (with disabled subscriptions) to it.
    2. Call helper->EnablePubSubs with the uris vector and SUBSCRIBER_ID.
    3. Iterate through the returned vector of OperationResult objects.
    4. Check the errCode_ of each OperationResult to verify enable success.
 * @tc.expect:
    1. Each OperationResult.errCode_ in the returned vector is 0 (successful enabling of subscriptions).
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
 * @tc.desc: Verify the basic functionality of unsubscribing from active published data changes in the data
 *           share proxy.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The pre-initialized DataShareHelper instance (dataShareHelper) is available and non-null.
    2. DATA_SHARE_PROXY_URI and SUBSCRIBER_ID are valid, and there are active published data subscriptions
       for the specified uris and subscriber ID.
    3. The UnsubscribePublishedData method returns OperationResult objects to indicate unsubscription status.
 * @tc.step:
    1. Create a vector<std::string> (uris) and add DATA_SHARE_PROXY_URI (with active subscriptions) to it.
    2. Call helper->UnsubscribePublishedData with the uris vector and SUBSCRIBER_ID.
    3. Check the size of the returned OperationResult vector matches the size of the uris vector.
    4. Verify each OperationResult.errCode_ in the vector is 0.
 * @tc.expect:
    1. The size of the OperationResult vector is equal to the size of the uris vector.
    2. Each OperationResult.errCode_ is 0 (successful unsubscription).
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
 * @tc.name: ProxyDatasTest_GetDataShareHelperType_Test_001
 * @tc.desc: Verify helper type
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: None
 * @tc.step:
    1. get silent datasharehelper type
 * @tc.expect:
    1. GetDataShareHelperType returns SILENT
 */
HWTEST_F(ProxyDatasTest, ProxyDatasTest_GetDataShareHelperType_Test_001, TestSize.Level0)
{
    LOG_INFO("ProxyDatasTest_GetDataShareHelperType_Test_001::Start");
    auto helper = dataShareHelper;
    DataShareType ret = helper->GetDataShareHelperType();
    EXPECT_EQ(ret, SILENT);
    LOG_INFO("ProxyDatasTest_GetDataShareHelperType_Test_001::End");
}

/**
 * @tc.name: ProxyDatasTest_SetDataShareHelperExtUri_Test_001
 * @tc.desc: Verify the SetDataShareHelperExtUri function for silent datashare.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: None
 * @tc.step:
    1. set silent datasharehelper extUri
 * @tc.expect:
    1. GetDataShareHelperType returns E_OK
 */
HWTEST_F(ProxyDatasTest, ProxyDatasTest_SetDataShareHelperExtUri_Test_001, TestSize.Level0)
{
    LOG_INFO("ProxyDatasTest_SetDataShareHelperExtUri_Test_001::Start");
    auto helper = dataShareHelper;
    int32_t ret = helper->SetDataShareHelperExtUri("NON_SILENT_URI");
    EXPECT_EQ(ret, E_DATASHARE_INVALID_URI);
    ret = helper->SetDataShareHelperExtUri("datashare//media");
    EXPECT_EQ(ret, E_DATASHARE_INVALID_URI);
    ret = helper->SetDataShareHelperExtUri("datashare://media");
    EXPECT_EQ(ret, E_DATASHARE_INVALID_URI);
    ret = helper->SetDataShareHelperExtUri("datashare:///media?user=100");
    EXPECT_EQ(ret, E_DATASHARE_INVALID_URI);
    ret = helper->SetDataShareHelperExtUri("datashare:///");
    EXPECT_EQ(ret, E_DATASHARE_INVALID_URI);
    ret = helper->SetDataShareHelperExtUri("aaaaaadatashare:///media");
    EXPECT_EQ(ret, E_DATASHARE_INVALID_URI);
    ret = helper->SetDataShareHelperExtUri("datashare:///media");
    EXPECT_EQ(ret, E_OK);
    LOG_INFO("ProxyDatasTest_SetDataShareHelperExtUri_Test_001::End");
}

/**
 * @tc.name: ProxyDatasTest_extSpCtl_Null_Test_001
 * @tc.desc: Verify the behavior of extended special control operations (GetFileTypes, OpenFile, OpenRawFile) in the
 *           data share proxy after releasing the DataShareHelper instance.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The pre-initialized DataShareHelper instance (dataShareHelper) is available and non-null before release.
    2. The Release method of DataShareHelper returns a boolean to indicate release status.
    3. Empty Uri ("") and empty string are valid inputs for the tested extended operations.
 * @tc.step:
    1. Call helper->Release() to release the DataShareHelper instance; verify the return value is true.
    2. Create an empty Uri (uri) and an empty string (str).
    3. Call helper->GetFileTypes(uri, str) and check the size of the returned vector<std::string>.
    4. Call helper->OpenFile(uri, str) and record the return value (error code).
    5. Call helper->OpenRawFile(uri, str) and record the return value (error code).
 * @tc.expect:
    1. The Release method returns true (successful release of the helper).
    2. GetFileTypes returns an empty vector<std::string> (size = 0).
    3. Both OpenFile and OpenRawFile return -1 (operation failure after release).
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
 * @tc.desc: Verify the behavior of NormalizeUri and DenormalizeUri operations in the data share proxy after
 *           releasing the DataShareHelper instance.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The pre-initialized DataShareHelper instance (dataShareHelper) is available and non-null before release.
    2. The Release method of DataShareHelper returns a boolean to indicate release status.
    3. The Uri class supports equality comparison (==) to verify unchanged output.
 * @tc.step:
    1. Call helper->Release() to release the DataShareHelper instance; verify the return value is true.
    2. Create an empty Uri (inputUri) to use as the input for NormalizeUri and DenormalizeUri.
    3. Call helper->NormalizeUri(inputUri) and store the result in uriResult1.
    4. Call helper->DenormalizeUri(inputUri) and store the result in uriResult2.
    5. Compare uriResult1 and uriResult2 with inputUri to check if they are unchanged.
 * @tc.expect:
    1. The Release method returns true (successful release of the helper).
    2. The result of NormalizeUri (uriResult1) is equal to the input empty Uri.
    3. The result of DenormalizeUri (uriResult2) is equal to the input empty Uri.
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