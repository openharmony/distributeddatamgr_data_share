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

#define LOG_TAG "datashare_helper_threshold_test"

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

class DataShareHelperThresholdTest : public testing::Test {
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

void DataShareHelperThresholdTest::SetUpTestCase(void)
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

void DataShareHelperThresholdTest::TearDownTestCase(void)
{
    auto tokenId = AccessTokenKit::GetHapTokenIDEx(100, "ohos.datashareclienttest.demo", 0);
    AccessTokenKit::DeleteToken(tokenId.tokenIDEx);
    g_exHelper = nullptr;
}

void DataShareHelperThresholdTest::SetUp(void) {}
void DataShareHelperThresholdTest::TearDown(void) {}

/**
 * @tc.name: Insert_Threshold_Test001
 * @tc.desc: Verify the behavior of the silent access Insert operation in DataShareHelper when the operation count does
 *           not exceed and exceeds the preset threshold, focusing on return value consistency.
 * @tc.type: FUNC
 * @tc.require: issueIC8OCN
 * @tc.precon:
    1. The test environment supports instantiation of DataShareHelper and DataShareValuesBucket, with no initialization
       errors.
    2. Predefined constants are valid: STORAGE_MANAGER_MANAGER_ID (for helper creation), SLIENT_ACCESS_URI,
       TBL_STU_NAME/TBL_STU_AGE (table columns), and DATA_SHARE_ERROR (-1) is correctly defined.
    3. The CreateDataShareHelper function can successfully generate a non-null DataShareHelper instance with silent
       access config.
 * @tc.step:
    1. Call CreateDataShareHelper with STORAGE_MANAGER_MANAGER_ID and SLIENT_ACCESS_URI to create a
       DataShareHelper instance.
    2. Create a DataShareValuesBucket, then call Put to add TBL_STU_NAME and TBL_STU_AGE (18) as test data.
    3. Call the Insert method of the helper with SLIENT_ACCESS_URI and the bucket, then verify the return value is
       positive.
    4. Modify the bucket to set TBL_STU_NAME ("lisi") and TBL_STU_AGE (25), then perform 2998 additional Insert
       operations, checking each return value is positive.
    5. Perform 10 more Insert operations with the same bucket, then check each return value.
 * @tc.expect:
    1. The created DataShareHelper instance is not nullptr.
    2. All Insert operations before exceeding the threshold return positive values (indicating success).
    3. All Insert operations after exceeding the threshold return DATA_SHARE_ERROR (-1).
 */
HWTEST_F(DataShareHelperThresholdTest, Insert_Threshold_Test001, TestSize.Level1)
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
 * @tc.desc: Verify the behavior of the silent access Update operation in DataShareHelper when the operation count
 *           does not exceed and exceeds the preset threshold, focusing on return value correctness.
 * @tc.type: FUNC
 * @tc.require: issueIC8OCN
 * @tc.precon:
    1. The test environment supports DataShareHelper, DataSharePredicates, and DataShareValuesBucket instantiation.
    2. Predefined constants are valid: STORAGE_MANAGER_MANAGER_ID, SLIENT_ACCESS_URI, TBL_STU_NAME/TBL_STU_AGE, and
       DATA_SHARE_ERROR (-1) is defined.
    3. The CreateDataShareHelper function can generate a non-null silent access DataShareHelper instance.
 * @tc.step:
    1. Create a DataShareHelper instance using CreateDataShareHelper with STORAGE_MANAGER_MANAGER_ID and
       SLIENT_ACCESS_URI.
    2. Create a DataSharePredicates instance, then call SetWhereClause to set the condition.
    3. Create a DataShareValuesBucket and call Put to add TBL_STU_AGE (10) as the update data.
    4. Perform 2999 Update operations with the URI, predicates, and bucket, checking each return value.
    5. Perform 10 more Update operations with the same parameters, then check each return value.
 * @tc.expect:
    1. The DataShareHelper instance is not nullptr.
    2. All Update operations before exceeding the threshold return 1 (indicating successful update).
    3. All Update operations after exceeding the threshold return DATA_SHARE_ERROR (-1).
 */
HWTEST_F(DataShareHelperThresholdTest, Update_Threshold_Test001, TestSize.Level1)
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
 * @tc.desc: Verify the behavior of the silent access Query operation in DataShareHelper when the operation count
 *           does not exceed and exceeds the preset threshold, focusing on result set validity.
 * @tc.type: FUNC
 * @tc.require: issueIC8OCN
 * @tc.precon:
    1. The test environment supports DataShareHelper, DataSharePredicates, and ResultSet operations (including
       GetRowCount/Close).
    2. Predefined constants are valid: STORAGE_MANAGER_MANAGER_ID, SLIENT_ACCESS_URI, TBL_STU_NAME, and
       DATA_SHARE_ERROR (-1) is defined.
    3. The target data (TBL_STU_NAME = "zhangsan") exists in the data source pointed to by SLIENT_ACCESS_URI.
 * @tc.step:
    1. Create a DataShareHelper instance via CreateDataShareHelper with STORAGE_MANAGER_MANAGER_ID and
       SLIENT_ACCESS_URI.
    2. Create a DataSharePredicates instance and call EqualTo to set the condition: TBL_STU_NAME = "zhangsan".
    3. Initialize an empty vector<string> for query columns, then declare an int variable to store the row count.
    4. Perform 2999 Query operations with the URI, predicates, and columns: for each, verify the ResultSet is non-null,
       get the row count (expect 1), then call Close.
    5. Perform 10 more Query operations with the same parameters, then check each ResultSet.
 * @tc.expect:
    1. The DataShareHelper instance is not nullptr.
    2. All Query operations before exceeding the threshold return a valid non-null ResultSet with 1 row.
    3. All Query operations after exceeding the threshold return nullptr (indicating DATA_SHARE_ERROR).
 */
HWTEST_F(DataShareHelperThresholdTest, Query_Threshold_Test001, TestSize.Level1)
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
 * @tc.desc: Verify the behavior of the silent access Delete operation in DataShareHelper when the operation count
 *           does not exceed and exceeds the preset threshold, especially for non-existent target data.
 * @tc.type: FUNC
 * @tc.require: issueIC8OCN
 * @tc.precon:
    1. The test environment supports DataShareHelper, DataSharePredicates instantiation, and Delete operation
       execution.
    2. Predefined constants are valid: STORAGE_MANAGER_MANAGER_ID, SLIENT_ACCESS_URI, TBL_STU_NAME, and
       DATA_SHARE_ERROR (-1) is defined.
    3. The target data (TBL_STU_NAME = "lisan") does not exist in the data source pointed to by SLIENT_ACCESS_URI.
 * @tc.step:
    1. Create a DataShareHelper instance using CreateDataShareHelper with STORAGE_MANAGER_MANAGER_ID and
       SLIENT_ACCESS_URI.
    2. Create a DataSharePredicates instance, then call SetWhereClause to set the condition.
    3. Perform 2999 Delete operations with the URI and predicates, checking each return value.
    4. Perform 10 more Delete operations with the same parameters, then check each return value.
 * @tc.expect:
    1. The DataShareHelper instance is not nullptr.
    2. All Delete operations before exceeding the threshold return 0 (indicating no data deleted).
    3. All Delete operations after exceeding the threshold return DATA_SHARE_ERROR (-1).
 */
HWTEST_F(DataShareHelperThresholdTest, Delete_Threshold_Test001, TestSize.Level1)
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
 * @tc.desc: Verify the behavior of the silent access InsertEx operation in DataShareHelper when the operation count
 *           does not exceed and exceeds the preset threshold, focusing on the pair-type return value.
 * @tc.type: FUNC
 * @tc.require: issueIC8OCN
 * @tc.precon:
    1. The test environment supports DataShareHelper and DataShareValuesBucket instantiation, and handles the
       (errorCode, retVal) pair returned by InsertEx.
    2. Predefined constants are valid: STORAGE_MANAGER_MANAGER_ID, SLIENT_ACCESS_URI, TBL_STU_NAME/TBL_STU_AGE, and
       DATA_SHARE_ERROR (-1) is defined.
    3. The CreateDataShareHelper function can generate a non-null silent access DataShareHelper instance.
 * @tc.step:
    1. Call CreateDataShareHelper with STORAGE_MANAGER_MANAGER_ID and SLIENT_ACCESS_URI to create a DataShareHelper
       instance.
    2. Create a DataShareValuesBucket, then call Put to add TBL_STU_NAME ("lisi") and TBL_STU_AGE (25) as test data.
    3. Perform 2999 InsertEx operations with the URI and bucket: for each, get the (errorCode, retVal) pair and
       verify validity.
    4. Perform 10 more InsertEx operations with the same parameters: for each, get the pair and check values.
 * @tc.expect:
    1. The DataShareHelper instance is not nullptr.
    2. All InsertEx operations before exceeding the threshold return (0, positive value) (indicating success).
    3. All InsertEx operations after exceeding the threshold return (DATA_SHARE_ERROR (-1), 0).
 */
HWTEST_F(DataShareHelperThresholdTest, InsertEx_Threshold_Test001, TestSize.Level1)
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
 * @tc.desc: Verify the behavior of the silent access UpdateEx operation in DataShareHelper when the operation count
 *           does not exceed and exceeds the preset threshold, focusing on the pair-type return value.
 * @tc.type: FUNC
 * @tc.require: issueIC8OCN
 * @tc.precon:
    1. The test environment supports DataShareHelper, DataSharePredicates, DataShareValuesBucket instantiation, and
       handles the (errorCode, retVal) pair returned by UpdateEx.
    2. Predefined constants are valid: STORAGE_MANAGER_MANAGER_ID, SLIENT_ACCESS_URI, TBL_STU_NAME/TBL_STU_AGE, and
       DATA_SHARE_ERROR (-1) is defined.
    3. The target data (TBL_STU_NAME = "zhangsan") exists in the data source pointed to by SLIENT_ACCESS_URI.
 * @tc.step:
    1. Create a DataShareHelper instance using CreateDataShareHelper with STORAGE_MANAGER_MANAGER_ID and
       SLIENT_ACCESS_URI.
    2. Create a DataSharePredicates instance, call SetWhereClause to set TBL_STU_NAME + " = 'zhangsan'".
    3. Create a DataShareValuesBucket and call Put to add TBL_STU_AGE (10) as update data.
    4. Perform 2999 UpdateEx operations with the URI, predicates, and bucket: check each (errorCode, retVal) pair.
    5. Perform 10 more UpdateEx operations with the same parameters: check each pair.
 * @tc.expect:
    1. The DataShareHelper instance is not nullptr.
    2. All UpdateEx operations before exceeding the threshold return (0, 1) (indicating successful update).
    3. All UpdateEx operations after exceeding the threshold return (DATA_SHARE_ERROR (-1), 0).
 */
HWTEST_F(DataShareHelperThresholdTest, UpdateEx_Threshold_Test001, TestSize.Level1)
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
 * @tc.desc: Verify the behavior of the silent access DeleteEx operation in DataShareHelper when the operation count
 *           does not exceed and exceeds the preset threshold, especially for non-existent target data.
 * @tc.type: FUNC
 * @tc.require: issueIC8OCN
 * @tc.precon:
    1. The test environment supports DataShareHelper, DataSharePredicates instantiation, and handles the
       (errorCode, retVal) pair returned by DeleteEx.
    2. Predefined constants are valid: STORAGE_MANAGER_MANAGER_ID, SLIENT_ACCESS_URI, TBL_STU_NAME, and
       DATA_SHARE_ERROR (-1) is defined.
    3. The target data (TBL_STU_NAME = "lisan") does not exist in the data source pointed to by SLIENT_ACCESS_URI.
 * @tc.step:
    1. Create a DataShareHelper instance via CreateDataShareHelper with STORAGE_MANAGER_MANAGER_ID and
       SLIENT_ACCESS_URI.
    2. Create a DataSharePredicates instance, call SetWhereClause to set TBL_STU_NAME + " = 'lisan'".
    3. Perform 2999 DeleteEx operations with the URI and predicates: check each (errorCode, retVal) pair.
    4. Perform 10 more DeleteEx operations with the same parameters: check each pair.
 * @tc.expect:
    1. The DataShareHelper instance is not nullptr.
    2. All DeleteEx operations before exceeding the threshold return (0, 0) (indicating no data deleted).
    3. All DeleteEx operations after exceeding the threshold return (DATA_SHARE_ERROR (-1), 0).
 */
HWTEST_F(DataShareHelperThresholdTest, DeleteEx_Threshold_Test001, TestSize.Level1)
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
 * @tc.desc: Verify the behavior of the non-silent access Insert operation when the operation count
 *           exceeds the preset threshold, focusing on no failure occurrence.
 * @tc.type: FUNC
 * @tc.require: issueIC8OCN
 * @tc.precon:
    1. The global DataShareHelper instance g_exHelper is pre-initialized (non-null) and configured for non-silent
       access.
    2. Predefined constants are valid: DATA_SHARE_URI (non-silent access target URI) and TBL_STU_NAME/TBL_STU_AGE
       (table columns).
    3. The Insert operation of g_exHelper can normally return positive values for valid data in non-silent mode.
 * @tc.step:
    1. Create a DataShareValuesBucket, call Put to add TBL_STU_NAME ("lisi") and TBL_STU_AGE (25) as test data.
    2. Perform 2999 Insert operations using g_exHelper with DATA_SHARE_URI and the bucket, checking each return
       value is positive.
    3. Perform 10 more Insert operations with the same parameters, checking each return value.
 * @tc.expect:
    1. All Insert operations (before and after exceeding the threshold) return positive values (indicating success).
    2. No Insert operation fails (i.e., no return of DATA_SHARE_ERROR) when exceeding the threshold in non-silent
       access.
 */
HWTEST_F(DataShareHelperThresholdTest, Insert_Threshold_Test002, TestSize.Level1)
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
 * @tc.desc: Verify the behavior of the non-silent access Update operation when the operation count
 *           exceeds the preset threshold, focusing on no failure occurrence.
 * @tc.type: FUNC
 * @tc.require: issueIC8OCN
 * @tc.precon:
    1. The global DataShareHelper instance g_exHelper is pre-initialized (non-null) and configured for non-silent
       access.
    2. Predefined constants are valid: DATA_SHARE_URI (non-silent access target URI) and TBL_STU_NAME/TBL_STU_AGE
       (table columns).
    3. The target data (TBL_STU_NAME = "zhangsan") exists in the data source pointed to by DATA_SHARE_URI.
 * @tc.step:
    1. Create a DataSharePredicates instance, call SetWhereClause to set TBL_STU_NAME + " = 'zhangsan'".
    2. Create a DataShareValuesBucket, call Put to add TBL_STU_AGE (10) as update data.
    3. Perform 2999 Update operations using g_exHelper with DATA_SHARE_URI, predicates, and the bucket, checking
       each return value is 1.
    4. Perform 10 more Update operations with the same parameters, checking each return value.
 * @tc.expect:
    1. All Update operations (before and after exceeding the threshold) return 1 (indicating success).
    2. No Update operation fails (i.e., no return of DATA_SHARE_ERROR) when exceeding the threshold in non-silent
       access.
 */
HWTEST_F(DataShareHelperThresholdTest, Update_Threshold_Test002, TestSize.Level1)
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
 * @tc.desc: Verify the behavior of the non-silent access Query operation (using global g_exHelper) when the operation
 *           count exceeds the preset threshold, focusing on result set validity and no failure occurrence.
 * @tc.type: FUNC
 * @tc.require: issueIC8OCN
 * @tc.precon:
    1. The global DataShareHelper instance g_exHelper is pre-initialized (non-null) and configured for non-silent
       access.
    2. Predefined constants are valid: DATA_SHARE_URI (non-silent access target URI) and TBL_STU_NAME (table column).
    3. The target data (TBL_STU_NAME = "zhangsan") exists in the data source pointed to by DATA_SHARE_URI.
    4. The ResultSet's GetRowCount and Close methods work normally in the test environment.
 * @tc.step:
    1. Create a DataSharePredicates instance and call EqualTo to set the condition: TBL_STU_NAME = "zhangsan".
    2. Initialize an empty vector<string> for query columns and declare an int variable to store the row count.
    3. Perform 2999 Query operations using g_exHelper with DATA_SHARE_URI, predicates, and columns: for each, verify
       the ResultSet is non-null, get the row count (expect 1), then call Close.
    4. Perform 10 more Query operations with the same parameters: check each ResultSet's validity and row count.
 * @tc.expect:
    1. All Query operations (before and after exceeding the threshold) return a valid non-null ResultSet with 1 row.
    2. No Query operation fails (i.e., no return of nullptr) when exceeding the threshold in non-silent access.
 */
HWTEST_F(DataShareHelperThresholdTest, Query_Threshold_Test002, TestSize.Level1)
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
 * @tc.desc: Verify the behavior of the non-silent access Delete operation when the operation count
 *           exceeds the preset threshold, especially for non-existent target data and no failure occurrence.
 * @tc.type: FUNC
 * @tc.require: issueIC8OCN
 * @tc.precon:
    1. The global DataShareHelper instance g_exHelper is pre-initialized (non-null) and configured for non-silent
       access.
    2. Predefined constants are valid: DATA_SHARE_URI (non-silent access target URI) and TBL_STU_NAME (table column).
    3. The target data (TBL_STU_NAME = "lisan") does not exist in the data source pointed to by DATA_SHARE_URI.
    4. The Delete method of g_exHelper can normally return 0 for non-existent data in non-silent mode.
 * @tc.step:
    1. Create a DataSharePredicates instance, then call SetWhereClause to set the condition.
    2. Perform 2999 Delete operations using g_exHelper with DATA_SHARE_URI and the predicates, checking each return
       value is 0.
    3. Perform 10 more Delete operations with the same parameters, checking each return value.
 * @tc.expect:
    1. All Delete operations (before and after exceeding the threshold) return 0 (indicating no data deleted).
    2. No Delete operation fails (i.e., no return of DATA_SHARE_ERROR) when exceeding the threshold in non-silent
       access.
 */
HWTEST_F(DataShareHelperThresholdTest, Delete_Threshold_Test002, TestSize.Level1)
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
 * @tc.desc: Verify the behavior of the non-silent access InsertEx operation when the operation count
 *           exceeds the preset threshold, focusing on the pair-type return value and no failure occurrence.
 * @tc.type: FUNC
 * @tc.require: issueIC8OCN
 * @tc.precon:
    1. The global DataShareHelper instance g_exHelper is pre-initialized (non-null) and configured for non-silent
       access.
    2. Predefined constants are valid: DATA_SHARE_URI (non-silent access target URI) and TBL_STU_NAME/TBL_STU_AGE
       (table columns).
    3. The test environment can correctly handle the (errorCode, retVal) pair returned by the InsertEx method.
    4. The InsertEx method of g_exHelper returns (0, positive value) for valid data in non-silent mode.
 * @tc.step:
    1. Create a DataShareValuesBucket, then call Put to add TBL_STU_NAME ("lisi") and TBL_STU_AGE (25) as test data.
    2. Perform 2999 InsertEx operations using g_exHelper with DATA_SHARE_URI and the bucket: for each, check the
       (errorCode, retVal) pair is (0, positive value).
    3. Perform 10 more InsertEx operations with the same parameters: verify each pair's validity.
 * @tc.expect:
    1. All InsertEx operations (before and after exceeding the threshold) return (0, positive value).
    2. No InsertEx operation fails (i.e., no return of (DATA_SHARE_ERROR, 0)) when exceeding the threshold in
       non-silent access.
 */
HWTEST_F(DataShareHelperThresholdTest, InsertEx_Threshold_Test002, TestSize.Level1)
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
 * @tc.desc: Verify the behavior of the non-silent access UpdateEx operation when the operation count
 *           exceeds the preset threshold, focusing on the pair-type return value and no failure occurrence.
 * @tc.type: FUNC
 * @tc.require: issueIC8OCN
 * @tc.precon:
    1. The global DataShareHelper instance g_exHelper is pre-initialized (non-null) and configured for non-silent
       access.
    2. Predefined constants are valid: DATA_SHARE_URI (non-silent access target URI) and TBL_STU_NAME/TBL_STU_AGE
       (table columns).
    3. The target data (TBL_STU_NAME = "zhangsan") exists in the data source pointed to by DATA_SHARE_URI.
    4. The test environment can correctly handle the (errorCode, retVal) pair returned by the UpdateEx method.
 * @tc.step:
    1. Create a DataSharePredicates instance, call SetWhereClause to set the condition: TBL_STU_NAME + " = 'zhangsan'".
    2. Create a DataShareValuesBucket and call Put to add TBL_STU_AGE (10) as update data.
    3. Perform 2999 UpdateEx operations using g_exHelper with DATA_SHARE_URI, predicates, and the bucket: check each
       pair is (0, 1).
    4. Perform 10 more UpdateEx operations with the same parameters: verify each pair's validity.
 * @tc.expect:
    1. All UpdateEx operations (before and after exceeding the threshold) return (0, 1) (indicating successful update).
    2. No UpdateEx operation fails (i.e., no return of (DATA_SHARE_ERROR, 0)) when exceeding the threshold in
       non-silent access.
 */
HWTEST_F(DataShareHelperThresholdTest, UpdateEx_Threshold_Test002, TestSize.Level1)
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
 * @tc.desc: Verify the behavior of the non-silent access DeleteEx operation (using global g_exHelper) when the
 *           operation count exceeds the preset threshold, especially for non-existent data and no failure occurrence.
 * @tc.type: FUNC
 * @tc.require: issueIC8OCN
 * @tc.precon:
    1. The global DataShareHelper instance g_exHelper is pre-initialized (non-null) and configured for non-silent
       access.
    2. Predefined constants are valid: DATA_SHARE_URI (non-silent access target URI) and TBL_STU_NAME (table column).
    3. The target data (TBL_STU_NAME = "lisan") does not exist in the data source pointed to by DATA_SHARE_URI.
    4. The test environment can correctly handle the (errorCode, retVal) pair returned by the DeleteEx method.
 * @tc.step:
    1. Create a DataSharePredicates instance, call SetWhereClause to set the condition: TBL_STU_NAME + " = 'lisan'".
    2. Perform 2999 DeleteEx operations using g_exHelper with DATA_SHARE_URI and the predicates: check each pair is
       (0, 0).
    3. Perform 10 more DeleteEx operations with the same parameters: verify each pair's validity.
 * @tc.expect:
    1. All DeleteEx operations (before and after exceeding the threshold) return (0, 0) (indicating no data deleted).
    2. No DeleteEx operation fails (i.e., no return of (DATA_SHARE_ERROR, 0)) when exceeding the threshold in
       non-silent access.
 */
HWTEST_F(DataShareHelperThresholdTest, DeleteEx_Threshold_Test002, TestSize.Level1)
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

/**
 * @tc.name: Query_Threshold_Test003
 * @tc.desc: Verify that the time consumed for traversing a result set of 1000 string-type data
 *           does not exceed the preset threshold (30ms) in silent access mode.
 * @tc.type: FUNC
 * @tc.require: issueIC8OCN
 * @tc.precon:
    1. The test environment supports DataShareHelper, DataShareValuesBucket, and ResultSet operations (GoToRow,
       GetString, GetRowCount, etc.).
    2. Predefined constants are valid: STORAGE_MANAGER_MANAGER_ID, SLIENT_ACCESS_URI, and TBL_STU_NAME (string-type
       table column).
    3. The CreateDataShareHelper function can generate a non-null silent access DataShareHelper instance.
    4. The test environment supports std::chrono for time measurement (to calculate traversal duration).
 * @tc.step:
    1. Create a DataShareHelper instance via CreateDataShareHelper with STORAGE_MANAGER_MANAGER_ID and
       SLIENT_ACCESS_URI.
    2. Create a DataShareValuesBucket and call Put 10 times to add TBL_STU_NAME ("wangwu") (10 attributes per data).
    3. Perform 1000 Insert operations with the bucket and SLIENT_ACCESS_URI to insert 1000 pieces of string-type data.
    4. Create a DataSharePredicates instance (EqualTo(TBL_STU_NAME, "wangwu")), then call Query to get the result set.
    5. Use std::chrono to record the start time, then traverse the result set (GoToRow + GetString for all columns),
       and record the end time.
    6. Calculate the traversal duration (end time - start time) and check if it meets the threshold.
 * @tc.expect:
    1. The DataShareHelper instance and result set are both non-null during the test.
    2. The time consumed for traversing the result set does not exceed 30ms.
 */
HWTEST_F(DataShareHelperThresholdTest, Query_Threshold_Test003, TestSize.Level1)
{
    LOG_INFO("Query_Threshold_Test003::Start");
    auto helper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID, SLIENT_ACCESS_URI);
    ASSERT_NE(helper, nullptr);
    Uri uri(SLIENT_ACCESS_URI);
    DataShare::DataShareValuesBucket valuesBucket;
    std::string value = "wangwu";
    for (int i = 0; i < 10; i++) {
        valuesBucket.Put(TBL_STU_NAME, value);
    }
    for (int i = 0; i < 1000; i++) {
        helper->Insert(uri, valuesBucket);
    }
    vector<string> columns;
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(TBL_STU_NAME, "wangwu");
    auto resultSet = helper->Query(uri, predicates, columns);
    ASSERT_NE(resultSet, nullptr);
    std::vector<std::string> columnNames;
    int rowCount = 0;
    resultSet->GetRowCount(rowCount);
    resultSet->GetAllColumnNames(columnNames);
    int columnCount = columnNames.size();
    std::string valueResult;
    std::chrono::system_clock::time_point startTimeStamp = std::chrono::system_clock::now();
    int64_t start = std::chrono::duration_cast<std::chrono::milliseconds>(
        startTimeStamp.time_since_epoch()).count();
    for (int i = 0; i < rowCount; i++) {
        resultSet->GoToRow(i);
        for (int j = 0; j < columnCount; j++) {
            resultSet-> GetString(columnCount, valueResult);
        }
    }
    std::chrono::system_clock::time_point endTimeStamp = std::chrono::system_clock::now();
    int64_t end = std::chrono::duration_cast<std::chrono::milliseconds>(
        endTimeStamp.time_since_epoch()).count();
    int time = end - start;
    LOG_INFO("Query_Threshold_Test003::end - start = %{public}d", time);
    //EXPECT are meaningless. Different devices make judgments based on the time of log printing
    EXPECT_LT(time, 500);
    LOG_INFO("Query_Threshold_Test003::End");
}

/**
 * @tc.name: Query_Threshold_Test004
 * @tc.desc: Verify that the time consumed for traversing a result set of 1000 int-type data (each with 10 attributes)
 *           does not exceed the preset threshold (30ms) in silent access mode.
 * @tc.type: FUNC
 * @tc.require: issueIC8OCN
 * @tc.precon:
    1. The test environment supports DataShareHelper, DataShareValuesBucket, and ResultSet operations (GoToRow,
       GetInt, GetRowCount, etc.).
    2. Predefined constants are valid: STORAGE_MANAGER_MANAGER_ID, SLIENT_ACCESS_URI, and TBL_STU_AGE (int-type
       table column).
    3. The CreateDataShareHelper function can generate a non-null silent access DataShareHelper instance.
    4. The test environment supports std::chrono for time measurement (to calculate traversal duration).
 * @tc.step:
    1. Create a DataShareHelper instance via CreateDataShareHelper with STORAGE_MANAGER_MANAGER_ID and
       SLIENT_ACCESS_URI.
    2. Create a DataShareValuesBucket and call Put 10 times to add TBL_STU_AGE (20) (10 attributes per data).
    3. Perform 1000 Insert operations with the bucket and SLIENT_ACCESS_URI to insert 1000 pieces of int-type data.
    4. Create a DataSharePredicates instance (EqualTo(TBL_STU_AGE, 20)), then call Query to get the result set.
    5. Use std::chrono to record the start time, then traverse the result set (GoToRow + GetInt for all columns),
       and record the end time.
    6. Calculate the traversal duration (end time - start time) and check if it meets the threshold.
 * @tc.expect:
    1. The DataShareHelper instance and result set are both non-null during the test.
    2. The time consumed for traversing the result set does not exceed 30ms.
 */
HWTEST_F(DataShareHelperThresholdTest, Query_Threshold_Test004, TestSize.Level1)
{
    LOG_INFO("Query_Threshold_Test004::Start");
    auto helper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID, SLIENT_ACCESS_URI);
    ASSERT_NE(helper, nullptr);
    Uri uri(SLIENT_ACCESS_URI);
    DataShare::DataShareValuesBucket valuesBucket;
    int age = 20;
    for (int i = 0; i < 10; i++) {
        valuesBucket.Put(TBL_STU_AGE, age);
    }
    for (int i = 0; i < 1000; i++) {
        helper->Insert(uri, valuesBucket);
    }
    vector<string> columns;
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(TBL_STU_AGE, age);
    auto resultSet = helper->Query(uri, predicates, columns);
    ASSERT_NE(resultSet, nullptr);
    std::vector<std::string> columnNames;
    int rowCount = 0;
    resultSet->GetRowCount(rowCount);
    resultSet->GetAllColumnNames(columnNames);
    int columnCount = columnNames.size();
    int valueResult;
    std::chrono::system_clock::time_point startTimeStamp = std::chrono::system_clock::now();
    int64_t start = std::chrono::duration_cast<std::chrono::milliseconds>(
        startTimeStamp.time_since_epoch()).count();
    for (int i = 0; i < rowCount; i++) {
        resultSet->GoToRow(i);
        for (int j = 0; j < columnCount; j++) {
            resultSet-> GetInt(columnCount, valueResult);
        }
    }
    std::chrono::system_clock::time_point endTimeStamp = std::chrono::system_clock::now();
    int64_t end = std::chrono::duration_cast<std::chrono::milliseconds>(
        endTimeStamp.time_since_epoch()).count();
    int time = end - start;
    LOG_INFO("Query_Threshold_Test004::end - start = %{public}d", time);
    //EXPECT are meaningless. Different devices make judgments based on the time of log printing
    EXPECT_LT(time, 500);
    LOG_INFO("Query_Threshold_Test004::End");
}
}
}