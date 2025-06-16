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

#define LOG_TAG "DataProxyHandle"

#include <utility>
#include "dataproxy_handle.h"

#include "adaptor.h"
#include "datashare_errno.h"
#include "datashare_log.h"
#include "data_share_manager_impl.h"
#include "proxy_data_subscriber_manager.h"

namespace OHOS {
namespace DataShare {

std::pair<int, std::shared_ptr<DataProxyHandle>> DataProxyHandle::Create()
{
    DISTRIBUTED_DATA_HITRACE(std::string(LOG_TAG) + "::" + std::string(__FUNCTION__));
    auto manager = DataShareManagerImpl::GetInstance();
    if (manager == nullptr) {
        LOG_ERROR("Manager is nullptr");
        return std::make_pair(E_ERROR, nullptr);
    }
    if (DataShareManagerImpl::GetServiceProxy() == nullptr) {
        LOG_ERROR("Service proxy is nullptr.");
        return std::make_pair(E_ERROR, nullptr);
    }
    auto handler = std::make_shared<DataProxyHandle>();
    if (handler != nullptr) {
        return std::make_pair(E_OK, handler);
    }
    return std::make_pair(E_ERROR, nullptr);
}

std::vector<DataProxyResult> DataProxyHandle::PublishProxyData(
    const std::vector<DataShareProxyData> &proxyData, const DataProxyConfig &proxyConfig)
{
    auto proxy = DataShareManagerImpl::GetServiceProxy();
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return std::vector<DataProxyResult>();
    }
    return proxy->PublishProxyData(proxyData, proxyConfig);
}

std::vector<DataProxyResult> DataProxyHandle::DeleteProxyData(
    const std::vector<std::string> &uris, const DataProxyConfig &proxyConfig)
{
    auto proxy = DataShareManagerImpl::GetServiceProxy();
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return std::vector<DataProxyResult>();
    }
    return proxy->DeleteProxyData(uris, proxyConfig);
}

std::vector<DataProxyGetResult> DataProxyHandle::GetProxyData(
    const std::vector<std::string> uris, const DataProxyConfig &proxyConfig)
{
    auto proxy = DataShareManagerImpl::GetServiceProxy();
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return std::vector<DataProxyGetResult>();
    }
    return proxy->GetProxyData(uris, proxyConfig);
}

std::vector<DataProxyResult> DataProxyHandle::SubscribeProxyData(const std::vector<std::string> &uris,
    const std::function<void(const std::vector<DataProxyChangeInfo> &changeNode)> &callback)
{
    auto proxy = DataShareManagerImpl::GetServiceProxy();
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return std::vector<DataProxyResult>();
    }
    return ProxyDataSubscriberManager::GetInstance().AddObservers(this, proxy, uris, callback);
}

std::vector<DataProxyResult> DataProxyHandle::UnsubscribeProxyData(const std::vector<std::string> &uris)
{
    auto proxy = DataShareManagerImpl::GetServiceProxy();
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return std::vector<DataProxyResult>();
    }
    return ProxyDataSubscriberManager::GetInstance().DelObservers(this, proxy, uris);
}
} // namespace DataShare
} // namespace OHOS