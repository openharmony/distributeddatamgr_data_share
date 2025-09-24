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
#define LOG_TAG "ext_special_controller_test"
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

class ExtSpecialControllerTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void ExtSpecialControllerTest::SetUpTestCase(void) {}
void ExtSpecialControllerTest::TearDownTestCase(void) {}
void ExtSpecialControllerTest::SetUp(void) {}
void ExtSpecialControllerTest::TearDown(void) {}

/**
* @tc.name: OpenFileTest_001
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
HWTEST_F(ExtSpecialControllerTest, OpenFileTest_001, TestSize.Level0)
{
    LOG_INFO("ExtSpecialControllerTest OpenFileTest_001::Start");
    Uri uri("");
    std::shared_ptr<DataShare::ExtSpecialController> tempExtSpeCon =
        std::make_shared<DataShare::ExtSpecialController>(nullptr, uri, nullptr);
    std::string mode = "test001";
    int result = tempExtSpeCon->OpenFile(uri, mode);
    EXPECT_EQ((result < 0), true);
    LOG_INFO("ExtSpecialControllerTest OpenFileTest_001::End");
}

/**
* @tc.name: OpenFileTest_002
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
HWTEST_F(ExtSpecialControllerTest, OpenFileTest_002, TestSize.Level0)
{
    LOG_INFO("ExtSpecialControllerTest OpenFileTest_002::Start");
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
    LOG_INFO("ExtSpecialControllerTest OpenFileTest_002::End");
}

/**
* @tc.name: OpenFileWithErrCodeTest_001
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
HWTEST_F(ExtSpecialControllerTest, OpenFileWithErrCodeTest_001, TestSize.Level0)
{
    LOG_INFO("ExtSpecialControllerTest OpenFileWithErrCodeTest_001::Start");
    Uri uri("");
    std::shared_ptr<DataShare::ExtSpecialController> tempExtSpeCon =
        std::make_shared<DataShare::ExtSpecialController>(nullptr, uri, nullptr);
    std::string mode = "test001";
    int32_t errCode = 0;
    int fd = tempExtSpeCon->OpenFileWithErrCode(uri, mode, errCode);
    EXPECT_EQ(fd, -1);
    LOG_INFO("ExtSpecialControllerTest OpenFileWithErrCodeTest_001::End");
}

/**
* @tc.name: OpenFileWithErrCodeTest_002
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
HWTEST_F(ExtSpecialControllerTest, OpenFileWithErrCodeTest_002, TestSize.Level0)
{
    LOG_INFO("ExtSpecialControllerTest OpenFileWithErrCodeTest_002::Start");
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
    LOG_INFO("ExtSpecialControllerTest OpenFileWithErrCodeTest_002::End");
}

/**
* @tc.name: OpenRawFileTest_001
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
HWTEST_F(ExtSpecialControllerTest, OpenRawFileTest_001, TestSize.Level0)
{
    LOG_INFO("ExtSpecialControllerTest OpenRawFileTest_001::Start");
    Uri uri("");
    std::shared_ptr<DataShare::ExtSpecialController> tempExtSpeCon =
        std::make_shared<DataShare::ExtSpecialController>(nullptr, uri, nullptr);
    std::string mode = "test001";
    int result = tempExtSpeCon->OpenRawFile(uri, mode);
    EXPECT_EQ((result < 0), true);
    LOG_INFO("ExtSpecialControllerTest OpenRawFileTest_001::End");
}

/**
* @tc.name: OpenRawFileTest_002
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
HWTEST_F(ExtSpecialControllerTest, OpenRawFileTest_002, TestSize.Level0)
{
    LOG_INFO("ExtSpecialControllerTest OpenRawFileTest_002::Start");
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
    LOG_INFO("ExtSpecialControllerTest OpenRawFileTest_002::End");
}

/**
* @tc.name: GetTypeTest_001
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
HWTEST_F(ExtSpecialControllerTest, GetTypeTest_001, TestSize.Level0)
{
    LOG_INFO("ExtSpecialControllerTest GetTypeTest_001::Start");
    Uri uri("");
    std::shared_ptr<DataShare::ExtSpecialController> tempExtSpeCon =
        std::make_shared<DataShare::ExtSpecialController>(nullptr, uri, nullptr);
    std::string result = tempExtSpeCon->GetType(uri);
    EXPECT_EQ(result, "");
    LOG_INFO("ExtSpecialControllerTest GetTypeTest_001::End");
}

/**
* @tc.name: GetTypeTest_002
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
HWTEST_F(ExtSpecialControllerTest, GetTypeTest_002, TestSize.Level0)
{
    LOG_INFO("ExtSpecialControllerTest GetTypeTest_002::Start");
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
    LOG_INFO("ExtSpecialControllerTest GetTypeTest_002::End");
}

/**
* @tc.name: BatchInsertTest_001
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
HWTEST_F(ExtSpecialControllerTest, BatchInsertTest_001, TestSize.Level0)
{
    LOG_INFO("ExtSpecialControllerTest BatchInsertTest_001::Start");
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
    LOG_INFO("ExtSpecialControllerTest BatchInsertTest_001::End");
}

/**
* @tc.name: BatchInsertTest_002
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
HWTEST_F(ExtSpecialControllerTest, BatchInsertTest_002, TestSize.Level0)
{
    LOG_INFO("ExtSpecialControllerTest BatchInsertTest_002::Start");
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
    LOG_INFO("ExtSpecialControllerTest BatchInsertTest_002::End");
}

/**
* @tc.name: BatchUpdateTest_001
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
HWTEST_F(ExtSpecialControllerTest, BatchUpdateTest_001, TestSize.Level0)
{
    LOG_INFO("ExtSpecialControllerTest BatchUpdateTest_001::Start");
    Uri uri("");
    std::shared_ptr<DataShare::ExtSpecialController> tempExtSpeCon =
        std::make_shared<DataShare::ExtSpecialController>(nullptr, uri, nullptr);
    DataShare::UpdateOperations operations;
    std::vector<DataShare::BatchUpdateResult> results;
    int result = tempExtSpeCon->BatchUpdate(operations, results);
    EXPECT_EQ(result, -1);
    LOG_INFO("ExtSpecialControllerTest BatchUpdateTest_001::End");
}

/**
* @tc.name: BatchUpdateTest_002
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
HWTEST_F(ExtSpecialControllerTest, BatchUpdateTest_002, TestSize.Level0)
{
    LOG_INFO("ExtSpecialControllerTest BatchUpdateTest_002::Start");
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
    LOG_INFO("ExtSpecialControllerTest BatchUpdateTest_002::End");
}

/**
* @tc.name: InsertExtTest_001
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
HWTEST_F(ExtSpecialControllerTest, InsertExtTest_001, TestSize.Level0)
{
    LOG_INFO("ExtSpecialControllerTest InsertExtTest_001::Start");
    Uri uri("");
    std::shared_ptr<DataShare::ExtSpecialController> tempExtSpeCon =
        std::make_shared<DataShare::ExtSpecialController>(nullptr, uri, nullptr);
    DataShare::DataShareValuesBucket valuesBucket;
    valuesBucket.Put("name", "dataShareTest006");
    valuesBucket.Put("phoneNumber", 20.6);
    std::string result1 = "test001";
    int result = tempExtSpeCon->InsertExt(uri, valuesBucket, result1);
    EXPECT_EQ(result, -1);
    LOG_INFO("ExtSpecialControllerTest InsertExtTest_001::End");
}

/**
* @tc.name: InsertExtTest_002
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
HWTEST_F(ExtSpecialControllerTest, InsertExtTest_002, TestSize.Level0)
{
    LOG_INFO("ExtSpecialControllerTest InsertExtTest_002::Start");
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
    LOG_INFO("ExtSpecialControllerTest InsertExtTest_002::End");
}

/**
* @tc.name: ExecuteBatchTest_001
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
HWTEST_F(ExtSpecialControllerTest, ExecuteBatchTest_001, TestSize.Level0)
{
    LOG_INFO("ExtSpecialControllerTest ExecuteBatchTest_001::Start");
    Uri uri("");
    std::shared_ptr<DataShare::ExtSpecialController> tempExtSpeCon =
        std::make_shared<DataShare::ExtSpecialController>(nullptr, uri, nullptr);
    std::vector<DataShare::OperationStatement> statements;
    std::vector<DataShare::BatchUpdateResult> results;
    ExecResultSet result1;
    int result = tempExtSpeCon->ExecuteBatch(statements, result1);
    EXPECT_EQ(result, -1);
    LOG_INFO("ExtSpecialControllerTest ExecuteBatchTest_001::End");
}

/**
* @tc.name: ExecuteBatchTest_002
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
HWTEST_F(ExtSpecialControllerTest, ExecuteBatchTest_002, TestSize.Level0)
{
    LOG_INFO("ExtSpecialControllerTest ExecuteBatchTest_002::Start");
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
    LOG_INFO("ExtSpecialControllerTest ExecuteBatchTest_002::End");
}

/**
* @tc.name: GetFileTypesTest_001
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
HWTEST_F(ExtSpecialControllerTest, GetFileTypesTest_001, TestSize.Level0)
{
    LOG_INFO("ExtSpecialControllerTest GetFileTypesTest_001::Start");
    Uri uri("");
    std::shared_ptr<DataShare::ExtSpecialController> tempExtSpeCon =
        std::make_shared<DataShare::ExtSpecialController>(nullptr, uri, nullptr);
    std::string getFileTypes = "test001";
    std::vector<std::string> result = tempExtSpeCon->GetFileTypes(uri, getFileTypes);
    EXPECT_EQ(result, std::vector<std::string>());
    LOG_INFO("ExtSpecialControllerTest GetFileTypesTest_001::End");
}

/**
* @tc.name: GetFileTypesTest_002
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
HWTEST_F(ExtSpecialControllerTest, GetFileTypesTest_002, TestSize.Level0)
{
    LOG_INFO("ExtSpecialControllerTest GetFileTypesTest_002::Start");
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
    LOG_INFO("ExtSpecialControllerTest GetFileTypesTest_002::End");
}

/**
* @tc.name: NormalizeUriTest_001
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
HWTEST_F(ExtSpecialControllerTest, NormalizeUriTest_001, TestSize.Level0)
{
    LOG_INFO("ExtSpecialControllerTest NormalizeUriTest_001::Start");
    Uri uri("");
    std::shared_ptr<DataShare::ExtSpecialController> tempExtSpeCon =
        std::make_shared<DataShare::ExtSpecialController>(nullptr, uri, nullptr);
    Uri result = tempExtSpeCon->NormalizeUri(uri);
    EXPECT_EQ(result, Uri(""));
    LOG_INFO("ExtSpecialControllerTest NormalizeUriTest_001::End");
}

/**
* @tc.name: NormalizeUriTest_002
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
HWTEST_F(ExtSpecialControllerTest, NormalizeUriTest_002, TestSize.Level0)
{
    LOG_INFO("ExtSpecialControllerTest NormalizeUriTest_002::Start");
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
    LOG_INFO("ExtSpecialControllerTest NormalizeUriTest_002::End");
}

/**
* @tc.name: DenormalizeUriTest_001
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
HWTEST_F(ExtSpecialControllerTest, DenormalizeUriTest_001, TestSize.Level0)
{
    LOG_INFO("ExtSpecialControllerTest DenormalizeUriTest_001::Start");
    Uri uri("");
    std::shared_ptr<DataShare::ExtSpecialController> tempExtSpeCon =
        std::make_shared<DataShare::ExtSpecialController>(nullptr, uri, nullptr);
    Uri result = tempExtSpeCon->DenormalizeUri(uri);
    EXPECT_EQ(result, Uri(""));
    LOG_INFO("ExtSpecialControllerTest DenormalizeUriTest_001::End");
}

/**
* @tc.name: DenormalizeUriTest_002
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
HWTEST_F(ExtSpecialControllerTest, DenormalizeUriTest_002, TestSize.Level0)
{
    LOG_INFO("ExtSpecialControllerTest DenormalizeUriTest_002::Start");
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
    LOG_INFO("ExtSpecialControllerTest DenormalizeUriTest_002::End");
}
} // namespace DataShare
} // namespace OHOS