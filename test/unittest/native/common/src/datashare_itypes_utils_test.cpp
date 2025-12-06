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
#include <sstream>
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
#include "datashare_operation_statement.h"

namespace OHOS {
namespace DataShare {
using namespace testing::ext;
DataShareValuesBucket testBucket;

class DatashareItypesUtilsTest : public testing::Test {
public:
    static void SetUpTestCase(void);
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

bool operator==(const ExecResultSet set1, const ExecResultSet set2)
{
    if (set1.errorCode != set2.errorCode) {
        return false;
    }
    size_t size1 = set1.results.size();
    size_t size2 = set2.results.size();
    if (size1 != size2) {
        return false;
    }
    for (size_t i = 0; i < size1; ++i) {
        bool equal = set1.results[i].operationType == set2.results[i].operationType
            && set1.results[i].code == set2.results[i].code
            && set1.results[i].message == set2.results[i].message;
        if (!equal) {
            return equal;
        }
    }
    return true;
}

void GenerateValuesBucket()
{
    // empty bucket
    testBucket.Clear();
    // each loop need to contain 90 bytes, 30 loops makeup total 2700 bytes
    for (int i = 0; i < 30; ++i) {
        // 1 byte of typeID, 8 bytes + key length + 8bytes + value length, when less then 10 data length is 1
        if (i < 10) {
            testBucket.Put("datasharete" + std::to_string(i),
                "Initialize the vector with a large number of key value pairs.");
        }
        testBucket.Put("datasharete" + std::to_string(i),
            "Initialize the vector with a large number of key value pairs");
    }
}

void DatashareItypesUtilsTest::SetUpTestCase(void)
{
    LOG_INFO("SetUpTestCase invoked");
    GenerateValuesBucket();
    LOG_INFO("SetUpTestCase end");
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

/**
* @tc.name: Marshal_OperationStatement_001
* @tc.desc: Test the marshalling and unmarshalling functionality of one OperationStatement.
* @tc.type: FUNC
* @tc.require: 1014
* @tc.precon: None
* @tc.step:
*    1. Create a MessageParcel and a vector of OperationStatement objects.
*    2. Initialize the vector with 1 OperationStatement objects.
*    3. Marshal the vector to the MessageParcel and verify the operation succeeds.
*    4. Unmarshal the vector from the MessageParcel and verify the operation succeeds.
*    5. Compare the unmarshaled vector with the original vector to ensure data consistency.
* @tc.expect: The marshalling and unmarshalling process should succeed, data correctness and consistency should.
*    be ensured.
*/
HWTEST_F(DatashareItypesUtilsTest, Marshal_OperationStatement_001, TestSize.Level0)
{
    LOG_INFO("Marshal_OperationStatement_001 starts");
    // prepare test data
    std::vector<OperationStatement> operationStatements;
    OperationStatement statement;
    int value = 123;
    statement.operationType = Operation::INSERT;
    statement.uri = "test_uri";
    statement.predicates.SetWhereClause("`DB_NUM` > 100");
    statement.valuesBucket.Put("key", value);
    statement.backReference.SetColumn("column");
    statement.backReference.SetFromIndex(1);
    operationStatements.emplace_back(statement);

    // marshal
    MessageParcel parcel;
    ASSERT_TRUE(ITypesUtil::MarshalOperationStatementVec(operationStatements, parcel));

    // unmarshal
    std::vector<OperationStatement> operationStatementsOut;
    ASSERT_TRUE(ITypesUtil::UnmarshalOperationStatementVec(operationStatementsOut, parcel));

    // result comparison
    EXPECT_EQ(operationStatements.size(), operationStatementsOut.size());
    EXPECT_EQ(operationStatements[0].operationType, operationStatementsOut[0].operationType);
    EXPECT_EQ(operationStatements[0].uri, operationStatementsOut[0].uri);
    EXPECT_EQ(operationStatements[0].predicates.GetWhereClause(),
              operationStatementsOut[0].predicates.GetWhereClause());
    bool isValid = false;
    int valueOut = operationStatements[0].valuesBucket.Get("key", isValid);
    EXPECT_EQ(value, valueOut);
    EXPECT_EQ(operationStatements[0].backReference.GetColumn(), operationStatementsOut[0].backReference.GetColumn());
    EXPECT_EQ(operationStatements[0].backReference.GetFromIndex(),
        operationStatementsOut[0].backReference.GetFromIndex());
    LOG_INFO("Marshal_OperationStatement_001 ends");
}

/**
* @tc.name: Marshal_OperationStatement_002
* @tc.desc: Test the marshalling OperationStatement with operationType exceeds defined types.
* @tc.type: FUNC
* @tc.require: 1014
* @tc.precon: None
* @tc.step:
*    1. Create a MessageParcel and a vector of OperationStatement objects.
*    2. Initialize the vector with 1 OperationStatement objects which has an opeartionType exceeds defined types.
*    3. Marshal the vector to the MessageParcel and verify the operation success.
* @tc.expect: The marshalling and unmarshalling process should succeed.
*    be ensured.
*/
HWTEST_F(DatashareItypesUtilsTest, Marshal_OperationStatement_002, TestSize.Level0)
{
    LOG_INFO("Marshal_OperationStatement_002 starts");
    // prepare test data
    std::vector<OperationStatement> operationStatements;
    OperationStatement statement;
    int value = 123;
    statement.operationType = static_cast<Operation>(4);
    statement.uri = "datashare://";
    statement.predicates.SetWhereClause("`DB_NUM` > 100");
    statement.valuesBucket.Put("key", value);
    statement.backReference.SetColumn("column");
    statement.backReference.SetFromIndex(1);
    operationStatements.emplace_back(statement);

    // marshal
    MessageParcel parcel;
    ASSERT_TRUE(ITypesUtil::MarshalOperationStatementVec(operationStatements, parcel));

    // unmarshal
    std::vector<OperationStatement> operationStatementsOut;
    ASSERT_TRUE(ITypesUtil::UnmarshalOperationStatementVec(operationStatementsOut, parcel));

    // result comparison
    EXPECT_EQ(operationStatements.size(), operationStatementsOut.size());
    EXPECT_EQ(operationStatements[0].operationType, operationStatementsOut[0].operationType);
    EXPECT_EQ(operationStatements[0].uri, operationStatementsOut[0].uri);
    EXPECT_EQ(operationStatements[0].predicates.GetWhereClause(),
              operationStatementsOut[0].predicates.GetWhereClause());
    bool isValid = false;
    int valueOut = operationStatements[0].valuesBucket.Get("key", isValid);
    EXPECT_EQ(value, valueOut);
    EXPECT_EQ(operationStatements[0].backReference.GetColumn(), operationStatementsOut[0].backReference.GetColumn());
    EXPECT_EQ(operationStatements[0].backReference.GetFromIndex(),
        operationStatementsOut[0].backReference.GetFromIndex());
    LOG_INFO("Marshal_OperationStatement_002 ends");
}

/**
* @tc.name: Marshal_OperationStatement_003
* @tc.desc: Test the marshalling OperationStatement with operationType that is negative.
* @tc.type: FUNC
* @tc.require: 1014
* @tc.precon: None
* @tc.step:
*    1. Create a MessageParcel and a vector of OperationStatement objects.
*    2. Initialize the vector with 1 OperationStatement objects which has an negative opeartionType.
*    3. Marshal the vector to the MessageParcel and verify the operation success.
* @tc.expect: The marshalling and unmarshalling process should succeed.
*    be ensured.
*/
HWTEST_F(DatashareItypesUtilsTest, Marshal_OperationStatement_003, TestSize.Level0)
{
    LOG_INFO("Marshal_OperationStatement_003 starts");
    // prepare test data
    std::vector<OperationStatement> operationStatements;
    OperationStatement statement;
    int value = 123;
    statement.operationType = static_cast<Operation>(-1);
    statement.uri = "datashare://";
    statement.predicates.SetWhereClause("`DB_NUM` > 100");
    statement.valuesBucket.Put("key", value);
    statement.backReference.SetColumn("column");
    statement.backReference.SetFromIndex(1);
    operationStatements.emplace_back(statement);

    // marshal
    MessageParcel parcel;
    ASSERT_TRUE(ITypesUtil::MarshalOperationStatementVec(operationStatements, parcel));

    // unmarshal
    std::vector<OperationStatement> operationStatementsOut;
    ASSERT_TRUE(ITypesUtil::UnmarshalOperationStatementVec(operationStatementsOut, parcel));

    // result comparison
    EXPECT_EQ(operationStatements.size(), operationStatementsOut.size());
    EXPECT_EQ(operationStatements[0].operationType, operationStatementsOut[0].operationType);
    EXPECT_EQ(operationStatements[0].uri, operationStatementsOut[0].uri);
    EXPECT_EQ(operationStatements[0].predicates.GetWhereClause(),
              operationStatementsOut[0].predicates.GetWhereClause());
    bool isValid = false;
    int valueOut = operationStatements[0].valuesBucket.Get("key", isValid);
    EXPECT_EQ(value, valueOut);
    EXPECT_EQ(operationStatements[0].backReference.GetColumn(), operationStatementsOut[0].backReference.GetColumn());
    EXPECT_EQ(operationStatements[0].backReference.GetFromIndex(),
        operationStatementsOut[0].backReference.GetFromIndex());
    LOG_INFO("Marshal_OperationStatement_003 ends");
}

/**
* @tc.name: Marshal_OperationStatement_004
* @tc.desc: Test the marshalling OperationStatement with valuebucket that contains boolean value.
* @tc.type: FUNC
* @tc.require: 1014
* @tc.precon: None
* @tc.step:
*    1. Create a MessageParcel and a vector of OperationStatement objects.
*    2. Initialize the vector with 1 OperationStatement, where its valuebucket contains a boolean value.
*    3. Marshal the vector to the MessageParcel and verify the operation success.
* @tc.expect: The marshalling and unmarshalling process should succeed.
*    be ensured.
*/
HWTEST_F(DatashareItypesUtilsTest, Marshal_OperationStatement_004, TestSize.Level0)
{
    LOG_INFO("Marshal_OperationStatement_004 starts");
    // prepare test data
    std::vector<OperationStatement> operationStatements;
    OperationStatement statement;
    bool value = true;
    statement.operationType = Operation::UPDATE;
    statement.uri = "datashare://";
    statement.predicates.SetWhereClause("`DB_NUM` > 100");
    statement.valuesBucket.Put("key", value);
    statement.backReference.SetColumn("column");
    statement.backReference.SetFromIndex(1);
    operationStatements.emplace_back(statement);

    // marshal
    MessageParcel parcel;
    ASSERT_TRUE(ITypesUtil::MarshalOperationStatementVec(operationStatements, parcel));

    // unmarshal
    std::vector<OperationStatement> operationStatementsOut;
    ASSERT_TRUE(ITypesUtil::UnmarshalOperationStatementVec(operationStatementsOut, parcel));

    // result comparison
    EXPECT_EQ(operationStatements.size(), operationStatementsOut.size());
    EXPECT_EQ(operationStatements[0].operationType, operationStatementsOut[0].operationType);
    EXPECT_EQ(operationStatements[0].uri, operationStatementsOut[0].uri);
    EXPECT_EQ(operationStatements[0].predicates.GetWhereClause(),
              operationStatementsOut[0].predicates.GetWhereClause());
    bool isValid = false;
    bool valueOut = operationStatements[0].valuesBucket.Get("key", isValid);
    EXPECT_EQ(value, valueOut);
    EXPECT_EQ(operationStatements[0].backReference.GetColumn(), operationStatementsOut[0].backReference.GetColumn());
    EXPECT_EQ(operationStatements[0].backReference.GetFromIndex(),
        operationStatementsOut[0].backReference.GetFromIndex());
    LOG_INFO("Marshal_OperationStatement_004 ends");
}

/**
* @tc.name: Marshal_OperationStatement_005
* @tc.desc: Test the marshalling OperationStatement with valuebucket that contains blob(std::vector<uint8_t>) value.
* @tc.type: FUNC
* @tc.require: 1014
* @tc.precon: None
* @tc.step:
*    1. Create a MessageParcel and a vector of OperationStatement objects.
*    2. Initialize the vector with 1 OperationStatement where its valuebucket contains a blob value.
*    3. Marshal the vector to the MessageParcel and verify the operation success.
* @tc.expect: The marshalling and unmarshalling process should succeed.
*    be ensured.
*/
HWTEST_F(DatashareItypesUtilsTest, Marshal_OperationStatement_005, TestSize.Level0)
{
    LOG_INFO("Marshal_OperationStatement_005 starts");
    // prepare test data
    std::vector<OperationStatement> operationStatements;
    OperationStatement statement;
    std::vector<uint8_t> value(10, 0xFF);
    statement.operationType = Operation::INSERT;
    statement.uri = "datashare://";
    statement.predicates.SetWhereClause("`DB_NUM` > 100");
    statement.valuesBucket.Put("key", value);
    statement.backReference.SetColumn("column");
    statement.backReference.SetFromIndex(1);
    operationStatements.emplace_back(statement);

    // marshal
    MessageParcel parcel;
    ASSERT_TRUE(ITypesUtil::MarshalOperationStatementVec(operationStatements, parcel));

    // unmarshal
    std::vector<OperationStatement> operationStatementsOut;
    ASSERT_TRUE(ITypesUtil::UnmarshalOperationStatementVec(operationStatementsOut, parcel));

    // result comparison
    EXPECT_EQ(operationStatements.size(), operationStatementsOut.size());
    EXPECT_EQ(operationStatements[0].operationType, operationStatementsOut[0].operationType);
    EXPECT_EQ(operationStatements[0].uri, operationStatementsOut[0].uri);
    EXPECT_EQ(operationStatements[0].predicates.GetWhereClause(),
              operationStatementsOut[0].predicates.GetWhereClause());
    bool isValid = false;
    std::vector<uint8_t> valueOut = operationStatements[0].valuesBucket.Get("key", isValid);
    EXPECT_EQ(value, valueOut);
    EXPECT_EQ(operationStatements[0].backReference.GetColumn(), operationStatementsOut[0].backReference.GetColumn());
    EXPECT_EQ(operationStatements[0].backReference.GetFromIndex(),
        operationStatementsOut[0].backReference.GetFromIndex());
    LOG_INFO("Marshal_OperationStatement_005 ends");
}

/**
* @tc.name: Marshal_OperationStatement_006
* @tc.desc: Test the marshalling OperationStatement with valuebucket that contains std::monostate value.
* @tc.type: FUNC
* @tc.require: 1014
* @tc.precon: None
* @tc.step:
*    1. Create a MessageParcel and a vector of OperationStatement objects.
*    2. Initialize the vector with 1 OperationStatement objects where its valuebucket contains a monostate value.
*    3. Marshal the vector to the MessageParcel and verify the operation success.
* @tc.expect: The marshalling and unmarshalling process should succeed.
*    be ensured.
*/
HWTEST_F(DatashareItypesUtilsTest, Marshal_OperationStatement_006, TestSize.Level0)
{
    LOG_INFO("Marshal_OperationStatement_006 starts");
    // prepare test data
    std::vector<OperationStatement> operationStatements;
    OperationStatement statement;
    DataShareValueObject value;
    statement.operationType = Operation::INSERT;
    statement.uri = "datashare://";
    statement.predicates.SetWhereClause("`DB_NUM` > 100");
    statement.valuesBucket.Put("key", value);
    statement.backReference.SetColumn("column");
    statement.backReference.SetFromIndex(1);
    operationStatements.emplace_back(statement);

    // marshal
    MessageParcel parcel;
    ASSERT_TRUE(ITypesUtil::MarshalOperationStatementVec(operationStatements, parcel));

    // unmarshal
    std::vector<OperationStatement> operationStatementsOut;
    ASSERT_TRUE(ITypesUtil::UnmarshalOperationStatementVec(operationStatementsOut, parcel));

    // result comparison
    EXPECT_EQ(operationStatements.size(), operationStatementsOut.size());
    EXPECT_EQ(operationStatements[0].operationType, operationStatementsOut[0].operationType);
    EXPECT_EQ(operationStatements[0].uri, operationStatementsOut[0].uri);
    EXPECT_EQ(operationStatements[0].predicates.GetWhereClause(),
              operationStatementsOut[0].predicates.GetWhereClause());
    bool isValid = false;
    DataShareValueObject valueOut = operationStatements[0].valuesBucket.Get("key", isValid);
    uint8_t typeId = valueOut.value.index();
    EXPECT_EQ(0, typeId);
    EXPECT_EQ(operationStatements[0].backReference.GetColumn(), operationStatementsOut[0].backReference.GetColumn());
    EXPECT_EQ(operationStatements[0].backReference.GetFromIndex(),
        operationStatementsOut[0].backReference.GetFromIndex());
    LOG_INFO("Marshal_OperationStatement_006 ends");
}

/**
* @tc.name: MarshalOperationStatementVec_001
* @tc.desc: Test the marshalling and unmarshalling functionality of OperationStatementVec with normal data.
* @tc.type: FUNC
* @tc.require: 1014
* @tc.precon: None
* @tc.step:
*    1. Create a MessageParcel and a vector of OperationStatement objects.
*    2. Initialize the vector with 2 OperationStatement objects.
*    3. Marshal the vector to the MessageParcel and verify the operation succeeds.
*    4. Unmarshal the vector from the MessageParcel and verify the operation succeeds.
*    5. Compare the unmarshaled vector with the original vector to ensure data consistency.
* @tc.expect: The marshalling and unmarshalling operations should succeed, and the unmarshaled data should match
*    the original data.
*/
HWTEST_F(DatashareItypesUtilsTest, MarshalOperationStatementVec_001, TestSize.Level0)
{
    LOG_INFO("MarshalOperationStatementVec_001 starts");

    // Step 1: Create a MessageParcel
    MessageParcel parcel;

    // Step 2: Initialize the vector with OperationStatement objects
    std::vector<OperationStatement> operationStatements;
    OperationStatement statement1;
    std::string value = "MarshalOperationStatementVec_001";
    statement1.operationType = Operation::INSERT;
    statement1.uri = "datashare://com.ohos.contactsdataability";
    statement1.predicates.SetWhereClause("`DB_NUM` > 100");
    statement1.valuesBucket.Put("key0", value);
    operationStatements.emplace_back(statement1);

    OperationStatement statement2;
    statement2.operationType = Operation::INSERT;
    statement2.uri = "datashareproxy://com.ohos.contactsdataability/contacts/setting";
    statement2.predicates.SetWhereClause("`TESTCASE` < 200");
    statement2.valuesBucket.Put("key1", value);
    statement2.backReference.SetColumn("UserName");
    statement2.backReference.SetFromIndex(0);
    operationStatements.emplace_back(statement2);

    // Step 3: Marshal the vector to the MessageParcel
    ASSERT_TRUE(ITypesUtil::MarshalOperationStatementVec(operationStatements, parcel));

    // Step 4: Unmarshal the vector from the MessageParcel
    std::vector<OperationStatement> operationStatementsOut;
    ASSERT_TRUE(ITypesUtil::UnmarshalOperationStatementVec(operationStatementsOut, parcel));

    // Step 5: Compare the unmarshaled vector with the original vector
    EXPECT_EQ(operationStatements.size(), operationStatementsOut.size());
    for (size_t i = 0; i < operationStatements.size(); ++i) {
        EXPECT_EQ(operationStatements[i].operationType, operationStatementsOut[i].operationType);
        EXPECT_EQ(operationStatements[i].uri, operationStatementsOut[i].uri);
        EXPECT_EQ(operationStatements[i].predicates.GetWhereClause(),
                  operationStatementsOut[i].predicates.GetWhereClause());
        bool isValid = false;
        std::string valueOut = operationStatementsOut[i].valuesBucket.Get("key" + std::to_string(i), isValid);
        EXPECT_EQ(value, valueOut);
        EXPECT_EQ(operationStatements[i].backReference.GetColumn(),
                  operationStatementsOut[i].backReference.GetColumn());
        EXPECT_EQ(operationStatements[i].backReference.GetFromIndex(),
                  operationStatementsOut[i].backReference.GetFromIndex());
    }
    LOG_INFO("MarshalOperationStatementVec_001 ends");
}

/**
* @tc.name: MarshalOperationStatementVec_002
* @tc.desc: Test the marshalling and unmarshalling functionality of an empty OperationStatementVec.
* @tc.type: FUNC
* @tc.require: 1014
* @tc.precon: None
* @tc.step:
*    1. Create a MessageParcel and an empty vector of OperationStatement objects.
*    2. Marshal the empty vector to the MessageParcel and verify the operation succeeds.
*    3. Unmarshal the vector from the MessageParcel and verify the operation succeeds.
*    4. Verify that the unmarshaled vector is empty.
* @tc.experct: The marshalling and unmarshalling operations should succeed, and the unmarshaled vector should be empty.
*/
HWTEST_F(DatashareItypesUtilsTest, MarshalOperationStatementVec_002, TestSize.Level0)
{
    LOG_INFO("MarshalOperationStatementVec_002 starts");

    // Step 1: Create a MessageParcel
    MessageParcel parcel;

    // Step 2: Initialize an empty vector
    std::vector<OperationStatement> operationStatements;

    // Step 3: Marshal the empty vector to the MessageParcel
    ASSERT_TRUE(ITypesUtil::MarshalOperationStatementVec(operationStatements, parcel));

    // Step 4: Unmarshal the vector from the MessageParcel
    std::vector<OperationStatement> operationStatementsOut;
    ASSERT_TRUE(ITypesUtil::UnmarshalOperationStatementVec(operationStatementsOut, parcel));

    // Step 5: Verify that the unmarshaled vector is empty
    EXPECT_TRUE(operationStatementsOut.empty());

    LOG_INFO("MarshalOperationStatementVec_002 ends");
}

/**
* @tc.name: MarshalOperationStatementVecCapacity_001
* @tc.desc: Test the marshalling functionality of OperationStatementVec with data exceeding MAX_IPC_SIZE.
* @tc.type: FUNC
* @tc.require: 1014
* @tc.precon: None
* @tc.step:
*    1. Create a MessageParcel and a vector of OperationStatement objects.
*    2. Initialize the vector with a large number of OperationStatement objects such that the serialized size
*       exceeds MAX_IPC_SIZE(134,217,728 bytes).
*    3. Marshal the vector to the MessageParcel and verify the operation fails.
* @tc.experct: The marshalling operation should fail due to exceeding the maximum size limit.
*/
HWTEST_F(DatashareItypesUtilsTest, MarshalOperationStatementVecCapacity_001, TestSize.Level0) {
    LOG_INFO("MarshalOperationStatementVecCapacity_001 starts");

    // Step 1: Create a MessageParcel
    MessageParcel parcel;

    // Step 2: Initialize the vector with a large number of OperationStatement objects
    std::vector<OperationStatement> operationStatements;

    // vector size 8 bytes, 2,856 bytes per loops, 46995 * 2856 + 8 = MAX_IPC_SIZE
    int loops = 46995;
    if (sizeof(void*) == 4) {
        // on 32bit system size_t take less spaces, therefore more loops required
        loops = 60000;
    }
    for (int i = 0; i < loops + 1; ++i) {
        OperationStatement statement;
        statement.operationType = Operation::INSERT;    // 4 bytes
        // 8 bytes + data length 70
        statement.uri = "datashareproxy://com.ohos.contactsdataability/contacts/settingssssssss";
        // operationItem 8 bytes
        // where cluase 8 bytes + data length 14
        // whereArgs 8 bytes, order 8 bytes
        // mode 2 bytes
        statement.predicates.SetWhereClause("`DB_NUM` > 100");
        statement.valuesBucket = testBucket;   // 8 bytes + 2700 bytes
        statement.backReference.SetColumn("column");    // 8 bytes + data length
        statement.backReference.SetFromIndex(i);    // 4 bytes
        operationStatements.emplace_back(statement);
    }

    // Step 3: Marshal the vector to the MessageParcel and verify the operation fails
    ASSERT_FALSE(ITypesUtil::MarshalOperationStatementVec(operationStatements, parcel));

    LOG_INFO("MarshalOperationStatementVecCapacity_001 ends");
}

/**
* @tc.name: MarshalOperationStatementVecCapacity_002
* @tc.desc: Test the marshalling functionality of OperationStatementVec with data equal MAX_IPC_SIZE.
* @tc.type: FUNC
* @tc.require: 1014
* @tc.precon: None
* @tc.step:
*    1. Create a MessageParcel and a vector of OperationStatement objects.
*    2. Initialize the vector with a large number of OperationStatement objects such that the serialized size
*       equals MAX_IPC_SIZE(134,217,728 bytes).
*    3. Marshal the vector to the MessageParcel and verify the operation success.
* @tc.experct: The marshalling operation should succeed.
*/
HWTEST_F(DatashareItypesUtilsTest, MarshalOperationStatementVecCapacity_002, TestSize.Level0) {
    LOG_INFO("MarshalOperationStatementVecCapacity_002 starts");

    // Step 1: Create a MessageParcel
    MessageParcel parcel;

    // Step 2: Initialize the vector with a large number of OperationStatement objects
    std::vector<OperationStatement> operationStatements;

    // vector size 8 bytes, 2,856 bytes per loops, 46995 * 2856 + 8 = MAX_IPC_SIZE
    for (int i = 0; i < 46995; ++i) {
        OperationStatement statement;
        statement.operationType = Operation::INSERT;    // 4 bytes
        // 8 bytes + data length 70
        statement.uri = "datashareproxy://com.ohos.contactsdataability/contacts/settingssssssss";
        // operationItem 8 bytes
        // where cluase 8 bytes + data length 14
        // whereArgs 8 bytes, order 8 bytes
        // mode 2 bytes
        statement.predicates.SetWhereClause("`DB_NUM` > 100");
        statement.valuesBucket = testBucket;   // 8 bytes + 2700 bytes
        statement.backReference.SetColumn("column");    // 8 bytes + data length
        statement.backReference.SetFromIndex(i);    // 4 bytes
        operationStatements.emplace_back(statement);
    }

    // Step 3: Marshal the vector to the MessageParcel and verify the operation fails
    ASSERT_TRUE(ITypesUtil::MarshalOperationStatementVec(operationStatements, parcel));

    LOG_INFO("MarshalOperationStatementVecCapacity_002 ends");
}

/**
* @tc.name: MarshalExecResultSet_001
* @tc.desc: Test the marshalling functionality of ExecResultSet that contain one ExecResult.
* @tc.type: FUNC
* @tc.require: 1014
* @tc.precon: None
* @tc.step:
*    1. Create a MessageParcel and a ExecResultSet objects.
*    2. Initialize ExecResultSet.results vector with 1 ExecResult objects.
*    3. Marshal the vector to the MessageParcel and verify the operation succeeds.
*    4. Unmarshal the vector from the MessageParcel and verify the operation succeeds.
*    5. Compare the unmarshaled ExecResultSet with the original object to ensure data consistency.
* @tc.experct: The marshalling and unmarshalling operation should succeed. ExecResultSet should be consistent.
*/
HWTEST_F(DatashareItypesUtilsTest, MarshalExecResultSet_001, TestSize.Level0) {
    LOG_INFO("MarshalExecResultSet_001 starts");
    MessageParcel parcel;
    ExecResultSet execResultSet;
    // Initialize ExecResult
    ExecResult execResult;
    execResult.operationType = Operation::INSERT;
    execResult.code = 0;
    execResult.message = "23";

    execResultSet.errorCode = ExecErrorCode::EXEC_SUCCESS;
    execResultSet.results.emplace_back(execResult);

    // Marshal the ExecResultSet object to the MessageParcel
    ASSERT_TRUE(ITypesUtil::Marshalling(execResultSet, parcel));

    ExecResultSet execResultSetOut;
    ASSERT_TRUE(ITypesUtil::Unmarshalling(execResultSetOut, parcel));
    EXPECT_EQ(execResultSet, execResultSetOut);
    LOG_INFO("MarshalExecResultSet_001 ends");
}

/**
* @tc.name: MarshalExecResultSet_002
* @tc.desc: Test the marshalling functionality of ExecResultSet that contain multiple ExecResult.
* @tc.type: FUNC
* @tc.require: 1014
* @tc.precon: None
* @tc.step:
*    1. Create a MessageParcel and a ExecResultSet objects.
*    2. Initialize ExecResultSet.results vector with multiple ExecResult objects.
*    3. Marshal the vector to the MessageParcel and verify the operation succeeds.
*    4. Unmarshal the vector from the MessageParcel and verify the operation succeeds.
*    5. Compare the unmarshaled ExecResultSet with the original object to ensure data consistency.
* @tc.experct: The marshalling and unmarshalling operation should succeed. ExecResultSet should be consistent.
*/
HWTEST_F(DatashareItypesUtilsTest, MarshalExecResultSet_002, TestSize.Level0) {
    LOG_INFO("MarshalExecResultSet_002 starts");
    MessageParcel parcel;
    ExecResultSet execResultSet;

    for (int i = 0; i < 10; ++i) {
        ExecResult execResult;
        execResult.operationType = Operation::INSERT;
        execResult.code = 0;
        execResult.message = "test" + std::to_string(i);
        execResultSet.results.emplace_back(execResult);
    }

    // add execResult with a different code
    ExecResult execResult;
    execResult.operationType = Operation::INSERT;
    execResult.code = 1;
    execResult.message = "testcode1";

    execResultSet.errorCode = ExecErrorCode::EXEC_SUCCESS;
    execResultSet.results.emplace_back(execResult);

    // Marshal the ExecResultSet object to the MessageParcel
    ASSERT_TRUE(ITypesUtil::Marshalling(execResultSet, parcel));

    ExecResultSet execResultSetOut;
    ASSERT_TRUE(ITypesUtil::Unmarshalling(execResultSetOut, parcel));
    EXPECT_EQ(execResultSet, execResultSetOut);
    LOG_INFO("MarshalExecResultSet_002 ends");
}

/**
* @tc.name: MarshalExecResultSet_003
* @tc.desc: Test the marshalling functionality of empty ExecResultSet
* @tc.type: FUNC
* @tc.require: 1014
* @tc.precon: None
* @tc.step:
*    1. Create a MessageParcel and a ExecResultSet objects.
*    2. Marshal the ExecResultSet to the MessageParcel and verify the operation succeeds.
*    3. Unmarshal the vector from the MessageParcel and verify the operation succeeds.
*    4. Compare the unmarshaled ExecResultSet with the original object to ensure data consistency.
* @tc.experct: The marshalling and unmarshalling operation should succeed. ExecResultSet should be consistent.
*/
HWTEST_F(DatashareItypesUtilsTest, MarshalExecResultSet_003, TestSize.Level0) {
    LOG_INFO("MarshalExecResultSet_003 starts");
    MessageParcel parcel;
    ExecResultSet execResultSet;

    // Marshal the ExecResultSet object to the MessageParcel
    ASSERT_TRUE(ITypesUtil::Marshalling(execResultSet, parcel));

    ExecResultSet execResultSetOut;
    ASSERT_TRUE(ITypesUtil::Unmarshalling(execResultSetOut, parcel));

    EXPECT_EQ(execResultSet, execResultSetOut);
    LOG_INFO("MarshalExecResultSet_003 ends");
}

/**
* @tc.name: MarshalExecResultSet_004
* @tc.desc: Test the marshalling functionality of ExecResultSet that contain negative operationType and errorCode.
* @tc.type: FUNC
* @tc.require: 1014
* @tc.precon: None
* @tc.step:
*    1. Create a MessageParcel and a ExecResultSet objects.
*    2. Initialize ExecResultSet with negative operationType and errorCode.
*    3. Marshal the vector to the MessageParcel and verify the operation succeeds.
*    4. Unmarshal the vector from the MessageParcel and verify the operation succeeds.
*    5. Compare the unmarshaled ExecResultSet with the original object to ensure data consistency.
* @tc.experct: The marshalling and unmarshalling operation should succeed. ExecResultSet should be consistent.
*/
HWTEST_F(DatashareItypesUtilsTest, MarshalExecResultSet_004, TestSize.Level0) {
    LOG_INFO("MarshalExecResultSet_004 starts");
    MessageParcel parcel;
    ExecResultSet execResultSet;
    // Initialize ExecResult
    ExecResult execResult;
    execResult.operationType = static_cast<Operation>(-1);
    execResult.code = 0;
    execResult.message = "23";

    execResultSet.errorCode = static_cast<ExecErrorCode>(-1);
    execResultSet.results.emplace_back(execResult);

    // Marshal the ExecResultSet object to the MessageParcel
    ASSERT_TRUE(ITypesUtil::Marshalling(execResultSet, parcel));

    ExecResultSet execResultSetOut;
    ASSERT_TRUE(ITypesUtil::Unmarshalling(execResultSetOut, parcel));
    EXPECT_EQ(execResultSet, execResultSetOut);
    LOG_INFO("MarshalExecResultSet_004 ends");
}

/**
* @tc.name: MarshalExecResultSet_005
* @tc.desc: Test the marshalling functionality of ExecResultSet that contain operationType and errorCode
* exceed defined type.
* @tc.type: FUNC
* @tc.require: 1014
* @tc.precon: None
* @tc.step:
*    1. Create a MessageParcel and a ExecResultSet objects.
*    2. Initialize ExecResultSet with operationType and errorCode exceed defined type.
*    3. Marshal the vector to the MessageParcel and verify the operation succeeds.
*    4. Unmarshal the vector from the MessageParcel and verify the operation succeeds.
*    5. Compare the unmarshaled ExecResultSet with the original object to ensure data consistency.
* @tc.experct: The marshalling and unmarshalling operation should succeed. ExecResultSet should be consistent.
*/
HWTEST_F(DatashareItypesUtilsTest, MarshalExecResultSet_005, TestSize.Level0) {
    LOG_INFO("MarshalExecResultSet_005 starts");
    MessageParcel parcel;
    ExecResultSet execResultSet;
    // Initialize ExecResult
    ExecResult execResult;
    execResult.operationType = static_cast<Operation>(4);
    execResult.code = 0;
    execResult.message = "23";

    execResultSet.errorCode = static_cast<ExecErrorCode>(4);
    execResultSet.results.emplace_back(execResult);

    // Marshal the ExecResultSet object to the MessageParcel
    ASSERT_TRUE(ITypesUtil::Marshalling(execResultSet, parcel));

    ExecResultSet execResultSetOut;
    ASSERT_TRUE(ITypesUtil::Unmarshalling(execResultSetOut, parcel));
    EXPECT_EQ(execResultSet, execResultSetOut);
    LOG_INFO("MarshalExecResultSet_005 ends");
}

/**
* @tc.name: MarshalExecResultSetCapacity_001
* @tc.desc: Test the marshalling functionality of ExecResultSet that exceeds 200KB
* exceed defined type.
* @tc.type: FUNC
* @tc.require: 1014
* @tc.precon: None
* @tc.step:
*    1. Create a MessageParcel and a ExecResultSet objects.
*    2. Initialize ExecResultSet that exceeds 200KB.
*    3. Marshal the vector to the MessageParcel and verify the operation succeeds.
*    4. Unmarshal the vector from the MessageParcel and verify the operation succeeds.
*    5. Compare the unmarshaled ExecResultSet with the original object to ensure data consistency.
* @tc.experct: The marshalling and unmarshalling operation should succeed. ExecResultSet should be consistent.
*/
HWTEST_F(DatashareItypesUtilsTest, MarshalExecResultSetCapacity_001, TestSize.Level0) {
    LOG_INFO("MarshalExecResultSetCapacity_001 starts");
    MessageParcel parcel;
    ExecResultSet execResultSet;
    // Initialize ExecResult
    ExecResult execResult;
    execResult.operationType = Operation::INSERT;
    execResult.code = 0;
    execResult.message = "Initialize the message with a large number of key value pairs.";

    execResultSet.errorCode = ExecErrorCode::EXEC_FAILED;
    for (int i = 0; i < 4000 + 1; ++i) {
        execResultSet.results.emplace_back(execResult);
    }

    // Marshal the ExecResultSet object to the MessageParcel
    ASSERT_FALSE(ITypesUtil::Marshalling(execResultSet, parcel));
    LOG_INFO("MarshalExecResultSetCapacity_001 ends");
}
}
}