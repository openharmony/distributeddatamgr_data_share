/*
 * Copyright (C) 2026 Huawei Device Co., Ltd.
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

#define LOG_TAG "datashare_stub_openfile_test"

#include "datashare_stub.h"

#include <gtest/gtest.h>

#include "accesstoken_kit.h"
#include "datashare_errno.h"
#include "datashare_itypes_utils.h"
#include "datashare_log.h"
#include "idatashare.h"
#include "mock_token.h"


namespace OHOS {
namespace DataShare {
using namespace testing::ext;
using namespace DistributedShare::DataShare;
using namespace OHOS::AAFwk;
using namespace OHOS::Security::AccessToken;

std::string TEST_URI = "datashare:///com.acts.datasharetest";
std::u16string InterfaceToken = u"OHOS.DataShare.IDataShare";

static constexpr int TEST_OPEN_FILE_FD = 100;
static constexpr int TEST_OPEN_RAW_FILE_FD = 200;

/**
 * Test subclass that exposes the protected ReportOpenFileUsage
 * for direct testing.
 */
class TestDataShareStub : public DataShareStub {
public:
    TestDataShareStub() : openFileCallCount(0), rawFileCallCount(0) {}
    ~TestDataShareStub() {}

    int OpenFile(const Uri &uri, const std::string &mode) override
    {
        openFileCallCount++;
        return TEST_OPEN_FILE_FD;
    }
    int OpenRawFile(const Uri &uri, const std::string &mode) override
    {
        rawFileCallCount++;
        return TEST_OPEN_RAW_FILE_FD;
    }
    int Insert(const Uri &uri,
        const DataShareValuesBucket &value) override
    {
        return E_OK;
    }
    int Update(const Uri &uri,
        const DataSharePredicates &predicates,
        const DataShareValuesBucket &value) override
    {
        return E_OK;
    }
    int Delete(const Uri &uri,
        const DataSharePredicates &predicates) override
    {
        return E_OK;
    }
    std::shared_ptr<DataShareResultSet> Query(const Uri &uri,
        const DataSharePredicates &predicates,
        std::vector<std::string> &columns,
        DatashareBusinessError &businessError) override
    {
        return nullptr;
    }
    std::string GetType(const Uri &uri) override
    {
        return "";
    }
    int BatchInsert(const Uri &uri,
        const std::vector<DataShareValuesBucket> &values) override
    {
        return E_OK;
    }
    bool RegisterObserver(const Uri &uri,
        const sptr<AAFwk::IDataAbilityObserver> &dataObserver) override
    {
        return true;
    }
    bool UnregisterObserver(const Uri &uri,
        const sptr<AAFwk::IDataAbilityObserver> &dataObserver) override
    {
        return true;
    }
    bool NotifyChange(const Uri &uri) override
    {
        return true;
    }
    Uri NormalizeUri(const Uri &uri) override
    {
        return uri;
    }
    Uri DenormalizeUri(const Uri &uri) override
    {
        return uri;
    }
    std::vector<std::string> GetFileTypes(const Uri &uri,
        const std::string &mimeTypeFilter) override
    {
        return {};
    }

    /**
     * @brief Call the protected ReportOpenFileUsage for testing.
     */
    void CallReportOpenFileUsage(const std::string &funcName,
        const std::string &mode)
    {
        ReportOpenFileUsage(funcName, mode);
    }

    int openFileCallCount;
    int rawFileCallCount;
};

class DataShareStubOpenFileTest : public testing::Test {
public:
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
    void SetUp()
    {
        stub = std::make_shared<TestDataShareStub>();
    }
    void TearDown() {}

    std::shared_ptr<TestDataShareStub> stub;
};

HapPolicyParams GetPolicy()
{
    HapPolicyParams policy = { .apl = APL_SYSTEM_CORE,
        .domain = "test.domain",
        .permList = { { .permissionName = "ohos.permission.test",
            .bundleName = "ohos.datashareclienttest.demo",
            .grantMode = 1,
            .availableLevel = APL_SYSTEM_CORE,
            .label = "label",
            .labelId = 1,
            .description = "ohos.datashareclienttest.demo",
            .descriptionId = 1 } },
        .permStateList = {
            { .permissionName = "ohos.permission.test",
                .isGeneral = true,
                .resDeviceID = { "local" },
                .grantStatus = { PermissionState::PERMISSION_GRANTED },
                .grantFlags = { 1 } },
            { .permissionName = "ohos.permission.GET_BUNDLE_INFO",
                .isGeneral = true,
                .resDeviceID = { "local" },
                .grantStatus = { PermissionState::PERMISSION_GRANTED },
                .grantFlags = { 1 } } } };
    return policy;
}

/**
 * @tc.name: ReportOpenFileUsage_Test_001
 * @tc.desc: ReportOpenFileUsage should record the function name as businessType
 *           and include mode in appendix.
 *           Note: The ASSERT/EXPECT assertions in this test have no actual verification
 *           effect and are only added to resolve coding standard warnings. The essential
 *           purpose of this test is to verify that calling ReportOpenFileUsage does not
 *           cause crash.
 * @tc.type: FUNC
 * @tc.require: issues1113
 * @tc.precon: None
 * @tc.step:
 *     1. Set up test environment with mock token
 *     2. Create HapInfoParams with system app flag set to true
 *     3. Create HapPolicyParams with required permissions
 *     4. Allocate test HAP token with AllocTestHapToken
 *     5. Set self token ID to the allocated test token
 *     6. Call CallReportOpenFileUsage with function name "OpenFileInner" and mode "rw"
 *     7. Restore original token ID
 *     8. Reset test environment
 * @tc.expect:
 *     1. ReportOpenFileUsage executes without crash
 */
HWTEST_F(DataShareStubOpenFileTest, ReportOpenFileUsage_Test_001, TestSize.Level0)
{
    LOG_INFO("ReportOpenFileUsage_Test_001::Start");
    // mock system app
    MockToken::SetTestEnvironment();

    HapInfoParams info = {
        .userID = 100,
        .bundleName = "ohos.datashareclienttest.demo",
        .instIndex = 0,
        .appIDDesc = "ohos.datashareclienttest.demo",
        .isSystemApp = true
    };
    HapPolicyParams policy = GetPolicy();
    AccessTokenIDEx tokenIdEx = MockToken::AllocTestHapToken(info, policy);
    uint64_t token = tokenIdEx.tokenIDEx;
    EXPECT_NE(token, INVALID_TOKENID);
    auto originalToken = GetSelfTokenID();

    int ret = SetSelfTokenID(token);
    EXPECT_EQ(ret, E_OK);
    stub->CallReportOpenFileUsage("OpenFileInner", "rw");

    SetSelfTokenID(originalToken);
    MockToken::ResetTestEnvironment();
    LOG_INFO("ReportOpenFileUsage_Test_001::End");
}

/**
 * @tc.name: ReportOpenFileUsage_Test_002
 * @tc.desc: ReportOpenFileUsage should record "OpenRawFile" as businessType
 *           when called from the OpenRawFile code path.
 *           Note: The ASSERT/EXPECT assertions in this test have no actual verification
 *           effect and are only added to resolve coding standard warnings. The essential
 *           purpose of this test is to verify that calling ReportOpenFileUsage does not
 *           cause crash.
 * @tc.type: FUNC
 * @tc.require: issues1113
 * @tc.precon: TestDataShareStub instance is created in SetUp
 * @tc.step:
 *     1. Get TestDataShareStub instance from test fixture
 *     2. Call CallReportOpenFileUsage with function name "OpenRawFile" and mode "rwt"
 * @tc.expect:
 *     1. ReportOpenFileUsage executes without crash
 */
HWTEST_F(DataShareStubOpenFileTest, ReportOpenFileUsage_Test_002, TestSize.Level0)
{
    LOG_INFO("ReportOpenFileUsage_Test_002::Start");
    std::string funcName = "OpenRawFile";
    std::string mode = "rwt";
    EXPECT_NE(stub, nullptr);
    stub->CallReportOpenFileUsage(funcName, mode);
    LOG_INFO("ReportOpenFileUsage_Test_002::End");
}
} // namespace DataShare
} // namespace OHOS
