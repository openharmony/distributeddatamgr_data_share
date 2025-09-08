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

#define LOG_TAG "ishared_result_set_stub"

#include <cinttypes>
#include "ishared_result_set_stub.h"
#include "datashare_log.h"
#include "datashare_errno.h"
#include "ipc_skeleton.h"
#include "string_ex.h"

namespace OHOS::DataShare {
std::function<sptr<ISharedResultSet>(std::shared_ptr<DataShareResultSet>,
    MessageParcel &)> ISharedResultSet::providerCreator_ = ISharedResultSetStub::CreateStub;
constexpr ISharedResultSetStub::Handler ISharedResultSetStub::handlers[static_cast<uint32_t>(ResultCode::FUNC_BUTT)];
const std::chrono::milliseconds TIME_THRESHOLD = std::chrono::milliseconds(500);

sptr<ISharedResultSet> ISharedResultSetStub::CreateStub(std::shared_ptr<DataShareResultSet> result,
    OHOS::MessageParcel &parcel)
{
    if (result == nullptr) {
        LOG_ERROR("result is nullptr");
        return nullptr;
    }
    sptr<ISharedResultSet> stub = new (std::nothrow) ISharedResultSetStub(result);
    if (stub == nullptr) {
        LOG_ERROR("stub is nullptr");
        return stub;
    }
    parcel.WriteRemoteObject(stub->AsObject());
    result->Marshalling(parcel);
    return stub;
}

ISharedResultSetStub::ISharedResultSetStub(std::shared_ptr<DataShareResultSet> resultSet)
    : resultSet_(std::move(resultSet))
{
}

ISharedResultSetStub::~ISharedResultSetStub()
{
}

int ISharedResultSetStub::OnRemoteRequest(uint32_t code, OHOS::MessageParcel &data,
    OHOS::MessageParcel &reply, OHOS::MessageOption &option)
{
    std::u16string descriptor = ISharedResultSetStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        LOG_ERROR("IPC descriptor is  not equal to remote, GetDescriptor = %{public}s, ReadDescriptor = %{public}s",
            Str16ToStr8(descriptor).c_str(), Str16ToStr8(remoteDescriptor).c_str());
        return INVALID_FD;
    }

    auto callingPid = IPCSkeleton::GetCallingPid();
    if (code >= static_cast<uint32_t>(ResultCode::FUNC_BUTT)) {
        LOG_ERROR("method code(%{public}d) out of range, callingPid:%{public}d", code, callingPid);
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    Handler handler = handlers[code];
    if (handler == nullptr) {
        LOG_ERROR("method code(%{public}d) is not support, callingPid:%{public}d", code, callingPid);
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    auto start = std::chrono::steady_clock::now();
    int ret = (this->*handler)(data, reply);
    auto finish = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
    if (duration >= TIME_THRESHOLD) {
        int64_t milliseconds = duration.count();
        LOG_WARN("over time, code:%{public}u callingPid:%{public}d, cost:%{public}" PRIi64 "ms",
            code, callingPid, milliseconds);
    }
    return ret;
}

int ISharedResultSetStub::HandleGetRowCountRequest(MessageParcel &data, MessageParcel &reply)
{
    int count = -1;
    int errCode = resultSet_->GetRowCount(count);
    reply.WriteInt32(errCode);
    if (errCode == E_OK) {
        reply.WriteInt32(count);
    }
    LOG_DEBUG("errCode %{public}d", errCode);
    return NO_ERROR;
}

int ISharedResultSetStub::HandleGetAllColumnNamesRequest(MessageParcel &data, MessageParcel &reply)
{
    std::vector<std::string> names;
    int errCode = resultSet_->GetAllColumnNames(names);
    reply.WriteInt32(errCode);
    if (errCode == E_OK) {
        reply.WriteStringVector(names);
    }
    LOG_DEBUG("errCode %{public}d", errCode);
    return NO_ERROR;
}

int ISharedResultSetStub::HandleOnGoRequest(MessageParcel &data, MessageParcel &reply)
{
    int oldRow = data.ReadInt32();
    int newRow = data.ReadInt32();
    int cachedIndex = 0;
    bool ret = resultSet_->OnGo(oldRow, newRow, &cachedIndex);
    if (!ret) {
        reply.WriteInt32(-1);
    } else {
        reply.WriteInt32(cachedIndex);
    }
    LOG_DEBUG("HandleOnGoRequest call %{public}d", cachedIndex);
    return NO_ERROR;
}

int ISharedResultSetStub::HandleCloseRequest(MessageParcel &data, MessageParcel &reply)
{
    int errCode = resultSet_->Close();
    reply.WriteInt32(errCode);
    LOG_DEBUG("errCode %{public}d", errCode);
    return NO_ERROR;
}
} // namespace OHOS::DataShare