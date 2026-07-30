/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include <atomic>
#include <memory>

#include "datashare_ani.h"

namespace OHOS {
namespace DataShare {
using namespace testing::ext;
using namespace DataShareAni;

class DataProxyHandleHolderTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void DataProxyHandleHolderTest::SetUpTestCase(void)
{
}
void DataProxyHandleHolderTest::TearDownTestCase(void)
{
}
void DataProxyHandleHolderTest::SetUp()
{
}
void DataProxyHandleHolderTest::TearDown()
{
}

/**
 * @tc.name: HolderDestructorReleasesManagerTest001
 * @tc.desc: Regression for DataProxyHandle observer callback retaining
 *           a freed manager which causes use-after-free. After the fix
 *           the manager class inherits std::enable_shared_from_this,
 *           so a weak_ptr captured by the proxy-data change callback
 *           is invalidated as soon as the holder (the only owner) is
 *           destroyed.
 */
HWTEST_F(DataProxyHandleHolderTest, HolderDestructorReleasesManagerTest001, TestSize.Level0)
{
    auto manager = std::make_shared<AniProxyDataSubscriberManager>(std::weak_ptr<DataProxyHandle>{});
    ASSERT_NE(manager, nullptr);

    std::weak_ptr<AniProxyDataSubscriberManager> weakSelf = manager->weak_from_this();
    EXPECT_FALSE(weakSelf.expired());

    manager.reset();
    EXPECT_TRUE(weakSelf.expired());
    EXPECT_EQ(weakSelf.lock(), nullptr);
}

/**
 * @tc.name: HolderDestructorLambdaIsSafeTest002
 * @tc.desc: Regression for the same DataProxyHandle use-after-free.
 *           Simulate the lambda body that the production fix uses.
 *           After the holder releases its shared_ptr, calling the lambda
 *           must be a no-op rather than dereferencing a freed object.
 */
HWTEST_F(DataProxyHandleHolderTest, HolderDestructorLambdaIsSafeTest002, TestSize.Level0)
{
    auto manager = std::make_shared<AniProxyDataSubscriberManager>(std::weak_ptr<DataProxyHandle>{});
    std::weak_ptr<AniProxyDataSubscriberManager> weakSelf = manager->weak_from_this();

    manager.reset();

    std::atomic<int> emitCount{ 0 };
    auto cb = [&emitCount, weakSelf](const std::vector<DataProxyChangeInfo> &) {
        auto self = weakSelf.lock();
        if (self == nullptr) {
            return;
        }
        emitCount.fetch_add(1, std::memory_order_relaxed);
    };

    std::vector<DataProxyChangeInfo> change;
    cb(change);
    cb(change);
    EXPECT_EQ(emitCount.load(), 0);
}

/**
 * @tc.name: HolderDestructorNoLeakTest003
 * @tc.desc: Regression for DataProxyHandle cleanup racing with observer
 *           callback which causes use-after-free. DataProxyHandleHolder
 *           now has a destructor that drops the subscriber manager first
 *           so any in-flight callback holding a weak_ptr can no longer
 *           lock() to a live object.
 */
HWTEST_F(DataProxyHandleHolderTest, HolderDestructorNoLeakTest003, TestSize.Level0)
{
    std::shared_ptr<DataProxyHandle> dummyHandle;
    std::weak_ptr<AniProxyDataSubscriberManager> weakSelf;

    {
        DataProxyHandleHolder holder(dummyHandle);
        holder.jsProxyDataObsManager_ =
            std::make_shared<AniProxyDataSubscriberManager>(std::weak_ptr<DataProxyHandle>{ dummyHandle });
        weakSelf = holder.jsProxyDataObsManager_->weak_from_this();
        EXPECT_FALSE(weakSelf.expired());
    }
    EXPECT_TRUE(weakSelf.expired());
}
} // namespace DataShare
} // namespace OHOS