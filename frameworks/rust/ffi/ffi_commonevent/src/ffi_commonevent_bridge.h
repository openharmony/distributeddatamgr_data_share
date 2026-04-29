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

#ifndef FFI_COMMONEVENT_BRIDGE_H
#define FFI_COMMONEVENT_BRIDGE_H

#include "cxx.h"
#include <memory>
#include "common_event_subscriber.h"
#include "common_event_subscribe_info.h"
#include "common_event_data.h"

namespace OHOS::EventFwk {

// CommonEventCallback is a Rust extern type — CXX generates its declaration
// in wrapper.rs.h. Forward-declare here for function signatures.
struct CommonEventCallback;

// Full class definition needed for CXX static_assert(sizeof(T) > 0).
// Method bodies are in ffi_bridge.cpp (after wrapper.rs.h is included).
class CppCommonEventSubscriber : public CommonEventSubscriber {
public:
    CppCommonEventSubscriber(const CommonEventSubscribeInfo &info,
                             rust::Box<CommonEventCallback> cb);
    ~CppCommonEventSubscriber() override;
    void OnReceiveEvent(const CommonEventData &data) override;

private:
    rust::Box<CommonEventCallback> callback_;
};

std::shared_ptr<CppCommonEventSubscriber> subscribe_common_event(
    rust::Str event_name,
    rust::Box<CommonEventCallback> callback);

bool unsubscribe_common_event(std::shared_ptr<CppCommonEventSubscriber> subscriber);

bool publish_common_event(rust::Str event_name, int32_t code);

} // namespace OHOS::EventFwk

#endif // FFI_COMMONEVENT_BRIDGE_H
