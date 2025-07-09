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

HWTEST_F(ControllerTest, Generalcontroller_ServiceImplUpdateExTest_002, TestSize.Level0)
{
    LOG_INFO("Generalcontroller_ServiceImplUpdateExTest_001::Start");
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
* @tc.desc: Fill the branch connection == nullptr and proxy == nullptr
* @tc.type: FUNC
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
* @tc.desc: Fill the branch connection == nullptr and proxy == nullptr
* @tc.type: FUNC
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
* @tc.desc: Fill the branch connection == nullptr and proxy == nullptr
* @tc.type: FUNC
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
* @tc.desc: test ServiceImpl RegisterObserverExtProvider func
* @tc.type: FUNC
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
* @tc.desc: test ServiceImpl UnregisterObserverExtProvider func
* @tc.type: FUNC
*/
HWTEST_F(ControllerTest, Generalcontroller_ServiceImpl_UnregisterObserverExtProvider_Test_001, TestSize.Level0)
{
    LOG_INFO("Generalcontroller_ServiceImpl_RegisterObserverExtProvider_Test_001::Start");

    Uri uri("");
    std::string extUri = "GeneralControllerServiceImpl";

    std::shared_ptr<DataShare::GeneralController> tempGenConProImp =
        std::make_shared<DataShare::GeneralControllerServiceImpl>(extUri);
    ASSERT_NE(tempGenConProImp, nullptr);
    sptr<AAFwk::IDataAbilityObserver> dataObserver;

    int ret = tempGenConProImp->UnregisterObserverExtProvider(uri, dataObserver);
    EXPECT_EQ(ret, -1);

    LOG_INFO("Generalcontroller_ServiceImpl_RegisterObserverExtProvider_Test_001::End");
}

/**
* @tc.name: Generalcontroller_ServiceImpl_NotifyChangeExtProvider_Test_001
* @tc.desc: test ServiceImpl NotifyChangeExtProvider func
* @tc.type: FUNC
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
* @tc.desc: test OpenFileWithErrCode func
* @tc.type: FUNC
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
* @tc.desc: test OpenFileWithErrCode func
* @tc.type: FUNC
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
