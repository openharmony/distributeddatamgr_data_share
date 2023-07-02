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

#include "ikvstore_data_service.h"
#include "datashare_log.h"

using namespace OHOS::DistributedShare::DataShare::IKvStoreDataInterfaceCode;

namespace OHOS {
namespace DataShare {
DataShareKvServiceProxy::DataShareKvServiceProxy(const sptr<IRemoteObject> &impl)
    : IRemoteProxy<IKvStoreDataService>(impl)
{
    LOG_DEBUG("Init data service proxy.");
}

sptr<IRemoteObject> DataShareKvServiceProxy::GetFeatureInterface(const std::string &name)
{
    LOG_INFO("GetDataShareService enter.");
    MessageParcel data;
    if (!data.WriteInterfaceToken(DataShareKvServiceProxy::GetDescriptor())) {
        LOG_ERROR("Write descriptor failed");
        return nullptr;
    }
    if (!data.WriteString(name)) {
        LOG_ERROR("Write name failed");
        return nullptr;
    }

    MessageParcel reply;
    MessageOption mo { MessageOption::TF_SYNC };
    int32_t error = Remote()->SendRequest(static_cast<uint32_t>(GET_FEATURE_INTERFACE), data, reply, mo);
    if (error != 0) {
        LOG_ERROR("SendRequest returned %{public}d", error);
        return nullptr;
    }
    auto remoteObject = reply.ReadRemoteObject();
    if (remoteObject == nullptr) {
        LOG_ERROR("Remote object is nullptr!");
        return nullptr;
    }
    return remoteObject;
}
}
}
