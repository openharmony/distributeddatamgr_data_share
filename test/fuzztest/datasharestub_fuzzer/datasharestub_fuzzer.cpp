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
#define LOG_TAG "datasharestub_fuzzer"

#include <fuzzer/FuzzedDataProvider.h>
#include "datasharestub_fuzzer.h"
#include "datashare_itypes_utils.h"
#include "datashare_helper_impl.h"
#include "datashare_log.h"
#include "datashare_observer.h"
#include "datashare_operation_statement.h"
#include "datashare_proxy.h"
#include "datashare_stub_impl.h"
#include "distributeddata_data_share_ipc_interface_code.h"
#include "idatashare.h"
#include "ipc_types.h"
#include "message_parcel.h"
#include "uri.h"

#include <cstddef>
#include <cstdint>

using namespace OHOS::DataShare;
namespace OHOS {
static sptr<DataShareStubImpl> g_dataShareStubImpl = nullptr;

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

char GetLowercaseChar(FuzzedDataProvider &provider)
{
    return provider.ConsumeIntegralInRange<char>('a', 'z');
}

std::string GetFuzzString(FuzzedDataProvider &provider)
{
    std::string str;
    uint8_t len = provider.ConsumeIntegral<uint8_t>();
    str.push_back(GetLowercaseChar(provider));
    for (uint8_t i = 0; i < len; i++) {
        str.push_back(GetLowercaseChar(provider));
    }
    return str;
}

// Uri has to be ascii string and have scheme. Invalid uri will be rejected
// by Uri class
Uri GetUri(FuzzedDataProvider &provider)
{
    std::string uriStr;
    // scheme can't be empty
    uriStr.push_back(GetLowercaseChar(provider));
    uriStr += GetFuzzString(provider);
    // scheme delimetor
    uriStr.push_back(':');
    uriStr.push_back('/');
    uriStr.push_back('/');
    uriStr.push_back('/');
    // path can't be empty
    uriStr.push_back(GetLowercaseChar(provider));
    uriStr += GetFuzzString(provider);
    return Uri(uriStr);
}

DataShareValuesBucket GetValuesBucket(FuzzedDataProvider &provider)
{
    // Mock a simple valuesbucket
    DataShareValueObject::Type intValue = provider.ConsumeIntegral<int64_t>();
    std::map<std::string, DataShareValueObject::Type> values;
    values["int_key"] = intValue;
    DataShareValuesBucket bucket(values);
    return bucket;
}

OperationStatement GetOperationStatement(FuzzedDataProvider &provider)
{
    OperationStatement op;
    int32_t typ = provider.ConsumeIntegralInRange<int32_t>(
        static_cast<int32_t>(OHOS::DataShare::Operation::INSERT),
        static_cast<int32_t>(OHOS::DataShare::Operation::DELETE)
    );
    op.operationType = static_cast<OHOS::DataShare::Operation>(typ);
    Uri uri = GetUri(provider);
    op.uri = uri.ToString();
    op.predicates = DataSharePredicates();
    op.valuesBucket = GetValuesBucket(provider);
    return op;
}

/**
 * @tc.name: OnRemoteRequestFuzz
 * @tc.desc: Fuzz test for OnRemoteRequest interface, verifying stability with random ipc parcel inputs
 * @tc.type: Fuzzing
 * @tc.require: FuzzedDataProvider library and MessageParcel/MessageOption-related headers are included
 * @tc.precon: Empty MessageParcel 'data' and 'reply' are initialized
 * @tc.step:
    1. Generate random code and data
    2. Call OnRemoteRequest with code and data
 * @tc.expect:
    1. No interface crash or custom function execution exception occurs
    2. Interface handles empty message parcels and default option gracefully
 */
void OnRemoteRequestFuzz(FuzzedDataProvider &provider)
{
    LOG_INFO("OnRemoteRequestFuzz start");
    sptr<DataShareStubImpl> dataShareStub = g_dataShareStubImpl;
    if (dataShareStub == nullptr) {
        LOG_ERROR("%{public}s OnRemoteRequestFuzz stub nullptr", FUZZ_PROJECT_NAME);
        return;
    }
    uint32_t codeMin = static_cast<uint32_t>(DistributedShare::DataShare::IDataShareInterfaceCode::CMD_GET_FILE_TYPES);
    uint32_t codeMax = static_cast<uint32_t>(
        DistributedShare::DataShare::IDataShareInterfaceCode::CMD_OPEN_FILE_WITH_ERR_CODE
	);
    uint32_t code = provider.ConsumeIntegralInRange<uint32_t>(codeMin, codeMax);
    MessageParcel data;
    std::vector<uint8_t> fuzzData = provider.ConsumeRemainingBytes<uint8_t>();
    data.WriteInterfaceToken(DataShareProxy::GetDescriptor());
    data.WriteBuffer(static_cast<void *>(fuzzData.data()), fuzzData.size());
    data.RewindRead(0);
    MessageParcel reply;
    MessageOption option;

    dataShareStub->OnRemoteRequest(code, data, reply, option);
    LOG_INFO("OnRemoteRequestFuzz end");
}

/**
 * @tc.name: CmdFileMethodsFuzz
 * @tc.desc: Fuzz test for CmdFileMethods interface, verifying stability with random ipc parcel inputs
 * @tc.type: Fuzzing
 * @tc.require: FuzzedDataProvider library and MessageParcel/MessageOption-related headers are included
 * @tc.precon: Empty MessageParcel 'data' and 'reply' are initialized
 * @tc.step:
    1. Generate random uri and string
    2. Call CmdFileMethods with data and reply
 * @tc.expect:
    1. No interface crash or custom function execution exception occurs
    2. Interface handles empty message parcels and default option gracefully
 */
void CmdFileMethodsFuzz(FuzzedDataProvider &provider)
{
    sptr<DataShareStubImpl> dataShareStub = g_dataShareStubImpl;
    if (dataShareStub == nullptr) {
        LOG_ERROR("%{public}s CmdFileMethodsFuzz stub nullptr", FUZZ_PROJECT_NAME);
        return;
    }
    MessageParcel data;
    // Call 4 file methods
    for (int32_t i = 0; i < 4; i++) {
        OHOS::Uri uri = GetUri(provider);
        std::string str = provider.ConsumeRandomLengthString();
        data.WriteParcelable(&uri);
        data.WriteString(str);
    }

    // Call
    MessageParcel reply;
    LOG_INFO("In CmdFileMethodsFuzz: CmdGetFileTypes");
    dataShareStub->CmdGetFileTypes(data, reply);
    LOG_INFO("In CmdFileMethodsFuzz: CmdOpenFile");
    dataShareStub->CmdOpenFile(data, reply);
    LOG_INFO("In CmdFileMethodsFuzz: CmdOpenRawFile");
    dataShareStub->CmdOpenRawFile(data, reply);
    LOG_INFO("In CmdFileMethodsFuzz: CmdOpenFileWithErrCode");
    dataShareStub->CmdOpenFileWithErrCode(data, reply);
}

/**
 * @tc.name: CmdInsertFuzz
 * @tc.desc: Fuzz test for CmdInsert interface, verifying stability with random ipc parcel inputs
 * @tc.type: Fuzzing
 * @tc.require: FuzzedDataProvider library and MessageParcel/MessageOption-related headers are included
 * @tc.precon: Empty MessageParcel 'data' and 'reply' are initialized
 * @tc.step:
    1. Generate random uri and valuebucket
    2. Call CmdInsert with data and valuebucket
 * @tc.expect:
    1. No interface crash or custom function execution exception occurs
    2. Interface handles empty message parcels and default option gracefully
 */
void CmdInsertFuzz(FuzzedDataProvider &provider)
{
    sptr<DataShareStubImpl> dataShareStub = g_dataShareStubImpl;
    if (dataShareStub == nullptr) {
        LOG_ERROR("%{public}s CmdInsertFuzz stub nullptr", FUZZ_PROJECT_NAME);
        return;
    }
    MessageParcel data;
    // Call 3 methods
    for (int32_t i = 0; i < 3; i++) {
        OHOS::Uri uri = GetUri(provider);
        DataShareValuesBucket bucket = GetValuesBucket(provider);
        ITypesUtil::Marshal(data, uri, bucket);
    }

    // Call
    MessageParcel reply;
    LOG_INFO("In CmdInsertFuzz: CmdInsert");
    dataShareStub->CmdInsert(data, reply);
    LOG_INFO("In CmdInsertFuzz: CmdInsertEx");
    dataShareStub->CmdInsertEx(data, reply);
    LOG_INFO("In CmdInsertFuzz: CmdInsertExt");
    dataShareStub->CmdInsertExt(data, reply);
}

/**
 * @tc.name: CmdUpdateFuzz
 * @tc.desc: Fuzz test for CmdUpdate interface, verifying stability with random ipc parcel inputs
 * @tc.type: Fuzzing
 * @tc.require: FuzzedDataProvider library and MessageParcel/MessageOption-related headers are included
 * @tc.precon: Empty MessageParcel 'data' and 'reply' are initialized
 * @tc.step:
    1. Generate random uri, predicates and valuebucket
    2. Call CmdUpdate with data, predicates and valuebucket
 * @tc.expect:
    1. No interface crash or custom function execution exception occurs
    2. Interface handles empty message parcels and default option gracefully
 */
void CmdUpdateFuzz(FuzzedDataProvider &provider)
{
    sptr<DataShareStubImpl> dataShareStub = g_dataShareStubImpl;
    if (dataShareStub == nullptr) {
        LOG_ERROR("%{public}s CmdUpdateFuzz stub nullptr", FUZZ_PROJECT_NAME);
        return;
    }
    MessageParcel data;
    // Call 2 methods
    for (int32_t i = 0; i < 2; i++) {
        OHOS::Uri uri = GetUri(provider);
        DataSharePredicates pre;
        DataShareValuesBucket bucket = GetValuesBucket(provider);
        ITypesUtil::Marshal(data, uri, pre, bucket);
    }

    // Call
    MessageParcel reply;
    LOG_INFO("In CmdUpdateFuzz: CmdUpdate");
    dataShareStub->CmdUpdate(data, reply);
    LOG_INFO("In CmdUpdateFuzz: CmdUpdateEx");
    dataShareStub->CmdUpdateEx(data, reply);
}

/**
 * @tc.name: CmdBatchUpdateFuzz
 * @tc.desc: Fuzz test for CmdBatchUpdate interface, verifying stability with random ipc parcel inputs
 * @tc.type: Fuzzing
 * @tc.require: FuzzedDataProvider library and MessageParcel/MessageOption-related headers are included
 * @tc.precon: Empty MessageParcel 'data' and 'reply' are initialized
 * @tc.step:
    1. Generate random uri and updateoperations
    2. Call CmdBatchUpdate with data and updateoperations
 * @tc.expect:
    1. No interface crash or custom function execution exception occurs
    2. Interface handles empty message parcels and default option gracefully
 */
void CmdBatchUpdateFuzz(FuzzedDataProvider &provider)
{
    sptr<DataShareStubImpl> dataShareStub = g_dataShareStubImpl;
    if (dataShareStub == nullptr) {
        LOG_ERROR("%{public}s CmdBatchUpdateFuzz stub nullptr", FUZZ_PROJECT_NAME);
        return;
    }
    UpdateOperations ops;
    uint8_t len = provider.ConsumeIntegral<uint8_t>();
    for (uint8_t i = 0; i < len; i++) {
        std::string uri = provider.ConsumeRandomLengthString();
        uint8_t opLen = provider.ConsumeIntegral<uint8_t>();
        std::vector<UpdateOperation> opVec;
        for (uint8_t j = 0; j < opLen; j++) {
            DataSharePredicates pre;
            DataShareValuesBucket bucket = GetValuesBucket(provider);
            UpdateOperation op;
            op.valuesBucket = bucket;
            op.predicates = pre;
            opVec.emplace_back(op);
        }
        ops.emplace(uri, opVec);
    }

    MessageParcel data;
    ITypesUtil::Marshal(data, ops);

    // Call
    MessageParcel reply;
    dataShareStub->CmdBatchUpdate(data, reply);
}

/**
 * @tc.name: CmdDeleteFuzz
 * @tc.desc: Fuzz test for CmdDelete interface, verifying stability with random ipc parcel inputs
 * @tc.type: Fuzzing
 * @tc.require: FuzzedDataProvider library and MessageParcel/MessageOption-related headers are included
 * @tc.precon: Empty MessageParcel 'data' and 'reply' are initialized
 * @tc.step:
    1. Generate random uri and predicates
    2. Call CmdDelete with data and predicates
 * @tc.expect:
    1. No interface crash or custom function execution exception occurs
    2. Interface handles empty message parcels and default option gracefully
 */
void CmdDeleteFuzz(FuzzedDataProvider &provider)
{
    sptr<DataShareStubImpl> dataShareStub = g_dataShareStubImpl;
    if (dataShareStub == nullptr) {
        LOG_ERROR("%{public}s CmdDeleteFuzz stub nullptr", FUZZ_PROJECT_NAME);
        return;
    }
    MessageParcel data;
    // Call 2 methods
    for (int32_t i = 0; i < 2; i++) {
        OHOS::Uri uri = GetUri(provider);
        DataSharePredicates pre;
        ITypesUtil::Marshal(data, uri, pre);
    }

    // Call
    MessageParcel reply;
    LOG_INFO("In CmdDeleteFuzz: CmdDelete");
    dataShareStub->CmdDelete(data, reply);
    LOG_INFO("In CmdDeleteFuzz: CmdDeleteEx");
    dataShareStub->CmdDeleteEx(data, reply);
}

/**
 * @tc.name: CmdQueryFuzz
 * @tc.desc: Fuzz test for CmdQuery interface, verifying stability with random ipc parcel inputs
 * @tc.type: Fuzzing
 * @tc.require: FuzzedDataProvider library and MessageParcel/MessageOption-related headers are included
 * @tc.precon: Empty MessageParcel 'data' and 'reply' are initialized
 * @tc.step:
    1. Generate random uri, colName and predicates
    2. Call CmdQuery with data and predicates
 * @tc.expect:
    1. No interface crash or custom function execution exception occurs
    2. Interface handles empty message parcels and default option gracefully
 */
void CmdQueryFuzz(FuzzedDataProvider &provider)
{
    sptr<DataShareStubImpl> dataShareStub = g_dataShareStubImpl;
    if (dataShareStub == nullptr) {
        LOG_ERROR("%{public}s CmdQueryFuzz stub nullptr", FUZZ_PROJECT_NAME);
        return;
    }
    OHOS::Uri uri = GetUri(provider);
    DataSharePredicates pre;
    uint8_t len = provider.ConsumeIntegral<uint8_t>();
    std::vector<std::string> columns;
    for (uint8_t i = 0; i < len; i++) {
        std::string col = provider.ConsumeRandomLengthString();
        columns.push_back(col);
    }
    
    MessageParcel data;
    ITypesUtil::Marshal(data, uri, columns);
    ITypesUtil::MarshalPredicates(pre, data);

    // Call
    MessageParcel reply;
    dataShareStub->CmdQuery(data, reply);
}

/**
 * @tc.name: CmdGetTypeFuzz
 * @tc.desc: Fuzz test for CmdGetType interface, verifying stability with random ipc parcel inputs
 * @tc.type: Fuzzing
 * @tc.require: FuzzedDataProvider library and MessageParcel/MessageOption-related headers are included
 * @tc.precon: Empty MessageParcel 'data' and 'reply' are initialized
 * @tc.step:
    1. Generate random uri
    2. Call CmdGetType with uri
 * @tc.expect:
    1. No interface crash or custom function execution exception occurs
    2. Interface handles empty message parcels and default option gracefully
 */
void CmdGetTypeFuzz(FuzzedDataProvider &provider)
{
    sptr<DataShareStubImpl> dataShareStub = g_dataShareStubImpl;
    if (dataShareStub == nullptr) {
        LOG_ERROR("%{public}s CmdGetTypeFuzz stub nullptr", FUZZ_PROJECT_NAME);
        return;
    }
    OHOS::Uri uri = GetUri(provider);

    MessageParcel data;
    ITypesUtil::Marshal(data, uri);

    // Call
    MessageParcel reply;
    dataShareStub->CmdGetType(data, reply);
}

/**
 * @tc.name: CmdUserDefinedFuncFuzz
 * @tc.desc: Fuzz test for CmdBatchUpdate interface, verifying stability with random ipc parcel inputs
 * @tc.type: Fuzzing
 * @tc.require: FuzzedDataProvider library and MessageParcel/MessageOption-related headers are included
 * @tc.precon: Empty MessageParcel 'data' and 'reply' are initialized
 * @tc.step:
    1. Generate random data
    2. Call CmdUserDefinedFunc with random data
 * @tc.expect:
    1. No interface crash or custom function execution exception occurs
    2. Interface handles empty message parcels and default option gracefully
 */
void CmdUserDefinedFuncFuzz(FuzzedDataProvider &provider)
{
    sptr<DataShareStubImpl> dataShareStub = g_dataShareStubImpl;
    if (dataShareStub == nullptr) {
        LOG_ERROR("%{public}s CmdUserDefinedFuncFuzz stub nullptr", FUZZ_PROJECT_NAME);
        return;
    }
    std::vector<uint8_t> fuzzData = provider.ConsumeRemainingBytes<uint8_t>();
    
    MessageParcel data;
    data.WriteBuffer(static_cast<void *>(fuzzData.data()), fuzzData.size());
    data.RewindRead(0);

    // Call
    MessageParcel reply;
    MessageOption option;
    dataShareStub->CmdUserDefineFunc(data, reply, option);
}

/**
 * @tc.name: CmdBatchInsertFuzz
 * @tc.desc: Fuzz test for CmdBatchInsert interface, verifying stability with random ipc parcel inputs
 * @tc.type: Fuzzing
 * @tc.require: FuzzedDataProvider library and MessageParcel/MessageOption-related headers are included
 * @tc.precon: Empty MessageParcel 'data' and 'reply' are initialized
 * @tc.step:
    1. Generate random uri and value buckets
    2. Call CmdBatchInsert with data and value buckets
 * @tc.expect:
    1. No interface crash or custom function execution exception occurs
    2. Interface handles empty message parcels and default option gracefully
 */
void CmdBatchInsertFuzz(FuzzedDataProvider &provider)
{
    sptr<DataShareStubImpl> dataShareStub = g_dataShareStubImpl;
    if (dataShareStub == nullptr) {
        LOG_ERROR("%{public}s CmdBatchInsertFuzz stub nullptr", FUZZ_PROJECT_NAME);
        return;
    }
    Uri uri = GetUri(provider);
    std::vector<DataShareValuesBucket> vbs;
    uint8_t len = provider.ConsumeIntegral<uint8_t>();
    for (uint8_t i = 0; i < len; i++) {
        DataShareValuesBucket bucket = GetValuesBucket(provider);
        vbs.push_back(bucket);
    }

    MessageParcel data;
    ITypesUtil::Marshal(data, uri);
    ITypesUtil::MarshalValuesBucketVec(vbs, data);

    // Call
    MessageParcel reply;
    dataShareStub->CmdBatchInsert(data, reply);
}

/**
 * @tc.name: CmdRegisterObserverFuzz
 * @tc.desc: Fuzz test for CmdRegisterObserver and CmdUnregisterObserver interface, verifying stability with random
 *  ipc parcel inputs
 * @tc.type: Fuzzing
 * @tc.require: FuzzedDataProvider library and MessageParcel/MessageOption-related headers are included
 * @tc.precon: Empty MessageParcel 'data' and 'reply' are initialized
 * @tc.step:
    1. Generate random uri and a mock observer
    2. Call CmdRegisterObserver and CmdUnregisterObserver with uri and observer
 * @tc.expect:
    1. No interface crash or custom function execution exception occurs
    2. Interface handles empty message parcels and default option gracefully
 */
void CmdRegisterObserverFuzz(FuzzedDataProvider &provider)
{
    sptr<DataShareStubImpl> dataShareStub = g_dataShareStubImpl;
    if (dataShareStub == nullptr) {
        LOG_ERROR("%{public}s CmdRegisterObserverFuzz stub nullptr", FUZZ_PROJECT_NAME);
        return;
    }
    Uri uri = GetUri(provider);
    std::string uriStr = uri.ToString();
    std::shared_ptr<DataShareObserver> dataObserver = std::make_shared<DataShareObserverTest>(uriStr);
    sptr<ObserverImpl> obs = ObserverImpl::GetObserver(uri, dataObserver);

    MessageParcel data;
	// Call 2 methods
    for (int32_t i = 0; i < 2; i++) {
        ITypesUtil::Marshal(data, uri, obs->AsObject());
    }

    // Call
    MessageParcel reply;
    LOG_INFO("In CmdRegisterObserverFuzz: CmdRegisterObserver");
    dataShareStub->CmdRegisterObserver(data, reply);
    LOG_INFO("In CmdRegisterObserverFuzz: CmdUnregisterObserver");
    dataShareStub->CmdUnregisterObserver(data, reply);
    ObserverImpl::DeleteObserver(uri, dataObserver);
}

/**
 * @tc.name: CmdNotifyChangeFuzz
 * @tc.desc: Fuzz test for CmdNotifyChange interface, verifying stability with random ipc parcel inputs
 * @tc.type: Fuzzing
 * @tc.require: FuzzedDataProvider library and MessageParcel/MessageOption-related headers are included
 * @tc.precon: Empty MessageParcel 'data' and 'reply' are initialized
 * @tc.step:
    1. Generate random uri
    2. Call CmdNotifyChange with uri
 * @tc.expect:
    1. No interface crash or custom function execution exception occurs
    2. Interface handles empty message parcels and default option gracefully
 */
void CmdNotifyChangeFuzz(FuzzedDataProvider &provider)
{
    sptr<DataShareStubImpl> dataShareStub = g_dataShareStubImpl;
    if (dataShareStub == nullptr) {
        LOG_ERROR("%{public}s CmdNotifyChangeFuzz stub nullptr", FUZZ_PROJECT_NAME);
        return;
    }
    OHOS::Uri uri = GetUri(provider);

    MessageParcel data;
    ITypesUtil::Marshal(data, uri);

    // Call
    MessageParcel reply;
    dataShareStub->CmdNotifyChange(data, reply);
}

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
    sptr<DataShareStubImpl> dataShareStub = g_dataShareStubImpl;
    if (dataShareStub == nullptr) {
        LOG_ERROR("%{public}s CmdNotifyChangeExtProviderFuzz stub nullptr", FUZZ_PROJECT_NAME);
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
        uris.push_back(GetUri(provider));
    }
    info.uris_ = uris;
    info.size_ = provider.ConsumeIntegral<uint32_t>();

    MessageParcel data;
    ChangeInfo::Marshalling(info, data);

    // Call
    MessageParcel reply;
    dataShareStub->CmdNotifyChangeExtProvider(data, reply);
}

/**
 * @tc.name: CmdUnregisterObserverExtProviderFuzz
 * @tc.desc: Fuzz test for CmdUnregisterObserverExtProvider interface, verifying stability with random ipc parcel
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
    sptr<DataShareStubImpl> dataShareStub = g_dataShareStubImpl;
    if (dataShareStub == nullptr) {
        LOG_ERROR("%{public}s CmdUnregisterObserverExtProviderFuzz stub nullptr", FUZZ_PROJECT_NAME);
        return;
    }
    OHOS::Uri uri = GetUri(provider);
    std::string uriStr = uri.ToString();
    std::shared_ptr<DataShareObserver> dataObserver = std::make_shared<DataShareObserverTest>(uriStr);
    sptr<ObserverImpl> obs = ObserverImpl::GetObserver(uri, dataObserver);

    MessageParcel data;
    ITypesUtil::Marshal(data, uri, obs->AsObject());

    // Call
    MessageParcel reply;
    dataShareStub->CmdUnregisterObserverExtProvider(data, reply);
    ObserverImpl::DeleteObserver(uri, dataObserver);
}

/**
 * @tc.name: CmdRegisterObserverExtProviderFuzz
 * @tc.desc: Fuzz test for CmdRegisterObserverExtProvider interface, verifying stability with random ipc parcel
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
    sptr<DataShareStubImpl> dataShareStub = g_dataShareStubImpl;
    if (dataShareStub == nullptr) {
        LOG_ERROR("%{public}s CmdRegisterObserverExtProviderFuzz stub nullptr", FUZZ_PROJECT_NAME);
        return;
    }
    OHOS::Uri uri = GetUri(provider);
    std::string uriStr = uri.ToString();
    std::shared_ptr<DataShareObserver> dataObserver = std::make_shared<DataShareObserverTest>(uriStr);
    sptr<ObserverImpl> obs = ObserverImpl::GetObserver(uri, dataObserver);
    bool isDescendants = provider.ConsumeBool();
    RegisterOption op;
    op.isReconnect = provider.ConsumeBool();

    MessageParcel data;
    ITypesUtil::Marshal(data, uri, obs->AsObject(), isDescendants, op);

    // Call
    MessageParcel reply;
    dataShareStub->CmdRegisterObserverExtProvider(data, reply);
    ObserverImpl::DeleteObserver(uri, dataObserver);
}

/**
 * @tc.name: CmdNormalizeUriFuzz
 * @tc.desc: Fuzz test for CmdNormalizeUri interface, verifying stability with random ipc parcel inputs
 * @tc.type: Fuzzing
 * @tc.require: FuzzedDataProvider library and MessageParcel/MessageOption-related headers are included
 * @tc.precon: Empty MessageParcel 'data' and 'reply' are initialized
 * @tc.step:
    1. Generate random uri
    2. Call CmdNormalizeUri with uri
 * @tc.expect:
    1. No interface crash or custom function execution exception occurs
    2. Interface handles empty message parcels and default option gracefully
 */
void CmdNormalizeUriFuzz(FuzzedDataProvider &provider)
{
    sptr<DataShareStubImpl> dataShareStub = g_dataShareStubImpl;
    if (dataShareStub == nullptr) {
        LOG_ERROR("%{public}s CmdNormalizeUriFuzz stub nullptr", FUZZ_PROJECT_NAME);
        return;
    }
    OHOS::Uri uri = GetUri(provider);

    MessageParcel data;
    // Call 2 methods
    for (int32_t i = 0; i < 2; i++) {
        ITypesUtil::Marshal(data, uri);
    }

    // Call
    MessageParcel reply;
    dataShareStub->CmdNormalizeUri(data, reply);
    dataShareStub->CmdDenormalizeUri(data, reply);
}


/**
 * @tc.name: CmdExecuteBatchFuzz
 * @tc.desc: Fuzz test for CmdExecuteBatch interface, verifying stability with random ipc parcel inputs
 * @tc.type: Fuzzing
 * @tc.require: FuzzedDataProvider library and MessageParcel/MessageOption-related headers are included
 * @tc.precon: Empty MessageParcel 'data' and 'reply' are initialized
 * @tc.step:
    1. Generate random uri
    2. Call CmdExecuteBatch with uri
 * @tc.expect:
    1. No interface crash or custom function execution exception occurs
    2. Interface handles empty message parcels and default option gracefully
 */
void CmdExecuteBatchFuzz(FuzzedDataProvider &provider)
{
    sptr<DataShareStubImpl> dataShareStub = g_dataShareStubImpl;
    if (dataShareStub == nullptr) {
        LOG_ERROR("%{public}s CmdExecuteBatchFuzz stub nullptr", FUZZ_PROJECT_NAME);
        return;
    }
    std::vector<OperationStatement> statements;
    uint8_t len = provider.ConsumeIntegral<uint8_t>();
    for (uint8_t i = 0; i < len; i++) {
        statements.push_back(GetOperationStatement(provider));
    }

    MessageParcel data;
    ITypesUtil::MarshalOperationStatementVec(statements, data);

    // Call
    MessageParcel reply;
    dataShareStub->CmdExecuteBatch(data, reply);
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerInitialize(int *argc, char ***argv)
{
    LOG_INFO("%{public}s LLVMFuzzerInitialize start", FUZZ_PROJECT_NAME);
    OHOS::g_dataShareStubImpl = OHOS::sptr<DataShareStubImpl>(new (std::nothrow) DataShareStubImpl(nullptr));
    if (OHOS::g_dataShareStubImpl == nullptr) {
        LOG_ERROR("%{public}s LLVMFuzzerInitialize err, g_dataShareStubImpl nullptr", FUZZ_PROJECT_NAME);
        return -1;
    }
    LOG_INFO("%{public}s LLVMFuzzerInitialize end", FUZZ_PROJECT_NAME);
    return 0;
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    LOG_INFO("%{public}s LLVMFuzzerTestOneInput start", FUZZ_PROJECT_NAME);
    FuzzedDataProvider provider(data, size);
    LOG_INFO("OnRemoteRequestFuzz start");
    OHOS::OnRemoteRequestFuzz(provider);
    LOG_INFO("CmdFileMethodsFuzz start");
    OHOS::CmdFileMethodsFuzz(provider);
    LOG_INFO("CmdInsertFuzz start");
    OHOS::CmdInsertFuzz(provider);
    LOG_INFO("CmdUpdateFuzz start");
    OHOS::CmdUpdateFuzz(provider);
    LOG_INFO("CmdBatchUpdateFuzz start");
    OHOS::CmdBatchUpdateFuzz(provider);
    LOG_INFO("CmdDeleteFuzz start");
    OHOS::CmdDeleteFuzz(provider);
    LOG_INFO("CmdQueryFuzz start");
    OHOS::CmdQueryFuzz(provider);
    LOG_INFO("CmdGetTypeFuzz start");
    OHOS::CmdGetTypeFuzz(provider);
    LOG_INFO("CmdUserDefinedFuncFuzz start");
    OHOS::CmdUserDefinedFuncFuzz(provider);
    LOG_INFO("CmdBatchInsertFuzz start");
    OHOS::CmdBatchInsertFuzz(provider);
    LOG_INFO("CmdRegisterObserverFuzz start");
    OHOS::CmdRegisterObserverFuzz(provider);
    LOG_INFO("CmdNotifyChangeFuzz start");
    OHOS::CmdNotifyChangeFuzz(provider);
    LOG_INFO("CmdNotifyChangeExtProviderFuzz start");
    OHOS::CmdNotifyChangeExtProviderFuzz(provider);
    LOG_INFO("CmdUnregisterObserverExtProviderFuzz start");
    OHOS::CmdUnregisterObserverExtProviderFuzz(provider);
    LOG_INFO("CmdRegisterObserverExtProviderFuzz start");
    OHOS::CmdRegisterObserverExtProviderFuzz(provider);
    LOG_INFO("CmdNormalizeUriFuzz start");
    OHOS::CmdNormalizeUriFuzz(provider);
    LOG_INFO("CmdExecuteBatchFuzz start");
    OHOS::CmdExecuteBatchFuzz(provider);
    LOG_INFO("%{public}s LLVMFuzzerTestOneInput end", FUZZ_PROJECT_NAME);
    return 0;
}
