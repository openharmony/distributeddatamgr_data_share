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
#include "base_proxy.h"

namespace OHOS {

namespace DataShare {
class IDataShare : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.DataShare.IDataShare");

    enum {
        CMD_GET_FILE_TYPES = 1,
        CMD_OPEN_FILE,
        CMD_OPEN_RAW_FILE,
        CMD_INSERT,
        CMD_UPDATE,
        CMD_DELETE,
        CMD_QUERY,
        CMD_GET_TYPE,
        CMD_BATCH_INSERT,
        CMD_REGISTER_OBSERVER,
        CMD_UNREGISTER_OBSERVER,
        CMD_NOTIFY_CHANGE,
        CMD_NORMALIZE_URI,
        CMD_DENORMALIZE_URI,
        CMD_EXECUTE_BATCH,
    };
};
} // namespace DataShare
} // namespace OHOS
#endif // I_DATASHARE_H

