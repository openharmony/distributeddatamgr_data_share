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

#define LOG_TAG "datashare_helper_impl"

#include <cinttypes>
#include "datashare_helper_impl.h"

#include "adaptor.h"
#include "dataobs_mgr_client.h"
#include "datashare_log.h"
#include "datashare_string_utils.h"
#include "datashare_radar_reporter.h"
#include "datashare_result_set.h"
#include "datashare_string_utils.h"
#include "general_controller_provider_impl.h"
#include "general_controller_service_impl.h"

namespace OHOS {
namespace DataShare {
using namespace AppExecFwk;
// non-silent access
DataShareHelperImpl::DataShareHelperImpl(const Uri &uri, const sptr<IRemoteObject> &token,
    std::shared_ptr<DataShareConnection> connection, bool isSystem)
{
    LOG_DEBUG("starts");
    isSystem_ = isSystem;
    generalCtl_ = std::make_shared<GeneralControllerProviderImpl>(connection, uri, token);
    extSpCtl_ = std::make_shared<ExtSpecialController>(connection, uri, token);
}

// silent access
DataShareHelperImpl::DataShareHelperImpl(std::string extUri, bool isSystem)
{
    LOG_DEBUG("starts");
    isSystem_ = isSystem;
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

int DataShareHelperImpl::OpenFileWithErrCode(Uri &uri, const std::string &mode, int32_t &errCode)
{
    auto extSpCtl = extSpCtl_;
    if (extSpCtl == nullptr) {
        LOG_ERROR("extSpCtl is nullptr");
        return DATA_SHARE_ERROR;
    }
    return extSpCtl->OpenFileWithErrCode(uri, mode, errCode);
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
    DataShareServiceProxy::SetSystem(isSystem_);
    auto res = generalCtl->Insert(uri, value);
    DataShareServiceProxy::CleanSystem();
    return res;
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
    DataShareServiceProxy::SetSystem(isSystem_);
    auto res = generalCtl->Update(uri, predicates, value);
    DataShareServiceProxy::CleanSystem();
    return res;
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
    DataShareServiceProxy::SetSystem(isSystem_);
    auto res = generalCtl->Delete(uri, predicates);
    DataShareServiceProxy::CleanSystem();
    return res;
}

std::pair<int32_t, int32_t> DataShareHelperImpl::InsertEx(Uri &uri, const DataShareValuesBucket &value)
{
    DISTRIBUTED_DATA_HITRACE(std::string(LOG_TAG) + "::" + std::string(__FUNCTION__));
    auto generalCtl = generalCtl_;
    if (generalCtl == nullptr) {
        LOG_ERROR("generalCtl_ is nullptr");
        return std::make_pair(DATA_SHARE_ERROR, 0);
    }
    DataShareServiceProxy::SetSystem(isSystem_);
    auto [errCode, status] = generalCtl->InsertEx(uri, value);
    DataShareServiceProxy::CleanSystem();
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
    DataShareServiceProxy::SetSystem(isSystem_);
    auto [errCode, status] = generalCtl->UpdateEx(uri, predicates, value);
    DataShareServiceProxy::CleanSystem();
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
    DataShareServiceProxy::SetSystem(isSystem_);
    auto [errCode, status] = generalCtl->DeleteEx(uri, predicates);
    DataShareServiceProxy::CleanSystem();
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
    DataShareOption option;
    DatashareBusinessError error;
    DataShareServiceProxy::SetSystem(isSystem_);
    auto resultSet = generalCtl->Query(uri, predicates, columns, error, option);
    DataShareServiceProxy::CleanSystem();
    if (businessError != nullptr) {
        *businessError = error;
    }
    return resultSet;
}

std::shared_ptr<DataShareResultSet> DataShareHelperImpl::Query(Uri &uri, const DataSharePredicates &predicates,
    std::vector<std::string> &columns, DataShareOption &option, DatashareBusinessError *businessError)
{
    DISTRIBUTED_DATA_HITRACE(std::string(LOG_TAG) + "::" + std::string(__FUNCTION__));
    auto generalCtl = generalCtl_;
    if (generalCtl == nullptr) {
        LOG_ERROR("generalCtl is nullptr");
        return nullptr;
    }
    DatashareBusinessError error;
    DataShareServiceProxy::SetSystem(isSystem_);
    auto resultSet = generalCtl->Query(uri, predicates, columns, error, option);
    DataShareServiceProxy::CleanSystem();
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
    DataShareServiceProxy::SetSystem(isSystem_);
    int ret = generalCtl->RegisterObserver(uri, dataObserver);
    DataShareServiceProxy::CleanSystem();
    return ret;
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
    DataShareServiceProxy::SetSystem(isSystem_);
    int ret = generalCtl->UnregisterObserver(uri, dataObserver);
    DataShareServiceProxy::CleanSystem();
    return ret;
}

void DataShareHelperImpl::NotifyChange(const Uri &uri)
{
    DISTRIBUTED_DATA_HITRACE(std::string(LOG_TAG) + "::" + std::string(__FUNCTION__));
    auto generalCtl = generalCtl_;
    if (generalCtl == nullptr) {
        LOG_ERROR("extSpCtl is nullptr");
        return;
    }
    DataShareServiceProxy::SetSystem(isSystem_);
    generalCtl->NotifyChange(uri);
    DataShareServiceProxy::CleanSystem();
    return;
}

/**
 * Registers an observer to DataObsMgr specified by the given Uri. Only non-silent is supported, and there is no
 * default implemention for the provider. It needs to be handled by the user.
 *
 * @param uri, Indicates the path of the data to operate.
 * @param dataObserver, Indicates the DataShareObserver object.
 * @param isDescendants, Indicates the Whether to note the change of descendants.
 */
int DataShareHelperImpl::RegisterObserverExtProvider(const Uri &uri, std::shared_ptr<DataShareObserver> dataObserver,
    bool isDescendants)
{
    if (dataObserver == nullptr) {
        LOG_ERROR("dataObserver is nullptr");
        return E_NULL_OBSERVER;
    }
    sptr<ObserverImpl> obs = ObserverImpl::GetObserver(uri, dataObserver);
    if (obs == nullptr) {
        LOG_ERROR("new ObserverImpl failed");
        return E_NULL_OBSERVER;
    }
    auto generalCtl = generalCtl_;
    if (generalCtl == nullptr) {
        LOG_ERROR("generalCtl is nullptr");
        return E_HELPER_DIED;
    }
    DataShareServiceProxy::SetSystem(isSystem_);
    // only support non-silent access
    ErrCode ret = generalCtl->RegisterObserverExtProvider(uri, obs, isDescendants);
    DataShareServiceProxy::CleanSystem();
    LOG_INFO("Register observerExt, ret:%{public}d, uri:%{public}s",
        ret, DataShareStringUtils::Anonymous(uri.ToString()).c_str());
    if (ret != E_OK) {
        ObserverImpl::DeleteObserver(uri, dataObserver);
    }
    return ret;
}

/**
 * Deregisters an observer used for DataObsMgr specified by the given Uri. Only non-silent is supported, and there is
 * no default implemention for the provider. It needs to be handled by the user.
 *
 * @param uri, Indicates the path of the data to operate.
 * @param dataObserver, Indicates the DataShareObserver object.
 */
int DataShareHelperImpl::UnregisterObserverExtProvider(const Uri &uri, std::shared_ptr<DataShareObserver> dataObserver)
{
    if (dataObserver == nullptr) {
        LOG_ERROR("dataObserver is nullptr");
        return E_NULL_OBSERVER;
    }
    if (!ObserverImpl::FindObserver(uri, dataObserver)) {
        LOG_ERROR("observer not exit!");
        return E_NULL_OBSERVER;
    }
    sptr<ObserverImpl> obs = ObserverImpl::GetObserver(uri, dataObserver);
    if (obs == nullptr) {
        LOG_ERROR("new ObserverImpl failed");
        return E_NULL_OBSERVER;
    }
    auto generalCtl = generalCtl_;
    if (generalCtl == nullptr) {
        LOG_ERROR("generalCtl is nullptr");
        return E_HELPER_DIED;
    }
    DataShareServiceProxy::SetSystem(isSystem_);
    // only support non-silent access
    ErrCode ret = generalCtl->UnregisterObserverExtProvider(uri, obs);
    DataShareServiceProxy::CleanSystem();
    if (ret != E_OK) {
        return ret;
    }
    ObserverImpl::DeleteObserver(uri, dataObserver);
    return E_OK;
}

/**
 * Notifies the registered observers of a change to the data resource specified by Uris. Only non-silent is supported,
 * and there is no default implemention for the provider. It needs to be handled by the user.
 *
 * @param changeInfo Indicates the info of the data to operate.
 */
void DataShareHelperImpl::NotifyChangeExtProvider(const DataShareObserver::ChangeInfo &changeInfo)
{
    auto generalCtl = generalCtl_;
    if (generalCtl == nullptr) {
        LOG_ERROR("extSpCtl is nullptr");
        return;
    }
    DataShareServiceProxy::SetSystem(isSystem_);
    // only support non-silent access
    ErrCode ret = generalCtl->NotifyChangeExtProvider(ObserverImpl::ConvertInfo(changeInfo));
    DataShareServiceProxy::CleanSystem();
    LOG_INFO("Notify changeExt, ret:%{public}d", ret);
    return;
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
    DataShareServiceProxy::SetSystem(isSystem_);
    auto res = persistentDataCtl->AddQueryTemplate(uri, subscriberId, tpl);
    DataShareServiceProxy::CleanSystem();
    return res;
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
    DataShareServiceProxy::SetSystem(isSystem_);
    auto res = persistentDataCtl->DelQueryTemplate(uri, subscriberId);
    DataShareServiceProxy::CleanSystem();
    return res;
}

std::vector<OperationResult> DataShareHelperImpl::Publish(const Data &data, const std::string &bundleName)
{
    DISTRIBUTED_DATA_HITRACE(std::string(LOG_TAG) + "::" + std::string(__FUNCTION__));
    auto publishedDataCtl = publishedDataCtl_;
    if (publishedDataCtl == nullptr) {
        LOG_ERROR("publishedDataCtl is nullptr");
        return std::vector<OperationResult>();
    }
    DataShareServiceProxy::SetSystem(isSystem_);
    auto res = publishedDataCtl->Publish(data, bundleName);
    DataShareServiceProxy::CleanSystem();
    return res;
}

Data DataShareHelperImpl::GetPublishedData(const std::string &bundleName, int &resultCode)
{
    DISTRIBUTED_DATA_HITRACE(std::string(LOG_TAG) + "::" + std::string(__FUNCTION__));
    auto publishedDataCtl = publishedDataCtl_;
    if (publishedDataCtl == nullptr) {
        LOG_ERROR("publishedDataCtl is nullptr");
        return Data();
    }
    DataShareServiceProxy::SetSystem(isSystem_);
    auto res = publishedDataCtl->GetPublishedData(bundleName, resultCode);
    DataShareServiceProxy::CleanSystem();
    return res;
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
    DataShareServiceProxy::SetSystem(isSystem_);
    auto res = persistentDataCtl->SubscribeRdbData(this, uris, templateId, callback);
    DataShareServiceProxy::CleanSystem();
    return res;
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
    DataShareServiceProxy::SetSystem(isSystem_);
    auto res = persistentDataCtl->UnSubscribeRdbData(this, uris, templateId);
    DataShareServiceProxy::CleanSystem();
    return res;
}

std::vector<OperationResult> DataShareHelperImpl::EnableRdbSubs(const std::vector<std::string> &uris,
    const TemplateId &templateId)
{
    LOG_DEBUG("Start EnableSubscribeRdbData");
    std::string uriAll = "";
    for (auto uri : uris) {
        uriAll += (DataShareStringUtils::Anonymous(uri) + ",");
    }
    LOG_INFO("uri is %{public}s bundleName is %{public}s, subscriberId is %{public}" PRId64 "",
        DataShareStringUtils::Anonymous(uriAll).c_str(), templateId.bundleName_.c_str(), templateId.subscriberId_);
    auto persistentDataCtl = persistentDataCtl_;
    if (persistentDataCtl == nullptr) {
        LOG_ERROR("persistentDataCtl is nullptr");
        return std::vector<OperationResult>();
    }
    DataShareServiceProxy::SetSystem(isSystem_);
    auto res = persistentDataCtl->EnableSubscribeRdbData(this, uris, templateId);
    DataShareServiceProxy::CleanSystem();
    return res;
}

std::vector<OperationResult> DataShareHelperImpl::DisableRdbSubs(const std::vector<std::string> &uris,
    const TemplateId &templateId)
{
    LOG_DEBUG("Start DisableSubscribeRdbData");
    std::string uriAll = "";
    for (auto uri : uris) {
        uriAll += (DataShareStringUtils::Anonymous(uri) + ",");
    }
    LOG_INFO("uri is %{public}s bundleName is %{public}s, subscriberId is %{public}" PRId64 "",
        DataShareStringUtils::Anonymous(uriAll).c_str(), templateId.bundleName_.c_str(), templateId.subscriberId_);
    auto persistentDataCtl = persistentDataCtl_;
    if (persistentDataCtl == nullptr) {
        LOG_ERROR("persistentDataCtl is nullptr");
        return std::vector<OperationResult>();
    }
    DataShareServiceProxy::SetSystem(isSystem_);
    auto res = persistentDataCtl->DisableSubscribeRdbData(this, uris, templateId);
    DataShareServiceProxy::CleanSystem();
    return res;
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
    DataShareServiceProxy::SetSystem(isSystem_);
    auto res = publishedDataCtl->SubscribePublishedData(this, uris, subscriberId, callback);
    DataShareServiceProxy::CleanSystem();
    return res;
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
    DataShareServiceProxy::SetSystem(isSystem_);
    auto res = publishedDataCtl->UnSubscribePublishedData(this, uris, subscriberId);
    DataShareServiceProxy::CleanSystem();
    return res;
}

std::vector<OperationResult> DataShareHelperImpl::EnablePubSubs(const std::vector<std::string> &uris,
    int64_t subscriberId)
{
    LOG_DEBUG("Start enablePubSubs");
    std::string uriAll = "";
    for (auto uri : uris) {
        uriAll += (DataShareStringUtils::Anonymous(uri) + ",");
    }
    LOG_INFO("uri is %{public}s subscriberId is %{public}" PRId64 "",
        DataShareStringUtils::Anonymous(uriAll).c_str(), subscriberId);
    auto publishedDataCtl = publishedDataCtl_;
    if (publishedDataCtl == nullptr) {
        LOG_ERROR("publishedDataCtl is nullptr");
        return std::vector<OperationResult>();
    }
    DataShareServiceProxy::SetSystem(isSystem_);
    auto res = publishedDataCtl->EnableSubscribePublishedData(this, uris, subscriberId);
    DataShareServiceProxy::CleanSystem();
    return res;
}

std::vector<OperationResult> DataShareHelperImpl::DisablePubSubs(const std::vector<std::string> &uris,
    int64_t subscriberId)
{
    LOG_DEBUG("Start disablePubSubs");
    std::string uriAll = "";
    for (auto uri : uris) {
        uriAll += (DataShareStringUtils::Anonymous(uri) + ",");
    }
    LOG_INFO("uri is %{public}s subscriberId is %{public}" PRId64 "",
        DataShareStringUtils::Anonymous(uriAll).c_str(), subscriberId);
    auto publishedDataCtl = publishedDataCtl_;
    if (publishedDataCtl == nullptr) {
        LOG_ERROR("publishedDataCtl is nullptr");
        return std::vector<OperationResult>();
    }
    DataShareServiceProxy::SetSystem(isSystem_);
    auto res = publishedDataCtl->DisableSubscribePublishedData(this, uris, subscriberId);
    DataShareServiceProxy::CleanSystem();
    return res;
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

/**
 * Registers an observer to DataObsMgr specified by the given Uri, then return error code.
 *
 * @param uri, Indicates the path of the data to operate.
 * @param dataObserver, Indicates the DataShareObserver object.
 * @param isDescendants, Indicates the Whether to note the change of descendants.
 * @param isSystem, Indicates the app is system app or not.
 *
 * @return Returns the result. Error codes are listed in DataShare datashare_errno.h and
 * DataObs dataobs_mgr_errors.h.
 */
int DataShareHelperImpl::TryRegisterObserverExt(const Uri &uri, std::shared_ptr<DataShareObserver> dataObserver,
    bool isDescendants, bool isSystem)
{
    return TryRegisterObserverExtInner(uri, dataObserver, isDescendants, isSystem);
}

/**
 * Deregisters an observer used for DataObsMgr specified by the given Uri, then return error code.
 *
 * @param uri, Indicates the path of the data to operate.
 * @param dataObserver, Indicates the DataShareObserver object.
 * @param isSystem, Indicates the app is system app or not.
 *
 * @return Returns the result. Error codes are listed in DataShare datashare_errno.h and
 * DataObs dataobs_mgr_errors.h.
 */
int DataShareHelperImpl::TryUnregisterObserverExt(const Uri &uri, std::shared_ptr<DataShareObserver> dataObserver,
    bool isSystem)
{
    return TryUnregisterObserverExtInner(uri, dataObserver, isSystem);
}

/**
 * Registers an observer to DataObsMgr specified by the given Uri, then return error code,
 * here is the internal implementation.
 *
 * @param uri, Indicates the path of the data to operate.
 * @param dataObserver, Indicates the DataShareObserver object.
 * @param isDescendants, Indicates the Whether to note the change of descendants.
 * @param isSystem, Indicates the app is system app or not.
 *
 * @return Returns the result. Error codes are listed in DataShare datashare_errno.h and
 * DataObs dataobs_mgr_errors.h.
 */
int TryRegisterObserverExtInner(const Uri &uri, std::shared_ptr<DataShareObserver> dataObserver,
    bool isDescendants, bool isSystem)
{
    if (dataObserver == nullptr) {
        LOG_ERROR("dataObserver is nullptr");
        return E_NULL_OBSERVER;
    }

    auto obsMgrClient = OHOS::AAFwk::DataObsMgrClient::GetInstance();
    if (obsMgrClient == nullptr) {
        LOG_ERROR("get DataObsMgrClient failed");
        return E_NULL_OBSERVER_CLIENT;
    }

    sptr<ObserverImpl> obs = ObserverImpl::GetObserver(uri, dataObserver);
    if (obs == nullptr) {
        LOG_ERROR("new ObserverImpl failed");
        return E_NULL_OBSERVER;
    }

    ErrCode ret = obsMgrClient->RegisterObserverExt(uri, obs, isDescendants, AAFwk::DataObsOption(isSystem, true));
    if (ret != ERR_OK) {
        ObserverImpl::DeleteObserver(uri, dataObserver);
    }

    LOG_INFO("Register observerExt with error, ret:%{public}d, uri:%{public}s",
        ret, DataShareStringUtils::Anonymous(uri.ToString()).c_str());
    return ret;
}

/**
 * Deregisters an observer used for DataObsMgr specified by the given Uri, then return error code,
 * here is the internal implementation.
 *
 * @param uri, Indicates the path of the data to operate.
 * @param dataObserver, Indicates the DataShareObserver object.
 * @param isSystem, Indicates the app is system app or not.
 *
 * @return Returns the result. Error codes are listed in DataShare datashare_errno.h and
 * DataObs dataobs_mgr_errors.h.
 */
int TryUnregisterObserverExtInner(const Uri &uri, std::shared_ptr<DataShareObserver> dataObserver,
    bool isSystem)
{
    if (dataObserver == nullptr) {
        LOG_ERROR("dataObserver is nullptr");
        return E_NULL_OBSERVER;
    }

    auto obsMgrClient = OHOS::AAFwk::DataObsMgrClient::GetInstance();
    if (obsMgrClient == nullptr) {
        LOG_ERROR("get DataObsMgrClient failed");
        return E_NULL_OBSERVER_CLIENT;
    }

    if (!ObserverImpl::FindObserver(uri, dataObserver)) {
        LOG_ERROR("observer not exit!");
        return E_NULL_OBSERVER;
    }

    sptr<ObserverImpl> obs = ObserverImpl::GetObserver(uri, dataObserver);
    if (obs == nullptr) {
        LOG_ERROR("new ObserverImpl failed");
        return E_NULL_OBSERVER;
    }

    ErrCode ret = obsMgrClient->UnregisterObserverExt(uri, obs, AAFwk::DataObsOption(isSystem, true));
    LOG_INFO("Unregister observerExt, ret:%{public}d, uri:%{public}s",
        ret, DataShareStringUtils::Anonymous(uri.ToString()).c_str());
    if (ret != ERR_OK) {
        return ret;
    }
    ObserverImpl::DeleteObserver(uri, dataObserver);
    return ret;
}
} // namespace DataShare
} // namespace OHOS