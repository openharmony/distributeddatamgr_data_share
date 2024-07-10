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

#include <cinttypes>

#include "datashare_log.h"
#include "datashare_predicates_proxy.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace DataShare {
static constexpr std::chrono::milliseconds TIME_THRESHOLD = std::chrono::milliseconds(500);
EXTERN_C_START
/*
 * The module initialization.
 */
static napi_value Init(napi_env env, napi_value exports)
{
    LOG_DEBUG("Init DataSharePredicates");
    auto start = std::chrono::steady_clock::now();
    DataSharePredicatesProxy::Init(env, exports);
    auto finish = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
    if (duration >= TIME_THRESHOLD) {
        int64_t milliseconds = duration.count();
        LOG_WARN("Init is too slow, cost:%{public}" PRIi64 "ms", milliseconds);
    }
    return exports;
}
EXTERN_C_END

/*
 * The module definition.
 */
static napi_module _module = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = Init,
    .nm_modname = "data.dataSharePredicates",
    .nm_priv = ((void *)0),
    .reserved = {0}
};

/*
 * The module registration.
 */
static __attribute__((constructor)) void RegisterModule(void)
{
    napi_module_register(&_module);
}
}  // namespace DataShare
}  // namespace OHOS