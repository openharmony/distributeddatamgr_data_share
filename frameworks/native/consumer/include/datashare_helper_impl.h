/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef DATA_SHARE_HELPER_IMPL_H
#define DATA_SHARE_HELPER_IMPL_H

#include "concurrent_map.h"
#include "data_ability_observer_stub.h"
#include "datashare_helper.h"
#include "ext_special_controller.h"
#include "general_controller.h"
#include "persistent_data_controller.h"
#include "published_data_controller.h"

namespace OHOS::DataShare {
class ObserverImpl : public AAFwk::DataAbilityObserverStub {
public:
    explicit ObserverImpl(const std::shared_ptr<DataShareObserver> dataShareObserver)
        : dataShareObserver_(dataShareObserver){};
    void OnChange();
    void OnChangeExt(const AAFwk::ChangeInfo &info);
    static DataShareObserver::ChangeInfo ConvertInfo(const AAFwk::ChangeInfo &info);
    static AAFwk::ChangeInfo ConvertInfo(const DataShareObserver::ChangeInfo &info);
    static sptr<ObserverImpl> GetObserver(const Uri& uri, const std::shared_ptr<DataShareObserver> &observer);
    static bool FindObserver(const Uri& uri, const std::shared_ptr<DataShareObserver> &observer);
    static bool DeleteObserver(const Uri& uri, const std::shared_ptr<DataShareObserver> &observer);
private:
    struct ObserverParam {
        sptr<ObserverImpl> obs_;
        std::list<Uri> uris_;
    };
    std::shared_ptr<DataShareObserver> dataShareObserver_;
    static ConcurrentMap<DataShareObserver *, ObserverParam> observers_;
};

class DataShareHelperImpl : public DataShareHelper {
public:
    DataShareHelperImpl(const Uri &uri, const sptr<IRemoteObject> &token,
        std::shared_ptr<DataShareConnection> connection, bool isSystem = false);
    DataShareHelperImpl(std::string extUri = "", bool isSystem = false);

    ~DataShareHelperImpl() override;

    bool Release() override;

    std::vector<std::string> GetFileTypes(Uri &uri, const string &mimeTypeFilter) override;

    int OpenFile(Uri &uri, const string &mode) override;

    int OpenFileWithErrCode(Uri &uri, const string &mode, int32_t &errCode) override;

    int OpenRawFile(Uri &uri, const string &mode) override;

    int Insert(Uri &uri, const DataShareValuesBucket &value) override;

    int InsertExt(Uri &uri, const DataShareValuesBucket &value, std::string &result) override;

    int Update(Uri &uri, const DataSharePredicates &predicates, const DataShareValuesBucket &value) override;
    
    int BatchUpdate(const UpdateOperations &operations, std::vector<BatchUpdateResult> &results) override;

    int Delete(Uri &uri, const DataSharePredicates &predicates) override;

    std::shared_ptr<DataShareResultSet> Query(Uri &uri, const DataSharePredicates &predicates,
        std::vector<std::string> &columns, DatashareBusinessError *businessError) override;

    std::shared_ptr<DataShareResultSet> QueryTimeout(Uri &uri, const DataSharePredicates &predicates,
        std::vector<std::string> &columns, DataShareOption &option, DatashareBusinessError *businessError) override;

    string GetType(Uri &uri) override;

    int BatchInsert(Uri &uri, const std::vector<DataShareValuesBucket> &values) override;

    int ExecuteBatch(const std::vector<OperationStatement> &statements, ExecResultSet &result) override;

    int RegisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver) override;

    int UnregisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver) override;

    void NotifyChange(const Uri &uri) override;

    int RegisterObserverExtProvider(const Uri &uri, std::shared_ptr<DataShareObserver> dataObserver,
        bool isDescendants) override;

    int UnregisterObserverExtProvider(const Uri &uri, std::shared_ptr<DataShareObserver> dataObserver) override;

    void NotifyChangeExtProvider(const DataShareObserver::ChangeInfo &changeInfo) override;

    Uri NormalizeUri(Uri &uri) override;

    Uri DenormalizeUri(Uri &uri) override;

    int AddQueryTemplate(const string &uri, int64_t subscriberId, Template &tpl) override;

    int DelQueryTemplate(const string &uri, int64_t subscriberId) override;

    std::vector<OperationResult> Publish(const Data &data, const string &bundleName) override;

    Data GetPublishedData(const string &bundleName, int &resultCode) override;

    std::vector<OperationResult> SubscribeRdbData(const std::vector<std::string> &uris, const TemplateId &templateId,
        const std::function<void(const RdbChangeNode &)> &callback) override;

    std::vector<OperationResult> UnsubscribeRdbData(const std::vector<std::string> &uris,
        const TemplateId &templateId) override;

    std::vector<OperationResult> EnableRdbSubs(const std::vector<std::string> &uris,
        const TemplateId &templateId) override;

    std::vector<OperationResult> DisableRdbSubs(const std::vector<std::string> &uris,
        const TemplateId &templateId) override;

    std::vector<OperationResult> SubscribePublishedData(const std::vector<std::string> &uris, int64_t subscriberId,
        const std::function<void(const PublishedDataChangeNode &)> &callback) override;

    std::vector<OperationResult> UnsubscribePublishedData(const std::vector<std::string> &uris,
        int64_t subscriberId) override;

    std::vector<OperationResult> EnablePubSubs(const std::vector<std::string> &uris, int64_t subscriberId) override;

    std::vector<OperationResult> DisablePubSubs(const std::vector<std::string> &uris, int64_t subscriberId) override;

    std::pair<int32_t, int32_t> InsertEx(Uri &uri, const DataShareValuesBucket &value) override;

    std::pair<int32_t, int32_t> UpdateEx(
        Uri &uri, const DataSharePredicates &predicates, const DataShareValuesBucket &value) override;

    std::pair<int32_t, int32_t> DeleteEx(Uri &uri, const DataSharePredicates &predicates) override;

    int32_t UserDefineFunc(
        MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    std::shared_ptr<ExtSpecialController> extSpCtl_ = nullptr;
    std::shared_ptr<GeneralController> generalCtl_ = nullptr;
    std::shared_ptr<PersistentDataController> persistentDataCtl_ = nullptr;
    std::shared_ptr<PublishedDataController> publishedDataCtl_ = nullptr;
    bool isSystem_;
};
} // namespace OHOS::DataShare
#endif // DATA_SHARE_HELPER_IMPL_H
