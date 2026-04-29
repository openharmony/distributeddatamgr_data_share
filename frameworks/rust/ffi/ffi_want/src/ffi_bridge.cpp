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

#include "ffi_want_bridge.h"
#include "wrapper.rs.h"
#include "element_name.h"

namespace ffi_want {

// === Uri shim functions ===
std::unique_ptr<Uri> parse_uri(rust::Str uri_str)
{
    return std::make_unique<Uri>(std::string(uri_str));
}

rust::String uri_to_string(const Uri& uri)
{
    return rust::String(uri.ToString());
}

rust::String uri_get_path(const Uri& uri)
{
    return rust::String(const_cast<Uri&>(uri).GetPath());
}

rust::String uri_get_host(const Uri& uri)
{
    return rust::String(const_cast<Uri&>(uri).GetHost());
}

rust::String uri_get_query(const Uri& uri)
{
    return rust::String(const_cast<Uri&>(uri).GetQuery());
}

rust::String uri_get_scheme(const Uri& uri)
{
    return rust::String(const_cast<Uri&>(uri).GetScheme());
}

// === Want shim functions ===
std::unique_ptr<Want> create_want()
{
    return std::make_unique<Want>();
}

void want_set_uri(Want& want, rust::Str uri_str)
{
    want.SetUri(Uri(std::string(uri_str)));
}

rust::String want_get_uri(const Want& want)
{
    return rust::String(const_cast<Want&>(want).GetUri().ToString());
}

void want_set_param(Want& want, rust::Str key, rust::Str value)
{
    want.SetParam(std::string(key), std::string(value));
}

rust::String want_get_param(const Want& want, rust::Str key)
{
    auto val = const_cast<Want&>(want).GetStringParam(std::string(key));
    return rust::String(val);
}

void want_set_element(Want& want, rust::Str bundle, rust::Str ability)
{
    OHOS::AppExecFwk::ElementName element;
    element.SetBundleName(std::string(bundle));
    element.SetAbilityName(std::string(ability));
    want.SetElement(element);
}

rust::String want_get_action(const Want& want)
{
    return rust::String(const_cast<Want&>(want).GetAction());
}

void want_set_action(Want& want, rust::Str action)
{
    want.SetAction(std::string(action));
}

rust::String want_get_element_bundle_name(const Want& want)
{
    auto element = const_cast<Want&>(want).GetElement();
    return rust::String(element.GetBundleName());
}

rust::String want_get_element_ability_name(const Want& want)
{
    auto element = const_cast<Want&>(want).GetElement();
    return rust::String(element.GetAbilityName());
}

} // namespace ffi_want
