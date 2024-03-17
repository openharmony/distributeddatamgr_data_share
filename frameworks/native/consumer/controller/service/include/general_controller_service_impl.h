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

#ifndef GENERAL_CONTROLLER_SERVICE_IMPL_H
#define GENERAL_CONTROLLER_SERVICE_IMPL_H

#include <memory>

#include "concurrent_map.h"
#include "data_share_manager_impl.h"
#include "general_controller.h"
#include "uri.h"

namespace OHOS {
namespace AAFwk {
class IDataAbilityObserver;
}

namespace DataShare {
class GeneralControllerServiceImpl : public GeneralController {
public:
    GeneralControllerServiceImpl() = default;

    virtual ~GeneralControllerServiceImpl();

    int Insert(const Uri &uri, const DataShareValuesBucket &value) override;

    int Update(const Uri &uri, const DataSharePredicates &predicates, const DataShareValuesBucket &value) override;

    int Delete(const Uri &uri, const DataSharePredicates &predicates) override;

    std::shared_ptr<DataShareResultSet> Query(const Uri &uri, const DataSharePredicates &predicates,
        std::vector<std::string> &columns, DatashareBusinessError &businessError) override;

    void RegisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver) override;

    void UnregisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver) override;

    void NotifyChange(const Uri &uri) override;
private:
    void ReRegisterObserver();

    void SetRegisterCallback();

    ConcurrentMap<sptr<AAFwk::IDataAbilityObserver>, std::list<Uri>> observers_;
};
} // namespace DataShare
} // namespace OHOS
#endif // GENERAL_CONTROLLER_SERVICE_IMPL_H
