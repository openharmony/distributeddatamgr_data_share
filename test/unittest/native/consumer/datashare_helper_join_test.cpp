/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#define LOG_TAG "datashare_helper_join_test"

#include <gtest/gtest.h>
#include <unistd.h>

#include "accesstoken_kit.h"
#include "datashare_helper.h"
#include "datashare_log.h"
#include "datashare_values_bucket.h"
#include "hap_token_info.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "token_setproc.h"

namespace OHOS {
namespace DataShare {
using namespace testing::ext;
using namespace OHOS::Security::AccessToken;
constexpr int STORAGE_MANAGER_MANAGER_ID = 5003;
std::string DATA_SHARE_URI = "datashare:///com.acts.datasharetest";
std::string SLIENT_ACCESS_URI = "datashare:///com.acts.datasharetest?Proxy=true";
std::string USER_URI = "datashareproxy://com.acts.datasharetest/entry/DB00/user";
std::string BOOK_URI = "datashareproxy://com.acts.datasharetest/entry/DB00/book";
std::string TBL_STU_NAME = "name";
std::string TBL_STU_AGE = "age";
int64_t USERId1 = 1;
int64_t USERId2 = 2;
int64_t USERId3 = 3;
int64_t USERId4 = 4;
int64_t USERId5 = 5;

std::shared_ptr<DataShare::DataShareHelper> g_slientAccessHelper;

class DataShareHelperJoinTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    static void InsertUserDates();
    static void InsertBookDates();
    int ResultSize(std::shared_ptr<DataShareResultSet> &resultSet);
};

std::shared_ptr<DataShare::DataShareHelper> CreateDataShareHelper(int32_t systemAbilityId, std::string uri)
{
    LOG_INFO("CreateDataShareHelper start");
    auto saManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (saManager == nullptr) {
        LOG_ERROR("GetSystemAbilityManager get samgr failed.");
        return nullptr;
    }
    auto remoteObj = saManager->GetSystemAbility(systemAbilityId);
    if (remoteObj == nullptr) {
        LOG_ERROR("GetSystemAbility service failed.");
        return nullptr;
    }
    return DataShare::DataShareHelper::Creator(remoteObj, uri);
}

void DataShareHelperJoinTest::SetUpTestCase(void)
{
    LOG_INFO("SetUpTestCase invoked");
    auto dataShareHelper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID, DATA_SHARE_URI);
    ASSERT_TRUE(dataShareHelper != nullptr);
    int sleepTime = 3;
    sleep(sleepTime);

    HapInfoParams info = {
        .userID = 100,
        .bundleName = "ohos.datashareclienttest.demo",
        .instIndex = 0,
        .isSystemApp = true,
        .appIDDesc = "ohos.datashareclienttest.demo" };
    HapPolicyParams policy = { .apl = APL_SYSTEM_CORE,
        .domain = "test.domain",
        .permList = { { .permissionName = "ohos.permission.test",
            .bundleName = "ohos.datashareclienttest.demo",
            .grantMode = 1,
            .availableLevel = APL_SYSTEM_CORE,
            .label = "label",
            .labelId = 1,
            .description = "ohos.datashareclienttest.demo",
            .descriptionId = 1 }
        },
        .permStateList = {
            {
                .permissionName = "ohos.permission.test",
                .isGeneral = true,
                .resDeviceID = { "local" },
                .grantStatus = { PermissionState::PERMISSION_GRANTED },
                .grantFlags = { 1 }
            },
            {
                .permissionName = "ohos.permission.GET_BUNDLE_INFO",
                .isGeneral = true,
                .resDeviceID = { "local" },
                .grantStatus = { PermissionState::PERMISSION_GRANTED },
                .grantFlags = { 1 }
            }
        }
    };
    AccessTokenKit::AllocHapToken(info, policy);
    auto testTokenId = Security::AccessToken::AccessTokenKit::GetHapTokenIDEx(
        info.userID, info.bundleName, info.instIndex);
    SetSelfTokenID(testTokenId.tokenIDEx);

    g_slientAccessHelper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID, SLIENT_ACCESS_URI);
    ASSERT_TRUE(g_slientAccessHelper != nullptr);
    DataShareHelperJoinTest::InsertUserDates();
    DataShareHelperJoinTest::InsertBookDates();
    LOG_INFO("SetUpTestCase end");
}

void DataShareHelperJoinTest::TearDownTestCase(void)
{
    auto tokenId = AccessTokenKit::GetHapTokenID(100, "ohos.datashareclienttest.demo", 0);
    AccessTokenKit::DeleteToken(tokenId);
    g_slientAccessHelper = nullptr;
}

void DataShareHelperJoinTest::SetUp(void) {}
void DataShareHelperJoinTest::TearDown(void) {}

void DataShareHelperJoinTest::InsertUserDates()
{
    LOG_INFO("DataShareHelperJoinTest::InsertUserDates start");
    DataShare::DataShareValuesBucket values;
    values.Put("userId", USERId1);
    values.Put("firstName", "Zhang");
    values.Put("lastName", "San");
    int64_t age1 = 29;
    values.Put("age", age1);
    double balance1 = 100.51;
    values.Put("balance", balance1);
    Uri userUri (USER_URI);
    g_slientAccessHelper->Insert(userUri, values);
    values.Clear();
    values.Put("userId", USERId2);
    values.Put("firstName", "Li");
    values.Put("lastName", "Si");
    int64_t age2 = 30;
    values.Put("age", age2);
    double balance2 = 200.51;
    values.Put("balance", balance2);
    g_slientAccessHelper->Insert(userUri, values);
    values.Clear();
    values.Put("userId", USERId3);
    values.Put("firstName", std::string("Wang"));
    values.Put("lastName", std::string("Wu"));
    values.Put("age", age2);
    double balance3 = 300.51;
    values.Put("balance", balance3);
    g_slientAccessHelper->Insert(userUri, values);
    values.Clear();
    values.Put("userId", USERId4);
    values.Put("firstName", "Sun");
    values.Put("lastName", "Liu");
    int64_t age3 = 31;
    values.Put("age", age3);
    double balance4 = 400.51;
    values.Put("balance", balance4);
    g_slientAccessHelper->Insert(userUri, values);
    values.Clear();
    values.Put("userId", USERId5);
    values.Put("firstName", "Ma");
    values.Put("lastName", "Qi");
    int64_t age4 = 32;
    values.Put("age", age4);
    double balance5 = 500.51;
    values.Put("balance", balance5);
    g_slientAccessHelper->Insert(userUri, values);
    LOG_INFO("DataShareHelperJoinTest::InsertUserDates ends");
}

void DataShareHelperJoinTest::InsertBookDates()
{
    LOG_INFO("DataShareHelperJoinTest::InsertBookDates start");
    DataShare::DataShareValuesBucket values;

    int64_t id1 = 1;
    values.Put("id", id1);
    values.Put("name", "SanGuo");
    values.Put("userId", USERId1);
    Uri bookUri (BOOK_URI);
    g_slientAccessHelper->Insert(bookUri, values);

    values.Clear();
    int64_t id2 = 2;
    values.Put("id", id2);
    values.Put("name", "XiYouJi");
    values.Put("userId", USERId2);
    g_slientAccessHelper->Insert(bookUri, values);

    values.Clear();
    int64_t id3 = 3;
    values.Put("id", id3);
    values.Put("name", "ShuiHuZhuan");
    values.Put("userId", USERId3);
    g_slientAccessHelper->Insert(bookUri, values);
    LOG_INFO("DataShareHelperJoinTest::InsertBookDates end");
}

int DataShareHelperJoinTest::ResultSize(std::shared_ptr<DataShareResultSet> &resultSet)
{
    if (resultSet->GoToFirstRow() != E_OK) {
        return 0;
    }
    int count = 1;
    while (resultSet->GoToNextRow() == E_OK) {
        count++;
    }
    return count;
}

/**
 * @tc.name: Join_CrossJoin_001
 * @tc.desc: Verify that the cross join operation between the user table and book table (with the join condition
 *           "user.userId = book.userId") returns correct results, including the expected row count and first row data.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports the CrossJoin and On methods of DataSharePredicates to define cross join
       conditions.
    2. The global g_slientAccessHelper is properly initialized (non-null) and can access the user table via USER_URI.
    3. The user table and book table contain preset test data: the join condition "user.userId = book.userId" matches
       exactly 3 rows of combined data, with the first row corresponding to userId = 1.
    4. The USER_URI constant is valid and points to the user table; the DataShareResultSet's GetRowCount, GoToFirstRow,
       GetInt, GetString, and GetDouble methods work normally.
 * @tc.step:
    1. Create a DataSharePredicates object, call its CrossJoin method with "book" (target join table), then call On
       with a vector containing the condition "user.userId = book.userId" to set the join clause.
    2. Initialize an empty std::vector<std::string> (columns) to specify query columns, then get the user table's URI
       via USER_URI.
    3. Call g_slientAccessHelper->Query with USER_URI, the created predicates, and columns to execute the cross join
       query,
       and obtain the returned DataShareResultSet.
    4. Call GetRowCount on the result set to get the total row count, then verify it equals 3.
    5. Call GoToFirstRow to move to the first row of the result set, then check the values of columns (userId,
       firstName, lastName, age, balance, book.id, book.name, book.userId) to confirm they match the expected values
       for userId = 1.
 * @tc.expect:
    1. The query returns a DataShareResultSet with a row count of 3.
    2. The first row of the result set contains the correct data: userId = 1, firstName = "Zhang", lastName = "San",
       age = 29, balance = 100.51, book.id = 1, book.name = "SanGuo", book.userId = 1.
 */
HWTEST_F(DataShareHelperJoinTest, Join_CrossJoin_001, TestSize.Level0)
{
    LOG_INFO("Join_CrossJoin_001 start");
    auto helper = g_slientAccessHelper;
    DataShare::DataSharePredicates predicates;
    std::vector<std::string> clauses;
    clauses.push_back("user.userId = book.userId");
    predicates.CrossJoin("book")->On(clauses);

    std::vector<std::string> columns;
    Uri userUri (USER_URI);
    auto resultSet = g_slientAccessHelper->Query(userUri, predicates, columns);
    int rowCount;
    resultSet->GetRowCount(rowCount);
    EXPECT_EQ(3, rowCount);

    EXPECT_EQ(E_OK, resultSet->GoToFirstRow());
    int userId;
    EXPECT_EQ(E_OK, resultSet->GetInt(0, userId));
    EXPECT_EQ(1, userId);

    std::string firstName;
    EXPECT_EQ(E_OK, resultSet->GetString(1, firstName));
    EXPECT_EQ("Zhang", firstName);

    std::string lastName;
    EXPECT_EQ(E_OK, resultSet->GetString(2, lastName));
    EXPECT_EQ("San", lastName);

    int age;
    EXPECT_EQ(E_OK, resultSet->GetInt(3, age));
    EXPECT_EQ(29, age);

    double balance;
    EXPECT_EQ(E_OK, resultSet->GetDouble(4, balance));
    EXPECT_EQ(100.51, balance);

    int id;
    EXPECT_EQ(E_OK, resultSet->GetInt(5, id));
    EXPECT_EQ(1, id);

    std::string name;
    EXPECT_EQ(E_OK, resultSet->GetString(6, name));
    EXPECT_EQ("SanGuo", name);

    int userId_1;
    EXPECT_EQ(E_OK, resultSet->GetInt(7, userId_1));
    EXPECT_EQ(1, userId_1);
    LOG_INFO("Join_CrossJoin_001 end");
}

/**
 * @tc.name: Join_InnerJoin_001
 * @tc.desc: Verify that the inner join between the user table and book table (with the join condition "user.userId
 *           = book.userId" and filter "book.name = 'SanGuo'") returns a single correct result row.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports the InnerJoin, On, and EqualTo methods of DataSharePredicates to define inner join
       conditions and filters.
    2. The global g_slientAccessHelper is properly initialized (non-null) and can access the user table via USER_URI.
    3. The user table and book table contain preset data: only one row matches both the inner join condition
       "user.userId = book.userId" and the filter "book.name = 'SanGuo'".
    4. The USER_URI constant is valid; the DataShareResultSet's row and column access methods (GetRowCount,
       GoToFirstRow, GetInt, etc.) work.
 * @tc.step:
    1. Create a DataSharePredicates object, call its InnerJoin method with "book", then call On with a vector
       containing "user.userId = book.userId" to set the join condition.
    2. Call the EqualTo method on the predicates to add a filter: "book.name" equals "SanGuo".
    3. Initialize an empty std::vector<std::string> (columns) for query columns, then get the user table's URI via
       USER_URI.
    4. Execute the query using g_slientAccessHelper->Query with USER_URI, predicates, and columns, then obtain the
       DataShareResultSet.
    5. Verify the result set's row count is 1 by calling GetRowCount.
    6. Move to the first row via GoToFirstRow, then check the values of all columns (user and book fields) to confirm
       they match the expected data for the "SanGuo" book.
 * @tc.expect:
    1. The query returns a DataShareResultSet with a row count of 1.
    2. The single row contains correct data: userId = 1, firstName = "Zhang", lastName = "San", age = 29,
       balance = 100.51, book.id = 1, book.name = "SanGuo", book.userId = 1.
 */
HWTEST_F(DataShareHelperJoinTest, Join_InnerJoin_001, TestSize.Level0)
{
    LOG_INFO("Join_InnerJoin_001 start");
    auto helper = g_slientAccessHelper;
    DataShare::DataSharePredicates predicates;
    std::vector<std::string> clauses;
    clauses.push_back("user.userId = book.userId");
    predicates.InnerJoin("book")->On(clauses)->EqualTo("book.name", "SanGuo");

    std::vector<std::string> columns;
    Uri userUri (USER_URI);
    auto resultSet = g_slientAccessHelper->Query(userUri, predicates, columns);

    int rowCount;
    resultSet->GetRowCount(rowCount);
    EXPECT_EQ(1, rowCount);
    EXPECT_EQ(E_OK, resultSet->GoToFirstRow());

    int userId;
    EXPECT_EQ(E_OK, resultSet->GetInt(0, userId));
    EXPECT_EQ(1, userId);

    std::string firstName;
    EXPECT_EQ(E_OK, resultSet->GetString(1, firstName));
    EXPECT_EQ("Zhang", firstName);

    std::string lastName;
    EXPECT_EQ(E_OK, resultSet->GetString(2, lastName));
    EXPECT_EQ("San", lastName);

    int age;
    EXPECT_EQ(E_OK, resultSet->GetInt(3, age));
    EXPECT_EQ(29, age);

    double balance;
    EXPECT_EQ(E_OK, resultSet->GetDouble(4, balance));
    EXPECT_EQ(100.51, balance);

    int id;
    EXPECT_EQ(E_OK, resultSet->GetInt(5, id));
    EXPECT_EQ(1, id);

    std::string name;
    EXPECT_EQ(E_OK, resultSet->GetString(6, name));
    EXPECT_EQ("SanGuo", name);

    int userId_1;
    EXPECT_EQ(E_OK, resultSet->GetInt(7, userId_1));
    EXPECT_EQ(1, userId_1);
    LOG_INFO("Join_InnerJoin_001 end");
}

/**
 * @tc.name: Join_LeftOuterJoin_001
 * @tc.desc: Verify that the left outer join between the user table and book table (using the "userId" column via the
 *           Using clause and filtering for "name = 'SanGuo'") returns a single correct result row.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports the LeftOuterJoin and Using methods of DataSharePredicates, allowing join
       conditions to be set via the Using clause with a vector of shared columns.
    2. The global g_slientAccessHelper is properly initialized (non-null) and can access the user table via USER_URI.
    3. The user table and book table have preset data: only one row matches the left outer join (on userId) and the
       filter "name = 'SanGuo'".
    4. The USER_URI is valid; the DataShareResultSet's methods for row/column access and row count checking work
       normally.
 * @tc.step:
    1. Create a DataSharePredicates object, call its LeftOuterJoin method with "book" (target join table).
    2. Create a std::vector<std::string> (fields) containing "userId", then call the Using method on the predicates to
       set the shared join column.
    3. Add a filter to the predicates via EqualTo: "name" equals "SanGuo".
    4. Initialize an empty std::vector<std::string> (columns) for query columns, then get the user table's URI via
       USER_URI.
    5. Execute the query using g_slientAccessHelper->Query with USER_URI, predicates, and columns, then obtain the
       DataShareResultSet.
    6. Verify the result set's row count is 1, then move to the first row and check all column values against the
       expected data.
 * @tc.expect:
    1. The query returns a DataShareResultSet with a row count of 1.
    2. The single row contains correct data: userId = 1, firstName = "Zhang", lastName = "San", age = 29,
       balance = 100.51, book.id = 1, book.name = "SanGuo".
 */
HWTEST_F(DataShareHelperJoinTest, Join_LeftOuterJoin_001, TestSize.Level0)
{
    LOG_INFO("Join_LeftOuterJoin_001 start");
    auto helper = g_slientAccessHelper;
    DataShare::DataSharePredicates predicates;
    std::vector<std::string> fields;
    fields.push_back("userId");
    predicates.LeftOuterJoin("book")->Using(fields)->EqualTo("name", "SanGuo");

    std::vector<std::string> columns;
    Uri userUri (USER_URI);
    auto resultSet = g_slientAccessHelper->Query(userUri, predicates, columns);

    int rowCount;
    resultSet->GetRowCount(rowCount);
    EXPECT_EQ(1, rowCount);
    EXPECT_EQ(E_OK, resultSet->GoToFirstRow());

    int userId;
    EXPECT_EQ(E_OK, resultSet->GetInt(0, userId));
    EXPECT_EQ(1, userId);

    std::string firstName;
    EXPECT_EQ(E_OK, resultSet->GetString(1, firstName));
    EXPECT_EQ("Zhang", firstName);

    std::string lastName;
    EXPECT_EQ(E_OK, resultSet->GetString(2, lastName));
    EXPECT_EQ("San", lastName);

    int age;
    EXPECT_EQ(E_OK, resultSet->GetInt(3, age));
    EXPECT_EQ(29, age);

    double balance;
    EXPECT_EQ(E_OK, resultSet->GetDouble(4, balance));
    EXPECT_EQ(100.51, balance);

    int id;
    EXPECT_EQ(E_OK, resultSet->GetInt(5, id));
    EXPECT_EQ(1, id);

    std::string name;
    EXPECT_EQ(E_OK, resultSet->GetString(6, name));
    EXPECT_EQ("SanGuo", name);
    LOG_INFO("Join_LeftOuterJoin_001 end");
}

/**
 * @tc.name: Join_LeftOuterJoin_002
 * @tc.desc: Verify that the left outer join between the user table and book table (with the condition
 *           "user.userId = book.userId") returns all user records (including those without matching books), resulting
 *           in a total of 5 rows.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports the LeftOuterJoin and On methods of DataSharePredicates to define left outer join
       conditions.
    2. The global g_slientAccessHelper is properly initialized (non-null) and can access the user table via USER_URI.
    3. The user table contains enough preset records such that a left outer join with the book table
       (on "user.userId = book.userId") results in exactly 5 rows (including users with no matching books).
    4. The USER_URI is valid; the DataShareResultSet's GetRowCount method works correctly to return the total number of
       rows.
 * @tc.step:
    1. Create a DataSharePredicates object, call its LeftOuterJoin method with "book" (target join table).
    2. Call the On method on the predicates with a vector containing the condition "user.userId = book.userId" to set
       the join clause.
    3. Initialize an empty std::vector<std::string> (columns) for query columns, then get the user table's URI via
       USER_URI.
    4. Execute the query using g_slientAccessHelper->Query with USER_URI, predicates, and columns, then obtain the
       DataShareResultSet.
    5. Call GetRowCount on the result set to get the total number of rows, then verify the count equals 5.
 * @tc.expect:
    1. The query returns a DataShareResultSet with a row count of 5.
    2. All user records from the user table are included in the result set (including users with no matching entries in
       the book table).
 */
HWTEST_F(DataShareHelperJoinTest, Join_LeftOuterJoin_002, TestSize.Level0)
{
    LOG_INFO("Join_LeftOuterJoin_002 start");
    auto helper = g_slientAccessHelper;
    DataShare::DataSharePredicates predicates;
    std::vector<std::string> clauses;
    clauses.push_back("user.userId = book.userId");
    predicates.LeftOuterJoin("book")->On(clauses);

    std::vector<std::string> columns;
    Uri userUri (USER_URI);
    auto resultSet = g_slientAccessHelper->Query(userUri, predicates, columns);
    int rowCount;
    resultSet->GetRowCount(rowCount);
    EXPECT_EQ(5, rowCount);
    LOG_INFO("Join_LeftOuterJoin_002 end");
}
} // namespace DataShare
} // namespace OHOS