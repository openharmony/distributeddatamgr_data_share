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
 * @tc.desc: Verify RegisterClientDeathObserver with null observer
 * @tc.type: FUNC
 * @tc.precon: None
 * @tc.step:
    1. Create DataShareKvServiceProxy with null parameter
    2. Call RegisterClientDeathObserver with empty appId and null observer
    3. Check return value
 * @tc.expect:
    1. Method returns -1 (failure)
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
* @tc.desc: test RegisterClientDeathObserver function when observer = nullptr
* @tc.type: FUNC
* @tc.require: issueIC9GIH
* @tc.precon: None
* @tc.step:
    1.Creat a MockDataShareAbsResultSet object when observer = nullptr
    2.call RegisterClientDeathObserver function and check the result
* @tc.experct: RegisterClientDeathObserver failed and return -1
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