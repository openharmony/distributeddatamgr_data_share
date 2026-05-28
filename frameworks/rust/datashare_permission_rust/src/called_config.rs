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

//! DataShareCalledConfig — Called config with URI parsing and BMS queries.
//!
//! Corresponds to C++ `DataShareCalledConfig` in
//! `frameworks/native/permission/src/data_share_called_config.cpp` (181 lines).
//!
//! Parses DataShare URIs to extract provider bundle information, resolves
//! provider permission configuration from BMS (Bundle Manager Service), and
//! handles cross-user access.
//!
//! URI format:
//! - Non-proxy: `datashare:///bundleName/path`
//! - Proxy: `datashareproxy://authority/path`

use crate::permission::SCHEMA_DATASHARE_PROXY;

/// Default user ID when none is resolved
const DEFAULT_USER_ID: i32 = -1;

/// Provider information resolved from BMS
///
/// Corresponds to C++ `DataShareCalledConfig::ProviderInfo`.
#[derive(Debug, Clone)]
pub struct ProviderInfo {
    /// Full URI of the provider
    pub uri: String,
    /// URI schema (e.g., "datashare", "datashareproxy")
    pub schema: String,
    /// Bundle name of the provider
    pub bundle_name: String,
    /// Read permission required by the provider
    pub read_permission: String,
    /// Write permission required by the provider
    pub write_permission: String,
    /// Module name containing the provider
    pub module_name: String,
    /// Current user ID for cross-user access
    pub current_user_id: i32,
}

impl Default for ProviderInfo {
    fn default() -> Self {
        Self {
            uri: String::new(),
            schema: String::new(),
            bundle_name: String::new(),
            read_permission: String::new(),
            write_permission: String::new(),
            module_name: String::new(),
            current_user_id: DEFAULT_USER_ID,
        }
    }
}

/// Bundle information from BMS
///
/// Placeholder for BMS BundleInfo
#[derive(Debug, Clone)]
pub struct BundleInfo {
    /// Bundle name
    pub name: String,
    /// Is system app
    pub is_system_app: bool,
    /// App identifier
    pub app_identifier: String,
}

impl Default for BundleInfo {
    fn default() -> Self {
        Self {
            name: String::new(),
            is_system_app: false,
            app_identifier: String::new(),
        }
    }
}

/// Extension ability information from BMS
///
/// Placeholder for BMS ExtensionAbilityInfo
#[derive(Debug, Clone)]
pub struct ExtensionAbilityInfo {
    /// Bundle name
    pub bundle_name: String,
    /// Module name
    pub module_name: String,
    /// Read permission
    pub read_permission: String,
    /// Write permission
    pub write_permission: String,
    /// URI
    pub uri: String,
}

impl Default for ExtensionAbilityInfo {
    fn default() -> Self {
        Self {
            bundle_name: String::new(),
            module_name: String::new(),
            read_permission: String::new(),
            write_permission: String::new(),
            uri: String::new(),
        }
    }
}

/// DataShareCalledConfig — Called config with URI parsing and BMS queries
///
/// Corresponds to C++ `DataShareCalledConfig`.
///
/// Parses DataShare URIs to extract provider information, resolves
/// permissions from BMS, and handles user context.
pub struct DataShareCalledConfig {
    /// Original URI
    uri: String,
    /// Parsed bundle name from URI
    bundle_name: String,
    /// Whether this is a proxy URI
    is_proxy: bool,
    /// Calling token ID
    token_id: u32,
}

impl DataShareCalledConfig {
    /// Create a new DataShareCalledConfig by parsing the URI
    ///
    /// Corresponds to C++ `DataShareCalledConfig::DataShareCalledConfig(uri, tokenId)`.
    ///
    /// URI parsing logic:
    /// - Proxy URIs (`datashareproxy://authority/path`): authority = bundle name
    /// - Non-proxy URIs (`datashare:///bundleName/path`): third path segment = bundle name
    pub fn new(uri: &str, token_id: u32) -> Self {
        let is_proxy = uri.starts_with(SCHEMA_DATASHARE_PROXY);
        let bundle_name = Self::extract_bundle_name(uri, is_proxy);

        Self {
            uri: uri.to_string(),
            bundle_name,
            is_proxy,
            token_id,
        }
    }

    /// Extract bundle name from URI
    fn extract_bundle_name(uri: &str, is_proxy: bool) -> String {
        if is_proxy {
            // Proxy URI: datashareproxy://authority/path
            // Extract authority part
            if let Some(rest) = uri.strip_prefix(SCHEMA_DATASHARE_PROXY) {
                if let Some(slash_pos) = rest.find('/') {
                    return rest[..slash_pos].to_string();
                }
                return rest.to_string();
            }
        } else {
            // Non-proxy URI: datashare:///bundleName/path
            // Extract bundle name from path
            let parts: Vec<&str> = uri.split('/').collect();
            // datashare:///bundleName → split by '/' gives ["datashare:", "", "", "bundleName", ...]
            if parts.len() >= 4 {
                return parts[3].to_string();
            }
        }
        String::new()
    }

    /// Get the parsed URI
    pub fn uri(&self) -> &str {
        &self.uri
    }

    /// Get the parsed bundle name
    pub fn bundle_name(&self) -> &str {
        &self.bundle_name
    }

    /// Whether this is a proxy URI
    pub fn is_proxy(&self) -> bool {
        self.is_proxy
    }

    /// Get user ID by calling token
    ///
    /// Corresponds to C++ `DataShareCalledConfig::GetUserByToken(tokenId)`.
    ///
    /// TODO: Use AccessTokenKit to resolve user ID from token
    pub fn get_user_by_token(token_id: u32) -> i32 {
        // TODO: Call AccessTokenKit::GetTokenInfo(token_id)
        // TODO: Extract user_id from token info
        let _ = token_id;
        DEFAULT_USER_ID
    }

    /// Resolve provider info from BMS proxy data
    ///
    /// Corresponds to C++ `DataShareCalledConfig::GetFromProxyData()`.
    ///
    /// TODO: Query BMS for proxy data configuration
    pub fn get_from_proxy_data(&self) -> Option<ProviderInfo> {
        // TODO: Query BMS proxy data using self.bundle_name
        // TODO: Build ProviderInfo from proxy data
        None
    }

    /// Get provider information for a specific user
    ///
    /// Corresponds to C++ `DataShareCalledConfig::GetProviderInfo(user)`.
    ///
    /// Main entry point for resolving provider configuration.
    /// Returns (error_code, ProviderInfo) tuple.
    pub fn get_provider_info(&self, user_id: i32) -> (i32, ProviderInfo) {
        // TODO: If proxy, call get_from_proxy_data()
        // TODO: Otherwise, query BMS using get_extension_info_from_bms()
        // TODO: Build ProviderInfo from BMS result
        let info = ProviderInfo {
            uri: self.uri.clone(),
            schema: if self.is_proxy {
                "datashareproxy".to_string()
            } else {
                "datashare".to_string()
            },
            bundle_name: self.bundle_name.clone(),
            current_user_id: user_id,
            ..Default::default()
        };
        (0, info) // 0 = E_OK
    }

    /// Get provider information using token to resolve user
    pub fn get_provider_info_by_token(&self) -> (i32, ProviderInfo) {
        let user_id = Self::get_user_by_token(self.token_id);
        self.get_provider_info(user_id)
    }

    /// Get bundle info from BMS (Bundle Manager Service)
    ///
    /// Corresponds to C++ `DataShareCalledConfig::GetBundleInfoFromBMS(bundleName, user)`.
    ///
    /// TODO: Query BMS with identity reset for cross-process calls
    pub fn get_bundle_info_from_bms(bundle_name: &str, _user_id: i32) -> Option<BundleInfo> {
        // TODO: Call IPCSkeleton::ResetCallingIdentity()
        // TODO: Call BundleMgrHelper::GetBundleInfoForSelf(bundle_name, flags, bundle_info, user_id)
        // TODO: Call IPCSkeleton::SetCallingIdentity(identity)
        Some(BundleInfo {
            name: bundle_name.to_string(),
            ..Default::default()
        })
    }

    /// Get extension ability info from BMS by URI
    ///
    /// Corresponds to C++ `DataShareCalledConfig::GetExtensionInfoFromBMS(uri, user)`.
    ///
    /// TODO: Query BMS for extension ability info matching the URI
    pub fn get_extension_info_from_bms(uri: &str, _user_id: i32) -> Option<ExtensionAbilityInfo> {
        // TODO: Call BundleMgrHelper::QueryExtensionAbilityInfos(want, flags, extension_infos, user_id)
        // TODO: Filter by URI match
        Some(ExtensionAbilityInfo {
            uri: uri.to_string(),
            ..Default::default()
        })
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_provider_info_default() {
        let info = ProviderInfo::default();
        assert!(info.uri.is_empty());
        assert!(info.bundle_name.is_empty());
        assert_eq!(info.current_user_id, DEFAULT_USER_ID);
    }

    #[test]
    fn test_parse_non_proxy_uri() {
        let config = DataShareCalledConfig::new("datashare:///com.example.app/table1", 1);
        assert_eq!(config.bundle_name(), "com.example.app");
        assert!(!config.is_proxy());
    }

    #[test]
    fn test_parse_proxy_uri() {
        let config = DataShareCalledConfig::new("datashareproxy://com.example.app/table1", 1);
        assert_eq!(config.bundle_name(), "com.example.app");
        assert!(config.is_proxy());
    }

    #[test]
    fn test_parse_proxy_uri_no_path() {
        let config = DataShareCalledConfig::new("datashareproxy://com.example.app", 1);
        assert_eq!(config.bundle_name(), "com.example.app");
        assert!(config.is_proxy());
    }

    #[test]
    fn test_parse_invalid_uri() {
        let config = DataShareCalledConfig::new("invalid://", 1);
        assert!(config.bundle_name().is_empty());
        assert!(!config.is_proxy());
    }

    #[test]
    fn test_get_user_by_token() {
        assert_eq!(DataShareCalledConfig::get_user_by_token(1), DEFAULT_USER_ID);
    }

    #[test]
    fn test_get_provider_info() {
        let config = DataShareCalledConfig::new("datashare:///com.example.app/table1", 1);
        let (err, info) = config.get_provider_info(100);
        assert_eq!(err, 0);
        assert_eq!(info.bundle_name, "com.example.app");
        assert_eq!(info.current_user_id, 100);
        assert_eq!(info.schema, "datashare");
    }

    #[test]
    fn test_get_provider_info_proxy() {
        let config = DataShareCalledConfig::new("datashareproxy://com.example.app/table1", 1);
        let (err, info) = config.get_provider_info(100);
        assert_eq!(err, 0);
        assert_eq!(info.schema, "datashareproxy");
    }

    #[test]
    fn test_get_provider_info_by_token() {
        let config = DataShareCalledConfig::new("datashare:///com.example.app/table1", 1);
        let (err, info) = config.get_provider_info_by_token();
        assert_eq!(err, 0);
        assert_eq!(info.bundle_name, "com.example.app");
    }

    #[test]
    fn test_get_bundle_info_from_bms() {
        let info = DataShareCalledConfig::get_bundle_info_from_bms("com.example.app", 100);
        assert!(info.is_some());
        assert_eq!(info.unwrap().name, "com.example.app");
    }

    #[test]
    fn test_get_extension_info_from_bms() {
        let info = DataShareCalledConfig::get_extension_info_from_bms("datashare:///test", 100);
        assert!(info.is_some());
        assert_eq!(info.unwrap().uri, "datashare:///test");
    }

    #[test]
    fn test_bundle_info_default() {
        let info = BundleInfo::default();
        assert!(info.name.is_empty());
        assert!(!info.is_system_app);
    }

    #[test]
    fn test_extension_ability_info_default() {
        let info = ExtensionAbilityInfo::default();
        assert!(info.bundle_name.is_empty());
        assert!(info.uri.is_empty());
    }
}
