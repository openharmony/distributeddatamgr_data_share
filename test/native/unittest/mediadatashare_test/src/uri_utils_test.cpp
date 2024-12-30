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

HWTEST_F(URIUtilsTest, GetUserFromUri_001, TestSize.Level0)
{
    ZLOGI("GetUserFromUri_001 starts");
    std::string uri =  "datashare:///com.acts.datasharetest";
    int32_t user = -1;
    bool res = DataShareURIUtils::GetUserFromUri(uri, user);
    EXPECT_EQ(res, false);
    EXPECT_EQ(user, -1);
    ZLOGI("GetUserFromUri_001 ends");
}

HWTEST_F(URIUtilsTest, GetUserFromUri_002, TestSize.Level0)
{
    ZLOGI("GetUserFromUri_002 starts");
    std::string uri =  "datashare:///com.acts.datasharetest?user=100";
    int32_t user = -1;
    bool res = DataShareURIUtils::GetUserFromUri(uri, user);
    EXPECT_EQ(res, true);
    EXPECT_EQ(user, 100);
    ZLOGI("GetUserFromUri_002 ends");
}

HWTEST_F(URIUtilsTest, GetUserFromUri_003, TestSize.Level0)
{
    ZLOGI("GetUserFromUri_003 starts");
    std::string uri =  "datashare:///com.acts.datasharetest?user=f";
    int32_t user = -1;
    bool res = DataShareURIUtils::GetUserFromUri(uri, user);
    EXPECT_EQ(res, false);
    EXPECT_EQ(user, -1);
    ZLOGI("GetUserFromUri_003 ends");
}

HWTEST_F(URIUtilsTest, GetQueryParams_001, TestSize.Level0)
{
    ZLOGI("GetQueryParams_001 starts");
    std::string uri =  "datashare:///com.acts.datasharetest?user=100&srcToken=12345";
    auto res = DataShareURIUtils::GetQueryParams(uri);
    EXPECT_EQ(res.empty(), false);
    ZLOGI("GetQueryParams_001 ends");
}

HWTEST_F(URIUtilsTest, Strtoul_001, TestSize.Level0)
{
    ZLOGI("Strtoul_001 starts");
    std::string str =  "";
    std::pair<bool, uint32_t> res = DataShareURIUtils::Strtoul(str);
    EXPECT_EQ(res.first, false);
    EXPECT_EQ(res.second, 0);
    ZLOGI("Strtoul_001 ends");
}

HWTEST_F(URIUtilsTest, FormatUri_001, TestSize.Level0)
{
    ZLOGI("FormatUri_001 starts");
    std::string uri =  "datashare:///com.acts.datasharetest?user=100&srcToken=12345";
    std::string res = DataShareURIUtils::FormatUri(uri);
    EXPECT_EQ(res, "datashare:///com.acts.datasharetest");
    ZLOGI("FormatUri_001 ends");
}

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