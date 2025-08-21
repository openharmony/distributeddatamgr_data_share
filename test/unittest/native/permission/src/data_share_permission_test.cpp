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
#include <gtest/gtest.h>
#include <unistd.h>
#include "datashare_log.h"
#include "data_share_permission.h"
#include "datashare_errno.h"

namespace OHOS {
namespace DataShare {
using namespace testing::ext;
class DataSharePermissionTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void DataSharePermissionTest::SetUpTestCase(void) {}
void DataSharePermissionTest::TearDownTestCase(void) {}
void DataSharePermissionTest::SetUp(void) {}
void DataSharePermissionTest::TearDown(void) {}

/**
 * @tc.name: CheckExtensionTrusts001
 * @tc.desc: test VerifyPermission function when permission is empty
 * @tc.type: FUNC
 * @tc.require:issueICU06G
 * @tc.precon: None
 * @tc.step:
    1.define permission as empty
    2.call VerifyPermission function and check the result
 * @tc.experct: VerifyPermission return true
 */
HWTEST_F(DataSharePermissionTest, VerifyPermissionTest001, TestSize.Level0)
{
    LOG_INFO("DataSharePermissionTest VerifyPermissionTest001::Start");
    uint32_t tokenID = 123;
    std::string permission = "";
    bool result = DataSharePermission::VerifyPermission(tokenID, permission);
    EXPECT_EQ(result, true);
    LOG_INFO("DataSharePermissionTest VerifyPermissionTest001::End");
}

/**
 * @tc.name: CheckExtensionTrusts001
 * @tc.desc: test CheckExtensionTrusts function when consumerToken and providerToken do not have corresponding haps
 * @tc.type: FUNC
 * @tc.require:issueICU06G
 * @tc.precon: None
 * @tc.step:
    1.Create False consumerToken and providerToken
    2.call CheckExtensionTrusts function and check the result
 * @tc.experct: CheckExtensionTrusts reutrn nullptr
 */
HWTEST_F(DataSharePermissionTest, CheckExtensionTrusts001, TestSize.Level0)
{
    LOG_INFO("DataSharePermissionTest CheckExtensionTrusts001::Start");
    uint32_t consumerToken = 123;
    uint32_t providerToken = 123;
    int result = DataSharePermission::CheckExtensionTrusts(consumerToken, providerToken);
    EXPECT_EQ(result, DataShare::E_GET_CALLER_NAME_FAILED);
    LOG_INFO("DataSharePermissionTest CheckExtensionTrusts001::End");
}
}
}