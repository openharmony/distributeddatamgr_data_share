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

#include "datashare_stub.h"

#include "data_ability_observer_interface.h"
#include "datashare_itypes_utils.h"
#include "datashare_log.h"
#include "ipc_types.h"
#include "ishared_result_set.h"
#include "unistd.h"

namespace OHOS {
namespace DataShare {
constexpr int DEFAULT_NUMBER = -1;
constexpr int PERMISSION_ERROR_NUMBER = -2;
DataShareStub::DataShareStub()
{
    stubFuncMap_[CMD_GET_FILE_TYPES] = &DataShareStub::CmdGetFileTypes;
    stubFuncMap_[CMD_OPEN_FILE] = &DataShareStub::CmdOpenFile;
    stubFuncMap_[CMD_OPEN_RAW_FILE] = &DataShareStub::CmdOpenRawFile;
    stubFuncMap_[CMD_INSERT] = &DataShareStub::CmdInsert;
    stubFuncMap_[CMD_UPDATE] = &DataShareStub::CmdUpdate;
    stubFuncMap_[CMD_DELETE] = &DataShareStub::CmdDelete;
    stubFuncMap_[CMD_QUERY] = &DataShareStub::CmdQuery;
    stubFuncMap_[CMD_GET_TYPE] = &DataShareStub::CmdGetType;
    stubFuncMap_[CMD_BATCH_INSERT] = &DataShareStub::CmdBatchInsert;
    stubFuncMap_[CMD_REGISTER_OBSERVER] = &DataShareStub::CmdRegisterObserver;
    stubFuncMap_[CMD_UNREGISTER_OBSERVER] = &DataShareStub::CmdUnregisterObserver;
    stubFuncMap_[CMD_NOTIFY_CHANGE] = &DataShareStub::CmdNotifyChange;
    stubFuncMap_[CMD_NORMALIZE_URI] = &DataShareStub::CmdNormalizeUri;
    stubFuncMap_[CMD_DENORMALIZE_URI] = &DataShareStub::CmdDenormalizeUri;
}

DataShareStub::~DataShareStub()
{
    stubFuncMap_.clear();
}

int DataShareStub::OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply,
    MessageOption& option)
{
    std::u16string descriptor = DataShareStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        LOG_INFO("local descriptor is not equal to remote");
        return ERR_INVALID_STATE;
    }

    const auto &itFunc = stubFuncMap_.find(code);
    if (itFunc != stubFuncMap_.end()) {
        return (this->*(itFunc->second))(data, reply);
    }

    LOG_INFO("remote request unhandled: %{public}d", code);
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

ErrCode DataShareStub::CmdGetFileTypes(MessageParcel &data, MessageParcel &reply)
{
    Uri uri("");
    std::string mimeTypeFilter;
    if (!ITypesUtil::Unmarshal(data, uri, mimeTypeFilter)) {
        LOG_ERROR("Unmarshalling value is nullptr");
        return ERR_INVALID_VALUE;
    }
    if (mimeTypeFilter.empty()) {
        LOG_ERROR("mimeTypeFilter is nullptr");
        return ERR_INVALID_VALUE;
    }
    std::vector<std::string> types = GetFileTypes(uri, mimeTypeFilter);
    if (!ITypesUtil::Marshal(reply, types)) {
        LOG_ERROR("Marshal value is nullptr");
        return ERR_INVALID_VALUE;
    }
    return DATA_SHARE_NO_ERROR;
}

ErrCode DataShareStub::CmdOpenFile(MessageParcel &data, MessageParcel &reply)
{
    Uri uri("");
    std::string mode;
    if (!ITypesUtil::Unmarshal(data, uri, mode)) {
        LOG_ERROR("Unmarshalling value is nullptr");
        return ERR_INVALID_VALUE;
    }
    if (mode.empty()) {
        LOG_ERROR("mode is nullptr");
        return ERR_INVALID_VALUE;
    }
    int fd = OpenFile(uri, mode);
    if (fd < 0) {
        LOG_ERROR("OpenFile fail, fd is %{pubilc}d", fd);
        return ERR_INVALID_VALUE;
    }
    if (!reply.WriteFileDescriptor(fd)) {
        LOG_ERROR("fail to WriteFileDescriptor fd");
        close(fd);
        return ERR_INVALID_VALUE;
    }
    close(fd);
    return DATA_SHARE_NO_ERROR;
}

ErrCode DataShareStub::CmdOpenRawFile(MessageParcel &data, MessageParcel &reply)
{
    Uri uri("");
    std::string mode;
    if (!ITypesUtil::Unmarshal(data, uri, mode)) {
        LOG_ERROR("Unmarshalling value is nullptr");
        return ERR_INVALID_VALUE;
    }
    int fd = OpenRawFile(uri, mode);
    if (!ITypesUtil::Marshal(reply, fd)) {
        LOG_ERROR("Marshal value is nullptr");
        return ERR_INVALID_VALUE;
    }
    return DATA_SHARE_NO_ERROR;
}

ErrCode DataShareStub::CmdInsert(MessageParcel &data, MessageParcel &reply)
{
    Uri uri("");
    DataShareValuesBucket value;
    if (!ITypesUtil::Unmarshal(data, uri, value)) {
        LOG_ERROR("Unmarshalling value is nullptr");
        return ERR_INVALID_VALUE;
    }
    int index = Insert(uri, value);
    if (index == DEFAULT_NUMBER) {
        LOG_ERROR("Insert inner error");
        return ERR_INVALID_VALUE;
    } else if (index == PERMISSION_ERROR_NUMBER) {
        LOG_ERROR("Insert permission error");
        return ERR_PERMISSION_DENIED;
    }
    if (!reply.WriteInt32(index)) {
        LOG_ERROR("fail to WriteInt32 index");
        return ERR_INVALID_VALUE;
    }
    LOG_INFO("DataShareStub::CmdInsertInner end");
    return DATA_SHARE_NO_ERROR;
}

ErrCode DataShareStub::CmdUpdate(MessageParcel &data, MessageParcel &reply)
{
    Uri uri("");
    DataSharePredicates predicates;
    DataShareValuesBucket value;
    if (!ITypesUtil::Unmarshal(data, uri, predicates, value)) {
        LOG_ERROR("Unmarshalling predicates is nullptr");
        return ERR_INVALID_VALUE;
    }
    int index = Update(uri, predicates, value);
    if (index == DEFAULT_NUMBER) {
        LOG_ERROR("Update inner error");
        return ERR_INVALID_VALUE;
    } else if (index == PERMISSION_ERROR_NUMBER) {
        LOG_ERROR("Update permission error");
        return ERR_PERMISSION_DENIED;
    }
    if (!reply.WriteInt32(index)) {
        LOG_ERROR("fail to WriteInt32 index");
        return ERR_INVALID_VALUE;
    }
    return DATA_SHARE_NO_ERROR;
}

ErrCode DataShareStub::CmdDelete(MessageParcel &data, MessageParcel &reply)
{
    Uri uri("");
    DataSharePredicates predicates;
    if (!ITypesUtil::Unmarshal(data, uri, predicates)) {
        LOG_ERROR("Unmarshalling predicates is nullptr");
        return ERR_INVALID_VALUE;
    }
    int index = Delete(uri, predicates);
    if (index == DEFAULT_NUMBER) {
        LOG_ERROR("Delete inner error");
        return ERR_INVALID_VALUE;
    } else if (index == PERMISSION_ERROR_NUMBER) {
        LOG_ERROR("Delete permission error");
        return ERR_PERMISSION_DENIED;
    }
    if (!reply.WriteInt32(index)) {
        LOG_ERROR("fail to WriteInt32 index");
        return ERR_INVALID_VALUE;
    }
    return DATA_SHARE_NO_ERROR;
}

ErrCode DataShareStub::CmdQuery(MessageParcel &data, MessageParcel &reply)
{
    Uri uri("");
    DataSharePredicates predicates;
    std::vector<std::string> columns;
    if (!ITypesUtil::Unmarshal(data, uri, predicates, columns)) {
        LOG_ERROR("Unmarshalling predicates is nullptr");
        return ERR_INVALID_VALUE;
    }
    DatashareBusinessError businessError;
    auto resultSet = Query(uri, predicates, columns, businessError);
    auto result = ISharedResultSet::WriteToParcel(std::move(resultSet), reply);
    reply.WriteInt32(businessError.GetCode());
    reply.WriteString(businessError.GetMessage());
    if (result == nullptr) {
        LOG_ERROR("!resultSet->Marshalling(reply)");
        return ERR_INVALID_VALUE;
    }
    return DATA_SHARE_NO_ERROR;
}

ErrCode DataShareStub::CmdGetType(MessageParcel &data, MessageParcel &reply)
{
    Uri uri("");
    if (!ITypesUtil::Unmarshal(data, uri)) {
        LOG_ERROR("Unmarshalling predicates is nullptr");
        return ERR_INVALID_VALUE;
    }
    std::string type = GetType(uri);
    if (!reply.WriteString(type)) {
        LOG_ERROR("fail to WriteString type");
        return ERR_INVALID_VALUE;
    }
    return DATA_SHARE_NO_ERROR;
}

ErrCode DataShareStub::CmdBatchInsert(MessageParcel &data, MessageParcel &reply)
{
    Uri uri("");
    std::vector<DataShareValuesBucket> values;
    if (!ITypesUtil::Unmarshal(data, uri, values)) {
        LOG_ERROR("Unmarshalling predicates is nullptr");
        return ERR_INVALID_VALUE;
    }

    int ret = BatchInsert(uri, values);
    if (ret == DEFAULT_NUMBER) {
        LOG_ERROR("BatchInsert inner error");
        return ERR_INVALID_VALUE;
    } else if (ret == PERMISSION_ERROR_NUMBER) {
        LOG_ERROR("BatchInsert permission error");
        return ERR_PERMISSION_DENIED;
    }
    if (!reply.WriteInt32(ret)) {
        LOG_ERROR("fail to WriteInt32 ret");
        return ERR_INVALID_VALUE;
    }
    return DATA_SHARE_NO_ERROR;
}

ErrCode DataShareStub::CmdRegisterObserver(MessageParcel &data, MessageParcel &reply)
{
    Uri uri("");
    sptr<IRemoteObject> observer;
    if (!ITypesUtil::Unmarshal(data, uri, observer)) {
        LOG_ERROR("Unmarshalling predicates is nullptr");
        return ERR_INVALID_VALUE;
    }
    auto obServer = iface_cast<AAFwk::IDataAbilityObserver>(observer);
    if (obServer == nullptr) {
        LOG_ERROR("obServer is nullptr");
        return ERR_INVALID_VALUE;
    }

    bool ret = RegisterObserver(uri, obServer);
    if (!reply.WriteInt32(ret)) {
        LOG_ERROR("fail to WriteInt32 ret");
        return ERR_INVALID_VALUE;
    }
    return DATA_SHARE_NO_ERROR;
}

ErrCode DataShareStub::CmdUnregisterObserver(MessageParcel &data, MessageParcel &reply)
{
    Uri uri("");
    sptr<IRemoteObject> observer;
    if (!ITypesUtil::Unmarshal(data, uri, observer)) {
        LOG_ERROR("Unmarshalling predicates is nullptr");
        return ERR_INVALID_VALUE;
    }
    auto obServer = iface_cast<AAFwk::IDataAbilityObserver>(observer);
    if (obServer == nullptr) {
        LOG_ERROR("obServer is nullptr");
        return ERR_INVALID_VALUE;
    }

    bool ret = UnregisterObserver(uri, obServer);
    if (!reply.WriteInt32(ret)) {
        LOG_ERROR("fail to WriteInt32 ret");
        return ERR_INVALID_VALUE;
    }
    return DATA_SHARE_NO_ERROR;
}

ErrCode DataShareStub::CmdNotifyChange(MessageParcel &data, MessageParcel &reply)
{
    Uri uri("");
    if (!ITypesUtil::Unmarshal(data, uri)) {
        LOG_ERROR("Unmarshalling predicates is nullptr");
        return ERR_INVALID_VALUE;
    }

    bool ret = NotifyChange(uri);
    if (!reply.WriteInt32(ret)) {
        LOG_ERROR("fail to WriteInt32 ret");
        return ERR_INVALID_VALUE;
    }
    return DATA_SHARE_NO_ERROR;
}

ErrCode DataShareStub::CmdNormalizeUri(MessageParcel &data, MessageParcel &reply)
{
    Uri uri("");
    if (!ITypesUtil::Unmarshal(data, uri)) {
        LOG_ERROR("Unmarshalling predicates is nullptr");
        return ERR_INVALID_VALUE;
    }
    auto ret = NormalizeUri(uri);
    if (!ITypesUtil::Marshal(reply, ret)) {
        LOG_ERROR("Write to message parcel failed!");
        return ERR_INVALID_VALUE;
    }
    return DATA_SHARE_NO_ERROR;
}

ErrCode DataShareStub::CmdDenormalizeUri(MessageParcel &data, MessageParcel &reply)
{
    Uri uri("");
    if (!ITypesUtil::Unmarshal(data, uri)) {
        LOG_ERROR("Unmarshalling predicates is nullptr");
        return ERR_INVALID_VALUE;
    }

    auto ret = DenormalizeUri(uri);
    if (!ITypesUtil::Marshal(reply, ret)) {
        LOG_ERROR("Write to message parcel failed!");
        return ERR_INVALID_VALUE;
    }
    return DATA_SHARE_NO_ERROR;
}

int DataShareStub::AddQueryTemplate(const std::string &uri, int64_t subscriberId, Template &tpl)
{
    return -1;
}

int DataShareStub::DelQueryTemplate(const std::string &uri, int64_t subscriberId)
{
    return -1;
}

std::vector<OperationResult> DataShareStub::Publish(const Data &data, const std::string &bundleName)
{
    return {};
}

Data DataShareStub::GetPublishedData(const std::string &bundleName)
{
    return {};
}

std::vector<OperationResult> DataShareStub::SubscribeRdbData(const std::vector<std::string> &uris,
    const TemplateId &templateId, const sptr<IDataProxyRdbObserver> &observer)
{
    return {};
}

std::vector<OperationResult> DataShareStub::UnSubscribeRdbData(
    const std::vector<std::string> &uris, const TemplateId &templateId)
{
    return {};
}

std::vector<OperationResult> DataShareStub::EnableSubscribeRdbData(
    const std::vector<std::string> &uris, const TemplateId &templateId)
{
    return {};
}

std::vector<OperationResult> DataShareStub::DisableSubscribeRdbData(
    const std::vector<std::string> &uris, const TemplateId &templateId)
{
    return {};
}

std::vector<OperationResult> DataShareStub::SubscribePublishedData(const std::vector<std::string> &uris,
    int64_t subscriberId, const sptr<IDataProxyPublishedDataObserver> &observer)
{
    return {};
}

std::vector<OperationResult> DataShareStub::UnSubscribePublishedData(
    const std::vector<std::string> &uris, int64_t subscriberId)
{
    return {};
}

std::vector<OperationResult> DataShareStub::EnableSubscribePublishedData(
    const std::vector<std::string> &uris, int64_t subscriberId)
{
    return {};
}

std::vector<OperationResult> DataShareStub::DisableSubscribePublishedData(
    const std::vector<std::string> &uris, int64_t subscriberId)
{
    return {};
}
} // namespace DataShare
} // namespace OHOS