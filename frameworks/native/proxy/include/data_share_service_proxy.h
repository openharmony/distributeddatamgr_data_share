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

namespace OHOS::DataShare {
class DataShareServiceProxy : public IRemoteProxy<IDataShareService> {
public:
    explicit DataShareServiceProxy(const sptr<IRemoteObject> &object);

    int32_t Insert(const std::string &uri, const DataShareValuesBucket &valuesBucket) override;
    int32_t Update(const std::string &uri, const DataSharePredicates &predicate,
        const DataShareValuesBucket &valuesBucket) override;
    int32_t Delete(const std::string &uri, const DataSharePredicates &predicate) override;
    std::shared_ptr<DataShareResultSet> Query(const std::string &uri, const DataSharePredicates &predicates,
        const std::vector<std::string> &columns) override;

private:
    static inline BrokerDelegator<DataShareServiceProxy> delegator_;
};
} // namespace OHOS::DataShare
#endif
