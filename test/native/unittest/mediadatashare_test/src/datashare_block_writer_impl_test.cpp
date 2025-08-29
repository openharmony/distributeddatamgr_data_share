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
* @tc.desc: test AllocRow function when shareBlock_ = nullptr
* @tc.type: FUNC
* @tc.require:issueIC413F
* @tc.precon: None
* @tc.step:
    1.Create a DataShareBlockWriterImpl object and shareBlock_ = nullptr
    2.call AllocRow function and check the result
* @tc.experct: AllocRow failed and reutrn E_ERROR
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
* @tc.desc: test Write function when shareBlock_ = nullptr
* @tc.type: FUNC
* @tc.require:issueIC413F
* @tc.precon: None
* @tc.step:
    1.Create a DataShareBlockWriterImpl object and shareBlock_ = nullptr
    2.call Write function and check the result
* @tc.experct: Write failed and reutrn E_ERROR
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
* @tc.desc: test Write function when shareBlock_ = nullptr
* @tc.type: FUNC
* @tc.require:issueIC413F
* @tc.precon: None
* @tc.step:
    1.Create a DataShareBlockWriterImpl object and shareBlock_ = nullptr
    2.call Write function and check the result
* @tc.experct: Write failed and reutrn E_ERROR
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
* @tc.desc: test Write function when shareBlock_ = nullptr
* @tc.type: FUNC
* @tc.require:issueIC413F
* @tc.precon: None
* @tc.step:
    1.Create a DataShareBlockWriterImpl object and shareBlock_ = nullptr
    2.call Write function and check the result
* @tc.experct: Write failed and reutrn E_ERROR
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
* @tc.desc: test Write function when shareBlock_ = nullptr
* @tc.type: FUNC
* @tc.require:issueIC413F
* @tc.precon: None
* @tc.step:
    1.Create a DataShareBlockWriterImpl object and shareBlock_ = nullptr
    2.call Write function and check the result
* @tc.experct: Write failed and reutrn E_ERROR
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
* @tc.desc: test Write function when shareBlock_ = nullptr
* @tc.type: FUNC
* @tc.require:issueIC413F
* @tc.precon: None
* @tc.step:
    1.Create a DataShareBlockWriterImpl object and shareBlock_ = nullptr
    2.call Write function and check the result
* @tc.experct: Write failed and reutrn E_ERROR
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
* @tc.desc: test GetCurrentRowIndex function when shareBlock_ = nullptr
* @tc.type: FUNC
* @tc.require:issueIC413F
* @tc.precon: None
* @tc.step:
    1.Create a DataShareBlockWriterImpl object and shareBlock_ = nullptr
    2.call GetCurrentRowIndex function and check the result
* @tc.experct: GetCurrentRowIndex failed and reutrn false
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