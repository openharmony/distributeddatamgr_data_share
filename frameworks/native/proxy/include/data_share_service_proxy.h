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

#include <iremote_proxy.h>

#include <atomic>
#include <list>

#include "datashare_values_bucket.h"
#include "idata_share_service.h"
#include "uri.h"
#include "datashare_business_error.h"

namespace OHOS::DataShare {
class DataShareServiceProxy : public IRemoteProxy<IDataShareService>, public BaseProxy {
public:
    explicit DataShareServiceProxy(const sptr<IRemoteObject> &object);
    virtual int Insert(const Uri &uri, const DataShareValuesBucket &valuesBucket) override;

    virtual  int Update(const Uri &uri, const DataSharePredicates &predicate,
                        const DataShareValuesBucket &valuesBucket) override;

    virtual  int Delete(const Uri &uri, const DataSharePredicates &predicate) override;

    virtual std::shared_ptr<DataShareResultSet> Query(const Uri &uri, const DataSharePredicates &predicates,
        std::vector<std::string> &columns, DatashareBusinessError &businessError) override;

    virtual std::vector<std::string> GetFileTypes(const Uri &uri, const std::string &mimeTypeFilter) override;

    virtual int OpenFile(const Uri &uri, const std::string &mode) override;

    virtual int OpenRawFile(const Uri &uri, const std::string &mode) override;

    virtual std::string GetType(const Uri &uri) override;

    virtual int BatchInsert(const Uri &uri, const std::vector<DataShareValuesBucket> &values) override;

    virtual bool RegisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver) override;

    virtual bool UnregisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver) override;

    virtual bool NotifyChange(const Uri &uri) override;

    virtual bool RegisterObserverExt(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver,
        bool isDescendants) override;

    virtual bool UnregisterObserverExt(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver) override;

    virtual bool UnregisterObserverExt(const sptr<AAFwk::IDataAbilityObserver> &dataObserver) override;

    virtual bool NotifyChangeExt(const AAFwk::ChangeInfo &changeInfo) override;

    virtual Uri NormalizeUri(const Uri &uri) override;

    virtual Uri DenormalizeUri(const Uri &uri) override;

private:
    static inline BrokerDelegator<DataShareServiceProxy> delegator_;
};
} // namespace OHOS::DataShare
#endif
