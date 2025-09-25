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
#include <vector>

#include "accesstoken_kit.h"
#include "data_ability_observer_stub.h"
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
using ChangeInfo = DataShareObserver::ChangeInfo;
constexpr int STORAGE_MANAGER_MANAGER_ID = 5003;
std::string DATA_SHARE_URI = "datashare:///com.acts.datasharetest";


class SlientTimeoutTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void SlientTimeoutTest::SetUpTestCase(void) {}
void SlientTimeoutTest::TearDownTestCase(void) {}
void SlientTimeoutTest::SetUp(void) {}
void SlientTimeoutTest::TearDown(void) {}

/**
* @tc.name: SlientTimeout_Creator_When_TimeOut_Test_001
* @tc.desc: Test DataShareHelper creation with timeout
* @tc.type: FUNC
* @tc.require: NA
* @tc.precon: None
* @tc.step:
* 1. Get SystemAbilityManager instance and remote object for STORAGE_MANAGER_MANAGER_ID
* 2. Try to create DataShareHelper with valid URI but timeout parameter 0
* 3. Check helper instance
* @tc.expect: Helper creation fails due to timeout (helper == nullptr)
*/
HWTEST_F(SlientTimeoutTest, SlientTimeout_Creator_When_TimeOut_Test_001, TestSize.Level0)
{
    LOG_INFO("SlientTimeout SlientTimeout_Creator_When_TimeOut_Test_001::Begin");
    auto saManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (saManager == nullptr) {
        LOG_ERROR("GetSystemAbilityManager get samgr failed.");
    }
    auto remoteObj = saManager->GetSystemAbility(STORAGE_MANAGER_MANAGER_ID);
    if (remoteObj == nullptr) {
        LOG_ERROR("GetSystemAbility service failed.");
    }
    std::string uriStr(DATA_SHARE_URI);
    auto helper = DataShare::DataShareHelper::Creator(remoteObj, uriStr, uriStr, 0);
    EXPECT_EQ(helper, nullptr);
    LOG_INFO("SlientTimeout SlientTimeout_Creator_When_TimeOut_Test_001::End");
}
}
}