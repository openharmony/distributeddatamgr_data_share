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

#ifndef FFI_HICOLLIE_BRIDGE_H
#define FFI_HICOLLIE_BRIDGE_H

#include "cxx.h"

namespace OHOS::HiviewDFX {

int32_t xcollie_set_timer(rust::Str tag, uint32_t timeout_seconds, uint32_t flags);
void xcollie_cancel_timer(int32_t id);

} // namespace OHOS::HiviewDFX

#endif // FFI_HICOLLIE_BRIDGE_H
