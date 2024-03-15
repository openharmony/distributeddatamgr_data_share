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
#define LOG_TAG "DataShareHelperImpl"

#include "datashare_helper_impl.h"

#include "adaptor.h"
#include "concurrent_map.h"
#include "data_ability_observer_interface.h"
#include "dataobs_mgr_client.h"
#include "datashare_log.h"
#include "datashare_result_set.h"

#include "general_controller_provider_impl.h"
#include "general_controller_service_impl.h"

namespace OHOS {
namespace DataShare {
using namespace AppExecFwk;
constexpr int INVALID_VALUE = -1;
DataShareHelperImpl::DataShareHelperImpl(const Uri &uri, const sptr<IRemoteObject> &token,
    std::shared_ptr<DataShareConnection> connection)
{
    LOG_DEBUG("starts");
    generalCtl_ = std::make_shared<GeneralControllerProviderImpl>(connection, uri, token);
    extSpCtl_ = std::make_shared<ExtSpecialController>(connection, uri, token);
}

DataShareHelperImpl::DataShareHelperImpl()
{
    LOG_DEBUG("starts");
    generalCtl_ = std::make_shared<GeneralControllerServiceImpl>();
    persistentDataCtl_ = std::make_shared<PersistentDataController>();
    publishedDataCtl_ = std::make_shared<PublishedDataController>();
}

DataShareHelperImpl::~DataShareHelperImpl()
{
    if (persistentDataCtl_ != nullptr && publishedDataCtl_ != nullptr) {
        persistentDataCtl_->UnSubscribeRdbData(this, {}, {});
        publishedDataCtl_->UnSubscribePublishedData(this, {}, {});
    }
}

bool DataShareHelperImpl::Release()
{
    extSpCtl_ = nullptr;
    generalCtl_ = nullptr;
    return true;
}

std::vector<std::string> DataShareHelperImpl::GetFileTypes(Uri &uri, const std::string &mimeTypeFilter)
{
    auto extSpCtl = extSpCtl_;
    if (extSpCtl == nullptr) {
        LOG_ERROR("extSpCtl is nullptr");
        return std::vector<std::string>();
    }
    return extSpCtl->GetFileTypes(uri, mimeTypeFilter);
}

int DataShareHelperImpl::OpenFile(Uri &uri, const std::string &mode)
{
    auto extSpCtl = extSpCtl_;
    if (extSpCtl == nullptr) {
        LOG_ERROR("extSpCtl is nullptr");
        return INVALID_VALUE;
    }
    return extSpCtl->OpenFile(uri, mode);
}

int DataShareHelperImpl::OpenRawFile(Uri &uri, const std::string &mode)
{
    auto extSpCtl = extSpCtl_;
    if (extSpCtl == nullptr) {
        LOG_ERROR("extSpCtl is nullptr");
        return INVALID_VALUE;
    }
    return extSpCtl->OpenRawFile(uri, mode);
}

int DataShareHelperImpl::Insert(Uri &uri, const DataShareValuesBucket &value)
{
    DISTRIBUTED_DATA_HITRACE(std::string(LOG_TAG) + "::" + std::string(__FUNCTION__));
    auto generalCtl = generalCtl_;
    if (generalCtl == nullptr) {
        LOG_ERROR("generalCtl_ is nullptr");
        return INVALID_VALUE;
    }
    return generalCtl->Insert(uri, value);
}

int DataShareHelperImpl::InsertExt(Uri &uri, const DataShareValuesBucket &value, std::string &result)
{
    DISTRIBUTED_DATA_HITRACE(std::string(LOG_TAG) + "::" + std::string(__FUNCTION__));
    auto extSpCtl = extSpCtl_;
    if (extSpCtl == nullptr) {
        LOG_ERROR("providerSpCtl is nullptr");
        return INVALID_VALUE;
    }
    return extSpCtl->InsertExt(uri, value, result);
}

int DataShareHelperImpl::Update(Uri &uri, const DataSharePredicates &predicates, const DataShareValuesBucket &value)
{
    DISTRIBUTED_DATA_HITRACE(std::string(LOG_TAG) + "::" + std::string(__FUNCTION__));
    auto generalCtl = generalCtl_;
    if (generalCtl == nullptr) {
        LOG_ERROR("generalCtl is nullptr");
        return INVALID_VALUE;
    }
    return generalCtl->Update(uri, predicates, value);
}

int DataShareHelperImpl::BatchUpdate(const UpdateOperations &operations, std::vector<BatchUpdateResult> &results)
{
    auto extSpCtl = extSpCtl_;
    if (extSpCtl == nullptr) {
        LOG_ERROR("extSpCtl is nullptr");
        return INVALID_VALUE;
    }
    return extSpCtl->BatchUpdate(operations, results);
}

int DataShareHelperImpl::Delete(Uri &uri, const DataSharePredicates &predicates)
{
    DISTRIBUTED_DATA_HITRACE(std::string(LOG_TAG) + "::" + std::string(__FUNCTION__));
    auto generalCtl = generalCtl_;
    if (generalCtl == nullptr) {
        LOG_ERROR("generalCtl is nullptr");
        return INVALID_VALUE;
    }
    return generalCtl->Delete(uri, predicates);
}

std::shared_ptr<DataShareResultSet> DataShareHelperImpl::Query(Uri &uri, const DataSharePredicates &predicates,
    std::vector<std::string> &columns, DatashareBusinessError *businessError)
{
    DISTRIBUTED_DATA_HITRACE(std::string(LOG_TAG) + "::" + std::string(__FUNCTION__));
    auto generalCtl = generalCtl_;
    if (generalCtl == nullptr) {
        LOG_ERROR("generalCtl is nullptr");
        return nullptr;
    }
    DatashareBusinessError error;
    auto resultSet = generalCtl->Query(uri, predicates, columns, error);
    if (businessError != nullptr) {
        *businessError = error;
    }
    return resultSet;
}

std::string DataShareHelperImpl::GetType(Uri &uri)
{
    auto extSpCtl = extSpCtl_;
    if (extSpCtl == nullptr) {
        LOG_ERROR("extSpCtl is nullptr");
        return "";
    }
    return extSpCtl->GetType(uri);
}

int DataShareHelperImpl::BatchInsert(Uri &uri, const std::vector<DataShareValuesBucket> &values)
{
    DISTRIBUTED_DATA_HITRACE(std::string(LOG_TAG) + "::" + std::string(__FUNCTION__));
    auto extSpCtl = extSpCtl_;
    if (extSpCtl == nullptr) {
        LOG_ERROR("providerSepOperator is nullptr");
        return INVALID_VALUE;
    }
    return extSpCtl->BatchInsert(uri, values);
}

int DataShareHelperImpl::ExecuteBatch(const std::vector<OperationStatement> &statements, ExecResultSet &result)
{
    auto extSpCtl = extSpCtl_;
    if (extSpCtl == nullptr) {
        LOG_ERROR("extSpCtl is nullptr");
        return INVALID_VALUE;
    }
    return extSpCtl->ExecuteBatch(statements, result);
}

void DataShareHelperImpl::RegisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver)
{
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
    DISTRIBUTED_DATA_HITRACE(std::string(LOG_TAG) + "::" + std::string(__FUNCTION__));
    auto generalCtl = generalCtl_;
    if (generalCtl == nullptr) {
        LOG_ERROR("extSpCtl is nullptr");
        return;
    }
    return generalCtl->NotifyChange(uri);
}

Uri DataShareHelperImpl::NormalizeUri(Uri &uri)
{
    DISTRIBUTED_DATA_HITRACE(std::string(LOG_TAG) + "::" + std::string(__FUNCTION__));
    auto extSpCtl = extSpCtl_;
    if (extSpCtl == nullptr) {
        LOG_ERROR("extSpCtl is nullptr");
        return Uri("");
    }
    return extSpCtl->NormalizeUri(uri);
}

Uri DataShareHelperImpl::DenormalizeUri(Uri &uri)
{
    DISTRIBUTED_DATA_HITRACE(std::string(LOG_TAG) + "::" + std::string(__FUNCTION__));
    auto extSpCtl = extSpCtl_;
    if (extSpCtl == nullptr) {
        LOG_ERROR("extSpCtl is nullptr");
        return Uri("");
    }
    return extSpCtl->DenormalizeUri(uri);
}

int DataShareHelperImpl::AddQueryTemplate(const std::string &uri, int64_t subscriberId, Template &tpl)
{
    auto persistentDataCtl = persistentDataCtl_;
    if (persistentDataCtl == nullptr) {
        LOG_ERROR("persistentDataCtl is nullptr");
        return INVALID_VALUE;
    }
    return persistentDataCtl->AddQueryTemplate(uri, subscriberId, tpl);
}

int DataShareHelperImpl::DelQueryTemplate(const std::string &uri, int64_t subscriberId)
{
    auto persistentDataCtl = persistentDataCtl_;
    if (persistentDataCtl == nullptr) {
        LOG_ERROR("persistentDataCtl is nullptr");
        return INVALID_VALUE;
    }
    return persistentDataCtl->DelQueryTemplate(uri, subscriberId);
}

std::vector<OperationResult> DataShareHelperImpl::Publish(const Data &data, const std::string &bundleName)
{
    DISTRIBUTED_DATA_HITRACE(std::string(LOG_TAG) + "::" + std::string(__FUNCTION__));
    auto publishedDataCtl = publishedDataCtl_;
    if (publishedDataCtl == nullptr) {
        LOG_ERROR("publishedDataCtl is nullptr");
        return std::vector<OperationResult>();
    }
    return publishedDataCtl->Publish(data, bundleName);
}

Data DataShareHelperImpl::GetPublishedData(const std::string &bundleName, int &resultCode)
{
    DISTRIBUTED_DATA_HITRACE(std::string(LOG_TAG) + "::" + std::string(__FUNCTION__));
    auto publishedDataCtl = publishedDataCtl_;
    if (publishedDataCtl == nullptr) {
        LOG_ERROR("publishedDataCtl is nullptr");
        return Data();
    }
    return publishedDataCtl->GetPublishedData(bundleName, resultCode);
}

std::vector<OperationResult> DataShareHelperImpl::SubscribeRdbData(const std::vector<std::string> &uris,
    const TemplateId &templateId, const std::function<void(const RdbChangeNode &changeNode)> &callback)
{
    LOG_DEBUG("Start SubscribeRdbData");
    auto persistentDataCtl = persistentDataCtl_;
    if (persistentDataCtl == nullptr) {
        LOG_ERROR("persistentDataCtl is nullptr");
        return std::vector<OperationResult>();
    }
    return persistentDataCtl->SubscribeRdbData(this, uris, templateId, callback);
}

std::vector<OperationResult> DataShareHelperImpl::UnsubscribeRdbData(const std::vector<std::string> &uris,
    const TemplateId &templateId)
{
    LOG_DEBUG("Start UnsubscribeRdbData");
    auto persistentDataCtl = persistentDataCtl_;
    if (persistentDataCtl == nullptr) {
        LOG_ERROR("persistentDataCtl is nullptr");
        return std::vector<OperationResult>();
    }
    return persistentDataCtl->UnSubscribeRdbData(this, uris, templateId);
}

std::vector<OperationResult> DataShareHelperImpl::EnableRdbSubs(const std::vector<std::string> &uris,
    const TemplateId &templateId)
{
    LOG_DEBUG("Start EnableSubscribeRdbData");
    auto persistentDataCtl = persistentDataCtl_;
    if (persistentDataCtl == nullptr) {
        LOG_ERROR("persistentDataCtl is nullptr");
        return std::vector<OperationResult>();
    }
    return persistentDataCtl->EnableSubscribeRdbData(this, uris, templateId);
}

std::vector<OperationResult> DataShareHelperImpl::DisableRdbSubs(const std::vector<std::string> &uris,
    const TemplateId &templateId)
{
    LOG_DEBUG("Start DisableSubscribeRdbData");
    auto persistentDataCtl = persistentDataCtl_;
    if (persistentDataCtl == nullptr) {
        LOG_ERROR("persistentDataCtl is nullptr");
        return std::vector<OperationResult>();
    }
    return persistentDataCtl->DisableSubscribeRdbData(this, uris, templateId);
}

std::vector<OperationResult> DataShareHelperImpl::SubscribePublishedData(const std::vector<std::string> &uris,
    int64_t subscriberId, const std::function<void(const PublishedDataChangeNode &changeNode)> &callback)
{
    LOG_DEBUG("Start SubscribePublishedData");
    auto publishedDataCtl = publishedDataCtl_;
    if (publishedDataCtl == nullptr) {
        LOG_ERROR("publishedDataCtl is nullptr");
        return std::vector<OperationResult>();
    }
    return publishedDataCtl->SubscribePublishedData(this, uris, subscriberId, callback);
}

std::vector<OperationResult> DataShareHelperImpl::UnsubscribePublishedData(const std::vector<std::string> &uris,
    int64_t subscriberId)
{
    LOG_DEBUG("Start UnSubscribePublishedData");
    auto publishedDataCtl = publishedDataCtl_;
    if (publishedDataCtl == nullptr) {
        LOG_ERROR("publishedDataCtl is nullptr");
        return std::vector<OperationResult>();
    }
    return publishedDataCtl->UnSubscribePublishedData(this, uris, subscriberId);
}

std::vector<OperationResult> DataShareHelperImpl::EnablePubSubs(const std::vector<std::string> &uris,
    int64_t subscriberId)
{
    LOG_DEBUG("Start enablePubSubs");
    auto publishedDataCtl = publishedDataCtl_;
    if (publishedDataCtl == nullptr) {
        LOG_ERROR("publishedDataCtl is nullptr");
        return std::vector<OperationResult>();
    }
    return publishedDataCtl->EnableSubscribePublishedData(this, uris, subscriberId);
}

std::vector<OperationResult> DataShareHelperImpl::DisablePubSubs(const std::vector<std::string> &uris,
    int64_t subscriberId)
{
    LOG_DEBUG("Start disablePubSubs");
    auto publishedDataCtl = publishedDataCtl_;
    if (publishedDataCtl == nullptr) {
        LOG_ERROR("publishedDataCtl is nullptr");
        return std::vector<OperationResult>();
    }
    return publishedDataCtl->DisableSubscribePublishedData(this, uris, subscriberId);
}
} // namespace DataShare
} // namespace OHOS