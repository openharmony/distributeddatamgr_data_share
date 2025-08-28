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
 * @tc.desc: test VerifyPermission function when permission is empty
 * @tc.type: FUNC
 * @tc.require:issueICU06G
 * @tc.precon: None
 * @tc.step:
    1.define permission as empty
    2.call VerifyPermission function and check the result
 * @tc.experct: VerifyPermission return true
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
 * @tc.name: CheckExtensionTrusts001
 * @tc.desc: test CheckExtensionTrusts function when consumerToken and providerToken do not have corresponding haps
 * @tc.type: FUNC
 * @tc.require:issueICU06G
 * @tc.precon: None
 * @tc.step:
    1.Create False consumerToken and providerToken
    2.call CheckExtensionTrusts function and check the result
 * @tc.experct: CheckExtensionTrusts reutrn nullptr
 */
HWTEST_F(DataSharePermissionTest, CheckExtensionTrusts001, TestSize.Level0)
{
    LOG_INFO("DataSharePermissionTest CheckExtensionTrusts001::Start");
    uint32_t consumerToken = 123;
    uint32_t providerToken = 123;
    int result = DataSharePermission::CheckExtensionTrusts(consumerToken, providerToken);
    EXPECT_EQ(result, DataShare::E_GET_CALLER_NAME_FAILED);
    LOG_INFO("DataSharePermissionTest CheckExtensionTrusts001::End");
}

/**
 * @tc.name: Init001
 * @tc.desc: test Init function
 * @tc.type: FUNC
 * @tc.require:issueICU06G
 * @tc.precon: None
 * @tc.step:
    1.Init success
 * @tc.experct: permission->subscriber is not nullptr
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
 * @tc.name: Init001
 * @tc.desc: test SysEventSubscriber
 * @tc.type: FUNC
 * @tc.require:issueICU06G
 * @tc.precon: None
 * @tc.step:
    1.OnReceiveEvent success
 * @tc.experct: OnReceiveEvent clean cache
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
 * @tc.desc: test SysEventSubscriber
 * @tc.type: FUNC
 * @tc.require:issueICU06G
 * @tc.precon: None
 * @tc.step:
    1.DeleteCache success
 * @tc.experct: DeleteCache clean cache
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
 * @tc.desc: test SysEventSubscriber
 * @tc.type: FUNC
 * @tc.require:issueICU06G
 * @tc.precon: None
 * @tc.step:
    1.DeleteCache success
 * @tc.experct: DeleteCache clean cache
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
 * @tc.name: Init001
 * @tc.desc: test SysEventSubscriber
 * @tc.type: FUNC
 * @tc.require:issueICU06G
 * @tc.precon: None
 * @tc.step:
    1.DeleteCache success
 * @tc.experct: OnUpdate clean cache
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