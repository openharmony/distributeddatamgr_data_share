/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef DATASHARESERVICE_DATA_SHARE_SERVICE_PROXY_H
#define DATASHARESERVICE_DATA_SHARE_SERVICE_PROXY_H

#include <atomic>
#include <iremote_proxy.h>
#include <list>

#include "idata_share_service.h"

namespace OHOS::DataShare {
class DataShareServiceProxy final : public IRemoteProxy<IDataShareService> {
public:
    explicit DataShareServiceProxy(const sptr<IRemoteObject> &object);
    int Insert(const Uri &uri, const DataShareValuesBucket &valuesBucket) override;

    int Update(const Uri &uri, const DataSharePredicates &predicate,
        const DataShareValuesBucket &valuesBucket) override;

    int Delete(const Uri &uri, const DataSharePredicates &predicate) override;

    std::shared_ptr<DataShareResultSet> Query(const Uri &uri, const DataSharePredicates &predicates,
        std::vector<std::string> &columns, DatashareBusinessError &businessError) override;

    int AddQueryTemplate(const std::string &uri, int64_t subscriberId, Template &tpl) override;

    int DelQueryTemplate(const std::string &uri, int64_t subscriberId) override;

    std::vector<OperationResult> Publish(const Data &data, const std::string &bundleName) override;

    Data GetPublishedData(const std::string &bundleName, int &resultCode) override;

    std::vector<OperationResult> SubscribeRdbData(const std::vector<std::string> &uris, const TemplateId &templateId,
        const sptr<IDataProxyRdbObserver> &observer) override;

    std::vector<OperationResult> UnSubscribeRdbData(
        const std::vector<std::string> &uris, const TemplateId &templateId) override;

    std::vector<OperationResult> EnableSubscribeRdbData(
        const std::vector<std::string> &uris, const TemplateId &templateId) override;

    std::vector<OperationResult> DisableSubscribeRdbData(
        const std::vector<std::string> &uris, const TemplateId &templateId) override;

    std::vector<OperationResult> SubscribePublishedData(const std::vector<std::string> &uris, int64_t subscriberId,
        const sptr<IDataProxyPublishedDataObserver> &observer) override;

    std::vector<OperationResult> UnSubscribePublishedData(
        const std::vector<std::string> &uris, int64_t subscriberId) override;

    std::vector<OperationResult> EnableSubscribePublishedData(
        const std::vector<std::string> &uris, int64_t subscriberId) override;

    std::vector<OperationResult> DisableSubscribePublishedData(
        const std::vector<std::string> &uris, int64_t subscriberId) override;
    void Notify(const std::string &uri) override;

    int SetSilentSwitch(const Uri &uri, bool enable) override;

    bool IsSilentProxyEnable(const std::string &uri) override;
    
    int RegisterObserver(const Uri &uri,
        const sptr<OHOS::IRemoteObject> &dataObserver) override;

    int UnRegisterObserver(const Uri &uri,
        const sptr<OHOS::IRemoteObject> &dataObserver) override;

private:
    static inline BrokerDelegator<DataShareServiceProxy> delegator_;
};
} // namespace OHOS::DataShare
#endif
