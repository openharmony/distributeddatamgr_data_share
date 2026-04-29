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

#ifndef FFI_HISYSEVENT_BRIDGE_H
#define FFI_HISYSEVENT_BRIDGE_H

#include "cxx.h"

namespace OHOS::HiviewDFX {

// EventType and KeyValue are CXX shared types defined in wrapper.rs.
// Forward-declare them here; full definitions come from wrapper.rs.h in the .cpp.
enum class EventType : uint8_t;
struct KeyValue;

void write_sys_event(
    rust::Str domain,
    rust::Str name,
    EventType event_type,
    rust::Slice<const KeyValue> key_values);

} // namespace OHOS::HiviewDFX

#endif // FFI_HISYSEVENT_BRIDGE_H
