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
#include "datasharehelp_fuzzer.h"
#include "datashare_helper.h"
#include "datashare_helper_impl.h"
#include "datashare_observer.h"

#include <cstddef>
#include <cstdint>

using namespace OHOS::DataShare;
namespace OHOS {

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
 * @tc.name: CreateFuzz
 * @tc.desc: Fuzz test for DataShareHelper::Create interface, verifying stability under random inputs (URI, wait time)
 * @tc.type: Fuzzing
 * @tc.require: FuzzedDataProvider library and DataShare-related headers are included
 * @tc.precon: DataShare service is normally deployed in the test environment
 * @tc.step:
    1. Generate random-length string strUri as resource URI via FuzzedDataProvider
    2. Generate random-length string extUri as extended URI via FuzzedDataProvider
    3. Generate random integer waitTime as waiting time via FuzzedDataProvider
    4. Call DataShareHelper::Create interface with the above random parameters
 * @tc.expect:
    1. No interface crash or memory leak occurs
    2. Interface return results comply with preset error handling logic
 */
void CreateFuzz(FuzzedDataProvider &provider)
{
    sptr<IRemoteObject> token = nullptr;
    std::string strUri =  provider.ConsumeRandomLengthString();
    std::string extUri =  provider.ConsumeRandomLengthString();
    int waitTime = provider.ConsumeIntegral<int>();
    DataShareHelper::Create(token, strUri, extUri, waitTime);
}

/**
 * @tc.name: GetFileTypesFuzz
 * @tc.desc: Fuzz test for GetFileTypes interface, verifying stability under random URI and MIME type filter
 * @tc.type: Fuzzing
 * @tc.require: FuzzedDataProvider library, Uri and DataShareHelperImpl-related headers are included
 * @tc.precon: DataShareHelperImpl instance is initialized
 * @tc.step:
    1. Generate random boolean isSystem via FuzzedDataProvider, initialize DataShareHelperImpl instance
    2. Generate random-length string uriString via FuzzedDataProvider, convert to Uri object
    3. Generate random-length string mimeTypeFilter as MIME filter condition via FuzzedDataProvider
    4. Call dataShareHelperImpl.GetFileTypes interface with Uri and mimeTypeFilter
 * @tc.expect:
    1. No interface crash or memory access error occurs
    2. Interface has normal fault tolerance for invalid Uri and empty MIME filter
 */
void GetFileTypesFuzz(FuzzedDataProvider &provider)
{
    bool isSystem = provider.ConsumeBool();
    DataShareHelperImpl dataShareHelperImpl("datashare://datasharehelperimpl", isSystem);
    std::string uriString = provider.ConsumeRandomLengthString();
    Uri uri(uriString);
    std::string mimeTypeFilter =  provider.ConsumeRandomLengthString();
    dataShareHelperImpl.GetFileTypes(uri, mimeTypeFilter);
}

/**
 * @tc.name: OpenFileFuzz
 * @tc.desc: Fuzz test for DataShareHelperImpl::OpenFile interface, verifying stability under random URI and open mode
 * @tc.type: Fuzzing
 * @tc.require: FuzzedDataProvider library, Uri and DataShareHelperImpl-related headers are included
 * @tc.precon: DataShareHelperImpl instance is initialized
 * @tc.step:
    1. Generate random boolean isSystem via FuzzedDataProvider, initialize DataShareHelperImpl instance
    2. Generate random-length string uriString via FuzzedDataProvider, convert to Uri object
    3. Generate random-length string mode as file open mode (e.g., "r"/"w") via FuzzedDataProvider
    4. Call dataShareHelperImpl.OpenFile interface with Uri and mode
 * @tc.expect:
    1. No interface crash or file handle leak occurs
    2. Interface has normal fault tolerance for invalid Uri and invalid open mode
 */
void OpenFileFuzz(FuzzedDataProvider &provider)
{
    bool isSystem = provider.ConsumeBool();
    DataShareHelperImpl dataShareHelperImpl("datashare://datasharehelperimpl", isSystem);
    std::string uriString = provider.ConsumeRandomLengthString();
    Uri uri(uriString);
    std::string mode =  provider.ConsumeRandomLengthString();
    dataShareHelperImpl.OpenFile(uri, mode);
}

/**
 * @tc.name: OpenRawFileFuzz
 * @tc.desc: Fuzz test for OpenRawFile interface, verifying stability under random URI and open mode
 * @tc.type: Fuzzing
 * @tc.require: FuzzedDataProvider library, Uri and DataShareHelperImpl-related headers are included
 * @tc.precon: DataShareHelperImpl instance is initialized
 * @tc.step:
    1. Generate random boolean isSystem via FuzzedDataProvider, initialize DataShareHelperImpl instance
    2. Generate random-length string uriString via FuzzedDataProvider, convert to Uri object
    3. Generate random-length string mode as raw file open mode via FuzzedDataProvider
    4. Call dataShareHelperImpl.OpenRawFile interface with Uri and mode
 * @tc.expect:
    1. No interface crash or raw file access error occurs
    2. Interface has normal fault tolerance for invalid Uri and invalid open mode
 */
void OpenRawFileFuzz(FuzzedDataProvider &provider)
{
    bool isSystem = provider.ConsumeBool();
    DataShareHelperImpl dataShareHelperImpl("datashare://datasharehelperimpl", isSystem);
    std::string uriString = provider.ConsumeRandomLengthString();
    Uri uri(uriString);
    std::string mode =  provider.ConsumeRandomLengthString();
    dataShareHelperImpl.OpenRawFile(uri, mode);
}

/**
 * @tc.name: InsertExtFuzz
 * @tc.desc: Fuzz test for InsertExt interface, verifying stability under random URI and result parameter
 * @tc.type: Fuzzing
 * @tc.require: FuzzedDataProvider library, Uri and DataShareHelperImpl-related headers are included
 * @tc.precon: Empty DataShareValuesBucket object 'value' is initialized
 * @tc.step:
    1. Generate random boolean isSystem via FuzzedDataProvider, initialize DataShareHelperImpl instance
    2. Generate random-length string uriString via FuzzedDataProvider, convert to Uri object
    3. Generate random-length string 'result' as insert result storage variable via FuzzedDataProvider
    4. Call dataShareHelperImpl.InsertExt interface with Uri, value and result
 * @tc.expect:
    1. No interface crash or data writing exception occurs
    2. Interface has normal fault tolerance for invalid Uri and empty data bucket
 */
void InsertExtFuzz(FuzzedDataProvider &provider)
{
    bool isSystem = provider.ConsumeBool();
    DataShareHelperImpl dataShareHelperImpl("datashare://datasharehelperimpl", isSystem);
    std::string uriString = provider.ConsumeRandomLengthString();
    Uri uri(uriString);
    DataShareValuesBucket value;
    std::string result =  provider.ConsumeRandomLengthString();
    dataShareHelperImpl.InsertExt(uri, value, result);
}

/**
 * @tc.name: BatchUpdateFuzz
 * @tc.desc: Fuzz test for BatchUpdate interface, verifying stability under empty operations and result list
 * @tc.type: Fuzzing
 * @tc.require: FuzzedDataProvider library and DataShareHelperImpl-related headers are included
 * @tc.precon: Empty UpdateOperations object 'operations' and empty result list 'results' are initialized
 * @tc.step:
    1. Generate random boolean isSystem via FuzzedDataProvider, initialize DataShareHelperImpl instance
    2. Directly call dataShareHelperImpl.BatchUpdate interface with operations and results
 * @tc.expect:
    1. No interface crash or null pointer exception occurs
    2. Interface has normal fault tolerance for empty operation list
 */
void BatchUpdateFuzz(FuzzedDataProvider &provider)
{
    bool isSystem = provider.ConsumeBool();
    DataShareHelperImpl dataShareHelperImpl("datashare://datasharehelperimpl", isSystem);
    UpdateOperations operations;
    std::vector<BatchUpdateResult> results = {};
    dataShareHelperImpl.BatchUpdate(operations, results);
}

/**
 * @tc.name: QueryFuzz
 * @tc.desc: Fuzz test for Query interface, verifying stability under random URI and column list
 * @tc.type: Fuzzing
 * @tc.require: FuzzedDataProvider library, Uri and DataShareHelperImpl-related headers are included
 * @tc.precon: Empty DataSharePredicates object 'predicates'
 *             and DatashareBusinessError object 'businessError' are initialized
 * @tc.step:
    1. Generate random boolean isSystem via FuzzedDataProvider, initialize DataShareHelperImpl instance
    2. Generate random-length string uriString via FuzzedDataProvider, convert to Uri object
    3. Generate two random-length strings (bytes1, bytes2) via FuzzedDataProvider, add to vector 'bytes'
    4. Call dataShareHelperImpl.Query interface with Uri, predicates, bytes and &businessError
 * @tc.expect:
    1. No interface crash or query logic exception occurs
    2. Interface has normal fault tolerance for invalid Uri and random column list (bytes)
 */
void QueryFuzz(FuzzedDataProvider &provider)
{
    bool isSystem = provider.ConsumeBool();
    DataShareHelperImpl dataShareHelperImpl("datashare://datasharehelperimpl", isSystem);
    std::string uriString = provider.ConsumeRandomLengthString();
    Uri uri(uriString);
    DataSharePredicates predicates;
    std::vector<std::string> bytes;
    const std::string bytes1 = provider.ConsumeRandomLengthString();
    const std::string bytes2 = provider.ConsumeRandomLengthString();
    bytes.emplace_back(bytes1);
    bytes.emplace_back(bytes2);
    DatashareBusinessError businessError;
    dataShareHelperImpl.Query(uri, predicates, bytes, &businessError);
}

/**
 * @tc.name: GetTypeFuzz
 * @tc.desc: Fuzz test for DataShareHelperImpl::GetType interface, verifying stability under random URI
 * @tc.type: Fuzzing
 * @tc.require: FuzzedDataProvider library, Uri and DataShareHelperImpl-related headers are included
 * @tc.precon: DataShareHelperImpl instance is initialized
 * @tc.step:
    1. Generate random boolean isSystem via FuzzedDataProvider, initialize DataShareHelperImpl instance
    2. Generate random-length string uriString via FuzzedDataProvider, convert to Uri object
    3. Call dataShareHelperImpl.GetType interface with Uri
 * @tc.expect:
    1. No interface crash or type recognition error occurs
    2. Interface has normal fault tolerance for invalid Uri
 */
void GetTypeFuzz(FuzzedDataProvider &provider)
{
    bool isSystem = provider.ConsumeBool();
    DataShareHelperImpl dataShareHelperImpl("datashare://datasharehelperimpl", isSystem);
    std::string uriString = provider.ConsumeRandomLengthString();
    Uri uri(uriString);
    dataShareHelperImpl.GetType(uri);
}

/**
 * @tc.name: DataShare_BatchInsertFuzz_001
 * @tc.desc: Fuzz test for BatchInsert interface, verifying stability under random URI and empty data list
 * @tc.type: Fuzzing
 * @tc.require: FuzzedDataProvider library, Uri and DataShareHelperImpl-related headers are included
 * @tc.precon: Empty vector 'values' (storing DataShareValuesBucket) is initialized
 * @tc.step:
    1. Generate random boolean isSystem via FuzzedDataProvider, initialize DataShareHelperImpl instance
    2. Generate random-length string uriString via FuzzedDataProvider, convert to Uri object
    3. Call dataShareHelperImpl.BatchInsert interface with Uri and values
 * @tc.expect:
    1. No interface crash or batch insertion exception occurs
    2. Interface has normal fault tolerance for invalid Uri and empty data list
 */
void BatchInsertFuzz(FuzzedDataProvider &provider)
{
    bool isSystem = provider.ConsumeBool();
    DataShareHelperImpl dataShareHelperImpl("datashare://datasharehelperimpl", isSystem);
    std::string uriString = provider.ConsumeRandomLengthString();
    Uri uri(uriString);
    std::vector<DataShareValuesBucket> values = {};
    dataShareHelperImpl.BatchInsert(uri, values);
}

/**
 * @tc.name: ExecuteBatchFuzz
 * @tc.desc: Fuzz test for ExecuteBatch interface, verifying stability under empty statement list
 * @tc.type: Fuzzing
 * @tc.require: FuzzedDataProvider library and DataShareHelperImpl-related headers are included
 * @tc.precon: Empty vector 'statements' (storing OperationStatement) and ExecResultSet object 'result' are initialized
 * @tc.step:
    1. Generate random boolean isSystem via FuzzedDataProvider, initialize DataShareHelperImpl instance
    2. Call dataShareHelperImpl.ExecuteBatch interface with statements and result
 * @tc.expect:
    1. No interface crash or batch execution error occurs
    2. Interface has normal fault tolerance for empty statement list
 */
void ExecuteBatchFuzz(FuzzedDataProvider &provider)
{
    bool isSystem = provider.ConsumeBool();
    DataShareHelperImpl dataShareHelperImpl("datashare://datasharehelperimpl", isSystem);
    std::vector<OperationStatement> statements = {};
    ExecResultSet result;
    dataShareHelperImpl.ExecuteBatch(statements, result);
}

/**
 * @tc.name: RegisterObserverFuzz
 * @tc.desc: Fuzz test for RegisterObserver interface, verifying stability under random URI and null observer
 * @tc.type: Fuzzing
 * @tc.require: FuzzedDataProvider library, Uri and AAFwk::IDataAbilityObserver-related headers are included
 * @tc.precon: Null sptr<AAFwk::IDataAbilityObserver> object 'dataObserver' is initialized
 * @tc.step:
    1. Generate random boolean isSystem via FuzzedDataProvider, initialize DataShareHelperImpl instance
    2. Generate random-length string uriString via FuzzedDataProvider, convert to Uri object
    3. Call dataShareHelperImpl.RegisterObserver interface with Uri and dataObserver
 * @tc.expect:
    1. No interface crash or observer registration exception occurs
    2. Interface has normal fault tolerance for invalid Uri and null observer
 */
void RegisterObserverFuzz(FuzzedDataProvider &provider)
{
    bool isSystem = provider.ConsumeBool();
    DataShareHelperImpl dataShareHelperImpl("datashare://datasharehelperimpl", isSystem);
    std::string uriString = provider.ConsumeRandomLengthString();
    Uri uri(uriString);
    sptr<AAFwk::IDataAbilityObserver> dataObserver;
    dataShareHelperImpl.RegisterObserver(uri, dataObserver);
}

/**
 * @tc.name: UnregisterObserverFuzz
 * @tc.desc: Fuzz test for UnregisterObserver interface, verifying stability under random URI and null observer
 * @tc.type: Fuzzing
 * @tc.require: FuzzedDataProvider library, Uri and AAFwk::IDataAbilityObserver-related headers are included
 * @tc.precon: Null sptr<AAFwk::IDataAbilityObserver> object 'dataObserver' is initialized
 * @tc.step:
    1. Generate random boolean isSystem via FuzzedDataProvider, initialize DataShareHelperImpl instance
    2. Generate random-length string uriString via FuzzedDataProvider, convert to Uri object
    3. Call dataShareHelperImpl.UnregisterObserver interface with Uri and dataObserver
 * @tc.expect:
    1. No interface crash or observer unregistration exception occurs
    2. Interface has normal fault tolerance for invalid Uri and null observer
 */
void UnregisterObserverFuzz(FuzzedDataProvider &provider)
{
    bool isSystem = provider.ConsumeBool();
    DataShareHelperImpl dataShareHelperImpl("datashare://datasharehelperimpl", isSystem);
    std::string uriString = provider.ConsumeRandomLengthString();
    Uri uri(uriString);
    sptr<AAFwk::IDataAbilityObserver> dataObserver;
    dataShareHelperImpl.UnregisterObserver(uri, dataObserver);
}

/**
 * @tc.name: NotifyChangeFuzz
 * @tc.desc: Fuzz test for NotifyChange interface, verifying stability under random URI
 * @tc.type: Fuzzing
 * @tc.require: FuzzedDataProvider library, Uri and DataShareHelperImpl-related headers are included
 * @tc.precon: DataShareHelperImpl instance is initialized
 * @tc.step:
    1. Generate random boolean isSystem via FuzzedDataProvider, initialize DataShareHelperImpl instance
    2. Generate random-length string uriString via FuzzedDataProvider, convert to Uri object
    3. Call dataShareHelperImpl.NotifyChange interface with Uri
 * @tc.expect:
    1. No interface crash or notification logic exception occurs
    2. Interface has normal fault tolerance for invalid Uri
 */
void NotifyChangeFuzz(FuzzedDataProvider &provider)
{
    bool isSystem = provider.ConsumeBool();
    DataShareHelperImpl dataShareHelperImpl("datashare://datasharehelperimpl", isSystem);
    std::string uriString = provider.ConsumeRandomLengthString();
    Uri uri(uriString);
    dataShareHelperImpl.NotifyChange(uri);
}

/**
 * @tc.name: RegisterObserverExtProviderFuzz
 * @tc.desc: Fuzz test for RegisterObserverExtProvider interface,
 *           verifying stability under random URI, observer URI and descendants flag
 * @tc.type: Fuzzing
 * @tc.require: FuzzedDataProvider library, Uri and DataShareObserver-related headers are included
 * @tc.precon: DataShareObserverTest instance can be initialized with random observer URI
 * @tc.step:
    1. Generate random boolean isSystem via FuzzedDataProvider, initialize DataShareHelperImpl instance
    2. Generate random-length string uriString via FuzzedDataProvider, convert to Uri object
    3. Generate random boolean isDescendants via FuzzedDataProvider
    4. Generate random-length string observerUri via FuzzedDataProvider, create DataShareObserver instance
    5. Call dataShareHelperImpl.RegisterObserverExtProvider with Uri, dataObserver and isDescendants
 * @tc.expect:
    1. No interface crash or external observer registration exception occurs
    2. Interface has normal fault tolerance for invalid URIs and random isDescendants values
 */
void RegisterObserverExtProviderFuzz(FuzzedDataProvider &provider)
{
    bool isSystem = provider.ConsumeBool();
    DataShareHelperImpl dataShareHelperImpl("datashare://datasharehelperimpl", isSystem);
    std::string uriString = provider.ConsumeRandomLengthString();
    Uri uri(uriString);
    bool isDescendants = provider.ConsumeBool();
    std::string observerUri = provider.ConsumeRandomLengthString();
    std::shared_ptr<DataShareObserver> dataObserver = std::make_shared<DataShareObserverTest>(observerUri);
    dataShareHelperImpl.RegisterObserverExtProvider(uri, dataObserver, isDescendants);
}

/**
 * @tc.name: UnregisterObserverExtProviderFuzz
 * @tc.desc: Fuzz test for UnregisterObserverExtProvider interface,
 *           verifying stability under random URI and observer URI
 * @tc.type: Fuzzing
 * @tc.require: FuzzedDataProvider library, Uri and DataShareObserver-related headers are included
 * @tc.precon: DataShareObserverTest instance can be initialized with random observer URI
 * @tc.step:
    1. Generate random boolean isSystem via FuzzedDataProvider, initialize DataShareHelperImpl instance
    2. Generate random-length string uriString via FuzzedDataProvider, convert to Uri object
    3. Generate random-length string observerUri via FuzzedDataProvider, create DataShareObserver instance
    4. Call dataShareHelperImpl.UnregisterObserverExtProvider with Uri and dataObserver
 * @tc.expect:
    1. No interface crash or external observer unregistration exception occurs
    2. Interface has normal fault tolerance for invalid URIs
 */
void UnregisterObserverExtProviderFuzz(FuzzedDataProvider &provider)
{
    bool isSystem = provider.ConsumeBool();
    DataShareHelperImpl dataShareHelperImpl("datashare://datasharehelperimpl", isSystem);
    std::string uriString = provider.ConsumeRandomLengthString();
    Uri uri(uriString);
    std::string observerUri = provider.ConsumeRandomLengthString();
    std::shared_ptr<DataShareObserver> dataObserver = std::make_shared<DataShareObserverTest>(observerUri);
    dataShareHelperImpl.UnregisterObserverExtProvider(uri, dataObserver);
}

/**
 * @tc.name: NotifyChangeExtProviderFuzz
 * @tc.desc: Fuzz test for NotifyChangeExtProvider interface, verifying stability with empty change info
 * @tc.type: Fuzzing
 * @tc.require: FuzzedDataProvider library and DataShareHelperImpl-related headers are included
 * @tc.precon: Empty DataShareObserver::ChangeInfo object 'changeInfo' is initialized
 * @tc.step:
    1. Generate random boolean isSystem via FuzzedDataProvider, initialize DataShareHelperImpl instance
    2. Call dataShareHelperImpl.NotifyChangeExtProvider with changeInfo
 * @tc.expect:
    1. No interface crash or external provider notification error occurs
    2. Interface has normal fault tolerance for empty changeInfo
 */
void NotifyChangeExtProviderFuzz(FuzzedDataProvider &provider)
{
    bool isSystem = provider.ConsumeBool();
    DataShareHelperImpl dataShareHelperImpl("datashare://datasharehelperimpl", isSystem);
    DataShareObserver::ChangeInfo changeInfo;
    // changeInfo.changeType_ = provider.ConsumeEnum<ChangeType>();
    dataShareHelperImpl.NotifyChangeExtProvider(changeInfo);
}

/**
 * @tc.name: NormalizeUriFuzz
 * @tc.desc: Fuzz test for NormalizeUri interface, verifying stability under random URI
 * @tc.type: Fuzzing
 * @tc.require: FuzzedDataProvider library, Uri and DataShareHelperImpl-related headers are included
 * @tc.precon: DataShareHelperImpl instance is initialized
 * @tc.step:
    1. Generate random boolean isSystem via FuzzedDataProvider, initialize DataShareHelperImpl instance
    2. Generate random-length string uriString via FuzzedDataProvider, convert to Uri object
    3. Call dataShareHelperImpl.NormalizeUri with Uri
 * @tc.expect:
    1. No interface crash or URI normalization exception occurs
    2. Interface handles malformed URIs gracefully
 */
void NormalizeUriFuzz(FuzzedDataProvider &provider)
{
    bool isSystem = provider.ConsumeBool();
    DataShareHelperImpl dataShareHelperImpl("datashare://datasharehelperimpl", isSystem);
    std::string uriString = provider.ConsumeRandomLengthString();
    Uri uri(uriString);
    dataShareHelperImpl.NormalizeUri(uri);
}

/**
 * @tc.name: DenormalizeUriFuzz
 * @tc.desc: Fuzz test for DenormalizeUri interface, verifying stability under random URI
 * @tc.type: Fuzzing
 * @tc.require: FuzzedDataProvider library, Uri and DataShareHelperImpl-related headers are included
 * @tc.precon: DataShareHelperImpl instance is initialized
 * @tc.step:
    1. Generate random boolean isSystem via FuzzedDataProvider, initialize DataShareHelperImpl instance
    2. Generate random-length string uriString via FuzzedDataProvider, convert to Uri object
    3. Call dataShareHelperImpl.DenormalizeUri with Uri
 * @tc.expect:
    1. No interface crash or URI denormalization exception occurs
    2. Interface handles malformed URIs gracefully
 */
void DenormalizeUriFuzz(FuzzedDataProvider &provider)
{
    bool isSystem = provider.ConsumeBool();
    DataShareHelperImpl dataShareHelperImpl("datashare://datasharehelperimpl", isSystem);
    std::string uriString = provider.ConsumeRandomLengthString();
    Uri uri(uriString);
    dataShareHelperImpl.DenormalizeUri(uri);
}

/**
 * @tc.name: AddQueryTemplateFuzz
 * @tc.desc: Fuzz test for AddQueryTemplate interface,
 *           verifying stability under random URI, subscriber ID and empty template
 * @tc.type: Fuzzing
 * @tc.require: FuzzedDataProvider library and DataShareHelperImpl-related headers are included
 * @tc.precon: Empty Template object 'tpl' is initialized
 * @tc.step:
    1. Generate random boolean isSystem via FuzzedDataProvider, initialize DataShareHelperImpl instance
    2. Generate random-length string uri via FuzzedDataProvider
    3. Generate random int64_t subscriberId via FuzzedDataProvider
    4. Call dataShareHelperImpl.AddQueryTemplate with uri, subscriberId and tpl
 * @tc.expect:
    1. No interface crash or template addition exception occurs
    2. Interface has normal fault tolerance for invalid URI, random subscriberId and empty tpl
 */
void AddQueryTemplateFuzz(FuzzedDataProvider &provider)
{
    bool isSystem = provider.ConsumeBool();
    DataShareHelperImpl dataShareHelperImpl("datashare://datasharehelperimpl", isSystem);
    std::string uri = provider.ConsumeRandomLengthString();
    int64_t subscriberId = provider.ConsumeIntegral<int64_t >();
    Template tpl = Template();
    dataShareHelperImpl.AddQueryTemplate(uri, subscriberId, tpl);
}

/**
 * @tc.name: DelQueryTemplateFuzz
 * @tc.desc: Fuzz test for DelQueryTemplate interface, verifying stability under random URI and subscriber ID
 * @tc.type: Fuzzing
 * @tc.require: FuzzedDataProvider library and DataShareHelperImpl-related headers are included
 * @tc.precon: DataShareHelperImpl instance is initialized
 * @tc.step:
    1. Generate random boolean isSystem via FuzzedDataProvider, initialize DataShareHelperImpl instance
    2. Generate random-length string uri via FuzzedDataProvider
    3. Generate random int64_t subscriberId via FuzzedDataProvider
    4. Call dataShareHelperImpl.DelQueryTemplate with uri and subscriberId
 * @tc.expect:
    1. No interface crash or template deletion exception occurs
    2. Interface has normal fault tolerance for invalid URI and non-existent subscriberId
 */
void DelQueryTemplateFuzz(FuzzedDataProvider &provider)
{
    bool isSystem = provider.ConsumeBool();
    DataShareHelperImpl dataShareHelperImpl("datashare://datasharehelperimpl", isSystem);
    std::string uri = provider.ConsumeRandomLengthString();
    int64_t subscriberId = provider.ConsumeIntegral<int64_t >();
    dataShareHelperImpl.DelQueryTemplate(uri, subscriberId);
}

/**
 * @tc.name: GetPublishedDataFuzz
 * @tc.desc: Fuzz test for GetPublishedData interface, verifying stability under random URI and subscriber ID
 * @tc.type: Fuzzing
 * @tc.require: FuzzedDataProvider library and DataShareHelperImpl-related headers are included
 * @tc.precon: DataShareHelperImpl instance is initialized
 * @tc.step:
    1. Generate random boolean isSystem via FuzzedDataProvider, initialize DataShareHelperImpl instance
    2. Generate random-length string uri via FuzzedDataProvider
    3. Generate random int64_t subscriberId via FuzzedDataProvider
    4. Call dataShareHelperImpl.GetPublishedData with uri and subscriberId
 * @tc.expect:
    1. No interface crash or published data retrieval exception occurs
    2. Interface has normal fault tolerance for invalid URI and non-existent subscriberId
 */
void GetPublishedDataFuzz(FuzzedDataProvider &provider)
{
    bool isSystem = provider.ConsumeBool();
    DataShareHelperImpl dataShareHelperImpl("datashare://datasharehelperimpl", isSystem);
    std::string uri = provider.ConsumeRandomLengthString();
    int64_t subscriberId = provider.ConsumeIntegral<int64_t >();
    dataShareHelperImpl.DelQueryTemplate(uri, subscriberId);
}

/**
 * @tc.name: SubscribeRdbDataFuzz
 * @tc.desc: Fuzz test for SubscribeRdbData interface,
 *           verifying stability under random URIs, template ID and empty callback
 * @tc.type: Fuzzing
 * @tc.require: FuzzedDataProvider library, TemplateId and RdbChangeNode-related headers are included
 * @tc.precon: Empty vector 'uris' and default-initialized TemplateId 'templateId' are prepared
 * @tc.step:
    1. Generate random boolean isSystem via FuzzedDataProvider, initialize DataShareHelperImpl instance
    2. Generate two random-length strings (uri1, uei2) via FuzzedDataProvider, add to vector 'uris'
    3. Define empty callback function for RdbChangeNode
    4. Call dataShareHelperImpl.SubscribeRdbData with uris, templateId and callback
 * @tc.expect:
    1. No interface crash or RDB data subscription exception occurs
    2. Interface handles invalid URIs, empty callback and uninitialized templateId gracefully
 */
void SubscribeRdbDataFuzz(FuzzedDataProvider &provider)
{
    bool isSystem = provider.ConsumeBool();
    DataShareHelperImpl dataShareHelperImpl("datashare://datasharehelperimpl", isSystem);
    std::string uri = provider.ConsumeRandomLengthString();
    std::vector<std::string> uris;
    const std::string uri1 = provider.ConsumeRandomLengthString();
    const std::string uei2 = provider.ConsumeRandomLengthString();
    TemplateId templateId;
    std::function<void(const RdbChangeNode &changeNode)> callback;
    dataShareHelperImpl.SubscribeRdbData(uris, templateId, callback);
}

/**
 * @tc.name: UnsubscribeRdbDataFuzz
 * @tc.desc: Fuzz test for UnsubscribeRdbData interface, verifying stability under random URIs and template ID
 * @tc.type: Fuzzing
 * @tc.require: FuzzedDataProvider library and TemplateId-related headers are included
 * @tc.precon: Empty vector 'uris' and default-initialized TemplateId 'templateId' are prepared
 * @tc.step:
    1. Generate random boolean isSystem via FuzzedDataProvider, initialize DataShareHelperImpl instance
    2. Generate two random-length strings (uri1, uei2) via FuzzedDataProvider, add to vector 'uris'
    3. Call dataShareHelperImpl.UnsubscribeRdbData with uris and templateId
 * @tc.expect:
    1. No interface crash or RDB data unsubscription exception occurs
    2. Interface handles invalid URIs and unsubscribed templateId gracefully
 */
void UnsubscribeRdbDataFuzz(FuzzedDataProvider &provider)
{
    bool isSystem = provider.ConsumeBool();
    DataShareHelperImpl dataShareHelperImpl("datashare://datasharehelperimpl", isSystem);
    std::string uri = provider.ConsumeRandomLengthString();
    std::vector<std::string> uris;
    const std::string uri1 = provider.ConsumeRandomLengthString();
    const std::string uei2 = provider.ConsumeRandomLengthString();
    TemplateId templateId;
    dataShareHelperImpl.UnsubscribeRdbData(uris, templateId);
}

/**
 * @tc.name: EnableRdbSubsFuzz
 * @tc.desc: Fuzz test for EnableRdbSubs interface, verifying stability under random URIs and template ID
 * @tc.type: Fuzzing
 * @tc.require: FuzzedDataProvider library and TemplateId-related headers are included
 * @tc.precon: Empty vector 'uris' and default-initialized TemplateId 'templateId' are prepared
 * @tc.step:
    1. Generate random boolean isSystem via FuzzedDataProvider, initialize DataShareHelperImpl instance
    2. Generate two random-length strings (uri1, uei2) via FuzzedDataProvider, add to vector 'uris'
    3. Call dataShareHelperImpl.EnableRdbSubs with uris and templateId
 * @tc.expect:
    1. No interface crash or RDB subscription activation exception occurs
    2. Interface handles invalid URIs and unregistered templateId gracefully
 */
void EnableRdbSubsFuzz(FuzzedDataProvider &provider)
{
    bool isSystem = provider.ConsumeBool();
    DataShareHelperImpl dataShareHelperImpl("datashare://datasharehelperimpl", isSystem);
    std::string uri = provider.ConsumeRandomLengthString();
    std::vector<std::string> uris;
    const std::string uri1 = provider.ConsumeRandomLengthString();
    const std::string uei2 = provider.ConsumeRandomLengthString();
    TemplateId templateId;
    dataShareHelperImpl.EnableRdbSubs(uris, templateId);
}

/**
 * @tc.name: DisableRdbSubsFuzz
 * @tc.desc: Fuzz test for DisableRdbSubs interface, verifying stability under random URIs and template ID
 * @tc.type: Fuzzing
 * @tc.require: FuzzedDataProvider library and TemplateId-related headers are included
 * @tc.precon: Empty vector 'uris' and default-initialized TemplateId 'templateId' are prepared
 * @tc.step:
    1. Generate random boolean isSystem via FuzzedDataProvider, initialize DataShareHelperImpl instance
    2. Generate two random-length strings (uri1, uei2) via FuzzedDataProvider, add to vector 'uris'
    3. Call dataShareHelperImpl.DisableRdbSubs with uris and templateId
 * @tc.expect:
    1. No interface crash or RDB subscription deactivation exception occurs
    2. Interface handles invalid URIs and unregistered templateId gracefully
 */
void DisableRdbSubsFuzz(FuzzedDataProvider &provider)
{
    bool isSystem = provider.ConsumeBool();
    DataShareHelperImpl dataShareHelperImpl("datashare://datasharehelperimpl", isSystem);
    std::string uri = provider.ConsumeRandomLengthString();
    std::vector<std::string> uris;
    const std::string uri1 = provider.ConsumeRandomLengthString();
    const std::string uei2 = provider.ConsumeRandomLengthString();
    TemplateId templateId;
    dataShareHelperImpl.DisableRdbSubs(uris, templateId);
}

/**
 * @tc.name: SubscribePublishedDataFuzz
 * @tc.desc: Fuzz test for SubscribePublishedData interface,
 *           verifying stability under random URIs, subscriber ID and empty callback
 * @tc.type: Fuzzing
 * @tc.require: FuzzedDataProvider library and PublishedDataChangeNode-related headers are included
 * @tc.precon: Empty vector 'uris' is prepared
 * @tc.step:
    1. Generate random boolean isSystem via FuzzedDataProvider, initialize DataShareHelperImpl instance
    2. Generate two random-length strings (uri1, uei2) via FuzzedDataProvider, add to vector 'uris'
    3. Generate random int subscriberId via FuzzedDataProvider
    4. Define empty callback function for PublishedDataChangeNode
    5. Call dataShareHelperImpl.SubscribePublishedData with uris, subscriberId and callback
 * @tc.expect:
    1. No interface crash or published data subscription exception occurs
    2. Interface handles invalid URIs, random subscriberId and empty callback gracefully
 */
void SubscribePublishedDataFuzz(FuzzedDataProvider &provider)
{
    bool isSystem = provider.ConsumeBool();
    DataShareHelperImpl dataShareHelperImpl("datashare://datasharehelperimpl", isSystem);
    std::vector<std::string> uris;
    const std::string uri1 = provider.ConsumeRandomLengthString();
    const std::string uei2 = provider.ConsumeRandomLengthString();
    int subscriberId = provider.ConsumeIntegral<int>();
    std::function<void(const PublishedDataChangeNode &changeNode)> callback;
    dataShareHelperImpl.SubscribePublishedData(uris, subscriberId, callback);
}

/**
 * @tc.name: UnsubscribePublishedDataFuzz
 * @tc.desc: Fuzz test for UnsubscribePublishedData interface, verifying stability under random URIs and subscriber ID
 * @tc.type: Fuzzing
 * @tc.require: FuzzedDataProvider library and DataShareHelperImpl-related headers are included
 * @tc.precon: Empty vector 'uris' is prepared
 * @tc.step:
    1. Generate random boolean isSystem via FuzzedDataProvider, initialize DataShareHelperImpl instance
    2. Generate two random-length strings (uri1, uei2) via FuzzedDataProvider, add to vector 'uris'
    3. Generate random int subscriberId via FuzzedDataProvider
    4. Call dataShareHelperImpl.UnsubscribePublishedData with uris and subscriberId
 * @tc.expect:
    1. No interface crash or published data unsubscription exception occurs
    2. Interface handles invalid URIs and unsubscribed subscriberId gracefully
 */
void UnsubscribePublishedDataFuzz(FuzzedDataProvider &provider)
{
    bool isSystem = provider.ConsumeBool();
    DataShareHelperImpl dataShareHelperImpl("datashare://datasharehelperimpl", isSystem);
    std::vector<std::string> uris;
    const std::string uri1 = provider.ConsumeRandomLengthString();
    const std::string uei2 = provider.ConsumeRandomLengthString();
    int subscriberId = provider.ConsumeIntegral<int>();
    dataShareHelperImpl.UnsubscribePublishedData(uris, subscriberId);
}

/**
 * @tc.name: EnablePubSubsFuzz
 * @tc.desc: Fuzz test for EnablePubSubs interface, verifying stability under random URIs and subscriber ID
 * @tc.type: Fuzzing
 * @tc.require: FuzzedDataProvider library and DataShareHelperImpl-related headers are included
 * @tc.precon: Empty vector 'uris' is prepared
 * @tc.step:
    1. Generate random boolean isSystem via FuzzedDataProvider, initialize DataShareHelperImpl instance
    2. Generate two random-length strings (uri1, uei2) via FuzzedDataProvider, add to vector 'uris'
    3. Generate random int subscriberId via FuzzedDataProvider
    4. Call dataShareHelperImpl.EnablePubSubs with uris and subscriberId
 * @tc.expect:
    1. No interface crash or published subscription activation exception occurs
    2. Interface handles invalid URIs and unregistered subscriberId gracefully
 */
void EnablePubSubsFuzz(FuzzedDataProvider &provider)
{
    bool isSystem = provider.ConsumeBool();
    DataShareHelperImpl dataShareHelperImpl("datashare://datasharehelperimpl", isSystem);
    std::vector<std::string> uris;
    const std::string uri1 = provider.ConsumeRandomLengthString();
    const std::string uei2 = provider.ConsumeRandomLengthString();
    int subscriberId = provider.ConsumeIntegral<int>();
    dataShareHelperImpl.EnablePubSubs(uris, subscriberId);
}

/**
 * @tc.name: DisablePubSubsFuzz
 * @tc.desc: Fuzz test for DisablePubSubs interface, verifying stability under random URIs and subscriber ID
 * @tc.type: Fuzzing
 * @tc.require: FuzzedDataProvider library and DataShareHelperImpl-related headers are included
 * @tc.precon: Empty vector 'uris' is prepared
 * @tc.step:
    1. Generate random boolean isSystem via FuzzedDataProvider, initialize DataShareHelperImpl instance
    2. Generate two random-length strings (uri1, uei2) via FuzzedDataProvider, add to vector 'uris'
    3. Generate random int subscriberId via FuzzedDataProvider
    4. Call dataShareHelperImpl.DisablePubSubs with uris and subscriberId
 * @tc.expect:
    1. No interface crash or published subscription deactivation exception occurs
    2. Interface handles invalid URIs and unregistered subscriberId gracefully
 */
void DisablePubSubsFuzz(FuzzedDataProvider &provider)
{
    bool isSystem = provider.ConsumeBool();
    DataShareHelperImpl dataShareHelperImpl("datashare://datasharehelperimpl", isSystem);
    std::vector<std::string> uris;
    const std::string uri1 = provider.ConsumeRandomLengthString();
    const std::string uei2 = provider.ConsumeRandomLengthString();
    int subscriberId = provider.ConsumeIntegral<int>();
    dataShareHelperImpl.DisablePubSubs(uris, subscriberId);
}

/**
 * @tc.name: SetSilentSwitchFuzz
 * @tc.desc: Fuzz test for SetSilentSwitch interface, verifying stability under random URI, enable flag and system flag
 * @tc.type: Fuzzing
 * @tc.require: FuzzedDataProvider library, Uri and DataShareHelper-related headers are included
 * @tc.precon: None
 * @tc.step:
    1. Generate random-length string uriString via FuzzedDataProvider, convert to Uri object
    2. Generate random boolean enable via FuzzedDataProvider (silent mode switch)
    3. Generate random boolean isSystem via FuzzedDataProvider
    4. Call DataShareHelper::SetSilentSwitch with Uri, enable and isSystem
 * @tc.expect:
    1. No interface crash or silent switch setting exception occurs
    2. Interface handles invalid Uri and random boolean flags gracefully
 */
void SetSilentSwitchFuzz(FuzzedDataProvider &provider)
{
    std::string uriString = provider.ConsumeRandomLengthString();
    Uri uri(uriString);
    bool enable = provider.ConsumeBool();
    bool isSystem = provider.ConsumeBool();
    DataShareHelper::SetSilentSwitch(uri, enable, isSystem);
}

/**
 * @tc.name: InsertExFuzz
 * @tc.desc: Fuzz test for InsertEx interface, verifying stability under random URI and empty data bucket
 * @tc.type: Fuzzing
 * @tc.require: FuzzedDataProvider library, Uri and DataShareValuesBucket-related headers are included
 * @tc.precon: Empty DataShareValuesBucket object 'value' is initialized
 * @tc.step:
    1. Generate random boolean isSystem via FuzzedDataProvider, initialize DataShareHelperImpl instance
    2. Generate random-length string uriString via FuzzedDataProvider, convert to Uri object
    3. Call dataShareHelperImpl.InsertEx with Uri and value
 * @tc.expect:
    1. No interface crash or data insertion exception occurs
    2. Interface handles invalid Uri and empty data bucket (value) gracefully
 */
void InsertExFuzz(FuzzedDataProvider &provider)
{
    bool isSystem = provider.ConsumeBool();
    DataShareHelperImpl dataShareHelperImpl("datashare://datasharehelperimpl", isSystem);
    std::string uriString = provider.ConsumeRandomLengthString();
    Uri uri(uriString);
    DataShareValuesBucket value;
    dataShareHelperImpl.InsertEx(uri, value);
}

/**
 * @tc.name: UpdateExFuzz
 * @tc.desc: Fuzz test for UpdateEx interface, verifying stability under random URI, empty predicates and data bucket
 * @tc.type: Fuzzing
 * @tc.require: FuzzedDataProvider library, Uri, DataSharePredicates and DataShareValuesBucket-related
 *              headers are included
 * @tc.precon: Empty DataSharePredicates 'predicates' and DataShareValuesBucket 'value' are initialized
 * @tc.step:
    1. Generate random boolean isSystem via FuzzedDataProvider, initialize DataShareHelperImpl instance
    2. Generate random-length string uriString via FuzzedDataProvider, convert to Uri object
    3. Call dataShareHelperImpl.UpdateEx with Uri, predicates and value
 * @tc.expect:
    1. No interface crash or data update exception occurs
    2. Interface handles invalid Uri, empty predicates and empty value gracefully
 */
void UpdateExFuzz(FuzzedDataProvider &provider)
{
    bool isSystem = provider.ConsumeBool();
    DataShareHelperImpl dataShareHelperImpl("datashare://datasharehelperimpl", isSystem);
    std::string uriString = provider.ConsumeRandomLengthString();
    Uri uri(uriString);
    DataSharePredicates predicates;
    DataShareValuesBucket value;
    dataShareHelperImpl.UpdateEx(uri, predicates, value);
}

/**
 * @tc.name: DeleteExFuzz
 * @tc.desc: Fuzz test for DeleteEx interface, verifying stability under random URI and empty predicates
 * @tc.type: Fuzzing
 * @tc.require: FuzzedDataProvider library, Uri and DataSharePredicates-related headers are included
 * @tc.precon: Empty DataSharePredicates object 'predicates' is initialized
 * @tc.step:
    1. Generate random boolean isSystem via FuzzedDataProvider, initialize DataShareHelperImpl instance
    2. Generate random-length string uriString via FuzzedDataProvider, convert to Uri object
    3. Call dataShareHelperImpl.DeleteEx with Uri and predicates
 * @tc.expect:
    1. No interface crash or data deletion exception occurs
    2. Interface handles invalid Uri and empty predicates gracefully
 */
void DeleteExFuzz(FuzzedDataProvider &provider)
{
    bool isSystem = provider.ConsumeBool();
    DataShareHelperImpl dataShareHelperImpl("datashare://datasharehelperimpl", isSystem);
    std::string uriString = provider.ConsumeRandomLengthString();
    Uri uri(uriString);
    DataSharePredicates predicates;
    DataShareValuesBucket value;
    dataShareHelperImpl.DeleteEx(uri, predicates);
}

/**
 * @tc.name: UserDefineFuncFuzz
 * @tc.desc: Fuzz test for UserDefineFunc interface, verifying stability with empty message parcels and default option
 * @tc.type: Fuzzing
 * @tc.require: FuzzedDataProvider library and MessageParcel/MessageOption-related headers are included
 * @tc.precon: Empty MessageParcel 'data', 'reply' and default MessageOption 'option' are initialized
 * @tc.step:
    1. Generate random boolean isSystem via FuzzedDataProvider, initialize DataShareHelperImpl instance
    2. Call dataShareHelperImpl.UserDefineFunc with data, reply and option
 * @tc.expect:
    1. No interface crash or custom function execution exception occurs
    2. Interface handles empty message parcels and default option gracefully
 */
void UserDefineFuncFuzz(FuzzedDataProvider &provider)
{
    bool isSystem = provider.ConsumeBool();
    DataShareHelperImpl dataShareHelperImpl("datashare://datasharehelperimpl", isSystem);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    dataShareHelperImpl.UserDefineFunc(data, reply, option);
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    OHOS::CreateFuzz(provider);
    OHOS::GetFileTypesFuzz(provider);
    OHOS::OpenFileFuzz(provider);
    OHOS::OpenRawFileFuzz(provider);
    OHOS::InsertExtFuzz(provider);
    OHOS::BatchUpdateFuzz(provider);
    OHOS::QueryFuzz(provider);
    OHOS::GetTypeFuzz(provider);
    OHOS::BatchInsertFuzz(provider);
    OHOS::ExecuteBatchFuzz(provider);
    OHOS::RegisterObserverFuzz(provider);
    OHOS::UnregisterObserverFuzz(provider);
    OHOS::NotifyChangeFuzz(provider);
    OHOS::RegisterObserverExtProviderFuzz(provider);
    OHOS::UnregisterObserverExtProviderFuzz(provider);
    OHOS::NotifyChangeExtProviderFuzz(provider);
    OHOS::NormalizeUriFuzz(provider);
    OHOS::DenormalizeUriFuzz(provider);
    OHOS::AddQueryTemplateFuzz(provider);
    OHOS::DelQueryTemplateFuzz(provider);
    OHOS::GetPublishedDataFuzz(provider);
    OHOS::SubscribeRdbDataFuzz(provider);
    OHOS::UnsubscribeRdbDataFuzz(provider);
    OHOS::EnableRdbSubsFuzz(provider);
    OHOS::DisableRdbSubsFuzz(provider);
    OHOS::SubscribePublishedDataFuzz(provider);
    OHOS::UnsubscribePublishedDataFuzz(provider);
    OHOS::EnablePubSubsFuzz(provider);
    OHOS::DisablePubSubsFuzz(provider);
    OHOS::SetSilentSwitchFuzz(provider);
    OHOS::InsertExFuzz(provider);
    OHOS::UpdateExFuzz(provider);
    OHOS::DeleteExFuzz(provider);
    OHOS::UserDefineFuncFuzz(provider);
    return 0;
}