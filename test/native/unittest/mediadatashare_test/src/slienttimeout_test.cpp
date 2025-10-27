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

#define LOG_TAG "slienttimeout_test"

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
 * @tc.desc: Test the creation process of DataShareHelper via its Creator method, focusing on the scenario where
 *           the timeout parameter is set to 0, to verify if the creation fails due to timeout.
 * @tc.type: FUNC
 * @tc.require: NA
 * @tc.precon:
    1. The test environment supports the instantiation of SystemAbilityManagerClient and the calling of its
       GetInstance() method to obtain a valid SystemAbilityManager instance.
    2. The STORAGE_MANAGER_MANAGER_ID constant is predefined and valid, enabling the SystemAbilityManager to
       execute the GetSystemAbility() method to retrieve a remote object (regardless of internal validity checks).
    3. The DATA_SHARE_URI constant is a predefined valid string, which can be used to construct the URI string
       required by the DataShareHelper::Creator method.
    4. The DataShare::DataShareHelper class provides a valid Creator static method that accepts parameters:
       sptr<IRemoteObject>, std::string, std::string, and int (timeout).
 * @tc.step:
    1. Call SystemAbilityManagerClient::GetInstance() to obtain a SystemAbilityManagerClient instance, then
       call its GetSystemAbilityManager() method to get a SystemAbilityManager instance.
    2. Use the SystemAbilityManager instance to call GetSystemAbility(STORAGE_MANAGER_MANAGER_ID) and retrieve
       the corresponding remote object (remoteObj).
    3. Define a std::string variable uriStr and initialize it with the predefined DATA_SHARE_URI constant.
    4. Call DataShare::DataShareHelper::Creator(), passing remoteObj, uriStr (twice as the second and third
       parameters), and 0 (timeout parameter), and store the returned result in a helper variable.
    5. Check whether the helper variable is a null pointer.
 * @tc.expect:
    1. The DataShareHelper instance created via the Creator method is nullptr, indicating that the creation
       fails due to the timeout parameter being set to 0.
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