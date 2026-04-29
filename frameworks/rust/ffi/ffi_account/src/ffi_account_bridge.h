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

#ifndef FFI_ACCOUNT_BRIDGE_H
#define FFI_ACCOUNT_BRIDGE_H

#include "cxx.h"
#include <cstdint>

namespace OHOS::DistributedData {

bool is_deactivating(int32_t user_id);
bool is_verified(int32_t user_id);
int32_t get_user_by_token(uint32_t token_id);
int32_t query_foreground_user_id();
bool is_caller_system_app();

} // namespace OHOS::DistributedData

#endif // FFI_ACCOUNT_BRIDGE_H
