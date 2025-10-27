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