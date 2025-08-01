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

#ifndef DATASHARE_PROXY_H
#define DATASHARE_PROXY_H

#include <iremote_proxy.h>
#include <memory>

#include "concurrent_map.h"
#include "dataobs_mgr_changeinfo.h"
#include "idatashare.h"

namespace OHOS {
namespace DataShare {
using ChangeInfo = AAFwk::ChangeInfo;
class DataShareProxy final : public IRemoteProxy<IDataShare> {
public:
    explicit DataShareProxy(const sptr<IRemoteObject>& remote) : IRemoteProxy<IDataShare>(remote) {}

    virtual ~DataShareProxy() {}

    virtual std::vector<std::string> GetFileTypes(const Uri &uri, const std::string &mimeTypeFilter) override;

    virtual int OpenFile(const Uri &uri, const std::string &mode) override;

    int OpenFileWithErrCode(const Uri &uri, const std::string &mode, int32_t &errCode);

    virtual int OpenRawFile(const Uri &uri, const std::string &mode) override;

    virtual int Insert(const Uri &uri, const DataShareValuesBucket &value) override;

    virtual int InsertExt(const Uri &uri, const DataShareValuesBucket &value, std::string &result) override;

    virtual int Update(const Uri &uri, const DataSharePredicates &predicates,
        const DataShareValuesBucket &value) override;

    virtual int BatchUpdate(const UpdateOperations &operations, std::vector<BatchUpdateResult> &results) override;

    virtual int Delete(const Uri &uri, const DataSharePredicates &predicates) override;

    virtual std::shared_ptr<DataShareResultSet> Query(const Uri &uri, const DataSharePredicates &predicates,
        std::vector<std::string> &columns, DatashareBusinessError &businessError) override;

    virtual std::string GetType(const Uri &uri) override;

    virtual int BatchInsert(const Uri &uri, const std::vector<DataShareValuesBucket> &values) override;

    virtual int ExecuteBatch(const std::vector<OperationStatement> &statements, ExecResultSet &result) override;

    virtual bool RegisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver) override;

    virtual bool UnregisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver) override;

    virtual bool NotifyChange(const Uri &uri) override;

    virtual int RegisterObserverExtProvider(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver,
        bool isDescendants, RegisterOption option) override;

    virtual int UnregisterObserverExtProvider(const Uri &uri,
        const sptr<AAFwk::IDataAbilityObserver> &dataObserver) override;

    virtual int NotifyChangeExtProvider(const ChangeInfo &changeInfo) override;

    virtual Uri NormalizeUri(const Uri &uri) override;

    virtual Uri DenormalizeUri(const Uri &uri) override;

    virtual std::pair<int32_t, int32_t> InsertEx(const Uri &uri, const DataShareValuesBucket &value) override;

    virtual std::pair<int32_t, int32_t> UpdateEx(const Uri &uri, const DataSharePredicates &predicates,
        const DataShareValuesBucket &value) override;

    virtual std::pair<int32_t, int32_t> DeleteEx(const Uri &uri, const DataSharePredicates &predicates) override;

    virtual int32_t UserDefineFunc(
        MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    bool CheckSize(const UpdateOperations &operations);
    int OpenFileInner(const Uri &uri, const std::string &mode, uint32_t requestCode, int32_t &errCode);
    static inline BrokerDelegator<DataShareProxy> delegator_;
    static const size_t MTU_SIZE = 921600; // 900k
    static const size_t MAX_SIZE = 4000;
};
} // namespace DataShare
} // namespace OHOS
#endif // DATASHARE_PROXY_H
