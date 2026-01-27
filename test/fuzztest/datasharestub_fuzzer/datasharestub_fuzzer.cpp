/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include <fuzzer/FuzzedDataProvider.h>
#include "datasharestub_fuzzer.h"
#include "datashare_itypes_utils.h"
#include "datashare_helper_impl.h"
#include "datashare_log.h"
#include "datashare_observer.h"
#include "datashare_operation_statement.h"
#include "datashare_proxy.h"
#include "datashare_stub_impl.h"
#include "idatashare.h"
#include "ipc_types.h"
#include "message_parcel.h"
#include "uri.h"

#include <cstddef>
#include <cstdint>

using namespace OHOS::DataShare;
namespace OHOS {
static std::shared_ptr<DataShareStubImpl> g_dataShareStubImpl = nullptr;

template <typename T>
class ConditionLock {
public:
    explicit ConditionLock() {}
    ~ConditionLock() {}
public:
    void Notify(const T &data)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        data_ = data;
        isSet_ = true;
        cv_.notify_one();
    }
    
    T Wait()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait_for(lock, std::chrono::seconds(interval), [this]() { return isSet_; });
        T data = data_;
        cv_.notify_one();
        return data;
    }
    
    void Clear()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        isSet_ = false;
        cv_.notify_one();
    }

private:
    bool isSet_ = false;
    T data_;
    std::mutex mutex_;
    std::condition_variable cv_;
    static constexpr int64_t interval = 2;
};

class DataShareObserverTest : public DataShare::DataShareObserver {
public:
    explicit DataShareObserverTest(std::string uri)
    {
        uri_ = uri;
    }

    ~DataShareObserverTest() {}
    
    void OnChange(const ChangeInfo &changeInfo) override
    {
        changeInfo_ = changeInfo;
        data.Notify(changeInfo);
    }
    
    void Clear()
    {
        changeInfo_.changeType_ = INVAILD;
        changeInfo_.uris_.clear();
        changeInfo_.data_ = nullptr;
        changeInfo_.size_ = 0;
        changeInfo_.valueBuckets_ = {};
        data.Clear();
    }
    
    ChangeInfo changeInfo_;
    ConditionLock<ChangeInfo> data;
    std::string uri_;
};

/**
 * @tc.name: CmdNotifyChangeExtProviderFuzz
 * @tc.desc: Fuzz test for CmdNotifyChangeExtProvider interface, verifying stability with random ipc parcel inputs
 * @tc.type: Fuzzing
 * @tc.require: FuzzedDataProvider library and MessageParcel/MessageOption-related headers are included
 * @tc.precon: Empty MessageParcel 'data' and 'reply' are initialized
 * @tc.step:
    1. Generate random ChangeInfo
    2. Call CmdNotifyChangeExtProvider with data and reply
 * @tc.expect:
    1. No interface crash or custom function execution exception occurs
    2. Interface handles empty message parcels and default option gracefully
 */
void CmdNotifyChangeExtProviderFuzz(FuzzedDataProvider &provider)
{
    std::shared_ptr<DataShareStubImpl> dataShareStub = g_dataShareStubImpl;
    if (dataShareStub == nullptr) {
        LOG_ERROR("%s CmdNotifyChangeExtProviderFuzz stub nullptr", FUZZ_PROJECT_NAME);
        return;
    }
    ChangeInfo info;
    uint32_t typ = provider.ConsumeIntegralInRange<uint32_t>(
        OHOS::AAFwk::ChangeInfo::ChangeType::INSERT,
        OHOS::AAFwk::ChangeInfo::ChangeType::INVAILD
    );
    info.changeType_ = static_cast<OHOS::AAFwk::ChangeInfo::ChangeType>(typ);
    std::list<Uri> uris;
    uint8_t len = provider.ConsumeIntegral<uint8_t>();
    for (uint8_t i = 0; i < len; i++) {
        std::string uri = provider.ConsumeRandomLengthString();
        uris.push_back(OHOS::Uri(uri));
    }
    info.uris_ = uris;
    info.size_ = provider.ConsumeIntegral<uint32_t>();

    MessageParcel data;
    data.WriteInterfaceToken(DataShareProxy::GetDescriptor());
    ChangeInfo::Marshalling(info, data);

    // Call
    MessageParcel reply;
    dataShareStub->CmdNotifyChangeExtProvider(data, reply);
}

/**
 * @tc.name: CmdUnregisterObserverExtProviderFuzz
 * @tc.desc: Fuzz test for CmdUnregisterObserverExtProviderFuzz interface, verifying stability with random ipc parcel
 *  inputs
 * @tc.type: Fuzzing
 * @tc.require: FuzzedDataProvider library and MessageParcel/MessageOption-related headers are included
 * @tc.precon: Empty MessageParcel 'data' and 'reply' are initialized
 * @tc.step:
    1. Generate random uri and a mock observer
    2. Call CmdUnregisterObserverExtProvider with data and reply
 * @tc.expect:
    1. No interface crash or custom function execution exception occurs
    2. Interface handles empty message parcels and default option gracefully
 */
void CmdUnregisterObserverExtProviderFuzz(FuzzedDataProvider &provider)
{
    std::shared_ptr<DataShareStubImpl> dataShareStub = g_dataShareStubImpl;
    if (dataShareStub == nullptr) {
        LOG_ERROR("%s CmdUnregisterObserverExtProviderFuzz stub nullptr", FUZZ_PROJECT_NAME);
        return;
    }
    std::string uriStr = provider.ConsumeRandomLengthString();
    OHOS::Uri uri = Uri(uriStr);
    std::shared_ptr<DataShareObserver> dataObserver = std::make_shared<DataShareObserverTest>(uriStr);
    sptr<ObserverImpl> obs = ObserverImpl::GetObserver(uri, dataObserver);

    MessageParcel data;
    data.WriteInterfaceToken(DataShareProxy::GetDescriptor());
    ITypesUtil::Marshal(data, uri, obs->AsObject());

    // Call
    MessageParcel reply;
    dataShareStub->CmdUnregisterObserverExtProvider(data, reply);
}

/**
 * @tc.name: CmdRegisterObserverExtProviderFuzz
 * @tc.desc: Fuzz test for CmdRegisterObserverExtProviderFuzz interface, verifying stability with random ipc parcel
 *  inputs
 * @tc.type: Fuzzing
 * @tc.require: FuzzedDataProvider library and MessageParcel/MessageOption-related headers are included
 * @tc.precon: Empty MessageParcel 'data' and 'reply' are initialized
 * @tc.step:
    1. Generate random uri, a mock observer, and options for this function
    2. Call CmdRegisterObserverExtProvider with data and reply
 * @tc.expect:
    1. No interface crash or custom function execution exception occurs
    2. Interface handles empty message parcels and default option gracefully
 */
void CmdRegisterObserverExtProviderFuzz(FuzzedDataProvider &provider)
{
    std::shared_ptr<DataShareStubImpl> dataShareStub = g_dataShareStubImpl;
    if (dataShareStub == nullptr) {
        LOG_ERROR("%s CmdRegisterObserverExtProviderFuzz stub nullptr", FUZZ_PROJECT_NAME);
        return;
    }
    std::string uriStr = provider.ConsumeRandomLengthString();
    OHOS::Uri uri = Uri(uriStr);
    std::shared_ptr<DataShareObserver> dataObserver = std::make_shared<DataShareObserverTest>(uriStr);
    sptr<ObserverImpl> obs = ObserverImpl::GetObserver(uri, dataObserver);
    bool isDescendants = provider.ConsumeBool();
    RegisterOption op;
    op.isReconnect = provider.ConsumeBool();

    MessageParcel data;
    data.WriteInterfaceToken(DataShareProxy::GetDescriptor());
    ITypesUtil::Marshal(data, uri, obs->AsObject(), isDescendants, op);

    // Call
    MessageParcel reply;
    dataShareStub->CmdRegisterObserverExtProvider(data, reply);
}

/**
 * @tc.name: CmdOpenFileWithErrCodeFuzz
 * @tc.desc: Fuzz test for CmdOpenFileWithErrCodeFuzz interface, verifying stability with random ipc parcel inputs
 * @tc.type: Fuzzing
 * @tc.require: FuzzedDataProvider library and MessageParcel/MessageOption-related headers are included
 * @tc.precon: Empty MessageParcel 'data' and 'reply' are initialized
 * @tc.step:
    1. Generate random uri and mode string
    2. Call CmdOpenFileWithErrCode with data and reply
 * @tc.expect:
    1. No interface crash or custom function execution exception occurs
    2. Interface handles empty message parcels and default option gracefully
 */
void CmdOpenFileWithErrCodeFuzz(FuzzedDataProvider &provider)
{
    std::shared_ptr<DataShareStubImpl> dataShareStub = g_dataShareStubImpl;
    if (dataShareStub == nullptr) {
        LOG_ERROR("%s CmdOpenFileWithErrCodeFuzz stub nullptr", FUZZ_PROJECT_NAME);
        return;
    }
    std::string uriStr = provider.ConsumeRandomLengthString();
    OHOS::Uri uri = Uri(uriStr);
    std::string mode = provider.ConsumeRandomLengthString();

    MessageParcel data;
    data.WriteInterfaceToken(DataShareProxy::GetDescriptor());
    data.WriteParcelable(&uri);
    data.WriteString(mode);

    // Call
    MessageParcel reply;
    dataShareStub->CmdOpenFileWithErrCode(data, reply);
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerInitialize(int *argc, char ***argv)
{
    LOG_INFO("%s LLVMFuzzerInitialize start", FUZZ_PROJECT_NAME);
    OHOS::g_dataShareStubImpl = std::make_shared<DataShareStubImpl>(nullptr);
    if (OHOS::g_dataShareStubImpl == nullptr) {
        LOG_ERROR("%s LLVMFuzzerInitialize err, g_dataShareStubImpl nullptr", FUZZ_PROJECT_NAME);
        return -1;
    }
    LOG_INFO("%s LLVMFuzzerInitialize end", FUZZ_PROJECT_NAME);
    return 0;
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    LOG_INFO("%s LLVMFuzzerTestOneInput start", FUZZ_PROJECT_NAME);
    FuzzedDataProvider provider(data, size);
    OHOS::CmdNotifyChangeExtProviderFuzz(provider);
    OHOS::CmdUnregisterObserverExtProviderFuzz(provider);
    OHOS::CmdRegisterObserverExtProviderFuzz(provider);
    OHOS::CmdOpenFileWithErrCodeFuzz(provider);
    LOG_INFO("%s LLVMFuzzerTestOneInput end", FUZZ_PROJECT_NAME);
    return 0;
}