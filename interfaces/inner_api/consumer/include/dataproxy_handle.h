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

#ifndef DATAPROXY_HANDLE_H
#define DATAPROXY_HANDLE_H

#include <string>

#include "datashare_business_error.h"
#include "datashare_errno.h"
#include "dataproxy_handle_common.h"
#include "datashare_observer.h"

namespace OHOS {

namespace DataShare {
class DataProxyHandle : public std::enable_shared_from_this<DataProxyHandle> {
public:
    /**
     * @brief Destructor.
     */
    ~DataProxyHandle() = default;

    /**
     * @brief Creates a DataProxyHandle instance.
     *
     * @return Returns the created DataProxyHandle instance.
     */
    static std::pair<int, std::shared_ptr<DataProxyHandle>> Create();

    std::vector<DataProxyResult> PublishProxyData(
        const std::vector<DataShareProxyData> &proxyData, const DataProxyConfig &proxyConfig);

    std::vector<DataProxyResult> DeleteProxyData(
        const std::vector<std::string> &uris, const DataProxyConfig &proxyConfig);

    static std::vector<DataProxyGetResult> GetProxyData(
        const std::vector<std::string> uris, const DataProxyConfig &proxyConfig);

    std::vector<DataProxyResult> SubscribeProxyData(const std::vector<std::string> &uris,
        const std::function<void(const std::vector<DataProxyChangeInfo> &changeNode)> &callback);

    std::vector<DataProxyResult> UnsubscribeProxyData(const std::vector<std::string> &uris);
};
} // namespace DataShare
} // namespace OHOS
#endif // DATAPROXY_HANDLE_H