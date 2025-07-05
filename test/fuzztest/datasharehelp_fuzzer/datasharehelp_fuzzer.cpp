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

void CreateFuzz(FuzzedDataProvider &provider)
{
    sptr<IRemoteObject> token = nullptr;
    std::string strUri =  provider.ConsumeRandomLengthString();
    std::string extUri =  provider.ConsumeRandomLengthString();
    int waitTime = provider.ConsumeIntegral<int>();
    DataShareHelper::Create(token, strUri, extUri, waitTime);
}

void GetFileTypesFuzz(FuzzedDataProvider &provider)
{
    bool isSystem = provider.ConsumeBool();
    DataShareHelperImpl dataShareHelperImpl("datashare://datasharehelperimpl", isSystem);
    std::string uriString = provider.ConsumeRandomLengthString();
    Uri uri(uriString);
    std::string mimeTypeFilter =  provider.ConsumeRandomLengthString();
    dataShareHelperImpl.GetFileTypes(uri, mimeTypeFilter);
}

void OpenFileFuzz(FuzzedDataProvider &provider)
{
    bool isSystem = provider.ConsumeBool();
    DataShareHelperImpl dataShareHelperImpl("datashare://datasharehelperimpl", isSystem);
    std::string uriString = provider.ConsumeRandomLengthString();
    Uri uri(uriString);
    std::string mode =  provider.ConsumeRandomLengthString();
    dataShareHelperImpl.OpenFile(uri, mode);
}

void OpenRawFileFuzz(FuzzedDataProvider &provider)
{
    bool isSystem = provider.ConsumeBool();
    DataShareHelperImpl dataShareHelperImpl("datashare://datasharehelperimpl", isSystem);
    std::string uriString = provider.ConsumeRandomLengthString();
    Uri uri(uriString);
    std::string mode =  provider.ConsumeRandomLengthString();
    dataShareHelperImpl.OpenRawFile(uri, mode);
}

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

void BatchUpdateFuzz(FuzzedDataProvider &provider)
{
    bool isSystem = provider.ConsumeBool();
    DataShareHelperImpl dataShareHelperImpl("datashare://datasharehelperimpl", isSystem);
    UpdateOperations operations;
    std::vector<BatchUpdateResult> results = {};
    dataShareHelperImpl.BatchUpdate(operations, results);
}

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

void GetTypeFuzz(FuzzedDataProvider &provider)
{
    bool isSystem = provider.ConsumeBool();
    DataShareHelperImpl dataShareHelperImpl("datashare://datasharehelperimpl", isSystem);
    std::string uriString = provider.ConsumeRandomLengthString();
    Uri uri(uriString);
    dataShareHelperImpl.GetType(uri);
}

void BatchInsertFuzz(FuzzedDataProvider &provider)
{
    bool isSystem = provider.ConsumeBool();
    DataShareHelperImpl dataShareHelperImpl("datashare://datasharehelperimpl", isSystem);
    std::string uriString = provider.ConsumeRandomLengthString();
    Uri uri(uriString);
    std::vector<DataShareValuesBucket> values = {};
    dataShareHelperImpl.BatchInsert(uri, values);
}

void ExecuteBatchFuzz(FuzzedDataProvider &provider)
{
    bool isSystem = provider.ConsumeBool();
    DataShareHelperImpl dataShareHelperImpl("datashare://datasharehelperimpl", isSystem);
    std::vector<OperationStatement> statements = {};
    ExecResultSet result;
    dataShareHelperImpl.ExecuteBatch(statements, result);
}

void RegisterObserverFuzz(FuzzedDataProvider &provider)
{
    bool isSystem = provider.ConsumeBool();
    DataShareHelperImpl dataShareHelperImpl("datashare://datasharehelperimpl", isSystem);
    std::string uriString = provider.ConsumeRandomLengthString();
    Uri uri(uriString);
    sptr<AAFwk::IDataAbilityObserver> dataObserver;
    dataShareHelperImpl.RegisterObserver(uri, dataObserver);
}

void UnregisterObserverFuzz(FuzzedDataProvider &provider)
{
    bool isSystem = provider.ConsumeBool();
    DataShareHelperImpl dataShareHelperImpl("datashare://datasharehelperimpl", isSystem);
    std::string uriString = provider.ConsumeRandomLengthString();
    Uri uri(uriString);
    sptr<AAFwk::IDataAbilityObserver> dataObserver;
    dataShareHelperImpl.UnregisterObserver(uri, dataObserver);
}

void NotifyChangeFuzz(FuzzedDataProvider &provider)
{
    bool isSystem = provider.ConsumeBool();
    DataShareHelperImpl dataShareHelperImpl("datashare://datasharehelperimpl", isSystem);
    std::string uriString = provider.ConsumeRandomLengthString();
    Uri uri(uriString);
    dataShareHelperImpl.NotifyChange(uri);
}

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

void NotifyChangeExtProviderFuzz(FuzzedDataProvider &provider)
{
    bool isSystem = provider.ConsumeBool();
    DataShareHelperImpl dataShareHelperImpl("datashare://datasharehelperimpl", isSystem);
    DataShareObserver::ChangeInfo changeInfo;
    // changeInfo.changeType_ = provider.ConsumeEnum<ChangeType>();
    dataShareHelperImpl.NotifyChangeExtProvider(changeInfo);
}

void NormalizeUriFuzz(FuzzedDataProvider &provider)
{
    bool isSystem = provider.ConsumeBool();
    DataShareHelperImpl dataShareHelperImpl("datashare://datasharehelperimpl", isSystem);
    std::string uriString = provider.ConsumeRandomLengthString();
    Uri uri(uriString);
    dataShareHelperImpl.NormalizeUri(uri);
}

void DenormalizeUriFuzz(FuzzedDataProvider &provider)
{
    bool isSystem = provider.ConsumeBool();
    DataShareHelperImpl dataShareHelperImpl("datashare://datasharehelperimpl", isSystem);
    std::string uriString = provider.ConsumeRandomLengthString();
    Uri uri(uriString);
    dataShareHelperImpl.DenormalizeUri(uri);
}

void AddQueryTemplateFuzz(FuzzedDataProvider &provider)
{
    bool isSystem = provider.ConsumeBool();
    DataShareHelperImpl dataShareHelperImpl("datashare://datasharehelperimpl", isSystem);
    std::string uri = provider.ConsumeRandomLengthString();
    int64_t subscriberId = provider.ConsumeIntegral<int64_t >();
    Template tpl = Template();
    dataShareHelperImpl.AddQueryTemplate(uri, subscriberId, tpl);
}

void DelQueryTemplateFuzz(FuzzedDataProvider &provider)
{
    bool isSystem = provider.ConsumeBool();
    DataShareHelperImpl dataShareHelperImpl("datashare://datasharehelperimpl", isSystem);
    std::string uri = provider.ConsumeRandomLengthString();
    int64_t subscriberId = provider.ConsumeIntegral<int64_t >();
    dataShareHelperImpl.DelQueryTemplate(uri, subscriberId);
}

void GetPublishedDataFuzz(FuzzedDataProvider &provider)
{
    bool isSystem = provider.ConsumeBool();
    DataShareHelperImpl dataShareHelperImpl("datashare://datasharehelperimpl", isSystem);
    std::string uri = provider.ConsumeRandomLengthString();
    int64_t subscriberId = provider.ConsumeIntegral<int64_t >();
    dataShareHelperImpl.DelQueryTemplate(uri, subscriberId);
}

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

void SetSilentSwitchFuzz(FuzzedDataProvider &provider)
{
    std::string uriString = provider.ConsumeRandomLengthString();
    Uri uri(uriString);
    bool enable = provider.ConsumeBool();
    bool isSystem = provider.ConsumeBool();
    DataShareHelper::SetSilentSwitch(uri, enable, isSystem);
}

void InsertExFuzz(FuzzedDataProvider &provider)
{
    bool isSystem = provider.ConsumeBool();
    DataShareHelperImpl dataShareHelperImpl("datashare://datasharehelperimpl", isSystem);
    std::string uriString = provider.ConsumeRandomLengthString();
    Uri uri(uriString);
    DataShareValuesBucket value;
    dataShareHelperImpl.InsertEx(uri, value);
}

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