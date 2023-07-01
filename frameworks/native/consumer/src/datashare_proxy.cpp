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

#include "datashare_proxy.h"

#include <string_ex.h>

#include "data_ability_observer_interface.h"
#include "datashare_itypes_utils.h"
#include "datashare_log.h"
#include "datashare_result_set.h"
#include "ipc_types.h"
#include "ishared_result_set.h"
#include "pac_map.h"

namespace OHOS {
namespace DataShare {
constexpr int32_t PERMISSION_ERR = 1;
constexpr int PERMISSION_ERR_CODE = -2;
std::vector<std::string> DataShareProxy::GetFileTypes(const Uri &uri, const std::string &mimeTypeFilter)
{
    std::vector<std::string> types;

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(DataShareProxy::GetDescriptor())) {
        LOG_ERROR("WriteInterfaceToken failed");
        return types;
    }

    if (!data.WriteParcelable(&uri)) {
        LOG_ERROR("fail to WriteParcelable uri");
        return types;
    }

    if (!data.WriteString(mimeTypeFilter)) {
        LOG_ERROR("fail to WriteString mimeTypeFilter");
        return types;
    }

    int32_t err = Remote()->SendRequest(
        static_cast<uint32_t>(DistributedShare::DataShare::DataShareInterfaceCode::CMD_GET_FILE_TYPES), data, reply,
        option);
    if (err != DATA_SHARE_NO_ERROR) {
        LOG_ERROR("GetFileTypes fail to SendRequest. err: %{public}d", err);
    }

    if (!reply.ReadStringVector(&types)) {
        LOG_ERROR("fail to ReadStringVector types");
    }

    return types;
}

int DataShareProxy::OpenFile(const Uri &uri, const std::string &mode)
{
    int fd = -1;
    MessageParcel data;
    if (!data.WriteInterfaceToken(DataShareProxy::GetDescriptor())) {
        LOG_ERROR("WriteInterfaceToken failed");
        return fd;
    }

    if (!data.WriteParcelable(&uri)) {
        LOG_ERROR("fail to WriteParcelable uri");
        return fd;
    }

    if (!data.WriteString(mode)) {
        LOG_ERROR("fail to WriteString mode");
        return fd;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(
        static_cast<uint32_t>(DistributedShare::DataShare::DataShareInterfaceCode::CMD_OPEN_FILE), data, reply, option);
    if (err != DATA_SHARE_NO_ERROR) {
        LOG_ERROR("OpenFile fail to SendRequest. err: %{public}d", err);
        return fd;
    }

    fd = reply.ReadFileDescriptor();
    if (fd == -1) {
        LOG_ERROR("fail to ReadFileDescriptor fd");
        return fd;
    }

    return fd;
}

int DataShareProxy::OpenRawFile(const Uri &uri, const std::string &mode)
{
    int fd = -1;
    MessageParcel data;
    if (!data.WriteInterfaceToken(DataShareProxy::GetDescriptor())) {
        LOG_ERROR("WriteInterfaceToken failed");
        return fd;
    }

    if (!data.WriteParcelable(&uri)) {
        LOG_ERROR("fail to WriteParcelable uri");
        return fd;
    }

    if (!data.WriteString(mode)) {
        LOG_ERROR("fail to WriteString mode");
        return fd;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(
        static_cast<uint32_t>(DistributedShare::DataShare::DataShareInterfaceCode::CMD_OPEN_RAW_FILE), data, reply,
        option);
    if (err != DATA_SHARE_NO_ERROR) {
        LOG_ERROR("OpenRawFile fail to SendRequest. err: %{public}d", err);
        return fd;
    }

    if (!reply.ReadInt32(fd)) {
        LOG_ERROR("fail to ReadInt32 fd");
        return fd;
    }

    return fd;
}

int DataShareProxy::Insert(const Uri &uri, const DataShareValuesBucket &value)
{
    int index = -1;
    MessageParcel data;
    if (!data.WriteInterfaceToken(DataShareProxy::GetDescriptor())) {
        LOG_ERROR("WriteInterfaceToken failed");
        return index;
    }
    if (!ITypesUtil::Marshal(data, uri, value)) {
        LOG_ERROR("fail to Marshal value");
        return index;
    }
    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(
        static_cast<uint32_t>(DistributedShare::DataShare::DataShareInterfaceCode::CMD_INSERT), data, reply, option);
    if (err != DATA_SHARE_NO_ERROR) {
        LOG_ERROR("Insert fail to SendRequest. err: %{public}d", err);
        return err == PERMISSION_ERR ? PERMISSION_ERR_CODE : index;
    }
    if (!ITypesUtil::Unmarshal(reply, index)) {
        LOG_ERROR("fail to Unmarshal index");
        return index;
    }

    return index;
}

int DataShareProxy::Update(const Uri &uri, const DataSharePredicates &predicates, const DataShareValuesBucket &value)
{
    int index = -1;
    MessageParcel data;
    if (!data.WriteInterfaceToken(DataShareProxy::GetDescriptor())) {
        LOG_ERROR("WriteInterfaceToken failed");
        return index;
    }
    if (!ITypesUtil::Marshal(data, uri, predicates, value)) {
        LOG_ERROR("fail to Marshal value");
        return index;
    }
    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(
        static_cast<uint32_t>(DistributedShare::DataShare::DataShareInterfaceCode::CMD_UPDATE), data, reply, option);
    if (err != DATA_SHARE_NO_ERROR) {
        LOG_ERROR("Update fail to SendRequest. err: %{public}d", err);
        return err == PERMISSION_ERR ? PERMISSION_ERR_CODE : index;
    }
    if (!ITypesUtil::Unmarshal(reply, index)) {
        LOG_ERROR("fail to Unmarshal index");
        return index;
    }
    return index;
}

int DataShareProxy::Delete(const Uri &uri, const DataSharePredicates &predicates)
{
    int index = -1;
    MessageParcel data;
    if (!data.WriteInterfaceToken(DataShareProxy::GetDescriptor())) {
        LOG_ERROR("WriteInterfaceToken failed");
        return index;
    }
    if (!ITypesUtil::Marshal(data, uri, predicates)) {
        LOG_ERROR("fail to Marshalling predicates");
        return index;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(
        static_cast<uint32_t>(DistributedShare::DataShare::DataShareInterfaceCode::CMD_DELETE), data, reply, option);
    if (err != DATA_SHARE_NO_ERROR) {
        LOG_ERROR("Delete fail to SendRequest. err: %{public}d", err);
        return err == PERMISSION_ERR ? PERMISSION_ERR_CODE : index;
    }
    if (!ITypesUtil::Unmarshal(reply, index)) {
        LOG_ERROR("fail to Unmarshal index");
        return index;
    }
    return index;
}

std::shared_ptr<DataShareResultSet> DataShareProxy::Query(const Uri &uri, const DataSharePredicates &predicates,
    std::vector<std::string> &columns, DatashareBusinessError &businessError)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(DataShareProxy::GetDescriptor())) {
        LOG_ERROR("WriteInterfaceToken failed");
        return nullptr;
    }
    if (!ITypesUtil::Marshal(data, uri, predicates, columns)) {
        LOG_ERROR("fail to Marshalling");
        return nullptr;
    }
    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(
        static_cast<uint32_t>(DistributedShare::DataShare::DataShareInterfaceCode::CMD_QUERY), data, reply, option);
    auto result = ISharedResultSet::ReadFromParcel(reply);
    businessError.SetCode(reply.ReadInt32());
    businessError.SetMessage(reply.ReadString());
    if (err != DATA_SHARE_NO_ERROR) {
        LOG_ERROR("Query fail to SendRequest. err: %{public}d", err);
        return nullptr;
    }
    return result;
}

std::string DataShareProxy::GetType(const Uri &uri)
{
    LOG_INFO("begin.");
    std::string type;
    MessageParcel data;
    if (!data.WriteInterfaceToken(DataShareProxy::GetDescriptor())) {
        LOG_ERROR("WriteInterfaceToken failed");
        return type;
    }

    if (!ITypesUtil::Marshal(data, uri)) {
        LOG_ERROR("fail to Marshal value");
        return type;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(
        static_cast<uint32_t>(DistributedShare::DataShare::DataShareInterfaceCode::CMD_GET_TYPE), data, reply, option);
    if (err != DATA_SHARE_NO_ERROR) {
        LOG_ERROR("GetFileTypes fail to SendRequest. err: %{public}d", err);
        return type;
    }
    if (!ITypesUtil::Unmarshal(reply, type)) {
        LOG_ERROR("fail to Unmarshal index");
        return type;
    }
    if (type.empty()) {
        LOG_ERROR("fail to ReadString type");
        return type;
    }

    LOG_INFO("end successfully.");
    return type;
}

int DataShareProxy::BatchInsert(const Uri &uri, const std::vector<DataShareValuesBucket> &values)
{
    LOG_INFO("begin.");
    int ret = -1;
    MessageParcel data;
    if (!data.WriteInterfaceToken(DataShareProxy::GetDescriptor())) {
        LOG_ERROR("WriteInterfaceToken failed");
        return ret;
    }
    if (!ITypesUtil::Marshal(data, uri, values)) {
        LOG_ERROR("fail to Marshalling");
        return ret;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(
        static_cast<uint32_t>(DistributedShare::DataShare::DataShareInterfaceCode::CMD_BATCH_INSERT), data, reply,
        option);
    if (err != DATA_SHARE_NO_ERROR) {
        LOG_ERROR("fail to SendRequest. err: %{public}d", err);
        return err == PERMISSION_ERR ? PERMISSION_ERR_CODE : ret;
    }
    if (!ITypesUtil::Unmarshal(reply, ret)) {
        LOG_ERROR("fail to Unmarshal index");
        return ret;
    }
    LOG_INFO("end successfully.");
    return ret;
}

bool DataShareProxy::RegisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver)
{
    LOG_INFO("begin.");
    MessageParcel data;
    if (!data.WriteInterfaceToken(DataShareProxy::GetDescriptor())) {
        LOG_ERROR("WriteInterfaceToken failed");
        return false;
    }

    if (!ITypesUtil::Marshal(data, uri, dataObserver->AsObject())) {
        LOG_ERROR("fail to Marshalling");
        return false;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t result = Remote()->SendRequest(
        static_cast<uint32_t>(DistributedShare::DataShare::DataShareInterfaceCode::CMD_REGISTER_OBSERVER), data, reply,
        option);
    if (result == ERR_NONE) {
        LOG_INFO("SendRequest ok, retval is %{public}d", reply.ReadInt32());
    } else {
        LOG_ERROR("SendRequest error, result=%{public}d", result);
        return false;
    }
    LOG_INFO("end.");
    return true;
}

bool DataShareProxy::UnregisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver)
{
    LOG_INFO("begin.");
    MessageParcel data;
    if (!data.WriteInterfaceToken(DataShareProxy::GetDescriptor())) {
        LOG_ERROR("WriteInterfaceToken failed");
        return false;
    }
    if (!ITypesUtil::Marshal(data, uri, dataObserver->AsObject())) {
        LOG_ERROR("fail to Marshalling");
        return false;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t result = Remote()->SendRequest(
        static_cast<uint32_t>(DistributedShare::DataShare::DataShareInterfaceCode::CMD_UNREGISTER_OBSERVER), data,
        reply, option);
    if (result == ERR_NONE) {
        LOG_INFO("SendRequest ok, retval is %{public}d", reply.ReadInt32());
    } else {
        LOG_ERROR("SendRequest error, result=%{public}d", result);
        return false;
    }
    LOG_INFO("end successfully.");
    return true;
}

bool DataShareProxy::NotifyChange(const Uri &uri)
{
    LOG_INFO("begin.");
    MessageParcel data;
    if (!data.WriteInterfaceToken(DataShareProxy::GetDescriptor())) {
        LOG_ERROR("WriteInterfaceToken failed");
        return false;
    }
    if (!ITypesUtil::Marshal(data, uri)) {
        LOG_ERROR("fail to Marshalling");
        return false;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t result = Remote()->SendRequest(
        static_cast<uint32_t>(DistributedShare::DataShare::DataShareInterfaceCode::CMD_NOTIFY_CHANGE), data, reply,
        option);
    if (result == ERR_NONE) {
        LOG_INFO("SendRequest ok, retval is %{public}d", reply.ReadInt32());
    } else {
        LOG_ERROR("SendRequest error, result=%{public}d", result);
        return false;
    }
    LOG_INFO("end successfully.");
    return true;
}

Uri DataShareProxy::NormalizeUri(const Uri &uri)
{
    LOG_INFO("begin.");
    MessageParcel data;
    if (!data.WriteInterfaceToken(DataShareProxy::GetDescriptor())) {
        LOG_ERROR("WriteInterfaceToken failed");
        return Uri("");
    }
    if (!ITypesUtil::Marshal(data, uri)) {
        LOG_ERROR("fail to Marshalling");
        return Uri("");
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(
        static_cast<uint32_t>(DistributedShare::DataShare::DataShareInterfaceCode::CMD_NORMALIZE_URI),
        data, reply, option);
    if (err != DATA_SHARE_NO_ERROR) {
        LOG_ERROR("NormalizeUri fail to SendRequest. err: %{public}d", err);
        return Uri("");
    }
    Uri info("");
    if (!ITypesUtil::Unmarshal(reply, info)) {
        LOG_ERROR("fail to Unmarshal index");
        return Uri("");
    }
    LOG_INFO("end successfully.");
    return info;
}

Uri DataShareProxy::DenormalizeUri(const Uri &uri)
{
    LOG_INFO("begin.");
    MessageParcel data;
    if (!data.WriteInterfaceToken(DataShareProxy::GetDescriptor())) {
        LOG_ERROR("WriteInterfaceToken failed");
        return Uri("");
    }

    if (!ITypesUtil::Marshal(data, uri)) {
        LOG_ERROR("fail to Marshalling");
        return Uri("");
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(
        static_cast<uint32_t>(DistributedShare::DataShare::DataShareInterfaceCode::CMD_DENORMALIZE_URI), data, reply,
        option);
    if (err != DATA_SHARE_NO_ERROR) {
        LOG_ERROR("DenormalizeUri fail to SendRequest. err: %{public}d", err);
        return Uri("");
    }

    Uri info("");
    if (!ITypesUtil::Unmarshal(reply, info)) {
        LOG_ERROR("fail to Unmarshal index");
        return Uri("");
    }
    LOG_INFO("end successfully.");
    return info;
}
} // namespace DataShare
} // namespace OHOS
