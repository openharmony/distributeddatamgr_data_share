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

#define LOG_TAG "datashare_connection_test"

#include "datashare_connection.h"

#include <chrono>
#include <gtest/gtest.h>
#include <thread>

#include "accesstoken_kit.h"
#include "ams_mgr_proxy.h"
#include "data_ability_observer_interface.h"
#include "datashare_errno.h"
#include "datashare_helper.h"
#include "datashare_log.h"
#include "datashare_proxy.h"
#include "datashare_task_executor.h"
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
    void OnAbilityConnectDone(const std::shared_ptr<DataShare::DataShareConnection> &connection,
        const sptr<IRemoteObject> &token, std::atomic<bool> &stop);
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

class AmsMgrProxyMock : public AmsMgrProxy {
public:
    explicit AmsMgrProxyMock() {}
    ~AmsMgrProxyMock() {}

    int Connect(const std::string &uri, const sptr<IRemoteObject> &connect,
        const sptr<IRemoteObject> &callerToken)
    {
        (void)uri;
        (void)connect;
        (void)callerToken;
        connectCount_++;
        return connectResult_;
    }
    int DisConnect(sptr<IRemoteObject> connect)
    {
        (void)connect;
        disConnectCount_++;
        return 0;
    }

    static AmsMgrProxyMock* GetInstance()
    {
        if (GetInstanceNull()) {
            return nullptr;
        }
        return GetSingleton();
    }
    static void SetConnectResult(int result)
    {
        GetSingleton()->connectResult_ = result;
    }
    static int GetConnectCount()
    {
        return GetSingleton()->connectCount_;
    }
    static int GetDisConnectCount()
    {
        return GetSingleton()->disConnectCount_;
    }
    static void SetGetInstanceNull(bool isNull)
    {
        GetInstanceNull() = isNull;
    }
    static void Reset()
    {
        AmsMgrProxyMock* instance = GetSingleton();
        instance->connectCount_ = 0;
        instance->disConnectCount_ = 0;
        instance->connectResult_ = 0;
        GetInstanceNull() = false;
    }

private:
    static AmsMgrProxyMock* GetSingleton()
    {
        static AmsMgrProxyMock instance;
        return &instance;
    }
    static bool& GetInstanceNull()
    {
        static bool isNull = false;
        return isNull;
    }

    // Increased on executor pool threads and polled from the test thread, so atomic is required.
    std::atomic<int> connectCount_ = 0;
    std::atomic<int> disConnectCount_ = 0;
    std::atomic<int> connectResult_ = 0;
};

AmsMgrProxy::~AmsMgrProxy() {}

AmsMgrProxy* AmsMgrProxy::GetInstance()
{
    return AmsMgrProxyMock::GetInstance();
}

int AmsMgrProxy::Connect(const std::string &uri, const sptr<IRemoteObject> &connect,
    const sptr<IRemoteObject> &callerToken)
{
    AmsMgrProxyMock* mock = AmsMgrProxyMock::GetInstance();
    if (mock == nullptr) {
        return -1;
    }
    return mock->Connect(uri, connect, callerToken);
}

int AmsMgrProxy::DisConnect(sptr<IRemoteObject> connect)
{
    AmsMgrProxyMock* mock = AmsMgrProxyMock::GetInstance();
    if (mock == nullptr) {
        return -1;
    }
    return mock->DisConnect(connect);
}

std::mutex AmsMgrProxy::pmutex_;

std::string g_dataShareUri = "datashare:///com.acts.datasharetest";
std::string g_dataShareUrI1 = "datashare:///com.acts.datasharetest1";
constexpr int TEST_TIME = 20;

void DataShareConnectionTest::SetUpTestCase(void) {}
void DataShareConnectionTest::TearDownTestCase(void) {}
void DataShareConnectionTest::SetUp(void) {}
void DataShareConnectionTest::TearDown(void) {}

void DataShareConnectionTest::OnAbilityConnectDone(const std::shared_ptr<DataShare::DataShareConnection> &connection,
    const sptr<IRemoteObject> &token, std::atomic<bool> &stop)
{
    int i = 0;
    while (!stop.load()) {
        LOG_INFO("OnAbilityConnectDone start %{public}d", i);
        std::string deviceId = "deviceId";
        std::string bundleName = "bundleName";
        std::string abilityName = "abilityName";
        AppExecFwk::ElementName element(deviceId, bundleName, abilityName);
        int resultCode = 0;
        connection->OnAbilityConnectDone(element, token, resultCode);
        LOG_INFO("OnAbilityConnectDone end %{public}d", i);
        i++;
    }
}

/**
 * @tc.name: DataShareConnection_UpdateObserverExtsProviderMap_Test_001
 * @tc.desc: Verify the normal functionality of the UpdateObserverExtsProviderMap method in DataShareConnection,
 *           focusing on whether observers can be correctly added to the observer map and the map size is
 *           updated accordingly.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment is properly set up, supporting the instantiation and operation of DataShareConnection
       and related classes.
    2. Valid test URIs (g_dataShareUri and g_dataShareUrI1) are predefined and accessible.
    3. The IRemoteObject and IDataAbilityObserverTest classes can be normally instantiated without initialization
       errors.
 * @tc.step:
    1. Create a DataShareConnection object using the test URI (g_dataShareUri) and a valid IRemoteObject token.
    2. Verify that the observerExtsProvider_ map in the created DataShareConnection is initially empty.
    3. Create the first IDataAbilityObserverTest instance with g_dataShareUri, and add it to the map using
       UpdateObserverExtsProviderMap with the "true" flag.
    4. Create the second IDataAbilityObserverTest instance with g_dataShareUrI1, and add it to the map using
       UpdateObserverExtsProviderMap with the "true" flag.
    5. Check the size of the observerExtsProvider_ map after adding the two observers.
 * @tc.expect:
    1. The observerExtsProvider_ map is empty before adding any observers.
    2. After adding the two observers, the size of the observerExtsProvider_ map is 2.
 */
HWTEST_F(DataShareConnectionTest, DataShareConnection_UpdateObserverExtsProviderMap_Test_001, TestSize.Level0)
{
    LOG_INFO("DataShareConnection_UpdateObserverExtsProviderMap_Test_001::Start");

    Uri uri(g_dataShareUri);
    std::u16string tokenString = u"OHOS.DataShare.IDataShare";
    sptr<IRemoteObject> token = new (std::nothrow) RemoteObjectTest(tokenString);
    ASSERT_NE(token, nullptr);
    std::shared_ptr<DataShare::DataShareConnection> connection =
        std::make_shared<DataShare::DataShareConnection>(uri, token);
    ASSERT_NE(connection, nullptr);

    // insert data
    EXPECT_TRUE(connection->observerExtsProvider_.Empty());
    sptr<IDataAbilityObserverTest> dataObserver = new (std::nothrow) IDataAbilityObserverTest(g_dataShareUri);
    ASSERT_NE(dataObserver, nullptr);
    connection->UpdateObserverExtsProviderMap(uri, dataObserver, true);

    Uri uri1(g_dataShareUrI1);
    sptr<IDataAbilityObserverTest> dataObserver1 = new (std::nothrow) IDataAbilityObserverTest(g_dataShareUrI1);
    ASSERT_NE(dataObserver1, nullptr);
    connection->UpdateObserverExtsProviderMap(uri1, dataObserver1, true);

    EXPECT_FALSE(connection->observerExtsProvider_.Empty());
    EXPECT_EQ(connection->observerExtsProvider_.Size(), 2);
    connection = nullptr;

    LOG_INFO("DataShareConnection_UpdateObserverExtsProviderMap_Test_001::End");
}

/**
 * @tc.name: DataShareConnection_DeleteObserverExtsProviderMap_001
 * @tc.desc: Verify the functionality of the DeleteObserverExtsProviderMap method in DataShareConnection, including
 *           correctly removing valid observers and leaving the map unchanged when removing invalid observers.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports the creation of DataShareConnection, IRemoteObject, and IDataAbilityObserverTest
       instances.
    2. Two distinct test URIs (g_dataShareUri and g_dataShareUrI1) are available for observer initialization.
    3. The UpdateObserverExtsProviderMap method can successfully add observers to the observerExtsProvider_ map.
 * @tc.step:
    1. Create a DataShareConnection object with the test URI (g_dataShareUri) and a valid IRemoteObject token.
    2. Add two observers to the observerExtsProvider_ map using UpdateObserverExtsProviderMap: one with g_dataShareUri
       and another with g_dataShareUrI1.
    3. Verify that the initial size of the observerExtsProvider_ map is 2 after adding the observers.
    4. Call DeleteObserverExtsProviderMap to remove the observer associated with g_dataShareUrI1, then check the map
       size.
    5. Call DeleteObserverExtsProviderMap again to attempt removing an invalid observer (mismatched URI and observer),
       then check the map size.
 * @tc.expect:
    1. After deleting the valid observer (g_dataShareUrI1), the size of the observerExtsProvider_ map is 1.
    2. After attempting to delete the invalid observer, the size of the observerExtsProvider_ map remains 1.
 */
HWTEST_F(DataShareConnectionTest, DataShareConnection_DeleteObserverExtsProviderMap_001, TestSize.Level0)
{
    LOG_INFO("DataShareConnection_DeleteObserverExtsProviderMap_001::Start");

    Uri uri(g_dataShareUri);
    std::u16string tokenString = u"OHOS.DataShare.IDataShare";
    sptr<IRemoteObject> token = new (std::nothrow) RemoteObjectTest(tokenString);
    ASSERT_NE(token, nullptr);
    std::shared_ptr<DataShare::DataShareConnection> connection =
        std::make_shared<DataShare::DataShareConnection>(uri, token);
    ASSERT_NE(connection, nullptr);

    // insert data
    EXPECT_TRUE(connection->observerExtsProvider_.Empty());
    sptr<IDataAbilityObserverTest> dataObserver = new (std::nothrow) IDataAbilityObserverTest(g_dataShareUri);
    connection->UpdateObserverExtsProviderMap(uri, dataObserver, true);

    Uri uri1(g_dataShareUrI1);
    sptr<IDataAbilityObserverTest> dataObserver1 = new (std::nothrow) IDataAbilityObserverTest(g_dataShareUrI1);
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
 * @tc.desc: Verify that the ReRegisterObserverExtProvider method in DataShareConnection can successfully re-register
 *           existing observers, ensuring the observer map remains populated after re-registration.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment allows instantiation of DataShareConnection, DataShareProxy, IRemoteObject, and
       IDataAbilityObserverTest objects.
    2. A valid DataShareProxy instance can be associated with the DataShareConnection's dataShareProxy_ member.
    3. The UpdateObserverExtsProviderMap method works correctly to add observers to the map.
 * @tc.step:
    1. Create a DataShareConnection object with the test URI (g_dataShareUri) and a valid IRemoteObject token.
    2. Create a DataShareProxy instance using the token and assign it to the dataShareProxy_ member of the
       DataShareConnection.
    3. Add two observers to the observerExtsProvider_ map using UpdateObserverExtsProviderMap (with g_dataShareUri and
       g_dataShareUrI1 respectively).
    4. Verify that the observerExtsProvider_ map is not empty and has a size of 2 after adding the observers.
    5. Call the ReRegisterObserverExtProvider method of the DataShareConnection.
    6. Check whether the observerExtsProvider_ map remains non-empty after re-registration.
 * @tc.expect:
    1. The observerExtsProvider_ map remains non-empty after calling ReRegisterObserverExtProvider.
 */
HWTEST_F(DataShareConnectionTest, DataShareConnection_ReRegisterObserverExtProvider_Test_001, TestSize.Level0)
{
    LOG_INFO("DataShareConnection_ReRegisterObserverExtProvider_Test_001::Start");

    Uri uri(g_dataShareUri);
    std::u16string tokenString = u"OHOS.DataShare.IDataShare";
    sptr<IRemoteObject> token = new (std::nothrow) RemoteObjectTest(tokenString);
    ASSERT_NE(token, nullptr);
    std::shared_ptr<DataShare::DataShareConnection> connection =
        std::make_shared<DataShare::DataShareConnection>(uri, token);
    ASSERT_NE(connection, nullptr);

    // get proxy not null
    std::shared_ptr<DataShareProxy> tokenProxy = std::make_shared<DataShareProxy>(token);
    ASSERT_NE(tokenProxy, nullptr);
    connection->dataShareProxy_ = tokenProxy;

    // insert data
    EXPECT_TRUE(connection->observerExtsProvider_.Empty());
    sptr<IDataAbilityObserverTest> dataObserver = new (std::nothrow) IDataAbilityObserverTest(g_dataShareUri);
    connection->UpdateObserverExtsProviderMap(uri, dataObserver, true);

    Uri uri1(g_dataShareUrI1);
    sptr<IDataAbilityObserverTest> dataObserver1 = new (std::nothrow) IDataAbilityObserverTest(g_dataShareUrI1);
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
 * @tc.name: DataShareConnection_ReRegisterObserverExtProvider_Test_002
 * @tc.desc: Verify that test won't crash if call this method with nullptr proxy
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: NA
 * @tc.step:
    1. Create a DataShareConnection object with the demo test URI (g_dataShareUri) and a demo IRemoteObject token.
    2. Set dataShareProxy to be nullptr.
    3. Call ReRegisterObserverExtProvider and the test won't crash.
 * @tc.expect:
    1. The test won't crash.
 */
HWTEST_F(DataShareConnectionTest, DataShareConnection_ReRegisterObserverExtProvider_Test_002, TestSize.Level1)
{
    LOG_INFO("DataShareConnection_ReRegisterObserverExtProvider_Test_002::Start");
    Uri uri(g_dataShareUri);
    std::u16string tokenString = u"OHOS.DataShare.IDataShare";
    sptr<IRemoteObject> token = new (std::nothrow) RemoteObjectTest(tokenString);
    std::shared_ptr<DataShare::DataShareConnection> connection =
        std::make_shared<DataShare::DataShareConnection>(uri, token);
    ASSERT_NE(connection, nullptr);
    connection->dataShareProxy_ = nullptr;
    // Add observers
    EXPECT_TRUE(connection->observerExtsProvider_.Empty());
    sptr<IDataAbilityObserverTest> dataObserver = new (std::nothrow) IDataAbilityObserverTest(g_dataShareUri);
    connection->UpdateObserverExtsProviderMap(uri, dataObserver, true);
    EXPECT_EQ(connection->observerExtsProvider_.Size(), 1);
    // Shouldn't crash
    connection->ReRegisterObserverExtProvider();
    LOG_INFO("DataShareConnection_ReRegisterObserverExtProvider_Test_002::End");
}

/**
 * @tc.name: DataShareConnection_OnAbilityConnectDone_Test_001
 * @tc.desc: Verify that the OnAbilityConnectDone method in DataShareConnection correctly handles reconnection (when
 *           isReconnect_ is true) and ensures the observer map remains intact after the connection is established.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports creating DataShareConnection, DataShareProxy, IRemoteObject,
       IDataAbilityObserverTest, and AppExecFwk::ElementName instances.
    2. The isReconnect_ member of DataShareConnection can be set to true via the store method.
    3. Observers can be successfully added to the observerExtsProvider_ map using UpdateObserverExtsProviderMap.
 * @tc.step:
    1. Create a DataShareConnection object with the test URI (g_dataShareUri) and a valid IRemoteObject token.
    2. Create a DataShareProxy instance using the token and assign it to the dataShareConnection's dataShareProxy_
       member.
    3. Add two observers to the observerExtsProvider_ map (with g_dataShareUri and g_dataShareUrI1 respectively) using
       UpdateObserverExtsProviderMap.
    4. Set the isReconnect_ flag of the DataShareConnection to true using isReconnect_.store(true).
    5. Create an AppExecFwk::ElementName instance with test parameters and a resultCode of 0.
    6. Call the OnAbilityConnectDone method with the ElementName, token, and resultCode as parameters.
    7. Check whether the observerExtsProvider_ map remains non-empty after the connection is done.
 * @tc.expect:
    1. The observerExtsProvider_ map remains non-empty after the OnAbilityConnectDone method is executed.
 */
HWTEST_F(DataShareConnectionTest, DataShareConnection_OnAbilityConnectDone_Test_001, TestSize.Level0)
{
    LOG_INFO("DataShareConnection_OnAbilityConnectDone_Test_001::Start");

    Uri uri(g_dataShareUri);
    std::u16string tokenString = u"OHOS.DataShare.IDataShare";
    sptr<IRemoteObject> token = new (std::nothrow) RemoteObjectTest(tokenString);
    ASSERT_NE(token, nullptr);
    std::shared_ptr<DataShare::DataShareConnection> connection =
        std::make_shared<DataShare::DataShareConnection>(uri, token);
    ASSERT_NE(connection, nullptr);

    // get proxy not null
    std::shared_ptr<DataShareProxy> tokenProxy = std::make_shared<DataShareProxy>(token);
    ASSERT_NE(tokenProxy, nullptr);
    connection->dataShareProxy_ = tokenProxy;

    // insert data
    EXPECT_TRUE(connection->observerExtsProvider_.Empty());
    sptr<IDataAbilityObserverTest> dataObserver = new (std::nothrow) IDataAbilityObserverTest(g_dataShareUri);
    connection->UpdateObserverExtsProviderMap(uri, dataObserver, true);

    Uri uri1(g_dataShareUrI1);
    sptr<IDataAbilityObserverTest> dataObserver1 = new (std::nothrow) IDataAbilityObserverTest(g_dataShareUrI1);
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
 * @tc.name: DataShareConnection_OnAbilityConnectDone_Test_002
 * @tc.desc: Verify OnAbilityConnectDone method called after disconnect
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: None
 * @tc.step:
    1. Create a DataShareConnection object with the test URI (g_dataShareUri) and a valid IRemoteObject token.
    2. Create a DataShareProxy instance using the token and assign it to the dataShareConnection's dataShareProxy_
       member.
    3. Add two observers to the observerExtsProvider_ map (with g_dataShareUri and g_dataShareUrI1 respectively) using
       UpdateObserverExtsProviderMap.
    4. Set the isInvalid_ flag of the DataShareConnection to true.
    5. Create an AppExecFwk::ElementName instance with test parameters and a resultCode of 0.
    6. Call the OnAbilityConnectDone method with the ElementName, token, and resultCode as parameters.
    7. Check whether the observerExtsProvider_ map remains non-empty after the connection is done.
 * @tc.expect:
    1. The observerExtsProvider_ map remains non-empty after the OnAbilityConnectDone method is executed.
 */
HWTEST_F(DataShareConnectionTest, DataShareConnection_OnAbilityConnectDone_Test_002, TestSize.Level0)
{
    LOG_INFO("DataShareConnection_OnAbilityConnectDone_Test_002::Start");

    Uri uri(g_dataShareUri);
    std::u16string tokenString = u"OHOS.DataShare.IDataShare";
    sptr<IRemoteObject> token = new (std::nothrow) RemoteObjectTest(tokenString);
    ASSERT_NE(token, nullptr);
    std::shared_ptr<DataShare::DataShareConnection> connection =
        std::make_shared<DataShare::DataShareConnection>(uri, token);
    ASSERT_NE(connection, nullptr);

    // get proxy not null
    std::shared_ptr<DataShareProxy> tokenProxy = std::make_shared<DataShareProxy>(token);
    ASSERT_NE(tokenProxy, nullptr);
    connection->dataShareProxy_ = tokenProxy;

    // insert data
    EXPECT_TRUE(connection->observerExtsProvider_.Empty());
    sptr<IDataAbilityObserverTest> dataObserver = new (std::nothrow) IDataAbilityObserverTest(g_dataShareUri);
    connection->UpdateObserverExtsProviderMap(uri, dataObserver, true);

    Uri uri1(g_dataShareUrI1);
    sptr<IDataAbilityObserverTest> dataObserver1 = new (std::nothrow) IDataAbilityObserverTest(g_dataShareUrI1);
    connection->UpdateObserverExtsProviderMap(uri1, dataObserver1, true);

    EXPECT_FALSE(connection->observerExtsProvider_.Empty());
    EXPECT_EQ(connection->observerExtsProvider_.Size(), 2);

    // test onAbilityConnectDone after disconnect scene
    connection->isInvalid_.store(true);
    std::string deviceId = "deviceId";
    std::string bundleName = "bundleName";
    std::string abilityName = "abilityName";
    AppExecFwk::ElementName element(deviceId, bundleName, abilityName);
    int resultCode = 0;
    connection->OnAbilityConnectDone(element, token, resultCode);
    EXPECT_FALSE(connection->observerExtsProvider_.Empty());
    connection = nullptr;

    LOG_INFO("DataShareConnection_OnAbilityConnectDone_Test_002::End");
}

/**
 * @tc.name: DataShareConnection_OnAbilityDisconnectDone_Test_001
 * @tc.desc: Verify that the thread name of the executor pool shared by all DataShare components through
 *           DataShareTaskExecutor is correctly set to the expected value after the OnAbilityDisconnectDone
 *           method is called during reconnection.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment allows instantiation of DataShareConnection, IRemoteObject, and ElementName objects.
    2. The isReconnect_ member of DataShareConnection can be set to true, and the shared executor pool is properly
       initialized.
    3. The expected thread name is predefined and accessible.
 * @tc.step:
    1. Create a DataShareConnection object with the test URI (g_dataShareUri) and a valid IRemoteObject token.
    2. Set the isReconnect_ flag of the DataShareConnection to true using isReconnect_.store(true).
    3. Create an AppExecFwk::ElementName instance with test parameters and a resultCode of 0.
    4. Call the OnAbilityDisconnectDone method with the ElementName and resultCode as parameters.
    5. Get the shared executor from DataShareTaskExecutor and check the threadName_ member of its pool.
 * @tc.expect:
    1. The threadName_ of the shared executor pool is set to DATASHARE_EXECUTOR_NAME after
       OnAbilityDisconnectDone is called.
 */
HWTEST_F(DataShareConnectionTest, DataShareConnection_OnAbilityDisconnectDone_Test_001, TestSize.Level1)
{
    LOG_INFO("DataShareConnection_OnAbilityDisconnectDone_Test_001::Start");
    Uri uri(g_dataShareUri);
    std::u16string tokenString = u"OHOS.DataShare.IDataShare";
    sptr<IRemoteObject> token = new (std::nothrow) RemoteObjectTest(tokenString);
    std::shared_ptr<DataShare::DataShareConnection> connection =
        std::make_shared<DataShare::DataShareConnection>(uri, token);

    connection->isReconnect_.store(true);
    std::string deviceId = "deviceId";
    std::string bundleName = "bundleName";
    std::string abilityName = "abilityName";
    AppExecFwk::ElementName element(deviceId, bundleName, abilityName);
    int resultCode = 0;
    connection->OnAbilityDisconnectDone(element, resultCode);
    auto executor = DataShareTaskExecutor::GetInstance().GetExecutor();
    ASSERT_NE(executor, nullptr);
    EXPECT_EQ(executor->pool_.threadName_, DATASHARE_EXECUTOR_NAME);
    LOG_INFO("DataShareConnection_OnAbilityDisconnectDone_Test_001::End");
}

/**
 * @tc.name: DataShareConnection_ConcurrentOnAbilityConnectDone_Test_001
 * @tc.desc: Verify concurrent OnAbilityConnectDone operations
 * @tc.type: concurrent
 * @tc.require: None
 * @tc.precon: None
 * @tc.step:
    1. Create a DataShareConnection instance
    2. Get DataShareProxy
    3. Create three threads to concurrently perform onAbilityConnectDone
    4. Run the concurrent operations for a specified test duration
    5. Stop all threads and wait for their completion
 * @tc.expect:
    1. All concurrent OnAbilityConnectDone operations complete without crashes
    2. No deadlocks occur during concurrent observer management
 */
HWTEST_F(DataShareConnectionTest, DataShareConnection_ConcurrentOnAbilityConnectDone_Test_001, TestSize.Level0)
{
    LOG_INFO("DataShareConnection_ConcurrentOnAbilityConnectDone_Test_001::Start");
    std::atomic<bool> stop = false;
    int testTime = TEST_TIME;

    Uri uri(g_dataShareUri);
    std::u16string tokenString = u"OHOS.DataShare.IDataShare";
    sptr<IRemoteObject> token = new (std::nothrow) RemoteObjectTest(tokenString);
    ASSERT_NE(token, nullptr);
    std::shared_ptr<DataShare::DataShareConnection> connection =
        std::make_shared<DataShare::DataShareConnection>(uri, token);
    ASSERT_NE(connection, nullptr);

    // get proxy not null
    std::shared_ptr<DataShareProxy> tokenProxy = std::make_shared<DataShareProxy>(token);
    ASSERT_NE(tokenProxy, nullptr);
    connection->dataShareProxy_ = tokenProxy;

    // insert data
    EXPECT_TRUE(connection->observerExtsProvider_.Empty());
    sptr<IDataAbilityObserverTest> dataObserver = new (std::nothrow) IDataAbilityObserverTest(g_dataShareUri);
    ASSERT_NE(dataObserver, nullptr);
    connection->UpdateObserverExtsProviderMap(uri, dataObserver, true);

    Uri uri1(g_dataShareUrI1);
    sptr<IDataAbilityObserverTest> dataObserver1 = new (std::nothrow) IDataAbilityObserverTest(g_dataShareUrI1);
    ASSERT_NE(dataObserver1, nullptr);
    connection->UpdateObserverExtsProviderMap(uri1, dataObserver1, true);

    EXPECT_FALSE(connection->observerExtsProvider_.Empty());
    EXPECT_EQ(connection->observerExtsProvider_.Size(), 2);
    std::function<void()> func1 = [&connection, &token, &stop, this]() {
        OnAbilityConnectDone(connection, token, stop);
    };
    std::function<void()> func2 = [&connection, &token, &stop, this]() {
        OnAbilityConnectDone(connection, token, stop);
    };
    std::function<void()> func3 = [&connection, &token, &stop, this]() {
        OnAbilityConnectDone(connection, token, stop);
    };
    std::function<void()> func4 = [&connection, &token, &stop, this]() {
        OnAbilityConnectDone(connection, token, stop);
    };
    std::thread t1(func1);
    std::thread t2(func2);
    std::thread t3(func3);
    std::thread t4(func4);
    sleep(testTime);
    stop = true;
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    LOG_INFO("DataShareConnection_ConcurrentOnAbilityConnectDone_Test_001::End");
}

/**
 * @tc.name: DataShareConnectionCallback_OnConnectDone_TargetAlive_001
 * @tc.desc: Verify DataShareConnectionCallback forwards OnAbilityConnectDone to the live target and updates
 *           dataShareProxy_ on the connection.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
 *     1. A DataShareConnection can be instantiated via std::make_shared (shared ownership) so that
 *        weak_from_this() yields a valid weak_ptr.
 *     2. A DataShareConnectionCallback can be constructed with a weak_ptr<DataShareConnection> target.
 * @tc.step:
 *     1. Create a DataShareConnection with the test URI and a valid token, hold it via shared_ptr.
 *     2. Construct a DataShareConnectionCallback bound to that weak_ptr target.
 *     3. Invoke callback->OnAbilityConnectDone with a non-null remoteObject and resultCode=0.
 *     4. Verify connection->dataShareProxy_ is now non-null.
 * @tc.expect:
 *     1. The callback forwards to DataShareConnection::OnAbilityConnectDone successfully.
 *     2. dataShareProxy_ is populated after the callback returns.
 */
HWTEST_F(DataShareConnectionTest, DataShareConnectionCallback_OnConnectDone_TargetAlive_001, TestSize.Level0)
{
    LOG_INFO("DataShareConnectionCallback_OnConnectDone_TargetAlive_001::Start");
    Uri uri(g_dataShareUri);
    std::u16string tokenString = u"OHOS.DataShare.IDataShare";
    sptr<IRemoteObject> token = new (std::nothrow) RemoteObjectTest(tokenString);
    ASSERT_NE(token, nullptr);
    auto connection = std::make_shared<DataShare::DataShareConnection>(uri, token);
    ASSERT_NE(connection, nullptr);

    sptr<DataShare::DataShareConnection::ConnectionCallback> callback =
        new (std::nothrow) DataShare::DataShareConnection::ConnectionCallback(connection);
    ASSERT_NE(callback, nullptr);

    std::string deviceId = "deviceId";
    std::string bundleName = "bundleName";
    std::string abilityName = "abilityName";
    AppExecFwk::ElementName element(deviceId, bundleName, abilityName);
    callback->OnAbilityConnectDone(element, token, 0);

    EXPECT_NE(connection->dataShareProxy_, nullptr);
    connection.reset();
    LOG_INFO("DataShareConnectionCallback_OnConnectDone_TargetAlive_001::End");
}

/**
 * @tc.name: DataShareConnectionCallback_OnConnectDone_TargetExpired_002
 * @tc.desc: Verify DataShareConnectionCallback::OnAbilityConnectDone does not crash when the target weak_ptr is
 *           already expired (DataShareConnection destroyed).
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
 *     1. A DataShareConnectionCallback can be constructed with an empty weak_ptr.
 * @tc.step:
 *     1. Construct a DataShareConnectionCallback with an empty weak_ptr (target already gone).
 *     2. Invoke callback->OnAbilityConnectDone with a non-null remoteObject and resultCode=0.
 * @tc.expect:
 *     1. No crash; the callback silently drops the event when target_ is expired.
 */
HWTEST_F(DataShareConnectionTest, DataShareConnectionCallback_OnConnectDone_TargetExpired_002, TestSize.Level0)
{
    LOG_INFO("DataShareConnectionCallback_OnConnectDone_TargetExpired_002::Start");
    sptr<DataShare::DataShareConnection::ConnectionCallback> callback =
        new (std::nothrow) DataShare::DataShareConnection::ConnectionCallback(
            std::weak_ptr<DataShare::DataShareConnection>());
    ASSERT_NE(callback, nullptr);

    std::string deviceId = "deviceId";
    std::string bundleName = "bundleName";
    std::string abilityName = "abilityName";
    AppExecFwk::ElementName element(deviceId, bundleName, abilityName);
    callback->OnAbilityConnectDone(element, nullptr, 0);
    LOG_INFO("DataShareConnectionCallback_OnConnectDone_TargetExpired_002::End");
}

/**
 * @tc.name: DataShareConnectionCallback_OnDisconnectDone_TargetAlive_001
 * @tc.desc: Verify DataShareConnectionCallback forwards OnAbilityDisconnectDone to the live target and the
 *           connection tolerates the call (no crash).
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
 *     1. A DataShareConnection can be instantiated via std::make_shared.
 * @tc.step:
 *     1. Create a DataShareConnection with the test URI and a valid token, hold it via shared_ptr.
 *     2. Construct a DataShareConnectionCallback bound to that weak_ptr target.
 *     3. Pre-populate dataShareProxy_ so the connection has something to clear.
 *     4. Invoke callback->OnAbilityDisconnectDone with resultCode=0.
 * @tc.expect:
 *     1. The callback forwards to DataShareConnection::OnAbilityDisconnectDone without crashing.
 *     2. connection remains alive after the callback returns.
 */
HWTEST_F(DataShareConnectionTest, DataShareConnectionCallback_OnDisconnectDone_TargetAlive_001, TestSize.Level0)
{
    LOG_INFO("DataShareConnectionCallback_OnDisconnectDone_TargetAlive_001::Start");
    Uri uri(g_dataShareUri);
    std::u16string tokenString = u"OHOS.DataShare.IDataShare";
    sptr<IRemoteObject> token = new (std::nothrow) RemoteObjectTest(tokenString);
    ASSERT_NE(token, nullptr);
    auto connection = std::make_shared<DataShare::DataShareConnection>(uri, token);
    ASSERT_NE(connection, nullptr);

    std::shared_ptr<DataShareProxy> tokenProxy = std::make_shared<DataShareProxy>(token);
    ASSERT_NE(tokenProxy, nullptr);
    connection->dataShareProxy_ = tokenProxy;

    sptr<DataShare::DataShareConnection::ConnectionCallback> callback =
        new (std::nothrow) DataShare::DataShareConnection::ConnectionCallback(connection);
    ASSERT_NE(callback, nullptr);

    std::string deviceId = "deviceId";
    std::string bundleName = "bundleName";
    std::string abilityName = "abilityName";
    AppExecFwk::ElementName element(deviceId, bundleName, abilityName);
    callback->OnAbilityDisconnectDone(element, 0);

    EXPECT_NE(connection.get(), nullptr);
    connection.reset();
    LOG_INFO("DataShareConnectionCallback_OnDisconnectDone_TargetAlive_001::End");
}

/**
 * @tc.name: DataShareConnectionCallback_OnDisconnectDone_TargetExpired_002
 * @tc.desc: Verify DataShareConnectionCallback::OnAbilityDisconnectDone does not crash when the target
 *           weak_ptr is already expired.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
 *     1. A DataShareConnectionCallback can be constructed with an empty weak_ptr.
 * @tc.step:
 *     1. Construct a DataShareConnectionCallback with an empty weak_ptr.
 *     2. Invoke callback->OnAbilityDisconnectDone.
 * @tc.expect:
 *     1. No crash; the callback silently drops the event when target_ is expired.
 */
HWTEST_F(DataShareConnectionTest, DataShareConnectionCallback_OnDisconnectDone_TargetExpired_002, TestSize.Level0)
{
    LOG_INFO("DataShareConnectionCallback_OnDisconnectDone_TargetExpired_002::Start");
    sptr<DataShare::DataShareConnection::ConnectionCallback> callback =
        new (std::nothrow) DataShare::DataShareConnection::ConnectionCallback(
            std::weak_ptr<DataShare::DataShareConnection>());
    ASSERT_NE(callback, nullptr);

    std::string deviceId = "deviceId";
    std::string bundleName = "bundleName";
    std::string abilityName = "abilityName";
    AppExecFwk::ElementName element(deviceId, bundleName, abilityName);
    callback->OnAbilityDisconnectDone(element, 0);
    LOG_INFO("DataShareConnectionCallback_OnDisconnectDone_TargetExpired_002::End");
}

/**
 * @tc.name: DataShareConnection_Init_AllocatesCallback_001
 * @tc.desc: Verify Init() allocates the DataShareConnectionCallback and stores it in the connection's
 *           callback_ member.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
 *     1. A DataShareConnection can be instantiated via std::make_shared.
 * @tc.step:
 *     1. Create a DataShareConnection with the test URI and a valid token.
 *     2. Verify connection->callback_ is nullptr before Init() is called.
 *     3. Invoke connection->Init().
 *     4. Verify connection->callback_ is non-null after Init().
 * @tc.expect:
 *     1. callback_ is nullptr before Init() is called.
 *     2. After Init(), callback_ is non-null after Init().
 */
HWTEST_F(DataShareConnectionTest, DataShareConnection_Init_AllocatesCallback_001, TestSize.Level0)
{
    LOG_INFO("DataShareConnection_Init_AllocatesCallback_001::Start");
    Uri uri(g_dataShareUri);
    std::u16string tokenString = u"OHOS.DataShare.IDataShare";
    sptr<IRemoteObject> token = new (std::nothrow) RemoteObjectTest(tokenString);
    ASSERT_NE(token, nullptr);
    auto connection = std::make_shared<DataShare::DataShareConnection>(uri, token);
    ASSERT_NE(connection, nullptr);

    EXPECT_EQ(connection->callback_, nullptr);
    EXPECT_TRUE(connection->Init());
    EXPECT_NE(connection->callback_, nullptr);
    connection.reset();
    LOG_INFO("DataShareConnection_Init_AllocatesCallback_001::End");
}

/**
 * @tc.name: DataShareConnection_GetCallback_CachedOnSecondCall_002
 * @tc.desc: Verify connection->callback_ returns the same underlying callback on subsequent reads.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
 *     1. A DataShareConnection can be instantiated via std::make_shared.
 * @tc.step:
 *     1. Create a DataShareConnection with the test URI and a valid token.
 *     2. Invoke connection->Init() to allocate the callback.
 *     3. Invoke connection->callback_ twice.
 * @tc.expect:
 *     1. Both calls return sptrs that compare equal (same underlying object).
 */
HWTEST_F(DataShareConnectionTest, DataShareConnection_GetCallback_CachedOnSecondCall_002, TestSize.Level0)
{
    LOG_INFO("DataShareConnection_GetCallback_CachedOnSecondCall_002::Start");
    Uri uri(g_dataShareUri);
    std::u16string tokenString = u"OHOS.DataShare.IDataShare";
    sptr<IRemoteObject> token = new (std::nothrow) RemoteObjectTest(tokenString);
    ASSERT_NE(token, nullptr);
    auto connection = std::make_shared<DataShare::DataShareConnection>(uri, token);
    ASSERT_NE(connection, nullptr);
    ASSERT_TRUE(connection->Init());

    sptr<DataShare::DataShareConnection::ConnectionCallback> first = connection->callback_;
    ASSERT_NE(first, nullptr);
    sptr<DataShare::DataShareConnection::ConnectionCallback> second = connection->callback_;
    ASSERT_NE(second, nullptr);
    EXPECT_EQ(first, second);
    connection.reset();
    LOG_INFO("DataShareConnection_GetCallback_CachedOnSecondCall_002::End");
}

/**
 * @tc.name: DataShareConnection_DelayConnectExtAbility_RescheduleRemovesOldTask_Test_002
 * @tc.desc: Verify DelayConnectExtAbility removes the old task before scheduling a new one on second call.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: None
 * @tc.step:
     1. Create a DataShareConnection, call Init(), get the shared executor from DataShareTaskExecutor.
     2. Set reConnects_.count to 1, prevTime to now (delay=10s, task stays pending).
     3. Call DelayConnectExtAbility (first schedule, reconnectTaskId_==INVALID, no Remove).
     4. Call DelayConnectExtAbility again (reconnectTaskId_!=INVALID, Remove old then Schedule new).
 * @tc.expect:
     1. After first call, reconnectTaskId_ is valid and task is in delayTasks_.
     2. After second call, old task id no longer in delayTasks_, new task id is in delayTasks_.
 */
HWTEST_F(DataShareConnectionTest, DataShareConnection_DelayConnectExtAbility_RescheduleRemovesOldTask_Test_002,
    TestSize.Level0)
{
    LOG_INFO("DataShareConnection_DelayConnectExtAbility_RescheduleRemovesOldTask_Test_002::Start");
    Uri uri(g_dataShareUri);
    std::u16string tokenString = u"OHOS.DataShare.IDataShare";
    sptr<IRemoteObject> token = new (std::nothrow) RemoteObjectTest(tokenString);
    ASSERT_NE(token, nullptr);
    auto connection = std::make_shared<DataShare::DataShareConnection>(uri, token);
    ASSERT_NE(connection, nullptr);
    ASSERT_TRUE(connection->Init());

    auto executor = DataShareTaskExecutor::GetInstance().GetExecutor();
    ASSERT_NE(executor, nullptr);
    auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    connection->reConnects_.count.store(1);
    connection->reConnects_.prevTime.store(now);

    connection->DelayConnectExtAbility(g_dataShareUri);
    auto firstTaskId = connection->reconnectTaskId_;
    EXPECT_NE(firstTaskId, ExecutorPool::INVALID_TASK_ID);
    EXPECT_TRUE(executor->delayTasks_.Find(firstTaskId).Valid());

    connection->DelayConnectExtAbility(g_dataShareUri);
    auto secondTaskId = connection->reconnectTaskId_;
    EXPECT_NE(secondTaskId, ExecutorPool::INVALID_TASK_ID);
    EXPECT_FALSE(executor->delayTasks_.Find(firstTaskId).Valid());
    EXPECT_TRUE(executor->delayTasks_.Find(secondTaskId).Valid());

    connection.reset();
    LOG_INFO("DataShareConnection_DelayConnectExtAbility_RescheduleRemovesOldTask_Test_002::End");
}

/**
 * @tc.name: DataShareConnection_DelayConnectExtAbility_InvalidConnSkipReconnect_Test_003
 * @tc.desc: Verify the scheduled lambda returns early when isInvalid_ is true.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: None
 * @tc.step:
     1. Create a DataShareConnection, call Init(), get the shared executor from DataShareTaskExecutor.
     2. Set reConnects_.count to 1, prevTime to now-20s (delay=0, immediate execution).
     3. Set isInvalid_ to true.
     4. Call DelayConnectExtAbility and wait for task execution.
 * @tc.expect:
     1. No crash; lambda checks isInvalid_ and returns early without calling DoReconnectTask.
     2. Task leaves delayTasks_ after execution.
 */
HWTEST_F(DataShareConnectionTest, DataShareConnection_DelayConnectExtAbility_InvalidConnSkipReconnect_Test_003,
    TestSize.Level0)
{
    LOG_INFO("DataShareConnection_DelayConnectExtAbility_InvalidConnSkipReconnect_Test_003::Start");
    Uri uri(g_dataShareUri);
    std::u16string tokenString = u"OHOS.DataShare.IDataShare";
    sptr<IRemoteObject> token = new (std::nothrow) RemoteObjectTest(tokenString);
    ASSERT_NE(token, nullptr);
    auto connection = std::make_shared<DataShare::DataShareConnection>(uri, token);
    ASSERT_NE(connection, nullptr);
    ASSERT_TRUE(connection->Init());

    auto executor = DataShareTaskExecutor::GetInstance().GetExecutor();
    ASSERT_NE(executor, nullptr);
    auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    connection->reConnects_.count.store(1);
    connection->reConnects_.prevTime.store(now - 20000);
    connection->isInvalid_.store(true);

    connection->DelayConnectExtAbility(g_dataShareUri);
    EXPECT_NE(connection->reconnectTaskId_, ExecutorPool::INVALID_TASK_ID);
    sleep(1);
    EXPECT_FALSE(executor->delayTasks_.Find(connection->reconnectTaskId_).Valid());
    connection.reset();
    LOG_INFO("DataShareConnection_DelayConnectExtAbility_InvalidConnSkipReconnect_Test_003::End");
}

/**
 * @tc.name: DataShareConnection_ReconnectExtAbility_SchedulesDelayTask_Test_004
 * @tc.desc: Verify ReconnectExtAbility routes to DelayConnectExtAbility when count!=0 and records reconnectTaskId_.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: None
 * @tc.step:
     1. Create a DataShareConnection, call Init(), get the shared executor from DataShareTaskExecutor.
     2. Set reConnects_.count to 1, prevTime to now (delay=10s, task stays pending).
     3. Call ReconnectExtAbility (count!=0 routes to DelayConnectExtAbility).
 * @tc.expect:
     1. reconnectTaskId_ is valid and task is pending in delayTasks_.
 */
HWTEST_F(DataShareConnectionTest, DataShareConnection_ReconnectExtAbility_SchedulesDelayTask_Test_004,
    TestSize.Level0)
{
    LOG_INFO("DataShareConnection_ReconnectExtAbility_SchedulesDelayTask_Test_004::Start");
    Uri uri(g_dataShareUri);
    std::u16string tokenString = u"OHOS.DataShare.IDataShare";
    sptr<IRemoteObject> token = new (std::nothrow) RemoteObjectTest(tokenString);
    ASSERT_NE(token, nullptr);
    auto connection = std::make_shared<DataShare::DataShareConnection>(uri, token);
    ASSERT_NE(connection, nullptr);
    ASSERT_TRUE(connection->Init());

    auto executor = DataShareTaskExecutor::GetInstance().GetExecutor();
    ASSERT_NE(executor, nullptr);
    auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    connection->reConnects_.count.store(1);
    connection->reConnects_.prevTime.store(now);

    connection->ReconnectExtAbility(g_dataShareUri);
    EXPECT_NE(connection->reconnectTaskId_, ExecutorPool::INVALID_TASK_ID);
    EXPECT_TRUE(executor->delayTasks_.Find(connection->reconnectTaskId_).Valid());

    connection.reset();
    LOG_INFO("DataShareConnection_ReconnectExtAbility_SchedulesDelayTask_Test_004::End");
}

/**
 * @tc.name: DataShareConnection_Destructor_RemovesPendingReconnectTask_Test_005
 * @tc.desc: Verify the destructor removes the pending reconnect task when reconnectTaskId_!=INVALID.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: None
 * @tc.step:
     1. Create a DataShareConnection, call Init(), get the shared executor from DataShareTaskExecutor.
     2. Schedule a pending reconnect task (delay=10s, reconnectTaskId_ becomes valid).
     3. Keep an external shared_ptr to the shared executor for post-destruction observation.
     4. connection.reset() triggers destructor.
 * @tc.expect:
     1. No crash, no freeze.
     2. After destruction, the task id is no longer in delayTasks_ (removed by destructor).
 */
HWTEST_F(DataShareConnectionTest, DataShareConnection_Destructor_RemovesPendingReconnectTask_Test_005,
    TestSize.Level0)
{
    LOG_INFO("DataShareConnection_Destructor_RemovesPendingReconnectTask_Test_005::Start");
    Uri uri(g_dataShareUri);
    std::u16string tokenString = u"OHOS.DataShare.IDataShare";
    sptr<IRemoteObject> token = new (std::nothrow) RemoteObjectTest(tokenString);
    ASSERT_NE(token, nullptr);
    auto connection = std::make_shared<DataShare::DataShareConnection>(uri, token);
    ASSERT_NE(connection, nullptr);
    ASSERT_TRUE(connection->Init());

    auto pool = DataShareTaskExecutor::GetInstance().GetExecutor();
    ASSERT_NE(pool, nullptr);
    auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    connection->reConnects_.count.store(1);
    connection->reConnects_.prevTime.store(now);

    connection->DelayConnectExtAbility(g_dataShareUri);
    auto taskId = connection->reconnectTaskId_;
    ASSERT_NE(taskId, ExecutorPool::INVALID_TASK_ID);
    ASSERT_TRUE(pool->delayTasks_.Find(taskId).Valid());

    connection.reset();
    EXPECT_FALSE(pool->delayTasks_.Find(taskId).Valid());
    LOG_INFO("DataShareConnection_Destructor_RemovesPendingReconnectTask_Test_005::End");
}

/**
 * @tc.name: DataShareConnection_Destructor_PoolNoPendingTask_Test_006
 * @tc.desc: Verify the destructor handles the case with no pending reconnect task
 *           (reconnectTaskId_==INVALID, nothing to remove).
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: None
 * @tc.step:
     1. Create a DataShareConnection, call Init(), get the shared executor from DataShareTaskExecutor.
     2. Do not schedule any task (reconnectTaskId_ stays INVALID).
     3. Keep an external shared_ptr to the shared executor for post-destruction observation.
     4. connection.reset() triggers destructor.
 * @tc.expect:
     1. No crash; the shared executor stays available after destruction.
 */
HWTEST_F(DataShareConnectionTest, DataShareConnection_Destructor_PoolNoPendingTask_Test_006, TestSize.Level0)
{
    LOG_INFO("DataShareConnection_Destructor_PoolNoPendingTask_Test_006::Start");
    Uri uri(g_dataShareUri);
    std::u16string tokenString = u"OHOS.DataShare.IDataShare";
    sptr<IRemoteObject> token = new (std::nothrow) RemoteObjectTest(tokenString);
    ASSERT_NE(token, nullptr);
    auto connection = std::make_shared<DataShare::DataShareConnection>(uri, token);
    ASSERT_NE(connection, nullptr);
    ASSERT_TRUE(connection->Init());

    EXPECT_EQ(connection->reconnectTaskId_, ExecutorPool::INVALID_TASK_ID);
    auto pool = DataShareTaskExecutor::GetInstance().GetExecutor();
    ASSERT_NE(pool, nullptr);

    connection.reset();
    EXPECT_NE(DataShareTaskExecutor::GetInstance().GetExecutor(), nullptr);
    LOG_INFO("DataShareConnection_Destructor_PoolNoPendingTask_Test_006::End");
}

/**
 * @tc.name: DataShareConnection_DelayConnectExtAbility_TaskExecutesConnect_Test_007
 * @tc.desc: Verify the lambda takes the normal execution path (isInvalid_==false) and calls DoReconnectTask.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: None
 * @tc.step:
     1. Create a DataShareConnection, call Init(), get the shared executor from DataShareTaskExecutor.
     2. Set reConnects_.count to 1, prevTime to now-20s (delay=0, immediate execution).
     3. Keep isInvalid_ = false.
     4. Call DelayConnectExtAbility and wait for task execution.
 * @tc.expect:
     1. No crash; lambda executes DoReconnectTask (which returns early since AmsMgrProxy is null in test env).
     2. Task leaves delayTasks_ after execution.
 */
HWTEST_F(DataShareConnectionTest, DataShareConnection_DelayConnectExtAbility_TaskExecutesConnect_Test_007,
    TestSize.Level0)
{
    LOG_INFO("DataShareConnection_DelayConnectExtAbility_TaskExecutesConnect_Test_007::Start");
    Uri uri(g_dataShareUri);
    std::u16string tokenString = u"OHOS.DataShare.IDataShare";
    sptr<IRemoteObject> token = new (std::nothrow) RemoteObjectTest(tokenString);
    ASSERT_NE(token, nullptr);
    auto connection = std::make_shared<DataShare::DataShareConnection>(uri, token);
    ASSERT_NE(connection, nullptr);
    ASSERT_TRUE(connection->Init());

    auto executor = DataShareTaskExecutor::GetInstance().GetExecutor();
    ASSERT_NE(executor, nullptr);
    auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    connection->reConnects_.count.store(1);
    connection->reConnects_.prevTime.store(now - 20000);
    EXPECT_FALSE(connection->isInvalid_.load());

    connection->DelayConnectExtAbility(g_dataShareUri);
    EXPECT_NE(connection->reconnectTaskId_, ExecutorPool::INVALID_TASK_ID);
    sleep(1);
    EXPECT_FALSE(executor->delayTasks_.Find(connection->reconnectTaskId_).Valid());
    connection.reset();
    LOG_INFO("DataShareConnection_DelayConnectExtAbility_TaskExecutesConnect_Test_007::End");
}

/**
 * @tc.name: DataShareConnection_DelayConnectExtAbility_ExpiredConnSkipReconnect_Test_008
 * @tc.desc: Verify the lambda returns early when selfSharedPtr is nullptr (weak_ptr expired).
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: None
 * @tc.step:
     1. Create a DataShareConnection, call Init(), get the shared executor from DataShareTaskExecutor.
     2. Set reConnects_.count to 1, prevTime to now (delay=10s, task stays pending).
     3. Schedule a pending task via DelayConnectExtAbility, record taskId.
     4. Manually clear reconnectTaskId_ so destructor skips Remove.
     5. Keep external shared_ptr to the shared executor for post-destruction observation.
     6. connection.reset() destroys the connection (weak_ptr in lambda becomes expired).
     7. pool->Reset(taskId, 500ms) shortens the delay to trigger execution.
     8. Wait for task execution.
 * @tc.expect:
     1. Reset returns the original taskId.
     2. No crash; lambda checks selfSharedPtr==nullptr and returns early.
     3. Task leaves delayTasks_ after execution.
 */
HWTEST_F(DataShareConnectionTest, DataShareConnection_DelayConnectExtAbility_ExpiredConnSkipReconnect_Test_008,
    TestSize.Level0)
{
    LOG_INFO("DataShareConnection_DelayConnectExtAbility_ExpiredConnSkipReconnect_Test_008::Start");
    Uri uri(g_dataShareUri);
    std::u16string tokenString = u"OHOS.DataShare.IDataShare";
    sptr<IRemoteObject> token = new (std::nothrow) RemoteObjectTest(tokenString);
    ASSERT_NE(token, nullptr);
    auto connection = std::make_shared<DataShare::DataShareConnection>(uri, token);
    ASSERT_NE(connection, nullptr);
    ASSERT_TRUE(connection->Init());

    auto pool = DataShareTaskExecutor::GetInstance().GetExecutor();
    ASSERT_NE(pool, nullptr);
    auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    connection->reConnects_.count.store(1);
    connection->reConnects_.prevTime.store(now);

    connection->DelayConnectExtAbility(g_dataShareUri);
    auto taskId = connection->reconnectTaskId_;
    ASSERT_NE(taskId, ExecutorPool::INVALID_TASK_ID);
    ASSERT_TRUE(pool->delayTasks_.Find(taskId).Valid());

    connection->reconnectTaskId_ = ExecutorPool::INVALID_TASK_ID;
    connection.reset();

    EXPECT_EQ(pool->Reset(taskId, std::chrono::milliseconds(500)), taskId);
    sleep(1);
    EXPECT_FALSE(pool->delayTasks_.Find(taskId).Valid());
    LOG_INFO("DataShareConnection_DelayConnectExtAbility_ExpiredConnSkipReconnect_Test_008::End");
}

/**
 * @tc.name: DataShareConnection_DelayConnectExtAbility_ScheduleFailed_Test_009
 * @tc.desc: Verify DelayConnectExtAbility handles Schedule failure when pool is at max capacity.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: None
 * @tc.step:
     1. Create a DataShareConnection, call Init().
     2. Inject a dedicated executor through DataShareTaskExecutor::SetExecutor so the capacity
        exhaustion does not affect the shared executor used by other tests.
     3. Exhaust the injected pool's task capacity by scheduling many long-delay tasks.
     4. Set reConnects_.count to 1, prevTime to now.
     5. Call DelayConnectExtAbility, then restore the shared executor.
 * @tc.expect:
     1. reconnectTaskId_ is INVALID_TASK_ID (Schedule failed, line 169 true branch).
 */
HWTEST_F(DataShareConnectionTest, DataShareConnection_DelayConnectExtAbility_ScheduleFailed_Test_009,
    TestSize.Level0)
{
    LOG_INFO("DataShareConnection_DelayConnectExtAbility_ScheduleFailed_Test_009::Start");
    Uri uri(g_dataShareUri);
    std::u16string tokenString = u"OHOS.DataShare.IDataShare";
    sptr<IRemoteObject> token = new (std::nothrow) RemoteObjectTest(tokenString);
    ASSERT_NE(token, nullptr);
    auto connection = std::make_shared<DataShare::DataShareConnection>(uri, token);
    ASSERT_NE(connection, nullptr);
    ASSERT_TRUE(connection->Init());

    auto originalExecutor = DataShareTaskExecutor::GetInstance().GetExecutor();
    auto testPool = std::make_shared<ExecutorPool>(MAX_THREADS, MIN_THREADS, "DsConnFloodTest");
    ASSERT_NE(testPool, nullptr);
    DataShareTaskExecutor::GetInstance().SetExecutor(testPool);

    auto longDelay = std::chrono::hours(1);
    bool poolFull = false;
    for (int i = 0; i < 10000; i++) {
        auto id = testPool->Schedule(longDelay, []() {});
        if (id == ExecutorPool::INVALID_TASK_ID) {
            poolFull = true;
            break;
        }
    }
    if (!poolFull) {
        LOG_INFO("Pool capacity not reached after 10000 tasks, skip Schedule failure test");
        DataShareTaskExecutor::GetInstance().SetExecutor(originalExecutor);
        connection.reset();
        return;
    }

    auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    connection->reConnects_.count.store(1);
    connection->reConnects_.prevTime.store(now);

    connection->DelayConnectExtAbility(g_dataShareUri);
    EXPECT_EQ(connection->reconnectTaskId_, ExecutorPool::INVALID_TASK_ID);
    DataShareTaskExecutor::GetInstance().SetExecutor(originalExecutor);
    connection.reset();
    LOG_INFO("DataShareConnection_DelayConnectExtAbility_ScheduleFailed_Test_009::End");
}

/**
 * @tc.name: DataShareConnection_DoReconnectTask_DirectCall_Test_010
 * @tc.desc: Verify DoReconnectTask synchronously exercises the AmsMgrProxy::GetInstance and Connect path.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: None
 * @tc.step:
     1. Create a DataShareConnection, call Init() (so callback_ is valid).
     2. Call DoReconnectTask directly (synchronous, not via async lambda).
 * @tc.expect:
     1. No crash; GetInstance() returns non-null singleton, Connect() returns error (no samgr in test env).
     2. Covers line 180 false branch (instance != nullptr) and line 187 false branch (ret != E_OK).
 */
HWTEST_F(DataShareConnectionTest, DataShareConnection_DoReconnectTask_DirectCall_Test_010, TestSize.Level0)
{
    LOG_INFO("DataShareConnection_DoReconnectTask_DirectCall_Test_010::Start");
    Uri uri(g_dataShareUri);
    std::u16string tokenString = u"OHOS.DataShare.IDataShare";
    sptr<IRemoteObject> token = new (std::nothrow) RemoteObjectTest(tokenString);
    ASSERT_NE(token, nullptr);
    auto connection = std::make_shared<DataShare::DataShareConnection>(uri, token);
    ASSERT_NE(connection, nullptr);
    ASSERT_TRUE(connection->Init());

    connection->DoReconnectTask(g_dataShareUri);
    connection.reset();
    LOG_INFO("DataShareConnection_DoReconnectTask_DirectCall_Test_010::End");
}

/**
 * @tc.name: DataShareConnection_ConnectTimeout_CallbackDisconnect_001
 * @tc.desc: Verify that when the upper-layer business connects via ConnectDataShareExtAbility and the extension is not
 *           connected within the timeout (2s), the connection returns nullptr and the DataShareConnection is released.
 *           When later calls back OnAbilityConnectDone on the released connection, the callback should proactively
 *           DisConnect to tear down the just-established extension connection, avoiding a leaked extension process.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
 *     1. AmsMgrProxy::Connect is mocked to return E_OK so that ConnectDataShareExtAbility enters the 2s wait.
 *     2. The mock AmsMgrProxy records Connect/DisConnect calls.
 * @tc.step:
 *     1. Create a DataShareConnection with waitTime=2 and initialize it.
 *     2. Call ConnectDataShareExtAbility directly to connect; no callback arrives within 2s, so it returns nullptr
 *        after timeout.
 *     3. Save the ConnectionCallback handle, then reset the connection shared_ptr to destroy it (target expired).
 *     4. Simulate a late AmsMgrProxy OnAbilityConnectDone callback with a non-null remoteObject.
 * @tc.expect:
 *     1. ConnectDataShareExtAbility returns nullptr after the 2s timeout.
 *     2. The ConnectionCallback calls AmsMgrProxy::DisConnect (disconnect count is 1) when the target is expired.
 */
HWTEST_F(DataShareConnectionTest, DataShareConnection_ConnectTimeout_CallbackDisconnect_001, TestSize.Level0)
{
    LOG_INFO("DataShareConnection_ConnectTimeout_CallbackDisconnect_001::Start");
    AmsMgrProxyMock::Reset();
    AmsMgrProxyMock::SetConnectResult(E_OK);

    Uri uri(g_dataShareUri);
    std::u16string tokenString = u"OHOS.DataShare.IDataShare";
    sptr<IRemoteObject> token = new (std::nothrow) RemoteObjectTest(tokenString);
    ASSERT_NE(token, nullptr);
    std::shared_ptr<DataShare::DataShareConnection> connection =
        std::make_shared<DataShare::DataShareConnection>(uri, token);
    ASSERT_NE(connection, nullptr);
    ASSERT_TRUE(connection->Init());

    std::shared_ptr<DataShareProxy> proxy = connection->ConnectDataShareExtAbility(uri, token);
    EXPECT_EQ(proxy, nullptr);
    EXPECT_GE(AmsMgrProxyMock::GetConnectCount(), 1);

    sptr<DataShare::DataShareConnection::ConnectionCallback> callback = connection->callback_;
    ASSERT_NE(callback, nullptr);
    connection.reset();

    std::string deviceId = "deviceId";
    std::string bundleName = "bundleName";
    std::string abilityName = "abilityName";
    AppExecFwk::ElementName element(deviceId, bundleName, abilityName);
    callback->OnAbilityConnectDone(element, token, 0);

    EXPECT_EQ(AmsMgrProxyMock::GetDisConnectCount(), 1);
    LOG_INFO("DataShareConnection_ConnectTimeout_CallbackDisconnect_001::End");
}

/**
 * @tc.name: DataShareConnection_ConnectSuccess_CallbackNoDisconnect_002
 * @tc.desc: Verify that in the normal connection path (target is alive), the callback forwards to
 *           DataShareConnection::OnAbilityConnectDone and does NOT trigger an extra DisConnect.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
 *     1. The mock AmsMgrProxy records Connect/DisConnect calls.
 *     2. A DataShareConnection can be instantiated and initialized so its callback target is alive.
 * @tc.step:
 *     1. Create and initialize a DataShareConnection.
 *     2. Invoke callback->OnAbilityConnectDone with a non-null remoteObject while the target is still alive.
 * @tc.expect:
 *     1. The connection's dataShareProxy_ is populated after the callback.
 *     2. DisConnect is NOT called in the normal (target-alive) path.
 */
HWTEST_F(DataShareConnectionTest, DataShareConnection_ConnectSuccess_CallbackNoDisconnect_002, TestSize.Level0)
{
    LOG_INFO("DataShareConnection_ConnectSuccess_CallbackNoDisconnect_002::Start");
    AmsMgrProxyMock::Reset();

    Uri uri(g_dataShareUri);
    std::u16string tokenString = u"OHOS.DataShare.IDataShare";
    sptr<IRemoteObject> token = new (std::nothrow) RemoteObjectTest(tokenString);
    ASSERT_NE(token, nullptr);
    std::shared_ptr<DataShare::DataShareConnection> connection =
        std::make_shared<DataShare::DataShareConnection>(uri, token);
    ASSERT_NE(connection, nullptr);
    ASSERT_TRUE(connection->Init());

    std::string deviceId = "deviceId";
    std::string bundleName = "bundleName";
    std::string abilityName = "abilityName";
    AppExecFwk::ElementName element(deviceId, bundleName, abilityName);
    connection->OnAbilityConnectDone(element, token, 0);

    EXPECT_NE(connection->dataShareProxy_, nullptr);
    EXPECT_EQ(AmsMgrProxyMock::GetDisConnectCount(), 0);
    LOG_INFO("DataShareConnection_ConnectSuccess_CallbackNoDisconnect_002::End");
}

/**
 * @tc.name: DataShareConnection_ConnectGetInstanceNull_003
 * @tc.desc: Verify that when AmsMgrProxy::GetInstance() returns nullptr, ConnectDataShareExtAbility returns nullptr
 *           early without hanging, and no Connect is issued.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
 *     1. The mock AmsMgrProxy can be forced to return nullptr from GetInstance().
 * @tc.step:
 *     1. Set the mock GetInstance() to return nullptr.
 *     2. Call ConnectDataShareExtAbility directly.
 * @tc.expect:
 *     1. ConnectDataShareExtAbility returns nullptr immediately.
 *     2. No AmsMgrProxy::Connect call is made (connect count is 0).
 */
HWTEST_F(DataShareConnectionTest, DataShareConnection_ConnectGetInstanceNull_003, TestSize.Level0)
{
    LOG_INFO("DataShareConnection_ConnectGetInstanceNull_003::Start");
    AmsMgrProxyMock::Reset();
    AmsMgrProxyMock::SetGetInstanceNull(true);

    Uri uri(g_dataShareUri);
    std::u16string tokenString = u"OHOS.DataShare.IDataShare";
    sptr<IRemoteObject> token = new (std::nothrow) RemoteObjectTest(tokenString);
    ASSERT_NE(token, nullptr);
    std::shared_ptr<DataShare::DataShareConnection> connection =
        std::make_shared<DataShare::DataShareConnection>(uri, token);
    ASSERT_NE(connection, nullptr);
    ASSERT_TRUE(connection->Init());

    std::shared_ptr<DataShareProxy> proxy = connection->ConnectDataShareExtAbility(uri, token);
    EXPECT_EQ(proxy, nullptr);
    EXPECT_EQ(AmsMgrProxyMock::GetConnectCount(), 0);
    LOG_INFO("DataShareConnection_ConnectGetInstanceNull_003::End");
}

/**
 * @tc.name: DataShareConnection_DelayConnectExtAbility_Schedule_002
 * @tc.desc: Verify that the delayed reconnect task scheduled by DelayConnectExtAbility runs on the
 *           process-wide executor pool shared through DataShareTaskExecutor when the reconnect count
 *           is non-zero and the previous reconnect happened before the reconnect time interval.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. AmsMgrProxyMock is linked and records Connect calls, with the connect result set to E_OK.
    2. The reConnects_ members of DataShareConnection can be set to steer ReconnectExtAbility into
       DelayConnectExtAbility.
 * @tc.step:
    1. Reset AmsMgrProxyMock and set the connect result to E_OK.
    2. Create a DataShareConnection and call Init to allocate the connection callback.
    3. Set reConnects_.count to 1 so that ReconnectExtAbility goes to DelayConnectExtAbility.
    4. Set reConnects_.prevTime to 60 seconds ago, which is older than RECONNECT_TIME_INTERVAL so the
       task is scheduled without delay, and newer than MAX_RECONNECT_TIME_INTERVAL so the count is kept.
    5. Call OnAbilityDisconnectDone and poll for the scheduled task to run on the shared executor.
 * @tc.expect:
    1. The scheduled task executes AmsMgrProxy Connect on the shared executor pool.
    2. The reconnect count increases to 2 after the task runs successfully.
 */
HWTEST_F(DataShareConnectionTest, DataShareConnection_DelayConnectExtAbility_Schedule_002, TestSize.Level0)
{
    LOG_INFO("DataShareConnection_DelayConnectExtAbility_Schedule_002::Start");
    AmsMgrProxyMock::Reset();
    AmsMgrProxyMock::SetConnectResult(E_OK);
    Uri uri(g_dataShareUri);
    std::u16string tokenString = u"OHOS.DataShare.IDataShare";
    sptr<IRemoteObject> token = new (std::nothrow) RemoteObjectTest(tokenString);
    ASSERT_NE(token, nullptr);
    std::shared_ptr<DataShare::DataShareConnection> connection =
        std::make_shared<DataShare::DataShareConnection>(uri, token);
    ASSERT_NE(connection, nullptr);
    ASSERT_TRUE(connection->Init());
    connection->reConnects_.count.store(1);
    int64_t pastTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count() - 60000;
    connection->reConnects_.prevTime.store(pastTime);
    std::string deviceId = "deviceId";
    std::string bundleName = "bundleName";
    std::string abilityName = "abilityName";
    AppExecFwk::ElementName element(deviceId, bundleName, abilityName);
    connection->OnAbilityDisconnectDone(element, 0);
    bool scheduled = false;
    for (int i = 0; i < 40 && !scheduled; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        scheduled = AmsMgrProxyMock::GetConnectCount() >= 1;
    }
    EXPECT_TRUE(scheduled);
    EXPECT_EQ(connection->reConnects_.count.load(), 2);
    LOG_INFO("DataShareConnection_DelayConnectExtAbility_Schedule_002::End");
}

/**
 * @tc.name: DataShareConnection_DelayConnectExtAbility_MaxReconnect_003
 * @tc.desc: Verify that DelayConnectExtAbility stops scheduling reconnect tasks once the reconnect
 *           count reaches MAX_RECONNECT.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. AmsMgrProxyMock is linked and records Connect calls.
    2. The reConnects_ members of DataShareConnection can be set to reach the reconnect upper limit.
 * @tc.step:
    1. Reset AmsMgrProxyMock.
    2. Create a DataShareConnection and call Init to allocate the connection callback.
    3. Set reConnects_.count to MAX_RECONNECT and reConnects_.prevTime to 60 seconds ago so the
       interval reset does not clear the count.
    4. Call OnAbilityDisconnectDone, wait for a while, and check the mock connect count.
 * @tc.expect:
    1. No reconnect task is scheduled and the reconnect count stays at MAX_RECONNECT.
 */
HWTEST_F(DataShareConnectionTest, DataShareConnection_DelayConnectExtAbility_MaxReconnect_003, TestSize.Level0)
{
    LOG_INFO("DataShareConnection_DelayConnectExtAbility_MaxReconnect_003::Start");
    AmsMgrProxyMock::Reset();
    Uri uri(g_dataShareUri);
    std::u16string tokenString = u"OHOS.DataShare.IDataShare";
    sptr<IRemoteObject> token = new (std::nothrow) RemoteObjectTest(tokenString);
    ASSERT_NE(token, nullptr);
    std::shared_ptr<DataShare::DataShareConnection> connection =
        std::make_shared<DataShare::DataShareConnection>(uri, token);
    ASSERT_NE(connection, nullptr);
    ASSERT_TRUE(connection->Init());
    connection->reConnects_.count.store(DataShareConnection::MAX_RECONNECT);
    int64_t pastTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()).count() - 60000;
    connection->reConnects_.prevTime.store(pastTime);
    std::string deviceId = "deviceId";
    std::string bundleName = "bundleName";
    std::string abilityName = "abilityName";
    AppExecFwk::ElementName element(deviceId, bundleName, abilityName);
    connection->OnAbilityDisconnectDone(element, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    EXPECT_EQ(AmsMgrProxyMock::GetConnectCount(), 0);
    EXPECT_EQ(connection->reConnects_.count.load(), DataShareConnection::MAX_RECONNECT);
    LOG_INFO("DataShareConnection_DelayConnectExtAbility_MaxReconnect_003::End");
}
}
}