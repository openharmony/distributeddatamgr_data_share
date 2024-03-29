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
std::string EMPTY_URI = "";
std::string PROXY_ERROR_BUNDLE_URI = "datashareproxy://com.acts.datasharetest.error/test";
std::string URI_DIFF_PROXY_DATA = "datashareproxy://com.acts.datasharetest/test/error";
std::string PROXY_URI_OK = "datashareproxy://com.acts.datasharetest/test";
std::string PROXY_URI_HAVA_QUERY = "datashareproxy://com.acts.datasharetest/test?table=user&key=zhangsan";
std::string DATA_SHARE_ERROR_BUNDLE_URI = "datashare:///error.bundle.name/test";
std::string DATA_SHARE_URI = "datashare:///com.acts.datasharetest/test";
std::string DATA_SHARE_WRITEURI = "datashare:///com.acts.datasharetest/test/permission";

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

HWTEST_F(PermissionTest, PermissionTest_Uri_Empty_Test_001, TestSize.Level0)
{
    LOG_INFO("PermissionTest_Uri_Scheme_Error_Test_001::Start");
    auto tokenId = AccessTokenKit::GetHapTokenIDEx(USER_100, "ohos.datashareclienttest.demo", 0);
    Uri uri(EMPTY_URI);
    auto ret = DataShare::DataSharePermission::VerifyPermission(tokenId.tokenIDEx, uri, true);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
    LOG_INFO("PermissionTest_Uri_Scheme_Error_Test_001::End");
}

HWTEST_F(PermissionTest, PermissionTest_Bundle_Name_Error_Test_001, TestSize.Level0)
{
    LOG_INFO("PermissionTest_Bundle_Name_Error_Test_001::Start");
    auto tokenId = AccessTokenKit::GetHapTokenIDEx(USER_100, "ohos.datashareclienttest.demo", 0);
    Uri uri(PROXY_ERROR_BUNDLE_URI);
    auto ret = DataShare::DataSharePermission::VerifyPermission(tokenId.tokenIDEx, uri, true);
    EXPECT_EQ(ret, E_BUNDLE_NAME_NOT_EXIST);
    LOG_INFO("PermissionTest_Bundle_Name_Error_Test_001::End");
}

HWTEST_F(PermissionTest, PermissionTest_Uri_Diff_ProxyData_Test_001, TestSize.Level0)
{
    LOG_INFO("PermissionTest_Uri_Diff_ProxyData_Test_001::Start");
    auto tokenId = AccessTokenKit::GetHapTokenIDEx(USER_100, "ohos.datashareclienttest.demo", 0);
    Uri uri(URI_DIFF_PROXY_DATA);
    auto ret = DataShare::DataSharePermission::VerifyPermission(tokenId.tokenIDEx, uri, true);
    EXPECT_EQ(ret, E_URI_NOT_EXIST);
    LOG_INFO("PermissionTest_Uri_Diff_ProxyData_Test_001::End");
}

HWTEST_F(PermissionTest, PermissionTest_ProxyUri_OK_Test_001, TestSize.Level0)
{
    LOG_INFO("PermissionTest_ProxyUri_OK_Test_001::Start");
    auto tokenId = AccessTokenKit::GetHapTokenIDEx(USER_100, "ohos.datashareclienttest.demo", 0);
    Uri uri(PROXY_URI_OK);
    auto ret = DataShare::DataSharePermission::VerifyPermission(tokenId.tokenIDEx, uri, true);
    EXPECT_EQ(ret, E_OK);
    LOG_INFO("PermissionTest_ProxyUri_OK_Test_001::End");
}

HWTEST_F(PermissionTest, PermissionTest_ProxyUri_OK_Write_Permission_Error_Test_001, TestSize.Level0)
{
    LOG_INFO("PermissionTest_ProxyUri_OK_Write_Permission_Error_Test_001::Start");
    auto tokenId = AccessTokenKit::GetHapTokenIDEx(USER_100, "ohos.datashareclienttest.demo", 0);
    Uri uri(PROXY_URI_OK);
    // isRead is false, verify write permission
    auto ret = DataShare::DataSharePermission::VerifyPermission(tokenId.tokenIDEx, uri, false);
    EXPECT_EQ(ret, ERR_PERMISSION_DENIED);
    LOG_INFO("PermissionTest_ProxyUri_OK_Write_Permission_Error_Test_001::End");
}

HWTEST_F(PermissionTest, PermissionTest_Error_Bundle_Name_Test_001, TestSize.Level0)
{
    LOG_INFO("PermissionTest_Error_Bundle_Name_Test_001::Start");
    auto tokenId = AccessTokenKit::GetHapTokenIDEx(USER_100, "ohos.datashareclienttest.demo", 0);
    Uri uri(DATA_SHARE_ERROR_BUNDLE_URI);
    auto ret = DataShare::DataSharePermission::VerifyPermission(tokenId.tokenIDEx, uri, true);
    EXPECT_EQ(ret, E_BUNDLE_NAME_NOT_EXIST);
    LOG_INFO("PermissionTest_Error_Bundle_Name_Test_001::End");
}

HWTEST_F(PermissionTest, PermissionTest_No_Read_Permission_Test_001, TestSize.Level0)
{
    LOG_INFO("PermissionTest_No_Read_Permission_Test_001::Start");
    auto tokenId = AccessTokenKit::GetHapTokenIDEx(USER_100, "ohos.datashareclienttest.demo", 0);
    Uri uri(DATA_SHARE_URI);
    // proxyData requiredReadPermission is ohos.permission.READ_CALL_LOG
    auto ret = DataShare::DataSharePermission::VerifyPermission(tokenId.tokenIDEx, uri, true);
    EXPECT_EQ(ret, ERR_PERMISSION_DENIED);
    LOG_INFO("PermissionTest_No_Read_Permission_Test_001::End");
}

HWTEST_F(PermissionTest, PermissionTest_Have_Write_Permission_Test_001, TestSize.Level0)
{
    LOG_INFO("PermissionTest_HAVA_WRITE_PERMISSION_Test_001::Start");
    auto tokenId = AccessTokenKit::GetHapTokenIDEx(USER_100, "ohos.datashareclienttest.demo", 0);
    Uri uri(DATA_SHARE_WRITEURI);
    // proxyData requiredWritePermission is ohos.permission.WRITE_CALL_LOG
    auto ret = DataShare::DataSharePermission::VerifyPermission(tokenId.tokenIDEx, uri, false);
    EXPECT_EQ(ret, E_OK);
    LOG_INFO("PermissionTest_HAVA_WRITE_PERMISSION_Test_001::End");
}

HWTEST_F(PermissionTest, PermissionTest_Empty_Read_Permission_Test_001, TestSize.Level0)
{
    LOG_INFO("PermissionTest_Empty_Read_Permission_Test_001::Start");
    auto tokenId = AccessTokenKit::GetHapTokenIDEx(USER_100, "ohos.datashareclienttest.demo", 0);
    Uri uri(DATA_SHARE_WRITEURI);
    // proxyData not config requiredReadPermission
    auto ret = DataShare::DataSharePermission::VerifyPermission(tokenId.tokenIDEx, uri, true);
    EXPECT_EQ(ret, ERR_PERMISSION_DENIED);
    LOG_INFO("PermissionTest_Empty_Read_Permission_Test_001::End");
}

HWTEST_F(PermissionTest, PermissionTest_Empty_Write_Permission_Test_001, TestSize.Level0)
{
    LOG_INFO("PermissionTest_Empty_Write_Permission_Test_001::Start");
    auto tokenId = AccessTokenKit::GetHapTokenIDEx(USER_100, "ohos.datashareclienttest.demo", 0);
    Uri uri(DATA_SHARE_URI);
    // proxyData not config requiredWritePermission
    auto ret = DataShare::DataSharePermission::VerifyPermission(tokenId.tokenIDEx, uri, false);
    EXPECT_EQ(ret, ERR_PERMISSION_DENIED);
    LOG_INFO("PermissionTest_Empty_Write_Permission_Test_001::End");
}

HWTEST_F(PermissionTest, PermissionTest_Have_Query_Param_001, TestSize.Level0)
{
    LOG_INFO("PermissionTest_Have_Query_Param_001::Start");
    auto tokenId = AccessTokenKit::GetHapTokenIDEx(USER_100, "ohos.datashareclienttest.demo", 0);
    Uri uri(PROXY_URI_HAVA_QUERY);
    auto ret = DataShare::DataSharePermission::VerifyPermission(tokenId.tokenIDEx, uri, true);
    EXPECT_EQ(ret, E_OK);
    LOG_INFO("PermissionTest_Have_Query_Param_001::End");
}

HWTEST_F(PermissionTest, PermissionTest_Have_Write_Test_001, TestSize.Level0)
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

HWTEST_F(PermissionTest, PermissionTest_Hava_Read_Permission_Test_001, TestSize.Level0)
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