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
std::string SLIENT_ACCESS_URI = "datashare:///com.acts.datasharetest/entry/DB00/TBL00?Proxy=true";
std::string DATA_SHARE_PROXY_URI = "datashare:///com.acts.ohos.data.datasharetest/test";
constexpr int SUBSCRIBER_ID = 1000;
std::string TBL_STU_NAME = "name";
std::string TBL_STU_AGE = "age";
int32_t ERROR = -1;
std::shared_ptr<DataShare::DataShareHelper> g_slientAccessHelper;

class SlientSwitchTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

std::shared_ptr<DataShare::DataShareHelper> CreateDataShareHelper(int32_t systemAbilityId,
    const std::string &silentProxyUri, const std::string &providerUri = "")
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
    return DataShare::DataShareHelper::Creator(remoteObj, silentProxyUri, providerUri);
}

void SlientSwitchTest::SetUpTestCase(void)
{
    LOG_INFO("SetUpTestCase invoked");
    auto dataShareHelper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID, DATA_SHARE_URI);
    ASSERT_TRUE(dataShareHelper != nullptr);
    int sleepTime = 3;
    sleep(sleepTime);

    HapInfoParams info = {
        .userID = 100,
        .bundleName = "com.acts.datasharetest",
        .instIndex = 0,
        .appIDDesc = "com.acts.datasharetest"
    };
    HapPolicyParams policy = {
        .apl = APL_NORMAL,
        .domain = "test.domain",
        .permList = {
            {
                .permissionName = "ohos.permission.test",
                .bundleName = "com.acts.datasharetest",
                .grantMode = 1,
                .availableLevel = APL_NORMAL,
                .label = "label",
                .labelId = 1,
                .description = "com.acts.datasharetest",
                .descriptionId = 1
            }
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
    LOG_INFO("SetUpTestCase end");
}

void SlientSwitchTest::TearDownTestCase(void)
{
    auto tokenId = AccessTokenKit::GetHapTokenID(100, "com.acts.datasharetest", 0);
    AccessTokenKit::DeleteToken(tokenId);
    g_slientAccessHelper = nullptr;
}

void SlientSwitchTest::SetUp(void) {}
void SlientSwitchTest::TearDown(void) {}

HWTEST_F(SlientSwitchTest, SlientSwitch_SetSilentSwitch_Test_001, TestSize.Level0)
{
    LOG_INFO("SlientSwitch_SetSilentSwitch_Test_001::Start");
    Uri uri(SLIENT_ACCESS_URI);
    int retVal = DataShareHelper::SetSilentSwitch(uri, true);
    EXPECT_EQ(retVal, E_OK);
    LOG_INFO("SlientSwitch_SetSilentSwitch_Test_001::End");
}

HWTEST_F(SlientSwitchTest, SlientSwitch_SetSilentSwitch_Test_002, TestSize.Level0)
{
    LOG_INFO("SlientSwitch_SetSilentSwitch_Test_002::Start");
    Uri uri(SLIENT_ACCESS_URI);
    int retVal = DataShareHelper::SetSilentSwitch(uri, false);
    EXPECT_EQ(retVal, E_OK);
    LOG_INFO("SlientSwitch_SetSilentSwitch_Test_002::End");
}

HWTEST_F(SlientSwitchTest, SlientSwitch_SetSilentSwitch_Test_003, TestSize.Level0)
{
    LOG_INFO("SlientSwitch_SetSilentSwitch_Test_003::Start");
    Uri uri("");
    int retVal = DataShareHelper::SetSilentSwitch(uri, false);
    EXPECT_EQ(retVal, E_OK);
    LOG_INFO("SlientSwitch_SetSilentSwitch_Test_003::End");
}

HWTEST_F(SlientSwitchTest, SlientSwitch_SetSilentSwitch_Test_004, TestSize.Level0)
{
    LOG_INFO("SlientSwitch_SetSilentSwitch_Test_004::Start");
    Uri uri("");
    int retVal = DataShareHelper::SetSilentSwitch(uri, true);
    EXPECT_EQ(retVal, E_OK);
    LOG_INFO("SlientSwitch_SetSilentSwitch_Test_004::End");
}

HWTEST_F(SlientSwitchTest, SlientSwitch_SwitchDisable_Insert_Test_001, TestSize.Level0)
{
    LOG_INFO("SlientSwitch_SwitchDisable_Insert_Test_001::Start");
    Uri uri(SLIENT_ACCESS_URI);
    int retVal = DataShareHelper::SetSilentSwitch(uri, false);
    EXPECT_EQ(retVal, E_OK);

    auto helper = g_slientAccessHelper;
    DataShare::DataShareValuesBucket valuesBucket;
    std::string value = "lisi";
    valuesBucket.Put(TBL_STU_NAME, value);
    int age = 25;
    valuesBucket.Put(TBL_STU_AGE, age);

    retVal = helper->Insert(uri, valuesBucket);
    EXPECT_EQ((retVal > 0), false);
    LOG_INFO("SlientSwitch_SwitchDisable_Insert_Test_001::End");
}

HWTEST_F(SlientSwitchTest, SlientSwitch_SwitchDisable_Insert_Test_002, TestSize.Level0)
{
    LOG_INFO("SlientSwitch_SwitchDisable_Insert_Test_002::Start");
    Uri uri("");
    int retVal = DataShareHelper::SetSilentSwitch(uri, false);
    EXPECT_EQ(retVal, E_OK);

    auto helper = g_slientAccessHelper;
    DataShare::DataShareValuesBucket valuesBucket;
    std::string value = "wangwu";
    valuesBucket.Put(TBL_STU_NAME, value);
    int age = 25;
    valuesBucket.Put(TBL_STU_AGE, age);
    uri = Uri(SLIENT_ACCESS_URI);
    retVal = helper->Insert(uri, valuesBucket);
    EXPECT_EQ((retVal > 0), false);
    LOG_INFO("SlientSwitch_SwitchDisable_Insert_Test_002::End");
}

HWTEST_F(SlientSwitchTest, SlientSwitch_SwitchEnable_Insert_Test_001, TestSize.Level0)
{
    LOG_INFO("SlientSwitch_SwitchEnable_Insert_Test_001::Start");
    Uri uri(SLIENT_ACCESS_URI);
    int retVal = DataShareHelper::SetSilentSwitch(uri, true);
    EXPECT_EQ(retVal, E_OK);

    auto helper = g_slientAccessHelper;
    DataShare::DataShareValuesBucket valuesBucket;
    std::string value = "lisi";
    valuesBucket.Put(TBL_STU_NAME, value);
    int age = 25;
    valuesBucket.Put(TBL_STU_AGE, age);

    retVal = helper->Insert(uri, valuesBucket);
    EXPECT_EQ((retVal > 0), true);
    LOG_INFO("SlientSwitch_SwitchEnable_Insert_Test_001::End");
}

HWTEST_F(SlientSwitchTest, SlientSwitch_SwitchEnable_Insert_Test_002, TestSize.Level0)
{
    LOG_INFO("SlientSwitch_SwitchEnable_Insert_Test_002::Start");
    Uri uri("");
    int retVal = DataShareHelper::SetSilentSwitch(uri, true);
    EXPECT_EQ(retVal, E_OK);

    auto helper = g_slientAccessHelper;
    DataShare::DataShareValuesBucket valuesBucket;
    std::string value = "wangwu";
    valuesBucket.Put(TBL_STU_NAME, value);
    int age = 25;
    valuesBucket.Put(TBL_STU_AGE, age);
    uri = Uri(SLIENT_ACCESS_URI);
    retVal = helper->Insert(uri, valuesBucket);
    EXPECT_EQ((retVal > 0), true);
    LOG_INFO("SlientSwitch_SwitchEnable_Insert_Test_002::End");
}

HWTEST_F(SlientSwitchTest, SlientSwitch_SwitchDisable_Update_Test_001, TestSize.Level0)
{
    LOG_INFO("SlientSwitch_SwitchDisable_Update_Test_001::Start");
    Uri uri(SLIENT_ACCESS_URI);
    int retVal = DataShareHelper::SetSilentSwitch(uri, false);
    EXPECT_EQ(retVal, E_OK);

    auto helper = g_slientAccessHelper;
    DataShare::DataShareValuesBucket valuesBucket;
    int value = 50;
    valuesBucket.Put(TBL_STU_AGE, value);
    DataShare::DataSharePredicates predicates;
    std::string selections = TBL_STU_NAME + " = 'lisi'";
    predicates.SetWhereClause(selections);
    retVal = helper->Update(uri, predicates, valuesBucket);
    EXPECT_EQ((retVal > 0), false);
    LOG_INFO("SlientSwitch_SwitchDisable_Update_Test_001::End");
}

HWTEST_F(SlientSwitchTest, SlientSwitch_SwitchDisable_Update_Test_002, TestSize.Level0)
{
    LOG_INFO("SlientSwitch_SwitchDisable_Update_Test_002::Start");
    Uri uri("");
    int retVal = DataShareHelper::SetSilentSwitch(uri, false);
    EXPECT_EQ(retVal, E_OK);

    auto helper = g_slientAccessHelper;
    DataShare::DataShareValuesBucket valuesBucket;
    int value = 50;
    valuesBucket.Put(TBL_STU_AGE, value);
    DataShare::DataSharePredicates predicates;
    std::string selections = TBL_STU_NAME + " = 'wangwu'";
    predicates.SetWhereClause(selections);
    uri = Uri(SLIENT_ACCESS_URI);
    retVal = helper->Update(uri, predicates, valuesBucket);
    EXPECT_EQ((retVal > 0), false);
    LOG_INFO("SlientSwitch_SwitchDisable_Update_Test_002::End");
}

HWTEST_F(SlientSwitchTest, SlientSwitch_SwitchEnable_Update_Test_001, TestSize.Level0)
{
    LOG_INFO("SlientSwitch_SwitchEnable_Update_Test_001::Start");
    Uri uri(SLIENT_ACCESS_URI);
    int retVal = DataShareHelper::SetSilentSwitch(uri, true);
    EXPECT_EQ(retVal, E_OK);

    auto helper = g_slientAccessHelper;
    DataShare::DataShareValuesBucket valuesBucket;
    int value = 50;
    valuesBucket.Put(TBL_STU_AGE, value);
    DataShare::DataSharePredicates predicates;
    std::string selections = TBL_STU_NAME + " = 'lisi'";
    predicates.SetWhereClause(selections);
    retVal = helper->Update(uri, predicates, valuesBucket);
    EXPECT_EQ((retVal > 0), true);
    LOG_INFO("SlientSwitch_SwitchEnable_Update_Test_001::End");
}

HWTEST_F(SlientSwitchTest, SlientSwitch_SwitchEnable_Update_Test_002, TestSize.Level0)
{
    LOG_INFO("SlientSwitch_SwitchEnable_Update_Test_002::Start");
    Uri uri("");
    int retVal = DataShareHelper::SetSilentSwitch(uri, true);
    EXPECT_EQ(retVal, E_OK);

    auto helper = g_slientAccessHelper;
    DataShare::DataShareValuesBucket valuesBucket;
    int value = 50;
    valuesBucket.Put(TBL_STU_AGE, value);
    DataShare::DataSharePredicates predicates;
    std::string selections = TBL_STU_NAME + " = 'wangwu'";
    predicates.SetWhereClause(selections);
    uri = Uri(SLIENT_ACCESS_URI);
    retVal = helper->Update(uri, predicates, valuesBucket);
    EXPECT_EQ((retVal > 0), true);
    LOG_INFO("SlientSwitch_SwitchEnable_Update_Test_002::End");
}

HWTEST_F(SlientSwitchTest, SlientSwitch_SwitchDisable_Query_Test_001, TestSize.Level0)
{
    LOG_INFO("SlientSwitch_SwitchDisable_Query_Test_001::Start");
    Uri uri(SLIENT_ACCESS_URI);
    int retVal = DataShareHelper::SetSilentSwitch(uri, false);
    EXPECT_EQ(retVal, E_OK);

    auto helper = g_slientAccessHelper;
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(TBL_STU_NAME, "lisi");
    vector<string> columns;
    auto resultSet = helper->Query(uri, predicates, columns);
    ASSERT_TRUE(resultSet == nullptr);
    LOG_INFO("SlientSwitch_SwitchDisable_Query_Test_001::End");
}

HWTEST_F(SlientSwitchTest, SlientSwitch_SwitchDisable_Query_Test_002, TestSize.Level0)
{
    LOG_INFO("SlientSwitch_SwitchDisable_Query_Test_002::Start");
    Uri uri("");
    int retVal = DataShareHelper::SetSilentSwitch(uri, false);
    EXPECT_EQ(retVal, E_OK);

    auto helper = g_slientAccessHelper;
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(TBL_STU_NAME, "wangwu");
    vector<string> columns;
    uri = Uri(SLIENT_ACCESS_URI);
    auto resultSet = helper->Query(uri, predicates, columns);
    ASSERT_TRUE(resultSet == nullptr);
    LOG_INFO("SlientSwitch_SwitchDisable_Query_Test_002::End");
}

HWTEST_F(SlientSwitchTest, SlientSwitch_SwitchEnable_Query_Test_001, TestSize.Level0)
{
    LOG_INFO("SlientSwitch_SwitchEnable_Query_Test_001::Start");
    Uri uri(SLIENT_ACCESS_URI);
    int retVal = DataShareHelper::SetSilentSwitch(uri, true);
    EXPECT_EQ(retVal, E_OK);

    auto helper = g_slientAccessHelper;
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(TBL_STU_NAME, "lisi");
    vector<string> columns;
    auto resultSet = helper->Query(uri, predicates, columns);
    int result = 0;
    if (resultSet != nullptr) {
        resultSet->GetRowCount(result);
    }
    EXPECT_EQ(result, 1);
    LOG_INFO("SlientSwitch_SwitchEnable_Query_Test_001::End");
}

HWTEST_F(SlientSwitchTest, SlientSwitch_SwitchEnable_Query_Test_002, TestSize.Level0)
{
    LOG_INFO("SlientSwitch_SwitchEnable_Query_Test_002::Start");
    Uri uri("");
    int retVal = DataShareHelper::SetSilentSwitch(uri, true);
    EXPECT_EQ(retVal, E_OK);

    auto helper = g_slientAccessHelper;
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(TBL_STU_NAME, "wangwu");
    vector<string> columns;
    uri = Uri(SLIENT_ACCESS_URI);
    auto resultSet = helper->Query(uri, predicates, columns);
    int result = 0;
    if (resultSet != nullptr) {
        resultSet->GetRowCount(result);
    }
    EXPECT_EQ(result, 1);
    LOG_INFO("SlientSwitch_SwitchEnable_Query_Test_002::End");
}

HWTEST_F(SlientSwitchTest, SlientSwitch_SwitchDisable_Delete_Test_001, TestSize.Level0)
{
    LOG_INFO("SlientSwitch_SwitchDisable_Delete_Test_001::Start");
    Uri uri(SLIENT_ACCESS_URI);
    int retVal = DataShareHelper::SetSilentSwitch(uri, false);
    EXPECT_EQ(retVal, E_OK);

    auto helper = g_slientAccessHelper;
    DataShare::DataSharePredicates deletePredicates;
    std::string selections = TBL_STU_NAME + " = 'lisi'";
    deletePredicates.SetWhereClause(selections);
    retVal = helper->Delete(uri, deletePredicates);
    EXPECT_EQ((retVal > 0), false);
    LOG_INFO("SlientSwitch_SwitchDisable_Delete_Test_001::End");
}

HWTEST_F(SlientSwitchTest, SlientSwitch_SwitchDisable_Delete_Test_002, TestSize.Level0)
{
    LOG_INFO("SlientSwitch_SwitchDisable_Delete_Test_002::Start");
    Uri uri("");
    int retVal = DataShareHelper::SetSilentSwitch(uri, false);
    EXPECT_EQ(retVal, E_OK);

    auto helper = g_slientAccessHelper;
    DataShare::DataSharePredicates deletePredicates;
    std::string selections = TBL_STU_NAME + " = 'wangwu'";
    deletePredicates.SetWhereClause(selections);
    uri = Uri(SLIENT_ACCESS_URI);
    retVal = helper->Delete(uri, deletePredicates);
    EXPECT_EQ((retVal > 0), false);
    LOG_INFO("SlientSwitch_SwitchDisable_Delete_Test_002::End");
}

HWTEST_F(SlientSwitchTest, SlientSwitch_SwitchEnable_Delete_Test_001, TestSize.Level0)
{
    LOG_INFO("SlientSwitch_SwitchEnable_Delete_Test_001::Start");
    Uri uri(SLIENT_ACCESS_URI);
    int retVal = DataShareHelper::SetSilentSwitch(uri, true);
    EXPECT_EQ(retVal, E_OK);

    auto helper = g_slientAccessHelper;
    DataShare::DataSharePredicates deletePredicates;
    std::string selections = TBL_STU_NAME + " = 'lisi'";
    deletePredicates.SetWhereClause(selections);
    retVal = helper->Delete(uri, deletePredicates);
    EXPECT_EQ((retVal > 0), true);
    LOG_INFO("SlientSwitch_SwitchEnable_Delete_Test_001::End");
}

HWTEST_F(SlientSwitchTest, SlientSwitch_SwitchEnable_Delete_Test_002, TestSize.Level0)
{
    LOG_INFO("SlientSwitch_SwitchEnable_Delete_Test_002::Start");
    Uri uri("");
    int retVal = DataShareHelper::SetSilentSwitch(uri, true);
    EXPECT_EQ(retVal, E_OK);

    auto helper = g_slientAccessHelper;
    DataShare::DataSharePredicates deletePredicates;
    std::string selections = TBL_STU_NAME + " = 'wangwu'";
    deletePredicates.SetWhereClause(selections);
    uri = Uri(SLIENT_ACCESS_URI);
    retVal = helper->Delete(uri, deletePredicates);
    EXPECT_EQ((retVal > 0), true);
    LOG_INFO("SlientSwitch_SwitchEnable_Delete_Test_002::End");
}

HWTEST_F(SlientSwitchTest, SlientSwitch_SwitchDisable_CreateHelper_Test_001, TestSize.Level0)
{
    LOG_INFO("SlientSwitch_SwitchDisable_CreateHelper_Test_001::Start");
    auto helper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID, SLIENT_ACCESS_URI);
    ASSERT_TRUE(helper != nullptr);

    Uri uri("");
    int retVal = DataShareHelper::SetSilentSwitch(uri, false);
    EXPECT_EQ(retVal, E_OK);

    helper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID, SLIENT_ACCESS_URI);
    ASSERT_TRUE(helper == nullptr);
    retVal = DataShareHelper::SetSilentSwitch(uri, true);
    EXPECT_EQ(retVal, E_OK);
    LOG_INFO("SlientSwitch_SwitchDisable_CreateHelper_Test_001::End");
}

Template GetTemplate()
{
    PredicateTemplateNode node1("p1", "select name0 as name from TBL00");
    PredicateTemplateNode node2("p2", "select name1 as name from TBL00");
    std::vector<PredicateTemplateNode> nodes;
    nodes.emplace_back(node1);
    nodes.emplace_back(node2);
    Template tpl(nodes, "select name1 as name from TBL00");
    return tpl;
}

HWTEST_F(SlientSwitchTest, SlientSwitch_SwitchDisable_CreateHelper_Test_002, TestSize.Level0)
{
    LOG_INFO("SlientSwitch_SwitchDisable_CreateHelper_Test_002::Start");
    Uri uri("");
    int retVal = DataShareHelper::SetSilentSwitch(uri, false);
    EXPECT_EQ(retVal, E_OK);

    auto helper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID, SLIENT_ACCESS_URI, DATA_SHARE_URI);
    ASSERT_TRUE(helper != nullptr);

    auto tpl = GetTemplate();
    auto result = helper->AddQueryTemplate(DATA_SHARE_PROXY_URI, SUBSCRIBER_ID, tpl);
    EXPECT_EQ(result, ERROR);
    retVal = DataShareHelper::SetSilentSwitch(uri, true);
    EXPECT_EQ(retVal, E_OK);
    LOG_INFO("SlientSwitch_SwitchDisable_CreateHelper_Test_002::End");
}

HWTEST_F(SlientSwitchTest, SlientSwitch_SwitchEnable_CreateHelper_Test_001, TestSize.Level0)
{
    LOG_INFO("SlientSwitch_SwitchEnable_CreateHelper_Test_001::Start");
    Uri uri("");
    int retVal = DataShareHelper::SetSilentSwitch(uri, true);
    EXPECT_EQ(retVal, E_OK);

    auto helper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID, SLIENT_ACCESS_URI);
    ASSERT_TRUE(helper != nullptr);

    auto tpl = GetTemplate();
    auto result = helper->AddQueryTemplate(DATA_SHARE_PROXY_URI, SUBSCRIBER_ID, tpl);
    EXPECT_EQ(result, E_BUNDLE_NAME_NOT_EXIST);
    LOG_INFO("SlientSwitch_SwitchEnable_CreateHelper_Test_001::End");
}

HWTEST_F(SlientSwitchTest, SlientSwitch_SwitchEnable_CreateHelper_Test_002, TestSize.Level0)
{
    LOG_INFO("SlientSwitch_SwitchEnable_CreateHelper_Test_002::Start");
    Uri uri("");
    int retVal = DataShareHelper::SetSilentSwitch(uri, true);
    EXPECT_EQ(retVal, E_OK);

    auto helper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID, SLIENT_ACCESS_URI, DATA_SHARE_URI);
    ASSERT_TRUE(helper != nullptr);

    auto tpl = GetTemplate();
    auto result = helper->AddQueryTemplate(DATA_SHARE_PROXY_URI, SUBSCRIBER_ID, tpl);
    EXPECT_EQ(result, E_BUNDLE_NAME_NOT_EXIST);
    LOG_INFO("SlientSwitch_SwitchEnable_CreateHelper_Test_002::End");
}
} // namespace DataShare
} // namespace OHOS