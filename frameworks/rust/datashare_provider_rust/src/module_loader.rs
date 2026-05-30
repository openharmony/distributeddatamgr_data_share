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

//! DataShareExtAbilityModuleLoader — Module loader singleton.
//!
//! Corresponds to C++ `DataShareExtAbilityModuleLoader` in
//! `frameworks/native/provider/src/datashare_ext_ability_module_loader.cpp` (49 lines).
//!
//! Provides a singleton module loader that registers a factory function
//! for creating DataShareExtAbility instances. Used for dynamic loading
//! via `OHOS_EXTENSION_GetExtensionModule()`.

use std::collections::HashMap;
use std::ffi::c_void;
use std::sync::OnceLock;

/// Module type identifier for DataShareExtAbility
const MODULE_TYPE: &str = "5";

/// Module name for DataShareExtAbility
const MODULE_NAME: &str = "DataShareExtAbility";

/// Extension module descriptor (placeholder)
///
/// Corresponds to C++ `ExtensionModule` struct used by the
/// extension loading framework.
#[repr(C)]
pub struct ExtensionModule {
    /// Module name
    pub name: [u8; 64],
    /// Module type
    pub module_type: i32,
}

/// Custom creator function type
type CreatorFn = unsafe extern "C" fn(*mut c_void) -> *mut c_void;

/// DataShareExtAbilityModuleLoader — Singleton module loader.
///
/// Corresponds to C++ `DataShareExtAbilityModuleLoader`.
///
/// Registers a factory function for creating DataShareExtAbility
/// instances, used during dynamic module loading.
pub struct DataShareExtAbilityModuleLoader {
    /// Custom creator function (set via SetCreator)
    creator: std::sync::Mutex<Option<CreatorFn>>,
}

/// Global singleton instance
static INSTANCE: OnceLock<DataShareExtAbilityModuleLoader> = OnceLock::new();

impl DataShareExtAbilityModuleLoader {
    /// Get the singleton instance.
    ///
    /// Corresponds to C++ `DataShareExtAbilityModuleLoader::GetInstance()`.
    pub fn get_instance() -> &'static DataShareExtAbilityModuleLoader {
        INSTANCE.get_or_init(|| DataShareExtAbilityModuleLoader {
            creator: std::sync::Mutex::new(None),
        })
    }

    /// Create a DataShareExtAbility instance using the registered factory.
    ///
    /// Corresponds to C++ `DataShareExtAbilityModuleLoader::Create(runtime)`.
    ///
    /// TODO: Delegate to DataShareExtAbility::Create(runtime)
    pub fn create(&self, runtime: *mut c_void) -> *mut c_void {
        let creator = self.creator.lock().unwrap();
        if let Some(creator_fn) = *creator {
            // Use custom creator if set
            unsafe { creator_fn(runtime) }
        } else {
            // TODO: Call DataShareExtAbility::Create(runtime)
            std::ptr::null_mut()
        }
    }

    /// Get module parameters (metadata).
    ///
    /// Corresponds to C++ `DataShareExtAbilityModuleLoader::GetParams()`.
    ///
    /// Returns a map with type="5", name="DataShareExtAbility".
    pub fn get_params(&self) -> HashMap<String, String> {
        let mut params = HashMap::new();
        params.insert("type".to_string(), MODULE_TYPE.to_string());
        params.insert("name".to_string(), MODULE_NAME.to_string());
        params
    }

    /// Set a custom creator function.
    pub fn set_creator(&self, creator: CreatorFn) {
        let mut guard = self.creator.lock().unwrap();
        *guard = Some(creator);
    }
}

/// Get the extension module descriptor (C-FFI export).
///
/// Corresponds to C++ `OHOS_EXTENSION_GetExtensionModule()`.
///
/// Called by the extension loading framework to get the module descriptor.
///
/// # Safety
/// Returns a static reference. Caller must not free the returned pointer.
#[no_mangle]
pub extern "C" fn OHOS_EXTENSION_GetExtensionModule() -> *const c_void {
    // TODO: Return actual ExtensionModule pointer
    // For now, ensure the singleton is initialized
    let _ = DataShareExtAbilityModuleLoader::get_instance();
    std::ptr::null()
}

/// Set a custom creator function (C-FFI export).
///
/// Corresponds to C++ `SetCreator(creator)`.
///
/// # Safety
/// `creator` must be a valid function pointer.
#[no_mangle]
pub unsafe extern "C" fn SetCreator(creator: CreatorFn) {
    DataShareExtAbilityModuleLoader::get_instance().set_creator(creator);
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_get_instance() {
        let instance = DataShareExtAbilityModuleLoader::get_instance();
        let params = instance.get_params();
        assert_eq!(params.get("type").unwrap(), "5");
        assert_eq!(params.get("name").unwrap(), "DataShareExtAbility");
    }

    #[test]
    fn test_singleton_identity() {
        let a = DataShareExtAbilityModuleLoader::get_instance();
        let b = DataShareExtAbilityModuleLoader::get_instance();
        assert!(std::ptr::eq(a, b));
    }

    #[test]
    fn test_get_params() {
        let loader = DataShareExtAbilityModuleLoader::get_instance();
        let params = loader.get_params();
        assert_eq!(params.len(), 2);
        assert!(params.contains_key("type"));
        assert!(params.contains_key("name"));
    }

    #[test]
    fn test_create_without_creator() {
        let loader = DataShareExtAbilityModuleLoader::get_instance();
        let result = loader.create(std::ptr::null_mut());
        assert!(result.is_null());
    }

    #[test]
    fn test_get_extension_module() {
        let result = OHOS_EXTENSION_GetExtensionModule();
        assert!(result.is_null()); // placeholder
    }
}
