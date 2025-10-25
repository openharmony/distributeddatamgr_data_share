/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#define LOG_TAG "controller_test"

#include <gtest/gtest.h>
#include <unistd.h>

#include <condition_variable>

#include "abs_shared_result_set.h"
#include "accesstoken_kit.h"
#include "data_ability_observer_interface.h"
#include "dataobs_mgr_changeinfo.h"
#include "datashare_connection.h"
#include "datashare_errno.h"
#include "datashare_helper.h"
#include "datashare_log.h"
#include "ext_special_controller.h"
#include "extension_manager_proxy.h"
#include "general_controller.h"
#include "general_controller_provider_impl.h"
#include "general_controller_service_impl.h"
#include "hap_token_info.h"
#include "iservice_registry.h"
#include "rdb_data_ability_utils.h"
#include "system_ability_definition.h"
#include "token_setproc.h"

namespace OHOS {
namespace DataShare {
using namespace testing::ext;
using namespace OHOS::Security::AccessToken;

class ControllerTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void ControllerTest::SetUpTestCase(void) {}
void ControllerTest::TearDownTestCase(void) {}
void ControllerTest::SetUp(void) {}
void ControllerTest::TearDown(void) {}

/**
 * @tc.name: ControllerTest_ProviderImplInsertTest_001
 * @tc.desc: Verify the Insert operation in GeneralControllerProviderImpl when the connection is null, focusing on
 *           the negative return value to confirm operation failure.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports instantiation of GeneralControllerProviderImpl, DataShareValuesBucket, and Uri
       without initialization errors.
    2. "phoneNumber" is a valid column name for data storage in the test context.
 * @tc.step:
    1. Create a GeneralControllerProviderImpl instance with a null connection, an empty Uri, and a null third
       parameter.
    2. Create a DataShareValuesBucket object, then call Put to add a key-value pair: "phoneNumber" = 20.07.
    3. Call the Insert method of the GeneralControllerProviderImpl instance with the empty Uri and the values bucket.
    4. Check whether the return value of the Insert method is negative.
 * @tc.expect:
    1. The return value of the Insert operation is negative, indicating operation failure.
 */
HWTEST_F(ControllerTest, ControllerTest_ProviderImplInsertTest_001, TestSize.Level0)
{
    LOG_INFO("ControllerTest_ProviderImplInsertTest_001::Start");
    Uri uri("");
    std::shared_ptr<DataShare::GeneralController> tempGenConProImp =
        std::make_shared<DataShare::GeneralControllerProviderImpl>(nullptr, uri, nullptr);
    DataShare::DataShareValuesBucket valuesBucket;
    double valueD1 = 20.07;
    valuesBucket.Put("phoneNumber", valueD1);
    int result = tempGenConProImp->Insert(uri, valuesBucket);
    EXPECT_EQ((result < 0), true);
    LOG_INFO("ControllerTest_ProviderImplInsertTest_001::End");
}

/**
 * @tc.name: ControllerTest_ProviderImplInsertTest_002
 * @tc.desc: Verify the Insert operation in GeneralControllerProviderImpl with a valid but unconnected
 *           DataShareConnection, confirming failure via negative return value.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports sptr/shared_ptr for DataShareConnection, and instantiation of
       GeneralControllerProviderImpl, DataShareValuesBucket, and Uri.
    2. The DataShareConnection’s DisconnectDataShareExtAbility method is available for the shared_ptr deleter.
    3. "phoneNumber" is a valid column name.
 * @tc.step:
    1. Create a DataShareConnection instance (via sptr and new std::nothrow) with an empty Uri and a null token.
    2. Wrap the DataShareConnection into a shared_ptr with a custom deleter that calls DisconnectDataShareExtAbility.
    3. Create a GeneralControllerProviderImpl instance with the shared_ptr connection, empty Uri, and null third
       parameter.
    4. Prepare a DataShareValuesBucket with "phoneNumber" = 20.07, then call Insert with the empty Uri and bucket.
    5. Check if the Insert method’s return value is negative.
 * @tc.expect:
    1. The return value of the Insert operation is negative, indicating operation failure.
 */
HWTEST_F(ControllerTest, ControllerTest_ProviderImplInsertTest_002, TestSize.Level0)
{
    LOG_INFO("ControllerTest_ProviderImplInsertTest_002::Start");
    Uri uri("");
    sptr<DataShare::DataShareConnection> connection =
        new (std::nothrow) DataShare::DataShareConnection(uri, nullptr);
    auto dataShareConnection =
        std::shared_ptr<DataShare::DataShareConnection>(connection.GetRefPtr(), [holder = connection](const auto *) {
            holder->DisconnectDataShareExtAbility();
        });
    std::shared_ptr<DataShare::GeneralController> tempGenConProImp =
        std::make_shared<DataShare::GeneralControllerProviderImpl>(dataShareConnection, uri, nullptr);
    DataShare::DataShareValuesBucket valuesBucket;
    double valueD1 = 20.07;
    valuesBucket.Put("phoneNumber", valueD1);
    int result = tempGenConProImp->Insert(uri, valuesBucket);
    EXPECT_EQ((result < 0), true);
    LOG_INFO("ControllerTest_ProviderImplInsertTest_002::End");
}

/**
 * @tc.name: ControllerTest_ProviderImplUpdateTest_001
 * @tc.desc: Verify the Update operation in GeneralControllerProviderImpl when the connection is null, focusing on
 *           the negative return value to confirm operation failure.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports instantiation of GeneralControllerProviderImpl, DataSharePredicates,
       DataShareValuesBucket, and Uri.
    2. "name" is a valid column name for setting predicates and update data.
 * @tc.step:
    1. Create a GeneralControllerProviderImpl instance with a null connection, an empty Uri, and a null third parameter.
    2. Create a DataSharePredicates instance and call EqualTo to set the condition: "name" = "Controller_Test001".
    3. Create a DataShareValuesBucket and call Put to add the update data: "name" = "Controller_Test002".
    4. Call the Update method with the empty Uri, predicates, and values bucket; check if the return value is negative.
 * @tc.expect:
    1. The return value of the Update operation is negative, indicating operation failure.
 */
HWTEST_F(ControllerTest, ControllerTest_ProviderImplUpdateTest_001, TestSize.Level0)
{
    LOG_INFO("ControllerTest_ProviderImplUpdateTest_001::Start");
    Uri uri("");
    std::shared_ptr<DataShare::GeneralController> tempGenConProImp =
        std::make_shared<DataShare::GeneralControllerProviderImpl>(nullptr, uri, nullptr);
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo("name", "Controller_Test001");
    DataShare::DataShareValuesBucket valuesBucket;
    valuesBucket.Put("name", "Controller_Test002");
    int result = tempGenConProImp->Update(uri, predicates, valuesBucket);
    EXPECT_EQ((result < 0), true);
    LOG_INFO("ControllerTest_ProviderImplUpdateTest_001::End");
}

/**
 * @tc.name: ControllerTest_ProviderImplUpdateTest_002
 * @tc.desc: Verify the Update operation in GeneralControllerProviderImpl with a valid but unconnected
 *           DataShareConnection, confirming failure via negative return value.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports sptr/shared_ptr for DataShareConnection, and instantiation of
       GeneralControllerProviderImpl, DataSharePredicates, DataShareValuesBucket, and Uri.
    2. The DataShareConnection’s DisconnectDataShareExtAbility method works for the shared_ptr deleter.
    3. "name" is a valid column name for predicates and update data.
 * @tc.step:
    1. Create a DataShareConnection instance (via sptr and new std::nothrow) with an empty Uri and a null token.
    2. Wrap the connection into a shared_ptr with a deleter that calls DisconnectDataShareExtAbility.
    3. Create a GeneralControllerProviderImpl instance with the shared_ptr connection, empty Uri, and null
       third parameter.
    4. Prepare predicates ("name" = "Controller_Test001") and a values bucket ("name" = "Controller_Test002").
    5. Call Update with the empty Uri, predicates, and bucket; check if the return value is negative.
 * @tc.expect:
    1. The return value of the Update operation is negative, indicating operation failure.
 */
HWTEST_F(ControllerTest, ControllerTest_ProviderImplUpdateTest_002, TestSize.Level0)
{
    LOG_INFO("ControllerTest_ProviderImplUpdateTest_002::Start");
    Uri uri("");
    sptr<DataShare::DataShareConnection> connection =
        new (std::nothrow) DataShare::DataShareConnection(uri, nullptr);
    auto dataShareConnection =
        std::shared_ptr<DataShare::DataShareConnection>(connection.GetRefPtr(), [holder = connection](const auto *) {
            holder->DisconnectDataShareExtAbility();
        });
    std::shared_ptr<DataShare::GeneralController> tempGenConProImp =
        std::make_shared<DataShare::GeneralControllerProviderImpl>(dataShareConnection, uri, nullptr);
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo("name", "Controller_Test001");
    DataShare::DataShareValuesBucket valuesBucket;
    valuesBucket.Put("name", "Controller_Test002");
    int result = tempGenConProImp->Update(uri, predicates, valuesBucket);
    EXPECT_EQ((result < 0), true);
    LOG_INFO("ControllerTest_ProviderImplUpdateTest_002::End");
}

/**
 * @tc.name: ControllerTest_ProviderImplDeleteTest_001
 * @tc.desc: Verify the Delete operation in GeneralControllerProviderImpl when the connection is null, focusing on
 *           the negative return value to confirm operation failure.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports instantiation of GeneralControllerProviderImpl, DataSharePredicates, and Uri.
    2. "name" is a valid column name for setting the delete condition in DataSharePredicates.
 * @tc.step:
    1. Create a GeneralControllerProviderImpl instance with a null connection, an empty Uri, and a null third
       parameter.
    2. Create a DataSharePredicates instance and call EqualTo to set the condition: "name" = "Controller_Test001".
    3. Call the Delete method of the GeneralControllerProviderImpl instance with the empty Uri and predicates.
    4. Check whether the return value of the Delete method is negative.
 * @tc.expect:
    1. The return value of the Delete operation is negative, indicating operation failure.
 */
HWTEST_F(ControllerTest, ControllerTest_ProviderImplDeleteTest_001, TestSize.Level0)
{
    LOG_INFO("ControllerTest_ProviderImplDeleteTest_001::Start");
    Uri uri("");
    std::shared_ptr<DataShare::GeneralController> tempGenConProImp =
        std::make_shared<DataShare::GeneralControllerProviderImpl>(nullptr, uri, nullptr);
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo("name", "Controller_Test001");
    int result = tempGenConProImp->Delete(uri, predicates);
    EXPECT_EQ((result < 0), true);
    LOG_INFO("ControllerTest_ProviderImplDeleteTest_001::End");
}

/**
 * @tc.name: ControllerTest_ProviderImplDeleteTest_002
 * @tc.desc: Verify the Delete operation in GeneralControllerProviderImpl with a valid but unconnected
 *           DataShareConnection, confirming failure via negative return value.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports sptr/shared_ptr for DataShareConnection, and instantiation of
       GeneralControllerProviderImpl, DataSharePredicates, and Uri.
    2. The DataShareConnection’s DisconnectDataShareExtAbility method is available for the shared_ptr deleter.
    3. "name" is a valid column name for the delete condition.
 * @tc.step:
    1. Create a DataShareConnection instance (via sptr and new std::nothrow) with an empty Uri and a null token.
    2. Wrap the connection into a shared_ptr with a custom deleter that calls DisconnectDataShareExtAbility.
    3. Create a GeneralControllerProviderImpl instance with the shared_ptr connection, empty Uri, and null third
       parameter.
    4. Create DataSharePredicates with the condition: "name" = "Controller_Test001".
    5. Call Delete with the empty Uri and predicates; check if the return value is negative.
 * @tc.expect:
    1. The return value of the Delete operation is negative, indicating operation failure.
 */
HWTEST_F(ControllerTest, ControllerTest_ProviderImplDeleteTest_002, TestSize.Level0)
{
    LOG_INFO("ControllerTest_ProviderImplDeleteTest_002::Start");
    Uri uri("");
    sptr<DataShare::DataShareConnection> connection =
        new (std::nothrow) DataShare::DataShareConnection(uri, nullptr);
    auto dataShareConnection =
        std::shared_ptr<DataShare::DataShareConnection>(connection.GetRefPtr(), [holder = connection](const auto *) {
            holder->DisconnectDataShareExtAbility();
        });
    std::shared_ptr<DataShare::GeneralController> tempGenConProImp =
        std::make_shared<DataShare::GeneralControllerProviderImpl>(dataShareConnection, uri, nullptr);
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo("name", "Controller_Test001");
    int result = tempGenConProImp->Delete(uri, predicates);
    EXPECT_EQ((result < 0), true);
    LOG_INFO("ControllerTest_ProviderImplDeleteTest_002::End");
}

/**
 * @tc.name: Generalcontroller_ServiceImplInsertExTest_001
 * @tc.desc: Verify the InsertEx operation in GeneralControllerProviderImpl with a null connection, confirming
 *           failure via the return pair (DATA_SHARE_ERROR, 0).
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports instantiation of GeneralControllerProviderImpl, DataShareValuesBucket, and Uri.
    2. DATA_SHARE_ERROR is a predefined error code; the InsertEx method returns a std::pair<int, int> for result.
    3. "phoneNumber" is a valid column name.
 * @tc.step:
    1. Create a GeneralControllerProviderImpl instance with a null connection, an empty Uri, and a null third
       parameter.
    2. Create a DataShareValuesBucket and call Put to add: "phoneNumber" = 20.07 (double type).
    3. Call the InsertEx method with the empty Uri and values bucket; store the returned std::pair.
    4. Check if the first element of the pair is DATA_SHARE_ERROR and the second is 0.
 * @tc.expect:
    1. The InsertEx operation returns the pair (DATA_SHARE_ERROR, 0), indicating operation failure.
 */
HWTEST_F(ControllerTest, Generalcontroller_ServiceImplInsertExTest_001, TestSize.Level0)
{
    LOG_INFO("Generalcontroller_ServiceImplInsertExTest_001::Start");
    Uri uri("");
    std::shared_ptr<DataShare::GeneralController> tempGenConProImp =
        std::make_shared<DataShare::GeneralControllerProviderImpl>(nullptr, uri, nullptr);
    DataShare::DataShareValuesBucket valuesBucket;
    double valueD1 = 20.07;
    valuesBucket.Put("phoneNumber", valueD1);
    std::pair result = tempGenConProImp->InsertEx(uri, valuesBucket);
    EXPECT_EQ(result.first, DATA_SHARE_ERROR);
    EXPECT_EQ(result.second, 0);
    LOG_INFO("Generalcontroller_ServiceImplInsertExTest_001::End");
}

/**
 * @tc.name: Generalcontroller_ServiceImplInsertExTest_002
 * @tc.desc: Verify the InsertEx operation in GeneralControllerProviderImpl with a valid but unconnected
 *           DataShareConnection, confirming failure via (DATA_SHARE_ERROR, 0).
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports sptr/shared_ptr for DataShareConnection, and instantiation of
       GeneralControllerProviderImpl, DataShareValuesBucket, and Uri.
    2. DATA_SHARE_ERROR is predefined; InsertEx returns a std::pair<int, int>; DataShareConnection’s disconnect
       method works.
    3. "phoneNumber" is a valid column name.
 * @tc.step:
    1. Create a DataShareConnection instance (via sptr and new std::nothrow) with an empty Uri and a null token.
    2. Wrap the connection into a shared_ptr with a deleter that calls DisconnectDataShareExtAbility.
    3. Create a GeneralControllerProviderImpl instance with the shared_ptr connection, empty Uri, and null third
       parameter.
    4. Prepare a values bucket with "phoneNumber" = 20.07, then call InsertEx with the empty Uri and bucket.
    5. Check if the returned pair is (DATA_SHARE_ERROR, 0).
 * @tc.expect:
    1. The InsertEx operation returns the pair (DATA_SHARE_ERROR, 0), indicating operation failure.
 */
HWTEST_F(ControllerTest, Generalcontroller_ServiceImplInsertExTest_002, TestSize.Level0)
{
    LOG_INFO("Generalcontroller_ServiceImplInsertExTest_002::Start");
    Uri uri("");
    sptr<DataShare::DataShareConnection> connection =
        new (std::nothrow) DataShare::DataShareConnection(uri, nullptr);
    auto dataShareConnection =
        std::shared_ptr<DataShare::DataShareConnection>(connection.GetRefPtr(), [holder = connection](const auto *) {
            holder->DisconnectDataShareExtAbility();
        });
    std::shared_ptr<DataShare::GeneralController> tempGenConProImp =
        std::make_shared<DataShare::GeneralControllerProviderImpl>(dataShareConnection, uri, nullptr);
    DataShare::DataShareValuesBucket valuesBucket;
    double valueD1 = 20.07;
    valuesBucket.Put("phoneNumber", valueD1);
    std::pair result = tempGenConProImp->InsertEx(uri, valuesBucket);
    EXPECT_EQ(result.first, DATA_SHARE_ERROR);
    EXPECT_EQ(result.second, 0);
    LOG_INFO("Generalcontroller_ServiceImplInsertExTest_002::End");
}

/**
 * @tc.name: Generalcontroller_ServiceImplUpdateExTest_001
 * @tc.desc: Verify the UpdateEx operation in GeneralControllerProviderImpl with a null connection, confirming
 *           failure via the return pair (DATA_SHARE_ERROR, 0).
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports instantiation of GeneralControllerProviderImpl, DataSharePredicates,
       DataShareValuesBucket, and Uri.
    2. DATA_SHARE_ERROR is predefined; UpdateEx returns a std::pair<int, int>; "name" and "phoneNumber" are valid
       columns.
 * @tc.step:
    1. Create a GeneralControllerProviderImpl instance with a null connection, an empty Uri, and a null third
       parameter.
    2. Create predicates: EqualTo("name", "Controller_Test001"); create a values bucket: Put("phoneNumber", 20.07).
    3. Call UpdateEx with the empty Uri, predicates, and bucket; store the returned std::pair.
    4. Check if the pair’s first element is DATA_SHARE_ERROR and the second is 0.
 * @tc.expect:
    1. The UpdateEx operation returns the pair (DATA_SHARE_ERROR, 0), indicating operation failure.
 */
HWTEST_F(ControllerTest, Generalcontroller_ServiceImplUpdateExTest_001, TestSize.Level0)
{
    LOG_INFO("Generalcontroller_ServiceImplUpdateExTest_001::Start");
    Uri uri("");
    std::shared_ptr<DataShare::GeneralController> tempGenConProImp =
        std::make_shared<DataShare::GeneralControllerProviderImpl>(nullptr, uri, nullptr);
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo("name", "Controller_Test001");
    DataShare::DataShareValuesBucket valuesBucket;
    double valueD1 = 20.07;
    valuesBucket.Put("phoneNumber", valueD1);
    std::pair result = tempGenConProImp->UpdateEx(uri, predicates, valuesBucket);
    EXPECT_EQ(result.first, DATA_SHARE_ERROR);
    EXPECT_EQ(result.second, 0);
    LOG_INFO("Generalcontroller_ServiceImplUpdateExTest_001::End");
}

/**
 * @tc.name: Generalcontroller_ServiceImplUpdateExTest_002
 * @tc.desc: Verify the UpdateEx operation in GeneralControllerProviderImpl with a valid but unconnected
 *           DataShareConnection, confirming failure via (DATA_SHARE_ERROR, 0).
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports sptr/shared_ptr for DataShareConnection, and instantiation of
       GeneralControllerProviderImpl, DataSharePredicates, DataShareValuesBucket, and Uri.
    2. DATA_SHARE_ERROR is predefined; UpdateEx returns a std::pair<int, int>; DataShareConnection’s disconnect
       method works.
    3. "name" and "phoneNumber" are valid columns.
 * @tc.step:
    1. Create a DataShareConnection instance (via sptr and new std::nothrow) with an empty Uri and a null token.
    2. Wrap the connection into a shared_ptr with a deleter that calls DisconnectDataShareExtAbility.
    3. Create a GeneralControllerProviderImpl instance with the shared_ptr connection, empty Uri, and null third
       parameter.
    4. Prepare predicates ("name" = "Controller_Test001") and a bucket ("phoneNumber" = 20.07); call UpdateEx.
    5. Check if the returned pair is (DATA_SHARE_ERROR, 0).
 * @tc.expect:
    1. The UpdateEx operation returns the pair (DATA_SHARE_ERROR, 0), indicating operation failure.
 */
HWTEST_F(ControllerTest, Generalcontroller_ServiceImplUpdateExTest_002, TestSize.Level0)
{
    LOG_INFO("Generalcontroller_ServiceImplUpdateExTest_002::Start");
    Uri uri("");
    sptr<DataShare::DataShareConnection> connection =
        new (std::nothrow) DataShare::DataShareConnection(uri, nullptr);
    auto dataShareConnection =
        std::shared_ptr<DataShare::DataShareConnection>(connection.GetRefPtr(), [holder = connection](const auto *) {
            holder->DisconnectDataShareExtAbility();
        });
    std::shared_ptr<DataShare::GeneralController> tempGenConProImp =
        std::make_shared<DataShare::GeneralControllerProviderImpl>(dataShareConnection, uri, nullptr);
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo("name", "Controller_Test001");
    DataShare::DataShareValuesBucket valuesBucket;
    double valueD1 = 20.07;
    valuesBucket.Put("phoneNumber", valueD1);
    std::pair result = tempGenConProImp->UpdateEx(uri, predicates, valuesBucket);
    EXPECT_EQ(result.first, DATA_SHARE_ERROR);
    EXPECT_EQ(result.second, 0);
    LOG_INFO("Generalcontroller_ServiceImplUpdateExTest_002::End");
}

/**
 * @tc.name: Generalcontroller_ServiceImplDeleteExTest_001
 * @tc.desc: Verify the DeleteEx operation in GeneralControllerProviderImpl with a null connection, confirming
 *           failure via the return pair (DATA_SHARE_ERROR, 0).
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports instantiation of GeneralControllerProviderImpl, DataSharePredicates, and Uri.
    2. DATA_SHARE_ERROR is a predefined error code; the DeleteEx method returns a std::pair<int, int>.
    3. "name" is a valid column name for the delete condition.
 * @tc.step:
    1. Create a GeneralControllerProviderImpl instance with a null connection, an empty Uri, and a null third
       parameter.
    2. Create a DataSharePredicates instance and set the condition: EqualTo("name", "Controller_Test001").
    3. Call the DeleteEx method with the empty Uri and predicates; store the returned std::pair.
    4. Check if the first element of the pair is DATA_SHARE_ERROR and the second is 0.
 * @tc.expect:
    1. The DeleteEx operation returns the pair (DATA_SHARE_ERROR, 0), indicating operation failure.
 */
HWTEST_F(ControllerTest, Generalcontroller_ServiceImplDeleteExTest_001, TestSize.Level0)
{
    LOG_INFO("Generalcontroller_ServiceImplDeleteExTest_001::Start");
    Uri uri("");
    std::shared_ptr<DataShare::GeneralController> tempGenConProImp =
        std::make_shared<DataShare::GeneralControllerProviderImpl>(nullptr, uri, nullptr);
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo("name", "Controller_Test001");
    std::pair result = tempGenConProImp->DeleteEx(uri, predicates);
    EXPECT_EQ(result.first, DATA_SHARE_ERROR);
    EXPECT_EQ(result.second, 0);
    LOG_INFO("Generalcontroller_ServiceImplDeleteExTest_001::End");
}

/**
 * @tc.name: Generalcontroller_ServiceImplDeleteExTest_002
 * @tc.desc: Verify the DeleteEx operation in GeneralControllerProviderImpl with a valid but unconnected
 *           DataShareConnection, confirming failure via (DATA_SHARE_ERROR, 0).
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports sptr/shared_ptr for DataShareConnection, and instantiation of
       GeneralControllerProviderImpl, DataSharePredicates, and Uri.
    2. DATA_SHARE_ERROR is predefined; DeleteEx returns a std::pair<int, int>; DataShareConnection’s disconnect method
       works.
    3. "name" is a valid column name for the delete condition.
 * @tc.step:
    1. Create a DataShareConnection instance (via sptr and new std::nothrow) with an empty Uri and a null token.
    2. Wrap the connection into a shared_ptr with a deleter that calls DisconnectDataShareExtAbility.
    3. Create a GeneralControllerProviderImpl instance with the shared_ptr connection, empty Uri, and null third
       parameter.
    4. Create DataSharePredicates with "name" = "Controller_Test001", then call DeleteEx with the empty Uri and
       predicates.
    5. Check if the returned pair is (DATA_SHARE_ERROR, 0).
 * @tc.expect:
    1. The DeleteEx operation returns the pair (DATA_SHARE_ERROR, 0), indicating operation failure.
 */
HWTEST_F(ControllerTest, Generalcontroller_ServiceImplDeleteExTest_002, TestSize.Level0)
{
    LOG_INFO("Generalcontroller_ServiceImplDeleteExTest_002::Start");
    Uri uri("");
    sptr<DataShare::DataShareConnection> connection =
        new (std::nothrow) DataShare::DataShareConnection(uri, nullptr);
    auto dataShareConnection =
        std::shared_ptr<DataShare::DataShareConnection>(connection.GetRefPtr(), [holder = connection](const auto *) {
            holder->DisconnectDataShareExtAbility();
        });
    std::shared_ptr<DataShare::GeneralController> tempGenConProImp =
        std::make_shared<DataShare::GeneralControllerProviderImpl>(dataShareConnection, uri, nullptr);
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo("name", "Controller_Test001");
    std::pair result = tempGenConProImp->DeleteEx(uri, predicates);
    EXPECT_EQ(result.first, DATA_SHARE_ERROR);
    EXPECT_EQ(result.second, 0);
    LOG_INFO("Generalcontroller_ServiceImplDeleteExTest_002::End");
}

/**
 * @tc.name: ControllerTest_ProviderImplQueryTest_001
 * @tc.desc: Verify the Query operation in GeneralControllerProviderImpl when the connection is null, confirming
 *           failure via a nullptr return value.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports instantiation of GeneralControllerProviderImpl, DataSharePredicates,
       DatashareBusinessError, DataShareOption, Uri, and vector<string>.
    2. The Query method returns a pointer to the result set (nullptr on failure).
    3. "name" is a valid column name for setting the query condition.
 * @tc.step:
    1. Create a GeneralControllerProviderImpl instance with a null connection, an empty Uri, and a null third
       parameter.
    2. Create a DataSharePredicates instance (EqualTo("name", "Controller_Test001")), an empty vector<string> for
       columns, a DatashareBusinessError object, and a DataShareOption object.
    3. Call the Query method with the empty Uri, predicates, columns, DatashareBusinessError, and DataShareOption.
    4. Check whether the returned result set pointer is nullptr.
 * @tc.expect:
    1. The Query operation returns a nullptr, indicating operation failure.
 */
HWTEST_F(ControllerTest, ControllerTest_ProviderImplQueryTest_001, TestSize.Level0)
{
    LOG_INFO("ControllerTest_ProviderImplQueryTest_001::Start");
    Uri uri("");
    std::shared_ptr<DataShare::GeneralController> tempGenConProImp =
        std::make_shared<DataShare::GeneralControllerProviderImpl>(nullptr, uri, nullptr);
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo("name", "Controller_Test001");
    vector<string> columns;
    DatashareBusinessError error;
    DataShareOption option;
    auto result = tempGenConProImp->Query(uri, predicates, columns, error, option);
    EXPECT_EQ(result, nullptr);
    LOG_INFO("ControllerTest_ProviderImplQueryTest_001::End");
}

/**
 * @tc.name: ControllerTest_ProviderImplQueryTest_002
 * @tc.desc: Verify the Query operation in GeneralControllerProviderImpl with a valid but unconnected
 *           DataShareConnection, confirming failure via a nullptr return value.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports sptr/shared_ptr for DataShareConnection, and instantiation of
       GeneralControllerProviderImpl, DataSharePredicates, DatashareBusinessError, DataShareOption, Uri, and
       vector<string>.
    2. The DataShareConnection’s DisconnectDataShareExtAbility method works for the shared_ptr deleter.
    3. "name" is a valid column name for setting the query condition.
 * @tc.step:
    1. Create a DataShareConnection instance (via sptr and new std::nothrow) with an empty Uri and a null token.
    2. Wrap the connection into a shared_ptr with a deleter that calls DisconnectDataShareExtAbility.
    3. Create a GeneralControllerProviderImpl instance with the shared_ptr connection, empty Uri, and null third
       parameter.
    4. Prepare predicates ("name" = "Controller_Test001"), empty columns, DatashareBusinessError, and DataShareOption.
    5. Call Query with the empty Uri, predicates, columns, error, and option; check if the result is nullptr.
 * @tc.expect:
    1. The Query operation returns a nullptr, indicating operation failure.
 */
HWTEST_F(ControllerTest, ControllerTest_ProviderImplQueryTest_002, TestSize.Level0)
{
    LOG_INFO("ControllerTest_ProviderImplQueryTest_002::Start");
    Uri uri("");
    sptr<DataShare::DataShareConnection> connection =
        new (std::nothrow) DataShare::DataShareConnection(uri, nullptr);
    auto dataShareConnection =
        std::shared_ptr<DataShare::DataShareConnection>(connection.GetRefPtr(), [holder = connection](const auto *) {
            holder->DisconnectDataShareExtAbility();
        });
    std::shared_ptr<DataShare::GeneralController> tempGenConProImp =
        std::make_shared<DataShare::GeneralControllerProviderImpl>(dataShareConnection, uri, nullptr);
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo("name", "Controller_Test001");
    vector<string> columns;
    DatashareBusinessError error;
    DataShareOption option;
    auto result = tempGenConProImp->Query(uri, predicates, columns, error, option);
    EXPECT_EQ(result, nullptr);
    LOG_INFO("ControllerTest_ProviderImplQueryTest_002::End");
}

/**
 * @tc.name: Generalcontroller_ServiceImplRegisterObserverTest_001
 * @tc.desc: Verify the RegisterObserver operation in GeneralControllerProviderImpl with a null connection,
 *           ensuring the input empty Uri remains unchanged.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports instantiation of GeneralControllerProviderImpl, sptr<AAFwk::IDataAbilityObserver>,
       and Uri.
    2. The Uri class supports equality comparison (==) to verify the empty Uri remains unchanged.
 * @tc.step:
    1. Create an empty Uri object and a GeneralControllerProviderImpl instance with a null connection, the empty Uri,
       and a null third parameter.
    2. Create a null sptr<AAFwk::IDataAbilityObserver> (dataObserver).
    3. Call the RegisterObserver method of the GeneralControllerProviderImpl instance with the empty Uri and
       dataObserver.
    4. Compare the original empty Uri with a new empty Uri to verify it remains unchanged.
 * @tc.expect:
    1. The input empty Uri remains unchanged after the RegisterObserver operation (uri == Uri("")).
 */
HWTEST_F(ControllerTest, Generalcontroller_ServiceImplRegisterObserverTest_001, TestSize.Level0)
{
    LOG_INFO("Generalcontroller_ServiceImplRegisterObserverTest_001::Start");
    Uri uri("");
    std::shared_ptr<DataShare::GeneralController> tempGenConProImp =
        std::make_shared<DataShare::GeneralControllerProviderImpl>(nullptr, uri, nullptr);
    sptr<AAFwk::IDataAbilityObserver> dataObserver;
    tempGenConProImp->RegisterObserver(uri, dataObserver);
    EXPECT_EQ(uri, Uri(""));
    LOG_INFO("Generalcontroller_ServiceImplRegisterObserverTest_001::End");
}

/**
 * @tc.name: Generalcontroller_ServiceImplRegisterObserverTest_002
 * @tc.desc: Verify the RegisterObserver operation in GeneralControllerProviderImpl with a valid but unconnected
 *           DataShareConnection, ensuring the input empty Uri remains unchanged.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports sptr/shared_ptr for DataShareConnection, and instantiation of
       GeneralControllerProviderImpl, sptr<AAFwk::IDataAbilityObserver>, and Uri.
    2. The DataShareConnection’s DisconnectDataShareExtAbility method works for the shared_ptr deleter.
    3. Uri equality comparison (==) is supported to check the empty Uri.
 * @tc.step:
    1. Create a DataShareConnection (sptr, new std::nothrow) with an empty Uri and null token; wrap it into a
       shared_ptr.
    2. Create a GeneralControllerProviderImpl instance with the shared_ptr connection, empty Uri, and null third
       parameter.
    3. Create a null sptr<AAFwk::IDataAbilityObserver> (dataObserver).
    4. Call RegisterObserver with the empty Uri and dataObserver; verify the Uri remains empty.
 * @tc.expect:
    1. The input empty Uri remains unchanged after the RegisterObserver operation (uri == Uri("")).
 */
HWTEST_F(ControllerTest, Generalcontroller_ServiceImplRegisterObserverTest_002, TestSize.Level0)
{
    LOG_INFO("Generalcontroller_ServiceImplRegisterObserverTest_002::Start");
    Uri uri("");
    sptr<DataShare::DataShareConnection> connection =
        new (std::nothrow) DataShare::DataShareConnection(uri, nullptr);
    auto dataShareConnection =
        std::shared_ptr<DataShare::DataShareConnection>(connection.GetRefPtr(), [holder = connection](const auto *) {
            holder->DisconnectDataShareExtAbility();
        });
    std::shared_ptr<DataShare::GeneralController> tempGenConProImp =
        std::make_shared<DataShare::GeneralControllerProviderImpl>(dataShareConnection, uri, nullptr);
    sptr<AAFwk::IDataAbilityObserver> dataObserver;
    tempGenConProImp->RegisterObserver(uri, dataObserver);
    EXPECT_EQ(uri, Uri(""));
    LOG_INFO("Generalcontroller_ServiceImplRegisterObserverTest_002::End");
}

/**
 * @tc.name: Generalcontroller_ServiceImplUnregisterObserverTest_001
 * @tc.desc: Verify the UnregisterObserver operation in GeneralControllerProviderImpl with a null connection,
 *           ensuring the input empty Uri remains unchanged.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports instantiation of GeneralControllerProviderImpl, sptr<AAFwk::IDataAbilityObserver>,
       and Uri.
    2. Uri equality comparison (==) works to confirm the empty Uri is unchanged.
 * @tc.step:
    1. Create an empty Uri and a GeneralControllerProviderImpl instance with a null connection, the empty Uri,
       and a null third parameter.
    2. Create a null sptr<AAFwk::IDataAbilityObserver> (dataObserver).
    3. Call the UnregisterObserver method with the empty Uri and dataObserver.
    4. Compare the original Uri with a new empty Uri to verify no change.
 * @tc.expect:
    1. The input empty Uri remains unchanged after the UnregisterObserver operation (uri == Uri("")).
 */
HWTEST_F(ControllerTest, Generalcontroller_ServiceImplUnregisterObserverTest_001, TestSize.Level0)
{
    LOG_INFO("Generalcontroller_ServiceImplUnregisterObserverTest_001::Start");
    Uri uri("");
    std::shared_ptr<DataShare::GeneralController> tempGenConProImp =
        std::make_shared<DataShare::GeneralControllerProviderImpl>(nullptr, uri, nullptr);
    sptr<AAFwk::IDataAbilityObserver> dataObserver;
    tempGenConProImp->UnregisterObserver(uri, dataObserver);
    EXPECT_EQ(uri, Uri(""));
    LOG_INFO("Generalcontroller_ServiceImplUnregisterObserverTest_001::End");
}

/**
 * @tc.name: Generalcontroller_ServiceImplUnregisterObserverTest_002
 * @tc.desc: Verify the UnregisterObserver operation in GeneralControllerProviderImpl with a valid but unconnected
 *           DataShareConnection, ensuring the input empty Uri remains unchanged.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports sptr/shared_ptr for DataShareConnection, and instantiation of
       GeneralControllerProviderImpl, sptr<AAFwk::IDataAbilityObserver>, and Uri.
    2. DataShareConnection’s DisconnectDataShareExtAbility method is available for the shared_ptr deleter.
    3. Uri equality comparison (==) is supported.
 * @tc.step:
    1. Create a DataShareConnection (sptr, new std::nothrow) with an empty Uri and null token; wrap it into a
       shared_ptr.
    2. Create a GeneralControllerProviderImpl instance with the shared_ptr connection, empty Uri, and null third
       parameter.
    3. Create a null sptr<AAFwk::IDataAbilityObserver> (dataObserver).
    4. Call UnregisterObserver with the empty Uri and dataObserver; check if the Uri remains empty.
 * @tc.expect:
    1. The input empty Uri remains unchanged after the UnregisterObserver operation (uri == Uri("")).
 */
HWTEST_F(ControllerTest, Generalcontroller_ServiceImplUnregisterObserverTest_002, TestSize.Level0)
{
    LOG_INFO("Generalcontroller_ServiceImplUnregisterObserverTest_002::Start");
    Uri uri("");
    sptr<DataShare::DataShareConnection> connection =
        new (std::nothrow) DataShare::DataShareConnection(uri, nullptr);
    auto dataShareConnection =
        std::shared_ptr<DataShare::DataShareConnection>(connection.GetRefPtr(), [holder = connection](const auto *) {
            holder->DisconnectDataShareExtAbility();
        });
    std::shared_ptr<DataShare::GeneralController> tempGenConProImp =
        std::make_shared<DataShare::GeneralControllerProviderImpl>(dataShareConnection, uri, nullptr);
    sptr<AAFwk::IDataAbilityObserver> dataObserver;
    tempGenConProImp->UnregisterObserver(uri, dataObserver);
    EXPECT_EQ(uri, Uri(""));
    LOG_INFO("Generalcontroller_ServiceImplUnregisterObserverTest_002::End");
}

/**
 * @tc.name: Generalcontroller_ServiceImplNotifyChangeTest_001
 * @tc.desc: Verify the NotifyChange operation in GeneralControllerProviderImpl with a null connection,
 *           ensuring the input empty Uri remains unchanged.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports instantiation of GeneralControllerProviderImpl and Uri.
    2. Uri equality comparison (==) works to confirm the empty Uri is unchanged.
 * @tc.step:
    1. Create an empty Uri and a GeneralControllerProviderImpl instance with a null connection, the empty Uri,
       and a null third parameter.
    2. Call the NotifyChange method of the GeneralControllerProviderImpl instance with the empty Uri.
    3. Compare the original empty Uri with a new empty Uri to verify no change.
 * @tc.expect:
    1. The input empty Uri remains unchanged after the NotifyChange operation (uri == Uri("")).
 */
HWTEST_F(ControllerTest, Generalcontroller_ServiceImplNotifyChangeTest_001, TestSize.Level0)
{
    LOG_INFO("Generalcontroller_ServiceImplNotifyChangeTest_001::Start");
    Uri uri("");
    std::shared_ptr<DataShare::GeneralController> tempGenConProImp =
        std::make_shared<DataShare::GeneralControllerProviderImpl>(nullptr, uri, nullptr);
    tempGenConProImp->NotifyChange(uri);
    EXPECT_EQ(uri, Uri(""));
    LOG_INFO("Generalcontroller_ServiceImplNotifyChangeTest_001::End");
}

/**
 * @tc.name: Generalcontroller_ServiceImplNotifyChangeTest_002
 * @tc.desc: Verify the NotifyChange operation in GeneralControllerProviderImpl with a valid but unconnected
 *           DataShareConnection, ensuring the input empty Uri remains unchanged.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports sptr/shared_ptr for DataShareConnection, and instantiation of
       GeneralControllerProviderImpl and Uri.
    2. DataShareConnection’s DisconnectDataShareExtAbility method works for the shared_ptr deleter.
    3. Uri equality comparison (==) is supported.
 * @tc.step:
    1. Create a DataShareConnection (sptr, new std::nothrow) with an empty Uri and null token; wrap it into a
       shared_ptr.
    2. Create a GeneralControllerProviderImpl instance with the shared_ptr connection, empty Uri, and null third
       parameter.
    3. Call the NotifyChange method with the empty Uri.
    4. Check if the Uri remains equal to an empty Uri.
 * @tc.expect:
    1. The input empty Uri remains unchanged after the NotifyChange operation (uri == Uri("")).
 */
HWTEST_F(ControllerTest, Generalcontroller_ServiceImplNotifyChangeTest_002, TestSize.Level0)
{
    LOG_INFO("Generalcontroller_ServiceImplNotifyChangeTest_002::Start");
    Uri uri("");
    sptr<DataShare::DataShareConnection> connection =
        new (std::nothrow) DataShare::DataShareConnection(uri, nullptr);
    auto dataShareConnection =
        std::shared_ptr<DataShare::DataShareConnection>(connection.GetRefPtr(), [holder = connection](const auto *) {
            holder->DisconnectDataShareExtAbility();
        });
    std::shared_ptr<DataShare::GeneralController> tempGenConProImp =
        std::make_shared<DataShare::GeneralControllerProviderImpl>(dataShareConnection, uri, nullptr);
    tempGenConProImp->NotifyChange(uri);
    EXPECT_EQ(uri, Uri(""));
    LOG_INFO("Generalcontroller_ServiceImplNotifyChangeTest_002::End");
}

/**
 * @tc.name: Generalcontroller_ProviderImpl_RegisterObserverExtProvider_Test_001
 * @tc.desc: Verify the RegisterObserverExtProvider operation in GeneralControllerProviderImpl for two scenarios:
 *           null connection and valid but unconnected connection, checking return error codes.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports sptr/shared_ptr for DataShareConnection, and instantiation of
       GeneralControllerProviderImpl, sptr<AAFwk::IDataAbilityObserver>, and Uri.
    2. Predefined error codes E_PROVIDER_CONN_NULL and E_PROVIDER_NOT_CONNECTED are valid.
    3. DataShareConnection’s DisconnectDataShareExtAbility method works for the shared_ptr deleter.
 * @tc.step:
    1. Create an empty Uri; create a GeneralControllerProviderImpl instance with a null connection, empty Uri,
       and null third parameter; call RegisterObserverExtProvider (isDescendants = false) and record the return code.
    2. Create a DataShareConnection (sptr, new std::nothrow) with empty Uri and null token; wrap it into a shared_ptr.
    3. Create a new GeneralControllerProviderImpl instance with the shared_ptr connection; call
       RegisterObserverExtProvider (isDescendants = false) and record the return code.
 * @tc.expect:
    1. The first RegisterObserverExtProvider call returns E_PROVIDER_CONN_NULL.
    2. The second RegisterObserverExtProvider call returns E_PROVIDER_NOT_CONNECTED.
 */
HWTEST_F(ControllerTest, Generalcontroller_ProviderImpl_RegisterObserverExtProvider_Test_001, TestSize.Level0)
{
    LOG_INFO("Generalcontroller_ProviderImpl_RegisterObserverExtProvider_Test_001::Start");

    Uri uri("");
    // connection is nullptr
    std::shared_ptr<DataShare::GeneralController> tempGenConProImp =
        std::make_shared<DataShare::GeneralControllerProviderImpl>(nullptr, uri, nullptr);
    ASSERT_NE(tempGenConProImp, nullptr);
    sptr<AAFwk::IDataAbilityObserver> dataObserver;
    // make isDescendants false
    int ret = tempGenConProImp->RegisterObserverExtProvider(uri, dataObserver, false);
    EXPECT_EQ(ret, E_PROVIDER_CONN_NULL);

    // connection not null
    sptr<DataShare::DataShareConnection> connection =
        new (std::nothrow) DataShare::DataShareConnection(uri, nullptr);
    ASSERT_NE(connection, nullptr);
    auto dataShareConnection =
        std::shared_ptr<DataShare::DataShareConnection>(connection.GetRefPtr(), [holder = connection](const auto *) {
            holder->DisconnectDataShareExtAbility();
        });
    tempGenConProImp =
        std::make_shared<DataShare::GeneralControllerProviderImpl>(dataShareConnection, uri, nullptr);
    ret = tempGenConProImp->RegisterObserverExtProvider(uri, dataObserver, false);
    EXPECT_EQ(ret, E_PROVIDER_NOT_CONNECTED);

    LOG_INFO("Generalcontroller_ProviderImpl_RegisterObserverExtProvider_Test_001::End");
}

/**
 * @tc.name: Generalcontroller_ProviderImpl_UnregisterObserverExtProvider_Test_001
 * @tc.desc: Verify the UnregisterObserverExtProvider operation in GeneralControllerProviderImpl for two scenarios:
 *           null connection and valid but unconnected connection, checking return error codes.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports sptr/shared_ptr for DataShareConnection, and instantiation of
       GeneralControllerProviderImpl, sptr<AAFwk::IDataAbilityObserver>, and Uri.
    2. Predefined error codes E_PROVIDER_CONN_NULL and E_PROVIDER_NOT_CONNECTED are valid.
    3. DataShareConnection’s dataShareProxy_ can be explicitly set to nullptr.
 * @tc.step:
    1. Create an empty Uri; create a GeneralControllerProviderImpl instance with a null connection; call
       UnregisterObserverExtProvider and record the return code.
    2. Create a DataShareConnection (sptr, new std::nothrow) with empty Uri and null token; set its dataShareProxy_ to
       nullptr.
    3. Wrap the connection into a shared_ptr; create a new GeneralControllerProviderImpl instance with it; call
       UnregisterObserverExtProvider and record the return code.
 * @tc.expect:
    1. The first UnregisterObserverExtProvider call returns E_PROVIDER_CONN_NULL.
    2. The second UnregisterObserverExtProvider call returns E_PROVIDER_NOT_CONNECTED.
 */
HWTEST_F(ControllerTest, Generalcontroller_ProviderImpl_UnregisterObserverExtProvider_Test_001, TestSize.Level0)
{
    LOG_INFO("Generalcontroller_ProviderImpl_UnregisterObserverExtProvider_Test_001::Start");

    Uri uri("");
    // connection is nullptr
    std::shared_ptr<DataShare::GeneralController> tempGenConProImp =
        std::make_shared<DataShare::GeneralControllerProviderImpl>(nullptr, uri, nullptr);
    ASSERT_NE(tempGenConProImp, nullptr);
    sptr<AAFwk::IDataAbilityObserver> dataObserver;
    // make isDescendants false
    int ret = tempGenConProImp->UnregisterObserverExtProvider(uri, dataObserver);
    EXPECT_EQ(ret, E_PROVIDER_CONN_NULL);

    // connection not null but dataShareProxy is nullptr
    sptr<DataShare::DataShareConnection> connection =
        new (std::nothrow) DataShare::DataShareConnection(uri, nullptr);
    ASSERT_NE(connection, nullptr);
    connection->dataShareProxy_ = nullptr;
    ASSERT_EQ(connection->dataShareProxy_, nullptr);
    auto dataShareConnection =
        std::shared_ptr<DataShare::DataShareConnection>(connection.GetRefPtr(), [holder = connection](const auto *) {
            holder->DisconnectDataShareExtAbility();
        });
    tempGenConProImp =
        std::make_shared<DataShare::GeneralControllerProviderImpl>(dataShareConnection, uri, nullptr);
    ret = tempGenConProImp->UnregisterObserverExtProvider(uri, dataObserver);
    EXPECT_EQ(ret, E_PROVIDER_NOT_CONNECTED);

    LOG_INFO("Generalcontroller_ProviderImpl_UnregisterObserverExtProvider_Test_001::End");
}

/**
 * @tc.name: Generalcontroller_ProviderImpl_NotifyChangeExtProvider_Test_001
 * @tc.desc: Verify the NotifyChangeExtProvider operation in GeneralControllerProviderImpl for two scenarios:
 *           null connection and valid but unconnected connection, checking return error codes.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports sptr/shared_ptr for DataShareConnection, and instantiation of
       GeneralControllerProviderImpl, ChangeInfo, and Uri.
    2. Predefined error codes E_PROVIDER_CONN_NULL and E_PROVIDER_NOT_CONNECTED are valid.
    3. ChangeInfo supports initialization with ChangeType::INSERT and a Uri list.
 * @tc.step:
    1. Create an empty Uri and a ChangeInfo (ChangeType::INSERT, {empty Uri}); create a GeneralControllerProviderImpl
       instance with a null connection; call NotifyChangeExtProvider and record the return code.
    2. Create a DataShareConnection (sptr, new std::nothrow) with empty Uri and null token; wrap it into a shared_ptr.
    3. Create a new GeneralControllerProviderImpl instance with the shared_ptr connection; call NotifyChangeExtProvider
       with the same ChangeInfo and record the return code.
 * @tc.expect:
    1. The first NotifyChangeExtProvider call returns E_PROVIDER_CONN_NULL.
    2. The second NotifyChangeExtProvider call returns E_PROVIDER_NOT_CONNECTED.
 */
HWTEST_F(ControllerTest, Generalcontroller_ProviderImpl_NotifyChangeExtProvider_Test_001, TestSize.Level0)
{
    LOG_INFO("Generalcontroller_ProviderImpl_NotifyChangeExtProvider_Test_001::Start");

    Uri uri("");
    // connection is nullptr
    ChangeInfo changeInfo = { ChangeInfo::ChangeType::INSERT, { uri } };
    std::shared_ptr<DataShare::GeneralController> tempGenConProImp =
        std::make_shared<DataShare::GeneralControllerProviderImpl>(nullptr, uri, nullptr);
    sptr<AAFwk::IDataAbilityObserver> dataObserver;
    // make isDescendants false
    int ret = tempGenConProImp->NotifyChangeExtProvider(changeInfo);
    EXPECT_EQ(ret, E_PROVIDER_CONN_NULL);

    // connection not null
    sptr<DataShare::DataShareConnection> connection =
        new (std::nothrow) DataShare::DataShareConnection(uri, nullptr);
    auto dataShareConnection =
        std::shared_ptr<DataShare::DataShareConnection>(connection.GetRefPtr(), [holder = connection](const auto *) {
            holder->DisconnectDataShareExtAbility();
        });
    tempGenConProImp =
        std::make_shared<DataShare::GeneralControllerProviderImpl>(dataShareConnection, uri, nullptr);
    ret = tempGenConProImp->NotifyChangeExtProvider(changeInfo);
    EXPECT_EQ(ret, E_PROVIDER_NOT_CONNECTED);

    LOG_INFO("Generalcontroller_ProviderImpl_NotifyChangeExtProvider_Test_001::End");
}

/**
 * @tc.name: Generalcontroller_ServiceImpl_RegisterObserverExtProvider_Test_001
 * @tc.desc: Verify the RegisterObserverExtProvider operation in GeneralControllerServiceImpl with an empty Uri
 *           and null observer, confirming failure via a return code of -1.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports instantiation of GeneralControllerServiceImpl, sptr<AAFwk::IDataAbilityObserver>,
       and Uri.
    2. GeneralControllerServiceImpl can be initialized with a test URI string ("GeneralControllerServiceImpl").
 * @tc.step:
    1. Create an empty Uri and a GeneralControllerServiceImpl instance initialized with the test URI
       ("GeneralControllerServiceImpl").
    2. Create a null sptr<AAFwk::IDataAbilityObserver> (dataObserver).
    3. Call the RegisterObserverExtProvider method with the empty Uri, dataObserver, and isDescendants = false.
    4. Check the return code of the RegisterObserverExtProvider operation.
 * @tc.expect:
    1. The RegisterObserverExtProvider operation returns -1, indicating failure.
 */
HWTEST_F(ControllerTest, Generalcontroller_ServiceImpl_RegisterObserverExtProvider_Test_001, TestSize.Level0)
{
    LOG_INFO("Generalcontroller_ServiceImpl_RegisterObserverExtProvider_Test_001::Start");

    Uri uri("");
    std::string extUri = "GeneralControllerServiceImpl";

    std::shared_ptr<DataShare::GeneralController> tempGenConProImp =
        std::make_shared<DataShare::GeneralControllerServiceImpl>(extUri);
    ASSERT_NE(tempGenConProImp, nullptr);
    sptr<AAFwk::IDataAbilityObserver> dataObserver;

    int ret = tempGenConProImp->RegisterObserverExtProvider(uri, dataObserver, false);
    EXPECT_EQ(ret, -1);

    LOG_INFO("Generalcontroller_ServiceImpl_RegisterObserverExtProvider_Test_001::End");
}

/**
 * @tc.name: Generalcontroller_ServiceImpl_UnregisterObserverExtProvider_Test_001
 * @tc.desc: Verify the UnregisterObserverExtProvider operation in GeneralControllerServiceImpl with an empty Uri
 *           and null observer, confirming failure via a return code of -1.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports instantiation of GeneralControllerServiceImpl, sptr<AAFwk::IDataAbilityObserver>,
       and Uri.
    2. GeneralControllerServiceImpl can be initialized with a test URI string ("GeneralControllerServiceImpl").
 * @tc.step:
    1. Create an empty Uri and a GeneralControllerServiceImpl instance initialized with the test URI
       ("GeneralControllerServiceImpl").
    2. Create a null sptr<AAFwk::IDataAbilityObserver> (dataObserver).
    3. Call the UnregisterObserverExtProvider method with the empty Uri and dataObserver.
    4. Check the return code of the UnregisterObserverExtProvider operation.
 * @tc.expect:
    1. The UnregisterObserverExtProvider operation returns -1, indicating failure.
 */
HWTEST_F(ControllerTest, Generalcontroller_ServiceImpl_UnregisterObserverExtProvider_Test_001, TestSize.Level0)
{
    LOG_INFO("Generalcontroller_ServiceImpl_UnregisterObserverExtProvider_Test_001::Start");

    Uri uri("");
    std::string extUri = "GeneralControllerServiceImpl";

    std::shared_ptr<DataShare::GeneralController> tempGenConProImp =
        std::make_shared<DataShare::GeneralControllerServiceImpl>(extUri);
    ASSERT_NE(tempGenConProImp, nullptr);
    sptr<AAFwk::IDataAbilityObserver> dataObserver;

    int ret = tempGenConProImp->UnregisterObserverExtProvider(uri, dataObserver);
    EXPECT_EQ(ret, -1);

    LOG_INFO("Generalcontroller_ServiceImpl_UnregisterObserverExtProvider_Test_001::End");
}

/**
 * @tc.name: Generalcontroller_ServiceImpl_NotifyChangeExtProvider_Test_001
 * @tc.desc: Verify the NotifyChangeExtProvider operation in GeneralControllerServiceImpl with a ChangeInfo containing
 *           an empty Uri, confirming failure via a return code of -1.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports instantiation of GeneralControllerServiceImpl, ChangeInfo, and Uri.
    2. GeneralControllerServiceImpl can be initialized with a test URI string ("GeneralControllerServiceImpl").
    3. ChangeInfo supports initialization with ChangeType::INSERT and a list containing an empty Uri.
 * @tc.step:
    1. Create an empty Uri and a ChangeInfo (ChangeType::INSERT, {empty Uri}).
    2. Create a GeneralControllerServiceImpl instance initialized with the test URI ("GeneralControllerServiceImpl").
    3. Call the NotifyChangeExtProvider method with the created ChangeInfo.
    4. Check the return code of the NotifyChangeExtProvider operation.
 * @tc.expect:
    1. The NotifyChangeExtProvider operation returns -1, indicating failure.
 */
HWTEST_F(ControllerTest, Generalcontroller_ServiceImpl_NotifyChangeExtProvider_Test_001, TestSize.Level0)
{
    LOG_INFO("Generalcontroller_ServiceImpl_NotifyChangeExtProvider_Test_001::Start");

    Uri uri("");
    std::string extUri = "GeneralControllerServiceImpl";

    ChangeInfo changeInfo = { ChangeInfo::ChangeType::INSERT, { uri } };
    // connection is nullptr
    std::shared_ptr<DataShare::GeneralController> tempGenConProImp =
        std::make_shared<DataShare::GeneralControllerServiceImpl>(extUri);
    ASSERT_NE(tempGenConProImp, nullptr);
    sptr<AAFwk::IDataAbilityObserver> dataObserver;

    int ret = tempGenConProImp->NotifyChangeExtProvider(changeInfo);
    EXPECT_EQ(ret, -1);

    LOG_INFO("Generalcontroller_ServiceImpl_NotifyChangeExtProvider_Test_001::End");
}

/**
 * @tc.name: ControllerTest_ExtSpecialControllerOpenFileTest_001
 * @tc.desc: Verify the OpenFile operation in ExtSpecialController with a null connection, confirming failure
 *           via a negative return value.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports instantiation of ExtSpecialController, Uri, and string (for mode parameter).
    2. "test001" is a valid mode string for the OpenFile operation.
 * @tc.step:
    1. Create an empty Uri and an ExtSpecialController instance with a null connection, the empty Uri, and a null third
       parameter.
    2. Define a mode string "test001" for the OpenFile operation.
    3. Call the OpenFile method of the ExtSpecialController instance with the empty Uri and mode string.
    4. Check whether the return value of the OpenFile operation is negative.
 * @tc.expect:
    1. The OpenFile operation returns a negative value, indicating operation failure.
 */
HWTEST_F(ControllerTest, ControllerTest_ExtSpecialControllerOpenFileTest_001, TestSize.Level0)
{
    LOG_INFO("ControllerTest_ExtSpecialControllerOpenFileTest_001::Start");
    Uri uri("");
    std::shared_ptr<DataShare::ExtSpecialController> tempExtSpeCon =
        std::make_shared<DataShare::ExtSpecialController>(nullptr, uri, nullptr);
    std::string mode = "test001";
    int result = tempExtSpeCon->OpenFile(uri, mode);
    EXPECT_EQ((result < 0), true);
    LOG_INFO("ControllerTest_ExtSpecialControllerOpenFileTest_001::End");
}

/**
 * @tc.name: ControllerTest_ExtSpecialControllerOpenFileTest_002
 * @tc.desc: Verify the OpenFile operation in ExtSpecialController with a valid but unconnected
 *           DataShareConnection, confirming failure via a negative return value.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports sptr/shared_ptr for DataShareConnection, and instantiation of
       ExtSpecialController, Uri, and string (for mode parameter).
    2. DataShareConnection’s DisconnectDataShareExtAbility method works for the shared_ptr deleter.
    3. "test001" is a valid mode string for the OpenFile operation.
 * @tc.step:
    1. Create a DataShareConnection (sptr, new std::nothrow) with an empty Uri and null token; wrap it into a
       shared_ptr.
    2. Create an ExtSpecialController instance with the shared_ptr connection, empty Uri, and null third parameter.
    3. Define a mode string "test001"; call OpenFile with the empty Uri and mode.
    4. Check if the return value of OpenFile is negative.
 * @tc.expect:
    1. The OpenFile operation returns a negative value, indicating operation failure.
 */
HWTEST_F(ControllerTest, ControllerTest_ExtSpecialControllerOpenFileTest_002, TestSize.Level0)
{
    LOG_INFO("ControllerTest_ExtSpecialControllerOpenFileTest_002::Start");
    Uri uri("");
    sptr<DataShare::DataShareConnection> connection =
        new (std::nothrow) DataShare::DataShareConnection(uri, nullptr);
    auto dataShareConnection =
        std::shared_ptr<DataShare::DataShareConnection>(connection.GetRefPtr(), [holder = connection](const auto *) {
            holder->DisconnectDataShareExtAbility();
        });
    std::shared_ptr<DataShare::ExtSpecialController> tempExtSpeCon =
        std::make_shared<DataShare::ExtSpecialController>(dataShareConnection, uri, nullptr);
    std::string mode = "test001";
    int result = tempExtSpeCon->OpenFile(uri, mode);
    EXPECT_EQ((result < 0), true);
    LOG_INFO("ControllerTest_ExtSpecialControllerOpenFileTest_002::End");
}

/**
 * @tc.name: ControllerTest_ExtSpecialControllerOpenFileWithErrCodeTest_001
 * @tc.desc: Verify the behavior of the OpenFileWithErrCode function in ExtSpecialController when the
 *           DataShareConnection is set to null, focusing on the returned file descriptor.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports instantiation of ExtSpecialController, Uri, and handling of int32_t
       error codes and string modes without initialization errors.
    2. The ExtSpecialController constructor accepts null DataShareConnection, empty Uri, and null
       additional parameters (third argument) as valid inputs.
    3. The OpenFileWithErrCode function of ExtSpecialController accepts Uri, std::string (mode), and
       int32_t& (errCode) as parameters and returns an int file descriptor.
 * @tc.step:
    1. Create an empty Uri object (uri) with no specific path or authority.
    2. Create an ExtSpecialController instance (tempExtSpeCon) by passing null as the DataShareConnection,
       the empty uri, and null as the third constructor parameter.
    3. Define a std::string mode ("test001") and an int32_t errCode initialized to 0.
    4. Call the OpenFileWithErrCode method of tempExtSpeCon, passing uri, mode, and errCode as arguments.
    5. Check the integer file descriptor returned by the OpenFileWithErrCode method.
 * @tc.expect:
    1. The OpenFileWithErrCode function returns a file descriptor of -1.
 */
HWTEST_F(ControllerTest, ControllerTest_ExtSpecialControllerOpenFileWithErrCodeTest_001, TestSize.Level0)
{
    LOG_INFO("ControllerTest_ExtSpecialControllerOpenFileWithErrCodeTest_001::Start");
    Uri uri("");
    std::shared_ptr<DataShare::ExtSpecialController> tempExtSpeCon =
        std::make_shared<DataShare::ExtSpecialController>(nullptr, uri, nullptr);
    std::string mode = "test001";
    int32_t errCode = 0;
    int fd = tempExtSpeCon->OpenFileWithErrCode(uri, mode, errCode);
    EXPECT_EQ(fd, -1);
    LOG_INFO("ControllerTest_ExtSpecialControllerOpenFileWithErrCodeTest_001::End");
}

/**
 * @tc.name: ControllerTest_ExtSpecialControllerOpenFileWithErrCodeTest_002
 * @tc.desc: Verify the behavior of the OpenFileWithErrCode function in ExtSpecialController when the
 *           DataShareConnection is valid but in an unconnected state, focusing on the returned file descriptor.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports instantiation of DataShareConnection (with empty Uri and null token),
       ExtSpecialController, and Uri without initialization errors.
    2. The DataShareConnection created with an empty Uri and null token remains in an unconnected state.
    3. The ExtSpecialController constructor accepts a valid (unconnected) DataShareConnection, empty Uri,
       and null third parameter as inputs.
 * @tc.step:
    1. Create an empty Uri object (uri) with no specific path or authority.
    2. Create a DataShareConnection instance (connection) using the empty uri and a null IRemoteObject token,
       then wrap it in a shared_ptr (dataShareConnection) with a custom deleter for disconnection.
    3. Create an ExtSpecialController instance (tempExtSpeCon) by passing dataShareConnection, the empty uri,
       and null as the third constructor parameter.
    4. Define a std::string mode ("test001") and an int32_t errCode initialized to 0.
    5. Call tempExtSpeCon->OpenFileWithErrCode(uri, mode, errCode) and record the returned file descriptor.
    6. Check if the returned file descriptor matches the expected value.
 * @tc.expect:
    1. The OpenFileWithErrCode function returns a file descriptor of -1.
 */
HWTEST_F(ControllerTest, ControllerTest_ExtSpecialControllerOpenFileWithErrCodeTest_002, TestSize.Level0)
{
    LOG_INFO("ControllerTest_ExtSpecialControllerOpenFileWithErrCodeTest_002::Start");
    Uri uri("");
    sptr<DataShare::DataShareConnection> connection =
        new (std::nothrow) DataShare::DataShareConnection(uri, nullptr);
    auto dataShareConnection =
        std::shared_ptr<DataShare::DataShareConnection>(connection.GetRefPtr(), [holder = connection](const auto *) {
            holder->DisconnectDataShareExtAbility();
        });
    std::shared_ptr<DataShare::ExtSpecialController> tempExtSpeCon =
        std::make_shared<DataShare::ExtSpecialController>(dataShareConnection, uri, nullptr);
    std::string mode = "test001";
    int32_t errCode = 0;
    int fd = tempExtSpeCon->OpenFileWithErrCode(uri, mode, errCode);
    EXPECT_EQ(fd, -1);
    LOG_INFO("ControllerTest_ExtSpecialControllerOpenFileWithErrCodeTest_002::End");
}

/**
 * @tc.name: ControllerTest_ExtSpecialControllerOpenRawFileTest_001
 * @tc.desc: Verify the behavior of the OpenRawFile function in ExtSpecialController when the
 *           DataShareConnection is set to null, focusing on whether the returned result is negative.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports creating ExtSpecialController (with null connection) and empty Uri
       without initialization errors.
    2. The OpenRawFile function of ExtSpecialController accepts Uri and std::string (mode) as parameters
       and returns an int result.
    3. Negative return values from OpenRawFile indicate operation failure (as per test expectations).
 * @tc.step:
    1. Create an empty Uri object (uri) with no specific content.
    2. Create an ExtSpecialController instance (tempExtSpeCon) using null as the DataShareConnection,
       the empty uri, and null as the third constructor parameter.
    3. Define a std::string mode ("test001") for the OpenRawFile call.
    4. Call tempExtSpeCon->OpenRawFile(uri, mode) and record the returned int result.
    5. Check if the returned result is a negative value.
 * @tc.expect:
    1. The OpenRawFile function returns a negative integer value.
 */
HWTEST_F(ControllerTest, ControllerTest_ExtSpecialControllerOpenRawFileTest_001, TestSize.Level0)
{
    LOG_INFO("ControllerTest_ExtSpecialControllerOpenRawFileTest_001::Start");
    Uri uri("");
    std::shared_ptr<DataShare::ExtSpecialController> tempExtSpeCon =
        std::make_shared<DataShare::ExtSpecialController>(nullptr, uri, nullptr);
    std::string mode = "test001";
    int result = tempExtSpeCon->OpenRawFile(uri, mode);
    EXPECT_EQ((result < 0), true);
    LOG_INFO("ControllerTest_ExtSpecialControllerOpenRawFileTest_001::End");
}

/**
 * @tc.name: ControllerTest_ExtSpecialControlleOpenRawFileTest_002
 * @tc.desc: Verify the behavior of the OpenRawFile function in ExtSpecialController when the
 *           DataShareConnection is valid but unconnected, focusing on whether the returned result is negative.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports instantiation of DataShareConnection (empty Uri, null token),
       ExtSpecialController, and empty Uri without errors.
    2. The DataShareConnection created with empty Uri and null token stays in an unconnected state.
    3. The OpenRawFile function of ExtSpecialController returns an int, with negative values indicating failure.
 * @tc.step:
    1. Create an empty Uri object (uri) with no specific path.
    2. Create a DataShareConnection instance (connection) using the empty uri and null IRemoteObject token,
       then wrap it in a shared_ptr (dataShareConnection) with a deleter that calls DisconnectDataShareExtAbility.
    3. Create an ExtSpecialController instance (tempExtSpeCon) by passing dataShareConnection, the empty uri,
       and null as the third constructor parameter.
    4. Define a std::string mode ("test001") for the OpenRawFile call.
    5. Call tempExtSpeCon->OpenRawFile(uri, mode) and record the returned int result.
    6. Check if the returned result is negative.
 * @tc.expect:
    1. The OpenRawFile function returns a negative integer value.
 */
HWTEST_F(ControllerTest, ControllerTest_ExtSpecialControlleOpenRawFileTest_002, TestSize.Level0)
{
    LOG_INFO("ControllerTest_ExtSpecialControllerOpenRawFileTest_002::Start");
    Uri uri("");
    sptr<DataShare::DataShareConnection> connection =
        new (std::nothrow) DataShare::DataShareConnection(uri, nullptr);
    auto dataShareConnection =
        std::shared_ptr<DataShare::DataShareConnection>(connection.GetRefPtr(), [holder = connection](const auto *) {
            holder->DisconnectDataShareExtAbility();
        });
    std::shared_ptr<DataShare::ExtSpecialController> tempExtSpeCon =
        std::make_shared<DataShare::ExtSpecialController>(dataShareConnection, uri, nullptr);
    std::string mode = "test001";
    int result = tempExtSpeCon->OpenRawFile(uri, mode);
    EXPECT_EQ((result < 0), true);
    LOG_INFO("ControllerTest_ExtSpecialControllerOpenRawFileTest_002::End");
}

/**
 * @tc.name: ControllerTest_ExtSpecialControllerGetTypeTest_001
 * @tc.desc: Verify the behavior of the GetType function in ExtSpecialController when the
 *           DataShareConnection is set to null, focusing on the returned string value.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports creating ExtSpecialController (with null connection) and empty Uri
       without initialization errors.
    2. The GetType function of ExtSpecialController accepts a Uri parameter and returns a std::string.
    3. An empty string returned by GetType indicates operation failure (as per test expectations).
 * @tc.step:
    1. Create an empty Uri object (uri) with no specific authority or path.
    2. Create an ExtSpecialController instance (tempExtSpeCon) using null as the DataShareConnection,
       the empty uri, and null as the third constructor parameter.
    3. Call tempExtSpeCon->GetType(uri) and record the returned std::string result.
    4. Check if the returned string is empty.
 * @tc.expect:
    1. The GetType function returns an empty std::string.
 */
HWTEST_F(ControllerTest, ControllerTest_ExtSpecialControllerGetTypeTest_001, TestSize.Level0)
{
    LOG_INFO("ControllerTest_ExtSpecialControllerGetTypeTest_001::Start");
    Uri uri("");
    std::shared_ptr<DataShare::ExtSpecialController> tempExtSpeCon =
        std::make_shared<DataShare::ExtSpecialController>(nullptr, uri, nullptr);
    std::string result = tempExtSpeCon->GetType(uri);
    EXPECT_EQ(result, "");
    LOG_INFO("ControllerTest_ExtSpecialControllerGetTypeTest_001::End");
}

/**
 * @tc.name: ControllerTest_ExtSpecialControlleGetTypeTest_002
 * @tc.desc: Verify the behavior of the GetType function in ExtSpecialController when the
 *           DataShareConnection is valid but unconnected, focusing on the returned string value.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports instantiation of DataShareConnection (empty Uri, null token),
       ExtSpecialController, and empty Uri without errors.
    2. The DataShareConnection created with empty Uri and null token remains unconnected.
    3. The GetType function of ExtSpecialController returns a std::string, with empty string indicating failure.
 * @tc.step:
    1. Create an empty Uri object (uri) with no specific content.
    2. Create a DataShareConnection instance (connection) using the empty uri and null IRemoteObject token,
       then wrap it in a shared_ptr (dataShareConnection) with a deleter for disconnection.
    3. Create an ExtSpecialController instance (tempExtSpeCon) by passing dataShareConnection, the empty uri,
       and null as the third constructor parameter.
    4. Call tempExtSpeCon->GetType(uri) and record the returned std::string result.
    5. Check if the returned string is empty.
 * @tc.expect:
    1. The GetType function returns an empty std::string.
 */
HWTEST_F(ControllerTest, ControllerTest_ExtSpecialControlleGetTypeTest_002, TestSize.Level0)
{
    LOG_INFO("ControllerTest_ExtSpecialControllerGetTypeTest_002::Start");
    Uri uri("");
    sptr<DataShare::DataShareConnection> connection =
        new (std::nothrow) DataShare::DataShareConnection(uri, nullptr);
    auto dataShareConnection =
        std::shared_ptr<DataShare::DataShareConnection>(connection.GetRefPtr(), [holder = connection](const auto *) {
            holder->DisconnectDataShareExtAbility();
        });
    std::shared_ptr<DataShare::ExtSpecialController> tempExtSpeCon =
        std::make_shared<DataShare::ExtSpecialController>(dataShareConnection, uri, nullptr);
    std::string result = tempExtSpeCon->GetType(uri);
    EXPECT_EQ(result, "");
    LOG_INFO("ControllerTest_ExtSpecialControllerGetTypeTest_002::End");
}

/**
 * @tc.name: ControllerTest_ExtSpecialControllerBatchInsertTest_001
 * @tc.desc: Verify the behavior of the BatchInsert function in ExtSpecialController when the
 *           DataShareConnection is set to null, focusing on whether the returned result is negative.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports creating ExtSpecialController (null connection), Uri, DataShareValuesBucket,
       and std::vector<DataShareValuesBucket> without initialization errors.
    2. The DataShareValuesBucket class allows adding key-value pairs (e.g., "name" as string, "phoneNumber" as double).
    3. The BatchInsert function of ExtSpecialController accepts Uri and std::vector<DataShareValuesBucket>
       and returns an int, with negative values indicating failure.
 * @tc.step:
    1. Create an empty Uri object (uri) with no specific path.
    2. Create an ExtSpecialController instance (tempExtSpeCon) using null as the DataShareConnection,
       the empty uri, and null as the third constructor parameter.
    3. Create two DataShareValuesBucket objects: add "name" = "dataShareTest006" and "phoneNumber" = 20.6 to the first,
       and "name" = "dataShareTest007" and "phoneNumber" = 20.5 to the second.
    4. Push both buckets into a std::vector<DataShareValuesBucket> (values).
    5. Call tempExtSpeCon->BatchInsert(uri, values) and record the returned int result.
    6. Check if the returned result is negative.
 * @tc.expect:
    1. The BatchInsert function returns a negative integer value.
 */
HWTEST_F(ControllerTest, ControllerTest_ExtSpecialControllerBatchInsertTest_001, TestSize.Level0)
{
    LOG_INFO("ControllerTest_ExtSpecialControllerBatchInsertTest_001::Start");
    Uri uri("");
    std::shared_ptr<DataShare::ExtSpecialController> tempExtSpeCon =
        std::make_shared<DataShare::ExtSpecialController>(nullptr, uri, nullptr);
    DataShare::DataShareValuesBucket valuesBucket1;
    valuesBucket1.Put("name", "dataShareTest006");
    valuesBucket1.Put("phoneNumber", 20.6);
    DataShare::DataShareValuesBucket valuesBucket2;
    valuesBucket2.Put("name", "dataShareTest007");
    valuesBucket2.Put("phoneNumber", 20.5);
    std::vector<DataShare::DataShareValuesBucket> values;
    values.push_back(valuesBucket1);
    values.push_back(valuesBucket2);
    int result = tempExtSpeCon->BatchInsert(uri, values);
    EXPECT_EQ((result < 0), true);
    LOG_INFO("ControllerTest_ExtSpecialControllerBatchInsertTest_001::End");
}

/**
 * @tc.name: ControllerTest_ExtSpecialControllerBatchInsertTest_002
 * @tc.desc: Verify the behavior of the BatchInsert function in ExtSpecialController when the
 *           DataShareConnection is valid but unconnected, focusing on whether the returned result is negative.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports instantiation of DataShareConnection (empty Uri, null token),
       ExtSpecialController, Uri, DataShareValuesBucket, and vector of buckets without errors.
    2. The DataShareConnection created with empty Uri and null token stays in an unconnected state.
    3. The BatchInsert function of ExtSpecialController returns an int, with negative values indicating failure.
 * @tc.step:
    1. Create an empty Uri object (uri) with no specific content.
    2. Create a DataShareConnection instance (connection) using the empty uri and null IRemoteObject token,
       then wrap it in a shared_ptr (dataShareConnection) with a deleter that calls DisconnectDataShareExtAbility.
    3. Create an ExtSpecialController instance (tempExtSpeCon) by passing dataShareConnection, the empty uri,
       and null as the third constructor parameter.
    4. Create two DataShareValuesBucket objects: add "name" = "dataShareTest006" + "phoneNumber" = 20.6 to the first,
       and "name" = "dataShareTest007" + "phoneNumber" = 20.5 to the second; push both into a vector (values).
    5. Call tempExtSpeCon->BatchInsert(uri, values) and record the returned int result.
    6. Check if the returned result is negative.
 * @tc.expect:
    1. The BatchInsert function returns a negative integer value.
 */
HWTEST_F(ControllerTest, ControllerTest_ExtSpecialControllerBatchInsertTest_002, TestSize.Level0)
{
    LOG_INFO("ControllerTest_ExtSpecialControllerBatchInsertTest_002::Start");
    Uri uri("");
    sptr<DataShare::DataShareConnection> connection =
        new (std::nothrow) DataShare::DataShareConnection(uri, nullptr);
    auto dataShareConnection =
        std::shared_ptr<DataShare::DataShareConnection>(connection.GetRefPtr(), [holder = connection](const auto *) {
            holder->DisconnectDataShareExtAbility();
        });
    std::shared_ptr<DataShare::ExtSpecialController> tempExtSpeCon =
        std::make_shared<DataShare::ExtSpecialController>(dataShareConnection, uri, nullptr);
    DataShare::DataShareValuesBucket valuesBucket1;
    valuesBucket1.Put("name", "dataShareTest006");
    valuesBucket1.Put("phoneNumber", 20.6);
    DataShare::DataShareValuesBucket valuesBucket2;
    valuesBucket2.Put("name", "dataShareTest007");
    valuesBucket2.Put("phoneNumber", 20.5);
    std::vector<DataShare::DataShareValuesBucket> values;
    values.push_back(valuesBucket1);
    values.push_back(valuesBucket2);
    int result = tempExtSpeCon->BatchInsert(uri, values);
    EXPECT_EQ((result < 0), true);
    LOG_INFO("ControllerTest_ExtSpecialControllerBatchInsertTest_002::End");
}

/**
 * @tc.name: ControllerTest_ExtSpecialControllerBatchUpdateTest_001
 * @tc.desc: Verify the behavior of the BatchUpdate function in ExtSpecialController when the
 *           DataShareConnection is set to null, focusing on the returned result code.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports creating ExtSpecialController (null connection), Uri, UpdateOperations,
       and std::vector<BatchUpdateResult> without initialization errors.
    2. The BatchUpdate function of ExtSpecialController accepts UpdateOperations and vector<BatchUpdateResult>
       as parameters and returns an int result code.
    3. A return value of -1 from BatchUpdate indicates operation failure (as per test expectations).
 * @tc.step:
    1. Create an empty Uri object (uri) with no specific path or authority.
    2. Create an ExtSpecialController instance (tempExtSpeCon) using null as the DataShareConnection,
       the empty uri, and null as the third constructor parameter.
    3. Create an empty UpdateOperations object (operations) and an empty std::vector<BatchUpdateResult> (results).
    4. Call tempExtSpeCon->BatchUpdate(operations, results) and record the returned int result.
    5. Check if the returned result matches the expected error code.
 * @tc.expect:
    1. The BatchUpdate function returns a result code of -1.
 */
HWTEST_F(ControllerTest, ControllerTest_ExtSpecialControllerBatchUpdateTest_001, TestSize.Level0)
{
    LOG_INFO("ControllerTest_ExtSpecialControllerBatchUpdateTest_001::Start");
    Uri uri("");
    std::shared_ptr<DataShare::ExtSpecialController> tempExtSpeCon =
        std::make_shared<DataShare::ExtSpecialController>(nullptr, uri, nullptr);
    DataShare::UpdateOperations operations;
    std::vector<DataShare::BatchUpdateResult> results;
    int result = tempExtSpeCon->BatchUpdate(operations, results);
    EXPECT_EQ(result, -1);
    LOG_INFO("ControllerTest_ExtSpecialControllerBatchUpdateTest_001::End");
}

/**
 * @tc.name: ControllerTest_ExtSpecialControllerBatchUpdateTest_002
 * @tc.desc: Verify the behavior of the BatchUpdate function in ExtSpecialController when the
 *           DataShareConnection is valid but unconnected, focusing on the returned result code.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports instantiation of DataShareConnection (empty Uri, null token),
       ExtSpecialController, Uri, UpdateOperations, and vector<BatchUpdateResult> without errors.
    2. The DataShareConnection created with empty Uri and null token remains unconnected.
    3. The BatchUpdate function of ExtSpecialController returns an int, with -1 indicating operation failure.
 * @tc.step:
    1. Create an empty Uri object (uri) with no specific content.
    2. Create a DataShareConnection instance (connection) using the empty uri and null IRemoteObject token,
       then wrap it in a shared_ptr (dataShareConnection) with a deleter for disconnection.
    3. Create an ExtSpecialController instance (tempExtSpeCon) by passing dataShareConnection, the empty uri,
       and null as the third constructor parameter.
    4. Create an empty UpdateOperations object (operations) and an empty std::vector<BatchUpdateResult> (results).
    5. Call tempExtSpeCon->BatchUpdate(operations, results) and record the returned int result.
    6. Check if the returned result is -1.
 * @tc.expect:
    1. The BatchUpdate function returns a result code of -1.
 */
HWTEST_F(ControllerTest, ControllerTest_ExtSpecialControllerBatchUpdateTest_002, TestSize.Level0)
{
    LOG_INFO("ControllerTest_ExtSpecialControllerBatchUpdateTest_002::Start");
    Uri uri("");
    sptr<DataShare::DataShareConnection> connection =
        new (std::nothrow) DataShare::DataShareConnection(uri, nullptr);
    auto dataShareConnection =
        std::shared_ptr<DataShare::DataShareConnection>(connection.GetRefPtr(), [holder = connection](const auto *) {
            holder->DisconnectDataShareExtAbility();
        });
    std::shared_ptr<DataShare::ExtSpecialController> tempExtSpeCon =
        std::make_shared<DataShare::ExtSpecialController>(dataShareConnection, uri, nullptr);
    DataShare::UpdateOperations operations;
    std::vector<DataShare::BatchUpdateResult> results;
    int result = tempExtSpeCon->BatchUpdate(operations, results);
    EXPECT_EQ(result, -1);
    LOG_INFO("ControllerTest_ExtSpecialControllerBatchUpdateTest_002::End");
}

/**
 * @tc.name: ControllerTest_ExtSpecialControllerInsertExtTest_001
 * @tc.desc: Verify the behavior of the InsertExt function in ExtSpecialController when the
 *           DataShareConnection is set to null, focusing on the returned result code.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports creating ExtSpecialController (null connection), Uri, DataShareValuesBucket,
       and std::string (result) without initialization errors.
    2. The DataShareValuesBucket allows adding key-value pairs (e.g., "name" as string, "phoneNumber" as double).
    3. The InsertExt function of ExtSpecialController accepts Uri, DataShareValuesBucket, and std::string&
       as parameters and returns an int, with -1 indicating failure.
 * @tc.step:
    1. Create an empty Uri object (uri) with no specific path.
    2. Create an ExtSpecialController instance (tempExtSpeCon) using null as the DataShareConnection,
       the empty uri, and null as the third constructor parameter.
    3. Create a DataShareValuesBucket and add "name" = "dataShareTest006" and "phoneNumber" = 20.6 to it.
    4. Define a std::string (result1) initialized to "test001" to store the function's output string.
    5. Call tempExtSpeCon->InsertExt(uri, valuesBucket, result1) and record the returned int result.
    6. Check if the returned result is -1.
 * @tc.expect:
    1. The InsertExt function returns a result code of -1.
 */
HWTEST_F(ControllerTest, ControllerTest_ExtSpecialControllerInsertExtTest_001, TestSize.Level0)
{
    LOG_INFO("ControllerTest_ExtSpecialControllerInsertExtTest_001::Start");
    Uri uri("");
    std::shared_ptr<DataShare::ExtSpecialController> tempExtSpeCon =
        std::make_shared<DataShare::ExtSpecialController>(nullptr, uri, nullptr);
    DataShare::DataShareValuesBucket valuesBucket;
    valuesBucket.Put("name", "dataShareTest006");
    valuesBucket.Put("phoneNumber", 20.6);
    std::string result1 = "test001";
    int result = tempExtSpeCon->InsertExt(uri, valuesBucket, result1);
    EXPECT_EQ(result, -1);
    LOG_INFO("ControllerTest_ExtSpecialControllerInsertExtTest_001::End");
}

/**
 * @tc.name: ControllerTest_ExtSpecialControllerInsertExtTest_002
 * @tc.desc: Verify the behavior of the InsertExt function in ExtSpecialController when the
 *           DataShareConnection is valid but unconnected, focusing on the returned result code.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports instantiation of DataShareConnection (empty Uri, null token),
       ExtSpecialController, Uri, DataShareValuesBucket, and std::string without errors.
    2. The DataShareConnection created with empty Uri and null token stays in an unconnected state.
    3. The InsertExt function of ExtSpecialController returns an int, with -1 indicating operation failure.
 * @tc.step:
    1. Create an empty Uri object (uri) with no specific content.
    2. Create a DataShareConnection instance (connection) using the empty uri and null IRemoteObject token,
       then wrap it in a shared_ptr (dataShareConnection) with a deleter that calls DisconnectDataShareExtAbility.
    3. Create an ExtSpecialController instance (tempExtSpeCon) by passing dataShareConnection, the empty uri,
       and null as the third constructor parameter.
    4. Create a DataShareValuesBucket (valuesBucket) and add "name" = "dataShareTest006" + "phoneNumber" = 20.6 to it.
    5. Define a std::string (result1) initialized to "test001"; call tempExtSpeCon->InsertExt.
    6. Record the returned int result and check if it is -1.
 * @tc.expect:
    1. The InsertExt function returns a result code of -1.
 */
HWTEST_F(ControllerTest, ControllerTest_ExtSpecialControllerInsertExtTest_002, TestSize.Level0)
{
    LOG_INFO("ControllerTest_ExtSpecialControllerInsertExtTest_002::Start");
    Uri uri("");
    sptr<DataShare::DataShareConnection> connection =
        new (std::nothrow) DataShare::DataShareConnection(uri, nullptr);
    auto dataShareConnection =
        std::shared_ptr<DataShare::DataShareConnection>(connection.GetRefPtr(), [holder = connection](const auto *) {
            holder->DisconnectDataShareExtAbility();
        });
    std::shared_ptr<DataShare::ExtSpecialController> tempExtSpeCon =
        std::make_shared<DataShare::ExtSpecialController>(dataShareConnection, uri, nullptr);
    DataShare::DataShareValuesBucket valuesBucket;
    valuesBucket.Put("name", "dataShareTest006");
    valuesBucket.Put("phoneNumber", 20.6);
    std::string result1 = "test001";
    int result = tempExtSpeCon->InsertExt(uri, valuesBucket, result1);
    EXPECT_EQ(result, -1);
    LOG_INFO("ControllerTest_ExtSpecialControllerInsertExtTest_002::End");
}

/**
 * @tc.name: ControllerTest_ExtSpecialControllerExecuteBatchTest_001
 * @tc.desc: Verify the behavior of the ExecuteBatch function in ExtSpecialController when the
 *           DataShareConnection is set to null, focusing on the returned result code.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports creating ExtSpecialController (null connection), Uri,
       std::vector<OperationStatement>, and ExecResultSet without initialization errors.
    2. The ExecuteBatch function of ExtSpecialController accepts vector<OperationStatement> and ExecResultSet
       as parameters and returns an int result code.
    3. A return value of -1 from ExecuteBatch indicates operation failure (as per test expectations).
 * @tc.step:
    1. Create an empty Uri object (uri) with no specific path or authority.
    2. Create an ExtSpecialController instance (tempExtSpeCon) using null as the DataShareConnection,
       the empty uri, and null as the third constructor parameter.
    3. Create an empty std::vector<OperationStatement> (statements) and an empty ExecResultSet (result1).
    4. Call tempExtSpeCon->ExecuteBatch(statements, result1) and record the returned int result.
    5. Check if the returned result matches the expected error code.
 * @tc.expect:
    1. The ExecuteBatch function returns a result code of -1.
 */
HWTEST_F(ControllerTest, ControllerTest_ExtSpecialControllerExecuteBatchTest_001, TestSize.Level0)
{
    LOG_INFO("ControllerTest_ExtSpecialControllerExecuteBatchTest_001::Start");
    Uri uri("");
    std::shared_ptr<DataShare::ExtSpecialController> tempExtSpeCon =
        std::make_shared<DataShare::ExtSpecialController>(nullptr, uri, nullptr);
    std::vector<DataShare::OperationStatement> statements;
    std::vector<DataShare::BatchUpdateResult> results;
    ExecResultSet result1;
    int result = tempExtSpeCon->ExecuteBatch(statements, result1);
    EXPECT_EQ(result, -1);
    LOG_INFO("ControllerTest_ExtSpecialControllerExecuteBatchTest_001::End");
}

/**
 * @tc.name: ControllerTest_ExtSpecialControllerExecuteBatchTest_002
 * @tc.desc: Verify the behavior of the ExecuteBatch operation in ExtSpecialController when using a valid
 *           DataShareConnection that is in an unconnected state.
 * @tc.type: FUNC
 * @tc.precon:
    1. The test environment supports instantiation of DataShareConnection (with empty URI) and ExtSpecialController.
    2. The ExtSpecialController can be initialized with a DataShareConnection, empty URI, and null parameters.
    3. The ExecuteBatch method accepts std::vector<OperationStatement> and ExecResultSet as input parameters
       and returns an integer result.
 * @tc.step:
    1. Create a DataShareConnection instance with an empty URI ("") and a null pointer.
    2. Create an ExtSpecialController instance using the created DataShareConnection, empty URI, and null pointer.
    3. Prepare an empty std::vector<OperationStatement> (statements) and an ExecResultSet (result1).
    4. Call the ExecuteBatch method of the ExtSpecialController, passing 'statements' and result1.
    5. Check the integer return value of the ExecuteBatch operation.
 * @tc.expect:
    1. The ExecuteBatch operation returns -1.
 */
HWTEST_F(ControllerTest, ControllerTest_ExtSpecialControllerExecuteBatchTest_002, TestSize.Level0)
{
    LOG_INFO("ControllerTest_ExtSpecialControllerExecuteBatchTest_002::Start");
    Uri uri("");
    sptr<DataShare::DataShareConnection> connection =
        new (std::nothrow) DataShare::DataShareConnection(uri, nullptr);
    auto dataShareConnection =
        std::shared_ptr<DataShare::DataShareConnection>(connection.GetRefPtr(), [holder = connection](const auto *) {
            holder->DisconnectDataShareExtAbility();
        });
    std::shared_ptr<DataShare::ExtSpecialController> tempExtSpeCon =
        std::make_shared<DataShare::ExtSpecialController>(dataShareConnection, uri, nullptr);
    std::vector<DataShare::OperationStatement> statements;
    std::vector<DataShare::BatchUpdateResult> results;
    ExecResultSet result1;
    int result = tempExtSpeCon->ExecuteBatch(statements, result1);
    EXPECT_EQ(result, -1);
    LOG_INFO("ControllerTest_ExtSpecialControllerExecuteBatchTest_002::End");
}

/**
 * @tc.name: ControllerTest_ExtSpecialControllerGetFileTypesTest_001
 * @tc.desc: Verify the behavior of the GetFileTypes operation in ExtSpecialController when using a null
 *           DataShareConnection.
 * @tc.type: FUNC
 * @tc.precon:
    1. The test environment supports instantiation of ExtSpecialController with a null connection, empty URI,
       and null parameters.
    2. The GetFileTypes method accepts a Uri and std::string (type) as input and returns a std::vector<std::string>.
 * @tc.step:
    1. Create an ExtSpecialController instance with a null DataShareConnection, empty URI (""), and null pointer.
    2. Define a test type string ("test001") and use an empty URI as input parameters.
    3. Call the GetFileTypes method of the ExtSpecialController, passing the empty URI and test type string.
    4. Check the returned std::vector<std::string> from the GetFileTypes operation.
 * @tc.expect:
    1. The GetFileTypes operation returns an empty std::vector<std::string>.
 */
HWTEST_F(ControllerTest, ControllerTest_ExtSpecialControllerGetFileTypesTest_001, TestSize.Level0)
{
    LOG_INFO("ControllerTest_ExtSpecialControllerGetFileTypesTest_001::Start");
    Uri uri("");
    std::shared_ptr<DataShare::ExtSpecialController> tempExtSpeCon =
        std::make_shared<DataShare::ExtSpecialController>(nullptr, uri, nullptr);
    std::string getFileTypes = "test001";
    std::vector<std::string> result = tempExtSpeCon->GetFileTypes(uri, getFileTypes);
    EXPECT_EQ(result, std::vector<std::string>());
    LOG_INFO("ControllerTest_ExtSpecialControllerGetFileTypesTest_001::End");
}

/**
 * @tc.name: ControllerTest_ExtSpecialControlleGetFileTypesTest_002
 * @tc.desc: Verify the behavior of the GetFileTypes operation in ExtSpecialController when using a valid
 *           DataShareConnection that is in an unconnected state.
 * @tc.type: FUNC
 * @tc.precon:
    1. The test environment supports instantiation of DataShareConnection (with empty URI) and ExtSpecialController.
    2. The ExtSpecialController can be initialized with a DataShareConnection, empty URI, and null parameters.
    3. The GetFileTypes method accepts a Uri and std::string (type) as input and returns a std::vector<std::string>.
 * @tc.step:
    1. Create a DataShareConnection instance with an empty URI ("") and a null pointer.
    2. Create an ExtSpecialController instance using the created DataShareConnection, empty URI, and null pointer.
    3. Define a test type string ("test001") and use an empty URI as input parameters.
    4. Call the GetFileTypes method of the ExtSpecialController, passing the empty URI and test type string.
    5. Check the returned std::vector<std::string> from the GetFileTypes operation.
 * @tc.expect:
    1. The GetFileTypes operation returns an empty std::vector<std::string>.
 */
HWTEST_F(ControllerTest, ControllerTest_ExtSpecialControlleGetFileTypesTest_002, TestSize.Level0)
{
    LOG_INFO("ControllerTest_ExtSpecialControllerGetFileTypesTest_002::Start");
    Uri uri("");
    sptr<DataShare::DataShareConnection> connection =
        new (std::nothrow) DataShare::DataShareConnection(uri, nullptr);
    auto dataShareConnection =
        std::shared_ptr<DataShare::DataShareConnection>(connection.GetRefPtr(), [holder = connection](const auto *) {
            holder->DisconnectDataShareExtAbility();
        });
    std::shared_ptr<DataShare::ExtSpecialController> tempExtSpeCon =
        std::make_shared<DataShare::ExtSpecialController>(dataShareConnection, uri, nullptr);
    std::string getFileTypes = "test001";
    std::vector<std::string> result = tempExtSpeCon->GetFileTypes(uri, getFileTypes);
    EXPECT_EQ(result, std::vector<std::string>());
    LOG_INFO("ControllerTest_ExtSpecialControllerGetFileTypesTest_002::End");
}

/**
 * @tc.name: ControllerTest_ExtSpecialControllerNormalizeUriTest_001
 * @tc.desc: Verify the behavior of the NormalizeUri operation in ExtSpecialController when using a null
 *           DataShareConnection.
 * @tc.type: FUNC
 * @tc.precon:
    1. The test environment supports instantiation of ExtSpecialController with a null connection, empty URI,
       and null parameters.
    2. The NormalizeUri method accepts a Uri as input and returns a Uri.
 * @tc.step:
    1. Create an ExtSpecialController instance with a null DataShareConnection, empty URI (""), and null pointer.
    2. Use an empty URI ("") as the input parameter for the NormalizeUri method.
    3. Call the NormalizeUri method of the ExtSpecialController, passing the empty URI.
    4. Check the returned Uri from the NormalizeUri operation.
 * @tc.expect:
    1. The NormalizeUri operation returns an empty URI ("").
 */
HWTEST_F(ControllerTest, ControllerTest_ExtSpecialControllerNormalizeUriTest_001, TestSize.Level0)
{
    LOG_INFO("ControllerTest_ExtSpecialControllerNormalizeUriTest_001::Start");
    Uri uri("");
    std::shared_ptr<DataShare::ExtSpecialController> tempExtSpeCon =
        std::make_shared<DataShare::ExtSpecialController>(nullptr, uri, nullptr);
    Uri result = tempExtSpeCon->NormalizeUri(uri);
    EXPECT_EQ(result, Uri(""));
    LOG_INFO("ControllerTest_ExtSpecialControllerNormalizeUriTest_001::End");
}

/**
 * @tc.name: ControllerTest_ExtSpecialControllerNormalizeUriTest_002
 * @tc.desc: Verify the behavior of the NormalizeUri operation in ExtSpecialController when using a valid
 *           DataShareConnection that is in an unconnected state.
 * @tc.type: FUNC
 * @tc.precon:
    1. The test environment supports instantiation of DataShareConnection (with empty URI) and ExtSpecialController.
    2. The ExtSpecialController can be initialized with a DataShareConnection, empty URI, and null parameters.
    3. The NormalizeUri method accepts a Uri as input and returns a Uri.
 * @tc.step:
    1. Create a DataShareConnection instance with an empty URI ("") and a null pointer.
    2. Create an ExtSpecialController instance using the created DataShareConnection, empty URI, and null pointer.
    3. Use an empty URI ("") as the input parameter for the NormalizeUri method.
    4. Call the NormalizeUri method of the ExtSpecialController, passing the empty URI.
    5. Check the returned Uri from the NormalizeUri operation.
 * @tc.expect:
    1. The NormalizeUri operation returns an empty URI ("").
 */
HWTEST_F(ControllerTest, ControllerTest_ExtSpecialControllerNormalizeUriTest_002, TestSize.Level0)
{
    LOG_INFO("ControllerTest_ExtSpecialControllerNormalizeUriTest_002::Start");
    Uri uri("");
    sptr<DataShare::DataShareConnection> connection =
        new (std::nothrow) DataShare::DataShareConnection(uri, nullptr);
    auto dataShareConnection =
        std::shared_ptr<DataShare::DataShareConnection>(connection.GetRefPtr(), [holder = connection](const auto *) {
            holder->DisconnectDataShareExtAbility();
        });
    std::shared_ptr<DataShare::ExtSpecialController> tempExtSpeCon =
        std::make_shared<DataShare::ExtSpecialController>(dataShareConnection, uri, nullptr);
    Uri result = tempExtSpeCon->NormalizeUri(uri);
    EXPECT_EQ(result, Uri(""));
    LOG_INFO("ControllerTest_ExtSpecialControllerNormalizeUriTest_002::End");
}

/**
 * @tc.name: ControllerTest_ExtSpecialControllerDenormalizeUriTest_001
 * @tc.desc: Verify the behavior of the DenormalizeUri operation in ExtSpecialController when using a null
 *           DataShareConnection.
 * @tc.type: FUNC
 * @tc.precon:
    1. The test environment supports instantiation of ExtSpecialController with a null connection, empty URI,
       and null parameters.
    2. The DenormalizeUri method accepts a Uri as input and returns a Uri.
 * @tc.step:
    1. Create an ExtSpecialController instance with a null DataShareConnection, empty URI (""), and null pointer.
    2. Use an empty URI ("") as the input parameter for the DenormalizeUri method.
    3. Call the DenormalizeUri method of the ExtSpecialController, passing the empty URI.
    4. Check the returned Uri from the DenormalizeUri operation.
 * @tc.expect:
    1. The DenormalizeUri operation returns an empty URI ("").
 */
HWTEST_F(ControllerTest, ControllerTest_ExtSpecialControllerDenormalizeUriTest_001, TestSize.Level0)
{
    LOG_INFO("ControllerTest_ExtSpecialControllerDenormalizeUriTest_001::Start");
    Uri uri("");
    std::shared_ptr<DataShare::ExtSpecialController> tempExtSpeCon =
        std::make_shared<DataShare::ExtSpecialController>(nullptr, uri, nullptr);
    Uri result = tempExtSpeCon->DenormalizeUri(uri);
    EXPECT_EQ(result, Uri(""));
    LOG_INFO("ControllerTest_ExtSpecialControllerDenormalizeUriTest_001::End");
}

/**
 * @tc.name: ControllerTest_ExtSpecialControllerDenormalizeUriTest_002
 * @tc.desc: Verify the behavior of the DenormalizeUri operation in ExtSpecialController when using a valid
 *           DataShareConnection that is in an unconnected state.
 * @tc.type: FUNC
 * @tc.precon:
    1. The test environment supports instantiation of DataShareConnection (with empty URI) and ExtSpecialController.
    2. The ExtSpecialController can be initialized with a DataShareConnection, empty URI, and null parameters.
    3. The DenormalizeUri method accepts a Uri as input and returns a Uri.
 * @tc.step:
    1. Create a DataShareConnection instance with an empty URI ("") and a null pointer.
    2. Create an ExtSpecialController instance using the created DataShareConnection, empty URI, and null pointer.
    3. Use an empty URI ("") as the input parameter for the DenormalizeUri method.
    4. Call the DenormalizeUri method of the ExtSpecialController, passing the empty URI.
    5. Check the returned Uri from the DenormalizeUri operation.
 * @tc.expect:
    1. The DenormalizeUri operation returns an empty URI ("").
 */
HWTEST_F(ControllerTest, ControllerTest_ExtSpecialControllerDenormalizeUriTest_002, TestSize.Level0)
{
    LOG_INFO("ControllerTest_ExtSpecialControllerDenormalizeUriTest_002::Start");
    Uri uri("");
    sptr<DataShare::DataShareConnection> connection =
        new (std::nothrow) DataShare::DataShareConnection(uri, nullptr);
    auto dataShareConnection =
        std::shared_ptr<DataShare::DataShareConnection>(connection.GetRefPtr(), [holder = connection](const auto *) {
            holder->DisconnectDataShareExtAbility();
        });
    std::shared_ptr<DataShare::ExtSpecialController> tempExtSpeCon =
        std::make_shared<DataShare::ExtSpecialController>(dataShareConnection, uri, nullptr);
    Uri result = tempExtSpeCon->DenormalizeUri(uri);
    EXPECT_EQ(result, Uri(""));
    LOG_INFO("ControllerTest_ExtSpecialControllerDenormalizeUriTest_002::End");
}
} // namespace DataShare
} // namespace OHOS
