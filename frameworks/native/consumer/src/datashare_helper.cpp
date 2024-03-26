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
#define LOG_TAG "DataShareHelper"

#include "datashare_helper.h"
#include "datashare_helper_impl.h"

#include "adaptor.h"
#include "concurrent_map.h"
#include "data_ability_observer_interface.h"
#include "data_ability_observer_stub.h"
#include "dataobs_mgr_client.h"
#include "datashare_log.h"
#include "datashare_string_utils.h"

namespace OHOS {
namespace DataShare {
using namespace AppExecFwk;
namespace {
static constexpr const char *DATA_SHARE_PREFIX = "datashare:///";
static constexpr const char *FILE_PREFIX = "file://";
} // namespace
constexpr int INVALID_VALUE = -1;
class ObserverImpl : public AAFwk::DataAbilityObserverStub {
public:
    explicit ObserverImpl(const std::shared_ptr<DataShareObserver> dataShareObserver)
        : dataShareObserver_(dataShareObserver){};
    void OnChange();
    void OnChangeExt(const AAFwk::ChangeInfo &info);
    static DataShareObserver::ChangeInfo ConvertInfo(const AAFwk::ChangeInfo &info);
    static AAFwk::ChangeInfo ConvertInfo(const DataShareObserver::ChangeInfo &info);
    static sptr<ObserverImpl> GetObserver(const Uri& uri, const std::shared_ptr<DataShareObserver> &observer);
    static bool FindObserver(const Uri& uri, const std::shared_ptr<DataShareObserver> &observer);
    static bool DeleteObserver(const Uri& uri, const std::shared_ptr<DataShareObserver> &observer);
private:
    struct ObserverParam {
        sptr<ObserverImpl> obs_;
        std::list<Uri> uris_;
    };
    std::shared_ptr<DataShareObserver> dataShareObserver_;
    static ConcurrentMap<DataShareObserver *, ObserverParam> observers_;
};

ConcurrentMap<DataShareObserver *, ObserverImpl::ObserverParam> ObserverImpl::observers_;

std::string DataShareHelper::TransferUriPrefix(const std::string &originPrefix, const std::string &replacedPrefix,
    const std::string &originUriStr)
{
    if (originUriStr.find(originPrefix) != 0) {
        return originUriStr;
    }
    return replacedPrefix + originUriStr.substr(originPrefix.length());
}

/**
 * @brief You can use this method to specify the Uri of the data to operate and set the binding relationship
 * between the ability using the Data template (data share for short) and the associated client process in
 * a DataShareHelper instance.
 *
 * @param token Indicates the System token.
 * @param strUri Indicates the database table or disk file to operate.
 *
 * @return Returns the created DataShareHelper instance.
 */
std::shared_ptr<DataShareHelper> DataShareHelper::Creator(
    const sptr<IRemoteObject> &token, const std::string &strUri, const std::string &extUri)
{
    DISTRIBUTED_DATA_HITRACE(std::string(LOG_TAG) + "::" + std::string(__FUNCTION__));
    if (token == nullptr) {
        LOG_ERROR("token == nullptr");
        return nullptr;
    }

    std::string replacedUriStr = TransferUriPrefix(FILE_PREFIX, DATA_SHARE_PREFIX, strUri);
    Uri uri(replacedUriStr);

    if (uri.GetQuery().find("Proxy=true") != std::string::npos) {
        auto result = CreateServiceHelper();
        if (result != nullptr && IsSilentProxyEnable(strUri)) {
            return result;
        }
        if (extUri.empty()) {
            return nullptr;
        }
        Uri ext(extUri);
        return CreateExtHelper(ext, token);
    }
    return CreateExtHelper(uri, token);
}

std::shared_ptr<DataShareHelper> DataShareHelper::Creator(const string &strUri, const CreateOptions &options,
    const std::string &bundleName)
{
    DISTRIBUTED_DATA_HITRACE(std::string(LOG_TAG) + "::" + std::string(__FUNCTION__));
    Uri uri(strUri);
    if (!options.isProxy_ && options.token_ == nullptr) {
        LOG_ERROR("token is nullptr");
        return nullptr;
    }
    if (options.isProxy_) {
        return CreateServiceHelper(bundleName);
    }
    return CreateExtHelper(uri, options.token_);
}

std::shared_ptr<DataShareHelper> DataShareHelper::CreateServiceHelper(const std::string &bundleName)
{
    auto manager = DataShareManagerImpl::GetInstance();
    if (manager == nullptr) {
        LOG_ERROR("manager_ is nullptr");
        return nullptr;
    }
    manager->SetBundleName(bundleName);
    if (DataShareManagerImpl::GetServiceProxy() == nullptr) {
        LOG_ERROR("service proxy is nullptr.");
        return nullptr;
    }
    return std::make_shared<DataShareHelperImpl>();
}

bool DataShareHelper::IsSilentProxyEnable(const std::string &uri)
{
    auto proxy = DataShareManagerImpl::GetServiceProxy();
    if (proxy == nullptr) {
        LOG_ERROR("Service proxy is nullptr.");
        return false;
    }
    return proxy->IsSilentProxyEnable(uri);
}

std::shared_ptr<DataShareHelper> DataShareHelper::CreateExtHelper(Uri &uri, const sptr<IRemoteObject> &token)
{
    sptr<DataShareConnection> connection = new (std::nothrow) DataShareConnection(uri, token);
    if (connection == nullptr) {
        LOG_ERROR("Create DataShareConnection failed.");
        return nullptr;
    }
    auto dataShareConnection =
        std::shared_ptr<DataShareConnection>(connection.GetRefPtr(), [holder = connection](const auto *) {
            holder->DisconnectDataShareExtAbility();
        });
    if (dataShareConnection->GetDataShareProxy(uri, token) == nullptr) {
        LOG_ERROR("connect failed");
        return nullptr;
    }
    return std::make_shared<DataShareHelperImpl>(uri, token, dataShareConnection);
}

/**
 * Registers an observer to DataObsMgr specified by the given Uri.
 *
 * @param uri, Indicates the path of the data to operate.
 * @param dataObserver, Indicates the DataShareObserver object.
 * @param isDescendants, Indicates the Whether to note the change of descendants.
 */
void DataShareHelper::RegisterObserverExt(const Uri &uri, std::shared_ptr<DataShareObserver> dataObserver,
    bool isDescendants)
{
    if (dataObserver == nullptr) {
        LOG_ERROR("dataObserver is nullptr");
        return;
    }
    auto obsMgrClient = OHOS::AAFwk::DataObsMgrClient::GetInstance();
    if (obsMgrClient == nullptr) {
        LOG_ERROR("get DataObsMgrClient failed");
        return;
    }
    sptr<ObserverImpl> obs = ObserverImpl::GetObserver(uri, dataObserver);
    if (obs == nullptr) {
        LOG_ERROR("new ObserverImpl failed");
        return;
    }
    ErrCode ret = obsMgrClient->RegisterObserverExt(uri, obs, isDescendants);
    if (ret != ERR_OK) {
        ObserverImpl::DeleteObserver(uri, dataObserver);
    }
    LOG_INFO("Register observerExt, ret:%{public}d, uri:%{public}s",
        ret, DataShareStringUtils::Anonymous(uri.ToString()).c_str());
}

/**
 * Deregisters an observer used for DataObsMgr specified by the given Uri.
 *
 * @param uri, Indicates the path of the data to operate.
 * @param dataObserver, Indicates the DataShareObserver object.
 */
void DataShareHelper::UnregisterObserverExt(const Uri &uri, std::shared_ptr<DataShareObserver> dataObserver)
{
    if (dataObserver == nullptr) {
        LOG_ERROR("dataObserver is nullptr");
        return;
    }
    auto obsMgrClient = OHOS::AAFwk::DataObsMgrClient::GetInstance();
    if (obsMgrClient == nullptr) {
        LOG_ERROR("get DataObsMgrClient failed");
        return;
    }

    if (!ObserverImpl::FindObserver(uri, dataObserver)) {
        LOG_ERROR("observer not exit!");
        return;
    }

    sptr<ObserverImpl> obs = ObserverImpl::GetObserver(uri, dataObserver);
    if (obs == nullptr) {
        LOG_ERROR("new ObserverImpl failed");
        return;
    }
    ErrCode ret = obsMgrClient->UnregisterObserverExt(uri, obs);
    LOG_INFO("Unregister observerExt, ret:%{public}d, uri:%{public}s",
        ret, DataShareStringUtils::Anonymous(uri.ToString()).c_str());
    if (ret != ERR_OK) {
        return;
    }
    ObserverImpl::DeleteObserver(uri, dataObserver);
}

/**
 * Notifies the registered observers of a change to the data resource specified by Uris.
 *
 * @param changeInfo Indicates the info of the data to operate.
 */
void DataShareHelper::NotifyChangeExt(const DataShareObserver::ChangeInfo &changeInfo)
{
    auto obsMgrClient = OHOS::AAFwk::DataObsMgrClient::GetInstance();
    if (obsMgrClient == nullptr) {
        LOG_ERROR("get DataObsMgrClient failed");
        return;
    }

    ErrCode ret = obsMgrClient->NotifyChangeExt(ObserverImpl::ConvertInfo(changeInfo));
    LOG_INFO("Notify changeExt, ret:%{public}d", ret);
}

void ObserverImpl::OnChange() {}

void ObserverImpl::OnChangeExt(const AAFwk::ChangeInfo &info)
{
    dataShareObserver_->OnChange(ConvertInfo(info));
}

DataShareObserver::ChangeInfo ObserverImpl::ConvertInfo(const AAFwk::ChangeInfo &info)
{
    DataShareObserver::ChangeInfo changeInfo;
    changeInfo.changeType_ = static_cast<const DataShareObserver::ChangeType>(info.changeType_);
    changeInfo.uris_ = std::move(info.uris_);
    changeInfo.data_ = info.data_;
    changeInfo.size_ = info.size_;
    changeInfo.valueBuckets_ = std::move(info.valueBuckets_);
    return changeInfo;
}

AAFwk::ChangeInfo ObserverImpl::ConvertInfo(const DataShareObserver::ChangeInfo &info)
{
    AAFwk::ChangeInfo changeInfo;
    changeInfo.changeType_ = static_cast<const AAFwk::ChangeInfo::ChangeType>(info.changeType_);
    changeInfo.uris_ = std::move(info.uris_);
    changeInfo.data_ = const_cast<void*>(info.data_);
    changeInfo.size_ = info.size_;
    changeInfo.valueBuckets_ =std::move(info.valueBuckets_);
    return changeInfo;
}

sptr<ObserverImpl> ObserverImpl::GetObserver(const Uri& uri, const std::shared_ptr<DataShareObserver> &observer)
{
    sptr<ObserverImpl> result = nullptr;
    observers_.Compute(observer.get(), [&result, &uri, &observer](const auto &key, auto &value) {
        if (value.obs_ == nullptr) {
            value.obs_ = new (std::nothrow) ObserverImpl(observer);
            value.uris_.push_back(uri);
        } else {
            auto it = std::find(value.uris_.begin(), value.uris_.end(), uri);
            if (it == value.uris_.end()) {
                value.uris_.push_back(uri);
            }
        }

        result = value.obs_;
        return result != nullptr;
    });

    return result;
}

bool ObserverImpl::FindObserver(const Uri& uri, const std::shared_ptr<DataShareObserver> &observer)
{
    auto result = observers_.Find(observer.get());
    if (result.first) {
        auto it = std::find(result.second.uris_.begin(), result.second.uris_.end(), uri);
        if (it == result.second.uris_.end()) {
            return false;
        }
    }
    return result.first;
}

bool ObserverImpl::DeleteObserver(const Uri& uri, const std::shared_ptr<DataShareObserver> &observer)
{
    return observers_.ComputeIfPresent(observer.get(), [&uri](auto &key, auto &value) {
        value.uris_.remove_if([&uri](const auto &value) {
            return uri == value;
        });
        return !value.uris_.empty();
    });
}

int DataShareHelper::SetSilentSwitch(Uri &uri, bool enable)
{
    auto proxy = DataShareManagerImpl::GetServiceProxy();
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return INVALID_VALUE;
    }
    return proxy->SetSilentSwitch(uri, enable);
}
} // namespace DataShare
} // namespace OHOS