/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef DATASHARE_BOOT_TIME_ADAPTOR_H
#define DATASHARE_BOOT_TIME_ADAPTOR_H

#include <cinttypes>

#include "datashare_log.h"

namespace OHOS {
namespace DataShare {
namespace {
static constexpr int MILLI_TO_SEC = 1000LL;
static constexpr int NANO_TO_SEC = 1000000000LL;
constexpr int32_t NANO_TO_MILLI = NANO_TO_SEC / MILLI_TO_SEC;
}

class BootTimeAdaptor {
public:
    BootTimeAdaptor() = default;
    int64_t GetBootTimeMs()
    {
        int64_t time = -1;
        struct timespec tv {};
        if (clock_gettime(CLOCK_BOOTTIME, &tv) < 0) {
            LOG_ERROR("get  boot time failed, errno: %{public}s", strerror(errno));
            return time;
        }
        time = tv.tv_sec * MILLI_TO_SEC + tv.tv_nsec / NANO_TO_MILLI;
        return time;
    };
};
} // namespace DataShare
} // namespace OHOS
#endif