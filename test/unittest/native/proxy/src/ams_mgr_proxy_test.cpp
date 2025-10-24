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

#define LOG_TAG "ams_mgr_proxy_test"

#include <gtest/gtest.h>
#include <unistd.h>
#include <memory>
#include "datashare_log.h"
#include "ams_mgr_proxy.h"

namespace OHOS {
namespace DataShare {
using namespace testing::ext;

class AmsMgrProxyTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void AmsMgrProxyTest::SetUpTestCase(void)
{
}
void AmsMgrProxyTest::TearDownTestCase(void)
{
}
void AmsMgrProxyTest::SetUp(void)
{
}
void AmsMgrProxyTest::TearDown(void)
{
}

/**
 * @tc.name: AmsMgrProxyOnProxyDiedTest001
 * @tc.desc: Verify AmsMgrProxy behavior when proxy dies
 * @tc.type: FUNC
 * @tc.precon: None
 * @tc.step:
    1. Create AmsMgrProxy with null service and proxy
    2. Call OnProxyDied and verify state
    3. Create new proxy, connect to SA, verify initialization
    4. Call OnProxyDied again and verify cleanup
 * @tc.expect:
    1. OnProxyDied handles null pointers gracefully
    2. Proxy initializes properly after ConnectSA
    3. OnProxyDied cleans up resources
 */
HWTEST_F(AmsMgrProxyTest, AmsMgrProxyOnProxyDiedTest001, TestSize.Level0)
{
    LOG_INFO("AmsMgrProxyOnProxyDiedTest001::Start");
    AmsMgrProxy* proxy = new AmsMgrProxy();
    proxy->sa_ = nullptr;
    proxy->proxy_ = nullptr;
    proxy->OnProxyDied();
    delete proxy;
    proxy = new AmsMgrProxy();
    proxy->sa_ = nullptr;
    proxy->proxy_ = nullptr;
    proxy->ConnectSA();
    EXPECT_NE(proxy->sa_, nullptr);
    EXPECT_NE(proxy->proxy_, nullptr);
    EXPECT_NE(proxy->deathRecipient_, nullptr);
    proxy->OnProxyDied();
    delete proxy;
    LOG_INFO("AmsMgrProxyOnProxyDiedTest001::End");
}

/**
* @tc.name: AmsMgrProxyOnProxyDiedTest002
* @tc.desc: Test sa_ with nullptr and destructor of AmsMgrProxy
* @tc.type: FUNC
* @tc.precon: None
* @tc.expect: Successfully process SetRegisterCallback
* @tc.step:  1. Create a AmsMgrProxy instance;
             2. Clear the proxy;
             3. After clear, try to connect SA;
             4. Check if this proxy can be connected successfully.
* @tc.require: issueIBX9HL
*/
HWTEST_F(AmsMgrProxyTest, AmsMgrProxyOnProxyDiedTest002, TestSize.Level0)
{
    LOG_INFO("AmsMgrProxyOnProxyDiedTest002::Start");
    AmsMgrProxy* proxy = AmsMgrProxy::GetInstance();
    proxy->OnProxyDied();
    proxy->ConnectSA();
    EXPECT_NE(proxy->sa_, nullptr);
    EXPECT_NE(proxy->proxy_, nullptr);
    EXPECT_NE(proxy->deathRecipient_, nullptr);
    LOG_INFO("AmsMgrProxyOnProxyDiedTest002::End");
}

/**
 * @tc.name: DataShareServiceProxySubscribeRdbDataTest001
 * @tc.desc: Verify DataShareServiceProxy subscription behavior after proxy death
 * @tc.type: FUNC
 * @tc.precon: None
 * @tc.step:
    1. Create AmsMgrProxy with null service and proxy
    2. Call OnProxyDied and verify cleanup
    3. Create new proxy, connect to SA, verify initialization
    4. Call OnProxyDied again and verify cleanup
 * @tc.expect:
    1. Proxy handles null states gracefully
    2. Proxy initializes properly after ConnectSA
    3. Resources are cleaned up after OnProxyDied
 */
HWTEST_F(AmsMgrProxyTest, DataShareServiceProxySubscribeRdbDataTest001, TestSize.Level0)
{
    LOG_INFO("DataShareServiceProxySubscribeRdbDataTest001::Start");
    AmsMgrProxy* proxy = new AmsMgrProxy();
    proxy->sa_ = nullptr;
    proxy->proxy_ = nullptr;
    proxy->OnProxyDied();
    delete proxy;
    proxy = new AmsMgrProxy();
    proxy->sa_ = nullptr;
    proxy->proxy_ = nullptr;
    proxy->ConnectSA();
    EXPECT_NE(proxy->sa_, nullptr);
    EXPECT_NE(proxy->proxy_, nullptr);
    EXPECT_NE(proxy->deathRecipient_, nullptr);
    proxy->OnProxyDied();
    delete proxy;
    LOG_INFO("DataShareServiceProxySubscribeRdbDataTest001::End");
}
}
}