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
#define LOG_TAG "ikvstore_data_service_test"

#include <gtest/gtest.h>
#include <unistd.h>
#include <memory>
#include "accesstoken_kit.h"
#include "data_share_manager_impl.h"
#include "data_share_service_proxy.h"
#include "datashare_helper.h"
#include "datashare_log.h"
#include "datashare_template.h"
#include "hap_token_info.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "token_setproc.h"
#include "datashare_errno.h"
#include "published_data_subscriber_manager.h"
#include "rdb_subscriber_manager.h"
#include "ishared_result_set_stub.h"
#include "message_parcel.h"
#include "ikvstore_data_service.h"
#include "shared_block.h"
#include "datashare_block_writer_impl.h"
#include "datashare_connection.h"
#include "general_controller_service_impl.h"
#include "persistent_data_controller.h"
#include "published_data_controller.h"

namespace OHOS {
namespace DataShare {
using namespace testing::ext;
using namespace OHOS::Security::AccessToken;

class IkvStoreDataServiceTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void IkvStoreDataServiceTest::SetUpTestCase(void)
{
}
void IkvStoreDataServiceTest::TearDownTestCase(void)
{
}
void IkvStoreDataServiceTest::SetUp(void)
{
}
void IkvStoreDataServiceTest::TearDown(void)
{
}

/**
 * @tc.name: RegisterClientDeathObserverNull_Test_001
 * @tc.desc: Verify the behavior of DataShareKvServiceProxy::RegisterClientDeathObserver when the proxy is initialized
 *           with a null parameter, and called with an empty appId and null observer.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports instantiation of DataShareKvServiceProxy with a null constructor parameter.
    2. DataShareKvServiceProxy has the RegisterClientDeathObserver method, which accepts std::string (appId) and
       IRemoteObject* (observer) as parameters, returning a uint32_t result.
    3. The return code -1 is predefined as the indicator of operation failure for this method.
 * @tc.step:
    1. Instantiate a DataShareKvServiceProxy object using a null pointer in the constructor.
    2. Define an empty std::string variable named appId.
    3. Call the proxy's RegisterClientDeathObserver method with appId (empty) and a null IRemoteObject pointer.
    4. Check the uint32_t return value of the method.
 * @tc.expect:
    1. The RegisterClientDeathObserver method returns -1 (operation failure).
 */
HWTEST_F(IkvStoreDataServiceTest, RegisterClientDeathObserverNull_Test_001, TestSize.Level0)
{
    LOG_INFO("RegisterClientDeathObserverNull_Test_001::Start");
    DataShareKvServiceProxy proxy(nullptr);
    std::string appId;
    uint32_t result = proxy.RegisterClientDeathObserver(appId, nullptr);
    EXPECT_EQ(result, -1);
    LOG_INFO("RegisterClientDeathObserverNull_Test_001::End");
}

/**
 * @tc.name: RegisterClientDeathObserver001
 * @tc.desc: Test the behavior of DataShareKvServiceProxy::RegisterClientDeathObserver when the observer parameter
 *           (IRemoteObject*) is set to nullptr.
 * @tc.type: FUNC
 * @tc.require: issueIC9GIH
 * @tc.precon:
    1. The test environment supports instantiation of DataShareKvServiceProxy with an IRemoteObject pointer (including
       null).
    2. DataShareKvServiceProxy has the RegisterClientDeathObserver method, which accepts std::string (appId) and
       IRemoteObject* (observer) as parameters, returning a uint32_t result.
    3. The return code -1 is predefined as the indicator of operation failure for this method.
    4. The string "testAppid" is a valid input for the appId parameter of the method.
 * @tc.step:
    1. Set an IRemoteObject pointer (observer) to nullptr.
    2. Define a std::string appId and initialize it to "testAppid".
    3. Instantiate a DataShareKvServiceProxy object using the null observer as the constructor parameter.
    4. Call the proxy's RegisterClientDeathObserver method with appId and the null observer.
    5. Check the uint32_t return value of the method.
 * @tc.expect:
    1. The RegisterClientDeathObserver method fails and returns -1.
 */
HWTEST_F(IkvStoreDataServiceTest, RegisterClientDeathObserver001, TestSize.Level0)
{
    LOG_INFO("RegisterClientDeathObserver001::Start");
    sptr<IRemoteObject> observer = nullptr;
    std::string appId = "testAppid";
    DataShareKvServiceProxy dataShareKvServiceProxy(observer);
    auto result = dataShareKvServiceProxy.RegisterClientDeathObserver(appId, observer);
    EXPECT_EQ(result, -1);
    LOG_INFO("RegisterClientDeathObserver001::End");
}
}
}