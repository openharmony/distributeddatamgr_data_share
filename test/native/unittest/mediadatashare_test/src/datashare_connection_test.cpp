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

#include "datashare_connection.h"

#include <gtest/gtest.h>

#include "accesstoken_kit.h"
#include "data_ability_observer_interface.h"
#include "datashare_errno.h"
#include "datashare_helper.h"
#include "datashare_log.h"
#include "datashare_proxy.h"
#include "extension_manager_proxy.h"
#include "general_controller.h"
#include "general_controller_provider_impl.h"
#include "general_controller_service_impl.h"

namespace OHOS {
namespace DataShare {
using namespace testing::ext;
using namespace OHOS::AAFwk;
class DataShareConnectionTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    bool UrisEqual(std::list<Uri> uri1, std::list<Uri> uri2)
    {
        if (uri1.size() != uri2.size()) {
            return false;
        }
        auto cmp = [](const Uri &first, const Uri &second) {
            return first.ToString() < second.ToString();
        };
        uri1.sort(cmp);
        uri2.sort(cmp);
        auto it1 = uri1.begin();
        auto it2 = uri2.begin();
        for (; it1 != uri1.end() && it2 != uri2.end(); it1++, it2++) {
            if (!it1->Equals(*it2)) {
                return false;
            }
        }
        return true;
    }
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
    explicit IDataAbilityObserverTest(std::string uri) {uri_ = uri;}
    ~IDataAbilityObserverTest()
    {}

    void OnChange()
    {
        GTEST_LOG_(INFO) << "OnChange enter";
    }
    std::string uri_;
};

std::string DATA_SHARE_URI = "datashare:///com.acts.datasharetest";
std::string DATA_SHARE_URI1 = "datashare:///com.acts.datasharetest1";

void DataShareConnectionTest::SetUpTestCase(void) {}
void DataShareConnectionTest::TearDownTestCase(void) {}
void DataShareConnectionTest::SetUp(void) {}
void DataShareConnectionTest::TearDown(void) {}

/**
* @tc.name: DataShareConnection_UpdateObserverExtsProviderMap_Test_001
* @tc.desc: test UpdateObserverExtsProviderMap normal func
* @tc.type: FUNC
*/
HWTEST_F(DataShareConnectionTest, DataShareConnection_UpdateObserverExtsProviderMap_Test_001, TestSize.Level0)
{
    LOG_INFO("DataShareConnection_UpdateObserverExtsProviderMap_Test_001::Start");

    Uri uri(DATA_SHARE_URI);
    std::u16string tokenString = u"OHOS.DataShare.IDataShare";
    sptr<IRemoteObject> token = new (std::nothrow) RemoteObjectTest(tokenString);
    ASSERT_NE(token, nullptr);
    sptr<DataShare::DataShareConnection> connection =
        new (std::nothrow) DataShare::DataShareConnection(uri, token);
    ASSERT_NE(connection, nullptr);

    // insert data
    EXPECT_TRUE(connection->observerExtsProvider_.Empty());
    sptr<IDataAbilityObserverTest> dataObserver = new (std::nothrow) IDataAbilityObserverTest(DATA_SHARE_URI);
    ASSERT_NE(dataObserver, nullptr);
    connection->UpdateObserverExtsProviderMap(uri, dataObserver, true);

    Uri uri1(DATA_SHARE_URI1);
    sptr<IDataAbilityObserverTest> dataObserver1 = new (std::nothrow) IDataAbilityObserverTest(DATA_SHARE_URI1);
    ASSERT_NE(dataObserver1, nullptr);
    connection->UpdateObserverExtsProviderMap(uri1, dataObserver1, true);

    EXPECT_FALSE(connection->observerExtsProvider_.Empty());
    EXPECT_EQ(connection->observerExtsProvider_.Size(), 2);
    connection = nullptr;

    LOG_INFO("DataShareConnection_UpdateObserverExtsProviderMap_Test_001::End");
}

/**
* @tc.name: DataShareConnection_DeleteObserverExtsProviderMap_001
* @tc.desc: test DeleteObserverExtsProviderMap default func
* @tc.type: FUNC
*/
HWTEST_F(DataShareConnectionTest, DataShareConnection_DeleteObserverExtsProviderMap_001, TestSize.Level0)
{
    LOG_INFO("DataShareConnection_DeleteObserverExtsProviderMap_001::Start");

    Uri uri(DATA_SHARE_URI);
    std::u16string tokenString = u"OHOS.DataShare.IDataShare";
    sptr<IRemoteObject> token = new (std::nothrow) RemoteObjectTest(tokenString);
    ASSERT_NE(token, nullptr);
    sptr<DataShare::DataShareConnection> connection =
        new (std::nothrow) DataShare::DataShareConnection(uri, token);
    ASSERT_NE(connection, nullptr);

    // insert data
    EXPECT_TRUE(connection->observerExtsProvider_.Empty());
    sptr<IDataAbilityObserverTest> dataObserver = new (std::nothrow) IDataAbilityObserverTest(DATA_SHARE_URI);
    connection->UpdateObserverExtsProviderMap(uri, dataObserver, true);

    Uri uri1(DATA_SHARE_URI1);
    sptr<IDataAbilityObserverTest> dataObserver1 = new (std::nothrow) IDataAbilityObserverTest(DATA_SHARE_URI1);
    connection->UpdateObserverExtsProviderMap(uri1, dataObserver1, true);

    EXPECT_FALSE(connection->observerExtsProvider_.Empty());
    EXPECT_EQ(connection->observerExtsProvider_.Size(), 2);

    // delete data that uri can match observer
    connection->DeleteObserverExtsProviderMap(uri1, dataObserver1);
    EXPECT_EQ(connection->observerExtsProvider_.Size(), 1);

    // delete data that uri can not match observer
    connection->DeleteObserverExtsProviderMap(uri1, dataObserver);
    EXPECT_EQ(connection->observerExtsProvider_.Size(), 1);
    connection = nullptr;

    LOG_INFO("DataShareConnection_DeleteObserverExtsProviderMap_001::End");
}

/**
* @tc.name: DataShareConnection_ReRegisterObserverExtProvider_Test_001
* @tc.desc: test ReRegisterObserverExtProvider default func
* @tc.type: FUNC
*/
HWTEST_F(DataShareConnectionTest, DataShareConnection_ReRegisterObserverExtProvider_Test_001, TestSize.Level0)
{
    LOG_INFO("DataShareConnection_ReRegisterObserverExtProvider_Test_001::Start");

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

    // insert data
    EXPECT_TRUE(connection->observerExtsProvider_.Empty());
    sptr<IDataAbilityObserverTest> dataObserver = new (std::nothrow) IDataAbilityObserverTest(DATA_SHARE_URI);
    connection->UpdateObserverExtsProviderMap(uri, dataObserver, true);

    Uri uri1(DATA_SHARE_URI1);
    sptr<IDataAbilityObserverTest> dataObserver1 = new (std::nothrow) IDataAbilityObserverTest(DATA_SHARE_URI1);
    connection->UpdateObserverExtsProviderMap(uri1, dataObserver1, true);

    EXPECT_FALSE(connection->observerExtsProvider_.Empty());
    EXPECT_EQ(connection->observerExtsProvider_.Size(), 2);

    // test ReRegister func
    connection->ReRegisterObserverExtProvider();
    // reRegister success, update observer map
    EXPECT_FALSE(connection->observerExtsProvider_.Empty());
    connection = nullptr;

    LOG_INFO("DataShareConnection_ReRegisterObserverExtProvider_Test_001::End");
}

/**
* @tc.name: DataShareConnection_OnAbilityConnectDone_Test_001
* @tc.desc: test ReRegisterObserverExtProvider default func
* @tc.type: FUNC
*/
HWTEST_F(DataShareConnectionTest, DataShareConnection_OnAbilityConnectDone_Test_001, TestSize.Level0)
{
    LOG_INFO("DataShareConnection_OnAbilityConnectDone_Test_001::Start");

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

    // insert data
    EXPECT_TRUE(connection->observerExtsProvider_.Empty());
    sptr<IDataAbilityObserverTest> dataObserver = new (std::nothrow) IDataAbilityObserverTest(DATA_SHARE_URI);
    connection->UpdateObserverExtsProviderMap(uri, dataObserver, true);

    Uri uri1(DATA_SHARE_URI1);
    sptr<IDataAbilityObserverTest> dataObserver1 = new (std::nothrow) IDataAbilityObserverTest(DATA_SHARE_URI1);
    connection->UpdateObserverExtsProviderMap(uri1, dataObserver1, true);

    EXPECT_FALSE(connection->observerExtsProvider_.Empty());
    EXPECT_EQ(connection->observerExtsProvider_.Size(), 2);

    // test ReRegister func
    connection->isReconnect_.store(true);
    std::string deviceId = "deviceId";
    std::string bundleName = "bundleName";
    std::string abilityName = "abilityName";
    AppExecFwk::ElementName element(deviceId, bundleName, abilityName);
    int resultCode = 0;
    connection->OnAbilityConnectDone(element, token, resultCode);

    // reRegister success, update observer map
    EXPECT_FALSE(connection->observerExtsProvider_.Empty());
    connection = nullptr;

    LOG_INFO("DataShareConnection_OnAbilityConnectDone_Test_001::End");
}

/**
* @tc.name: DataShareConnection_OnAbilityDisconnectDone_Test_001
* @tc.desc: Verifying that the thread name of Pool_ is correctly set.
* @tc.type: FUNC
*/
HWTEST_F(DataShareConnectionTest, DataShareConnection_OnAbilityDisconnectDone_Test_001, TestSize.Level1)
{
    LOG_INFO("DataShareConnection_OnAbilityDisconnectDone_Test_001::Start");
    Uri uri(DATA_SHARE_URI);
    std::u16string tokenString = u"OHOS.DataShare.IDataShare";
    sptr<IRemoteObject> token = new (std::nothrow) RemoteObjectTest(tokenString);
    sptr<DataShare::DataShareConnection> connection =
        new (std::nothrow) DataShare::DataShareConnection(uri, token);

    connection->isReconnect_.store(true);
    std::string deviceId = "deviceId";
    std::string bundleName = "bundleName";
    std::string abilityName = "abilityName";
    AppExecFwk::ElementName element(deviceId, bundleName, abilityName);
    int resultCode = 0;
    connection->OnAbilityDisconnectDone(element, resultCode);
    EXPECT_EQ(connection->pool_->pool_.threadName_, "DShare_Connect");
    LOG_INFO("DataShareConnection_OnAbilityDisconnectDone_Test_001::End");
}
}
}