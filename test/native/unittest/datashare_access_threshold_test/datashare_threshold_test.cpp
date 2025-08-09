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
#include "datashare_helper.h"

#include <gtest/gtest.h>

#include "accesstoken_kit.h"
#include "data_ability_observer_stub.h"
#include "datashare_log.h"
#include "hap_token_info.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "token_setproc.h"

namespace OHOS {
namespace DataShare {
using namespace testing::ext;
using namespace OHOS::Security::AccessToken;
using ChangeInfo = DataShareObserver::ChangeInfo;
constexpr int STORAGE_MANAGER_MANAGER_ID = 5003;
std::string DATA_SHARE_URI = "datashare:///com.acts.datasharetest";
std::string SLIENT_ACCESS_URI = "datashareproxy://com.acts.datasharetest/test?Proxy=true";
std::string TBL_STU_NAME = "name";
std::string TBL_STU_AGE = "age";
std::shared_ptr<DataShare::DataShareHelper> g_exHelper;

class DataShareThresholdTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

class IDataShareAbilityObserverTest : public AAFwk::DataAbilityObserverStub {
public:
    explicit IDataShareAbilityObserverTest(int code)
    {
        code_ = code;
    }

    ~IDataShareAbilityObserverTest()
    {}

    void OnChange() {}

    int code_;
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


std::vector<PermissionStateFull> GetPermissionStateFulls()
{
    std::vector<PermissionStateFull> permissionStateFulls = {
        {
            .permissionName = "ohos.permission.WRITE_CONTACTS",
            .isGeneral = true,
            .resDeviceID = { "local" },
            .grantStatus = { PermissionState::PERMISSION_GRANTED },
            .grantFlags = { 1 }
        },
        {
            .permissionName = "ohos.permission.WRITE_CALL_LOG",
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
    };
    return permissionStateFulls;
}

void DataShareThresholdTest::SetUpTestCase(void)
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
        .appIDDesc = "ohos.datashareclienttest.demo"
    };
    auto permStateList = GetPermissionStateFulls();
    HapPolicyParams policy = {
        .apl = APL_NORMAL,
        .domain = "test.domain",
        .permList = {
            {
                .permissionName = "ohos.permission.test",
                .bundleName = "ohos.datashareclienttest.demo",
                .grantMode = 1,
                .availableLevel = APL_NORMAL,
                .label = "label",
                .labelId = 1,
                .description = "ohos.datashareclienttest.demo",
                .descriptionId = 1
            }
        },
        .permStateList = permStateList
    };
    AccessTokenKit::AllocHapToken(info, policy);
    auto testTokenId = Security::AccessToken::AccessTokenKit::GetHapTokenIDEx(
        info.userID, info.bundleName, info.instIndex);
    SetSelfTokenID(testTokenId.tokenIDEx);

    g_exHelper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID, DATA_SHARE_URI);
    ASSERT_TRUE(g_exHelper != nullptr);
    LOG_INFO("SetUpTestCase end");
}

void DataShareThresholdTest::TearDownTestCase(void)
{
    auto tokenId = AccessTokenKit::GetHapTokenIDEx(100, "ohos.datashareclienttest.demo", 0);
    AccessTokenKit::DeleteToken(tokenId.tokenIDEx);
    g_exHelper = nullptr;
}

void DataShareThresholdTest::SetUp(void) {}
void DataShareThresholdTest::TearDown(void) {}

/**
* @tc.name: Insert_Threshold_Test001
* @tc.desc: test silent access insert over threshold case
* @tc.type: FUNC
*/
HWTEST_F(DataShareThresholdTest, Insert_Threshold_Test001, TestSize.Level1)
{
    LOG_INFO("Insert_Threshold_Test001::Start");
    auto helper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID, SLIENT_ACCESS_URI);
    ASSERT_TRUE(helper != nullptr);

    Uri uri(SLIENT_ACCESS_URI);

    DataShare::DataShareValuesBucket valuesBucket;
    std::string value = "zhangsan";
    valuesBucket.Put(TBL_STU_NAME, value);
    int age = 18;
    valuesBucket.Put(TBL_STU_AGE, age);
    int retVal = helper->Insert(uri, valuesBucket);
    EXPECT_EQ((retVal > 0), true);

    DataShare::DataShareValuesBucket valuesBucket1;
    std::string value1 = "lisi";
    valuesBucket1.Put(TBL_STU_NAME, value1);
    age = 25;
    valuesBucket1.Put(TBL_STU_AGE, age);
    // not over threshold, insert success
    for (int i = 0; i < 2998; i++) {
        retVal = helper->Insert(uri, valuesBucket1);
        EXPECT_EQ((retVal > 0), true);
    }

    // over threshold, insert ret DATA_SHARE_ERROR(-1)
    for (int i = 0; i < 10; i++) {
        retVal = helper->Insert(uri, valuesBucket1);
        EXPECT_EQ(retVal, -1);
    }
    LOG_INFO("Insert_Threshold_Test001::End");
}

/**
* @tc.name: Update_Threshold_Test001
* @tc.desc: test silent access update over threshold case
* @tc.type: FUNC
*/
HWTEST_F(DataShareThresholdTest, Update_Threshold_Test001, TestSize.Level1)
{
    LOG_INFO("Update_Threshold_Test001::Start");
    auto helper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID, SLIENT_ACCESS_URI);
    ASSERT_TRUE(helper != nullptr);

    Uri uri(SLIENT_ACCESS_URI);

    DataShare::DataSharePredicates predicates;
    std::string selections = TBL_STU_NAME + " = 'zhangsan'";
    predicates.SetWhereClause(selections);

    DataShare::DataShareValuesBucket valuesBucket;
    valuesBucket.Put(TBL_STU_AGE, 10);
    // not over threshold, update success
    for (int i = 0; i < 2999; i++) {
        int retVal = helper->Update(uri, predicates, valuesBucket);
        EXPECT_EQ(retVal, 1);
    }

    // over threshold, update ret DATA_SHARE_ERROR(-1)
    for (int i = 0; i < 10; i++) {
        int retVal = helper->Update(uri, predicates, valuesBucket);
        EXPECT_EQ(retVal, -1);
    }
    LOG_INFO("Update_Threshold_Test001::End");
}

/**
* @tc.name: Query_Threshold_Test001
* @tc.desc: test silent access query over threshold case
* @tc.type: FUNC
*/
HWTEST_F(DataShareThresholdTest, Query_Threshold_Test001, TestSize.Level1)
{
    LOG_INFO("Query_Threshold_Test001::Start");
    auto helper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID, SLIENT_ACCESS_URI);
    ASSERT_TRUE(helper != nullptr);

    Uri uri(SLIENT_ACCESS_URI);

    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(TBL_STU_NAME, "zhangsan");
    vector<string> columns;
    int result = 0;

    // not over threshold, query success
    for (int i = 0; i < 2999; i++) {
        auto resultSet = helper->Query(uri, predicates, columns);
        ASSERT_NE(resultSet, nullptr);
        resultSet->GetRowCount(result);
        EXPECT_EQ(result, 1);
        resultSet->Close();
    }

    // over threshold, query ret DATA_SHARE_ERROR(-1)
    for (int i = 0; i < 10; i++) {
        auto resultSet = helper->Query(uri, predicates, columns);
        ASSERT_EQ(resultSet, nullptr);
    }
    LOG_INFO("Query_Threshold_Test001::End");
}

/**
* @tc.name: Delete_Threshold_Test001
* @tc.desc: test silent access delete delete over threshold case
* @tc.type: FUNC
*/
HWTEST_F(DataShareThresholdTest, Delete_Threshold_Test001, TestSize.Level1)
{
    LOG_INFO("Delete_Threshold_Test001::Start");
    auto helper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID, SLIENT_ACCESS_URI);
    ASSERT_TRUE(helper != nullptr);

    Uri uri(SLIENT_ACCESS_URI);

    DataShare::DataSharePredicates deletePredicates;
    std::string selections = TBL_STU_NAME + " = 'lisan'";
    deletePredicates.SetWhereClause(selections);

    // not over threshold, delete data not exists return 0
    for (int i = 0; i < 2999; i++) {
        int retVal = helper->Delete(uri, deletePredicates);
        EXPECT_EQ(retVal, 0);
    }

    // over threshold, delete ret -1
    for (int i = 0; i < 10; i++) {
        int retVal = helper->Delete(uri, deletePredicates);
        EXPECT_EQ(retVal, -1);
    }
    LOG_INFO("Delete_Threshold_Test001::End");
}

/**
* @tc.name: InsertEx_Threshold_Test001
* @tc.desc: test silent access insertEx over threshold case
* @tc.type: FUNC
*/
HWTEST_F(DataShareThresholdTest, InsertEx_Threshold_Test001, TestSize.Level1)
{
    LOG_INFO("InsertEx_Threshold_Test001::Start");

    auto helper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID, SLIENT_ACCESS_URI);
    ASSERT_TRUE(helper != nullptr);

    Uri uri(SLIENT_ACCESS_URI);
    DataShare::DataShareValuesBucket valuesBucket;
    std::string value = "lisi";
    valuesBucket.Put(TBL_STU_NAME, value);
    int age = 25;
    valuesBucket.Put(TBL_STU_AGE, age);
    // not over threshold, insertEx success
    for (int i = 0; i < 2999; i++) {
        auto [errCode, retVal] = helper->InsertEx(uri, valuesBucket);
        EXPECT_EQ(errCode, 0);
        EXPECT_EQ((retVal > 0), true);
    }

    // over threshold, insertEx ret pair(DATA_SHARE_ERROR, 0)
    for (int i = 0; i < 10; i++) {
        auto [errCode, retVal] = helper->InsertEx(uri, valuesBucket);
        EXPECT_EQ(errCode, -1);
        EXPECT_EQ(retVal, 0);
    }
    LOG_INFO("InsertEx_Threshold_Test001::End");
}

/**
* @tc.name: UpdateEx_Threshold_Test001
* @tc.desc: test silent access updateEx over threshold case
* @tc.type: FUNC
*/
HWTEST_F(DataShareThresholdTest, UpdateEx_Threshold_Test001, TestSize.Level1)
{
    LOG_INFO("UpdateEx_Threshold_Test001::Start");

    auto helper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID, SLIENT_ACCESS_URI);
    ASSERT_TRUE(helper != nullptr);

    Uri uri(SLIENT_ACCESS_URI);

    DataShare::DataSharePredicates predicates;
    std::string selections = TBL_STU_NAME + " = 'zhangsan'";
    predicates.SetWhereClause(selections);
    DataShare::DataShareValuesBucket valuesBucket;
    valuesBucket.Put(TBL_STU_AGE, 10);

    // not over threshold, updateEx success
    for (int i = 0; i < 2999; i++) {
        auto [errCode, retVal] = helper->UpdateEx(uri, predicates, valuesBucket);
        EXPECT_EQ(errCode, 0);
        EXPECT_EQ(retVal, 1);
    }

    // over threshold, updateEx ret pair(DATA_SHARE_ERROR, 0)
    for (int i = 0; i < 10; i++) {
        auto [errCode, retVal] = helper->UpdateEx(uri, predicates, valuesBucket);
        EXPECT_EQ(errCode, -1);
        EXPECT_EQ(retVal, 0);
    }
    LOG_INFO("UpdateEx_Threshold_Test001::End");
}

/**
* @tc.name: DeleteEx_Threshold_Test001
* @tc.desc: test silent access deleteEx over threshold case
* @tc.type: FUNC
*/
HWTEST_F(DataShareThresholdTest, DeleteEx_Threshold_Test001, TestSize.Level1)
{
    LOG_INFO("DeleteEx_Threshold_Test001::Start");

    auto helper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID, SLIENT_ACCESS_URI);
    ASSERT_TRUE(helper != nullptr);

    Uri uri(SLIENT_ACCESS_URI);

    DataShare::DataSharePredicates deletePredicates;
    std::string selections = TBL_STU_NAME + " = 'lisan'";
    deletePredicates.SetWhereClause(selections);
    // not over threshold, deleteEx data not exists return pair(0, 0)
    for (int i = 0; i < 2999; i++) {
        auto [errCode, retVal] = helper->DeleteEx(uri, deletePredicates);
        EXPECT_EQ(errCode, 0);
        EXPECT_EQ(retVal, 0);
    }

    // over threshold, deleteEx ret pair(DATA_SHARE_ERROR, 0)
    for (int i = 0; i < 10; i++) {
        auto [errCode, retVal] = helper->DeleteEx(uri, deletePredicates);
        EXPECT_EQ(errCode, -1);
        EXPECT_EQ(retVal, 0);
    }
    LOG_INFO("DeleteEx_Threshold_Test001::End");
}

/**
* @tc.name: Insert_Threshold_Test002
* @tc.desc: test non-silent acess insert over threshold case
* @tc.type: FUNC
*/
HWTEST_F(DataShareThresholdTest, Insert_Threshold_Test002, TestSize.Level1)
{
    LOG_INFO("Insert_Threshold_Test002::Start");

    Uri uri(DATA_SHARE_URI);
    DataShare::DataShareValuesBucket valuesBucket;
    std::string value = "lisi";
    valuesBucket.Put(TBL_STU_NAME, value);
    int age = 25;
    valuesBucket.Put(TBL_STU_AGE, age);
    // not over threshold, insert success
    for (int i = 0; i < 2999; i++) {
        int retVal = g_exHelper->Insert(uri, valuesBucket);
        EXPECT_EQ((retVal > 0), true);
    }

    // over threshold, insert ret success
    for (int i = 0; i < 10; i++) {
        int retVal = g_exHelper->Insert(uri, valuesBucket);
        EXPECT_EQ((retVal > 0), true);
    }
    LOG_INFO("Insert_Threshold_Test002::End");
}

/**
* @tc.name: Update_Threshold_Test002
* @tc.desc: test non-silent access update over threshold case
* @tc.type: FUNC
*/
HWTEST_F(DataShareThresholdTest, Update_Threshold_Test002, TestSize.Level1)
{
    LOG_INFO("Update_Threshold_Test002::Start");

    Uri uri(DATA_SHARE_URI);

    DataShare::DataSharePredicates predicates;
    std::string selections = TBL_STU_NAME + " = 'zhangsan'";
    predicates.SetWhereClause(selections);
    DataShare::DataShareValuesBucket valuesBucket;
    valuesBucket.Put(TBL_STU_AGE, 10);
    // not over threshold, update success
    for (int i = 0; i < 2999; i++) {
        int retVal = g_exHelper->Update(uri, predicates, valuesBucket);
        EXPECT_EQ(retVal, 1);
    }

    // over threshold, update success
    for (int i = 0; i < 10; i++) {
        int retVal = g_exHelper->Update(uri, predicates, valuesBucket);
        EXPECT_EQ(retVal, 1);
    }
    LOG_INFO("Update_Threshold_Test002::End");
}

/**
* @tc.name: Query_Threshold_Test002
* @tc.desc: test non-silent access query over threshold case
* @tc.type: FUNC
*/
HWTEST_F(DataShareThresholdTest, Query_Threshold_Test002, TestSize.Level1)
{
    LOG_INFO("Query_Threshold_Test002::Start");

    Uri uri(DATA_SHARE_URI);
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(TBL_STU_NAME, "zhangsan");
    vector<string> columns;
    int result = 0;

    // not over threshold, query success
    for (int i = 0; i < 2999; i++) {
        auto resultSet = g_exHelper->Query(uri, predicates, columns);
        ASSERT_NE(resultSet, nullptr);
        resultSet->GetRowCount(result);
        EXPECT_EQ(result, 1);
        resultSet->Close();
    }

    // over threshold, query success
    for (int i = 0; i < 10; i++) {
        auto resultSet = g_exHelper->Query(uri, predicates, columns);
        ASSERT_NE(resultSet, nullptr);
        resultSet->GetRowCount(result);
        EXPECT_EQ(result, 1);
        resultSet->Close();
    }
    LOG_INFO("Query_Threshold_Test002::End");
}

/**
* @tc.name: Delete_Threshold_Test002
* @tc.desc: test non-silent accessdelete over threshold case
* @tc.type: FUNC
*/
HWTEST_F(DataShareThresholdTest, Delete_Threshold_Test002, TestSize.Level1)
{
    LOG_INFO("Delete_Threshold_Test002::Start");

    Uri uri(DATA_SHARE_URI);

    DataShare::DataSharePredicates deletePredicates;
    std::string selections = TBL_STU_NAME + " = 'lisan'";
    deletePredicates.SetWhereClause(selections);

    // not over threshold, delete data not exists return 0
    for (int i = 0; i < 2999; i++) {
        int retVal = g_exHelper->Delete(uri, deletePredicates);
        EXPECT_EQ(retVal, 0);
    }

    // over threshold, delete data not exists return 0
    for (int i = 0; i < 10; i++) {
        int retVal = g_exHelper->Delete(uri, deletePredicates);
        EXPECT_EQ(retVal, 0);
    }
    LOG_INFO("Delete_Threshold_Test002::End");
}

/**
* @tc.name: InsertEx_Threshold_Test002
* @tc.desc: test non-silent access insertEx over threshold case
* @tc.type: FUNC
*/
HWTEST_F(DataShareThresholdTest, InsertEx_Threshold_Test002, TestSize.Level1)
{
    LOG_INFO("InsertEx_Threshold_Test002::Start");

    Uri uri(DATA_SHARE_URI);
    DataShare::DataShareValuesBucket valuesBucket;
    std::string value = "lisi";
    valuesBucket.Put(TBL_STU_NAME, value);
    int age = 25;
    valuesBucket.Put(TBL_STU_AGE, age);
    // not over threshold, insertEx success
    for (int i = 0; i < 2999; i++) {
        auto [errCode, retVal] = g_exHelper->InsertEx(uri, valuesBucket);
        EXPECT_EQ(errCode, 0);
        EXPECT_EQ((retVal > 0), true);
    }

    // over threshold, insertEx success
    for (int i = 0; i < 10; i++) {
        auto [errCode, retVal] = g_exHelper->InsertEx(uri, valuesBucket);
        EXPECT_EQ(errCode, 0);
        EXPECT_EQ((retVal > 0), true);
    }
    LOG_INFO("InsertEx_Threshold_Test002::End");
}

/**
* @tc.name: UpdateEx_Threshold_Test002
* @tc.desc: test non-silent access updateEx over threshold case
* @tc.type: FUNC
*/
HWTEST_F(DataShareThresholdTest, UpdateEx_Threshold_Test002, TestSize.Level1)
{
    LOG_INFO("UpdateEx_Threshold_Test002::Start");

    Uri uri(DATA_SHARE_URI);

    DataShare::DataSharePredicates predicates;
    std::string selections = TBL_STU_NAME + " = 'zhangsan'";
    predicates.SetWhereClause(selections);
    DataShare::DataShareValuesBucket valuesBucket;
    valuesBucket.Put(TBL_STU_AGE, 10);
    // not over threshold, updateEx sccuess
    for (int i = 0; i < 2999; i++) {
        auto [errCode, retVal] = g_exHelper->UpdateEx(uri, predicates, valuesBucket);
        EXPECT_EQ(errCode, 0);
        EXPECT_EQ(retVal, 1);
    }

    // over threshold, updateEx ret success
    for (int i = 0; i < 10; i++) {
        auto [errCode, retVal] = g_exHelper->UpdateEx(uri, predicates, valuesBucket);
        EXPECT_EQ(errCode, 0);
        EXPECT_EQ(retVal, 1);
    }
    LOG_INFO("UpdateEx_Threshold_Test002::End");
}

/**
* @tc.name: DeleteEx_Threshold_Test002
* @tc.desc: test non-silent access deleteEx over threshold case
* @tc.type: FUNC
*/
HWTEST_F(DataShareThresholdTest, DeleteEx_Threshold_Test002, TestSize.Level1)
{
    LOG_INFO("DeleteEx_Threshold_Test002::Start");

    Uri uri(DATA_SHARE_URI);

    DataShare::DataSharePredicates deletePredicates;
    std::string selections = TBL_STU_NAME + " = 'lisan'";
    deletePredicates.SetWhereClause(selections);
    // not over threshold, deleteEx data not exists return pair(0, 0)
    for (int i = 0; i < 2999; i++) {
        auto [errCode, retVal] = g_exHelper->DeleteEx(uri, deletePredicates);
        EXPECT_EQ(errCode, 0);
        EXPECT_EQ(retVal, 0);
    }

    // over threshold, deleteEx data not exists return pair(0, 0)
    for (int i = 0; i < 10; i++) {
        auto [errCode, retVal] = g_exHelper->DeleteEx(uri, deletePredicates);
        EXPECT_EQ(errCode, 0);
        EXPECT_EQ(retVal, 0);
    }
    LOG_INFO("DeleteEx_Threshold_Test002::End");
}
}
}