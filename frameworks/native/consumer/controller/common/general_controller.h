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

#ifndef GENERAL_CONTROLLER_H
#define GENERAL_CONTROLLER_H

#include <memory>
#include <string_ex.h>

#include "data_ability_observer_interface.h"
#include "datashare_business_error.h"
#include "datashare_errno.h"
#include "datashare_predicates.h"
#include "datashare_result_set.h"
#include "datashare_values_bucket.h"
#include "uri.h"

namespace OHOS {
namespace AAFwk {
class IDataAbilityObserver;
}

namespace DataShare {
using ChangeInfo = AAFwk::ChangeInfo;
class GeneralController {
public:
    virtual ~GeneralController() = default;

    virtual int Insert(const Uri &uri, const DataShareValuesBucket &value) = 0;

    virtual int Update(const Uri &uri, const DataSharePredicates &predicates, const DataShareValuesBucket &value) = 0;

    virtual int Delete(const Uri &uri, const DataSharePredicates &predicates) = 0;

    virtual std::shared_ptr<DataShareResultSet> Query(const Uri &uri, const DataSharePredicates &predicates,
        std::vector<std::string> &columns, DatashareBusinessError &businessError, DataShareOption &option) = 0;

    virtual int RegisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver) = 0;

    virtual int UnregisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver) = 0;

    virtual void NotifyChange(const Uri &uri) = 0;

    /**
     * Registers an observer specified by the given Uri to the provider. This function is supported only when using
     * non-silent DataShareHelper, and there is no default implemention in the provider side. It needs to be handled by
     * the user. Otherwise, the provider side will do nothing but simply return error.
     */
    virtual int RegisterObserverExtProvider(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver,
        bool isDescendants) = 0;

    /**
     * Deregisters an observer specified by the given Uri to the provider. This function is supported only when using
     * non-silent DataShareHelper, and there is no default implemention in the provider side. It needs to be handled by
     * the user. Otherwise, the provider side will do nothing but simply return error.
     */
    virtual int UnregisterObserverExtProvider(const Uri &uri,
        const sptr<AAFwk::IDataAbilityObserver> &dataObserver) = 0;

    /**
     * Notifies the registered observers of a change to the data resource specified by Uris. This function is supported
     * only when using non-silent DataShareHelper, and there is no default implemention in the provider side. It needs
     * to be handled by the user. Otherwise, the provider side will do nothing but simply return true.
     */
    virtual int NotifyChangeExtProvider(const ChangeInfo &changeInfo) = 0;

    virtual std::pair<int32_t, int32_t> InsertEx(const Uri &uri, const DataShareValuesBucket &value) = 0;

    virtual std::pair<int32_t, int32_t> UpdateEx(
        const Uri &uri, const DataSharePredicates &predicates, const DataShareValuesBucket &value) = 0;

    virtual std::pair<int32_t, int32_t> DeleteEx(const Uri &uri, const DataSharePredicates &predicates) = 0;
};
} // namespace DataShare
} // namespace OHOS
#endif // GENERAL_CONTROLLER_H
