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
#include <gtest/gtest.h>
#include <unistd.h>

#include "callbacks_manager.h"
#include "datashare_errno.h"
#include "datashare_log.h"
#include "datashare_template.h"
#include "published_data_subscriber_manager.h"
#include "rdb_subscriber_manager.h"

namespace OHOS {
namespace DataShare {
using namespace testing::ext;
using RdbBaseCallbacks = CallbacksManager<RdbObserverMapKey, RdbObserver>;
using RdbCallback = std::function<void(const RdbChangeNode &changeNode)>;
RdbCallback g_rbdCallback = [](const RdbChangeNode &changeNode) {};
RdbChangeNode g_rdbChangeNode;
using PublishedBaseCallbacks = CallbacksManager<PublishedObserverMapKey, PublishedDataObserver>;
using PublishedDataCallback = std::function<void(PublishedDataChangeNode &changeNode)>;
PublishedDataCallback g_publishedCallback = [](const PublishedDataChangeNode &changeNode) {};
PublishedDataChangeNode g_publishedChangeNode;
constexpr int TEST_TIME = 20;
void *g_subscriber;

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
    void DelObservers(int64_t subscriberId, std::string &bundleName, std::string &uri, std::atomic<bool> &stop);
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

/**
 * @tc.name:ConcurrentRdbObserverTest
 * @tc.desc:verify concurrent SubscribeRdbData/UnsubscribeRdbData scenario
 * @tc.type:concurrent
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
 * @tc.name:ConcurrentPublishObserverTest
 * @tc.desc:verify concurrent SubscribePublishedData/UnsubscribePublishedData scenario
 * @tc.type:concurrent
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

} // namespace DataShare
} // namespace OHOS