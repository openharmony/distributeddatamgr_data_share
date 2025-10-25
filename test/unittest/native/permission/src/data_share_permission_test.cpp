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

static const std::string TEST_BUNDLE_NAME = "com.acts.datasharetest1";
static const std::string TEST_BUNDLE_NAME2 = "com.acts.datasharetest2";
static const int32_t TEST_USERID = 100;

namespace OHOS {
namespace DataShare {
using namespace testing::ext;
class DataSharePermissionTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void DataSharePermissionTest::SetUpTestCase(void) {}
void DataSharePermissionTest::TearDownTestCase(void) {}
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
    permissionInfo.bundleName = TEST_BUNDLE_NAME;
    std::string uri = "";
    DataSharePermission::UriKey uriKey(uri, TEST_USERID);
    permission->silentCache_.Emplace(uriKey, permissionInfo);
    EXPECT_EQ(permission->silentCache_.Size(), 1);

    EventFwk::CommonEventData event;
    AAFwk::Want want;
    OHOS::AppExecFwk::ElementName element;
    element.SetBundleName(TEST_BUNDLE_NAME);
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
    permissionInfo.bundleName = TEST_BUNDLE_NAME;
    std::string uri = "";
    DataSharePermission::UriKey uriKey(uri, TEST_USERID);
    permission->silentCache_.Emplace(uriKey, permissionInfo);
    permission->extensionCache_.Emplace(uriKey, permissionInfo);
    EXPECT_EQ(permission->silentCache_.Size(), 1);
    EXPECT_EQ(permission->extensionCache_.Size(), 1);

    permission->DeleteCache(TEST_BUNDLE_NAME);
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
    permissionInfo.bundleName = TEST_BUNDLE_NAME;
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

}
}
