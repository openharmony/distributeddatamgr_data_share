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

// C++ bridge implementation for HiTrace FFI.

#include "ffi_hitrace_bridge.h"
#include "wrapper.rs.h"
#include "hitrace_meter.h"
#include "hitracechain.h"

namespace OHOS::HiviewDFX {

// HiTraceId in this namespace is the CXX shared struct (from wrapper.rs.h).
// The system HiTraceId class lives in OHOS::HiviewDFX as well, so we use
// the fully qualified chain class and convert explicitly.

FfiHiTraceId begin_section(rust::Str name)
{
    std::string nameStr(name.data(), name.size());
    ::StartTrace(HITRACE_TAG_DISTRIBUTEDDATA, nameStr);
    auto id = HiTraceChain::GetId();
    FfiHiTraceId result;
    result.chain_id = id.GetChainId();
    result.span_id = id.GetSpanId();
    result.parent_span_id = id.GetParentSpanId();
    result.flags = static_cast<uint32_t>(id.GetFlags());
    result.is_valid = id.IsValid();
    return result;
}

void end_section()
{
    ::FinishTrace(HITRACE_TAG_DISTRIBUTEDDATA);
}

FfiHiTraceId create_trace_id(rust::Str name)
{
    std::string nameStr(name.data(), name.size());
    auto id = HiTraceChain::Begin(nameStr, HITRACE_FLAG_DEFAULT);
    FfiHiTraceId result;
    result.chain_id = id.GetChainId();
    result.span_id = id.GetSpanId();
    result.parent_span_id = id.GetParentSpanId();
    result.flags = static_cast<uint32_t>(id.GetFlags());
    result.is_valid = id.IsValid();
    return result;
}

FfiHiTraceId get_current_trace_id()
{
    auto id = HiTraceChain::GetId();
    FfiHiTraceId result;
    result.chain_id = id.GetChainId();
    result.span_id = id.GetSpanId();
    result.parent_span_id = id.GetParentSpanId();
    result.flags = static_cast<uint32_t>(id.GetFlags());
    result.is_valid = id.IsValid();
    return result;
}

bool is_trace_enabled()
{
    return HiTraceChain::GetId().IsValid();
}

} // namespace OHOS::HiviewDFX
