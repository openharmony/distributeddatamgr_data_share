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
#define LOG_TAG "general_controller_service_impl_test"
#include <gtest/gtest.h>
#include <unistd.h>

#include <condition_variable>

#include "abs_shared_result_set.h"
#include "accesstoken_kit.h"
#include "data_ability_observer_interface.h"
#include "dataobs_mgr_changeinfo.h"
#include "datashare_connection.h"
#include "datashare_errno.h"
#include "datashare_helper.h"
#include "datashare_log.h"
#include "ext_special_controller.h"
#include "extension_manager_proxy.h"
#include "general_controller.h"
#include "general_controller_provider_impl.h"
#include "general_controller_service_impl.h"
#include "hap_token_info.h"
#include "iservice_registry.h"
#include "rdb_data_ability_utils.h"
#include "system_ability_definition.h"
#include "token_setproc.h"

namespace OHOS {
namespace DataShare {
using namespace testing::ext;
using namespace OHOS::Security::AccessToken;

class GeneralControllerServiceImplTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void GeneralControllerServiceImplTest::SetUpTestCase(void) {}
void GeneralControllerServiceImplTest::TearDownTestCase(void) {}
void GeneralControllerServiceImplTest::SetUp(void) {}
void GeneralControllerServiceImplTest::TearDown(void) {}

/**
 * @tc.name: RegisterObserverExtProviderTest001
 * @tc.desc: Verify the RegisterObserverExtProvider operation in GeneralControllerServiceImpl with an empty Uri
 *           and null observer, confirming failure via a return code of -1.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports instantiation of GeneralControllerServiceImpl, sptr<AAFwk::IDataAbilityObserver>,
       and Uri.
    2. GeneralControllerServiceImpl can be initialized with a test URI string ("GeneralControllerServiceImpl").
 * @tc.step:
    1. Create an empty Uri and a GeneralControllerServiceImpl instance initialized with the test URI
       ("GeneralControllerServiceImpl").
    2. Create a null sptr<AAFwk::IDataAbilityObserver> (dataObserver).
    3. Call the RegisterObserverExtProvider method with the empty Uri, dataObserver, and isDescendants = false.
    4. Check the return code of the RegisterObserverExtProvider operation.
 * @tc.expect:
    1. The RegisterObserverExtProvider operation returns -1, indicating failure.
 */
HWTEST_F(GeneralControllerServiceImplTest, RegisterObserverExtProviderTest001, TestSize.Level0)
{
    LOG_INFO("GeneralControllerServiceImplTest RegisterObserverExtProviderTest001::Start");

    Uri uri("");
    std::string extUri = "GeneralControllerServiceImpl";

    std::shared_ptr<DataShare::GeneralController> tempGenConProImp =
        std::make_shared<DataShare::GeneralControllerServiceImpl>(extUri);
    ASSERT_NE(tempGenConProImp, nullptr);
    sptr<AAFwk::IDataAbilityObserver> dataObserver;

    int ret = tempGenConProImp->RegisterObserverExtProvider(uri, dataObserver, false);
    EXPECT_EQ(ret, -1);

    LOG_INFO("GeneralControllerServiceImplTest RegisterObserverExtProviderTest001::End");
}

/**
 * @tc.name: UnregisterObserverExtProviderTest001
 * @tc.desc: Verify the UnregisterObserverExtProvider operation in GeneralControllerServiceImpl with an empty Uri
 *           and null observer, confirming failure via a return code of -1.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports instantiation of GeneralControllerServiceImpl, sptr<AAFwk::IDataAbilityObserver>,
       and Uri.
    2. GeneralControllerServiceImpl can be initialized with a test URI string ("GeneralControllerServiceImpl").
 * @tc.step:
    1. Create an empty Uri and a GeneralControllerServiceImpl instance initialized with the test URI
       ("GeneralControllerServiceImpl").
    2. Create a null sptr<AAFwk::IDataAbilityObserver> (dataObserver).
    3. Call the UnregisterObserverExtProvider method with the empty Uri and dataObserver.
    4. Check the return code of the UnregisterObserverExtProvider operation.
 * @tc.expect:
    1. The UnregisterObserverExtProvider operation returns -1, indicating failure.
 */
HWTEST_F(GeneralControllerServiceImplTest, UnregisterObserverExtProviderTest001, TestSize.Level0)
{
    LOG_INFO("GeneralControllerServiceImplTest UnregisterObserverExtProviderTest001::Start");

    Uri uri("");
    std::string extUri = "GeneralControllerServiceImpl";

    std::shared_ptr<DataShare::GeneralController> tempGenConProImp =
        std::make_shared<DataShare::GeneralControllerServiceImpl>(extUri);
    ASSERT_NE(tempGenConProImp, nullptr);
    sptr<AAFwk::IDataAbilityObserver> dataObserver;

    int ret = tempGenConProImp->UnregisterObserverExtProvider(uri, dataObserver);
    EXPECT_EQ(ret, -1);

    LOG_INFO("GeneralControllerServiceImplTest UnregisterObserverExtProviderTest001::End");
}

/**
 * @tc.name: NotifyChangeExtProviderTest001
 * @tc.desc: Verify the NotifyChangeExtProvider operation in GeneralControllerServiceImpl with a ChangeInfo containing
 *           an empty Uri, confirming failure via a return code of -1.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports instantiation of GeneralControllerServiceImpl, ChangeInfo, and Uri.
    2. GeneralControllerServiceImpl can be initialized with a test URI string ("GeneralControllerServiceImpl").
    3. ChangeInfo supports initialization with ChangeType::INSERT and a list containing an empty Uri.
 * @tc.step:
    1. Create an empty Uri and a ChangeInfo (ChangeType::INSERT, {empty Uri}).
    2. Create a GeneralControllerServiceImpl instance initialized with the test URI ("GeneralControllerServiceImpl").
    3. Call the NotifyChangeExtProvider method with the created ChangeInfo.
    4. Check the return code of the NotifyChangeExtProvider operation.
 * @tc.expect:
    1. The NotifyChangeExtProvider operation returns -1, indicating failure.
 */
HWTEST_F(GeneralControllerServiceImplTest, NotifyChangeExtProviderTest001, TestSize.Level0)
{
    LOG_INFO("GeneralControllerServiceImplTest NotifyChangeExtProviderTest001::Start");

    Uri uri("");
    std::string extUri = "GeneralControllerServiceImpl";

    ChangeInfo changeInfo = { ChangeInfo::ChangeType::INSERT, { uri } };
    // connection is nullptr
    std::shared_ptr<DataShare::GeneralController> tempGenConProImp =
        std::make_shared<DataShare::GeneralControllerServiceImpl>(extUri);
    ASSERT_NE(tempGenConProImp, nullptr);
    sptr<AAFwk::IDataAbilityObserver> dataObserver;

    int ret = tempGenConProImp->NotifyChangeExtProvider(changeInfo);
    EXPECT_EQ(ret, -1);

    LOG_INFO("GeneralControllerServiceImplTest NotifyChangeExtProviderTest001::End");
}
}
}