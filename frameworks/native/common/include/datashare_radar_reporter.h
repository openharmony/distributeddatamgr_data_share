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
enum BizScene {
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
    PROXY_QUERY = 3,
    PROXY_GET_SUPPLIER = 4,
    PROXY_PERMISSION = 5,
    PROXY_MATEDATA_EXISTS = 6,
    PROXY_CALL_RDB = 7,
    PROXY_CONNECT_EXT = 8,
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
    CONNECT_EXTENSION_ERROR = 27590656,
    CREATE_HELPER_ERROR = 27590657,
    CREATE_SHARE_BLOCK_ERROR = 27590658,
    SHARE_BLOCK_FULL = 27590659,
    DISTRIBUTEDDATA_NOT_START = 27590660,
    GET_BMS_FAILED = 27590661,
    SUPPLIER_ERROR = 27590662,
    URI_ERROR = 27590663,
    PERMISSION_DENIED_ERROR = 27590664,
    GET_RDB_STORE_ERROR = 27590665,
    META_DATA_NOT_EXISTS = 27590666,
    REGISTER_ERROR = 27590667,
    UNREGISTER_ERROR = 27590668,
    NOTIFY_ERROR = 27590669,
    SILENT_PROXY_DISABLE = 27590670,
    ADD_TEMPLATE_ERROR = 27590671,
    NOT_SUBCRIBE_ERROR = 27590672,
    CREATE_DELEGATE_ERROR = 27590673,
    INSERT_RDB_ERROR = 27590674,
    QUERY_RDB_ERROR = 27590675,
    DELETE_RDB_ERROR = 27590676,
    UPDATE_RDB_ERROR = 27590677,
    GET_BUNDLE_INFP_FAILED = 27590678,
    QUERY_ERROR = 27590679,
};
static constexpr char DOMAIN[] = "DISTDATAMGR";
const std::string EVENT_NAME = "DISTRIBUTED_DATA_SHARE_BEHAVIOR";
const std::string ORG_PKG = "distributeddata";
const std::string BIZ_STATE = "BIZ_STATE";
const std::string ERROR_CODE = "ERROR_CODE";
static constexpr HiviewDFX::HiSysEvent::EventType TYPE = HiviewDFX::HiSysEvent::EventType::BEHAVIOR;

#define RADAR_REPORT(bizScene, bizStage, stageRes, ...)                                    \
({                                                                                         \
    HiSysEventWrite(RadarReporter::DOMAIN, RadarReporter::EVENT_NAME, RadarReporter::TYPE, \
        "ORG_PKG", RadarReporter::ORG_PKG, "FUNC", __FUNCTION__,                           \
        "BIZ_SCENE", bizScene, "BIZ_STAGE", bizStage, "STAGE_RES", stageRes,               \
        ##__VA_ARGS__);                                                                   \
})
} // namespace RadarReporter
} // namespace DataShare
} // namespace OHOS
#endif // DATASHARE_RADAR_REPORTER_H