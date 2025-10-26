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
} // namespace DataShare
}