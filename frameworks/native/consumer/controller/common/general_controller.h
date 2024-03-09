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

#include "datashare_business_error.h"
#include "datashare_predicates.h"
#include "datashare_result_set.h"
#include "datashare_values_bucket.h"
#include "uri.h"

namespace OHOS {
namespace AAFwk {
class IDataAbilityObserver;
}

namespace DataShare {
class GeneralController {
public:
    virtual ~GeneralController() = default;

    virtual int Insert(const Uri &uri, const DataShareValuesBucket &value) = 0;

    virtual int Update(const Uri &uri, const DataSharePredicates &predicates, const DataShareValuesBucket &value) = 0;

    virtual int Delete(const Uri &uri, const DataSharePredicates &predicates) = 0;

    virtual std::shared_ptr<DataShareResultSet> Query(const Uri &uri, const DataSharePredicates &predicates,
        std::vector<std::string> &columns, DatashareBusinessError &businessError) = 0;

    virtual void RegisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver) = 0;

    virtual void UnregisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver) = 0;

    virtual void NotifyChange(const Uri &uri) = 0;
};
} // namespace DataShare
} // namespace OHOS
#endif // GENERAL_CONTROLLER_H
