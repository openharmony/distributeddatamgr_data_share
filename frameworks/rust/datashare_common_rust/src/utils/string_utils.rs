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

/// String utility functions
pub struct StringUtils;

impl StringUtils {
    /// Check if a string is null or empty
    pub fn is_empty_or_null(s: Option<&str>) -> bool {
        match s {
            None => true,
            Some(s) => s.is_empty(),
        }
    }

    /// Trim whitespace from a string
    pub fn trim(s: &str) -> &str {
        s.trim()
    }

    /// Convert string to uppercase
    pub fn to_upper(s: &str) -> String {
        s.to_uppercase()
    }

    /// Convert string to lowercase
    pub fn to_lower(s: &str) -> String {
        s.to_lowercase()
    }

    /// Check if string starts with prefix
    pub fn starts_with(s: &str, prefix: &str) -> bool {
        s.starts_with(prefix)
    }

    /// Check if string ends with suffix
    pub fn ends_with(s: &str, suffix: &str) -> bool {
        s.ends_with(suffix)
    }

    /// Check if string contains substring
    pub fn contains(s: &str, substr: &str) -> bool {
        s.contains(substr)
    }

    /// Split string by delimiter
    pub fn split(s: &str, delimiter: &str) -> Vec<String> {
        s.split(delimiter).map(|p| p.to_string()).collect()
    }

    /// Replace all occurrences of pattern with replacement
    pub fn replace_all(s: &str, pattern: &str, replacement: &str) -> String {
        s.replace(pattern, replacement)
    }

    /// Get substring
    pub fn substring(s: &str, start: usize, end: usize) -> &str {
        if start >= s.len() || end > s.len() || start >= end {
            ""
        } else {
            &s[start..end]
        }
    }

    /// Anonymize a string (C++ compatible)
    /// Returns "******" + last 10 chars if len > 10, else "******"
    pub fn anonymous(name: &str) -> String {
        const END_SIZE: usize = 10;
        const DEFAULT_ANONYMOUS: &str = "******";

        if name.len() <= END_SIZE {
            DEFAULT_ANONYMOUS.to_string()
        } else {
            format!("{}{}", DEFAULT_ANONYMOUS, &name[name.len() - END_SIZE..])
        }
    }

    /// Remove query string from URI (C++ compatible)
    /// Removes everything after last '?' (matches C++ find_last_of)
    pub fn remove_from_query(uri: &str) -> String {
        if let Some(pos) = uri.rfind('?') {
            uri[..pos].to_string()
        } else {
            uri.to_string()
        }
    }

    /// Change/anonymize a string (C++ compatible)
    /// Returns name as-is if len <= 12, else "******" + substring from pos 12
    pub fn change(name: &str) -> String {
        const ANONYMOUS_SIZE: usize = 12;
        const DEFAULT_ANONYMOUS: &str = "******";

        if name.len() <= ANONYMOUS_SIZE {
            name.to_string()
        } else {
            format!("{}{}", DEFAULT_ANONYMOUS, &name[ANONYMOUS_SIZE..])
        }
    }

    /// Get random number between min and max inclusive (C++ compatible)
    /// Uses getrandom syscall for quality matching C++ mt19937 + random_device
    pub fn get_random_number(min: i32, max: i32) -> i32 {
        if min >= max {
            return min;
        }

        let range = (max as i64 - min as i64 + 1) as u64;
        let mut buf = [0u8; 8];
        // Use libc getrandom syscall for hardware-quality entropy
        #[cfg(target_os = "linux")]
        {
            extern "C" {
                fn getrandom(buf: *mut u8, buflen: usize, flags: u32) -> isize;
            }
            let ret = unsafe { getrandom(buf.as_mut_ptr(), 8, 0) };
            if ret < 0 {
                // Fallback: return min on failure
                return min;
            }
        }
        #[cfg(not(target_os = "linux"))]
        {
            use std::time::{SystemTime, UNIX_EPOCH};
            let seed = SystemTime::now()
                .duration_since(UNIX_EPOCH)
                .unwrap_or_default()
                .as_nanos() as u64;
            buf = seed.to_ne_bytes();
        }
        let random_val = u64::from_ne_bytes(buf);
        min + (random_val % range) as i32
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_is_empty_or_null() {
        assert!(StringUtils::is_empty_or_null(None));
        assert!(StringUtils::is_empty_or_null(Some("")));
        assert!(!StringUtils::is_empty_or_null(Some("text")));
    }

    #[test]
    fn test_trim() {
        assert_eq!(StringUtils::trim("  hello  "), "hello");
    }

    #[test]
    fn test_split() {
        let parts = StringUtils::split("a,b,c", ",");
        assert_eq!(parts.len(), 3);
        assert_eq!(parts[0], "a");
    }
}
