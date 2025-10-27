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

#define LOG_TAG "datashare_helper_impl_test"

#include <gtest/gtest.h>
#include <memory>
#include "datashare_helper_impl.h"
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
 * @tc.name: BatchUpdateTest001
 * @tc.desc: Verify the behavior of the BatchUpdate function in DataShareHelperImpl when given empty UpdateOperations,
 *           ensuring it returns the expected error code.
 * @tc.type: FUNC
 * @tc.require: issueIC8OCN
 * @tc.precon:
    1. The DataShareHelperImpl instance is properly initialized via DataShareHelperImplTest::GetInstance().
    2. The test environment supports instantiation of empty UpdateOperations (operation list) and
       std::vector<BatchUpdateResult> (result list).
    3. The DATA_SHARE_ERROR constant is predefined and accessible as the expected error return value.
    4. The BatchUpdate function accepts UpdateOperations and std::vector<BatchUpdateResult> as input parameters.
 * @tc.step:
    1. Create an empty UpdateOperations object (operations) to store no batch update operations.
    2. Create an empty std::vector<BatchUpdateResult> object (results) to store the function's output results.
    3. Call the BatchUpdate function of the DataShareHelperImpl instance, passing 'operations' and 'results' as
       parameters.
    4. Check the integer return code of the BatchUpdate function to verify if it matches the expected error code.
 * @tc.expect:
    1. The BatchUpdate function returns DATA_SHARE_ERROR when provided with empty UpdateOperations.
    2. The 'results' vector remains empty after the function call (no invalid result entries added).
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
 * @tc.desc: Verify the behavior of the InsertEx function in DataShareHelperImpl when the general controller is null
 *           and when the mock controller returns an error (E_REGISTERED_REPEATED), checking the pair-type return
 *           value.
 * @tc.type: FUNC
 * @tc.require: issueIC8OCN
 * @tc.precon:
    1. The DataShareHelperImpl instance is properly initialized via DataShareHelperImplTest::GetInstance().
    2. The test environment supports creating DataShareValuesBucket, test URI, MockGeneralController, and handling
       the std::pair<int32_t, int32_t> return type of InsertEx.
    3. Predefined error codes (DATA_SHARE_ERROR, E_REGISTERED_REPEATED) are valid and accessible.
    4. The 'generalCtl_' member of DataShareHelperImpl can be set to null or a mock controller.
 * @tc.step:
    1. Create a test URI ("datashare:///com.datasharehelperimpl.test") and an empty DataShareValuesBucket (value).
    2. Set the 'generalCtl_' of the DataShareHelperImpl instance to null, then call InsertEx with the URI and 'value';
       record the returned (errorCode, retVal) pair.
    3. Create a mock MockGeneralController via DataShareHelperImplTest::GetController(), assign it to 'generalCtl_'.
    4. Set an expectation that the mock controller's InsertEx method returns (E_REGISTERED_REPEATED, 0), then call
       InsertEx again with the same URI and 'value'; record the new pair.
    5. Compare the recorded pairs with the expected error codes.
 * @tc.expect:
    1. The first InsertEx call (null controller) returns (DATA_SHARE_ERROR, 0).
    2. The second InsertEx call (mock controller with error) returns (E_REGISTERED_REPEATED, 0).
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
 * @tc.desc: Verify the behavior of the UpdateEx function in DataShareHelperImpl when the general controller is null
 *           and when the mock controller returns an error (E_REGISTERED_REPEATED), checking the pair-type return
 *           value.
 * @tc.type: FUNC
 * @tc.require: issueIC8OCN
 * @tc.precon:
    1. The DataShareHelperImpl instance is properly initialized via DataShareHelperImplTest::GetInstance().
    2. The test environment supports creating DataShareValuesBucket, DataSharePredicates, test URI,
       MockGeneralController, and handling the std::pair<int32_t, int32_t> return type of UpdateEx.
    3. Predefined error codes (DATA_SHARE_ERROR, E_REGISTERED_REPEATED) are valid and accessible.
    4. The 'generalCtl_' member of DataShareHelperImpl can be set to null or a mock controller.
 * @tc.step:
    1. Create a test URI ("datashare:///com.datasharehelperimpl.test"), empty DataShareValuesBucket (value),
       and empty DataSharePredicates (predicates).
    2. Set 'generalCtl_' to null, call UpdateEx with the URI, predicates, and 'value'; record the (errorCode, retVal)
       pair.
    3. Create a mock MockGeneralController via DataShareHelperImplTest::GetController(), assign it to 'generalCtl_'.
    4. Set an expectation that the mock controller's UpdateEx returns (E_REGISTERED_REPEATED, 0), then call UpdateEx
       again with the same parameters; record the new pair.
    5. Compare the recorded pairs with the expected error codes.
 * @tc.expect:
    1. The first UpdateEx call (null controller) returns (DATA_SHARE_ERROR, 0).
    2. The second UpdateEx call (mock controller with error) returns (E_REGISTERED_REPEATED, 0).
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
 * @tc.desc: Verify the behavior of the DeleteEx function in DataShareHelperImpl when the general controller is null
 *           and when the mock controller returns an error (E_REGISTERED_REPEATED), checking the pair-type return
 *           value.
 * @tc.type: FUNC
 * @tc.require: issueIC8OCN
 * @tc.precon:
    1. The DataShareHelperImpl instance is properly initialized via DataShareHelperImplTest::GetInstance().
    2. The test environment supports creating DataSharePredicates, test URI, MockGeneralController, and handling
       the std::pair<int32_t, int32_t> return type of DeleteEx.
    3. Predefined error codes (DATA_SHARE_ERROR, E_REGISTERED_REPEATED) are valid and accessible.
    4. The 'generalCtl_' member of DataShareHelperImpl can be set to null or a mock controller.
 * @tc.step:
    1. Create a test URI ("datashare:///com.datasharehelperimpl.test") and an empty DataSharePredicates (predicates).
    2. Set 'generalCtl_' to null, call DeleteEx with the URI and predicates; record the (errorCode, retVal) pair.
    3. Create a mock MockGeneralController via DataShareHelperImplTest::GetController(), assign it to 'generalCtl_'.
    4. Set an expectation that the mock controller's DeleteEx returns (E_REGISTERED_REPEATED, 0), then call DeleteEx
       again with the same URI and predicates; record the new pair.
    5. Compare the recorded pairs with the expected error codes.
 * @tc.expect:
    1. The first DeleteEx call (null controller) returns (DATA_SHARE_ERROR, 0).
    2. The second DeleteEx call (mock controller with error) returns (E_REGISTERED_REPEATED, 0).
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
 * @tc.desc: Verify the behavior of the DelQueryTemplate function in DataShareHelperImpl when called with valid
 *           parameters (test URI and subscriber ID), ensuring it returns the expected error code.
 * @tc.type: FUNC
 * @tc.require: issueIC8OCN
 * @tc.precon:
    1. The DataShareHelperImpl instance is properly initialized via DataShareHelperImplTest::GetInstance().
    2. The test environment supports creating std::string (for URI) and int64_t (for subscriber ID) variables without
       errors.
    3. The DATA_SHARE_ERROR constant is predefined and accessible as the expected return value.
    4. The DelQueryTemplate function accepts std::string (URI) and int64_t (subscriberId) as input parameters.
 * @tc.step:
    1. Define a test URI as a std::string: "datashare:///com.datasharehelperimpl.test".
    2. Define an int64_t subscriber ID and initialize it to 0 (valid value for testing).
    3. Call the DelQueryTemplate function of the DataShareHelperImpl instance, passing the test URI and subscriber ID.
    4. Record the integer return code of the function and compare it with DATA_SHARE_ERROR.
 * @tc.expect:
    1. The DelQueryTemplate function returns DATA_SHARE_ERROR when called with the valid test parameters.
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
 * @tc.desc: Verify the behavior of the Publish function in DataShareHelperImpl when called with an empty Data object
 *           and a valid test bundle name, ensuring it returns an empty result vector.
 * @tc.type: FUNC
 * @tc.require: issueIC8OCN
 * @tc.precon:
    1. The DataShareHelperImpl instance is properly initialized via DataShareHelperImplTest::GetInstance().
    2. The test environment supports instantiation of empty Data objects, std::string (bundle name), and
       std::vector<OperationResult> (result vector).
    3. The Publish function accepts Data (data) and std::string (bundleName) as input parameters and returns
       a std::vector<OperationResult>.
 * @tc.step:
    1. Create an empty Data object (data) with no predefined content or attributes.
    2. Define a test bundle name as a std::string: "datashare:///com.datasharehelperimpl.test".
    3. Call the Publish function of the DataShareHelperImpl instance, passing 'data' and the test bundle name.
    4. Record the returned std::vector<OperationResult> and check its size.
 * @tc.expect:
    1. The Publish function returns a std::vector<OperationResult> with a size of 0 (empty vector).
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
 * @tc.desc: Verify the behavior of the GetPublishedData function in DataShareHelperImpl when called with a valid
 *           test bundle name, ensuring it returns a Data object with version 0 and an empty data vector.
 * @tc.type: FUNC
 * @tc.require: issueIC8OCN
 * @tc.precon:
    1. The DataShareHelperImpl instance is properly initialized via DataShareHelperImplTest::GetInstance().
    2. The test environment supports creating std::string (bundle name), int (result code), and Data objects; the
       Data class has 'version_' (int) and 'datas_' (std::vector) members.
    3. The GetPublishedData function accepts std::string (bundleName) and int& (resultCode) as parameters and returns
       a Data object.
 * @tc.step:
    1. Define an int variable (resultCode) and initialize it to 0 to store the function's result code.
    2. Define a test bundle name as a std::string: "datashare:///com.datasharehelperimpl.test".
    3. Call the GetPublishedData function of the DataShareHelperImpl instance, passing the test bundle name and
       a reference to resultCode.
    4. Record the returned Data object, then check the values of its 'version_' member and the size of its 'datas_'
       member.
 * @tc.expect:
    1. The 'version_' member of the returned Data object is 0.
    2. The 'datas_' member of the returned Data object is an empty std::vector (size = 0).
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
 * @tc.desc: Verify the behavior of the SubscribeRdbData function in DataShareHelperImpl when called with an empty
 *           URIs vector, valid TemplateId, and empty callback, ensuring it returns an empty result vector.
 * @tc.type: FUNC
 * @tc.require: issueIC8OCN
 * @tc.precon:
    1. The DataShareHelperImpl instance is properly initialized via DataShareHelperImplTest::GetInstance().
    2. The test environment supports creating empty std::vector<std::string> (URIs), TemplateId objects, empty
       std::function callbacks, and std::vector<OperationResult> (result vector).
    3. The SubscribeRdbData function accepts the above parameters and returns a std::vector<OperationResult>.
 * @tc.step:
    1. Create an empty std::vector<std::string> (uris) with no URI entries.
    2. Create a default-initialized TemplateId object (templateId) and an empty std::function callback
       (for RdbChangeNode handling).
    3. Call the SubscribeRdbData function of the DataShareHelperImpl instance, passing 'uris', templateId, and the
       callback.
    4. Record the returned std::vector<OperationResult> and check its size.
 * @tc.expect:
    1. The SubscribeRdbData function returns a std::vector<OperationResult> with a size of 0 (empty vector).
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
 * @tc.desc: Verify the behavior of the UnsubscribeRdbData function in DataShareHelperImpl when called with an empty
 *           URIs vector and valid TemplateId, ensuring it returns an empty result vector.
 * @tc.type: FUNC
 * @tc.require: issueIC8OCN
 * @tc.precon:
    1. The DataShareHelperImpl instance is properly initialized via DataShareHelperImplTest::GetInstance().
    2. The test environment supports creating empty std::vector<std::string> (URIs), TemplateId objects, and
       std::vector<OperationResult> (result vector).
    3. The UnsubscribeRdbData function accepts std::vector<std::string> (uris) and TemplateId (templateId) as
       parameters and returns a std::vector<OperationResult>.
 * @tc.step:
    1. Create an empty std::vector<std::string> (uris) with no URI entries.
    2. Create a default-initialized TemplateId object (templateId).
    3. Call the UnsubscribeRdbData function of the DataShareHelperImpl instance, passing 'uris' and templateId.
    4. Record the returned std::vector<OperationResult> and check its size.
 * @tc.expect:
    1. The UnsubscribeRdbData function returns a std::vector<OperationResult> with a size of 0 (empty vector).
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
 * @tc.desc: Verify the behavior of the DisableRdbSubs function in DataShareHelperImpl when called with an empty
 *           URIs vector and valid TemplateId, ensuring it returns an empty result vector.
 * @tc.type: FUNC
 * @tc.require: issueIC8OCN
 * @tc.precon:
    1. The DataShareHelperImpl instance is properly initialized via DataShareHelperImplTest::GetInstance().
    2. The test environment supports creating empty std::vector<std::string> (URIs), TemplateId objects, and
       std::vector<OperationResult> (result vector).
    3. The DisableRdbSubs function accepts std::vector<std::string> (uris) and TemplateId (templateId) as parameters
       and returns a std::vector<OperationResult>.
 * @tc.step:
    1. Create an empty std::vector<std::string> (uris) with no URI entries.
    2. Create a default-initialized TemplateId object (templateId).
    3. Call the DisableRdbSubs function of the DataShareHelperImpl instance, passing 'uris' and templateId.
    4. Record the returned std::vector<OperationResult> and check its size.
 * @tc.expect:
    1. The DisableRdbSubs function returns a std::vector<OperationResult> with a size of 0 (empty vector).
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
 * @tc.desc: Verify the behavior of the SubscribePublishedData function in DataShareHelperImpl when called with an
 *           empty URIs vector, valid subscriber ID, and empty callback, ensuring it returns an empty result vector.
 * @tc.type: FUNC
 * @tc.require: issueIC8OCN
 * @tc.precon:
    1. The DataShareHelperImpl instance is properly initialized via DataShareHelperImplTest::GetInstance().
    2. The test environment supports creating empty std::vector<std::string> (URIs), int64_t (subscriber ID), empty
       std::function callbacks, and std::vector<OperationResult> (result vector).
    3. The SubscribePublishedData function accepts the above parameters and returns a std::vector<OperationResult>.
 * @tc.step:
    1. Create an empty std::vector<std::string> (uris) with no URI entries.
    2. Define an int64_t subscriber ID and initialize it to 0; create an empty std::function callback (for
       PublishedDataChangeNode handling).
    3. Call the SubscribePublishedData function of the DataShareHelperImpl instance, passing 'uris', subscriber ID,
       and the callback.
    4. Record the returned std::vector<OperationResult> and check its size.
 * @tc.expect:
    1. The SubscribePublishedData function returns a std::vector<OperationResult> with a size of 0 (empty vector).
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
 * @tc.desc: Verify the behavior of the UnsubscribePublishedData function in DataShareHelperImpl when called with an
 *           empty URIs vector and valid subscriber ID, ensuring it returns an empty result vector.
 * @tc.type: FUNC
 * @tc.precon:
    1. The DataShareHelperImpl instance is properly initialized via DataShareHelperImplTest::GetInstance().
    2. The test environment supports creating empty std::vector<std::string> (URIs) and int64_t (subscriber ID).
    3. The UnsubscribePublishedData function accepts std::vector<std::string> (uris) and int64_t (subscriberId) as
       parameters and returns a std::vector<OperationResult>.
 * @tc.step:
    1. Create an empty std::vector<std::string> (uris) with no URI entries.
    2. Define an int64_t subscriber ID and initialize it to 0.
    3. Call the UnsubscribePublishedData function of the DataShareHelperImpl instance, passing 'uris' and subscriberID.
    4. Check the size of the returned std::vector<OperationResult>.
 * @tc.expect:
    1. The returned std::vector<OperationResult> has a size of 0 (empty vector).
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
 * @tc.desc: Verify the behavior of the EnablePubSubs function in DataShareHelperImpl when called with an empty
 *           URIs vector and valid subscriber ID, ensuring it returns an empty result vector.
 * @tc.type: FUNC
 * @tc.precon:
    1. The DataShareHelperImpl instance is properly initialized via DataShareHelperImplTest::GetInstance().
    2. The test environment supports creating empty std::vector<std::string> (URIs) and int64_t (subscriber ID).
    3. The EnablePubSubs function accepts std::vector<std::string> (uris) and int64_t (subscriberId) as parameters
       and returns a std::vector<OperationResult>.
 * @tc.step:
    1. Create an empty std::vector<std::string> (uris) with no URI entries.
    2. Define an int64_t subscriber ID and initialize it to 0.
    3. Call the EnablePubSubs function of the DataShareHelperImpl instance, passing 'uris' and subscriber ID.
    4. Check the size of the returned std::vector<OperationResult>.
 * @tc.expect:
    1. The returned std::vector<OperationResult> has a size of 0 (empty vector).
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
 * @tc.desc: Verify the behavior of the DisablePubSubs function in DataShareHelperImpl when called with an empty
 *           URIs vector and valid subscriber ID, ensuring it returns an empty result vector.
 * @tc.type: FUNC
 * @tc.precon:
    1. The DataShareHelperImpl instance is properly initialized via DataShareHelperImplTest::GetInstance().
    2. The test environment supports creating empty std::vector<std::string> (URIs) and int64_t (subscriber ID).
    3. The DisablePubSubs function accepts std::vector<std::string> (uris) and int64_t (subscriberId) as parameters
       and returns a std::vector<OperationResult>.
 * @tc.step:
    1. Create an empty std::vector<std::string> (uris) with no URI entries.
    2. Define an int64_t subscriber ID and initialize it to 0.
    3. Call the DisablePubSubs function of the DataShareHelperImpl instance, passing 'uris' and subscriber ID.
    4. Check the size of the returned std::vector<OperationResult>.
 * @tc.expect:
    1. The returned std::vector<OperationResult> has a size of 0 (empty vector).
 */
HWTEST_F(DataShareHelperImplTest, DisablePubSubsTest001, TestSize.Level0)
{
    LOG_INFO("DisableRdbSubsTest001::Start");
    std::vector<std::string> uris = {};
    int64_t subscriberId = 0;
    std::vector<OperationResult> result = DataShareHelperImplTest::GetInstance()->DisablePubSubs(uris, subscriberId);
    EXPECT_EQ(result.size(), 0);
    LOG_INFO("DisablePubSubsTest001::End");
}

/**
 * @tc.name: User_Define_func_No_ExtSpCtl_Test001
 * @tc.desc: Verify the behavior of the UserDefineFunc function in DataShareHelperImpl when the 'extSpCtl_' member
 *           (extension controller) is null, ensuring it returns the expected error code.
 * @tc.type: FUNC
 * @tc.precon:
    1. The DataShareHelperImpl instance is properly initialized via DataShareHelperImplTest::GetInstance().
    2. The test environment supports creating MessageParcel (data, reply) and MessageOption objects.
    3. The 'extSpCtl_' member of DataShareHelperImpl can be explicitly set to null.
    4. The DATA_SHARE_ERROR constant is predefined and accessible as the expected error return value.
    5. The UserDefineFunc function accepts MessageParcel, MessageParcel, and MessageOption as input parameters.
 * @tc.step:
    1. Set the 'extSpCtl_' member of the DataShareHelperImpl instance to null.
    2. Create empty MessageParcel objects (data, reply) and a default-initialized MessageOption (option).
    3. Call the UserDefineFunc function of the DataShareHelperImpl instance, passing 'data', 'reply', and 'option'.
    4. Check the integer return code of the function against DATA_SHARE_ERROR.
 * @tc.expect:
    1. The UserDefineFunc function returns DATA_SHARE_ERROR when 'extSpCtl_' is null.
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