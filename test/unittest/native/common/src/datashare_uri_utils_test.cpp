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
#define LOG_TAG "datashare_uri_utils_test"

#include <gtest/gtest.h>
#include <unistd.h>

#include "datashare_uri_utils.h"
#include "log_print.h"

namespace OHOS {
namespace DataShare {
using namespace testing::ext;
using namespace OHOS::DataShare;
class DataShareURIUtilsTest : public testing::Test {
public:
    static void SetUpTestCase(void){};
    static void TearDownTestCase(void){};
    void SetUp(){};
    void TearDown(){};
};

/**
* @tc.name: GetUserFromUri_001
* @tc.desc: Test getting user from URI without user parameter
* @tc.type: FUNC
* @tc.require: NA
* @tc.precon: NA
* @tc.step:
* 1. Create URI "datashare:///com.acts.datasharetest" without user parameter
* 2. Call DataShareURIUtils::GetUserFromUri() with the URI
* 3. Check the return result and user value
* @tc.expect:
* 1. The result is true
* 2. The user value is -1 (default)
*/
HWTEST_F(DataShareURIUtilsTest, GetUserFromUri_001, TestSize.Level0)
{
    ZLOGI("GetUserFromUri_001 starts");
    std::string uri =  "datashare:///com.acts.datasharetest";
    auto [res, user] = DataShareURIUtils::GetUserFromUri(uri);
    EXPECT_EQ(res, true);
    EXPECT_EQ(user, -1);
    ZLOGI("GetUserFromUri_001 ends");
}

/**
* @tc.name: GetUserFromUri_002
* @tc.desc: Test getting valid positive user from URI
* @tc.type: FUNC
* @tc.require: NA
* @tc.precon: NA
* @tc.step:
* 1. Create URI "datashare:///com.acts.datasharetest?user=100" with valid user parameter
* 2. Call DataShareURIUtils::GetUserFromUri() with the URI
* 3. Check the return result and user value
* @tc.expect:
* 1. The result is true
* 2. The user value is 100
*/
HWTEST_F(DataShareURIUtilsTest, GetUserFromUri_002, TestSize.Level0)
{
    ZLOGI("GetUserFromUri_002 starts");
    std::string uri =  "datashare:///com.acts.datasharetest?user=100";
    auto [res, user] = DataShareURIUtils::GetUserFromUri(uri);
    EXPECT_EQ(res, true);
    EXPECT_EQ(user, 100);
    ZLOGI("GetUserFromUri_002 ends");
}

/**
* @tc.name: GetUserFromUri_003
* @tc.desc: Test getting user from URI with non-numeric user parameter
* @tc.type: FUNC
* @tc.require: NA
* @tc.precon: NA
* @tc.step:
* 1. Create URI "datashare:///com.acts.datasharetest?user=f" with invalid user parameter
* 2. Call DataShareURIUtils::GetUserFromUri() with the URI
* 3. Check the return result and user value
* @tc.expect:
* 1. The result is false
* 2. The user value is -1
*/
HWTEST_F(DataShareURIUtilsTest, GetUserFromUri_003, TestSize.Level0)
{
    ZLOGI("GetUserFromUri_003 starts");
    std::string uri =  "datashare:///com.acts.datasharetest?user=f";
    auto [res, user] = DataShareURIUtils::GetUserFromUri(uri);
    EXPECT_EQ(res, false);
    EXPECT_EQ(user, -1);
    ZLOGI("GetUserFromUri_003 ends");
}

/**
* @tc.name: GetUserFromUri_004
* @tc.desc: Test getting user from URI with negative user parameter
* @tc.type: FUNC
* @tc.require: NA
* @tc.precon: NA
* @tc.step:
* 1. Create URI "datashare:///com.acts.datasharetest?user=-1" with negative user parameter
* 2. Call DataShareURIUtils::GetUserFromUri() with the URI
* 3. Check the return result and user value
* @tc.expect:
* 1. The result is false
* 2. The user value is -1
*/
HWTEST_F(DataShareURIUtilsTest, GetUserFromUri_004, TestSize.Level0)
{
    ZLOGI("GetUserFromUri_004 starts");
    std::string uri =  "datashare:///com.acts.datasharetest?user=-1";
    auto [res, user] = DataShareURIUtils::GetUserFromUri(uri);
    EXPECT_EQ(res, false);
    EXPECT_EQ(user, -1);
    ZLOGI("GetUserFromUri_004 ends");
}

/**
* @tc.name: GetUserFromUri_005
* @tc.desc: Test getting user from URI with empty user parameter
* @tc.type: FUNC
* @tc.require: NA
* @tc.precon: NA
* @tc.step:
* 1. Create URI "datashare:///com.acts.datasharetest?user=" with empty user parameter
* 2. Call DataShareURIUtils::GetUserFromUri() with the URI
* 3. Check the return result and user value
* @tc.expect:
* 1. The result is true
* 2. The user value is -1
*/
HWTEST_F(DataShareURIUtilsTest, GetUserFromUri_005, TestSize.Level0)
{
    ZLOGI("GetUserFromUri_005 starts");
    std::string uri =  "datashare:///com.acts.datasharetest?user=";
    auto [res, user] = DataShareURIUtils::GetUserFromUri(uri);
    EXPECT_EQ(res, true);
    EXPECT_EQ(user, -1);
    ZLOGI("GetUserFromUri_005 ends");
}

/**
* @tc.name: GetUserFromUri_006
* @tc.desc: Test getting user from URI with whitespace user parameter
* @tc.type: FUNC
* @tc.require: NA
* @tc.precon: NA
* @tc.step:
* 1. Create URI "datashare:///com.acts.datasharetest?user= " with whitespace user parameter
* 2. Call DataShareURIUtils::GetUserFromUri() with the URI
* 3. Check the return result and user value
* @tc.expect:
* 1. The result is false
* 2. The user value is -1
*/
HWTEST_F(DataShareURIUtilsTest, GetUserFromUri_006, TestSize.Level0)
{
    ZLOGI("GetUserFromUri_006 starts");
    std::string uri =  "datashare:///com.acts.datasharetest?user= ";
    auto [res, user] = DataShareURIUtils::GetUserFromUri(uri);
    EXPECT_EQ(res, false);
    EXPECT_EQ(user, -1);
    ZLOGI("GetUserFromUri_006 ends");
}

/**
* @tc.name: GetUserFromUri_007
* @tc.desc: Test getting user from URI with overflow user parameter
* @tc.type: FUNC
* @tc.require: NA
* @tc.precon: NA
* @tc.step:
    1. Create URI "datashare:///com.acts.datasharetest?user=2147483648" with overflow user parameter
    2. Call DataShareURIUtils::GetUserFromUri() with the URI
    3. Check the return result and user value
* @tc.expect:
    1. The result is false
    2. The user value is -1
*/
HWTEST_F(DataShareURIUtilsTest, GetUserFromUri_007, TestSize.Level0)
{
    ZLOGI("GetUserFromUri_007 starts");
    std::string uri =  "datashare:///com.acts.datasharetest?user=2147483648";
    auto [res, user] = DataShareURIUtils::GetUserFromUri(uri);
    EXPECT_EQ(res, false);
    EXPECT_EQ(user, -1);
    ZLOGI("GetUserFromUri_007 ends");
}

/**
* @tc.name: GetUserFromUri_008
* @tc.desc: Test getting user from URI with multiple user parameters
* @tc.type: FUNC
* @tc.require: NA
* @tc.precon: NA
* @tc.step:
* 1. Create URI "datashare:///com.acts.datasharetest?user=100&user=111" with multiple user parameters
* 2. Call DataShareURIUtils::GetUserFromUri() with the URI
* 3. Check the return result and user value
* @tc.expect:
* 1. The result is true
* 2. The user value is 111 (last occurrence)
*/
HWTEST_F(DataShareURIUtilsTest, GetUserFromUri_008, TestSize.Level0)
{
    ZLOGI("GetUserFromUri_008 starts");
    std::string uri =  "datashare:///com.acts.datasharetest?user=100&user=111";
    auto [res, user] = DataShareURIUtils::GetUserFromUri(uri);
    EXPECT_EQ(res, true);
    EXPECT_EQ(user, 111);
    ZLOGI("GetUserFromUri_008 ends");
}

/**
* @tc.name: GetQueryParams_001
* @tc.desc: Test getting query parameters from URI
* @tc.type: FUNC
* @tc.require: NA
* @tc.precon: NA
* @tc.step:
* 1. Create URI "datashare:///com.acts.datasharetest?user=100&srcToken=12345" with query parameters
* 2. Call DataShareURIUtils::GetQueryParams() with the URI
* 3. Check if the result is not empty
* @tc.expect:The returned query parameters are not empty
*/
HWTEST_F(DataShareURIUtilsTest, GetQueryParams_001, TestSize.Level0)
{
    ZLOGI("GetQueryParams_001 starts");
    std::string uri =  "datashare:///com.acts.datasharetest?user=100&srcToken=12345";
    auto res = DataShareURIUtils::GetQueryParams(uri);
    EXPECT_EQ(res.empty(), false);
    ZLOGI("GetQueryParams_001 ends");
}

/**
* @tc.name: Strtoul_001
* @tc.desc: Test converting empty string to unsigned integer
* @tc.type: FUNC
* @tc.require: NA
* @tc.precon: NA
* @tc.step:
* 1. Create empty string
* 2. Call DataShareURIUtils::Strtoul() with the empty string
* 3. Check the return result and converted value
* @tc.expect:
* 1. The result is false
* 2. The converted value is 0
*/
HWTEST_F(DataShareURIUtilsTest, Strtoul_001, TestSize.Level0)
{
    ZLOGI("Strtoul_001 starts");
    std::string str =  "";
    std::pair<bool, uint32_t> res = DataShareURIUtils::Strtoul(str);
    EXPECT_EQ(res.first, false);
    EXPECT_EQ(res.second, 0);
    ZLOGI("Strtoul_001 ends");
}

/**
* @tc.name: FormatUri_001
* @tc.desc: Test formatting URI by removing query parameters
* @tc.type: FUNC
* @tc.require: NA
* @tc.precon: NA
* @tc.step:
* 1. Create URI "datashare:///com.acts.datasharetest?user=100&srcToken=12345" with query parameters
* 2. Call DataShareURIUtils::FormatUri() with the URI
* 3. Check the formatted URI
* @tc.expect:The formatted URI is "datashare:///com.acts.datasharetest" without query parameters
*/
HWTEST_F(DataShareURIUtilsTest, FormatUri_001, TestSize.Level0)
{
    ZLOGI("FormatUri_001 starts");
    std::string uri =  "datashare:///com.acts.datasharetest?user=100&srcToken=12345";
    std::string res = DataShareURIUtils::FormatUri(uri);
    EXPECT_EQ(res, "datashare:///com.acts.datasharetest");
    ZLOGI("FormatUri_001 ends");
}

/**
* @tc.name: FormatUri_002
* @tc.desc: Test formatting URI without query parameters
* @tc.type: FUNC
* @tc.require: NA
* @tc.precon: NA
* @tc.step:
* 1. Create URI "datashare:///com.acts.datasharetest" without query parameters
* 2. Call DataShareURIUtils::FormatUri() with the URI
* 3. Check the formatted URI
* @tc.expect:The formatted URI is the same as input "datashare:///com.acts.datasharetest"
*/
HWTEST_F(DataShareURIUtilsTest, FormatUri_002, TestSize.Level0)
{
    ZLOGI("FormatUri_002 starts");
    std::string uri =  "datashare:///com.acts.datasharetest";
    std::string res = DataShareURIUtils::FormatUri(uri);
    EXPECT_EQ(res, "datashare:///com.acts.datasharetest");
    ZLOGI("FormatUri_002 ends");
}

/**
 * @tc.name: ExtractFirstPathSegment_001
 * @tc.desc: Test extracting the first path segment from a URI with authority
 * @tc.type: FUNC
 * @tc.require: NA
 * @tc.precon: NA
 * @tc.step:
 * 1. Create URI "datashare://com.example.app" with authority
 * 2. Call DataShareURIUtils::ExtractFirstPathSegment() with the URI
 * 3. Check the extracted path segment
 * @tc.expect: The extracted path segment is "com.example.app"
 */
HWTEST_F(DataShareURIUtilsTest, ExtractFirstPathSegment_001, TestSize.Level0)
{
    ZLOGI("ExtractFirstPathSegment_001 starts");
    std::string uri = "datashare://com.example.app";
    std::string result = DataShareURIUtils::ExtractFirstPathSegment(uri);
    EXPECT_EQ(result, "com.example.app");
    ZLOGI("ExtractFirstPathSegment_001 ends");
}

/**
 * @tc.name: ExtractFirstPathSegment_002
 * @tc.desc: Test extracting the first path segment from a URI with ":///" format
 * @tc.type: FUNC
 * @tc.require: NA
 * @tc.precon: NA
 * @tc.step:
 * 1. Create URI "datashare:///system/data" with ":///" format
 * 2. Call DataShareURIUtils::ExtractFirstPathSegment() with the URI
 * 3. Check the extracted path segment
 * @tc.expect: The extracted path segment is "system"
 */
HWTEST_F(DataShareURIUtilsTest, ExtractFirstPathSegment_002, TestSize.Level0)
{
    ZLOGI("ExtractFirstPathSegment_002 starts");
    std::string uri = "datashare:///system/data";
    std::string result = DataShareURIUtils::ExtractFirstPathSegment(uri);
    EXPECT_EQ(result, "system");
    ZLOGI("ExtractFirstPathSegment_002 ends");
}

/**
 * @tc.name: ExtractFirstPathSegment_003
 * @tc.desc: Test extracting the first path segment from a URI with multiple paths
 * @tc.type: FUNC
 * @tc.require: NA
 * @tc.precon: NA
 * @tc.step:
 * 1. Create URI "datashareproxy://com.example.app3/data" with multiple paths
 * 2. Call DataShareURIUtils::ExtractFirstPathSegment() with the URI
 * 3. Check the extracted path segment
 * @tc.expect: The extracted path segment is "com.example.app3"
 */
HWTEST_F(DataShareURIUtilsTest, ExtractFirstPathSegment_003, TestSize.Level0)
{
    ZLOGI("ExtractFirstPathSegment_003 starts");
    std::string uri = "datashareproxy://com.example.app3/data";
    std::string result = DataShareURIUtils::ExtractFirstPathSegment(uri);
    EXPECT_EQ(result, "com.example.app3");
    ZLOGI("ExtractFirstPathSegment_003 ends");
}

/**
 * @tc.name: ExtractFirstPathSegment_004
 * @tc.desc: Test extracting the first path segment from a URI without a valid format
 * @tc.type: FUNC
 * @tc.require: NA
 * @tc.precon: NA
 * @tc.step:
 * 1. Create URI "invalid_uri_format" without a valid format
 * 2. Call DataShareURIUtils::ExtractFirstPathSegment() with the URI
 * 3. Check the extracted path segment
 * @tc.expect: The extracted path segment is an empty string
 */
HWTEST_F(DataShareURIUtilsTest, ExtractFirstPathSegment_004, TestSize.Level0)
{
    ZLOGI("ExtractFirstPathSegment_004 starts");
    std::string uri = "invalid_uri_format";
    std::string result = DataShareURIUtils::ExtractFirstPathSegment(uri);
    EXPECT_EQ(result, "");
    ZLOGI("ExtractFirstPathSegment_004 ends");
}

/**
 * @tc.name: ExtractFirstPathSegment_005
 * @tc.desc: Test extracting the first path segment from a URI with only "://"
 * @tc.type: FUNC
 * @tc.require: NA
 * @tc.precon: NA
 * @tc.step:
 * 1. Create URI "datashare://" with only "://"
 * 2. Call DataShareURIUtils::ExtractFirstPathSegment() with the URI
 * 3. Check the extracted path segment
 * @tc.expect: The extracted path segment is an empty string
 */
HWTEST_F(DataShareURIUtilsTest, ExtractFirstPathSegment_005, TestSize.Level0)
{
    ZLOGI("ExtractFirstPathSegment_005 starts");
    std::string uri = "datashare://";
    std::string result = DataShareURIUtils::ExtractFirstPathSegment(uri);
    EXPECT_EQ(result, "");
    ZLOGI("ExtractFirstPathSegment_005 ends");
}

/**
 * @tc.name: ExtractFirstPathSegment_006
 * @tc.desc: Test extracting the first path segment from a URI with only ":///"
 * @tc.type: FUNC
 * @tc.require: NA
 * @tc.precon: NA
 * @tc.step:
 * 1. Create URI "datashare:///" with only ":///"
 * 2. Call DataShareURIUtils::ExtractFirstPathSegment() with the URI
 * 3. Check the extracted path segment
 * @tc.expect: The extracted path segment is an empty string
 */
HWTEST_F(DataShareURIUtilsTest, ExtractFirstPathSegment_006, TestSize.Level0)
{
    ZLOGI("ExtractFirstPathSegment_006 starts");
    std::string uri = "datashare:///";
    std::string result = DataShareURIUtils::ExtractFirstPathSegment(uri);
    EXPECT_EQ(result, "");
    ZLOGI("ExtractFirstPathSegment_006 ends");
}

/**
 * @tc.name: GetSystemAbilityId_001
 * @tc.desc: Test getting system ability ID from URI with various formats, including invalid URIs,
 *           missing authority, invalid SAID values, and valid SAID values.
 * @tc.type: FUNC
 * @tc.require: NA
 * @tc.precon: NA
 * @tc.step:
 * 1. Test URI "datashare:/" - expect result false, value -1
 * 2. Test URI "datashare://SAID=1301" - expect result false, value -1 (missing authority)
 * 3. Test URI "datashare:///SAID=1301" - expect result false, value -1 (missing authority)
 * 4. Test URI "datashare:///SAID=/" - expect result false, value -1 (empty SAID)
 * 5. Test URI "datashare://distributeddata/SAID=rrrr" - expect result false, value -1 (invalid SAID)
 * 6. Test URI "datashare://distributeddata/SAID=1301" - expect result true, value 1301 (valid)
 * 7. Test URI "datashare://distributeddata/SAID=1301/test" - expect result true, value 1301 (valid with path)
 * 8. Test URI "datashare://distributeddata/SAID=16777215" - expect result true, value 16777215 (valid)
 * 9. Test URI "datashare://distributeddata/SAID=16777216" - expect result false (out of saId boundary)
 * 6. Test URI "datashare://distributeddata/SAID=1301/SAID=1111" - expect result true, value 1301 (valid)
 * @tc.expect:
 * 1. URIs without proper authority format return false or -1
 * 2. URIs with invalid SAID values return true with -1
 * 3. URIs with valid SAID values return true with correct ID (1301)
 */
HWTEST_F(DataShareURIUtilsTest, GetSystemAbilityId_001, TestSize.Level0)
{
    ZLOGI("GetSystemAbilityId_001 starts");
    std::string uri = "datashare:/";
    auto [res, value] = DataShareURIUtils::GetSystemAbilityId(uri);
    EXPECT_FALSE(res);
    EXPECT_EQ(value, -1);
    uri = "datashare://SAID=1301";
    std::tie(res, value) = DataShareURIUtils::GetSystemAbilityId(uri);
    EXPECT_FALSE(res);
    EXPECT_EQ(value, -1);
    uri = "datashare:///SAID=1301";
    std::tie(res, value) = DataShareURIUtils::GetSystemAbilityId(uri);
    EXPECT_FALSE(res);
    EXPECT_EQ(value, -1);
    uri = "datashare:///SAID=/";
    std::tie(res, value) = DataShareURIUtils::GetSystemAbilityId(uri);
    EXPECT_FALSE(res);
    EXPECT_EQ(value, -1);
    uri = "datashare://distributeddata/SAID=rrrr";
    std::tie(res, value) = DataShareURIUtils::GetSystemAbilityId(uri);
    EXPECT_FALSE(res);
    EXPECT_EQ(value, -1);
    uri = "datashare://distributeddata/SAID=1301";
    std::tie(res, value) = DataShareURIUtils::GetSystemAbilityId(uri);
    EXPECT_TRUE(res);
    EXPECT_EQ(value, 1301);
    uri = "datashare://distributeddata/SAID=1301/test";
    std::tie(res, value) = DataShareURIUtils::GetSystemAbilityId(uri);
    EXPECT_TRUE(res);
    EXPECT_EQ(value, 1301);
    ZLOGI("GetSystemAbilityId_001 ends");
    uri = "datashare://distributeddata/SAID=16777215";
    std::tie(res, value) = DataShareURIUtils::GetSystemAbilityId(uri);
    EXPECT_TRUE(res);
    EXPECT_EQ(value, 16777215); // 16777215 is boundary value
    uri = "datashare://distributeddata/SAID=16777216";
    std::tie(res, value) = DataShareURIUtils::GetSystemAbilityId(uri);
    EXPECT_FALSE(res);
    EXPECT_EQ(value, -1);
    uri = "datashare://distributeddata/SAID=1301/SAID=1111";
    std::tie(res, value) = DataShareURIUtils::GetSystemAbilityId(uri);
    EXPECT_TRUE(res);
    EXPECT_EQ(value, 1301);
    ZLOGI("GetSystemAbilityId_001 ends");
}
} // namespace DataShare
}