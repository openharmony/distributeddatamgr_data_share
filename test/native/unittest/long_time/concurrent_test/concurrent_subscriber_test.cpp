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

#define LOG_TAG "concurrent_subscriber_test"

#include <gtest/gtest.h>
#include <unistd.h>

#include "accesstoken_kit.h"
#include "callbacks_manager.h"
#include "data_ability_observer_stub.h"
#include "datashare_errno.h"
#include "datashare_helper.h"
#include "datashare_log.h"
#include "datashare_template.h"
#include "datashare_string_utils.h"
#include "hap_token_info.h"
#include "iservice_registry.h"
#include "published_data_subscriber_manager.h"
#include "rdb_subscriber_manager.h"
#include "token_setproc.h"

namespace OHOS {
namespace DataShare {
using namespace testing::ext;
using namespace OHOS::Security::AccessToken;
using RdbBaseCallbacks = CallbacksManager<RdbObserverMapKey, RdbObserver>;
using RdbCallback = std::function<void(const RdbChangeNode &changeNode)>;
RdbCallback g_rbdCallback = [](const RdbChangeNode &changeNode) {};
RdbCallback g_timeConsumingRbdCallback = [](const RdbChangeNode &changeNode) {
    LOG_INFO("rdb change sleep begin");
    sleep(2);
    LOG_INFO("rdb change sleep end");
};
RdbChangeNode g_rdbChangeNode;
using PublishedBaseCallbacks = CallbacksManager<PublishedObserverMapKey, PublishedDataObserver>;
using PublishedDataCallback = std::function<void(PublishedDataChangeNode &changeNode)>;
PublishedDataCallback g_publishedCallback = [](const PublishedDataChangeNode &changeNode) {};
PublishedDataChangeNode g_publishedChangeNode;
constexpr int TEST_TIME = 20;
void *g_subscriber;
std::string DATA_SHARE_PROXY_URI = "datashareproxy://com.acts.ohos.data.datasharetest/test";

/**
 * @Usage: add long_time/concurrent_test/ConcurrentTest to unittest.deps of file
 * foundation/distributeddatamgr/data_share/test/native/BUILD.gn
 */
class ConcurrentSubscriberTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void ConcurrentSubscriberTest::SetUpTestCase(void)
{
    LOG_INFO("SetUpTestCase invoked");
    int sleepTime = 1;
    sleep(sleepTime);

    HapInfoParams info = { .userID = 100,
        .bundleName = "ohos.datashareproxyclienttest.demo",
        .instIndex = 0,
        .appIDDesc = "ohos.datashareproxyclienttest.demo",
        .isSystemApp = true };
    HapPolicyParams policy = { .apl = APL_SYSTEM_BASIC,
        .domain = "test.domain",
        .permList = { { .permissionName = "ohos.permission.GET_BUNDLE_INFO",
            .bundleName = "ohos.datashareproxyclienttest.demo",
            .grantMode = 1,
            .availableLevel = APL_SYSTEM_BASIC,
            .label = "label",
            .labelId = 1,
            .description = "ohos.datashareproxyclienttest.demo",
            .descriptionId = 1 } },
        .permStateList = { { .permissionName = "ohos.permission.GET_BUNDLE_INFO",
            .isGeneral = true,
            .resDeviceID = { "local" },
            .grantStatus = { PermissionState::PERMISSION_GRANTED },
            .grantFlags = { 1 } } } };
    AccessTokenKit::AllocHapToken(info, policy);
    auto testTokenId = Security::AccessToken::AccessTokenKit::GetHapTokenIDEx(
        info.userID, info.bundleName, info.instIndex);
    SetSelfTokenID(testTokenId.tokenIDEx);
    LOG_INFO("SetUpTestCase end");
}

void ConcurrentSubscriberTest::TearDownTestCase(void)
{
}

void ConcurrentSubscriberTest::SetUp(void)
{
    // input testCase setup step，setup invoked before each testCase
    testing::UnitTest *test = testing::UnitTest::GetInstance();
    ASSERT_NE(test, nullptr);
    const testing::TestInfo *testInfo = test->current_test_info();
    ASSERT_NE(testInfo, nullptr);
    string testCaseName = string(testInfo->name());
    LOG_INFO("[SetUp] %{public}s start", testCaseName.c_str());
    GTEST_LOG_(INFO) << testCaseName.append(" start");
}
void ConcurrentSubscriberTest::TearDown(void)
{
    // input testCase teardown step，teardown invoked after each testCase
    testing::UnitTest *test = testing::UnitTest::GetInstance();
    ASSERT_NE(test, nullptr);
    const testing::TestInfo *testInfo = test->current_test_info();
    ASSERT_NE(testInfo, nullptr);
    string testCaseName = string(testInfo->name());
    LOG_INFO("[SetUp] %{public}s end", testCaseName.c_str());
    GTEST_LOG_(INFO) << testCaseName.append(" end");
}

class RdbSubscriberManagerTest : public CallbacksManager<RdbObserverMapKey, RdbObserver> {
public:
    using Key = RdbObserverMapKey;
    using Observer = RdbObserver;
    void AddObservers(int64_t subscriberId, std::string &bundleName, std::string &uri, std::atomic<bool> &stop);
    void AddTimeConsumingObservers(std::shared_ptr<DataShare::DataShareHelper> helper, int64_t subscriberId,
        std::string &bundleName, std::string &uri, std::atomic<bool> &stop);
    void DelObservers(int64_t subscriberId, std::string &bundleName, std::string &uri, std::atomic<bool> &stop);
    void DelTimeConsumingObservers(std::shared_ptr<DataShare::DataShareHelper> helper, int64_t subscriberId,
        std::string &bundleName, std::string &uri, std::atomic<bool> &stop);
};

void RdbSubscriberManagerTest::AddObservers(
    int64_t subscriberId, std::string &bundleName, std::string &uri, std::atomic<bool> &stop)
{
    void *subscriber = g_subscriber;
    RdbChangeNode *changeNode = &g_rdbChangeNode;
    DataShare::TemplateId templateId;
    templateId.subscriberId_ = subscriberId, templateId.bundleName_ = bundleName;
    std::vector<std::string> uris;
    uris.emplace_back(uri);
    Key rdbKey(uri, templateId);
    std::vector<Key> keys;
    std::for_each(uris.begin(), uris.end(), [&keys, &templateId](auto &uri) { keys.emplace_back(uri, templateId); });
    while (!stop.load()) {
        LOG_INFO("Rdb AddObservers start, subscriberId: %{public}d", static_cast<int>(subscriberId));
        RdbBaseCallbacks::AddObservers(
            keys, subscriber, std::make_shared<Observer>(g_rbdCallback),
            [](const std::vector<Key> &localRegisterKeys, const std::shared_ptr<Observer> observer) {},
            [&subscriber, &templateId, &rdbKey, &changeNode](const std::vector<Key> &firstAddKeys,
                const std::shared_ptr<Observer> observer, std::vector<OperationResult> &opResult) {
                std::vector<std::string> firstAddUris;
                std::for_each(firstAddKeys.begin(), firstAddKeys.end(),
                    [&firstAddUris](auto &result) { firstAddUris.emplace_back(result); });
                if (firstAddUris.empty()) {
                    return;
                }
                RdbSubscriberManager::GetInstance().lastChangeNodeMap_.InsertOrAssign(rdbKey, *changeNode);
            });
        LOG_INFO("Rdb AddObservers end, subscriberId: %{public}d", static_cast<int>(subscriberId));
    }
}

void RdbSubscriberManagerTest::AddTimeConsumingObservers(std::shared_ptr<DataShare::DataShareHelper> helper,
    int64_t subscriberId, std::string &bundleName, std::string &uri, std::atomic<bool> &stop)
{
    std::vector<std::string> uris = { uri };
    TemplateId templateId;
    templateId.bundleName_ = bundleName;
    templateId.subscriberId_ = subscriberId;
    std::vector<PredicateTemplateNode> nodes;
    Template tpl(nodes, "select name1 as name from TBL00");
    auto result1 = helper->AddQueryTemplate(DATA_SHARE_PROXY_URI, subscriberId, tpl);
    EXPECT_EQ(result1, 0);
    while (!stop.load()) {
        LOG_INFO("Rdb AddObservers start, subscriberId: %{public}d", static_cast<int>(subscriberId));
        helper->SubscribeRdbData(uris, templateId, g_timeConsumingRbdCallback);
        LOG_INFO("Rdb AddObservers end, subscriberId: %{public}d", static_cast<int>(subscriberId));
    }
}

void RdbSubscriberManagerTest::DelObservers(
    int64_t subscriberId, std::string &bundleName, std::string &uri, std::atomic<bool> &stop)
{
    void *subscriber = g_subscriber;
    DataShare::TemplateId templateId;
    templateId.subscriberId_ = subscriberId, templateId.bundleName_ = bundleName;
    std::vector<std::string> uris;
    uris.emplace_back(uri);
    std::vector<Key> keys;
    std::for_each(uris.begin(), uris.end(), [&keys, &templateId](auto &uri) { keys.emplace_back(uri, templateId); });
    while (!stop.load()) {
        LOG_INFO("Rdb DelObservers start, subscriberId: %{public}d", static_cast<int>(subscriberId));
        RdbBaseCallbacks::DelObservers(
            keys, subscriber, [](const std::vector<Key> &lastDelKeys, std::vector<OperationResult> &opResult) {
                std::for_each(lastDelKeys.begin(), lastDelKeys.end(),
                    [](auto &result) { RdbSubscriberManager::GetInstance().lastChangeNodeMap_.Erase(result); });
            });
        LOG_INFO("Rdb DelObservers end, subscriberId: %{public}d", static_cast<int>(subscriberId));
    }
}

void RdbSubscriberManagerTest::DelTimeConsumingObservers(std::shared_ptr<DataShare::DataShareHelper> helper,
    int64_t subscriberId, std::string &bundleName, std::string &uri, std::atomic<bool> &stop)
{
    std::vector<std::string> uris = { uri };
    TemplateId templateId;
    templateId.bundleName_ = bundleName;
    templateId.subscriberId_ = subscriberId;
    while (!stop.load()) {
        LOG_INFO("Rdb AddObservers start, subscriberId: %{public}d", static_cast<int>(subscriberId));
        helper->UnsubscribeRdbData(uris, templateId);
        LOG_INFO("Rdb AddObservers end, subscriberId: %{public}d", static_cast<int>(subscriberId));
    }
}

/**
 * @tc.name: ConcurrentRdbObserverTest
 * @tc.desc: Verify the functionality and stability of concurrent SubscribeRdbData and UnsubscribeRdbData operations,
 *           focusing on crash prevention, deadlock avoidance, and consistency of observer management.
 * @tc.type: concurrent
 * @tc.require: None
 * @tc.precon:
    1. The RdbSubscriberManager is properly initialized and in a functional state before the test starts.
    2. The RdbSubscriberManagerTest class provides valid AddObservers and DelObservers methods to manage RDB observers.
    3. The test environment supports multi-threaded operations with std::thread, std::atomic, and thread
       synchronization.
 * @tc.step:
    1. Create an instance of RdbSubscriberManagerTest for observer management operations.
    2. Define two test URIs (uri0 = "uri0", uri1 = "uri1") and two bundle names (bundleName0 = "bundleName0",
       bundleName1 = "bundleName1").
    3. Create four threads to perform concurrent operations:
        - Thread 1: Call AddObservers for uri0 with bundleName0 (controlled by a stop flag).
        - Thread 2: Call DelObservers for uri0 with bundleName0 (controlled by a stop flag).
        - Thread 3: Call AddObservers for uri1 with bundleName1 (controlled by a stop flag).
        - Thread 4: Call DelObservers for uri1 with bundleName1 (controlled by a stop flag).
    4. Run the concurrent operations for a specified test duration (TEST_TIME).
    5. Set the stop flag to true, then join all threads to wait for their completion.
 * @tc.expect:
    1. All concurrent add/delete observer operations complete without crashes or exceptions.
    2. No deadlocks occur during the entire test duration.
    3. The RdbSubscriberManager maintains internal consistency in observer management (no corrupted states).
 */
HWTEST_F(ConcurrentSubscriberTest, ConcurrentRdbObserverTest, TestSize.Level0)
{
    std::atomic<bool> stop = false;
    int testTime = TEST_TIME;
    RdbSubscriberManagerTest instance;
    std::string uri0 = "uri0";
    std::string uri1 = "uri1";
    std::string bundleName0 = "bundleName0";
    std::string bundleName1 = "bundleName1";
    std::function<void()> func1 = [&instance, &bundleName0, &uri0, &stop]() {
        instance.AddObservers(0, bundleName0, uri0, stop);
    };
    std::function<void()> func2 = [&instance, &bundleName0, &uri0, &stop]() {
        instance.DelObservers(0, bundleName0, uri0, stop);
    };
    std::function<void()> func3 = [&instance, &bundleName1, &uri1, &stop]() {
        instance.AddObservers(1, bundleName1, uri1, stop);
    };
    std::function<void()> func4 = [&instance, &bundleName1, &uri1, &stop]() {
        instance.DelObservers(1, bundleName1, uri1, stop);
    };
    std::thread t1(func1);
    std::thread t2(func2);
    std::thread t3(func3);
    std::thread t4(func4);
    while (testTime > 0) {
        sleep(1);
        testTime--;
    }
    stop = true;
    t1.join();
    t2.join();
    t3.join();
    t4.join();
}


/**
 * @tc.name: ConcurrentRdbOnChangeTest
 * @tc.desc: Verify concurrent SubscribeRdbData and UnsubscribeRdbData and Notify operations
 * @tc.type: concurrent
 * @tc.require: None
 * @tc.precon: RdbSubscriberManager is properly initialized
 * @tc.step: Subscribe and Unsubscribe rdbDataChange of two uris and notify in multithread
 * @tc.expect:
    1. All concurrent operations complete without crashes
    2. No deadlocks occur during concurrent subscription management
 */
HWTEST_F(ConcurrentSubscriberTest, ConcurrentRdbOnChangeTest, TestSize.Level0)
{
    CreateOptions options;
    options.enabled_ = true;
    std::shared_ptr<DataShare::DataShareHelper> helper =
        DataShare::DataShareHelper::Creator(DATA_SHARE_PROXY_URI, options);
    ASSERT_NE(helper, nullptr);

    std::atomic<bool> stop = false;
    int testTime = TEST_TIME;
    RdbSubscriberManagerTest instance;
    std::string uri0 = DATA_SHARE_PROXY_URI;
    std::string uri1 = "uri1";
    std::string bundleName0 = "bundleName0";
    std::string bundleName1 = "bundleName1";
    std::function<void()> func1 = [&helper, &instance, &bundleName0, &uri0, &stop]() {
        instance.AddTimeConsumingObservers(helper, 0, bundleName0, uri0, stop);
    };
    std::function<void()> func2 = [&helper, &instance, &bundleName0, &uri1, &stop]() {
        instance.AddTimeConsumingObservers(helper, 1, bundleName0, uri1, stop);
    };
    std::function<void()> func3 = [&helper, &instance, &bundleName0, &uri1, &stop]() {
        instance.DelTimeConsumingObservers(helper, 1, bundleName0, uri1, stop);
    };
    std::function<void()> func4 = [&helper, &instance, &bundleName1, &uri0, &stop]() {
        Uri uri(uri0);
        DataShare::DataShareValuesBucket valuesBucket;
        valuesBucket.Put("name1", 1);
        auto ret = helper->Insert(uri, valuesBucket);
        EXPECT_GT(ret, 0);
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
}

class PublishedDataSubscriberManagerTest : public CallbacksManager<PublishedObserverMapKey, PublishedDataObserver> {
public:
    using Callback = std::function<void(const PublishedDataChangeNode &changeNode)>;
    using Key = PublishedObserverMapKey;
    using Observer = PublishedDataObserver;
    void AddObservers(int64_t subscriberId, std::string &uri, std::atomic<bool> &stop);
    void DelObservers(int64_t subscriberId, std::string &uri, std::atomic<bool> &stop);
};

void PublishedDataSubscriberManagerTest::AddObservers(int64_t subscriberId, std::string &uri, std::atomic<bool> &stop)
{
    void *subscriber = g_subscriber;
    PublishedDataChangeNode *changeNode = &g_publishedChangeNode;
    std::vector<std::string> uris;
    uris.emplace_back(uri);
    Key publishedKey(uri, subscriberId);
    std::vector<Key> keys;
    std::for_each(
        uris.begin(), uris.end(), [&keys, &subscriberId](auto &uri) { keys.emplace_back(uri, subscriberId); });
    while (!stop.load()) {
        LOG_INFO("Published AddObservers start, subscriberId: %{public}d", static_cast<int>(subscriberId));
        PublishedBaseCallbacks::AddObservers(
            keys, subscriber, std::make_shared<Observer>(g_publishedCallback),
            [](const std::vector<Key> &localRegisterKeys, const std::shared_ptr<Observer> observer) {},
            [&subscriber, &publishedKey, &changeNode](const std::vector<Key> &firstAddKeys,
                const std::shared_ptr<Observer> observer, std::vector<OperationResult> &opResult) {
                std::vector<std::string> firstAddUris;
                std::for_each(firstAddKeys.begin(), firstAddKeys.end(),
                    [&firstAddUris](auto &result) { firstAddUris.emplace_back(result); });
                if (firstAddUris.empty()) {
                    return;
                }
                PublishedDataSubscriberManager::GetInstance().lastChangeNodeMap_.Compute(
                    publishedKey, [](const Key &, PublishedDataChangeNode &value) {
                        value.datas_.clear();
                        return true;
                    });
                PublishedDataSubscriberManager::GetInstance().lastChangeNodeMap_.Compute(
                    publishedKey, [&publishedKey](const Key &, PublishedDataChangeNode &value) {
                        value.datas_.emplace_back(publishedKey.uri_, publishedKey.subscriberId_, "data");
                        value.ownerBundleName_ = "";
                        return true;
                    });
            });
        LOG_INFO("Published AddObservers end, subscriberId: %{public}d", static_cast<int>(subscriberId));
    }
}

void PublishedDataSubscriberManagerTest::DelObservers(int64_t subscriberId, std::string &uri, std::atomic<bool> &stop)
{
    void *subscriber = g_subscriber;
    std::vector<std::string> uris;
    uris.emplace_back(uri);
    std::vector<Key> keys;
    std::for_each(
        uris.begin(), uris.end(), [&keys, &subscriberId](auto &uri) { keys.emplace_back(uri, subscriberId); });
    while (!stop.load()) {
        LOG_INFO("Published DelObservers start, subscriberId: %{public}d", static_cast<int>(subscriberId));
        PublishedBaseCallbacks::DelObservers(
            keys, subscriber, [](const std::vector<Key> &lastDelKeys, std::vector<OperationResult> &opResult) {
                std::for_each(lastDelKeys.begin(), lastDelKeys.end(), [](auto &result) {
                    PublishedDataSubscriberManager::GetInstance().lastChangeNodeMap_.Erase(result);
                });
            });
        LOG_INFO("Published DelObservers end, subscriberId: %{public}d", static_cast<int>(subscriberId));
    }
}

/**
 * @tc.name: ConcurrentPublishObserverTest
 * @tc.desc: Verify the functionality and stability of concurrent SubscribePublishedData and
 *           UnsubscribePublishedData operations, focusing on crash prevention, deadlock avoidance,
 *           and consistency of published data observer management.
 * @tc.type: concurrent
 * @tc.require: None
 * @tc.precon:
    1. The PublishedDataSubscriberManager is properly initialized and in a functional state before
       the test starts.
    2. The PublishedDataSubscriberManagerTest class provides valid AddObservers and DelObservers
       methods to manage published data observers.
    3. The test environment supports multi-threaded operations with std::thread, std::atomic, and
       thread synchronization mechanisms.
 * @tc.step:
    1. Create an instance of PublishedDataSubscriberManagerTest for managing published data observers.
    2. Define two test URIs: uri0 = "uri0" and uri1 = "uri1".
    3. Create four threads to perform concurrent operations:
        - Thread 1: Call AddObservers with subscriber ID 0 and uri0 (controlled by a stop flag).
        - Thread 2: Call DelObservers with subscriber ID 0 and uri0 (controlled by a stop flag).
        - Thread 3: Call AddObservers with subscriber ID 1 and uri1 (controlled by a stop flag).
        - Thread 4: Call DelObservers with subscriber ID 1 and uri1 (controlled by a stop flag).
    4. Run the concurrent operations for a specified test duration (TEST_TIME).
    5. Set the stop flag to true, then join all threads to wait for their completion.
 * @tc.expect:
    1. All concurrent add/delete observer operations complete without crashes or exceptions.
    2. No deadlocks occur during the entire test duration.
    3. The published data observer map maintains internal consistency (no corrupted states).
    4. Change node data is properly managed and remains consistent during concurrent access.
 */
HWTEST_F(ConcurrentSubscriberTest, ConcurrentPublishObserverTest, TestSize.Level0)
{
    std::atomic<bool> stop = false;
    int testTime = TEST_TIME;
    PublishedDataSubscriberManagerTest instance;
    std::string uri0 = "uri0";
    std::string uri1 = "uri1";
    std::function<void()> func1 = [&instance, &uri0, &stop]() { instance.AddObservers(0, uri0, stop); };
    std::function<void()> func2 = [&instance, &uri0, &stop]() { instance.DelObservers(0, uri0, stop); };
    std::function<void()> func3 = [&instance, &uri1, &stop]() { instance.AddObservers(1, uri1, stop); };
    std::function<void()> func4 = [&instance, &uri1, &stop]() { instance.DelObservers(1, uri1, stop); };
    std::thread t1(func1);
    std::thread t2(func2);
    std::thread t3(func3);
    std::thread t4(func4);
    while (testTime > 0) {
        sleep(1);
        testTime--;
    }
    stop = true;
    t1.join();
    t2.join();
    t3.join();
    t4.join();
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
    static constexpr int64_t INTERVAL = 2;
};

class DataShareObserverTest : public DataShare::DataShareObserver {
public:
    explicit DataShareObserverTest(std::string uri)
    {
        uri_ = uri;
    }
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
    std::string uri_;
};

class ConcurrentRegisterObserverExtProvider {
public:
    void RegisterObserverExtProvider(std::shared_ptr<DataShare::DataShareHelper> helper, const Uri &uri,
        std::shared_ptr<DataShareObserver> dataObserver, std::atomic<bool> &stop)
    {
        while (!stop.load()) {
            LOG_INFO("RegisterObserverExtProvider start, uri: %{public}s",
                DataShareStringUtils::Anonymous(uri.ToString()).c_str());

            helper->RegisterObserverExtProvider(uri, dataObserver, true);

            LOG_INFO("RegisterObserverExtProvider end, uri: %{public}s",
                DataShareStringUtils::Anonymous(uri.ToString()).c_str());
        }
    }

    void UnregisterObserverExtProvider(std::shared_ptr<DataShare::DataShareHelper> helper, const Uri &uri,
        std::shared_ptr<DataShareObserver> dataObserver, std::atomic<bool> &stop)
    {
        while (!stop.load()) {
            LOG_INFO("UnregisterObserverExtProvider start, uri: %{public}s",
                DataShareStringUtils::Anonymous(uri.ToString()).c_str());

            helper->UnregisterObserverExtProvider(uri, dataObserver);

            LOG_INFO("UnregisterObserverExtProvider end, uri: %{public}s",
                DataShareStringUtils::Anonymous(uri.ToString()).c_str());
        }
    }
};

std::string DATA_SHARE_URI = "datashare:///com.acts.datasharetest";
std::string DATA_SHARE_URI1 = "datashare:///com.acts.datasharetest1";
std::string DATA_SHARE_URI2 = "datashare:///com.acts.datasharetest2";
constexpr int STORAGE_MANAGER_MANAGER_ID = 5003;

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

/**
 * @tc.name: ConcurrentRegisterObserverExtProviderTest
 * @tc.desc: Verify concurrent RegisterObserverExtProvider and UnregisterObserverExtProvider operations
 * @tc.type: concurrent
 * @tc.require: None
 * @tc.precon:
    1. DataShare service is properly initialized
    2. STORAGE_MANAGER_MANAGER_ID system ability is available
 * @tc.step:
    1. Create a DataShareHelper instance using STORAGE_MANAGER_MANAGER_ID
    2. Define two URIs for testing (DATA_SHARE_URI1 and DATA_SHARE_URI2)
    3. Create two DataShareObserverTest instances for the URIs
    4. Create four threads to concurrently perform:
        - Register observer for URI1
        - Unregister observer for URI1
        - Register observer for URI2
        - Unregister observer for URI2
    5. Run the concurrent operations for a specified test duration
    6. Stop all threads and wait for their completion
 * @tc.expect:
    1. DataShareHelper is created successfully (not nullptr)
    2. All concurrent registration and unregistration operations complete without crashes
    3. No deadlocks occur during concurrent observer management
    4. Observer registration state remains consistent during concurrent access
 */
HWTEST_F(ConcurrentSubscriberTest, ConcurrentRegisterObserverExtProviderTest, TestSize.Level0)
{
    LOG_INFO("ConcurrentRegisterObserverExtProviderTest::Start");
    std::atomic<bool> stop = false;
    int testTime = TEST_TIME;
    ConcurrentRegisterObserverExtProvider instance;
    Uri uri1(DATA_SHARE_URI1);
    Uri uri2(DATA_SHARE_URI2);
    std::shared_ptr<DataShareObserver> dataObserver1 = std::make_shared<DataShareObserverTest>(DATA_SHARE_URI1);
    std::shared_ptr<DataShareObserver> dataObserver2 = std::make_shared<DataShareObserverTest>(DATA_SHARE_URI2);
    std::shared_ptr<DataShare::DataShareHelper> helper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID);
    ASSERT_NE(helper, nullptr);

    std::function<void()> func1 = [&instance, &helper, &uri1, &dataObserver1, &stop]() {
        instance.RegisterObserverExtProvider(helper, uri1, dataObserver1, stop);
    };
    std::function<void()> func2 = [&instance, &helper, &uri1, &dataObserver1, &stop]() {
        instance.UnregisterObserverExtProvider(helper, uri1, dataObserver1, stop);
    };
    std::function<void()> func3 = [&instance, &helper, &uri2, &dataObserver2, &stop]() {
        instance.RegisterObserverExtProvider(helper, uri2, dataObserver2, stop);
    };
    std::function<void()> func4 = [&instance, &helper, &uri2, &dataObserver2, &stop]() {
        instance.UnregisterObserverExtProvider(helper, uri2, dataObserver2, stop);
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
    LOG_INFO("ConcurrentRegisterObserverExtProviderTest::end");
}
} // namespace DataShare
} // namespace OHOS