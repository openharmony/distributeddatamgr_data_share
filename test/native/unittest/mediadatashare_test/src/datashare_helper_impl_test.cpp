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
            DatashareBusinessError &businessError, DataShareOption &option), (override));
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

/**
* @tc.name: QueryTest001
* @tc.desc: Verify Query function behavior under different controller states
* @tc.type: FUNC
* @tc.require: issueIC8OCN
* @tc.precon: DataShareHelperImpl instance is properly initialized
* @tc.step:
    1. Create test URI and empty predicates/columns
    2. Set general controller to null and execute Query
    3. Initialize mock controller and set it as general controller
    4. Verify Query returns expected result set with mock controller
    5. Verify Query works with business error parameter
* @tc.expect:
    1. Query returns nullptr when controller is null
    2. Query returns expected result set when using mock controller
    3. Query works correctly with business error parameter
*/
HWTEST_F(DataShareHelperImplTest, QueryTest001, TestSize.Level0)
{
    LOG_INFO("QueryTest001::Start");
    OHOS::Uri uri("datashare:///com.datasharehelperimpl.test");
    DataSharePredicates predicates;
    std::vector<std::string> columns;
    DataShareHelperImplTest::GetInstance()->generalCtl_ = nullptr;
    auto result = DataShareHelperImplTest::GetInstance()->Query(uri, predicates, columns, nullptr);
    EXPECT_EQ(result, nullptr);
    auto expectResult = std::make_shared<DataShareResultSet>();
    std::shared_ptr<MockGeneralController> controller = DataShareHelperImplTest::GetController();
    DataShareHelperImplTest::GetInstance()->generalCtl_ = controller;
    EXPECT_CALL(*controller, Query(testing::_, testing::_, testing::_, testing::_, testing::_))
        .WillOnce(testing::Return(expectResult));
    result = DataShareHelperImplTest::GetInstance()->Query(uri, predicates, columns, nullptr);
    EXPECT_EQ(result.get(), expectResult.get());
    DatashareBusinessError error;
    EXPECT_CALL(*controller, Query(testing::_, testing::_, testing::_, testing::_, testing::_))
        .WillOnce(testing::Return(expectResult));
    result = DataShareHelperImplTest::GetInstance()->Query(uri, predicates, columns, &error);
    EXPECT_EQ(result.get(), expectResult.get());
    LOG_INFO("QueryTest001::End");
}

/**
* @tc.name: QueryTimeoutTest001
* @tc.desc: Verify QueryTimeout function behavior under different controller states
* @tc.type: FUNC
* @tc.require: issueIC8OCN
* @tc.precon: DataShareHelperImpl instance is properly initialized
* @tc.step:
    1. Create test URI, empty predicates/columns and default option
    2. Set general controller to null and execute QueryTimeout
    3. Initialize mock controller and set it as general controller
    4. Verify QueryTimeout returns expected result set with mock controller
    5. Verify QueryTimeout works with business error parameter
* @tc.expect:
    1. QueryTimeout returns nullptr when controller is null
    2. QueryTimeout returns expected result set when using mock controller
    3. QueryTimeout works correctly with business error parameter
*/
HWTEST_F(DataShareHelperImplTest, QueryTimeoutTest001, TestSize.Level0)
{
    LOG_INFO("QueryTimeoutTest001::Start");
    OHOS::Uri uri("datashare:///com.datasharehelperimpl.test");
    DataSharePredicates predicates;
    std::vector<std::string> columns;
    DataShareOption option;
    DataShareHelperImplTest::GetInstance()->generalCtl_ = nullptr;
    auto result = DataShareHelperImplTest::GetInstance()->QueryTimeout(uri, predicates, columns, option, nullptr);
    EXPECT_EQ(result, nullptr);
    auto expectResult = std::make_shared<DataShareResultSet>();
    std::shared_ptr<MockGeneralController> controller = DataShareHelperImplTest::GetController();
    DataShareHelperImplTest::GetInstance()->generalCtl_ = controller;
    EXPECT_CALL(*controller, Query(testing::_, testing::_, testing::_, testing::_, testing::_))
        .WillOnce(testing::Return(expectResult));
    result = DataShareHelperImplTest::GetInstance()->QueryTimeout(uri, predicates, columns, option, nullptr);
    EXPECT_EQ(result.get(), expectResult.get());
    DatashareBusinessError error;
    EXPECT_CALL(*controller, Query(testing::_, testing::_, testing::_, testing::_, testing::_))
        .WillOnce(testing::Return(expectResult));
    result = DataShareHelperImplTest::GetInstance()->QueryTimeout(uri, predicates, columns, option, &error);
    EXPECT_EQ(result.get(), expectResult.get());
    LOG_INFO("QueryTimeoutTest001::End");
}

/**
* @tc.name: BatchUpdateTest001
* @tc.desc: Verify BatchUpdate function behavior with empty operations
* @tc.type: FUNC
* @tc.require: issueIC8OCN
* @tc.precon: DataShareHelperImpl instance is properly initialized
* @tc.step:
    1. Create empty update operations and results vector
    2. Execute BatchUpdate with empty parameters
    3. Check return error code
* @tc.expect:
    1. BatchUpdate returns DATA_SHARE_ERROR when given empty operations
*/
HWTEST_F(DataShareHelperImplTest, BatchUpdateTest001, TestSize.Level0)
{
    LOG_INFO("BatchUpdateTest001::Start");
    UpdateOperations operations;
    std::vector<BatchUpdateResult> results = {};
    int result = DataShareHelperImplTest::GetInstance()->BatchUpdate(operations, results);
    EXPECT_EQ(result, DATA_SHARE_ERROR);
    LOG_INFO("BatchUpdateTest001::End");
}

/**
* @tc.name: InsertExTest001
* @tc.desc: Verify InsertEx handles null controller and error responses
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Set general controller to null and call InsertEx
    2. Restore mock controller and set expectation for error response
    3. Call InsertEx again with valid parameters
    4. Check returned results in both cases
* @tc.expect:
    1. First call returns DATA_SHARE_ERROR
    2. Second call returns E_REGISTERED_REPEATED error
*/
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

/**
* @tc.name: UpdateExTest001
* @tc.desc: Verify UpdateEx handles null controller and error responses
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Set general controller to null and call UpdateEx
    2. Restore mock controller and set expectation for error response
    3. Call UpdateEx again with valid parameters
    4. Check returned results in both cases
* @tc.expect:
    1. First call returns DATA_SHARE_ERROR
    2. Second call returns E_REGISTERED_REPEATED error
*/
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

/**
* @tc.name: DeleteExTest001
* @tc.desc: Verify DeleteEx handles null controller and error responses
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Set general controller to null and call DeleteEx
    2. Restore mock controller and set expectation for error response
    3. Call DeleteEx again with valid parameters
    4. Check returned results in both cases
* @tc.expect:
    1. First call returns DATA_SHARE_ERROR
    2. Second call returns E_REGISTERED_REPEATED error
*/
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

/**
* @tc.name: DelQueryTemplateTest001
* @tc.desc: Verify DelQueryTemplate returns error with valid parameters
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Call DelQueryTemplate with test URI and subscriber ID
    2. Check returned result code
* @tc.expect:
    1. DelQueryTemplate returns DATA_SHARE_ERROR
*/
HWTEST_F(DataShareHelperImplTest, DelQueryTemplateTest001, TestSize.Level0)
{
    LOG_INFO("DelQueryTemplateTest001::Start");
    std::string uri("datashare:///com.datasharehelperimpl.test");
    int64_t subscriberId = 0;
    int result = DataShareHelperImplTest::GetInstance()->DelQueryTemplate(uri, subscriberId);
    EXPECT_EQ(result, DATA_SHARE_ERROR);
    LOG_INFO("DelQueryTemplateTest001::End");
}

/**
* @tc.name: PublishTest001
* @tc.desc: Verify Publish returns empty result with valid parameters
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Create empty Data object and test bundle name
    2. Call Publish with these parameters
    3. Check size of returned result vector
* @tc.expect:
    1. Returned result vector is empty
*/
HWTEST_F(DataShareHelperImplTest, PublishTest001, TestSize.Level0)
{
    LOG_INFO("PublishTest001::Start");
    Data data;
    std::string bundleName("datashare:///com.datasharehelperimpl.test");
    std::vector<OperationResult> result = DataShareHelperImplTest::GetInstance()->Publish(data, bundleName);
    EXPECT_EQ(result.size(), 0);
    LOG_INFO("PublishTest001::End");
}

/**
* @tc.name: GetPublishedDataTest001
* @tc.desc: Verify GetPublishedData returns empty data with valid parameters
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Create test bundle name and result code variable
    2. Call GetPublishedData with these parameters
    3. Check version and size of returned Data object
* @tc.expect:
    1. Returned Data object has version 0 and empty data vector
*/
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

/**
* @tc.name: SubscribeRdbDataTest001
* @tc.desc: Verify SubscribeRdbData returns empty result with empty URIs
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Create empty URIs vector, template ID and callback
    2. Call SubscribeRdbData with these parameters
    3. Check size of returned result vector
* @tc.expect:
    1. Returned result vector is empty
*/
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

/**
* @tc.name: UnsubscribeRdbDataTest001
* @tc.desc: Verify UnsubscribeRdbData returns empty result with empty URIs
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Create empty URIs vector and template ID
    2. Call UnsubscribeRdbData with these parameters
    3. Check size of returned result vector
* @tc.expect:
    1. Returned result vector is empty
*/
HWTEST_F(DataShareHelperImplTest, UnsubscribeRdbDataTest001, TestSize.Level0)
{
    LOG_INFO("UnsubscribeRdbDataTest001::Start");
    std::vector<std::string> uris = {};
    TemplateId templateId;
    std::vector<OperationResult> result = DataShareHelperImplTest::GetInstance()->UnsubscribeRdbData(uris, templateId);
    EXPECT_EQ(result.size(), 0);
    LOG_INFO("UnsubscribeRdbDataTest001::End");
}

/**
* @tc.name: DisableRdbSubsTest001
* @tc.desc: Verify DisableRdbSubs returns empty result with empty URIs
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Create empty URIs vector and template ID
    2. Call DisableRdbSubs with these parameters
    3. Check size of returned result vector
* @tc.expect:
    1. Returned result vector is empty
*/
HWTEST_F(DataShareHelperImplTest, DisableRdbSubsTest001, TestSize.Level0)
{
    LOG_INFO("DisableRdbSubsTest001::Start");
    std::vector<std::string> uris = {};
    TemplateId templateId;
    std::vector<OperationResult> result = DataShareHelperImplTest::GetInstance()->DisableRdbSubs(uris, templateId);
    EXPECT_EQ(result.size(), 0);
    LOG_INFO("DisableRdbSubsTest001::End");
}

/**
* @tc.name: SubscribePublishedDataTest001
* @tc.desc: Verify SubscribePublishedData returns empty result with empty URIs
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Create empty URIs vector, subscriber ID and callback
    2. Call SubscribePublishedData with these parameters
    3. Check size of returned result vector
* @tc.expect:
    1. Returned result vector is empty
*/
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

/**
* @tc.name: UnsubscribePublishedDataTest001
* @tc.desc: Verify UnsubscribePublishedData returns empty result with empty URIs
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Create empty URIs vector and subscriber ID
    2. Call UnsubscribePublishedData with these parameters
    3. Check size of returned result vector
* @tc.expect:
    1. Returned result vector is empty
*/
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

/**
* @tc.name: EnablePubSubsTest001
* @tc.desc: Verify EnablePubSubs returns empty result with empty URIs
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Create empty URIs vector and subscriber ID
    2. Call EnablePubSubs with these parameters
    3. Check size of returned result vector
* @tc.expect:
    1. Returned result vector is empty
*/
HWTEST_F(DataShareHelperImplTest, EnablePubSubsTest001, TestSize.Level0)
{
    LOG_INFO("EnablePubSubsTest001::Start");
    std::vector<std::string> uris = {};
    int64_t subscriberId = 0;
    std::vector<OperationResult> result = DataShareHelperImplTest::GetInstance()->EnablePubSubs(uris, subscriberId);
    EXPECT_EQ(result.size(), 0);
    LOG_INFO("EnablePubSubsTest001::End");
}

/**
* @tc.name: DisablePubSubsTest001
* @tc.desc: Verify DisablePubSubs returns empty result with empty URIs
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Create empty URIs vector and subscriber ID
    2. Call DisablePubSubs with these parameters
    3. Check size of returned result vector
* @tc.expect:
    1. Returned result vector is empty
*/
HWTEST_F(DataShareHelperImplTest, DisablePubSubsTest001, TestSize.Level0)
{
    LOG_INFO("DisablePubSubsTest001::Start");
    std::vector<std::string> uris = {};
    int64_t subscriberId = 0;
    std::vector<OperationResult> result = DataShareHelperImplTest::GetInstance()->DisablePubSubs(uris, subscriberId);
    EXPECT_EQ(result.size(), 0);
    LOG_INFO("DisablePubSubsTest001::End");
}

/**
* @tc.name: User_Define_func_No_ExtSpCtl_Test001
* @tc.desc: Verify UserDefineFunc returns error when extSpCtl_ is null
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Set extSpCtl_ to null
    2. Create empty MessageParcel objects and MessageOption
    3. Call UserDefineFunc with these parameters
    4. Check returned result code
* @tc.expect:
    1. UserDefineFunc returns DATA_SHARE_ERROR
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