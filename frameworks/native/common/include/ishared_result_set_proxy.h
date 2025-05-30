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

#ifndef DATASHARE_I_SHARED_RESULT_SET_PROXY_H
#define DATASHARE_I_SHARED_RESULT_SET_PROXY_H
#include <memory>

#include "ishared_result_set.h"
#include "iremote_proxy.h"
#include "datashare_errno.h"

namespace OHOS::DataShare {
class ISharedResultSetProxy : public IRemoteProxy<ISharedResultSet> {
public:
    static std::shared_ptr<DataShareResultSet> CreateProxy(MessageParcel &parcel);
    explicit ISharedResultSetProxy(const sptr<IRemoteObject> &impl);
    virtual ~ISharedResultSetProxy() = default;
    int GetAllColumnNames(std::vector<std::string> &columnNames) override;
    int GetRowCount(int &count) override;
    bool OnGo(int startRowIndex, int targetRowIndex, int *cachedIndex = nullptr) override;
    int Close() override;
private:
    std::mutex mutex_;
    static BrokerDelegator<ISharedResultSetProxy> delegator_;
    std::vector<std::string> columnNames_;
    int32_t rowCount_ = -1;
};
} // namespace OHOS::DataShare
#endif // DATASHARE_I_SHARED_RESULT_SET_PROXY_H
