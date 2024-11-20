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

#include "data_share_service_proxy.h"

#include <cinttypes>
#include "data_ability_observer_interface.h"
#include "datashare_itypes_utils.h"
#include "datashare_log.h"
#include "datashare_string_utils.h"
#include "ishared_result_set.h"

namespace OHOS {
namespace DataShare {
using InterfaceCode = OHOS::DistributedShare::DataShare::DataShareServiceInterfaceCode;
DataShareServiceProxy::DataShareServiceProxy(const sptr<IRemoteObject> &object)
    : IRemoteProxy<IDataShareService>(object)
{
}

int32_t DataShareServiceProxy::Insert(const Uri &uri, const Uri &extUri, const DataShareValuesBucket &value)
{
    auto [errCode, status] = InsertEx(uri, extUri, value);
    if (errCode == NO_ERROR) {
        return status;
    } else if (errCode < NO_ERROR) {
        return errCode;
    }
    LOG_ERROR("DataShareServiceProxy insert failed, errCode = %{public}d", errCode);
    return DATA_SHARE_ERROR;
}

int32_t DataShareServiceProxy::Update(const Uri &uri, const Uri &extUri,
    const DataSharePredicates &predicate, const DataShareValuesBucket &valuesBucket)
{
    auto [errCode, status] = UpdateEx(uri, extUri, predicate, valuesBucket);
    if (errCode == NO_ERROR) {
        return status;
    } else if (errCode < NO_ERROR) {
        return errCode;
    }
    LOG_ERROR("DataShareServiceProxy update failed, errCode = %{public}d", errCode);
    return DATA_SHARE_ERROR;
}

int32_t DataShareServiceProxy::Delete(const Uri &uri, const Uri &extUri, const DataSharePredicates &predicate)
{
    auto [errCode, status] = DeleteEx(uri, extUri, predicate);
    if (errCode == NO_ERROR) {
        return status;
    } else if (errCode < NO_ERROR) {
        return errCode;
    }
    LOG_ERROR("DataShareServiceProxy delete failed, errCode = %{public}d", errCode);
    return DATA_SHARE_ERROR;
}

std::pair<int32_t, int32_t> DataShareServiceProxy::InsertEx(const Uri &uri, const Uri &extUri,
    const DataShareValuesBucket &value)
{
    const std::string &uriStr = uri.ToString();
    MessageParcel data;
    if (!data.WriteInterfaceToken(IDataShareService::GetDescriptor())) {
        LOG_ERROR("Write descriptor failed!");
        return std::make_pair(E_WRITE_TO_PARCE_ERROR, 0);
    }
    if (!ITypesUtil::Marshal(data, uriStr, extUri.ToString(), value)) {
        LOG_ERROR("Write to message parcel failed!");
        return std::make_pair(E_MARSHAL_ERROR, 0);
    }

    int32_t result = -1;
    int32_t errCode = -1;
    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(
        static_cast<uint32_t>(InterfaceCode::DATA_SHARE_SERVICE_CMD_INSERTEX), data, reply, option);
    if (err != NO_ERROR) {
        LOG_ERROR("InsertEx fail to sendRequest. uri: %{public}s, err: %{public}d",
            DataShareStringUtils::Anonymous(uriStr).c_str(), err);
        return std::make_pair(DATA_SHARE_ERROR, 0);
    }
    if (!ITypesUtil::Unmarshal(reply, errCode, result)) {
        LOG_ERROR("fail to Unmarshal");
        return std::make_pair(E_UNMARSHAL_ERROR, 0);
    }
    return std::make_pair(errCode, result);
}

std::pair<int32_t, int32_t> DataShareServiceProxy::UpdateEx(const Uri &uri, const Uri &extUri,
    const DataSharePredicates &predicate, const DataShareValuesBucket &valuesBucket)
{
    const std::string &uriStr = uri.ToString();
    MessageParcel data;
    if (!data.WriteInterfaceToken(IDataShareService::GetDescriptor())) {
        LOG_ERROR("Write descriptor failed!");
        return std::make_pair(E_WRITE_TO_PARCE_ERROR, 0);
    }
    if (!ITypesUtil::Marshal(data, uriStr, extUri.ToString(), predicate, valuesBucket)) {
        LOG_ERROR("Write to message parcel failed!");
        return std::make_pair(E_MARSHAL_ERROR, 0);
    }

    int32_t result = -1;
    int32_t errCode = -1;
    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(
        static_cast<uint32_t>(InterfaceCode::DATA_SHARE_SERVICE_CMD_UPDATEEX), data, reply, option);
    if (err != NO_ERROR) {
        LOG_ERROR("UpdateEx fail to sendRequest. uri: %{public}s, err: %{public}d",
            DataShareStringUtils::Anonymous(uriStr).c_str(), err);
        return std::make_pair(DATA_SHARE_ERROR, 0);
    }
    if (!ITypesUtil::Unmarshal(reply, errCode, result)) {
        LOG_ERROR("fail to Unmarshal");
        return std::make_pair(E_UNMARSHAL_ERROR, 0);
    }
    return std::make_pair(errCode, result);
}

std::pair<int32_t, int32_t> DataShareServiceProxy::DeleteEx(const Uri &uri, const Uri &extUri,
    const DataSharePredicates &predicate)
{
    const std::string &uriStr = uri.ToString();
    MessageParcel data;
    if (!data.WriteInterfaceToken(IDataShareService::GetDescriptor())) {
        LOG_ERROR("Write descriptor failed!");
        return std::make_pair(E_WRITE_TO_PARCE_ERROR, 0);
    }
    if (!ITypesUtil::Marshal(data, uriStr, extUri.ToString(), predicate)) {
        LOG_ERROR("Write to message parcel failed!");
        return std::make_pair(E_MARSHAL_ERROR, 0);
    }

    int32_t result = -1;
    int32_t errCode = -1;
    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(
        static_cast<uint32_t>(InterfaceCode::DATA_SHARE_SERVICE_CMD_DELETEEX), data, reply, option);
    if (err != NO_ERROR) {
        LOG_ERROR("DeleteEx fail to sendRequest. uri: %{public}s, err: %{public}d",
            DataShareStringUtils::Anonymous(uriStr).c_str(), err);
        return std::make_pair(DATA_SHARE_ERROR, 0);
    }
    if (!ITypesUtil::Unmarshal(reply, errCode, result)) {
        LOG_ERROR("fail to Unmarshal");
        return std::make_pair(E_UNMARSHAL_ERROR, 0);
    }
    return std::make_pair(errCode, result);
}

std::shared_ptr<DataShareResultSet> DataShareServiceProxy::Query(const Uri &uri, const Uri &extUri,
    const DataSharePredicates &predicates, std::vector<std::string> &columns, DatashareBusinessError &businessError)
{
    const std::string &uriStr = uri.ToString();
    MessageParcel data;
    if (!data.WriteInterfaceToken(IDataShareService::GetDescriptor())) {
        LOG_ERROR("WriteInterfaceToken failed!");
        return nullptr;
    }

    if (!ITypesUtil::Marshal(data, uriStr, extUri.ToString(), predicates, columns)) {
        LOG_ERROR("Write to message parcel failed!");
        return nullptr;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(
        static_cast<uint32_t>(InterfaceCode::DATA_SHARE_SERVICE_CMD_QUERY), data, reply, option);
    
    auto result = ISharedResultSet::ReadFromParcel(reply);
    businessError.SetCode(reply.ReadInt32());
    if (err != NO_ERROR) {
        LOG_ERROR("Query fail to sendRequest. uri: %{public}s, err: %{public}d",
            DataShareStringUtils::Anonymous(uriStr).c_str(), err);
        return nullptr;
    }
    return result;
}

int DataShareServiceProxy::AddQueryTemplate(const std::string &uri, int64_t subscriberId, Template &tpl)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(IDataShareService::GetDescriptor())) {
        LOG_ERROR("Write descriptor failed!");
        return DATA_SHARE_ERROR;
    }
    if (!tpl.update_.empty()) {
        std::string updateSqlPrefix = "UPDATE";
        std::string checkPrefix = tpl.update_.substr(0, updateSqlPrefix.size());
        std::for_each(std::begin(checkPrefix), std::end(checkPrefix), [](auto &c) {
            c = std::toupper(c);
        });
        if (checkPrefix != updateSqlPrefix) {
            LOG_ERROR("Parameter update only support update SQL, Parameter: %{public}s", checkPrefix.c_str());
            return DATA_SHARE_ERROR;
        }
    }
    if (!ITypesUtil::Marshal(data, uri, subscriberId, tpl.update_, tpl.predicates_, tpl.scheduler_)) {
        LOG_ERROR("Write to message parcel failed!");
        return DATA_SHARE_ERROR;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(
        static_cast<uint32_t>(InterfaceCode::DATA_SHARE_SERVICE_CMD_ADD_TEMPLATE), data, reply, option);
    if (err != NO_ERROR) {
        LOG_ERROR("AddTemplate fail to sendRequest. uri: %{public}s, err: %{public}d",
            DataShareStringUtils::Anonymous(uri).c_str(), err);
        return DATA_SHARE_ERROR;
    }
    return reply.ReadInt32();
}

int DataShareServiceProxy::DelQueryTemplate(const std::string &uri, int64_t subscriberId)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(IDataShareService::GetDescriptor())) {
        LOG_ERROR("Write descriptor failed!");
        return DATA_SHARE_ERROR;
    }
    if (!ITypesUtil::Marshal(data, uri, subscriberId)) {
        LOG_ERROR("Write to message parcel failed!");
        return DATA_SHARE_ERROR;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(
        static_cast<uint32_t>(InterfaceCode::DATA_SHARE_SERVICE_CMD_DEL_TEMPLATE), data, reply, option);
    if (err != NO_ERROR) {
        LOG_ERROR("Delete template fail to sendRequest. uri: %{public}s, err: %{public}d",
            DataShareStringUtils::Anonymous(uri).c_str(), err);
        return DATA_SHARE_ERROR;
    }
    return reply.ReadInt32();
}

std::vector<OperationResult> DataShareServiceProxy::Publish(const Data &data, const std::string &bundleName)
{
    std::vector<OperationResult> results;
    MessageParcel parcel;
    if (!parcel.WriteInterfaceToken(IDataShareService::GetDescriptor())) {
        LOG_ERROR("Write descriptor failed!");
        return results;
    }
    if (!ITypesUtil::Marshal(parcel, data, bundleName)) {
        LOG_ERROR("Marshalfailed!");
        return results;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(
        static_cast<uint32_t>(InterfaceCode::DATA_SHARE_SERVICE_CMD_PUBLISH), parcel, reply, option);
    if (err != NO_ERROR) {
        LOG_ERROR("Publish fail to sendRequest. err: %{public}d", err);
        return results;
    }

    ITypesUtil::Unmarshal(reply, results);
    return results;
}

Data DataShareServiceProxy::GetPublishedData(const std::string &bundleName, int &resultCode)
{
    Data results;
    MessageParcel data;
    if (!data.WriteInterfaceToken(IDataShareService::GetDescriptor())) {
        LOG_ERROR("Write descriptor failed!");
        return results;
    }
    if (!ITypesUtil::Marshal(data, bundleName)) {
        LOG_ERROR("Write to message parcel failed!");
        return results;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(
        static_cast<uint32_t>(InterfaceCode::DATA_SHARE_SERVICE_CMD_GET_DATA), data, reply, option);
    if (err != NO_ERROR) {
        LOG_ERROR("Get published data fail to sendRequest, err: %{public}d", err);
        return results;
    }
    ITypesUtil::Unmarshal(reply, results.datas_, resultCode);
    return results;
}

std::vector<OperationResult> DataShareServiceProxy::SubscribeRdbData(const std::vector<std::string> &uris,
    const TemplateId &templateId, const sptr<IDataProxyRdbObserver> &observer)
{
    std::vector<OperationResult> results;
    if (observer == nullptr) {
        LOG_ERROR("Observer is nullptr, subscriberId: %{public}" PRId64, templateId.subscriberId_);
        return results;
    }
    MessageParcel data;
    if (!data.WriteInterfaceToken(IDataShareService::GetDescriptor())) {
        LOG_ERROR("Write descriptor failed!");
        return results;
    }

    if (!ITypesUtil::Marshal(data, uris, templateId.subscriberId_, templateId.bundleName_)) {
        LOG_ERROR("Write to message parcel failed!");
        return results;
    }
    if (!data.WriteRemoteObject(observer->AsObject())) {
        LOG_ERROR("Failed to write parcelable dataObserver ");
        return results;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(
        static_cast<uint32_t>(InterfaceCode::DATA_SHARE_SERVICE_CMD_SUBSCRIBE_RDB), data, reply, option);
    if (err != NO_ERROR) {
        LOG_ERROR("SubscribeRdbData fail to sendRequest. err: %{public}d", err);
        return results;
    }
    ITypesUtil::Unmarshal(reply, results);
    return results;
}

std::vector<OperationResult> DataShareServiceProxy::UnSubscribeRdbData(
    const std::vector<std::string> &uris, const TemplateId &templateId)
{
    std::vector<OperationResult> results;
    MessageParcel data;
    if (!data.WriteInterfaceToken(IDataShareService::GetDescriptor())) {
        LOG_ERROR("Write descriptor failed!");
        return results;
    }

    if (!ITypesUtil::Marshal(data, uris, templateId.subscriberId_, templateId.bundleName_)) {
        LOG_ERROR("Write to message parcel failed!");
        return results;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(
        static_cast<uint32_t>(InterfaceCode::DATA_SHARE_SERVICE_CMD_UNSUBSCRIBE_RDB), data, reply, option);
    if (err != NO_ERROR) {
        LOG_ERROR("Fail to sendRequest. err: %{public}d", err);
        return results;
    }
    ITypesUtil::Unmarshal(reply, results);
    return results;
}

std::vector<OperationResult> DataShareServiceProxy::EnableSubscribeRdbData(
    const std::vector<std::string> &uris, const TemplateId &templateId)
{
    std::vector<OperationResult> results;
    MessageParcel data;
    if (!data.WriteInterfaceToken(IDataShareService::GetDescriptor())) {
        LOG_ERROR("Write descriptor failed!");
        return results;
    }

    if (!ITypesUtil::Marshal(data, uris, templateId.subscriberId_, templateId.bundleName_)) {
        LOG_ERROR("Write to message parcel failed!");
        return results;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(
        static_cast<uint32_t>(InterfaceCode::DATA_SHARE_SERVICE_CMD_ENABLE_SUBSCRIBE_RDB), data, reply, option);
    if (err != NO_ERROR) {
        LOG_ERROR("Fail to sendRequest. err: %{public}d", err);
        return results;
    }
    ITypesUtil::Unmarshal(reply, results);
    return results;
}

std::vector<OperationResult> DataShareServiceProxy::DisableSubscribeRdbData(
    const std::vector<std::string> &uris, const TemplateId &templateId)
{
    std::vector<OperationResult> results;
    MessageParcel data;
    if (!data.WriteInterfaceToken(IDataShareService::GetDescriptor())) {
        LOG_ERROR("Write descriptor failed!");
        return results;
    }

    if (!ITypesUtil::Marshal(data, uris, templateId.subscriberId_, templateId.bundleName_)) {
        LOG_ERROR("Write to message parcel failed!");
        return results;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(
        static_cast<uint32_t>(InterfaceCode::DATA_SHARE_SERVICE_CMD_DISABLE_SUBSCRIBE_RDB), data, reply, option);
    if (err != NO_ERROR) {
        LOG_ERROR("Disable subscribe RdbData fail to sendRequest. err: %{public}d", err);
        return results;
    }
    ITypesUtil::Unmarshal(reply, results);
    return results;
}

std::vector<OperationResult> DataShareServiceProxy::SubscribePublishedData(
    const std::vector<std::string> &uris, int64_t subscriberId, const sptr<IDataProxyPublishedDataObserver> &observer)
{
    std::vector<OperationResult> results;
    if (observer == nullptr) {
        LOG_ERROR("Observer is nullptr, subscriberId: %{public}" PRId64, subscriberId);
        return results;
    }
    MessageParcel data;
    if (!data.WriteInterfaceToken(IDataShareService::GetDescriptor())) {
        LOG_ERROR("Write descriptor failed!");
        return results;
    }
    if (!ITypesUtil::Marshal(data, uris, subscriberId)) {
        LOG_ERROR("Write to message parcel failed!");
        return results;
    }
    if (!data.WriteRemoteObject(observer->AsObject())) {
        LOG_ERROR("Failed to write remote object dataObserver ");
        return results;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(
        static_cast<uint32_t>(InterfaceCode::DATA_SHARE_SERVICE_CMD_SUBSCRIBE_PUBLISHED), data, reply, option);
    if (err != NO_ERROR) {
        LOG_ERROR("Subscribe published data fail to sendRequest. err: %{public}d", err);
        return results;
    }
    ITypesUtil::Unmarshal(reply, results);
    return results;
}

std::vector<OperationResult> DataShareServiceProxy::UnSubscribePublishedData(
    const std::vector<std::string> &uris, int64_t subscriberId)
{
    std::vector<OperationResult> results;
    MessageParcel data;
    if (!data.WriteInterfaceToken(IDataShareService::GetDescriptor())) {
        LOG_ERROR("Write descriptor failed!");
        return results;
    }
    if (!ITypesUtil::Marshal(data, uris, subscriberId)) {
        LOG_ERROR("Write to message parcel failed!");
        return results;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(
        static_cast<uint32_t>(InterfaceCode::DATA_SHARE_SERVICE_CMD_UNSUBSCRIBE_PUBLISHED), data, reply, option);
    if (err != NO_ERROR) {
        LOG_ERROR("UnSubscribe published data fail to sendRequest. err: %{public}d", err);
        return results;
    }
    ITypesUtil::Unmarshal(reply, results);
    return results;
}

std::vector<OperationResult> DataShareServiceProxy::EnableSubscribePublishedData(
    const std::vector<std::string> &uris, int64_t subscriberId)
{
    std::vector<OperationResult> results;
    MessageParcel data;
    if (!data.WriteInterfaceToken(IDataShareService::GetDescriptor())) {
        LOG_ERROR("Write descriptor failed!");
        return results;
    }
    if (!ITypesUtil::Marshal(data, uris, subscriberId)) {
        LOG_ERROR("Write to message parcel failed!");
        return results;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(
        static_cast<uint32_t>(InterfaceCode::DATA_SHARE_SERVICE_CMD_ENABLE_SUBSCRIBE_PUBLISHED), data, reply, option);
    if (err != NO_ERROR) {
        LOG_ERROR("Enable subscribe published data fail to sendRequest. err: %{public}d", err);
        return results;
    }
    ITypesUtil::Unmarshal(reply, results);
    return results;
}

std::vector<OperationResult> DataShareServiceProxy::DisableSubscribePublishedData(
    const std::vector<std::string> &uris, int64_t subscriberId)
{
    std::vector<OperationResult> results;
    MessageParcel data;
    if (!data.WriteInterfaceToken(IDataShareService::GetDescriptor())) {
        LOG_ERROR("Write descriptor failed!");
        return results;
    }
    if (!ITypesUtil::Marshal(data, uris, subscriberId)) {
        LOG_ERROR("Write to message parcel failed!");
        return results;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(
        static_cast<uint32_t>(InterfaceCode::DATA_SHARE_SERVICE_CMD_DISABLE_SUBSCRIBE_PUBLISHED), data, reply, option);
    if (err != NO_ERROR) {
        LOG_ERROR("Disable subscribe published data fail to sendRequest. err: %{public}d", err);
        return results;
    }
    ITypesUtil::Unmarshal(reply, results);
    return results;
}

void DataShareServiceProxy::Notify(const std::string &uri)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(IDataShareService::GetDescriptor())) {
        LOG_ERROR("Write descriptor failed!");
        return;
    }
    if (!ITypesUtil::Marshal(data, uri)) {
        LOG_ERROR("Write to message parcel failed!");
        return;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(
        static_cast<uint32_t>(InterfaceCode::DATA_SHARE_SERVICE_CMD_NOTIFY_OBSERVERS), data, reply, option);
    if (err != NO_ERROR) {
        LOG_ERROR("Notify fail to sendRequest. err: %{public}d", err);
        return;
    }
}

int DataShareServiceProxy::SetSilentSwitch(const Uri &uri, bool enable)
{
    const std::string &uriStr = uri.ToString();
    MessageParcel data;
    if (!data.WriteInterfaceToken(IDataShareService::GetDescriptor())) {
        LOG_ERROR("Write descriptor failed!");
        return DATA_SHARE_ERROR;
    }
    if (!ITypesUtil::Marshal(data, uriStr, enable)) {
        LOG_ERROR("Write to message parcel failed!");
        return DATA_SHARE_ERROR;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(
        static_cast<uint32_t>(InterfaceCode::DATA_SHARE_SERVICE_CMD_SET_SILENT_SWITCH), data, reply, option);
    if (err != NO_ERROR) {
        LOG_ERROR("SetSilentSwitch fail to sendRequest. uri: %{public}s, err: %{public}d",
            DataShareStringUtils::Anonymous(uriStr).c_str(), err);
        return DATA_SHARE_ERROR;
    }
    return reply.ReadInt32();
}

int DataShareServiceProxy::GetSilentProxyStatus(const std::string &uri)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(IDataShareService::GetDescriptor())) {
        LOG_ERROR("Write descriptor failed!");
        return DATA_SHARE_ERROR;
    }
    if (!ITypesUtil::Marshal(data, uri)) {
        LOG_ERROR("Write to message parcel failed!");
        return DATA_SHARE_ERROR;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(
        static_cast<uint32_t>(InterfaceCode::DATA_SHARE_SERVICE_CMD_GET_SILENT_PROXY_STATUS), data, reply, option);
    if (err != NO_ERROR) {
        LOG_ERROR("Is silent proxy enable fail to sendRequest. uri: %{public}s, err: %{public}d",
            DataShareStringUtils::Anonymous(uri).c_str(), err);
        return DATA_SHARE_ERROR;
    }
    return reply.ReadInt32();
}

int DataShareServiceProxy::RegisterObserver(const Uri &uri, const sptr<OHOS::IRemoteObject> &dataObserver)
{
    if (dataObserver == nullptr) {
        LOG_ERROR("DataObserver is nullptr, uri:%{public}s",
            DataShareStringUtils::Anonymous(uri.ToString()).c_str());
        return DATA_SHARE_ERROR;
    }
    MessageParcel data;
    if (!data.WriteInterfaceToken(IDataShareService::GetDescriptor())) {
        LOG_ERROR("Write interface token failed");
        return DATA_SHARE_ERROR;
    }
    if (!ITypesUtil::Marshal(data, uri.ToString(), dataObserver)) {
        LOG_ERROR("Failed to marshalling");
        return DATA_SHARE_ERROR;
    }
    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(
        static_cast<uint32_t>(InterfaceCode::DATA_SHARE_SERVICE_CMD_REGISTER_OBSERVER), data, reply, option);
    if (err != NO_ERROR) {
        LOG_ERROR("RegisterObserver fail to sendRequest. uri:%{public}s, err:%{public}d",
            DataShareStringUtils::Anonymous(uri.ToString()).c_str(), err);
        return DATA_SHARE_ERROR;
    }
    return reply.ReadInt32();
}

int DataShareServiceProxy::UnRegisterObserver(const Uri &uri, const sptr<OHOS::IRemoteObject> &dataObserver)
{
    if (dataObserver == nullptr) {
        LOG_ERROR("DataObserver is nullptr, uri:%{public}s",
            DataShareStringUtils::Anonymous(uri.ToString()).c_str());
        return DATA_SHARE_ERROR;
    }
    MessageParcel data;
    if (!data.WriteInterfaceToken(IDataShareService::GetDescriptor())) {
        LOG_ERROR("Write interface token failed");
        return DATA_SHARE_ERROR;
    }
    if (!ITypesUtil::Marshal(data, uri.ToString(), dataObserver)) {
        LOG_ERROR("Failed to Marshalling");
        return DATA_SHARE_ERROR;
    }
    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(
        static_cast<uint32_t>(InterfaceCode::DATA_SHARE_SERVICE_CMD_UNREGISTER_OBSERVER), data, reply, option);
    if (err != NO_ERROR) {
        LOG_ERROR("UnRegisterObserver fail to sendRequest. uri: %{public}s, err: %{public}d",
            DataShareStringUtils::Anonymous(uri.ToString()).c_str(), err);
        return DATA_SHARE_ERROR;
    }
    return reply.ReadInt32();
}
} // namespace DataShare
} // namespace OHOS
