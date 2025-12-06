/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef STS_DATASHARE_EXT_ABILITY_H
#define STS_DATASHARE_EXT_ABILITY_H

#include <memory>
#include "datashare_result_set.h"
#include "datashare_predicates.h"
#include "datashare_ext_ability.h"
#include "ets_runtime.h"
#include "ets_native_reference.h"
#include "datashare_business_error.h"
#include "datashare_result.h"
#include "ani.h"

namespace OHOS {
namespace DataShare {
using namespace AbilityRuntime;

/**
 * @brief Sts datashare extension ability components.
 */
class StsDataShareExtAbility : public DataShareExtAbility {
public:
    explicit StsDataShareExtAbility(ETSRuntime& stsRuntime);
    virtual ~StsDataShareExtAbility() override;

    /**
     * @brief Create StsDataShareExtAbility.
     *
     * @param runtime The runtime.
     * @return The StsDataShareExtAbility instance.
     */
    static StsDataShareExtAbility* Create(const std::unique_ptr<Runtime>& runtime);

    /**
     * @brief Init the extension.
     *
     * @param record the extension record.
     * @param application the application info.
     * @param handler the extension handler.
     * @param token the remote token.
     */
    void Init(const std::shared_ptr<AppExecFwk::AbilityLocalRecord> &record,
        const std::shared_ptr<AppExecFwk::OHOSApplication> &application,
        std::shared_ptr<AppExecFwk::AbilityHandler> &handler,
        const sptr<IRemoteObject> &token) override;
    
    /**
     * @brief Called when this datashare extension ability is started. You must override this function if you want to
     *        perform some initialization operations during extension startup.
     *
     * This function can be called only once in the entire lifecycle of an extension.
     * @param Want Indicates the {@link Want} structure containing startup information about the extension.
     */
    void OnStart(const AAFwk::Want &want) override;

    /**
     * @brief Called when this datashare extension ability is connected for the first time.
     *
     * You can override this function to implement your own processing logic.
     *
     * @param want Indicates the {@link Want} structure containing connection information about the datashare
     * extension.
     * @return Returns a pointer to the <b>sid</b> of the connected datashare extension ability.
     */
    sptr<IRemoteObject> OnConnect(const AAFwk::Want &want) override;
    
    /**
     * @brief Inserts a single data record into the database.
     *
     * @param uri Indicates the path of the data to operate.
     * @param value  Indicates the data record to insert. If this parameter is null, a blank row will be inserted.
     *
     * @return Returns the index of the inserted data record.
     */
    int Insert(const Uri &uri, const DataShareValuesBucket &value) override;

    /**
     * @brief Updates data records in the database.
     *
     * @param uri Indicates the path of data to update.
     * @param predicates Indicates filter criteria. You should define the processing logic when this parameter is null.
     * @param value Indicates the data to update. This parameter can be null.
     *
     * @return Returns the number of data records updated.
     */
    int Update(const Uri &uri, const DataSharePredicates &predicates,
        const DataShareValuesBucket &value) override;
    
    /**
     * @brief Deletes one or more data records from the database.
     *
     * @param uri Indicates the path of the data to operate.
     * @param predicates Indicates filter criteria. You should define the processing logic when this parameter is null.
     *
     * @return Returns the number of data records deleted.
     */
    int Delete(const Uri &uri, const DataSharePredicates &predicates) override;

    /**
     * @brief Deletes one or more data records from the database.
     *
     * @param uri Indicates the path of data to query.
     * @param predicates Indicates filter criteria. You should define the processing logic when this parameter is null.
     * @param columns Indicates the columns to query. If this parameter is null, all columns are queried.
     *
     * @return Returns the query result.
     */
    std::shared_ptr<DataShareResultSet> Query(const Uri &uri, const DataSharePredicates &predicates,
        std::vector<std::string> &columns, DatashareBusinessError &businessError) override;
    
    /**
     * @brief Inserts multiple data records into the database.
     *
     * @param uri Indicates the path of the data to operate.
     * @param values Indicates the data records to insert.
     *
     * @return Returns the number of data records inserted.
     */
    int BatchInsert(const Uri &uri, const std::vector<DataShareValuesBucket> &values) override;

    /**
     * @brief Registers an observer to DataObsMgr specified by the given Uri.
     *
     * @param uri, Indicates the path of the data to operate.
     * @param dataObserver, Indicates the IDataAbilityObserver object.
     */
    bool RegisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver) override;

    /**
     * @brief Deregisters an observer used for DataObsMgr specified by the given Uri.
     *
     * @param uri, Indicates the path of the data to operate.
     * @param dataObserver, Indicates the IDataAbilityObserver object.
     */
    bool UnregisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver) override;

    /**
     * @brief Notifies the registered observers of a change to the data resource specified by Uri.
     *
     * @param uri, Indicates the path of the data to operate.
     *
     * @return Return true if success. otherwise return false.
     */
    bool NotifyChange(const Uri &uri) override;

    /**
     * @brief Notifies the registered observers of a change to the data resource specified by Uri.
     *
     * @param uri, Indicates the path of the data to operate.
     *
     * @return Return true if success. otherwise return false.
     */
    bool NotifyChangeWithUser(const Uri &uri, int32_t userId, uint32_t callingToken, int32_t callingPid) override;

    void InitResult(std::shared_ptr<ResultWrap> result) override;

    Uri NormalizeUri(const Uri &uri) override;

    Uri DenormalizeUri(const Uri &uri) override;

    int BatchUpdate(const UpdateOperations &operations, std::vector<BatchUpdateResult> &results) override;

private:
    void SaveNewCallingInfo(ani_env *env);
    void ResetEnv(ani_env *env);

    ETSRuntime& stsRuntime_;
    std::shared_ptr<ETSNativeReference> stsObj_ = nullptr;
    std::shared_ptr<ResultWrap> result_;
};

} // namespace DataShare
} // namespace OHOS
#endif // STS_DATASHARE_EXT_ABILITY_H
