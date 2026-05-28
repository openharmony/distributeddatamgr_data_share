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

use crate::error::DataShareError;
use std::collections::BTreeMap;

/// URI utility functions for DataShare
pub struct UriUtils;

impl UriUtils {
    /// Check if URI is valid DataShare URI (starts with content://)
    pub fn is_datashare_uri(uri: &str) -> bool {
        uri.starts_with("content://")
    }

    /// Extract authority from URI
    pub fn get_authority(uri: &str) -> Option<String> {
        if !uri.starts_with("content://") {
            return None;
        }
        let after_scheme = &uri["content://".len()..];
        after_scheme.split('/').next().map(|s| s.to_string())
    }

    /// Extract path from URI
    pub fn get_path(uri: &str) -> Option<String> {
        if !uri.starts_with("content://") {
            return None;
        }
        let after_scheme = &uri["content://".len()..];
        let parts: Vec<&str> = after_scheme.splitn(2, '/').collect();
        if parts.len() > 1 {
            Some(format!("/{}", parts[1]))
        } else {
            Some("/".to_string())
        }
    }

    /// Parse URI into components
    pub fn parse_uri(uri: &str) -> Result<(String, String), DataShareError> {
        if !Self::is_datashare_uri(uri) {
            return Err(DataShareError::DataShareInvalidUri);
        }

        let authority = Self::get_authority(uri).ok_or(DataShareError::DataShareInvalidUri)?;
        let path = Self::get_path(uri).ok_or(DataShareError::DataShareInvalidUri)?;

        Ok((authority, path))
    }

    /// Build URI from authority and path
    pub fn build_uri(authority: &str, path: &str) -> String {
        format!("content://{}{}", authority, path)
    }

    /// Normalize URI (ensure proper format)
    pub fn normalize_uri(uri: &str) -> String {
        if uri.starts_with("content://") {
            uri.to_string()
        } else if uri.starts_with("//") {
            format!("content:{}", uri)
        } else {
            format!("content://{}", uri)
        }
    }

    /// Check if two URIs are equal (case-insensitive scheme and authority)
    pub fn uri_equals(uri1: &str, uri2: &str) -> bool {
        uri1 == uri2 // Simple equality for now
    }

    /// Extract bundle name from URI authority
    pub fn get_bundle_name(uri: &str) -> Option<String> {
        Self::get_authority(uri).map(|auth| {
            // Typically authority is "bundleName.dataShareAbilityName"
            auth.split('.').next().unwrap_or(&auth).to_string()
        })
    }

    /// Format URI by removing query string (C++ compatible)
    pub fn format_uri(uri: &str) -> String {
        if let Some(pos) = uri.rfind('?') {
            uri[..pos].to_string()
        } else {
            uri.to_string()
        }
    }

    /// Parse string to unsigned long (C++ compatible)
    /// Returns (success, value)
    /// Matches C strtoul behavior: accepts leading whitespace and optional '+'
    pub fn strtoul(s: &str) -> (bool, u32) {
        if s.is_empty() {
            return (false, 0);
        }
        // Match C strtoul: skip leading whitespace and optional '+'
        let trimmed = s.trim_start();
        let stripped = trimmed.strip_prefix('+').unwrap_or(trimmed);
        if stripped.is_empty() {
            return (false, 0);
        }
        match stripped.parse::<u32>() {
            Ok(v) => (true, v),
            Err(_) => (false, 0),
        }
    }

    /// Parse query parameters from URI (C++ compatible)
    pub fn get_query_params(uri: &str) -> BTreeMap<String, String> {
        let query_start = match uri.find('?') {
            Some(pos) => pos,
            None => return BTreeMap::new(),
        };

        let query = &uri[query_start + 1..];
        let mut params = BTreeMap::new();
        let mut start = 0;

        while start < query.len() {
            let delimiter = query[start..].find('&').map_or(query.len(), |p| start + p);
            let equal_pos = query[start..].find('=').map(|p| start + p);

            if let Some(eq) = equal_pos {
                if eq < delimiter {
                    let key = &query[start..eq];
                    let value = &query[eq + 1..delimiter];
                    params.insert(key.to_string(), value.to_string());
                }
            }
            start = delimiter + 1;
        }
        params
    }

    /// Get user from URI query parameters (C++ compatible)
    /// Returns (success, user_id). user_id is -1 if not present.
    pub fn get_user_from_uri(uri: &str) -> (bool, i32) {
        let params = Self::get_query_params(uri);
        match params.get("user") {
            None => {
                // -1 is placeholder for visit provider's user
                (true, -1)
            }
            Some(user_str) if user_str.is_empty() => (true, -1),
            Some(user_str) => match Self::strtoul(user_str) {
                (true, data) => {
                    if data > i32::MAX as u32 {
                        (false, -1)
                    } else {
                        (true, data as i32)
                    }
                }
                (false, _) => (false, -1),
            },
        }
    }

    /// Extract first path segment from URI.
    ///
    /// Supports:
    /// - `xxx://authority/path` → extracts `authority`
    /// - `xxx:///path1/path2` → extracts `path1`
    ///
    /// Returns empty string if no segment found.
    /// Equivalent to C++ `DataShareURIUtils::ExtractFirstPathSegment`.
    pub fn extract_first_path_segment(uri: &str) -> String {
        let colon_pos = match uri.find("://") {
            Some(pos) => pos,
            None => return String::new(),
        };

        let after_scheme = &uri[colon_pos + 3..];
        let trimmed = after_scheme.trim_start_matches('/');
        if trimmed.is_empty() {
            return String::new();
        }

        match trimmed.find('/') {
            Some(pos) => trimmed[..pos].to_string(),
            None => trimmed.to_string(),
        }
    }

    /// Parse system ability ID from a non-silent datashare URI.
    ///
    /// The URI must start with `datashare://` and contain `/SAID=<number>`.
    /// Returns `(true, said)` on success, `(false, -1)` on failure.
    /// Equivalent to C++ `DataShareURIUtils::GetSystemAbilityId`.
    pub fn get_system_ability_id(uri: &str) -> (bool, i32) {
        const SA_SCHEMA: &str = "datashare://";
        const SA_ID_TAG: &str = "/SAID=";
        const LAST_SYS_ABILITY_ID: u32 = 0x00FF_FFFF;

        if !uri.starts_with(SA_SCHEMA) {
            return (false, -1);
        }

        let sa_id_pos = match uri.find(SA_ID_TAG) {
            Some(pos) if pos > SA_SCHEMA.len() => pos,
            _ => return (false, -1),
        };

        let value_start = sa_id_pos + SA_ID_TAG.len();
        let value_end = uri[value_start..]
            .find('/')
            .map_or(uri.len(), |p| value_start + p);
        let sa_id_str = &uri[value_start..value_end];

        if sa_id_str.is_empty() {
            return (false, -1);
        }

        let (ok, sa_id) = Self::strtoul(sa_id_str);
        if !ok || sa_id > LAST_SYS_ABILITY_ID {
            return (false, -1);
        }
        (true, sa_id as i32)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_is_datashare_uri() {
        assert!(UriUtils::is_datashare_uri("content://com.example/table"));
        assert!(!UriUtils::is_datashare_uri("http://example.com"));
    }

    #[test]
    fn test_get_authority() {
        let uri = "content://com.example.provider/table/1";
        assert_eq!(
            UriUtils::get_authority(uri),
            Some("com.example.provider".to_string())
        );
    }

    #[test]
    fn test_get_path() {
        let uri = "content://com.example.provider/table/1";
        assert_eq!(UriUtils::get_path(uri), Some("/table/1".to_string()));
    }

    #[test]
    fn test_parse_uri() {
        let uri = "content://com.example.provider/contacts";
        let result = UriUtils::parse_uri(uri);
        assert!(result.is_ok());
        let (auth, path) = result.unwrap();
        assert_eq!(auth, "com.example.provider");
        assert_eq!(path, "/contacts");
    }

    #[test]
    fn test_build_uri() {
        let uri = UriUtils::build_uri("com.example", "/table");
        assert_eq!(uri, "content://com.example/table");
    }

    #[test]
    fn test_normalize_uri() {
        assert_eq!(
            UriUtils::normalize_uri("content://auth/path"),
            "content://auth/path"
        );
        assert_eq!(
            UriUtils::normalize_uri("//auth/path"),
            "content://auth/path"
        );
    }

    #[test]
    fn test_extract_first_path_segment() {
        assert_eq!(
            UriUtils::extract_first_path_segment("content://authority/path"),
            "authority"
        );
        assert_eq!(
            UriUtils::extract_first_path_segment("datashare:///path1/path2"),
            "path1"
        );
        assert_eq!(UriUtils::extract_first_path_segment("xxx://host"), "host");
        assert_eq!(UriUtils::extract_first_path_segment("xxx:///"), "");
        assert_eq!(UriUtils::extract_first_path_segment("invalid"), "");
    }

    #[test]
    fn test_get_system_ability_id() {
        let (ok, id) = UriUtils::get_system_ability_id("datashare://authority/SAID=1234/path");
        assert!(ok);
        assert_eq!(id, 1234);

        let (ok, _) = UriUtils::get_system_ability_id("content://authority/SAID=1234");
        assert!(!ok); // wrong scheme

        let (ok, _) = UriUtils::get_system_ability_id("datashare:///SAID=1234");
        assert!(!ok); // SAID at schema boundary

        let (ok, _) = UriUtils::get_system_ability_id("datashare://auth/SAID=");
        assert!(!ok); // empty SAID value

        let (ok, id) = UriUtils::get_system_ability_id("datashare://auth/SAID=16777215");
        assert!(ok);
        assert_eq!(id, 0x00FF_FFFF); // max valid

        let (ok, _) = UriUtils::get_system_ability_id("datashare://auth/SAID=16777216");
        assert!(!ok); // over max
    }
}
