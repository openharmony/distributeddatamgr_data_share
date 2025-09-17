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
#define LOG_TAG "general_controller_provider_impl_test"

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

class GeneralControllerProviderImplTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void GeneralControllerProviderImplTest::SetUpTestCase(void) {}
void GeneralControllerProviderImplTest::TearDownTestCase(void) {}
void GeneralControllerProviderImplTest::SetUp(void) {}
void GeneralControllerProviderImplTest::TearDown(void) {}

/**
* @tc.name: ProviderImplInsertTest001
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
HWTEST_F(GeneralControllerProviderImplTest, ProviderImplInsertTest001, TestSize.Level0)
{
    LOG_INFO("GeneralControllerProviderImplTest ProviderImplInsertTest001::Start");
    Uri uri("");
    std::shared_ptr<DataShare::GeneralController> tempGenConProImp =
        std::make_shared<DataShare::GeneralControllerProviderImpl>(nullptr, uri, nullptr);
    DataShare::DataShareValuesBucket valuesBucket;
    double valueD1 = 20.07;
    valuesBucket.Put("phoneNumber", valueD1);
    int result = tempGenConProImp->Insert(uri, valuesBucket);
    EXPECT_EQ((result < 0), true);
    LOG_INFO("GeneralControllerProviderImplTest ProviderImplInsertTest001::End");
}

/**
* @tc.name: ProviderImplInsertTest002
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
HWTEST_F(GeneralControllerProviderImplTest, ProviderImplInsertTest002, TestSize.Level0)
{
    LOG_INFO("GeneralControllerProviderImplTest ProviderImplInsertTest002::Start");
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
    LOG_INFO("GeneralControllerProviderImplTest ProviderImplInsertTest002::End");
}

/**
* @tc.name: ProviderImplUpdateTest001
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
HWTEST_F(GeneralControllerProviderImplTest, ProviderImplUpdateTest001, TestSize.Level0)
{
    LOG_INFO("GeneralControllerProviderImplTest ProviderImplUpdateTest001::Start");
    Uri uri("");
    std::shared_ptr<DataShare::GeneralController> tempGenConProImp =
        std::make_shared<DataShare::GeneralControllerProviderImpl>(nullptr, uri, nullptr);
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo("name", "Controller_Test001");
    DataShare::DataShareValuesBucket valuesBucket;
    valuesBucket.Put("name", "Controller_Test002");
    int result = tempGenConProImp->Update(uri, predicates, valuesBucket);
    EXPECT_EQ((result < 0), true);
    LOG_INFO("GeneralControllerProviderImplTest ProviderImplUpdateTest001::End");
}

/**
* @tc.name: ProviderImplUpdateTest002
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
HWTEST_F(GeneralControllerProviderImplTest, ProviderImplUpdateTest002, TestSize.Level0)
{
    LOG_INFO("GeneralControllerProviderImplTest ProviderImplUpdateTest002::Start");
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
    LOG_INFO("GeneralControllerProviderImplTest ProviderImplUpdateTest002::End");
}

/**
* @tc.name: ProviderImplDeleteTest001
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
HWTEST_F(GeneralControllerProviderImplTest, ProviderImplDeleteTest001, TestSize.Level0)
{
    LOG_INFO("GeneralControllerProviderImplTest ProviderImplDeleteTest001::Start");
    Uri uri("");
    std::shared_ptr<DataShare::GeneralController> tempGenConProImp =
        std::make_shared<DataShare::GeneralControllerProviderImpl>(nullptr, uri, nullptr);
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo("name", "Controller_Test001");
    int result = tempGenConProImp->Delete(uri, predicates);
    EXPECT_EQ((result < 0), true);
    LOG_INFO("GeneralControllerProviderImplTest ProviderImplDeleteTest001::End");
}

/**
* @tc.name: ProviderImplDeleteTest002
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
HWTEST_F(GeneralControllerProviderImplTest, ProviderImplDeleteTest002, TestSize.Level0)
{
    LOG_INFO("GeneralControllerProviderImplTest ProviderImplDeleteTest002::Start");
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
    LOG_INFO("GeneralControllerProviderImplTest ProviderImplDeleteTest002::End");
}

/**
* @tc.name: ServiceImplInsertExTest001
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
HWTEST_F(GeneralControllerProviderImplTest, ServiceImplInsertExTest001, TestSize.Level0)
{
    LOG_INFO("GeneralControllerProviderImplTest ServiceImplInsertExTest001::Start");
    Uri uri("");
    std::shared_ptr<DataShare::GeneralController> tempGenConProImp =
        std::make_shared<DataShare::GeneralControllerProviderImpl>(nullptr, uri, nullptr);
    DataShare::DataShareValuesBucket valuesBucket;
    double valueD1 = 20.07;
    valuesBucket.Put("phoneNumber", valueD1);
    std::pair result = tempGenConProImp->InsertEx(uri, valuesBucket);
    EXPECT_EQ(result.first, DATA_SHARE_ERROR);
    EXPECT_EQ(result.second, 0);
    LOG_INFO("GeneralControllerProviderImplTest ServiceImplInsertExTest001::End");
}

/**
* @tc.name: ServiceImplInsertExTest002
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
HWTEST_F(GeneralControllerProviderImplTest, ServiceImplInsertExTest002, TestSize.Level0)
{
    LOG_INFO("GeneralControllerProviderImplTest ServiceImplInsertExTest002::Start");
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
    LOG_INFO("GeneralControllerProviderImplTest ServiceImplInsertExTest002::End");
}

/**
* @tc.name: ServiceImplUpdateExTest001
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
HWTEST_F(GeneralControllerProviderImplTest, ServiceImplUpdateExTest001, TestSize.Level0)
{
    LOG_INFO("GeneralControllerProviderImplTest ServiceImplUpdateExTest001::Start");
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
    LOG_INFO("GeneralControllerProviderImplTest ServiceImplUpdateExTest001::End");
}

/**
* @tc.name: ServiceImplUpdateExTest002
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
HWTEST_F(GeneralControllerProviderImplTest, ServiceImplUpdateExTest002, TestSize.Level0)
{
    LOG_INFO("GeneralControllerProviderImplTest ServiceImplUpdateExTest002::Start");
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
    LOG_INFO("GeneralControllerProviderImplTest ServiceImplUpdateExTest002::End");
}

/**
* @tc.name: ServiceImplDeleteExTest001
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
HWTEST_F(GeneralControllerProviderImplTest, ServiceImplDeleteExTest001, TestSize.Level0)
{
    LOG_INFO("GeneralControllerProviderImplTest ServiceImplDeleteExTest001::Start");
    Uri uri("");
    std::shared_ptr<DataShare::GeneralController> tempGenConProImp =
        std::make_shared<DataShare::GeneralControllerProviderImpl>(nullptr, uri, nullptr);
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo("name", "Controller_Test001");
    std::pair result = tempGenConProImp->DeleteEx(uri, predicates);
    EXPECT_EQ(result.first, DATA_SHARE_ERROR);
    EXPECT_EQ(result.second, 0);
    LOG_INFO("GeneralControllerProviderImplTest ServiceImplDeleteExTest001::End");
}

/**
* @tc.name: ServiceImplDeleteExTest002
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
HWTEST_F(GeneralControllerProviderImplTest, ServiceImplDeleteExTest002, TestSize.Level0)
{
    LOG_INFO("GeneralControllerProviderImplTest ServiceImplDeleteExTest002::Start");
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
    LOG_INFO("GeneralControllerProviderImplTest ServiceImplDeleteExTest002::End");
}

/**
* @tc.name: ProviderImplQueryTest001
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
HWTEST_F(GeneralControllerProviderImplTest, ProviderImplQueryTest001, TestSize.Level0)
{
    LOG_INFO("GeneralControllerProviderImplTest ProviderImplQueryTest001::Start");
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
    LOG_INFO("GeneralControllerProviderImplTest ProviderImplQueryTest001::End");
}

/**
* @tc.name: ProviderImplQueryTest002
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
HWTEST_F(GeneralControllerProviderImplTest, ProviderImplQueryTest002, TestSize.Level0)
{
    LOG_INFO("GeneralControllerProviderImplTest ProviderImplQueryTest002::Start");
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
    LOG_INFO("GeneralControllerProviderImplTest ProviderImplQueryTest002::End");
}

/**
* @tc.name: ServiceImplRegisterObserverTest001
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
HWTEST_F(GeneralControllerProviderImplTest, ServiceImplRegisterObserverTest001, TestSize.Level0)
{
    LOG_INFO("GeneralControllerProviderImplTest ServiceImplRegisterObserverTest001::Start");
    Uri uri("");
    std::shared_ptr<DataShare::GeneralController> tempGenConProImp =
        std::make_shared<DataShare::GeneralControllerProviderImpl>(nullptr, uri, nullptr);
    sptr<AAFwk::IDataAbilityObserver> dataObserver;
    tempGenConProImp->RegisterObserver(uri, dataObserver);
    EXPECT_EQ(uri, Uri(""));
    LOG_INFO("GeneralControllerProviderImplTest ServiceImplRegisterObserverTest001::End");
}

/**
* @tc.name: ServiceImplRegisterObserverTest002
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
HWTEST_F(GeneralControllerProviderImplTest, ServiceImplRegisterObserverTest002, TestSize.Level0)
{
    LOG_INFO("GeneralControllerProviderImplTest ServiceImplRegisterObserverTest002::Start");
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
    LOG_INFO("GeneralControllerProviderImplTest ServiceImplRegisterObserverTest002::End");
}

/**
* @tc.name: ServiceImplUnregisterObserverTest001
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
HWTEST_F(GeneralControllerProviderImplTest, ServiceImplUnregisterObserverTest001, TestSize.Level0)
{
    LOG_INFO("GeneralControllerProviderImplTest ServiceImplUnregisterObserverTest001::Start");
    Uri uri("");
    std::shared_ptr<DataShare::GeneralController> tempGenConProImp =
        std::make_shared<DataShare::GeneralControllerProviderImpl>(nullptr, uri, nullptr);
    sptr<AAFwk::IDataAbilityObserver> dataObserver;
    tempGenConProImp->UnregisterObserver(uri, dataObserver);
    EXPECT_EQ(uri, Uri(""));
    LOG_INFO("GeneralControllerProviderImplTest ServiceImplUnregisterObserverTest001::End");
}

/**
* @tc.name: ServiceImplUnregisterObserverTest002
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
HWTEST_F(GeneralControllerProviderImplTest, ServiceImplUnregisterObserverTest002, TestSize.Level0)
{
    LOG_INFO("GeneralControllerProviderImplTest ServiceImplUnregisterObserverTest002::Start");
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
    LOG_INFO("GeneralControllerProviderImplTest ServiceImplUnregisterObserverTest002::End");
}

/**
* @tc.name: ServiceImplNotifyChangeTest001
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
HWTEST_F(GeneralControllerProviderImplTest, ServiceImplNotifyChangeTest001, TestSize.Level0)
{
    LOG_INFO("GeneralControllerProviderImplTest ServiceImplNotifyChangeTest001::Start");
    Uri uri("");
    std::shared_ptr<DataShare::GeneralController> tempGenConProImp =
        std::make_shared<DataShare::GeneralControllerProviderImpl>(nullptr, uri, nullptr);
    tempGenConProImp->NotifyChange(uri);
    EXPECT_EQ(uri, Uri(""));
    LOG_INFO("GeneralControllerProviderImplTest ServiceImplNotifyChangeTest001::End");
}

/**
* @tc.name: ServiceImplNotifyChangeTest002
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
HWTEST_F(GeneralControllerProviderImplTest, ServiceImplNotifyChangeTest002, TestSize.Level0)
{
    LOG_INFO("GeneralControllerProviderImplTest ServiceImplNotifyChangeTest002::Start");
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
    LOG_INFO("GeneralControllerProviderImplTest ServiceImplNotifyChangeTest002::End");
}

/**
* @tc.name: RegisterObserverExtProviderTest001
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
HWTEST_F(GeneralControllerProviderImplTest, RegisterObserverExtProviderTest001, TestSize.Level0)
{
    LOG_INFO("GeneralControllerProviderImplTest RegisterObserverExtProviderTest001::Start");

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

    LOG_INFO("GeneralControllerProviderImplTest RegisterObserverExtProviderTest001::End");
}

/**
* @tc.name: UnregisterObserverExtProviderTest001
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
HWTEST_F(GeneralControllerProviderImplTest, UnregisterObserverExtProviderTest001, TestSize.Level0)
{
    LOG_INFO("GeneralControllerProviderImplTest UnregisterObserverExtProviderTest001::Start");

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

    LOG_INFO("GeneralControllerProviderImplTest UnregisterObserverExtProviderTest001::End");
}
}
}