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

HWTEST_F(DataShareURIUtilsTest, GetUserFromUri_001, TestSize.Level0)
{
    ZLOGI("GetUserFromUri_001 starts");
    std::string uri =  "datashare:///com.acts.datasharetest";
    auto [res, user] = DataShareURIUtils::GetUserFromUri(uri);
    EXPECT_EQ(res, true);
    EXPECT_EQ(user, -1);
    ZLOGI("GetUserFromUri_001 ends");
}

HWTEST_F(DataShareURIUtilsTest, GetUserFromUri_002, TestSize.Level0)
{
    ZLOGI("GetUserFromUri_002 starts");
    std::string uri =  "datashare:///com.acts.datasharetest?user=100";
    auto [res, user] = DataShareURIUtils::GetUserFromUri(uri);
    EXPECT_EQ(res, true);
    EXPECT_EQ(user, 100);
    ZLOGI("GetUserFromUri_002 ends");
}

HWTEST_F(DataShareURIUtilsTest, GetUserFromUri_003, TestSize.Level0)
{
    ZLOGI("GetUserFromUri_003 starts");
    std::string uri =  "datashare:///com.acts.datasharetest?user=f";
    auto [res, user] = DataShareURIUtils::GetUserFromUri(uri);
    EXPECT_EQ(res, false);
    EXPECT_EQ(user, -1);
    ZLOGI("GetUserFromUri_003 ends");
}

HWTEST_F(DataShareURIUtilsTest, GetUserFromUri_004, TestSize.Level0)
{
    ZLOGI("GetUserFromUri_004 starts");
    std::string uri =  "datashare:///com.acts.datasharetest?user=-1";
    auto [res, user] = DataShareURIUtils::GetUserFromUri(uri);
    EXPECT_EQ(res, false);
    EXPECT_EQ(user, -1);
    ZLOGI("GetUserFromUri_004 ends");
}

HWTEST_F(DataShareURIUtilsTest, GetUserFromUri_005, TestSize.Level0)
{
    ZLOGI("GetUserFromUri_005 starts");
    std::string uri =  "datashare:///com.acts.datasharetest?user=";
    auto [res, user] = DataShareURIUtils::GetUserFromUri(uri);
    EXPECT_EQ(res, true);
    EXPECT_EQ(user, -1);
    ZLOGI("GetUserFromUri_005 ends");
}

HWTEST_F(DataShareURIUtilsTest, GetUserFromUri_006, TestSize.Level0)
{
    ZLOGI("GetUserFromUri_006 starts");
    std::string uri =  "datashare:///com.acts.datasharetest?user= ";
    auto [res, user] = DataShareURIUtils::GetUserFromUri(uri);
    EXPECT_EQ(res, false);
    EXPECT_EQ(user, -1);
    ZLOGI("GetUserFromUri_006 ends");
}

HWTEST_F(DataShareURIUtilsTest, GetUserFromUri_007, TestSize.Level0)
{
    ZLOGI("GetUserFromUri_007 starts");
    std::string uri =  "datashare:///com.acts.datasharetest?user=2147483648";
    auto [res, user] = DataShareURIUtils::GetUserFromUri(uri);
    EXPECT_EQ(res, false);
    EXPECT_EQ(user, -1);
    ZLOGI("GetUserFromUri_007 ends");
}

HWTEST_F(DataShareURIUtilsTest, GetUserFromUri_008, TestSize.Level0)
{
    ZLOGI("GetUserFromUri_008 starts");
    std::string uri =  "datashare:///com.acts.datasharetest?user=100&user=111";
    auto [res, user] = DataShareURIUtils::GetUserFromUri(uri);
    EXPECT_EQ(res, true);
    EXPECT_EQ(user, 111);
    ZLOGI("GetUserFromUri_008 ends");
}

HWTEST_F(DataShareURIUtilsTest, GetQueryParams_001, TestSize.Level0)
{
    ZLOGI("GetQueryParams_001 starts");
    std::string uri =  "datashare:///com.acts.datasharetest?user=100&srcToken=12345";
    auto res = DataShareURIUtils::GetQueryParams(uri);
    EXPECT_EQ(res.empty(), false);
    ZLOGI("GetQueryParams_001 ends");
}

HWTEST_F(DataShareURIUtilsTest, Strtoul_001, TestSize.Level0)
{
    ZLOGI("Strtoul_001 starts");
    std::string str =  "";
    std::pair<bool, uint32_t> res = DataShareURIUtils::Strtoul(str);
    EXPECT_EQ(res.first, false);
    EXPECT_EQ(res.second, 0);
    ZLOGI("Strtoul_001 ends");
}

HWTEST_F(DataShareURIUtilsTest, FormatUri_001, TestSize.Level0)
{
    ZLOGI("FormatUri_001 starts");
    std::string uri =  "datashare:///com.acts.datasharetest?user=100&srcToken=12345";
    std::string res = DataShareURIUtils::FormatUri(uri);
    EXPECT_EQ(res, "datashare:///com.acts.datasharetest");
    ZLOGI("FormatUri_001 ends");
}

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