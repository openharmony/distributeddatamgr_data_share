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

#define LOG_TAG "data_proxy_observer_stub"

#include "data_proxy_observer_stub.h"

#include "dataproxy_handle_common.h"
#include "datashare_itypes_utils.h"
#include "datashare_log.h"

namespace OHOS {
namespace DataShare {
static constexpr int MAX_VEC_SIZE = 1024;
static constexpr int REQUEST_CODE = 0;
int RdbObserverStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    std::u16string descriptor = RdbObserverStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        LOG_ERROR("local descriptor is not equal to remote");
        return ERR_INVALID_STATE;
    }
    if (code != REQUEST_CODE) {
        LOG_ERROR("not support code:%u", code);
        return ERR_INVALID_STATE;
    }
    RdbChangeNode changeNode;
    if (!ITypesUtil::Unmarshal(data, changeNode)) {
        LOG_ERROR("Unmarshalling  is nullptr");
        return ERR_INVALID_VALUE;
    }
    OnChangeFromRdb(changeNode);
    return ERR_OK;
}

int RdbObserverStub::ReadAshmem(RdbChangeNode &changeNode, const void **data, int size, int &offset)
{
    if (changeNode.memory_ == nullptr) {
        LOG_ERROR("changeNode memory is nullptr.");
        return E_ERROR;
    }
    const void *read = changeNode.memory_->ReadFromAshmem(size, offset);
    if (read == nullptr) {
        LOG_ERROR("failed to read from ashmem.");
        changeNode.memory_->UnmapAshmem();
        changeNode.memory_->CloseAshmem();
        changeNode.memory_ = nullptr;
        return E_ERROR;
    }
    *data = read;
    offset += size;
    return E_OK;
}

int RdbObserverStub::DeserializeDataFromAshmem(RdbChangeNode &changeNode)
{
    if (changeNode.memory_ == nullptr) {
        LOG_ERROR("changeNode.memory_ is null.");
        return E_ERROR;
    }
    bool mapRet = changeNode.memory_->MapReadAndWriteAshmem();
    if (!mapRet) {
        LOG_ERROR("failed to map read and write ashmem, ret=%{public}d", mapRet);
        changeNode.memory_->CloseAshmem();
        changeNode.memory_ = nullptr;
        return E_ERROR;
    }
    LOG_DEBUG("receive data size: %{public}d", changeNode.size_);
    // Read data size
    int intLen = 4;
    int offset = 0;
    const int *vecLenRead;
    if (ReadAshmem(changeNode, (const void **)&vecLenRead, intLen, offset) != E_OK) {
        LOG_ERROR("failed to read data with len %{public}d, offset %{public}d.", intLen, offset);
        return E_ERROR;
    }
    int vecLen = *vecLenRead;
    if (vecLen > MAX_VEC_SIZE || vecLen < 0) {
        LOG_ERROR("vecLen is invalid: %{public}d", vecLen);
        return E_ERROR;
    }
    // Read data
    for (int i = 0; i < vecLen; i++) {
        const int *dataLenRead;
        if (ReadAshmem(changeNode, (const void **)&dataLenRead, intLen, offset) != E_OK) {
            LOG_ERROR(
                "failed to read data with index %{public}d, len %{public}d, offset %{public}d.", i, intLen, offset);
            return E_ERROR;
        }
        int dataLen = *dataLenRead;
        const char *dataRead;
        if (ReadAshmem(changeNode, (const void **)&dataRead, dataLen, offset) != E_OK) {
            LOG_ERROR(
                "failed to read data with index %{public}d, len %{public}d, offset %{public}d.", i, dataLen, offset);
            return E_ERROR;
        }
        std::string data(dataRead, dataLen);
        changeNode.data_.push_back(data);
    }
    return E_OK;
}

int RdbObserverStub::RecoverRdbChangeNodeData(RdbChangeNode &changeNode)
{
    int ret = E_OK;
    if (changeNode.isSharedMemory_) {
        // Recover form Ashmem
        if (DeserializeDataFromAshmem(changeNode) != E_OK) {
            LOG_ERROR("failed to deserialize data from ashmem.");
            ret = E_ERROR;
        }
        if (changeNode.memory_ != nullptr) {
            changeNode.memory_->UnmapAshmem();
            changeNode.memory_->CloseAshmem();
            changeNode.memory_ = nullptr;
        }
        changeNode.isSharedMemory_ = false;
        changeNode.size_ = 0;
    }
    return ret;
}

void RdbObserverStub::OnChangeFromRdb(RdbChangeNode &changeNode)
{
    std::lock_guard<decltype(mutex_)> lock(mutex_);
    if (RecoverRdbChangeNodeData(changeNode) != E_OK) {
        LOG_ERROR("failed to recover RdbChangeNode data.");
        return;
    }
    if (callback_) {
        callback_(changeNode);
    }
}

RdbObserverStub::RdbObserverStub(RdbCallback callback) : callback_(callback)
{
}

RdbObserverStub::~RdbObserverStub()
{
    ClearCallback();
}

void RdbObserverStub::ClearCallback()
{
    std::lock_guard<decltype(mutex_)> lock(mutex_);
    callback_ = nullptr;
}

int PublishedDataObserverStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    std::u16string descriptor = PublishedDataObserverStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        LOG_ERROR("local descriptor is not equal to remote");
        return ERR_INVALID_STATE;
    }
    if (code != REQUEST_CODE) {
        LOG_ERROR("not support code:%u", code);
        return ERR_INVALID_STATE;
    }
    PublishedDataChangeNode changeNode;
    if (!ITypesUtil::Unmarshal(data, changeNode)) {
        LOG_ERROR("Unmarshalling  is nullptr");
        return ERR_INVALID_VALUE;
    }
    OnChangeFromPublishedData(changeNode);
    return ERR_OK;
}

void PublishedDataObserverStub::OnChangeFromPublishedData(PublishedDataChangeNode &changeNode)
{
    std::lock_guard<decltype(mutex_)> lock(mutex_);
    if (callback_) {
        callback_(changeNode);
    }
}

void PublishedDataObserverStub::ClearCallback()
{
    std::lock_guard<decltype(mutex_)> lock(mutex_);
    callback_ = nullptr;
}

PublishedDataObserverStub::~PublishedDataObserverStub()
{
    ClearCallback();
}

int ProxyDataObserverStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    std::u16string descriptor = PublishedDataObserverStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        LOG_ERROR("local descriptor is not equal to remote");
        return ERR_INVALID_STATE;
    }
    if (code != REQUEST_CODE) {
        LOG_ERROR("not support code:%u", code);
        return ERR_INVALID_STATE;
    }
    std::vector<DataProxyChangeInfo> changeInfo;
    if (!ITypesUtil::Unmarshal(data, changeInfo)) {
        LOG_ERROR("Unmarshalling  is nullptr");
        return ERR_INVALID_VALUE;
    }
    OnChangeFromProxyData(changeInfo);
    return ERR_OK;
}

void ProxyDataObserverStub::OnChangeFromProxyData(std::vector<DataProxyChangeInfo> &changeInfo)
{
    std::lock_guard<decltype(mutex_)> lock(mutex_);
    if (callback_) {
        callback_(changeInfo);
    }
}

void ProxyDataObserverStub::ClearCallback()
{
    std::lock_guard<decltype(mutex_)> lock(mutex_);
    callback_ = nullptr;
}

ProxyDataObserverStub::~ProxyDataObserverStub()
{
    ClearCallback();
}
} // namespace DataShare
} // namespace OHOS
