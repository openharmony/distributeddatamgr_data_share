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

#include "data_share_service_proxy.h"

#include "datashare_log.h"
#include "ishared_result_set.h"
#include "itypes_utils.h"

namespace OHOS {
namespace DataShare {
DataShareServiceProxy::DataShareServiceProxy(const sptr<IRemoteObject> &object)
    : IRemoteProxy<IDataShareService>(object)
{
    LOG_INFO("Construct complete.");
}

int DataShareServiceProxy::Insert(const Uri &uri, const DataShareValuesBucket &value)
{
    std::string uriStr = uri.ToString();
    MessageParcel data;
    if (!data.WriteInterfaceToken(IDataShareService::GetDescriptor())) {
        LOG_ERROR("Write descriptor failed!");
        return DATA_SHARE_ERROR;
    }
    if (!ITypesUtils::Marshal(data, uriStr, value)) {
        LOG_ERROR("Write to message parcel failed!");
        return DATA_SHARE_ERROR;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(DATA_SHARE_SERVICE_CMD_INSERT, data, reply, option);
    if (err != NO_ERROR) {
        LOG_ERROR("Insert fail to SendRequest. uri: %{public}s, err: %{public}d", uriStr.c_str(), err);
        return DATA_SHARE_ERROR;
    }
    return reply.ReadInt32();
}

int32_t DataShareServiceProxy::Update(const Uri &uri,
    const DataSharePredicates &predicate, const DataShareValuesBucket &valuesBucket)
{
    std::string uriStr = uri.ToString();
    MessageParcel data;
    if (!data.WriteInterfaceToken(IDataShareService::GetDescriptor())) {
        LOG_ERROR("Write descriptor failed!");
        return DATA_SHARE_ERROR;
    }
    if (!ITypesUtils::Marshal(data, uriStr, predicate, valuesBucket)) {
        LOG_ERROR("Write to message parcel failed!");
        return DATA_SHARE_ERROR;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(DATA_SHARE_SERVICE_CMD_UPDATE, data, reply, option);
    if (err != NO_ERROR) {
        LOG_ERROR("Update fail to SendRequest. uri: %{public}s, err: %{public}d", uriStr.c_str(), err);
        return DATA_SHARE_ERROR;
    }
    return reply.ReadInt32();
}

    int DataShareServiceProxy::Delete(const Uri &uri, const DataSharePredicates &predicate)
{
    std::string uriStr = uri.ToString();
    MessageParcel data;
    if (!data.WriteInterfaceToken(IDataShareService::GetDescriptor())) {
        LOG_ERROR("Write descriptor failed!");
        return DATA_SHARE_ERROR;
    }
    if (!ITypesUtils::Marshal(data, uriStr, predicate)) {
        LOG_ERROR("Write to message parcel failed!");
        return DATA_SHARE_ERROR;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(DATA_SHARE_SERVICE_CMD_DELETE, data, reply, option);
    if (err != NO_ERROR) {
        LOG_ERROR("Delete fail to SendRequest. uri: %{public}s, err: %{public}d", uriStr.c_str(), err);
        return DATA_SHARE_ERROR;
    }
    return reply.ReadInt32();
}

std::shared_ptr<DataShareResultSet> DataShareServiceProxy::Query(
    const Uri &uri, const DataSharePredicates &predicates, std::vector<std::string> &columns)
{
    std::string uriStr = uri.ToString();
    MessageParcel data;
    if (!data.WriteInterfaceToken(IDataShareService::GetDescriptor())) {
        LOG_ERROR("WriteInterfaceToken failed!");
        return nullptr;
    }

    if (!ITypesUtils::Marshal(data, uriStr, predicates, columns)) {
        LOG_ERROR("Write to message parcel failed!");
        return nullptr;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(DATA_SHARE_SERVICE_CMD_QUERY, data, reply, option);
    if (err != NO_ERROR) {
        LOG_ERROR("Query fail to SendRequest. uri: %{public}s, err: %{public}d", uriStr.c_str(), err);
        return nullptr;
    }
    return ISharedResultSet::ReadFromParcel(reply);
}


int DataShareServiceProxy::OpenFile(const Uri &uri, const std::string &mode)
{
    return 0;
}

std::vector<std::string> DataShareServiceProxy::GetFileTypes(const Uri &uri, const std::string &mimeTypeFilter)
{
    return std::vector<std::string>();
}

int DataShareServiceProxy::OpenRawFile(const Uri &uri, const std::string &mode)
{
    return 0;
}

std::string DataShareServiceProxy::GetType(const Uri &uri)
{
    return std::string();
}

int DataShareServiceProxy::BatchInsert(const Uri &uri, const std::vector<DataShareValuesBucket> &values)
{
    return 0;
}

bool DataShareServiceProxy::RegisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver)
{
    return false;
}

bool DataShareServiceProxy::UnregisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver)
{
    return false;
}

bool DataShareServiceProxy::NotifyChange(const Uri &uri)
{
    return false;
}

Uri DataShareServiceProxy::NormalizeUri(const Uri &uri)
{
    return Uri("");
}

Uri DataShareServiceProxy::DenormalizeUri(const Uri &uri)
{
    return Uri("");
}
}
}