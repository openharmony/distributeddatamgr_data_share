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
#define LOG_TAG "URIUtilsTest"

#include <gtest/gtest.h>
#include <unistd.h>

#include "datashare_uri_utils.h"
#include "log_print.h"

namespace OHOS {
namespace DataShare {
using namespace testing::ext;
using namespace OHOS::DataShare;
class URIUtilsTest : public testing::Test {
public:
    static void SetUpTestCase(void){};
    static void TearDownTestCase(void){};
    void SetUp(){};
    void TearDown(){};
};

/**
 * @tc.name: GetUserFromUri_001
 * @tc.desc: Test the DataShareURIUtils::GetUserFromUri method to retrieve the user ID from a URI that does not contain
 *           a "user" query parameter.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The DataShareURIUtils::GetUserFromUri method is available and returns a pair of (bool, int), where the first
       element indicates operation success and the second is the retrieved user ID.
    2. The test URI string ("datashare:///com.acts.datasharetest") is a valid DataShare URI format.
 * @tc.step:
    1. Define a test URI string as "datashare:///com.acts.datasharetest" (no "user" query parameter).
    2. Call DataShareURIUtils::GetUserFromUri with the test URI, and record the returned (res, user) pair.
    3. Check the boolean result (res) and the integer user ID (user) against the expected values.
 * @tc.expect:
    1. The boolean result (res) returned by GetUserFromUri is true.
    2. The retrieved user ID (user) is -1 (the default value when no "user" parameter exists).
 */
HWTEST_F(URIUtilsTest, GetUserFromUri_001, TestSize.Level0)
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
HWTEST_F(URIUtilsTest, GetUserFromUri_002, TestSize.Level0)
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
HWTEST_F(URIUtilsTest, GetUserFromUri_003, TestSize.Level0)
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
HWTEST_F(URIUtilsTest, GetUserFromUri_004, TestSize.Level0)
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
HWTEST_F(URIUtilsTest, GetUserFromUri_005, TestSize.Level0)
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
HWTEST_F(URIUtilsTest, GetUserFromUri_006, TestSize.Level0)
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
HWTEST_F(URIUtilsTest, GetUserFromUri_007, TestSize.Level0)
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
HWTEST_F(URIUtilsTest, GetUserFromUri_008, TestSize.Level0)
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
 * @tc.desc: Test the DataShareURIUtils::GetQueryParams method to retrieve query parameters from a URI that contains
 *           multiple key-value pairs ("user=100" and "srcToken=12345").
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The DataShareURIUtils::GetQueryParams method returns a collection (e.g., map, list) of query parameters, which
       supports checking if it is empty via the empty() method.
    2. The test URI string ("datashare:///com.acts.datasharetest?user=100&srcToken=12345") is a valid DataShare URI
       with query parameters.
 * @tc.step:
    1. Define a test URI string as "datashare:///com.acts.datasharetest?user=100&srcToken=12345" (with two query
       parameters).
    2. Call DataShareURIUtils::GetQueryParams with the test URI, and record the returned result (res).
    3. Check if the returned result (res) is empty using the empty() method.
 * @tc.expect:
    1. The collection of query parameters returned by GetQueryParams is not empty (res.empty() returns false).
 */
HWTEST_F(URIUtilsTest, GetQueryParams_001, TestSize.Level0)
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
HWTEST_F(URIUtilsTest, Strtoul_001, TestSize.Level0)
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
 * @tc.desc: Test the DataShareURIUtils::FormatUri method to remove query parameters from a URI, returning only the
 *           base URI without the "?" and subsequent key-value pairs.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The DataShareURIUtils::FormatUri method takes a URI string with query parameters and returns a new string
       representing the base URI (without query parameters).
    2. The test URI string ("datashare:///com.acts.datasharetest?user=100&srcToken=12345") is a valid DataShare URI
       with query parameters.
 * @tc.step:
    1. Define a test URI string as "datashare:///com.acts.datasharetest?user=100&srcToken=12345" (with query
       parameters).
    2. Call DataShareURIUtils::FormatUri with the test URI, and record the returned formatted URI (res).
    3. Compare the formatted URI (res) with the expected base URI string.
 * @tc.expect:
    1. The formatted URI returned by FormatUri is "datashare:///com.acts.datasharetest" (no query parameters).
 */
HWTEST_F(URIUtilsTest, FormatUri_001, TestSize.Level0)
{
    ZLOGI("FormatUri_001 starts");
    std::string uri =  "datashare:///com.acts.datasharetest?user=100&srcToken=12345";
    std::string res = DataShareURIUtils::FormatUri(uri);
    EXPECT_EQ(res, "datashare:///com.acts.datasharetest");
    ZLOGI("FormatUri_001 ends");
}

/**
 * @tc.name: FormatUri_002
 * @tc.desc: Test the DataShareURIUtils::FormatUri method to handle a URI that has no query parameters (returns the
 *           original URI string unchanged).
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The DataShareURIUtils::FormatUri method returns the original URI string when there are no query parameters to
       remove.
    2. The test URI string ("datashare:///com.acts.datasharetest") is a valid DataShare URI without query parameters.
 * @tc.step:
    1. Define a test URI string as "datashare:///com.acts.datasharetest" (no query parameters).
    2. Call DataShareURIUtils::FormatUri with the test URI, and record the returned formatted URI (res).
    3. Compare the formatted URI (res) with the original test URI string.
 * @tc.expect:
    1. The formatted URI returned by FormatUri is identical to the input "datashare:///com.acts.datasharetest".
 */
HWTEST_F(URIUtilsTest, FormatUri_002, TestSize.Level0)
{
    ZLOGI("FormatUri_002 starts");
    std::string uri =  "datashare:///com.acts.datasharetest";
    std::string res = DataShareURIUtils::FormatUri(uri);
    EXPECT_EQ(res, "datashare:///com.acts.datasharetest");
    ZLOGI("FormatUri_002 ends");
}
} // namespace DataShare
}