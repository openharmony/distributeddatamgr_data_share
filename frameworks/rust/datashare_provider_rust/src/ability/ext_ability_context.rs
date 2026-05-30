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

//! DataShareExtAbilityContext — context wrapper for DataShare extensions.
//!
//! Corresponds to C++ `DataShareExtAbilityContext` in
//! `frameworks/native/provider/src/datashare_ext_ability_context.cpp` (25 lines)
//! and header `frameworks/native/provider/include/datashare_ext_ability_context.h`.
//!
//! Lightweight context wrapper that extends ExtensionContext
//! with a type ID for runtime type checking.

use std::collections::hash_map::DefaultHasher;
use std::hash::{Hash, Hasher};

/// Compute a hash-based type ID from a string, matching C++
/// `std::hash<const char*>{}("DataShareExtAbilityContext")`.
fn compute_context_type_id() -> u64 {
    let mut hasher = DefaultHasher::new();
    "DataShareExtAbilityContext".hash(&mut hasher);
    hasher.finish()
}

/// DataShareExtAbilityContext — context for DataShare extension abilities.
///
/// Corresponds to C++ `DataShareExtAbilityContext`.
///
/// Extends `ExtensionContext` (placeholder) with a type ID for
/// runtime type checking via `IsContext()`.
pub struct DataShareExtAbilityContext {
    /// Context type ID for runtime type checking.
    context_type_id: u64,
}

impl DataShareExtAbilityContext {
    /// Context type ID (static constant).
    pub fn type_id() -> u64 {
        compute_context_type_id()
    }

    /// Create a new DataShareExtAbilityContext.
    pub fn new() -> Self {
        Self {
            context_type_id: compute_context_type_id(),
        }
    }

    /// Check if this context matches the given type ID.
    ///
    /// Corresponds to C++ `IsContext(size_t contextTypeId)`.
    pub fn is_context(&self, context_type_id: u64) -> bool {
        context_type_id == self.context_type_id
    }
}

impl Default for DataShareExtAbilityContext {
    fn default() -> Self {
        Self::new()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_context_creation() {
        let context = DataShareExtAbilityContext::new();
        assert!(context.is_context(DataShareExtAbilityContext::type_id()));
    }

    #[test]
    fn test_context_type_id_consistency() {
        let id1 = DataShareExtAbilityContext::type_id();
        let id2 = DataShareExtAbilityContext::type_id();
        assert_eq!(id1, id2);
    }

    #[test]
    fn test_context_type_id_nonzero() {
        let id = DataShareExtAbilityContext::type_id();
        assert_ne!(id, 0);
    }

    #[test]
    fn test_is_context_with_matching_id() {
        let context = DataShareExtAbilityContext::new();
        assert!(context.is_context(DataShareExtAbilityContext::type_id()));
    }

    #[test]
    fn test_is_context_with_wrong_id() {
        let context = DataShareExtAbilityContext::new();
        assert!(!context.is_context(999));
    }

    #[test]
    fn test_default_context() {
        let context = DataShareExtAbilityContext::default();
        assert!(context.is_context(DataShareExtAbilityContext::type_id()));
    }
}
