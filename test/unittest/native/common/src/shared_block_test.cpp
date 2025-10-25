/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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
 * @tc.desc: Test the Init function of the SharedBlock class when the start address (startAddr_) of the associated
 *           Ashmem instance is set to nullptr, verifying if initialization fails as expected.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports instantiation of Ashmem and SharedBlock objects without initialization errors.
    2. The Ashmem class allows explicit modification of the startAddr_ member variable to nullptr.
    3. The SharedBlock class can be initialized with an Ashmem instance, name, offset, and read-only flag.
    4. The SharedBlock::Init() method returns a boolean value indicating initialization success or failure.
 * @tc.step:
    1. Create an Ashmem instance named "ahsmem" with a size equal to the size of SharedBlock::SharedBlockHeader.
    2. Set the startAddr_ member of the created Ashmem instance to nullptr.
    3. Create a SharedBlock instance with the name "name", the modified Ashmem instance, offset 0, and read-only flag
       true.
    4. Call the Init() method on the SharedBlock instance.
    5. Check the boolean return value of the Init() method.
 * @tc.expect:
    1. The SharedBlock::Init() method returns false (initialization fails due to null Ashmem start address).
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
 * @tc.desc: Test the CreateSharedBlock function of the SharedBlock class when the start address (startAddr_) of the
 *           associated Ashmem instance is set to nullptr, verifying the returned error code.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports instantiation of Ashmem objects and declaration of null SharedBlock pointers.
    2. The Ashmem class allows explicit modification of the startAddr_ member variable to nullptr.
    3. The SharedBlock::CreateSharedBlock function accepts parameters including name, size, Ashmem instance, and
       a SharedBlock pointer, returning an error code.
    4. The SharedBlock::SHARED_BLOCK_ASHMEM_ERROR constant is predefined and accessible.
 * @tc.step:
    1. Create an Ashmem instance named "ahsmem" with a size equal to the size of SharedBlock::SharedBlockHeader.
    2. Set the startAddr_ member of the Ashmem instance to nullptr.
    3. Declare a null pointer of type AppDataFwk::SharedBlock (named sharedBlock).
    4. Call SharedBlock::CreateSharedBlock with parameters: name "name", size equal to SharedBlock::SharedBlockHeader,
       the modified Ashmem instance, and the null sharedBlock pointer.
    5. Check the error code returned by CreateSharedBlock.
 * @tc.expect:
    1. The SharedBlock::CreateSharedBlock function returns SharedBlock::SHARED_BLOCK_ASHMEM_ERROR.
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
 * @tc.desc: Test the static Create function of the SharedBlock class when a negative value is passed as the size
 *           parameter, verifying the returned error code.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports declaration of null SharedBlock pointers and calling static SharedBlock methods.
    2. The SharedBlock::Create static function accepts parameters including name, size, and a SharedBlock pointer,
       returning an error code.
    3. The SharedBlock::SHARED_BLOCK_ASHMEM_ERROR constant is predefined and accessible.
 * @tc.step:
    1. Declare a null pointer of type AppDataFwk::SharedBlock (named sharedBlock).
    2. Call the static function SharedBlock::Create with parameters: name "name", size -1 (negative value),
       and the null sharedBlock pointer.
    3. Check the error code returned by SharedBlock::Create.
 * @tc.expect:
    1. The SharedBlock::Create static function returns SharedBlock::SHARED_BLOCK_ASHMEM_ERROR.
 */
HWTEST_F(SharedBlockTest, CreateTest001, TestSize.Level0)
{
    LOG_INFO("CreateTest001::Start");
    AppDataFwk::SharedBlock *sharedBlock = nullptr;
    EXPECT_EQ(SharedBlock::Create("name", -1, sharedBlock), SharedBlock::SHARED_BLOCK_ASHMEM_ERROR);
    LOG_INFO("CreateTest001::End");
}

/**
 * @tc.name: CreateTest001
 * @tc.desc: Test the static Create function of the SharedBlock class when a negative value is passed as the size
 *           parameter, verifying the returned error code.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports declaration of null SharedBlock pointers and calling static SharedBlock methods.
    2. The SharedBlock::Create static function accepts parameters including name, size, and a SharedBlock pointer,
       returning an error code.
    3. The SharedBlock::SHARED_BLOCK_ASHMEM_ERROR constant is predefined and accessible.
 * @tc.step:
    1. Declare a null pointer of type AppDataFwk::SharedBlock (named sharedBlock).
    2. Call the static function SharedBlock::Create with parameters: name "name", size -1 (negative value),
       and the null sharedBlock pointer.
    3. Check the error code returned by SharedBlock::Create.
 * @tc.expect:
    1. The SharedBlock::Create static function returns SharedBlock::SHARED_BLOCK_ASHMEM_ERROR.
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
 * @tc.desc: Test the Clear function of the SharedBlock class under three states: read-only mode, read-write mode
 *           with insufficient size, and read-write mode with valid size, verifying the returned error codes.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports creating SharedBlock instances via SharedBlock::Create with valid size.
    2. The SharedBlock class allows modification of the mReadOnly member (read-only flag) and mSize member (size).
    3. The SharedBlock::Clear() method returns an error code; predefined codes (SHARED_BLOCK_INVALID_OPERATION,
       SHARED_BLOCK_BAD_VALUE, SHARED_BLOCK_OK) are accessible.
 * @tc.step:
    1. Call SharedBlock::Create to create a SharedBlock instance (named sharedBlock) with name "name" and size equal
       to (sizeof(SharedBlock) + sizeof(SharedBlock::SharedBlockHeader)), verifying the instance is not null.
    2. Set the mReadOnly member of sharedBlock to true, call Clear(), and check the returned error code.
    3. Set mReadOnly to false, set mSize to sizeof(SharedBlock::SharedBlockHeader) (insufficient size), call Clear(),
       and check the error code.
    4. Restore mSize to the original valid value (sizeof(SharedBlock) + sizeof(SharedBlock::SharedBlockHeader)),
       call Clear(), and check the error code.
 * @tc.expect:
    1. Clear() returns SharedBlock::SHARED_BLOCK_INVALID_OPERATION when in read-only mode.
    2. Clear() returns SharedBlock::SHARED_BLOCK_BAD_VALUE when in read-write mode with insufficient size.
    3. Clear() returns SharedBlock::SHARED_BLOCK_OK when in read-write mode with valid size.
 */
HWTEST_F(SharedBlockTest, ClearTest001, TestSize.Level0)
{
    LOG_INFO("ClearTest001::Start");
    AppDataFwk::SharedBlock *sharedBlock = nullptr;
    EXPECT_EQ(SharedBlock::Create("name", sizeof(SharedBlock) + sizeof(SharedBlock::SharedBlockHeader), sharedBlock),
        SharedBlock::SHARED_BLOCK_OK);
    EXPECT_NE(sharedBlock, nullptr);
    sharedBlock->mReadOnly = true;
    EXPECT_EQ(sharedBlock->Clear(), SharedBlock::SHARED_BLOCK_INVALID_OPERATION);
    sharedBlock->mReadOnly = false;
    sharedBlock->mSize = sizeof(SharedBlock::SharedBlockHeader);
    EXPECT_EQ(sharedBlock->Clear(), SharedBlock::SHARED_BLOCK_BAD_VALUE);
    sharedBlock->mSize = sizeof(SharedBlock) + sizeof(SharedBlock::SharedBlockHeader);
    EXPECT_EQ(sharedBlock->Clear(), SharedBlock::SHARED_BLOCK_OK);
    LOG_INFO("ClearTest001::End");
}

/**
 * @tc.name: SetColumnNumTest001
 * @tc.desc: Test the SetColumnNum function of the SharedBlock class under various conditions: read-only mode,
 *           invalid row/column number combinations, max column limit, and valid configurations, verifying error codes.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports creating SharedBlock instances via SharedBlock::Create with valid size.
    2. The SharedBlock class allows modification of mReadOnly and access to mHeader (SharedBlockHeader) members
       (columnNums and rowNums).
    3. Predefined error codes (SHARED_BLOCK_INVALID_OPERATION, SHARED_BLOCK_OK) and max column limit (32768) are
       accessible.
 * @tc.step:
    1. Create a SharedBlock instance (sharedBlock) via SharedBlock::Create (name "name", valid size), verifying it is
       not null.
    2. Set mReadOnly to true, call SetColumnNum(1), and check the error code.
    3. Set mReadOnly to false, configure mHeader->columnNums = 1 and mHeader->rowNums = 1, call SetColumnNum(0),
       and check the error code.
    4. Set mHeader->columnNums = 0 and mHeader->rowNums = 1, call SetColumnNum(1), and check the error code.
    5. Set mHeader->columnNums = 32768 and mHeader->rowNums = 32768, call SetColumnNum(32768), and check the errorcode.
    6. Configure mHeader->columnNums = 1 and mHeader->rowNums = 1, call SetColumnNum(1), and check the error code.
    7. Set mHeader->columnNums = 0 and mHeader->rowNums = 0, call SetColumnNum(0), and check the error code.
 * @tc.expect:
    1. SetColumnNum returns SHARED_BLOCK_INVALID_OPERATION in read-only mode.
    2. SetColumnNum returns SHARED_BLOCK_INVALID_OPERATION for conflicting row/column number combinations.
    3. SetColumnNum returns SHARED_BLOCK_OK for valid configurations (matching row/column nums or both zero).
 */
HWTEST_F(SharedBlockTest, SetColumnNumTest001, TestSize.Level0)
{
    LOG_INFO("SetColumnNumTest001::Start");
    AppDataFwk::SharedBlock *sharedBlock = nullptr;
    EXPECT_EQ(SharedBlock::Create("name", sizeof(SharedBlock) + sizeof(SharedBlock::SharedBlockHeader), sharedBlock),
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
 * @tc.desc: Test the AllocRow function of the SharedBlock class under two states: read-only mode and read-write mode
 *           with insufficient memory (mSize = 0), verifying the returned error codes, and clean up with Clear().
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports creating SharedBlock instances via SharedBlock::Create with valid size.
    2. The SharedBlock class allows modification of mReadOnly (read-only flag) and mSize (memory size).
    3. Predefined error codes (SHARED_BLOCK_INVALID_OPERATION, SHARED_BLOCK_NO_MEMORY, SHARED_BLOCK_OK) are accessible.
 * @tc.step:
    1. Create a SharedBlock instance (sharedBlock) via SharedBlock::Create (name "name", valid size), verifying it is
       not null.
    2. Set mReadOnly to true, call AllocRow(), and check the returned error code.
    3. Set mReadOnly to false, set mSize to 0 (insufficient memory), call AllocRow(), and check the error code.
    4. Restore mSize to the original valid value, call Clear(), and check the error code.
 * @tc.expect:
    1. AllocRow() returns SharedBlock::SHARED_BLOCK_INVALID_OPERATION when in read-only mode.
    2. AllocRow() returns SharedBlock::SHARED_BLOCK_NO_MEMORY when in read-write mode with mSize = 0.
    3. Clear() returns SharedBlock::SHARED_BLOCK_OK after restoring mSize to valid value.
 */
HWTEST_F(SharedBlockTest, AllocRow, TestSize.Level0)
{
    LOG_INFO("AllocRowTest001::Start");
    AppDataFwk::SharedBlock *sharedBlock = nullptr;
    EXPECT_EQ(SharedBlock::Create("name", sizeof(SharedBlock) + sizeof(SharedBlock::SharedBlockHeader), sharedBlock),
        SharedBlock::SHARED_BLOCK_OK);
    EXPECT_NE(sharedBlock, nullptr);
    sharedBlock->mReadOnly = true;
    EXPECT_EQ(sharedBlock->AllocRow(), SharedBlock::SHARED_BLOCK_INVALID_OPERATION);
    sharedBlock->mReadOnly = false;
    sharedBlock->mSize = 0;
    EXPECT_EQ(sharedBlock->AllocRow(), SharedBlock::SHARED_BLOCK_NO_MEMORY);
    sharedBlock->mSize = sizeof(SharedBlock) + sizeof(SharedBlock::SharedBlockHeader);
    EXPECT_EQ(sharedBlock->Clear(), SharedBlock::SHARED_BLOCK_OK);
    LOG_INFO("AllocRowTest001::End");
}

/**
 * @tc.name: FreeLastRowTest001
 * @tc.desc: Test the FreeLastRow function of the SharedBlock class under three states: read-only mode, read-write mode
 *           with rowNums = 0, and read-write mode with positive rowNums, verifying the returned error codes.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports creating SharedBlock instances via SharedBlock::Create with valid size.
    2. The SharedBlock class allows modification of mReadOnly and access to mHeader->rowNums (number of rows).
    3. Predefined error codes (SHARED_BLOCK_INVALID_OPERATION, SHARED_BLOCK_OK) are accessible.
 * @tc.step:
    1. Create a SharedBlock instance (sharedBlock) via SharedBlock::Create (name "name", valid size), verifying it is
       not null.
    2. Set mReadOnly to true, call FreeLastRow(), and check the error code.
    3. Set mReadOnly to false, save the original mHeader->rowNums to a temporary variable, set rowNums to 0,
       call FreeLastRow(), and check the error code.
    4. Restore rowNums to the original positive value, call FreeLastRow(), and check the error code.
    5. Call Clear() to clean up, and check the error code.
 * @tc.expect:
    1. FreeLastRow() returns SharedBlock::SHARED_BLOCK_INVALID_OPERATION when in read-only mode.
    2. FreeLastRow() returns SharedBlock::SHARED_BLOCK_OK when rowNums = 0 (no rows to free).
    3. FreeLastRow() returns SharedBlock::SHARED_BLOCK_OK when rowNums is positive (frees last row).
    4. Clear() returns SharedBlock::SHARED_BLOCK_OK.
 */
HWTEST_F(SharedBlockTest, FreeLastRowTest001, TestSize.Level0)
{
    LOG_INFO("FreeLastRowTest001::Start");
    AppDataFwk::SharedBlock *sharedBlock = nullptr;
    EXPECT_EQ(SharedBlock::Create("name", sizeof(SharedBlock) + sizeof(SharedBlock::SharedBlockHeader), sharedBlock),
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
 * @tc.desc: Test the AllocRowOffset function of the SharedBlock class when the mSize member is set to 0 (insufficient
 *           memory), verifying the returned value, and clean up with Clear() after restoring mSize.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports creating SharedBlock instances via SharedBlock::Create with valid size.
    2. The SharedBlock class allows modification of the mSize member (memory size).
    3. The SharedBlock::AllocRowOffset() method returns a pointer (nullptr on failure); Clear() returns
       SHARED_BLOCK_OK.
 * @tc.step:
    1. Create a SharedBlock instance (sharedBlock) via SharedBlock::Create (name "name", valid size), verifying it is
       not null.
    2. Save the original mSize value to a temporary variable, then set mSize to 0 (insufficient memory).
    3. Call AllocRowOffset() and check if the returned pointer is nullptr.
    4. Restore mSize to the original valid value, call Clear(), and check the error code.
 * @tc.expect:
    1. AllocRowOffset() returns nullptr when mSize is 0 (insufficient memory).
    2. Clear() returns SharedBlock::SHARED_BLOCK_OK after restoring mSize to valid value.
 */
HWTEST_F(SharedBlockTest, AllocRowOffsetTest001, TestSize.Level0)
{
    LOG_INFO("AllocRowOffsetTest001::Start");
    AppDataFwk::SharedBlock *sharedBlock = nullptr;
    EXPECT_EQ(SharedBlock::Create("name", sizeof(SharedBlock) + sizeof(SharedBlock::SharedBlockHeader), sharedBlock),
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
 * @tc.desc: Test the GetCellUnit function of the SharedBlock class under invalid conditions: invalid row index,
 *           invalid column index, and insufficient memory (mSize = 0), verifying if it returns nullptr in all cases.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports creating SharedBlock instances via SharedBlock::Create with valid size.
    2. The SharedBlock class allows modification of mSize and access to mHeader->rowNums/columnNums.
    3. The SharedBlock::GetCellUnit() method returns a pointer (nullptr on invalid conditions).
 * @tc.step:
    1. Create a SharedBlock instance (sharedBlock) via SharedBlock::Create (name "name", valid size), verifying it is
       not null.
    2. Save the original values of mHeader->rowNums, mHeader->columnNums, and mSize to temporary variables.
    3. Set mHeader->rowNums = 1, mHeader->columnNums = 1, and mSize = 0 (insufficient memory).
    4. Call GetCellUnit(1, 0) (invalid row index) and check if the return is nullptr.
    5. Call GetCellUnit(0, 1) (invalid column index) and check if the return is nullptr.
    6. Call GetCellUnit(0, 0) (valid indices but insufficient memory) and check if the return is nullptr.
    7. Restore all original values, call Clear(), and check the error code.
 * @tc.expect:
    1. All calls to GetCellUnit (invalid row, invalid column, valid indices with insufficient memory) return nullptr.
    2. Clear() returns SharedBlock::SHARED_BLOCK_OK after restoring original values.
 */
HWTEST_F(SharedBlockTest, GetCellUnitTest001, TestSize.Level0)
{
    LOG_INFO("GetCellUnitTest001::Start");
    AppDataFwk::SharedBlock *sharedBlock = nullptr;
    EXPECT_EQ(SharedBlock::Create("name", sizeof(SharedBlock) + sizeof(SharedBlock::SharedBlockHeader), sharedBlock),
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
 * @tc.desc: Test the PutBlobOrString function of the SharedBlock class under two states: read-only mode and read-write
 *           mode with invalid row index, verifying the returned error codes, and clean up with Clear().
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports creating SharedBlock instances via SharedBlock::Create with valid size.
    2. The SharedBlock class allows modification of mReadOnly and access to mHeader->rowNums.
    3. Predefined error codes (SHARED_BLOCK_INVALID_OPERATION, SHARED_BLOCK_BAD_VALUE, SHARED_BLOCK_OK) are accessible.
 * @tc.step:
    1. Create a SharedBlock instance (sharedBlock) via SharedBlock::Create (name "name", valid size), verifying it is
       not null.
    2. Set mReadOnly to true, define a test string "string", call PutBlobOrString with parameters (mHeader->rowNums, 1,
       &str, str.size(), 1), and check the error code.
    3. Set mReadOnly to false, call PutBlobOrString with the same parameters (invalid row index), and check the
       errorcode.
    4. Call Clear() to clean up, and check the error code.
 * @tc.expect:
    1. PutBlobOrString returns SharedBlock::SHARED_BLOCK_INVALID_OPERATION when in read-only mode.
    2. PutBlobOrString returns SharedBlock::SHARED_BLOCK_BAD_VALUE when in read-write mode with invalid row index.
    3. Clear() returns SharedBlock::SHARED_BLOCK_OK.
 */
HWTEST_F(SharedBlockTest, PutBlobOrStringTest001, TestSize.Level0)
{
    LOG_INFO("PutBlobOrStringTest001::Start");
    AppDataFwk::SharedBlock *sharedBlock = nullptr;
    EXPECT_EQ(SharedBlock::Create("name", sizeof(SharedBlock) + sizeof(SharedBlock::SharedBlockHeader), sharedBlock),
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
 * @tc.desc: Test the PutLong function of the SharedBlock class under two states: read-only mode and read-write mode
 *           with invalid indices, verifying the returned error codes, and clean up with Clear().
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports creating SharedBlock instances via SharedBlock::Create with valid size.
    2. The SharedBlock class allows modification of mReadOnly and access to mHeader->rowNums.
    3. Predefined error codes (SHARED_BLOCK_INVALID_OPERATION, SHARED_BLOCK_BAD_VALUE, SHARED_BLOCK_OK) are accessible.
 * @tc.step:
    1. Create a SharedBlock instance (sharedBlock) via SharedBlock::Create (name "name", valid size), verifying it is
       not null.
    2. Define an int64_t variable (temp = 0), set mReadOnly to true, call PutLong(mHeader->rowNums, 1, temp),
       and check the error code.
    3. Set mReadOnly to false, call PutLong(mHeader->rowNums, 1, temp) (invalid row index), and check the error code.
    4. Call Clear() to clean up, and check the error code.
 * @tc.expect:
    1. PutLong returns SharedBlock::SHARED_BLOCK_INVALID_OPERATION when in read-only mode.
    2. PutLong returns SharedBlock::SHARED_BLOCK_BAD_VALUE when in read-write mode with invalid indices.
    3. Clear() returns SharedBlock::SHARED_BLOCK_OK.
 */
HWTEST_F(SharedBlockTest, PutLongTest001, TestSize.Level0)
{
    LOG_INFO("PutLongTest001::Start");
    AppDataFwk::SharedBlock *sharedBlock = nullptr;
    EXPECT_EQ(SharedBlock::Create("name", sizeof(SharedBlock) + sizeof(SharedBlock::SharedBlockHeader), sharedBlock),
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
 * @tc.desc: Test the PutDouble function of the SharedBlock class under two states: read-only mode and read-write mode
 *           with invalid indices, verifying the returned error codes, and clean up with Clear().
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports creating SharedBlock instances via SharedBlock::Create with valid size.
    2. The SharedBlock class allows modification of mReadOnly and access to mHeader->rowNums.
    3. Predefined error codes (SHARED_BLOCK_INVALID_OPERATION, SHARED_BLOCK_BAD_VALUE, SHARED_BLOCK_OK) are accessible.
 * @tc.step:
    1. Create a SharedBlock instance (sharedBlock) via SharedBlock::Create (name "name", valid size), verifying it is
       not null.
    2. Define a double variable (temp = 0.0), set mReadOnly to true, call PutDouble(mHeader->rowNums, 1, temp),
       and check the error code.
    3. Set mReadOnly to false, call PutDouble(mHeader->rowNums, 1, temp) (invalid row index), and check the error code.
    4. Call Clear() to clean up, and check the error code.
 * @tc.expect:
    1. PutDouble returns SharedBlock::SHARED_BLOCK_INVALID_OPERATION when in read-only mode.
    2. PutDouble returns SharedBlock::SHARED_BLOCK_BAD_VALUE when in read-write mode with invalid indices.
    3. Clear() returns SharedBlock::SHARED_BLOCK_OK.
 */
HWTEST_F(SharedBlockTest, PutDoubleTest001, TestSize.Level0)
{
    LOG_INFO("PutDoubleTest001::Start");
    AppDataFwk::SharedBlock *sharedBlock = nullptr;
    EXPECT_EQ(SharedBlock::Create("name", sizeof(SharedBlock) + sizeof(SharedBlock::SharedBlockHeader), sharedBlock),
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
 * @tc.desc: Test the PutNull function of the SharedBlock class under two states: read-only mode and read-write mode
 *           with invalid indices, verifying the returned error codes, and clean up with Clear().
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports creating SharedBlock instances via SharedBlock::Create with valid size.
    2. The SharedBlock class allows modification of mReadOnly and access to mHeader->rowNums.
    3. Predefined error codes (SHARED_BLOCK_INVALID_OPERATION, SHARED_BLOCK_BAD_VALUE, SHARED_BLOCK_OK) are accessible.
 * @tc.step:
    1. Create a SharedBlock instance (sharedBlock) via SharedBlock::Create (name "name", valid size), verifying it is
       not null.
    2. Set mReadOnly to true, call PutNull(mHeader->rowNums, 1) (invalid indices), and check the error code.
    3. Set mReadOnly to false, call PutNull(mHeader->rowNums, 1) (invalid indices), and check the error code.
    4. Call Clear() to clean up, and check the error code.
 * @tc.expect:
    1. PutNull returns SharedBlock::SHARED_BLOCK_INVALID_OPERATION when in read-only mode.
    2. PutNull returns SharedBlock::SHARED_BLOCK_BAD_VALUE when in read-write mode with invalid indices.
    3. Clear() returns SharedBlock::SHARED_BLOCK_OK.
 */
HWTEST_F(SharedBlockTest, PutNullTest001, TestSize.Level0)
{
    LOG_INFO("PutNullTest001::Start");
    AppDataFwk::SharedBlock *sharedBlock = nullptr;
    EXPECT_EQ(SharedBlock::Create("name", sizeof(SharedBlock) + sizeof(SharedBlock::SharedBlockHeader), sharedBlock),
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
 * @tc.desc: Test the SetRawData function of the SharedBlock class under three conditions: size = 0, mSize = 0
 *           (insufficient memory), and valid mSize, verifying the returned error codes, and clean up with Clear().
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports creating SharedBlock instances via SharedBlock::Create with valid size.
    2. The SharedBlock class allows modification of mSize and access to mHeader (SharedBlockHeader) for data copying.
    3. Predefined error codes (SHARED_BLOCK_INVALID_OPERATION, SHARED_BLOCK_NO_MEMORY, SHARED_BLOCK_OK) are accessible.
 * @tc.step:
    1. Create a SharedBlock instance (sharedBlock) via SharedBlock::Create (name "name", valid size), verifying it is
       not null.
    2. Save the original mSize to a temporary variable, set mSize to 0 (insufficient memory), and create a
       SharedBlock::SharedBlockHeader variable (mHeader) to copy data from sharedBlock->mHeader.
    3. Call SetRawData(&mHeader, 0) (size = 0) and check the error code.
    4. Call SetRawData(&mHeader, sizeof(mHeader)) (valid size but mSize = 0) and check the error code.
    5. Restore mSize to the original valid value, call SetRawData(&mHeader, sizeof(mHeader)), and check the error code.
    6. Call Clear() to clean up, and check the error code.
 * @tc.expect:
    1. SetRawData returns SharedBlock::SHARED_BLOCK_INVALID_OPERATION when the input size is 0.
    2. SetRawData returns SharedBlock::SHARED_BLOCK_NO_MEMORY when mSize = 0 (insufficient memory).
    3. SetRawData returns SharedBlock::SHARED_BLOCK_OK when mSize is valid.
    4. Clear() returns SharedBlock::SHARED_BLOCK_OK.
 */
HWTEST_F(SharedBlockTest, SetRawDataTest001, TestSize.Level0)
{
    LOG_INFO("SetRawDataTest001::Start");
    AppDataFwk::SharedBlock *sharedBlock = nullptr;
    EXPECT_EQ(SharedBlock::Create("name", sizeof(SharedBlock) + sizeof(SharedBlock::SharedBlockHeader), sharedBlock),
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
} // namespace DataShare
} // namespace OHOS