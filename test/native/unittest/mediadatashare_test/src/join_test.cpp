/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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
std::string USER_URI = "datashare:///com.acts.datasharetest/entry/DB00/user?Proxy=true";
std::string BOOK_URI = "datashare:///com.acts.datasharetest/entry/DB00/book?Proxy=true";
std::string TBL_STU_NAME = "name";
std::string TBL_STU_AGE = "age";
std::shared_ptr<DataShare::DataShareHelper> g_slientAccessHelper;

class JoinTest : public testing::Test {
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

void JoinTest::SetUpTestCase(void)
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
        .appIDDesc = "ohos.datashareclienttest.demo" };
    HapPolicyParams policy = { .apl = APL_NORMAL,
        .domain = "test.domain",
        .permList = { { .permissionName = "ohos.permission.test",
            .bundleName = "ohos.datashareclienttest.demo",
            .grantMode = 1,
            .availableLevel = APL_NORMAL,
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
            }
        }
    };
    AccessTokenKit::AllocHapToken(info, policy);
    auto testTokenId = Security::AccessToken::AccessTokenKit::GetHapTokenID(
        info.userID, info.bundleName, info.instIndex);
    SetSelfTokenID(testTokenId);

    g_slientAccessHelper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID, SLIENT_ACCESS_URI);
    ASSERT_TRUE(g_slientAccessHelper != nullptr);
    JoinTest::InsertUserDates();
    JoinTest::InsertBookDates();
    LOG_INFO("SetUpTestCase end");
}

void JoinTest::TearDownTestCase(void)
{
    auto tokenId = AccessTokenKit::GetHapTokenID(100, "ohos.datashareclienttest.demo", 0);
    AccessTokenKit::DeleteToken(tokenId);
    g_slientAccessHelper = nullptr;
}

void JoinTest::SetUp(void) {}
void JoinTest::TearDown(void) {}

void JoinTest::InsertUserDates()
{
    LOG_INFO("JoinTest::InsertUserDates start");
    DataShare::DataShareValuesBucket values;

    values.Put("userId", 1);
    values.Put("firstName", "Zhang");
    values.Put("lastName", "San");
    values.Put("age", 29);
    values.Put("balance", 100.51);
    Uri userUri (USER_URI);
    g_slientAccessHelper->Insert(userUri, values);

    values.Clear();
    values.Put("userId", 2);
    values.Put("firstName", "Li");
    values.Put("lastName", "Si");
    values.Put("age", 30);
    values.Put("balance", 200.51);
    g_slientAccessHelper->Insert(userUri, values);

    values.Clear();
    values.Put("userId", 3);
    values.Put("firstName", std::string("Wang"));
    values.Put("lastName", std::string("Wu"));
    values.Put("age", 30);
    values.Put("balance", 300.51);
    g_slientAccessHelper->Insert(userUri, values);

    values.Clear();
    values.Put("userId", 4);
    values.Put("firstName", "Sun");
    values.Put("lastName", "Liu");
    values.Put("age", 31);
    values.Put("balance", 400.51);
    g_slientAccessHelper->Insert(userUri, values);

    values.Clear();
    values.Put("userId", 5);
    values.Put("firstName", "Ma");
    values.Put("lastName", "Qi");
    values.Put("age", 32);
    values.Put("balance", 500.51);
    g_slientAccessHelper->Insert(userUri, values);
    LOG_INFO("JoinTest::InsertUserDates ends");
}

void JoinTest::InsertBookDates()
{
    LOG_INFO("JoinTest::InsertBookDates start");
    DataShare::DataShareValuesBucket values;

    values.Put("id", 1);
    values.Put("name", "SanGuo");
    values.Put("userId", 1);
    Uri bookUri (BOOK_URI);
    g_slientAccessHelper->Insert(bookUri, values);

    values.Clear();
    values.Put("id", 2);
    values.Put("name", "XiYouJi");
    values.Put("userId", 2);
    g_slientAccessHelper->Insert(bookUri, values);

    values.Clear();
    values.Put("id", 3);
    values.Put("name", "ShuiHuZhuan");
    values.Put("userId", 3);
    g_slientAccessHelper->Insert(bookUri, values);
    LOG_INFO("JoinTest::InsertBookDates end");
}

int JoinTest::ResultSize(std::shared_ptr<DataShareResultSet> &resultSet)
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

HWTEST_F(JoinTest, Join_CrossJoin_001, TestSize.Level0)
{
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
}

HWTEST_F(JoinTest, Join_InnerJoin_001, TestSize.Level0)
{
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
}

HWTEST_F(JoinTest, Join_LeftOuterJoin_001, TestSize.Level0)
{
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
}

HWTEST_F(JoinTest, Join_LeftOuterJoin_002, TestSize.Level0)
{
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
}
} // namespace DataShare
} // namespace OHOS