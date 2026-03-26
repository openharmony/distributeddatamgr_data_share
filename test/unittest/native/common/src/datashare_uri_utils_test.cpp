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
} // namespace DataShare
}