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
 * @tc.require: None
 * @tc.precon:
    1. AmsMgrProxy class can be instantiated using the new operator and destroyed with delete
    2. AmsMgrProxy has member variables: sa_ (service pointer), proxy_ (proxy pointer), and deathRecipient_ (death
       recipient pointer)
    3. ConnectSA() is a member function of AmsMgrProxy that initializes sa_, proxy_, and deathRecipient_ to non-null
       values upon successful connection
    4. OnProxyDied() is a member function of AmsMgrProxy that sets sa_, proxy_, and deathRecipient_ to nullptr
       to clean up resources
    5. AmsMgrProxy can handle null values for sa_ and proxy_ without crashing when OnProxyDied() is called
 * @tc.step:
    1. Create an AmsMgrProxy instance using new, set its sa_ and proxy_ to nullptr
    2. Call OnProxyDied() on the instance, then delete the instance to verify null pointer handling
    3. Create a new AmsMgrProxy instance using new, set its sa_ and proxy_ to nullptr
    4. Call ConnectSA() on the new instance to initialize resources
    5. Verify that sa_, proxy_, and deathRecipient_ are non-null after initialization
    6. Call OnProxyDied() on the instance to trigger resource cleanup
    7. Delete the instance to complete the test
 * @tc.expect:
    1. Step 2: No crash occurs when OnProxyDied() is called with null sa_ and proxy_
    2. Step 5: sa_, proxy_, and deathRecipient_ are all non-null after ConnectSA()
    3. Step 6: OnProxyDied() successfully cleans up resources (no crash during cleanup)
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
* @tc.precon:
    1. AmsMgrProxy::GetInstance() returns a singleton instance of AmsMgrProxy
    2. OnProxyDied() sets sa_ to null even if it was initially null
    3. ConnectSA() can re-initialize sa_, proxy_, and deathRecipient_ to non-null after OnProxyDied()
    4. AmsMgrProxy's destructor handles non-null member variables gracefully
* @tc.require: issueIBX9HL
* @tc.step:
    1. Get AmsMgrProxy singleton instance via GetInstance()
    2. Call OnProxyDied() to set sa_ to null
    3. Call ConnectSA() to re-establish SA connection
    4. Check the states of sa_, proxy_, and deathRecipient_ after ConnectSA()
* @tc.expect:
    1. After ConnectSA(): sa_ != nullptr, proxy_ != nullptr, deathRecipient_ != nullptr
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
 * @tc.desc: Verify AmsMgrProxy's behavior when proxy dies (creation, initialization, cleanup)
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. AmsMgrProxy class supports instantiation via new and destruction via delete
    2. AmsMgrProxy has member variables: sa_ (service pointer), proxy_, deathRecipient_ (death recipient pointer)
    3. AmsMgrProxy::ConnectSA() initializes sa_, proxy_, and deathRecipient_ to non-null on success
    4. AmsMgrProxy::OnProxyDied() sets sa_, proxy_, and deathRecipient_ to nullptr for resource cleanup
    5. AmsMgrProxy handles null values for sa_ and proxy_ gracefully (no crash when calling OnProxyDied())
 * @tc.step:
    1. Create an AmsMgrProxy instance via new, set its sa_ and proxy_ to nullptr
    2. Call OnProxyDied() on the instance, then delete the instance to test null handling
    3. Create a new AmsMgrProxy instance via new, set its sa_ and proxy_ to nullptr
    4. Call ConnectSA() on the new instance to initialize resources
    5. Verify sa_, proxy_, and deathRecipient_ are non-null after initialization
    6. Call OnProxyDied() on the instance to trigger cleanup
    7. Delete the instance to complete the test
 * @tc.expect:
    1. Step 2: No crash occurs when OnProxyDied() is called with null sa_ and proxy_
    2. Step 5: sa_, proxy_, and deathRecipient_ are all non-null (initialization success)
    3. Step 6: No crash occurs during resource cleanup (OnProxyDied() works)
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