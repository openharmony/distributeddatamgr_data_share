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

#define LOG_TAG "datashare_predicates_verify_test"

#include <gtest/gtest.h>
#include <unistd.h>

#include "datashare_errno.h"
#include "datashare_predicates.h"
#include "datashare_predicates_verify.h"
#include "datashare_log.h"

namespace OHOS {
namespace DataShare {
using namespace testing::ext;
class DataSharePredicatesVerifyTest : public testing::Test {
public:
    static void SetUpTestCase(void){};
    static void TearDownTestCase(void){};
    void SetUp(){};
    void TearDown(){};
};

static int32_t SINGLE_2_PARAMS_PUBLIC_TEST = static_cast<int32_t>(OperationType::ORDER_BY_ASC);
static int32_t SINGLE_3_PARAMS_PUBLIC_TEST = static_cast<int32_t>(OperationType::EQUAL_TO);
static int32_t SINGLE_2_PARAMS_SYS_TEST = static_cast<int32_t>(OperationType::IS_NULL);
static int32_t SINGLE_3_PARAMS_SYS_TEST = static_cast<int32_t>(OperationType::GREATER_THAN);
static int32_t MULTI_2_PARAMS_SYS_TEST = static_cast<int32_t>(OperationType::GROUP_BY);
static int32_t VERIFY_DEFAULT_TEST = -1;

/**
* @tc.name: GetPredicatesVerifyType001
* @tc.desc: Verify the correct mapping of operation types to predicates verification types
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Verify the mapping of SINGLE_2_PARAMS_PUBLIC_TEST to SINGLE_2_PARAMS_PUBLIC
    2. Verify the mapping of SINGLE_3_PARAMS_PUBLIC_TEST to SINGLE_3_PARAMS_PUBLIC
    3. Verify the mapping of SINGLE_2_PARAMS_SYS_TEST to SINGLE_2_PARAMS_SYS
    4. Verify the mapping of SINGLE_3_PARAMS_SYS_TEST to SINGLE_3_PARAMS_SYS
    5. Verify the mapping of MULTI_2_PARAMS_SYS_TEST to MULTI_2_PARAMS_SYS
    6. Verify the mapping of VERIFY_DEFAULT_TEST to VERIFY_DEFAULT
* @tc.experct:
    1. Each operation type maps to the correct predicates verification type
*/
HWTEST_F(DataSharePredicatesVerifyTest, GetPredicatesVerifyType001, TestSize.Level0)
{
    LOG_INFO("DataSharePredicatesVerifyTest GetPredicatesVerifyType001::Start");
    DataSharePredicatesVerify predicatesVerify;
    auto ret = predicatesVerify.GetPredicatesVerifyType(SINGLE_2_PARAMS_PUBLIC_TEST);
    EXPECT_EQ(ret, DataSharePredicatesVerify::PredicatesVerifyType::SINGLE_2_PARAMS_PUBLIC);

    ret = predicatesVerify.GetPredicatesVerifyType(SINGLE_3_PARAMS_PUBLIC_TEST);
    EXPECT_EQ(ret, DataSharePredicatesVerify::PredicatesVerifyType::SINGLE_3_PARAMS_PUBLIC);

    ret = predicatesVerify.GetPredicatesVerifyType(SINGLE_2_PARAMS_SYS_TEST);
    EXPECT_EQ(ret, DataSharePredicatesVerify::PredicatesVerifyType::SINGLE_2_PARAMS_SYS);

    ret = predicatesVerify.GetPredicatesVerifyType(SINGLE_3_PARAMS_SYS_TEST);
    EXPECT_EQ(ret, DataSharePredicatesVerify::PredicatesVerifyType::SINGLE_3_PARAMS_SYS);

    ret = predicatesVerify.GetPredicatesVerifyType(MULTI_2_PARAMS_SYS_TEST);
    EXPECT_EQ(ret, DataSharePredicatesVerify::PredicatesVerifyType::MULTI_2_PARAMS_SYS);

    ret = predicatesVerify.GetPredicatesVerifyType(VERIFY_DEFAULT_TEST);
    EXPECT_EQ(ret, DataSharePredicatesVerify::PredicatesVerifyType::VERIFY_DEFAULT);
    LOG_INFO("DataSharePredicatesVerifyTest GetPredicatesVerifyType001::End");
}

/**
* @tc.name: VerifyPredicates001
* @tc.desc: Verify predicates validation for normal and illegal fields
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Create a normal predicates and verify its type and error code
    2. Create a predicates with illegal field and verify its type and error code
* @tc.experct:
    1. Normal predicates validation returns E_OK for both type and error code
    2. Illegal field predicates validation returns type 3 and error code E_FIELD_ILLEGAL
*/
HWTEST_F(DataSharePredicatesVerifyTest, VerifyPredicates001, TestSize.Level0)
{
    LOG_INFO("DataSharePredicatesVerifyTest VerifyPredicates001::Start");
    DataSharePredicatesVerify predicatesVerify;
    DataSharePredicates predicatesNormal;
    predicatesNormal.EqualTo("name", "test");
    auto [type1, errCode1] = predicatesVerify.VerifyPredicates(predicatesNormal);
    EXPECT_EQ(type1, E_OK);
    EXPECT_EQ(errCode1, E_OK);

    // test predicates with illegal field
    DataSharePredicates predicatesIllegalField;
    predicatesIllegalField.GreaterThan("true or name", "test");
    auto [type2, errCode2] = predicatesVerify.VerifyPredicates(predicatesIllegalField);
    // 3 is type of predicates GreaterThan
    EXPECT_EQ(type2, 3);
    EXPECT_EQ(errCode2, E_FIELD_ILLEGAL);
    LOG_INFO("DataSharePredicatesVerifyTest VerifyPredicates001::End");
}

/**
* @tc.name: VerifyPredicatesByType001
* @tc.desc: Verify predicates validation by type for default and single parameter public operations
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Verify predicates with VERIFY_DEFAULT type and default test operation
    2. Verify predicates with SINGLE_2_PARAMS_PUBLIC type and default test operation
* @tc.experct:
    1. Both verification operations return E_OK
*/
HWTEST_F(DataSharePredicatesVerifyTest, VerifyPredicatesByType001, TestSize.Level0)
{
    LOG_INFO("DataSharePredicatesVerifyTest VerifyPredicatesByType001::Start");
    DataSharePredicatesVerify predicatesVerify;
    // verifyType is VERIFY_DEFAULT
    OperationItem defaultTest {};
    defaultTest.operation = VERIFY_DEFAULT_TEST;
    auto ret = predicatesVerify.VerifyPredicatesByType(
        DataSharePredicatesVerify::PredicatesVerifyType::VERIFY_DEFAULT, defaultTest);
    EXPECT_EQ(ret, E_OK);

    // CheckParamNum return false
    ret = predicatesVerify.VerifyPredicatesByType(
        DataSharePredicatesVerify::PredicatesVerifyType::SINGLE_2_PARAMS_PUBLIC, defaultTest);
    EXPECT_EQ(ret, E_OK);
    LOG_INFO("DataSharePredicatesVerifyTest VerifyPredicatesByType001::End");
}

/**
* @tc.name: VerifyPredicatesByType002
* @tc.desc: Verify predicates validation by type for single parameter public and system operations
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Verify predicates with SINGLE_2_PARAMS_PUBLIC type and public test operation
    2. Verify predicates with SINGLE_3_PARAMS_PUBLIC type and public test operation
    3. Verify predicates with SINGLE_3_PARAMS_PUBLIC type and invalid field
* @tc.experct:
    1. First two verification operations return E_OK
    2. Third verification operation returns E_FIELD_INVALID
*/
HWTEST_F(DataSharePredicatesVerifyTest, VerifyPredicatesByType002, TestSize.Level0)
{
    LOG_INFO("DataSharePredicatesVerifyTest VerifyPredicatesByType002::Start");
    DataSharePredicatesVerify predicatesVerify;
    // verifyType is SINGLE_2_PARAMS_PUBLIC
    OperationItem publicTest {};
    publicTest.operation = SINGLE_2_PARAMS_PUBLIC_TEST;
    publicTest.singleParams.push_back("test");
    auto ret = predicatesVerify.VerifyPredicatesByType(
        DataSharePredicatesVerify::PredicatesVerifyType::SINGLE_2_PARAMS_PUBLIC, publicTest);
    EXPECT_EQ(ret, E_OK);

    // verifyType is SINGLE_3_PARAMS_PUBLIC
    ret = predicatesVerify.VerifyPredicatesByType(
        DataSharePredicatesVerify::PredicatesVerifyType::SINGLE_3_PARAMS_PUBLIC, publicTest);
    EXPECT_EQ(ret, E_OK);

    // verifyType is SINGLE_3_PARAMS_PUBLIC and filed is invalid
    OperationItem publicTest1 {};
    publicTest1.operation = SINGLE_2_PARAMS_PUBLIC_TEST;
    publicTest1.singleParams.push_back("./test");
    ret = predicatesVerify.VerifyPredicatesByType(
        DataSharePredicatesVerify::PredicatesVerifyType::SINGLE_3_PARAMS_PUBLIC, publicTest1);
    EXPECT_EQ(ret, E_FIELD_INVALID);
    LOG_INFO("DataSharePredicatesVerifyTest VerifyPredicatesByType002::End");
}

/**
* @tc.name: VerifyPredicatesByType003
* @tc.desc: Verify predicates validation by type for single parameter system operations
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Verify predicates with SINGLE_2_PARAMS_SYS type and system test operation
    2. Verify predicates with SINGLE_3_PARAMS_SYS type and system test operation
    3. Verify predicates with SINGLE_3_PARAMS_SYS type and illegal field
* @tc.experct:
    1. First two verification operations return E_OK
    2. Third verification operation returns E_FIELD_ILLEGAL
*/
HWTEST_F(DataSharePredicatesVerifyTest, VerifyPredicatesByType003, TestSize.Level0)
{
    LOG_INFO("DataSharePredicatesVerifyTest VerifyPredicatesByType003::Start");
    DataSharePredicatesVerify predicatesVerify;
    // verifyType is SINGLE_2_PARAMS_SYS
    OperationItem publicTest {};
    publicTest.operation = SINGLE_2_PARAMS_SYS_TEST;
    publicTest.singleParams.push_back("test");
    auto ret = predicatesVerify.VerifyPredicatesByType(
        DataSharePredicatesVerify::PredicatesVerifyType::SINGLE_2_PARAMS_SYS, publicTest);
    EXPECT_EQ(ret, E_OK);

    // verifyType is SINGLE_3_PARAMS_SYS
    ret = predicatesVerify.VerifyPredicatesByType(
        DataSharePredicatesVerify::PredicatesVerifyType::SINGLE_3_PARAMS_SYS, publicTest);
    EXPECT_EQ(ret, E_OK);

    // verifyType is SINGLE_3_PARAMS_SYS and filed is invalid
    OperationItem publicTest1 {};
    publicTest1.operation = SINGLE_3_PARAMS_SYS_TEST;
    publicTest1.singleParams.push_back("test and true");
    ret = predicatesVerify.VerifyPredicatesByType(
        DataSharePredicatesVerify::PredicatesVerifyType::SINGLE_3_PARAMS_SYS, publicTest1);
    EXPECT_EQ(ret, E_FIELD_ILLEGAL);
    LOG_INFO("DataSharePredicatesVerifyTest VerifyPredicatesByType003::End");
}

/**
* @tc.name: VerifyPredicatesByType004
* @tc.desc: Verify predicates validation by type for multi parameter system operations
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Verify predicates with MULTI_2_PARAMS_SYS type and normal multi parameters
    2. Verify predicates with MULTI_2_PARAMS_SYS type and abnormal multi parameters
* @tc.experct:
    1. First verification operation returns E_OK
    2. Second verification operation returns E_FIELD_ILLEGAL
*/
HWTEST_F(DataSharePredicatesVerifyTest, VerifyPredicatesByType004, TestSize.Level0)
{
    LOG_INFO("DataSharePredicatesVerifyTest VerifyPredicatesByType004::Start");
    DataSharePredicatesVerify predicatesVerify;
    // verifyType is MULTI_2_PARAMS_SYS
    OperationItem publicTest {};
    publicTest.operation = MULTI_2_PARAMS_SYS_TEST;
    std::vector<std::string> normalVec = {"test1", "test2"};
    publicTest.multiParams.push_back(normalVec);
    auto ret = predicatesVerify.VerifyPredicatesByType(
        DataSharePredicatesVerify::PredicatesVerifyType::MULTI_2_PARAMS_SYS, publicTest);
    EXPECT_EQ(ret, E_OK);

    // verifyType is MULTI_2_PARAMS_SYS and fileds is invalid
    OperationItem publicTest1 {};
    publicTest1.operation = MULTI_2_PARAMS_SYS_TEST;
    std::vector<std::string> abnormalVec = {"test", "(test"};
    publicTest1.multiParams.push_back(abnormalVec);
    ret = predicatesVerify.VerifyPredicatesByType(
        DataSharePredicatesVerify::PredicatesVerifyType::MULTI_2_PARAMS_SYS, publicTest1);
    EXPECT_EQ(ret, E_FIELD_ILLEGAL);
    LOG_INFO("DataSharePredicatesVerifyTest VerifyPredicatesByType004::End");
}

/**
* @tc.name: CheckParamNum001
* @tc.desc: Verify parameter number check for predicates validation
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Check parameter number with VERIFY_DEFAULT type and normal parameters
    2. Check parameter number with SINGLE_2_PARAMS_SYS type and abnormal parameters
* @tc.experct:
    1. First check returns true
    2. Second check returns false
*/
HWTEST_F(DataSharePredicatesVerifyTest, CheckParamNum001, TestSize.Level0)
{
    LOG_INFO("DataSharePredicatesVerifyTest CheckParamNum001::Start");
    DataSharePredicatesVerify predicatesVerify;
    // verifyType is MULTI_2_PARAMS_SYS
    OperationItem publicTest {};
    publicTest.operation = MULTI_2_PARAMS_SYS_TEST;
    std::vector<std::string> normalVec = {"test1", "test2"};
    publicTest.multiParams.push_back(normalVec);
    auto ret = predicatesVerify.CheckParamNum(
        DataSharePredicatesVerify::PredicatesVerifyType::VERIFY_DEFAULT, publicTest);
    EXPECT_TRUE(ret);

    // verifyType is SINGLE_2_PARAMS_SYS and fileds is invalid
    OperationItem publicTest1 {};
    publicTest1.operation = SINGLE_2_PARAMS_SYS_TEST;
    std::vector<std::string> abnormalVec = {"test", "(test"};
    publicTest1.multiParams.push_back(abnormalVec);
    ret = predicatesVerify.VerifyPredicatesByType(
        DataSharePredicatesVerify::PredicatesVerifyType::SINGLE_2_PARAMS_SYS, publicTest1);
    EXPECT_FALSE(ret);
    LOG_INFO("DataSharePredicatesVerifyTest CheckParamNum001::End");
}

/**
* @tc.name: VerifyField001
* @tc.desc: Verify field validation for various field formats
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Verify empty field
    2. Verify COLNAME_OPTIONAL_BRACKETS field
    3. Verify COLNAME_ALIAS_OPTIONAL_BRACKETS field
    4. Verify TABLENAME_DOT_COLNAME_OPTIONAL_BRACKETS field
    5. Verify abnormal field
* @tc.experct:
    1. Empty field verification returns true
    2. COLNAME_OPTIONAL_BRACKETS verification returns true
    3. COLNAME_ALIAS_OPTIONAL_BRACKETS verification returns true
    4. TABLENAME_DOT_COLNAME_OPTIONAL_BRACKETS verification returns true
    5. Abnormal field verification returns false
*/
HWTEST_F(DataSharePredicatesVerifyTest, VerifyField001, TestSize.Level0)
{
    LOG_INFO("DataSharePredicatesVerifyTest VerifyField001::Start");
    DataSharePredicatesVerify predicatesVerify;
    // empty field test
    std::string test = "";
    auto ret = predicatesVerify.VerifyField(test);
    EXPECT_TRUE(ret);

    // COLNAME_OPTIONAL_BRACKETS test
    test = "COLNAME_OPTIONAL_BRACKETS";
    ret = predicatesVerify.VerifyField(test);
    EXPECT_TRUE(ret);

    // COLNAME_ALIAS_OPTIONAL_BRACKETS test
    test = "(colName as name)";
    ret = predicatesVerify.VerifyField(test);
    EXPECT_FALSE(ret);

    // TABLENAME_DOT_COLNAME_OPTIONAL_BRACKETS test
    test = "[tableName.columnName]";
    ret = predicatesVerify.VerifyField(test);
    EXPECT_TRUE(ret);

    // TABLENAME_DOT_COLNAME_OPTIONAL_BRACKETS test
    test = "[$.columnName]";
    ret = predicatesVerify.VerifyField(test);
    EXPECT_TRUE(ret);

    // abnormal branch test
    test = "columnName]";
    ret = predicatesVerify.VerifyField(test);
    EXPECT_FALSE(ret);

    test = "test..";
    ret = predicatesVerify.VerifyField(test);
    EXPECT_FALSE(ret);

    test = "(.test.)";
    ret = predicatesVerify.VerifyField(test);
    EXPECT_FALSE(ret);
    LOG_INFO("DataSharePredicatesVerifyTest VerifyField001::End");
}

/**
* @tc.name: VerifyFields001
* @tc.desc: Verify fields validation for various field lists
* @tc.type: FUNC
* @tc.require: None
* @tc.precon: None
* @tc.step:
    1. Verify fields list with abnormal fields
    2. Verify fields list with normal fields
* @tc.experct:
    1. First verification returns false
    2. Second verification returns true
*/
HWTEST_F(DataSharePredicatesVerifyTest, VerifyFields001, TestSize.Level0)
{
    LOG_INFO("DataSharePredicatesVerifyTest VerifyFields001::Start");
    DataSharePredicatesVerify predicatesVerify;
    std::vector<std::string> fieldsTest = {"test", "../test"};
    auto ret = predicatesVerify.VerifyFields(fieldsTest);
    EXPECT_FALSE(ret);

    fieldsTest = {"test1", "test2"};
    ret = predicatesVerify.VerifyFields(fieldsTest);
    EXPECT_TRUE(ret);
    LOG_INFO("DataSharePredicatesVerifyTest VerifyFields001::End");
}
}
}