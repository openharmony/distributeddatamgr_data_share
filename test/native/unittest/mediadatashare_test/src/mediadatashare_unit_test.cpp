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

#include "mediadatashare_unit_test.h"

#include <gtest/gtest.h>
#include <unistd.h>

#include "abs_shared_result_set.h"
#include "accesstoken_kit.h"
#include "dataobs_mgr_changeinfo.h"
#include "datashare_log.h"
#include "datashare_valuebucket_convert.h"
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

HWTEST_F(MediaDataShareUnitTest, MediaDataShare_Predicates_Test_020, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_Predicates_Test_020::Start");
    DataShare::DataSharePredicates predicates;
    predicates.SetSettingMode(DataShare::SettingMode::PREDICATES_METHOD);
    int16_t setting = predicates.GetSettingMode();
    EXPECT_EQ(setting, DataShare::SettingMode::PREDICATES_METHOD);
    LOG_INFO("MediaDataShare_Predicates_Test_020, End");
}

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
    block = resultSet->GetBlock();
    EXPECT_NE(block, nullptr);

    resultSet->SetBlock(block);
    EXPECT_EQ(block, resultSet->GetBlock());
    resultSet->FillBlock(0, block);
    EXPECT_EQ(block, resultSet->GetBlock());
    LOG_INFO("MediaDataShare_ResultSet_Test_003, End");
}

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

HWTEST_F(MediaDataShareUnitTest, Creator_IRemoteObjectNull_Test_001, TestSize.Level0)
{
    LOG_INFO("Creator_IRemoteObjectNull_Test_001::Start");
    sptr<IRemoteObject> remoteObjNull = nullptr;
    auto remoteNull = DataShare::DataShareHelper::Creator(remoteObjNull, MEDIALIBRARY_DATA_URI);
    EXPECT_EQ(remoteNull, nullptr);
    LOG_INFO("Creator_IRemoteObjectNull_Test_001 End");
}

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

HWTEST_F(MediaDataShareUnitTest, MediaDataShare_NotImplPredicates_Test_zyp, TestSize.Level0)
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
    EXPECT_EQ(operationItems[0].singleParams[0].index(), 4);
    EXPECT_EQ(str, "name");
    EXPECT_EQ(operationItems[0].multiParams[0].index(), 1);
    for (int i = 0; i < ret.size(); i++) {
        EXPECT_EQ(ret[i], i + 1);
    }
    operationItems.clear();
    LOG_INFO("MediaDataShare_ImplPredicates_Test_001, End");
}

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

HWTEST_F(MediaDataShareUnitTest, MediaDataShare_Observer_001, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_Observer_001 start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_dataShareHelper;
    ASSERT_TRUE(helper != nullptr);
    Uri uri(MEDIALIBRARY_DATA_URI);
    sptr<IDataAbilityObserverTest> dataObserver;
    helper->RegisterObserver(uri, dataObserver);

    DataShare::DataShareValuesBucket valuesBucket;
    valuesBucket.Put("name", "Datashare_Observer_Test001");
    int retVal = helper->Insert(uri, valuesBucket);
    EXPECT_EQ((retVal > 0), true);
    helper->NotifyChange(uri);

    DataShare::DataSharePredicates deletePredicates;
    deletePredicates.EqualTo("name", "Datashare_Observer_Test001");
    retVal = helper->Delete(uri, deletePredicates);
    EXPECT_EQ((retVal >= 0), true);
    helper->NotifyChange(uri);
    helper->UnregisterObserver(uri, dataObserver);
    LOG_INFO("MediaDataShare_Observer_001 end");
}

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

HWTEST_F(MediaDataShareUnitTest, ControllerTest_HelperRegisterObserverControllerNullTest_001, TestSize.Level0)
{
    LOG_INFO("ControllerTest_HelperRegisterObserverControllerNullTest_001 start");
    auto helper= CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID);
    ASSERT_TRUE(helper != nullptr);
    Uri uri(MEDIALIBRARY_DATA_URI);
    helper->Release();
    sptr<IDataAbilityObserverTest> dataObserver;
    helper->RegisterObserver(uri, dataObserver);
    
    DataShare::DataShareValuesBucket valuesBucket;
    valuesBucket.Put("name", "Datashare_Observer_Test001");
    int retVal = helper->Insert(uri, valuesBucket);
    EXPECT_EQ((retVal < 0), true);
    helper->NotifyChange(uri);
    
    DataShare::DataSharePredicates deletePredicates;
    deletePredicates.EqualTo("name", "Datashare_Observer_Test001");
    retVal = helper->Delete(uri, deletePredicates);
    EXPECT_EQ((retVal < 0), true);
    helper->NotifyChange(uri);
    helper->UnregisterObserver(uri, dataObserver);
    LOG_INFO("ControllerTest_HelperRegisterObserverControllerNullTest_001 end");
}

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
} // namespace DataShare
} // namespace OHOS