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

// C++ bridge implementation for HiSysEvent FFI.

#include "ffi_hisysevent_bridge.h"
#include "wrapper.rs.h"
#include "hisysevent.h"
#include "securec.h"
#include <string>
#include <vector>

namespace OHOS::HiviewDFX {

void write_sys_event(
    rust::Str domain,
    rust::Str name,
    EventType event_type,
    rust::Slice<const KeyValue> key_values)
{
    std::string domain_str(domain.data(), domain.size());
    std::string name_str(name.data(), name.size());

    // Map the CXX shared enum values to the C API event type.
    // CXX enum (wrapper.rs): BEHAVIOR=0, SECURITY=1, STATISTIC=2, FAULT=3.
    // C API: HISYSEVENT_FAULT=1, HISYSEVENT_STATISTIC=2, HISYSEVENT_SECURITY=3, HISYSEVENT_BEHAVIOR=4.
    HiSysEventEventType c_type;
    switch (event_type) {
        case EventType::BEHAVIOR:   c_type = HISYSEVENT_BEHAVIOR;  break;
        case EventType::SECURITY:   c_type = HISYSEVENT_SECURITY;  break;
        case EventType::STATISTIC:  c_type = HISYSEVENT_STATISTIC; break;
        case EventType::FAULT:      c_type = HISYSEVENT_FAULT;     break;
        default:                    c_type = HISYSEVENT_BEHAVIOR;  break;
    }

    if (key_values.size() == 0) {
        HiSysEvent_Write(__FUNCTION__, __LINE__,
            domain_str.c_str(), name_str.c_str(), c_type, nullptr, 0);
        return;
    }

    // Keep std::string copies of values alive for the duration of the Write call.
    std::vector<std::string> value_strings;
    value_strings.reserve(key_values.size());
    for (size_t i = 0; i < key_values.size(); ++i) {
        const rust::String &v = key_values[i].value;
        value_strings.emplace_back(v.data(), v.size());
    }

    std::vector<HiSysEventParam> params;
    params.resize(key_values.size());
    for (size_t i = 0; i < key_values.size(); ++i) {
        const rust::String &k = key_values[i].key;
        std::string key_str(k.data(), k.size());
        size_t copy_len = key_str.size() < MAX_LENGTH_OF_PARAM_NAME
            ? key_str.size() : MAX_LENGTH_OF_PARAM_NAME - 1;
        (void)memset_s(params[i].name, sizeof(params[i].name), 0, sizeof(params[i].name));
        (void)memcpy_s(params[i].name, sizeof(params[i].name), key_str.c_str(), copy_len);
        params[i].t = HISYSEVENT_STRING;
        params[i].v.s = const_cast<char *>(value_strings[i].c_str());
        params[i].arraySize = 0;
    }

    HiSysEvent_Write(__FUNCTION__, __LINE__,
        domain_str.c_str(), name_str.c_str(), c_type,
        params.data(), params.size());
}

} // namespace OHOS::HiviewDFX
