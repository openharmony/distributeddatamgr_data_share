/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#define LOG_TAG "slientaccess_test"

#include <gtest/gtest.h>
#include <unistd.h>
#include <vector>

#include "accesstoken_kit.h"
#include "data_ability_observer_stub.h"
#include "datashare_helper.h"
#include "datashare_log.h"
#include "hap_token_info.h"
#include "iservice_registry.h"
#include "mock_token.h"
#include "system_ability_definition.h"
#include "token_setproc.h"

namespace OHOS {
namespace DataShare {
using namespace testing::ext;
using namespace OHOS::Security::AccessToken;
using ChangeInfo = DataShareObserver::ChangeInfo;
constexpr int STORAGE_MANAGER_MANAGER_ID = 5003;
static int USER_100 = 100;
std::string DATA_SHARE_URI = "datashare:///com.acts.datasharetest";
std::string DATA_SHARE_ERROR_URI = "datashare:///com.acts.datasharetest000";
std::string SLIENT_ACCESS_URI = "datashare:///com.acts.datasharetest/entry/DB00/TBL00?Proxy=true";
std::string SLIENT_ERROR_URI = "datashare:///com.acts.datashare/entry/DB00/TBL00?Proxy=true";
std::string SLIENT_ERROR_DATABASE_URI = "datashare:///com.acts.datasharetest/entry/DB6666/TBL00?Proxy=true";
std::string SLIENT_REGISTER_URI = "datashare:///com.acts.datasharetest/entry/DB00/TBL02?Proxy=true";
std::string SLIENT_ACCESS_PERMISSION1_URI = "datashare:///com.acts.datasharetest/entry/DB00/permission1?Proxy=true";
std::string SLIENT_PROXY_PERMISSION1_URI = "datashareproxy://com.acts.datasharetest/entry/DB00/permission1";
std::string SLIENT_ACCESS_PERMISSION2_URI = "datashare:///com.acts.datasharetest/entry/DB00/permission2?Proxy=true";
std::string SLIENT_PROXY_PERMISSION2_URI = "datashareproxy://com.acts.datasharetest/entry/DB00/permission2";
std::string TBL_STU_NAME = "name";
std::string TBL_STU_AGE = "age";
std::shared_ptr<DataShare::DataShareHelper> g_slientAccessHelper;

template <typename T>
class ConditionLock {
public:
    explicit ConditionLock() {}
    ~ConditionLock() {}
public:
    void Notify()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        isSet_ = true;
        cv_.notify_one();
    }
    
    void Wait()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait_for(lock, std::chrono::seconds(INTERVAL), [this]() { return isSet_; });
        cv_.notify_one();
        return;
    }
    
    void Clear()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        isSet_ = false;
        cv_.notify_one();
    }

private:
    bool isSet_ = false;
    T data_;
    std::mutex mutex_;
    std::condition_variable cv_;
    static constexpr int64_t INTERVAL = 2;
};

class SlientAccessTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

class IDataShareAbilityObserverTest : public AAFwk::DataAbilityObserverStub {
public:
    IDataShareAbilityObserverTest() =  default;

    ~IDataShareAbilityObserverTest()
    {}

    void OnChange()
    {
        name = "OnChangeName";
        data.Notify();
    }

    std::string GetName()
    {
        return name;
    }

    void SetName(std::string name)
    {
        this->name = name;
    }
	
    void Clear()
    {
        data.Clear();
    }
    ConditionLock<std::string> data;
private:
    std::string name;
};

class DataShareObserverTest : public DataShare::DataShareObserver {
public:
    DataShareObserverTest() {}
    ~DataShareObserverTest() {}
    
    void OnChange(const ChangeInfo &changeInfo) override
    {
        changeInfo_ = changeInfo;
        data.Notify();
    }
    
    void Clear()
    {
        changeInfo_.changeType_ = INVAILD;
        changeInfo_.uris_.clear();
        changeInfo_.data_ = nullptr;
        changeInfo_.size_ = 0;
        changeInfo_.valueBuckets_ = {};
        data.Clear();
    }
    
    ChangeInfo changeInfo_;
    ConditionLock<ChangeInfo> data;
};

std::shared_ptr<DataShare::DataShareHelper> CreateDataShareHelper(int32_t systemAbilityId, std::string uri)
{
    LOG_INFO("CreateDataShareHelper start");
    auto saManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (saManager == nullptr) {
        LOG_ERROR("GetSystemAbilityManager get samgr failed.");
        return nullptr;
    }
    auto remoteObj = saManager->GetSystemAbility(systemAbilityId);
    if (remoteObj == nullptr) {
        LOG_ERROR("GetSystemAbility service failed.");
        return nullptr;
    }
    return DataShare::DataShareHelper::Creator(remoteObj, uri);
}

std::vector<PermissionStateFull> GetPermissionStateFulls()
{
    std::vector<PermissionStateFull> permissionStateFulls = {
        {
            .permissionName = "ohos.permission.WRITE_CONTACTS",
            .isGeneral = true,
            .resDeviceID = { "local" },
            .grantStatus = { PermissionState::PERMISSION_GRANTED },
            .grantFlags = { 1 }
        },
        {
            .permissionName = "ohos.permission.WRITE_CALL_LOG",
            .isGeneral = true,
            .resDeviceID = { "local" },
            .grantStatus = { PermissionState::PERMISSION_GRANTED },
            .grantFlags = { 1 }
        },
        {
            .permissionName = "ohos.permission.GET_BUNDLE_INFO",
            .isGeneral = true,
            .resDeviceID = { "local" },
            .grantStatus = { PermissionState::PERMISSION_GRANTED },
            .grantFlags = { 1 }
        }
    };
    return permissionStateFulls;
}

HapPolicyParams GetPolicy()
{
    auto permStateList = GetPermissionStateFulls();
    HapPolicyParams policy = {
        .apl = APL_SYSTEM_CORE,
        .domain = "test.domain",
        .permList = {
            {
                .permissionName = "ohos.permission.test",
                .bundleName = "ohos.datashareclienttest.demo",
                .grantMode = 1,
                .availableLevel = APL_SYSTEM_CORE,
                .label = "label",
                .labelId = 1,
                .description = "ohos.datashareclienttest.demo",
                .descriptionId = 1
            }
        },
        .permStateList = permStateList
    };
    return policy;
}

void SlientAccessTest::SetUpTestCase(void)
{
    LOG_INFO("SetUpTestCase invoked");
    MockToken::SetTestEnvironment();
    auto dataShareHelper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID, DATA_SHARE_URI);
    if (dataShareHelper != nullptr) {
        LOG_INFO("SlientAccessTest non-silent helper connect done");
    } else {
        LOG_ERROR("SlientAccessTest non-silent helper connect fail");
    }
    int sleepTime = 3;
    sleep(sleepTime);

    HapInfoParams info = {
        .userID = 100,
        .bundleName = "ohos.datashareclienttest.demo",
        .instIndex = 0,
        .isSystemApp = true,
        .appIDDesc = "ohos.datashareclienttest.demo"
    };
    HapPolicyParams policy = GetPolicy();
    AccessTokenIDEx tokenIdEx = MockToken::AllocTestHapToken(info, policy);
    uint64_t token = tokenIdEx.tokenIdExStruct.tokenID;
    if (token == INVALID_TOKENID) {
        LOG_ERROR("SlientAccessTest token invalid.");
        return;
    }
    int ret = SetSelfTokenID(token);
    if (ret != E_OK) {
        LOG_ERROR("SlientAccessTest SetSelfTokenID: %{public}d", ret);
        return;
    }

    g_slientAccessHelper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID, SLIENT_ACCESS_URI);
    if (g_slientAccessHelper != nullptr) {
        LOG_INFO("SlientAccessTest silent helper connect end");
    } else {
        LOG_ERROR("SlientAccessTest silent helper connect fail");
    }
    LOG_INFO("SetUpTestCase end");
}

void SlientAccessTest::TearDownTestCase(void)
{
    auto tokenId = AccessTokenKit::GetHapTokenIDEx(100, "ohos.datashareclienttest.demo", 0);
    AccessTokenKit::DeleteToken(tokenId.tokenIDEx);
    g_slientAccessHelper = nullptr;
    MockToken::ResetTestEnvironment();
}

void SlientAccessTest::SetUp(void) {}
void SlientAccessTest::TearDown(void) {}

/**
 * @tc.name: SlientAccess_Creator_Errorcode_Test_001
 * @tc.desc: Test the creation process of DataShareHelper using valid parameters (valid SystemAbilityManager,
 *           remote object for STORAGE_MANAGER_MANAGER_ID, and valid URIs), verifying the return code and
 *           non-null status of the helper instance.
 * @tc.type: FUNC
 * @tc.require: NA
 * @tc.precon:
    1. The test environment supports accessing the SystemAbilityManagerClient and obtaining a valid
       SystemAbilityManager instance via GetSystemAbilityManager().
    2. The STORAGE_MANAGER_MANAGER_ID constant is predefined and valid, ensuring the SystemAbilityManager can
       retrieve the corresponding remote object for this system ability.
    3. The SLIENT_ACCESS_URI and DATA_SHARE_URI constants are valid URI strings, compatible with the
       DataShareHelper::Create() method's parameter requirements.
    4. The DataShareHelper::Create() method returns a pair of (integer error code, std::shared_ptr<DataShareHelper>),
       and the E_OK constant is predefined as the success error code.
 * @tc.step:
    1. Call SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager() to obtain the
       SystemAbilityManager instance (saManager).
    2. Use the obtained saManager to call GetSystemAbility(STORAGE_MANAGER_MANAGER_ID), retrieving the
       remote object corresponding to the STORAGE_MANAGER_MANAGER_ID system ability.
    3. Define two valid URI strings: one using SLIENT_ACCESS_URI (uriStr1) and another using DATA_SHARE_URI (uriStr2).
    4. Call DataShare::DataShareHelper::Create() with the retrieved remote object, uriStr1, and uriStr2 as parameters,
       recording the returned (ret, helper) pair.
    5. Check the value of the return code (ret) and verify whether the helper instance is non-null.
 * @tc.expect:
    1. The return code (ret) from DataShareHelper::Create() is equal to DataShare::E_OK (indicating successful
       creation).
    2. The returned DataShareHelper instance (helper) is not nullptr.
 */
HWTEST_F(SlientAccessTest, SlientAccess_Creator_Errorcode_Test_001, TestSize.Level0)
{
    LOG_INFO("SlientAccess_Creator_Errorcode_Test_001::Start");
    std::string uriStr1(SLIENT_ACCESS_URI);
    std::string uriStr2 (DATA_SHARE_URI);
    auto saManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (saManager == nullptr) {
        LOG_ERROR("GetSystemAbilityManager get samgr failed.");
    }
    auto remoteObj = saManager->GetSystemAbility(STORAGE_MANAGER_MANAGER_ID);
    if (remoteObj == nullptr) {
        LOG_ERROR("GetSystemAbility service failed.");
    }
    auto [ret, helper] = DataShare::DataShareHelper::Create(remoteObj, uriStr1, uriStr2);
    EXPECT_EQ(ret, DataShare::E_OK);
    EXPECT_NE(helper, nullptr);
    helper = nullptr;
    LOG_INFO("SlientAccess_Creator_Errorcode_Test_001::End");
}

/**
 * @tc.name: SlientAccess_Creator_Errorcode_Test_002
 * @tc.desc: Test the creation process of DataShareHelper when passing a null remote object (with valid URIs),
 *           verifying the return error code and null status of the helper instance.
 * @tc.type: FUNC
 * @tc.require: NA
 * @tc.precon:
    1. The test environment supports the DataShare::DataShareHelper::Create() method, which accepts a remote object
       and two URI strings, returning a (error code, helper instance) pair.
    2. Predefined constants SLIENT_ACCESS_URI and DATA_SHARE_URI are valid URI strings (compatible with Create()
       parameters).
    3. The E_TOKEN_EMPTY constant is predefined as the error code for null remote object, accessible in the test
       context.
 * @tc.step:
    1. Define two valid URI strings: uriStr1 using SLIENT_ACCESS_URI and uriStr2 using DATA_SHARE_URI.
    2. Call DataShare::DataShareHelper::Create() with a null pointer (as remote object), uriStr1, and uriStr2.
    3. Record the returned pair (ret, helper) from the Create() method.
    4. Check the value of the return code (ret) and verify whether the helper instance is null.
 * @tc.expect:
    1. The return code (ret) from Create() is equal to DataShare::E_TOKEN_EMPTY.
    2. The returned DataShareHelper instance (helper) is nullptr.
 */
HWTEST_F(SlientAccessTest, SlientAccess_Creator_Errorcode_Test_002, TestSize.Level0)
{
    LOG_INFO("SlientAccess_Creator_Errorcode_Test_002::Start");
    std::string uriStr1(SLIENT_ACCESS_URI);
    std::string uriStr2 (DATA_SHARE_URI);
    auto [ret, helper] = DataShare::DataShareHelper::Create(nullptr, uriStr1, uriStr2);
    EXPECT_EQ(ret, DataShare::E_TOKEN_EMPTY);
    EXPECT_EQ(helper, nullptr);
    LOG_INFO("SlientAccess_Creator_Errorcode_Test_002::End");
}

/**
 * @tc.name: SlientAccess_Creator_Errorcode_Test_003
 * @tc.desc: Test the creation process of DataShareHelper with a valid remote object but invalid URIs (error URI
 *           and empty URI), verifying the return error code and null helper instance.
 * @tc.type: FUNC
 * @tc.require: NA
 * @tc.precon:
    1. The test environment allows obtaining a valid SystemAbilityManager instance via
       SystemAbilityManagerClient::GetInstance().
    2. STORAGE_MANAGER_MANAGER_ID is predefined, enabling SystemAbilityManager to retrieve its corresponding remote
       object.
    3. SLIENT_ERROR_URI (invalid) and empty string ("") are recognized as invalid URIs by Create(); E_EXT_URI_INVALID
       is predefined as the corresponding error code.
 * @tc.step:
    1. Call SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager() to get the SystemAbilityManager
       instance (saManager).
    2. Use saManager->GetSystemAbility(STORAGE_MANAGER_MANAGER_ID) to retrieve the valid remote object for the system
       ability.
    3. Define two invalid URIs: uriStr1 using SLIENT_ERROR_URI and uriStr2 as an empty string ("").
    4. Call DataShare::DataShareHelper::Create() with the valid remote object, uriStr1, and uriStr2.
    5. Record the returned (ret, helper) pair and check the error code and helper instance status.
 * @tc.expect:
    1. The return code (ret) from Create() is equal to DataShare::E_EXT_URI_INVALID.
    2. The returned DataShareHelper instance (helper) is nullptr.
 */
HWTEST_F(SlientAccessTest, SlientAccess_Creator_Errorcode_Test_003, TestSize.Level0)
{
    LOG_INFO("SlientAccess_Creator_Errorcode_Test_003::Start");
    std::string uriStr1(SLIENT_ERROR_URI);
    std::string uriStr2 ("");
    auto saManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (saManager == nullptr) {
        LOG_ERROR("GetSystemAbilityManager get samgr failed.");
    }
    auto remoteObj = saManager->GetSystemAbility(STORAGE_MANAGER_MANAGER_ID);
    if (remoteObj == nullptr) {
        LOG_ERROR("GetSystemAbility service failed.");
    }
    // slientUri is error bundle name, extUri is empty, slient access can't find the bundle name
    auto [ret, helper] = DataShare::DataShareHelper::Create(remoteObj, uriStr1, uriStr2);
    EXPECT_EQ(ret, DataShare::E_EXT_URI_INVALID);
    EXPECT_EQ(helper, nullptr);
    helper = nullptr;
    LOG_INFO("SlientAccess_Creator_Errorcode_Test_003::End");
}

/**
 * @tc.name: SlientAccess_Creator_Errorcode_Test_004
 * @tc.desc: Test the creation process of DataShareHelper with a valid remote object and two identical valid URIs
 *           (DATA_SHARE_URI), verifying successful creation via return code and non-null helper.
 * @tc.type: FUNC
 * @tc.require: NA
 * @tc.precon:
    1. The test environment supports retrieving a valid SystemAbilityManager instance and the remote object for
       STORAGE_MANAGER_MANAGER_ID.
    2. DATA_SHARE_URI is a predefined valid URI string; using two identical copies of it is compatible with Create()
       parameters.
    3. The E_OK constant is predefined as the success error code for Create().
 * @tc.step:
    1. Obtain the SystemAbilityManager instance (saManager) via
       SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager().
    2. Retrieve the valid remote object for STORAGE_MANAGER_MANAGER_ID using saManager->GetSystemAbility().
    3. Define two identical URI strings (uriStr1 and uriStr2), both using DATA_SHARE_URI.
    4. Call DataShare::DataShareHelper::Create() with the valid remote object, uriStr1, and uriStr2.
    5. Check the returned error code (ret) and whether the helper instance is non-null.
 * @tc.expect:
    1. The return code (ret) from Create() is equal to DataShare::E_OK.
    2. The returned DataShareHelper instance (helper) is not nullptr.
 */
HWTEST_F(SlientAccessTest, SlientAccess_Creator_Errorcode_Test_004, TestSize.Level0)
{
    LOG_INFO("SlientAccess_Creator_Errorcode_Test_004::Start");
    std::string uriStr1(DATA_SHARE_URI);
    std::string uriStr2 (DATA_SHARE_URI);
    auto saManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (saManager == nullptr) {
        LOG_ERROR("GetSystemAbilityManager get samgr failed.");
    }
    auto remoteObj = saManager->GetSystemAbility(STORAGE_MANAGER_MANAGER_ID);
    if (remoteObj == nullptr) {
        LOG_ERROR("GetSystemAbility service failed.");
    }
    auto [ret, helper] = DataShare::DataShareHelper::Create(remoteObj, uriStr1, uriStr2);
    EXPECT_EQ(ret, DataShare::E_OK);
    EXPECT_NE(helper, nullptr);
    helper = nullptr;
    LOG_INFO("SlientAccess_Creator_Errorcode_Test_004::End");
}

/**
 * @tc.name: SlientAccess_InsertEx_Test_002
 * @tc.desc: Test the InsertEx operation of the DataShareHelper base class (not the silent access subclass) with
 *           valid test data, verifying the returned (error code, row ID) pair.
 * @tc.type: FUNC
 * @tc.require: NA
 * @tc.precon:
    1. The global silent access helper (g_slientAccessHelper) is pre-initialized and non-null (to call base class
       method).
    2. Predefined constants SLIENT_ACCESS_URI, TBL_STU_NAME, and TBL_STU_AGE are valid for data preparation.
    3. The base class method DataShareHelper::InsertEx is accessible and returns a (error code, row ID) pair.
 * @tc.step:
    1. Create a DataShare::DataShareValuesBucket (valuesBucket) and add test data: TBL_STU_NAME = "lisi" and
       TBL_STU_AGE = 25.
    2. Create a Uri instance using SLIENT_ACCESS_URI.
    3. Call the base class method g_slientAccessHelper->DataShareHelper::InsertEx(uri, valuesBucket).
    4. Record the returned (error code, row ID) pair and compare it with the expected value.
 * @tc.expect:
    1. The base class InsertEx method returns a pair (0, 0).
 */
HWTEST_F(SlientAccessTest, SlientAccess_InsertEx_Test_002, TestSize.Level0)
{
    LOG_INFO("SilentAccess_InsertEx_Test_002::Start");
    ASSERT_NE(g_slientAccessHelper, nullptr);
    auto helper = g_slientAccessHelper;
    Uri uri(SLIENT_ACCESS_URI);
    DataShare::DataShareValuesBucket valuesBucket;
    std::string value = "lisi";
    valuesBucket.Put(TBL_STU_NAME, value);
    int age = 25;
    valuesBucket.Put(TBL_STU_AGE, age);

    EXPECT_EQ(helper->DataShareHelper::InsertEx(uri, valuesBucket), std::make_pair(0, 0));
    LOG_INFO("SilentAccess_InsertEx_Test_002::End");
}

/**
 * @tc.name: SlientAccess_UpdateEx_Test_002
 * @tc.desc: Test the UpdateEx operation of the DataShareHelper base class (not the silent access subclass) with
 *           valid data and predicates, verifying the returned (error code, affected rows) pair.
 * @tc.type: FUNC
 * @tc.require: NA
 * @tc.precon:
    1. The global helper (g_slientAccessHelper) is pre-initialized, enabling calls to the base class method.
    2. Predefined constants SLIENT_ACCESS_URI, TBL_STU_NAME, and TBL_STU_AGE are valid for data and predicate
       preparation.
    3. The base class method DataShareHelper::UpdateEx is accessible and returns a (error code, affected rows) pair.
 * @tc.step:
    1. Create a DataShareValuesBucket (valuesBucket) and set TBL_STU_AGE = 50 via Put().
    2. Create a DataSharePredicates instance and set the filter to TBL_STU_NAME + " = 'lisi'".
    3. Create a Uri instance using SLIENT_ACCESS_URI.
    4. Call the base class method g_slientAccessHelper->DataShareHelper::UpdateEx(uri, predicates, valuesBucket).
    5. Compare the returned pair with the expected value.
 * @tc.expect:
    1. The base class UpdateEx method returns a pair (0, 0).
 */
HWTEST_F(SlientAccessTest, SlientAccess_UpdateEx_Test_002, TestSize.Level0)
{
    LOG_INFO("SilentAccess_UpdateEx_Test_002::Start");
    ASSERT_NE(g_slientAccessHelper, nullptr);
    auto helper = g_slientAccessHelper;
    Uri uri(SLIENT_ACCESS_URI);
    DataShare::DataShareValuesBucket valuesBucket;
    int value = 50;
    valuesBucket.Put(TBL_STU_AGE, value);
    DataShare::DataSharePredicates predicates;
    std::string selections = TBL_STU_NAME + " = 'lisi'";
    predicates.SetWhereClause(selections);

    EXPECT_EQ(helper->DataShareHelper::UpdateEx(uri, predicates, valuesBucket), std::make_pair(0, 0));
    LOG_INFO("SilentAccess_UpdateEx_Test_002::End");
}

/**
 * @tc.name: SlientAccess_DeleteEx_Test_002
 * @tc.desc: Test the DeleteEx operation of the DataShareHelper base class (not the silent access subclass) with
 *           valid predicates, verifying the returned (error code, deleted rows) pair.
 * @tc.type: FUNC
 * @tc.require: NA
 * @tc.precon:
    1. The global helper (g_slientAccessHelper) is pre-initialized, allowing calls to the base class method.
    2. Predefined constants SLIENT_ACCESS_URI and TBL_STU_NAME are valid for predicate preparation.
    3. The base class method DataShareHelper::DeleteEx is accessible and returns a (error code, deleted rows) pair.
 * @tc.step:
    1. Create a DataShare::DataSharePredicates instance and set the filter to TBL_STU_NAME + " = 'lisi'".
    2. Create a Uri instance using SLIENT_ACCESS_URI.
    3. Call the base class method g_slientAccessHelper->DataShareHelper::DeleteEx(uri, deletePredicates).
    4. Record the returned (error code, deleted rows) pair and compare it with expectations.
 * @tc.expect:
    1. The base class DeleteEx method returns a pair (0, 0).
 */
HWTEST_F(SlientAccessTest, SlientAccess_DeleteEx_Test_002, TestSize.Level0)
{
    LOG_INFO("SilentAccess_DeleteEx_Test_002::Start");
    ASSERT_NE(g_slientAccessHelper, nullptr);
    auto helper = g_slientAccessHelper;
    Uri uri(SLIENT_ACCESS_URI);
    DataShare::DataSharePredicates deletePredicates;
    std::string selections = TBL_STU_NAME + " = 'lisi'";
    deletePredicates.SetWhereClause(selections);

    EXPECT_EQ(helper->DataShareHelper::DeleteEx(uri, deletePredicates), std::make_pair(0, 0));
    LOG_INFO("SilentAccess_DeleteEx_Test_002::End");
}

/**
 * @tc.name: SlientAccess_Permission_Insert_Test_002
 * @tc.desc: Test the Insert operation of the silent access DataShareHelper without the required permissions,
 *           verifying it fails with the expected error code.
 * @tc.type: FUNC
 * @tc.require: NA
 * @tc.precon:
    1. The test environment supports AccessTokenKit methods (AllocHapToken, GetHapTokenIDEx, DeleteToken) and the
       SetSelfTokenID function.
    2. The CreateDataShareHelper function can generate a DataShareHelper instance with STORAGE_MANAGER_MANAGER_ID and
       SLIENT_ACCESS_URI.
    3. SLIENT_ACCESS_PERMISSION1_URI requires specific permissions not granted to the HAP token; the Insert method
       returns -2 on permission failure.
    4. Predefined constants USER_100, APL_SYSTEM_CORE, PermissionState::PERMISSION_GRANTED, and TBL_STU columns are
       valid.
 * @tc.step:
    1. Initialize HapInfoParams (info) with USER_100, bundle name "ohos.permission.write.demo", and other required
       fields.
    2. Initialize HapPolicyParams (policy) with APL_SYSTEM_CORE, granting only "ohos.permission.WRITE_CONTACTS" (no
       required permissions for the target URI).
    3. Call AccessTokenKit::AllocHapToken(info, policy) to allocate the limited-permission token; get the token ID via
       GetHapTokenIDEx.
    4. Call SetSelfTokenID(testTokenId.tokenIDEx) to set the token as the current process's token.
    5. Create a DataShareHelper instance via CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID, SLIENT_ACCESS_URI).
    6. Prepare a DataShareValuesBucket with TBL_STU_NAME = "lisi" and TBL_STU_AGE = 25; call Insert on
       SLIENT_ACCESS_PERMISSION1_URI, record retVal.
    7. Set the helper to nullptr, then call AccessTokenKit::DeleteToken(testTokenId.tokenIDEx) to clean up.
    8. Check the value of retVal against the expected error code.
 * @tc.expect:
    1. The Insert operation fails and returns retVal = -2 (permission denied).
 */
HWTEST_F(SlientAccessTest, SlientAccess_Permission_Insert_Test_002, TestSize.Level0)
{
    LOG_INFO("SlientAccess_Permission_Insert_Test_002::Start");
    HapInfoParams info = {
        .userID = USER_100,
        .bundleName = "ohos.permission.write.demo",
        .instIndex = 0,
        .isSystemApp = true,
        .apiVersion = 8,
        .appIDDesc = "ohos.permission.write.demo"
    };
    HapPolicyParams policy = {
        .apl = APL_SYSTEM_CORE,
        .domain = "test.domain",
        .permStateList = {
            {
                .permissionName = "ohos.permission.WRITE_CONTACTS",
                .isGeneral = true,
                .resDeviceID = { "local" },
                .grantStatus = { PermissionState::PERMISSION_GRANTED },
                .grantFlags = { 1 }
            }
        }
    };
    AccessTokenIDEx tokenIdEx = MockToken::AllocTestHapToken(info, policy);
    uint64_t token = tokenIdEx.tokenIdExStruct.tokenID;
    ASSERT_NE(token, INVALID_TOKENID);
    ASSERT_EQ(E_OK, SetSelfTokenID(token));

    auto helper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID, SLIENT_ACCESS_URI);
    ASSERT_NE(helper, nullptr);
    Uri uri(SLIENT_ACCESS_PERMISSION1_URI);
    DataShare::DataShareValuesBucket valuesBucket;
    std::string value = "lisi";
    valuesBucket.Put(TBL_STU_NAME, value);
    int age = 25;
    valuesBucket.Put(TBL_STU_AGE, age);
    int retVal = helper->Insert(uri, valuesBucket);
    EXPECT_EQ(retVal, -2);
    helper = nullptr;
    ASSERT_EQ(E_OK, MockToken::DeleteTestHapToken(token));
    LOG_INFO("SlientAccess_Permission_Insert_Test_002::End");
}

/**
 * @tc.name: SlientAccess_Permission_Update_Test_002
 * @tc.desc: Test the Update operation of the silent access DataShareHelper without the required permissions,
 *           verifying it fails with the expected error code.
 * @tc.type: FUNC
 * @tc.require: NA
 * @tc.precon:
    1. The test environment supports AccessTokenKit operations (AllocHapToken, GetHapTokenIDEx, DeleteToken) and
       SetSelfTokenID.
    2. CreateDataShareHelper can generate a valid instance; SLIENT_ACCESS_PERMISSION1_URI requires permissions not
       granted to the HAP token.
    3. The Update method returns -2 on permission failure; there is a target record (TBL_STU_NAME = "lisi") in the data
       source.
    4. Predefined constants for HAP params, APL, and TBL_STU columns are valid.
 * @tc.step:
    1. Initialize HapInfoParams (info) and HapPolicyParams (policy) with limited permissions (only
       "ohos.permission.WRITE_CONTACTS" granted).
    2. Allocate the HAP token via AccessTokenKit::AllocHapToken, get the token ID, and set it as the current process
       token with SetSelfTokenID.
    3. Create a DataShareHelper instance using CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID, SLIENT_ACCESS_URI).
    4. Prepare a DataShareValuesBucket (TBL_STU_AGE = 50) and predicates (filter TBL_STU_NAME = "lisi").
    5. Call helper->Update() on SLIENT_ACCESS_PERMISSION1_URI with the bucket and predicates; record retVal.
    6. Clean up: set helper to nullptr and delete the token via AccessTokenKit::DeleteToken.
    7. Check if retVal matches the expected permission error code.
 * @tc.expect:
    1. The Update operation fails and returns retVal = -2 (permission denied).
 */
HWTEST_F(SlientAccessTest, SlientAccess_Permission_Update_Test_002, TestSize.Level0)
{
    LOG_INFO("SlientAccess_Permission_Update_Test_002::Start");
    HapInfoParams info = {
        .userID = USER_100,
        .bundleName = "ohos.permission.write.demo",
        .instIndex = 0,
        .isSystemApp = true,
        .apiVersion = 8,
        .appIDDesc = "ohos.permission.write.demo"
    };
    HapPolicyParams policy = {
        .apl = APL_SYSTEM_CORE,
        .domain = "test.domain",
        .permStateList = {
            {
                .permissionName = "ohos.permission.WRITE_CONTACTS",
                .isGeneral = true,
                .resDeviceID = { "local" },
                .grantStatus = { PermissionState::PERMISSION_GRANTED },
                .grantFlags = { 1 }
            }
        }
    };
    AccessTokenIDEx tokenIdEx = MockToken::AllocTestHapToken(info, policy);
    uint64_t token = tokenIdEx.tokenIdExStruct.tokenID;
    ASSERT_NE(token, INVALID_TOKENID);
    ASSERT_EQ(E_OK, SetSelfTokenID(token));

    auto helper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID, SLIENT_ACCESS_URI);
    Uri uri(SLIENT_ACCESS_PERMISSION1_URI);
    DataShare::DataShareValuesBucket valuesBucket;
    int value = 50;
    valuesBucket.Put(TBL_STU_AGE, value);
    DataShare::DataSharePredicates predicates;
    std::string selections = TBL_STU_NAME + " = 'lisi'";
    predicates.SetWhereClause(selections);
    int retVal = helper->Update(uri, predicates, valuesBucket);
    EXPECT_EQ(retVal, -2);
    helper = nullptr;
    ASSERT_EQ(E_OK, MockToken::DeleteTestHapToken(token));
    LOG_INFO("SlientAccess_Permission_Update_Test_002::End");
}

/**
 * @tc.name: SlientAccess_Permission_Query_Test_001
 * @tc.desc: Test the Query operation of the silent access DataShareHelper without the required permissions (only
 *           WRITE_CONTACTS granted), verifying it fails with the expected result set and error code.
 * @tc.type: FUNC
 * @tc.require: NA
 * @tc.precon:
    1. The test environment supports AccessTokenKit, SetSelfTokenID, and CreateDataShareHelper;
       DatashareBusinessError::GetCode() works.
    2. SLIENT_ACCESS_PERMISSION2_URI requires read permissions not granted to the HAP token; the Query method returns
       nullptr on failure.
    3. Predefined constants for HAP params, token operations, and TBL_STU_NAME are valid.
 * @tc.step:
    1. Initialize HapInfoParams (info) and HapPolicyParams (policy) to grant only "ohos.permission.WRITE_CONTACTS" (no
       read permissions).
    2. Allocate the HAP token, get its ID, and set it as the current process token with SetSelfTokenID.
    3. Create a DataShareHelper instance via CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID, SLIENT_ACCESS_URI).
    4. Create a Uri (SLIENT_ACCESS_PERMISSION2_URI), predicates (EqualTo(TBL_STU_NAME, "lisi")), empty columns, and
       DatashareBusinessError (businessError).
    5. Call helper->Query(uri, predicates, columns, &businessError) and record the ResultSet (resultSet).
    6. Clean up: set helper to nullptr and delete the token; check resultSet and businessError.GetCode().
 * @tc.expect:
    1. The Query operation returns a nullptr ResultSet.
    2. The DatashareBusinessError code (from GetCode()) is -2 (permission denied).
 */
HWTEST_F(SlientAccessTest, SlientAccess_Permission_Query_Test_001, TestSize.Level0)
{
    LOG_INFO("SlientAccess_Permission_Query_Test_001::Start");
    HapInfoParams info = {
        .userID = USER_100,
        .bundleName = "ohos.permission.write.demo",
        .instIndex = 0,
        .isSystemApp = true,
        .apiVersion = 8,
        .appIDDesc = "ohos.permission.write.demo"
    };
    HapPolicyParams policy = {
        .apl = APL_SYSTEM_CORE,
        .domain = "test.domain",
        .permStateList = {
            {
                .permissionName = "ohos.permission.WRITE_CONTACTS",
                .isGeneral = true,
                .resDeviceID = { "local" },
                .grantStatus = { PermissionState::PERMISSION_GRANTED },
                .grantFlags = { 1 }
            }
        }
    };
    AccessTokenIDEx tokenIdEx = MockToken::AllocTestHapToken(info, policy);
    uint64_t token = tokenIdEx.tokenIdExStruct.tokenID;
    ASSERT_NE(token, INVALID_TOKENID);
    ASSERT_EQ(E_OK, SetSelfTokenID(token));

    auto helper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID, SLIENT_ACCESS_URI);
    Uri uri(SLIENT_ACCESS_PERMISSION2_URI);
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(TBL_STU_NAME, "lisi");
    vector<string> columns;
    DatashareBusinessError businessError;
    auto resultSet = helper->Query(uri, predicates, columns, &businessError);
    EXPECT_EQ(resultSet, nullptr);
    EXPECT_EQ(businessError.GetCode(), -2);
    helper = nullptr;
    ASSERT_EQ(E_OK, MockToken::DeleteTestHapToken(token));
    LOG_INFO("SlientAccess_Permission_Query_Test_001::End");
}

/**
 * @tc.name: SlientAccess_Access_When_Uri_Error_Test_001
 * @tc.desc: Test the Insert operation of the silent access DataShareHelper with an invalid URI (SLIENT_ERROR_URI),
 *           verifying it fails with a negative return value.
 * @tc.type: FUNC
 * @tc.require: NA
 * @tc.precon:
    1. The global silent access helper (g_slientAccessHelper) is pre-initialized and non-null.
    2. SLIENT_ERROR_URI is a predefined invalid URI that cannot be resolved to any valid data source.
    3. The Insert method returns a negative integer when the URI is invalid; TBL_STU_NAME and TBL_STU_AGE are valid
       columns.
 * @tc.step:
    1. Create a DataShare::DataShareValuesBucket (valuesBucket) and add test data: TBL_STU_NAME = "lisi" and
       TBL_STU_AGE = 25.
    2. Create a Uri instance using SLIENT_ERROR_URI (invalid URI).
    3. Call g_slientAccessHelper->Insert(uri, valuesBucket) and record the returned integer (retVal).
    4. Check whether retVal meets the expected condition of being less than 0.
 * @tc.expect:
    1. The Insert operation fails and returns retVal < 0 (invalid URI).
 */
HWTEST_F(SlientAccessTest, SlientAccess_Access_When_Uri_Error_Test_001, TestSize.Level0)
{
    LOG_INFO("SlientAccess_Permission_Access_When_URI_ERROR_Test_001::Begin");
    ASSERT_NE(g_slientAccessHelper, nullptr);
    auto helper = g_slientAccessHelper;
    Uri uri(SLIENT_ERROR_URI);
    DataShare::DataShareValuesBucket valuesBucket;
    std::string value = "lisi";
    valuesBucket.Put(TBL_STU_NAME, value);
    int age = 25;
    valuesBucket.Put(TBL_STU_AGE, age);

    int retVal = helper->Insert(uri, valuesBucket);
    EXPECT_EQ((retVal < 0), true);
    LOG_INFO("SlientAccess_Permission_Access_When_URI_ERROR_Test_001::End");
}

/**
 * @tc.name: SlientAccess_Access_With_Uncreated_DataBase_Test_001
 * @tc.desc: Test the Insert operation of the silent access DataShareHelper with a URI pointing to an uncreated
 *           database (SLIENT_ERROR_DATABASE_URI), verifying it fails with a negative return value.
 * @tc.type: FUNC
 * @tc.require: NA
 * @tc.precon:
    1. The global silent access helper (g_slientAccessHelper) is pre-initialized and non-null.
    2. SLIENT_ERROR_DATABASE_URI points to a database that has not been created (no corresponding data source exists).
    3. The Insert method returns a negative integer when accessing an uncreated database; TBL_STU columns are valid.
 * @tc.step:
    1. Create a DataShare::DataShareValuesBucket (valuesBucket) and add test data: TBL_STU_NAME = "lisi" and
       TBL_STU_AGE = 25.
    2. Create a Uri instance using SLIENT_ERROR_DATABASE_URI (points to uncreated database).
    3. Call g_slientAccessHelper->Insert(uri, valuesBucket) and record the returned integer (retVal).
    4. Check whether retVal meets the expected condition of being less than 0.
 * @tc.expect:
    1. The Insert operation fails and returns retVal < 0 (uncreated database).
 */
HWTEST_F(SlientAccessTest, SlientAccess_Access_With_Uncreated_DataBase_Test_001, TestSize.Level0)
{
    LOG_INFO("SlientAccess_Access_With_Uncreated_DataBase_Test_001::Begin");
    ASSERT_NE(g_slientAccessHelper, nullptr);
    auto helper = g_slientAccessHelper;
    Uri uri(SLIENT_ERROR_DATABASE_URI);
    DataShare::DataShareValuesBucket valuesBucket;
    std::string value = "lisi";
    valuesBucket.Put(TBL_STU_NAME, value);
    int age = 25;
    valuesBucket.Put(TBL_STU_AGE, age);

    int retVal = helper->Insert(uri, valuesBucket);
    EXPECT_EQ((retVal < 0), true);
    LOG_INFO("SlientAccess_Access_With_Uncreated_DataBase_Test_001::End");
}

/**
 * @tc.name: SlientAccess_Creator_With_Uri_Error_Test_001
 * @tc.desc: Test the creation of DataShareHelper via the Creator method with an invalid URI (DATA_SHARE_ERROR_URI),
 *           verifying the helper instance is null (creation fails).
 * @tc.type: FUNC
 * @tc.require: NA
 * @tc.precon:
    1. The test environment supports obtaining a valid SystemAbilityManager instance via
       SystemAbilityManagerClient::GetInstance().
    2. The SystemAbilityManager can retrieve a valid remote object for STORAGE_MANAGER_MANAGER_ID.
    3. DATA_SHARE_ERROR_URI is a predefined invalid URI; the DataShareHelper::Creator method returns nullptr for
       invalid URIs.
    4. The Creator method accepts remote object, two URI strings, and an integer parameter (2 in the test).
 * @tc.step:
    1. Call SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager() to get the SystemAbilityManager
       instance (saManager).
    2. Use saManager->GetSystemAbility(STORAGE_MANAGER_MANAGER_ID) to retrieve the valid remote object for the system
       ability.
    3. Define a std::string (uriStr) using DATA_SHARE_ERROR_URI (invalid URI).
    4. Call DataShare::DataShareHelper::Creator(remoteObj, uriStr, uriStr, 2) to attempt creating the helper instance.
    5. Check whether the returned helper instance is null.
 * @tc.expect:
    1. The DataShareHelper creation fails, and the returned helper instance is nullptr.
 */
HWTEST_F(SlientAccessTest, SlientAccess_Creator_With_Uri_Error_Test_001, TestSize.Level0)
{
    LOG_INFO("SlientAccess_Creator_With_Uri_Error_Test_001::Begin");
    auto saManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (saManager == nullptr) {
        LOG_ERROR("GetSystemAbilityManager get samgr failed.");
    }
    auto remoteObj = saManager->GetSystemAbility(STORAGE_MANAGER_MANAGER_ID);
    if (remoteObj == nullptr) {
        LOG_ERROR("GetSystemAbility service failed.");
    }
    std::string uriStr(DATA_SHARE_ERROR_URI);
    auto helper = DataShare::DataShareHelper::Creator(remoteObj, uriStr, uriStr, 2);
    EXPECT_EQ(helper, nullptr);
    LOG_INFO("SlientAccess_Creator_With_Uri_Error_Test_001::End");
}

/**
 * @tc.name: SlientAccess_UserDefineFunc_Test_001
 * @tc.desc: Test the UserDefineFunc operation of the DataShareHelper base class (called via the silent access helper),
 *           verifying it succeeds with the expected return code.
 * @tc.type: FUNC
 * @tc.require: NA
 * @tc.precon:
    1. The global silent access helper (g_slientAccessHelper) is pre-initialized and non-null, allowing calls to the
       base class method.
    2. The test environment supports instantiation of empty MessageParcel (for data and reply) and default
       MessageOption.
    3. The base class method DataShareHelper::UserDefineFunc returns 0 on successful execution.
 * @tc.step:
    1. Create two empty MessageParcel instances: one for input data (data) and one for reply (reply).
    2. Create a MessageOption instance (option) with default initialization.
    3. Call g_slientAccessHelper->DataShareHelper::UserDefineFunc(data, reply, option) and record the returned result.
    4. Check whether the returned result matches the expected success code.
 * @tc.expect:
    1. The UserDefineFunc operation succeeds and returns result = 0.
 */
HWTEST_F(SlientAccessTest, SlientAccess_UserDefineFunc_Test_001, TestSize.Level0)
{
    LOG_INFO("SilentAccess_UserDefineFunc_Test_001::Start");
    ASSERT_NE(g_slientAccessHelper, nullptr);
    auto helper = g_slientAccessHelper;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto result = helper->DataShareHelper::UserDefineFunc(data, reply, option);
    EXPECT_EQ(result, 0);
    LOG_INFO("SilentAccess_UserDefineFunc_Test_001::End");
}

/**
 * @tc.name: SlientAccess_Creator_ErrorBundle_Test_001
 * @tc.desc: Test the creation process of DataShareHelper using a URI with an invalid bundle name
 *           ("com.acts.error.bundleName"), verifying that the helper fails to create as expected.
 * @tc.type: FUNC
 * @tc.require: NA
 * @tc.precon:
    1. The CreateDataShareHelper function is available; it accepts STORAGE_MANAGER_MANAGER_ID and a URI string to
       create a DataShareHelper instance.
    2. The predefined constant STORAGE_MANAGER_MANAGER_ID is valid and accessible for helper creation.
    3. The URI string with the invalid bundle name ("datashareproxy://com.acts.error.bundleName/test") is correctly
       formatted but references a non-existent bundle.
 * @tc.step:
    1. Define a URI string with an invalid bundle name: "datashareproxy://com.acts.error.bundleName/test".
    2. Call CreateDataShareHelper with STORAGE_MANAGER_MANAGER_ID and the invalid URI to attempt creating a
       DataShareHelper.
    3. Assign the returned value to a DataShareHelper pointer (helperSilent) and check its value.
 * @tc.expect:
    1. The DataShareHelper creation fails; the returned helperSilent pointer is nullptr.
 */
HWTEST_F(SlientAccessTest, SlientAccess_Creator_ErrorBundle_Test_001, TestSize.Level0)
{
    LOG_INFO("SlientAccess_Creator_ErrorBundle_Test_001::Start");
    std::string uriStr1("datashareproxy://com.acts.error.bundleName/test");
    // slientUri is error bundle name, slient access can't find the bundle name, return nullptr
    auto helperSilent = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID, uriStr1);
    EXPECT_EQ(helperSilent, nullptr);
    LOG_INFO("SlientAccess_Creator_ErrorBundle_Test_001::End");
}

/**
 * @tc.name: SlientAccess_Creator_ErrorBundle_ExtSuccess_Test_001
 * @tc.desc: Test the creation of DataShareHelper using an invalid silent URI (with wrong bundle name) but a valid ext
 *           URI, verifying that the helper creation succeeds due to the valid ext URI.
 * @tc.type: FUNC
 * @tc.require: NA
 * @tc.precon:
    1. The SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager() method can obtain a valid
       SystemAbilityManager instance.
    2. The SystemAbilityManager's GetSystemAbility method can retrieve a non-null remote object for
       STORAGE_MANAGER_MANAGER_ID.
    3. Predefined constants SLIENT_ERROR_URI (invalid silent URI) and DATA_SHARE_URI (valid ext URI) are correctly
       defined.
    4. The DataShare::DataShareHelper::Create method accepts a remote object, silent URI, and ext URI, returning a pair
       of (error code, helper).
 * @tc.step:
    1. Get the SystemAbilityManager instance via SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager().
    2. Call the SystemAbilityManager's GetSystemAbility method with STORAGE_MANAGER_MANAGER_ID to obtain a remote
       object.
    3. Define the invalid silent URI as SLIENT_ERROR_URI and the valid ext URI as DATA_SHARE_URI.
    4. Call DataShare::DataShareHelper::Create with the remote object, invalid silent URI, and valid ext URI.
    5. Extract the return code (ret) and helper instance from the returned pair, then check both values.
 * @tc.expect:
    1. The return code (ret) from DataShare::DataShareHelper::Create is DataShare::E_OK.
    2. The returned DataShareHelper instance is not nullptr (creation succeeds due to valid ext URI).
 */
HWTEST_F(SlientAccessTest, SlientAccess_Creator_ErrorBundle_ExtSuccess_Test_001, TestSize.Level0)
{
    LOG_INFO("SlientAccess_Creator_ErrorBundle_ExtSuccess_Test_001::Start");
    std::string uriStr1(SLIENT_ERROR_URI);
    std::string uriStr2 (DATA_SHARE_URI);
    auto saManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (saManager == nullptr) {
        LOG_ERROR("GetSystemAbilityManager get samgr failed.");
    }
    auto remoteObj = saManager->GetSystemAbility(STORAGE_MANAGER_MANAGER_ID);
    if (remoteObj == nullptr) {
        LOG_ERROR("GetSystemAbility service failed.");
    }
    // slientUri is error bundleName, extUri is effective, slient access can't find the bundleName, but ext success
    auto [ret, helper] = DataShare::DataShareHelper::Create(remoteObj, uriStr1, uriStr2);
    EXPECT_EQ(ret, DataShare::E_OK);
    EXPECT_NE(helper, nullptr);
    helper = nullptr;
    LOG_INFO("SlientAccess_Creator_ErrorBundle_ExtSuccess_Test_001::End");
}

/**
 * @tc.name: SlientAccess_Create_With_Invalid_AppIndex_Test_001
 * @tc.desc: Test DataShareHelper creation using a URI with an invalid appIndex ("appIndex=-1") via three methods:
 *           DataShare::DataShareHelper::Create, and two overloaded DataShare::DataShareHelper::Creator methods.
 * @tc.type: FUNC
 * @tc.require: NA
 * @tc.precon:
    1. The SystemAbilityManager can be obtained and can return a valid remote object for STORAGE_MANAGER_MANAGER_ID.
    2. The test URI is predefined with invalid appIndex:
       "datashareproxy://com.acts.datasharetest/test?Proxy=true&appIndex=-1".
    3. The CreateOptions structure is available and supports setting the isProxy_ member to true.
    4. Three helper creation methods (Create, Creator with remoteObj+URI, Creator with URI+CreateOptions) are
       accessible.
 * @tc.step:
    1. Define the test URI with invalid appIndex and set DATA_SHARE_URI as the ext URI.
    2. Get the SystemAbilityManager instance and retrieve the remote object for STORAGE_MANAGER_MANAGER_ID.
    3. Call DataShare::DataShareHelper::Create with remoteObj, test URI, and empty ext URI; record (ret, helper).
    4. Call DataShare::DataShareHelper::Creator with remoteObj and test URI; record the returned helper.
    5. Initialize CreateOptions (set isProxy_ to true), then call Creator with test URI and options; record the helper.
    6. Check the return code from step 3 and the helper instances from steps 3-5.
 * @tc.expect:
    1. The Create method returns E_EXT_URI_INVALID as the error code and a nullptr helper.
    2. The Creator method (with remoteObj+URI) returns a nullptr helper.
    3. The Creator method (with URI+CreateOptions) returns a nullptr helper.
 */
HWTEST_F(SlientAccessTest, SlientAccess_Create_With_Invalid_AppIndex_Test_001, TestSize.Level0)
{
    LOG_INFO("SlientAccess_Create_With_Invalid_AppIndex_Test_001::Start");
    std::string uri("datashareproxy://com.acts.datasharetest/test?Proxy=true&appIndex=-1");
    std::string extUri(DATA_SHARE_URI);
    auto saManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (saManager == nullptr) {
        LOG_ERROR("GetSystemAbilityManager get samgr failed.");
    }
    auto remoteObj = saManager->GetSystemAbility(STORAGE_MANAGER_MANAGER_ID);
    if (remoteObj == nullptr) {
        LOG_ERROR("GetSystemAbility service failed.");
    }
    auto [ret, helper] = DataShare::DataShareHelper::Create(remoteObj, uri, "");
    EXPECT_EQ(ret, E_EXT_URI_INVALID);
    EXPECT_EQ(helper, nullptr);

    helper = DataShare::DataShareHelper::Creator(remoteObj, uri);
    EXPECT_EQ(helper, nullptr);

    CreateOptions options;
    options.isProxy_ = true;
    helper = DataShare::DataShareHelper::Creator(uri, options);
    EXPECT_EQ(helper, nullptr);
    LOG_INFO("SlientAccess_Create_With_Invalid_AppIndex_Test_001::End");
}

/**
 * @tc.name: SlientAccess_RegisterObserverExtProvider_Test_001
 * @tc.desc: Test the RegisterObserverExtProvider method using an invalid URI (SLIENT_ERROR_URI), covering the code
 *           branch where the generalCtl_ member is nullptr, verifying no crashes and normal execution.
 * @tc.type: FUNC
 * @tc.require: NA
 * @tc.precon:
    1. The global DataShareHelper instance g_slientAccessHelper is pre-initialized and not nullptr.
    2. The DataShareObserverTest class can be instantiated as a shared pointer (std::shared_ptr<DataShareObserver>).
    3. The predefined constant SLIENT_ERROR_URI is a valid invalid URI string for testing.
    4. The RegisterObserverExtProvider method accepts a Uri, shared_ptr<DataShareObserver>, and bool as parameters.
 * @tc.step:
    1. Create a Uri instance using the SLIENT_ERROR_URI constant.
    2. Instantiate a DataShareObserverTest as a std::shared_ptr<DataShareObserver> (dataObserver).
    3. Verify that g_slientAccessHelper and dataObserver are both not nullptr.
    4. Call the RegisterObserverExtProvider method of g_slientAccessHelper with the invalid Uri, dataObserver, and
       false.
 * @tc.expect:
    1. No crashes or unexpected exceptions occur during the method call.
    2. The RegisterObserverExtProvider method executes normally without errors.
 */
HWTEST_F(SlientAccessTest, SlientAccess_RegisterObserverExtProvider_Test_001, TestSize.Level0)
{
    LOG_INFO("SlientAccess_RegisterObserverExtProvider_Test_001::Begin");
    ASSERT_NE(g_slientAccessHelper, nullptr);
    auto helper = g_slientAccessHelper;
    Uri uri(SLIENT_ERROR_URI);
    std::shared_ptr<DataShareObserver> dataObserver = std::make_shared<DataShareObserverTest>();
    ASSERT_NE(helper, nullptr);
    ASSERT_NE(dataObserver, nullptr);

    helper->RegisterObserverExtProvider(uri, dataObserver, false);

    LOG_INFO("SlientAccess_RegisterObserverExtProvider_Test_001::End");
}

/**
 * @tc.name: SlientAccess_UnregisterObserverExtProvider_Test_001
 * @tc.desc: Test the UnregisterObserverExtProvider method using an invalid URI (SLIENT_ERROR_URI), covering the code
 *           branch where generalCtl_ is nullptr, and verify no crashes and normal execution.
 * @tc.type: FUNC
 * @tc.require: NA
 * @tc.precon:
    1. The global DataShareHelper instance g_slientAccessHelper is pre-initialized and not nullptr.
    2. The DataShareObserverTest class can be instantiated as a std::shared_ptr<DataShareObserver>.
    3. The SLIENT_ERROR_URI constant is valid for creating an invalid Uri; the ChangeInfo structure is accessible.
    4. The UnregisterObserverExtProvider method accepts a Uri and shared_ptr<DataShareObserver> as parameters.
 * @tc.step:
    1. Create a Uri instance using the SLIENT_ERROR_URI constant.
    2. Instantiate a DataShareObserverTest as a std::shared_ptr<DataShareObserver> (dataObserver).
    3. Verify that g_slientAccessHelper and dataObserver are both not nullptr.
    4. Call the UnregisterObserverExtProvider method of g_slientAccessHelper with the invalid Uri and dataObserver.
    5. Create a dummy ChangeInfo object (set ChangeType to INSERT and include the invalid Uri in the URI list).
 * @tc.expect:
    1. No crashes or unexpected exceptions occur during the UnregisterObserverExtProvider call.
    2. The method executes normally, and creating the dummy ChangeInfo object does not cause errors.
 */
HWTEST_F(SlientAccessTest, SlientAccess_UnregisterObserverExtProvider_Test_001, TestSize.Level0)
{
    LOG_INFO("SlientAccess_UnregisterObserverExtProvider_Test_001::Begin");
    ASSERT_NE(g_slientAccessHelper, nullptr);
    auto helper = g_slientAccessHelper;
    Uri uri(SLIENT_ERROR_URI);
    std::shared_ptr<DataShareObserver> dataObserver = std::make_shared<DataShareObserverTest>();
    ASSERT_NE(helper, nullptr);
    ASSERT_NE(dataObserver, nullptr);

    helper->UnregisterObserverExtProvider(uri, dataObserver);

    ChangeInfo changeInfo = { DataShareObserver::ChangeType::INSERT, { uri } };
    LOG_INFO("SlientAccess_UnregisterObserverExtProvider_Test_001::End");
}

/**
 * @tc.name: SlientAccess_NotifyChangeExtProvider_Test_001
 * @tc.desc: Test the NotifyChangeExtProvider method using an invalid URI (SLIENT_ERROR_URI) (after registering an
 *           observer), covering the generalCtl_ == nullptr branch, verifying no crashes and normal notification
 *           process.
 * @tc.type: FUNC
 * @tc.require: NA
 * @tc.precon:
    1. The global DataShareHelper instance g_slientAccessHelper is pre-initialized and not nullptr.
    2. DataShareObserverTest can be instantiated as a shared pointer; RegisterObserverExtProvider works for invalid
       URIs.
    3. The ChangeInfo structure supports setting ChangeType and a list of Uris; SLIENT_ERROR_URI is valid.
    4. The NotifyChangeExtProvider method accepts a ChangeInfo object as a parameter.
 * @tc.step:
    1. Create a Uri instance using the SLIENT_ERROR_URI constant.
    2. Instantiate a DataShareObserverTest as a std::shared_ptr<DataShareObserver> (dataObserver).
    3. Verify that g_slientAccessHelper and dataObserver are not nullptr.
    4. Call g_slientAccessHelper->RegisterObserverExtProvider with the invalid Uri, dataObserver, and true.
    5. Create a ChangeInfo object: set ChangeType to DataShareObserver::ChangeType::INSERT, and include the invalid Uri
       in its URI list.
    6. Call g_slientAccessHelper->NotifyChangeExtProvider with the created ChangeInfo.
 * @tc.expect:
    1. No crashes or unexpected exceptions occur during registration and notification.
    2. The NotifyChangeExtProvider method executes normally without errors.
 */
HWTEST_F(SlientAccessTest, SlientAccess_NotifyChangeExtProvider_Test_001, TestSize.Level0)
{
    LOG_INFO("SlientAccess_NotifyChangeExtProvider_Test_001::Begin");
    ASSERT_NE(g_slientAccessHelper, nullptr);
    auto helper = g_slientAccessHelper;
    Uri uri(SLIENT_ERROR_URI);
    std::shared_ptr<DataShareObserver> dataObserver = std::make_shared<DataShareObserverTest>();
    ASSERT_NE(helper, nullptr);
    ASSERT_NE(dataObserver, nullptr);

    helper->RegisterObserverExtProvider(uri, dataObserver, true);

    ChangeInfo changeInfo = { DataShareObserver::ChangeType::INSERT, { uri } };
    helper->NotifyChangeExtProvider(changeInfo);

    LOG_INFO("SlientAccess_NotifyChangeExtProvider_Test_001::End");
}
} // namespace DataShare
} // namespace OHOS