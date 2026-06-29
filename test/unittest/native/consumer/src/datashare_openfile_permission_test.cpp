/*
 * Copyright (C) 2026 Huawei Device Co., Ltd.
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

#define LOG_TAG "datashare_openfile_permission_test"

#include <gtest/gtest.h>
#include <unistd.h>

#include "accesstoken_kit.h"
#include "datashare_helper.h"
#include "datashare_log.h"
#include "data_share_manager_impl.h"
#include "hap_token_info.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "token_setproc.h"
#include "datashare_errno.h"

namespace OHOS {
namespace DataShare {
using namespace testing::ext;
using namespace OHOS::Security::AccessToken;

constexpr int STORAGE_MANAGER_MANAGER_ID = 5003;
constexpr int ERR_INVALID_DATA = -1;
std::string DATA_SHARE_URI = "datashare:///com.acts.datasharetest2";
std::shared_ptr<DataShare::DataShareHelper> dataShareHelper;

class DataShareOpenFilePermissionTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void DataShareOpenFilePermissionTest::SetUpTestCase(void)
{
    LOG_INFO("SetUpTestCase invoked");
    int sleepTime = 1;
    sleep(sleepTime);
}

void DataShareOpenFilePermissionTest::TearDownTestCase(void)
{
    auto tokenId = AccessTokenKit::GetHapTokenID(100, "ohos.datashareclienttest.demo", 0);
    AccessTokenKit::DeleteToken(tokenId);
    dataShareHelper = nullptr;
}

void DataShareOpenFilePermissionTest::SetUp()
{
}

void DataShareOpenFilePermissionTest::TearDown()
{
}

std::shared_ptr<DataShare::DataShareHelper> CreateDataShareHelper(const std::string &uri)
{
    CreateOptions options;
    options.isProxy_ = false;
    auto saManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (saManager == nullptr) {
        return nullptr;
    }
    auto remoteObj = saManager->GetSystemAbility(STORAGE_MANAGER_MANAGER_ID);
    if (remoteObj == nullptr) {
        return nullptr;
    }
    options.token_ = remoteObj;
    return DataShare::DataShareHelper::Creator(uri, options);
}

/**
 * @tc.name: OpenFilePermissionTest001
 * @tc.desc: Test OpenFile with mode "r" when read permission is not granted - should return error.
 *           Since the hap application is not a pre-installed system app, it cannot obtain permissions,
 *           and all permission checks will be skipped. Therefore, a negative file descriptor (fd) is uniformly
 *           returned in the openfile implementation of the data provider app, resulting in a failed response
 * @tc.type: FUNC
 * @tc.require:
 * @tc.precon: No read permission granted.
 * @tc.step: Call OpenFile with mode "r" without read permission.
 * @tc.expect: Returns error code (ERR_INVALID_DATA).
 */
HWTEST_F(DataShareOpenFilePermissionTest, OpenFilePermissionTest001, TestSize.Level0)
{
    LOG_INFO("OpenFilePermissionTest001::Start");

    HapInfoParams info = { .userID = 100,
        .bundleName = "ohos.datashareclienttest.demo",
        .instIndex = 0,
        .appIDDesc = "ohos.datashareclienttest.demo",
        .isSystemApp = true };
    HapPolicyParams policy = { .apl = APL_SYSTEM_BASIC,
        .domain = "test.domain",
        .permList = {},
        .permStateList = {} };
    AccessTokenKit::AllocHapToken(info, policy);
    auto testTokenId = Security::AccessToken::AccessTokenKit::GetHapTokenIDEx(
        info.userID, info.bundleName, info.instIndex);
    SetSelfTokenID(testTokenId.tokenIDEx);

    dataShareHelper = CreateDataShareHelper(DATA_SHARE_URI);
    ASSERT_TRUE(dataShareHelper != nullptr);

    Uri uri(DATA_SHARE_URI);
    int result = dataShareHelper->OpenFile(uri, "r");

    EXPECT_EQ(result, ERR_INVALID_DATA);
    result = dataShareHelper->OpenRawFile(uri, "r");

    EXPECT_EQ(result, ERR_INVALID_DATA);
    LOG_INFO("OpenFilePermissionTest001::End");
}

/**
 * @tc.name: OpenFilePermissionTest002
 * @tc.desc: Test OpenFile with mode "w" when write permission is not granted - should return error.
 *           Since the hap application is not a pre-installed system app, it cannot obtain permissions,
 *           and all permission checks will be skipped. Therefore, a negative file descriptor (fd) is uniformly
 *           returned in the openfile implementation of the data provider app, resulting in a failed response
 * @tc.type: FUNC
 * @tc.require:
 * @tc.precon: No write permission granted.
 * @tc.step: Call OpenFile with mode "w" without write permission.
 * @tc.expect: Returns error code (ERR_INVALID_DATA).
 */
HWTEST_F(DataShareOpenFilePermissionTest, OpenFilePermissionTest002, TestSize.Level0)
{
    LOG_INFO("OpenFilePermissionTest002::Start");

    HapInfoParams info = { .userID = 100,
        .bundleName = "ohos.datashareclienttest.demo",
        .instIndex = 0,
        .appIDDesc = "ohos.datashareclienttest.demo",
        .isSystemApp = true };
    HapPolicyParams policy = { .apl = APL_SYSTEM_BASIC,
        .domain = "test.domain",
        .permList = {},
        .permStateList = {} };
    AccessTokenKit::AllocHapToken(info, policy);
    auto testTokenId = Security::AccessToken::AccessTokenKit::GetHapTokenIDEx(
        info.userID, info.bundleName, info.instIndex);
    SetSelfTokenID(testTokenId.tokenIDEx);

    dataShareHelper = CreateDataShareHelper(DATA_SHARE_URI);
    ASSERT_TRUE(dataShareHelper != nullptr);

    Uri uri(DATA_SHARE_URI);
    int result = dataShareHelper->OpenFile(uri, "w");

    EXPECT_EQ(result, ERR_INVALID_DATA);
    result = dataShareHelper->OpenRawFile(uri, "w");

    EXPECT_EQ(result, ERR_INVALID_DATA);
    LOG_INFO("OpenFilePermissionTest002::End");
}

/**
 * @tc.name: OpenFilePermissionTest003
 * @tc.desc: Test OpenFile with mode "rw" when write permission is not granted - should return error.
 *           Since the hap application is not a pre-installed system app, it cannot obtain permissions,
 *           and all permission checks will be skipped. Therefore, a negative file descriptor (fd) is uniformly
 *           returned in the openfile implementation of the data provider app, resulting in a failed response
 * @tc.type: FUNC
 * @tc.require:
 * @tc.precon: Only read permission granted, no write permission.
 * @tc.step: Call OpenFile with mode "rw" with only read permission.
 * @tc.expect: Returns error code (ERR_INVALID_DATA).
 */
HWTEST_F(DataShareOpenFilePermissionTest, OpenFilePermissionTest003, TestSize.Level0)
{
    LOG_INFO("OpenFilePermissionTest003::Start");

    HapInfoParams info = { .userID = 100,
        .bundleName = "ohos.datashareclienttest.demo",
        .instIndex = 0,
        .appIDDesc = "ohos.datashareclienttest.demo",
        .isSystemApp = true };
    HapPolicyParams policy = { .apl = APL_SYSTEM_BASIC,
        .domain = "test.domain",
        .permList = { { .permissionName = "ohos.permission.WRITE_CONTACTS",
            .bundleName = "ohos.datashareclienttest.demo",
            .grantMode = 1,
            .availableLevel = APL_SYSTEM_BASIC,
            .label = "label",
            .labelId = 1,
            .description = "ohos.datashareclienttest.demo",
            .descriptionId = 1 } },
        .permStateList = { { .permissionName = "ohos.permission.WRITE_CONTACTS",
            .isGeneral = true,
            .resDeviceID = { "local" },
            .grantStatus = { PermissionState::PERMISSION_GRANTED },
            .grantFlags = { 1 } } } };
    AccessTokenKit::AllocHapToken(info, policy);
    auto testTokenId = Security::AccessToken::AccessTokenKit::GetHapTokenIDEx(
        info.userID, info.bundleName, info.instIndex);
    SetSelfTokenID(testTokenId.tokenIDEx);

    dataShareHelper = CreateDataShareHelper(DATA_SHARE_URI);
    ASSERT_TRUE(dataShareHelper != nullptr);

    Uri uri(DATA_SHARE_URI);
    int result = dataShareHelper->OpenFile(uri, "rw");

    EXPECT_EQ(result, ERR_INVALID_DATA);
    result = dataShareHelper->OpenRawFile(uri, "rw");

    EXPECT_EQ(result, ERR_INVALID_DATA);
    LOG_INFO("OpenFilePermissionTest003::End");
}

/**
 * @tc.name: OpenFilePermissionTest004
 * @tc.desc: Test OpenFile with mode "rw" when write permission is granted
 *           but read permission is not - should return error.
 *           Since the hap application is not a pre-installed system app, it cannot obtain permissions,
 *           and all permission checks will be skipped. Therefore, a negative file descriptor (fd) is uniformly
 *           returned in the openfile implementation of the data provider app, resulting in a failed response
 * @tc.type: FUNC
 * @tc.require:
 * @tc.precon: Only write permission granted, no read permission.
 * @tc.step: Call OpenFile with mode "rw" with only write permission.
 * @tc.expect: Returns error code (ERR_INVALID_DATA).
 */
HWTEST_F(DataShareOpenFilePermissionTest, OpenFilePermissionTest004, TestSize.Level0)
{
    LOG_INFO("OpenFilePermissionTest004::Start");

    HapInfoParams info = { .userID = 100,
        .bundleName = "ohos.datashareclienttest.demo",
        .instIndex = 0,
        .appIDDesc = "ohos.datashareclienttest.demo",
        .isSystemApp = true };
    HapPolicyParams policy = { .apl = APL_SYSTEM_BASIC,
        .domain = "test.domain",
        .permList = { { .permissionName = "ohos.permission.GET_BUNDLE_INFO",
            .bundleName = "ohos.datashareclienttest.demo",
            .grantMode = 1,
            .availableLevel = APL_SYSTEM_BASIC,
            .label = "label",
            .labelId = 1,
            .description = "ohos.datashareclienttest.demo",
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

    dataShareHelper = CreateDataShareHelper(DATA_SHARE_URI);
    ASSERT_TRUE(dataShareHelper != nullptr);

    Uri uri(DATA_SHARE_URI);
    int result = dataShareHelper->OpenFile(uri, "rw");

    EXPECT_EQ(result, ERR_INVALID_DATA);
    result = dataShareHelper->OpenRawFile(uri, "rw");

    EXPECT_EQ(result, ERR_INVALID_DATA);
    LOG_INFO("OpenFilePermissionTest004::End");
}

/**
 * @tc.name: OpenFilePermissionTest005
 * @tc.desc: Test OpenFile with mode "rw" when both read and write permissions are granted.
 *           Since the hap application is not a pre-installed system app, it cannot obtain permissions,
 *           and all permission checks will be skipped. Therefore, a negative file descriptor (fd) is uniformly
 *           returned in the openfile implementation of the data provider app, resulting in a failed response
 * @tc.type: FUNC
 * @tc.require:
 * @tc.precon: Both read and write permissions are granted.
 * @tc.step: Call OpenFile with mode "rw" with both permissions.
 * @tc.expect: Returns error code (ERR_INVALID_DATA).
 */
HWTEST_F(DataShareOpenFilePermissionTest, OpenFilePermissionTest005, TestSize.Level0)
{
    LOG_INFO("OpenFilePermissionTest005::Start");

    HapInfoParams info = { .userID = 100,
        .bundleName = "ohos.datashareclienttest.demo",
        .instIndex = 0,
        .appIDDesc = "ohos.datashareclienttest.demo",
        .isSystemApp = true };
    HapPolicyParams policy = { .apl = APL_SYSTEM_BASIC,
        .domain = "test.domain",
        .permList = { { .permissionName = "ohos.permission.GET_BUNDLE_INFO",
            .bundleName = "ohos.datashareclienttest.demo",
            .grantMode = 1,
            .availableLevel = APL_SYSTEM_BASIC,
            .label = "label",
            .labelId = 1,
            .description = "ohos.datashareclienttest.demo",
            .descriptionId = 1 },
            { .permissionName = "ohos.permission.WRITE_CONTACTS",
            .bundleName = "ohos.datashareclienttest.demo",
            .grantMode = 1,
            .availableLevel = APL_SYSTEM_BASIC,
            .label = "label",
            .labelId = 1,
            .description = "ohos.datashareclienttest.demo",
            .descriptionId = 1 } },
        .permStateList = { { .permissionName = "ohos.permission.GET_BUNDLE_INFO",
            .isGeneral = true,
            .resDeviceID = { "local" },
            .grantStatus = { PermissionState::PERMISSION_GRANTED },
            .grantFlags = { 1 } },
            { .permissionName = "ohos.permission.WRITE_CONTACTS",
            .isGeneral = true,
            .resDeviceID = { "local" },
            .grantStatus = { PermissionState::PERMISSION_GRANTED },
            .grantFlags = { 1 } } } };
    AccessTokenKit::AllocHapToken(info, policy);
    auto testTokenId = Security::AccessToken::AccessTokenKit::GetHapTokenIDEx(
        info.userID, info.bundleName, info.instIndex);
    SetSelfTokenID(testTokenId.tokenIDEx);

    dataShareHelper = CreateDataShareHelper(DATA_SHARE_URI);
    ASSERT_TRUE(dataShareHelper != nullptr);

    Uri uri(DATA_SHARE_URI);
    int result = dataShareHelper->OpenFile(uri, "rw");

    EXPECT_EQ(result, ERR_INVALID_DATA);
    result = dataShareHelper->OpenRawFile(uri, "rw");

    EXPECT_EQ(result, ERR_INVALID_DATA);
    LOG_INFO("OpenFilePermissionTest005::End");
}
}
}