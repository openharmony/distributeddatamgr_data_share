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
#define LOG_TAG "datashare_helper_impl_register_test"

#include "datashare_helper_impl.h"

#include <gtest/gtest.h>
#include <memory>

#include "accesstoken_kit.h"
#include "datashare_helper.h"
#include "datashare_log.h"
#include "datashare_valuebucket_convert.h"
#include "dataobs_mgr_errors.h"
#include "gmock/gmock.h"
#include "hap_token_info.h"
#include "iservice_registry.h"
#include "token_setproc.h"

namespace OHOS {
namespace DataShare {
using namespace testing::ext;
using namespace OHOS::Security::AccessToken;
static constexpr int STORAGE_MANAGER_MANAGER_ID = 5003;
static constexpr int64_t INTERVAL = 2;
static const std::string DATA_SHARE_URI = "datashare:///com.acts.datasharetest";
static const std::string DATA_SHARE_URI_SILENT = "datashareproxy://com.acts.datasharetest/test?Proxy=true";
static const std::string REGISTER_URI = "datashareproxy://com.acts.datasharetest/test";
static const std::string REGISTER_URI_NOREAD = "datashareproxy://com.acts.datasharetest/noread";
static const std::string REGISTER_URI_NOWRITE = "datashareproxy://com.acts.datasharetest/nowrite";
static const std::string REGISTER_URI_NOALL = "datashareproxy://com.acts.datasharetest/noall";
static const std::string REGISTER_URI_INVALID = "abc";
static const std::string TBL_STU_NAME = "name";
static const std::string TBL_STU_AGE = "age";
std::shared_ptr<DataShare::DataShareHelper> g_dataShareHelper;
std::shared_ptr<DataShare::DataShareHelper> g_dataShareHelperSilent;

class DataShareHelperImplRegisterTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp() {};
    void TearDown() {};
    bool UrisEqual(std::list<Uri> uri1, std::list<Uri> uri2);
    bool ValueBucketEqual(const VBuckets &v1, const VBuckets &v2);
    bool ChangeInfoEqual(const DataShareObserver::ChangeInfo &changeInfo,
        const DataShareObserver::ChangeInfo &expectChangeInfo);
    static std::shared_ptr<DataShareHelperImpl> GetInstance(std::shared_ptr<DataShareHelperImpl> instance = nullptr);
};

std::shared_ptr<DataShare::DataShareHelper> CreateDataShareHelper(int32_t systemAbilityId, const std::string &uri)
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

void DataShareHelperImplRegisterTest::SetUpTestCase(void)
{
    LOG_INFO("SetUpTestCase invoked");
    g_dataShareHelper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID, DATA_SHARE_URI);
    ASSERT_TRUE(g_dataShareHelper != nullptr);
    int sleepTime = 2;
    sleep(sleepTime);
    g_dataShareHelperSilent = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID, DATA_SHARE_URI_SILENT);
    ASSERT_TRUE(g_dataShareHelperSilent != nullptr);

    HapInfoParams info = {
        .userID = 100,
        .bundleName = "ohos.datashareclienttest.demo",
        .instIndex = 0,
        .appIDDesc = "ohos.datashareclienttest.demo"
    };
    auto permStateList = GetPermissionStateFulls();
    HapPolicyParams policy = {
        .apl = APL_NORMAL,
        .domain = "test.domain",
        .permList = {
            {
                .permissionName = "ohos.permission.test",
                .bundleName = "ohos.datashareclienttest.demo",
                .grantMode = 1,
                .availableLevel = APL_NORMAL,
                .label = "label",
                .labelId = 1,
                .description = "ohos.datashareclienttest.demo",
                .descriptionId = 1
            }
        },
        .permStateList = permStateList
    };
    AccessTokenKit::AllocHapToken(info, policy);
    auto testTokenId = Security::AccessToken::AccessTokenKit::GetHapTokenIDEx(
        info.userID, info.bundleName, info.instIndex);
    SetSelfTokenID(testTokenId.tokenIDEx);

    LOG_INFO("SetUpTestCase end");
}

void DataShareHelperImplRegisterTest::TearDownTestCase(void)
{
    LOG_INFO("TearDownTestCase invoked");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    ASSERT_TRUE(g_dataShareHelper != nullptr);
    bool result = helper->Release();
    EXPECT_EQ(result, true);
    LOG_INFO("TearDownTestCase end");
}

template <typename T>
class ConditionLock {
public:
    explicit ConditionLock() {}
    ~ConditionLock() {}
public:
    void Notify(const T &data)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        data_ = data;
        isSet_ = true;
        cv_.notify_one();
    }

    void Notify()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        isSet_ = true;
        cv_.notify_one();
    }

    T Wait()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait_for(lock, std::chrono::seconds(INTERVAL), [this]() { return isSet_; });
        T data = data_;
        cv_.notify_one();
        return data;
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

    void SetName(std::string str)
    {
        this->name = str;
    }
	
    void Clear()
    {
        data.Clear();
    }
    ConditionLock<std::string> data;
private:
    std::string name;
};

class MockDatashareObserver : public DataShare::DataShareObserver {
public:
    MockDatashareObserver() {}
    ~MockDatashareObserver() {}

    void OnChange(const DataShareObserver::ChangeInfo &changeInfo) override
    {
        changeInfo_ = changeInfo;
        data.Notify(changeInfo);
        isNotify = true;
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

    DataShareObserver::ChangeInfo changeInfo_;
    ConditionLock<DataShareObserver::ChangeInfo> data;
    bool isNotify = false;
};

bool DataShareHelperImplRegisterTest::UrisEqual(std::list<Uri> uri1, std::list<Uri> uri2)
{
    if (uri1.size() != uri2.size()) {
        return false;
    }
    auto cmp = [](const Uri &first, const Uri &second) { return first.ToString() < second.ToString(); };
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

bool DataShareHelperImplRegisterTest::ValueBucketEqual(const VBuckets &v1, const VBuckets &v2)
{
    if (v1.size() != v2.size()) {
        return false;
    }
    for (size_t i = 0; i < v1.size(); i++) {
        const VBucket &vb1 = v1[i];
        const VBucket &vb2 = v2[i];
        if (vb1.size() != vb2.size()) {
            return false;
        }
        for (const auto &pair1 : vb1) {
            const auto &key = pair1.first;
            const auto &value1 = pair1.second;
            auto it2 = vb2.find(key);
            if (it2 == vb2.end() || it2->second != value1) {
                return false;
            }
        }
    }
    return true;
}

bool DataShareHelperImplRegisterTest::ChangeInfoEqual(const DataShareObserver::ChangeInfo &changeInfo,
    const DataShareObserver::ChangeInfo &expectChangeInfo)
{
    if (changeInfo.changeType_ != expectChangeInfo.changeType_) {
        return false;
    }

    if (!UrisEqual(changeInfo.uris_, expectChangeInfo.uris_)) {
        return false;
    }

    if (changeInfo.size_ != expectChangeInfo.size_) {
        return false;
    }

    if (changeInfo.size_ != 0) {
        if (changeInfo.data_ == nullptr && expectChangeInfo.data_ == nullptr) {
            return false;
        }
        return memcmp(changeInfo.data_, expectChangeInfo.data_, expectChangeInfo.size_) == 0;
    }

    return ValueBucketEqual(changeInfo.valueBuckets_, expectChangeInfo.valueBuckets_);
}


/**
 * @tc.name: TryRegisterObserverExt_001
 * @tc.desc: Test the normal function of TryRegisterObserverExt interface,
 *           verifying observer notification after INSERT operation
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: 
    1. Global DataShareHelper instance g_dataShareHelper is initialized and not null
    2. Valid DATA_SHARE_URI is predefined
    3. MockDatashareObserver class is implemented to monitor change notifications
 * @tc.step:
    1. Obtain the global DataShareHelper instance and verify it is not null
    2. Create a Uri object with DATA_SHARE_URI and a MockDatashareObserver instance
    3. Call TryRegisterObserverExt with the Uri, observer and isDescendants=true, verify return value is ERR_OK
    4. Create a DataShareValuesBucket, insert data with key "name" and call Insert interface, verify return value > 0
    5. Create INSERT-type ChangeInfo, call NotifyChangeExt to send notification
    6. Wait for observer's notification, verify the received ChangeInfo matches the sent one, then clear observer data
    7. Repeat steps 4-6 with a descendant Uri (DATA_SHARE_URI + "/com.ohos.example")
    8. Use DataSharePredicates to delete the inserted data, call Delete interface and verify return value > 0
    9. Create DELETE-type ChangeInfo with custom data, call NotifyChangeExt and verify observer receives
       correct notification
    10. Call TryUnregisterObserverExt to unregister the observer, verify return value is ERR_OK
 * @tc.expect:
    1. All interface calls (TryRegisterObserverExt, Insert, Delete, TryUnregisterObserverExt) return expected values
    2. Observer correctly receives INSERT notifications for both the main Uri and its descendant Uri
    3. Observer correctly receives the DELETE notification with custom data
    4. No null pointer exceptions or unexpected crashes occur during execution
 */
HWTEST_F(DataShareHelperImplRegisterTest, TryRegisterObserverExt_001, TestSize.Level0)
{
    LOG_INFO("TryRegisterObserverExt_001 start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    ASSERT_NE(helper, nullptr);
    Uri uri(DATA_SHARE_URI);
    std::shared_ptr<MockDatashareObserver> dataObserver = std::make_shared<MockDatashareObserver>();
    auto ret1 = helper->TryRegisterObserverExt(uri, dataObserver, true);
    EXPECT_EQ(ret1, ERR_OK);
    DataShare::DataShareValuesBucket valuesBucket;
    valuesBucket.Put("name", "Datashare_TryRegisterObserverExt_Test001");
    int retVal = helper->Insert(uri, valuesBucket);
    EXPECT_GT(retVal, 0);
    DataShareObserver::ChangeInfo uriChanges = { DataShareObserver::ChangeType::INSERT, { uri } };
    helper->NotifyChangeExt(uriChanges);

    dataObserver->data.Wait();
    EXPECT_TRUE(ChangeInfoEqual(dataObserver->changeInfo_, uriChanges));
    dataObserver->Clear();

    Uri descendantsUri(DATA_SHARE_URI + "/com.ohos.example");
    int retVal2 = helper->Insert(descendantsUri, valuesBucket);
    EXPECT_GT(retVal2, 0);
    DataShareObserver::ChangeInfo descendantsChanges = { DataShareObserver::ChangeType::INSERT, { descendantsUri } };
    helper->NotifyChangeExt(descendantsChanges);

    dataObserver->data.Wait();
    EXPECT_TRUE(ChangeInfoEqual(dataObserver->changeInfo_, descendantsChanges));
    dataObserver->Clear();

    DataShare::DataSharePredicates deletePredicates;
    deletePredicates.EqualTo("name", "Datashare_TryRegisterObserverExt_Test001");
    int retVal3 = helper->Delete(uri, deletePredicates);
    EXPECT_GT(retVal3, 0);
    char data[] = { 0x01, 0x02, 0x03, 0x04, 0x05 };
    DataShareObserver::ChangeInfo delChanges = { DataShareObserver::ChangeType::DELETE, { uri }, data,
        sizeof(data) / sizeof(data[0]) };
    helper->NotifyChangeExt(delChanges);

    dataObserver->data.Wait();
    EXPECT_TRUE(ChangeInfoEqual(dataObserver->changeInfo_, delChanges));
    dataObserver->Clear();

    auto ret2 = helper->TryUnregisterObserverExt(uri, dataObserver);
    EXPECT_EQ(ret2, ERR_OK);
    LOG_INFO("TryRegisterObserverExt_001 end");
}

/**
 * @tc.name: TryRegisterObserverExt_002
 * @tc.desc: Test the normal function of TryRegisterObserverExt interface, verifying observer
 *           notification after BatchInsert operation
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: 
    1. Global DataShareHelper instance g_dataShareHelper is initialized and not null
    2. Valid DATA_SHARE_URI is predefined
    3. MockDatashareObserver class and ValueProxy::Convert method are implemented
 * @tc.step:
    1. Obtain the global DataShareHelper instance and verify it is not null
    2. Create a Uri object with DATA_SHARE_URI and a MockDatashareObserver instance
    3. Call TryRegisterObserverExt with the Uri, observer and isDescendants=false, verify return value is ERR_OK
    4. Create two DataShareValuesBucket objects with different "name" values, add them to a vector
    5. Call BatchInsert interface with the Uri and vector, verify return value > 0
    6. Use ValueProxy::Convert to convert the vector to VBuckets, create INSERT-type ChangeInfo
    7. Call NotifyChangeExt to send the ChangeInfo, wait for observer's notification
    8. Verify the received ChangeInfo matches the sent one, then clear observer data
    9. Call TryUnregisterObserverExt to unregister the observer, verify return value is ERR_OK
 * @tc.expect:
    1. TryRegisterObserverExt and TryUnregisterObserverExt return ERR_OK
    2. BatchInsert returns a value greater than 0, indicating successful batch insertion
    3. Observer correctly receives the INSERT notification with VBuckets data
    4. No unexpected errors or crashes occur during the test
 */
HWTEST_F(DataShareHelperImplRegisterTest, TryRegisterObserverExt_002, TestSize.Level0)
{
    LOG_INFO("TryRegisterObserverExt_002 start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    ASSERT_NE(helper, nullptr);
    Uri uri(DATA_SHARE_URI);
    std::shared_ptr<MockDatashareObserver> dataObserver = std::make_shared<MockDatashareObserver>();
    auto ret1 = helper->TryRegisterObserverExt(uri, dataObserver, false);
    EXPECT_EQ(ret1, ERR_OK);
    DataShare::DataShareValuesBucket valuesBucket1;
    DataShare::DataShareValuesBucket valuesBucket2;
    valuesBucket1.Put("name", "Datashare_TryRegisterObserverExt_Test002");
    valuesBucket2.Put("name", "Datashare_TryRegisterObserverExt_Test002_2");
    std::vector<DataShareValuesBucket> vBuckets = { valuesBucket1, valuesBucket2 };
    int retVal = helper->BatchInsert(uri, vBuckets);
    EXPECT_GT(retVal, 0);
    DataShareObserver::ChangeInfo::VBuckets extends;
    extends = ValueProxy::Convert(std::move(vBuckets));
    DataShareObserver::ChangeInfo uriChanges = { DataShareObserver::ChangeType::INSERT, { uri }, nullptr, 0, extends };
    helper->NotifyChangeExt(uriChanges);

    dataObserver->data.Wait();
    EXPECT_TRUE(ChangeInfoEqual(dataObserver->changeInfo_, uriChanges));
    dataObserver->Clear();

    auto ret2 = helper->TryUnregisterObserverExt(uri, dataObserver);
    EXPECT_EQ(ret2, ERR_OK);
    LOG_INFO("TryRegisterObserverExt_002 end");
}

/**
 * @tc.name: TryRegisterObserverExt_003
 * @tc.desc: Test the normal function of TryRegisterObserverExt interface, verifying observer
 *           notification after Update operation
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: 
    1. Global DataShareHelper instance g_dataShareHelper is initialized and not null
    2. Valid DATA_SHARE_URI is predefined
    3. MockDatashareObserver class and ValueProxy::Convert method are implemented
 * @tc.step:
    1. Obtain the global DataShareHelper instance and verify it is not null
    2. Create a Uri object with DATA_SHARE_URI and a MockDatashareObserver instance
    3. Call TryRegisterObserverExt with the Uri, observer and isDescendants=false, verify return value is ERR_OK
    4. Create a DataShareValuesBucket with "name" = "TryRegisterObserverExt_003", call Insert and verify return
       value > 0
    5. Create another DataShareValuesBucket with "name" = "TryRegisterObserverExt_003_2"
    6. Create DataSharePredicates to filter data by the original "name", call Update and verify return value > 0
    7. Convert the update bucket to VBuckets, create UPDATE-type ChangeInfo
    8. Call NotifyChangeExt to send the ChangeInfo, wait for observer's notification
    9. Verify the received ChangeInfo matches the sent one, then clear observer data
    10. Call TryUnregisterObserverExt to unregister the observer, verify return value is ERR_OK
 * @tc.expect:
    1. Insert and Update operations return values greater than 0, indicating success
    2. TryRegisterObserverExt and TryUnregisterObserverExt return ERR_OK
    3. Observer correctly receives the UPDATE notification with VBuckets data
    4. No data inconsistency or unexpected crashes occur
 */
HWTEST_F(DataShareHelperImplRegisterTest, TryRegisterObserverExt_003, TestSize.Level0)
{
    LOG_INFO("TryRegisterObserverExt_003 start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    ASSERT_NE(helper, nullptr);
    Uri uri(DATA_SHARE_URI);
    std::shared_ptr<MockDatashareObserver> dataObserver = std::make_shared<MockDatashareObserver>();
    auto ret1 = helper->TryRegisterObserverExt(uri, dataObserver, false);
    EXPECT_EQ(ret1, ERR_OK);
    DataShare::DataShareValuesBucket valuesBucket;
    valuesBucket.Put("name", "TryRegisterObserverExt_003");
    int retVal = helper->Insert(uri, valuesBucket);
    EXPECT_GT(retVal, 0);
    DataShare::DataShareValuesBucket valuesBucket2;
    valuesBucket2.Put("name", "TryRegisterObserverExt_003_2");
    std::vector<DataShareValuesBucket> vBuckets = { valuesBucket2 };
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo("name", "TryRegisterObserverExt_003");
    int retVal2 = helper->Update(uri, predicates, valuesBucket2);
    EXPECT_GT(retVal2, 0);
    DataShareObserver::ChangeInfo::VBuckets extends;
    extends = ValueProxy::Convert(std::move(vBuckets));
    DataShareObserver::ChangeInfo uriChanges = { DataShareObserver::ChangeType::UPDATE, { uri }, nullptr, 0, extends };
    helper->NotifyChangeExt(uriChanges);

    dataObserver->data.Wait();
    EXPECT_TRUE(ChangeInfoEqual(dataObserver->changeInfo_, uriChanges));
    dataObserver->Clear();

    auto ret2 = helper->TryUnregisterObserverExt(uri, dataObserver);
    EXPECT_EQ(ret2, ERR_OK);
    LOG_INFO("TryRegisterObserverExt_003 end");
}

/**
 * @tc.name: TryRegisterObserverExt_004
 * @tc.desc: Test the normal function of TryRegisterObserverExt interface, verifying observer notification
 *           after Delete operation
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: 
    1. Global DataShareHelper instance g_dataShareHelper is initialized and not null
    2. Valid DATA_SHARE_URI is predefined
    3. MockDatashareObserver class and ValueProxy::Convert method are implemented
 * @tc.step:
    1. Obtain the global DataShareHelper instance and verify it is not null
    2. Create a Uri object with DATA_SHARE_URI and a MockDatashareObserver instance
    3. Call TryRegisterObserverExt with the Uri, observer and isDescendants=false, verify return value is ERR_OK
    4. Create a DataShareValuesBucket with "name" = "Datashare_TryRegisterObserverExt_Test004", call Insert and verify
       return value > 0
    5. Create DataSharePredicates to filter data by the inserted "name", call Delete and verify return value > 0
    6. Add the inserted bucket to a vector, convert to VBuckets, create DELETE-type ChangeInfo
    7. Call NotifyChangeExt to send the ChangeInfo, wait for observer's notification
    8. Verify the received ChangeInfo matches the sent one, then clear observer data
    9. Call TryUnregisterObserverExt to unregister the observer, verify return value is ERR_OK
 * @tc.expect:
    1. Insert and Delete operations return values greater than 0, indicating success
    2. TryRegisterObserverExt and TryUnregisterObserverExt return ERR_OK
    3. Observer correctly receives the DELETE notification with VBuckets data
    4. The test executes without unexpected errors or crashes
 */
HWTEST_F(DataShareHelperImplRegisterTest, TryRegisterObserverExt_004, TestSize.Level0)
{
    LOG_INFO("TryRegisterObserverExt_004 start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    ASSERT_NE(helper, nullptr);
    Uri uri(DATA_SHARE_URI);
    std::shared_ptr<MockDatashareObserver> dataObserver = std::make_shared<MockDatashareObserver>();
    auto ret1 = helper->TryRegisterObserverExt(uri, dataObserver, false);
    EXPECT_EQ(ret1, ERR_OK);
    DataShare::DataShareValuesBucket valuesBucket1;
    valuesBucket1.Put("name", "Datashare_TryRegisterObserverExt_Test004");
    int retVal = helper->Insert(uri, valuesBucket1);
    EXPECT_GT(retVal, 0);
    DataShare::DataShareValuesBucket valuesBucket2;
    valuesBucket2.Put("name", "Datashare_TryRegisterObserverExt_Test004");
    std::vector<DataShareValuesBucket> vBuckets2 = { valuesBucket2 };
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo("name", "Datashare_TryRegisterObserverExt_Test004");
    int retVal2 = helper->Delete(uri, predicates);
    EXPECT_GT(retVal2, 0);
    DataShareObserver::ChangeInfo::VBuckets extends;
    extends = ValueProxy::Convert(std::move(vBuckets2));
    DataShareObserver::ChangeInfo uriChanges = { DataShareObserver::ChangeType::DELETE, { uri }, nullptr, 0, extends };
    helper->NotifyChangeExt(uriChanges);

    dataObserver->data.Wait();
    EXPECT_TRUE(ChangeInfoEqual(dataObserver->changeInfo_, uriChanges));
    dataObserver->Clear();

    auto ret2 = helper->TryUnregisterObserverExt(uri, dataObserver);
    EXPECT_EQ(ret2, ERR_OK);
    LOG_INFO("TryRegisterObserverExt_004 end");
}

/**
 * @tc.name: TryRegisterObserverExt_005
 * @tc.desc: Test the function of TryRegisterObserverExt interface with empty Uri, verifying error handling
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: 
    1. Global DataShareHelper instance g_dataShareHelper is initialized and not null
    2. MockDatashareObserver class is implemented
 * @tc.step:
    1. Obtain the global DataShareHelper instance and verify it is not null
    2. Create an empty Uri object ("") and a MockDatashareObserver instance
    3. Call TryRegisterObserverExt with the empty Uri, observer and isDescendants=false
    4. Verify the return value is not ERR_OK (indicating failure for empty Uri)
 * @tc.expect:
    1. The DataShareHelper instance is not null
    2. TryRegisterObserverExt returns a value other than ERR_OK, correctly handling the empty Uri
    3. No null pointer exceptions occur during the test
 */
HWTEST_F(DataShareHelperImplRegisterTest, TryRegisterObserverExt_005, TestSize.Level0)
{
    LOG_INFO("TryRegisterObserverExt_005 start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    ASSERT_NE(helper, nullptr);
    Uri uri("");
    std::shared_ptr<MockDatashareObserver> dataObserver = std::make_shared<MockDatashareObserver>();
    auto ret1 = helper->TryRegisterObserverExt(uri, dataObserver, false);
    EXPECT_NE(ret1, ERR_OK);
    LOG_INFO("TryRegisterObserverExt_005 end");
}

/**
 * @tc.name: TryRegisterObserverExt_006
 * @tc.desc: Test the function of TryRegisterObserverExt interface with null observer, verifying error handling
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: 
    1. Global DataShareHelper instance g_dataShareHelper is initialized and not null
    2. Valid DATA_SHARE_URI is predefined
 * @tc.step:
    1. Obtain the global DataShareHelper instance and verify it is not null
    2. Create a Uri object with DATA_SHARE_URI and set dataObserver to nullptr
    3. Call TryRegisterObserverExt with the Uri, null observer and isDescendants=false
    4. Verify the return value equals E_NULL_OBSERVER
 * @tc.expect:
    1. The DataShareHelper instance is not null
    2. TryRegisterObserverExt returns E_NULL_OBSERVER, correctly handling the null observer
    3. No crash or undefined behavior occurs with a null observer input
 */
HWTEST_F(DataShareHelperImplRegisterTest, TryRegisterObserverExt_006, TestSize.Level0)
{
    LOG_INFO("TryRegisterObserverExt_006 start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    ASSERT_NE(helper, nullptr);
    Uri uri(DATA_SHARE_URI);
    std::shared_ptr<MockDatashareObserver> dataObserver = nullptr;
    auto ret1 = helper->TryRegisterObserverExt(uri, dataObserver, false);
    EXPECT_EQ(ret1, E_NULL_OBSERVER);
    LOG_INFO("TryRegisterObserverExt_006 end");
}

/**
 * @tc.name: TryRegisterObserverExt_007
 * @tc.desc: Test the function of TryRegisterObserverExt interface with isSystem=true, verifying permission handling
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: 
    1. Global DataShareHelper instance g_dataShareHelper is initialized and not null
    2. Valid DATA_SHARE_URI is predefined
    3. MockDatashareObserver class is implemented
 * @tc.step:
    1. Obtain the global DataShareHelper instance and verify it is not null
    2. Create a Uri object with DATA_SHARE_URI and a MockDatashareObserver instance
    3. Call TryRegisterObserverExt with the Uri, observer, isDescendants=false, and isSystem=true
    4. Verify the return value is not ERR_OK
 * @tc.expect:
    1. The DataShareHelper instance is not null
    2. TryRegisterObserverExt returns an error code (not ERR_OK) when isSystem=true
    3. No unexpected behavior occurs during system-level registration attempt
 */
HWTEST_F(DataShareHelperImplRegisterTest, TryRegisterObserverExt_007, TestSize.Level0)
{
    LOG_INFO("TryRegisterObserverExt_007 start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    ASSERT_NE(helper, nullptr);
    Uri uri(DATA_SHARE_URI);
    std::shared_ptr<MockDatashareObserver> dataObserver = std::make_shared<MockDatashareObserver>();
    auto ret1 = helper->TryRegisterObserverExt(uri, dataObserver, false, true);
    EXPECT_NE(ret1, ERR_OK);
    LOG_INFO("TryRegisterObserverExt_007 end");
}

/**
 * @tc.name: TryRegisterObserverExt_008
 * @tc.desc: Test the parent class implementation of TryRegisterObserverExt interface, verifying unimplemented behavior
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: 
    1. Global DataShareHelper instance g_dataShareHelper is initialized and not null
    2. Valid DATA_SHARE_URI is predefined
 * @tc.step:
    1. Obtain the global DataShareHelper instance and verify it is not null
    2. Create a Uri object with DATA_SHARE_URI and set dataObserver to nullptr
    3. Explicitly call the parent class method DataShareHelper::TryRegisterObserverExt with the Uri,
       null observer and isDescendants=false
    4. Verify the return value equals E_UNIMPLEMENT (expected for unimplemented parent method)
 * @tc.expect:
    1. The DataShareHelper instance is not null
    2. The parent class TryRegisterObserverExt returns E_UNIMPLEMENT, indicating it is unimplemented
    3. No crash occurs when calling the unimplemented parent method
 */
HWTEST_F(DataShareHelperImplRegisterTest, TryRegisterObserverExt_008, TestSize.Level0)
{
    LOG_INFO("TryRegisterObserverExt_008 start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    ASSERT_NE(helper, nullptr);
    Uri uri(DATA_SHARE_URI);
    std::shared_ptr<MockDatashareObserver> dataObserver = nullptr;
    auto ret1 = helper->DataShareHelper::TryRegisterObserverExt(uri, dataObserver, false);
    EXPECT_EQ(ret1, E_UNIMPLEMENT);
    LOG_INFO("TryRegisterObserverExt_008 end");
}

/**
 * @tc.name: TryRegisterObserverExt_009
 * @tc.desc: Test the function of TryRegisterObserverExt interface with invalid Uri, verifying notification behavior
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: 
    1. Global DataShareHelper instance g_dataShareHelper is initialized and not null
    2. Invalid REGISTER_URI_INVALID is predefined
    3. MockDatashareObserver class and ValueProxy::Convert method are implemented
 * @tc.step:
    1. Obtain the global DataShareHelper instance and verify it is not null
    2. Create a Uri object with REGISTER_URI_INVALID and a MockDatashareObserver instance
    3. Call TryRegisterObserverExt with the invalid Uri, observer and isDescendants=true, verify return value
       equals AAFwk::DATAOBS_INVALID_URI
    4. Create a DataShareValuesBucket, add to a vector, convert to VBuckets via ValueProxy::Convert
    5. Create INSERT-type ChangeInfo with the invalid Uri, call NotifyChangeExt to send notification
    6. Wait for observer's notification, verify observer is not notified (isNotify=false) and ChangeInfo does not match
    7. Clear observer data and unregister the observer
 * @tc.expect:
    1. TryRegisterObserverExt returns AAFwk::DATAOBS_INVALID_URI for invalid Uri
    2. Observer does not receive notification (isNotify=false) after NotifyChangeExt
    3. The received ChangeInfo does not match the sent one
    4. No unexpected errors occur during the test
 */
HWTEST_F(DataShareHelperImplRegisterTest, TryRegisterObserverExt_009, TestSize.Level0)
{
    LOG_INFO("TryRegisterObserverExt_008::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    ASSERT_NE(helper, nullptr);
    Uri uri(REGISTER_URI_INVALID);
    std::shared_ptr<MockDatashareObserver> dataObserver = std::make_shared<MockDatashareObserver>();
    auto ret1 = helper->TryRegisterObserverExt(uri, dataObserver, true);
    EXPECT_EQ(ret1, AAFwk::DATAOBS_INVALID_URI);

    DataShare::DataShareValuesBucket valuesBucket;
    valuesBucket.Put("name", "TryRegisterObserverExt_009");

    std::vector<DataShareValuesBucket> Buckets = { valuesBucket };
    DataShareObserver::ChangeInfo::VBuckets extends;
    extends = ValueProxy::Convert(std::move(Buckets));
    DataShareObserver::ChangeInfo uriChanges = { DataShareObserver::ChangeType::INSERT, { uri }, nullptr, 0, extends };
    helper->NotifyChangeExt(uriChanges);

    dataObserver->data.Wait();
    EXPECT_EQ(dataObserver->isNotify, false);
    EXPECT_FALSE(ChangeInfoEqual(dataObserver->changeInfo_, uriChanges));
    dataObserver->Clear();

    helper->UnregisterObserverExt(uri, dataObserver);
    LOG_INFO("TryRegisterObserverExt_008::End");
}

/**
 * @tc.name: TryUnregisterObserverExt_001
 * @tc.desc: Test the normal function of TryUnregisterObserverExt interface, verifying notification stops
 *           after unregistration
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: 
    1. Global DataShareHelper instance g_dataShareHelper is initialized and not null
    2. Valid DATA_SHARE_URI is predefined
    3. MockDatashareObserver class and ValueProxy::Convert method are implemented
 * @tc.step:
    1. Obtain the global DataShareHelper instance and verify it is not null
    2. Create a Uri object with DATA_SHARE_URI and a MockDatashareObserver instance
    3. Call TryRegisterObserverExt with the Uri, observer and isDescendants=true, verify return value is ERR_OK
    4. Create a DataShareValuesBucket, insert data via Insert interface, verify return value > 0
    5. Convert the bucket to VBuckets, create INSERT-type ChangeInfo, call NotifyChangeExt
    6. Wait for observer's notification, verify ChangeInfo matches, then clear observer data
    7. Call TryUnregisterObserverExt to unregister the observer, verify return value is ERR_OK
    8. Send another DELETE-type notification via NotifyChangeExt
    9. Wait for observer's notification, verify ChangeInfo does not match
 * @tc.expect:
    1. TryRegisterObserverExt and TryUnregisterObserverExt return ERR_OK
    2. Observer receives and matches the first (INSERT) notification before unregistration
    3. Observer does not match the second (DELETE) notification after unregistration
    4. Insert operation returns a value > 0, indicating success
 */
HWTEST_F(DataShareHelperImplRegisterTest, TryUnregisterObserverExt_001, TestSize.Level0)
{
    LOG_INFO("TryUnregisterObserverExt_001 start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    ASSERT_NE(helper, nullptr);
    Uri uri(DATA_SHARE_URI);
    std::shared_ptr<MockDatashareObserver> dataObserver = std::make_shared<MockDatashareObserver>();
    auto ret1 = helper->TryRegisterObserverExt(uri, dataObserver, true);
    EXPECT_EQ(ret1, ERR_OK);

    DataShare::DataShareValuesBucket valuesBucket;
    valuesBucket.Put("name", "TryUnregisterObserverExt_001");
    int retVal = helper->Insert(uri, valuesBucket);
    EXPECT_GT(retVal, 0);
    std::vector<DataShareValuesBucket> Buckets = { valuesBucket };
    DataShareObserver::ChangeInfo::VBuckets extends;
    extends = ValueProxy::Convert(std::move(Buckets));
    DataShareObserver::ChangeInfo uriChanges = { DataShareObserver::ChangeType::INSERT, { uri }, nullptr, 0, extends };
    helper->NotifyChangeExt(uriChanges);

    dataObserver->data.Wait();
    EXPECT_TRUE(ChangeInfoEqual(dataObserver->changeInfo_, uriChanges));
    dataObserver->Clear();

    auto ret2 = helper->TryUnregisterObserverExt(uri, dataObserver);
    EXPECT_EQ(ret2, ERR_OK);
    helper->NotifyChangeExt({ DataShareObserver::ChangeType::DELETE, { uri }, nullptr, 0, extends });

    dataObserver->data.Wait();
    EXPECT_FALSE(ChangeInfoEqual(dataObserver->changeInfo_, uriChanges));
    dataObserver->Clear();
    LOG_INFO("TryUnregisterObserverExt_001 end");
}

/**
 * @tc.name: TryUnregisterObserverExt_002
 * @tc.desc: Test the function of TryUnregisterObserverExt interface with null observer, verifying error handling
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: 
    1. Global DataShareHelper instance g_dataShareHelper is initialized and not null
    2. Valid DATA_SHARE_URI is predefined
 * @tc.step:
    1. Obtain the global DataShareHelper instance and verify it is not null
    2. Create a Uri object with DATA_SHARE_URI and set dataObserver to nullptr
    3. Call TryUnregisterObserverExt with the Uri and null observer
    4. Verify the return value equals E_NULL_OBSERVER
 * @tc.expect:
    1. The DataShareHelper instance is not null
    2. TryUnregisterObserverExt returns E_NULL_OBSERVER, correctly handling the null observer
    3. No crash occurs when unregistering a null observer
 */
HWTEST_F(DataShareHelperImplRegisterTest, TryUnregisterObserverExt_002, TestSize.Level0)
{
    LOG_INFO("TryUnregisterObserverExt_002 start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    ASSERT_NE(helper, nullptr);
    Uri uri(DATA_SHARE_URI);
    std::shared_ptr<MockDatashareObserver> dataObserver = nullptr;
    auto ret2 = helper->TryUnregisterObserverExt(uri, dataObserver);
    EXPECT_EQ(ret2, E_NULL_OBSERVER);
    LOG_INFO("TryUnregisterObserverExt_002 end");
}

/**
 * @tc.name: TryUnregisterObserverExt_003
 * @tc.desc: Test the function of TryUnregisterObserverExt interface with unregistered observer,
 *           verifying error handling
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: 
    1. Global DataShareHelper instance g_dataShareHelper is initialized and not null
    2. Valid DATA_SHARE_URI is predefined
    3. MockDatashareObserver class is implemented
 * @tc.step:
    1. Obtain the global DataShareHelper instance and verify it is not null
    2. Create a Uri object with DATA_SHARE_URI and a MockDatashareObserver instance (not registered)
    3. Call TryUnregisterObserverExt with the Uri and unregistered observer
    4. Verify the return value equals E_NULL_OBSERVER (expected error for unregistered observer)
 * @tc.expect:
    1. The DataShareHelper instance is not null
    2. TryUnregisterObserverExt returns E_NULL_OBSERVER when unregistering an unregistered observer
    3. No unexpected behavior occurs during the test
 */
HWTEST_F(DataShareHelperImplRegisterTest, TryUnregisterObserverExt_003, TestSize.Level0)
{
    LOG_INFO("TryUnregisterObserverExt_003 start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    ASSERT_NE(helper, nullptr);
    Uri uri(DATA_SHARE_URI);
    std::shared_ptr<MockDatashareObserver> dataObserver = std::make_shared<MockDatashareObserver>();

    auto ret2 = helper->TryUnregisterObserverExt(uri, dataObserver);
    EXPECT_EQ(ret2, E_NULL_OBSERVER);
    LOG_INFO("TryUnregisterObserverExt_003 end");
}

/**
 * @tc.name: TryUnregisterObserverExt_004
 * @tc.desc: Test the parent class implementation of TryUnregisterObserverExt interface, verifying
 *           unimplemented behavior
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: 
    1. Global DataShareHelper instance g_dataShareHelper is initialized and not null
    2. Valid DATA_SHARE_URI is predefined
 * @tc.step:
    1. Obtain the global DataShareHelper instance and verify it is not null
    2. Create a Uri object with DATA_SHARE_URI and set dataObserver to nullptr
    3. Explicitly call the parent class method DataShareHelper::TryUnregisterObserverExt with the Uri and null observer
    4. Verify the return value equals E_UNIMPLEMENT (expected for unimplemented parent method)
 * @tc.expect:
    1. The DataShareHelper instance is not null
    2. The parent class TryUnregisterObserverExt returns E_UNIMPLEMENT, indicating it is unimplemented
    3. No crash occurs when calling the unimplemented parent method
 */
HWTEST_F(DataShareHelperImplRegisterTest, TryUnregisterObserverExt_004, TestSize.Level0)
{
    LOG_INFO("TryUnregisterObserverExt_004 start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    ASSERT_NE(helper, nullptr);
    Uri uri(DATA_SHARE_URI);
    std::shared_ptr<MockDatashareObserver> dataObserver = nullptr;
    auto ret2 = helper->DataShareHelper::TryUnregisterObserverExt(uri, dataObserver);
    EXPECT_EQ(ret2, E_UNIMPLEMENT);
    LOG_INFO("TryUnregisterObserverExt_004 end");
}

/**
 * @tc.name: RegisterObserver_Register_Test_001
 * @tc.desc: Test the normal function of RegisterObserver interface, verifying successful registration and notification
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: 
    1. Global DataShareHelper instance is initialized and not null
    2. Valid REGISTER_URI is predefined
    3. IDataShareAbilityObserverTest class is implemented (with SetName, GetName, and Clear methods)
 * @tc.step:
    1. Obtain the DataShareHelper instance and verify it is not null
    2. Create a Uri object with REGISTER_URI and an IDataShareAbilityObserverTest instance, set observer name
       to "zhangsan"
    3. Call RegisterObserver with the Uri and observer, verify observer name remains "zhangsan"
    4. Call NotifyChange with the Uri, wait for observer's notification
    5. Verify observer name changes to "OnChangeName" (indicating notification received), then clear observer data
    6. Call UnregisterObserver with the Uri and observer, reset observer name to "zhangsan"
    7. Call NotifyChange again, verify observer name remains "zhangsan"
 * @tc.expect:
    1. RegisterObserver successfully registers the observer without errors
    2. Observer receives notification (name changes to "OnChangeName") before unregistration
    3. Observer does not receive notification (name remains "zhangsan") after unregistration
    4. No unexpected behavior or crashes occur during registration/unregistration
 */
HWTEST_F(DataShareHelperImplRegisterTest, RegisterObserver_Register_Test_001, TestSize.Level0)
{
    LOG_INFO("RegisterObserver_Register_Test_001::Start");
    auto helper = g_dataShareHelper;
    Uri uri(REGISTER_URI);
    sptr<IDataShareAbilityObserverTest> dataObserver(new (std::nothrow) IDataShareAbilityObserverTest());
    dataObserver->SetName("zhangsan");
    helper->RegisterObserver(uri, dataObserver);
    EXPECT_EQ(dataObserver->GetName(), "zhangsan");

    helper->NotifyChange(uri);
    dataObserver->data.Wait();
    EXPECT_EQ(dataObserver->GetName(), "OnChangeName");
    dataObserver->Clear();

    helper->UnregisterObserver(uri, dataObserver);
    dataObserver->SetName("zhangsan");
    helper->NotifyChange(uri);
    EXPECT_EQ(dataObserver->GetName(), "zhangsan");
    LOG_INFO("RegisterObserver_Register_Test_001::End");
}

/**
 * @tc.name: RegisterObserver_Register_Test_002
 * @tc.desc: Test RegisterObserver interface with insufficient read permission, verifying notification restriction
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: 
    1. Global DataShareHelper instance is initialized and not null
    2. REGISTER_URI_NOREAD (Uri without read permission) is predefined
    3. IDataShareAbilityObserverTest class is implemented
 * @tc.step:
    1. Obtain the DataShareHelper instance and verify it is not null
    2. Create a Uri object with REGISTER_URI_NOREAD and an IDataShareAbilityObserverTest instance, set name to
       "zhangsan"
    3. Call RegisterObserver with the Uri and observer, verify observer name remains "zhangsan"
    4. Call NotifyChange with the Uri, wait for observer's notification
    5. Verify observer name remains "zhangsan" (no notification due to missing read permission)
    6. Clear observer data and unregister the observer
 * @tc.expect:
    1. RegisterObserver executes without crash (even with insufficient permission)
    2. Observer does not receive notification (name remains "zhangsan") due to missing read permission
    3. Unregistration completes without errors
 */
HWTEST_F(DataShareHelperImplRegisterTest, RegisterObserver_Register_Test_002, TestSize.Level0)
{
    LOG_INFO("RegisterObserver_Register_Test_002::Start");
    auto helper = g_dataShareHelper;
    Uri uri(REGISTER_URI_NOREAD);
    sptr<IDataShareAbilityObserverTest> dataObserver(new (std::nothrow) IDataShareAbilityObserverTest());
    dataObserver->SetName("zhangsan");
    helper->RegisterObserver(uri, dataObserver);
    EXPECT_EQ(dataObserver->GetName(), "zhangsan");

    helper->NotifyChange(uri);
    dataObserver->data.Wait();
    EXPECT_EQ(dataObserver->GetName(), "zhangsan");
    dataObserver->Clear();

    helper->UnregisterObserver(uri, dataObserver);
    LOG_INFO("RegisterObserver_Register_Test_002::End");
}

/**
 * @tc.name: RegisterObserver_Register_Test_003
 * @tc.desc: Test RegisterObserver with sufficient read permission but NotifyChange with insufficient write permission
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: 
    1. Global DataShareHelper instance is initialized and not null
    2. REGISTER_URI_NOWRITE (Uri without write permission) is predefined
    3. IDataShareAbilityObserverTest class is implemented
 * @tc.step:
    1. Obtain the DataShareHelper instance and verify it is not null
    2. Create a Uri object with REGISTER_URI_NOWRITE and an IDataShareAbilityObserverTest instance, set name to
       "zhangsan"
    3. Call RegisterObserver with the Uri and observer, verify observer name remains "zhangsan"
    4. Call NotifyChange with the Uri (which lacks write permission), wait for observer's notification
    5. Verify observer name remains "zhangsan" (no notification due to missing write permission)
    6. Clear observer data and unregister the observer
 * @tc.expect:
    1. RegisterObserver executes without crash (read permission is sufficient for registration)
    2. Observer does not receive notification (name remains "zhangsan") because NotifyChange lacks write permission
    3. Unregistration completes without errors
 */
HWTEST_F(DataShareHelperImplRegisterTest, RegisterObserver_Register_Test_003, TestSize.Level0)
{
    LOG_INFO("RegisterObserver_Register_Test_003::Start");
    auto helper = g_dataShareHelper;
    Uri uri(REGISTER_URI_NOWRITE);
    sptr<IDataShareAbilityObserverTest> dataObserver(new (std::nothrow) IDataShareAbilityObserverTest());
    dataObserver->SetName("zhangsan");
    helper->RegisterObserver(uri, dataObserver);
    EXPECT_EQ(dataObserver->GetName(), "zhangsan");

    helper->NotifyChange(uri);
    dataObserver->data.Wait();
    EXPECT_EQ(dataObserver->GetName(), "zhangsan");
    dataObserver->Clear();

    helper->UnregisterObserver(uri, dataObserver);
    LOG_INFO("RegisterObserver_Register_Test_003::End");
}

/**
 * @tc.name: RegisterObserver_Register_Test_004
 * @tc.desc: Test RegisterObserver interface with neither read permission nor write permission for NotifyChange,
 *           verifying notification restriction
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: 
    1. Global DataShareHelper instance g_dataShareHelper is initialized and not null
    2. REGISTER_URI_NOALL (Uri without read and write permissions) is predefined
    3. IDataShareAbilityObserverTest class is implemented (supports SetName, GetName, Clear methods)
 * @tc.step:
    1. Obtain the DataShareHelper instance and verify it is not null
    2. Create a Uri object with REGISTER_URI_NOALL and an IDataShareAbilityObserverTest instance, set observer
       name to "zhangsan"
    3. Call RegisterObserver with the Uri and observer, verify observer name remains "zhangsan"
    4. Call NotifyChange with the Uri (lacks both read and write permissions), wait for observer's notification
    5. Verify observer name remains "zhangsan" (no notification due to insufficient permissions)
    6. Clear observer data and call UnregisterObserver to unregister the observer
 * @tc.expect:
    1. RegisterObserver and UnregisterObserver execute without crashes, even with insufficient permissions
    2. Observer does not receive notification (name stays "zhangsan") because of missing read/write permissions
    3. No unexpected errors occur during the entire test flow
 */
HWTEST_F(DataShareHelperImplRegisterTest, RegisterObserver_Register_Test_004, TestSize.Level0)
{
    LOG_INFO("RegisterObserver_Register_Test_004::Start");
    auto helper = g_dataShareHelper;
    Uri uri(REGISTER_URI_NOALL);
    sptr<IDataShareAbilityObserverTest> dataObserver(new (std::nothrow) IDataShareAbilityObserverTest());
    dataObserver->SetName("zhangsan");
    helper->RegisterObserver(uri, dataObserver);
    EXPECT_EQ(dataObserver->GetName(), "zhangsan");

    helper->NotifyChange(uri);
    dataObserver->data.Wait();
    EXPECT_EQ(dataObserver->GetName(), "zhangsan");
    dataObserver->Clear();

    helper->UnregisterObserver(uri, dataObserver);
    LOG_INFO("RegisterObserver_Register_Test_004::End");
}

/**
 * @tc.name: RegisterObserver_Register_Test_005
 * @tc.desc: Test RegisterObserver interface with invalid Uri, verifying error handling and notification behavior
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. Global DataShareHelper instance g_dataShareHelper is initialized and not null
    2. REGISTER_URI_INVALID (invalid Uri) is predefined
    3. IDataShareAbilityObserverTest class is implemented
    4. Error code E_REGISTER_ERROR is predefined for registration failures
 * @tc.step:
    1. Obtain the DataShareHelper instance and verify it is not null
    2. Create a Uri object with REGISTER_URI_INVALID and an IDataShareAbilityObserverTest instance, set name to
       "zhangsan"
    3. Call RegisterObserver with the Uri and observer, save the return value
    4. Verify observer name remains "zhangsan" and return value equals E_REGISTER_ERROR
    5. Call NotifyChange with the invalid Uri, wait for observer's notification
    6. Verify observer name remains "zhangsan" (no notification for invalid Uri)
    7. Clear observer data and call UnregisterObserver to unregister the observer
 * @tc.expect:
    1. RegisterObserver returns E_REGISTER_ERROR, correctly handling the invalid Uri
    2. Observer does not receive notification (name stays "zhangsan") due to invalid Uri
    3. UnregisterObserver executes without crashes, even for a failed registration
 */
HWTEST_F(DataShareHelperImplRegisterTest, RegisterObserver_Register_Test_005, TestSize.Level0)
{
    LOG_INFO("RegisterObserver_Register_Test_005::Start");
    auto helper = g_dataShareHelper;
    Uri uri(REGISTER_URI_INVALID);
    sptr<IDataShareAbilityObserverTest> dataObserver(new (std::nothrow) IDataShareAbilityObserverTest());
    dataObserver->SetName("zhangsan");
    auto ret = helper->RegisterObserver(uri, dataObserver);
    EXPECT_EQ(dataObserver->GetName(), "zhangsan");
    EXPECT_EQ(ret, E_REGISTER_ERROR);

    helper->NotifyChange(uri);
    dataObserver->data.Wait();
    EXPECT_EQ(dataObserver->GetName(), "zhangsan");
    dataObserver->Clear();

    helper->UnregisterObserver(uri, dataObserver);
    LOG_INFO("RegisterObserver_Register_Test_005::End");
}

/**
 * @tc.name: RegisterObserver_Register_Test_006
 * @tc.desc: Test RegisterObserver interface in silent mode, verifying successful registration and notification
 *           after InsertEx
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: 
    1. Global silent-mode DataShareHelper instance g_dataShareHelperSilent is initialized and not null
    2. DATA_SHARE_URI_SILENT (valid Uri for silent mode) is predefined
    3. IDataShareAbilityObserverTest class is implemented
    4. Table columns TBL_STU_NAME and TBL_STU_AGE are predefined
 * @tc.step:
    1. Obtain g_dataShareHelperSilent instance and verify it is not null
    2. Create a Uri object with DATA_SHARE_URI_SILENT and an IDataShareAbilityObserverTest instance, set name to
       "zhangsan"
    3. Call RegisterObserver with the Uri and observer, save the return value, verify return value is 0 (success)
       and name remains "zhangsan"
    4. Create a DataShareValuesBucket, add "lisi" to TBL_STU_NAME and 25 to TBL_STU_AGE
    5. Call InsertEx with the Uri and bucket, verify the returned errCode is 0 (insert success)
    6. Wait for observer's notification, verify observer name changes to "OnChangeName"
    7. Clear observer data and call UnregisterObserver to unregister the observer
 * @tc.expect:
    1. RegisterObserver returns 0, indicating successful registration in silent mode
    2. InsertEx returns errCode 0, indicating successful data insertion
    3. Observer receives notification (name changes to "OnChangeName") after InsertEx
    4. No unexpected errors occur in silent-mode operations
 */
HWTEST_F(DataShareHelperImplRegisterTest, RegisterObserver_Register_Test_006, TestSize.Level0)
{
    LOG_INFO("RegisterObserver_Register_Test_006::Start");
    auto helper = g_dataShareHelperSilent;
    Uri uri(DATA_SHARE_URI_SILENT);
    sptr<IDataShareAbilityObserverTest> dataObserver(new (std::nothrow) IDataShareAbilityObserverTest());
    dataObserver->SetName("zhangsan");
    auto ret = helper->RegisterObserver(uri, dataObserver);
    EXPECT_EQ(dataObserver->GetName(), "zhangsan");
    EXPECT_EQ(ret, 0);

    DataShare::DataShareValuesBucket valuesBucket;
    std::string value = "lisi";
    valuesBucket.Put(TBL_STU_NAME, value);
    int age = 25;
    valuesBucket.Put(TBL_STU_AGE, age);
    auto [errCode, retVal] = helper->InsertEx(uri, valuesBucket);
    EXPECT_EQ(errCode, 0);
    dataObserver->data.Wait();
    EXPECT_EQ(dataObserver->GetName(), "OnChangeName");
    dataObserver->Clear();

    helper->UnregisterObserver(uri, dataObserver);
    LOG_INFO("RegisterObserver_Register_Test_006::End");
}

/**
 * @tc.name: RegisterObserver_Register_Test_007
 * @tc.desc: Test RegisterObserver interface in silent mode with insufficient read permission, verifying registration
 *           failure and notification restriction
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: 
    1. Global silent-mode DataShareHelper instance g_dataShareHelperSilent is initialized and not null
    2. REGISTER_URI_NOREAD (Uri without read permission) is predefined
    3. IDataShareAbilityObserverTest class is implemented
    4. Error code AAFwk::DATAOBS_PERMISSION_DENY is predefined for permission failures
    5. Table columns TBL_STU_NAME and TBL_STU_AGE are predefined
 * @tc.step:
    1. Obtain g_dataShareHelperSilent instance and verify it is not null
    2. Create a Uri object with REGISTER_URI_NOREAD and an IDataShareAbilityObserverTest instance, set name to
       "zhangsan"
    3. Call RegisterObserver with the Uri and observer, save the return value
    4. Verify observer name remains "zhangsan" and return value equals AAFwk::DATAOBS_PERMISSION_DENY
    5. Create a DataShareValuesBucket, add "lisi" to TBL_STU_NAME and 25 to TBL_STU_AGE
    6. Call InsertEx with the Uri and bucket, verify the returned errCode is 0
    7. Wait for observer's notification, verify observer name remains "zhangsan"
    8. Clear observer data and call UnregisterObserver to unregister the observer
 * @tc.expect:
    1. RegisterObserver returns AAFwk::DATAOBS_PERMISSION_DENY, correctly handling missing read permission
    2. InsertEx returns errCode 0, indicating data insertion is not blocked by observer permission issues
    3. Observer does not receive notification (name stays "zhangsan") due to failed registration
 */
HWTEST_F(DataShareHelperImplRegisterTest, RegisterObserver_Register_Test_007, TestSize.Level0)
{
    LOG_INFO("RegisterObserver_Register_Test_007::Start");
    auto helper = g_dataShareHelperSilent;
    Uri uri(REGISTER_URI_NOREAD);
    sptr<IDataShareAbilityObserverTest> dataObserver(new (std::nothrow) IDataShareAbilityObserverTest());
    dataObserver->SetName("zhangsan");
    auto ret = helper->RegisterObserver(uri, dataObserver);
    EXPECT_EQ(dataObserver->GetName(), "zhangsan");
    EXPECT_EQ(ret, AAFwk::DATAOBS_PERMISSION_DENY);

    DataShare::DataShareValuesBucket valuesBucket;
    std::string value = "lisi";
    valuesBucket.Put(TBL_STU_NAME, value);
    int age = 25;
    valuesBucket.Put(TBL_STU_AGE, age);
    auto [errCode, retVal] = helper->InsertEx(uri, valuesBucket);
    EXPECT_EQ(errCode, 0);
    dataObserver->data.Wait();
    EXPECT_EQ(dataObserver->GetName(), "zhangsan");
    dataObserver->Clear();

    helper->UnregisterObserver(uri, dataObserver);
    LOG_INFO("RegisterObserver_Register_Test_007::End");
}

/**
 * @tc.name: RegisterObserver_Register_Test_008
 * @tc.desc: Test RegisterObserver interface in silent mode with invalid Uri, verifying error handling
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: 
    1. Global silent-mode DataShareHelper instance g_dataShareHelperSilent is initialized and not null
    2. REGISTER_URI_INVALID (invalid Uri) is predefined
    3. IDataShareAbilityObserverTest class is implemented
    4. Error code AAFwk::DATAOBS_INVALID_URI is predefined for invalid Uri
 * @tc.step:
    1. Obtain g_dataShareHelperSilent instance and verify it is not null
    2. Create a Uri object with REGISTER_URI_INVALID and an IDataShareAbilityObserverTest instance, set name to
       "zhangsan"
    3. Call RegisterObserver with the Uri and observer, save the return value
    4. Verify observer name remains "zhangsan" and return value equals AAFwk::DATAOBS_INVALID_URI (registration fails)
 * @tc.expect:
    1. The DataShareHelper instance is not null
    2. RegisterObserver returns AAFwk::DATAOBS_INVALID_URI, correctly handling the invalid Uri in silent mode
    3. Observer name remains "zhangsan" (no state change from failed registration)
    4. No crashes occur during the test
 */
HWTEST_F(DataShareHelperImplRegisterTest, RegisterObserver_Register_Test_008, TestSize.Level0)
{
    LOG_INFO("RegisterObserver_Register_Test_008::Start");
    auto helper = g_dataShareHelperSilent;
    Uri uri(REGISTER_URI_INVALID);
    sptr<IDataShareAbilityObserverTest> dataObserver(new (std::nothrow) IDataShareAbilityObserverTest());
    dataObserver->SetName("zhangsan");
    auto ret = helper->RegisterObserver(uri, dataObserver);
    EXPECT_EQ(dataObserver->GetName(), "zhangsan");
    EXPECT_EQ(ret, AAFwk::DATAOBS_INVALID_URI);

    LOG_INFO("RegisterObserver_Register_Test_008::End");
}

/**
 * @tc.name: RegisterObserver_RegisterExt_Test_001
 * @tc.desc: Test the normal function of RegisterObserverExt interface, verifying successful registration,
 *           notification, and unregistration
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: 
    1. Global DataShareHelper instance g_dataShareHelper is initialized and not null
    2. Valid REGISTER_URI is predefined
    3. MockDatashareObserver class (with isNotify, changeInfo_, Clear method) and ValueProxy::Convert are implemented
    4. ChangeInfoEqual function is implemented to compare ChangeInfo objects
 * @tc.step:
    1. Obtain the DataShareHelper instance and verify it is not null
    2. Create a Uri object with REGISTER_URI and a MockDatashareObserver instance
    3. Call RegisterObserverExt with the Uri, observer and isDescendants=true
    4. Create a DataShareValuesBucket with "name" = "RegisterObserver_RegisterExt_Test_001", add to a vector
    5. Convert the vector to VBuckets via ValueProxy::Convert, create INSERT-type ChangeInfo
    6. Call NotifyChangeExt with the ChangeInfo, wait for observer's notification
    7. Verify ChangeInfoEqual returns true (received matches sent) and clear observer data
    8. Call UnregisterObserverExt to unregister the observer
    9. Send a DELETE-type ChangeInfo via NotifyChangeExt, wait for observer's notification
    10. Verify ChangeInfoEqual returns false (no notification after unregistration) and clear observer data
 * @tc.expect:
    1. RegisterObserverExt and UnregisterObserverExt execute without errors
    2. Observer receives and matches the INSERT notification before unregistration
    3. Observer does not match the DELETE notification after unregistration
    4. No unexpected crashes or data inconsistencies occur
 */
HWTEST_F(DataShareHelperImplRegisterTest, RegisterObserver_RegisterExt_Test_001, TestSize.Level0)
{
    LOG_INFO("RegisterObserver_RegisterExt_Test_001 start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    ASSERT_NE(helper, nullptr);
    Uri uri(REGISTER_URI);
    std::shared_ptr<MockDatashareObserver> dataObserver = std::make_shared<MockDatashareObserver>();
    helper->RegisterObserverExt(uri, dataObserver, true);

    DataShare::DataShareValuesBucket valuesBucket;
    valuesBucket.Put("name", "RegisterObserver_RegisterExt_Test_001");

    std::vector<DataShareValuesBucket> Buckets = { valuesBucket };
    DataShareObserver::ChangeInfo::VBuckets extends;
    extends = ValueProxy::Convert(std::move(Buckets));
    DataShareObserver::ChangeInfo uriChanges = { DataShareObserver::ChangeType::INSERT, { uri }, nullptr, 0, extends };
    helper->NotifyChangeExt(uriChanges);

    dataObserver->data.Wait();
    EXPECT_TRUE(ChangeInfoEqual(dataObserver->changeInfo_, uriChanges));
    dataObserver->Clear();

    helper->UnregisterObserverExt(uri, dataObserver);
    helper->NotifyChangeExt({ DataShareObserver::ChangeType::DELETE, { uri }, nullptr, 0, extends });

    dataObserver->data.Wait();
    EXPECT_FALSE(ChangeInfoEqual(dataObserver->changeInfo_, uriChanges));
    dataObserver->Clear();
    LOG_INFO("RegisterObserver_RegisterExt_Test_001 end");
}

/**
 * @tc.name: RegisterObserver_RegisterExt_Test_002
 * @tc.desc: Test RegisterObserverExt interface with insufficient read permission, verifying notification restriction
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: 
    1. Global DataShareHelper instance g_dataShareHelper is initialized and not null
    2. REGISTER_URI_NOREAD (Uri without read permission) is predefined
    3. MockDatashareObserver class (with isNotify flag and Clear method) and ValueProxy::Convert are implemented
 * @tc.step:
    1. Obtain the DataShareHelper instance and verify it is not null (use ASSERT_NE to ensure validity)
    2. Create a Uri object with REGISTER_URI_NOREAD and a MockDatashareObserver instance
    3. Call RegisterObserverExt with the Uri, observer and isDescendants=true
    4. Create a DataShareValuesBucket, set "name" to "RegisterObserver_RegisterExt_Test_002"
    5. Add the bucket to a vector, convert the vector to VBuckets via ValueProxy::Convert
    6. Create an INSERT-type ChangeInfo with the Uri and VBuckets
    7. Call NotifyChangeExt to send the ChangeInfo to the observer
    8. Wait for the observer's notification signal (via data.Wait())
    9. Verify the observer's isNotify flag is false (no notification received due to missing read permission)
    10. Call Clear() on the observer to reset its state
    11. Call UnregisterObserverExt to unregister the observer from the Uri
 * @tc.expect:
    1. The DataShareHelper instance is confirmed not null (ASSERT_NE passes)
    2. RegisterObserverExt executes without crashing, even with insufficient read permission
    3. After NotifyChangeExt, the observer's isNotify remains false (no notification delivered)
    4. UnregisterObserverExt completes without errors, and observer state is successfully reset via Clear()
    5. No memory leaks or unexpected exceptions occur during the test flow
 */
HWTEST_F(DataShareHelperImplRegisterTest, RegisterObserver_RegisterExt_Test_002, TestSize.Level0)
{
    LOG_INFO("RegisterObserver_RegisterExt_Test_002 start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    ASSERT_NE(helper, nullptr);
    Uri uri(REGISTER_URI_NOREAD);
    std::shared_ptr<MockDatashareObserver> dataObserver = std::make_shared<MockDatashareObserver>();
    helper->RegisterObserverExt(uri, dataObserver, true);

    DataShare::DataShareValuesBucket valuesBucket;
    valuesBucket.Put("name", "RegisterObserver_RegisterExt_Test_002");

    std::vector<DataShareValuesBucket> Buckets = { valuesBucket };
    DataShareObserver::ChangeInfo::VBuckets extends;
    extends = ValueProxy::Convert(std::move(Buckets));
    DataShareObserver::ChangeInfo uriChanges = { DataShareObserver::ChangeType::INSERT, { uri }, nullptr, 0, extends };
    helper->NotifyChangeExt(uriChanges);

    dataObserver->data.Wait();
    EXPECT_EQ(dataObserver->isNotify, false);
    dataObserver->Clear();

    helper->UnregisterObserverExt(uri, dataObserver);
    LOG_INFO("RegisterObserver_RegisterExt_Test_002 end");
}

/**
 * @tc.name: RegisterObserver_RegisterExt_Test_003
 * @tc.desc: Test RegisterObserverExt with sufficient read permission but NotifyChangeExt with insufficient write
 *           permission
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: 
    1. Global DataShareHelper instance g_dataShareHelper is initialized and not null
    2. REGISTER_URI_NOWRITE (Uri with read permission but no write permission) is predefined
    3. MockDatashareObserver class (with isNotify flag and Clear method) and ValueProxy::Convert are implemented
 * @tc.step:
    1. Obtain the DataShareHelper instance and use ASSERT_NE to confirm it is not null
    2. Create a Uri object with REGISTER_URI_NOWRITE and initialize a MockDatashareObserver instance
    3. Call RegisterObserverExt with the Uri, observer and isDescendants=true
    4. Create a DataShareValuesBucket, set "name" to "RegisterObserver_RegisterExt_Test_003"
    5. Add the bucket to a vector, convert the vector to VBuckets using ValueProxy::Convert
    6. Construct an INSERT-type ChangeInfo containing the Uri and VBuckets
    7. Call NotifyChangeExt to send the ChangeInfo (write permission is required for notification delivery)
    8. Wait for the observer's notification signal (data.Wait())
    9. Check the observer's isNotify flag and verify it is false (notification blocked by missing write permission)
    10. Reset the observer's state by calling Clear()
    11. Unregister the observer from the Uri via UnregisterObserverExt
 * @tc.expect:
    1. ASSERT_NE passes, confirming the DataShareHelper instance is valid
    2. RegisterObserverExt succeeds (no error) because the Uri has read permission
    3. NotifyChangeExt executes without crashing, but no notification is delivered (isNotify remains false)
    4. UnregisterObserverExt and Clear() complete normally, resetting the test environment
    5. No unexpected behavior (e.g., null pointer exceptions) occurs during permission checks
 */
HWTEST_F(DataShareHelperImplRegisterTest, RegisterObserver_RegisterExt_Test_003, TestSize.Level0)
{
    LOG_INFO("RegisterObserver_RegisterExt_Test_003 start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    ASSERT_NE(helper, nullptr);
    Uri uri(REGISTER_URI_NOWRITE);
    std::shared_ptr<MockDatashareObserver> dataObserver = std::make_shared<MockDatashareObserver>();
    helper->RegisterObserverExt(uri, dataObserver, true);

    DataShare::DataShareValuesBucket valuesBucket;
    valuesBucket.Put("name", "RegisterObserver_RegisterExt_Test_003");

    std::vector<DataShareValuesBucket> Buckets = { valuesBucket };
    DataShareObserver::ChangeInfo::VBuckets extends;
    extends = ValueProxy::Convert(std::move(Buckets));
    DataShareObserver::ChangeInfo uriChanges = { DataShareObserver::ChangeType::INSERT, { uri }, nullptr, 0, extends };
    helper->NotifyChangeExt(uriChanges);

    dataObserver->data.Wait();
    EXPECT_EQ(dataObserver->isNotify, false);
    dataObserver->Clear();

    helper->UnregisterObserverExt(uri, dataObserver);
    LOG_INFO("RegisterObserver_RegisterExt_Test_003 end");
}

/**
 * @tc.name: RegisterObserver_RegisterExt_Test_004
 * @tc.desc: Test RegisterObserverExt interface with neither read nor write permission, verifying notification block
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: 
    1. Global DataShareHelper instance g_dataShareHelper is initialized and not null
    2. REGISTER_URI_NOALL (Uri without read or write permission) is predefined
    3. MockDatashareObserver class (with isNotify flag and Clear method) and ValueProxy::Convert are implemented
 * @tc.step:
    1. Obtain the DataShareHelper instance and use ASSERT_NE to ensure it is not null
    2. Create a Uri object with REGISTER_URI_NOALL and a MockDatashareObserver instance
    3. Call RegisterObserverExt with the Uri, observer and isDescendants=true
    4. Create a DataShareValuesBucket, set "name" to "RegisterObserver_RegisterExt_Test_004"
    5. Add the bucket to a vector, convert the vector to VBuckets via ValueProxy::Convert
    6. Create an INSERT-type ChangeInfo with the Uri and VBuckets
    7. Invoke NotifyChangeExt to send the ChangeInfo to the observer
    8. Wait for the observer's notification signal (data.Wait())
    9. Verify the observer's isNotify flag is false (no notification due to missing read/write permissions)
    10. Call Clear() on the observer to reset its state
    11. Unregister the observer using UnregisterObserverExt
 * @tc.expect:
    1. ASSERT_NE confirms the DataShareHelper instance is valid (no null pointer)
    2. RegisterObserverExt executes without crashing, even with no permissions
    3. NotifyChangeExt does not deliver the notification (isNotify remains false) due to missing permissions
    4. UnregisterObserverExt and Clear() complete successfully, leaving no residual state
    5. The test completes without memory leaks or unexpected exceptions
 */
HWTEST_F(DataShareHelperImplRegisterTest, RegisterObserver_RegisterExt_Test_004, TestSize.Level0)
{
    LOG_INFO("RegisterObserver_RegisterExt_Test_004 start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    ASSERT_NE(helper, nullptr);
    Uri uri(REGISTER_URI_NOALL);
    std::shared_ptr<MockDatashareObserver> dataObserver = std::make_shared<MockDatashareObserver>();
    helper->RegisterObserverExt(uri, dataObserver, true);

    DataShare::DataShareValuesBucket valuesBucket;
    valuesBucket.Put("name", "RegisterObserver_RegisterExt_Test_004");

    std::vector<DataShareValuesBucket> Buckets = { valuesBucket };
    DataShareObserver::ChangeInfo::VBuckets extends;
    extends = ValueProxy::Convert(std::move(Buckets));
    DataShareObserver::ChangeInfo uriChanges = { DataShareObserver::ChangeType::INSERT, { uri }, nullptr, 0, extends };
    helper->NotifyChangeExt(uriChanges);

    dataObserver->data.Wait();
    EXPECT_EQ(dataObserver->isNotify, false);
    dataObserver->Clear();

    helper->UnregisterObserverExt(uri, dataObserver);
    LOG_INFO("RegisterObserver_RegisterExt_Test_004 end");
}

/**
 * @tc.name: RegisterObserver_RegisterExt_Test_005
 * @tc.desc: Test RegisterObserverExt interface with invalid Uri, verifying notification block and error handling
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: 
    1. Global DataShareHelper instance g_dataShareHelper is initialized and not null
    2. REGISTER_URI_INVALID (malformed/invalid Uri) is predefined
    3. MockDatashareObserver class (with isNotify flag and Clear method) and ValueProxy::Convert are implemented
 * @tc.step:
    1. Obtain the DataShareHelper instance and use ASSERT_NE to confirm it is not null
    2. Create a Uri object with REGISTER_URI_INVALID and initialize a MockDatashareObserver instance
    3. Call RegisterObserverExt with the invalid Uri, observer and isDescendants=true (registration attempt)
    4. Create a DataShareValuesBucket, set "name" to "RegisterObserver_RegisterExt_Test_005"
    5. Add the bucket to a vector, convert the vector to VBuckets using ValueProxy::Convert
    6. Build an INSERT-type ChangeInfo that includes the invalid Uri and VBuckets
    7. Call NotifyChangeExt to send the ChangeInfo to the observer
    8. Wait for the observer's notification signal (data.Wait())
    9. Check the observer's isNotify flag and verify it is false (no notification for invalid Uri)
    10. Reset the observer's state by calling Clear()
    11. Unregister the observer from the invalid Uri via UnregisterObserverExt
 * @tc.expect:
    1. ASSERT_NE passes, ensuring the DataShareHelper instance is valid
    2. RegisterObserverExt does not crash when given an invalid Uri (graceful error handling)
    3. NotifyChangeExt does not deliver the notification (isNotify remains false) because the Uri is invalid
    4. UnregisterObserverExt completes without errors, even for an invalid Uri
    5. No unexpected exceptions (e.g., Uri parsing errors) are thrown during the test
 */
HWTEST_F(DataShareHelperImplRegisterTest, RegisterObserver_RegisterExt_Test_005, TestSize.Level0)
{
    LOG_INFO("RegisterObserver_RegisterExt_Test_005 start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    ASSERT_NE(helper, nullptr);
    Uri uri(REGISTER_URI_INVALID);
    std::shared_ptr<MockDatashareObserver> dataObserver = std::make_shared<MockDatashareObserver>();
    helper->RegisterObserverExt(uri, dataObserver, true);

    DataShare::DataShareValuesBucket valuesBucket;
    valuesBucket.Put("name", "RegisterObserver_RegisterExt_Test_005");

    std::vector<DataShareValuesBucket> Buckets = { valuesBucket };
    DataShareObserver::ChangeInfo::VBuckets extends;
    extends = ValueProxy::Convert(std::move(Buckets));
    DataShareObserver::ChangeInfo uriChanges = { DataShareObserver::ChangeType::INSERT, { uri },
        nullptr, 0, extends };
    helper->NotifyChangeExt(uriChanges);

    dataObserver->data.Wait();
    EXPECT_EQ(dataObserver->isNotify, false);
    dataObserver->Clear();

    helper->UnregisterObserverExt(uri, dataObserver);
    LOG_INFO("RegisterObserver_RegisterExt_Test_005 end");
}

/**
 * @tc.name: RegisterObserver_RegisterExt_Test_006
 * @tc.desc: Test RegisterObserverExt with mixed valid and invalid (no read permission) Uris, verifying selective
 *           notification
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: 
    1. Global DataShareHelper instance g_dataShareHelper is initialized and not null
    2. REGISTER_URI (valid, with full permissions) and REGISTER_URI_NOREAD (invalid for notifications) are predefined
    3. MockDatashareObserver class and ValueProxy::Convert are implemented
    4. ChangeInfoEqual function is available to compare ChangeInfo objects
 * @tc.step:
    1. Obtain the DataShareHelper instance and use ASSERT_NE to confirm it is not null
    2. Create two Uri objects: uri (REGISTER_URI, valid) and uri2 (REGISTER_URI_NOREAD, no read permission)
    3. Initialize a MockDatashareObserver instance
    4. Call RegisterObserverExt to register the observer to the valid uri (not uri2) with isDescendants=true
    5. Create a DataShareValuesBucket, set "name" to "RegisterObserver_RegisterExt_Test_006"
    6. Add the bucket to a vector, convert the vector to VBuckets via ValueProxy::Convert
    7. Create an INSERT-type ChangeInfo that includes both uris (valid + no read permission) and VBuckets
    8. Call NotifyChangeExt to send the mixed-uri ChangeInfo
    9. Wait for the observer's notification signal (data.Wait())
    10. Create an expected ChangeInfo that only includes the valid uri (excludes uri2) with the same VBuckets
    11. Verify two conditions:
        a. The observer's isNotify flag is true (notification received)
        b. ChangeInfoEqual returns true between the observer's changeInfo_ and the expected ChangeInfo
    12. Call Clear() on the observer to reset its state
    13. Unregister the observer from the valid uri using UnregisterObserverExt
 * @tc.expect:
    1. ASSERT_NE passes, confirming the DataShareHelper instance is valid
    2. The observer is only registered to the valid uri (uri), not uri2
    3. After NotifyChangeExt, isNotify is true (notification delivered)
    4. ChangeInfoEqual confirms the observer only receives the valid uri's ChangeInfo (uri2 is filtered out)
    5. Clear() and UnregisterObserverExt complete without errors, resetting the test environment
    6. No unexpected filtering or incorrect Uri delivery occurs
 */
HWTEST_F(DataShareHelperImplRegisterTest, RegisterObserver_RegisterExt_Test_006, TestSize.Level0)
{
    LOG_INFO("RegisterObserver_RegisterExt_Test_006 start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    ASSERT_NE(helper, nullptr);
    Uri uri(REGISTER_URI);
    Uri uri2(REGISTER_URI_NOREAD);
    std::shared_ptr<MockDatashareObserver> dataObserver = std::make_shared<MockDatashareObserver>();
    helper->RegisterObserverExt(uri, dataObserver, true);

    DataShare::DataShareValuesBucket valuesBucket;
    valuesBucket.Put("name", "RegisterObserver_RegisterExt_Test_006");

    std::vector<DataShareValuesBucket> Buckets = { valuesBucket };
    DataShareObserver::ChangeInfo::VBuckets extends;
    extends = ValueProxy::Convert(std::move(Buckets));
    DataShareObserver::ChangeInfo uriChanges = { DataShareObserver::ChangeType::INSERT, { uri, uri2 },
        nullptr, 0, extends };
    helper->NotifyChangeExt(uriChanges);

    DataShareObserver::ChangeInfo expectedUriChanges = { DataShareObserver::ChangeType::INSERT, { uri },
        nullptr, 0, extends };
    dataObserver->data.Wait();
    EXPECT_EQ(dataObserver->isNotify, true);
    EXPECT_TRUE(ChangeInfoEqual(dataObserver->changeInfo_, expectedUriChanges));
    dataObserver->Clear();

    helper->UnregisterObserverExt(uri, dataObserver);
    LOG_INFO("RegisterObserver_RegisterExt_Test_006 end");
}

/**
 * @tc.name: RegisterObserver_RegisterExt_Test_007
 * @tc.desc: Test RegisterObserverExt with mixed valid Uri and Uri without write permission, verifying selective
 *           notification delivery
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon: 
    1. Global DataShareHelper instance g_dataShareHelper is initialized and not null
    2. REGISTER_URI (valid Uri with full permissions) and REGISTER_URI_NOWRITE are predefined
    3. MockDatashareObserver class is implemented, including:
        - isNotify flag to track notification receipt
        - changeInfo_ to store received change information
        - Clear() method to reset state
        - data.Wait() method to wait for notification signals
    4. ValueProxy::Convert method is available to convert DataShareValuesBucket vector to VBuckets
    5. ChangeInfoEqual function is implemented to compare ChangeInfo objects for equality
 * @tc.step:
    1. Obtain the global DataShareHelper instance and verify it is not null using ASSERT_NE
    2. Initialize a MockDatashareObserver instance via std::make_shared
    3. Register the observer to the valid uri using RegisterObserverExt with isDescendants=true
    4. Create a DataShareValuesBucket and set "name" to "RegisterObserver_RegisterExt_Test_007"
    5. Add the valuesBucket to a vector<DataShareValuesBucket> named Buckets
    6. Convert the Buckets vector to DataShareObserver::ChangeInfo::VBuckets using ValueProxy::Convert
    7. Call NotifyChangeExt with uriChanges to send the mixed notification
    8. Wait for the observer to receive the notification using dataObserver->data.Wait()
    9. Verify two conditions:
        a. dataObserver->isNotify is true (notification was received)
        b. ChangeInfoEqual(dataObserver->changeInfo_, expectedUriChanges) returns true
    10. Reset the observer's state using dataObserver->Clear()
    11. Unregister the observer from uri using UnregisterObserverExt
 * @tc.expect:
    1. The DataShareHelper instance is confirmed not null (ASSERT_NE passes)
    2. RegisterObserverExt successfully registers the observer to the valid uri
    3. After NotifyChangeExt, the observer's isNotify flag is true (notification is received)
    4. The observer's received changeInfo_ matches expectedUriChanges
    5. Clear() resets the observer's state without errors
    6. UnregisterObserverExt completes successfully, removing the observer from the valid uri
    7. No unexpected crashes, memory leaks, or incorrect Uri filtering occur during the test
 */
HWTEST_F(DataShareHelperImplRegisterTest, RegisterObserver_RegisterExt_Test_007, TestSize.Level0)
{
    LOG_INFO("RegisterObserver_RegisterExt_Test_007 start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    ASSERT_NE(helper, nullptr);
    Uri uri(REGISTER_URI);
    Uri uri2(REGISTER_URI_NOWRITE);
    std::shared_ptr<MockDatashareObserver> dataObserver = std::make_shared<MockDatashareObserver>();
    helper->RegisterObserverExt(uri, dataObserver, true);

    DataShare::DataShareValuesBucket valuesBucket;
    valuesBucket.Put("name", "RegisterObserver_RegisterExt_Test_007");

    std::vector<DataShareValuesBucket> Buckets = { valuesBucket };
    DataShareObserver::ChangeInfo::VBuckets extends;
    extends = ValueProxy::Convert(std::move(Buckets));
    DataShareObserver::ChangeInfo uriChanges = { DataShareObserver::ChangeType::INSERT, { uri, uri2 },
        nullptr, 0, extends };
    helper->NotifyChangeExt(uriChanges);

    DataShareObserver::ChangeInfo expectedUriChanges = { DataShareObserver::ChangeType::INSERT, { uri },
        nullptr, 0, extends };
    dataObserver->data.Wait();
    EXPECT_EQ(dataObserver->isNotify, true);
    EXPECT_TRUE(ChangeInfoEqual(dataObserver->changeInfo_, expectedUriChanges));
    dataObserver->Clear();

    helper->UnregisterObserverExt(uri, dataObserver);
    LOG_INFO("RegisterObserver_RegisterExt_Test_007 end");
}
} // namespace DataShare
} // namespace OHOS