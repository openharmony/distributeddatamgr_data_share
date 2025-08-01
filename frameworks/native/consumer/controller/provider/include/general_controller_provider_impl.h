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

#ifndef GENERAL_CONTROLLER_PORVIDER_IMPL_H
#define GENERAL_CONTROLLER_PORVIDER_IMPL_H

#include "datashare_connection.h"
#include "datashare_option.h"
#include "general_controller.h"

namespace OHOS {
namespace AAFwk {
class IDataAbilityObserver;
}

namespace DataShare {
using ChangeInfo = AAFwk::ChangeInfo;
class GeneralControllerProviderImpl : public GeneralController {
public:
    GeneralControllerProviderImpl(std::shared_ptr<DataShareConnection> connection,
        const Uri &uri, const sptr<IRemoteObject> &token);

    virtual ~GeneralControllerProviderImpl() = default;

    int Insert(const Uri &uri, const DataShareValuesBucket &value) override;

    int Update(const Uri &uri, const DataSharePredicates &predicates, const DataShareValuesBucket &value) override;

    int Delete(const Uri &uri, const DataSharePredicates &predicates) override;

    std::shared_ptr<DataShareResultSet> Query(const Uri &uri, const DataSharePredicates &predicates,
        std::vector<std::string> &columns, DatashareBusinessError &businessError, DataShareOption &option) override;

    int RegisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver) override;

    int UnregisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver) override;

    void NotifyChange(const Uri &uri) override;

    std::pair<int32_t, int32_t> InsertEx(const Uri &uri, const DataShareValuesBucket &value) override;

    std::pair<int32_t, int32_t> UpdateEx(
        const Uri &uri, const DataSharePredicates &predicates, const DataShareValuesBucket &value) override;

    std::pair<int32_t, int32_t> DeleteEx(const Uri &uri, const DataSharePredicates &predicates) override;

    int RegisterObserverExtProvider(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver,
        bool isDescendants) override;

    int UnregisterObserverExtProvider(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver) override;

    int NotifyChangeExtProvider(const ChangeInfo &changeInfo) override;

private:
    std::shared_ptr<DataShareConnection> connection_ = nullptr;
    sptr<IRemoteObject> token_ = {};
    Uri uri_ = Uri("");
};
} // namespace DataShare
} // namespace OHOS
#endif // GENERAL_CONTROLLER_PORVIDER_IMPL_H
