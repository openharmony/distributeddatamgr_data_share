/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef DATASHARE_RADAR_REPORTER_H
#define DATASHARE_RADAR_REPORTER_H

#include "hisysevent.h"

namespace OHOS {
namespace DataShare {
namespace RadarReporter {
using namespace OHOS::HiviewDFX;
enum BizSence {
    CREATE_DATASHARE_HELPER = 1,
    HANDLE_DATASHARE_OPERATIONS = 2,
    REGISTER_DATA_CHANGE = 3,
    TEMPLATE_DATA_CHANGE = 4,
};

enum CreateDataShareHelperStage {
    CREATE_HELPER = 1,
    DISTRIBUTEDDATA_START = 2,
};

enum HandleDataShareOperationsStage {
    EXT_REQUEST = 1,
    GET_BMS = 2,
    PROXY_GET_SUPPLIER = 3,
    PROXY_PERMISSION = 4,
    PROXY_MATEDATA_EXISTS = 5,
    PROXY_CALL_RDB = 6,
    PROXY_CONNECT_EXT = 7,
};

enum RegisterDataChangeStage {
    REGISTER_OBSERVER = 1,
    UNREGISTER_OBSERVER = 2,
    NOTIFY_DATA_CHANGE = 3,
};

enum TemplateDataChangeStage {
    SUBSCRIBE_PUBLISHED_DATA = 1,
    SUBSCRIBE_RDB_DATA = 2,
    UNSUBSCRIBE_PUBLISHED_DATA = 3,
    UNSUBSCRIBE_RDB_DATA = 4,
    PUBLISHED_DATA_CHANGE = 5,
    RDB_DATA_CHANGE = 6,
};

enum StageRes {
    IDLE = 0,
    SUCCESS = 1,
    FAILED = 2,
    CANCELLED = 3,
};

enum BizState {
    START = 1,
    FINISHED = 2,
};

enum ErrorCode {
    CONNECT_EXTENSION_ERROR = 0,
    CREATE_HELPER_ERROR = 1,
    CREATE_SHARE_BLOCK_ERROR = 2,
    SHARE_BLOCK_FULL = 3,
    DISTRIBUTEDDATA_NOT_START = 4,
    GET_BMS_FAILED = 5,
    SUPPLIER_ERROR = 6,
    URI_ERROR = 7,
    PERMISSION_DENIED_ERROR = 8,
    GET_RDB_STORE_ERROR = 9,
    META_DATA_NOT_EXISTS = 10,
    REGISTER_ERROR = 11,
    UNREGISTER_ERROR = 12,
    NOTIFY_ERROR = 13,
    SILENT_PROXY_DISABLE = 14,
    ADD_TEMPLATE_ERROR = 15,
    NOT_SUBCRIBE_ERROR = 16,
    CREATE_DELEGATE_ERROR = 17,
    INSERT_RDB_ERROR = 18,
    QUERY_RDB_ERROR = 19,
    DELETE_RDB_ERROR = 20,
    UPDATE_RDB_ERROR = 21,
    GET_BUNDLE_INFP_FAILED = 22,
};
static constexpr char DOMAIN[] = "DATA_SHARE";
const std::string EVENT_NAME = "DISTDATAMGR_DATA_SHARE_BEHAVIOR";
const std::string ORG_PKG = "data_share";
const std::string BIZ_STATE = "BIZ_STATE";
const std::string ERROR_CODE = "ERROR_CODE";
static constexpr HiviewDFX::HiSysEvent::EventType TYPE = HiviewDFX::HiSysEvent::EventType::BEHAVIOR;

#define RADAR_REPORT(bizSence, bizStage, stageRes, ...)                                    \
({                                                                                         \
    HiSysEventWrite(RadarReporter::DOMAIN, RadarReporter::EVENT_NAME, RadarReporter::TYPE, \
        "ORG_PKG", RadarReporter::ORG_PKG, "FUNC", __FUNCTION__,                           \
        "BIZ_SECNE", bizSence, "BIZ_STAGE", bizStage, "STAGE_RES", stageRes,               \
        ##__VA_ARGS__);                                                                   \
})
} // namespace RadarReporter
} // namespace DataShare
} // namespace OHOS
#endif // DATASHARE_RADAR_REPORTER_H