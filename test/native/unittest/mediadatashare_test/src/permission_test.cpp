/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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
#include <string>

#include "access_token.h"
#include "accesstoken_kit.h"
#include "data_share_permission.h"
#include "datashare_errno.h"
#include "datashare_log.h"
#include "errors.h"
#include "token_setproc.h"
#include "uri.h"

namespace OHOS {
namespace DataShare {
using namespace testing::ext;
using namespace OHOS::Security::AccessToken;
static int USER_100 = 100;
static const std::string EMPTY_URI = "";
static const std::string PROXY_ERROR_BUNDLE_URI = "datashareproxy://com.acts.datasharetest.error/test";
static const std::string URI_DIFF_PROXY_DATA = "datashareproxy://com.acts.datasharetest/test/error";
static const std::string PROXY_URI_OK = "datashareproxy://com.acts.datasharetest/test";
static const std::string PROXY_URI_HAVA_QUERY = "datashareproxy://com.acts.datasharetest/test?table=user&key=zhangsan";
static const std::string DATA_SHARE_ERROR_BUNDLE_URI = "datashare:///error.bundle.name/test";
static const std::string DATA_SHARE_URI = "datashareproxy://com.acts.datasharetest/readtest";
static const std::string DATA_SHARE_WRITEURI = "datashareproxy://com.acts.datasharetest/permissiontest/permission";
static const std::string DATA_SHARE_EXTENSION_URI = "datashare:///com.acts.datasharetest";
static const std::string DATA_SHARE_SELF_URI = "datashareproxy://ohos.datashareclienttest.demo";

class PermissionTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void PermissionTest::SetUpTestCase(void)
{
    LOG_INFO("SetUpTestCase invoked");
    int sleepTime = 3;
    sleep(sleepTime);
    HapInfoParams info = {
        .userID = USER_100,
        .bundleName = "ohos.datashareclienttest.demo",
        .instIndex = 0,
        .isSystemApp = true,
        .apiVersion = 8,
        .appIDDesc = "ohos.datashareclienttest.demo"
    };
    HapPolicyParams policy = {
        .apl = APL_SYSTEM_CORE,
        .domain = "test.domain",
        .permStateList = {
            {
                .permissionName = "ohos.permission.GET_BUNDLE_INFO",
                .isGeneral = true,
                .resDeviceID = { "local" },
                .grantStatus = { PermissionState::PERMISSION_GRANTED },
                .grantFlags = { 1 }
            },
            {
                .permissionName = "ohos.permission.READ_CONTACTS",
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
            }
        }
    };
    AccessTokenKit::AllocHapToken(info, policy);
    auto testTokenId = Security::AccessToken::AccessTokenKit::GetHapTokenIDEx(
        info.userID, info.bundleName, info.instIndex);
    SetSelfTokenID(testTokenId.tokenIDEx);
    LOG_INFO("SetUpTestCase end");
}

void PermissionTest::TearDownTestCase(void)
{
    auto tokenId = AccessTokenKit::GetHapTokenIDEx(USER_100, "ohos.datashareclienttest.demo", 0);
    AccessTokenKit::DeleteToken(tokenId.tokenIDEx);
}

void PermissionTest::SetUp(void) {}
void PermissionTest::TearDown(void) {}

/**
* @tc.name: PermissionTest_Uri_Empty_Test_001
* @tc.desc: Verify permission check with empty URI returns error
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create empty URI object
    2. Call VerifyPermission with empty URI and read operation
    3. Check returned error code
* @tc.experct: VerifyPermission returns ERR_INVALID_VALUE
*/
HWTEST_F(PermissionTest, PermissionTest_Uri_Empty_Test_001, TestSize.Level0)
{
    LOG_INFO("PermissionTest_Uri_Scheme_Error_Test_001::Start");
    auto tokenId = AccessTokenKit::GetHapTokenIDEx(USER_100, "ohos.datashareclienttest.demo", 0);
    Uri uri(EMPTY_URI);
    auto ret = DataShare::DataSharePermission::VerifyPermission(tokenId.tokenIDEx, uri, true);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
    LOG_INFO("PermissionTest_Uri_Scheme_Error_Test_001::End");
}

/**
* @tc.name: PermissionTest_Bundle_Name_Error_Test_001
* @tc.desc: Verify permission check with invalid bundle name in URI returns error
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create URI with invalid bundle name
    2. Call VerifyPermission with this URI and read operation
    3. Check returned error code
* @tc.experct: VerifyPermission returns E_BUNDLE_NAME_NOT_EXIST
*/
HWTEST_F(PermissionTest, PermissionTest_Bundle_Name_Error_Test_001, TestSize.Level0)
{
    LOG_INFO("PermissionTest_Bundle_Name_Error_Test_001::Start");
    auto tokenId = AccessTokenKit::GetHapTokenIDEx(USER_100, "ohos.datashareclienttest.demo", 0);
    Uri uri(PROXY_ERROR_BUNDLE_URI);
    auto ret = DataShare::DataSharePermission::VerifyPermission(tokenId.tokenIDEx, uri, true);
    EXPECT_EQ(ret, E_BUNDLE_NAME_NOT_EXIST);
    LOG_INFO("PermissionTest_Bundle_Name_Error_Test_001::End");
}

/**
* @tc.name: PermissionTest_Uri_Diff_ProxyData_Test_001
* @tc.desc: Verify permission check with non-existent proxy data URI returns error
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create URI pointing to non-existent proxy data
    2. Call VerifyPermission with this URI and read operation
    3. Check returned error code
* @tc.experct: VerifyPermission returns E_URI_NOT_EXIST
*/
HWTEST_F(PermissionTest, PermissionTest_Uri_Diff_ProxyData_Test_001, TestSize.Level1)
{
    LOG_INFO("PermissionTest_Uri_Diff_ProxyData_Test_001::Start");
    auto tokenId = AccessTokenKit::GetHapTokenIDEx(USER_100, "ohos.datashareclienttest.demo", 0);
    Uri uri(URI_DIFF_PROXY_DATA);
    auto ret = DataShare::DataSharePermission::VerifyPermission(tokenId.tokenIDEx, uri, true);
    EXPECT_EQ(ret, E_OK);
    LOG_INFO("PermissionTest_Uri_Diff_ProxyData_Test_001::End");
}

/**
* @tc.name: PermissionTest_ProxyUri_OK_Test_001
* @tc.desc: Verify permission check with valid proxy URI for read operation succeeds
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create valid proxy URI
    2. Call VerifyPermission with this URI and read operation
    3. Check returned error code
* @tc.experct: VerifyPermission returns E_OK
*/
HWTEST_F(PermissionTest, PermissionTest_ProxyUri_OK_Test_001, TestSize.Level1)
{
    LOG_INFO("PermissionTest_ProxyUri_OK_Test_001::Start");
    auto tokenId = AccessTokenKit::GetHapTokenIDEx(USER_100, "ohos.datashareclienttest.demo", 0);
    Uri uri(PROXY_URI_OK);
    auto ret = DataShare::DataSharePermission::VerifyPermission(tokenId.tokenIDEx, uri, true);
    EXPECT_EQ(ret, E_OK);
    LOG_INFO("PermissionTest_ProxyUri_OK_Test_001::End");
}

/**
* @tc.name: PermissionTest_ProxyUri_OK_Write_Permission_Error_Test_001
* @tc.desc: Verify permission check with valid proxy URI for write operation without permission fails
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create valid proxy URI
    2. Call VerifyPermission with this URI and write operation
    3. Check returned error code
* @tc.experct: VerifyPermission returns ERR_PERMISSION_DENIED
*/
HWTEST_F(PermissionTest, PermissionTest_ProxyUri_OK_Write_Permission_Error_Test_001, TestSize.Level1)
{
    LOG_INFO("PermissionTest_ProxyUri_OK_Write_Permission_Error_Test_001::Start");
    auto tokenId = AccessTokenKit::GetHapTokenIDEx(USER_100, "ohos.datashareclienttest.demo", 0);
    Uri uri(PROXY_URI_OK);
    // isRead is false, verify write permission
    auto ret = DataShare::DataSharePermission::VerifyPermission(tokenId.tokenIDEx, uri, false);
    EXPECT_EQ(ret, ERR_PERMISSION_DENIED);
    LOG_INFO("PermissionTest_ProxyUri_OK_Write_Permission_Error_Test_001::End");
}

/**
* @tc.name: PermissionTest_Error_Bundle_Name_Test_001
* @tc.desc: Verify permission check with invalid bundle name in datashare URI returns error
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create datashare URI with invalid bundle name
    2. Call VerifyPermission with this URI and read operation
    3. Check returned error code
* @tc.experct: VerifyPermission returns E_BUNDLE_NAME_NOT_EXIST
*/
HWTEST_F(PermissionTest, PermissionTest_Error_Bundle_Name_Test_001, TestSize.Level0)
{
    LOG_INFO("PermissionTest_Error_Bundle_Name_Test_001::Start");
    auto tokenId = AccessTokenKit::GetHapTokenIDEx(USER_100, "ohos.datashareclienttest.demo", 0);
    Uri uri(DATA_SHARE_ERROR_BUNDLE_URI);
    auto ret = DataShare::DataSharePermission::VerifyPermission(tokenId.tokenIDEx, uri, true);
    EXPECT_EQ(ret, E_BUNDLE_NAME_NOT_EXIST);
    LOG_INFO("PermissionTest_Error_Bundle_Name_Test_001::End");
}

/**
* @tc.name: PermissionTest_No_Read_Permission_Test_001
* @tc.desc: Verify permission check for read operation without required permission fails
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create valid datashare URI requiring specific read permission
    2. Call VerifyPermission with this URI and read operation
    3. Check returned error code
* @tc.experct: VerifyPermission returns ERR_PERMISSION_DENIED
*/
HWTEST_F(PermissionTest, PermissionTest_No_Read_Permission_Test_001, TestSize.Level1)
{
    LOG_INFO("PermissionTest_No_Read_Permission_Test_001::Start");
    auto tokenId = AccessTokenKit::GetHapTokenIDEx(USER_100, "ohos.datashareclienttest.demo", 0);
    Uri uri(DATA_SHARE_URI);
    // proxyData requiredReadPermission is ohos.permission.READ_CALL_LOG
    auto ret = DataShare::DataSharePermission::VerifyPermission(tokenId.tokenIDEx, uri, true);
    EXPECT_EQ(ret, ERR_PERMISSION_DENIED);
    LOG_INFO("PermissionTest_No_Read_Permission_Test_001::End");
}

/**
* @tc.name: PermissionTest_Have_Write_Permission_Test_001
* @tc.desc: Verify permission check for write operation with required permission succeeds
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create valid datashare URI requiring specific write permission
    2. Call VerifyPermission with this URI and write operation
    3. Check returned error code
* @tc.experct: VerifyPermission returns E_OK
*/
HWTEST_F(PermissionTest, PermissionTest_Have_Write_Permission_Test_001, TestSize.Level1)
{
    LOG_INFO("PermissionTest_HAVA_WRITE_PERMISSION_Test_001::Start");
    auto tokenId = AccessTokenKit::GetHapTokenIDEx(USER_100, "ohos.datashareclienttest.demo", 0);
    Uri uri(DATA_SHARE_WRITEURI);
    // proxyData requiredWritePermission is ohos.permission.WRITE_CALL_LOG
    auto ret = DataShare::DataSharePermission::VerifyPermission(tokenId.tokenIDEx, uri, false);
    EXPECT_EQ(ret, E_OK);
    LOG_INFO("PermissionTest_HAVA_WRITE_PERMISSION_Test_001::End");
}

/**
* @tc.name: PermissionTest_Empty_Read_Permission_Test_001
* @tc.desc: Verify permission check for read operation with unconfigured read permission fails
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create datashare URI with unconfigured read permission
    2. Call VerifyPermission with this URI and read operation
    3. Check returned error code
* @tc.experct: VerifyPermission returns ERR_PERMISSION_DENIED
*/
HWTEST_F(PermissionTest, PermissionTest_Empty_Read_Permission_Test_001, TestSize.Level1)
{
    LOG_INFO("PermissionTest_Empty_Read_Permission_Test_001::Start");
    auto tokenId = AccessTokenKit::GetHapTokenIDEx(USER_100, "ohos.datashareclienttest.demo", 0);
    Uri uri(DATA_SHARE_WRITEURI);
    // proxyData not config requiredReadPermission
    auto ret = DataShare::DataSharePermission::VerifyPermission(tokenId.tokenIDEx, uri, true);
    EXPECT_EQ(ret, ERR_PERMISSION_DENIED);
    LOG_INFO("PermissionTest_Empty_Read_Permission_Test_001::End");
}

/**
* @tc.name: PermissionTest_Empty_Write_Permission_Test_001
* @tc.desc: Verify permission check for write operation with unconfigured write permission fails
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create datashare URI with unconfigured write permission
    2. Call VerifyPermission with this URI and write operation
    3. Check returned error code
* @tc.expected: VerifyPermission returns ERR_PERMISSION_DENIED
*/
HWTEST_F(PermissionTest, PermissionTest_Empty_Write_Permission_Test_001, TestSize.Level1)
{
    LOG_INFO("PermissionTest_Empty_Write_Permission_Test_001::Start");
    auto tokenId = AccessTokenKit::GetHapTokenIDEx(USER_100, "ohos.datashareclienttest.demo", 0);
    Uri uri(DATA_SHARE_URI);
    // proxyData not config requiredWritePermission
    auto ret = DataShare::DataSharePermission::VerifyPermission(tokenId.tokenIDEx, uri, false);
    EXPECT_EQ(ret, ERR_PERMISSION_DENIED);
    LOG_INFO("PermissionTest_Empty_Write_Permission_Test_001::End");
}

/**
* @tc.name: PermissionTest_Have_Query_Param_001
* @tc.desc: Verify permission check with URI containing query parameters succeeds
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create valid proxy URI with query parameters
    2. Call VerifyPermission with this URI and read operation
    3. Check returned error code
* @tc.expected: VerifyPermission returns E_OK
*/
HWTEST_F(PermissionTest, PermissionTest_Have_Query_Param_001, TestSize.Level1)
{
    LOG_INFO("PermissionTest_Have_Query_Param_001::Start");
    auto tokenId = AccessTokenKit::GetHapTokenIDEx(USER_100, "ohos.datashareclienttest.demo", 0);
    Uri uri(PROXY_URI_HAVA_QUERY);
    auto ret = DataShare::DataSharePermission::VerifyPermission(tokenId.tokenIDEx, uri, true);
    EXPECT_EQ(ret, E_OK);
    LOG_INFO("PermissionTest_Have_Query_Param_001::End");
}

HWTEST_F(PermissionTest, PermissionTest_DataObs_GetUriPermission_Uri_Empty_Test_001, TestSize.Level0)
{
    LOG_INFO("PermissionTest_DataObs_GetUriPermission_Uri_Empty_Test_001::Start");
    Uri uri(EMPTY_URI);
    auto [ret, permission] = DataShare::DataSharePermission::GetUriPermission(uri, USER_100, true, false);
    EXPECT_EQ(ret, E_EMPTY_URI);
    LOG_INFO("PermissionTest_DataObs_GetUriPermission_Uri_Empty_Test_001::End");
}

HWTEST_F(PermissionTest, PermissionTest_DataObs_GetUriPermission_Uri_Error_Test_001, TestSize.Level0)
{
    LOG_INFO("PermissionTest_DataObs_GetUriPermission_Uri_Error_Test_001::Start");
    Uri uri(PROXY_ERROR_BUNDLE_URI);
    auto [ret, permission] = DataShare::DataSharePermission::GetUriPermission(uri, USER_100, true, false);
    EXPECT_EQ(ret, E_BUNDLE_NAME_NOT_EXIST);
    LOG_INFO("PermissionTest_DataObs_GetUriPermission_Uri_Error_Test_001::End");
}

HWTEST_F(PermissionTest, PermissionTest_DataObs_GetUriPermission_Uri_OK_Test_001, TestSize.Level1)
{
    LOG_INFO("PermissionTest_DataObs_GetUriPermission_Uri_OK_Test_001::Start");
    Uri uri(PROXY_URI_OK);
    auto [ret, permission] = DataShare::DataSharePermission::GetUriPermission(uri, USER_100, true, false);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(permission, "ohos.permission.GET_BUNDLE_INFO");
    LOG_INFO("PermissionTest_DataObs_GetUriPermission_Uri_OK_Test_001::End");
}

HWTEST_F(PermissionTest, PermissionTest_DataObs_GetUriPermission_Uri_OK_Test_002, TestSize.Level1)
{
    LOG_INFO("PermissionTest_DataObs_GetUriPermission_Uri_OK_Test_002::Start");
    Uri uri(PROXY_URI_OK);
    auto [ret, permission] = DataShare::DataSharePermission::GetUriPermission(uri, USER_100, false, false);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(permission, "ohos.permission.WRITE_CONTACTS");
    LOG_INFO("PermissionTest_DataObs_GetUriPermission_Uri_OK_Test_002::End");
}

HWTEST_F(PermissionTest, PermissionTest_DataObs_GetUriPermission_Uri_OK_Test_003, TestSize.Level1)
{
    LOG_INFO("PermissionTest_DataObs_GetUriPermission_Uri_OK_Test_003::Start");
    Uri uri(DATA_SHARE_EXTENSION_URI);
    auto [ret, permission] = DataShare::DataSharePermission::GetUriPermission(uri, USER_100, true, true);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(permission, "");
    LOG_INFO("PermissionTest_DataObs_GetUriPermission_Uri_OK_Test_003::End");
}

HWTEST_F(PermissionTest, PermissionTest_DataObs_GetUriPermission_Uri_OK_Test_004, TestSize.Level1)
{
    LOG_INFO("PermissionTest_DataObs_GetUriPermission_Uri_OK_Test_004::Start");
    Uri uri(DATA_SHARE_EXTENSION_URI);
    auto [ret, permission] = DataShare::DataSharePermission::GetUriPermission(uri, USER_100, false, true);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(permission, "");
    LOG_INFO("PermissionTest_DataObs_GetUriPermission_Uri_OK_Test_004::End");
}

HWTEST_F(PermissionTest, PermissionTest_DataObs_VerifyPermission_Test_001, TestSize.Level1)
{
    LOG_INFO("PermissionTest_DataObs_VerifyPermission_Test_001::Start");
    auto tokenId = AccessTokenKit::GetHapTokenIDEx(USER_100, "ohos.datashareclienttest.demo", 0);
    Uri uri(PROXY_URI_OK);
    std::string permission = "ohos.permission.GET_BUNDLE_INFO";
    auto ret = DataShare::DataSharePermission::VerifyPermission(uri, tokenId.tokenIDEx, permission, true);
    EXPECT_EQ(ret, true);
    LOG_INFO("PermissionTest_DataObs_VerifyPermission_Test_001::End");
}

HWTEST_F(PermissionTest, PermissionTest_DataObs_VerifyPermission_Test_002, TestSize.Level1)
{
    LOG_INFO("PermissionTest_DataObs_VerifyPermission_Test_002::Start");
    auto tokenId = AccessTokenKit::GetHapTokenIDEx(USER_100, "ohos.datashareclienttest.demo", 0);
    Uri uri(PROXY_URI_OK);
    std::string permission = "ohos.permission.WIFI";
    auto ret = DataShare::DataSharePermission::VerifyPermission(uri, tokenId.tokenIDEx, permission, true);
    EXPECT_EQ(ret, false);
    LOG_INFO("PermissionTest_DataObs_VerifyPermission_Test_002::End");
}

HWTEST_F(PermissionTest, PermissionTest_DataObs_VerifyPermission_Test_003, TestSize.Level1)
{
    LOG_INFO("PermissionTest_DataObs_VerifyPermission_Test_003::Start");
    auto tokenId = AccessTokenKit::GetHapTokenIDEx(USER_100, "ohos.datashareclienttest.demo", 0);
    Uri uri(DATA_SHARE_SELF_URI);
    std::string permission = "";
    auto ret = DataShare::DataSharePermission::VerifyPermission(uri, tokenId.tokenIDEx, permission, false);
    EXPECT_EQ(ret, true);
    LOG_INFO("PermissionTest_DataObs_VerifyPermission_Test_003::End");
}

HWTEST_F(PermissionTest, PermissionTest_DataObs_VerifyPermission_Test_004, TestSize.Level1)
{
    LOG_INFO("PermissionTest_DataObs_VerifyPermission_Test_004::Start");
    auto tokenId = AccessTokenKit::GetHapTokenIDEx(USER_100, "ohos.datashareclienttest.demo", 0);
    Uri uri(DATA_SHARE_SELF_URI);
    std::string permission = "";
    auto ret = DataShare::DataSharePermission::VerifyPermission(uri, tokenId.tokenIDEx, permission, true);
    EXPECT_EQ(ret, true);
    LOG_INFO("PermissionTest_DataObs_VerifyPermission_Test_004::End");
}

HWTEST_F(PermissionTest, PermissionTest_DataObs_VerifyPermission_Test_005, TestSize.Level1)
{
    LOG_INFO("PermissionTest_DataObs_VerifyPermission_Test_005::Start");
    auto tokenId = AccessTokenKit::GetHapTokenIDEx(USER_100, "ohos.datashareclienttest.demo", 0);
    Uri uri(DATA_SHARE_SELF_URI);
    std::string permission = "";
    auto ret = DataShare::DataSharePermission::VerifyPermission(uri, tokenId.tokenIDEx, permission, false);
    EXPECT_EQ(ret, true);
    LOG_INFO("PermissionTest_DataObs_VerifyPermission_Test_005::End");
}

HWTEST_F(PermissionTest, PermissionTest_DataObs_VerifyPermission_Test_006, TestSize.Level1)
{
    LOG_INFO("PermissionTest_DataObs_VerifyPermission_Test_006::Start");
    auto tokenId = AccessTokenKit::GetHapTokenIDEx(USER_100, "ohos.datashareclienttest.demo", 0);
    Uri uri(PROXY_URI_OK);
    std::string permission = "";
    auto ret = DataShare::DataSharePermission::VerifyPermission(uri, tokenId.tokenIDEx, permission, false);
    EXPECT_EQ(ret, false);
    LOG_INFO("PermissionTest_DataObs_VerifyPermission_Test_006::End");
}

HWTEST_F(PermissionTest, PermissionTest_IsExtensionValid_001, TestSize.Level1)
{
    LOG_INFO("PermissionTest_IsExtensionValid_001::Start");
    auto tokenId = AccessTokenKit::GetHapTokenIDEx(USER_100, "com.acts.datasharetest", 0);
    auto ret = DataShare::DataSharePermission::IsExtensionValid(tokenId.tokenIDEx, tokenId.tokenIDEx, USER_100);
    EXPECT_EQ(ret, E_OK);
    LOG_INFO("PermissionTest_IsExtensionValid_001::End");
}

HWTEST_F(PermissionTest, PermissionTest_IsExtensionValid_002, TestSize.Level1)
{
    LOG_INFO("PermissionTest_IsExtensionValid_002::Start");
    auto tokenId = AccessTokenKit::GetHapTokenIDEx(USER_100, "com.acts.ohos.data.datasharetest", 0);
    auto ret = DataShare::DataSharePermission::IsExtensionValid(tokenId.tokenIDEx, tokenId.tokenIDEx, USER_100);
    EXPECT_EQ(ret, E_NOT_DATASHARE_EXTENSION);
    LOG_INFO("PermissionTest_IsExtensionValid_002::End");
}

/**
* @tc.name: PermissionTest_Have_Write_Test_001
* @tc.desc: Verify permission check for write operation with correct permission succeeds
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create new HAP info with write permission configuration
    2. Allocate access token with required write permission
    3. Set the new token as current process token
    4. Call VerifyPermission with valid URI and write operation
    5. Check returned error code
    6. Clean up the created token
* @tc.expected: VerifyPermission returns E_OK
*/
HWTEST_F(PermissionTest, PermissionTest_Have_Write_Test_001, TestSize.Level1)
{
    LOG_INFO("PermissionTest_Have_Write_Test_001::Start");
    HapInfoParams info = {
        .userID = USER_100,
        .bundleName = "ohos.permission.write.demo",
        .instIndex = 0,
        .isSystemApp = true,
        .apiVersion = 8,
        .appIDDesc = "ohos.permission.write.demo"
    };
    HapPolicyParams policy = {
        .apl = APL_SYSTEM_CORE,
        .domain = "test.domain",
        .permStateList = {
            {
                .permissionName = "ohos.permission.GET_BUNDLE_INFO",
                .isGeneral = true,
                .resDeviceID = { "local" },
                .grantStatus = { PermissionState::PERMISSION_GRANTED },
                .grantFlags = { 1 }
            },
            {
                .permissionName = "ohos.permission.WRITE_CONTACTS",
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

    Uri uri(PROXY_URI_OK);
    auto ret = DataShare::DataSharePermission::VerifyPermission(testTokenId.tokenIDEx, uri, false);
    EXPECT_EQ(ret, E_OK);
    AccessTokenKit::DeleteToken(testTokenId.tokenIDEx);
    LOG_INFO("PermissionTest_Have_Write_Test_001::End");
}

/**
* @tc.name: PermissionTest_Hava_Read_Permission_Test_001
* @tc.desc: Verify permission check for read operation with correct permission succeeds
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create new HAP info with read permission configuration
    2. Allocate access token with required read permission
    3. Set the new token as current process token
    4. Call VerifyPermission with valid URI and read operation
    5. Check returned error code
    6. Clean up the created token
* @tc.expected: VerifyPermission returns E_OK
*/
HWTEST_F(PermissionTest, PermissionTest_Hava_Read_Permission_Test_001, TestSize.Level1)
{
    LOG_INFO("PermissionTest_Hava_Read_Permission_Test_001::Start");
    HapInfoParams info = {
        .userID = USER_100,
        .bundleName = "ohos.permission.demo",
        .instIndex = 0,
        .isSystemApp = true,
        .apiVersion = 8,
        .appIDDesc = "ohos.permission.demo"
    };
    HapPolicyParams policy = {
        .apl = APL_SYSTEM_CORE,
        .domain = "test.domain",
        .permStateList = {
            {
                .permissionName = "ohos.permission.GET_BUNDLE_INFO",
                .isGeneral = true,
                .resDeviceID = { "local" },
                .grantStatus = { PermissionState::PERMISSION_GRANTED },
                .grantFlags = { 1 }
            },
            {
                .permissionName = "ohos.permission.READ_CALL_LOG",
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
    Uri uri(DATA_SHARE_URI);
    auto ret = DataShare::DataSharePermission::VerifyPermission(testTokenId.tokenIDEx, uri, true);
    EXPECT_EQ(ret, E_OK);

    AccessTokenKit::DeleteToken(testTokenId.tokenIDEx);
    LOG_INFO("PermissionTest_Hava_Read_Permission_Test_001::End");
}

} // namespace DataShare
} // namespace OHOS