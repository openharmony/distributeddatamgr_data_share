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

#include "datashare_helper.h"
#include "datashare_log.h"
#include "datashare_valuebucket_convert.h"
#include "gmock/gmock.h"
#include "iservice_registry.h"

namespace OHOS {
namespace DataShare {
using namespace testing::ext;

static constexpr int STORAGE_MANAGER_MANAGER_ID = 5003;
static constexpr int64_t INTERVAL = 2;
static const std::string DATA_SHARE_URI = "datashare:///com.acts.datasharetest";
std::shared_ptr<DataShare::DataShareHelper> g_dataShareHelper;

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

std::shared_ptr<DataShare::DataShareHelper> CreateDataShareHelper(int32_t systemAbilityId)
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
    return DataShare::DataShareHelper::Creator(remoteObj, DATA_SHARE_URI);
}

void DataShareHelperImplRegisterTest::SetUpTestCase(void)
{
    LOG_INFO("SetUpTestCase invoked");
    g_dataShareHelper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID);
    ASSERT_TRUE(g_dataShareHelper != nullptr);
    int sleepTime = 1;
    sleep(sleepTime);
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

class MockDatashareObserver : public DataShare::DataShareObserver {
public:
    MockDatashareObserver() {}
    ~MockDatashareObserver() {}

    void OnChange(const DataShareObserver::ChangeInfo &changeInfo) override
    {
        changeInfo_ = changeInfo;
        data.Notify(changeInfo);
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
} // namespace DataShare
} // namespace OHOS