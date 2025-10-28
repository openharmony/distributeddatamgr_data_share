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

#define LOG_TAG "data_share_permission_test"

#include <gtest/gtest.h>
#include <unistd.h>
#include <cstdint>
#include <string>
#include "datashare_log.h"
#include "data_share_permission.h"
#include "datashare_errno.h"
#include "int_wrapper.h"
#include "want_params.h"
#include <memory>
#include "access_token.h"
#include "accesstoken_kit.h"
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
static const std::string TEST_BUNDLE_NAME = "com.acts.datasharetest";
static const std::string TEST_PERMISSION = "ohos.permission.GET_BUNDLE_INFO";
static const std::string TEST_BUNDLE_NAME1 = "com.acts.datasharetest1";
static const std::string TEST_BUNDLE_NAME2 = "com.acts.datasharetest2";
static const int32_t TEST_USERID = 100;

class DataSharePermissionTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void DataSharePermissionTest::SetUpTestCase(void)
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

void DataSharePermissionTest::TearDownTestCase(void)
{
    auto tokenId = AccessTokenKit::GetHapTokenIDEx(USER_100, "ohos.datashareclienttest.demo", 0);
    AccessTokenKit::DeleteToken(tokenId.tokenIDEx);
}

void DataSharePermissionTest::SetUp(void) {}
void DataSharePermissionTest::TearDown(void) {}

/**
 * @tc.name: CheckExtensionTrusts001
 * @tc.desc: Test the behavior of the DataSharePermission::VerifyPermission function when the input permission string
 *           is empty, verifying whether the permission check returns the expected result.
 * @tc.type: FUNC
 * @tc.require: issueICU06G
 * @tc.precon:
    1. The test environment supports calling the DataSharePermission::VerifyPermission static method, which accepts a
       uint32_t token ID and a std::string permission as input parameters.
    2. The DataSharePermission class is properly initialized and accessible in the test context.
    3. Predefined uint32_t values (for token ID) can be normally defined and passed to the method.
 * @tc.step:
    1. Define a uint32_t variable tokenID and initialize it to 123 (a valid test token ID).
    2. Define a std::string variable permission and initialize it to an empty string ("").
    3. Call DataSharePermission::VerifyPermission with tokenID and permission as parameters, and store the returned
       boolean result.
    4. Check whether the returned boolean result matches the expected value.
 * @tc.expect:
    1. The DataSharePermission::VerifyPermission function returns true when the permission string is empty.
 */
HWTEST_F(DataSharePermissionTest, VerifyPermissionTest001, TestSize.Level0)
{
    LOG_INFO("DataSharePermissionTest VerifyPermissionTest001::Start");
    uint32_t tokenID = 123;
    std::string permission = "";
    bool result = DataSharePermission::VerifyPermission(tokenID, permission);
    EXPECT_EQ(result, true);
    LOG_INFO("DataSharePermissionTest VerifyPermissionTest001::End");
}

/**
 * @tc.name: Init001
 * @tc.desc: Test the initialization-related behavior of DataSharePermission, focusing on the SubscribeCommonEvent
 *           method and whether the subscriber_ member is correctly initialized to a non-null value after the method
 *           call.
 * @tc.type: FUNC
 * @tc.require: issueICU06G
 * @tc.precon:
    1. The DataSharePermission class can be instantiated as a shared pointer via std::make_shared.
    2. The DataSharePermission class has a public SubscribeCommonEvent method and a public subscriber_ member
       (initially null).
    3. The SubscribeCommonEvent method is designed to initialize subscriber_ to a non-null value upon successful
       execution.
 * @tc.step:
    1. Create a shared pointer of DataSharePermission (permission) using std::make_shared<DataSharePermission>().
    2. Check that the initial value of permission->subscriber_ is nullptr (uninitialized state).
    3. Call the permission->SubscribeCommonEvent() method to execute the subscription logic.
    4. Check the value of permission->subscriber_ again after the method call.
 * @tc.expect:
    1. Before calling SubscribeCommonEvent, permission->subscriber_ is nullptr.
    2. After calling SubscribeCommonEvent, permission->subscriber_ is not nullptr.
 */
HWTEST_F(DataSharePermissionTest, Init001, TestSize.Level0)
{
    LOG_INFO("DataSharePermissionTest Init001::Start");
    auto permission = std::make_shared<DataSharePermission>();
    EXPECT_EQ(permission->subscriber_, nullptr);
    permission->SubscribeCommonEvent();
    EXPECT_NE(permission->subscriber_, nullptr);
    LOG_INFO("DataSharePermissionTest Init001::End");
}

/**
 * @tc.name: SysEventSubscriber001
 * @tc.desc: Test the OnReceiveEvent method of the SysEventSubscriber (inner class of DataSharePermission), verifying
 *           whether it correctly cleans up the silentCache_ when receiving a package-removed common event.
 * @tc.type: FUNC
 * @tc.require: issueICU06G
 * @tc.precon:
    1. The DataSharePermission class supports creating a SysEventSubscriber via SubscribeCommonEvent, and the
       subscriber_ member is accessible.
    2. The silentCache_ (a cache structure of DataSharePermission) supports Emplace (to add entries) and Size (to check
       entry count) methods.
    3. The test environment supports instantiation of EventFwk::CommonEventData, AAFwk::Want, and AAFwk::WantParams,
       and setting event parameters (action, bundle name, user ID).
    4. Predefined constants (TEST_BUNDLE_NAME, TEST_USERID) are valid and accessible.
 * @tc.step:
    1. Create a shared pointer of DataSharePermission (permission) and call permission->SubscribeCommonEvent() to
       initialize subscriber_ (verify subscriber_ is not null).
    2. Initialize a DataSharePermission::Permission object, set its bundleName to TEST_BUNDLE_NAME; create a UriKey
       with empty URI and TEST_USERID, then call permission->silentCache_.Emplace to add the pair to the cache.
    3. Verify that permission->silentCache_.Size() is 1 (cache has one entry).
    4. Construct a CommonEventData (event): set Want with action COMMON_EVENT_PACKAGE_REMOVED, element with
       TEST_BUNDLE_NAME, and WantParams with USER_ID = TEST_USERID.
    5. Call permission->subscriber_->OnReceiveEvent(event) to trigger the event handling logic.
    6. Check the size of permission->silentCache_ again after the event is processed.
 * @tc.expect:
    1. After adding the entry, permission->silentCache_.Size() is 1.
    2. After calling OnReceiveEvent, permission->silentCache_.Size() is 0 (cache is cleaned up).
 */
HWTEST_F(DataSharePermissionTest, SysEventSubscriber001, TestSize.Level0)
{
    LOG_INFO("DataSharePermissionTest SysEventSubscriber001::Start");
    auto permission = std::make_shared<DataSharePermission>();
    permission->SubscribeCommonEvent();
    EXPECT_NE(permission->subscriber_, nullptr);

    DataSharePermission::Permission permissionInfo;
    permissionInfo.bundleName = TEST_BUNDLE_NAME1;
    std::string uri = "";
    DataSharePermission::UriKey uriKey(uri, TEST_USERID);
    permission->silentCache_.Emplace(uriKey, permissionInfo);
    EXPECT_EQ(permission->silentCache_.Size(), 1);

    EventFwk::CommonEventData event;
    AAFwk::Want want;
    OHOS::AppExecFwk::ElementName element;
    element.SetBundleName(TEST_BUNDLE_NAME1);
    want.SetElement(element);
    AAFwk::WantParams wantParams;
    wantParams.SetParam(DataSharePermission::SysEventSubscriber::USER_ID, AAFwk::Integer::Box(TEST_USERID));
    want.SetParams(wantParams);
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED);
    event.SetWant(want);
    permission->subscriber_->OnReceiveEvent(event);
    EXPECT_EQ(permission->silentCache_.Size(), 0);
    LOG_INFO("DataSharePermissionTest SysEventSubscriber001::End");
}

/**
 * @tc.name: DeleteCache001
 * @tc.desc: Test the DeleteCache method of DataSharePermission, verifying whether it correctly cleans up both
 *           silentCache_ and extensionCache_ when the input bundle name matches the bundleName in the cache entries.
 * @tc.type: FUNC
 * @tc.require: issueICU06G
 * @tc.precon:
    1. The DataSharePermission class has DeleteCache method (accepts std::string bundle name), and two cache
       structures: silentCache_ and extensionCache_ (both support Emplace and Size methods).
    2. The DataSharePermission can be instantiated as a shared pointer, and SubscribeCommonEvent initializes
       subscriber_ (verified to be non-null).
    3. Predefined constants (TEST_BUNDLE_NAME, TEST_USERID) are valid and accessible.
 * @tc.step:
    1. Create a shared pointer of DataSharePermission (permission), call SubscribeCommonEvent (verify subscriber_ is
       not null).
    2. Initialize a DataSharePermission::Permission object with bundleName = TEST_BUNDLE_NAME; create a UriKey with
       empty URI and TEST_USERID.
    3. Call permission->silentCache_.Emplace and permission->extensionCache_.Emplace to add the UriKey-Permission pair
       to both caches.
    4. Verify that both caches have a size of 1 (each has one entry).
    5. Call permission->DeleteCache(TEST_BUNDLE_NAME) to execute the cache deletion logic.
    6. Check the sizes of silentCache_ and extensionCache_ again.
 * @tc.expect:
    1. Before calling DeleteCache, both silentCache_.Size() and extensionCache_.Size() are 1.
    2. After calling DeleteCache(TEST_BUNDLE_NAME), both caches have a size of 0 (entries are cleaned up).
 */
HWTEST_F(DataSharePermissionTest, DeleteCache001, TestSize.Level0)
{
    LOG_INFO("DataSharePermissionTest DeleteCache001::Start");
    auto permission = std::make_shared<DataSharePermission>();
    permission->SubscribeCommonEvent();
    EXPECT_NE(permission->subscriber_, nullptr);

    DataSharePermission::Permission permissionInfo;
    permissionInfo.bundleName = TEST_BUNDLE_NAME1;
    std::string uri = "";
    DataSharePermission::UriKey uriKey(uri, TEST_USERID);
    permission->silentCache_.Emplace(uriKey, permissionInfo);
    permission->extensionCache_.Emplace(uriKey, permissionInfo);
    EXPECT_EQ(permission->silentCache_.Size(), 1);
    EXPECT_EQ(permission->extensionCache_.Size(), 1);

    permission->DeleteCache(TEST_BUNDLE_NAME1);
    EXPECT_EQ(permission->silentCache_.Size(), 0);
    EXPECT_EQ(permission->extensionCache_.Size(), 0);
    LOG_INFO("DataSharePermissionTest DeleteCache001::End");
}

/**
 * @tc.name: DeleteCache002
 * @tc.desc: Test the DeleteCache method of DataSharePermission, verifying whether it leaves silentCache_ and
 *           extensionCache_ unchanged when the input bundle name does not match the bundleName in the cache entries.
 * @tc.type: FUNC
 * @tc.require: issueICU06G
 * @tc.precon:
    1. The DataSharePermission class has DeleteCache method (accepts std::string bundle name), and two cache
       structures: silentCache_ and extensionCache_ (both support Emplace and Size methods).
    2. The DataSharePermission can be instantiated as a shared pointer, and SubscribeCommonEvent initializes
       subscriber_ (verified to be non-null).
    3. Predefined constants (TEST_BUNDLE_NAME, TEST_BUNDLE_NAME2, TEST_USERID) are valid and accessible.
 * @tc.step:
    1. Create a shared pointer of DataSharePermission (permission), call SubscribeCommonEvent (verify subscriber_ is
       not null).
    2. Initialize a DataSharePermission::Permission object with bundleName = TEST_BUNDLE_NAME; create a UriKey with
       empty URI and TEST_USERID.
    3. Call permission->silentCache_.Emplace and permission->extensionCache_.Emplace to add the UriKey-Permission pair
       to both caches.
    4. Verify that both caches have a size of 1 (each has one entry).
    5. Call permission->DeleteCache(TEST_BUNDLE_NAME2) (bundle name does not match cache entries) to execute deletion.
    6. Check the sizes of silentCache_ and extensionCache_ again.
 * @tc.expect:
    1. Before calling DeleteCache, both silentCache_.Size() and extensionCache_.Size() are 1.
    2. After calling DeleteCache(TEST_BUNDLE_NAME2), both caches still have a size of 1 (entries are not cleaned up).
 */
HWTEST_F(DataSharePermissionTest, DeleteCache002, TestSize.Level0)
{
    LOG_INFO("DataSharePermissionTest DeleteCache002::Start");
    auto permission = std::make_shared<DataSharePermission>();
    permission->SubscribeCommonEvent();
    EXPECT_NE(permission->subscriber_, nullptr);

    DataSharePermission::Permission permissionInfo;
    permissionInfo.bundleName = TEST_BUNDLE_NAME1;
    std::string uri = "";
    DataSharePermission::UriKey uriKey(uri, TEST_USERID);
    permission->silentCache_.Emplace(uriKey, permissionInfo);
    permission->extensionCache_.Emplace(uriKey, permissionInfo);
    EXPECT_EQ(permission->silentCache_.Size(), 1);
    EXPECT_EQ(permission->extensionCache_.Size(), 1);

    permission->DeleteCache(TEST_BUNDLE_NAME2);
    EXPECT_EQ(permission->silentCache_.Size(), 1);
    EXPECT_EQ(permission->extensionCache_.Size(), 1);
    LOG_INFO("DataSharePermissionTest DeleteCache002::End");
}

/**
 * @tc.name: OnUpdate001
 * @tc.desc: Test the OnUpdate method of the SysEventSubscriber (inner class of DataSharePermission), verifying
 *           whether it correctly cleans up the silentCache_ when called with a matching bundle name.
 * @tc.type: FUNC
 * @tc.require: issueICU06G
 * @tc.precon:
    1. The DataSharePermission class supports creating a SysEventSubscriber via SubscribeCommonEvent, and the
       subscriber_ member has a public OnUpdate method (accepts std::string bundle name).
    2. The silentCache_ of DataSharePermission supports Emplace (to add entries) and Size (to check entry count)
       methods.
    3. Predefined constants (TEST_BUNDLE_NAME, TEST_USERID) are valid and accessible.
 * @tc.step:
    1. Create a shared pointer of DataSharePermission (permission), call SubscribeCommonEvent (verify subscriber_ is
       not null).
    2. Initialize a DataSharePermission::Permission object with bundleName = TEST_BUNDLE_NAME; create a UriKey with
       empty URI and TEST_USERID, then add the pair to silentCache_ via Emplace.
    3. Verify that permission->silentCache_.Size() is 1 (cache has one entry).
    4. Call permission->subscriber_->OnUpdate(TEST_BUNDLE_NAME) to trigger the update and cache cleanup logic.
    5. Check the size of permission->silentCache_ again after the method call.
 * @tc.expect:
    1. Before calling OnUpdate, permission->silentCache_.Size() is 1.
    2. After calling OnUpdate(TEST_BUNDLE_NAME), permission->silentCache_.Size() is 0 (cache is cleaned up).
 */
HWTEST_F(DataSharePermissionTest, OnUpdate001, TestSize.Level0)
{
    LOG_INFO("DataSharePermissionTest OnUpdate001::Start");
    auto permission = std::make_shared<DataSharePermission>();
    permission->SubscribeCommonEvent();
    EXPECT_NE(permission->subscriber_, nullptr);

    DataSharePermission::Permission permissionInfo;
    permissionInfo.bundleName = TEST_BUNDLE_NAME;
    std::string uri = "";
    DataSharePermission::UriKey uriKey(uri, TEST_USERID);
    permission->silentCache_.Emplace(uriKey, permissionInfo);
    EXPECT_EQ(permission->silentCache_.Size(), 1);

    permission->subscriber_->OnUpdate(TEST_BUNDLE_NAME);
    EXPECT_EQ(permission->silentCache_.Size(), 0);
    LOG_INFO("DataSharePermissionTest OnUpdate001::End");
}

/**
 * @tc.name: Uri_Empty_Test_001
 * @tc.desc: Verify that the VerifyPermission method in DataSharePermission returns an error when checking
 *           permissions with an empty URI and a read operation.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports calling AccessTokenKit::GetHapTokenIDEx to obtain a valid token ID (tokenIDEx)
       with the specified user (USER_100), bundle name ("ohos.datashareclienttest.demo"), and app ID (0).
    2. The EMPTY_URI constant is predefined as an empty string, enabling the creation of an empty Uri object.
    3. The DataShare::DataSharePermission class provides a valid VerifyPermission static method that accepts
       parameters: uint64_t (tokenIDEx), Uri, and bool (isRead, indicating read operation).
    4. The ERR_INVALID_VALUE constant is predefined as the expected error code for invalid URI scenarios.
 * @tc.step:
    1. Call AccessTokenKit::GetHapTokenIDEx with USER_100, "ohos.datashareclienttest.demo", and 0 to obtain
       a token ID object, then extract its tokenIDEx member.
    2. Create an empty Uri object using the predefined EMPTY_URI constant.
    3. Call DataShare::DataSharePermission::VerifyPermission, passing the extracted tokenIDEx, empty Uri,
       and true (indicating read operation).
    4. Check the error code returned by the VerifyPermission method.
 * @tc.expect:
    1. The VerifyPermission method returns ERR_INVALID_VALUE.
 */
HWTEST_F(DataSharePermissionTest, Uri_Empty_Test_001, TestSize.Level0)
{
    LOG_INFO("Uri_Scheme_Error_Test_001::Start");
    auto tokenId = AccessTokenKit::GetHapTokenIDEx(USER_100, "ohos.datashareclienttest.demo", 0);
    Uri uri(EMPTY_URI);
    auto ret = DataShare::DataSharePermission::VerifyPermission(tokenId.tokenIDEx, uri, true);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
    LOG_INFO("Uri_Scheme_Error_Test_001::End");
}

/**
 * @tc.name: Bundle_Name_Error_Test_001
 * @tc.desc: Verify that the VerifyPermission method in DataSharePermission returns an error when checking
 *           permissions with a URI containing an invalid bundle name and a read operation.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports calling AccessTokenKit::GetHapTokenIDEx to obtain a valid tokenIDEx with
       USER_100, "ohos.datashareclienttest.demo", and 0.
    2. The PROXY_ERROR_BUNDLE_URI constant is predefined as a URI string containing an invalid bundle name,
       enabling the creation of a valid Uri object.
    3. The DataShare::DataSharePermission::VerifyPermission method is valid and accepts tokenIDEx, Uri, and bool
       (isRead).
    4. The E_BUNDLE_NAME_NOT_EXIST constant is predefined as the expected error code for invalid bundle name scenarios.
 * @tc.step:
    1. Call AccessTokenKit::GetHapTokenIDEx with USER_100, "ohos.datashareclienttest.demo", and 0 to get
       a token ID object, then extract its tokenIDEx.
    2. Create a Uri object using the predefined PROXY_ERROR_BUNDLE_URI (with invalid bundle name).
    3. Call DataShare::DataSharePermission::VerifyPermission, passing tokenIDEx, the created Uri, and true (read
       operation).
    4. Check the returned error code of the VerifyPermission method.
 * @tc.expect:
    1. The VerifyPermission method returns E_BUNDLE_NAME_NOT_EXIST.
 */
HWTEST_F(DataSharePermissionTest, Bundle_Name_Error_Test_001, TestSize.Level0)
{
    LOG_INFO("Bundle_Name_Error_Test_001::Start");
    auto tokenId = AccessTokenKit::GetHapTokenIDEx(USER_100, "ohos.datashareclienttest.demo", 0);
    Uri uri(PROXY_ERROR_BUNDLE_URI);
    auto ret = DataShare::DataSharePermission::VerifyPermission(tokenId.tokenIDEx, uri, true);
    EXPECT_EQ(ret, E_BUNDLE_NAME_NOT_EXIST);
    LOG_INFO("Bundle_Name_Error_Test_001::End");
}

/**
 * @tc.name: Uri_Diff_ProxyData_Test_001
 * @tc.desc: Verify that the VerifyPermission method in DataSharePermission returns a success code when checking
 *           permissions with a URI pointing to non-existent proxy data and a read operation.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports calling AccessTokenKit::GetHapTokenIDEx to obtain a valid tokenIDEx with
       USER_100, "ohos.datashareclienttest.demo", and 0.
    2. The URI_DIFF_PROXY_DATA constant is predefined as a URI string pointing to non-existent proxy data,
       enabling the creation of a valid Uri object.
    3. The DataShare::DataSharePermission::VerifyPermission method is valid and accepts tokenIDEx, Uri, and bool
       (isRead).
    4. The E_OK constant is predefined as the expected success code for the permission check.
 * @tc.step:
    1. Call AccessTokenKit::GetHapTokenIDEx with USER_100, "ohos.datashareclienttest.demo", and 0 to obtain
       a token ID object, then extract its tokenIDEx.
    2. Create a Uri object using the predefined URI_DIFF_PROXY_DATA (non-existent proxy data).
    3. Call DataShare::DataSharePermission::VerifyPermission, passing tokenIDEx, the created Uri, and true (read
       operation).
    4. Check the returned error code of the VerifyPermission method.
 * @tc.expect:
    1. The VerifyPermission method returns E_OK.
 */
HWTEST_F(DataSharePermissionTest, Uri_Diff_ProxyData_Test_001, TestSize.Level1)
{
    LOG_INFO("Uri_Diff_ProxyData_Test_001::Start");
    auto tokenId = AccessTokenKit::GetHapTokenIDEx(USER_100, "ohos.datashareclienttest.demo", 0);
    Uri uri(URI_DIFF_PROXY_DATA);
    auto ret = DataShare::DataSharePermission::VerifyPermission(tokenId.tokenIDEx, uri, true);
    EXPECT_EQ(ret, E_OK);
    LOG_INFO("Uri_Diff_ProxyData_Test_001::End");
}

/**
 * @tc.name: ProxyUri_OK_Test_001
 * @tc.desc: Verify that the VerifyPermission method in DataSharePermission returns a success code when checking
 *           permissions with a valid proxy URI and a read operation.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports calling AccessTokenKit::GetHapTokenIDEx to obtain a valid tokenIDEx with
       USER_100, "ohos.datashareclienttest.demo", and 0.
    2. The PROXY_URI_OK constant is predefined as a valid proxy URI string, enabling the creation of a valid Uri
       object.
    3. The DataShare::DataSharePermission::VerifyPermission method is valid and accepts tokenIDEx, Uri, and bool
       (isRead).
    4. The E_OK constant is predefined as the expected success code for successful permission checks.
 * @tc.step:
    1. Call AccessTokenKit::GetHapTokenIDEx with USER_100, "ohos.datashareclienttest.demo", and 0 to get
       a token ID object, then extract its tokenIDEx.
    2. Create a Uri object using the predefined PROXY_URI_OK (valid proxy URI).
    3. Call DataShare::DataSharePermission::VerifyPermission, passing tokenIDEx, the created Uri, and true (read
       operation).
    4. Check the returned error code of the VerifyPermission method.
 * @tc.expect:
    1. The VerifyPermission method returns E_OK.
 */
HWTEST_F(DataSharePermissionTest, ProxyUri_OK_Test_001, TestSize.Level1)
{
    LOG_INFO("ProxyUri_OK_Test_001::Start");
    auto tokenId = AccessTokenKit::GetHapTokenIDEx(USER_100, "ohos.datashareclienttest.demo", 0);
    Uri uri(PROXY_URI_OK);
    auto ret = DataShare::DataSharePermission::VerifyPermission(tokenId.tokenIDEx, uri, true);
    EXPECT_EQ(ret, E_OK);
    LOG_INFO("ProxyUri_OK_Test_001::End");
}

/**
 * @tc.name: ProxyUri_OK_Write_Permission_Error_Test_001
 * @tc.desc: Verify that the VerifyPermission method in DataSharePermission returns a permission-denied error when
 *           checking permissions with a valid proxy URI and a write operation (without required write permission).
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports calling AccessTokenKit::GetHapTokenIDEx to obtain a valid tokenIDEx with
       USER_100, "ohos.datashareclienttest.demo", and 0 (this token lacks write permission for the valid proxy URI).
    2. The PROXY_URI_OK constant is predefined as a valid proxy URI string, enabling the creation of a valid Uri
       object.
    3. The DataShare::DataSharePermission::VerifyPermission method is valid and accepts tokenIDEx, Uri, and bool
       (isRead,
       false indicating write operation).
    4. The ERR_PERMISSION_DENIED constant is predefined as the expected error code for insufficient write permission.
 * @tc.step:
    1. Call AccessTokenKit::GetHapTokenIDEx with USER_100, "ohos.datashareclienttest.demo", and 0 to obtain
       a token ID object, then extract its tokenIDEx.
    2. Create a Uri object using the predefined PROXY_URI_OK (valid proxy URI).
    3. Call DataShare::DataSharePermission::VerifyPermission, passing tokenIDEx, the created Uri, and false (indicating
       write operation).
    4. Check the returned error code of the VerifyPermission method.
 * @tc.expect:
    1. The VerifyPermission method returns ERR_PERMISSION_DENIED.
 */
HWTEST_F(DataSharePermissionTest, ProxyUri_OK_Write_Permission_Error_Test_001, TestSize.Level1)
{
    LOG_INFO("ProxyUri_OK_Write_Permission_Error_Test_001::Start");
    auto tokenId = AccessTokenKit::GetHapTokenIDEx(USER_100, "ohos.datashareclienttest.demo", 0);
    Uri uri(PROXY_URI_OK);
    // isRead is false, verify write permission
    auto ret = DataShare::DataSharePermission::VerifyPermission(tokenId.tokenIDEx, uri, false);
    EXPECT_EQ(ret, ERR_PERMISSION_DENIED);
    LOG_INFO("ProxyUri_OK_Write_Permission_Error_Test_001::End");
}

/**
 * @tc.name: Error_Bundle_Name_Test_001
 * @tc.desc: Verify that the VerifyPermission method in DataSharePermission returns an error when checking
 *           permissions for a datashare URI containing an invalid bundle name and a read operation.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports calling AccessTokenKit::GetHapTokenIDEx to obtain a valid token ID (tokenIDEx)
       with the specified user (USER_100), bundle name ("ohos.datashareclienttest.demo"), and app ID (0).
    2. The DATA_SHARE_ERROR_BUNDLE_URI constant is predefined as a datashare URI string containing an invalid
       bundle name, enabling the creation of a valid Uri object.
    3. The DataShare::DataSharePermission class provides a valid VerifyPermission static method that accepts
       parameters: uint64_t (tokenIDEx), Uri, and bool (isRead, indicating read operation).
    4. The E_BUNDLE_NAME_NOT_EXIST constant is predefined as the expected error code for invalid bundle name scenarios.
 * @tc.step:
    1. Call AccessTokenKit::GetHapTokenIDEx with USER_100, "ohos.datashareclienttest.demo", and 0 to obtain
       a token ID object, then extract its tokenIDEx member.
    2. Create a Uri object using the predefined DATA_SHARE_ERROR_BUNDLE_URI (datashare URI with invalid bundle name).
    3. Call DataShare::DataSharePermission::VerifyPermission, passing the extracted tokenIDEx, created Uri,
       and true (indicating read operation).
    4. Check the error code returned by the VerifyPermission method.
 * @tc.expect:
    1. The VerifyPermission method returns E_BUNDLE_NAME_NOT_EXIST.
 */
HWTEST_F(DataSharePermissionTest, Error_Bundle_Name_Test_001, TestSize.Level0)
{
    LOG_INFO("Error_Bundle_Name_Test_001::Start");
    auto tokenId = AccessTokenKit::GetHapTokenIDEx(USER_100, "ohos.datashareclienttest.demo", 0);
    Uri uri(DATA_SHARE_ERROR_BUNDLE_URI);
    auto ret = DataShare::DataSharePermission::VerifyPermission(tokenId.tokenIDEx, uri, true);
    EXPECT_EQ(ret, E_BUNDLE_NAME_NOT_EXIST);
    LOG_INFO("Error_Bundle_Name_Test_001::End");
}

/**
 * @tc.name: No_Read_Permission_Test_001
 * @tc.desc: Verify that the VerifyPermission method in DataSharePermission returns a permission-denied error when
 *           checking permissions for a read operation on a valid datashare URI, where the token lacks the required
 *           read permission.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports calling AccessTokenKit::GetHapTokenIDEx to obtain a token ID (tokenIDEx) with
       USER_100, "ohos.datashareclienttest.demo", and 0—this token does NOT have the required read permission
       (ohos.permission.READ_CALL_LOG) for the target URI.
    2. The DATA_SHARE_URI constant is predefined as a valid datashare URI whose associated proxyData requires
       the "ohos.permission.READ_CALL_LOG" read permission.
    3. The DataShare::DataSharePermission::VerifyPermission method is valid and accepts tokenIDEx, Uri, and bool
       (isRead).
    4. The ERR_PERMISSION_DENIED constant is predefined as the expected error code for insufficient read permission.
 * @tc.step:
    1. Call AccessTokenKit::GetHapTokenIDEx with USER_100, "ohos.datashareclienttest.demo", and 0 to get
       a token ID object, then extract its tokenIDEx.
    2. Create a Uri object using the predefined DATA_SHARE_URI (valid datashare URI requiring read permission).
    3. Call DataShare::DataSharePermission::VerifyPermission, passing tokenIDEx, the created Uri, and true (read
       operation).
    4. Check the returned error code of the VerifyPermission method.
 * @tc.expect:
    1. The VerifyPermission method returns ERR_PERMISSION_DENIED.
 */
HWTEST_F(DataSharePermissionTest, No_Read_Permission_Test_001, TestSize.Level1)
{
    LOG_INFO("No_Read_Permission_Test_001::Start");
    auto tokenId = AccessTokenKit::GetHapTokenIDEx(USER_100, "ohos.datashareclienttest.demo", 0);
    Uri uri(DATA_SHARE_URI);
    // proxyData requiredReadPermission is ohos.permission.READ_CALL_LOG
    auto ret = DataShare::DataSharePermission::VerifyPermission(tokenId.tokenIDEx, uri, true);
    EXPECT_EQ(ret, ERR_PERMISSION_DENIED);
    LOG_INFO("No_Read_Permission_Test_001::End");
}

/**
 * @tc.name: Have_Write_Permission_Test_001
 * @tc.desc: Verify that the VerifyPermission method in DataSharePermission returns a success code when checking
 *           permissions for a write operation on a valid datashare URI, where the token has the required write
 *           permission.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports calling AccessTokenKit::GetHapTokenIDEx to obtain a token ID (tokenIDEx) with
       USER_100, "ohos.datashareclienttest.demo", and 0—this token HAS the required write permission
       (ohos.permission.WRITE_CALL_LOG) for the target URI.
    2. The DATA_SHARE_WRITEURI constant is predefined as a valid datashare URI whose associated proxyData requires
       the "ohos.permission.WRITE_CALL_LOG" write permission.
    3. The DataShare::DataSharePermission::VerifyPermission method is valid and accepts tokenIDEx, Uri, and bool
       (isRead,
       false indicating write operation).
    4. The E_OK constant is predefined as the expected success code for sufficient write permission.
 * @tc.step:
    1. Call AccessTokenKit::GetHapTokenIDEx with USER_100, "ohos.datashareclienttest.demo", and 0 to obtain
       a token ID object, then extract its tokenIDEx.
    2. Create a Uri object using the predefined DATA_SHARE_WRITEURI (valid datashare URI requiring write permission).
    3. Call DataShare::DataSharePermission::VerifyPermission, passing tokenIDEx, the created Uri, and false (write
       operation).
    4. Check the returned error code of the VerifyPermission method.
 * @tc.expect:
    1. The VerifyPermission method returns E_OK.
 */
HWTEST_F(DataSharePermissionTest, Have_Write_Permission_Test_001, TestSize.Level1)
{
    LOG_INFO("HAVA_WRITE_PERMISSION_Test_001::Start");
    auto tokenId = AccessTokenKit::GetHapTokenIDEx(USER_100, "ohos.datashareclienttest.demo", 0);
    Uri uri(DATA_SHARE_WRITEURI);
    // proxyData requiredWritePermission is ohos.permission.WRITE_CALL_LOG
    auto ret = DataShare::DataSharePermission::VerifyPermission(tokenId.tokenIDEx, uri, false);
    EXPECT_EQ(ret, E_OK);
    LOG_INFO("HAVA_WRITE_PERMISSION_Test_001::End");
}

/**
 * @tc.name: Empty_Read_Permission_Test_001
 * @tc.desc: Verify that the VerifyPermission method in DataSharePermission returns a permission-denied error when
 *           checking permissions for a read operation on a datashare URI whose associated proxyData has no configured
 *           read permission.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports calling AccessTokenKit::GetHapTokenIDEx to obtain a valid tokenIDEx with
       USER_100, "ohos.datashareclienttest.demo", and 0.
    2. The DATA_SHARE_WRITEURI constant is predefined as a datashare URI whose associated proxyData does NOT have
       any configured required read permission (requiredReadPermission is unconfigured).
    3. The DataShare::DataSharePermission::VerifyPermission method is valid and accepts tokenIDEx, Uri, and bool
       (isRead).
    4. The ERR_PERMISSION_DENIED constant is predefined as the expected error code for unconfigured read permission.
 * @tc.step:
    1. Call AccessTokenKit::GetHapTokenIDEx with USER_100, "ohos.datashareclienttest.demo", and 0 to get
       a token ID object, then extract its tokenIDEx.
    2. Create a Uri object using the predefined DATA_SHARE_WRITEURI (datashare URI with unconfigured read permission).
    3. Call DataShare::DataSharePermission::VerifyPermission, passing tokenIDEx, the created Uri, and true
       (read operation).
    4. Check the returned error code of the VerifyPermission method.
 * @tc.expect:
    1. The VerifyPermission method returns ERR_PERMISSION_DENIED.
 */
HWTEST_F(DataSharePermissionTest, Empty_Read_Permission_Test_001, TestSize.Level1)
{
    LOG_INFO("Empty_Read_Permission_Test_001::Start");
    auto tokenId = AccessTokenKit::GetHapTokenIDEx(USER_100, "ohos.datashareclienttest.demo", 0);
    Uri uri(DATA_SHARE_WRITEURI);
    // proxyData not config requiredReadPermission
    auto ret = DataShare::DataSharePermission::VerifyPermission(tokenId.tokenIDEx, uri, true);
    EXPECT_EQ(ret, ERR_PERMISSION_DENIED);
    LOG_INFO("Empty_Read_Permission_Test_001::End");
}

/**
 * @tc.name: Empty_Write_Permission_Test_001
 * @tc.desc: Verify that the VerifyPermission method in DataSharePermission returns a permission-denied error when
 *           checking permissions for a write operation on a datashare URI whose associated proxyData has no configured
 *           write permission.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports calling AccessTokenKit::GetHapTokenIDEx to obtain a valid token ID (tokenIDEx)
       with USER_100, "ohos.datashareclienttest.demo", and 0.
    2. The DATA_SHARE_URI constant is predefined as a valid datashare URI whose associated proxyData does NOT have
       any configured required write permission (requiredWritePermission is unconfigured).
    3. The DataShare::DataSharePermission::VerifyPermission method is valid and accepts parameters: uint64_t
       (tokenIDEx),
       Uri, and bool (isRead, false indicating write operation).
    4. The ERR_PERMISSION_DENIED constant is predefined as the expected error code for unconfigured write permission.
 * @tc.step:
    1. Call AccessTokenKit::GetHapTokenIDEx with USER_100, "ohos.datashareclienttest.demo", and 0 to obtain a
       token ID object, then extract its tokenIDEx member.
    2. Create a Uri object using the predefined DATA_SHARE_URI (datashare URI with unconfigured write permission).
    3. Call DataShare::DataSharePermission::VerifyPermission, passing tokenIDEx, the created Uri, and false (indicating
       write operation).
    4. Check the error code returned by the VerifyPermission method.
 * @tc.expect:
    1. The VerifyPermission method returns ERR_PERMISSION_DENIED.
 */
HWTEST_F(DataSharePermissionTest, Empty_Write_Permission_Test_001, TestSize.Level1)
{
    LOG_INFO("Empty_Write_Permission_Test_001::Start");
    auto tokenId = AccessTokenKit::GetHapTokenIDEx(USER_100, "ohos.datashareclienttest.demo", 0);
    Uri uri(DATA_SHARE_URI);
    // proxyData not config requiredWritePermission
    auto ret = DataShare::DataSharePermission::VerifyPermission(tokenId.tokenIDEx, uri, false);
    EXPECT_EQ(ret, ERR_PERMISSION_DENIED);
    LOG_INFO("Empty_Write_Permission_Test_001::End");
}

/**
 * @tc.name: Have_Query_Param_001
 * @tc.desc: Verify that the VerifyPermission method in DataSharePermission returns a success code when checking
 *           permissions for a read operation on a valid proxy URI that contains query parameters.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports calling AccessTokenKit::GetHapTokenIDEx to obtain a valid tokenIDEx with
       USER_100, "ohos.datashareclienttest.demo", and 0—this token has the required read permission for the target URI.
    2. The PROXY_URI_HAVA_QUERY constant is predefined as a valid proxy URI string that includes query parameters,
       enabling the creation of a valid Uri object.
    3. The DataShare::DataSharePermission::VerifyPermission method is valid and accepts tokenIDEx, Uri, and bool
       (isRead).
    4. The E_OK constant is predefined as the expected success code for successful permission checks.
 * @tc.step:
    1. Call AccessTokenKit::GetHapTokenIDEx with USER_100, "ohos.datashareclienttest.demo", and 0 to get a
       token ID object, then extract its tokenIDEx.
    2. Create a Uri object using the predefined PROXY_URI_HAVA_QUERY (valid proxy URI with query parameters).
    3. Call DataShare::DataSharePermission::VerifyPermission, passing tokenIDEx, the created Uri, and true (read
       operation).
    4. Check the error code returned by the VerifyPermission method.
 * @tc.expect:
    1. The VerifyPermission method returns E_OK.
 */
HWTEST_F(DataSharePermissionTest, Have_Query_Param_001, TestSize.Level1)
{
    LOG_INFO("Have_Query_Param_001::Start");
    auto tokenId = AccessTokenKit::GetHapTokenIDEx(USER_100, "ohos.datashareclienttest.demo", 0);
    Uri uri(PROXY_URI_HAVA_QUERY);
    auto ret = DataShare::DataSharePermission::VerifyPermission(tokenId.tokenIDEx, uri, true);
    EXPECT_EQ(ret, E_OK);
    LOG_INFO("Have_Query_Param_001::End");
}

/**
 * @tc.name: DataObs_GetUriPermission_Uri_Empty_Test_001
 * @tc.desc: Verify that the GetUriPermission method in DataSharePermission returns an empty URI error when called
 *           with an empty URI, a specified user, read operation flag, and silent access flag.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports instantiation of the DataShare::DataSharePermission class via std::make_shared.
    2. The EMPTY_URI constant is predefined as an empty string, enabling the creation of an empty Uri object.
    3. The GetUriPermission method of DataSharePermission is valid and accepts parameters: Uri, int (userID), bool
       (isRead), bool (isSilent), returning a std::pair<int, std::string> (error code, permission).
    4. The E_EMPTY_URI constant is predefined as the expected error code for empty URI scenarios.
 * @tc.step:
    1. Create an empty Uri object using the predefined EMPTY_URI constant.
    2. Define a bool variable isSilent and initialize it to true (silent access mode).
    3. Use std::make_shared to create a DataShare::DataSharePermission instance (datashare).
    4. Call datashare->GetUriPermission, passing the empty Uri, USER_100, true (read operation), and isSilent;
       store the returned std::pair<int, std::string> (ret, permission).
    5. Check the error code (ret) from the returned pair.
 * @tc.expect:
    1. The error code (ret) returned by GetUriPermission is E_EMPTY_URI.
 */
HWTEST_F(DataSharePermissionTest, DataObs_GetUriPermission_Uri_Empty_Test_001, TestSize.Level0)
{
    LOG_INFO("DataObs_GetUriPermission_Uri_Empty_Test_001::Start");
    Uri uri(EMPTY_URI);
    bool isSilent = true;
    auto datashare = std::make_shared<DataShare::DataSharePermission>();
    auto [ret, permission] = datashare->GetUriPermission(uri, USER_100, true, isSilent);
    EXPECT_EQ(ret, E_EMPTY_URI);
    LOG_INFO("DataObs_GetUriPermission_Uri_Empty_Test_001::End");
}

/**
 * @tc.name: DataObs_GetUriPermission_Uri_Error_Test_001
 * @tc.desc: Verify that the GetUriPermission method in DataSharePermission returns a URI-not-exist error when called
 *           with an invalid proxy URI (PROXY_ERROR_BUNDLE_URI), specified user, read operation flag, and silent access
 *           flag.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports instantiation of DataShare::DataSharePermission via std::make_shared.
    2. The PROXY_ERROR_BUNDLE_URI constant is predefined as an invalid proxy URI string (non-existent), enabling
       the creation of a Uri object.
    3. The DataSharePermission::GetUriPermission method is valid, accepts Uri, int (userID), bool (isRead), bool
       (isSilent), and returns a std::pair<int, std::string> (error code, permission).
    4. The E_URI_NOT_EXIST constant is predefined as the expected error code for non-existent URI scenarios.
 * @tc.step:
    1. Create a Uri object using the predefined PROXY_ERROR_BUNDLE_URI (invalid/non-existent proxy URI).
    2. Define a bool variable isSilent and initialize it to true.
    3. Create a DataShare::DataSharePermission instance (datashare) via std::make_shared.
    4. Call datashare->GetUriPermission with the invalid Uri, USER_100, true (read operation), and isSilent;
       store the returned (ret, permission) pair.
    5. Check the error code (ret) from the pair.
 * @tc.expect:
    1. The error code (ret) returned by GetUriPermission is E_URI_NOT_EXIST.
 */
HWTEST_F(DataSharePermissionTest, DataObs_GetUriPermission_Uri_Error_Test_001, TestSize.Level0)
{
    LOG_INFO("DataObs_GetUriPermission_Uri_Error_Test_001::Start");
    Uri uri(PROXY_ERROR_BUNDLE_URI);
    bool isSilent = true;
    auto datashare = std::make_shared<DataShare::DataSharePermission>();
    auto [ret, permission] = datashare->GetUriPermission(uri, USER_100, true, isSilent);
    EXPECT_EQ(ret, E_URI_NOT_EXIST);
    LOG_INFO("DataObs_GetUriPermission_Uri_Error_Test_001::End");
}

/**
 * @tc.name: DataObs_GetUriPermission_Uri_OK_Test_001
 * @tc.desc: Verify that the GetUriPermission method in DataSharePermission returns a success code and the correct
 *           permission when called with a valid proxy URI (PROXY_URI_OK), specified user, read operation flag, and
 *           silent access flag.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports instantiation of DataShare::DataSharePermission via std::make_shared.
    2. The PROXY_URI_OK constant is predefined as a valid proxy URI string, whose associated permission is
       "ohos.permission.GET_BUNDLE_INFO".
    3. The DataSharePermission::GetUriPermission method is valid, accepts Uri, int (userID), bool (isRead), bool
       (isSilent),
       and returns a std::pair<int, std::string> (error code, permission).
    4. The E_OK constant is predefined as the expected success code; the target permission
       "ohos.permission.GET_BUNDLE_INFO" is predefined and valid.
 * @tc.step:
    1. Create a Uri object using the predefined PROXY_URI_OK (valid proxy URI).
    2. Define a bool variable isSilent and initialize it to true.
    3. Create a DataShare::DataSharePermission instance (datashare) via std::make_shared.
    4. Call datashare->GetUriPermission with the valid Uri, USER_100, true (read operation), and isSilent;
       store the returned (ret, permission) pair.
    5. Check both the error code (ret) and the permission string from the pair.
 * @tc.expect:
    1. The error code (ret) returned by GetUriPermission is E_OK.
    2. The permission string returned by GetUriPermission is "ohos.permission.GET_BUNDLE_INFO".
 */
HWTEST_F(DataSharePermissionTest, DataObs_GetUriPermission_Uri_OK_Test_001, TestSize.Level1)
{
    LOG_INFO("DataObs_GetUriPermission_Uri_OK_Test_001::Start");
    Uri uri(PROXY_URI_OK);
    bool isSilent = true;
    auto datashare = std::make_shared<DataShare::DataSharePermission>();
    auto [ret, permission] = datashare->GetUriPermission(uri, USER_100, true, isSilent);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(permission, "ohos.permission.GET_BUNDLE_INFO");
    LOG_INFO("DataObs_GetUriPermission_Uri_OK_Test_001::End");
}

/**
 * @tc.name: DataObs_GetUriPermission_Uri_OK_Test_002
 * @tc.desc: Verify that the GetUriPermission method in DataSharePermission returns a success code and the correct
 *           write permission when called with a valid proxy URI (PROXY_URI_OK), specified user, write operation flag
 *           (isRead=false), and silent access flag.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports instantiation of the DataShare::DataSharePermission class via std::make_shared.
    2. The PROXY_URI_OK constant is predefined as a valid proxy URI string, whose associated write permission is
       "ohos.permission.WRITE_CONTACTS".
    3. The GetUriPermission method of DataSharePermission is valid and accepts parameters: Uri, int (userID), bool
       (isRead), bool (isSilent), returning a std::pair<int, std::string> (error code, permission).
    4. The E_OK constant is predefined as the expected success code for valid operations.
 * @tc.step:
    1. Create a Uri object using the predefined PROXY_URI_OK (valid proxy URI for write operation).
    2. Define a bool variable isSilent and initialize it to true (silent access mode).
    3. Use std::make_shared to create a DataShare::DataSharePermission instance (datashare).
    4. Call datashare->GetUriPermission, passing the valid Uri, USER_100, false (write operation), and isSilent;
       store the returned std::pair<int, std::string> (ret, permission).
    5. Check both the error code (ret) and the permission string from the returned pair.
 * @tc.expect:
    1. The error code (ret) returned by GetUriPermission is E_OK.
    2. The permission string returned by GetUriPermission is "ohos.permission.WRITE_CONTACTS".
 */
HWTEST_F(DataSharePermissionTest, DataObs_GetUriPermission_Uri_OK_Test_002, TestSize.Level1)
{
    LOG_INFO("DataObs_GetUriPermission_Uri_OK_Test_002::Start");
    Uri uri(PROXY_URI_OK);
    bool isSilent = true;
    auto datashare = std::make_shared<DataShare::DataSharePermission>();
    auto [ret, permission] = datashare->GetUriPermission(uri, USER_100, false, isSilent);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(permission, "ohos.permission.WRITE_CONTACTS");
    LOG_INFO("DataObs_GetUriPermission_Uri_OK_Test_002::End");
}

/**
 * @tc.name: DataObs_GetUriPermission_Uri_OK_Test_003
 * @tc.desc: Verify that the GetUriPermission method in DataSharePermission returns a success code and an empty
 *           permission string when called with a valid DATA_SHARE_EXTENSION_URI, specified user, read operation flag,
 *           and silent access flag.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports instantiation of DataShare::DataSharePermission via std::make_shared.
    2. The DATA_SHARE_EXTENSION_URI constant is predefined as a valid URI string with no configured read permission.
    3. The DataSharePermission::GetUriPermission method is valid and returns a std::pair<int, std::string> (error code,
       permission) for the input parameters.
    4. The E_OK constant is predefined as the expected success code for valid URI processing.
 * @tc.step:
    1. Create a Uri object using the predefined DATA_SHARE_EXTENSION_URI (valid URI with no read permission).
    2. Define a bool variable isSilent and initialize it to true.
    3. Create a DataShare::DataSharePermission instance (datashare) via std::make_shared.
    4. Call datashare->GetUriPermission with the URI, USER_100, true (read operation), and isSilent; store the
       returned (ret, permission) pair.
    5. Check the error code (ret) and the permission string from the pair.
 * @tc.expect:
    1. The error code (ret) returned by GetUriPermission is E_OK.
    2. The permission string returned by GetUriPermission is empty ("").
 */
HWTEST_F(DataSharePermissionTest, DataObs_GetUriPermission_Uri_OK_Test_003, TestSize.Level1)
{
    LOG_INFO("DataObs_GetUriPermission_Uri_OK_Test_003::Start");
    Uri uri(DATA_SHARE_EXTENSION_URI);
    bool isSilent = true;
    auto datashare = std::make_shared<DataShare::DataSharePermission>();
    auto [ret, permission] = datashare->GetUriPermission(uri, USER_100, true, isSilent);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(permission, "");
    LOG_INFO("DataObs_GetUriPermission_Uri_OK_Test_003::End");
}

/**
 * @tc.name: DataObs_GetUriPermission_Uri_OK_Test_004
 * @tc.desc: Verify that the GetUriPermission method in DataSharePermission returns a success code and an empty
 *           permission string when called with a valid DATA_SHARE_EXTENSION_URI, specified user, write operation flag,
 *           and silent access flag.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports instantiation of DataShare::DataSharePermission via std::make_shared.
    2. The DATA_SHARE_EXTENSION_URI constant is predefined as a valid URI string with no configured write permission.
    3. The DataSharePermission::GetUriPermission method is valid and returns a std::pair<int, std::string> (error code,
       permission) for the input parameters.
    4. The E_OK constant is predefined as the expected success code for valid URI processing.
 * @tc.step:
    1. Create a Uri object using the predefined DATA_SHARE_EXTENSION_URI (valid URI with no write permission).
    2. Define a bool variable isSilent and initialize it to true.
    3. Create a DataShare::DataSharePermission instance (datashare) via std::make_shared.
    4. Call datashare->GetUriPermission with the URI, USER_100, false (write operation), and isSilent; store the
       returned (ret, permission) pair.
    5. Check the error code (ret) and the permission string from the pair.
 * @tc.expect:
    1. The error code (ret) returned by GetUriPermission is E_OK.
    2. The permission string returned by GetUriPermission is empty ("").
 */
HWTEST_F(DataSharePermissionTest, DataObs_GetUriPermission_Uri_OK_Test_004, TestSize.Level1)
{
    LOG_INFO("DataObs_GetUriPermission_Uri_OK_Test_004::Start");
    Uri uri(DATA_SHARE_EXTENSION_URI);
    bool isSilent = true;
    auto datashare = std::make_shared<DataShare::DataSharePermission>();
    auto [ret, permission] = datashare->GetUriPermission(uri, USER_100, false, isSilent);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(permission, "");
    LOG_INFO("DataObs_GetUriPermission_Uri_OK_Test_004::End");
}

/**
 * @tc.name: DataObs_VerifyPermission_Test_001
 * @tc.desc: Verify that the static VerifyPermission method in DataSharePermission returns true when checking a valid
 *           proxy URI (PROXY_URI_OK), a valid token ID, and the required permission ("ohos.permission.GET_BUNDLE_INFO")
 *           for a read operation.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports calling AccessTokenKit::GetHapTokenIDEx to obtain a valid token ID (tokenIDEx)
       with USER_100, "ohos.datashareclienttest.demo", and 0—this token has the "ohos.permission.GET_BUNDLE_INFO"
       permission.
    2. The PROXY_URI_OK constant is predefined as a valid proxy URI string that matches the permission check logic.
    3. The DataShare::DataSharePermission::VerifyPermission static method is valid and accepts parameters: Uri,
       uint64_t (tokenIDEx), std::string (permission), bool (isRead), returning a bool (permission check result).
 * @tc.step:
    1. Call AccessTokenKit::GetHapTokenIDEx with USER_100, "ohos.datashareclienttest.demo", and 0 to obtain a
       token ID object, then extract its tokenIDEx member.
    2. Create a Uri object using the predefined PROXY_URI_OK (valid proxy URI).
    3. Define a std::string variable permission and initialize it to "ohos.permission.GET_BUNDLE_INFO".
    4. Call DataShare::DataSharePermission::VerifyPermission, passing the Uri, tokenIDEx, permission, and true (read
       operation).
    5. Check the boolean return value of the VerifyPermission method.
 * @tc.expect:
    1. The VerifyPermission method returns true (permission check passes).
 */
HWTEST_F(DataSharePermissionTest, DataObs_VerifyPermission_Test_001, TestSize.Level1)
{
    LOG_INFO("DataObs_VerifyPermission_Test_001::Start");
    auto tokenId = AccessTokenKit::GetHapTokenIDEx(USER_100, "ohos.datashareclienttest.demo", 0);
    Uri uri(PROXY_URI_OK);
    std::string permission = "ohos.permission.GET_BUNDLE_INFO";
    auto ret = DataShare::DataSharePermission::VerifyPermission(uri, tokenId.tokenIDEx, permission, true);
    EXPECT_EQ(ret, true);
    LOG_INFO("DataObs_VerifyPermission_Test_001::End");
}

/**
 * @tc.name: DataObs_VerifyPermission_Test_002
 * @tc.desc: Verify that the static VerifyPermission method in DataSharePermission returns false when checking a valid
 *           proxy URI (PROXY_URI_OK), a valid token ID, and an unowned permission ("ohos.permission.WIFI") for a read
 *           operation.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports calling AccessTokenKit::GetHapTokenIDEx to obtain a valid token ID (tokenIDEx)
       with USER_100, "ohos.datashareclienttest.demo", and 0—this token DOES NOT have the "ohos.permission.WIFI"
       permission.
    2. The PROXY_URI_OK constant is predefined as a valid proxy URI string that matches the permission check logic.
    3. The DataShare::DataSharePermission::VerifyPermission static method is valid and accepts parameters: Uri,
       uint64_t (tokenIDEx), std::string (permission), bool (isRead), returning a bool (permission check result).
 * @tc.step:
    1. Call AccessTokenKit::GetHapTokenIDEx with USER_100, "ohos.datashareclienttest.demo", and 0 to obtain a
       token ID object, then extract its tokenIDEx member.
    2. Create a Uri object using the predefined PROXY_URI_OK (valid proxy URI).
    3. Define a std::string variable permission and initialize it to "ohos.permission.WIFI".
    4. Call DataShare::DataSharePermission::VerifyPermission, passing the Uri, tokenIDEx, permission, and true (read
       operation).
    5. Check the boolean return value of the VerifyPermission method.
 * @tc.expect:
    1. The VerifyPermission method returns false (permission check fails).
 */
HWTEST_F(DataSharePermissionTest, DataObs_VerifyPermission_Test_002, TestSize.Level1)
{
    LOG_INFO("DataObs_VerifyPermission_Test_002::Start");
    auto tokenId = AccessTokenKit::GetHapTokenIDEx(USER_100, "ohos.datashareclienttest.demo", 0);
    Uri uri(PROXY_URI_OK);
    std::string permission = "ohos.permission.WIFI";
    auto ret = DataShare::DataSharePermission::VerifyPermission(uri, tokenId.tokenIDEx, permission, true);
    EXPECT_EQ(ret, false);
    LOG_INFO("DataObs_VerifyPermission_Test_002::End");
}

/**
 * @tc.name: DataObs_VerifyPermission_Test_003
 * @tc.desc: Verify that the static VerifyPermission method in DataSharePermission returns true when checking a valid
 *           DATA_SHARE_SELF_URI, a valid token ID, and an empty permission string for a write operation.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports calling AccessTokenKit::GetHapTokenIDEx to obtain a valid token ID (tokenIDEx)
       with USER_100, "ohos.datashareclienttest.demo", and 0.
    2. The DATA_SHARE_SELF_URI constant is predefined as a valid URI string that requires no specific permission (empty
       permission string is acceptable) for write operations.
    3. The DataShare::DataSharePermission::VerifyPermission static method is valid and accepts parameters: Uri,
       uint64_t (tokenIDEx), std::string (permission), bool (isRead), returning a bool (permission check result).
 * @tc.step:
    1. Call AccessTokenKit::GetHapTokenIDEx with USER_100, "ohos.datashareclienttest.demo", and 0 to obtain a
       token ID object, then extract its tokenIDEx member.
    2. Create a Uri object using the predefined DATA_SHARE_SELF_URI (valid URI with no required permission).
    3. Define a std::string variable permission and initialize it to an empty string ("").
    4. Call DataShare::DataSharePermission::VerifyPermission, passing the Uri, tokenIDEx, permission, and false (write
       operation).
    5. Check the boolean return value of the VerifyPermission method.
 * @tc.expect:
    1. The VerifyPermission method returns true (permission check passes with empty permission).
 */
HWTEST_F(DataSharePermissionTest, DataObs_VerifyPermission_Test_003, TestSize.Level1)
{
    LOG_INFO("DataObs_VerifyPermission_Test_003::Start");
    auto tokenId = AccessTokenKit::GetHapTokenIDEx(USER_100, "ohos.datashareclienttest.demo", 0);
    Uri uri(DATA_SHARE_SELF_URI);
    std::string permission = "";
    auto ret = DataShare::DataSharePermission::VerifyPermission(uri, tokenId.tokenIDEx, permission, false);
    EXPECT_EQ(ret, true);
    LOG_INFO("DataObs_VerifyPermission_Test_003::End");
}

/**
 * @tc.name: DataObs_VerifyPermission_Test_004
 * @tc.desc: Verify that the static VerifyPermission method in DataSharePermission returns true when checking a valid
 *           DATA_SHARE_SELF_URI, a valid token ID, and an empty permission string for a read operation.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports calling AccessTokenKit::GetHapTokenIDEx to obtain a valid token ID (tokenIDEx)
       with USER_100, "ohos.datashareclienttest.demo", and 0.
    2. The DATA_SHARE_SELF_URI constant is predefined as a valid URI string that requires no specific permission (empty
       permission string is acceptable) for read operations.
    3. The DataShare::DataSharePermission::VerifyPermission static method is valid and accepts parameters: Uri,
       uint64_t (tokenIDEx), std::string (permission), bool (isRead), returning a bool (permission check result).
 * @tc.step:
    1. Call AccessTokenKit::GetHapTokenIDEx with USER_100, "ohos.datashareclienttest.demo", and 0 to obtain a
       token ID object, then extract its tokenIDEx member.
    2. Create a Uri object using the predefined DATA_SHARE_SELF_URI (valid URI with no required permission).
    3. Define a std::string variable permission and initialize it to an empty string ("").
    4. Call DataShare::DataSharePermission::VerifyPermission, passing the Uri, tokenIDEx, permission, and true (read
       operation).
    5. Check the boolean return value of the VerifyPermission method.
 * @tc.expect:
    1. The VerifyPermission method returns true (permission check passes with empty permission).
 */
HWTEST_F(DataSharePermissionTest, DataObs_VerifyPermission_Test_004, TestSize.Level1)
{
    LOG_INFO("DataObs_VerifyPermission_Test_004::Start");
    auto tokenId = AccessTokenKit::GetHapTokenIDEx(USER_100, "ohos.datashareclienttest.demo", 0);
    Uri uri(DATA_SHARE_SELF_URI);
    std::string permission = "";
    auto ret = DataShare::DataSharePermission::VerifyPermission(uri, tokenId.tokenIDEx, permission, true);
    EXPECT_EQ(ret, true);
    LOG_INFO("DataObs_VerifyPermission_Test_004::End");
}

/**
 * @tc.name: DataObs_VerifyPermission_Test_005
 * @tc.desc: Verify the behavior of DataSharePermission::VerifyPermission when using DATA_SHARE_SELF_URI, an empty
 *           permission string, and a valid HAP token ID (from "ohos.datashareclienttest.demo" under USER_100).
 * @tc.type: FUNC
 * @tc.precon:
    1. The AccessTokenKit::GetHapTokenIDEx method can successfully obtain a valid token ID for USER_100 and the
       bundle name "ohos.datashareclienttest.demo".
    2. The predefined constant DATA_SHARE_SELF_URI is a valid Uri string and accessible.
    3. The DataSharePermission::VerifyPermission method accepts Uri, token ID, permission string, and bool as
       parameters and returns a boolean result.
 * @tc.step:
    1. Call AccessTokenKit::GetHapTokenIDEx with USER_100, "ohos.datashareclienttest.demo", and 0 to get the token ID.
    2. Create a Uri instance using the DATA_SHARE_SELF_URI constant.
    3. Define an empty std::string for the permission parameter.
    4. Call DataShare::DataSharePermission::VerifyPermission with the created Uri, token ID, empty permission,
       and false (the last bool parameter).
    5. Check the boolean return value of the VerifyPermission method.
 * @tc.expect:
    1. The DataSharePermission::VerifyPermission method returns true.
 */
HWTEST_F(DataSharePermissionTest, DataObs_VerifyPermission_Test_005, TestSize.Level1)
{
    LOG_INFO("DataObs_VerifyPermission_Test_005::Start");
    auto tokenId = AccessTokenKit::GetHapTokenIDEx(USER_100, "ohos.datashareclienttest.demo", 0);
    Uri uri(DATA_SHARE_SELF_URI);
    std::string permission = "";
    auto ret = DataShare::DataSharePermission::VerifyPermission(uri, tokenId.tokenIDEx, permission, false);
    EXPECT_EQ(ret, true);
    LOG_INFO("DataObs_VerifyPermission_Test_005::End");
}

/**
 * @tc.name: DataObs_VerifyPermission_Test_006
 * @tc.desc: Verify the behavior of DataSharePermission::VerifyPermission when using PROXY_URI_OK, an empty
 *           permission string, and a valid HAP token ID (from "ohos.datashareclienttest.demo" under USER_100).
 * @tc.type: FUNC
 * @tc.precon:
    1. The AccessTokenKit::GetHapTokenIDEx method can successfully obtain a valid token ID for USER_100 and the
       bundle name "ohos.datashareclienttest.demo".
    2. The predefined constant PROXY_URI_OK is a valid Uri string and accessible.
    3. The DataSharePermission::VerifyPermission method accepts Uri, token ID, permission string, and bool as
       parameters and returns a boolean result.
 * @tc.step:
    1. Call AccessTokenKit::GetHapTokenIDEx with USER_100, "ohos.datashareclienttest.demo", and 0 to get the token ID.
    2. Create a Uri instance using the PROXY_URI_OK constant.
    3. Define an empty std::string for the permission parameter.
    4. Call DataShare::DataSharePermission::VerifyPermission with the created Uri, token ID, empty permission,
       and false (the last bool parameter).
    5. Check the boolean return value of the VerifyPermission method.
 * @tc.expect:
    1. The DataSharePermission::VerifyPermission method returns true.
 */
HWTEST_F(DataSharePermissionTest, DataObs_VerifyPermission_Test_006, TestSize.Level1)
{
    LOG_INFO("DataObs_VerifyPermission_Test_006::Start");
    auto tokenId = AccessTokenKit::GetHapTokenIDEx(USER_100, "ohos.datashareclienttest.demo", 0);
    Uri uri(PROXY_URI_OK);
    std::string permission = "";
    auto ret = DataShare::DataSharePermission::VerifyPermission(uri, tokenId.tokenIDEx, permission, false);
    EXPECT_EQ(ret, true);
    LOG_INFO("DataObs_VerifyPermission_Test_006::End");
}

/**
 * @tc.name: IsExtensionValid_001
 * @tc.desc: Verify the behavior of DataSharePermission::IsExtensionValid when using a valid token ID from the
 *           "com.acts.datasharetest" bundle (under USER_100), with matching caller and callee token IDs.
 * @tc.type: FUNC
 * @tc.precon:
    1. The AccessTokenKit::GetHapTokenIDEx method can successfully obtain a valid token ID for USER_100 and the
       bundle name "com.acts.datasharetest".
    2. Predefined error codes (E_OK) are valid and accessible.
    3. The DataSharePermission::IsExtensionValid method accepts caller token ID, callee token ID, and user ID as
       parameters and returns an integer error code.
 * @tc.step:
    1. Call AccessTokenKit::GetHapTokenIDEx with USER_100, "com.acts.datasharetest", and 0 to get the token ID.
    2. Extract the tokenIDEx from the obtained token ID (to use as both caller and callee token IDs).
    3. Call DataShare::DataSharePermission::IsExtensionValid with the extracted tokenIDEx (caller and callee),
       and USER_100.
    4. Check the integer return code of the IsExtensionValid method.
 * @tc.expect:
    1. The DataSharePermission::IsExtensionValid method returns E_OK.
 */
HWTEST_F(DataSharePermissionTest, IsExtensionValid_001, TestSize.Level1)
{
    LOG_INFO("IsExtensionValid_001::Start");
    auto tokenId = AccessTokenKit::GetHapTokenIDEx(USER_100, "com.acts.datasharetest", 0);
    auto ret = DataShare::DataSharePermission::IsExtensionValid(tokenId.tokenIDEx, tokenId.tokenIDEx, USER_100);
    EXPECT_EQ(ret, E_OK);
    LOG_INFO("IsExtensionValid_001::End");
}

/**
 * @tc.name: IsExtensionValid_002
 * @tc.desc: Verify the behavior of DataSharePermission::IsExtensionValid when using a token ID from the
 *           "com.acts.ohos.data.datasharetest" bundle (under USER_100), which is not a DataShare extension.
 * @tc.type: FUNC
 * @tc.precon:
    1. The AccessTokenKit::GetHapTokenIDEx method can successfully obtain a token ID for USER_100 and the
       bundle name "com.acts.ohos.data.datasharetest".
    2. Predefined error codes (E_NOT_DATASHARE_EXTENSION) are valid and accessible.
    3. The DataSharePermission::IsExtensionValid method accepts caller token ID, callee token ID, and user ID as
       parameters and returns an integer error code.
 * @tc.step:
    1. Call AccessTokenKit::GetHapTokenIDEx with USER_100, "com.acts.ohos.data.datasharetest", and 0 to get the
       token ID.
    2. Extract the tokenIDEx from the obtained token ID (to use as both caller and callee token IDs).
    3. Call DataShare::DataSharePermission::IsExtensionValid with the extracted tokenIDEx (caller and callee),
       and USER_100.
    4. Check the integer return code of the IsExtensionValid method.
 * @tc.expect:
    1. The DataSharePermission::IsExtensionValid method returns E_NOT_DATASHARE_EXTENSION.
 */
HWTEST_F(DataSharePermissionTest, IsExtensionValid_002, TestSize.Level1)
{
    LOG_INFO("IsExtensionValid_002::Start");
    auto tokenId = AccessTokenKit::GetHapTokenIDEx(USER_100, "com.acts.ohos.data.datasharetest", 0);
    auto ret = DataShare::DataSharePermission::IsExtensionValid(tokenId.tokenIDEx, tokenId.tokenIDEx, USER_100);
    EXPECT_EQ(ret, E_NOT_DATASHARE_EXTENSION);
    LOG_INFO("IsExtensionValid_002::End");
}

/**
 * @tc.name: GetSilentUriPermission_001
 * @tc.desc: Verify the behavior of DataSharePermission::GetSilentUriPermission when the silentCache_ already
 *           contains the target URI (PROXY_URI_OK) and permission information for USER_100.
 * @tc.type: FUNC
 * @tc.precon:
    1. The DataSharePermission class can be instantiated as a shared pointer, and its silentCache_ (a cache structure)
       supports the Emplace method to add UriKey-permission pairs.
    2. Predefined constants (TEST_BUNDLE_NAME, TEST_PERMISSION, PROXY_URI_OK, USER_100) are valid and accessible.
    3. The DataSharePermission::UriKey structure can be initialized with a URI string and user ID;
       GetSilentUriPermission returns a pair of (integer error code, std::string permission).
 * @tc.step:
    1. Create a shared pointer of DataSharePermission (datashare) using std::make_shared.
    2. Initialize a DataSharePermission::Permission object: set bundleName to TEST_BUNDLE_NAME and readPermission to
       TEST_PERMISSION.
    3. Create a DataSharePermission::UriKey with PROXY_URI_OK (as string) and USER_100, then call
       datashare->silentCache_.Emplace to add the UriKey and Permission pair to the cache.
    4. Create a Uri instance (dstUri) using PROXY_URI_OK.
    5. Call datashare->GetSilentUriPermission with dstUri, USER_100, and true; record the returned (ret, permission)
       pair.
    6. Check the return code (ret) and permission string.
 * @tc.expect:
    1. The GetSilentUriPermission method returns E_OK as the error code.
    2. The returned permission string matches TEST_PERMISSION.
 */
HWTEST_F(DataSharePermissionTest, GetSilentUriPermission_001, TestSize.Level1)
{
    LOG_INFO("GetSilentUriPermission_001::Start");
    auto datashare = std::make_shared<DataSharePermission>();

    DataSharePermission::Permission permissionInfo;
    permissionInfo.bundleName = TEST_BUNDLE_NAME;
    permissionInfo.readPermission = TEST_PERMISSION;
    std::string uri = PROXY_URI_OK;
    DataSharePermission::UriKey uriKey(uri, USER_100);
    datashare->silentCache_.Emplace(uriKey, permissionInfo);

    Uri dstUri(PROXY_URI_OK);
    auto [ret, permission] = datashare->GetSilentUriPermission(dstUri, USER_100, true);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(permission, TEST_PERMISSION);
    LOG_INFO("GetSilentUriPermission_001::End");
}

/**
 * @tc.name: GetSilentUriPermission_002
 * @tc.desc: Verify the behavior of DataSharePermission::GetSilentUriPermission when silentCache_ is filled to
 *           DataSharePermission::CACHE_SIZE, then querying a new URI (PROXY_URI_OK) for USER_100.
 * @tc.type: FUNC
 * @tc.precon:
    1. The DataSharePermission class can be instantiated as a shared pointer; silentCache_ supports Emplace and Size
       methods, with a maximum capacity of DataSharePermission::CACHE_SIZE.
    2. Predefined constants (TEST_PERMISSION, PROXY_URI_OK, USER_100, DataSharePermission::CACHE_SIZE) are valid and
       accessible.
    3. GetSilentUriPermission returns a pair of (integer error code, std::string permission).
 * @tc.step:
    1. Create a shared pointer of DataSharePermission (datashare) using std::make_shared.
    2. Loop from 0 to DataSharePermission::CACHE_SIZE - 1:
        a. Create a DataSharePermission::Permission object and set readPermission to TEST_PERMISSION.
        b. Convert the loop index to a string as the URI, then create a UriKey with this URI and USER_100.
        c. Call datashare->silentCache_.Emplace to add the UriKey-Permission pair to the cache.
    3. Verify that datashare->silentCache_.Size() equals DataSharePermission::CACHE_SIZE.
    4. Create a Uri instance (dstUri) using PROXY_URI_OK.
    5. Call datashare->GetSilentUriPermission with dstUri, USER_100, and true; record the returned (ret, permission)
       pair.
    6. Check the return code (ret), permission string, and new size of silentCache_.
 * @tc.expect:
    1. The GetSilentUriPermission method returns E_OK as the error code.
    2. The returned permission string matches TEST_PERMISSION.
    3. After the query, datashare->silentCache_.Size() is 1.
 */
HWTEST_F(DataSharePermissionTest, GetSilentUriPermission_002, TestSize.Level1)
{
    LOG_INFO("GetSilentUriPermission_002::Start");
    auto datashare = std::make_shared<DataSharePermission>();

    for (int32_t i = 0; i < DataSharePermission::CACHE_SIZE; i++) {
        DataSharePermission::Permission permissionInfo;
        permissionInfo.readPermission = TEST_PERMISSION;
        std::string uri = std::to_string(i);
        DataSharePermission::UriKey uriKey(uri, USER_100);
        datashare->silentCache_.Emplace(uriKey, permissionInfo);
    }
    EXPECT_EQ(datashare->silentCache_.Size(), DataSharePermission::CACHE_SIZE);

    Uri dstUri(PROXY_URI_OK);
    auto [ret, permission] = datashare->GetSilentUriPermission(dstUri, USER_100, true);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(permission, TEST_PERMISSION);
    EXPECT_EQ(datashare->silentCache_.Size(), 1);
    LOG_INFO("GetSilentUriPermission_002::End");
}

/**
 * @tc.name: GetSilentUriPermission_003
 * @tc.desc: Verify the behavior of DataSharePermission::GetSilentUriPermission when silentCache_ is initially empty,
 *           then querying the same URI (PROXY_URI_OK) twice for USER_100.
 * @tc.type: FUNC
 * @tc.precon:
    1. The DataSharePermission class can be instantiated as a shared pointer; silentCache_ supports Size method and
       auto-populates when querying a new URI.
    2. Predefined constants (TEST_PERMISSION, PROXY_URI_OK, USER_100) are valid and accessible.
    3. GetSilentUriPermission returns a pair of (integer error code, std::string permission).
 * @tc.step:
    1. Create a shared pointer of DataSharePermission (datashare) using std::make_shared.
    2. Verify that datashare->silentCache_.Size() is 0 (initially empty).
    3. Create a Uri instance (dstUri) using PROXY_URI_OK.
    4. First call: datashare->GetSilentUriPermission with dstUri, USER_100, and true; record (ret, permission) and
       check silentCache_ size.
    5. Second call: datashare->GetSilentUriPermission with the same parameters; record (ret2, permission2) and check
       silentCache_ size again.
 * @tc.expect:
    1. Both calls to GetSilentUriPermission return E_OK as the error code.
    2. Both returned permission strings match TEST_PERMISSION.
    3. After the first call, datashare->silentCache_.Size() is 1; after the second call, it remains 1.
 */
HWTEST_F(DataSharePermissionTest, GetSilentUriPermission_003, TestSize.Level1)
{
    LOG_INFO("GetSilentUriPermission_003::Start");
    auto datashare = std::make_shared<DataSharePermission>();
    EXPECT_EQ(datashare->silentCache_.Size(), 0);

    Uri dstUri(PROXY_URI_OK);
    auto [ret, permission] = datashare->GetSilentUriPermission(dstUri, USER_100, true);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(permission, TEST_PERMISSION);
    EXPECT_EQ(datashare->silentCache_.Size(), 1);

    auto [ret2, permission2] = datashare->GetSilentUriPermission(dstUri, USER_100, true);
    EXPECT_EQ(ret2, E_OK);
    EXPECT_EQ(permission2, TEST_PERMISSION);
    EXPECT_EQ(datashare->silentCache_.Size(), 1);
    LOG_INFO("GetSilentUriPermission_003::End");
}

/**
 * @tc.name: GetExtensionUriPermission_001
 * @tc.desc: Verify the behavior of DataSharePermission::GetExtensionUriPermission when the extensionCache_ already
 *           contains the target URI (DATA_SHARE_EXTENSION_URI) and permission info for USER_100.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The DataSharePermission class can be instantiated as a shared pointer; its extensionCache_ supports the Emplace
       method to add UriKey-permission pairs.
    2. Predefined constants (TEST_BUNDLE_NAME, TEST_PERMISSION, DATA_SHARE_EXTENSION_URI, USER_100) are valid and
       accessible.
    3. The DataSharePermission::UriKey structure initializes with a URI string and user ID; GetExtensionUriPermission
       returns a pair of (integer error code, std::string permission).
 * @tc.step:
    1. Create a shared pointer of DataSharePermission (datashare) using std::make_shared.
    2. Initialize a DataSharePermission::Permission object: set bundleName to TEST_BUNDLE_NAME and readPermission to
       TEST_PERMISSION.
    3. Create a DataSharePermission::UriKey with DATA_SHARE_EXTENSION_URI (as string) and USER_100, then call
       datashare->extensionCache_.Emplace to add the pair to the cache.
    4. Create a Uri instance (dstUri) using DATA_SHARE_EXTENSION_URI.
    5. Call datashare->GetExtensionUriPermission with dstUri, USER_100, and true; record the returned (ret, permission)
       pair.
    6. Check the error code (ret) and permission string against expected values.
 * @tc.expect:
    1. The GetExtensionUriPermission method returns E_OK as the error code.
    2. The returned permission string matches TEST_PERMISSION.
 */
HWTEST_F(DataSharePermissionTest, GetExtensionUriPermission_001, TestSize.Level1)
{
    LOG_INFO("GetExtensionUriPermission_001::Start");
    auto datashare = std::make_shared<DataSharePermission>();

    DataSharePermission::Permission permissionInfo;
    permissionInfo.bundleName = TEST_BUNDLE_NAME;
    permissionInfo.readPermission = TEST_PERMISSION;
    std::string uri = DATA_SHARE_EXTENSION_URI;
    DataSharePermission::UriKey uriKey(uri, USER_100);
    datashare->extensionCache_.Emplace(uriKey, permissionInfo);

    Uri dstUri(DATA_SHARE_EXTENSION_URI);
    auto [ret, permission] = datashare->GetExtensionUriPermission(dstUri, USER_100, true);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(permission, TEST_PERMISSION);
    LOG_INFO("GetExtensionUriPermission_001::End");
}

/**
 * @tc.name: GetExtensionUriPermission_002
 * @tc.desc: Verify the behavior of DataSharePermission::GetExtensionUriPermission when extensionCache_ is filled to
 *           DataSharePermission::CACHE_SIZE, then querying a new URI (DATA_SHARE_EXTENSION_URI) for USER_100.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The DataSharePermission class can be instantiated as a shared pointer; extensionCache_ supports Emplace/Size
       methods, with a maximum capacity of DataSharePermission::CACHE_SIZE.
    2. Predefined constants (TEST_PERMISSION, DATA_SHARE_EXTENSION_URI, USER_100, DataSharePermission::CACHE_SIZE) are
       valid.
    3. GetExtensionUriPermission returns a pair of (integer error code, std::string permission).
 * @tc.step:
    1. Create a shared pointer of DataSharePermission (datashare) using std::make_shared.
    2. Loop from 0 to DataSharePermission::CACHE_SIZE - 1:
        a. Create a DataSharePermission::Permission object and set readPermission to TEST_PERMISSION.
        b. Convert the loop index to a string as the URI, then create a UriKey with this URI and USER_100.
        c. Call datashare->extensionCache_.Emplace to add the UriKey-Permission pair to the cache.
    3. Verify that datashare->extensionCache_.Size() equals DataSharePermission::CACHE_SIZE.
    4. Create a Uri instance (dstUri) using DATA_SHARE_EXTENSION_URI.
    5. Call datashare->GetExtensionUriPermission with dstUri, USER_100, and true; record the returned (ret, permission)
       pair.
    6. Check the error code, permission string, and new size of extensionCache_.
 * @tc.expect:
    1. The GetExtensionUriPermission method returns E_OK as the error code.
    2. The returned permission string is empty ("").
    3. After the query, datashare->extensionCache_.Size() is 1.
 */
HWTEST_F(DataSharePermissionTest, GetExtensionUriPermission_002, TestSize.Level1)
{
    LOG_INFO("GetExtensionUriPermission_002::Start");
    auto datashare = std::make_shared<DataSharePermission>();

    for (int32_t i = 0; i < DataSharePermission::CACHE_SIZE; i++) {
        DataSharePermission::Permission permissionInfo;
        permissionInfo.readPermission = TEST_PERMISSION;
        std::string uri = std::to_string(i);
        DataSharePermission::UriKey uriKey(uri, USER_100);
        datashare->extensionCache_.Emplace(uriKey, permissionInfo);
    }
    EXPECT_EQ(datashare->extensionCache_.Size(), DataSharePermission::CACHE_SIZE);

    Uri dstUri(DATA_SHARE_EXTENSION_URI);
    auto [ret, permission] = datashare->GetExtensionUriPermission(dstUri, USER_100, true);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(permission, "");
    EXPECT_EQ(datashare->extensionCache_.Size(), 1);
    LOG_INFO("GetExtensionUriPermission_002::End");
}

/**
 * @tc.name: GetExtensionUriPermission_003
 * @tc.desc: Verify the behavior of DataSharePermission::GetExtensionUriPermission when extensionCache_ is initially
 *           empty, then querying the same URI (DATA_SHARE_EXTENSION_URI) twice for USER_100.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The DataSharePermission class can be instantiated as a shared pointer; extensionCache_ supports Size method and
       auto-populates when querying a new URI.
    2. Predefined constants (DATA_SHARE_EXTENSION_URI, USER_100) are valid and accessible.
    3. GetExtensionUriPermission returns a pair of (integer error code, std::string permission).
 * @tc.step:
    1. Create a shared pointer of DataSharePermission (datashare) using std::make_shared.
    2. Verify that datashare->extensionCache_.Size() is 0 (initially empty).
    3. Create a Uri instance (dstUri) using DATA_SHARE_EXTENSION_URI.
    4. First call: Call datashare->GetExtensionUriPermission with dstUri, USER_100, and true; record (ret, permission)
       and check extensionCache_ size.
    5. Second call: Call the method again with the same parameters; record (ret2, permission2) and check
       extensionCache_ size.
 * @tc.expect:
    1. Both calls to GetExtensionUriPermission return E_OK as the error code.
    2. Both returned permission strings are empty ("").
    3. After both calls, datashare->extensionCache_.Size() remains 1.
 */
HWTEST_F(DataSharePermissionTest, GetExtensionUriPermission_003, TestSize.Level1)
{
    LOG_INFO("GetExtensionUriPermission_003::Start");
    auto datashare = std::make_shared<DataSharePermission>();
    EXPECT_EQ(datashare->extensionCache_.Size(), 0);

    Uri dstUri(DATA_SHARE_EXTENSION_URI);
    auto [ret, permission] = datashare->GetExtensionUriPermission(dstUri, USER_100, true);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(permission, "");
    EXPECT_EQ(datashare->extensionCache_.Size(), 1);

    auto [ret2, permission2] = datashare->GetExtensionUriPermission(dstUri, USER_100, true);
    EXPECT_EQ(ret2, E_OK);
    EXPECT_EQ(permission2, "");
    LOG_INFO("GetExtensionUriPermission_003::End");
    EXPECT_EQ(datashare->extensionCache_.Size(), 1);
}

/**
 * @tc.name: Have_Write_Test_001
 * @tc.desc: Verify that the DataSharePermission::VerifyPermission method succeeds (returns E_OK) for a write operation
 *           when the process has the correct write permission ("ohos.permission.WRITE_CONTACTS") via a valid access
 *           token.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports instantiation of HapInfoParams and HapPolicyParams, and AccessTokenKit methods
       (AllocHapToken, GetHapTokenIDEx, DeleteToken) work normally.
    2. The SetSelfTokenID function can set the current process's token ID; predefined constants (USER_100,
       PROXY_URI_OK, APL_SYSTEM_CORE, PermissionState::PERMISSION_GRANTED, E_OK) are valid.
    3. DataSharePermission::VerifyPermission accepts token ID, Uri, and bool (write operation flag) as parameters and
       returns an integer error code.
 * @tc.step:
    1. Initialize HapInfoParams (info) with USER_100, bundle name "ohos.permission.write.demo", and other required
       fields.
    2. Initialize HapPolicyParams (policy) with APL_SYSTEM_CORE, and include "ohos.permission.WRITE_CONTACTS" (granted)
       in permStateList.
    3. Call AccessTokenKit::AllocHapToken with info and policy to allocate an access token.
    4. Get the token ID via AccessTokenKit::GetHapTokenIDEx (using info's userID, bundleName, instIndex), then call
       SetSelfTokenID to set it as the current process token.
    5. Create a Uri instance using PROXY_URI_OK, then call DataShare::DataSharePermission::VerifyPermission with the
       token ID, Uri, and false (write operation flag).
    6. Check the returned error code; call AccessTokenKit::DeleteToken to clean up the allocated token.
 * @tc.expect:
    1. The DataSharePermission::VerifyPermission method returns E_OK for the write operation.
    2. All AccessTokenKit operations (AllocHapToken, GetHapTokenIDEx, DeleteToken) and SetSelfTokenID execute without
       errors.
 */
HWTEST_F(DataSharePermissionTest, Have_Write_Test_001, TestSize.Level1)
{
    LOG_INFO("Have_Write_Test_001::Start");
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
    LOG_INFO("Have_Write_Test_001::End");
}

/**
 * @tc.name: Hava_Read_Permission_Test_001
 * @tc.desc: Verify that the DataSharePermission::VerifyPermission method succeeds (returns E_OK) for a read operation
 *           when the process has the correct read permission ("ohos.permission.READ_CALL_LOG") via a valid access
 *           token.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports instantiation of HapInfoParams and HapPolicyParams, and AccessTokenKit methods
       (AllocHapToken, GetHapTokenIDEx, DeleteToken) work normally.
    2. The SetSelfTokenID function can set the current process's token ID; predefined constants (USER_100,
       DATA_SHARE_URI, APL_SYSTEM_CORE, PermissionState::PERMISSION_GRANTED, E_OK) are valid.
    3. DataSharePermission::VerifyPermission accepts token ID, Uri, and bool (read operation flag) as parameters and
       returns an integer error code.
 * @tc.step:
    1. Initialize HapInfoParams (info) with USER_100, bundle name "ohos.permission.demo", and other required fields.
    2. Initialize HapPolicyParams (policy) with APL_SYSTEM_CORE, and include "ohos.permission.READ_CALL_LOG" (granted)
       in permStateList.
    3. Call AccessTokenKit::AllocHapToken with info and policy to allocate an access token.
    4. Get the token ID via AccessTokenKit::GetHapTokenIDEx (using info's userID, bundleName, instIndex), then call
       SetSelfTokenID to set it as the current process token.
    5. Create a Uri instance using DATA_SHARE_URI, then call DataShare::DataSharePermission::VerifyPermission with the
       token ID, Uri, and true (read operation flag).
    6. Check the returned error code; call AccessTokenKit::DeleteToken to clean up the allocated token.
 * @tc.expect:
    1. The DataSharePermission::VerifyPermission method returns E_OK for the read operation.
    2. All AccessTokenKit operations (AllocHapToken, GetHapTokenIDEx, DeleteToken) and SetSelfTokenID execute without
       errors.
 */
HWTEST_F(DataSharePermissionTest, Hava_Read_Permission_Test_001, TestSize.Level1)
{
    LOG_INFO("Hava_Read_Permission_Test_001::Start");
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
    LOG_INFO("Hava_Read_Permission_Test_001::End");
}

} // namespace DataShare
} // namespace OHOS