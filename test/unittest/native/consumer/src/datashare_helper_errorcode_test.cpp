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

#define LOG_TAG "errorcode_test"

#include <gtest/gtest.h>
#include <unistd.h>

#include "accesstoken_kit.h"
#include "datashare_helper.h"
#include "datashare_log.h"
#include "hap_token_info.h"
#include "iservice_registry.h"
#include "rdb_errno.h"
#include "system_ability_definition.h"
#include "token_setproc.h"

namespace OHOS {
namespace DataShare {
using namespace testing::ext;
using namespace OHOS::Security::AccessToken;
constexpr int STORAGE_MANAGER_MANAGER_ID = 5003;
std::string DATA_SHARE_URI = "datashare:///com.acts.errorcodetest";
std::string SLIENT_ACCESS_URI = "datashareproxy://com.acts.errorcodetest/test?Proxy=true";
std::string TBL_STU_NAME = "name";
std::string TBL_STU_AGE = "age";
std::shared_ptr<DataShare::DataShareHelper> g_slientAccessHelper;
std::shared_ptr<DataShare::DataShareHelper> dataShareHelper;

class DataShareHelperErrorCodeTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
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

HapPolicyParams GetPolicy()
{
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
    return policy;
}

void DataShareHelperErrorCodeTest::SetUpTestCase(void)
{
    LOG_INFO("SetUpTestCase invoked");
    dataShareHelper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID, DATA_SHARE_URI);
    ASSERT_TRUE(dataShareHelper != nullptr);
    int sleepTime = 3;
    sleep(sleepTime);

    HapInfoParams info = {
        .userID = 100,
        .bundleName = "ohos.datashareclienttest.demo",
        .instIndex = 0,
        .appIDDesc = "ohos.datashareclienttest.demo"
    };
    auto policy = GetPolicy();
    AccessTokenKit::AllocHapToken(info, policy);
    auto testTokenId = Security::AccessToken::AccessTokenKit::GetHapTokenID(
        info.userID, info.bundleName, info.instIndex);
    SetSelfTokenID(testTokenId);

    g_slientAccessHelper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID, SLIENT_ACCESS_URI);
    ASSERT_TRUE(g_slientAccessHelper != nullptr);
    LOG_INFO("SetUpTestCase end");
}

void DataShareHelperErrorCodeTest::TearDownTestCase(void)
{
    auto tokenId = AccessTokenKit::GetHapTokenID(100, "ohos.datashareclienttest.demo", 0);
    AccessTokenKit::DeleteToken(tokenId);
    g_slientAccessHelper = nullptr;
    dataShareHelper = nullptr;
}

void DataShareHelperErrorCodeTest::SetUp(void) {}
void DataShareHelperErrorCodeTest::TearDown(void) {}

/**
 * @tc.name: ErrorCodeTest_Insert_Test_001
 * @tc.desc: Verify that the Insert operation in the silent access DataShareHelper returns a positive ID when the
 *           insertion is successful.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The global silent access helper instance `g_slientAccessHelper` is pre-initialized and not nullptr.
    2. Predefined constants are valid: `SLIENT_ACCESS_URI` (target URI for silent access), `TBL_STU_NAME` (string-type
       table column), and `TBL_STU_AGE` (int-type table column).
    3. The `DataShareValuesBucket` class supports the `Put` method to add string and int data to the bucket.
 * @tc.step:
    1. Get the global silent access helper instance via `g_slientAccessHelper`.
    2. Create a `Uri` object using the predefined `SLIENT_ACCESS_URI`; create an empty `DataShareValuesBucket` object.
    3. Call `Put` on the bucket to add `TBL_STU_NAME` with value "lisi" and `TBL_STU_AGE` with value 25.
    4. Call the `Insert` method of `g_slientAccessHelper`, passing the created `Uri` and `DataShareValuesBucket`,
       and record the returned integer ID.
    5. Check if the returned ID is a positive value (greater than 0).
 * @tc.expect:
    1. The Insert operation succeeds, and the returned ID is a positive integer (retVal > 0).
 */
HWTEST_F(DataShareHelperErrorCodeTest, Insert_Test_001, TestSize.Level0)
{
    LOG_INFO("Insert_Test_001::Start");
    auto helper = g_slientAccessHelper;
    Uri uri(SLIENT_ACCESS_URI);
    DataShare::DataShareValuesBucket valuesBucket;
    std::string value = "lisi";
    valuesBucket.Put(TBL_STU_NAME, value);
    int age = 25;
    valuesBucket.Put(TBL_STU_AGE, age);

    int retVal = helper->Insert(uri, valuesBucket);
    EXPECT_EQ((retVal > 0), true);
    LOG_INFO("Insert_Test_001::End");
}

/**
 * @tc.name: ErrorCodeTest_QUERY_Test_001
 * @tc.desc: Verify that the Query operation in the silent access DataShareHelper returns correct error codes and
 *           result sets for valid URIs (with existing data) and invalid URIs (non-existent data source).
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The global silent access helper `g_slientAccessHelper` is pre-initialized and not nullptr.
    2. The valid URI `SLIENT_ACCESS_URI` has pre-existing test data (a record with `TBL_STU_NAME = "lisi"`).
    3. The invalid URI `ERR_SLIENT_ACCESS_URI` is predefined as and points to a non-existent data source.
    4. The `DatashareBusinessError` class supports the `GetCode` method to retrieve the operation's error code.
 * @tc.step:
    1. Create a `DataSharePredicates` object and call `EqualTo` to set the condition: `TBL_STU_NAME = "lisi"`;
       initialize an empty `vector<string>` to store query columns.
    2. Create a `DatashareBusinessError` object (`noError`), then call `g_slientAccessHelper->Query` with
       `SLIENT_ACCESS_URI`, predicates, columns, and `&noError`; record the returned `ResultSet`.
    3. Check the error code via `noError.GetCode()`; if the `ResultSet` is not nullptr, call `GetRowCount` to
       get the number of returned rows.
    4. Create a `Uri` object using the invalid `ERR_SLIENT_ACCESS_URI`; create another `DatashareBusinessError`
       object (`error`).
    5. Call `g_slientAccessHelper->Query` with the invalid `Uri`, predicates, columns, and `&error`; record the
       returned `ResultSet` and check `error.GetCode()`.
 * @tc.expect:
    1. For the valid URI query: `noError.GetCode()` returns 0 (no error), and the `ResultSet` has 1 row.
    2. For the invalid URI query: `error.GetCode()` returns `NativeRdb::E_DB_NOT_EXIST`, and the `ResultSet` is
       nullptr.
 */
HWTEST_F(DataShareHelperErrorCodeTest, QUERY_Test_001, TestSize.Level0)
{
    LOG_INFO("QUERY_Test_001::Start");
    auto helper = g_slientAccessHelper;
    Uri uri(SLIENT_ACCESS_URI);
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(TBL_STU_NAME, "lisi");
    vector<string> columns;
    DatashareBusinessError noError;
    auto resultSet = helper->Query(uri, predicates, columns, &noError);
    EXPECT_EQ(noError.GetCode(), 0);
    int result = 0;
    if (resultSet != nullptr) {
        resultSet->GetRowCount(result);
    }
    EXPECT_EQ(result, 1);

    std::string ERR_SLIENT_ACCESS_URI = "datashare:///com.acts.errorcodetest/entry/DB01/TBL01?Proxy=true";
    Uri uriErr(ERR_SLIENT_ACCESS_URI);
    DatashareBusinessError error;
    resultSet = helper->Query(uriErr, predicates, columns, &error);
    EXPECT_EQ(error.GetCode(), NativeRdb::E_DB_NOT_EXIST);
    EXPECT_EQ(resultSet, nullptr);
    LOG_INFO("QUERY_Test_001::End");
}

/**
 * @tc.name: ErrorCodeTest_QUERY_Test_002
 * @tc.desc: Verify that an unauthorized Query operation (using `dataShareHelper`) returns the correct 401 error
 *           code and null result set, while supporting successful Insert and Delete operations for test data
 *           management.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The `dataShareHelper` instance is pre-initialized and not nullptr.
    2. Predefined constant `DATA_SHARE_URI` is a valid target URI for Insert, Query, and Delete operations.
    3. The `DataShareValuesBucket` supports adding `TBL_STU_NAME` (string) and `TBL_STU_AGE` (int) via the `Put`
       method.
    4. The `DatashareBusinessError` class can correctly capture and return the 401 unauthorized error code.
 * @tc.step:
    1. Create a `DataShareValuesBucket`, call `Put` to add `TBL_STU_NAME = "wangwu"` and `TBL_STU_AGE = 30`;
       call `dataShareHelper->Insert` with `DATA_SHARE_URI` and the bucket, then check if the returned ID is positive.
    2. Create a `DataSharePredicates` object and call `EqualTo` to set the condition: `TBL_STU_NAME = "wangwu"`;
       initialize an empty `vector<string>` for query columns.
    3. Create a `DatashareBusinessError` object (`error`), call `dataShareHelper->Query` with `DATA_SHARE_URI`,
       predicates, columns, and `&error`; check the error code and if the `ResultSet` is nullptr.
    4. Create a delete `DataSharePredicates`, set its `WhereClause` to `TBL_STU_NAME + " = 'wangwu'"`;
       call `dataShareHelper->Delete` with `DATA_SHARE_URI` and the delete predicates.
    5. Check if the return value of the Delete operation is a positive integer (indicating successful deletion).
 * @tc.expect:
    1. The Insert operation succeeds and returns a positive integer (retVal > 0).
    2. The unauthorized Query operation returns `error.GetCode() = 401` and a `nullptr` `ResultSet`.
    3. The Delete operation succeeds and returns a positive integer (retVal > 0).
 */
HWTEST_F(DataShareHelperErrorCodeTest, QUERY_Test_002, TestSize.Level0)
{
    LOG_INFO("QUERY_Test_002::Start");
    ASSERT_TRUE(dataShareHelper != nullptr);
    Uri uri(DATA_SHARE_URI);

    DataShare::DataShareValuesBucket valuesBucket;
    std::string value = "wangwu";
    valuesBucket.Put(TBL_STU_NAME, value);
    int age = 30;
    valuesBucket.Put(TBL_STU_AGE, age);
    int retVal = dataShareHelper->Insert(uri, valuesBucket);
    EXPECT_EQ((retVal > 0), true);

    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(TBL_STU_NAME, "wangwu");
    vector<string> columns;
    DatashareBusinessError error;
    auto resultSet = dataShareHelper->Query(uri, predicates, columns, &error);
    EXPECT_EQ(error.GetCode(), 401);
    EXPECT_EQ(resultSet, nullptr);

    DataShare::DataSharePredicates deletePredicates;
    std::string selections = TBL_STU_NAME + " = 'wangwu'";
    deletePredicates.SetWhereClause(selections);
    retVal = dataShareHelper->Delete(uri, deletePredicates);
    EXPECT_EQ((retVal > 0), true);
    LOG_INFO("QUERY_Test_002::End");
}

/**
 * @tc.name: PREDICATES_VERIFY_Test_001
 * @tc.desc: Verify predicates and error handling for update, query, and delete operations with non-silent access
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: DataShareHelper instance is initialized
 * @tc.step:
    1. Insert test data into the database
    2. Update data with illegal predicates and verify error code
    3. Query data with invalid predicates and verify error code and result set
    4. Delete data with normal predicates and verify success
 * @tc.experct:
    1. Insert operation succeeds with positive return value
    2. Update operation returns -1 due to illegal predicates
    3. Query operation returns 401 error code and null result set
    4. Delete operation succeeds with positive return value
 */
HWTEST_F(DataShareHelperErrorCodeTest, Predicates_verify_Test_001, TestSize.Level0)
{
    LOG_INFO("Predicates_Verify_Test_001::Start");
    ASSERT_TRUE(dataShareHelper != nullptr);
    Uri uri(DATA_SHARE_URI);

    DataShare::DataShareValuesBucket valuesBucket;
    std::string value = "wangwu";
    valuesBucket.Put(TBL_STU_NAME, value);
    int age = 30;
    valuesBucket.Put(TBL_STU_AGE, age);
    int retVal = dataShareHelper->Insert(uri, valuesBucket);
    EXPECT_TRUE(retVal > 0);

    // update with illegal predicates
    DataShare::DataShareValuesBucket valuesBucket1;
    value = "wangwu_new";
    valuesBucket1.Put(TBL_STU_NAME, value);
    DataShare::DataSharePredicates predicates;
    predicates.GreaterThan("name and true", "wangwu");
    retVal = dataShareHelper->Update(uri, predicates, valuesBucket1);
    EXPECT_EQ(retVal, -1);

    // query with invalid predicates
    vector<string> columns;
    DatashareBusinessError error;
    DataShare::DataSharePredicates predicates1;
    predicates1.EqualTo("name AS colName", "wangwu");
    // add log only, do not return error when verify failed. 401 returns from JS provider
    auto resultSet = dataShareHelper->Query(uri, predicates1, columns, &error);
    EXPECT_EQ(error.GetCode(), 401);
    EXPECT_EQ(resultSet, nullptr);

    // delete with normal predicates
    DataShare::DataSharePredicates deletePredicates;
    std::string selections = TBL_STU_NAME + " = 'wangwu'";
    deletePredicates.SetWhereClause(selections);
    retVal = dataShareHelper->Delete(uri, deletePredicates);
    EXPECT_TRUE(retVal > 0);
    LOG_INFO("Predicates_Verify_Test_001::End");
}

/**
 * @tc.name: PREDICATES_VERIFY_Test_002
 * @tc.desc: Verify predicates and error handling for update and delete operations with non-silent access
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: DataShareHelper instance is initialized
 * @tc.step:
    1. Insert test data into the database
    2. Update data with normal predicates and verify success
    3. Delete data with illegal predicates and verify error code
    4. Delete data with normal predicates and verify success
 * @tc.experct:
    1. Insert operation succeeds with positive return value
    2. Update operation succeeds with return value 1
    3. Delete operation returns -1 due to illegal predicates
    4. Delete operation succeeds with positive return value
 */
HWTEST_F(DataShareHelperErrorCodeTest, Predicates_verify_Test_002, TestSize.Level0)
{
    LOG_INFO("Predicates_verify_Test_002::Start");
    ASSERT_TRUE(dataShareHelper != nullptr);
    Uri uri(DATA_SHARE_URI);

    DataShare::DataShareValuesBucket valuesBucket;
    std::string value = "wangwu";
    valuesBucket.Put(TBL_STU_NAME, value);
    int age = 30;
    valuesBucket.Put(TBL_STU_AGE, age);
    int retVal = dataShareHelper->Insert(uri, valuesBucket);
    EXPECT_TRUE(retVal > 0);

    // update with normal predicates
    DataShare::DataShareValuesBucket valuesBucket1;
    value = "wangwu_new";
    valuesBucket1.Put(TBL_STU_NAME, value);
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(TBL_STU_NAME, "wangwu");
    retVal = dataShareHelper->Update(uri, predicates, valuesBucket1);
    EXPECT_EQ(retVal, 1);

    // delete with illegal predicates
    DataShare::DataSharePredicates deletePredicates;
    deletePredicates.GreaterThanOrEqualTo("(1 = 1) OR true", "wangwu");
    retVal = dataShareHelper->Delete(uri, deletePredicates);
    EXPECT_EQ(retVal, -1);

    // delete with normal predicates
    DataShare::DataSharePredicates deletePredicates1;
    std::string selections = TBL_STU_NAME + " = 'wangwu_new'";
    deletePredicates1.SetWhereClause(selections);
    retVal = dataShareHelper->Delete(uri, deletePredicates1);
    EXPECT_TRUE(retVal > 0);
    LOG_INFO("Predicates_verify_Test_002::End");
}

/**
 * @tc.name: PREDICATES_VERIFY_Test_003
 * @tc.desc: Verify predicates and error handling for update, query, and delete operations with silent access
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: DataShareHelper instance is initialized
 * @tc.step:
    1. Insert test data into the database
    2. Update data with normal predicates and verify success
    3. Query data with illegal predicates and verify error code and result set
    4. Delete data with normal predicates and verify success
 * @tc.experct:
    1. Insert operation succeeds with positive return value
    2. Update operation succeeds with return value 1
    3. Query operation returns E_ERROR and null result set
    4. Delete operation succeeds with positive return value
 */
HWTEST_F(DataShareHelperErrorCodeTest, Predicates_verify_Test_003, TestSize.Level0)
{
    LOG_INFO("Predicates_verify_Test_003::Start");
    // insert data
    auto helper = g_slientAccessHelper;
    Uri uri(SLIENT_ACCESS_URI);
    DataShare::DataShareValuesBucket valuesBucket;
    std::string value = "ZhangSan";
    valuesBucket.Put(TBL_STU_NAME, value);
    int age = 25;
    valuesBucket.Put(TBL_STU_AGE, age);
    int retVal = helper->Insert(uri, valuesBucket);
    EXPECT_TRUE(retVal > 0);

    // update with normal predicates
    DataShare::DataShareValuesBucket valuesBucket1;
    value = "ZhangSan_new";
    valuesBucket1.Put(TBL_STU_NAME, value);
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(TBL_STU_NAME, "ZhangSan");
    retVal = helper->Update(uri, predicates, valuesBucket1);
    EXPECT_EQ(retVal, 1);

    // query with illegal predicates
    DataShare::DataSharePredicates predicates1;
    predicates1.NotEqualTo("(SELECT * from test)", "ZhangSan_new");
    vector<string> columns;
    DatashareBusinessError noError;
    auto resultSet = helper->Query(uri, predicates1, columns, &noError);
    EXPECT_EQ(noError.GetCode(), E_ERROR);
    EXPECT_EQ(resultSet, nullptr);

    // delete with normal predicates
    DataShare::DataSharePredicates deletePredicates;
    std::string selections = TBL_STU_NAME + " = 'ZhangSan_new'";
    deletePredicates.SetWhereClause(selections);
    retVal = helper->Delete(uri, deletePredicates);
    EXPECT_TRUE(retVal > 0);
    LOG_INFO("Predicates_verify_Test_003::End");
}

/**
 * @tc.name: PREDICATES_VERIFY_Test_004
 * @tc.desc: Verify predicates and error handling for update and delete operations with silent access
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: DataShareHelper instance is initialized
 * @tc.step:
    1. Insert test data into the database
    2. Update data with illegal predicates and verify error code
    3. Delete data with illegal predicates and verify error code
    4. Delete data with normal predicates and verify success
 * @tc.experct:
    1. Insert operation succeeds with positive return value
    2. Update operation returns DATA_SHARE_ERROR due to illegal predicates
    3. Delete operation returns -1 due to illegal predicates
    4. Delete operation succeeds with positive return value
 */
HWTEST_F(DataShareHelperErrorCodeTest, Predicates_verify_Test_004, TestSize.Level0)
{
    LOG_INFO("Predicates_verify_Test_004::Start");
    // insert data
    auto helper = g_slientAccessHelper;
    Uri uri(SLIENT_ACCESS_URI);
    DataShare::DataShareValuesBucket valuesBucket;
    std::string value = "ZhangSan";
    valuesBucket.Put(TBL_STU_NAME, value);
    int age = 25;
    valuesBucket.Put(TBL_STU_AGE, age);
    int retVal = helper->Insert(uri, valuesBucket);
    EXPECT_TRUE(retVal > 0);

    // update with illegal predicates
    DataShare::DataShareValuesBucket valuesBucket1;
    value = "ZhangSan_new";
    valuesBucket1.Put(TBL_STU_NAME, value);
    DataShare::DataSharePredicates predicates;
    predicates.LessThan("SUM(age) as name", "ZhangSan");
    retVal = helper->Update(uri, predicates, valuesBucket1);
    EXPECT_EQ(retVal, DATA_SHARE_ERROR);

    // delete with illegal predicates
    DataShare::DataSharePredicates deletePredicates;
    deletePredicates.GreaterThanOrEqualTo("name = name0 ) and (1 = 1 or name = ", "ZhangSan");
    retVal = helper->Delete(uri, deletePredicates);
    EXPECT_EQ(retVal, -1);

    // delete with normal predicates
    DataShare::DataSharePredicates deletePredicates1;
    std::string selections = TBL_STU_NAME + " = 'ZhangSan'";
    deletePredicates1.SetWhereClause(selections);
    retVal = helper->Delete(uri, deletePredicates1);
    EXPECT_TRUE(retVal > 0);
    LOG_INFO("Predicates_verify_Test_004::End");
}
} // namespace DataShare
} // namespace OHOS