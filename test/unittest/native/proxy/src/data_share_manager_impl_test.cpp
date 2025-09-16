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

#include "data_share_manager_impl.h"

#include <gtest/gtest.h>

#include "datashare_log.h"
#include "idata_share_client_death_observer.h"
#include "iremote_object.h"
#include "refbase.h"
#include "system_ability_definition.h"
#include "system_ability_status_change_stub.h"


namespace OHOS {
namespace DataShare {
using namespace testing::ext;
using namespace DistributedShare::DataShare;
class DataShareManagerImplTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void DataShareManagerImplTest::SetUpTestCase(void) {}
void DataShareManagerImplTest::TearDownTestCase(void) {}
void DataShareManagerImplTest::SetUp(void) {}
void DataShareManagerImplTest::TearDown(void) {}

class RemoteObjectTest : public IRemoteObject {
public:
    explicit RemoteObjectTest(std::u16string descriptor) : IRemoteObject(descriptor) {}
    ~RemoteObjectTest() {}
    
    int32_t GetObjectRefCount()
    {
        return 0;
    }
    int SendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
    {
        return 0;
    }
    bool AddDeathRecipient(const sptr<DeathRecipient> &recipient)
    {
        return true;
    }
    bool RemoveDeathRecipient(const sptr<DeathRecipient> &recipient)
    {
        return true;
    }
    int Dump(int fd, const std::vector<std::u16string> &args)
    {
        return 0;
    }
};

/**
 * @tc.name: ServiceProxyLoadCallback001
 * @tc.desc: test VerifyPermission function when permission is empty
 * @tc.type: FUNC
 * @tc.require:issueICU06G
 * @tc.precon: None
 * @tc.step:
    1.define permission as empty
    2.call VerifyPermission function and check the result
 * @tc.experct: VerifyPermission return true
 */
HWTEST_F(DataShareManagerImplTest, ServiceProxyLoadCallback001, TestSize.Level0)
{
    LOG_INFO("DataShareManagerImplTest ServiceProxyLoadCallback001::Start");
    // get callback sptr
    sptr<DataShareManagerImpl::ServiceProxyLoadCallback> loadCallback =
        new (std::nothrow) DataShareManagerImpl::ServiceProxyLoadCallback();

    // let remoteObject nullptr
    sptr<IRemoteObject> remoteObject = nullptr;
    // let systemAbilityId 0
    int32_t systemAbilityId = 0;
    // load success and fail callback
    loadCallback->OnLoadSystemAbilitySuccess(systemAbilityId, remoteObject);
    loadCallback->OnLoadSystemAbilityFail(systemAbilityId);

    // real systemAbilityId is 1301
    systemAbilityId = 1301;
    // // load success and fail callback
    loadCallback->OnLoadSystemAbilitySuccess(systemAbilityId, remoteObject);
    loadCallback->OnLoadSystemAbilityFail(systemAbilityId);

    EXPECT_EQ(systemAbilityId, DISTRIBUTED_KV_DATA_SERVICE_ABILITY_ID);

    // let remoteObject not nullptr
    std::u16string tokenString = u"OHOS.DataShare.IDataShare";
    remoteObject = new (std::nothrow) RemoteObjectTest(tokenString);
    ASSERT_NE(remoteObject, nullptr);
    loadCallback->OnLoadSystemAbilitySuccess(systemAbilityId, remoteObject);

    LOG_INFO("DataShareManagerImplTest ServiceProxyLoadCallback001::End");
}
}
}