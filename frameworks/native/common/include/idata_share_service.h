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

#ifndef IDATA_SHARE_SERVICE_H
#define IDATA_SHARE_SERVICE_H

#include <string>

#include "iremote_broker.h"
#include "datashare_errno.h"
#include "data_proxy_observer.h"
#include "datashare_business_error.h"
#include "datashare_common.h"
#include "datashare_predicates.h"
#include "datashare_result_set.h"
#include "datashare_template.h"
#include "datashare_values_bucket.h"
#include "distributeddata_data_share_ipc_interface_code.h"
#include "uri.h"
#include "dataproxy_handle_common.h"

namespace OHOS::DataShare {
class IDataShareService : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.DataShare.IDataShareService");

    virtual int Insert(const Uri &uri, const Uri &extUri, const DataShareValuesBucket &value) = 0;

    virtual int Update(const Uri &uri, const Uri &extUri, const DataSharePredicates &predicates,
        const DataShareValuesBucket &value) = 0;

    virtual int Delete(const Uri &uri, const Uri &extUri, const DataSharePredicates &predicates) = 0;

    virtual std::shared_ptr<DataShareResultSet> Query(DataShareParamSet &paramSet,
        const DataSharePredicates &predicates, std::vector<std::string> &columns,
        DatashareBusinessError &businessError) = 0;

    virtual int AddQueryTemplate(const std::string &uri, int64_t subscriberId, Template &tpl) = 0;

    virtual int DelQueryTemplate(const std::string &uri, int64_t subscriberId) = 0;

    virtual std::vector<OperationResult> Publish(const Data &data, const std::string &bundleName) = 0;

    virtual Data GetPublishedData(const std::string &bundleName, int &resultCode) = 0;

    virtual std::vector<OperationResult> SubscribeRdbData(const std::vector<std::string> &uris,
        const TemplateId &templateId, const sptr<IDataProxyRdbObserver> &observer) = 0;

    virtual std::vector<OperationResult> UnSubscribeRdbData(const std::vector<std::string> &uris,
        const TemplateId &templateId) = 0;

    virtual std::vector<OperationResult> EnableSubscribeRdbData(const std::vector<std::string> &uris,
        const TemplateId &templateId) = 0;

    virtual std::vector<OperationResult> DisableSubscribeRdbData(const std::vector<std::string> &uris,
        const TemplateId &templateId) = 0;

    virtual std::vector<OperationResult> SubscribePublishedData(const std::vector<std::string> &uris,
        int64_t subscriberId, const sptr<IDataProxyPublishedDataObserver> &observer) = 0;

    virtual std::vector<OperationResult> UnSubscribePublishedData(const std::vector<std::string> &uris,
        int64_t subscriberId) = 0;

    virtual std::vector<OperationResult> EnableSubscribePublishedData(const std::vector<std::string> &uris,
        int64_t subscriberId) = 0;

    virtual std::vector<OperationResult> DisableSubscribePublishedData(const std::vector<std::string> &uris,
        int64_t subscriberId) = 0;

    virtual void Notify(const std::string &uri) = 0;

    virtual int SetSilentSwitch(const Uri &uri, bool enable) = 0;

    virtual int GetSilentProxyStatus(const std::string &uri) = 0;

    virtual int RegisterObserver(const Uri &uri,
        const sptr<OHOS::IRemoteObject> &dataObserver) = 0;

    virtual int UnRegisterObserver(const Uri &uri,
        const sptr<OHOS::IRemoteObject> &dataObserver) = 0;

    virtual std::pair<int32_t, int32_t> InsertEx(const Uri &uri, const Uri &extUri,
        const DataShareValuesBucket &value) = 0;

    virtual std::pair<int32_t, int32_t> UpdateEx(const Uri &uri, const Uri &extUri,
        const DataSharePredicates &predicates, const DataShareValuesBucket &value) = 0;

    virtual std::pair<int32_t, int32_t> DeleteEx(const Uri &uri, const Uri &extUri,
        const DataSharePredicates &predicates) = 0;

    virtual std::vector<DataProxyResult> PublishProxyData(const std::vector<DataShareProxyData> &proxyData,
        const DataProxyConfig &proxyConfig) = 0;

    virtual std::vector<DataProxyResult> DeleteProxyData(const std::vector<std::string> &uris,
        const DataProxyConfig &proxyConfig) = 0;

    virtual std::vector<DataProxyGetResult> GetProxyData(const std::vector<std::string> uris,
        const DataProxyConfig &proxyConfig) = 0;

    virtual std::vector<DataProxyResult> SubscribeProxyData(const std::vector<std::string> &uris,
        const sptr<IProxyDataObserver> &observer) = 0;

    virtual std::vector<DataProxyResult> UnsubscribeProxyData(const std::vector<std::string> &uris) = 0;
};
} // namespace OHOS::DataShare
#endif
