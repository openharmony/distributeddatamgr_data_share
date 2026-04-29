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

#ifndef DATASHARE_FFI_DATAOBS_BRIDGE_H
#define DATASHARE_FFI_DATAOBS_BRIDGE_H

#include "cxx.h"
#include <cstdint>

// Use neutral namespace to avoid ChangeInfo collision with OHOS::AAFwk::ChangeInfo.
// Observer sptr management is kept entirely on the C++ side.
namespace ffi_dataobs {

// DataObsCallback is a Rust extern type — CXX generates its declaration
struct DataObsCallback;

int64_t register_observer(rust::Str uri, rust::Box<DataObsCallback> callback, int32_t user_id);
int32_t unregister_observer(int64_t handle);
int32_t notify_change(rust::Str uri, int32_t user_id);

int64_t register_observer_ext(
    rust::Str uri, rust::Box<DataObsCallback> callback, bool is_descendants);
int32_t unregister_observer_ext(int64_t handle);
int32_t notify_change_ext(uint32_t change_type, rust::Str uri);

} // namespace ffi_dataobs

#endif // DATASHARE_FFI_DATAOBS_BRIDGE_H
