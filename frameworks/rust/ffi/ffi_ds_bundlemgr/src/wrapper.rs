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

use std::collections::HashMap;
use std::sync::Mutex;

#[cxx::bridge(namespace = "OHOS::DataShare")]
mod ffi {
    /// Allow-list entry for a proxy data node.
    struct BmsAllowListEntry {
        /// Application identifier string.
        app_identifier: String,
        /// Whether this entry restricts to the main app only.
        only_main: bool,
    }

    /// Table config entry for cross-user mode resolution.
    struct BmsTableConfigEntry {
        /// DataShare URI for this table config.
        uri: String,
        /// Cross-user access mode (0 = default).
        cross_user_mode: i32,
    }

    /// All fields of a single proxy data entry, returned in one FFI call.
    struct BmsProxyEntry {
        /// Proxy data URI.
        uri: String,
        /// Required read permission.
        read_permission: String,
        /// Required write permission.
        write_permission: String,
        /// HAP module name.
        module_name: String,
        /// Profile parsing result (0 = OK, 1 = not found, 2 = error).
        profile_result_code: i32,
        /// RDB store name from profile path.
        store_name: String,
        /// RDB table name from profile path.
        table_name: String,
        /// Data type (e.g. "rdb", "publishedData").
        data_type: String,
        /// Scope ("module" or "application").
        scope: String,
        /// Backup URI.
        backup: String,
        /// Extension URI for connecting the provider.
        ext_uri: String,
        /// Whether to derive store metadata from the URI.
        store_meta_data_from_uri: bool,
        /// Allow-list entries.
        allow_lists: Vec<BmsAllowListEntry>,
        /// Table config entries for cross-user mode.
        table_config: Vec<BmsTableConfigEntry>,
    }

    unsafe extern "C++" {
        include!("ffi_ds_bundlemgr_bridge.h");

        type BundleConfigCpp;

        fn get_bundle_info(
            bundle_name: &str,
            user_id: i32,
            app_index: i32,
        ) -> SharedPtr<BundleConfigCpp>;

        fn bundle_config_result_code(config: &BundleConfigCpp) -> i32;
        fn bundle_config_app_identifier(config: &BundleConfigCpp) -> String;
        fn bundle_config_name(config: &BundleConfigCpp) -> String;
        fn bundle_config_singleton(config: &BundleConfigCpp) -> bool;
        fn bundle_config_is_system_app(config: &BundleConfigCpp) -> bool;

        fn bundle_config_proxy_count(config: &BundleConfigCpp) -> i32;
        fn bundle_config_proxy_uri(config: &BundleConfigCpp, index: i32) -> String;
        fn bundle_config_proxy_read_perm(config: &BundleConfigCpp, index: i32) -> String;
        fn bundle_config_proxy_write_perm(config: &BundleConfigCpp, index: i32) -> String;
        fn bundle_config_proxy_module_name(config: &BundleConfigCpp, index: i32) -> String;
        fn bundle_config_proxy_profile_result_code(config: &BundleConfigCpp, index: i32) -> i32;
        fn bundle_config_proxy_store_name(config: &BundleConfigCpp, index: i32) -> String;
        fn bundle_config_proxy_table_name(config: &BundleConfigCpp, index: i32) -> String;
        fn bundle_config_proxy_type(config: &BundleConfigCpp, index: i32) -> String;
        fn bundle_config_proxy_scope(config: &BundleConfigCpp, index: i32) -> String;

        fn bundle_config_extension_count(config: &BundleConfigCpp) -> i32;
        fn bundle_config_extension_type(config: &BundleConfigCpp, index: i32) -> i32;
        fn bundle_config_extension_read_perm(config: &BundleConfigCpp, index: i32) -> String;
        fn bundle_config_extension_write_perm(config: &BundleConfigCpp, index: i32) -> String;
        fn bundle_config_extension_uri(config: &BundleConfigCpp, index: i32) -> String;
        fn bundle_config_extension_is_silent_enabled(config: &BundleConfigCpp, index: i32) -> bool;

        fn bundle_config_proxy_backup(config: &BundleConfigCpp, index: i32) -> String;
        fn bundle_config_proxy_ext_uri(config: &BundleConfigCpp, index: i32) -> String;
        fn bundle_config_proxy_store_meta_data_from_uri(
            config: &BundleConfigCpp,
            index: i32,
        ) -> bool;

        fn bundle_config_proxy_allow_list_count(
            config: &BundleConfigCpp,
            proxy_idx: i32,
        ) -> i32;
        fn bundle_config_proxy_allow_list_app_id(
            config: &BundleConfigCpp,
            proxy_idx: i32,
            list_idx: i32,
        ) -> String;
        fn bundle_config_proxy_allow_list_only_main(
            config: &BundleConfigCpp,
            proxy_idx: i32,
            list_idx: i32,
        ) -> bool;

        fn bundle_config_proxy_table_config_count(
            config: &BundleConfigCpp,
            proxy_idx: i32,
        ) -> i32;
        fn bundle_config_proxy_table_config_uri(
            config: &BundleConfigCpp,
            proxy_idx: i32,
            cfg_idx: i32,
        ) -> String;
        fn bundle_config_proxy_table_config_cross_mode(
            config: &BundleConfigCpp,
            proxy_idx: i32,
            cfg_idx: i32,
        ) -> i32;

        fn bundle_config_extension_profile_result_code(
            config: &BundleConfigCpp,
            index: i32,
        ) -> i32;
        fn bundle_config_extension_table_config_count(
            config: &BundleConfigCpp,
            ext_idx: i32,
        ) -> i32;
        fn bundle_config_extension_table_config_uri(
            config: &BundleConfigCpp,
            ext_idx: i32,
            cfg_idx: i32,
        ) -> String;
        fn bundle_config_extension_table_config_cross_mode(
            config: &BundleConfigCpp,
            ext_idx: i32,
            cfg_idx: i32,
        ) -> i32;

        fn bundle_config_get_proxy_entry(
            config: &BundleConfigCpp,
            index: i32,
        ) -> BmsProxyEntry;
    }
}

pub use ffi::BmsAllowListEntry;
pub use ffi::BmsProxyEntry;
pub use ffi::BmsTableConfigEntry;

/// Wrapper enabling `Send` for the opaque C++ `SharedPtr`.
///
/// Safety: `BundleConfigCpp` is a read-only data aggregate whose fields are
/// populated once at construction and never mutated.  The underlying
/// `std::shared_ptr` reference counting is atomic.  All cache access is
/// serialized by the `Mutex` that protects `BundleCache`.
struct CacheValue(cxx::SharedPtr<ffi::BundleConfigCpp>);
unsafe impl Send for CacheValue {}

struct BundleCache {
    entries: HashMap<String, CacheValue>,
}

static CACHE: Mutex<Option<BundleCache>> = Mutex::new(None);

/// Builds the cache key matching C++ `BundleMgrProxy::GetBundleInfoFromBMS`.
fn make_cache_key(bundle_name: &str, user_id: i32, app_index: i32) -> String {
    if app_index != 0 {
        format!("{}{}appIndex{}", bundle_name, user_id, app_index)
    } else {
        format!("{}{}", bundle_name, user_id)
    }
}

/// Queried bundle configuration for a DataShare provider.
pub struct BundleConfig {
    inner: cxx::SharedPtr<ffi::BundleConfigCpp>,
}

impl BundleConfig {
    /// Loads bundle configuration, returning a cached result when available.
    ///
    /// Mirrors C++ `BundleMgrProxy::GetBundleInfoFromBMS` cache semantics:
    /// cache hit returns immediately; cache miss queries BMS via FFI and
    /// stores the result on success (`result_code == 0`).
    pub fn load(bundle_name: &str, user_id: i32, app_index: i32) -> Option<Self> {
        let key = make_cache_key(bundle_name, user_id, app_index);

        {
            let guard = CACHE.lock().unwrap_or_else(|e| e.into_inner());
            if let Some(cache) = guard.as_ref() {
                if let Some(entry) = cache.entries.get(&key) {
                    return Some(Self { inner: entry.0.clone() });
                }
            }
        }

        let inner = ffi::get_bundle_info(bundle_name, user_id, app_index);
        if inner.is_null() {
            return None;
        }

        if ffi::bundle_config_result_code(&inner) == 0 {
            let mut guard = CACHE.lock().unwrap_or_else(|e| e.into_inner());
            let cache = guard.get_or_insert_with(|| BundleCache {
                entries: HashMap::new(),
            });
            cache.entries.entry(key).or_insert(CacheValue(inner.clone()));
        }

        Some(Self { inner })
    }

    /// Returns the query result code.
    pub fn result_code(&self) -> i32 {
        ffi::bundle_config_result_code(&self.inner)
    }

    /// Returns the application identifier.
    pub fn app_identifier(&self) -> String {
        ffi::bundle_config_app_identifier(&self.inner)
    }

    /// Returns the bundle name.
    pub fn name(&self) -> String {
        ffi::bundle_config_name(&self.inner)
    }

    /// Returns whether the bundle is a singleton.
    pub fn singleton(&self) -> bool {
        ffi::bundle_config_singleton(&self.inner)
    }

    /// Returns whether the bundle is a system application.
    pub fn is_system_app(&self) -> bool {
        ffi::bundle_config_is_system_app(&self.inner)
    }

    /// Returns the number of proxy data entries.
    pub fn proxy_count(&self) -> i32 {
        ffi::bundle_config_proxy_count(&self.inner)
    }

    /// Returns the URI of the proxy data entry at `index`.
    pub fn proxy_uri(&self, index: i32) -> String {
        ffi::bundle_config_proxy_uri(&self.inner, index)
    }

    /// Returns the read permission of the proxy data entry at `index`.
    pub fn proxy_read_perm(&self, index: i32) -> String {
        ffi::bundle_config_proxy_read_perm(&self.inner, index)
    }

    /// Returns the write permission of the proxy data entry at `index`.
    pub fn proxy_write_perm(&self, index: i32) -> String {
        ffi::bundle_config_proxy_write_perm(&self.inner, index)
    }

    /// Returns the parent HAP module name of the proxy data entry at `index`.
    pub fn proxy_module_name(&self, index: i32) -> String {
        ffi::bundle_config_proxy_module_name(&self.inner, index)
    }

    /// Returns the profile result code of the proxy data entry at `index`.
    pub fn proxy_profile_result_code(&self, index: i32) -> i32 {
        ffi::bundle_config_proxy_profile_result_code(&self.inner, index)
    }

    /// Returns the profile store name of the proxy data entry at `index`.
    pub fn proxy_store_name(&self, index: i32) -> String {
        ffi::bundle_config_proxy_store_name(&self.inner, index)
    }

    /// Returns the profile table name of the proxy data entry at `index`.
    pub fn proxy_table_name(&self, index: i32) -> String {
        ffi::bundle_config_proxy_table_name(&self.inner, index)
    }

    /// Returns the profile type of the proxy data entry at `index`.
    pub fn proxy_type(&self, index: i32) -> String {
        ffi::bundle_config_proxy_type(&self.inner, index)
    }

    /// Returns the profile scope of the proxy data entry at `index`.
    pub fn proxy_scope(&self, index: i32) -> String {
        ffi::bundle_config_proxy_scope(&self.inner, index)
    }

    /// Returns the number of extension entries.
    pub fn extension_count(&self) -> i32 {
        ffi::bundle_config_extension_count(&self.inner)
    }

    /// Returns the type code of the extension at `index`.
    pub fn extension_type(&self, index: i32) -> i32 {
        ffi::bundle_config_extension_type(&self.inner, index)
    }

    /// Returns the read permission of the extension at `index`.
    pub fn extension_read_perm(&self, index: i32) -> String {
        ffi::bundle_config_extension_read_perm(&self.inner, index)
    }

    /// Returns the write permission of the extension at `index`.
    pub fn extension_write_perm(&self, index: i32) -> String {
        ffi::bundle_config_extension_write_perm(&self.inner, index)
    }

    /// Returns the URI of the extension at `index`.
    pub fn extension_uri(&self, index: i32) -> String {
        ffi::bundle_config_extension_uri(&self.inner, index)
    }

    /// Returns whether silent proxy is enabled for the extension at `index`.
    pub fn extension_is_silent_enabled(&self, index: i32) -> bool {
        ffi::bundle_config_extension_is_silent_enabled(&self.inner, index)
    }

    /// Returns the backup path of the proxy data entry at `index`.
    pub fn proxy_backup(&self, index: i32) -> String {
        ffi::bundle_config_proxy_backup(&self.inner, index)
    }

    /// Returns the extension URI of the proxy data entry at `index`.
    pub fn proxy_ext_uri(&self, index: i32) -> String {
        ffi::bundle_config_proxy_ext_uri(&self.inner, index)
    }

    /// Returns whether store metadata should be resolved from URI for proxy at `index`.
    pub fn proxy_store_meta_data_from_uri(&self, index: i32) -> bool {
        ffi::bundle_config_proxy_store_meta_data_from_uri(&self.inner, index)
    }

    /// Returns the number of allow list entries for the proxy data at `proxy_idx`.
    pub fn proxy_allow_list_count(&self, proxy_idx: i32) -> i32 {
        ffi::bundle_config_proxy_allow_list_count(&self.inner, proxy_idx)
    }

    /// Returns the app identifier of allow list entry `list_idx` for proxy `proxy_idx`.
    pub fn proxy_allow_list_app_id(&self, proxy_idx: i32, list_idx: i32) -> String {
        ffi::bundle_config_proxy_allow_list_app_id(&self.inner, proxy_idx, list_idx)
    }

    /// Returns whether allow list entry `list_idx` for proxy `proxy_idx` is main-only.
    pub fn proxy_allow_list_only_main(&self, proxy_idx: i32, list_idx: i32) -> bool {
        ffi::bundle_config_proxy_allow_list_only_main(&self.inner, proxy_idx, list_idx)
    }

    /// Returns the number of table config entries for the proxy data at `proxy_idx`.
    pub fn proxy_table_config_count(&self, proxy_idx: i32) -> i32 {
        ffi::bundle_config_proxy_table_config_count(&self.inner, proxy_idx)
    }

    /// Returns the URI of table config entry `cfg_idx` for proxy `proxy_idx`.
    pub fn proxy_table_config_uri(&self, proxy_idx: i32, cfg_idx: i32) -> String {
        ffi::bundle_config_proxy_table_config_uri(&self.inner, proxy_idx, cfg_idx)
    }

    /// Returns the cross-user mode of table config entry `cfg_idx` for proxy `proxy_idx`.
    pub fn proxy_table_config_cross_mode(&self, proxy_idx: i32, cfg_idx: i32) -> i32 {
        ffi::bundle_config_proxy_table_config_cross_mode(&self.inner, proxy_idx, cfg_idx)
    }

    /// Returns the profile result code of the extension at `index`.
    pub fn extension_profile_result_code(&self, index: i32) -> i32 {
        ffi::bundle_config_extension_profile_result_code(&self.inner, index)
    }

    /// Returns the number of table config entries for the extension at `ext_idx`.
    pub fn extension_table_config_count(&self, ext_idx: i32) -> i32 {
        ffi::bundle_config_extension_table_config_count(&self.inner, ext_idx)
    }

    /// Returns the URI of table config entry `cfg_idx` for extension `ext_idx`.
    pub fn extension_table_config_uri(&self, ext_idx: i32, cfg_idx: i32) -> String {
        ffi::bundle_config_extension_table_config_uri(&self.inner, ext_idx, cfg_idx)
    }

    /// Returns the cross-user mode of table config entry `cfg_idx` for extension `ext_idx`.
    pub fn extension_table_config_cross_mode(&self, ext_idx: i32, cfg_idx: i32) -> i32 {
        ffi::bundle_config_extension_table_config_cross_mode(&self.inner, ext_idx, cfg_idx)
    }

    /// Returns all fields of proxy entry at `index` in a single FFI call.
    pub fn get_proxy_entry(&self, index: i32) -> BmsProxyEntry {
        ffi::bundle_config_get_proxy_entry(&self.inner, index)
    }

    /// Removes cached BMS results for the given bundle/user/appIndex.
    ///
    /// Mirrors C++ `BundleMgrProxy::Delete`.  C++ also clears
    /// `callerInfoCache_` and `silentAccessStores_` — those are C++-only
    /// caches already cleared by the C++ `Delete` call that always runs
    /// alongside `ds_on_bundle_changed`.
    pub fn delete(bundle_name: &str, user_id: i32, app_index: i32) {
        let key = make_cache_key(bundle_name, user_id, app_index);
        let mut guard = CACHE.lock().unwrap_or_else(|e| e.into_inner());
        if let Some(cache) = guard.as_mut() {
            cache.entries.remove(&key);
        }
    }

    /// Clears the entire BMS result cache.
    pub fn clear_cache() {
        let mut guard = CACHE.lock().unwrap_or_else(|e| e.into_inner());
        *guard = None;
    }
}

#[cfg(test)]
mod ut_wrapper {
    include!("../tests/ut/ut_wrapper.rs");
}
