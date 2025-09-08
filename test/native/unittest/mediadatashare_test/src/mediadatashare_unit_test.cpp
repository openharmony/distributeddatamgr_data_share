/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#define LOG_TAG "mediadatashare_unit_test"

#include "mediadatashare_unit_test.h"

#include <gtest/gtest.h>
#include <unistd.h>

#include "abs_shared_result_set.h"
#include "accesstoken_kit.h"
#include "dataobs_mgr_changeinfo.h"
#include "datashare_log.h"
#include "datashare_valuebucket_convert.h"
#include "data_proxy_observer_stub.h"
#include "hap_token_info.h"
#include "iservice_registry.h"
#include "rdb_data_ability_utils.h"
#include "system_ability_definition.h"
#include "token_setproc.h"

namespace OHOS {
namespace DataShare {
using namespace testing::ext;
using namespace OHOS::Security::AccessToken;

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
    static constexpr int64_t INTERVAL = 2;
};

class DataShareObserverTest : public DataShare::DataShareObserver {
public:
    DataShareObserverTest() {}
    ~DataShareObserverTest() {}
    
    void OnChange(const ChangeInfo &changeInfo) override
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
    
    ChangeInfo changeInfo_;
    ConditionLock<ChangeInfo> data;
};

constexpr int STORAGE_MANAGER_MANAGER_ID = 5003;
std::string DATA_SHARE_URI = "datashare:///com.acts.datasharetest";
std::string MEDIALIBRARY_DATA_URI = "datashare:///com.acts.datasharetest";
std::string MEDIALIBRARY_DATA_URI_ERROR = "test:///com.acts.datasharetest";
std::string FILE_DATA_URI = "file://com.acts.datasharetest";
std::string NORMALIZE_URI = "normalize+datashare:///com.acts.datasharetest";
std::string DENORMALIZE_URI = "denormalize+datashare:///com.acts.datasharetest";
std::shared_ptr<DataShare::DataShareHelper> g_dataShareHelper;

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

bool MediaDataShareUnitTest::UrisEqual(std::list<Uri> uri1, std::list<Uri> uri2)
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

bool MediaDataShareUnitTest::ValueBucketEqual(const VBuckets& v1, const VBuckets& v2)
{
    if (v1.size() != v2.size()) {
        return false;
    }
    for (size_t i = 0; i < v1.size(); i++) {
        const VBucket& vb1 = v1[i];
        const VBucket& vb2 = v2[i];
        if (vb1.size() != vb2.size()) {
            return false;
        }
        for (const auto& pair1 : vb1) {
            const auto& key = pair1.first;
            const auto& value1 = pair1.second;
            auto it2 = vb2.find(key);
            if (it2 == vb2.end() || it2->second != value1) {
                return false;
            }
        }
    }
    return true;
}

bool MediaDataShareUnitTest::ChangeInfoEqual(const ChangeInfo &changeInfo, const ChangeInfo &expectChangeInfo)
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


void MediaDataShareUnitTest::SetUpTestCase(void)
{
    LOG_INFO("SetUpTestCase invoked");
    g_dataShareHelper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID);
    ASSERT_TRUE(g_dataShareHelper != nullptr);
    int sleepTime = 1;
    sleep(sleepTime);
    
    Uri uri(MEDIALIBRARY_DATA_URI);
    DataShare::DataShareValuesBucket valuesBucket;
    double valueD1 = 20.07;
    valuesBucket.Put("phoneNumber", valueD1);
    valuesBucket.Put("name", "dataShareTest003");
    int value1 = 1001;
    valuesBucket.Put("age", value1);
    int retVal = g_dataShareHelper->Insert(uri, valuesBucket);
    EXPECT_EQ((retVal > 0), true);
    
    valuesBucket.Clear();
    double valueD2 = 20.08;
    valuesBucket.Put("phoneNumber", valueD2);
    valuesBucket.Put("name", "dataShareTest004");
    int value2 = 1000;
    valuesBucket.Put("age", value2);
    retVal = g_dataShareHelper->Insert(uri, valuesBucket);
    EXPECT_EQ((retVal > 0), true);
    
    valuesBucket.Clear();
    double valueD3 = 20.09;
    valuesBucket.Put("phoneNumber", valueD3);
    valuesBucket.Put("name", "dataShareTest005");
    int value3 = 999;
    valuesBucket.Put("age", value3);
    retVal = g_dataShareHelper->Insert(uri, valuesBucket);
    EXPECT_EQ((retVal > 0), true);
    LOG_INFO("SetUpTestCase end");
}

void MediaDataShareUnitTest::TearDownTestCase(void)
{
    LOG_INFO("TearDownTestCase invoked");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    ASSERT_TRUE(g_dataShareHelper != nullptr);
    Uri deleteAssetUri(MEDIALIBRARY_DATA_URI);
    DataShare::DataSharePredicates predicates;
    predicates.GreaterThan("id", 0);
    int retVal = helper->Delete(deleteAssetUri, predicates);
    LOG_INFO("TearDownTestCase Delete retVal: %{public}d", retVal);
    EXPECT_EQ((retVal >= 0), true);
    
    bool result = helper->Release();
    EXPECT_EQ(result, true);
    LOG_INFO("TearDownTestCase end");
}

void MediaDataShareUnitTest::SetUp(void) {}
void MediaDataShareUnitTest::TearDown(void) {}

/**
* @tc.name: MediaDataShare_Predicates_Test_001
* @tc.desc: Verify query with Equals and Limit predicates returns correct result count
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create predicates with Equals("name", "dataShareTest003") and Limit(1, 0)
    2. Execute query with these predicates
    3. Check the returned row count
* @tc.expected: Query returns exactly 1 row
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_Predicates_Test_001, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_Predicates_Test_001::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    Uri uri(MEDIALIBRARY_DATA_URI);
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo("name", "dataShareTest003");
    predicates.Limit(1, 0);
    vector<string> columns;
    auto resultSet = helper->Query(uri, predicates, columns);
    int result = 0;
    if (resultSet != nullptr) {
        resultSet->GetRowCount(result);
    }
    EXPECT_EQ(result, 1);
    LOG_INFO("MediaDataShare_Predicates_Test_001, End");
}

/**
* @tc.name: MediaDataShare_Predicates_Test_002
* @tc.desc: Verify query with NotEqualTo predicate returns correct result count
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create predicates with NotEqualTo("name", "dataShareTest003")
    2. Execute query with these predicates
    3. Check the returned row count
* @tc.expected: Query returns exactly 2 rows
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_Predicates_Test_002, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_Predicates_Test_002::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    DataShare::DataSharePredicates predicates;
    predicates.NotEqualTo("name", "dataShareTest003");
    vector<string> columns;
    Uri uri(MEDIALIBRARY_DATA_URI);
    auto resultSet = helper->Query(uri, predicates, columns);
    int result = 0;
    if (resultSet != nullptr) {
        resultSet->GetRowCount(result);
    }
    EXPECT_EQ(result, 2);
    LOG_INFO("MediaDataShare_Predicates_Test_002, End");
}

/**
* @tc.name: MediaDataShare_Predicates_Test_003
* @tc.desc: Verify query with Contains predicate returns correct result count
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create predicates with Contains("name", "dataShareTest")
    2. Execute query with these predicates
    3. Check the returned row count
* @tc.expected: Query returns exactly 3 rows
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_Predicates_Test_003, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_Predicates_Test_003::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    DataShare::DataSharePredicates predicates;
    predicates.Contains("name", "dataShareTest");
    vector<string> columns;
    Uri uri(MEDIALIBRARY_DATA_URI);
    auto resultSet = helper->Query(uri, predicates, columns);
    int result = 0;
    if (resultSet != nullptr) {
        resultSet->GetRowCount(result);
    }
    EXPECT_EQ(result, 3);
    LOG_INFO("MediaDataShare_Predicates_Test_003, End");
}

/**
* @tc.name: MediaDataShare_Predicates_Test_004
* @tc.desc: Verify query with BeginsWith predicate returns correct result count
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create predicates with BeginsWith("name", "dataShare")
    2. Execute query with these predicates
    3. Check the returned row count
* @tc.expected: Query returns exactly 3 rows
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_Predicates_Test_004, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_Predicates_Test_004::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    DataShare::DataSharePredicates predicates;
    predicates.BeginsWith("name", "dataShare");
    vector<string> columns;
    Uri uri(MEDIALIBRARY_DATA_URI);
    auto resultSet = helper->Query(uri, predicates, columns);
    int result = 0;
    if (resultSet != nullptr) {
        resultSet->GetRowCount(result);
    }
    EXPECT_EQ(result, 3);
    LOG_INFO("MediaDataShare_Predicates_Test_004, End");
}

/**
* @tc.name: MediaDataShare_Predicates_Test_005
* @tc.desc: Verify query with EndsWith predicate returns correct result count
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create predicates with EndsWith("name", "003")
    2. Execute query with these predicates
    3. Check the returned row count
* @tc.expected: Query returns exactly 1 row
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_Predicates_Test_005, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_Predicates_Test_005::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    DataShare::DataSharePredicates predicates;
    predicates.EndsWith("name", "003");
    vector<string> columns;
    Uri uri(MEDIALIBRARY_DATA_URI);
    auto resultSet = helper->Query(uri, predicates, columns);
    int result = 0;
    if (resultSet != nullptr) {
        resultSet->GetRowCount(result);
    }
    EXPECT_EQ(result, 1);
    LOG_INFO("MediaDataShare_Predicates_Test_005, End");
}

/**
* @tc.name: MediaDataShare_Predicates_Test_006
* @tc.desc: Verify query with IsNull predicate returns correct result count
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create predicates with IsNull("name")
    2. Execute query with these predicates
    3. Check the returned row count
* @tc.expected: Query returns exactly 0 rows
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_Predicates_Test_006, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_Predicates_Test_006::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    DataShare::DataSharePredicates predicates;
    predicates.IsNull("name");
    vector<string> columns;
    Uri uri(MEDIALIBRARY_DATA_URI);
    auto resultSet = helper->Query(uri, predicates, columns);
    int result = -1;
    if (resultSet != nullptr) {
        resultSet->GetRowCount(result);
    }
    EXPECT_EQ(result, 0);
    LOG_INFO("MediaDataShare_Predicates_Test_006, End");
}

/**
* @tc.name: MediaDataShare_Predicates_Test_007
* @tc.desc: Verify query with IsNotNull predicate returns correct result count
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create predicates with IsNotNull("name")
    2. Execute query with these predicates
    3. Check the returned row count
* @tc.expected: Query returns exactly 3 rows
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_Predicates_Test_007, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_Predicates_Test_007::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    DataShare::DataSharePredicates predicates;
    predicates.IsNotNull("name");
    vector<string> columns;
    Uri uri(MEDIALIBRARY_DATA_URI);
    auto resultSet = helper->Query(uri, predicates, columns);
    int result = 0;
    if (resultSet != nullptr) {
        resultSet->GetRowCount(result);
    }
    EXPECT_EQ(result, 3);
    LOG_INFO("MediaDataShare_Predicates_Test_007, End");
}

/**
* @tc.name: MediaDataShare_Predicates_Test_008
* @tc.desc: Verify query with Like predicate returns correct result count
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create predicates with Like("name", "%Test003")
    2. Execute query with these predicates
    3. Check the returned row count
* @tc.expected: Query returns exactly 1 row
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_Predicates_Test_008, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_Predicates_Test_008::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    DataShare::DataSharePredicates predicates;
    predicates.Like("name", "%Test003");
    vector<string> columns;
    Uri uri(MEDIALIBRARY_DATA_URI);
    auto resultSet = helper->Query(uri, predicates, columns);
    int result = 0;
    if (resultSet != nullptr) {
        resultSet->GetRowCount(result);
    }
    EXPECT_EQ(result, 1);
    LOG_INFO("MediaDataShare_Predicates_Test_008, End");
}

/**
* @tc.name: MediaDataShare_Predicates_Test_009
* @tc.desc: Verify query with Glob predicate returns correct result count
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create predicates with Glob("name", "dataShareTes?003")
    2. Execute query with these predicates
    3. Check the returned row count
* @tc.expected: Query returns exactly 1 row
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_Predicates_Test_009, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_Predicates_Test_009::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    DataShare::DataSharePredicates predicates;
    predicates.Glob("name", "dataShareTes?003");
    vector<string> columns;
    Uri uri(MEDIALIBRARY_DATA_URI);
    auto resultSet = helper->Query(uri, predicates, columns);
    int result = 0;
    if (resultSet != nullptr) {
        resultSet->GetRowCount(result);
    }
    EXPECT_EQ(result, 1);
    LOG_INFO("MediaDataShare_Predicates_Test_009, End");
}

/**
* @tc.name: MediaDataShare_Predicates_Test_010
* @tc.desc: Verify query with Between predicate returns correct result count
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create predicates with Between("age", "0", "999")
    2. Execute query with these predicates
    3. Check the returned row count
* @tc.expected: Query returns exactly 1 row
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_Predicates_Test_010, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_Predicates_Test_010::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    DataShare::DataSharePredicates predicates;
    predicates.Between("age", "0", "999");
    vector<string> columns;
    Uri uri(MEDIALIBRARY_DATA_URI);
    auto resultSet = helper->Query(uri, predicates, columns);
    int result = 0;
    if (resultSet != nullptr) {
        resultSet->GetRowCount(result);
    }
    EXPECT_EQ(result, 1);
    LOG_INFO("MediaDataShare_Predicates_Test_010, End");
}

/**
* @tc.name: MediaDataShare_Predicates_Test_011
* @tc.desc: Verify query with NotBetween predicate returns correct result count
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create predicates with NotBetween("age", "0", "999")
    2. Execute query with these predicates
    3. Check the returned row count
* @tc.expected: Query returns exactly 2 rows
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_Predicates_Test_011, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_Predicates_Test_011::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    DataShare::DataSharePredicates predicates;
    predicates.NotBetween("age", "0", "999");
    vector<string> columns;
    Uri uri(MEDIALIBRARY_DATA_URI);
    auto resultSet = helper->Query(uri, predicates, columns);
    int result = 0;
    if (resultSet != nullptr) {
        resultSet->GetRowCount(result);
    }
    EXPECT_EQ(result, 2);
    LOG_INFO("MediaDataShare_Predicates_Test_011, End");
}

/**
* @tc.name: MediaDataShare_Predicates_Test_012
* @tc.desc: Verify query with GreaterThan predicate returns correct result count
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create predicates with GreaterThan("age", 999)
    2. Execute query with these predicates
    3. Check the returned row count
* @tc.expected: Query returns exactly 2 rows
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_Predicates_Test_012, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_Predicates_Test_012::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    DataShare::DataSharePredicates predicates;
    predicates.GreaterThan("age", 999);
    vector<string> columns;
    Uri uri(MEDIALIBRARY_DATA_URI);
    auto resultSet = helper->Query(uri, predicates, columns);
    int result = 0;
    if (resultSet != nullptr) {
        resultSet->GetRowCount(result);
    }
    EXPECT_EQ(result, 2);
    LOG_INFO("MediaDataShare_Predicates_Test_012, End");
}

/**
* @tc.name: MediaDataShare_Predicates_Test_013
* @tc.desc: Verify query with LessThan predicate returns correct result count
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create predicates with LessThan("age", 1000)
    2. Execute query with these predicates
    3. Check the returned row count
* @tc.expected: Query returns exactly 1 row
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_Predicates_Test_013, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_Predicates_Test_013::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    DataShare::DataSharePredicates predicates;
    predicates.LessThan("age", 1000);
    vector<string> columns;
    Uri uri(MEDIALIBRARY_DATA_URI);
    auto resultSet = helper->Query(uri, predicates, columns);
    int result = 0;
    if (resultSet != nullptr) {
        resultSet->GetRowCount(result);
    }
    EXPECT_EQ(result, 1);
    LOG_INFO("MediaDataShare_Predicates_Test_013, End");
}

/**
* @tc.name: MediaDataShare_Predicates_Test_014
* @tc.desc: Verify query with GreaterThanOrEqualTo predicate returns correct result count
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create predicates with GreaterThanOrEqualTo("age", 1000)
    2. Execute query with these predicates
    3. Check the returned row count
* @tc.expected: Query returns exactly 2 rows
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_Predicates_Test_014, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_Predicates_Test_014::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    DataShare::DataSharePredicates predicates;
    predicates.GreaterThanOrEqualTo("age", 1000);
    vector<string> columns;
    Uri uri(MEDIALIBRARY_DATA_URI);
    auto resultSet = helper->Query(uri, predicates, columns);
    int result = 0;
    if (resultSet != nullptr) {
        resultSet->GetRowCount(result);
    }
    EXPECT_EQ(result, 2);
    LOG_INFO("MediaDataShare_Predicates_Test_014, End");
}

/**
* @tc.name: MediaDataShare_Predicates_Test_015
* @tc.desc: Verify query with LessThanOrEqualTo predicate returns correct result count
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create predicates with LessThanOrEqualTo("age", 1000)
    2. Execute query with these predicates
    3. Check the returned row count
* @tc.expected: Query returns exactly 2 rows
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_Predicates_Test_015, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_Predicates_Test_015::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    DataShare::DataSharePredicates predicates;
    predicates.LessThanOrEqualTo("age", 1000);
    vector<string> columns;
    Uri uri(MEDIALIBRARY_DATA_URI);
    auto resultSet = helper->Query(uri, predicates, columns);
    int result = 0;
    if (resultSet != nullptr) {
        resultSet->GetRowCount(result);
    }
    EXPECT_EQ(result, 2);
    LOG_INFO("MediaDataShare_Predicates_Test_015, End");
}

/**
* @tc.name: MediaDataShare_Predicates_Test_016
* @tc.desc: Verify query with combined predicates (AND/OR) returns correct result count
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create complex predicates with nested conditions
    2. Execute query with these predicates
    3. Check the returned row count
* @tc.expected: Query returns exactly 1 row
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_Predicates_Test_016, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_Predicates_Test_016::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo("phoneNumber", 20.08)
        ->BeginWrap()
        ->EqualTo("name", "dataShareTest004")
        ->Or()
        ->EqualTo("age", 1000)
        ->EndWrap();
    vector<string> columns;
    Uri uri(MEDIALIBRARY_DATA_URI);
    auto resultSet = helper->Query(uri, predicates, columns);
    int result = 0;
    if (resultSet != nullptr) {
        resultSet->GetRowCount(result);
    }
    EXPECT_EQ(result, 1);
    LOG_INFO("MediaDataShare_Predicates_Test_016, End");
}

/**
* @tc.name: MediaDataShare_Predicates_Test_017
* @tc.desc: Verify query with AND combined predicates returns correct result count
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create predicates combining two EqualTo conditions with AND
    2. Execute query with these predicates
    3. Check the returned row count
* @tc.expected: Query returns exactly 1 row
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_Predicates_Test_017, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_Predicates_Test_017::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo("phoneNumber", 20.08)->And()->EqualTo("name", "dataShareTest004");
    vector<string> columns;
    Uri uri(MEDIALIBRARY_DATA_URI);
    auto resultSet = helper->Query(uri, predicates, columns);
    int result = 0;
    if (resultSet != nullptr) {
        resultSet->GetRowCount(result);
    }
    EXPECT_EQ(result, 1);
    LOG_INFO("MediaDataShare_Predicates_Test_017, End");
}

/**
* @tc.name: MediaDataShare_Predicates_Test_018
* @tc.desc: Verify query with ascending order predicate returns correctly ordered results
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create predicates with OrderByAsc("age")
    2. Execute query with these predicates
    3. Check the name of the first row in results
* @tc.expected: First row contains name "dataShareTest005"
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_Predicates_Test_018, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_Predicates_Test_018::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    DataShare::DataSharePredicates predicates;
    predicates.OrderByAsc("age");
    vector<string> columns;
    Uri uri(MEDIALIBRARY_DATA_URI);
    auto resultSet = helper->Query(uri, predicates, columns);
    int columnIndex = 0;
    std::string stringResult = "";
    if (resultSet != nullptr) {
        resultSet->GoToFirstRow();
        resultSet->GetColumnIndex("name", columnIndex);
        resultSet->GetString(columnIndex, stringResult);
    }
    EXPECT_EQ(stringResult, "dataShareTest005");
    LOG_INFO("MediaDataShare_Predicates_Test_018, End");
}

/**
* @tc.name: MediaDataShare_Predicates_Test_019
* @tc.desc: Verify query with descending order predicate returns correctly ordered results
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create predicates with OrderByDesc("phoneNumber")
    2. Execute query with these predicates
    3. Check the name of the first row in results
* @tc.expected: First row contains name "dataShareTest005"
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_Predicates_Test_019, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_Predicates_Test_019::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    DataShare::DataSharePredicates predicates;
    predicates.OrderByDesc("phoneNumber");
    vector<string> columns;
    Uri uri(MEDIALIBRARY_DATA_URI);
    auto resultSet = helper->Query(uri, predicates, columns);
    int columnIndex = 0;
    std::string stringResult = "";
    if (resultSet != nullptr) {
        resultSet->GoToFirstRow();
        resultSet->GetColumnIndex("name", columnIndex);
        resultSet->GetString(columnIndex, stringResult);
    }
    EXPECT_EQ(stringResult, "dataShareTest005");
    LOG_INFO("MediaDataShare_Predicates_Test_019, End");
}

/**
* @tc.name: MediaDataShare_Predicates_Test_020
* @tc.desc: Verify predicate setting mode can be set and retrieved correctly
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create empty predicates
    2. Set setting mode to PREDICATES_METHOD
    3. Retrieve and check the setting mode
* @tc.expected: Retrieved setting mode matches PREDICATES_METHOD
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_Predicates_Test_020, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_Predicates_Test_020::Start");
    DataShare::DataSharePredicates predicates;
    predicates.SetSettingMode(DataShare::SettingMode::PREDICATES_METHOD);
    int16_t setting = predicates.GetSettingMode();
    EXPECT_EQ(setting, DataShare::SettingMode::PREDICATES_METHOD);
    LOG_INFO("MediaDataShare_Predicates_Test_020, End");
}

/**
* @tc.name: MediaDataShare_Predicates_Test_021
* @tc.desc: Verify predicate operation list is correctly generated
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create predicates with Equals("name", "dataShareTest003")
    2. Retrieve the operation list
    3. Verify operation type and parameters
* @tc.expected: Operation list contains correct type and parameters
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_Predicates_Test_021, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_Predicates_Test_021::Start");
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo("name", "dataShareTest003");

    std::vector<DataShare::OperationItem> operationItems = predicates.GetOperationList();
    DataShare::OperationItem operationItem = operationItems[0];
    EXPECT_EQ(operationItem.operation, DataShare::OperationType::EQUAL_TO);
    DataShare::SingleValue sv1 = operationItem.singleParams[0];
    string param1 = sv1;
    DataShare::SingleValue sv2 = operationItem.singleParams[1];
    string param2 = sv2;
    EXPECT_EQ(param1, "name");
    EXPECT_EQ(param2, "dataShareTest003");
    LOG_INFO("MediaDataShare_Predicates_Test_021, End");
}

/**
* @tc.name: MediaDataShare_Predicates_Test_022
* @tc.desc: Verify where clause can be set and retrieved correctly
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create empty predicates
    2. Set where clause to "`data2` > ?"
    3. Retrieve and verify the where clause
* @tc.expected: Retrieved where clause matches the set value
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_Predicates_Test_022, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_Predicates_Test_023::Start");
    DataShare::DataSharePredicates predicates;
    int res = predicates.SetWhereClause("`data2` > ?");
    EXPECT_EQ(res, 0);
    string clause = predicates.GetWhereClause();
    EXPECT_EQ(clause, "`data2` > ?");
    LOG_INFO("MediaDataShare_Predicates_Test_023, End");
}

/**
* @tc.name: MediaDataShare_Predicates_Test_023
* @tc.desc: Verify where arguments can be set and retrieved correctly
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create empty predicates
    2. Set where arguments with vector containing "-5"
    3. Retrieve and verify the where arguments
* @tc.expected: Retrieved where arguments match the set values
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_Predicates_Test_023, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_Predicates_Test_024::Start");
    DataShare::DataSharePredicates predicates;
    int res = predicates.SetWhereArgs(std::vector<std::string>{ "-5" });
    EXPECT_EQ(res, 0);
    vector<string> args = predicates.GetWhereArgs();
    EXPECT_EQ(args[0], "-5");
    LOG_INFO("MediaDataShare_Predicates_Test_024, End");
}

/**
* @tc.name: MediaDataShare_Predicates_Test_024
* @tc.desc: Verify order clause can be set and retrieved correctly
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create empty predicates
    2. Set order clause to "data3"
    3. Retrieve and verify the order clause
* @tc.expected: Retrieved order clause matches the set value
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_Predicates_Test_024, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_Predicates_Test_025::Start");
    DataShare::DataSharePredicates predicates;
    int res = predicates.SetOrder("data3");
    EXPECT_EQ(res, 0);
    string order = predicates.GetOrder();
    EXPECT_EQ(order, "data3");
    LOG_INFO("MediaDataShare_Predicates_Test_025, End");
}

/**
* @tc.name: MediaDataShare_ValuesBucket_Test_001
* @tc.desc: Verify DataShareValuesBucket basic operations work correctly
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create empty ValuesBucket and verify it's empty
    2. Add a value and verify it's no longer empty
    3. Retrieve and verify the value
    4. Clear the bucket and verify it's empty again
* @tc.expected: All operations work as expected with correct values
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_ValuesBucket_Test_001, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_ValuesBucket_Test_001::Start");
    DataShare::DataShareValuesBucket valuesBucket;
    EXPECT_EQ(valuesBucket.IsEmpty(), true);

    valuesBucket.Put("name", "dataShare_Test_001");
    EXPECT_EQ(valuesBucket.IsEmpty(), false);

    bool isValid;
    DataShare::DataShareValueObject object = valuesBucket.Get("name", isValid);
    string value = object;
    EXPECT_EQ(value, "dataShare_Test_001");

    valuesBucket.Clear();
    EXPECT_EQ(valuesBucket.IsEmpty(), true);
    LOG_INFO("MediaDataShare_ValuesBucket_Test_001 End");
}

/**
* @tc.name: MediaDataShare_ValueObject_Test_001
* @tc.desc: Verify DataShareValueObject handles various data types correctly
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create ValueObjects with different data types (int, int64, double, bool, vector)
    2. Verify copy and move operations
    3. Check that retrieved values match original values
* @tc.expected: All value types are stored and retrieved correctly
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_ValueObject_Test_001, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_ValueObject_Test_001::Start");

    int base = 100;
    DataShare::DataShareValueObject object(base);
    int value = object;
    EXPECT_EQ(value, base);

    int64_t base64 = 100;
    DataShare::DataShareValueObject object64(base64);
    int64_t value64 = object64;
    EXPECT_EQ(value64, base64);

    double baseD = 10.0;
    DataShare::DataShareValueObject objectD(baseD);
    double valueD = objectD;
    EXPECT_EQ(valueD, baseD);

    bool baseB = true;
    DataShare::DataShareValueObject objectB(baseB);
    bool valueB = objectB;
    EXPECT_EQ(valueB, baseB);

    vector<uint8_t> baseV;
    DataShare::DataShareValueObject objectV(baseV);
    vector<uint8_t> valueV = objectV;
    EXPECT_EQ(valueV, baseV);

    DataShare::DataShareValueObject objectCopy(object);
    int valueCopy = objectCopy;
    EXPECT_EQ(valueCopy, value);

    DataShare::DataShareValueObject objectMove(std::move(object));
    int valueMove = objectMove;
    EXPECT_EQ(valueMove, value);

    LOG_INFO("MediaDataShare_ValueObject_Test_001 End");
}

/**
* @tc.name: MediaDataShare_batchInsert_Test_001
* @tc.desc: Verify batch insert operation correctly inserts multiple records
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create two ValuesBuckets with test data
    2. Perform batch insert with these buckets
    3. Verify the number of inserted records
* @tc.expected: Batch insert returns 2 indicating successful insertion of both records
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_batchInsert_Test_001, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_batchInsert_Test_001::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    ASSERT_TRUE(helper != nullptr);
    Uri uri(MEDIALIBRARY_DATA_URI);
    DataShare::DataShareValuesBucket valuesBucket1;
    valuesBucket1.Put("name", "dataShareTest006");
    valuesBucket1.Put("phoneNumber", 20.6);
    DataShare::DataShareValuesBucket valuesBucket2;
    valuesBucket2.Put("name", "dataShareTest007");
    valuesBucket2.Put("phoneNumber", 20.5);
    std::vector<DataShare::DataShareValuesBucket> values;
    values.push_back(valuesBucket1);
    values.push_back(valuesBucket2);
    int result = helper->BatchInsert(uri, values);
    EXPECT_EQ(result, 2);
    LOG_INFO("MediaDataShare_batchInsert_Test_001 End");
}

/**
* @tc.name: MediaDataShare_NormalizeUri_Test_001
* @tc.desc: Verify URI normalization function works correctly
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Call NormalizeUri with media library URI
    2. Compare result with expected normalized URI
* @tc.expected: Normalized URI matches the expected value
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_NormalizeUri_Test_001, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_NormalizeUri_Test_001::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    Uri uri(NORMALIZE_URI);
    Uri uri_media(MEDIALIBRARY_DATA_URI);
    auto normalUri = helper->NormalizeUri(uri_media);
    EXPECT_EQ(normalUri, uri);
    LOG_INFO("MediaDataShare_NormalizeUri_Test_001 End");
}

/**
* @tc.name: MediaDataShare_DenormalizeUri_Test_001
* @tc.desc: Verify URI denormalization function works correctly
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Call DenormalizeUri with media library URI
    2. Compare result with expected denormalized URI
* @tc.expected: Denormalized URI matches the expected value
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_DenormalizeUri_Test_001, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_DenormalizeUri_Test_001::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    Uri uri(DENORMALIZE_URI);
    Uri uri_media(MEDIALIBRARY_DATA_URI);
    auto denormalUri = helper->DenormalizeUri(uri_media);
    EXPECT_EQ(denormalUri, uri);
    LOG_INFO("MediaDataShare_DenormalizeUri_Test_001 End");
}

/**
* @tc.name: MediaDataShare_SingleValue_Test_001
* @tc.desc: Verify SingleValue handles various data types correctly
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create SingleValue objects with different data types
    2. Verify copy and move operations
    3. Check that retrieved values match original values
* @tc.expected: All value types are stored and retrieved correctly
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_SingleValue_Test_001, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_SingleValue_Test_001::Start");
    int base = 100;
    DataShare::SingleValue sv(base);
    int value = sv;
    EXPECT_EQ(value, base);

    int64_t base64 = 100;
    DataShare::SingleValue sv64(base64);
    int64_t value64 = sv64;
    EXPECT_EQ(value64, base64);

    double baseD = 10.0;
    DataShare::SingleValue svD(baseD);
    double valueD = svD;
    EXPECT_DOUBLE_EQ(valueD, baseD);

    bool baseB = true;
    DataShare::SingleValue svB(baseB);
    bool valueB = svB;
    EXPECT_EQ(valueB, baseB);

    string baseS = "dataShare_Test_001";
    DataShare::SingleValue svS(baseS);
    string valueS = svS;
    EXPECT_EQ(valueS, baseS);

    DataShare::SingleValue svCopy(sv);
    int valueCopy = svCopy;
    EXPECT_EQ(valueCopy, value);

    DataShare::SingleValue svMove(std::move(sv));
    int valueMove = svMove;
    EXPECT_EQ(valueMove, value);
    LOG_INFO("MediaDataShare_SingleValue_Test_001 End");
}

/**
* @tc.name: MediaDataShare_MutliValue_Test_001
* @tc.desc: Verify MutliValue handles various collection types correctly
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create MutliValue objects with different collection types
    2. Verify copy and move operations
    3. Check that retrieved values match original values
* @tc.expected: All collections are stored and retrieved correctly
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_MutliValue_Test_001, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_MutliValue_Test_001::Start");
    vector<int> base;
    base.push_back(100);
    DataShare::MutliValue mv(base);
    vector<int> value = mv;
    EXPECT_EQ(value[0], base[0]);

    vector<int64_t> base64;
    base64.push_back(100);
    DataShare::MutliValue mv64(base64);
    vector<int64_t> value64 = mv64;
    EXPECT_EQ(value64[0], base64[0]);

    vector<double> baseD;
    baseD.push_back(10.0);
    DataShare::MutliValue mvD(baseD);
    vector<double> valueD = mvD;
    EXPECT_DOUBLE_EQ(valueD[0], baseD[0]);

    vector<string> baseS;
    baseS.push_back("dataShare_Test_001");
    DataShare::MutliValue mvS(baseS);
    vector<string> valueS = mvS;
    EXPECT_EQ(valueS[0], baseS[0]);

    DataShare::MutliValue mvCopy(mv);
    vector<int> valueCopy = mvCopy;
    EXPECT_EQ(valueCopy[0], value[0]);

    DataShare::MutliValue mvMove(std::move(mv));
    vector<int> valueMove = mvMove;
    EXPECT_EQ(valueMove[0], value[0]);
    LOG_INFO("MediaDataShare_MutliValue_Test_001 End");
}

/**
* @tc.name: MediaDataShare_ResultSet_Test_001
* @tc.desc: Verify default ResultSet behavior with no data
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create empty ResultSet
    2. Attempt various operations (get columns, row count, move to row, get values)
    3. Verify operations return appropriate error codes
* @tc.expected: All operations on empty ResultSet return errors
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_ResultSet_Test_001, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_ResultSet_Test_005::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    DataShare::DataSharePredicates predicates;
    predicates.Contains("name", "dataShareTest");
    DataShare::DataShareResultSet resultSet;
    std::vector<string> names;
    int err = resultSet.GetAllColumnNames(names);
    EXPECT_NE(err, 0);
    int count;
    err = resultSet.GetRowCount(count);
    EXPECT_NE(err, 0);
    err = resultSet.GoToRow(1);
    EXPECT_NE(err, 0);
    std::vector<uint8_t> blob;
    err = resultSet.GetBlob(0, blob);
    EXPECT_NE(err, 0);
    int64_t longValue;
    err = resultSet.GetLong(0, longValue);
    EXPECT_NE(err, 0);
    double doubleValue;
    err = resultSet.GetDouble(0, doubleValue);
    EXPECT_NE(err, 0);

    bool isNull;
    err = resultSet.IsColumnNull(0, isNull);
    EXPECT_NE(err, 0);
    bool flag = resultSet.OnGo(0, 1);
    EXPECT_EQ(flag, false);
    LOG_INFO("MediaDataShare_ResultSet_Test_005, End");
}

/**
* @tc.name: MediaDataShare_ResultSet_Test_002
* @tc.require: None
* @tc.desc: Verify ResultSet column operations work correctly
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Query data for "dataShareTest003"
    2. Verify column name retrieval
    3. Check blob retrieval from result set
    4. Verify all column names can be retrieved
* @tc.expected: All column operations return valid results
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_ResultSet_Test_002, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_ResultSet_Test_002::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    ASSERT_TRUE(helper != nullptr);
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo("name", "dataShareTest003");
    vector<string> columns;
    Uri uri(MEDIALIBRARY_DATA_URI);
    auto resultSet = helper->Query(uri, predicates, columns);
    int columnIndex = 0;
    vector<uint8_t> blob;
    vector<string> columnNames;
    if (resultSet != nullptr) {
        resultSet->GoToFirstRow();
        resultSet->GetColumnIndex("name", columnIndex);
        std::string columnName;
        int err = resultSet->GetColumnName(columnIndex, columnName);
        EXPECT_EQ(err, 0);
        EXPECT_EQ(columnName, "name");
        err = resultSet->GetBlob(columnIndex, blob);
        EXPECT_EQ(err, 0);
        EXPECT_NE(blob.size(), 0);

        err = resultSet->GetAllColumnNames(columnNames);
        EXPECT_EQ(err, 0);
        EXPECT_NE(columnNames.size(), 0);
    }
    LOG_INFO("MediaDataShare_ResultSet_Test_002, End");
}

/**
* @tc.name: MediaDataShare_ResultSet_Test_003
* @tc.desc: Verify ResultSet block operations work correctly
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Query data for "dataShareTest003"
    2. Check if result set has a block
    3. Retrieve, set and fill block
    4. Verify block operations maintain correct reference
* @tc.expected: All block operations work as expected
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_ResultSet_Test_003, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_ResultSet_Test_003::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    ASSERT_TRUE(helper != nullptr);
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo("name", "dataShareTest003");
    vector<string> columns;
    Uri uri(MEDIALIBRARY_DATA_URI);
    auto resultSet = helper->Query(uri, predicates, columns);
    AppDataFwk::SharedBlock *block = nullptr;
    ASSERT_TRUE(resultSet != nullptr);

    bool hasBlock = resultSet->HasBlock();
    EXPECT_EQ(hasBlock, true);
    EXPECT_NE(resultSet->GetBlock(), nullptr);
    block = (resultSet->GetBlock()).get();
    resultSet->SetBlock(block);
    EXPECT_EQ(block, (resultSet->GetBlock()).get());
    resultSet->FillBlock(0, block);
    EXPECT_EQ(block, (resultSet->GetBlock()).get());
    LOG_INFO("MediaDataShare_ResultSet_Test_003, End");
}

/**
* @tc.name: MediaDataShare_ResultSet_Test_004
* @tc.desc: Verify ResultSet navigation works correctly
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Query data with names containing "dataShareTest"
    2. Move to first row
    3. Attempt navigation with OnGo method
* @tc.expected: Navigation operation returns true indicating success
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_ResultSet_Test_004, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_ResultSet_Test_004::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    ASSERT_TRUE(helper != nullptr);
    DataShare::DataSharePredicates predicates;
    predicates.Contains("name", "dataShareTest");
    vector<string> columns;
    Uri uri(MEDIALIBRARY_DATA_URI);
    auto resultSet = helper->Query(uri, predicates, columns);
    if (resultSet != nullptr) {
        resultSet->GoToFirstRow();
        bool ok = resultSet->OnGo(0, 1);
        EXPECT_EQ(ok, true);
    }
    LOG_INFO("MediaDataShare_ResultSet_Test_004, End");
}

/**
* @tc.name: MediaDataShare_ResultSet_Test_005
* @tc.desc: Verify empty ResultSet operations return appropriate errors
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create empty ResultSet
    2. Attempt various operations without data
    3. Verify error codes and return values
* @tc.expected: All operations on empty ResultSet return errors
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_ResultSet_Test_005, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_ResultSet_Test_005::Start");
    DataShare::DataShareResultSet resultSet;
    std::vector<string> names;
    int err = resultSet.GetAllColumnNames(names);
    EXPECT_NE(err, 0);
    int count;
    err = resultSet.GetRowCount(count);
    EXPECT_NE(err, 0);
    err = resultSet.GoToRow(1);
    EXPECT_NE(err, 0);
    std::vector<uint8_t> blob;
    err = resultSet.GetBlob(0, blob);
    EXPECT_NE(err, 0);
    int64_t longValue;
    err = resultSet.GetLong(0, longValue);
    EXPECT_NE(err, 0);
    double doubleValue;
    err = resultSet.GetDouble(0, doubleValue);
    EXPECT_NE(err, 0);

    bool isNull;
    err = resultSet.IsColumnNull(0, isNull);
    EXPECT_NE(err, 0);
    bool flag = resultSet.OnGo(0, 1);
    EXPECT_EQ(flag, false);
    LOG_INFO("MediaDataShare_ResultSet_Test_005, End");
}

/**
* @tc.name: MediaDataShare_ResultSet_Test_006
* @tc.desc: Verify ResultSet data type and value retrieval
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Insert test data with specific age value
    2. Query for the inserted data
    3. Verify data type and value match expectations
    4. Clean up test data
* @tc.expected: Data type is INTEGER and retrieved value matches inserted value
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_ResultSet_Test_006, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_ResultSet_Test_006::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    ASSERT_TRUE(helper != nullptr);
    Uri uri(MEDIALIBRARY_DATA_URI);
    DataShare::DataShareValuesBucket valuesBucket;
    int value = 1112;
    valuesBucket.Put("age", value);
    int retVal = helper->Insert(uri, valuesBucket);
    EXPECT_EQ((retVal > 0), true);

    DataShare::DataSharePredicates predicates;
    predicates.EqualTo("age", value);
    vector<string> columns;
    auto resultSet = helper->Query(uri, predicates, columns);
    int columnIndex = 0;
    int result = 0;
    ASSERT_TRUE(resultSet != nullptr);
    resultSet->GoToFirstRow();
    resultSet->GetColumnIndex("age", columnIndex);
    DataShare::DataType dt;
    resultSet->GetDataType(0, dt);
    EXPECT_EQ(dt, DataShare::DataType::TYPE_INTEGER);
    resultSet->GetInt(columnIndex, result);
    EXPECT_EQ(result, value);

    DataShare::DataSharePredicates deletePredicates;
    deletePredicates.EqualTo("age", 1112);
    retVal = helper->Delete(uri, deletePredicates);
    EXPECT_EQ((retVal >= 0), true);
    LOG_INFO("MediaDataShare_ResultSet_Test_006, End");
}

/**
* @tc.name: Creator_IRemoteObjectNull_Test_001
* @tc.desc: Verify Creator handles null remote object correctly
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Attempt to create DataShareHelper with null IRemoteObject
    2. Check if returned helper is null
* @tc.expected: Creator returns nullptr when remote object is null
*/
HWTEST_F(MediaDataShareUnitTest, Creator_IRemoteObjectNull_Test_001, TestSize.Level0)
{
    LOG_INFO("Creator_IRemoteObjectNull_Test_001::Start");
    sptr<IRemoteObject> remoteObjNull = nullptr;
    auto remoteNull = DataShare::DataShareHelper::Creator(remoteObjNull, MEDIALIBRARY_DATA_URI);
    EXPECT_EQ(remoteNull, nullptr);
    LOG_INFO("Creator_IRemoteObjectNull_Test_001 End");
}

/**
* @tc.name: Creator_UriError_Test_001
* @tc.desc: Verify Creator handles invalid URI correctly
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Get valid remote object from system ability manager
    2. Attempt to create DataShareHelper with invalid URI
    3. Check if returned helper is null
* @tc.expected: Creator returns nullptr when URI is invalid
*/
HWTEST_F(MediaDataShareUnitTest, Creator_UriError_Test_001, TestSize.Level0)
{
    LOG_INFO("Creator_UriError_Test_001::Start");
    auto saManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    EXPECT_NE(saManager, nullptr);
    auto remoteObj = saManager->GetSystemAbility(STORAGE_MANAGER_MANAGER_ID);
    EXPECT_NE(remoteObj, nullptr);
    auto uriError = DataShare::DataShareHelper::Creator(remoteObj, MEDIALIBRARY_DATA_URI_ERROR);
    EXPECT_EQ(uriError, nullptr);
    LOG_INFO("Creator_UriError_Test_001 End");
}

/**
* @tc.name: Insert_ConnectionNull_Test_001
* @tc.desc: Verify operations fail correctly when connection is released
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create DataShareHelper and immediately release it
    2. Attempt insert operation with released connection
    3. Attempt get type operation
    4. Verify error results
* @tc.expected: Insert returns -1 and GetType returns empty string
*/
HWTEST_F(MediaDataShareUnitTest, Insert_ConnectionNull_Test_001, TestSize.Level0)
{
    LOG_INFO("Insert_ConnectionNull_Test_001::Start");
    auto helper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID);
    ASSERT_TRUE(helper != nullptr);
    auto ret = helper->Release();
    EXPECT_TRUE(ret);
    Uri uri(MEDIALIBRARY_DATA_URI);
    DataShare::DataShareValuesBucket valuesBucket;

    valuesBucket.Clear();
    double valueD4 = 20.10;
    valuesBucket.Put("phoneNumber", valueD4);
    valuesBucket.Put("name", "dataShareTest006");
    int value4 = 998;
    valuesBucket.Put("age", value4);
    auto resultInsert = helper->Insert(uri, valuesBucket);
    EXPECT_EQ(resultInsert, -1);

    auto resultGetType = helper->GetType(uri);
    EXPECT_EQ(resultGetType.size(), 0);
    valuesBucket.Clear();
    LOG_INFO("Insert_ConnectionNull_Test_001 End");
}

/**
* @tc.name: MediaDataShare_CRUD_Test_001
* @tc.desc: Verify basic CRUD operations (Create, Read, Update, Delete) work correctly
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Insert a new record with name "Datashare_CRUD_Test001"
    2. Update the record to name "Datashare_CRUD_Test002" using predicates
    3. Query for the updated record to verify the change
    4. Delete the record using predicates
* @tc.expected:
    1.Insert returns a positive ID indicating success
    2.Update returns non-negative value indicating success
    3.Query returns exactly 1 row for the updated record
    4.Delete returns non-negative value indicating success
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_CRUD_Test_001, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_CRUD_Test_001::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    ASSERT_TRUE(helper != nullptr);
    Uri uri(MEDIALIBRARY_DATA_URI);
    DataShare::DataShareValuesBucket valuesBucket;
    valuesBucket.Put("name", "Datashare_CRUD_Test001");
    int retVal = helper->Insert(uri, valuesBucket);
    EXPECT_EQ((retVal > 0), true);

    DataShare::DataSharePredicates predicates;
    predicates.EqualTo("name", "Datashare_CRUD_Test001");

    valuesBucket.Clear();
    valuesBucket.Put("name", "Datashare_CRUD_Test002");
    retVal = helper->Update(uri, predicates, valuesBucket);
    EXPECT_EQ((retVal >= 0), true);

    DataShare::DataSharePredicates queryPredicates;
    queryPredicates.EqualTo("name", "Datashare_CRUD_Test002");
    vector<string> columns;
    auto resultSet = helper->Query(uri, queryPredicates, columns);
    int result = 0;
    if (resultSet != nullptr) {
        resultSet->GetRowCount(result);
    }
    EXPECT_EQ(result, 1);

    DataShare::DataSharePredicates deletePredicates;
    deletePredicates.EqualTo("name", "Datashare_CRUD_Test002'");
    retVal = helper->Delete(uri, deletePredicates);
    EXPECT_EQ((retVal >= 0), true);
    LOG_INFO("MediaDataShare_CRUD_Test_001, End");
}

/**
* @tc.name: MediaDataShare_CRUDEX_Test_001
* @tc.desc: Verify extended CRUD operations with error code returns work correctly
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Insert a new record using InsertEx and verify error code and return value
    2. Update the record using UpdateEx with predicates and verify results
    3. Query for the updated record to confirm changes
    4. Delete the record using DeleteEx and verify results
* @tc.expected:
    1.InsertEx returns error code 0 and non-negative ID
    2.UpdateEx returns error code 0 and non-negative affected rows
    3.Query returns exactly 1 row for the updated record
    4.DeleteEx returns error code 0 and non-negative affected rows
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_CRUDEX_Test_001, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_CRUDEX_Test_001::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    ASSERT_TRUE(helper != nullptr);
    Uri uri(MEDIALIBRARY_DATA_URI);
    DataShare::DataShareValuesBucket valuesBucket;
    valuesBucket.Put("name", "Datashare_CRUDEX_Test001");
    auto [errCode, retVal] = helper->InsertEx(uri, valuesBucket);
    EXPECT_EQ((errCode == 0), true);
    EXPECT_EQ((retVal >= 0), true);
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo("name", "Datashare_CRUDEX_Test001");

    valuesBucket.Clear();
    valuesBucket.Put("name", "Datashare_CRUDEX_Test002");
    auto [errCode1, retVal1] = helper->UpdateEx(uri, predicates, valuesBucket);
    EXPECT_EQ((errCode1 == 0), true);
    EXPECT_EQ((retVal1 >= 0), true);
    DataShare::DataSharePredicates queryPredicates;
    queryPredicates.EqualTo("name", "Datashare_CRUDEX_Test002");
    vector<string> columns;
    auto resultSet = helper->Query(uri, queryPredicates, columns);
    int result = 0;
    if (resultSet != nullptr) {
        resultSet->GetRowCount(result);
    }
    EXPECT_EQ(result, 1);

    DataShare::DataSharePredicates deletePredicates;
    deletePredicates.EqualTo("name", "Datashare_CRUDEX_Test002'");
    auto [errCode2, retVal2] = helper->DeleteEx(uri, deletePredicates);
    EXPECT_EQ((errCode2 == 0), true);
    EXPECT_EQ((retVal2 >= 0), true);
    LOG_INFO("MediaDataShare_CRUDEX_Test_001, End");
}

/**
* @tc.name: MediaDataShare_ImplPredicates_Test_001
* @tc.desc: Verify In predicate generates correct operation items
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create predicates with In("name", {1, 2, 3}) condition
    2. Retrieve the operation items from predicates
    3. Verify operation type, parameters, and values
* @tc.expected:
    1.Exactly 1 operation item is generated
    2.Operation type is correct for In condition
    3.Parameters contain "name" and the correct integer values
    4.Variant indices match expected types (string and integer vector)
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_ImplPredicates_Test_001, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_ImplPredicates_Test_001::Start");
    DataShare::DataSharePredicates predicates;
    vector<int> inColumn;
    inColumn.push_back(1);
    inColumn.push_back(2);
    inColumn.push_back(3);
    predicates.In("name", inColumn);

    std::vector<DataShare::OperationItem> operationItems = predicates.GetOperationList();
    std::string str = std::get<string>(operationItems[0].singleParams[0]);
    std::vector<int> ret = std::get<std::vector<int>>(operationItems[0].multiParams[0]);
    EXPECT_EQ(operationItems.size(), 1);
    // index of variant, 3 is string
    EXPECT_EQ(operationItems[0].singleParams[0].index(), 3);
    EXPECT_EQ(str, "name");
    EXPECT_EQ(operationItems[0].multiParams[0].index(), 1);
    for (int i = 0; i < ret.size(); i++) {
        EXPECT_EQ(ret[i], i + 1);
    }
    operationItems.clear();
    LOG_INFO("MediaDataShare_ImplPredicates_Test_001, End");
}

/**
* @tc.name: MediaDataShare_NotImplPredicates_Test_001
* @tc.desc: Verify multiple predicate combinations generate correct number of operation items
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create predicates with multiple conditions: In, NotIn, Unlike, GroupBy, Distinct, IndexedBy, KeyPrefix, InKeys
    2. Retrieve the operation items from predicates
    3. Verify the total number of operation items
* @tc.expected: Exactly 8 operation items are generated matching the number of conditions
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_NotImplPredicates_Test_001, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_NotImplPredicates_Test_001::Start");
    DataShare::DataSharePredicates predicates;
    vector<string> inColumn;
    inColumn.push_back("dataShare_Test_001");
    inColumn.push_back("dataShare_Test_002");
    predicates.In("name", inColumn);

    vector<string> notInColumn;
    notInColumn.push_back("dataShare_Test_003");
    notInColumn.push_back("dataShare_Test_004");
    predicates.NotIn("name", notInColumn);
    predicates.Unlike("name", "%Test003");

    vector<string> preV;
    preV.push_back("name");
    predicates.GroupBy(preV);
    predicates.Distinct();
    predicates.IndexedBy("name");
    predicates.KeyPrefix("%Test");
    predicates.InKeys(preV);

    std::vector<DataShare::OperationItem> operationItems = predicates.GetOperationList();
    EXPECT_EQ(operationItems.size(), 8);
    LOG_INFO("MediaDataShare_NotImplPredicates_Test_001, End");
}

/**
* @tc.name: MediaDataShare_Observer_001
* @tc.desc: Verify normal registration and operation of non-silent data observer
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Register a data observer for the media library URI
    2. Insert a new record and trigger notification
    3. Delete the record and trigger notification
    4. Unregister the observer
* @tc.expected:
    1.Registration returns 0 indicating success
    2.Insert returns positive ID and notification is triggered
    3.Delete returns positive value and notification is triggered
    4.Unregistration returns 0 indicating success
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_Observer_001, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_Observer_001 start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    ASSERT_TRUE(helper != nullptr);
    Uri uri(MEDIALIBRARY_DATA_URI);
    sptr<IDataAbilityObserverTest> dataObserver = new (std::nothrow) IDataAbilityObserverTest();
    int retVal = helper->RegisterObserver(uri, dataObserver);
    EXPECT_EQ(retVal, 0);

    DataShare::DataShareValuesBucket valuesBucket;
    valuesBucket.Put("name", "Datashare_Observer_Test001");
    retVal = helper->Insert(uri, valuesBucket);
    EXPECT_GT(retVal, 0);
    helper->NotifyChange(uri);

    DataShare::DataSharePredicates deletePredicates;
    deletePredicates.EqualTo("name", "Datashare_Observer_Test001");
    retVal = helper->Delete(uri, deletePredicates);
    EXPECT_GT(retVal, 0);
    helper->NotifyChange(uri);
    retVal = helper->UnregisterObserver(uri, dataObserver);
    EXPECT_EQ(retVal, 0);
    LOG_INFO("MediaDataShare_Observer_001 end");
}

/**
* @tc.name: MediaDataShare_ReregisterObserver_001
* @tc.desc: Verify error handling when re-registering an existing observer
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Register a data observer for the media library URI
    2. Attempt to register the same observer again
    3. Perform insert and delete operations with notifications
    4. Unregister the observer
* @tc.expected:
    1.First registration returns 0 indicating success
    2.Second registration returns E_REGISTER_ERROR
    3.Insert and delete operations complete successfully
    4.Unregistration returns 0 indicating success
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_ReregisterObserver_001, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_ReregisterObserver_001 start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    EXPECT_NE(helper, nullptr);
    Uri uri(MEDIALIBRARY_DATA_URI);
    sptr<IDataAbilityObserverTest> dataObserver = new (std::nothrow) IDataAbilityObserverTest();
    int retVal = helper->RegisterObserver(uri, dataObserver);
    EXPECT_EQ(retVal, 0);
    retVal = helper->RegisterObserver(uri, dataObserver);
    EXPECT_EQ(retVal, E_REGISTER_ERROR);

    DataShare::DataShareValuesBucket valuesBucket;
    valuesBucket.Put("name", "MediaDataShare_ReregisterObserver_001");
    retVal = helper->Insert(uri, valuesBucket);
    EXPECT_GT(retVal, 0);
    helper->NotifyChange(uri);

    DataShare::DataSharePredicates deletePredicates;
    deletePredicates.EqualTo("name", "MediaDataShare_ReregisterObserver_001");
    retVal = helper->Delete(uri, deletePredicates);
    EXPECT_GT(retVal, 0);
    helper->NotifyChange(uri);
    retVal = helper->UnregisterObserver(uri, dataObserver);
    EXPECT_EQ(retVal, 0);
    LOG_INFO("MediaDataShare_ReregisterObserver_001 end");
}

/**
* @tc.name: MediaDataShare_ObserverExt_001
* @tc.desc: Verify observer extension functionality for data change notifications
* @tc.type: FUNC
* @tc.require: issueIC8OCN
* @tc.precon: DataShareHelper instance is properly initialized and MEDIALIBRARY_DATA_URI is accessible
* @tc.step:
    1. Get DataShareHelper instance and verify it's not null
    2. Create test URI and DataShareObserverTest instance
    3. Register observer extension with the URI
    4. Insert test data and notify INSERT change
    5. Verify observer receives correct INSERT change information
    6. Insert data to descendant URI and notify INSERT change
    7. Verify observer receives correct descendant INSERT change information
    8. Delete test data and notify DELETE change with additional data
    9. Verify observer receives correct DELETE change information
    10. Unregister the observer
* @tc.expect:
    1. DataShareHelper instance is successfully obtained
    2. Test data is inserted successfully (retVal > 0)
    3. Observer receives accurate INSERT change information for main URI
    4. Observer receives accurate INSERT change information for descendant URI
    5. Observer receives accurate DELETE change information including additional data
    6. All operations complete without errors
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_ObserverExt_001, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_ObserverExt_001 start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    ASSERT_TRUE(helper != nullptr);
    Uri uri(MEDIALIBRARY_DATA_URI);
    std::shared_ptr<DataShareObserverTest> dataObserver = std::make_shared<DataShareObserverTest>();
    helper->RegisterObserverExt(uri, dataObserver, true);
    DataShare::DataShareValuesBucket valuesBucket;
    valuesBucket.Put("name", "Datashare_Observer_Test001");
    int retVal = helper->Insert(uri, valuesBucket);
    EXPECT_EQ((retVal > 0), true);
    ChangeInfo uriChanges = { DataShareObserver::ChangeType::INSERT, { uri } };
    helper->NotifyChangeExt(uriChanges);

    dataObserver->data.Wait();
    EXPECT_TRUE(ChangeInfoEqual(dataObserver->changeInfo_, uriChanges));
    dataObserver->Clear();

    Uri descendantsUri(MEDIALIBRARY_DATA_URI + "/com.ohos.example");
    helper->Insert(descendantsUri, valuesBucket);
    EXPECT_EQ((retVal > 0), true);
    ChangeInfo descendantsChanges = { DataShareObserver::ChangeType::INSERT, { descendantsUri } };
    helper->NotifyChangeExt(descendantsChanges);

    dataObserver->data.Wait();
    EXPECT_TRUE(ChangeInfoEqual(dataObserver->changeInfo_, descendantsChanges));
    dataObserver->Clear();

    DataShare::DataSharePredicates deletePredicates;
    deletePredicates.EqualTo("name", "Datashare_Observer_Test001");
    retVal = helper->Delete(uri, deletePredicates);
    EXPECT_EQ((retVal >= 0), true);
    char data[] = { 0x01, 0x02, 0x03, 0x04, 0x05 };
    ChangeInfo delChanges = { DataShareObserver::ChangeType::DELETE, { uri }, data, sizeof(data) / sizeof(data[0]) };
    helper->NotifyChangeExt(delChanges);

    dataObserver->data.Wait();
    EXPECT_TRUE(ChangeInfoEqual(dataObserver->changeInfo_, delChanges));
    dataObserver->Clear();

    helper->UnregisterObserverExt(uri, dataObserver);
    LOG_INFO("MediaDataShare_ObserverExt_001 end");
}

/**
* @tc.name: MediaDataShare_UnregisterObserverExt_001
* @tc.desc: Verify unregister functionality of observer extension
* @tc.type: FUNC
* @tc.require: issueIC8OCN
* @tc.precon: DataShareHelper instance is properly initialized and MEDIALIBRARY_DATA_URI is accessible
* @tc.step:
    1. Get DataShareHelper instance and verify it's not null
    2. Create test URI and DataShareObserverTest instance
    3. Register observer extension with the URI
    4. Insert test data and notify INSERT change
    5. Verify observer receives correct change information
    6. Unregister the observer from the URI
    7. Notify another DELETE change
    8. Verify observer no longer receives change information
* @tc.expect:
    1. DataShareHelper instance is successfully obtained
    2. Test data is inserted successfully (retVal > 0)
    3. Observer receives accurate change information before unregistration
    4. After unregistration, observer does NOT receive change notifications
    5. All operations complete without errors
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_UnregisterObserverExt_001, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_UnregisterObserverExt_001 start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    ASSERT_TRUE(helper != nullptr);
    Uri uri(MEDIALIBRARY_DATA_URI);
    std::shared_ptr<DataShareObserverTest> dataObserver = std::make_shared<DataShareObserverTest>();
    helper->RegisterObserverExt(uri, dataObserver, true);

    DataShare::DataShareValuesBucket valuesBucket;
    valuesBucket.Put("name", "Datashare_Observer_Test001");
    int retVal = helper->Insert(uri, valuesBucket);
    EXPECT_EQ((retVal > 0), true);
    ChangeInfo uriChanges = { DataShareObserver::ChangeType::INSERT, { uri } };
    helper->NotifyChangeExt(uriChanges);

    dataObserver->data.Wait();
    EXPECT_TRUE(ChangeInfoEqual(dataObserver->changeInfo_, uriChanges));
    dataObserver->Clear();

    helper->UnregisterObserverExt(uri, dataObserver);
    helper->NotifyChangeExt({ DataShareObserver::ChangeType::DELETE, { uri } });

    dataObserver->data.Wait();
    EXPECT_FALSE(ChangeInfoEqual(dataObserver->changeInfo_, uriChanges));
    dataObserver->Clear();
    LOG_INFO("MediaDataShare_UnregisterObserverExt_001 end");
}

/**
* @tc.name: MediaDataShare_ToAbsSharedResultSet_Test_001
* @tc.desc: Test conversion of DataShare query result to AbsSharedResultSet
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Get the DataShare helper instance
    2. Create predicates to filter data by name "dataShareTest003"
    3. Query the media library URI with the predicates and empty columns
    4. Convert the query result to AbsSharedResultSet using RdbDataAbilityUtils
    5. Get the row count from the converted result set
* @tc.expect:
    1.The converted AbsSharedResultSet is not null
    2.The row count of the result set is 1 (matches the expected test data)
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_ToAbsSharedResultSet_Test_001, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_ToAbsSharedResultSet_Test_001::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo("name", "dataShareTest003");
    vector<string> columns;
    Uri uri(MEDIALIBRARY_DATA_URI);
    auto resultSet = helper->Query(uri, predicates, columns);
    std::shared_ptr<NativeRdb::AbsSharedResultSet> absSharedResultSet =
        RdbDataAbilityAdapter::RdbDataAbilityUtils::ToAbsSharedResultSet(resultSet);
    int rowCount = 0;
    if (absSharedResultSet != nullptr) {
        absSharedResultSet->GetRowCount(rowCount);
    }
    EXPECT_EQ(rowCount, 1);
    LOG_INFO("MediaDataShare_ToAbsSharedResultSet_Test_001 End");
}

/**
* @tc.name: MediaDataShare_ExecuteBatch_Test_001
* @tc.desc: Test batch execution of INSERT and DELETE operations via DataShare helper
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Get the DataShare helper instance
    2. Create an INSERT operation statement with test data and predicates
    3. Create a DELETE operation statement with test data and predicates
    4. Add both statements to a batch list
    5. Execute the batch operation using helper->ExecuteBatch()
* @tc.expect: Batch execution returns 0 (success status)
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_ExecuteBatch_Test_001, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_ExecuteBatch_Test_001::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;

    std::vector<DataShare::OperationStatement> statements;
    DataShare::OperationStatement statement1;
    statement1.operationType = Operation::INSERT;
    statement1.uri = "datashare:///uri1";
    DataShare::DataShareValuesBucket valuesBucket;
    valuesBucket.Put("DB_NUM", 150);
    valuesBucket.Put("DB_TITLE", "ExecuteBatch_Test002");
    statement1.valuesBucket = valuesBucket;
    DataShare::DataSharePredicates predicates;
    predicates.SetWhereClause("`DB_NUM` > 100");
    statement1.predicates = predicates;
    statements.emplace_back(statement1);

    DataShare::OperationStatement statement2;
    statement2.operationType = Operation::DELETE;
    statement2.uri = "datashareproxy://com.uri2";
    DataShare::DataShareValuesBucket valuesBucket1;
    valuesBucket1.Put("DB_TITLE2", "ExecuteBatch_Test002");
    statement2.valuesBucket = valuesBucket1;
    DataShare::DataSharePredicates predicates1;
    predicates1.SetWhereClause("`DB_TITLE` = ExecuteBatch_Test002");
    statement2.predicates = predicates1;
    statements.emplace_back(statement2);

    DataShare::ExecResultSet resultSet;
    auto ret = helper->ExecuteBatch(statements, resultSet);
    EXPECT_EQ(ret, 0);
    LOG_INFO("MediaDataShare_ExecuteBatch_Test_001 End");
}

/**
* @tc.name: MediaDataShare_InsertExt_Test_001
* @tc.desc: Test extended insertion functionality (InsertExt) of DataShare helper
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Get the DataShare helper instance and verify it is not null
    2. Create a URI for media library data
    3. Create a ValuesBucket with test data (name="Datashare_CRUD_Test001")
    4. Call InsertExt with the URI, ValuesBucket, and result string
* @tc.expect: InsertExt returns 0 (success status)
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_InsertExt_Test_001, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_InsertExt_Test_001::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    ASSERT_TRUE(helper != nullptr);
    Uri uri(MEDIALIBRARY_DATA_URI);
    DataShare::DataShareValuesBucket valuesBucket;
    valuesBucket.Put("name", "Datashare_CRUD_Test001");
    std::string str;
    int ret = helper->InsertExt(uri, valuesBucket, str);
    EXPECT_EQ(ret, 0);
    LOG_INFO("MediaDataShare_InsertExt_Test_001 End");
}

/**
* @tc.name: MediaDataShare_TransferUri_Test_001
* @tc.desc: Test URI transfer functionality and DataShare helper creation for file data URI
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create a file data URI
    2. Get the SystemAbilityManager instance and verify it is not null
    3. Get the Storage Manager system ability remote object and verify it is not null
    4. Create a DataShare helper using the remote object and file data URI
* @tc.expect:
    1.SystemAbilityManager instance is not null
    2.Storage Manager remote object is not null
    3.DataShare helper instance is successfully created (not null)
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_TransferUri_Test_001, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_TransferUri_Test_001::Start");
    Uri uri(FILE_DATA_URI);

    auto saManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    EXPECT_NE(saManager, nullptr);
    if (saManager == nullptr) {
        LOG_ERROR("GetSystemAbilityManager get samgr failed.");
    }
    auto remoteObj = saManager->GetSystemAbility(STORAGE_MANAGER_MANAGER_ID);
    EXPECT_NE(remoteObj, nullptr);
    if (remoteObj == nullptr) {
        LOG_ERROR("GetSystemAbility service failed.");
    }
    std::shared_ptr<DataShare::DataShareHelper> helper = DataShare::DataShareHelper::Creator(remoteObj, FILE_DATA_URI);
    EXPECT_NE(helper, nullptr);
    LOG_INFO("MediaDataShare_TransferUri_Test_001 End");
}

/**
* @tc.name: ControllerTest_HelperInsertExtControllerNullTest_001
* @tc.desc: Test InsertExt operation when DataShare controller is released (null)
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create a DataShare helper instance and verify it is not null
    2. Create a test URI and release the helper's controller
    3. Create a ValuesBucket with test data (phoneNumber=20.07, name="dataShareTest003", age=1001)
    4. Call InsertExt with the URI, ValuesBucket, and result string
* @tc.expect: InsertExt returns a negative value (failure due to released controller)
*/
HWTEST_F(MediaDataShareUnitTest, ControllerTest_HelperInsertExtControllerNullTest_001, TestSize.Level0)
{
    LOG_INFO("ControllerTest_HelperInsertExtControllerNullTest_001::Start");
    auto helper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID);
    ASSERT_TRUE(helper != nullptr);
    Uri uri(DATA_SHARE_URI);
    helper->Release();
    DataShare::DataShareValuesBucket valuesBucket;
    double valueD1 = 20.07;
    valuesBucket.Put("phoneNumber", valueD1);
    valuesBucket.Put("name", "dataShareTest003");
    int value1 = 1001;
    valuesBucket.Put("age", value1);
    std::string result;
    int retVal = helper->InsertExt(uri, valuesBucket, result);
    EXPECT_EQ((retVal < 0), true);
    LOG_INFO("ControllerTest_HelperInsertExtControllerNullTest_001::End");
}

/**
* @tc.name: ControllerTest_HelperUpdateControllerNullTest_001
* @tc.desc: Test Update operation when DataShare controller is released (null)
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create a DataShare helper instance and verify it is not null
    2. Create a test URI and release the helper's controller
    3. Create predicates to filter data by name "Datashare_CRUD_Test001"
    4. Create a ValuesBucket to update name to "Datashare_CRUD_Test002"
    5. Call Update with the URI, predicates, and ValuesBucket
* @tc.expect: Update returns a negative value (failure due to released controller)
*/
HWTEST_F(MediaDataShareUnitTest, ControllerTest_HelperUpdateControllerNullTest_001, TestSize.Level0)
{
    LOG_INFO("ControllerTest_HelperUpdateControllerNullTest_001::Start");
    auto helper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID);
    ASSERT_TRUE(helper != nullptr);
    Uri uri(DATA_SHARE_URI);
    helper->Release();
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo("name", "Datashare_CRUD_Test001");
    DataShare::DataShareValuesBucket valuesBucket;
    valuesBucket.Put("name", "Datashare_CRUD_Test002");
    int retVal = helper->Update(uri, predicates, valuesBucket);
    EXPECT_EQ((retVal < 0), true);
    LOG_INFO("ControllerTest_HelperUpdateControllerNullTest_001::End");
}

/**
* @tc.name: ControllerTest_HelperDeleteControllerNullTest_001
* @tc.desc: Test Delete operation when DataShare controller is released (null)
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create a DataShare helper instance and verify it is not null
    2. Create a test URI and release the helper's controller
    3. Create predicates to filter data by age=1112
    4. Call Delete with the URI and predicates
* @tc.expect: Delete returns a negative value (failure due to released controller)
*/
HWTEST_F(MediaDataShareUnitTest, ControllerTest_HelperDeleteControllerNullTest_001, TestSize.Level0)
{
    LOG_INFO("ControllerTest_HelperDeleteControllerNullTest_001::Start");
    auto helper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID);
    ASSERT_TRUE(helper != nullptr);
    Uri uri(DATA_SHARE_URI);
    helper->Release();
    DataShare::DataSharePredicates deletePredicates;
    deletePredicates.EqualTo("age", 1112);
    int retVal = helper->Delete(uri, deletePredicates);
    EXPECT_EQ((retVal < 0), true);
    LOG_INFO("ControllerTest_HelperDeleteControllerNullTest_001::End");
}

/**
* @tc.name: ControllerTest_HelperQueryControllerNullTest_001
* @tc.desc: Test Query operation when DataShare controller is released (null)
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create a DataShare helper instance and verify it is not null
    2. Create a test URI and release the helper's controller
    3. Create predicates to filter data by name "dataShareTest003" with limit 1
    4. Call Query with the URI, predicates, and empty columns
* @tc.expect: Query returns a null result set (failure due to released controller)
*/
HWTEST_F(MediaDataShareUnitTest, ControllerTest_HelperQueryControllerNullTest_001, TestSize.Level0)
{
    LOG_INFO("ControllerTest_HelperQueryControllerNullTest_001::Start");
    auto helper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID);
    ASSERT_TRUE(helper != nullptr);
    Uri uri(DATA_SHARE_URI);
    helper->Release();
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo("name", "dataShareTest003");
    predicates.Limit(1, 0);
    vector<string> columns;
    auto resultSet = helper->Query(uri, predicates, columns);
    EXPECT_EQ(resultSet, nullptr);
    LOG_INFO("ControllerTest_HelperQueryControllerNullTest_001::End");
}

/**
* @tc.name: ControllerTest_HelperBatchInsertControllerNullTest_001
* @tc.desc: Test BatchInsert operation when DataShare controller is released (null)
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create a DataShare helper instance and verify it is not null
    2. Create a test URI and release the helper's controller
    3. Create two ValuesBuckets with test data (name and phoneNumber)
    4. Add both buckets to a batch list
    5. Call BatchInsert with the URI and batch list
* @tc.expect: BatchInsert returns a negative value (failure due to released controller)
*/
HWTEST_F(MediaDataShareUnitTest, ControllerTest_HelperBatchInsertControllerNullTest_001, TestSize.Level0)
{
    LOG_INFO("ControllerTest_HelperBatchInsertControllerNullTest_001::Start");
    auto helper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID);
    ASSERT_TRUE(helper != nullptr);
    Uri uri(DATA_SHARE_URI);
    helper->Release();
    DataShare::DataShareValuesBucket valuesBucket1;
    valuesBucket1.Put("name", "dataShareTest006");
    valuesBucket1.Put("phoneNumber", 20.6);
    DataShare::DataShareValuesBucket valuesBucket2;
    valuesBucket2.Put("name", "dataShareTest007");
    valuesBucket2.Put("phoneNumber", 20.5);
    std::vector<DataShare::DataShareValuesBucket> values;
    values.push_back(valuesBucket1);
    values.push_back(valuesBucket2);
    int result = helper->BatchInsert(uri, values);
    EXPECT_EQ((result < 0), true);
    LOG_INFO("ControllerTest_HelperBatchInsertControllerNullTest_001::End");
}

/**
* @tc.name: ControllerTest_HelperExecuteBatchControllerNullTest_001
* @tc.desc: Test ExecuteBatch operation when DataShare controller is released (null)
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create a DataShare helper instance and verify it is not null
    2. Create a test URI and release the helper's controller
    3. Create an INSERT operation statement with test data and predicates
    4. Create a DELETE operation statement with test data and predicates
    5. Add both statements to a batch list
    6. Call ExecuteBatch with the batch list and result set
* @tc.expect: ExecuteBatch returns a negative value (failure due to released controller)
*/
HWTEST_F(MediaDataShareUnitTest, ControllerTest_HelperExecuteBatchControllerNullTest_001, TestSize.Level0)
{
    LOG_INFO("ControllerTest_HelperExecuteBatchControllerNullTest_001::Start");
    auto helper= CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID);
    ASSERT_TRUE(helper != nullptr);
    Uri uri(DATA_SHARE_URI);
    helper->Release();
    std::vector<DataShare::OperationStatement> statements;
    DataShare::OperationStatement statement1;
    statement1.operationType = Operation::INSERT;
    statement1.uri = "datashare:///uri1";
    DataShare::DataShareValuesBucket valuesBucket;
    valuesBucket.Put("DB_NUM", 150);
    valuesBucket.Put("DB_TITLE", "ExecuteBatch_Test002");
    statement1.valuesBucket = valuesBucket;
    DataShare::DataSharePredicates predicates;
    predicates.SetWhereClause("`DB_NUM` > 100");
    statement1.predicates = predicates;
    statements.emplace_back(statement1);

    DataShare::OperationStatement statement2;
    statement2.operationType = Operation::DELETE;
    statement2.uri = "datashareproxy://com.uri2";
    DataShare::DataShareValuesBucket valuesBucket1;
    valuesBucket1.Put("DB_TITLE2", "ExecuteBatch_Test002");
    statement2.valuesBucket = valuesBucket1;
    DataShare::DataSharePredicates predicates1;
    predicates1.SetWhereClause("`DB_TITLE` = ExecuteBatch_Test002");
    statement2.predicates = predicates1;
    statements.emplace_back(statement2);

    DataShare::ExecResultSet resultSet;
    auto ret = helper->ExecuteBatch(statements, resultSet);
    EXPECT_EQ((ret < 0), true);
    LOG_INFO("ControllerTest_HelperExecuteBatchControllerNullTest_001::End");
}

/**
* @tc.name: ControllerTest_HelperRegisterObserverControllerNullTest_001
* @tc.desc: Test RegisterObserver, Insert, NotifyChange, Delete
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create a DataShare helper instance and verify it is not null
    2. Create a media library URI and release the helper's controller
    3. Create an IDataAbilityObserverTest instance
    4. Call RegisterObserver with the URI and observer, check the return value
    5. Insert test data into the URI and check the return value
    6. Notify a change for the URI
    7. Delete the test data using predicates and check the return value
    8. Notify a change again for the URI
    9. Call UnregisterObserver with the URI and observer, check the return value
* @tc.expect:
    1.RegisterObserver returns E_HELPER_DIED
    2.Insert returns a negative value (failure)
    3.Delete returns a negative value (failure)
    4.UnregisterObserver returns E_HELPER_DIED
*/
HWTEST_F(MediaDataShareUnitTest, ControllerTest_HelperRegisterObserverControllerNullTest_001, TestSize.Level0)
{
    LOG_INFO("ControllerTest_HelperRegisterObserverControllerNullTest_001 start");
    auto helper= CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID);
    ASSERT_TRUE(helper != nullptr);
    Uri uri(MEDIALIBRARY_DATA_URI);
    helper->Release();
    sptr<IDataAbilityObserverTest> dataObserver = new (std::nothrow) IDataAbilityObserverTest();
    int retVal = helper->RegisterObserver(uri, dataObserver);
    EXPECT_EQ(retVal, E_HELPER_DIED);
    
    DataShare::DataShareValuesBucket valuesBucket;
    valuesBucket.Put("name", "Datashare_Observer_Test001");
    retVal = helper->Insert(uri, valuesBucket);
    EXPECT_EQ((retVal < 0), true);
    helper->NotifyChange(uri);
    
    DataShare::DataSharePredicates deletePredicates;
    deletePredicates.EqualTo("name", "Datashare_Observer_Test001");
    retVal = helper->Delete(uri, deletePredicates);
    EXPECT_EQ((retVal < 0), true);
    helper->NotifyChange(uri);
    retVal = helper->UnregisterObserver(uri, dataObserver);
    EXPECT_EQ(retVal, E_HELPER_DIED);
    LOG_INFO("ControllerTest_HelperRegisterObserverControllerNullTest_001 end");
}

/**
* @tc.name: MediaDataShare_ObserverExt_002
* @tc.desc: Test DataShare observer extension for batch INSERT operation notification with extended data
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Get the DataShare helper instance and verify it is not null
    2. Create a media library URI and a DataShareObserverTest instance
    3. Register the observer extension with the URI
    4. Create two ValuesBuckets with test data and batch insert them
    5. Convert the batch data to extended format for change info
    6. Notify an INSERT change with the extended data
    7. Wait for observer notification and verify the change info matches
    8. Unregister the observer extension
* @tc.expect:
    1.Batch insertion returns a positive value (success)
    2.Observer correctly receives and matches INSERT change info with extended data
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_ObserverExt_002, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_ObserverExt_002 start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    ASSERT_TRUE(helper != nullptr);
    Uri uri(MEDIALIBRARY_DATA_URI);
    std::shared_ptr<DataShareObserverTest> dataObserver = std::make_shared<DataShareObserverTest>();
    helper->RegisterObserverExt(uri, dataObserver, false);
    DataShare::DataShareValuesBucket valuesBucket1;
    DataShare::DataShareValuesBucket valuesBucket2;
    valuesBucket1.Put("name", "Datashare_Observer_Test001");
    valuesBucket2.Put("name", "Datashare_Observer_Test002");
    std::vector<DataShareValuesBucket> VBuckets = {valuesBucket1, valuesBucket2};
    int retVal = helper->BatchInsert(uri, VBuckets);
    EXPECT_EQ((retVal > 0), true);
    DataShareObserver::ChangeInfo::VBuckets extends;
    extends = ValueProxy::Convert(std::move(VBuckets));
    ChangeInfo uriChanges = { DataShareObserver::ChangeType::INSERT, { uri }, nullptr, 0, extends};
    helper->NotifyChangeExt(uriChanges);

    dataObserver->data.Wait();
    EXPECT_TRUE(ChangeInfoEqual(dataObserver->changeInfo_, uriChanges));
    dataObserver->Clear();

    helper->UnregisterObserverExt(uri, dataObserver);
    LOG_INFO("MediaDataShare_ObserverExt_002 end");
}

/**
* @tc.name: MediaDataShare_ObserverExt_003
* @tc.desc: Test DataShare observer extension for UPDATE operation notification with extended data
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Get the DataShare helper instance and verify it is not null
    2. Create a media library URI and a DataShareObserverTest instance
    3. Register the observer extension with the URI
    4. Create a ValuesBucket with update data (name="Datashare_Observer_Test003")
    5. Update existing data filtered by predicates (name="Datashare_Observer_Test002")
    6. Convert the update data to extended format for change info
    7. Notify an UPDATE change with the extended data
    8. Wait for observer notification and verify the change info matches
    9. Unregister the observer extension
* @tc.expect:
    1.Update operation returns a positive value (success)
    2.Observer correctly receives and matches UPDATE change info with extended data
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_ObserverExt_003, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_ObserverExt_003 start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    ASSERT_TRUE(helper != nullptr);
    Uri uri(MEDIALIBRARY_DATA_URI);
    std::shared_ptr<DataShareObserverTest> dataObserver = std::make_shared<DataShareObserverTest>();
    helper->RegisterObserverExt(uri, dataObserver, false);
    DataShare::DataShareValuesBucket valuesBucket;
    valuesBucket.Put("name", "Datashare_Observer_Test003");
    std::vector<DataShareValuesBucket> VBuckets = {valuesBucket};
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo("name", "Datashare_Observer_Test002");
    int retVal = helper->Update(uri, predicates, valuesBucket);
    EXPECT_EQ((retVal > 0), true);
    DataShareObserver::ChangeInfo::VBuckets extends;
    extends = ValueProxy::Convert(std::move(VBuckets));
    ChangeInfo uriChanges = { DataShareObserver::ChangeType::UPDATE, { uri }, nullptr, 0, extends};
    helper->NotifyChangeExt(uriChanges);

    dataObserver->data.Wait();
    EXPECT_TRUE(ChangeInfoEqual(dataObserver->changeInfo_, uriChanges));
    dataObserver->Clear();

    helper->UnregisterObserverExt(uri, dataObserver);
    LOG_INFO("MediaDataShare_ObserverExt_003 end");
}

/**
* @tc.name: MediaDataShare_ObserverExt_004
* @tc.desc: Test DataShare observer extension for DELETE operation notification with extended data
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Get the DataShare helper instance and verify it is not null
    2. Create a media library URI and a DataShareObserverTest instance
    3. Register the observer extension with the URI
    4. Create predicates to filter data by name "Datashare_Observer_Test003"
    5. Delete the filtered data and verify success
    6. Convert the deleted data info to extended format for change info
    7. Notify a DELETE change with the extended data
    8. Wait for observer notification and verify the change info matches
    9. Unregister the observer extension
* @tc.expect:
    1.Delete operation returns a positive value (success)
    2.Observer correctly receives and matches DELETE change info with extended data
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_ObserverExt_004, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_ObserverExt_004 start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    ASSERT_TRUE(helper != nullptr);
    Uri uri(MEDIALIBRARY_DATA_URI);
    std::shared_ptr<DataShareObserverTest> dataObserver = std::make_shared<DataShareObserverTest>();
    helper->RegisterObserverExt(uri, dataObserver, false);
    DataShare::DataShareValuesBucket valuesBucket;
    valuesBucket.Put("name", "Datashare_Observer_Test003");
    std::vector<DataShareValuesBucket> VBuckets = {valuesBucket};
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo("name", "Datashare_Observer_Test003");
    int retVal = helper->Delete(uri, predicates);
    EXPECT_EQ((retVal > 0), true);
    DataShareObserver::ChangeInfo::VBuckets extends;
    extends = ValueProxy::Convert(std::move(VBuckets));
    ChangeInfo uriChanges = { DataShareObserver::ChangeType::DELETE, { uri }, nullptr, 0, extends};
    helper->NotifyChangeExt(uriChanges);

    dataObserver->data.Wait();
    EXPECT_TRUE(ChangeInfoEqual(dataObserver->changeInfo_, uriChanges));
    dataObserver->Clear();

    helper->UnregisterObserverExt(uri, dataObserver);
    LOG_INFO("MediaDataShare_ObserverExt_004 end");
}

/**
* @tc.name: MediaDataShare_UnregisterObserverExt_002
* @tc.desc: Test that unregistered DataShare observer no longer receives notifications with extended data
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Get the DataShare helper instance and verify it is not null
    2. Create a media library URI and a DataShareObserverTest instance
    3. Register the observer extension with the URI
    4. Insert test data into the URI and verify success
    5. Convert the inserted data to extended format, notify an INSERT change, and verify observer receives it
    6. Unregister the observer extension
    7. Notify a DELETE change with the same extended data
    8. Wait for observer notification and verify the change info does not match
* @tc.expect:
    1.Data insertion returns a positive value (success)
    2.Registered observer correctly receives INSERT change info with extended data
    3.Unregistered observer does not receive DELETE change info (change info mismatch)
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_UnregisterObserverExt_002, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_UnregisterObserverExt_002 start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    ASSERT_TRUE(helper != nullptr);
    Uri uri(MEDIALIBRARY_DATA_URI);
    std::shared_ptr<DataShareObserverTest> dataObserver = std::make_shared<DataShareObserverTest>();
    helper->RegisterObserverExt(uri, dataObserver, true);

    DataShare::DataShareValuesBucket valuesBucket;
    valuesBucket.Put("name", "Datashare_Observer_Test003");
    int retVal = helper->Insert(uri, valuesBucket);
    EXPECT_EQ((retVal > 0), true);
    std::vector<DataShareValuesBucket> Buckets = {valuesBucket};
    DataShareObserver::ChangeInfo::VBuckets extends;
    extends = ValueProxy::Convert(std::move(Buckets));
    ChangeInfo uriChanges = { DataShareObserver::ChangeType::INSERT, { uri }, nullptr, 0,  extends};
    helper->NotifyChangeExt(uriChanges);

    dataObserver->data.Wait();
    EXPECT_TRUE(ChangeInfoEqual(dataObserver->changeInfo_, uriChanges));
    dataObserver->Clear();

    helper->UnregisterObserverExt(uri, dataObserver);
    helper->NotifyChangeExt({ DataShareObserver::ChangeType::DELETE,  { uri }, nullptr, 0,  extends });

    dataObserver->data.Wait();
    EXPECT_FALSE(ChangeInfoEqual(dataObserver->changeInfo_, uriChanges));
    dataObserver->Clear();
    LOG_INFO("MediaDataShare_UnregisterObserverExt_002 end");
}

/**
* @tc.name: MediaDataShare_BatchUpdate_Test_001
* @tc.desc: Test batch update functionality of DataShare helper with multiple operations and URIs
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Get the DataShare helper instance and verify it is not null
    2. Create a media library URI and insert test data (name="batchUpdateTest")
    3. Create batch update operations:
        - For "uri1": Update existing test data to "batchUpdateTested"; Update non-existent data to "undefined1"
        - For "uri2": Update non-existent data to "undefined1"
    4. Execute batch update and get results
    5. Delete the test data to clean up
* @tc.expect:
    1.Initial insertion returns a positive value (success)
    2.Batch update returns success, with results size 2
    3.Results for "uri1" contain [1, 0] (1 success, 1 no match)
    4.Results for "uri2" contain [0] (no match)
    5.Cleanup deletion returns a positive value (success)
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_BatchUpdate_Test_001, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_BatchUpdate_Test_001::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    ASSERT_TRUE(helper != nullptr);
    Uri uri(MEDIALIBRARY_DATA_URI);
    DataShare::DataShareValuesBucket valuesBucket;
    valuesBucket.Put("name", "batchUpdateTest");
    int ret = helper->Insert(uri, valuesBucket);
    EXPECT_GT(ret, 0);

    DataShare::UpdateOperations operations;
    std::vector<DataShare::UpdateOperation> updateOperations1;
    DataShare::UpdateOperation updateOperation1;
    updateOperation1.valuesBucket.Put("name", "batchUpdateTested");
    updateOperation1.predicates.EqualTo("name", "batchUpdateTest");
    updateOperations1.push_back(updateOperation1);

    std::vector<DataShare::UpdateOperation> updateOperations2;
    DataShare::UpdateOperation updateOperation2;
    updateOperation2.valuesBucket.Put("name", "undefined1");
    updateOperation2.predicates.EqualTo("name", "undefined");
    updateOperations1.push_back(updateOperation2);
    updateOperations2.push_back(updateOperation2);

    operations.emplace("uri1", updateOperations1);
    operations.emplace("uri2", updateOperations2);
    std::vector<BatchUpdateResult> results;
    ret = helper->BatchUpdate(operations, results);
    EXPECT_EQ(results.size(), 2);
    EXPECT_EQ(results[0].codes[0], 1);
    EXPECT_EQ(results[0].codes[1], 0);
    EXPECT_EQ(results[1].codes[0], 0);
    DataShare::DataSharePredicates predicates3;
    predicates3.EqualTo("name", "batchUpdateTested");
    ret = helper->Delete(uri, predicates3);
    EXPECT_GT(ret, 0);
    LOG_INFO("MediaDataShare_BatchUpdate_Test_001 End");
}

/**
* @tc.name: MediaDataShare_BatchUpdateThanLimit_Test_001
* @tc.desc: Test batch update functionality when operation count exceeds the limit
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Get the DataShare helper instance and verify it is not null
    2. Create a media library URI
    3. Create a batch update operation list with 4001 identical update operations (exceeding limit)
    4. Execute batch update and check the return value and results
* @tc.expect:
    1.Batch update returns -1 (failure due to exceeding limit)
    2.Results list is empty
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_BatchUpdateThanLimit_Test_001, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_BatchUpdateThanLimit_Test_001::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    ASSERT_TRUE(helper != nullptr);
    Uri uri(MEDIALIBRARY_DATA_URI);

    DataShare::UpdateOperations operations;
    std::vector<DataShare::UpdateOperation> updateOperations1;
    DataShare::UpdateOperation updateOperation1;
    updateOperation1.valuesBucket.Put("name", "batchUpdateTested");
    updateOperation1.predicates.EqualTo("name", "batchUpdateTest");
    for (int i = 0; i < 4001; i++) {
        updateOperations1.push_back(updateOperation1);
    }
    operations.emplace("uri1", updateOperations1);
    std::vector<BatchUpdateResult> results;
    int ret = helper->BatchUpdate(operations, results);
    EXPECT_EQ(ret, -1);
    EXPECT_EQ(results.size(), 0);
    LOG_INFO("MediaDataShare_BatchUpdateThanLimit_Test_001 End");
}

void OnChangeCallback(const RdbChangeNode &changeNode)
{
    // In test, put 2 uris into the data vec
    int vecLen = 2;
    EXPECT_EQ(changeNode.data_.size(), vecLen);
    for (int i = 0; i < vecLen; i++) {
        EXPECT_EQ(changeNode.data_[i], DATA_SHARE_URI);
    }
}

void PrepareNodeContent(RdbChangeNode &node)
{
    OHOS::sptr<Ashmem> memory = Ashmem::CreateAshmem("PrepareNodeContent", DATA_SIZE_ASHMEM_TRANSFER_LIMIT);
    EXPECT_NE(memory, nullptr);
    bool mapRet = memory->MapReadAndWriteAshmem();
    ASSERT_TRUE(mapRet);
    // write 2 uris
    int vecLen = 2;
    int intByteLen = 4;
    int offset = 0;
    bool writeRet = memory->WriteToAshmem((void*)&vecLen, intByteLen, offset);
    ASSERT_TRUE(writeRet);
    offset += intByteLen;
    int len = DATA_SHARE_URI.length();
    const char *str = DATA_SHARE_URI.c_str();
    for (int i = 0; i < vecLen; i++) {
        writeRet = memory->WriteToAshmem((void*)&len, intByteLen, offset);
        ASSERT_TRUE(writeRet);
        offset += intByteLen;
        writeRet = memory->WriteToAshmem((void*)str, len, offset);
        ASSERT_TRUE(writeRet);
        offset += len;
    }
    node.memory_ = memory;
    node.size_ = offset;
    node.isSharedMemory_ = true;
}

/**
* @tc.name: ReadAshmem
* @tc.desc: Test ReadAshmem function for normal read operations and error scenarios
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create an Ashmem instance and map it for read/write
    2. Write test data (string length and URI string) into Ashmem
    3. Use RdbObserverStub to read the length from Ashmem and verify it matches
    4. Read the URI string from Ashmem and verify it matches the original
    5. Test error path: attempt to read data exceeding Ashmem size
* @tc.expect:
    1.Normal read of length returns E_OK and matches written length
    2.Normal read of string returns E_OK and matches original URI
    3.Error read (exceeding size) returns E_ERROR
*/
HWTEST_F(MediaDataShareUnitTest, ReadAshmem, TestSize.Level1)
{
    LOG_INFO("ReadAshmem starts");
    RdbChangeNode node;

    OHOS::sptr<Ashmem> memory = Ashmem::CreateAshmem("ReadAshmem", DATA_SIZE_ASHMEM_TRANSFER_LIMIT);
    EXPECT_NE(memory, nullptr);
    bool mapRet = memory->MapReadAndWriteAshmem();
    ASSERT_TRUE(mapRet);
    int len = DATA_SHARE_URI.length();
    bool writeRet = memory->WriteToAshmem((void*)&len, 4, 0);
    ASSERT_TRUE(writeRet);
    const char *str = DATA_SHARE_URI.c_str();
    writeRet = memory->WriteToAshmem((void*)str, len, 4);
    ASSERT_TRUE(writeRet);
    node.memory_ = memory;

    RdbObserverStub stub(OnChangeCallback);
    // Read an int
    const int *lenRead;
    int offset = 0;
    int readRet = stub.ReadAshmem(node, (const void**)&lenRead, 4, offset);
    EXPECT_EQ(readRet, E_OK);
    EXPECT_EQ(offset, 4);
    int lenFromAshmem = *lenRead;
    EXPECT_EQ(lenFromAshmem, len);
    // Read a string
    readRet = stub.ReadAshmem(node, (const void**)&str, lenFromAshmem, offset);
    EXPECT_EQ(readRet, E_OK);
    EXPECT_EQ(offset, 4 + len);
    std::string strRead(str, lenFromAshmem);
    EXPECT_EQ(strRead, DATA_SHARE_URI);

    // Error path test
    readRet = stub.ReadAshmem(node, (const void**)&str, DATA_SIZE_ASHMEM_TRANSFER_LIMIT, offset);
    EXPECT_EQ(readRet, E_ERROR);
    LOG_INFO("ReadAshmem ends");
}

/**
* @tc.name: DeserializeDataFromAshmem001
* @tc.desc: Test DeserializeDataFromAshmem function for successful data deserialization from Ashmem
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Prepare an RdbChangeNode with Ashmem containing serialized data (2 URIs) using PrepareNodeContent
    2. Create an RdbObserverStub with OnChangeCallback
    3. Call DeserializeDataFromAshmem on the stub with the node
    4. Verify the deserialized data in the node
* @tc.expect:
    1.Deserialization returns E_OK
    2.Node contains 2 URIs, both matching DATA_SHARE_URI
*/
HWTEST_F(MediaDataShareUnitTest, DeserializeDataFromAshmem001, TestSize.Level1)
{
    LOG_INFO("DeserializeDataFromAshmem001::Start");
    RdbChangeNode node;
    PrepareNodeContent(node);

    RdbObserverStub stub(OnChangeCallback);
    int readRet = stub.DeserializeDataFromAshmem(node);
    EXPECT_EQ(readRet, E_OK);
    EXPECT_EQ(node.data_.size(), 2);
    for (int i = 0; i < 2; i++) {
        EXPECT_EQ(node.data_[i], DATA_SHARE_URI);
    }
    LOG_INFO("DeserializeDataFromAshmem001::End");
}

/**
* @tc.name: DeserializeDataFromAshmem002
* @tc.desc: Test DeserializeDataFromAshmem function for error scenarios (invalid Ashmem data)
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Test with null Ashmem: call deserialization on a node with no memory_
    2. Test with invalid string length: write Ashmem with excessive string length and call deserialization
    3. Test with insufficient size for vector length: create small Ashmem and call deserialization
    4. Test with insufficient size for string length: create Ashmem with partial data and call deserialization
* @tc.expect: All error scenarios return E_ERROR
*/
HWTEST_F(MediaDataShareUnitTest, DeserializeDataFromAshmem002, TestSize.Level1)
{
    LOG_INFO("DeserializeDataFromAshmem002::Start");
    RdbChangeNode node;
    RdbObserverStub stub(OnChangeCallback);
    // memory_ is null.
    int ret = stub.DeserializeDataFromAshmem(node);
    EXPECT_EQ(ret, E_ERROR);

    // Error in read from Ashmem with error string length.
    OHOS::sptr<Ashmem> memory = Ashmem::CreateAshmem("DeserializeDataFromAshmem002", DATA_SIZE_ASHMEM_TRANSFER_LIMIT);
    EXPECT_NE(memory, nullptr);
    bool mapRet = memory->MapReadAndWriteAshmem();
    ASSERT_TRUE(mapRet);
    int vecLen = 1;
    int offset = 0;
    bool writeRet = memory->WriteToAshmem((void*)&vecLen, 4, offset);
    ASSERT_TRUE(writeRet);
    offset += 4;
    int len = DATA_SHARE_URI.length();
    int errorLen = DATA_SIZE_ASHMEM_TRANSFER_LIMIT;
    const char *str = DATA_SHARE_URI.c_str();
    writeRet = memory->WriteToAshmem((void*)&errorLen, 4, offset);
    ASSERT_TRUE(writeRet);
    offset += 4;
    writeRet = memory->WriteToAshmem((void*)str, len, offset);
    ASSERT_TRUE(writeRet);
    node.memory_ = memory;

    ret = stub.DeserializeDataFromAshmem(node);
    EXPECT_EQ(ret, E_ERROR);

    // Error in read from Ashmem with vec size
    OHOS::sptr<Ashmem> memory2 = Ashmem::CreateAshmem("DeserializeDataFromAshmem002", 2);
    EXPECT_NE(memory2, nullptr);
    mapRet = memory2->MapReadAndWriteAshmem();
    ASSERT_TRUE(mapRet);
    node.memory_ = memory2;
    ret = stub.DeserializeDataFromAshmem(node);
    EXPECT_EQ(ret, E_ERROR);

    // Error in read from Ashmem with str size
    OHOS::sptr<Ashmem> memory3 = Ashmem::CreateAshmem("DeserializeDataFromAshmem002", 5);
    EXPECT_NE(memory3, nullptr);
    mapRet = memory3->MapReadAndWriteAshmem();
    ASSERT_TRUE(mapRet);
    writeRet = memory3->WriteToAshmem((void*)&vecLen, 4, 0);
    ASSERT_TRUE(writeRet);
    node.memory_ = memory3;
    ret = stub.DeserializeDataFromAshmem(node);
    EXPECT_EQ(ret, E_ERROR);
    LOG_INFO("DeserializeDataFromAshmem002::End");
}

/**
* @tc.name: RecoverRdbChangeNodeData001
* @tc.desc: Test RecoverRdbChangeNodeData function for normal recovery and non-recovery scenarios
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Prepare an RdbChangeNode with shared memory containing 2 URIs using PrepareNodeContent
    2. Create an RdbObserverStub with OnChangeCallback
    3. Call RecoverRdbChangeNodeData on the stub with the node and verify return value
    4. Check that node data is correctly recovered, memory is nullified, and size is 0
    5. Prepare a second RdbChangeNode with shared memory but set isSharedMemory_ to false
    6. Call RecoverRdbChangeNodeData on the second node and verify return value
    7. Check that node data remains empty, memory is retained, and size is unchanged
* @tc.expect:
    1.First recovery returns E_OK, node contains 2 URIs, memory is null, size is 0, and isSharedMemory_ is false
    2.non-recovery returns E_OK, node data is empty, memory is not null, size is 82, and isSharedMemory_ is false
*/
HWTEST_F(MediaDataShareUnitTest, RecoverRdbChangeNodeData001, TestSize.Level0)
{
    LOG_INFO("RecoverRdbChangeNodeData::Start");
    
    // Recover
    RdbChangeNode node;
    PrepareNodeContent(node);
    RdbObserverStub stub(OnChangeCallback);
    int ret = stub.RecoverRdbChangeNodeData(node);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(node.data_.size(), 2);
    for (int i = 0; i < 2; i++) {
        EXPECT_EQ(node.data_[i], DATA_SHARE_URI);
    }
    EXPECT_EQ(node.memory_, nullptr);
    EXPECT_EQ(node.size_, 0);
    ASSERT_FALSE(node.isSharedMemory_);

    // Not recover
    RdbChangeNode node2;
    PrepareNodeContent(node2);
    node2.isSharedMemory_ = false;
    ret = stub.RecoverRdbChangeNodeData(node2);
    EXPECT_EQ(ret, E_OK);
    EXPECT_EQ(node2.data_.size(), 0);
    EXPECT_NE(node2.memory_, nullptr);
    EXPECT_EQ(node2.size_, 82);
    ASSERT_FALSE(node2.isSharedMemory_);

    LOG_INFO("RecoverRdbChangeNodeData End");
}

/**
* @tc.name: RecoverRdbChangeNodeData002
* @tc.desc: Test RecoverRdbChangeNodeData function with error scenario (null shared memory)
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create an RdbChangeNode with isSharedMemory_ set to true but no memory allocated
    2. Create an RdbObserverStub with OnChangeCallback
    3. Call RecoverRdbChangeNodeData on the stub with the node
    4. Verify the return value and node state after recovery
* @tc.expect:
    1.Recovery returns E_ERROR
    2.Node data is empty, memory is null, size is 0, and isSharedMemory_ is false
*/
HWTEST_F(MediaDataShareUnitTest, RecoverRdbChangeNodeData002, TestSize.Level0)
{
    LOG_INFO("RecoverRdbChangeNodeData002::Start");
    
    RdbChangeNode node;
    node.isSharedMemory_ = true;
    RdbObserverStub stub(OnChangeCallback);
    int ret = stub.RecoverRdbChangeNodeData(node);
    EXPECT_EQ(ret, E_ERROR);
    EXPECT_EQ(node.data_.size(), 0);
    EXPECT_EQ(node.memory_, nullptr);
    EXPECT_EQ(node.size_, 0);
    ASSERT_FALSE(node.isSharedMemory_);
    LOG_INFO("RecoverRdbChangeNodeData002::End");
}

/**
* @tc.name: OnChangeFromRdb001
* @tc.desc: Test OnChangeFromRdb function for normal data processing
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Prepare an RdbChangeNode with shared memory containing 2 URIs using PrepareNodeContent
    2. Create an RdbObserverStub with OnChangeCallback
    3. Call OnChangeFromRdb on the stub with the node
    4. Verify the node's data, memory, size, and isSharedMemory_ state
* @tc.expect:
    1.Node contains 2 URIs matching DATA_SHARE_URI
    2.Node memory is null, size is 0, and isSharedMemory_ is false
*/
HWTEST_F(MediaDataShareUnitTest, OnChangeFromRdb001, TestSize.Level0)
{
    LOG_INFO("OnChangeFromRdb001::Start");
    
    RdbChangeNode node;
    PrepareNodeContent(node);
    RdbObserverStub stub(OnChangeCallback);
    stub.OnChangeFromRdb(node);
    EXPECT_EQ(node.data_.size(), 2);
    for (int i = 0; i < 2; i++) {
        EXPECT_EQ(node.data_[i], DATA_SHARE_URI);
    }
    EXPECT_EQ(node.memory_, nullptr);
    EXPECT_EQ(node.size_, 0);
    ASSERT_FALSE(node.isSharedMemory_);
    LOG_INFO("OnChangeFromRdb001::End");
}

/**
* @tc.name: OnChangeFromRdb002
* @tc.desc: Test OnChangeFromRdb function with error scenario (null shared memory)
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create an RdbChangeNode with isSharedMemory_ set to true but no memory allocated
    2. Create an RdbObserverStub with OnChangeCallback
    3. Call OnChangeFromRdb on the stub with the node
    4. Verify the node's data, memory, size, and isSharedMemory_ state
* @tc.expect:
    1.Node data is empty
    2.Node memory is null, size is 0, and isSharedMemory_ is false
*/
HWTEST_F(MediaDataShareUnitTest, OnChangeFromRdb002, TestSize.Level0)
{
    LOG_INFO("OnChangeFromRdb002::Start");
    RdbChangeNode node;
    node.isSharedMemory_ = true;
    RdbObserverStub stub(OnChangeCallback);
    stub.OnChangeFromRdb(node);
    EXPECT_EQ(node.data_.size(), 0);
    EXPECT_EQ(node.memory_, nullptr);
    EXPECT_EQ(node.size_, 0);
    ASSERT_FALSE(node.isSharedMemory_);
    LOG_INFO("OnChangeFromRdb002::End");
}

/**
* @tc.name: OnremoteRequestTest001
* @tc.desc: Test OnRemoteRequest function with invalid and valid interface tokens and request codes
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create an RdbObserverStub with OnChangeCallback
    2. Prepare MessageParcel, reply, and option objects
    3. Write an invalid interface token to the parcel and call OnRemoteRequest with code 0
    4. Write the correct interface token and call OnRemoteRequest with code 1
    5. Call OnRemoteRequest with the correct token and code 0
    6. Verify the return values for each scenario
* @tc.expect:
    1.Request with invalid token returns ERR_INVALID_STATE
    2.Request with valid token and invalid code (1) returns ERR_INVALID_STATE
    3.Request with valid token and code 0 returns ERR_INVALID_VALUE
*/
HWTEST_F(MediaDataShareUnitTest, OnremoteRequestTest001, TestSize.Level0)
{
    LOG_INFO("OnremoteRequestTest001::Start");
    RdbObserverStub stub(OnChangeCallback);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    std::u16string descriptorError = u"ERROR";
    std::u16string descriptorCorrect = RdbObserverStub::GetDescriptor();
    data.WriteInterfaceToken(descriptorError);
    std::u16string descriptor;
    int ret = stub.OnRemoteRequest(0, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_STATE);
    data.WriteInterfaceToken(descriptorCorrect);
    ret = stub.OnRemoteRequest(1, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_STATE);
    data.WriteInterfaceToken(descriptorCorrect);
    ret = stub.OnRemoteRequest(0, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
    LOG_INFO("OnremoteRequestTest001::End");
}

/**
* @tc.name: ReadAshmemTest001
* @tc.desc: Test ReadAshmem function with null shared memory
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create an RdbObserverStub with OnChangeCallback
    2. Create an RdbChangeNode with a null Ashmem pointer
    3. Call ReadAshmem on the stub with the node, null data pointer, size 0, and undefined offset
    4. Verify the return value
* @tc.expect: ReadAshmem returns E_ERROR
*/
HWTEST_F(MediaDataShareUnitTest, ReadAshmemTest001, TestSize.Level0)
{
    LOG_INFO("ReadAshmemTest001::Start");
    RdbObserverStub stub(OnChangeCallback);
    RdbChangeNode changeNode;
    changeNode.memory_ = OHOS::sptr<Ashmem>(nullptr);
    const void *data = nullptr;
    int size = 0;
    int offset;
    int ret = stub.ReadAshmem(changeNode, &data, size, offset);
    EXPECT_EQ(ret, E_ERROR);
    LOG_INFO("ReadAshmemTest001::End");
}

/**
* @tc.name: MediaDataShare_User_Define_Func_Test_001
* @tc.desc: Test UserDefineFunc with no interface descriptor
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Get the DataShare helper instance
    2. Prepare empty MessageParcel, reply, and option objects (no descriptor written)
    3. Call UserDefineFunc on the helper
    4. Verify the error code
* @tc.expect: UserDefineFunc returns -1 (failure due to missing descriptor)
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_User_Define_Func_Test_001, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_User_Define_Func_Test_001::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto errCode = helper->UserDefineFunc(data, reply, option);
    // 10 is IPC error ERR_INVALID_STATE
    EXPECT_EQ(errCode, 10);
    LOG_INFO("MediaDataShare_User_Define_Func_Test_001 End");
}

/**
* @tc.name: MediaDataShare_User_Define_Func_Test_002
* @tc.desc: Test UserDefineFunc with a valid interface descriptor
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Get the DataShare helper instance
    2. Prepare MessageParcel, reply, and option objects
    3. Write the valid interface descriptor ("OHOS.DataShare.IDataShare") to the parcel
    4. Call UserDefineFunc on the helper
    5. Verify the error code
* @tc.expect: UserDefineFunc returns 0 (success with valid descriptor)
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_User_Define_Func_Test_002, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_User_Define_Func_Test_002::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    MessageParcel data;
    std::u16string descriptor = u"OHOS.DataShare.IDataShare";
    MessageParcel reply;
    MessageOption option;
    if (data.WriteInterfaceToken(descriptor)) {
        auto errCode = helper->UserDefineFunc(data, reply, option);
        EXPECT_EQ(errCode, 0);
    }
    LOG_INFO("MediaDataShare_User_Define_Func_Test_002 End");
}

/**
* @tc.name: MediaDataShare_RegisterObserverExtProvider_Test_001
* @tc.desc: Test RegisterObserverExtProvider with null observer and null controller scenarios
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create a DataShare helper instance and verify it is not null
    2. Attempt to register a null observer with an empty URI, check error code
    3. Create a valid observer and use a media library URI
    4. Release the helper's controller and attempt to register the observer, check error code
* @tc.expect:
    1.Registering null observer returns E_NULL_OBSERVER
    2.Registering with released (null) controller returns E_HELPER_DIED
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_RegisterObserverExtProvider_Test_001, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_RegisterObserverExtProvider_Test_001::Start");

    Uri uri("");
    // GetObserver return not nullptr
    std::shared_ptr<DataShareObserver> dataObserver = nullptr;
    std::shared_ptr<DataShare::DataShareHelper> helper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID);
    ASSERT_NE(helper, nullptr);
    int errCode = helper->RegisterObserverExtProvider(uri, dataObserver, false);
    EXPECT_EQ(errCode, E_NULL_OBSERVER);

    // GetObserver return is not nullptr but controller is nullptr
    uri = Uri(MEDIALIBRARY_DATA_URI);
    dataObserver = std::make_shared<DataShareObserverTest>();
    ASSERT_NE(dataObserver, nullptr);
    bool ret = helper->Release();
    EXPECT_TRUE(ret);
    errCode = helper->RegisterObserverExtProvider(uri, dataObserver, false);
    EXPECT_EQ(errCode, E_HELPER_DIED);

    LOG_INFO("MediaDataShare_RegisterObserverExtProvider_Test_001::End");
}

/**
* @tc.name: MediaDataShare_RegisterObserverExtProvider_Test_002
* @tc.desc: Test normal functionality of RegisterObserverExtProvider
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create a media library URI and a valid DataShareObserverTest instance
    2. Create a DataShare helper instance and verify it is not null
    3. Call RegisterObserverExtProvider with the URI, observer, and false flag
    4. Verify the error code
* @tc.expect: RegisterObserverExtProvider returns E_OK (success)
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_RegisterObserverExtProvider_Test_002, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_RegisterObserverExtProvider_Test_002::Start");

    Uri uri(MEDIALIBRARY_DATA_URI);
    std::shared_ptr<DataShareObserver> dataObserver = std::make_shared<DataShareObserverTest>();
    std::shared_ptr<DataShare::DataShareHelper> helper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID);
    int errCode = helper->RegisterObserverExtProvider(uri, dataObserver, false);
    ASSERT_NE(helper, nullptr);
    EXPECT_EQ(errCode, E_OK);

    LOG_INFO("MediaDataShare_RegisterObserverExtProvider_Test_002::End");
}

/**
* @tc.name: MediaDataShare_UnregisterObserverExtProvider_Test_001
* @tc.desc: Test UnregisterObserverExtProvider with null observer, unregistered observer, and null controller
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create a DataShare helper instance and verify it is not null
    2. Attempt to unregister a null observer, check error code
    3. Create a valid observer and attempt to unregister it (not previously registered), check error code
    4. Register the observer, release the helper's controller, then attempt to unregister, check error code
* @tc.expect:
    - Unregistering null observer returns E_NULL_OBSERVER
    - Unregistering unregistered observer returns E_NULL_OBSERVER
    - Unregistering with released (null) controller returns E_HELPER_DIED
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_UnregisterObserverExtProvider_Test_001, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_UnregisterObserverExtProvider_Test_001::Start");

    Uri uri(MEDIALIBRARY_DATA_URI);
    // dataObserver is nullptr
    std::shared_ptr<DataShareObserver> dataObserver = nullptr;
    std::shared_ptr<DataShare::DataShareHelper> helper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID);
    ASSERT_NE(helper, nullptr);
    int errCode = helper->UnregisterObserverExtProvider(uri, dataObserver);
    EXPECT_EQ(errCode, E_NULL_OBSERVER);

    // FindObserver return false
    dataObserver = std::make_shared<DataShareObserverTest>();
    ASSERT_NE(dataObserver, nullptr);
    errCode = helper->UnregisterObserverExtProvider(uri, dataObserver);
    EXPECT_EQ(errCode, E_NULL_OBSERVER);

    // FindObserver return true and general controller is nullptr
    errCode = helper->RegisterObserverExtProvider(uri, dataObserver, false);
    EXPECT_EQ(errCode, E_OK);
    bool ret = helper->Release();
    EXPECT_TRUE(ret);
    errCode = helper->UnregisterObserverExtProvider(uri, dataObserver);
    EXPECT_EQ(errCode, E_HELPER_DIED);

    LOG_INFO("MediaDataShare_UnregisterObserverExtProvider_Test_001::End");
}

/**
* @tc.name: MediaDataShare_UnregisterObserverExtProvider_Test_002
* @tc.desc: Test normal functionality of UnregisterObserverExtProvider
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create a media library URI and a valid DataShareObserverTest instance
    2. Create a DataShare helper instance and verify it is not null
    3. Register the observer using RegisterObserverExtProvider and verify success
    4. Unregister the observer using UnregisterObserverExtProvider
    5. Verify the error code of unregistration
* @tc.expect:
    1.Registration returns E_OK
    2.Unregistration returns E_OK (success)
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_UnregisterObserverExtProvider_Test_002, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_UnregisterObserverExtProvider_Test_002::Start");

    Uri uri(MEDIALIBRARY_DATA_URI);
    // dataObserver is not nullptr
    std::shared_ptr<DataShareObserver> dataObserver = std::make_shared<DataShareObserverTest>();
    ASSERT_NE(dataObserver, nullptr);
    std::shared_ptr<DataShare::DataShareHelper> helper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID);
    ASSERT_NE(helper, nullptr);

    // FindObserver return true and general controller is not nullptr
    int errCode = helper->RegisterObserverExtProvider(uri, dataObserver, true);
    EXPECT_EQ(errCode, E_OK);

    errCode = helper->UnregisterObserverExtProvider(uri, dataObserver);
    EXPECT_EQ(errCode, E_OK);

    LOG_INFO("MediaDataShare_UnregisterObserverExtProvider_Test_002::End");
}

/**
* @tc.name: MediaDataShare_NotifyChangeExtProvider_Test_001
* @tc.desc: Test NotifyChangeExtProvider with null controller (released helper)
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create a media library URI and a valid DataShareObserverTest instance
    2. Create a DataShare helper instance, register the observer, and verify registration success
    3. Release the helper's controller to make it null
    4. Create an INSERT-type ChangeInfo and call NotifyChangeExtProvider with it
* @tc.expect: NotifyChangeExtProvider executes without crashing (no explicit error code check in test)
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_NotifyChangeExtProvider_Test_001, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_NotifyChangeExtProvider_Test_001::Start");

    Uri uri(MEDIALIBRARY_DATA_URI);
    std::shared_ptr<DataShareObserver> dataObserver = std::make_shared<DataShareObserverTest>();
    std::shared_ptr<DataShare::DataShareHelper> helper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID);
    ASSERT_NE(helper, nullptr);
    ASSERT_NE(dataObserver, nullptr);

    int errCode = helper->RegisterObserverExtProvider(uri, dataObserver, false);
    EXPECT_EQ(errCode, E_OK);
    // generalCtl is nullptr
    bool ret = helper->Release();
    EXPECT_TRUE(ret);

    ChangeInfo changeInfo = { DataShareObserver::ChangeType::INSERT, { uri } };
    helper->NotifyChangeExtProvider(changeInfo);
    LOG_INFO("MediaDataShare_NotifyChangeExtProvider_Test_001::End");
}

/**
* @tc.name: MediaDataShare_NotifyChangeExtProvider_Test_002
* @tc.desc: Test normal functionality of NotifyChangeExtProvider with valid controller
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create a media library URI and a valid DataShareObserverTest instance
    2. Create a DataShare helper instance, register the observer, and verify registration success
    3. Create an INSERT-type ChangeInfo and call NotifyChangeExtProvider with it
* @tc.expect: NotifyChangeExtProvider executes successfully (no errors)
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_NotifyChangeExtProvider_Test_002, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_NotifyChangeExtProvider_Test_002::Start");

    Uri uri(MEDIALIBRARY_DATA_URI);
    // generalCtl is not nullptr
    std::shared_ptr<DataShareObserver> dataObserver = std::make_shared<DataShareObserverTest>();
    std::shared_ptr<DataShare::DataShareHelper> helper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID);
    ASSERT_NE(helper, nullptr);
    ASSERT_NE(dataObserver, nullptr);

    int errCode = helper->RegisterObserverExtProvider(uri, dataObserver, false);
    EXPECT_EQ(errCode, E_OK);

    ChangeInfo changeInfo = { DataShareObserver::ChangeType::INSERT, { uri } };
    helper->NotifyChangeExtProvider(changeInfo);
    LOG_INFO("MediaDataShare_NotifyChangeExtProvider_Test_002::End");
}

/**
* @tc.name: MediaDataShare_OpenFileWithErrCode_Test_001
* @tc.desc: Test OpenFileWithErrCode function for file opening scenario
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create a DataShare helper instance and verify it is not null
    2. Create a media library URI and specify "rw" mode
    3. Call OpenFileWithErrCode with the URI, mode, and error code pointer
    4. Verify the returned file descriptor (fd) and error code
* @tc.expect:
    1.File descriptor (fd) is negative (failure to open)
    2.Error code is -1
*/
HWTEST_F(MediaDataShareUnitTest, MediaDataShare_OpenFileWithErrCode_Test_001, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_OpenFileWithErrCode_Test_001::Start");

    std::shared_ptr<DataShare::DataShareHelper> helper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID);
    ASSERT_NE(helper, nullptr);

    Uri uri(MEDIALIBRARY_DATA_URI);
    std::string mode = "rw";

    int32_t errCode = 0;
    int fd = helper->OpenFileWithErrCode(uri, mode, errCode);
    EXPECT_LT(fd, 0);
    EXPECT_EQ(errCode, -1);
    LOG_INFO("MediaDataShare_OpenFileWithErrCode_Test_001::End");
}
} // namespace DataShare
} // namespace OHOS