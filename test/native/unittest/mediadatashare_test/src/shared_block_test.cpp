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
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void SharedBlockTest::SetUpTestCase(void)
{
}
void SharedBlockTest::TearDownTestCase(void)
{
}
void SharedBlockTest::SetUp(void)
{
}
void SharedBlockTest::TearDown(void)
{
}

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

HWTEST_F(SharedBlockTest, CreateTest001, TestSize.Level0)
{
    LOG_INFO("CreateTest001::Start");
    AppDataFwk::SharedBlock *sharedBlock = nullptr;
    EXPECT_EQ(SharedBlock::Create("name", -1, sharedBlock), SharedBlock::SHARED_BLOCK_ASHMEM_ERROR);
    LOG_INFO("CreateTest001::End");
}

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