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
 * @tc.desc: Verify the AllocRow function of the DataShareBlockWriterImpl class when its member variable shareBlock_
 *           is set to nullptr, to confirm whether the function fails and returns the expected error code.
 * @tc.type: FUNC
 * @tc.require: issueIC413F
 * @tc.precon:
    1. The test environment is properly deployed and supports the execution of DataShareBlockWriterImpl-related
       test cases.
    2. The DataShareBlockWriterImpl class can be instantiated normally without initialization exceptions.
    3. The shareBlock_ member variable of DataShareBlockWriterImpl allows explicit assignment to nullptr.
 * @tc.step:
    1. Instantiate a DataShareBlockWriterImpl object named dataShareBlockWriterImpl.
    2. Explicitly set the shareBlock_ member variable of dataShareBlockWriterImpl to nullptr.
    3. Call the AllocRow function of the dataShareBlockWriterImpl object.
    4. Record the return result of the AllocRow function and check whether it meets the expected value.
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
 * @tc.name: WriteTest001
 * @tc.desc: Verify the Write function of the DataShareBlockWriterImpl class when its member variable shareBlock_
 *           is set to nullptr, to confirm whether the function fails and returns the expected error code.
 * @tc.type: FUNC
 * @tc.require: issueIC413F
 * @tc.precon:
    1. The test environment is properly deployed and supports running DataShareBlockWriterImpl test cases.
    2. The DataShareBlockWriterImpl class can be instantiated normally without initialization errors.
    3. The shareBlock_ member variable of DataShareBlockWriterImpl can be explicitly set to nullptr.
    4. The uint32_t type variable can be normally defined and assigned in the test environment.
 * @tc.step:
    1. Instantiate a DataShareBlockWriterImpl object named dataShareBlockWriterImpl.
    2. Set the shareBlock_ member variable of dataShareBlockWriterImpl to nullptr.
    3. Define a uint32_t variable column and initialize it to 1.
    4. Call the Write function of dataShareBlockWriterImpl, passing the column variable as the parameter.
    5. Check the return result of the Write function.
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
 * @tc.desc: Verify the Write function (with uint32_t column and int64_t value parameters) of the
 *           DataShareBlockWriterImpl class when its member variable shareBlock_ is set to nullptr,
 *           to confirm whether the function fails and returns the expected error code.
 * @tc.type: FUNC
 * @tc.require: issueIC413F
 * @tc.precon:
    1. The test environment is properly deployed and supports executing DataShareBlockWriterImpl-related test logic.
    2. The DataShareBlockWriterImpl class can be instantiated normally without initialization exceptions.
    3. The shareBlock_ member variable of DataShareBlockWriterImpl allows explicit assignment to nullptr.
    4. The uint32_t and int64_t type variables can be normally defined and assigned in the test environment.
 * @tc.step:
    1. Instantiate a DataShareBlockWriterImpl object named dataShareBlockWriterImpl.
    2. Explicitly set the shareBlock_ member variable of dataShareBlockWriterImpl to nullptr.
    3. Define a uint32_t variable column (initialized to 1) and an int64_t variable value (initialized to 1).
    4. Call the Write function of dataShareBlockWriterImpl, passing column and value as parameters.
    5. Check the return result of the Write function.
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
 * @tc.desc: Verify the Write function of the DataShareBlockWriterImpl class when its member variable shareBlock_
 *           is set to nullptr, to confirm whether the function fails and returns the expected error code.
 * @tc.type: FUNC
 * @tc.require: issueIC413F
 * @tc.precon:
    1. The test environment is properly deployed and supports running DataShareBlockWriterImpl test cases.
    2. The DataShareBlockWriterImpl class can be instantiated normally without initialization errors.
    3. The shareBlock_ member variable of DataShareBlockWriterImpl can be explicitly set to nullptr.
    4. The uint32_t and double type variables can be normally defined and assigned in the test environment.
 * @tc.step:
    1. Instantiate a DataShareBlockWriterImpl object named dataShareBlockWriterImpl.
    2. Set the shareBlock_ member variable of dataShareBlockWriterImpl to nullptr.
    3. Define a uint32_t variable column (initialized to 1) and a double variable value (initialized to 1.0).
    4. Call the Write function of dataShareBlockWriterImpl, passing column and value as parameters.
    5. Check the return result of the Write function.
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
 * @tc.desc: Verify the Write function of the DataShareBlockWriterImpl class when its member variable shareBlock_
 *           is set to nullptr, to confirm whether the function fails and returns the expected error code.
 * @tc.type: FUNC
 * @tc.require: issueIC413F
 * @tc.precon:
    1. The test environment is properly deployed and supports executing DataShareBlockWriterImpl-related test logic.
    2. The DataShareBlockWriterImpl class can be instantiated normally without initialization exceptions.
    3. The shareBlock_ member variable of DataShareBlockWriterImpl allows explicit assignment to nullptr.
    4. The uint32_t, uint8_t array and size_t type variables can be normally defined in the test environment.
 * @tc.step:
    1. Instantiate a DataShareBlockWriterImpl object named dataShareBlockWriterImpl.
    2. Explicitly set the shareBlock_ member variable of dataShareBlockWriterImpl to nullptr.
    3. Define a uint32_t variable column, a uint8_t array value[3] and a size_t variable size.
    4. Call the Write function of dataShareBlockWriterImpl, passing column, value and size as parameters.
    5. Check the return result of the Write function.
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
 * @tc.desc: Verify the Write function of the DataShareBlockWriterImpl class when its member variable shareBlock_
 *           is set to nullptr, to confirm whether the function fails and returns the expected error code.
 * @tc.type: FUNC
 * @tc.require: issueIC413F
 * @tc.precon:
    1. The test environment is properly deployed and supports running DataShareBlockWriterImpl test cases.
    2. The DataShareBlockWriterImpl class can be instantiated normally without initialization errors.
    3. The shareBlock_ member variable of DataShareBlockWriterImpl can be explicitly set to nullptr.
    4. The uint32_t, const char* and size_t type variables can be normally defined in the test environment.
 * @tc.step:
    1. Instantiate a DataShareBlockWriterImpl object named dataShareBlockWriterImpl.
    2. Set the shareBlock_ member variable of dataShareBlockWriterImpl to nullptr.
    3. Define a uint32_t variable column, a const char* variable value and a size_t variable sizeIncludingNull.
    4. Call the Write function of dataShareBlockWriterImpl, passing column, value and sizeIncludingNull as parameters.
    5. Check the return result of the Write function.
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
 * @tc.desc: Verify the GetCurrentRowIndex function of the DataShareBlockWriterImpl class when its member variable
 *           shareBlock_ is set to nullptr, to confirm whether the function fails and returns the expected boolean
 *           result.
 * @tc.type: FUNC
 * @tc.require: issueIC413F
 * @tc.precon:
    1. The test environment is properly deployed and supports executing DataShareBlockWriterImpl-related test logic.
    2. The DataShareBlockWriterImpl class can be instantiated normally without initialization exceptions.
    3. The shareBlock_ member variable of DataShareBlockWriterImpl allows explicit assignment to nullptr.
    4. The uint32_t type variable can be normally defined and passed as a parameter in the test environment.
 * @tc.step:
    1. Instantiate a DataShareBlockWriterImpl object named dataShareBlockWriterImpl.
    2. Explicitly set the shareBlock_ member variable of dataShareBlockWriterImpl to nullptr.
    3. Define a uint32_t variable rowIndex and initialize it to 1.
    4. Call the GetCurrentRowIndex function of dataShareBlockWriterImpl, passing rowIndex as the parameter.
    5. Check the return result of the GetCurrentRowIndex function.
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
} // namespace DataShare
} // namespace OHOS