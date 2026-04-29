/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef FFI_EXTENSION_CONNECT_BRIDGE_H
#define FFI_EXTENSION_CONNECT_BRIDGE_H

#include "cxx.h"
#include <cstdint>

namespace OHOS::DataShare {

int32_t connect_extension(
    rust::Str uri, rust::Str bundle_name, int32_t user_id, size_t want_params_ptr);
int32_t disconnect_extension(rust::Str bundle_name);
bool has_active_connection(rust::Str bundle_name);
void schedule_disconnect(rust::Str bundle_name, int32_t delay_secs);
void init_executor(size_t executor_ptr);
size_t build_corruption_want_params(rust::Str bundle_name, rust::Str store_name);
void destroy_want_params(size_t ptr);

} // namespace OHOS::DataShare

#endif // FFI_EXTENSION_CONNECT_BRIDGE_H
