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
#include "executor_pool.h"
#include "general_controller.h"
#include "uri.h"

namespace OHOS {
namespace AAFwk {
class IDataAbilityObserver;
}

namespace DataShare {
using ChangeInfo = AAFwk::ChangeInfo;

struct TimedQueryResult {
    bool isFinish_;
    DatashareBusinessError businessError_;
    std::shared_ptr<DataShareResultSet> resultSet_;

    explicit TimedQueryResult(bool isFinish, DatashareBusinessError businessError,
        std::shared_ptr<DataShareResultSet> resultSet) : isFinish_(isFinish),
        businessError_(businessError), resultSet_(resultSet) {}
};

class GeneralControllerServiceImpl : public GeneralController {
public:
    GeneralControllerServiceImpl(const std::string &ext);

    virtual ~GeneralControllerServiceImpl();

    int Insert(const Uri &uri, const DataShareValuesBucket &value) override;

    int Update(const Uri &uri, const DataSharePredicates &predicates, const DataShareValuesBucket &value) override;

    int Delete(const Uri &uri, const DataSharePredicates &predicates) override;

    std::shared_ptr<DataShareResultSet> Query(const Uri &uri, const DataSharePredicates &predicates,
        std::vector<std::string> &columns, DatashareBusinessError &businessError, DataShareOption &option) override;

    int RegisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver) override;

    int UnregisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver) override;

    void NotifyChange(const Uri &uri) override;

    int RegisterObserverExtProvider(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver,
        bool isDescendants) override;

    int UnregisterObserverExtProvider(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver) override;

    int NotifyChangeExtProvider(const ChangeInfo &changeInfo) override;

    std::pair<int32_t, int32_t> InsertEx(const Uri &uri, const DataShareValuesBucket &value) override;

    std::pair<int32_t, int32_t> UpdateEx(
        const Uri &uri, const DataSharePredicates &predicates, const DataShareValuesBucket &value) override;

    std::pair<int32_t, int32_t> DeleteEx(const Uri &uri, const DataSharePredicates &predicates) override;

    int32_t SetExtUri(const std::string &extUri) override;

private:
    void ReRegisterObserver();

    void SetRegisterCallback();

    std::string GetExtUri();

    bool IsExtUri(const std::string &extUri);

    std::pair<std::shared_ptr<DataShareResultSet>, DatashareBusinessError> TimedQuery(
        std::shared_ptr<DataShareServiceProxy> proxy, const UriInfo &paramSet,
        const DataSharePredicates &predicates, const std::vector<std::string> &columns);

    ConcurrentMap<sptr<AAFwk::IDataAbilityObserver>, std::list<Uri>> observers_;

    std::shared_mutex mutex_;

    std::string extUri_;

    std::shared_ptr<ExecutorPool> pool_;

    static constexpr int MAX_RETRY_COUNT = 3;

    static constexpr int RANDOM_MIN = 50;

    static constexpr int RANDOM_MAX = 150;
};
} // namespace DataShare
} // namespace OHOS
#endif // GENERAL_CONTROLLER_SERVICE_IMPL_H
