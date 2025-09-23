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
#define LOG_TAG "datashare_itypes_utils_test"

#include <gtest/gtest.h>
#include <unistd.h>
#include "datashare_errno.h"
#include "datashare_itypes_utils.h"
#include "datashare_log.h"
#include "datashare_abs_result_set.h"
#include "datashare_result_set.h"
#include "ikvstore_data_service.h"
#include "ipc_types.h"
#include "ishared_result_set_stub.h"
#include "itypes_util.h"
#include "message_parcel.h"

namespace OHOS {
namespace DataShare {
using namespace testing::ext;
class DatashareItypesUtilsTest : public testing::Test {
public:
    static void SetUpTestCase(void){};
    static void TearDownTestCase(void){};
    void SetUp(){};
    void TearDown(){};
};

RdbChangeNode CreateRdbChangeNode()
{
    TemplateId templateId = TemplateId{123, "com.acts.datasharetest"};
    RdbChangeNode changeNode = RdbChangeNode{
        "datashare:///com.acts.datasharetest",
        templateId,
        {"deleted", "inserted", "updated"},
        false,
        nullptr,
        8
    };
    return changeNode;
}

bool operator==(const RdbChangeNode node1, const RdbChangeNode node2)
{
    bool firstPart = node1.uri_ == node2.uri_
        && node1.templateId_ == node2.templateId_
        && node1.data_ == node2.data_
        && node1.isSharedMemory_ == node2.isSharedMemory_
        && node1.size_ == node2.size_;
    if (node1.memory_ == nullptr && node2.memory_ == nullptr) {
        return firstPart && true;
    } else if (node1.memory_ != nullptr && node2.memory_ != nullptr) {
        return firstPart;
    }
    return false;
}

/**
 * @tc.name: Marshal_BatchUpdateResult_001
 * @tc.desc: Verify BatchUpdateResult marshal and unmarshal with marshalable data.
 *    This test case checks if the BatchUpdateResult object can be
 *    orrectly marshaled and unmarshaled.
 * @tc.expect: The test expects that the marshaled and unmarshaled
 *    BatchUpdateResult objects are equal.
 * @tc.step:
 *    1. Create a BatchUpdateResult object and set its uri and codes.
 *    2. Marshal the BatchUpdateResult object into a MessageParcel.
 *    3. Unmarshal the BatchUpdateResult object from the MessageParcel.
 *    4. Compare the original and unmarshaled BatchUpdateResult objects.
 * @tc.type: FUNC
 * @tc.require:issueIBYE9X
 */
HWTEST_F(DatashareItypesUtilsTest, Marshal_BatchUpdateResult_001, TestSize.Level0)
{
    LOG_INFO("Marshal_BatchUpdateResult_001 starts");
    // Initialize the BatchUpdateResult object
    BatchUpdateResult result;
    BatchUpdateResult unmashalResult;
    result.uri = "datashare:///com.acts.datasharetest";
    result.codes = {12, 2, 4};
    MessageParcel parcel;
    bool res = ITypesUtil::Marshalling(result, parcel);
    EXPECT_TRUE(res);
    res = ITypesUtil::Unmarshalling(unmashalResult, parcel);
    EXPECT_TRUE(res);
    // Compare the original and unmarshalled results
    EXPECT_EQ(result.uri, unmashalResult.uri);
    EXPECT_EQ(result.codes, unmashalResult.codes);
    LOG_INFO("Marshal_BatchUpdateResult_001 ends");
}

/**
 * @tc.name: Marshal_RdbChangeNode_001
 * @tc.desc: Test the marshalling function of RdbChangeNode with isSharedMemory_ set to false.
 * @tc.expect: The marshalling and unmarshalling process should succeed and the original
 *    RdbChangeNode object should be equal to the unmarshalled one.
 * @tc.step:
 *    1. Create a RdbChangeNode object with isSharedMemory_ set to false and a MessageParcel object.
 *    2. Use the ITypesUtil::Marshalling function to marshal the RdbChangeNode object into
 *       the MessageParcel object.
 *    3. Use the ITypesUtil::Unmarshalling function to unmarshal the RdbChangeNode object from
 *       the MessageParcel object.
 *    4. Compare the original RdbChangeNode object with the unmarshalled one.
 * @tc.type: FUNC
 * @tc.precon: The test requires that the BatchUpdateResult object is initialized and isSharedMemory_
 *    is set to false.
 * @tc.require:issueIBYE9X
 */
HWTEST_F(DatashareItypesUtilsTest, Marshal_RdbChangeNode_001, TestSize.Level0)
{
    LOG_INFO("Marshal_RdbChangeNode_001 starts");
    RdbChangeNode unmashalchangeNode;
    MessageParcel parcel;
    RdbChangeNode changeNode = CreateRdbChangeNode();
    bool res = ITypesUtil::Marshalling(changeNode, parcel);
    EXPECT_TRUE(res);
    res = ITypesUtil::Unmarshalling(unmashalchangeNode, parcel);
    EXPECT_TRUE(res);
    EXPECT_TRUE(changeNode == unmashalchangeNode);
    LOG_INFO("Marshal_RdbChangeNode_001 ends");
}

/**
 * @tc.name: Marshal_RdbChangeNode_002
 * @tc.desc: Test the marshalling function of RdbChangeNode with isSharedMemory_ set to
 *    true and memory_ set to nullptr.
 * @tc.expect: The test expects the marshalling operation to fail due to the
 *             memory_ member being set to nullptr.
 * @tc.step:
 *    1. Create an RdbChangeNode instance and set its isSharedMemory_ flag to true.
 *    2: Set the memory_ member of the RdbChangeNode instance to nullptr.
 *    3: Attempt to marshalling the RdbChangeNode instance into a MessageParcel using
 *        the ITypesUtil::Marshalling method.
 *    4: Verify that the marshalling operation fails by checking the return value of
 *        the ITypesUtil::Marshalling method, which should be false.
 * @tc.type: FUNC
 * @tc.precon: The test requires that the BatchUpdateResult object is initialized, while isSharedMemory_
 *    is set to true, and memory_ set to nullptr.
 * @tc.require:issueIBYE9X
 */
HWTEST_F(DatashareItypesUtilsTest, Marshal_RdbChangeNode_002, TestSize.Level0)
{
    LOG_INFO("Marshal_RdbChangeNode_002 starts");
    RdbChangeNode unmashalchangeNode;
    MessageParcel parcel;
    RdbChangeNode changeNode = CreateRdbChangeNode();
    changeNode.isSharedMemory_ = true;
    changeNode.memory_ = nullptr;
    bool res = ITypesUtil::Marshalling(changeNode, parcel);
    EXPECT_FALSE(res);
    LOG_INFO("Marshal_RdbChangeNode_002 ends");
}

/**
 * @tc.name: Marshal_RdbChangeNode_003
 * @tc.desc: Test the marshalling and unmarshalling functionality of RdbChangeNode
 * @tc.expect: The marshalling and unmarshalling process should succeed,
 *     data correctness and consistency should be ensured.
 * @tc.step:
 *    1. Create a RdbChangeNode object and set isSharedMemory_ flag to true.
 *    2. Use CreateAshmem to create a shared memory object and set it to the memory_ member
 *        of the RdbChangeNode object.
 *    3. Use ashmem to write data into the shared memory of the RdbChangeNode object.
 *    3. Use the Unmarshalling method of ITypesUtil to deserialize the MessageParcel object
 *        into another RdbChangeNode object.
 *    4. Verify that the original RdbChangeNode object and the deserialized RdbChangeNode objects
 *        are consistent with each other and the unmarshalling procress is successful.
 *    5. Close the ashmem of the RdbChangeNode objects.
 * @tc.type: FUNC
 * @tc.precon: The test requires that the BatchUpdateResult object is initialized, while isSharedMemory_
 *    is set to true, and use CreateAshmem to create a shared memory object to put in memory_.
 * @tc.require:issueIBYE9X
 */
HWTEST_F(DatashareItypesUtilsTest, Marshal_RdbChangeNode_003, TestSize.Level0)
{
    LOG_INFO("Marshal_RdbChangeNode_003 starts");
    RdbChangeNode unmashalchangeNode;
    MessageParcel parcel;
    RdbChangeNode changeNode = CreateRdbChangeNode();
    changeNode.isSharedMemory_ = true;
    changeNode.memory_ = Ashmem::CreateAshmem("testd", 8);
    bool mapRet = changeNode.memory_->MapReadAndWriteAshmem();
    EXPECT_TRUE(mapRet);
    int32_t data = 123;
    bool writeRet = changeNode.memory_->WriteToAshmem(&data, sizeof(data), 0);
    EXPECT_TRUE(writeRet);
    bool res = ITypesUtil::Marshalling(changeNode, parcel);
    EXPECT_TRUE(res);
    res = ITypesUtil::Unmarshalling(unmashalchangeNode, parcel);
    EXPECT_TRUE(res);
    mapRet = unmashalchangeNode.memory_->MapReadAndWriteAshmem();
    EXPECT_TRUE(mapRet);
    auto changeNodeData = changeNode.memory_->ReadFromAshmem(sizeof(data), 0);
    EXPECT_NE(changeNodeData, nullptr);
    auto unmashalchangeNodeData = unmashalchangeNode.memory_->ReadFromAshmem(sizeof(data), 0);
    EXPECT_NE(unmashalchangeNodeData, nullptr);
    int32_t changeNodeValue = *(reinterpret_cast<const int32_t*>(changeNodeData));
    int32_t unmashalchangeNodeValue = *(reinterpret_cast<const int32_t*>(unmashalchangeNodeData));
    EXPECT_TRUE(changeNode == unmashalchangeNode && changeNodeValue == unmashalchangeNodeValue);
    changeNode.memory_->CloseAshmem();
    unmashalchangeNode.memory_->CloseAshmem();
    LOG_INFO("Marshal_RdbChangeNode_003 ends");
}

/**
 * @tc.name: Marshal_Predicates_001
 * @tc.desc: Test the marshalling and unmarshalling functionality of Predicates.
 * @tc.type: FUNC
 * @tc.require: The marshalling and unmarshalling process should succeed, data correctness and consistency should
 *     be ensured.
 * @tc.precon: None
 * @tc.step:
    1. Create a MessageParcel and a DataSharePredicates object.
    2. Define a string for testing and an abnormal size exceeding MAX_IPC_SIZE.
    3. Write the abnormal size and raw data to the MessageParcel.
    4. Attempt to unmarshal the predicates from the MessageParcel and verify the operation fails due to
        abnormal size.
    5. Test the normal marshalling and unmarshalling of the predicates.
 * @tc.experct: The marshalling operation should succeed, and the unmarshalling operation should correctly handle both
        normal and abnormal cases.
 */
HWTEST_F(DatashareItypesUtilsTest, Marshal_Predicates_001, TestSize.Level0)
{
    LOG_INFO("Marshal_Predicates_001 starts");
    MessageParcel parcel1;
    DataSharePredicates predicates1;
    std::string str = "Unmarshal_Predicates_001";
    size_t size = str.size();
    size_t abnormalSize = 128 * 1024 * 1024 + 1;
    bool ret = parcel1.WriteInt32(abnormalSize);
    EXPECT_TRUE(ret);
    ret = parcel1.WriteRawData(reinterpret_cast<const void *>(str.data()), size);
    EXPECT_TRUE(ret);
    // test unmarshal predicates abnormal func
    ret = ITypesUtil::UnmarshalPredicates(predicates1, parcel1);
    EXPECT_FALSE(ret);
    // test unmarshal predicates normal func
    MessageParcel parcel2;
    DataSharePredicates predicates2;
    predicates2.EqualTo("name", "Unmarshal_Predicates_001");
    ret = ITypesUtil::MarshalPredicates(predicates2, parcel2);
    EXPECT_TRUE(ret);
    DataSharePredicates predicates3;
    ret = ITypesUtil::UnmarshalPredicates(predicates3, parcel2);
    EXPECT_TRUE(ret);
    LOG_INFO("Marshal_Predicates_001 ends");
}

/**
* @tc.name: Marshal_ValuesBucketVec_001
* @tc.desc: Test the marshalling and unmarshalling functionality of Predicates.
* @tc.type: FUNC
* @tc.require: The marshalling and unmarshalling process should succeed, data correctness and consistency should
*    be ensured.
* @tc.precon: None
* @tc.step:
    1. Create a MessageParcel and a DataShareValuesBucket object.
    2. Add key-value pairs to the ValuesBucket, including "name" and "phoneNumber".
    3. Marshal the ValuesBucket vector to a buffer and verify the operation succeeds.
    4. Convert the marshaled data to a string and set an abnormal size exceeding MAX_IPC_SIZE.
    5. Write the abnormal size and marshaled data to the MessageParcel.
    6. Attempt to unmarshal the ValuesBucket vector from the MessageParcel and verify the operation fails due to
        abnormal size.
    7. Test the normal marshalling and unmarshalling of the ValuesBucket vector.
* @tc.experct: The marshalling operation should succeed, and the unmarshalling operation should correctly handle both
    normal and abnormal cases.
*/
HWTEST_F(DatashareItypesUtilsTest, Marshal_MarshalValuesBucketVec_001, TestSize.Level0)
{
    LOG_INFO("Marshal_ValuesBucketVec_001 starts");
    MessageParcel parcel1;
    std::vector<DataShareValuesBucket> buckets;
    std::string str = "Marshal_ValuesBucketVec_001";
    size_t size = str.size();
    size_t abnormalSize = 128 * 1024 * 1024 + 1;
    // marshal wrong data to message parcel
    bool ret = parcel1.WriteInt32(abnormalSize);
    EXPECT_TRUE(ret);
    ret = parcel1.WriteRawData(reinterpret_cast<const void *>(str.data()), size);
    EXPECT_TRUE(ret);
    // test unmarshal ValuesBucketVec abnormal func
    ret = ITypesUtil::UnmarshalValuesBucketVec(buckets, parcel1);
    EXPECT_FALSE(ret);
    buckets.clear();
    // test unmarshal ValuesBucketVec normal func
    MessageParcel parcel2;
    DataShareValuesBucket valuesBucket;
    valuesBucket.Put("name", "dataShareTest006");
    valuesBucket.Put("phoneNumber", 20.6);
    buckets.push_back(valuesBucket);
    ret = ITypesUtil::MarshalValuesBucketVec(buckets, parcel2);
    EXPECT_TRUE(ret);
    buckets.clear();
    ret = ITypesUtil::UnmarshalValuesBucketVec(buckets, parcel2);
    EXPECT_TRUE(ret);
    LOG_INFO("Marshal_ValuesBucketVec_001 ends");
}

/**
* @tc.name: UnmarshallingTest001
* @tc.desc: test Unmarshalling function when parcel is nullptr
* @tc.type: FUNC
* @tc.require: issueIC9GIH
* @tc.precon: None
* @tc.step:
    1.Creat a ITypesUtil object and parcel is nullptr
    2.call Unmarshalling function and check the result
* @tc.experct: Unmarshalling failed and return false
*/
HWTEST_F(DatashareItypesUtilsTest, UnmarshallingTest001, TestSize.Level0)
{
    LOG_INFO("UnmarshallingTest001::Start");
    ITypesUtil::Predicates predicates;
    MessageParcel parcel;
    auto result = ITypesUtil::Unmarshalling(predicates, parcel);
    EXPECT_FALSE(result);
    LOG_INFO("UnmarshallingTest001::End");
}

/**
* @tc.name: UnmarshallingTest002
* @tc.desc: test Unmarshalling function when parcel is nullptr
* @tc.type: FUNC
* @tc.require: issueIC9GIH
* @tc.precon: None
* @tc.step:
    1.Creat a ITypesUtil object and parcel is nullptr
    2.call Unmarshalling function and check the result
* @tc.experct: Unmarshalling failed and return false
*/
HWTEST_F(DatashareItypesUtilsTest, UnmarshallingTest002, TestSize.Level0)
{
    LOG_INFO("UnmarshallingTest002::Start");
    OperationStatement operationStatement;
    MessageParcel parcel;
    auto result = ITypesUtil::Unmarshalling(operationStatement, parcel);
    EXPECT_FALSE(result);
    LOG_INFO("UnmarshallingTest002::End");
}

/**
* @tc.name: UnmarshallingTest003
* @tc.desc: test Unmarshalling function when parcel is nullptr
* @tc.type: FUNC
* @tc.require: issueIC9GIH
* @tc.precon: None
* @tc.step:
    1.Creat a ITypesUtil object and parcel is nullptr
    2.call Unmarshalling function and check the result
* @tc.experct: Unmarshalling failed and return false
*/
HWTEST_F(DatashareItypesUtilsTest, UnmarshallingTest003, TestSize.Level0)
{
    LOG_INFO("UnmarshallingTest003::Start");
    ExecResult execResult;
    MessageParcel parcel;
    auto result = ITypesUtil::Unmarshalling(execResult, parcel);
    EXPECT_FALSE(result);
    LOG_INFO("UnmarshallingTest003::End");
}

/**
* @tc.name: UnmarshallingTest004
* @tc.desc: test Unmarshalling function when parcel is nullptr
* @tc.type: FUNC
* @tc.require: issueIC9GIH
* @tc.precon: None
* @tc.step:
    1.Creat a ITypesUtil object and parcel is nullptr
    2.call Unmarshalling function and check the result
* @tc.experct: Unmarshalling failed and return false
*/
HWTEST_F(DatashareItypesUtilsTest, UnmarshallingTest004, TestSize.Level0)
{
    LOG_INFO("UnmarshallingTest004::Start");
    ExecResultSet execResultSet;
    MessageParcel parcel;
    auto result = ITypesUtil::Unmarshalling(execResultSet, parcel);
    EXPECT_FALSE(result);
    LOG_INFO(" UnmarshallingTest004::End");
}

/**
* @tc.name: MarshallingTest001
* @tc.desc: test Marshalling function when parcel is nullptr
* @tc.type: FUNC
* @tc.require: issueIC9GIH
* @tc.precon: None
* @tc.step:
    1.Creat a ITypesUtil object and parcel is nullptr
    2.call Marshalling function and check the result
* @tc.experct: Marshalling success and return ture
*/
HWTEST_F(DatashareItypesUtilsTest, MarshallingTest001, TestSize.Level0)
{
    LOG_INFO("MarshallingTest001::Start");
    ITypesUtil::RdbChangeNode changeNode;
    MessageParcel parcel;
    auto result = ITypesUtil::Marshalling(changeNode, parcel);
    EXPECT_TRUE(result);
    LOG_INFO("MarshallingTest001::End");
}
}
}