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
static constexpr int DDMS_ID = 0xd;
static constexpr int DATA_SHARE_ID = 5;
enum BizScene {
    CREATE_DATASHARE_HELPER = 1,
    HANDLE_DATASHARE_OPERATIONS = 2,
    OBSERVER_MANAGER = 3,
    NOTIFY_OBSERVER_DATA_CHANGE = 4,
    TEMPLATE_DATA_MANAGER = 5,
    TEMPLATE_DATA_CHANGE = 6,
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
    PROXY_CONNECT_EXT = 6,
    PROXY_CALL_RDB = 7,
};

enum ObserverManagerStage {
    REGISTER_OBSERVER = 1,
    UNREGISTER_OBSERVER = 2,
};

enum NotifyObserverDataChangeStage {
    NOTIFY_DATA_CHANGE = 1,
};

enum TemplateDataManagerStage {
    SUBSCRIBE_PUBLISHED_DATA = 1,
    SUBSCRIBE_RDB_DATA = 2,
    UNSUBSCRIBE_PUBLISHED_DATA = 3,
    UNSUBSCRIBE_RDB_DATA = 4,
};

enum TemplateDataChangeStage {
    PUBLISHED_DATA_CHANGE = 1,
    RDB_DATA_CHANGE = 2,
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
    CONNECT_EXTENSION_ERROR = (DDMS_ID << 21) | (DATA_SHARE_ID << 16),
    CREATE_HELPER_ERROR,
    CREATE_SHARE_BLOCK_ERROR,
    SHARE_BLOCK_FULL,
    DISTRIBUTEDDATA_NOT_START,
    GET_BMS_FAILED,
    SUPPLIER_ERROR,
    URI_ERROR,
    PERMISSION_DENIED_ERROR,
    GET_RDB_STORE_ERROR,
    META_DATA_NOT_EXISTS,
    EMPTY_OBSERVER_ERROR,
    NOTIFY_ERROR,
    DATA_OBS_EMPTY_ERROR,
    SILENT_PROXY_DISABLE,
    ADD_TEMPLATE_ERROR,
    NOT_SUBCRIBE_ERROR,
    CREATE_DELEGATE_ERROR,
    INSERT_RDB_ERROR,
    QUERY_RDB_ERROR,
    DELETE_RDB_ERROR,
    UPDATE_RDB_ERROR,
    GET_BUNDLE_INFP_FAILED,
    QUERY_ERROR,
    EMPTY_PARAM_ERROR,
    INVALID_PARAM_ERROR,
    DATA_SHARE_DIED_ERROR,
};

static constexpr char DOMAIN[] = "DISTDATAMGR";
constexpr const char* EVENT_NAME = "DISTRIBUTED_DATA_SHARE_BEHAVIOR";
constexpr const char* ORG_PKG = "distributeddata";
constexpr const char* BIZ_STATE = "BIZ_STATE";
constexpr const char* ERROR_CODE = "ERROR_CODE";
static constexpr HiviewDFX::HiSysEvent::EventType TYPE = HiviewDFX::HiSysEvent::EventType::BEHAVIOR;

#define RADAR_REPORT(bizScene, bizStage, stageRes, ...)                                    \
({                                                                                         \
    HiSysEventWrite(RadarReporter::DOMAIN, RadarReporter::EVENT_NAME, RadarReporter::TYPE, \
        "ORG_PKG", RadarReporter::ORG_PKG, "FUNC", __FUNCTION__,                           \
        "BIZ_SCENE", bizScene, "BIZ_STAGE", bizStage, "STAGE_RES", stageRes,               \
        ##__VA_ARGS__);                                                                    \
})

class RadarReport final {
public:
    RadarReport(int32_t bizScene, int32_t bizStage)
    {
        RADAR_REPORT(bizScene, bizStage,
            RadarReporter::SUCCESS, RadarReporter::BIZ_STATE, RadarReporter::START);
        bizScene_ = bizScene;
        bizStage_ = bizStage;
    }

    ~RadarReport()
    {
        if (errorCode_ != 0) {
            RADAR_REPORT(bizScene_, bizStage_, RadarReporter::FAILED, RadarReporter::BIZ_STATE,
                RadarReporter::FINISHED, RadarReporter::ERROR_CODE, errorCode_);
        } else {
            RADAR_REPORT(bizScene_, bizStage_,
                RadarReporter::SUCCESS, RadarReporter::BIZ_STATE, RadarReporter::FINISHED);
        }
    }

    void SetError(int32_t errorCode)
    {
        errorCode_ = errorCode;

    }
private:
    int32_t bizScene_;
    int32_t bizStage_;
    int32_t errorCode_;
};
} // namespace RadarReporter
} // namespace DataShare
} // namespace OHOS
#endif // DATASHARE_RADAR_REPORTER_H