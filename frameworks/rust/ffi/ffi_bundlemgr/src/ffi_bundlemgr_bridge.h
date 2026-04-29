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

#ifndef DATASHARE_FFI_BUNDLEMGR_BRIDGE_H
#define DATASHARE_FFI_BUNDLEMGR_BRIDGE_H

#include "cxx.h"
#include <cstdint>

namespace OHOS::AppExecFwk {

struct FfiBundleInfo;

int32_t get_uid_for_bundle(rust::Str bundle_name);
rust::String get_bundle_name_for_uid(int32_t uid);
bool is_system_app(rust::Str bundle_name);
FfiBundleInfo get_bundle_info(rust::Str bundle_name);
FfiBundleInfo get_bundle_info_with_flags(rust::Str bundle_name, int32_t flags, int32_t user_id);
FfiBundleInfo get_bundle_info_for_self(int32_t flags);

} // namespace OHOS::AppExecFwk

#endif // DATASHARE_FFI_BUNDLEMGR_BRIDGE_H
