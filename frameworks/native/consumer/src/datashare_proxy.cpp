/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

using namespace OHOS::DistributedShare::DataShare;

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
        static_cast<uint32_t>(IDataShareInterfaceCode::CMD_GET_FILE_TYPES), data, reply, option);
    if (err != E_OK) {
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
        static_cast<uint32_t>(IDataShareInterfaceCode::CMD_OPEN_FILE), data, reply, option);
    if (err != E_OK) {
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
        static_cast<uint32_t>(IDataShareInterfaceCode::CMD_OPEN_RAW_FILE), data, reply, option);
    if (err != E_OK) {
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
    data.SetMaxCapacity(MTU_SIZE);
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
        static_cast<uint32_t>(IDataShareInterfaceCode::CMD_INSERT), data, reply, option);
    if (err != E_OK) {
        LOG_ERROR("Insert fail to SendRequest. err: %{public}d", err);
        return err == PERMISSION_ERR ? PERMISSION_ERR_CODE : index;
    }
    if (!ITypesUtil::Unmarshal(reply, index)) {
        LOG_ERROR("fail to Unmarshal index");
        return index;
    }

    return index;
}

int DataShareProxy::InsertExt(const Uri &uri, const DataShareValuesBucket &value, std::string &result)
{
    int index = -1;
    MessageParcel data;
    data.SetMaxCapacity(MTU_SIZE);
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
        static_cast<uint32_t>(IDataShareInterfaceCode::CMD_INSERT_EXT), data, reply, option);
    if (err != E_OK) {
        LOG_ERROR("Insert fail to SendRequest. err: %{public}d", err);
        return index;
    }
    if (!ITypesUtil::Unmarshal(reply, index, result)) {
        LOG_ERROR("fail to Unmarshal index");
        return index;
    }
    return index;
}

int DataShareProxy::Update(const Uri &uri, const DataSharePredicates &predicates, const DataShareValuesBucket &value)
{
    int index = -1;
    MessageParcel data;
    data.SetMaxCapacity(MTU_SIZE);
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
        static_cast<uint32_t>(IDataShareInterfaceCode::CMD_UPDATE), data, reply, option);
    if (err != E_OK) {
        LOG_ERROR("Update fail to SendRequest. err: %{public}d", err);
        return err == PERMISSION_ERR ? PERMISSION_ERR_CODE : index;
    }
    if (!ITypesUtil::Unmarshal(reply, index)) {
        LOG_ERROR("fail to Unmarshal index");
        return index;
    }
    return index;
}

int DataShareProxy::BatchUpdate(const UpdateOperations &operations, std::vector<BatchUpdateResult> &results)
{
    int ret = -1;
    MessageParcel data;
    data.SetMaxCapacity(MTU_SIZE);
    if (!data.WriteInterfaceToken(DataShareProxy::GetDescriptor())) {
        LOG_ERROR("WriteInterfaceToken failed");
        return ret;
    }
    if (!CheckSize(operations)) {
        return ret;
    }
    if (!ITypesUtil::Marshal(data, operations)) {
        LOG_ERROR("fail to Marshalling");
        return ret;
    }
    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(
        static_cast<uint32_t>(IDataShareInterfaceCode::CMD_BATCH_UPDATE), data, reply, option);
    if (err != E_OK) {
        LOG_ERROR("BatchUpdate fail to SendRequest. err: %{public}d", err);
        return err == PERMISSION_ERR ? PERMISSION_ERR_CODE : ret;
    }
    if (!ITypesUtil::Unmarshal(reply, results)) {
        LOG_ERROR("fail to Unmarshal result");
        return ret;
    }
    return 0;
}

int DataShareProxy::Delete(const Uri &uri, const DataSharePredicates &predicates)
{
    int index = -1;
    MessageParcel data;
    data.SetMaxCapacity(MTU_SIZE);
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
        static_cast<uint32_t>(IDataShareInterfaceCode::CMD_DELETE), data, reply, option);
    if (err != E_OK) {
        LOG_ERROR("Delete fail to SendRequest. err: %{public}d", err);
        return err == PERMISSION_ERR ? PERMISSION_ERR_CODE : index;
    }
    if (!ITypesUtil::Unmarshal(reply, index)) {
        LOG_ERROR("fail to Unmarshal index");
        return index;
    }
    return index;
}

std::pair<int32_t, int32_t> DataShareProxy::InsertEx(const Uri &uri, const DataShareValuesBucket &value)
{
    MessageParcel data;
    data.SetMaxCapacity(MTU_SIZE);
    if (!data.WriteInterfaceToken(DataShareProxy::GetDescriptor())) {
        LOG_ERROR("WriteInterfaceToken failed");
        return std::make_pair(E_WRITE_TO_PARCE_ERROR, 0);
    }
    if (!ITypesUtil::Marshal(data, uri, value)) {
        LOG_ERROR("fail to Marshal value");
        return std::make_pair(E_MARSHAL_ERROR, 0);
    }

    int32_t errCode = -1;
    int32_t result = -1;
    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(
        static_cast<uint32_t>(IDataShareInterfaceCode::CMD_INSERT_EX), data, reply, option);
    if (err != E_OK) {
        LOG_ERROR("InsertEx fail to SendRequest. err: %{public}d", err);
        return std::make_pair((err == PERMISSION_ERR ? PERMISSION_ERR_CODE : errCode), 0);
    }

    if (!ITypesUtil::Unmarshal(reply, errCode, result)) {
        LOG_ERROR("fail to Unmarshal");
        return std::make_pair(E_UNMARSHAL_ERROR, 0);
    }
    return std::make_pair(errCode, result);
}

std::pair<int32_t, int32_t> DataShareProxy::UpdateEx(const Uri &uri, const DataSharePredicates &predicates,
    const DataShareValuesBucket &value)
{
    MessageParcel data;
    data.SetMaxCapacity(MTU_SIZE);
    if (!data.WriteInterfaceToken(DataShareProxy::GetDescriptor())) {
        LOG_ERROR("WriteInterfaceToken failed");
        return std::make_pair(E_WRITE_TO_PARCE_ERROR, 0);
    }
    if (!ITypesUtil::Marshal(data, uri, predicates, value)) {
        LOG_ERROR("fail to Marshal value");
        return std::make_pair(E_MARSHAL_ERROR, 0);
    }

    int32_t errCode = -1;
    int32_t result = -1;
    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(
        static_cast<uint32_t>(IDataShareInterfaceCode::CMD_UPDATE_EX), data, reply, option);
    if (err != E_OK) {
        LOG_ERROR("UpdateEx fail to SendRequest. err: %{public}d", err);
        return std::make_pair((err == PERMISSION_ERR ? PERMISSION_ERR_CODE : errCode), 0);
    }

    if (!ITypesUtil::Unmarshal(reply, errCode, result)) {
        LOG_ERROR("fail to Unmarshal");
        return std::make_pair(E_UNMARSHAL_ERROR, 0);
    }
    return std::make_pair(errCode, result);
}

std::pair<int32_t, int32_t> DataShareProxy::DeleteEx(const Uri &uri, const DataSharePredicates &predicates)
{
    MessageParcel data;
    data.SetMaxCapacity(MTU_SIZE);
    if (!data.WriteInterfaceToken(DataShareProxy::GetDescriptor())) {
        LOG_ERROR("WriteInterfaceToken failed");
        return std::make_pair(E_WRITE_TO_PARCE_ERROR, 0);
    }
    if (!ITypesUtil::Marshal(data, uri, predicates)) {
        LOG_ERROR("fail to Marshalling predicates");
        return std::make_pair(E_MARSHAL_ERROR, 0);
    }

    int32_t errCode = -1;
    int32_t result = -1;
    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(
        static_cast<uint32_t>(IDataShareInterfaceCode::CMD_DELETE_EX), data, reply, option);
    if (err != E_OK) {
        LOG_ERROR("DeleteEx fail to SendRequest. err: %{public}d", err);
        return std::make_pair((err == PERMISSION_ERR ? PERMISSION_ERR_CODE : errCode), 0);
    }

    if (!ITypesUtil::Unmarshal(reply, errCode, result)) {
        LOG_ERROR("fail to Unmarshal");
        return std::make_pair(E_UNMARSHAL_ERROR, 0);
    }
    return std::make_pair(errCode, result);
}

std::shared_ptr<DataShareResultSet> DataShareProxy::Query(const Uri &uri, const DataSharePredicates &predicates,
    std::vector<std::string> &columns, DatashareBusinessError &businessError)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(DataShareProxy::GetDescriptor())) {
        LOG_ERROR("WriteInterfaceToken failed");
        businessError.SetCode(E_WRITE_TO_PARCE_ERROR);
        return nullptr;
    }
    if (!ITypesUtil::Marshal(data, uri, columns)) {
        LOG_ERROR("Marshalling uri and columns to data failed");
        businessError.SetCode(E_MARSHAL_ERROR);
        return nullptr;
    }
    if (!ITypesUtil::MarshalPredicates(predicates, data)) {
        LOG_ERROR("Marshalling predicates to shared-memory failed");
        businessError.SetCode(E_MARSHAL_ERROR);
        return nullptr;
    }
    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(
        static_cast<uint32_t>(IDataShareInterfaceCode::CMD_QUERY), data, reply, option);
    auto result = ISharedResultSet::ReadFromParcel(reply);
    if (err != E_OK) {
        LOG_ERROR("Query fail to SendRequest. err: %{public}d", err);
        businessError.SetCode(err);
        return nullptr;
    }
    businessError.SetCode(reply.ReadInt32());
    businessError.SetMessage(reply.ReadString());
    return result;
}

std::string DataShareProxy::GetType(const Uri &uri)
{
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
        static_cast<uint32_t>(IDataShareInterfaceCode::CMD_GET_TYPE), data, reply, option);
    if (err != E_OK) {
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

    return type;
}

int DataShareProxy::BatchInsert(const Uri &uri, const std::vector<DataShareValuesBucket> &values)
{
    int ret = -1;
    MessageParcel data;
    data.SetMaxCapacity(MTU_SIZE);
    if (!data.WriteInterfaceToken(DataShareProxy::GetDescriptor())) {
        LOG_ERROR("WriteInterfaceToken failed");
        return ret;
    }
    if (!ITypesUtil::Marshal(data, uri)) {
        LOG_ERROR("Marshalling uri to data failed");
        return ret;
    }
    if (!ITypesUtil::MarshalValuesBucketVec(values, data)) {
        LOG_ERROR("Marshalling DataShareValuesBucket to shared-memory failed");
        return ret;
    }
    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(
        static_cast<uint32_t>(IDataShareInterfaceCode::CMD_BATCH_INSERT), data, reply, option);
    if (err != E_OK) {
        LOG_ERROR("fail to SendRequest. err: %{public}d", err);
        return err == PERMISSION_ERR ? PERMISSION_ERR_CODE : ret;
    }
    if (!ITypesUtil::Unmarshal(reply, ret)) {
        LOG_ERROR("fail to Unmarshal index");
        return ret;
    }
    return ret;
}

int DataShareProxy::ExecuteBatch(const std::vector<OperationStatement> &statements, ExecResultSet &result)
{
    MessageParcel data;
    data.SetMaxCapacity(MTU_SIZE);
    if (!data.WriteInterfaceToken(DataShareProxy::GetDescriptor())) {
        LOG_ERROR("WriteInterfaceToken failed");
        return -1;
    }
    if (!ITypesUtil::Marshal(data, statements)) {
        LOG_ERROR("fail to Marshal");
        return -1;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t err = Remote()->SendRequest(
        static_cast<uint32_t>(IDataShareInterfaceCode::CMD_EXECUTE_BATCH), data, reply, option);
    if (err != E_OK) {
        LOG_ERROR("fail to SendRequest. err: %{public}d", err);
        return -1;
    }
    if (!ITypesUtil::Unmarshal(reply, result)) {
        LOG_ERROR("fail to Unmarshal result");
        return -1;
    }
    return 0;
}

bool DataShareProxy::RegisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver)
{
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
        static_cast<uint32_t>(IDataShareInterfaceCode::CMD_REGISTER_OBSERVER), data, reply, option);
    if (result != ERR_NONE) {
        LOG_ERROR("SendRequest error, result=%{public}d", result);
        return false;
    }
    // the stub write bool value as int value to reply, 0 is false
    return reply.ReadInt32() == 0 ? false : true;
}

bool DataShareProxy::UnregisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver)
{
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
        static_cast<uint32_t>(IDataShareInterfaceCode::CMD_UNREGISTER_OBSERVER), data, reply, option);
    if (result != ERR_NONE) {
        LOG_ERROR("SendRequest error, result=%{public}d", result);
        return false;
    }
    // the stub write bool value as int value to reply, 0 is false
    return reply.ReadInt32() == 0 ? false : true;
}

bool DataShareProxy::NotifyChange(const Uri &uri)
{
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
        static_cast<uint32_t>(IDataShareInterfaceCode::CMD_NOTIFY_CHANGE), data, reply, option);
    if (result != ERR_NONE) {
        LOG_ERROR("SendRequest error, result=%{public}d", result);
        return false;
    }
    return true;
}

Uri DataShareProxy::NormalizeUri(const Uri &uri)
{
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
        static_cast<uint32_t>(IDataShareInterfaceCode::CMD_NORMALIZE_URI), data, reply, option);
    if (err != E_OK) {
        LOG_ERROR("NormalizeUri fail to SendRequest. err: %{public}d", err);
        return Uri("");
    }
    Uri info("");
    if (!ITypesUtil::Unmarshal(reply, info)) {
        LOG_ERROR("fail to Unmarshal index");
        return Uri("");
    }
    return info;
}

Uri DataShareProxy::DenormalizeUri(const Uri &uri)
{
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
        static_cast<uint32_t>(IDataShareInterfaceCode::CMD_DENORMALIZE_URI), data, reply, option);
    if (err != E_OK) {
        LOG_ERROR("DenormalizeUri fail to SendRequest. err: %{public}d", err);
        return Uri("");
    }

    Uri info("");
    if (!ITypesUtil::Unmarshal(reply, info)) {
        LOG_ERROR("fail to Unmarshal index");
        return Uri("");
    }
    return info;
}

bool DataShareProxy::CheckSize(const UpdateOperations &operations)
{
    size_t size = 0;
    for (const auto &it : operations) {
        size += it.second.size();
    }
    if (size > MAX_SIZE) {
        LOG_ERROR("operations size greater than limit");
        return false;
    }
    return true;
}
int32_t DataShareProxy::UserDefineFunc(
    MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    int32_t errCode = -1;
    int32_t err = Remote()->SendRequest(
        static_cast<uint32_t>(IDataShareInterfaceCode::CMD_USER_DEFINE_FUNC), data, reply, option);
    if (err != E_OK) {
        LOG_ERROR("UserDefineFunc fail to SendRequest. err: %{public}d", err);
        return err == PERMISSION_ERR ? PERMISSION_ERR_CODE : errCode;
    }
    return err;
}
} // namespace DataShare
} // namespace OHOS
