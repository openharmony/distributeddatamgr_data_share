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

#include "datashare_proxy.h"

#include <gtest/gtest.h>

#include "accesstoken_kit.h"
#include "data_ability_observer_interface.h"
#include "datashare_connection.h"
#include "datashare_errno.h"
#include "datashare_helper.h"
#include "datashare_log.h"
#include "extension_manager_proxy.h"
#include "general_controller.h"
#include "general_controller_provider_impl.h"
#include "general_controller_service_impl.h"

namespace OHOS {
namespace DataShare {
using namespace testing::ext;
using namespace OHOS::AAFwk;
class DataShareProxyTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

class RemoteObjectTest : public IRemoteObject {
public:
    explicit RemoteObjectTest(std::u16string descriptor) : IRemoteObject(descriptor) {}
    ~RemoteObjectTest() {}
    
    int32_t GetObjectRefCount()
    {
        return 0;
    }
    int SendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
    {
        return 0;
    }
    bool AddDeathRecipient(const sptr<DeathRecipient> &recipient)
    {
        return true;
    }
    bool RemoveDeathRecipient(const sptr<DeathRecipient> &recipient)
    {
        return true;
    }
    int Dump(int fd, const std::vector<std::u16string> &args)
    {
        return 0;
    }
};

class IDataAbilityObserverTest : public DataAbilityObserverStub {
public:
    IDataAbilityObserverTest() {}
    ~IDataAbilityObserverTest()
    {}

    void OnChange()
    {
        GTEST_LOG_(INFO) << "OnChange enter";
    }
};

std::string DATA_SHARE_URI = "datashare:///com.acts.datasharetest";

void DataShareProxyTest::SetUpTestCase(void) {}
void DataShareProxyTest::TearDownTestCase(void) {}
void DataShareProxyTest::SetUp(void) {}
void DataShareProxyTest::TearDown(void) {}

/**
* @tc.name: DataShareProxy_RegisterObserverExtProvider_Test_001
* @tc.desc: test RegisterObserverExtProvider default func
* @tc.type: FUNC
*/
HWTEST_F(DataShareProxyTest, DataShareProxy_RegisterObserverExtProvider_Test_001, TestSize.Level0)
{
    LOG_INFO("DataShareProxy_RegisterObserverExtProvider_Test_001::Start");

    Uri uri(DATA_SHARE_URI);
    std::u16string tokenString = u"OHOS.DataShare.IDataShare";
    sptr<IRemoteObject> token = new (std::nothrow) RemoteObjectTest(tokenString);
    ASSERT_NE(token, nullptr);
    sptr<DataShare::DataShareConnection> connection =
        new (std::nothrow) DataShare::DataShareConnection(uri, token);
    ASSERT_NE(connection, nullptr);

    // get proxy not null
    std::shared_ptr<DataShareProxy> tokenProxy = std::make_shared<DataShareProxy>(token);
    ASSERT_NE(tokenProxy, nullptr);

    connection->dataShareProxy_ = tokenProxy;
    ASSERT_NE(connection->dataShareProxy_, nullptr);
    auto proxy = connection->GetDataShareProxy(uri, token);
    ASSERT_NE(proxy, nullptr);

    // Observer not null
    sptr<IDataAbilityObserverTest> dataObserver = new (std::nothrow) IDataAbilityObserverTest();
    ASSERT_NE(dataObserver, nullptr);
    // datashare_stub returns default 0(E_OK)
    int ret = proxy->RegisterObserverExtProvider(uri, dataObserver, true, { false });
    EXPECT_EQ(ret, 0);

    LOG_INFO("DataShareProxy_RegisterObserverExtProvider_Test_001::End");
}

/**
* @tc.name: DataShareProxy_UnregisterObserverExtProvider_Test_001
* @tc.desc: test UnregisterObserverExtProvider default func
* @tc.type: FUNC
*/
HWTEST_F(DataShareProxyTest, DataShareProxy_UnregisterObserverExtProvider_Test_001, TestSize.Level0)
{
    LOG_INFO("DataShareProxy_UnregisterObserverExtProvider_Test_001::Start");

    Uri uri(DATA_SHARE_URI);
    std::u16string tokenString = u"OHOS.DataShare.IDataShare";
    sptr<IRemoteObject> token = new (std::nothrow) RemoteObjectTest(tokenString);
    ASSERT_NE(token, nullptr);
    sptr<DataShare::DataShareConnection> connection =
        new (std::nothrow) DataShare::DataShareConnection(uri, token);
    ASSERT_NE(connection, nullptr);

    // get proxy not null
    std::shared_ptr<DataShareProxy> tokenProxy = std::make_shared<DataShareProxy>(token);
    ASSERT_NE(tokenProxy, nullptr);
    connection->dataShareProxy_ = tokenProxy;
    auto proxy = connection->GetDataShareProxy(uri, token);
    ASSERT_NE(proxy, nullptr);

    // observer  not null
    sptr<IDataAbilityObserverTest> dataObserver = new (std::nothrow) IDataAbilityObserverTest();
    ASSERT_NE(dataObserver, nullptr);
    // datashare_stub returns default 0(E_OK)
    int ret = proxy->UnregisterObserverExtProvider(uri, dataObserver);
    EXPECT_EQ(ret, 0);

    LOG_INFO("DataShareProxy_UnregisterObserverExtProvider_Test_001::End");
}

/**
* @tc.name: DataShareProxy_NotifyChangeExtProvider_Test_001
* @tc.desc: test NotifyChangeExtProvider default func
* @tc.type: FUNC
*/
HWTEST_F(DataShareProxyTest, DataShareProxy_NotifyChangeExtProvider_Test_001, TestSize.Level0)
{
    LOG_INFO("DataShareProxy_NotifyChangeExtProvider_Test_001::Start");

    Uri uri(DATA_SHARE_URI);
    std::u16string tokenString = u"OHOS.DataShare.IDataShare";
    sptr<IRemoteObject> token = new (std::nothrow) RemoteObjectTest(tokenString);
    ASSERT_NE(token, nullptr);
    sptr<DataShare::DataShareConnection> connection =
        new (std::nothrow) DataShare::DataShareConnection(uri, token);
    ASSERT_NE(connection, nullptr);

    // get proxy not null
    std::shared_ptr<DataShareProxy> tokenProxy = std::make_shared<DataShareProxy>(token);
    ASSERT_NE(tokenProxy, nullptr);
    connection->dataShareProxy_ = tokenProxy;
    auto proxy = connection->GetDataShareProxy(uri, token);
    ASSERT_NE(proxy, nullptr);

    ChangeInfo changeInfo = { ChangeInfo::ChangeType::INSERT, { uri } };
    // datashare_stub returns default 0(E_OK)
    int ret = proxy->NotifyChangeExtProvider(changeInfo);
    EXPECT_EQ(ret, 0);

    LOG_INFO("DataShareProxy_NotifyChangeExtProvider_Test_001::End");
}
}
}