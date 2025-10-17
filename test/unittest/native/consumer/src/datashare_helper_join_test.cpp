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
* @tc.desc: Verify cross join operation returns correct results
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create cross join predicate with "user.userId = book.userId" condition
    2. Execute query on user table with join predicate
    3. Verify row count is 3
    4. Check first row data matches expected values
* @tc.experct:
    1. Query returns 3 rows
    2. First row contains correct user and book data for userId = 1
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
* @tc.require: None
* @tc.desc: Verify inner join with filter returns correct single result
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Create inner join predicate with "user.userId = book.userId" condition
    2. Add filter for book.name = "SanGuo"
    3. Execute query on user table with join predicate
    4. Verify row count is 1
    5. Check row data matches expected values for "SanGuo" book
* @tc.experct:
    1. Query returns 1 row
    2. Row contains correct user and book data for "SanGuo" book
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
* @tc.desc: Verify left outer join with Using clause returns correct result
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create left outer join predicate using "userId" column
    2. Add filter for name = "SanGuo"
    3. Execute query on user table with join predicate
    4. Verify row count is 1
    5. Check row data matches expected values
* @tc.experct:
    1. Query returns 1 row
    2. Row contains correct user and book data for matching record
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
* @tc.desc: Verify left outer join returns all user records with matching books
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create left outer join predicate with "user.userId = book.userId" condition
    2. Execute query on user table with join predicate
    3. Verify row count is 5 (all users including those without books)
* @tc.experct:
    1. Query returns 5 rows
    2. All user records are included in results
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