/*
 * Copyright (c) 2026 Huawei Huawei Device Co., Ltd.
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
#define LOG_TAG "datashare_sa_connect_test"
#include <gtest/gtest.h>

#include "datashare_sa_connection.h"
#include "datashare_sa_provider_info.h"
#include "datashare_proxy.h"
#include "uri.h"

namespace OHOS {
namespace DataShare {
using namespace testing::ext;
using namespace OHOS::DataShare;
constexpr uint32_t WAIT_TIME = 10;
constexpr int32_t SA_ID = 1001;
std::string DATA_SHARE_URI = "datashare://distributedata/SAID=1301";

class DataShareSAConnectionTest : public testing::Test {
public:
    static void SetUpTestCase(void) {};
    static void TearfulTestCase(void) {};
    void SetUp() {};
    void TearDown() {};
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

/**
 * @tc.name: DataShareSAConnectionConstructor
 * @tc.desc: Verify the constructor of DataShareSAConnection class correctly initializes member variables
 *           with valid URI, SA ID, and wait time parameters.
 * @tc.type: FUNC
 * @tc.require: issueIC8OCN
 * @tc.precon:
 *     1. The test environment supports instantiation of Uri and DataShareSAConnection objects.
 *     2. Predefined constants are valid: DATA_SHARE_URI (test target URI), SA_ID (system ability ID = 1001),
 *        and WAIT_TIME (wait time = 10).
 *     3. The DataShareSAConnection class has public members uri_, saId_, and waitTime_ that can be accessed.
 * @tc.step:
 *     1. Create a Uri object using the DATA_SHARE_URI constant.
 *     2. Instantiate a DataShareSAConnection object by passing the created Uri, SA_ID, and WAIT_TIME.
 *     3. Verify that the uri_ member of the connection object matches DATA_SHARE_URI by comparing its ToString().
 *     4. Verify that the saId_ member of the connection object equals SA_ID.
 *     5. Verify that the waitTime_ member of the connection object equals WAIT_TIME.
 * @tc.expect:
 *     1. The connection.uri_.ToString() returns DATA_SHARE_URI.
 *     2. The connection.saId_ equals SA_ID (1001).
 *     3. The connection.waitTime_ equals WAIT_TIME (10).
 */
HWTEST_F(DataShareSAConnectionTest, DataShareSAConnectionConstructor, TestSize.Level0)
{
    Uri uri(DATA_SHARE_URI);
    DataShareSAConnection connection(uri, SA_ID, WAIT_TIME);
    EXPECT_EQ(connection.uri_.ToString(), DATA_SHARE_URI);
    EXPECT_EQ(connection.saId_, SA_ID);
    EXPECT_EQ(connection.waitTime_, WAIT_TIME);
}

/**
 * @tc.name: DataShareSAConnectionConstructorNegativeWaitTime
 * @tc.desc: Verify that constructor of DataShareSAConnection class correctly handles negative wait time
 *           by clamping it to zero.
 * @tc.type: FUNC
 * @tc.require: issueIC8OCN
 * @tc.precon:
 *     1. The test environment supports instantiation of Uri and DataShareSAConnection objects.
 *     2. Predefined constants are valid: DATA_SHARE_URI (test target URI) and SA_ID (system ability ID = 1001).
 *     3. The DataShareSAConnection class has public members uri_, saId_, and waitTime_ that can be accessed.
 *     4. The constructor should handle negative wait time by setting it to 0.
 * @tc.step:
 *     1. Create a Uri object using the DATA_SHARE_URI constant.
 *     2. Instantiate a DataShareSAConnection object by passing the created Uri, SA_ID, and -1 (negative wait time)
 *        as constructor parameters.
 *     3. Verify that the uri_ member of the connection object matches DATA_SHARE_URI by comparing its ToString().
 *     4. Verify that the saId_ member of the connection object equals SA_ID.
 *     5. Verify that the waitTime_ member of the connection object equals 0 (clamped from -1).
 * @tc.expect:
 *     1. The connection.uri_.ToString() returns DATA_SHARE_URI.
 *     2. The connection.saId_ equals SA_ID (1001).
 *     3. The connection.waitTime_ equals 0 (clamped from the negative input -1).
 */
HWTEST_F(DataShareSAConnectionTest, DataShareSAConnectionConstructorNegativeWaitTime, TestSize.Level0)
{
    Uri uri(DATA_SHARE_URI);
    DataShareSAConnection connection(uri, SA_ID, -1);
    EXPECT_EQ(connection.uri_.ToString(), DATA_SHARE_URI);
    EXPECT_EQ(connection.saId_, SA_ID);
    EXPECT_EQ(connection.waitTime_, 0);
}

/**
 * @tc.name: DataShareSAConnectionGetDataShareProxy
 * @tc.desc: Verify the GetDataShareProxy method of DataShareSAConnection class returns nullptr
 *           when no valid proxy is available.
 * @tc.type: FUNC
 * @tc.require: issueIC8OCN
 * @tc.precon:
 *     1. The test environment supports instantiation of Uri and DataShareSAConnection objects.
 *     2. Predefined constants are valid: SA_ID (system ability ID = 1001) and WAIT_TIME (wait time = 10).
 *     3. The DataShareSAConnection class provides a GetDataShareProxy method that returns a shared_ptr.
 *     4. The test environment has no valid system ability registered for the test URI.
 * @tc.step:
 *     1. Create a Uri object with the string "datashare://test/SAID=111".
 *     2. Instantiate a DataShareSAConnection object by passing the created Uri, SA_ID,
          and WAIT_TIME as constructor parameters.
 *     3. Call the GetDataShareProxy method on the connection object to retrieve the proxy.
 *     4. Verify that the returned proxy is nullptr.
 * @tc.expect:
 *     1. The GetDataShareProxy method returns nullptr, indicating no valid proxy is available.
 */
HWTEST_F(DataShareSAConnectionTest, DataShareSAConnectionGetDataShareProxy, TestSize.Level0)
{
    Uri uri("datashare://test/SAID=111");
    DataShareSAConnection connection(uri, SA_ID, WAIT_TIME);
    auto proxy = connection.GetDataShareProxy();
    EXPECT_EQ(proxy, nullptr);
}

/**
 * @tc.name: DataShareSAConnectionGetDataShareProxyWithToken
 * @tc.desc: Verify the GetDataShareProxy method of DataShareSAConnection class with URI and token parameters
 *           returns nullptr when no valid proxy is available or when pool is null.
 * @tc.type: FUNC
 * @tc.require: issueIC8OCN
 * @tc.precon:
 *     1. The test environment supports instantiation of Uri, DataShareSAConnection,
          and IRemoteObject objects without errors.
 *     2. Predefined constants are valid: SA_ID (system ability ID = 1001) and WAIT_TIME (wait time = 10).
 *     3. The DataShareSAConnection class provides a GetDataShareProxy method that accepts Uri
          and IRemoteObject parameters.
 *     4. The DataShareSAConnection class has a public pool_ member that can be set to nullptr.
 * @tc.step:
 *     1. Create a Uri object with the string "datashare://test/SAID=111".
 *     2. Instantiate a DataShareSAConnection object by passing the created Uri, SA_ID,
          and WAIT_TIME as constructor parameters.
 *     3. Set token to nullptr and call GetDataShareProxy with the URI and token parameters.
 *     4. Verify that the returned proxy is nullptr.
 *     5. Set the pool_ member of the connection object to nullptr.
 *     6. Call GetDataShareProxy again with the same URI and token parameters.
 *     7. Verify that the returned proxy is still nullptr.
 * @tc.expect:
 *     1. The first GetDataShareProxy call returns nullptr (no valid proxy available).
 *     2. The second GetDataShareProxy call returns nullptr (pool is null).
 */
HWTEST_F(DataShareSAConnectionTest, DataShareSAConnectionGetDataShareProxyWithToken, TestSize.Level0)
{
    Uri uri("datashare://test/SAID=111");
    DataShareSAConnection connection(uri, SA_ID, WAIT_TIME);
    sptr<IRemoteObject> token = nullptr;
    auto proxy = connection.GetDataShareProxy(uri, token);
    EXPECT_EQ(proxy, nullptr);
    connection.pool_ = nullptr;
    proxy = connection.GetDataShareProxy(uri, token);
    EXPECT_EQ(proxy, nullptr);
}

/**
 * @tc.name: DataShareSAConnectionOnRemoteDied
 * @tc.desc: Verify the OnRemoteDied method of DataShareSAConnection class correctly resets the
 *           dataShareProxy_ member to nullptr when the remote service dies.
 * @tc.type: FUNC
 * @tc.require: issueIC8OCN
 * @tc.precon:
 *     1. The test environment supports instantiation of Uri, DataShareSAConnection,
          and DataShareProxy objects without errors.
 *     2. Predefined constants are valid: SA_ID (system ability ID = 1001) and WAIT_TIME (wait time = 10).
 *     3. The DataShareSAConnection class has a public dataShareProxy_ member that can be set and accessed.
 *     4. The DataShareSAConnection class provides an OnRemoteDied method that should reset the proxy to nullptr.
 * @tc.step:
 *     1. Create a Uri object with the string "datashare://test/SAID=111".
 *     2. Instantiate a DataShareSAConnection object by passing the created Uri, SA_ID,
          and WAIT_TIME as constructor parameters.
 *     3. Set the dataShareProxy_ member to a valid DataShareProxy object created with nullptr.
 *     4. Call the OnRemoteDied method on the connection object to simulate remote service death.
 *     5. Verify that the dataShareProxy_ member is reset to nullptr.
 * @tc.expect:
 *     1. The connection.dataShareProxy_ equals nullptr after OnRemoteDied is called.
 */
HWTEST_F(DataShareSAConnectionTest, DataShareSAConnectionOnRemoteDied, TestSize.Level0)
{
    Uri uri("datashare://test/SAID=111");
    DataShareSAConnection connection(uri, SA_ID, WAIT_TIME);
    connection.dataShareProxy_ = std::make_shared<DataShareProxy>(nullptr);
    connection.OnRemoteDied();
    EXPECT_EQ(connection.dataShareProxy_, nullptr);
}

/**
 * @tc.name: DataShareSAConnectionSALoadCallbackOnLoadSystemAbilitySuccess
 * @tc.desc: Verify that OnLoadSystemAbilitySuccess method of SALoadCallback class correctly
 *           updates the isLoadSuccess_ flag based on the provided remote object.
 * @tc.type: FUNC
 * @tc.require: issueIC8OCN
 * @tc.precon:
 *     1. The test environment supports instantiation of SALoadCallback and RemoteObjectTest objects.
 *     2. Predefined constants are valid: SA_ID (system ability ID = 1001).
 *     3. The SALoadCallback class has a public isLoadSuccess_ member that can be accessed.
 *     4. The RemoteObjectTest class is a mock implementation of IRemoteObject for testing purposes.
 * @tc.step:
 *     1. Instantiate a SALoadCallback object.
 *     2. Call OnLoadSystemAbilitySuccess with SA_ID and nullptr to simulate failed system ability loading.
 *     3. Verify that isLoadSuccess_ is false.
 *     4. Create a valid RemoteObjectTest object with the descriptor u"OHOS.DataShare.IDataShare".
 *     5. Call OnLoadSystemAbilitySuccess again with SA_ID and the valid token.
 *     6. Verify that isLoadSuccess_ is true.
 * @tc.expect:
 *     1. After the first call with nullptr, callback.isLoadSuccess_ is false.
 *     2. After the second call with a valid token, callback.isLoadSuccess_ is true.
 */
HWTEST_F(DataShareSAConnectionTest, DataShareSAConnectionSALoadCallbackOnLoadSystemAbilitySuccess, TestSize.Level0)
{
    DataShareSAConnection::SALoadCallback callback;
    
    callback.OnLoadSystemAbilitySuccess(SA_ID, nullptr);
    EXPECT_FALSE(callback.isLoadSuccess_.load());
    std::u16string tokenString = u"OHOS.DataShare.IDataShare";
    sptr<IRemoteObject> token = new (std::nothrow) RemoteObjectTest(tokenString);
    callback.OnLoadSystemAbilitySuccess(SA_ID, token);
    EXPECT_TRUE(callback.isLoadSuccess_.load());
}

/**
 * @tc.name: DataShareSAConnectionSALoadCallback_OnLoadSystemAbilityFail
 * @tc.desc:: Verify that OnLoadSystemAbilityFail method of SALoadCallback class correctly
 *           sets the isLoadSuccess_ flag to false when system ability loading fails.
 * @tc.type: FUNC
 * @tc.require: issueIC8OCN
 * @tc.precon:
 *     1. The test environment supports instantiation of SALoadCallback objects.
 *     2. Predefined constants are valid: SA_ID (system ability ID = 1001).
 *     3. The SALoadCallback class has a public isLoadSuccess_ member that can be accessed.
 *     4. The SALoadCallback class provides an OnLoadSystemAbilityFail method that should set isLoadSuccess_ to false.
 * @tc.step:
 *     1. Instantiate a SALoadCallback object.
 *     2. Call OnLoadSystemAbilityFail with SA_ID to simulate failed system ability loading.
 *     3. Verify that isLoadSuccess_ is false.
 * @tc.expect:
 *     1. After calling OnLoadSystemAbilityFail, callback.isLoadSuccess_ is false.
 */
HWTEST_F(DataShareSAConnectionTest, DataShareSAConnectionSALoadCallback_OnLoadSystemAbilityFail, TestSize.Level0)
{
    DataShareSAConnection::SALoadCallback callback;
    
    callback.OnLoadSystemAbilityFail(SA_ID);
    EXPECT_FALSE(callback.isLoadSuccess_.load());
}

/**
 * @tc.name: ConnectionInterfaceInfoConstructor
 * @tc.desc: Verify the default constructor of ConnectionInterfaceInfo class correctly initializes
 *           member variables to default values.
 * @tc.type: FUNC
 * @tc.require: issueIC8OCN
 * @tc.precon:
 *     1. The test environment supports instantiation of ConnectionInterfaceInfo objects without errors.
 *     2. The ConnectionInterfaceInfo class has public members code_ and descriptor_
          that can be accessed for verification.
 *     3. The INVALID_INTERFACE_CODE constant is predefined and accessible.
 * @tc.step:
 *     1. Instantiate a ConnectionInterfaceInfo object using the default constructor.
 *     2. Verify that the code_ member equals INVALID_INTERFACE_CODE.
 *     3. Verify that the descriptor_ member is an empty string.
 * @tc.expect:
 *     1. The info.code_ equals INVALID_INTERFACE_CODE.
 *     2. The info.descriptor_ is an empty string.
 */
HWTEST_F(DataShareSAConnectionTest, ConnectionInterfaceInfoConstructor, TestSize.Level0)
{
    ConnectionInterfaceInfo info;
    EXPECT_EQ(info.code_, INVALID_INTERFACE_CODE);
    EXPECT_TRUE(info.descriptor_.empty());
}

/**
 * @tc.name: ConnectionInterfaceInfoConstructorWithParams
 * @tc.desc: Verify the parameterized constructor of ConnectionInterfaceInfo class correctly initializes
 *           member variables with the provided parameters.
 * @tc.type: FUNC
 * @tc.require: issueIC8OCN
 * @tc.precon:
 *     1. The test environment supports instantiation of ConnectionInterfaceInfo objects without errors.
 *     2. The ConnectionInterfaceInfo class has a constructor that
          accepts uint32_t code and std::u16string descriptor.
 *     3. The ConnectionInterfaceInfo class has public members code_ and descriptor_
          that can be accessed for verification.
 *     4. Predefined constants are valid: SA_ID (system ability ID = 1001).
 * @tc.step:
 *     1. Define a uint32_t variable code and initialize it to SA_ID.
 *     2. Define a std::u16string variable descriptor and initialize it to u"test_descriptor".
 *     3. Instantiate a ConnectionInterfaceInfo object by passing code and descriptor as constructor parameters.
 *     4. Verify that the code_ member equals the provided code value.
 *     5. Verify that the descriptor_ member equals the provided descriptor value.
 * @tc.expect:
 *     1. The info.code_ equals the provided code value (SA_ID).
 *     2. The info.descriptor_ equals the provided descriptor value (u"test_descriptor").
 */
HWTEST_F(DataShareSAConnectionTest, ConnectionInterfaceInfoConstructorWithParams, TestSize.Level0)
{
    uint32_t code = SA_ID;
    std::u16string descriptor = u"test_descriptor";
    ConnectionInterfaceInfo info(code, descriptor);
    EXPECT_EQ(info.code_, code);
    EXPECT_EQ(info.descriptor_, descriptor);
}

/**
 * @tc.name: NonSilentConfigRecordDefault
 * @tc.desc: Verify the default constructor of NonSilentConfigRecord class correctly initializes
 *           member variables to empty strings.
 * @tc.type: FUNC
 * @tc.require: issueIC8OCN
 * @tc.precon:
 *     1. The test environment supports instantiation of NonSilentConfigRecord objects without errors.
 *     2. The NonSilentConfigRecord class has public members uri,
          readPermission, and writePermission that can be accessed.
 *     3. The default constructor should initialize string members to empty strings.
 * @tc.step:
 *     1. Instantiate a NonSilentConfigRecord object using the default constructor.
 *     2. Verify that the uri member is an empty string.
 *     3. Verify that the readPermission member is an empty string.
 *     4. Verify that the writePermission member is an empty string.
 * @tc.expect:
 *     1. The record.uri is an empty string.
 *     2. The record.readPermission is an empty string.
 *     3. The record.writePermission is an empty string.
 */
HWTEST_F(DataShareSAConnectionTest, NonSilentConfigRecordDefault, TestSize.Level0)
{
    NonSilentConfigRecord record;
    EXPECT_TRUE(record.uri.empty());
    EXPECT_TRUE(record.readPermission.empty());
    EXPECT_TRUE(record.writePermission.empty());
}

/**
 * @tc.name: DataShareNonSilentConfigDefault
 * @tc.desc: Verify the default constructor of DataShareNonSilentConfig class correctly initializes
 *           the records member to an empty vector.
 * @tc.type: FUNC
 * @tc.require: issueIC8OCN
 * @tc.precon:
 *     1. The test environment supports instantiation of DataShareNonSilentConfig objects without errors.
 *     2. The DataShareNonSilentConfig class has a public records member that can be accessed for verification.
 *     3. The default constructor should initialize the records member to an empty vector.
 * @tc.step:
 *     1. Instantiate a DataShareNonSilentConfig object using the default constructor.
 *     2. Verify that the records member is an empty vector.
 * @tc.expect:
 *     1. The config.records is an empty vector.
 */
HWTEST_F(DataShareSAConnectionTest, DataShareNonSilentConfigDefault, TestSize.Level0)
{
    DataShareNonSilentConfig config;
    EXPECT_TRUE(config.records.empty());
}
}
}