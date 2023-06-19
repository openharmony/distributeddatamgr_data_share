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

#include "datashare_helper_impl.h"

#include "concurrent_map.h"
#include "data_ability_observer_interface.h"
#include "dataobs_mgr_client.h"
#include "datashare_log.h"
#include "datashare_result_set.h"

#include "general_controller_porvider_impl.h"
#include "general_controller_service_impl.h"
#include "persistent_data_controller.h"
#include "provider_special_controller.h"
#include "published_data_controller.h"

namespace OHOS {
namespace DataShare {
using namespace AppExecFwk;
constexpr int INVALID_VALUE = -1;
DataShareHelperImpl::DataShareHelperImpl(const Uri &uri, const sptr<IRemoteObject> &token,
    std::shared_ptr<DataShareConnection> connection)
{
    generalCtl_ = std::make_shared<GeneralControllerProviderImpl>(connection, uri, token);
    providerSpCtl_ = std::make_shared<ProviderSpecialController>(connection, uri, token);
}

DataShareHelperImpl::DataShareHelperImpl(std::shared_ptr<DataShareManagerImpl> serviceImpl)
{
    generalCtl_ = std::make_shared<GeneralControllerServiceImpl>(serviceImpl);
    persistentDataCtl_ = std::make_shared<PersistentDataController>(serviceImpl);
    publishedDataCtl_ = std::make_shared<PublishedDataController>(serviceImpl);
}

DataShareHelperImpl::~DataShareHelperImpl()
{
    if (needCleanSubscriber_) {
        persistentDataCtl_->UnSubscribeRdbData(this, {}, {});
        publishedDataCtl_->UnSubscribePublishedData(this, {}, {});
    }
}

bool DataShareHelperImpl::Release()
{
    providerSpCtl_ = nullptr;
    generalCtl_->Release();
    return true;
}

std::vector<std::string> DataShareHelperImpl::GetFileTypes(Uri &uri, const std::string &mimeTypeFilter)
{
    std::vector<std::string> matchedMIMEs;
    auto providerSpCtl = providerSpCtl_;
    if (providerSpCtl == nullptr) {
        LOG_ERROR("providerSpCtl is nullptr");
        return matchedMIMEs;
    }
    return providerSpCtl->GetFileTypes(uri, mimeTypeFilter);
}

int DataShareHelperImpl::OpenFile(Uri &uri, const std::string &mode)
{
    int fd = INVALID_VALUE;
    auto providerSpCtl = providerSpCtl_;
    if (providerSpCtl == nullptr) {
        LOG_ERROR("providerSpCtl is nullptr");
        return fd;
    }
    return providerSpCtl->OpenFile(uri, mode);
}

int DataShareHelperImpl::OpenRawFile(Uri &uri, const std::string &mode)
{
    int fd = INVALID_VALUE;
    auto providerSpCtl = providerSpCtl_;
    if (providerSpCtl == nullptr) {
        LOG_ERROR("providerSpCtl is nullptr");
        return fd;
    }
    return providerSpCtl->OpenRawFile(uri, mode);
}

int DataShareHelperImpl::Insert(Uri &uri, const DataShareValuesBucket &value)
{
    int index = INVALID_VALUE;
    auto generalCtl = generalCtl_;
    if (generalCtl == nullptr) {
        LOG_ERROR("generalCtl_ is nullptr");
        return index;
    }
    return generalCtl->Insert(uri, value);
}

int DataShareHelperImpl::Update(Uri &uri, const DataSharePredicates &predicates, const DataShareValuesBucket &value)
{
    int index = INVALID_VALUE;
    auto generalCtl = generalCtl_;
    if (generalCtl == nullptr) {
        LOG_ERROR("generalCtl is nullptr");
        return index;
    }
    return generalCtl->Update(uri, predicates, value);
}

int DataShareHelperImpl::Delete(Uri &uri, const DataSharePredicates &predicates)
{
    int index = INVALID_VALUE;
    auto generalCtl = generalCtl_;
    if (generalCtl == nullptr) {
        LOG_ERROR("generalCtl is nullptr");
        return index;
    }
    return generalCtl->Delete(uri, predicates);
}

std::shared_ptr<DataShareResultSet> DataShareHelperImpl::Query(Uri &uri, const DataSharePredicates &predicates,
    std::vector<std::string> &columns, DatashareBusinessError *businessError)
{
    std::shared_ptr<DataShareResultSet> resultSet = nullptr;
    auto generalCtl = generalCtl_;
    if (generalCtl == nullptr) {
        LOG_ERROR("generalCtl is nullptr");
        return resultSet;
    }
    DatashareBusinessError error;
    resultSet = generalCtl->Query(uri, predicates, columns, error);
    if (businessError != nullptr) {
        *businessError = error;
    }
    return resultSet;
}

std::string DataShareHelperImpl::GetType(Uri &uri)
{
    std::string type;
    auto providerSpCtl = providerSpCtl_;
    if (providerSpCtl == nullptr) {
        LOG_ERROR("providerSpCtl is nullptr");
        return type;
    }
    return providerSpCtl->GetType(uri);
}

int DataShareHelperImpl::BatchInsert(Uri &uri, const std::vector<DataShareValuesBucket> &values)
{
    int ret = INVALID_VALUE;
    auto providerSpCtl = providerSpCtl_;
    if (providerSpCtl == nullptr) {
        LOG_ERROR("providerSepOperator is nullptr");
        return ret;
    }
    return providerSpCtl->BatchInsert(uri, values);
}

void DataShareHelperImpl::RegisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver)
{
    LOG_INFO("Start");
    if (dataObserver == nullptr) {
        LOG_ERROR("dataObserver is nullptr");
        return;
    }
    auto generalCtl = generalCtl_;
    if (generalCtl == nullptr) {
        LOG_ERROR("generalCtl is nullptr");
        return;
    }
    return generalCtl->RegisterObserver(uri, dataObserver);
}

void DataShareHelperImpl::UnregisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver)
{
    LOG_INFO("Start");
    if (dataObserver == nullptr) {
        LOG_ERROR("dataObserver is nullptr");
        return;
    }
    auto generalCtl = generalCtl_;
    if (generalCtl == nullptr) {
        LOG_ERROR("generalCtl is nullptr");
        return;
    }
    return generalCtl->UnregisterObserver(uri, dataObserver);
}

void DataShareHelperImpl::NotifyChange(const Uri &uri)
{
    auto generalCtl = generalCtl_;
    if (generalCtl == nullptr) {
        LOG_ERROR("generalCtl is nullptr");
        return;
    }
    return generalCtl->NotifyChange(uri);
}

Uri DataShareHelperImpl::NormalizeUri(Uri &uri)
{
    Uri uriValue("");
    auto providerSpCtl = providerSpCtl_;
    if (providerSpCtl == nullptr) {
        LOG_ERROR("providerSpCtl is nullptr");
        return uriValue;
    }
    return providerSpCtl->NormalizeUri(uri);
}

Uri DataShareHelperImpl::DenormalizeUri(Uri &uri)
{
    Uri uriValue("");
    auto providerSpCtl = providerSpCtl_;
    if (providerSpCtl == nullptr) {
        LOG_ERROR("providerSpCtl is nullptr");
        return uriValue;
    }
    return providerSpCtl->DenormalizeUri(uri);
}

int DataShareHelperImpl::AddQueryTemplate(const std::string &uri, int64_t subscriberId, Template &tpl)
{
    int errNum = INVALID_VALUE;
    auto persistentDataCtl = persistentDataCtl_;
    if (persistentDataCtl == nullptr) {
        LOG_ERROR("persistentDataCtl is nullptr");
        return errNum;
    }
    return persistentDataCtl->AddQueryTemplate(uri, subscriberId, tpl);
}

int DataShareHelperImpl::DelQueryTemplate(const std::string &uri, int64_t subscriberId)
{
    int errNum = INVALID_VALUE;
    auto persistentDataCtl = persistentDataCtl_;
    if (persistentDataCtl == nullptr) {
        LOG_ERROR("persistentDataCtl is nullptr");
        return errNum;
    }
    return persistentDataCtl->DelQueryTemplate(uri, subscriberId);
}

std::vector<OperationResult> DataShareHelperImpl::Publish(const Data &data, const std::string &bundleName)
{
    std::vector<OperationResult> results;
    auto publishedDataCtl = publishedDataCtl_;
    if (publishedDataCtl == nullptr) {
        LOG_ERROR("publishedDataCtl is nullptr");
        return results;
    }
    return publishedDataCtl->Publish(data, bundleName);
}

Data DataShareHelperImpl::GetPublishedData(const std::string &bundleName, int &resultCode)
{
    Data results;
    auto publishedDataCtl = publishedDataCtl_;
    if (publishedDataCtl == nullptr) {
        LOG_ERROR("publishedDataCtl is nullptr");
        return results;
    }
    return publishedDataCtl->GetPublishedData(bundleName, resultCode);
}

std::vector<OperationResult> DataShareHelperImpl::SubscribeRdbData(const std::vector<std::string> &uris,
    const TemplateId &templateId, const std::function<void(const RdbChangeNode &changeNode)> &callback)
{
    LOG_DEBUG("Start SubscribeRdbData");
    std::vector<OperationResult> results;
    auto persistentDataCtl = persistentDataCtl_;
    if (persistentDataCtl == nullptr) {
        LOG_ERROR("persistentDataCtl is nullptr");
        return results;
    }
    return persistentDataCtl->SubscribeRdbData(this, uris, templateId, callback);
}

std::vector<OperationResult> DataShareHelperImpl::UnsubscribeRdbData(const std::vector<std::string> &uris,
    const TemplateId &templateId)
{
    LOG_DEBUG("Start UnsubscribeRdbData");
    std::vector<OperationResult> results;
    auto persistentDataCtl = persistentDataCtl_;
    if (persistentDataCtl == nullptr) {
        LOG_ERROR("persistentDataCtl is nullptr");
        return results;
    }
    return persistentDataCtl->UnSubscribeRdbData(this, uris, templateId);
}

std::vector<OperationResult> DataShareHelperImpl::EnableRdbSubs(const std::vector<std::string> &uris,
    const TemplateId &templateId)
{
    LOG_DEBUG("Start EnableSubscribeRdbData");
    std::vector<OperationResult> results;
    auto persistentDataCtl = persistentDataCtl_;
    if (persistentDataCtl == nullptr) {
        LOG_ERROR("persistentDataCtl is nullptr");
        return results;
    }
    return persistentDataCtl->EnableSubscribeRdbData(this, uris, templateId);
}

std::vector<OperationResult> DataShareHelperImpl::DisableRdbSubs(const std::vector<std::string> &uris,
    const TemplateId &templateId)
{
    LOG_DEBUG("Start DisableSubscribeRdbData");
    std::vector<OperationResult> results;
    auto persistentDataCtl = persistentDataCtl_;
    if (persistentDataCtl == nullptr) {
        LOG_ERROR("persistentDataCtl is nullptr");
        return results;
    }
    return persistentDataCtl->DisableSubscribeRdbData(this, uris, templateId);
}

std::vector<OperationResult> DataShareHelperImpl::SubscribePublishedData(const std::vector<std::string> &uris,
    int64_t subscriberId, const std::function<void(const PublishedDataChangeNode &changeNode)> &callback)
{
    std::vector<OperationResult> results;
    auto publishedDataCtl = publishedDataCtl_;
    if (publishedDataCtl == nullptr) {
        LOG_ERROR("publishedDataCtl is nullptr");
        return results;
    }
    return publishedDataCtl->SubscribePublishedData(this, uris, subscriberId, callback);
}

std::vector<OperationResult> DataShareHelperImpl::UnsubscribePublishedData(const std::vector<std::string> &uris,
    int64_t subscriberId)
{
    LOG_DEBUG("Start UnSubscribePublishedData");
    std::vector<OperationResult> results;
    auto publishedDataCtl = publishedDataCtl_;
    if (publishedDataCtl == nullptr) {
        LOG_ERROR("publishedDataCtl is nullptr");
        return results;
    }
    return publishedDataCtl->UnSubscribePublishedData(this, uris, subscriberId);
}

std::vector<OperationResult> DataShareHelperImpl::EnablePubSubs(const std::vector<std::string> &uris,
    int64_t subscriberId)
{
    LOG_DEBUG("Start UnSubscribePublishedData");
    std::vector<OperationResult> results;
    auto publishedDataCtl = publishedDataCtl_;
    if (publishedDataCtl == nullptr) {
        LOG_ERROR("publishedDataCtl is nullptr");
        return results;
    }
    return publishedDataCtl->EnableSubscribePublishedData(this, uris, subscriberId);
}

std::vector<OperationResult> DataShareHelperImpl::DisablePubSubs(const std::vector<std::string> &uris,
    int64_t subscriberId)
{
    LOG_DEBUG("Start UnSubscribePublishedData");
    std::vector<OperationResult> results;
    auto publishedDataCtl = publishedDataCtl_;
    if (publishedDataCtl == nullptr) {
        LOG_ERROR("publishedDataCtl is nullptr");
        return results;
    }
    return publishedDataCtl->DisableSubscribePublishedData(this, uris, subscriberId);
}
} // namespace DataShare
} // namespace OHOS