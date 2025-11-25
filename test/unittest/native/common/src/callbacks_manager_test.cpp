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
#define LOG_TAG "callbacks_manager_test"

#include <gtest/gtest.h>
#include <cstdint>
#include <memory>
#include <string>
#include "ashmem.h"
#include "datashare_log.h"
#include "refbase.h"
#include "callbacks_manager.h"
#include "rdb_subscriber_manager.h"

namespace OHOS {
namespace DataShare {
using namespace testing::ext;

class CallbacksManagerTest : public testing::Test {
public:
    static void SetUpTestCase(void){};
    static void TearDownTestCase(void){};
    void SetUp(){};
    void TearDown(){};
};

/**
* @tc.name: IsOperationSuccessTest001
* @tc.desc: Test IsOperationSuccess
* @tc.type: FUNC
* @tc.step:
    1. Create a TemplateId and OperationResult vector
    2. Insert correct and failed operation results into OperationResult.
    3. Create URIs for the correct and failed operations.
    4. Call IsOperationSuccess to check whether the URI operation is successful.
* @tc.expect: If the URI operation is successful, true is returned. Otherwise, false is returned.
*/
HWTEST_F(CallbacksManagerTest, IsOperationSuccessTest001, TestSize.Level0)
{
    LOG_INFO("IsOperationSuccessTest001::Start");

    std::string uri1 =  "datashare:///com.acts.datasharetest1";
    std::string uri2 =  "datashare:///com.acts.datasharetest2";
    CallbacksManager<RdbObserverMapKey, RdbObserver> callbacksManager;
    std::vector<OperationResult> unsubResult;
    TemplateId templateId;
    RdbObserverMapKey key1(uri1, templateId);
    EXPECT_TRUE(callbacksManager.AreAllOpsSucceeded(unsubResult, key1));
    unsubResult.emplace_back(uri1, 0);
    EXPECT_TRUE(callbacksManager.AreAllOpsSucceeded(unsubResult, key1));
    RdbObserverMapKey key2(uri2, templateId);
    unsubResult.emplace_back(uri2, -1);
    EXPECT_FALSE(callbacksManager.AreAllOpsSucceeded(unsubResult, key2));
    EXPECT_TRUE(callbacksManager.AreAllOpsSucceeded(unsubResult, key1));
    RdbObserverMapKey key3("", templateId);
    EXPECT_TRUE(callbacksManager.AreAllOpsSucceeded(unsubResult, key3));
    
    LOG_INFO("IsOperationSuccessTest001::End");
}

/**
* @tc.name: recoverLocalObserversTest001
* @tc.desc: Test recoverLocalObservers
* @tc.type: FUNC
* @tc.step:
    1. Create a recoverCallbacks and OperationResult vector
    2. Insert correct and failed operation results into OperationResult.
    3. Create recoverCallbacks to be recovered.
    4. Call recoverLocalObservers to recover failed operation.
* @tc.expect: If the URI operation is failed, callbacks will be recovered.
*/
HWTEST_F(CallbacksManagerTest, recoverLocalObserversTest001, TestSize.Level0)
{
    LOG_INFO("recoverLocalObserversTest001::Start");
    
    std::string uri1 =  "aa";
    std::string uri2 =  "bb";
    std::string uri3 =  "cc";
    const RdbCallback callback = [](const RdbChangeNode &changeNode){};
    std::vector<OperationResult> unsubResult;
    TemplateId templateId;
    std::map<RdbObserverMapKey, std::vector<CallbacksManager<RdbObserverMapKey,
        RdbObserver>::ObserverNode>> recoverCallbacks;

    CallbacksManager<RdbObserverMapKey, RdbObserver> callbacksManager;
    EXPECT_TRUE(callbacksManager.callbacks_.empty());

    callbacksManager.RecoverLocalObservers(recoverCallbacks, unsubResult);
    EXPECT_TRUE(callbacksManager.callbacks_.empty());

    unsubResult.emplace_back(uri1, -1);
    unsubResult.emplace_back(uri2, 0);
    unsubResult.emplace_back(uri3, -1);
    callbacksManager.RecoverLocalObservers(recoverCallbacks, unsubResult);
    EXPECT_TRUE(callbacksManager.callbacks_.empty());

    RdbObserverMapKey key1(uri1, templateId);
    RdbObserverMapKey key2(uri2, templateId);
    RdbObserverMapKey key3(uri3, templateId);
    recoverCallbacks[key1].emplace_back(std::make_shared<RdbObserver>(callback), nullptr);
    recoverCallbacks[key2].emplace_back(std::make_shared<RdbObserver>(callback), nullptr);
    recoverCallbacks[key3].emplace_back(std::make_shared<RdbObserver>(callback), nullptr);
    callbacksManager.RecoverLocalObservers(recoverCallbacks, unsubResult);
    EXPECT_EQ(callbacksManager.callbacks_.size(), 2);

    LOG_INFO("recoverLocalObserversTest001::End");
}

/**
* @tc.name: DelObserversTest001
* @tc.desc: Test DelObservers
* @tc.type: FUNC
* @tc.step:
    1. Invoke AddObservers to insert three [keys, observer].
    2. Invoke DelObservers to delete three [keys, observer].
    3. Check whether callbacks_ is deleted.
* @tc.expect: callbacks_ is empty.
*/
HWTEST_F(CallbacksManagerTest, DelObserversTest001, TestSize.Level0)
{
    LOG_INFO("DelObserversTest001::Start");

    const RdbCallback callback = [](const RdbChangeNode &changeNode){};
    void *subscriber = nullptr;
    TemplateId templateId;
    std::vector<std::string> uris = {"aa", "bb", "cc"};
    CallbacksManager<RdbObserverMapKey, RdbObserver> callbacksManager;
    std::vector<RdbObserverMapKey> keys;
    std::for_each(uris.begin(), uris.end(), [&keys, &templateId](auto &uri) { keys.emplace_back(uri, templateId); });
    EXPECT_EQ(keys.size(), 3);
    EXPECT_TRUE(callbacksManager.callbacks_.empty());
    callbacksManager.AddObservers(keys, subscriber, std::make_shared<RdbObserver>(callback),
        [](const std::vector<RdbObserverMapKey> &localRegisterKeys, const std::shared_ptr<RdbObserver> observer) {},
        [](const std::vector<RdbObserverMapKey> &firstAddKeys,
            const std::shared_ptr<RdbObserver> &observer, std::vector<OperationResult> &opResult) {});
    EXPECT_EQ(callbacksManager.callbacks_.size(), 3);

    callbacksManager.DelObservers(keys, subscriber,
        [](const std::vector<RdbObserverMapKey> &lastDelKeys, std::vector<OperationResult> &opResult){});
    EXPECT_TRUE(callbacksManager.callbacks_.empty());
    
    LOG_INFO("DelObserversTest001::End");
}

/**
* @tc.name: DelObserversTest002
* @tc.desc: Test DelObservers
* @tc.type: FUNC
* @tc.step:
    1. Invoke AddObservers to insert three [keys, observer].
    2. Invoke DelObservers to delete three [keys, observer], opResult is failed during deletion.
    3. Invoke DelObservers to delete three [keys, observer], opResult is success during deletion.
* @tc.expect: When opResult failed, DelObservers will recover callbacks.
*/
HWTEST_F(CallbacksManagerTest, DelObserversTest002, TestSize.Level0)
{
    LOG_INFO("DelObserversTest002::Start");

    const RdbCallback callback = [](const RdbChangeNode &changeNode){};
    void *subscriber = nullptr;
    TemplateId templateId;
    std::vector<std::string> uris = {"aa", "bb", "cc"};
    CallbacksManager<RdbObserverMapKey, RdbObserver> callbacksManager;
    std::vector<RdbObserverMapKey> keys;
    std::for_each(uris.begin(), uris.end(), [&keys, &templateId](auto &uri) { keys.emplace_back(uri, templateId); });
    EXPECT_EQ(keys.size(), 3);
    EXPECT_TRUE(callbacksManager.callbacks_.empty());
    callbacksManager.AddObservers(keys, subscriber, std::make_shared<RdbObserver>(callback),
        [](const std::vector<RdbObserverMapKey> &localRegisterKeys, const std::shared_ptr<RdbObserver> observer) {},
        [](const std::vector<RdbObserverMapKey> &firstAddKeys,
            const std::shared_ptr<RdbObserver> &observer, std::vector<OperationResult> &opResult) {});
    EXPECT_EQ(callbacksManager.callbacks_.size(), 3);

    callbacksManager.DelObservers(keys, subscriber,
        [](const std::vector<RdbObserverMapKey> &lastDelKeys, std::vector<OperationResult> &opResult) {
            std::for_each(lastDelKeys.begin(), lastDelKeys.end(), [&opResult](auto &result) {
                opResult.emplace_back(result.uri_, -1);
            });
        });
    EXPECT_EQ(callbacksManager.callbacks_.size(), 3);

    callbacksManager.DelObservers(subscriber,
        [](const std::vector<RdbObserverMapKey> &lastDelKeys, std::vector<OperationResult> &opResult) {
            std::for_each(lastDelKeys.begin(), lastDelKeys.end(), [&opResult](auto &result) {
                opResult.emplace_back(result.uri_, 0);
            });
        });
    EXPECT_TRUE(callbacksManager.callbacks_.empty());
    
    LOG_INFO("DelObserversTest002::End");
}


} // namespace DataShare
} // namespace OHOS