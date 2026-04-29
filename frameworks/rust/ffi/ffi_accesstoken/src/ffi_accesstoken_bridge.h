// Copyright (c) 2026 Huawei Device Co., Ltd.
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef DATASHARE_FFI_ACCESSTOKEN_BRIDGE_H
#define DATASHARE_FFI_ACCESSTOKEN_BRIDGE_H

#include "cxx.h"
#include <cstdint>

namespace OHOS::Security::AccessToken {

// CallingInfo, FfiHapTokenInfo, FfiNativeTokenInfo are CXX shared structs —
// defined in the generated wrapper.rs.h before this header is included.
struct CallingInfo;
struct FfiHapTokenInfo;
struct FfiNativeTokenInfo;

uint64_t get_calling_token_id();
uint64_t get_calling_process_token_id();
int32_t verify_permission(uint64_t token_id, rust::Str permission_name);
int32_t get_token_type(uint64_t token_id);
rust::String get_bundle_name(uint64_t token_id);
CallingInfo get_calling_info();
FfiHapTokenInfo get_hap_token_info(uint64_t token_id);
FfiNativeTokenInfo get_native_token_info(uint64_t token_id);
uint64_t get_hap_token_id(int32_t user_id, rust::Str bundle_name, int32_t app_index);

} // namespace OHOS::Security::AccessToken

#endif // DATASHARE_FFI_ACCESSTOKEN_BRIDGE_H
