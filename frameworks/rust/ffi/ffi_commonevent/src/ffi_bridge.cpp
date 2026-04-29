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

// C++ bridge implementation for CommonEvent FFI.

#include "ffi_commonevent_bridge.h"
#include "wrapper.rs.h"
#include "common_event_manager.h"
#include "matching_skills.h"

namespace OHOS::EventFwk {

CppCommonEventSubscriber::CppCommonEventSubscriber(
    const CommonEventSubscribeInfo &info,
    rust::Box<CommonEventCallback> cb)
    : CommonEventSubscriber(info), callback_(std::move(cb)) {}

CppCommonEventSubscriber::~CppCommonEventSubscriber() = default;

void CppCommonEventSubscriber::OnReceiveEvent(const CommonEventData &data)
{
    std::string action = data.GetWant().GetAction();
    callback_->on_receive(rust::Str(action.data(), action.size()));
}

bool publish_common_event(rust::Str event_name, int32_t code)
{
    Want want;
    want.SetAction(std::string(event_name));
    CommonEventData data(want);
    data.SetCode(code);
    return CommonEventManager::PublishCommonEvent(data);
}

std::shared_ptr<CppCommonEventSubscriber> subscribe_common_event(
    rust::Str event_name,
    rust::Box<CommonEventCallback> callback)
{
    MatchingSkills skills;
    skills.AddEvent(std::string(event_name));
    CommonEventSubscribeInfo info(skills);
    auto subscriber = std::make_shared<CppCommonEventSubscriber>(info, std::move(callback));
    if (!CommonEventManager::SubscribeCommonEvent(subscriber)) {
        return nullptr;
    }
    return subscriber;
}

bool unsubscribe_common_event(std::shared_ptr<CppCommonEventSubscriber> subscriber)
{
    if (subscriber == nullptr) {
        return false;
    }
    return CommonEventManager::UnSubscribeCommonEvent(subscriber);
}

} // namespace OHOS::EventFwk
