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
#include "datashare_radar_reporter.h"
#include "datashare_result_set.h"

#include "general_controller_provider_impl.h"
#include "general_controller_service_impl.h"

namespace OHOS {
namespace DataShare {
using namespace AppExecFwk;
DataShareHelperImpl::DataShareHelperImpl(const Uri &uri, const sptr<IRemoteObject> &token,
    std::shared_ptr<DataShareConnection> connection)
{
    LOG_DEBUG("starts");
    generalCtl_ = std::make_shared<GeneralControllerProviderImpl>(connection, uri, token);
    extSpCtl_ = std::make_shared<ExtSpecialController>(connection, uri, token);
}

DataShareHelperImpl::DataShareHelperImpl(std::string extUri)
{
    LOG_DEBUG("starts");
    generalCtl_ = std::make_shared<GeneralControllerServiceImpl>(extUri);
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
        return DATA_SHARE_ERROR;
    }
    return extSpCtl->OpenFile(uri, mode);
}

int DataShareHelperImpl::OpenRawFile(Uri &uri, const std::string &mode)
{
    auto extSpCtl = extSpCtl_;
    if (extSpCtl == nullptr) {
        LOG_ERROR("extSpCtl is nullptr");
        return DATA_SHARE_ERROR;
    }
    return extSpCtl->OpenRawFile(uri, mode);
}

int DataShareHelperImpl::Insert(Uri &uri, const DataShareValuesBucket &value)
{
    DISTRIBUTED_DATA_HITRACE(std::string(LOG_TAG) + "::" + std::string(__FUNCTION__));
    auto generalCtl = generalCtl_;
    if (generalCtl == nullptr) {
        LOG_ERROR("generalCtl_ is nullptr");
        return DATA_SHARE_ERROR;
    }
    return generalCtl->Insert(uri, value);
}

int DataShareHelperImpl::InsertExt(Uri &uri, const DataShareValuesBucket &value, std::string &result)
{
    DISTRIBUTED_DATA_HITRACE(std::string(LOG_TAG) + "::" + std::string(__FUNCTION__));
    auto extSpCtl = extSpCtl_;
    if (extSpCtl == nullptr) {
        LOG_ERROR("providerSpCtl is nullptr");
        return DATA_SHARE_ERROR;
    }
    return extSpCtl->InsertExt(uri, value, result);
}

int DataShareHelperImpl::Update(Uri &uri, const DataSharePredicates &predicates, const DataShareValuesBucket &value)
{
    DISTRIBUTED_DATA_HITRACE(std::string(LOG_TAG) + "::" + std::string(__FUNCTION__));
    auto generalCtl = generalCtl_;
    if (generalCtl == nullptr) {
        LOG_ERROR("generalCtl is nullptr");
        return DATA_SHARE_ERROR;
    }
    return generalCtl->Update(uri, predicates, value);
}

int DataShareHelperImpl::BatchUpdate(const UpdateOperations &operations, std::vector<BatchUpdateResult> &results)
{
    auto extSpCtl = extSpCtl_;
    if (extSpCtl == nullptr) {
        LOG_ERROR("extSpCtl is nullptr");
        return DATA_SHARE_ERROR;
    }
    return extSpCtl->BatchUpdate(operations, results);
}

int DataShareHelperImpl::Delete(Uri &uri, const DataSharePredicates &predicates)
{
    DISTRIBUTED_DATA_HITRACE(std::string(LOG_TAG) + "::" + std::string(__FUNCTION__));
    auto generalCtl = generalCtl_;
    if (generalCtl == nullptr) {
        LOG_ERROR("generalCtl is nullptr");
        return DATA_SHARE_ERROR;
    }
    return generalCtl->Delete(uri, predicates);
}

std::pair<int32_t, int32_t> DataShareHelperImpl::InsertEx(Uri &uri, const DataShareValuesBucket &value)
{
    DISTRIBUTED_DATA_HITRACE(std::string(LOG_TAG) + "::" + std::string(__FUNCTION__));
    auto generalCtl = generalCtl_;
    if (generalCtl == nullptr) {
        LOG_ERROR("generalCtl_ is nullptr");
        return std::make_pair(DATA_SHARE_ERROR, 0);
    }
    auto [errCode, status] = generalCtl->InsertEx(uri, value);
    if (errCode != E_OK) {
        LOG_ERROR("generalCtl insert failed, errCode = %{public}d", errCode);
    }
    return std::make_pair(errCode, status);
}

std::pair<int32_t, int32_t> DataShareHelperImpl::UpdateEx(
    Uri &uri, const DataSharePredicates &predicates, const DataShareValuesBucket &value)
{
    DISTRIBUTED_DATA_HITRACE(std::string(LOG_TAG) + "::" + std::string(__FUNCTION__));
    auto generalCtl = generalCtl_;
    if (generalCtl == nullptr) {
        LOG_ERROR("generalCtl is nullptr");
        return std::make_pair(DATA_SHARE_ERROR, 0);
    }
    auto [errCode, status] = generalCtl->UpdateEx(uri, predicates, value);
    if (errCode != E_OK) {
        LOG_ERROR("generalCtl update failed, errCode = %{public}d", errCode);
    }
    return std::make_pair(errCode, status);
}

std::pair<int32_t, int32_t> DataShareHelperImpl::DeleteEx(Uri &uri, const DataSharePredicates &predicates)
{
    DISTRIBUTED_DATA_HITRACE(std::string(LOG_TAG) + "::" + std::string(__FUNCTION__));
    auto generalCtl = generalCtl_;
    if (generalCtl == nullptr) {
        LOG_ERROR("generalCtl is nullptr");
        return std::make_pair(DATA_SHARE_ERROR, 0);
    }
    auto [errCode, status] = generalCtl->DeleteEx(uri, predicates);
    if (errCode != E_OK) {
        LOG_ERROR("generalCtl delete failed, errCode = %{public}d", errCode);
    }
    return std::make_pair(errCode, status);
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
        return DATA_SHARE_ERROR;
    }
    return extSpCtl->BatchInsert(uri, values);
}

int DataShareHelperImpl::ExecuteBatch(const std::vector<OperationStatement> &statements, ExecResultSet &result)
{
    auto extSpCtl = extSpCtl_;
    if (extSpCtl == nullptr) {
        LOG_ERROR("extSpCtl is nullptr");
        return DATA_SHARE_ERROR;
    }
    return extSpCtl->ExecuteBatch(statements, result);
}

int DataShareHelperImpl::RegisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver)
{
    RadarReporter::RadarReport report(RadarReporter::OBSERVER_MANAGER,
        RadarReporter::REGISTER_OBSERVER, __FUNCTION__);
    if (dataObserver == nullptr) {
        LOG_ERROR("dataObserver is nullptr");
        report.SetError(RadarReporter::EMPTY_OBSERVER_ERROR);
        return E_NULL_OBSERVER;
    }
    auto generalCtl = generalCtl_;
    if (generalCtl == nullptr) {
        LOG_ERROR("generalCtl is nullptr");
        report.SetError(RadarReporter::DATA_SHARE_DIED_ERROR);
        return E_HELPER_DIED;
    }
    return generalCtl->RegisterObserver(uri, dataObserver);
}

int DataShareHelperImpl::UnregisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver)
{
    RadarReporter::RadarReport report(RadarReporter::OBSERVER_MANAGER,
        RadarReporter::UNREGISTER_OBSERVER, __FUNCTION__);
    if (dataObserver == nullptr) {
        LOG_ERROR("dataObserver is nullptr");
        report.SetError(RadarReporter::EMPTY_OBSERVER_ERROR);
        return E_NULL_OBSERVER;
    }
    auto generalCtl = generalCtl_;
    if (generalCtl == nullptr) {
        LOG_ERROR("generalCtl is nullptr");
        report.SetError(RadarReporter::DATA_SHARE_DIED_ERROR);
        return E_HELPER_DIED;
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
    RadarReporter::RadarReport report(RadarReporter::TEMPLATE_DATA_MANAGER,
        RadarReporter::ADD_TEMPLATE, __FUNCTION__);
    auto persistentDataCtl = persistentDataCtl_;
    if (persistentDataCtl == nullptr) {
        LOG_ERROR("persistentDataCtl is nullptr");
        report.SetError(RadarReporter::DATA_SHARE_DIED_ERROR);
        return DATA_SHARE_ERROR;
    }
    return persistentDataCtl->AddQueryTemplate(uri, subscriberId, tpl);
}

int DataShareHelperImpl::DelQueryTemplate(const std::string &uri, int64_t subscriberId)
{
    RadarReporter::RadarReport report(RadarReporter::TEMPLATE_DATA_MANAGER,
        RadarReporter::DELETE_TEMPLATE, __FUNCTION__);
    auto persistentDataCtl = persistentDataCtl_;
    if (persistentDataCtl == nullptr) {
        LOG_ERROR("persistentDataCtl is nullptr");
        report.SetError(RadarReporter::DATA_SHARE_DIED_ERROR);
        return DATA_SHARE_ERROR;
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
    RadarReporter::RadarReport report(RadarReporter::TEMPLATE_DATA_MANAGER,
        RadarReporter::SUBSCRIBE_RDB_DATA, __FUNCTION__);
    auto persistentDataCtl = persistentDataCtl_;
    if (persistentDataCtl == nullptr) {
        LOG_ERROR("persistentDataCtl is nullptr");
        report.SetError(RadarReporter::DATA_SHARE_DIED_ERROR);
        return std::vector<OperationResult>();
    }
    return persistentDataCtl->SubscribeRdbData(this, uris, templateId, callback);
}

__attribute__((no_sanitize("cfi"))) std::vector<OperationResult> DataShareHelperImpl::UnsubscribeRdbData(
    const std::vector<std::string> &uris, const TemplateId &templateId)
{
    LOG_DEBUG("Start UnsubscribeRdbData");
    RadarReporter::RadarReport report(RadarReporter::TEMPLATE_DATA_MANAGER,
        RadarReporter::UNSUBSCRIBE_RDB_DATA, __FUNCTION__);
    auto persistentDataCtl = persistentDataCtl_;
    if (persistentDataCtl == nullptr) {
        LOG_ERROR("persistentDataCtl is nullptr");
        report.SetError(RadarReporter::DATA_SHARE_DIED_ERROR);
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
    RadarReporter::RadarReport report(RadarReporter::TEMPLATE_DATA_MANAGER,
        RadarReporter::SUBSCRIBE_PUBLISHED_DATA, __FUNCTION__);
    auto publishedDataCtl = publishedDataCtl_;
    if (publishedDataCtl == nullptr) {
        LOG_ERROR("publishedDataCtl is nullptr");
        report.SetError(RadarReporter::DATA_SHARE_DIED_ERROR);
        return std::vector<OperationResult>();
    }
    return publishedDataCtl->SubscribePublishedData(this, uris, subscriberId, callback);
}

std::vector<OperationResult> DataShareHelperImpl::UnsubscribePublishedData(const std::vector<std::string> &uris,
    int64_t subscriberId)
{
    LOG_DEBUG("Start UnSubscribePublishedData");
    RadarReporter::RadarReport report(RadarReporter::TEMPLATE_DATA_MANAGER,
        RadarReporter::UNSUBSCRIBE_PUBLISHED_DATA, __FUNCTION__);
    auto publishedDataCtl = publishedDataCtl_;
    if (publishedDataCtl == nullptr) {
        LOG_ERROR("publishedDataCtl is nullptr");
        report.SetError(RadarReporter::DATA_SHARE_DIED_ERROR);
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

int32_t DataShareHelperImpl::UserDefineFunc(
    MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    DISTRIBUTED_DATA_HITRACE(std::string(LOG_TAG) + "::" + std::string(__FUNCTION__));
    auto extSpCtl = extSpCtl_;
    if (extSpCtl == nullptr) {
        LOG_ERROR("providerSpCtl is nullptr");
        return DATA_SHARE_ERROR;
    }
    return extSpCtl->UserDefineFunc(data, reply, option);
}
} // namespace DataShare
} // namespace OHOS