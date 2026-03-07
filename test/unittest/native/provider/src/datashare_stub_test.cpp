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

#define LOG_TAG "datashare_stub_test"

#include "datashare_stub.h"

#include <gtest/gtest.h>

#include "accesstoken_kit.h"
#include "data_ability_observer_stub.h"
#include "datashare_business_error.h"
#include "datashare_errno.h"
#include "datashare_itypes_utils.h"
#include "datashare_log.h"
#include "datashare_operation_statement.h"
#include "datashare_stub_impl.h"
#include "extension_manager_proxy.h"
#include "idatashare.h"
#include "ipc_skeleton.h"
#include "ipc_types.h"
#include "ishared_result_set.h"
#include "mock_token.h"


namespace OHOS {
namespace DataShare {
using namespace testing::ext;
using namespace DistributedShare::DataShare;
using namespace OHOS::AAFwk;
using namespace OHOS::Security::AccessToken;
using ChangeInfo = AAFwk::ChangeInfo;
class DataShareStubTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
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

class TestDataShareStub : public DataShareStub {
public:
    TestDataShareStub() {}
    ~TestDataShareStub() {}
    int OpenFile(const Uri &uri, const std::string &mode)
    {
        return 0;
    }
    int OpenRawFile(const Uri &uri, const std::string &mode)
    {
        return 0;
    }
    int Insert(const Uri &uri, const DataShareValuesBucket &value)
    {
        return 0;
    }
    int Update(const Uri &uri, const DataSharePredicates &predicates, const DataShareValuesBucket &value)
    {
        return 0;
    }
    int Delete(const Uri &uri, const DataSharePredicates &predicates)
    {
        return 0;
    }
    std::shared_ptr<DataShareResultSet> Query(const Uri &uri, const DataSharePredicates &predicates,
        std::vector<std::string> &columns, DatashareBusinessError &businessError)
    {
        return nullptr;
    }
    std::string GetType(const Uri &uri)
    {
        return "";
    }
    int BatchInsert(const Uri &uri, const std::vector<DataShareValuesBucket> &values)
    {
        return 0;
    }
    bool RegisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver)
    {
        return true;
    }
    bool UnregisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver)
    {
        return true;
    }
    bool NotifyChange(const Uri &uri)
    {
        return true;
    }
    Uri NormalizeUri(const Uri &uri)
    {
        return uri;
    }
    Uri DenormalizeUri(const Uri &uri)
    {
        return uri;
    }
    std::vector<std::string> GetFileTypes(const Uri &uri, const std::string &mimeTypeFilter)
    {
        return {};
    }
    DataShareNonSilentConfig GetConfig()
    {
        NonSilentConfigRecord record = {
            "datashare://distributeddata/SAID=1301",
            "ohos.permission.APPROXIMATELY_LOCATION",
            "ohos.permission.GET_BUNDLE_INFO"
        };
        NonSilentConfigRecord record2 = {
            "datashare://appgallery_service/SAID=65974",
            "ohos.permission.GET_BUNDLE_INFO",
            "ohos.permission.APPROXIMATELY_LOCATION"
        };
        NonSilentConfigRecord record3 = {
            "datashare://test/SAID=11111",
            "ohos.permission.APPROXIMATELY_LOCATION",
            "ohos.permission.APPROXIMATELY_LOCATION"
        };
        DataShareNonSilentConfig saconfig;
        saconfig.records.push_back(record);
        saconfig.records.push_back(record2);
        saconfig.records.push_back(record3);
        return saconfig;
    }
};

std::string DATA_SHARE_URI = "datashare:///com.acts.datasharetest";
std::shared_ptr<DataShareStub> dataShareStub = std::make_shared<TestDataShareStub>();
std::u16string InterfaceToken = u"OHOS.DataShare.IDataShare";

HapPolicyParams GetPolicy()
{
    HapPolicyParams policy = { .apl = APL_SYSTEM_CORE,
        .domain = "test.domain",
        .permList = { { .permissionName = "ohos.permission.test",
            .bundleName = "ohos.datashareclienttest.demo",
            .grantMode = 1,
            .availableLevel = APL_SYSTEM_CORE,
            .label = "label",
            .labelId = 1,
            .description = "ohos.datashareclienttest.demo",
            .descriptionId = 1 } },
        .permStateList = {
            { .permissionName = "ohos.permission.test",
                .isGeneral = true,
                .resDeviceID = { "local" },
                .grantStatus = { PermissionState::PERMISSION_GRANTED },
                .grantFlags = { 1 } },
            { .permissionName = "ohos.permission.GET_BUNDLE_INFO",
                .isGeneral = true,
                .resDeviceID = { "local" },
                .grantStatus = { PermissionState::PERMISSION_GRANTED },
                .grantFlags = { 1 } } } };
    return policy;
}

void DataShareStubTest::SetUpTestCase(void)
{
    LOG_INFO("JoinTest SetUpTestCase invoked. get tokenId start.");
    MockToken::SetTestEnvironment();
    int sleepTime = 3;
    sleep(sleepTime);

    HapInfoParams info = {
        .userID = 100,
        .bundleName = "ohos.datashareclienttest.demo",
        .instIndex = 0,
        .isSystemApp = true,
        .appIDDesc = "ohos.datashareclienttest.demo" };
    HapPolicyParams policy = GetPolicy();
    AccessTokenIDEx tokenIdEx = MockToken::AllocTestHapToken(info, policy);
    uint64_t token = tokenIdEx.tokenIdExStruct.tokenID;
    if (token == INVALID_TOKENID) {
        LOG_ERROR("JoinTest token invalid.");
        return;
    }
    int ret = SetSelfTokenID(token);
    if (ret != E_OK) {
        LOG_ERROR("JoinTest SetSelfTokenID: %{public}d", ret);
        return;
    }
}
void DataShareStubTest::TearDownTestCase(void) {}
void DataShareStubTest::SetUp(void) {}
void DataShareStubTest::TearDown(void) {}

/**
 * @tc.name: DataShareStub_CmdRegisterObserverExtProvider_Test_001
 * @tc.desc: Verify the handling functionality of the CMD_REGISTER_OBSERVEREXT_PROVIDER command in DataShareStub,
 *           focusing on parameter marshaling, remote request execution, and result unmarshaling.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports instantiation of Uri, IDataAbilityObserverTest, MessageParcel, MessageOption,
       and RegisterOption objects without initialization errors.
    2. Predefined constants are valid: DATA_SHARE_URI (test target URI), InterfaceToken (interface token for
       parcel writing), and IDataShareInterfaceCode::CMD_REGISTER_OBSERVEREXT_PROVIDER (command code).
    3. The ITypesUtil class provides valid Marshal and Unmarshal methods for Uri, IRemoteObject, bool,
       RegisterOption, and int types.
    4. The global dataShareStub instance is properly initialized (non-null) before the test.
 * @tc.step:
    1. Create a test Uri using the DATA_SHARE_URI constant, then instantiate an IDataAbilityObserverTest object
       via new (std::nothrow) and verify it is non-null.
    2. Convert the CMD_REGISTER_OBSERVEREXT_PROVIDER command code to uint32_t type, then create a MessageParcel
       (data) and write the InterfaceToken to it using WriteInterfaceToken.
    3. Instantiate a RegisterOption object, then call ITypesUtil::Marshal to serialize the Uri, IRemoteObject
       (from dataObserver->AsObject()), bool (true), and RegisterOption into the 'data' parcel; verify marshaling
       succeeds (returns true).
    4. Create empty MessageParcel (reply) and default MessageOption (option), then call dataShareStub->OnRemoteRequest
       with the command code, 'data' parcel, 'reply' parcel, and 'option'; record the returned error code.
    5. Call ITypesUtil::Unmarshal to deserialize the error code from the 'reply' parcel; verify unmarshaling succeeds
       (returns true) and check the error code value.
 * @tc.expect:
    1. All initialized objects (IDataAbilityObserverTest, MessageParcel, etc.) are non-null and valid.
    2. The OnRemoteRequest method returns 0 (indicating successful command handling).
    3. The unmarshaled error code from the 'reply' parcel is 0 (E_OK, indicating no execution errors).
 */
HWTEST_F(DataShareStubTest, DataShareStub_CmdRegisterObserverExtProvider_Test_001, TestSize.Level0)
{
    LOG_INFO("DataShareStub_CmdRegisterObserverExtProvider_Test_001::Start");

    Uri uri(DATA_SHARE_URI);
    sptr<IDataAbilityObserverTest> dataObserver = new (std::nothrow) IDataAbilityObserverTest();
    ASSERT_NE(dataObserver, nullptr);
    uint32_t code = static_cast<uint32_t>(IDataShareInterfaceCode::CMD_REGISTER_OBSERVEREXT_PROVIDER);

    MessageParcel data;
    data.WriteInterfaceToken(InterfaceToken);
    RegisterOption registerOption;
    bool ret = ITypesUtil::Marshal(data, uri, dataObserver->AsObject(), true, registerOption);
    EXPECT_TRUE(ret);

    MessageParcel reply;
    MessageOption option;
    ASSERT_NE(dataShareStub, nullptr);
    int errCode = dataShareStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(errCode, 0);
    ret = ITypesUtil::Unmarshal(reply, errCode);
    EXPECT_TRUE(ret);
    // return default errCode 0(E_OK)
    EXPECT_EQ(errCode, 0);

    LOG_INFO("DataShareStub_CmdRegisterObserverExtProvider_Test_001::End");
}

/**
 * @tc.name: DataShareStub_CmdUnregisterObserverExtProvider_Test_001
 * @tc.desc: Verify the handling functionality of the CMD_UNREGISTER_OBSERVEREXT_PROVIDER command in DataShareStub,
 *           focusing on parameter marshaling, remote request execution, and result unmarshaling.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports instantiation of Uri, IDataAbilityObserverTest, MessageParcel, and
       MessageOption objects without initialization errors.
    2. Predefined constants are valid: DATA_SHARE_URI (test target URI), InterfaceToken (interface token for
       parcel writing), and IDataShareInterfaceCode::CMD_UNREGISTER_OBSERVEREXT_PROVIDER (command code).
    3. The ITypesUtil class provides valid Marshal and Unmarshal methods for Uri, IRemoteObject, and int types.
    4. The global dataShareStub instance is properly initialized (non-null) before the test.
 * @tc.step:
    1. Create a test Uri using the DATA_SHARE_URI constant, then instantiate an IDataAbilityObserverTest object
       via new (std::nothrow) and verify it is non-null.
    2. Convert the CMD_UNREGISTER_OBSERVEREXT_PROVIDER command code to uint32_t type, then create a MessageParcel
       (data) and write the InterfaceToken to it using WriteInterfaceToken.
    3. Call ITypesUtil::Marshal to serialize the Uri and IRemoteObject (from dataObserver->AsObject()) into the
       'data' parcel; verify marshaling succeeds (returns true).
    4. Create empty MessageParcel (reply) and default MessageOption (option), then call dataShareStub->OnRemoteRequest
       with the command code, 'data' parcel, 'reply' parcel, and 'option'; record the returned error code.
    5. Call ITypesUtil::Unmarshal to deserialize the error code from the 'reply' parcel; verify unmarshaling succeeds
       (returns true) and check the error code value.
 * @tc.expect:
    1. All initialized objects (IDataAbilityObserverTest, MessageParcel, etc.) are non-null and valid.
    2. The OnRemoteRequest method returns 0 (indicating successful command handling).
    3. The unmarshaled error code from the 'reply' parcel is 0 (E_OK, indicating no execution errors).
 */
HWTEST_F(DataShareStubTest, DataShareStub_CmdUnregisterObserverExtProvider_Test_001, TestSize.Level0)
{
    LOG_INFO("DataShareStub_CmdUnregisterObserverExtProvider_Test_001::Start");

    Uri uri(DATA_SHARE_URI);
    sptr<IDataAbilityObserverTest> dataObserver = new (std::nothrow) IDataAbilityObserverTest();
    ASSERT_NE(dataObserver, nullptr);
    uint32_t code = static_cast<uint32_t>(IDataShareInterfaceCode::CMD_UNREGISTER_OBSERVEREXT_PROVIDER);

    MessageParcel data;
    data.WriteInterfaceToken(InterfaceToken);
    bool ret = ITypesUtil::Marshal(data, uri, dataObserver->AsObject());
    EXPECT_TRUE(ret);

    MessageParcel reply;
    MessageOption option;
    int errCode = dataShareStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(errCode, 0);
    ret = ITypesUtil::Unmarshal(reply, errCode);
    EXPECT_TRUE(ret);
    // return default errCode 0(E_OK)
    EXPECT_EQ(errCode, 0);

    LOG_INFO("DataShareStub_CmdUnregisterObserverExtProvider_Test_001::End");
}

/**
 * @tc.name: DataShareStub_CmdNotifyChangeExtProvider_Test_001
 * @tc.desc: Verify the handling functionality of the CMD_NOTIFY_CHANGEEXT_PROVIDER command in DataShareStub,
 *           focusing on ChangeInfo marshaling, remote request execution, and result unmarshaling.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports instantiation of Uri, ChangeInfo, MessageParcel, and MessageOption objects
       without initialization errors.
    2. Predefined constants are valid: DATA_SHARE_URI (test target URI), InterfaceToken (interface token for
       parcel writing), and IDataShareInterfaceCode::CMD_NOTIFY_CHANGEEXT_PROVIDER (command code).
    3. The ChangeInfo class provides a valid Marshalling static method; the ITypesUtil class provides a valid
       Unmarshal method for int types.
    4. The global dataShareStub instance is properly initialized (non-null) before the test.
 * @tc.step:
    1. Create a test Uri using the DATA_SHARE_URI constant, then instantiate a ChangeInfo object: set its ChangeType
       to INSERT and assign a vector containing the test Uri to its 'uris' member.
    2. Convert the CMD_NOTIFY_CHANGEEXT_PROVIDER command code to uint32_t type, then create a MessageParcel (data)
       and write the InterfaceToken to it using WriteInterfaceToken.
    3. Call ChangeInfo::Marshalling to serialize the ChangeInfo object into the 'data' parcel; verify marshaling
       succeeds (returns true).
    4. Create empty MessageParcel (reply) and default MessageOption (option), then call dataShareStub->OnRemoteRequest
       with the command code, 'data' parcel, 'reply' parcel, and 'option'; record the returned error code.
    5. Call ITypesUtil::Unmarshal to deserialize the error code from the 'reply' parcel; verify unmarshaling succeeds
       (returns true) and check the error code value.
 * @tc.expect:
    1. All initialized objects (ChangeInfo, MessageParcel, etc.) are non-null and valid.
    2. The OnRemoteRequest method returns 0 (indicating successful command handling).
    3. The unmarshaled error code from the 'reply' parcel is 0 (E_OK, indicating no execution errors).
 */
HWTEST_F(DataShareStubTest, DataShareStub_CmdNotifyChangeExtProvider_Test_001, TestSize.Level0)
{
    LOG_INFO("DataShareStub_CmdNotifyChangeExtProvider_Test_001::Start");

    Uri uri(DATA_SHARE_URI);
    ChangeInfo changeInfo = { ChangeInfo::ChangeType::INSERT, { uri } };
    uint32_t code = static_cast<uint32_t>(IDataShareInterfaceCode::CMD_NOTIFY_CHANGEEXT_PROVIDER);

    MessageParcel data;
    data.WriteInterfaceToken(InterfaceToken);
    bool ret = ChangeInfo::Marshalling(changeInfo, data);
    EXPECT_TRUE(ret);

    MessageParcel reply;
    MessageOption option;
    int errCode = dataShareStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(errCode, 0);
    ret = ITypesUtil::Unmarshal(reply, errCode);
    EXPECT_TRUE(ret);
    // return default errCode 0(E_OK)
    EXPECT_EQ(errCode, 0);

    LOG_INFO("DataShareStub_CmdNotifyChangeExtProvider_Test_001::End");
}

/**
 * @tc.name: DataShareStub_VerifyPermissionAndUri_Test_001
 * @tc.desc: Verify VerifyPermissionAndUri method of DataShareStub class correctly validates
 *           permissions and URIs for different system ability configurations.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
 *     1. The test environment supports calling GetSelfTokenID() to obtain current token ID.
 *     2. The TestDataShareStub class provides a GetConfig() method that returns a DataShareNonSilentConfig
 *        containing predefined URI and permission configurations.
 *     3. The DataShareNonSilentConfig contains records for:
 *        - "datashare://distributeddata/SAID=1301" with read/write permissions
 *        - "datashare://appgallery_service/SAID=65974" with read/write permissions
 *        - "datashare://test/SAID=11111" with read/write permissions
 *     4. The DataShareStub class provides a VerifyPermissionAndUri method that accepts URI and token ID.
 * @tc.step:
 *     1. Call VerifyPermissionAndUri with "datashare://distributeddata/SAID=1301" and current token ID;
 *        verify return is true.
 *     2. Call VerifyPermissionAndUri with "datashare://appgallery_service/SAID=65974" and current token ID;
 *        verify return is true.
 *     3. Call VerifyPermissionAndUri with "datashare://test/SAID=11111" and current token ID;
 *        verify return is false.
 * @tc.expect:
 *     1. First call returns true (URI and permissions are valid).
 *     2. Second call returns true (URI and permissions are valid).
 *     3. Third call returns false (URI or permissions are invalid).
 */
HWTEST_F(DataShareStubTest, DataShareStub_VerifyPermissionAndUri_Test_001, TestSize.Level0)
{
    LOG_INFO("DataShareStub_VerifyPermissionAndUri_Test_001::Start");
    EXPECT_TRUE(dataShareStub->VerifyPermissionAndUri("datashare://distributeddata/SAID=1301", GetSelfTokenID()));
    EXPECT_TRUE(dataShareStub->VerifyPermissionAndUri("datashare://appgallery_service/SAID=65974", GetSelfTokenID()));
    EXPECT_FALSE(dataShareStub->VerifyPermissionAndUri("datashare://test/SAID=11111", GetSelfTokenID()));
    LOG_INFO("DataShareStub_VerifyPermissionAndUri_Test_001::End");
}
}
}