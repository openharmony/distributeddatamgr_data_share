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

#include "datashare_stub.h"

#include <gtest/gtest.h>

#include "accesstoken_kit.h"
#include "data_ability_observer_stub.h"
#include "datashare_business_error.h"
#include "datashare_errno.h"
#include "datashare_itypes_utils.h"
#include "datashare_log.h"
#include "datashare_operation_statement.h"
#include "datashare_stub_impl.h"
#include "extension_manager_proxy.h"
#include "idatashare.h"
#include "ipc_skeleton.h"
#include "ipc_types.h"
#include "ishared_result_set.h"


namespace OHOS {
namespace DataShare {
using namespace testing::ext;
using namespace DistributedShare::DataShare;
using namespace OHOS::AAFwk;
using ChangeInfo = AAFwk::ChangeInfo;
class DataShareStubTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};


class IDataAbilityObserverTest : public DataAbilityObserverStub {
public:
    IDataAbilityObserverTest() {}
    ~IDataAbilityObserverTest()
    {}

    void OnChange()
    {
        GTEST_LOG_(INFO) << "OnChange enter";
    }
};

class TestDataShareStub : public DataShareStub {
public:
    TestDataShareStub() {}
    ~TestDataShareStub() {}
    int OpenFile(const Uri &uri, const std::string &mode)
    {
        return 0;
    }
    int OpenRawFile(const Uri &uri, const std::string &mode)
    {
        return 0;
    }
    int Insert(const Uri &uri, const DataShareValuesBucket &value)
    {
        return 0;
    }
    int Update(const Uri &uri, const DataSharePredicates &predicates, const DataShareValuesBucket &value)
    {
        return 0;
    }
    int Delete(const Uri &uri, const DataSharePredicates &predicates)
    {
        return 0;
    }
    std::shared_ptr<DataShareResultSet> Query(const Uri &uri, const DataSharePredicates &predicates,
        std::vector<std::string> &columns, DatashareBusinessError &businessError)
    {
        return nullptr;
    }
    std::string GetType(const Uri &uri)
    {
        return "";
    }
    int BatchInsert(const Uri &uri, const std::vector<DataShareValuesBucket> &values)
    {
        return 0;
    }
    bool RegisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver)
    {
        return true;
    }
    bool UnregisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver)
    {
        return true;
    }
    bool NotifyChange(const Uri &uri)
    {
        return true;
    }
    Uri NormalizeUri(const Uri &uri)
    {

        return uri;
    }
    Uri DenormalizeUri(const Uri &uri)
    {
        return uri;
    }
    std::vector<std::string> GetFileTypes(const Uri &uri, const std::string &mimeTypeFilter)
    {
        return {};
    }
};

std::string DATA_SHARE_URI = "datashare:///com.acts.datasharetest";
std::shared_ptr<DataShareStub> dataShareStub = std::make_shared<TestDataShareStub>();
std::u16string InterfaceToken = u"OHOS.DataShare.IDataShare";

void DataShareStubTest::SetUpTestCase(void) {}
void DataShareStubTest::TearDownTestCase(void) {}
void DataShareStubTest::SetUp(void) {}
void DataShareStubTest::TearDown(void) {}

/**
* @tc.name: DataShareStub_CmdRegisterObserverExtProvider_Test_001
* @tc.desc: test CmdRegisterObserverExtProvideroteRequest default func
* @tc.type: FUNC
*/
HWTEST_F(DataShareStubTest, DataShareStub_CmdRegisterObserverExtProvider_Test_001, TestSize.Level0)
{
    LOG_INFO("DataShareStub_CmdRegisterObserverExtProvider_Test_001::Start");

    Uri uri(DATA_SHARE_URI);
    sptr<IDataAbilityObserverTest> dataObserver = new (std::nothrow) IDataAbilityObserverTest();
    ASSERT_NE(dataObserver, nullptr);
    uint32_t code = static_cast<uint32_t>(IDataShareInterfaceCode::CMD_REGISTER_OBSERVEREXT_PROVIDER);

    MessageParcel data;
    data.WriteInterfaceToken(InterfaceToken);
    bool ret = ITypesUtil::Marshal(data, uri, dataObserver->AsObject(), true);
    EXPECT_TRUE(ret);

    MessageParcel reply;
    MessageOption option;
    ASSERT_NE(dataShareStub, nullptr);
    int errCode = dataShareStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(errCode, 0);
    ret = ITypesUtil::Unmarshal(reply, errCode);
    EXPECT_TRUE(ret);
    EXPECT_EQ(errCode, 1);

    LOG_INFO("DataShareStub_CmdRegisterObserverExtProvider_Test_001::End");
}

/**
* @tc.name: DataShareStub_CmdUnregisterObserverExtProvider_Test_001
* @tc.desc: test CmdUnregisterObserverExtProvideroteRequest default func
* @tc.type: FUNC
*/
HWTEST_F(DataShareStubTest, DataShareStub_CmdUnregisterObserverExtProvider_Test_001, TestSize.Level0)
{
    LOG_INFO("DataShareStub_CmdUnregisterObserverExtProvider_Test_001::Start");

    Uri uri(DATA_SHARE_URI);
    sptr<IDataAbilityObserverTest> dataObserver = new (std::nothrow) IDataAbilityObserverTest();
    ASSERT_NE(dataObserver, nullptr);
    uint32_t code = static_cast<uint32_t>(IDataShareInterfaceCode::CMD_UNREGISTER_OBSERVEREXT_PROVIDER);

    MessageParcel data;
    data.WriteInterfaceToken(InterfaceToken);
    bool ret = ITypesUtil::Marshal(data, uri, dataObserver->AsObject());
    EXPECT_TRUE(ret);

    MessageParcel reply;
    MessageOption option;
    int errCode = dataShareStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(errCode, 0);
    ret = ITypesUtil::Unmarshal(reply, errCode);
    EXPECT_TRUE(ret);
    EXPECT_EQ(errCode, 1);

    LOG_INFO("DataShareStub_CmdUnregisterObserverExtProvider_Test_001::End");
}

/**
* @tc.name: DataShareStub_CmdNotifyChangeExtProvider_Test_001
* @tc.desc: test CmdNotifyChangeExtProvider default func
* @tc.type: FUNC
*/
HWTEST_F(DataShareStubTest, DataShareStub_CmdNotifyChangeExtProvider_Test_001, TestSize.Level0)
{
    LOG_INFO("DataShareStub_CmdNotifyChangeExtProvider_Test_001::Start");

    Uri uri(DATA_SHARE_URI);
    ChangeInfo changeInfo = { ChangeInfo::ChangeType::INSERT, { uri } };
    uint32_t code = static_cast<uint32_t>(IDataShareInterfaceCode::CMD_NOTIFY_CHANGEEXT_PROVIDER);

    MessageParcel data;
    data.WriteInterfaceToken(InterfaceToken);
    bool ret = ChangeInfo::Marshalling(changeInfo, data);
    EXPECT_TRUE(ret);

    MessageParcel reply;
    MessageOption option;
    int errCode = dataShareStub->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(errCode, 0);
    ret = ITypesUtil::Unmarshal(reply, errCode);
    EXPECT_TRUE(ret);
    EXPECT_EQ(errCode, 1);

    LOG_INFO("DataShareStub_CmdNotifyChangeExtProvider_Test_001::End");
}
}
}