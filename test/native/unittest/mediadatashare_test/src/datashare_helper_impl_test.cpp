/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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
#include "datashare_helper_impl.h"

#include <gtest/gtest.h>

#include <memory>

#include "datashare_log.h"
#include "gmock/gmock.h"

namespace OHOS {
namespace DataShare {
using namespace testing::ext;

class MockGeneralController : public GeneralController {
public:
    MOCK_METHOD(int, Insert, (const Uri &uri, const DataShareValuesBucket &value), (override));
    MOCK_METHOD(int, Update,
        (const Uri &uri, const DataSharePredicates &predicates, const DataShareValuesBucket &value), (override));
    MOCK_METHOD(int, Delete, (const Uri &uri, const DataSharePredicates &predicates), (override));
    MOCK_METHOD((std::shared_ptr<DataShareResultSet>), Query,
        (const Uri &uri, const DataSharePredicates &predicates, (std::vector<std::string> & columns),
            DatashareBusinessError &businessError),
        (override));
    MOCK_METHOD(
        int, RegisterObserver, (const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver), (override));
    MOCK_METHOD(int32_t, UnregisterObserver, (const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver),
        (override));
    MOCK_METHOD(void, NotifyChange, (const Uri &uri), (override));
    MOCK_METHOD(
        int, RegisterObserverExtProvider, (const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver,
            bool isDescendants), (override));
    MOCK_METHOD(int32_t, UnregisterObserverExtProvider, (const Uri &uri,
        const sptr<AAFwk::IDataAbilityObserver> &dataObserver), (override));
    MOCK_METHOD(int32_t, NotifyChangeExtProvider, (const ChangeInfo &changeInfo), (override));
    MOCK_METHOD(
        (std::pair<int32_t, int32_t>), InsertEx, (const Uri &uri, const DataShareValuesBucket &value), (override));
    MOCK_METHOD((std::pair<int32_t, int32_t>), UpdateEx,
        (const Uri &uri, const DataSharePredicates &predicates, const DataShareValuesBucket &value), (override));
    MOCK_METHOD(
        (std::pair<int32_t, int32_t>), DeleteEx, (const Uri &uri, const DataSharePredicates &predicates), (override));
};

class DataShareHelperImplTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    static std::shared_ptr<DataShareHelperImpl> GetInstance(std::shared_ptr<DataShareHelperImpl> instance = nullptr);
    static std::shared_ptr<MockGeneralController> GetController(
        std::shared_ptr<MockGeneralController> instance = nullptr);
};

void DataShareHelperImplTest::SetUpTestCase(void)
{
    DataShareHelperImplTest::GetInstance(std::make_shared<DataShareHelperImpl>("datashare://datasharehelperimpl"));
    EXPECT_NE(DataShareHelperImplTest::GetInstance(), nullptr);
    DataShareHelperImplTest::GetController(std::make_shared<MockGeneralController>());
    EXPECT_NE(DataShareHelperImplTest::GetController(), nullptr);
    DataShareHelperImplTest::GetInstance()->generalCtl_ = DataShareHelperImplTest::GetController();
    EXPECT_NE(DataShareHelperImplTest::GetInstance()->generalCtl_, nullptr);
    EXPECT_EQ(DataShareHelperImplTest::GetInstance()->extSpCtl_, nullptr);
    DataShareHelperImplTest::GetInstance()->persistentDataCtl_ = nullptr;
    DataShareHelperImplTest::GetInstance()->publishedDataCtl_ = nullptr;
}
void DataShareHelperImplTest::TearDownTestCase(void)
{
}
void DataShareHelperImplTest::SetUp(void)
{
}
void DataShareHelperImplTest::TearDown(void)
{
}
std::shared_ptr<DataShareHelperImpl> DataShareHelperImplTest::GetInstance(std::shared_ptr<DataShareHelperImpl> instance)
{
    static std::shared_ptr<DataShareHelperImpl> helperInstance = nullptr;
    if (instance != nullptr) {
        helperInstance = instance;
    }
    return helperInstance;
}
std::shared_ptr<MockGeneralController> DataShareHelperImplTest::GetController(
    std::shared_ptr<MockGeneralController> instance)
{
    static std::shared_ptr<MockGeneralController> controllerInstance = nullptr;
    if (instance != nullptr) {
        controllerInstance = instance;
    }
    return controllerInstance;
}

HWTEST_F(DataShareHelperImplTest, BatchUpdateTest001, TestSize.Level0)
{
    LOG_INFO("BatchUpdateTest001::Start");
    UpdateOperations operations;
    std::vector<BatchUpdateResult> results = {};
    int result = DataShareHelperImplTest::GetInstance()->BatchUpdate(operations, results);
    EXPECT_EQ(result, DATA_SHARE_ERROR);
    LOG_INFO("BatchUpdateTest001::End");
}

HWTEST_F(DataShareHelperImplTest, InsertExTest001, TestSize.Level0)
{
    LOG_INFO("InsertExTest001::Start");
    OHOS::Uri uri("datashare:///com.datasharehelperimpl.test");
    DataShareValuesBucket value;
    DataShareHelperImplTest::GetInstance()->generalCtl_ = nullptr;
    std::pair<int32_t, int32_t> result = DataShareHelperImplTest::GetInstance()->InsertEx(uri, value);
    EXPECT_EQ(result.first, DATA_SHARE_ERROR);
    EXPECT_EQ(result.second, 0);
    std::shared_ptr<MockGeneralController> controller = DataShareHelperImplTest::GetController();
    DataShareHelperImplTest::GetInstance()->generalCtl_ = controller;
    EXPECT_CALL(*controller, InsertEx(testing::_, testing::_))
        .WillOnce(testing::Return(std::make_pair(E_REGISTERED_REPEATED, 0)));
    result = DataShareHelperImplTest::GetInstance()->InsertEx(uri, value);
    EXPECT_EQ(result.first, E_REGISTERED_REPEATED);
    EXPECT_EQ(result.second, 0);
    LOG_INFO("InsertExTest001::End");
}

HWTEST_F(DataShareHelperImplTest, UpdateExTest001, TestSize.Level0)
{
    LOG_INFO("UpdateExTest001::Start");
    OHOS::Uri uri("datashare:///com.datasharehelperimpl.test");
    DataShareValuesBucket value;
    DataSharePredicates predicates;
    DataShareHelperImplTest::GetInstance()->generalCtl_ = nullptr;
    std::pair<int32_t, int32_t> result = DataShareHelperImplTest::GetInstance()->UpdateEx(uri, predicates, value);
    EXPECT_EQ(result.first, DATA_SHARE_ERROR);
    EXPECT_EQ(result.second, 0);
    std::shared_ptr<MockGeneralController> controller = DataShareHelperImplTest::GetController();
    DataShareHelperImplTest::GetInstance()->generalCtl_ = controller;
    EXPECT_CALL(*controller, UpdateEx(testing::_, testing::_, testing::_))
        .WillOnce(testing::Return(std::make_pair(E_REGISTERED_REPEATED, 0)));
    result = DataShareHelperImplTest::GetInstance()->UpdateEx(uri, predicates, value);
    EXPECT_EQ(result.first, E_REGISTERED_REPEATED);
    EXPECT_EQ(result.second, 0);
    LOG_INFO("UpdateExTest001::End");
}

HWTEST_F(DataShareHelperImplTest, DeleteExTest001, TestSize.Level0)
{
    LOG_INFO("DeleteExTest001::Start");
    OHOS::Uri uri("datashare:///com.datasharehelperimpl.test");
    DataSharePredicates predicates;
    DataShareHelperImplTest::GetInstance()->generalCtl_ = nullptr;
    std::pair<int32_t, int32_t> result = DataShareHelperImplTest::GetInstance()->DeleteEx(uri, predicates);
    EXPECT_EQ(result.first, DATA_SHARE_ERROR);
    EXPECT_EQ(result.second, 0);
    std::shared_ptr<MockGeneralController> controller = DataShareHelperImplTest::GetController();
    DataShareHelperImplTest::GetInstance()->generalCtl_ = controller;
    EXPECT_CALL(*controller, DeleteEx(testing::_, testing::_))
        .WillOnce(testing::Return(std::make_pair(E_REGISTERED_REPEATED, 0)));
    result = DataShareHelperImplTest::GetInstance()->DeleteEx(uri, predicates);
    EXPECT_EQ(result.first, E_REGISTERED_REPEATED);
    EXPECT_EQ(result.second, 0);
    LOG_INFO("DeleteExTest001::End");
}

HWTEST_F(DataShareHelperImplTest, DelQueryTemplateTest001, TestSize.Level0)
{
    LOG_INFO("DelQueryTemplateTest001::Start");
    std::string uri("datashare:///com.datasharehelperimpl.test");
    int64_t subscriberId = 0;
    int result = DataShareHelperImplTest::GetInstance()->DelQueryTemplate(uri, subscriberId);
    EXPECT_EQ(result, DATA_SHARE_ERROR);
    LOG_INFO("DelQueryTemplateTest001::End");
}

HWTEST_F(DataShareHelperImplTest, PublishTest001, TestSize.Level0)
{
    LOG_INFO("PublishTest001::Start");
    Data data;
    std::string bundleName("datashare:///com.datasharehelperimpl.test");
    std::vector<OperationResult> result = DataShareHelperImplTest::GetInstance()->Publish(data, bundleName);
    EXPECT_EQ(result.size(), 0);
    LOG_INFO("PublishTest001::End");
}

HWTEST_F(DataShareHelperImplTest, GetPublishedDataTest001, TestSize.Level0)
{
    LOG_INFO("GetPublishedDataTest001::Start");
    int resultCode = 0;
    std::string bundleName("datashare:///com.datasharehelperimpl.test");
    Data result = DataShareHelperImplTest::GetInstance()->GetPublishedData(bundleName, resultCode);
    EXPECT_EQ(result.version_, 0);
    EXPECT_EQ(result.datas_.size(), 0);
    LOG_INFO("GetPublishedDataTest001::End");
}

HWTEST_F(DataShareHelperImplTest, SubscribeRdbDataTest001, TestSize.Level0)
{
    LOG_INFO("SubscribeRdbDataTest001::Start");
    std::vector<std::string> uris = {};
    TemplateId templateId;
    std::function<void(const RdbChangeNode &changeNode)> callback;
    std::vector<OperationResult> result =
        DataShareHelperImplTest::GetInstance()->SubscribeRdbData(uris, templateId, callback);
    EXPECT_EQ(result.size(), 0);
    LOG_INFO("SubscribeRdbDataTest001::End");
}

HWTEST_F(DataShareHelperImplTest, UnsubscribeRdbDataTest001, TestSize.Level0)
{
    LOG_INFO("UnsubscribeRdbDataTest001::Start");
    std::vector<std::string> uris = {};
    TemplateId templateId;
    std::vector<OperationResult> result = DataShareHelperImplTest::GetInstance()->UnsubscribeRdbData(uris, templateId);
    EXPECT_EQ(result.size(), 0);
    LOG_INFO("UnsubscribeRdbDataTest001::End");
}

HWTEST_F(DataShareHelperImplTest, DisableRdbSubsTest001, TestSize.Level0)
{
    LOG_INFO("DisableRdbSubsTest001::Start");
    std::vector<std::string> uris = {};
    TemplateId templateId;
    std::vector<OperationResult> result = DataShareHelperImplTest::GetInstance()->DisableRdbSubs(uris, templateId);
    EXPECT_EQ(result.size(), 0);
    LOG_INFO("DisableRdbSubsTest001::End");
}

HWTEST_F(DataShareHelperImplTest, SubscribePublishedDataTest001, TestSize.Level0)
{
    LOG_INFO("SubscribePublishedDataTest001::Start");
    std::vector<std::string> uris = {};
    int64_t subscriberId = 0;
    std::function<void(const PublishedDataChangeNode &changeNode)> callback;
    std::vector<OperationResult> result =
        DataShareHelperImplTest::GetInstance()->SubscribePublishedData(uris, subscriberId, callback);
    EXPECT_EQ(result.size(), 0);
    LOG_INFO("SubscribePublishedDataTest001::End");
}

HWTEST_F(DataShareHelperImplTest, UnsubscribePublishedDataTest001, TestSize.Level0)
{
    LOG_INFO("UnsubscribePublishedDataTest001::Start");
    std::vector<std::string> uris = {};
    int64_t subscriberId = 0;
    std::vector<OperationResult> result =
        DataShareHelperImplTest::GetInstance()->UnsubscribePublishedData(uris, subscriberId);
    EXPECT_EQ(result.size(), 0);
    LOG_INFO("UnsubscribePublishedDataTest001::End");
}

HWTEST_F(DataShareHelperImplTest, EnablePubSubsTest001, TestSize.Level0)
{
    LOG_INFO("EnablePubSubsTest001::Start");
    std::vector<std::string> uris = {};
    int64_t subscriberId = 0;
    std::vector<OperationResult> result = DataShareHelperImplTest::GetInstance()->EnablePubSubs(uris, subscriberId);
    EXPECT_EQ(result.size(), 0);
    LOG_INFO("EnablePubSubsTest001::End");
}

HWTEST_F(DataShareHelperImplTest, DisablePubSubsTest001, TestSize.Level0)
{
    LOG_INFO("DisableRdbSubsTest001::Start");
    std::vector<std::string> uris = {};
    int64_t subscriberId = 0;
    std::vector<OperationResult> result = DataShareHelperImplTest::GetInstance()->DisablePubSubs(uris, subscriberId);
    EXPECT_EQ(result.size(), 0);
    LOG_INFO("DisablePubSubsTest001::End");
}

/*
* @tc.desc: test UserDefineFunc with no extSpCtl_
* @tc.require: Null
*/
HWTEST_F(DataShareHelperImplTest, User_Define_func_No_ExtSpCtl_Test001, TestSize.Level0)
{
    LOG_INFO("User_Define_func_No_ExtSpCtl_Test001::Start");
    DataShareHelperImplTest::GetInstance()->extSpCtl_ = nullptr;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    auto result = DataShareHelperImplTest::GetInstance()->UserDefineFunc(data, reply, option);
    EXPECT_EQ(result, DATA_SHARE_ERROR);
    LOG_INFO("User_Define_func_No_ExtSpCtl_Test001::End");
}
} // namespace DataShare
} // namespace OHOS