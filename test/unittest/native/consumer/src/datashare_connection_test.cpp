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
 * @tc.desc: Verify the normal functionality of the UpdateObserverExtsProviderMap method in DataShareConnection,
 *           focusing on whether observers can be correctly added to the observer map and the map size is
 *           updated accordingly.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment is properly set up, supporting the instantiation and operation of DataShareConnection
       and related classes.
    2. Valid test URIs (DATA_SHARE_URI and DATA_SHARE_URI1) are predefined and accessible.
    3. The IRemoteObject and IDataAbilityObserverTest classes can be normally instantiated without initialization
       errors.
 * @tc.step:
    1. Create a DataShareConnection object using the test URI (DATA_SHARE_URI) and a valid IRemoteObject token.
    2. Verify that the observerExtsProvider_ map in the created DataShareConnection is initially empty.
    3. Create the first IDataAbilityObserverTest instance with DATA_SHARE_URI, and add it to the map using
       UpdateObserverExtsProviderMap with the "true" flag.
    4. Create the second IDataAbilityObserverTest instance with DATA_SHARE_URI1, and add it to the map using
       UpdateObserverExtsProviderMap with the "true" flag.
    5. Check the size of the observerExtsProvider_ map after adding the two observers.
 * @tc.expect:
    1. The observerExtsProvider_ map is empty before adding any observers.
    2. After adding the two observers, the size of the observerExtsProvider_ map is 2.
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
 * @tc.desc: Verify the functionality of the DeleteObserverExtsProviderMap method in DataShareConnection, including
 *           correctly removing valid observers and leaving the map unchanged when removing invalid observers.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports the creation of DataShareConnection, IRemoteObject, and IDataAbilityObserverTest
       instances.
    2. Two distinct test URIs (DATA_SHARE_URI and DATA_SHARE_URI1) are available for observer initialization.
    3. The UpdateObserverExtsProviderMap method can successfully add observers to the observerExtsProvider_ map.
 * @tc.step:
    1. Create a DataShareConnection object with the test URI (DATA_SHARE_URI) and a valid IRemoteObject token.
    2. Add two observers to the observerExtsProvider_ map using UpdateObserverExtsProviderMap: one with DATA_SHARE_URI
       and another with DATA_SHARE_URI1.
    3. Verify that the initial size of the observerExtsProvider_ map is 2 after adding the observers.
    4. Call DeleteObserverExtsProviderMap to remove the observer associated with DATA_SHARE_URI1, then check the map
       size.
    5. Call DeleteObserverExtsProviderMap again to attempt removing an invalid observer (mismatched URI and observer),
       then check the map size.
 * @tc.expect:
    1. After deleting the valid observer (DATA_SHARE_URI1), the size of the observerExtsProvider_ map is 1.
    2. After attempting to delete the invalid observer, the size of the observerExtsProvider_ map remains 1.
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
    1. Create a DataShareConnection object with the test URI (DATA_SHARE_URI) and a valid IRemoteObject token.
    2. Create a DataShareProxy instance using the token and assign it to the dataShareProxy_ member of the
       DataShareConnection.
    3. Add two observers to the observerExtsProvider_ map using UpdateObserverExtsProviderMap (with DATA_SHARE_URI and
       DATA_SHARE_URI1 respectively).
    4. Verify that the observerExtsProvider_ map is not empty and has a size of 2 after adding the observers.
    5. Call the ReRegisterObserverExtProvider method of the DataShareConnection.
    6. Check whether the observerExtsProvider_ map remains non-empty after re-registration.
 * @tc.expect:
    1. The observerExtsProvider_ map remains non-empty after calling ReRegisterObserverExtProvider.
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
    1. Create a DataShareConnection object with the test URI (DATA_SHARE_URI) and a valid IRemoteObject token.
    2. Create a DataShareProxy instance using the token and assign it to the dataShareConnection's dataShareProxy_
       member.
    3. Add two observers to the observerExtsProvider_ map (with DATA_SHARE_URI and DATA_SHARE_URI1 respectively) using
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
 * @tc.desc: Verify that the thread name in the connection pool of DataShareConnection is correctly set to the expected
 *           value after the OnAbilityDisconnectDone method is called during reconnection.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment allows instantiation of DataShareConnection, IRemoteObject, and ElementName objects.
    2. The isReconnect_ member of DataShareConnection can be set to true, and the connection pool is properly
       initialized.
    3. The expected thread name is predefined and accessible.
 * @tc.step:
    1. Create a DataShareConnection object with the test URI (DATA_SHARE_URI) and a valid IRemoteObject token.
    2. Set the isReconnect_ flag of the DataShareConnection to true using isReconnect_.store(true).
    3. Create an AppExecFwk::ElementName instance with test parameters and a resultCode of 0.
    4. Call the OnAbilityDisconnectDone method with the ElementName and resultCode as parameters.
    5. Check the threadName_ member of the connection pool's pool_ in the DataShareConnection.
 * @tc.expect:
    1. The threadName_ of the connection pool's pool_ is set to DATASHARE_EXECUTOR_NAME after
       OnAbilityDisconnectDone is called.
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
    EXPECT_EQ(connection->pool_->pool_.threadName_, DATASHARE_EXECUTOR_NAME);
    LOG_INFO("DataShareConnection_OnAbilityDisconnectDone_Test_001::End");
}
}
}