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
#include "ikvstore_data_service.h"
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
 * @tc.desc: Test the normal functionality of DataShareManagerImpl::ServiceProxyLoadCallback, including
 *           OnLoadSystemAbilitySuccess and OnLoadSystemAbilityFail under different parameter scenarios
 *           (remoteObject as null/non-null, different systemAbilityId values).
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. The test environment supports instantiation of DataShareManagerImpl::ServiceProxyLoadCallback and
       RemoteObjectTest (with std::u16string token) without initialization errors.
    2. The predefined constant DISTRIBUTED_KV_DATA_SERVICE_ABILITY_ID is valid and equals 1301.
    3. IRemoteObject pointers can be set to nullptr or assigned valid RemoteObjectTest instances.
    4. The ServiceProxyLoadCallback’s OnLoad methods accept int32_t systemAbilityId and IRemoteObject* as parameters.
 * @tc.step:
    1. Create an sptr of DataShareManagerImpl::ServiceProxyLoadCallback (loadCallback) using new (std::nothrow).
    2. Set IRemoteObject* remoteObject to nullptr and int32_t systemAbilityId to 0.
    3. Call loadCallback->OnLoadSystemAbilitySuccess(systemAbilityId, remoteObject) and
       loadCallback->OnLoadSystemAbilityFail(systemAbilityId).
    4. Set systemAbilityId to DISTRIBUTED_KV_DATA_SERVICE_ABILITY_ID (1301), then call the two OnLoad methods again
       with remoteObject still as nullptr.
    5. Verify that systemAbilityId matches DISTRIBUTED_KV_DATA_SERVICE_ABILITY_ID.
    6. Create a RemoteObjectTest instance with token u"OHOS.DataShare.IDataShare", assign it to remoteObject,
       then call OnLoadSystemAbilitySuccess(systemAbilityId, remoteObject).
 * @tc.expect:
    1. All calls to OnLoadSystemAbilitySuccess and OnLoadSystemAbilityFail execute without exceptions.
    2. The systemAbilityId (1301) matches the predefined DISTRIBUTED_KV_DATA_SERVICE_ABILITY_ID.
    3. The valid RemoteObjectTest instance is successfully passed to OnLoadSystemAbilitySuccess without errors.
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

/**
 * @tc.name: GetDataShareServiceProxy001
 * @tc.desc: Test the normal functionality of DataShareManagerImpl::GetDataShareServiceProxy, verifying its return
 *           value when the dataMgrService_ member is null and non-null.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. DataShareManagerImpl can be obtained via GetInstance() and returns a non-null pointer.
    2. DataShareManagerImpl::GetDistributedDataManager() returns a valid (non-null) dataMgrService_ instance.
    3. The GetDataShareServiceProxy method returns a non-null proxy pointer under valid runtime conditions.
    4. The dataMgrService_ member of DataShareManagerImpl can be explicitly set to nullptr or a valid instance.
 * @tc.step:
    1. Call DataShareManagerImpl::GetInstance() to get a manager instance, verify it is non-null.
    2. Set manager->dataMgrService_ to nullptr.
    3. Call manager->GetDataShareServiceProxy() and store the returned proxy, verify the proxy is non-null.
    4. Assign manager->dataMgrService_ to the result of DataShareManagerImpl::GetDistributedDataManager(),
       verify dataMgrService_ is non-null.
    5. Call manager->GetDataShareServiceProxy() again, store the new proxy, verify it is non-null.
 * @tc.expect:
    1. The DataShareManagerImpl instance obtained via GetInstance() is non-null.
    2. When dataMgrService_ is null, GetDataShareServiceProxy returns a non-null proxy.
    3. When dataMgrService_ is non-null, GetDataShareServiceProxy returns a non-null proxy.
 */
HWTEST_F(DataShareManagerImplTest, GetDataShareServiceProxy001, TestSize.Level0)
{
    LOG_INFO("DataShareManagerImplTest GetDataShareServiceProxy001::Start");

    auto manager = DataShareManagerImpl::GetInstance();
    ASSERT_NE(manager, nullptr);
    // manager->dataMgrService_ is nullptr
    manager->dataMgrService_ = nullptr;
    auto proxy = manager->GetDataShareServiceProxy();
    ASSERT_NE(proxy, nullptr);

    // manager->dataMgrService_ is not nullptr
    manager->dataMgrService_ = DataShareManagerImpl::GetDistributedDataManager();
    ASSERT_NE(manager->dataMgrService_, nullptr);
    proxy = manager->GetDataShareServiceProxy();
    ASSERT_NE(proxy, nullptr);

    LOG_INFO("DataShareManagerImplTest GetDataShareServiceProxy001::End");
}

/**
 * @tc.name: GetProxy001
 * @tc.desc: Test the normal functionality of DataShareManagerImpl::GetProxy, verifying its return value when the
 *           dataMgrService_ member is null and non-null.
 * @tc.type: FUNC
 * @tc.require: None
 * @tc.precon:
    1. DataShareManagerImpl can be obtained via GetInstance() and returns a non-null pointer.
    2. DataShareManagerImpl::GetDistributedDataManager() returns a valid (non-null) dataMgrService_ instance.
    3. The GetProxy method returns a non-null proxy pointer under valid runtime conditions.
    4. The dataMgrService_ member of DataShareManagerImpl can be explicitly set to nullptr or a valid instance.
 * @tc.step:
    1. Call DataShareManagerImpl::GetInstance() to get a manager instance, verify it is non-null.
    2. Set manager->dataMgrService_ to nullptr.
    3. Call manager->GetProxy() and store the returned proxy, verify the proxy is non-null.
    4. Assign manager->dataMgrService_ to the result of DataShareManagerImpl::GetDistributedDataManager(),
       verify dataMgrService_ is non-null.
    5. Call manager->GetProxy() again, store the new proxy, verify it is non-null.
 * @tc.expect:
    1. The DataShareManagerImpl instance obtained via GetInstance() is non-null.
    2. When dataMgrService_ is null, GetProxy returns a non-null proxy.
    3. When dataMgrService_ is non-null, GetProxy returns a non-null proxy.
 */
HWTEST_F(DataShareManagerImplTest, GetProxy001, TestSize.Level0)
{
    LOG_INFO("DataShareManagerImplTest GetProxy001::Start");

    auto manager = DataShareManagerImpl::GetInstance();
    ASSERT_NE(manager, nullptr);
    // manager->dataMgrService_ is nullptr
    manager->dataMgrService_ = nullptr;
    auto proxy = manager->GetProxy();
    ASSERT_NE(proxy, nullptr);

    // manager->dataMgrService_ is not nullptr
    manager->dataMgrService_ = DataShareManagerImpl::GetDistributedDataManager();
    ASSERT_NE(manager->dataMgrService_, nullptr);
    proxy = manager->GetProxy();
    ASSERT_NE(proxy, nullptr);

    LOG_INFO("DataShareManagerImplTest GetProxy001::End");
}
}
}