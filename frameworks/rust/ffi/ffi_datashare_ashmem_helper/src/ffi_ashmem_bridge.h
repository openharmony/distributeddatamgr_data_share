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

#ifndef DATASHARE_FFI_ASHMEM_BRIDGE_H
#define DATASHARE_FFI_ASHMEM_BRIDGE_H

#include "ashmem.h"
#include "cxx.h"

#include <cstdint>
#include <memory>

namespace OHOS::DataShare::FfiAshmem {

// Build a managed shared_ptr<OHOS::Ashmem> from an existing fd + size.
// The wrapping matches the cxx `SharedPtr<Ashmem>` returned from
// `utils_rust::ashmem::ffi::CreateAshmemStd` (i.e. std::shared_ptr).
std::shared_ptr<OHOS::Ashmem> AshmemFromFd(int32_t fd, int32_t size);

} // namespace OHOS::DataShare::FfiAshmem

#endif // DATASHARE_FFI_ASHMEM_BRIDGE_H
