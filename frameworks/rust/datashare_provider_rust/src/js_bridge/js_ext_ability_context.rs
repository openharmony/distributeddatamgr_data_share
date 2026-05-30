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

//! JsDataShareExtAbilityContext — JS context wrapper for DataShare extensions.
//!
//! Corresponds to C++ `JsDataShareExtAbilityContext` in
//! `frameworks/native/provider/src/js_datashare_ext_ability_context.cpp` (66 lines).
//!
//! Provides a NAPI-wrapped JS context object that holds a weak reference
//! to the DataShareExtAbilityContext.

use std::sync::Weak;

use crate::ability::ext_ability_context::DataShareExtAbilityContext;

/// JsDataShareExtAbilityContext — JS context wrapper.
///
/// Corresponds to C++ `JsDataShareExtAbilityContext`.
///
/// Holds a weak reference to the native DataShareExtAbilityContext
/// and exposes it as a NAPI JS object.
pub struct JsDataShareExtAbilityContext {
    /// Weak reference to the native context.
    _context: Option<Weak<DataShareExtAbilityContext>>,
}

impl JsDataShareExtAbilityContext {
    /// Create a new JsDataShareExtAbilityContext.
    pub fn new() -> Self {
        Self { _context: None }
    }

    /// Create a new JsDataShareExtAbilityContext with a context reference.
    pub fn with_context(context: Weak<DataShareExtAbilityContext>) -> Self {
        Self {
            _context: Some(context),
        }
    }
}

impl Default for JsDataShareExtAbilityContext {
    fn default() -> Self {
        Self::new()
    }
}

/// Create a JS DataShareExtAbilityContext object.
///
/// Corresponds to C++ `CreateJsDataShareExtAbilityContext()`.
///
/// TODO: Implement with NAPI
/// - Creates JS extension context object
/// - Wraps JsDataShareExtAbilityContext via napi_wrap
/// - Sets up finalizer for cleanup
pub fn create_js_datashare_ext_ability_context(
    _context: Weak<DataShareExtAbilityContext>,
) -> Option<JsDataShareExtAbilityContext> {
    // TODO: Call CreateJsExtensionContext(env, context) for base object
    // TODO: napi_wrap(env, objValue, jsContext, Finalizer, ...)
    Some(JsDataShareExtAbilityContext::new())
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::sync::Arc;

    #[test]
    fn test_js_context_creation() {
        let _ctx = JsDataShareExtAbilityContext::new();
    }

    #[test]
    fn test_js_context_default() {
        let _ctx = JsDataShareExtAbilityContext::default();
    }

    #[test]
    fn test_js_context_with_context() {
        let context = Arc::new(DataShareExtAbilityContext::new());
        let weak = Arc::downgrade(&context);
        let _js_ctx = JsDataShareExtAbilityContext::with_context(weak);
    }

    #[test]
    fn test_create_js_context() {
        let context = Arc::new(DataShareExtAbilityContext::new());
        let weak = Arc::downgrade(&context);
        let result = create_js_datashare_ext_ability_context(weak);
        assert!(result.is_some());
    }

    #[test]
    fn test_create_js_context_with_dropped_ref() {
        let weak = {
            let context = Arc::new(DataShareExtAbilityContext::new());
            Arc::downgrade(&context)
        };
        // Context is dropped, but creation should still work (placeholder)
        let result = create_js_datashare_ext_ability_context(weak);
        assert!(result.is_some());
    }
}
