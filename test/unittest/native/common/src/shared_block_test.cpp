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
#define LOG_TAG "shared_block_test"

#include "shared_block.h"
#include <gtest/gtest.h>
#include <cstdint>
#include <memory>
#include <string>
#include "ashmem.h"
#include "datashare_log.h"
#include "refbase.h"

namespace OHOS {
namespace DataShare {
using namespace testing::ext;
using namespace OHOS::AppDataFwk;

class SharedBlockTest : public testing::Test {
public:
    static void SetUpTestCase(void){};
    static void TearDownTestCase(void){};
    void SetUp(){};
    void TearDown(){};
};

/**
* @tc.name: InitTest001
* @tc.desc: Test SharedBlock Init function when Ashmem start address is null
* @tc.type: FUNC
* @tc.step:
    1. Create an Ashmem instance named "ahsmem" with size equal to SharedBlockHeader
    2. Set Ashmem's startAddr_ to nullptr
    3. Create a SharedBlock instance with the Ashmem, name "name", offset 0, and read-only flag true
    4. Call Init() on the SharedBlock instance
    5. Check the return value of Init()
* @tc.expect: SharedBlock::Init() returns false (initialization fails due to null Ashmem start address)
*/
HWTEST_F(SharedBlockTest, InitTest001, TestSize.Level0)
{
    LOG_INFO("InitTest001::Start");
    sptr<Ashmem> ashmem = Ashmem::CreateAshmem("ahsmem", sizeof(SharedBlock::SharedBlockHeader));
    EXPECT_NE(ashmem, nullptr);
    ashmem->startAddr_ = nullptr;
    SharedBlock *sharedBlock = new SharedBlock("name", ashmem, 0, true);
    EXPECT_EQ(sharedBlock->Init(), false);
    LOG_INFO("InitTest001::End");
}

/**
* @tc.name: CreateSharedBlockTest001
* @tc.desc: Test CreateSharedBlock function when Ashmem start address is null
* @tc.type: FUNC
* @tc.step:
    1. Create an Ashmem instance named "ahsmem" with size equal to SharedBlockHeader
    2. Set Ashmem's startAddr_ to nullptr
    3. Declare a null SharedBlock pointer
    4. Call CreateSharedBlock with name "name", size equal to SharedBlockHeader, the Ashmem, and the null pointer
    5. Check the return error code
* @tc.expect: CreateSharedBlock returns SharedBlock::SHARED_BLOCK_ASHMEM_ERROR
*/
HWTEST_F(SharedBlockTest, CreateSharedBlockTest001, TestSize.Level0)
{
    LOG_INFO("CreateSharedBlockTest001::Start");
    sptr<Ashmem> ashmem = Ashmem::CreateAshmem("ahsmem", sizeof(SharedBlock::SharedBlockHeader));
    EXPECT_NE(ashmem, nullptr);
    ashmem->startAddr_ = nullptr;
    AppDataFwk::SharedBlock *sharedBlock = nullptr;
    EXPECT_EQ(sharedBlock->CreateSharedBlock("name", sizeof(SharedBlock::SharedBlockHeader), ashmem, sharedBlock),
        SharedBlock::SHARED_BLOCK_ASHMEM_ERROR);
    LOG_INFO("CreateSharedBlockTest001::End");
}

/**
* @tc.name: CreateTest001
* @tc.desc: Test SharedBlock::Create function with negative size parameter
* @tc.type: FUNC
* @tc.step:
    1. Declare a null SharedBlock pointer
    2. Call SharedBlock::Create with name "name", negative size (-1), and the null pointer
    3. Check the return error code
* @tc.expect: SharedBlock::Create returns SharedBlock::SHARED_BLOCK_ASHMEM_ERROR
*/
HWTEST_F(SharedBlockTest, CreateTest001, TestSize.Level0)
{
    LOG_INFO("CreateTest001::Start");
    AppDataFwk::SharedBlock *sharedBlock = nullptr;
    EXPECT_EQ(SharedBlock::Create("name", -1, sharedBlock), SharedBlock::SHARED_BLOCK_ASHMEM_ERROR);
    LOG_INFO("CreateTest001::End");
}

/**
* @tc.name: ReadMessageParcelTest001
* @tc.desc: Test SharedBlock::ReadMessageParcel function with valid parcel data
* @tc.type: FUNC
* @tc.step:
    1. Create a MessageParcel instance
    2. Write a string16 ("string") and the Ashmem instance to the parcel
    3. Declare a null SharedBlock pointer
    4. Call SharedBlock::ReadMessageParcel with the parcel and the null pointer
    5. Check the return error code
* @tc.expect: SharedBlock::ReadMessageParcel returns SharedBlock::SHARED_BLOCK_OK (successfully reads parcel data)
*/
HWTEST_F(SharedBlockTest, ReadMessageParcelTest001, TestSize.Level0)
{
    LOG_INFO("ReadMessageParcelTest001::Start");
    MessageParcel parcel;
    std::u16string str = u"string";
    SharedBlock *block = nullptr;
    sptr<Ashmem> ashmem = Ashmem::CreateAshmem("ahsmem", sizeof(SharedBlock::SharedBlockHeader));
    EXPECT_NE(ashmem, nullptr);
    ashmem->startAddr_ = nullptr;
    parcel.WriteString16(str);
    parcel.WriteAshmem(ashmem);
    EXPECT_EQ(SharedBlock::ReadMessageParcel(parcel, block), SharedBlock::SHARED_BLOCK_OK);

    LOG_INFO("ReadMessageParcelTest001::End");
}

/**
* @tc.name: ClearTest001
* @tc.desc: Test SharedBlock Clear function under different states (read-only, insufficient size, normal)
* @tc.type: FUNC
* @tc.step:
    1. Create a SharedBlock instance with valid size, verify it is not null
    2. Set the SharedBlock to read-only mode, call Clear() and check return code
    3. Set the SharedBlock to read-write mode, set mSize to SharedBlockHeader size, call Clear and check return code
    4. Restore mSize to original valid size, call Clear() and check return code
* @tc.expect:
    - Clear() returns SHARED_BLOCK_INVALID_OPERATION when read-only
    - Clear() returns SHARED_BLOCK_BAD_VALUE when size is insufficient
    - Clear() returns SHARED_BLOCK_BAD_VALUE when in read-write mode with invalid size
    - Clear() returns SHARED_BLOCK_OK when in read-write mode with valid size
*/
HWTEST_F(SharedBlockTest, ClearTest001, TestSize.Level0)
{
    LOG_INFO("ClearTest001::Start");
    AppDataFwk::SharedBlock *sharedBlock = nullptr;
    EXPECT_EQ(SharedBlock::Create("name",
        sizeof(SharedBlock::RowGroupHeader) + sizeof(SharedBlock::SharedBlockHeader) + 1, sharedBlock),
        SharedBlock::SHARED_BLOCK_OK);
    EXPECT_NE(sharedBlock, nullptr);
    sharedBlock->mReadOnly = true;
    EXPECT_EQ(sharedBlock->Clear(), SharedBlock::SHARED_BLOCK_INVALID_OPERATION);
    sharedBlock->mReadOnly = false;
    sharedBlock->mSize = sizeof(SharedBlock::SharedBlockHeader);
    EXPECT_EQ(sharedBlock->Clear(), SharedBlock::SHARED_BLOCK_BAD_VALUE);
    sharedBlock->mSize = sizeof(SharedBlock::RowGroupHeader) + sizeof(SharedBlock::SharedBlockHeader);
    EXPECT_EQ(sharedBlock->Clear(), SharedBlock::SHARED_BLOCK_BAD_VALUE);
    sharedBlock->mSize = sizeof(SharedBlock::RowGroupHeader) + sizeof(SharedBlock::SharedBlockHeader) + 1;
    EXPECT_EQ(sharedBlock->Clear(), SharedBlock::SHARED_BLOCK_OK);
    LOG_INFO("ClearTest001::End");
}

/**
* @tc.name: SetColumnNumTest001
* @tc.desc: Test SetColumnNum function under various conditions (read-only, invalid values, max limit)
* @tc.type: FUNC
* @tc.step:
    1. Create a SharedBlock instance, verify it is not null
    2. Set to read-only mode, call SetColumnNum(1) and check return code
    3. Set to read-write mode, configure header with conflicting row/column nums, call SetColumnNum(0) and check
    4. Test with max column num (32768) and conflicting row nums, check return code
    5. Test valid configuration (matching row/column nums), check return code
    6. Test setting column num to 0 with no rows, check return code
* @tc.expect:
    - Returns SHARED_BLOCK_INVALID_OPERATION in read-only mode
    - Returns SHARED_BLOCK_INVALID_OPERATION for invalid value combinations
    - Returns SHARED_BLOCK_OK for valid configurations
*/
HWTEST_F(SharedBlockTest, SetColumnNumTest001, TestSize.Level0)
{
    LOG_INFO("SetColumnNumTest001::Start");
    AppDataFwk::SharedBlock *sharedBlock = nullptr;
    EXPECT_EQ(SharedBlock::Create("name",
        sizeof(SharedBlock::RowGroupHeader) + sizeof(SharedBlock::SharedBlockHeader) + 1, sharedBlock),
        SharedBlock::SHARED_BLOCK_OK);
    EXPECT_NE(sharedBlock, nullptr);
    sharedBlock->mReadOnly = true;
    uint32_t numColumns = 1;
    EXPECT_EQ(sharedBlock->SetColumnNum(numColumns), SharedBlock::SHARED_BLOCK_INVALID_OPERATION);
    sharedBlock->mReadOnly = false;
    sharedBlock->mHeader->columnNums = numColumns;
    sharedBlock->mHeader->rowNums = numColumns;
    EXPECT_EQ(sharedBlock->SetColumnNum(0), SharedBlock::SHARED_BLOCK_INVALID_OPERATION);
    sharedBlock->mHeader->columnNums = 0;
    sharedBlock->mHeader->rowNums = numColumns;
    EXPECT_EQ(sharedBlock->SetColumnNum(numColumns), SharedBlock::SHARED_BLOCK_INVALID_OPERATION);
    sharedBlock->mHeader->columnNums = numColumns;
    sharedBlock->mHeader->rowNums = 0;
    EXPECT_EQ(sharedBlock->SetColumnNum(0), SharedBlock::SHARED_BLOCK_INVALID_OPERATION);
    numColumns = 32768; // 32768 is max_num
    sharedBlock->mHeader->columnNums = numColumns;
    sharedBlock->mHeader->rowNums = numColumns;
    EXPECT_EQ(sharedBlock->SetColumnNum(numColumns), SharedBlock::SHARED_BLOCK_INVALID_OPERATION);
    numColumns = 1;
    sharedBlock->mHeader->columnNums = numColumns;
    sharedBlock->mHeader->rowNums = numColumns;
    EXPECT_EQ(sharedBlock->SetColumnNum(numColumns), SharedBlock::SHARED_BLOCK_OK);
    sharedBlock->mHeader->columnNums = 0;
    sharedBlock->mHeader->rowNums = 0;
    EXPECT_EQ(sharedBlock->SetColumnNum(0), SharedBlock::SHARED_BLOCK_OK);
    EXPECT_EQ(sharedBlock->Clear(), SharedBlock::SHARED_BLOCK_OK);
    LOG_INFO("SetColumnNumTest001::End");
}

/**
* @tc.name: AllocRowTest001
* @tc.desc: Test AllocRow function under read-only mode and insufficient memory
* @tc.type: FUNC
* @tc.step:
    1. Create a SharedBlock instance, verify it is not null
    2. Set to read-only mode, call AllocRow() and check return code
    3. Set to read-write mode, set mSize to 0 (insufficient memory), call AllocRow() and check return code
    4. Restore mSize to valid value, call Clear() to clean up
* @tc.expect:
    - AllocRow() returns SHARED_BLOCK_INVALID_OPERATION in read-only mode
    - AllocRow() returns SHARED_BLOCK_NO_MEMORY when size is insufficient
    - Clear() returns SHARED_BLOCK_OK after restoring size
*/
HWTEST_F(SharedBlockTest, AllocRow, TestSize.Level0)
{
    LOG_INFO("AllocRowTest001::Start");
    AppDataFwk::SharedBlock *sharedBlock = nullptr;
    EXPECT_EQ(SharedBlock::Create("name",
        sizeof(SharedBlock::RowGroupHeader) + sizeof(SharedBlock::SharedBlockHeader) + 1, sharedBlock),
        SharedBlock::SHARED_BLOCK_OK);
    EXPECT_NE(sharedBlock, nullptr);
    sharedBlock->mReadOnly = true;
    EXPECT_EQ(sharedBlock->AllocRow(), SharedBlock::SHARED_BLOCK_INVALID_OPERATION);
    sharedBlock->mReadOnly = false;
    sharedBlock->mSize = 0;
    EXPECT_EQ(sharedBlock->AllocRow(), SharedBlock::SHARED_BLOCK_NO_MEMORY);
    sharedBlock->mSize = sizeof(SharedBlock::RowGroupHeader) + sizeof(SharedBlock::SharedBlockHeader) + 1;
    EXPECT_EQ(sharedBlock->Clear(), SharedBlock::SHARED_BLOCK_OK);
    LOG_INFO("AllocRowTest001::End");
}

/**
* @tc.name: FreeLastRowTest001
* @tc.desc: Test FreeLastRow function under read-only mode and with different row counts
* @tc.type: FUNC
* @tc.step:
    1. Create a SharedBlock instance, verify it is not null
    2. Set to read-only mode, call FreeLastRow() and check return code
    3. Set to read-write mode, set rowNums to 0, call FreeLastRow() and check return code
    4. Set rowNums to a positive value, call FreeLastRow() and check return code
    5. Call Clear() to clean up
* @tc.expect:
    - FreeLastRow() returns SHARED_BLOCK_INVALID_OPERATION in read-only mode
    - FreeLastRow() returns SHARED_BLOCK_OK when rowNums is 0
    - FreeLastRow() returns SHARED_BLOCK_OK when rowNums is positive
    - Clear() returns SHARED_BLOCK_OK
*/
HWTEST_F(SharedBlockTest, FreeLastRowTest001, TestSize.Level0)
{
    LOG_INFO("FreeLastRowTest001::Start");
    AppDataFwk::SharedBlock *sharedBlock = nullptr;
    EXPECT_EQ(SharedBlock::Create("name",
        sizeof(SharedBlock::RowGroupHeader) + sizeof(SharedBlock::SharedBlockHeader) + 1, sharedBlock),
        SharedBlock::SHARED_BLOCK_OK);
    EXPECT_NE(sharedBlock, nullptr);
    sharedBlock->mReadOnly = true;
    EXPECT_EQ(sharedBlock->FreeLastRow(), SharedBlock::SHARED_BLOCK_INVALID_OPERATION);
    sharedBlock->mReadOnly = false;
    auto temp = sharedBlock->mHeader->rowNums + 1;
    sharedBlock->mHeader->rowNums = 0;
    EXPECT_EQ(sharedBlock->FreeLastRow(), SharedBlock::SHARED_BLOCK_OK);
    sharedBlock->mHeader->rowNums = temp;
    EXPECT_EQ(sharedBlock->FreeLastRow(), SharedBlock::SHARED_BLOCK_OK);
    EXPECT_EQ(sharedBlock->Clear(), SharedBlock::SHARED_BLOCK_OK);
    LOG_INFO("FreeLastRowTest001::End");
}

/**
* @tc.name: AllocRowOffsetTest001
* @tc.desc: Test AllocRowOffset function with insufficient memory
* @tc.type: FUNC
* @tc.step:
    1. Create a SharedBlock instance, verify it is not null
    2. Save original mSize, set mSize to 0 (insufficient memory)
    3. Call AllocRowOffset() and check return value
    4. Restore mSize to original value, call Clear() to clean up
* @tc.expect:
    - AllocRowOffset() returns nullptr when mSize is 0 (insufficient memory)
    - Clear() returns SHARED_BLOCK_OK after restoring size
*/
HWTEST_F(SharedBlockTest, AllocRowOffsetTest001, TestSize.Level0)
{
    LOG_INFO("AllocRowOffsetTest001::Start");
    AppDataFwk::SharedBlock *sharedBlock = nullptr;
    EXPECT_EQ(SharedBlock::Create("name",
        sizeof(SharedBlock::RowGroupHeader) + sizeof(SharedBlock::SharedBlockHeader) + 1, sharedBlock),
        SharedBlock::SHARED_BLOCK_OK);
    EXPECT_NE(sharedBlock, nullptr);
    auto temp = sharedBlock->mSize;
    sharedBlock->mSize = 0;
    EXPECT_EQ(sharedBlock->AllocRowOffset(), nullptr);
    sharedBlock->mSize = temp;
    EXPECT_EQ(sharedBlock->Clear(), SharedBlock::SHARED_BLOCK_OK);
    LOG_INFO("AllocRowOffsetTest001::End");
}

/**
* @tc.name: GetCellUnitTest001
* @tc.desc: Test GetCellUnit function with invalid row/column indices and insufficient memory
* @tc.type: FUNC
* @tc.step:
    1. Create a SharedBlock instance, verify it is not null
    2. Save original rowNums, columnNums, and mSize
    3. Set rowNums=1, columnNums=1, mSize=0 (insufficient memory)
    4. Call GetCellUnit with invalid row (1) and valid column (0), check return value
    5. Call GetCellUnit with valid row (0) and invalid column (1), check return value
    6. Call GetCellUnit with valid row/column (0,0), check return value
    7. Restore original values, call Clear() to clean up
* @tc.expect: All GetCellUnit calls return nullptr under invalid conditions
*/
HWTEST_F(SharedBlockTest, GetCellUnitTest001, TestSize.Level0)
{
    LOG_INFO("GetCellUnitTest001::Start");
    AppDataFwk::SharedBlock *sharedBlock = nullptr;
    EXPECT_EQ(SharedBlock::Create("name",
        sizeof(SharedBlock::RowGroupHeader) + sizeof(SharedBlock::SharedBlockHeader) + 1, sharedBlock),
        SharedBlock::SHARED_BLOCK_OK);
    EXPECT_NE(sharedBlock, nullptr);
    auto rowTemp = sharedBlock->mHeader->rowNums;
    auto culTemp = sharedBlock->mHeader->columnNums;
    auto sizeTemp = sharedBlock->mSize;
    sharedBlock->mHeader->rowNums = 1;
    sharedBlock->mHeader->columnNums = 1;
    sharedBlock->mSize = 0;
    EXPECT_EQ(sharedBlock->GetCellUnit(1, 0), nullptr);
    EXPECT_EQ(sharedBlock->GetCellUnit(0, 1), nullptr);
    EXPECT_EQ(sharedBlock->GetCellUnit(0, 0), nullptr);
    sharedBlock->mHeader->rowNums = rowTemp;
    sharedBlock->mHeader->columnNums = culTemp;
    sharedBlock->mSize = sizeTemp;
    EXPECT_EQ(sharedBlock->Clear(), SharedBlock::SHARED_BLOCK_OK);
    LOG_INFO("GetCellUnitTest001::End");
}

/**
* @tc.name: PutBlobOrStringTest001
* @tc.desc: Test PutBlobOrString function under read-only mode and invalid indices
* @tc.type: FUNC
* @tc.step:
    1. Create a SharedBlock instance, verify it is not null
    2. Set to read-only mode, call PutBlobOrString with valid parameters and check return code
    3. Set to read-write mode, call PutBlobOrString with invalid row index and check return code
    4. Call Clear() to clean up
* @tc.expect:
    - Returns SHARED_BLOCK_INVALID_OPERATION in read-only mode
    - Returns SHARED_BLOCK_BAD_VALUE with invalid row index
    - Clear() returns SHARED_BLOCK_OK
*/
HWTEST_F(SharedBlockTest, PutBlobOrStringTest001, TestSize.Level0)
{
    LOG_INFO("PutBlobOrStringTest001::Start");
    AppDataFwk::SharedBlock *sharedBlock = nullptr;
    EXPECT_EQ(SharedBlock::Create("name",
        sizeof(SharedBlock::RowGroupHeader) + sizeof(SharedBlock::SharedBlockHeader) + 1, sharedBlock),
        SharedBlock::SHARED_BLOCK_OK);
    EXPECT_NE(sharedBlock, nullptr);
    sharedBlock->mReadOnly = true;
    std::string str = "string";
    EXPECT_EQ(sharedBlock->PutBlobOrString(sharedBlock->mHeader->rowNums, 1, &str, str.size(), 1),
        SharedBlock::SHARED_BLOCK_INVALID_OPERATION);
    sharedBlock->mReadOnly = false;
    EXPECT_EQ(sharedBlock->PutBlobOrString(sharedBlock->mHeader->rowNums, 1, &str, str.size(), 1),
        SharedBlock::SHARED_BLOCK_BAD_VALUE);
    EXPECT_EQ(sharedBlock->Clear(), SharedBlock::SHARED_BLOCK_OK);
    LOG_INFO("PutBlobOrStringTest001::End");
}

/**
* @tc.name: PutLongTest001
* @tc.desc: Test PutLong function under read-only mode and invalid indices
* @tc.type: FUNC
* @tc.step:
    1. Create a SharedBlock instance, verify it is not null
    2. Set to read-only mode, call PutLong with invalid indices and check return code
    3. Set to read-write mode, call PutLong with invalid indices and check return code
    4. Call Clear() to clean up
* @tc.expect:
    - Returns SHARED_BLOCK_INVALID_OPERATION in read-only mode
    - Returns SHARED_BLOCK_BAD_VALUE with invalid indices
    - Clear() returns SHARED_BLOCK_OK
*/
HWTEST_F(SharedBlockTest, PutLongTest001, TestSize.Level0)
{
    LOG_INFO("PutLongTest001::Start");
    AppDataFwk::SharedBlock *sharedBlock = nullptr;
    EXPECT_EQ(SharedBlock::Create("name",
        sizeof(SharedBlock::RowGroupHeader) + sizeof(SharedBlock::SharedBlockHeader) + 1, sharedBlock),
        SharedBlock::SHARED_BLOCK_OK);
    EXPECT_NE(sharedBlock, nullptr);
    int64_t temp = 0;
    sharedBlock->mReadOnly = true;
    EXPECT_EQ(
        sharedBlock->PutLong(sharedBlock->mHeader->rowNums, 1, temp), SharedBlock::SHARED_BLOCK_INVALID_OPERATION);
    sharedBlock->mReadOnly = false;
    EXPECT_EQ(sharedBlock->PutLong(sharedBlock->mHeader->rowNums, 1, temp), SharedBlock::SHARED_BLOCK_BAD_VALUE);
    EXPECT_EQ(sharedBlock->Clear(), SharedBlock::SHARED_BLOCK_OK);
    LOG_INFO("PutLongTest001::End");
}

/**
* @tc.name: PutDoubleTest001
* @tc.desc: Test PutDouble function under read-only mode and invalid indices
* @tc.type: FUNC
* @tc.step:
    1. Create a SharedBlock instance, verify it is not null
    2. Set to read-only mode, call PutDouble with invalid indices and check return code
    3. Set to read-write mode, call PutDouble with invalid indices and check return code
    4. Call Clear() to clean up
* @tc.expect:
    - Returns SHARED_BLOCK_INVALID_OPERATION in read-only mode
    - Returns SHARED_BLOCK_BAD_VALUE with invalid indices
    - Clear() returns SHARED_BLOCK_OK
*/
HWTEST_F(SharedBlockTest, PutDoubleTest001, TestSize.Level0)
{
    LOG_INFO("PutDoubleTest001::Start");
    AppDataFwk::SharedBlock *sharedBlock = nullptr;
    EXPECT_EQ(SharedBlock::Create("name",
        sizeof(SharedBlock::RowGroupHeader) + sizeof(SharedBlock::SharedBlockHeader) + 1, sharedBlock),
        SharedBlock::SHARED_BLOCK_OK);
    EXPECT_NE(sharedBlock, nullptr);
    double temp = 0;
    sharedBlock->mReadOnly = true;
    EXPECT_EQ(
        sharedBlock->PutDouble(sharedBlock->mHeader->rowNums, 1, temp), SharedBlock::SHARED_BLOCK_INVALID_OPERATION);
    sharedBlock->mReadOnly = false;
    EXPECT_EQ(sharedBlock->PutDouble(sharedBlock->mHeader->rowNums, 1, temp), SharedBlock::SHARED_BLOCK_BAD_VALUE);
    EXPECT_EQ(sharedBlock->Clear(), SharedBlock::SHARED_BLOCK_OK);
    LOG_INFO("PutDoubleTest001::End");
}

/**
* @tc.name: PutNullTest001
* @tc.desc: Test PutNull function under read-only mode and invalid indices
* @tc.type: FUNC
* @tc.step:
    1. Create a SharedBlock instance, verify it is not null
    2. Set to read-only mode, call PutNull with invalid indices and check return code
    3. Set to read-write mode, call PutNull with invalid indices and check return code
    4. Call Clear() to clean up
* @tc.expect:
    - Returns SHARED_BLOCK_INVALID_OPERATION in read-only mode
    - Returns SHARED_BLOCK_BAD_VALUE with invalid indices
    - Clear() returns SHARED_BLOCK_OK
*/
HWTEST_F(SharedBlockTest, PutNullTest001, TestSize.Level0)
{
    LOG_INFO("PutNullTest001::Start");
    AppDataFwk::SharedBlock *sharedBlock = nullptr;
    EXPECT_EQ(SharedBlock::Create("name",
        sizeof(SharedBlock::RowGroupHeader) + sizeof(SharedBlock::SharedBlockHeader) + 1, sharedBlock),
        SharedBlock::SHARED_BLOCK_OK);
    EXPECT_NE(sharedBlock, nullptr);
    sharedBlock->mReadOnly = true;
    EXPECT_EQ(sharedBlock->PutNull(sharedBlock->mHeader->rowNums, 1), SharedBlock::SHARED_BLOCK_INVALID_OPERATION);
    sharedBlock->mReadOnly = false;
    EXPECT_EQ(sharedBlock->PutNull(sharedBlock->mHeader->rowNums, 1), SharedBlock::SHARED_BLOCK_BAD_VALUE);
    EXPECT_EQ(sharedBlock->Clear(), SharedBlock::SHARED_BLOCK_OK);
    LOG_INFO("PutNullTest001::End");
}

/**
* @tc.name: SetRawDataTest001
* @tc.desc: Test SetRawData function with invalid size and insufficient memory
* @tc.type: FUNC
* @tc.step:
    1. Create a SharedBlock instance, verify it is not null
    2. Save original mSize, set mSize to 0 (insufficient memory)
    3. Call SetRawData with header data and size 0, check return code
    4. Call SetRawData with header data and valid size, check return code
    5. Restore mSize to original value, call SetRawData with valid size, check return code
    6. Call Clear() to clean up
* @tc.expect:
    - Returns SHARED_BLOCK_INVALID_OPERATION when size is 0
    - Returns SHARED_BLOCK_NO_MEMORY when mSize is 0
    - Returns SHARED_BLOCK_OK when mSize is valid
    - Clear() returns SHARED_BLOCK_OK
*/
HWTEST_F(SharedBlockTest, SetRawDataTest001, TestSize.Level0)
{
    LOG_INFO("SetRawDataTest001::Start");
    AppDataFwk::SharedBlock *sharedBlock = nullptr;
    EXPECT_EQ(SharedBlock::Create("name",
        sizeof(SharedBlock::RowGroupHeader) + sizeof(SharedBlock::SharedBlockHeader) + 1, sharedBlock),
        SharedBlock::SHARED_BLOCK_OK);
    EXPECT_NE(sharedBlock, nullptr);
    auto sizeTemp = sharedBlock->mSize;
    sharedBlock->mSize = 0;
    SharedBlock::SharedBlockHeader mHeader;
    int result = memcpy_s(&mHeader, sizeof(mHeader), sharedBlock->mHeader, sizeof(mHeader));
    EXPECT_EQ(result, 0);
    EXPECT_EQ(sharedBlock->SetRawData(&mHeader, 0), SharedBlock::SHARED_BLOCK_INVALID_OPERATION);
    EXPECT_EQ(sharedBlock->SetRawData(&mHeader, sizeof(mHeader)), SharedBlock::SHARED_BLOCK_NO_MEMORY);
    sharedBlock->mSize = sizeTemp;
    EXPECT_EQ(sharedBlock->SetRawData(&mHeader, sizeof(mHeader)), SharedBlock::SHARED_BLOCK_OK);
    EXPECT_EQ(sharedBlock->Clear(), SharedBlock::SHARED_BLOCK_OK);
    LOG_INFO("SetRawDataTest001::End");
}

/**
 * @tc.name: AbnormalBranchTest_mReadOnlyInvalid_Test_001
 * @tc.desc: Verify invalid operations on read-only SharedBlock
 * @tc.type: FUNC
 * @tc.precon: None
 * @tc.step:
    1. Create read-only SharedBlock instance
    2. Attempt modification operations (Clear, SetColumnNum, AllocRow, etc.)
    3. Check return values of all operations
 * @tc.expect:
    1. All modification operations return SHARED_BLOCK_INVALID_OPERATION
    2. Init operation succeeds
 */
HWTEST_F(SharedBlockTest, MReadOnlyInvalid_Test_001, TestSize.Level0)
{
    LOG_INFO("MReadOnlyInvalid_Test_001::Start");
    std::string name = "Test Shared\0";
    bool readOnly = true;
    int32_t size = 1024;
    sptr<Ashmem> ashmem = Ashmem::CreateAshmem(name.c_str(), size);
    ashmem->MapReadAndWriteAshmem();
    AppDataFwk::SharedBlock temp(name, ashmem, size, readOnly);
    int result = temp.Init();
    EXPECT_TRUE(result);
    result = temp.Clear();
    EXPECT_EQ(result, AppDataFwk::SharedBlock::SHARED_BLOCK_INVALID_OPERATION);
    result = temp.SetColumnNum(1);
    EXPECT_EQ(result, AppDataFwk::SharedBlock::SHARED_BLOCK_INVALID_OPERATION);
    result = temp.AllocRow();
    EXPECT_EQ(result, AppDataFwk::SharedBlock::SHARED_BLOCK_INVALID_OPERATION);
    result = temp.FreeLastRow();
    EXPECT_EQ(result, AppDataFwk::SharedBlock::SHARED_BLOCK_INVALID_OPERATION);
    int64_t intValue = 0;
    result = temp.PutLong(1, 1, intValue);
    EXPECT_EQ(result, AppDataFwk::SharedBlock::SHARED_BLOCK_INVALID_OPERATION);
    double doubleValue = 0.0;
    result = temp.PutDouble(1, 1, doubleValue);
    EXPECT_EQ(result, AppDataFwk::SharedBlock::SHARED_BLOCK_INVALID_OPERATION);
    result = temp.PutNull(1, 1);
    EXPECT_EQ(result, AppDataFwk::SharedBlock::SHARED_BLOCK_INVALID_OPERATION);
    result = temp.SetRawData(nullptr, 0);
    EXPECT_EQ(result, AppDataFwk::SharedBlock::SHARED_BLOCK_INVALID_OPERATION);
    LOG_INFO("MReadOnlyInvalid_Test_001::End");
}

/**
* @tc.name: AllocTest001
* @tc.desc: Test Alloc function with different mSize values and check return values
* @tc.type: FUNC
* @tc.step:
    1. Create a SharedBlock instance, verify it is not null
    2. Set unusedOffset in the header to 1
    3. Set mSize to 0 and call Alloc with size 2, check return value is 0
    4. Set mSize to size + unusedOffset and call Alloc with size 2, check return value is 0
    5. Set mSize to size + unusedOffset + 1 and call Alloc with size 2, check return value is 1
    6. Call Clear() to clean up, check return value is SHARED_BLOCK_OK
* @tc.expect:
    - Returns 0 when mSize is 0
    - Returns 0 when mSize is exactly size + unusedOffset
    - Returns 1 when mSize is size + unusedOffset + 1
    - Clear() returns SHARED_BLOCK_OK
*/
HWTEST_F(SharedBlockTest, AllocTest001, TestSize.Level0)
{
    LOG_INFO("AllocTest001::Start");
    AppDataFwk::SharedBlock *sharedBlock = nullptr;
    EXPECT_EQ(SharedBlock::Create("name",
        sizeof(SharedBlock::RowGroupHeader) + sizeof(SharedBlock::SharedBlockHeader) + 1, sharedBlock),
        SharedBlock::SHARED_BLOCK_OK);
    EXPECT_NE(sharedBlock, nullptr);
    sharedBlock->mHeader->unusedOffset = 1;
    sharedBlock->mSize = 0;
    uint32_t size = 2;
    EXPECT_EQ(sharedBlock->Alloc(size, false), 0);
    sharedBlock->mSize = size + sharedBlock->mHeader->unusedOffset;
    EXPECT_EQ(sharedBlock->Alloc(size, false), 0);
    sharedBlock->mSize = size + sharedBlock->mHeader->unusedOffset + 1;
    EXPECT_EQ(sharedBlock->Alloc(size, false), 1);
    sharedBlock->mSize = sizeof(SharedBlock::RowGroupHeader) + sizeof(SharedBlock::SharedBlockHeader) + 1;
    EXPECT_EQ(sharedBlock->Clear(), SharedBlock::SHARED_BLOCK_OK);
    LOG_INFO("AllocTest001::End");
}
} // namespace DataShare
} // namespace OHOS