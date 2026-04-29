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

#ifndef FFI_WANT_BRIDGE_H
#define FFI_WANT_BRIDGE_H

#include "cxx.h"
#include <memory>
#include "uri.h"
#include "want.h"

// Use neutral namespace to avoid conflicts between OHOS::Uri and OHOS::AAFwk::Want
namespace ffi_want {

// CXX opaque types — aliases to the real C++ types
using Uri = OHOS::Uri;
using Want = OHOS::AAFwk::Want;

std::unique_ptr<Uri> parse_uri(rust::Str uri_str);
rust::String uri_to_string(const Uri& uri);
rust::String uri_get_path(const Uri& uri);
rust::String uri_get_host(const Uri& uri);
rust::String uri_get_query(const Uri& uri);
rust::String uri_get_scheme(const Uri& uri);

std::unique_ptr<Want> create_want();
void want_set_uri(Want& want, rust::Str uri_str);
rust::String want_get_uri(const Want& want);
void want_set_param(Want& want, rust::Str key, rust::Str value);
rust::String want_get_param(const Want& want, rust::Str key);
void want_set_element(Want& want, rust::Str bundle, rust::Str ability);
rust::String want_get_action(const Want& want);
void want_set_action(Want& want, rust::Str action);
rust::String want_get_element_bundle_name(const Want& want);
rust::String want_get_element_ability_name(const Want& want);

} // namespace ffi_want

#endif // FFI_WANT_BRIDGE_H
