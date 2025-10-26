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

#define LOG_TAG "datashare_block_writer_impl_test"

#include <gtest/gtest.h>
#include <unistd.h>

#include "datashare_block_writer_impl.h"
#include "datashare_log.h"

namespace OHOS {
namespace DataShare {
using namespace testing::ext;
class DataShareBlockWriterImplTest : public testing::Test {
public:
    static void SetUpTestCase(void){};
    static void TearDownTestCase(void){};
    void SetUp(){};
    void TearDown(){};
};

/**
 * @tc.name: AllocRowTest001
 * @tc.desc: Verify the behavior of the AllocRow function in DataShareBlockWriterImpl when its member variable
 *           shareBlock_ is set to nullptr, ensuring the function fails and returns the expected error code.
 * @tc.type: FUNC
 * @tc.require: issueIC413F
 * @tc.precon:
    1. The test environment supports instantiation of the DataShareBlockWriterImpl class without initialization errors.
    2. The shareBlock_ member variable of DataShareBlockWriterImpl allows explicit assignment to nullptr.
    3. The predefined constant E_ERROR (used to indicate operation failure) is correctly defined and accessible.
    4. The AllocRow function of DataShareBlockWriterImpl is properly declared and can be called normally.
 * @tc.step:
    1. Instantiate a DataShareBlockWriterImpl object named dataShareBlockWriterImpl.
    2. Explicitly set the shareBlock_ member variable of dataShareBlockWriterImpl to nullptr.
    3. Call the AllocRow function of the dataShareBlockWriterImpl object.
    4. Compare the return value of AllocRow with the predefined E_ERROR.
 * @tc.expect:
    1. The AllocRow function execution fails and returns E_ERROR.
 */
HWTEST_F(DataShareBlockWriterImplTest, AllocRowTest001, TestSize.Level0)
{
    LOG_INFO("DataShareBlockWriterImplTest AllocRowTest001::Start");
    DataShareBlockWriterImpl dataShareBlockWriterImpl;
    dataShareBlockWriterImpl.shareBlock_ = nullptr;
    auto result = dataShareBlockWriterImpl.AllocRow();
    EXPECT_EQ(result, E_ERROR);
    LOG_INFO("DataShareBlockWriterImplTest AllocRowTest001::End");
}

/**
 * @tc.name: FreeLastRowTest001
 * @tc.desc: Verify the behavior of the FreeLastRow function in DataShareBlockWriterImpl when its member variable
 *           shareBlock_ is set to nullptr, ensuring the function fails and returns the expected error code.
 * @tc.type: FUNC
 * @tc.require: issueIC413F
 * @tc.precon:
    1. The test environment supports instantiation of the DataShareBlockWriterImpl class without initialization errors.
    2. The shareBlock_ member variable of DataShareBlockWriterImpl allows explicit assignment to nullptr.
    3. The predefined constant E_ERROR (used to indicate operation failure) is correctly defined and accessible.
    4. The FreeLastRow function of DataShareBlockWriterImpl is properly declared and can be called normally.
 * @tc.step:
    1. Instantiate a DataShareBlockWriterImpl object named dataShareBlockWriterImpl.
    2. Explicitly set the shareBlock_ member variable of dataShareBlockWriterImpl to nullptr.
    3. Call the FreeLastRow function of the dataShareBlockWriterImpl object.
    4. Compare the return value of FreeLastRow with the predefined E_ERROR.
 * @tc.expect:
    1. The FreeLastRow function execution fails and returns E_ERROR.
 */
HWTEST_F(DataShareBlockWriterImplTest, FreeLastRowTest001, TestSize.Level0)
{
    LOG_INFO("DataShareBlockWriterImplTest FreeLastRowTest001::Start");
    DataShareBlockWriterImpl dataShareBlockWriterImpl;
    dataShareBlockWriterImpl.shareBlock_ = nullptr;
    auto result = dataShareBlockWriterImpl.FreeLastRow();
    EXPECT_EQ(result, E_ERROR);
    LOG_INFO("DataShareBlockWriterImplTest FreeLastRowTest001::End");
}

/**
 * @tc.name: WriteTest001
 * @tc.desc: Verify the behavior of the Write function (overload with uint32_t column parameter) in
 *           DataShareBlockWriterImpl when its member variable shareBlock_ is set to nullptr, ensuring the function
 *           fails and returns E_ERROR.
 * @tc.type: FUNC
 * @tc.require: issueIC413F
 * @tc.precon:
    1. The test environment supports instantiation of the DataShareBlockWriterImpl class without initialization errors.
    2. The shareBlock_ member variable of DataShareBlockWriterImpl allows explicit assignment to nullptr.
    3. The predefined constant E_ERROR and uint32_t type are valid and accessible in the test environment.
    4. The Write function (overload with uint32_t column) of DataShareBlockWriterImpl is properly declared and
       callable.
 * @tc.step:
    1. Instantiate a DataShareBlockWriterImpl object named dataShareBlockWriterImpl.
    2. Set the shareBlock_ member variable of dataShareBlockWriterImpl to nullptr.
    3. Define a uint32_t variable column and initialize it to 1.
    4. Call the Write function of dataShareBlockWriterImpl, passing the column variable as the parameter.
    5. Check if the return value of Write equals E_ERROR.
 * @tc.expect:
    1. The Write function execution fails and returns E_ERROR.
 */
HWTEST_F(DataShareBlockWriterImplTest, WriteTest001, TestSize.Level0)
{
    LOG_INFO("DataShareBlockWriterImplTest WriteTest001::Start");
    DataShareBlockWriterImpl dataShareBlockWriterImpl;
    dataShareBlockWriterImpl.shareBlock_ = nullptr;
    uint32_t column = 1;
    auto result = dataShareBlockWriterImpl.Write(column);
    EXPECT_EQ(result, E_ERROR);
    LOG_INFO("DataShareBlockWriterImplTest WriteTest001::End");
}

/**
 * @tc.name: WriteTest002
 * @tc.desc: Verify the behavior of the Write function (overload with uint32_t column and int64_t value) in
 *           DataShareBlockWriterImpl when its member variable shareBlock_ is set to nullptr, ensuring the function
 *           fails and returns E_ERROR.
 * @tc.type: FUNC
 * @tc.require: issueIC413F
 * @tc.precon:
    1. The test environment supports instantiation of the DataShareBlockWriterImpl class without initialization errors.
    2. The shareBlock_ member variable of DataShareBlockWriterImpl allows explicit assignment to nullptr.
    3. The predefined constant E_ERROR, uint32_t, and int64_t types are valid and accessible.
    4. The Write function (overload with uint32_t column and int64_t value) is properly declared and callable.
 * @tc.step:
    1. Instantiate a DataShareBlockWriterImpl object named dataShareBlockWriterImpl.
    2. Set the shareBlock_ member variable of dataShareBlockWriterImpl to nullptr.
    3. Define a uint32_t column (initialized to 1) and an int64_t value (initialized to 1).
    4. Call the Write function of dataShareBlockWriterImpl, passing column and value as parameters.
    5. Check if the return value of Write equals E_ERROR.
 * @tc.expect:
    1. The Write function execution fails and returns E_ERROR.
 */
HWTEST_F(DataShareBlockWriterImplTest, WriteTest002, TestSize.Level0)
{
    LOG_INFO("DataShareBlockWriterImplTest WriteTest002::Start");
    DataShareBlockWriterImpl dataShareBlockWriterImpl;
    dataShareBlockWriterImpl.shareBlock_ = nullptr;
    uint32_t column = 1;
    int64_t value = 1;
    auto result = dataShareBlockWriterImpl.Write(column, value);
    EXPECT_EQ(result, E_ERROR);
    LOG_INFO("DataShareBlockWriterImplTest WriteTest002::End");
}

/**
 * @tc.name: WriteTest003
 * @tc.desc: Verify the behavior of the Write function (overload with uint32_t column and double value) in
 *           DataShareBlockWriterImpl when its member variable shareBlock_ is set to nullptr, ensuring the function
 *           fails and returns E_ERROR.
 * @tc.type: FUNC
 * @tc.require: issueIC413F
 * @tc.precon:
    1. The test environment supports instantiation of the DataShareBlockWriterImpl class without initialization errors.
    2. The shareBlock_ member variable of DataShareBlockWriterImpl allows explicit assignment to nullptr.
    3. The predefined constant E_ERROR, uint32_t, and double types are valid and accessible.
    4. The Write function (overload with uint32_t column and double value) is properly declared and callable.
 * @tc.step:
    1. Instantiate a DataShareBlockWriterImpl object named dataShareBlockWriterImpl.
    2. Set the shareBlock_ member variable of dataShareBlockWriterImpl to nullptr.
    3. Define a uint32_t column (initialized to 1) and a double value (initialized to 1.0).
    4. Call the Write function of dataShareBlockWriterImpl, passing column and value as parameters.
    5. Check if the return value of Write equals E_ERROR.
 * @tc.expect:
    1. The Write function execution fails and returns E_ERROR.
 */
HWTEST_F(DataShareBlockWriterImplTest, WriteTest003, TestSize.Level0)
{
    LOG_INFO("DataShareBlockWriterImplTest WriteTest003::Start");
    DataShareBlockWriterImpl dataShareBlockWriterImpl;
    dataShareBlockWriterImpl.shareBlock_ = nullptr;
    uint32_t column = 1;
    double value = 1;
    auto result = dataShareBlockWriterImpl.Write(column, value);
    EXPECT_EQ(result, E_ERROR);
    LOG_INFO("DataShareBlockWriterImplTest WriteTest003::End");
}

/**
 * @tc.name: WriteTest004
 * @tc.desc: Verify the behavior of the Write function (overload with uint32_t column, uint8_t* value, size_t size) in
 *           DataShareBlockWriterImpl when shareBlock_ is set to nullptr, ensuring the function fails and returns
 *           E_ERROR.
 * @tc.type: FUNC
 * @tc.require: issueIC413F
 * @tc.precon:
    1. The test environment supports instantiation of the DataShareBlockWriterImpl class without initialization errors.
    2. The shareBlock_ member variable of DataShareBlockWriterImpl allows explicit assignment to nullptr.
    3. The predefined constant E_ERROR, uint32_t, uint8_t array, and size_t types are valid and accessible.
    4. The Write function (overload with uint32_t column, uint8_t* value, size_t size) is properly declared and
       callable.
 * @tc.step:
    1. Instantiate a DataShareBlockWriterImpl object named dataShareBlockWriterImpl.
    2. Set the shareBlock_ member variable of dataShareBlockWriterImpl to nullptr.
    3. Define a uint32_t column (1), a uint8_t array value[3] (initialized to {1}), and a size_t size (1).
    4. Call the Write function of dataShareBlockWriterImpl, passing column, value, and size as parameters.
    5. Check if the return value of Write equals E_ERROR.
 * @tc.expect:
    1. The Write function execution fails and returns E_ERROR.
 */
HWTEST_F(DataShareBlockWriterImplTest, WriteTest004, TestSize.Level0)
{
    LOG_INFO("DataShareBlockWriterImplTest WriteTest004::Start");
    DataShareBlockWriterImpl dataShareBlockWriterImpl;
    dataShareBlockWriterImpl.shareBlock_ = nullptr;
    uint32_t column = 1;
    uint8_t value[3] = {1};
    size_t size = 1;
    auto result = dataShareBlockWriterImpl.Write(column, value, size);
    EXPECT_EQ(result, E_ERROR);
    LOG_INFO("DataShareBlockWriterImplTest WriteTest004::End");
}

/**
 * @tc.name: WriteTest005
 * @tc.desc: Verify the behavior of the Write function (overload with uint32_t column, const char* value, size_t
 *           sizeIncludingNull) in DataShareBlockWriterImpl when shareBlock_ is set to nullptr, ensuring the function
 *           fails and returns E_ERROR.
 * @tc.type: FUNC
 * @tc.require: issueIC413F
 * @tc.precon:
    1. The test environment supports instantiation of the DataShareBlockWriterImpl class without initialization errors.
    2. The shareBlock_ member variable of DataShareBlockWriterImpl allows explicit assignment to nullptr.
    3. The predefined constant E_ERROR, uint32_t, const char*, and size_t types are valid and accessible.
    4. The Write function (overload with const char* value) of DataShareBlockWriterImpl is properly declared and
       callable.
 * @tc.step:
    1. Instantiate a DataShareBlockWriterImpl object named dataShareBlockWriterImpl.
    2. Set the shareBlock_ member variable of dataShareBlockWriterImpl to nullptr.
    3. Define a uint32_t column (1), a const char* value ("test"), and a size_t sizeIncludingNull (1).
    4. Call the Write function of dataShareBlockWriterImpl, passing column, value, and sizeIncludingNull as parameters.
    5. Check if the return value of Write equals E_ERROR.
 * @tc.expect:
    1. The Write function execution fails and returns E_ERROR.
 */
HWTEST_F(DataShareBlockWriterImplTest, WriteTest005, TestSize.Level0)
{
    LOG_INFO("DataShareBlockWriterImplTest WriteTest005::Start");
    DataShareBlockWriterImpl dataShareBlockWriterImpl;
    dataShareBlockWriterImpl.shareBlock_ = nullptr;
    uint32_t column = 1;
    const char* value = "test";
    size_t sizeIncludingNull = 1;
    auto result = dataShareBlockWriterImpl.Write(column, value, sizeIncludingNull);
    EXPECT_EQ(result, E_ERROR);
    LOG_INFO("DataShareBlockWriterImplTest WriteTest005::End");
}

/**
 * @tc.name: GetCurrentRowIndexTest001
 * @tc.desc: Verify the behavior of the GetCurrentRowIndex function in DataShareBlockWriterImpl when its member
 *           variable shareBlock_ is set to nullptr, ensuring the function fails and returns false.
 * @tc.type: FUNC
 * @tc.require: issueIC413F
 * @tc.precon:
    1. The test environment supports instantiation of the DataShareBlockWriterImpl class without initialization errors.
    2. The shareBlock_ member variable of DataShareBlockWriterImpl allows explicit assignment to nullptr.
    3. The uint32_t type is valid in the test environment, and the GetCurrentRowIndex function returns a bool.
    4. The GetCurrentRowIndex function of DataShareBlockWriterImpl is properly declared and can be called with a
       uint32_t reference.
 * @tc.step:
    1. Instantiate a DataShareBlockWriterImpl object named dataShareBlockWriterImpl.
    2. Explicitly set the shareBlock_ member variable of dataShareBlockWriterImpl to nullptr.
    3. Define a uint32_t variable rowIndex and initialize it to 1.
    4. Call the GetCurrentRowIndex function of dataShareBlockWriterImpl, passing rowIndex as a reference parameter.
    5. Check if the return value of GetCurrentRowIndex is false.
 * @tc.expect:
    1. The GetCurrentRowIndex function execution fails and returns false.
 */
HWTEST_F(DataShareBlockWriterImplTest, GetCurrentRowIndexTest001, TestSize.Level0)
{
    LOG_INFO("DataShareBlockWriterImplTest GetCurrentRowIndexTest001::Start");
    DataShareBlockWriterImpl dataShareBlockWriterImpl;
    dataShareBlockWriterImpl.shareBlock_ = nullptr;
    uint32_t rowIndex = 1;
    auto result = dataShareBlockWriterImpl.GetCurrentRowIndex(rowIndex);
    EXPECT_EQ(result, false);
    LOG_INFO("DataShareBlockWriterImplTest GetCurrentRowIndexTest001::End");
}

/**
 * @tc.name: DataShareBlockWriterImplTest_shareBlock_Null_Test_001
 * @tc.desc: Verify the behavior of multiple member functions (AllocRow and multiple Write overloads) in
 *           DataShareBlockWriterImpl when its member variable shareBlock_ is set to nullptr, ensuring all operations
 *           fail and return E_ERROR.
 * @tc.type: FUNC
 * @tc.precon:
    1. The test environment supports instantiation of the DataShareBlockWriterImpl class without initialization errors.
    2. The shareBlock_ member variable of DataShareBlockWriterImpl allows explicit assignment to nullptr (default if
       uninitialized).
    3. The predefined constant E_ERROR and types (uint32_t, int64_t, double, uint8_t*, const char*, size_t) are valid
       and accessible.
    4. The tested functions (AllocRow, Write with multiple overloads) of DataShareBlockWriterImpl are properly declared
       and callable.
 * @tc.step:
    1. Instantiate a DataShareBlockWriterImpl object named impl (shareBlock_ is uninitialized, defaulting to nullptr).
    2. Call the AllocRow function of impl and record its return value.
    3. Call the Write function (overload with uint32_t column=1) of impl and record its return value.
    4. Call the Write function (overload with uint32_t column=1 and int64_t value=0) of impl and record its return
       value.
    5. Call the Write function (overload with uint32_t column=1 and double value=0.0) of impl and record its return
       value.
    6. Call the Write function (overload with uint32_t column=1, uint8_t* value=nullptr, size_t size=0) of impl and
       record its return value.
    7. Call the Write function (overload with uint32_t column=1, const char* value=nullptr, size_t size=0) of impl and
       record its return value.
    8. Compare all recorded return values with E_ERROR.
 * @tc.expect:
    1. All tested functions (AllocRow and all Write overloads) execute fail and return E_ERROR.
 */
HWTEST_F(DataShareBlockWriterImplTest, ShareBlock_Null_Test_001, TestSize.Level0)
{
    LOG_INFO("ShareBlock_Null_Test_001::Start");
    DataShareBlockWriterImpl impl;
    int result = impl.AllocRow();
    EXPECT_EQ(result, E_ERROR);
    result = impl.Write(1);
    EXPECT_EQ(result, E_ERROR);
    int64_t intValue = 0;
    result = impl.Write(1, intValue);
    EXPECT_EQ(result, E_ERROR);
    double doubleValue = 0.0;
    result = impl.Write(1, doubleValue);
    EXPECT_EQ(result, E_ERROR);
    uint8_t *unitValue = nullptr;
    result = impl.Write(1, unitValue, 0);
    EXPECT_EQ(result, E_ERROR);
    char *charValue = nullptr;
    result = impl.Write(1, charValue, 0);
    EXPECT_EQ(result, E_ERROR);
    LOG_INFO("ShareBlock_Null_Test_001::End");
}
} // namespace DataShare
} // namespace OHOS