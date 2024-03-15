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

#ifndef I_DATASHARE_H
#define I_DATASHARE_H

#include <iremote_broker.h>

#include "datashare_business_error.h"
#include "datashare_operation_statement.h"
#include "datashare_predicates.h"
#include "datashare_result_set.h"
#include "datashare_values_bucket.h"
#include "distributeddata_data_share_ipc_interface_code.h"
#include "uri.h"

namespace OHOS {
namespace AAFwk {
class IDataAbilityObserver;
}

namespace DataShare {
class IDataShare : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.DataShare.IDataShare");

    virtual int OpenFile(const Uri &uri, const std::string &mode) = 0;

    virtual int OpenRawFile(const Uri &uri, const std::string &mode) = 0;

    virtual int Insert(const Uri &uri, const DataShareValuesBucket &value) = 0;

    virtual int InsertExt(const Uri &uri, const DataShareValuesBucket &value, std::string &result) = 0;

    virtual int Update(const Uri &uri, const DataSharePredicates &predicates, const DataShareValuesBucket &value) = 0;
    
    virtual int BatchUpdate(const UpdateOperations &operations, std::vector<BatchUpdateResult> &results) = 0;

    virtual int Delete(const Uri &uri, const DataSharePredicates &predicates) = 0;

    virtual std::shared_ptr<DataShareResultSet> Query(const Uri &uri, const DataSharePredicates &predicates,
        std::vector<std::string> &columns, DatashareBusinessError &businessError) = 0;

    virtual std::string GetType(const Uri &uri) = 0;

    virtual int BatchInsert(const Uri &uri, const std::vector<DataShareValuesBucket> &values) = 0;

    virtual int ExecuteBatch(const std::vector<OperationStatement> &statements, ExecResultSet &result) = 0;

    virtual bool RegisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver) = 0;

    virtual bool UnregisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver) = 0;

    virtual bool NotifyChange(const Uri &uri) = 0;

    virtual Uri NormalizeUri(const Uri &uri) = 0;

    virtual Uri DenormalizeUri(const Uri &uri) = 0;

    virtual std::vector<std::string> GetFileTypes(const Uri &uri, const std::string &mimeTypeFilter) = 0;
};
} // namespace DataShare
} // namespace OHOS
#endif // I_DATASHARE_H

