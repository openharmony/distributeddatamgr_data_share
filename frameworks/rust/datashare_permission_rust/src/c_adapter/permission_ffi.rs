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

//! C-FFI exports for datashare_permission.
//!
//! Provides C-compatible function exports for the DataShare permission
//! verification API. These functions are called by C++ code through
//! the shared library.

use std::ffi::CStr;
use std::os::raw::c_char;

/// Verify permission for a caller (C-FFI)
///
/// # Safety
/// `uri` and `permission` must be valid, null-terminated C strings.
#[no_mangle]
pub unsafe extern "C" fn DataSharePermission_VerifyPermission(
    token_id: u32,
    uri: *const c_char,
    permission: *const c_char,
) -> bool {
    if uri.is_null() || permission.is_null() {
        return false;
    }
    let uri = unsafe { CStr::from_ptr(uri) };
    let permission = unsafe { CStr::from_ptr(permission) };

    let uri_str = uri.to_str().unwrap_or("");
    let perm_str = permission.to_str().unwrap_or("");

    let perm = crate::permission::DataSharePermission::new();
    perm.verify_permission(token_id, uri_str, perm_str)
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::ffi::CString;

    #[test]
    fn test_ffi_verify_permission() {
        let uri = CString::new("datashare:///test").unwrap();
        let perm = CString::new("read").unwrap();
        let result =
            unsafe { DataSharePermission_VerifyPermission(1, uri.as_ptr(), perm.as_ptr()) };
        assert!(result);
    }

    #[test]
    fn test_ffi_verify_permission_null_uri() {
        let perm = CString::new("read").unwrap();
        let result =
            unsafe { DataSharePermission_VerifyPermission(1, std::ptr::null(), perm.as_ptr()) };
        assert!(!result);
    }
}
