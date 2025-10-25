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
* @tc.desc: Verify Insert operation with null connection in GeneralControllerProviderImpl
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Create GeneralControllerProviderImpl with null connection and empty URI
    2. Prepare DataShareValuesBucket with test data
    3. Call Insert method with empty URI and values bucket
    4. Check if returned result is negative (indicating failure)
* @tc.expect:
    1. Insert operation returns negative value
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
* @tc.desc: Verify Insert operation with valid connection but unconnected state in GeneralControllerProviderImpl
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Create DataShareConnection with empty URI
    2. Create GeneralControllerProviderImpl with the connection and empty URI
    3. Prepare DataShareValuesBucket with test data
    4. Call Insert method with empty URI and values bucket
    5. Check if returned result is negative (indicating failure)
* @tc.expect:
    1. Insert operation returns negative value
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
* @tc.desc: Verify Update operation with null connection in GeneralControllerProviderImpl
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Create GeneralControllerProviderImpl with null connection and empty URI
    2. Prepare DataSharePredicates and DataShareValuesBucket with test data
    3. Call Update method with empty URI, predicates and values bucket
    4. Check if returned result is negative (indicating failure)
* @tc.expect:
    1. Update operation returns negative value
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
* @tc.desc: Verify Update operation with valid connection but unconnected state in GeneralControllerProviderImpl
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Create DataShareConnection with empty URI
    2. Create GeneralControllerProviderImpl with the connection and empty URI
    3. Prepare DataSharePredicates and DataShareValuesBucket with test data
    4. Call Update method with empty URI, predicates and values bucket
    5. Check if returned result is negative (indicating failure)
* @tc.expect:
    1. Update operation returns negative value
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
* @tc.desc: Verify Delete operation with null connection in GeneralControllerProviderImpl
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Create GeneralControllerProviderImpl with null connection and empty URI
    2. Prepare DataSharePredicates with test conditions
    3. Call Delete method with empty URI and predicates
    4. Check if returned result is negative (indicating failure)
* @tc.expect:
    1. Delete operation returns negative value
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
* @tc.desc: Verify Delete operation with valid connection but unconnected state in GeneralControllerProviderImpl
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Create DataShareConnection with empty URI
    2. Create GeneralControllerProviderImpl with the connection and empty URI
    3. Prepare DataSharePredicates with test conditions
    4. Call Delete method with empty URI and predicates
    5. Check if returned result is negative (indicating failure)
* @tc.expect:
    1. Delete operation returns negative value
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
* @tc.desc: Verify InsertEx operation with null connection in GeneralControllerProviderImpl
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Create GeneralControllerProviderImpl with null connection and empty URI
    2. Prepare DataShareValuesBucket with test data
    3. Call InsertEx method with empty URI and values bucket
    4. Check if returned result is (DATA_SHARE_ERROR, 0)
* @tc.expect:
    1. InsertEx operation returns pair(DATA_SHARE_ERROR, 0)
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
* @tc.desc: Verify InsertEx operation with valid connection but unconnected state in GeneralControllerProviderImpl
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Create DataShareConnection with empty URI
    2. Create GeneralControllerProviderImpl with the connection and empty URI
    3. Prepare DataShareValuesBucket with test data
    4. Call InsertEx method with empty URI and values bucket
    5. Check if returned result is (DATA_SHARE_ERROR, 0)
* @tc.expect:
    1. InsertEx operation returns pair(DATA_SHARE_ERROR, 0)
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
* @tc.desc: Verify UpdateEx operation with null connection in GeneralControllerProviderImpl
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Create GeneralControllerProviderImpl with null connection and empty URI
    2. Prepare DataSharePredicates and DataShareValuesBucket with test data
    3. Call UpdateEx method with empty URI, predicates and values bucket
    4. Check if returned result is (DATA_SHARE_ERROR, 0)
* @tc.expect:
    1. UpdateEx operation returns pair(DATA_SHARE_ERROR, 0)
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
* @tc.desc: Verify UpdateEx operation with valid connection but unconnected state in GeneralControllerProviderImpl
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Create DataShareConnection with empty URI
    2. Create GeneralControllerProviderImpl with the connection and empty URI
    3. Prepare DataSharePredicates and DataShareValuesBucket with test data
    4. Call UpdateEx method with empty URI, predicates and values bucket
    5. Check if returned result is (DATA_SHARE_ERROR, 0)
* @tc.expect:
    1. UpdateEx operation returns pair(DATA_SHARE_ERROR, 0)
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
* @tc.desc: Verify DeleteEx operation with null connection in GeneralControllerProviderImpl
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Create GeneralControllerProviderImpl with null connection and empty URI
    2. Prepare DataSharePredicates with test conditions
    3. Call DeleteEx method with empty URI and predicates
    4. Check if returned result is (DATA_SHARE_ERROR, 0)
* @tc.expect:
    1. DeleteEx operation returns pair(DATA_SHARE_ERROR, 0)
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
* @tc.desc: Verify DeleteEx operation with valid connection but unconnected state in GeneralControllerProviderImpl
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Create DataShareConnection with empty URI
    2. Create GeneralControllerProviderImpl with the connection and empty URI
    3. Prepare DataSharePredicates with test conditions
    4. Call DeleteEx method with empty URI and predicates
    5. Check if returned result is (DATA_SHARE_ERROR, 0)
* @tc.expect:
    1. DeleteEx operation returns pair(DATA_SHARE_ERROR, 0)
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
* @tc.desc: Verify Query operation with null connection in GeneralControllerProviderImpl
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Create GeneralControllerProviderImpl with null connection and empty URI
    2. Prepare DataSharePredicates and column list
    3. Call Query method with empty URI, predicates and columns
    4. Check if returned result is nullptr
* @tc.expect:
    1. Query operation returns nullptr
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
* @tc.desc: Verify Query operation with valid connection but unconnected state in GeneralControllerProviderImpl
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Create DataShareConnection with empty URI
    2. Create GeneralControllerProviderImpl with the connection and empty URI
    3. Prepare DataSharePredicates and column list
    4. Call Query method with empty URI, predicates and columns
    5. Check if returned result is nullptr
* @tc.expect:
    1. Query operation returns nullptr
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
* @tc.desc: Verify RegisterObserver operation with null connection in GeneralControllerProviderImpl
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Create GeneralControllerProviderImpl with null connection and empty URI
    2. Call RegisterObserver method with empty URI and null observer
    3. Verify URI remains empty
* @tc.expect:
    1. URI remains empty after operation
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
* @tc.desc: Verify RegisterObserver with valid connection but unconnected state in GeneralControllerProviderImpl
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Create DataShareConnection with empty URI
    2. Create GeneralControllerProviderImpl with the connection and empty URI
    3. Call RegisterObserver method with empty URI and null observer
    4. Verify URI remains empty
* @tc.expect:
    1. URI remains empty after operation
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
* @tc.desc: Verify UnregisterObserver operation with null connection in GeneralControllerProviderImpl
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Create GeneralControllerProviderImpl with null connection and empty URI
    2. Call UnregisterObserver method with empty URI and null observer
    3. Verify URI remains empty
* @tc.expect:
    1. URI remains empty after operation
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
* @tc.desc: Verify UnregisterObserver with valid connection but unconnected state in GeneralControllerProviderImpl
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Create DataShareConnection with empty URI
    2. Create GeneralControllerProviderImpl with the connection and empty URI
    3. Call UnregisterObserver method with empty URI and null observer
    4. Verify URI remains empty
* @tc.expect:
    1. URI remains empty after operation
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
* @tc.desc: Verify NotifyChange operation with null connection in GeneralControllerProviderImpl
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Create GeneralControllerProviderImpl with null connection and empty URI
    2. Call NotifyChange method with empty URI
    3. Verify URI remains empty
* @tc.expect:
    1. URI remains empty after operation
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
* @tc.desc: Verify NotifyChange with valid connection but unconnected state in GeneralControllerProviderImpl
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Create DataShareConnection with empty URI
    2. Create GeneralControllerProviderImpl with the connection and empty URI
    3. Call NotifyChange method with empty URI
    4. Verify URI remains empty
* @tc.expect:
    1. URI remains empty after operation
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
* @tc.desc: Verify RegisterObserverExtProvider with null connection and unconnected state
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Create provider with null connection and call RegisterObserverExtProvider
    2. Create provider with valid but unconnected connection and call RegisterObserverExtProvider
    3. Check return codes for both cases
* @tc.expect:
    1. First call returns E_PROVIDER_CONN_NULL
    2. Second call returns E_PROVIDER_NOT_CONNECTED
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
* @tc.desc: Verify UnregisterObserverExtProvider with null connection and unconnected state
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Create provider with null connection and call UnregisterObserverExtProvider
    2. Create provider with valid but unconnected connection and call UnregisterObserverExtProvider
    3. Check return codes for both cases
* @tc.expect:
    1. First call returns E_PROVIDER_CONN_NULL
    2. Second call returns E_PROVIDER_NOT_CONNECTED
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
* @tc.desc: Verify NotifyChangeExtProvider with null connection and unconnected state
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Create provider with null connection and call NotifyChangeExtProvider
    2. Create provider with valid but unconnected connection and call NotifyChangeExtProvider
    3. Check return codes for both cases
* @tc.expect:
    1. First call returns E_PROVIDER_CONN_NULL
    2. Second call returns E_PROVIDER_NOT_CONNECTED
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
* @tc.desc: Verify RegisterObserverExtProvider operation in GeneralControllerServiceImpl
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Create GeneralControllerServiceImpl with test URI
    2. Call RegisterObserverExtProvider with empty URI and null observer
    3. Check return code
* @tc.expect:
    1. Operation returns -1 (failure)
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
* @tc.desc: Verify UnregisterObserverExtProvider operation in GeneralControllerServiceImpl
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Create GeneralControllerServiceImpl with test URI
    2. Call UnregisterObserverExtProvider with empty URI and null observer
    3. Check return code
* @tc.expect:
    1. Operation returns -1 (failure)
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
* @tc.desc: Verify NotifyChangeExtProvider operation in GeneralControllerServiceImpl
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Create GeneralControllerServiceImpl with test URI
    2. Prepare ChangeInfo with empty URI
    3. Call NotifyChangeExtProvider with the change info
    4. Check return code
* @tc.expect:
    1. Operation returns -1 (failure)
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
* @tc.desc: Verify OpenFile operation with null connection in ExtSpecialController
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Create ExtSpecialController with null connection and empty URI
    2. Call OpenFile with empty URI and test mode
    3. Check if returned result is negative
* @tc.expect:
    1. OpenFile operation returns negative value
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
* @tc.desc: Verify OpenFile operation with valid connection but unconnected state in ExtSpecialController
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Create DataShareConnection with empty URI
    2. Create ExtSpecialController with the connection and empty URI
    3. Call OpenFile with empty URI and test mode
    4. Check if returned result is negative
* @tc.expect:
    1. OpenFile operation returns negative value
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
* @tc.desc: Verify OpenFileWithErrCode function with null connection in ExtSpecialController
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Create ExtSpecialController with null connection and empty URI
    2. Call OpenFileWithErrCode with empty URI, test mode and error code
    3. Check if returned file descriptor is -1
* @tc.expect:
    1. OpenFileWithErrCode returns -1
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
* @tc.desc: Verify OpenFileWithErrCode function with valid connection but unconnected state in ExtSpecialController
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Create DataShareConnection with empty URI
    2. Create ExtSpecialController with the connection and empty URI
    3. Call OpenFileWithErrCode with empty URI, test mode and error code
    4. Check if returned file descriptor is -1
* @tc.expect:
    1. OpenFileWithErrCode returns -1
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
* @tc.desc: Verify OpenRawFile operation with null connection in ExtSpecialController
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Create ExtSpecialController with null connection and empty URI
    2. Call OpenRawFile with empty URI and test mode
    3. Check if returned result is negative
* @tc.expect:
    1. OpenRawFile operation returns negative value
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
* @tc.desc: Verify OpenRawFile operation with valid connection but unconnected state in ExtSpecialController
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Create DataShareConnection with empty URI
    2. Create ExtSpecialController with the connection and empty URI
    3. Call OpenRawFile with empty URI and test mode
    4. Check if returned result is negative
* @tc.expect:
    1. OpenRawFile operation returns negative value
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
* @tc.desc: Verify GetType operation with null connection in ExtSpecialController
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Create ExtSpecialController with null connection and empty URI
    2. Call GetType with empty URI
    3. Check if returned result is empty string
* @tc.expect:
    1. GetType operation returns empty string
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
* @tc.desc: Verify GetType operation with valid connection but unconnected state in ExtSpecialController
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Create DataShareConnection with empty URI
    2. Create ExtSpecialController with the connection and empty URI
    3. Call GetType with empty URI
    4. Check if returned result is empty string
* @tc.expect:
    1. GetType operation returns empty string
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
* @tc.desc: Verify BatchInsert operation with null connection in ExtSpecialController
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Create ExtSpecialController with null connection and empty URI
    2. Prepare vector of DataShareValuesBucket with test data
    3. Call BatchInsert with empty URI and values vector
    4. Check if returned result is negative
* @tc.expect:
    1. BatchInsert operation returns negative value
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
* @tc.desc: Verify BatchInsert operation with valid connection but unconnected state in ExtSpecialController
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Create DataShareConnection with empty URI
    2. Create ExtSpecialController with the connection and empty URI
    3. Prepare vector of DataShareValuesBucket with test data
    4. Call BatchInsert with empty URI and values vector
    5. Check if returned result is negative
* @tc.expect:
    1. BatchInsert operation returns negative value
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
* @tc.desc: Verify BatchUpdate operation with null connection in ExtSpecialController
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Create ExtSpecialController with null connection and empty URI
    2. Prepare empty UpdateOperations and results vector
    3. Call BatchUpdate with operations and results
    4. Check if returned result is -1
* @tc.expect:
    1. BatchUpdate operation returns -1
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
* @tc.desc: Verify BatchUpdate operation with valid connection but unconnected state in ExtSpecialController
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Create DataShareConnection with empty URI
    2. Create ExtSpecialController with the connection and empty URI
    3. Prepare empty UpdateOperations and results vector
    4. Call BatchUpdate with operations and results
    5. Check if returned result is -1
* @tc.expect:
    1. BatchUpdate operation returns -1
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
* @tc.desc: Verify InsertExt operation with null connection in ExtSpecialController
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Create ExtSpecialController with null connection and empty URI
    2. Prepare DataShareValuesBucket with test data and result string
    3. Call InsertExt with empty URI, values bucket and result string
    4. Check if returned result is -1
* @tc.expect:
    1. InsertExt operation returns -1
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
* @tc.desc: Verify InsertExt operation with valid connection but unconnected state in ExtSpecialController
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Create DataShareConnection with empty URI
    2. Create ExtSpecialController with the connection and empty URI
    3. Prepare DataShareValuesBucket with test data and result string
    4. Call InsertExt with empty URI, values bucket and result string
    5. Check if returned result is -1
* @tc.expect:
    1. InsertExt operation returns -1
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
* @tc.desc: Verify ExecuteBatch operation with null connection in ExtSpecialController
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Create ExtSpecialController with null connection and empty URI
    2. Prepare empty OperationStatement vector and ExecResultSet
    3. Call ExecuteBatch with statements and result set
    4. Check if returned result is -1
* @tc.expect:
    1. ExecuteBatch operation returns -1
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
* @tc.desc: Verify ExecuteBatch operation with valid connection but unconnected state in ExtSpecialController
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Create DataShareConnection with empty URI
    2. Create ExtSpecialController with the connection and empty URI
    3. Prepare empty OperationStatement vector and ExecResultSet
    4. Call ExecuteBatch with statements and result set
    5. Check if returned result is -1
* @tc.expect:
    1. ExecuteBatch operation returns -1
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
* @tc.desc: Verify GetFileTypes operation with null connection in ExtSpecialController
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Create ExtSpecialController with null connection and empty URI
    2. Call GetFileTypes with empty URI and test type string
    3. Check if returned result is empty vector
* @tc.expect:
    1. GetFileTypes operation returns empty vector
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
* @tc.desc: Verify GetFileTypes operation with valid connection but unconnected state in ExtSpecialController
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Create DataShareConnection with empty URI
    2. Create ExtSpecialController with the connection and empty URI
    3. Call GetFileTypes with empty URI and test type string
    4. Check if returned result is empty vector
* @tc.expect:
    1. GetFileTypes operation returns empty vector
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
* @tc.desc: Verify NormalizeUri operation with null connection in ExtSpecialController
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Create ExtSpecialController with null connection and empty URI
    2. Call NormalizeUri with empty URI
    3. Check if returned result is empty URI
* @tc.expect:
    1. NormalizeUri operation returns empty URI
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
* @tc.desc: Verify NormalizeUri operation with valid connection but unconnected state in ExtSpecialController
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Create DataShareConnection with empty URI
    2. Create ExtSpecialController with the connection and empty URI
    3. Call NormalizeUri with empty URI
    4. Check if returned result is empty URI
* @tc.expect:
    1. NormalizeUri operation returns empty URI
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
* @tc.desc: Verify DenormalizeUri operation with null connection in ExtSpecialController
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Create ExtSpecialController with null connection and empty URI
    2. Call DenormalizeUri with empty URI
    3. Check if returned result is empty URI
* @tc.expect:
    1. DenormalizeUri operation returns empty URI
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
* @tc.desc: Verify DenormalizeUri operation with valid connection but unconnected state in ExtSpecialController
* @tc.type: FUNC
* @tc.precon: None
* @tc.step:
    1. Create DataShareConnection with empty URI
    2. Create ExtSpecialController with the connection and empty URI
    3. Call DenormalizeUri with empty URI
    4. Check if returned result is empty URI
* @tc.expect:
    1. DenormalizeUri operation returns empty URI
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
