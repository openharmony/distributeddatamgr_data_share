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

//! CXX bridge wrapper for `MessageParcel::ReadRawData` / `WriteRawData`.
//!
//! Wire format (matching `MessageParcel::WriteRawData` in
//! `foundation/communication/ipc/ipc/native/src/core/framework/source/message_parcel.cpp`):
//! a 4-byte int32 length prefix followed by raw bytes (inline when size
//! ≤ `MIN_RAWDATA_SIZE` = 32 KiB, or an ashmem file descriptor otherwise).
//! This is strictly incompatible with `WriteUInt8Vector` /
//! `ReadUInt8Vector` (which is what `MsgParcel::read::<Vec<u8>>()` maps to).

use std::mem;
use std::pin::Pin;

use ipc::parcel::MsgParcel;
use ipc::{IpcResult, IpcStatusCode};

#[cxx::bridge(namespace = "OHOS::DistributedData")]
mod ffi {
    unsafe extern "C++" {
        include!("ffi_ipc_raw_data_bridge.h");

        #[namespace = "OHOS"]
        type MessageParcel = ipc::cxx_share::MessageParcel;

        /// Read exactly `length` bytes via `MessageParcel::ReadRawData` and
        /// copy into the pre-sized `out` buffer. `out.size()` must equal
        /// `length` before the call.
        fn DsRawDataRead(
            parcel: Pin<&mut MessageParcel>,
            length: usize,
            out: &mut Vec<u8>,
        ) -> bool;

        /// Write `data` via `MessageParcel::WriteRawData`.
        fn DsRawDataWrite(parcel: Pin<&mut MessageParcel>, data: &[u8]) -> bool;
    }
}

/// Run a closure with a `Pin<&mut MessageParcel>` derived from a
/// `&mut MsgParcel`.
///
/// Two paths:
/// - **Unique parcel** (`MsgParcel::new()` self-allocated): `pin_mut()`
///   succeeds and returns the pinned reference directly. The parcel
///   stays `ParcelMem::Unique`, so a subsequent
///   [`RemoteObj::send_request`] still works.
/// - **Borrow parcel** (handed in by IPC stub on the handler path):
///   `pin_mut()` returns `None`, so we fall back to a swap-trick that
///   round-trips through `into_raw` / `from_ptr`. The parcel ends up as
///   `ParcelMem::Borrow`, which is the original variant for this path
///   so no behaviour is lost.
fn with_pinned_parcel<T>(
    parcel: &mut MsgParcel,
    f: impl FnOnce(Pin<&mut ffi::MessageParcel>) -> T,
) -> T {
    if let Some(pinned) = parcel.pin_mut() {
        return f(pinned);
    }
    let taken = mem::replace(parcel, MsgParcel::new());
    let raw = taken.into_raw();
    let result = {
        // SAFETY: `raw` is valid and exclusively borrowed for the call
        // because the caller holds `&mut MsgParcel`, preventing any
        // concurrent access.
        let pinned = unsafe { Pin::new_unchecked(&mut *raw) };
        f(pinned)
    };
    *parcel = MsgParcel::from_ptr(raw);
    result
}

/// Read exactly `length` bytes from the parcel via
/// `MessageParcel::ReadRawData(length)`. The wire format is an int32
/// length prefix followed by raw bytes (or an ashmem file descriptor
/// when `length > 32 KiB`). Returns the bytes on success.
pub fn read_raw_data(parcel: &mut MsgParcel, length: usize) -> IpcResult<Vec<u8>> {
    if length == 0 {
        return Err(IpcStatusCode::Failed);
    }
    let mut buf = vec![0u8; length];
    let ok = with_pinned_parcel(parcel, |pinned| {
        ffi::DsRawDataRead(pinned, length, &mut buf)
    });
    if ok { Ok(buf) } else { Err(IpcStatusCode::Failed) }
}

/// Write `data` to the parcel via
/// `MessageParcel::WriteRawData(data.as_ptr(), data.len())`.
pub fn write_raw_data(parcel: &mut MsgParcel, data: &[u8]) -> IpcResult<()> {
    if data.is_empty() {
        return Err(IpcStatusCode::Failed);
    }
    let ok = with_pinned_parcel(parcel, |pinned| ffi::DsRawDataWrite(pinned, data));
    if ok { Ok(()) } else { Err(IpcStatusCode::Failed) }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn raw_data_roundtrip_small() {
        let mut parcel = MsgParcel::new();
        let data = b"hello raw data".to_vec();
        write_raw_data(&mut parcel, &data).unwrap();
        let out = read_raw_data(&mut parcel, data.len()).unwrap();
        assert_eq!(out, data);
    }

    #[test]
    fn raw_data_roundtrip_large_ashmem_path() {
        // Payload > 32 KiB triggers the ashmem code path inside
        // MessageParcel::WriteRawData / ReadRawData.
        let mut parcel = MsgParcel::new();
        let data: Vec<u8> = (0..(64 * 1024)).map(|i| (i % 251) as u8).collect();
        write_raw_data(&mut parcel, &data).unwrap();
        let out = read_raw_data(&mut parcel, data.len()).unwrap();
        assert_eq!(out, data);
    }

    #[test]
    fn empty_input_is_error() {
        let mut parcel = MsgParcel::new();
        assert!(write_raw_data(&mut parcel, &[]).is_err());
        assert!(read_raw_data(&mut parcel, 0).is_err());
    }
}
