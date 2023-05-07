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

#include "data_proxy_observer_stub.h"

#include "datashare_itypes_utils.h"
#include "datashare_log.h"

namespace OHOS {
namespace DataShare {
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

void RdbObserverStub::OnChangeFromRdb(const RdbChangeNode &changeNode)
{
    std::lock_guard<decltype(mutex_)> lock(mutex_);
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
} // namespace DataShare
} // namespace OHOS
