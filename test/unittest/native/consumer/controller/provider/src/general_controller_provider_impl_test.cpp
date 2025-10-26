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