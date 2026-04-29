/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef FFI_IPC_RAW_DATA_BRIDGE_H
#define FFI_IPC_RAW_DATA_BRIDGE_H

#include "cxx.h"
#include <cstddef>
#include <cstdint>

namespace OHOS {
class MessageParcel;
}

namespace OHOS::DistributedData {

/**
 * Read `length` bytes via `MessageParcel::ReadRawData` and copy into `out`.
 * `out.size()` must equal `length` (the Rust side pre-sizes the vector).
 * Returns true on success, false otherwise.
 */
bool DsRawDataRead(OHOS::MessageParcel &parcel, std::size_t length,
                   rust::Vec<std::uint8_t> &out);

/**
 * Write `data` via `MessageParcel::WriteRawData(data.data(), data.size())`.
 * Returns true on success, false otherwise.
 */
bool DsRawDataWrite(OHOS::MessageParcel &parcel,
                    rust::Slice<const std::uint8_t> data);

} // namespace OHOS::DistributedData

#endif // FFI_IPC_RAW_DATA_BRIDGE_H
