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

#ifndef I_DATASHARE_H
#define I_DATASHARE_H

#include <memory>
#include <string_ex.h>
#include <iremote_broker.h>

#include "uri.h"
#include "data_share_base_proxy.h"

namespace OHOS {

namespace DataShare {
class IDataShare : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.DataShare.IDataShare");

    enum {
        CMD_GET_FILE_TYPES = 1,
        CMD_OPEN_FILE = 2,
        CMD_OPEN_RAW_FILE = 3,
        CMD_INSERT = 4,
        CMD_UPDATE = 5,
        CMD_DELETE = 6,
        CMD_QUERY = 7,
        CMD_GET_TYPE = 8,
        CMD_BATCH_INSERT = 9,
        CMD_REGISTER_OBSERVER = 10,
        CMD_UNREGISTER_OBSERVER = 11,
        CMD_NOTIFY_CHANGE = 12,
        CMD_NORMALIZE_URI = 13,
        CMD_DENORMALIZE_URI = 14,
        CMD_EXECUTE_BATCH = 15,
    };
};
} // namespace DataShare
} // namespace OHOS
#endif // I_DATASHARE_H

