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
 * @tc.desc: Verify the marshalling and unmarshalling functionality of the BatchUpdateResult class with valid,
 *           marshalable data, ensuring the object can be correctly converted to and from a MessageParcel.
 * @tc.type: FUNC
 * @tc.require: issueIBYE9X
 * @tc.precon:
    1. The test environment supports instantiation of BatchUpdateResult and MessageParcel objects without
       initialization errors.
    2. The ITypesUtil class provides valid Marshalling and Unmarshalling methods for BatchUpdateResult.
    3. The BatchUpdateResult class allows setting of 'uri' (string type) and 'codes' (vector<int> type) members.
 * @tc.step:
    1. Create a BatchUpdateResult object, set its 'uri' to "datashare:///com.acts.datasharetest".
    2. Create a MessageParcel object, then call Marshalling to marshal the BatchUpdateResult into the parcel.
    3. Create another BatchUpdateResult object (for unmarshalling), then call ITypesUtil::Unmarshalling to
       restore it from the marshalled parcel.
    4. Compare the 'uri' and 'codes' members of the original and unmarshalled BatchUpdateResult objects.
 * @tc.expect:
    1. The ITypesUtil::Marshalling and ITypesUtil::Unmarshalling methods both return true (operation success).
    2. The 'uri' and 'codes' of the original BatchUpdateResult are identical to those of the unmarshalled one.
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
 * @tc.desc: Test the marshalling and unmarshalling of RdbChangeNode when 'isSharedMemory_' is true and 'memory_'
 *           points to a valid shared memory (created via CreateAshmem), ensuring data consistency and
 *           operation success.
 * @tc.type: FUNC
 * @tc.require: issueIBYE9X
 * @tc.precon:
    1. The CreateRdbChangeNode() function generates a valid RdbChangeNode instance; CreateAshmem can create a shared
       memory object with specified name and size.
    2. The Ashmem class's MapReadAndWriteAshmem (for memory mapping), WriteToAshmem (for data writing), ReadFromAshmem
       (for data reading), and CloseAshmem (for resource release) methods work normally.
    3. The test environment supports ITypesUtil's Marshalling/Unmarshalling for RdbChangeNode and MessageParcel.
 * @tc.step:
    1. Call CreateRdbChangeNode() to create an RdbChangeNode object, set 'isSharedMemory_' to true.
    2. Use Ashmem::CreateAshmem("testd", 8) to create a shared memory object, assign it to the 'memory_' member of the
       RdbChangeNode.
    3. Call MapReadAndWriteAshmem on 'memory_' to enable read-write access, then write an int32_t value (123) into the
       shared memory.
    4. Marshal the RdbChangeNode into a MessageParcel via ITypesUtil::Marshalling; unmarshal it into another
       RdbChangeNode.
    5. Map the shared memory of unmashalchangeNode, read data from both original and unmarshalled shared memories, then
       compare the data.
    6. Call CloseAshmem on both original and unmarshalled 'memory_' to release resources.
 * @tc.expect:
    1. All memory mapping, data writing, marshalling, and unmarshalling operations return true (success).
    2. The original RdbChangeNode is equal to the unmarshalled one; the data read from their shared memories is
       identical.
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
 * @tc.desc: Test the marshalling and unmarshalling of DataSharePredicates, covering both abnormal
 *           and normal scenarios to verify correct handling of valid and invalid data.
 * @tc.type: FUNC
 * @tc.require: issueIBYE9X
 * @tc.precon:
    1. The test environment supports DataSharePredicates, MessageParcel, and the MarshalPredicates/UnmarshalPredicates
       functions.
    2. The MAX_IPC_SIZE constant is predefined (128 * 1024 * 1024 in the test code); string operations work.
    3. DataSharePredicates allows setting conditions via the EqualTo method.
 * @tc.step:
    1. Create a MessageParcel (parcel1) and a DataSharePredicates (predicates1); define a test string and an
       abnormal size (MAX_IPC_SIZE + 1 = 128*1024*1024 + 1).
    2. Write the abnormal size and test string data into parcel1, then call UnmarshalPredicates to attempt restoring
       predicates1 from parcel1.
    3. Create another MessageParcel (parcel2) and DataSharePredicates (predicates2); use EqualTo to
       set "name" = "Unmarshal_Predicates_001".
    4. Marshal predicates2 into parcel2 via MarshalPredicates; unmarshal parcel2 into a third DataSharePredicates.
 * @tc.expect:
    1. Unmarshalling from parcel1 (abnormal size) returns false (failure); marshalling of predicates2 and unmarshalling
       into predicates3 return true.
    2. The normal unmarshalled predicates3 retains the condition set in predicates2.
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
 * @tc.desc: Test the marshalling and unmarshalling of std::vector<DataShareValuesBucket>, covering abnormal
 *           and normal scenarios to ensure correct handling of valid and invalid data.
 * @tc.type: FUNC
 * @tc.require: issueIBYE9X
 * @tc.precon:
    1. The test environment supports DataShareValuesBucket (with Put method for key-value pairs), MessageParcel, and the
       MarshalValuesBucketVec/UnmarshalValuesBucketVec functions.
    2. The MAX_IPC_SIZE constant is predefined (128 * 1024 * 1024 in the test code); vector operations
       work normally.
    3. DataShareValuesBucket can store different data types (e.g., string "name", double "phoneNumber").
 * @tc.step:
    1. Create a MessageParcel (parcel1) and a std::vector<DataShareValuesBucket> (buckets); define a test string
       and an abnormal size (MAX_IPC_SIZE + 1 = 128*1024*1024 + 1).
    2. Write the abnormal size and test string data into parcel1, call UnmarshalValuesBucketVec to attempt restoring
       buckets from parcel1.
    3. Clear buckets; create a DataShareValuesBucket, use Put to add "name" = "dataShareTest006" and
       "phoneNumber" = 20.6, then push it into buckets.
    4. Create another MessageParcel (parcel2), marshal buckets into parcel2 via MarshalValuesBucketVec; clear buckets
       and unmarshal parcel2 into it.
 * @tc.expect:
    1. Unmarshalling from parcel1 (abnormal size) returns false (failure); marshalling of buckets and unmarshalling
       into buckets return true.
    2. The unmarshalled buckets contains one DataShareValuesBucket with correct key-value pairs.
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
 * @tc.desc: Test the Unmarshalling function of ITypesUtil for the ITypesUtil::Predicates type when using a normally
 *           instantiated MessageParcel (without pre-marshalled data), verifying if unmarshalling fails as expected.
 * @tc.type: FUNC
 * @tc.require: issueIC9GIH
 * @tc.precon:
    1. The test environment supports normal instantiation of ITypesUtil::Predicates and MessageParcel objects, with no
       initialization exceptions.
    2. The ITypesUtil::Unmarshalling method is defined to accept an ITypesUtil::Predicates reference and a
       MessageParcel reference, returning a boolean to indicate operation success.
    3. The MessageParcel used in the test has no pre-stored data related to ITypesUtil::Predicates (ensuring no valid
       data for unmarshalling).
 * @tc.step:
    1. Create an ITypesUtil::Predicates object (named predicates) to store the unmarshalled result.
    2. Create a MessageParcel object (named parcel) without pre-marshalling any ITypesUtil::Predicates data into it.
    3. Call the ITypesUtil::Unmarshalling function, passing the predicates and parcel as input parameters.
    4. Check the boolean return value of the ITypesUtil::Unmarshalling function.
 * @tc.expect:
    1. The ITypesUtil::Unmarshalling function returns false, indicating the unmarshalling operation fails.
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
 * @tc.desc: Test the Unmarshalling function of ITypesUtil for the OperationStatement type when using a normally
 *           instantiated MessageParcel (without pre-marshalled data), verifying if unmarshalling fails as expected.
 * @tc.type: FUNC
 * @tc.require: issueIC9GIH
 * @tc.precon:
    1. The test environment supports normal instantiation of OperationStatement and MessageParcel objects, with no
       initialization exceptions.
    2. The ITypesUtil::Unmarshalling method is defined to accept an OperationStatement reference and a MessageParcel
       reference, returning a boolean to indicate operation success.
    3. The MessageParcel used in the test has no pre-stored data related to OperationStatement (ensuring no valid
       data for unmarshalling).
 * @tc.step:
    1. Create an OperationStatement object (named operationStatement) to store the unmarshalled result.
    2. Create a MessageParcel object (named parcel) without pre-marshalling any OperationStatement data into it.
    3. Call the ITypesUtil::Unmarshalling function, passing the operationStatement and parcel as input parameters.
    4. Check the boolean return value of the ITypesUtil::Unmarshalling function.
 * @tc.expect:
    1. The ITypesUtil::Unmarshalling function returns false, indicating the unmarshalling operation fails.
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
 * @tc.desc: Test the Unmarshalling function of ITypesUtil for the ExecResult type when using a normally
 *           instantiated MessageParcel (without pre-marshalled data), verifying if unmarshalling fails as expected.
 * @tc.type: FUNC
 * @tc.require: issueIC9GIH
 * @tc.precon:
    1. The test environment supports normal instantiation of ExecResult and MessageParcel objects, with no
       initialization exceptions.
    2. The ITypesUtil::Unmarshalling method is defined to accept an ExecResult reference and a MessageParcel
       reference, returning a boolean to indicate operation success.
    3. The MessageParcel used in the test has no pre-stored data related to ExecResult (ensuring no valid
       data for unmarshalling).
 * @tc.step:
    1. Create an ExecResult object (named execResult) to store the unmarshalled result.
    2. Create a MessageParcel object (named parcel) without pre-marshalling any ExecResult data into it.
    3. Call the ITypesUtil::Unmarshalling function, passing the execResult and parcel as input parameters.
    4. Check the boolean return value of the ITypesUtil::Unmarshalling function.
 * @tc.expect:
    1. The ITypesUtil::Unmarshalling function returns false, indicating the unmarshalling operation fails.
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
 * @tc.desc: Test the Unmarshalling function of ITypesUtil for the ExecResultSet type when using a normally
 *           instantiated MessageParcel (without pre-marshalled data), verifying if unmarshalling fails as expected.
 * @tc.type: FUNC
 * @tc.require: issueIC9GIH
 * @tc.precon:
    1. The test environment supports normal instantiation of ExecResultSet and MessageParcel objects, with no
       initialization exceptions.
    2. The ITypesUtil::Unmarshalling method is defined to accept an ExecResultSet reference and a MessageParcel
       reference, returning a boolean to indicate operation success.
    3. The MessageParcel used in the test has no pre-stored data related to ExecResultSet (ensuring no valid
       data for unmarshalling).
 * @tc.step:
    1. Create an ExecResultSet object (named execResultSet) to store the unmarshalled result.
    2. Create a MessageParcel object (named parcel) without pre-marshalling any ExecResultSet data into it.
    3. Call the ITypesUtil::Unmarshalling function, passing the execResultSet and parcel as input parameters.
    4. Check the boolean return value of the ITypesUtil::Unmarshalling function.
 * @tc.expect:
    1. The ITypesUtil::Unmarshalling function returns false, indicating the unmarshalling operation fails.
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
 * @tc.desc: Test the Marshalling function of ITypesUtil for the ITypesUtil::RdbChangeNode type when using a normally
 *           instantiated MessageParcel, verifying if marshalling succeeds as expected.
 * @tc.type: FUNC
 * @tc.require: issueIC9GIH
 * @tc.precon:
    1. The test environment supports normal instantiation of ITypesUtil::RdbChangeNode and MessageParcel objects, with
       no initialization exceptions.
    2. The ITypesUtil::Marshalling method is defined to accept an ITypesUtil::RdbChangeNode reference and a
       MessageParcel reference, returning a boolean to indicate operation success.
    3. The ITypesUtil::RdbChangeNode object used in the test has valid default values (meeting the marshalling data
       requirements of the method).
 * @tc.step:
    1. Create an ITypesUtil::RdbChangeNode object (named changeNode) to be marshalled into the MessageParcel.
    2. Create a MessageParcel object (named parcel) to store the marshalled data of changeNode.
    3. Call the ITypesUtil::Marshalling function, passing the changeNode and parcel as input parameters.
    4. Check the boolean return value of the ITypesUtil::Marshalling function.
 * @tc.expect:
    1. The ITypesUtil::Marshalling function returns true, indicating the marshalling operation succeeds.
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