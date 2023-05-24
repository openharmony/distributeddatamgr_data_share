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

#ifndef DISTRIBUTEDDATAFWK_IDATA_SHARE_SERVICE_H
#define DISTRIBUTEDDATAFWK_IDATA_SHARE_SERVICE_H

#include <string>

#include "base_proxy.h"
#include "iremote_broker.h"

namespace OHOS::DataShare {
class IDataShareService : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.DataShare.IDataShareService");
    enum {
        DATA_SHARE_SERVICE_CMD_INSERT,
        DATA_SHARE_SERVICE_CMD_DELETE,
        DATA_SHARE_SERVICE_CMD_UPDATE,
        DATA_SHARE_SERVICE_CMD_QUERY,
        DATA_SHARE_SERVICE_CMD_ADD_TEMPLATE,
        DATA_SHARE_SERVICE_CMD_DEL_TEMPLATE,
        DATA_SHARE_SERVICE_CMD_PUBLISH,
        DATA_SHARE_SERVICE_CMD_GET_DATA,
        DATA_SHARE_SERVICE_CMD_SUBSCRIBE_RDB,
        DATA_SHARE_SERVICE_CMD_UNSUBSCRIBE_RDB,
        DATA_SHARE_SERVICE_CMD_ENABLE_SUBSCRIBE_RDB,
        DATA_SHARE_SERVICE_CMD_DISABLE_SUBSCRIBE_RDB,
        DATA_SHARE_SERVICE_CMD_SUBSCRIBE_PUBLISHED,
        DATA_SHARE_SERVICE_CMD_UNSUBSCRIBE_PUBLISHED,
        DATA_SHARE_SERVICE_CMD_ENABLE_SUBSCRIBE_PUBLISHED,
        DATA_SHARE_SERVICE_CMD_DISABLE_SUBSCRIBE_PUBLISHED,
        DATA_SHARE_SERVICE_CMD_NOTIFY,
        DATA_SHARE_SERVICE_CMD_MAX
    };

    enum {
        DATA_SHARE_ERROR = -1,
        DATA_SHARE_OK = 0,
    };
};

class IKvStoreDataService : public IRemoteBroker {
public:
    enum { GET_FEATURE_INTERFACE = 0 };

    enum {
        DATA_SHARE_ERROR = -1,
        DATA_SHARE_OK = 0,
    };

    virtual sptr<IRemoteObject> GetFeatureInterface(const std::string &name) = 0;

    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.DistributedKv.IKvStoreDataService");
};

} // namespace OHOS::DataShare
#endif
