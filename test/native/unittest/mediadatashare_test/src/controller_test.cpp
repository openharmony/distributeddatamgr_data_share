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
#include "datashare_helper.h"
#include "datashare_log.h"
#include "ext_special_controller.h"
#include "extension_manager_proxy.h"
#include "general_controller.h"
#include "general_controller_provider_impl.h"
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
    auto result = tempGenConProImp->Query(uri, predicates, columns, error);
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
    auto result = tempGenConProImp->Query(uri, predicates, columns, error);
    EXPECT_EQ(result, nullptr);
    LOG_INFO("ControllerTest_ProviderImplQueryTest_002::End");
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
