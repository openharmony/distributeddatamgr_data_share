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
* @tc.desc: test TryRegisterObserverExt normal func, Insert.
* @tc.type: FUNC
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
* @tc.desc: test TryRegisterObserverExt normal func, BatchInsert.
* @tc.type: FUNC
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
* @tc.desc: test TryRegisterObserverExt normal func, Update.
* @tc.type: FUNC
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
* @tc.desc: test TryRegisterObserverExt normal func, Delete.
* @tc.type: FUNC
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
* @tc.desc: test TryRegisterObserverExt normal func, uri is "".
* @tc.type: FUNC
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
* @tc.desc: test TryRegisterObserverExt normal func, dataObserver is nullptr.
* @tc.type: FUNC
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
* @tc.desc: test TryRegisterObserverExt normal func, isSystem is true.
* @tc.type: FUNC
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
* @tc.desc: test TryRegisterObserverExt normal func, test parent interface.
* @tc.type: FUNC
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
 * @tc.desc: test TryRegisterObserverExt_009 normal func, use invalid uri.
 * @tc.type: FUNC
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
 * @tc.desc: test TryUnregisterObserverExt normal func, Insert.
 * @tc.type: FUNC
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
 * @tc.desc: test TryUnregisterObserverExt normal func, dataObserver is nullptr.
 * @tc.type: FUNC
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
 * @tc.desc: test TryUnregisterObserverExt normal func, observer not exit.
 * @tc.type: FUNC
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
 * @tc.desc: test TryUnregisterObserverExt normal func, test parent interface.
 * @tc.type: FUNC
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
 * @tc.desc: test RegisterObserver_Register_Test_001 normal func, success.
 * @tc.type: FUNC
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
 * @tc.desc: test RegisterObserver_Register_Test_002 normal func, RegisterObserver without read permission.
 * @tc.type: FUNC
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
 * @tc.desc: test RegisterObserver_Register_Test_003 normal func, NotifyChange without write permission.
 * @tc.type: FUNC
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
 * @tc.desc: test RegisterObserver_Register_Test_004 normal func, RegisterObserver without read permission,
 *           NotifyChange without write permission.
 * @tc.type: FUNC
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
 * @tc.desc: test RegisterObserver_Register_Test_005 normal func, RegisterObserver with invalid uri.
 * @tc.type: FUNC
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
 * @tc.desc: test RegisterObserver_Register_Test_006 normal func, RegisterObserver and insert silent success.
 * @tc.type: FUNC
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
 * @tc.desc: test RegisterObserver_Register_Test_007 normal func, insert silent success.
 *           But RegisterObserver without read permission.
 * @tc.type: FUNC
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
 * @tc.desc: test RegisterObserver_Register_Test_008 normal func, invalid uri.
 * @tc.type: FUNC
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
 * @tc.desc: test RegisterObserver_RegisterExt_Test_001 normal func, RegisterObserverExt success.
 * @tc.type: FUNC
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
 * @tc.desc: test RegisterObserver_RegisterExt_Test_002 normal func, RegisterObserverExt without read permission.
 * @tc.type: FUNC
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
 * @tc.desc: test RegisterObserver_RegisterExt_Test_003 normal func, NotifyChangeExt without write permission.
 * @tc.type: FUNC
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
 * @tc.desc: test RegisterObserver_RegisterExt_Test_004 normal func, without read and write permission.
 * @tc.type: FUNC
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
 * @tc.desc: test RegisterObserver_RegisterExt_Test_005 normal func, with invalid uri.
 * @tc.type: FUNC
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
 * @tc.desc: test RegisterObserver_RegisterExt_Test_006 normal func, RegisterObserverExt with valid and invalid uri.
 * @tc.type: FUNC
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
 * @tc.name: RegisterObserver_RegisterExt_Test_006
 * @tc.desc: test RegisterObserver_RegisterExt_Test_006 normal func, NotifyChangeExt with valid and invalid uri.
 * @tc.type: FUNC
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