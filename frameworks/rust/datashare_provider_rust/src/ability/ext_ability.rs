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

//! DataShareExtAbility — ExtensionAbility trait with CRUD virtual methods.
//!
//! Corresponds to C++ `DataShareExtAbility` in
//! `frameworks/native/provider/src/datashare_ext_ability.cpp` (172 lines).
//!
//! Provides a trait-based interface for DataShare operations.
//! Default implementations return error values or empty results.
//! Subclasses (e.g., JsDataShareExtAbility) override these methods
//! to provide actual business logic.

/// DataShareExtAbility — trait for DataShare extension ability.
///
/// Corresponds to C++ `DataShareExtAbility`.
///
/// Defines virtual methods for CRUD operations and lifecycle hooks.
/// Default implementations return error values or empty results.
pub trait DataShareExtAbility: Send + Sync {
    /// Get file types for a URI.
    ///
    /// Corresponds to C++ `GetFileTypes()`.
    fn get_file_types(&self, _uri: &str, _mime_type_filter: &str) -> Vec<String> {
        Vec::new()
    }

    /// Open a file for the given URI.
    ///
    /// Corresponds to C++ `OpenFile()`.
    fn open_file(&self, _uri: &str, _mode: &str) -> i32 {
        -1
    }

    /// Open a raw file for the given URI.
    ///
    /// Corresponds to C++ `OpenRawFile()`.
    fn open_raw_file(&self, _uri: &str, _mode: &str) -> i32 {
        -1
    }

    /// Insert a record.
    ///
    /// Corresponds to C++ `Insert()`.
    fn insert(&self, _uri: &str, _value: &str) -> i32 {
        -1
    }

    /// Insert with extended result.
    ///
    /// Corresponds to C++ `InsertExt()`.
    fn insert_ext(&self, _uri: &str, _value: &str) -> (i32, String) {
        (-1, String::new())
    }

    /// Update records.
    ///
    /// Corresponds to C++ `Update()`.
    fn update(&self, _uri: &str, _predicates: &str, _value: &str) -> i32 {
        -1
    }

    /// Batch update records.
    ///
    /// Corresponds to C++ `BatchUpdate()`.
    fn batch_update(&self, _operations: &[String]) -> i32 {
        0
    }

    /// Delete records.
    ///
    /// Corresponds to C++ `Delete()`.
    fn delete(&self, _uri: &str, _predicates: &str) -> i32 {
        -1
    }

    /// Query records.
    ///
    /// Corresponds to C++ `Query()`.
    fn query(&self, _uri: &str, _columns: &[String], _predicates: &str) -> Option<String> {
        None
    }

    /// Get MIME type for a URI.
    ///
    /// Corresponds to C++ `GetType()`.
    fn get_type(&self, _uri: &str) -> String {
        String::new()
    }

    /// Batch insert records.
    ///
    /// Corresponds to C++ `BatchInsert()`.
    fn batch_insert(&self, _uri: &str, _values: &[String]) -> i32 {
        -1
    }

    /// Execute batch operations.
    ///
    /// Corresponds to C++ `ExecuteBatch()`.
    fn execute_batch(&self, _statements: &[String]) -> i32 {
        -1
    }

    /// Register an observer for URI changes.
    ///
    /// Corresponds to C++ `RegisterObserver()`.
    fn register_observer(&self, _uri: &str, _observer_id: u64) -> bool {
        true
    }

    /// Unregister an observer.
    ///
    /// Corresponds to C++ `UnregisterObserver()`.
    fn unregister_observer(&self, _uri: &str, _observer_id: u64) -> bool {
        true
    }

    /// Notify observers of changes.
    ///
    /// Corresponds to C++ `NotifyChange()`.
    fn notify_change(&self, _uri: &str) -> bool {
        true
    }

    /// Register an observer for extended provider.
    ///
    /// Corresponds to C++ `RegisterObserverExtProvider()`.
    fn register_observer_ext_provider(
        &self,
        _uri: &str,
        _observer_id: u64,
        _is_descendants: bool,
    ) -> bool {
        true
    }

    /// Unregister an observer for extended provider.
    ///
    /// Corresponds to C++ `UnregisterObserverExtProvider()`.
    fn unregister_observer_ext_provider(&self, _uri: &str, _observer_id: u64) -> bool {
        true
    }

    /// Notify observers of changes for extended provider.
    ///
    /// Corresponds to C++ `NotifyChangeExtProvider()`.
    fn notify_change_ext_provider(&self, _change_info: &str) -> bool {
        true
    }

    /// Normalize a URI.
    ///
    /// Corresponds to C++ `NormalizeUri()`.
    fn normalize_uri(&self, uri: &str) -> String {
        uri.to_string()
    }

    /// Denormalize a URI.
    ///
    /// Corresponds to C++ `DenormalizeUri()`.
    fn denormalize_uri(&self, uri: &str) -> String {
        uri.to_string()
    }

    /// Insert with extended result (Ex variant).
    ///
    /// Corresponds to C++ `InsertEx()`.
    fn insert_ex(&self, _uri: &str, _value: &str) -> (i32, i32) {
        (0, 0)
    }

    /// Update with extended result (Ex variant).
    ///
    /// Corresponds to C++ `UpdateEx()`.
    fn update_ex(&self, _uri: &str, _predicates: &str, _value: &str) -> (i32, i32) {
        (0, 0)
    }

    /// Delete with extended result (Ex variant).
    ///
    /// Corresponds to C++ `DeleteEx()`.
    fn delete_ex(&self, _uri: &str, _predicates: &str) -> (i32, i32) {
        (0, 0)
    }

    /// Lifecycle hook: called when ability is created.
    ///
    /// TODO: Corresponds to C++ `OnCreate()`.
    fn on_create(&mut self) {
        // Placeholder
    }

    /// Lifecycle hook: called when ability is destroyed.
    ///
    /// TODO: Corresponds to C++ `OnDestroy()`.
    fn on_destroy(&mut self) {
        // Placeholder
    }
}

/// Default implementation of DataShareExtAbility.
pub struct DefaultDataShareExtAbility;

impl DataShareExtAbility for DefaultDataShareExtAbility {}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_default_get_file_types() {
        let ability = DefaultDataShareExtAbility;
        let types = ability.get_file_types("datashare:///test", "text/*");
        assert!(types.is_empty());
    }

    #[test]
    fn test_default_open_file() {
        let ability = DefaultDataShareExtAbility;
        assert_eq!(ability.open_file("datashare:///test", "r"), -1);
    }

    #[test]
    fn test_default_insert() {
        let ability = DefaultDataShareExtAbility;
        assert_eq!(ability.insert("datashare:///test", "{}"), -1);
    }

    #[test]
    fn test_default_update() {
        let ability = DefaultDataShareExtAbility;
        assert_eq!(ability.update("datashare:///test", "id=1", "{}"), -1);
    }

    #[test]
    fn test_default_delete() {
        let ability = DefaultDataShareExtAbility;
        assert_eq!(ability.delete("datashare:///test", "id=1"), -1);
    }

    #[test]
    fn test_default_query() {
        let ability = DefaultDataShareExtAbility;
        let result = ability.query("datashare:///test", &[], "");
        assert!(result.is_none());
    }

    #[test]
    fn test_default_get_type() {
        let ability = DefaultDataShareExtAbility;
        let mime_type = ability.get_type("datashare:///test");
        assert!(mime_type.is_empty());
    }

    #[test]
    fn test_default_batch_insert() {
        let ability = DefaultDataShareExtAbility;
        assert_eq!(ability.batch_insert("datashare:///test", &[]), -1);
    }

    #[test]
    fn test_default_register_observer() {
        let ability = DefaultDataShareExtAbility;
        assert!(ability.register_observer("datashare:///test", 1));
    }

    #[test]
    fn test_default_unregister_observer() {
        let ability = DefaultDataShareExtAbility;
        assert!(ability.unregister_observer("datashare:///test", 1));
    }

    #[test]
    fn test_default_notify_change() {
        let ability = DefaultDataShareExtAbility;
        assert!(ability.notify_change("datashare:///test"));
    }

    #[test]
    fn test_default_normalize_uri() {
        let ability = DefaultDataShareExtAbility;
        let uri = "datashare:///test";
        assert_eq!(ability.normalize_uri(uri), uri);
    }

    #[test]
    fn test_default_denormalize_uri() {
        let ability = DefaultDataShareExtAbility;
        let uri = "datashare:///test";
        assert_eq!(ability.denormalize_uri(uri), uri);
    }

    #[test]
    fn test_default_insert_ext() {
        let ability = DefaultDataShareExtAbility;
        let (code, result) = ability.insert_ext("datashare:///test", "{}");
        assert_eq!(code, -1);
        assert!(result.is_empty());
    }

    #[test]
    fn test_default_insert_ex() {
        let ability = DefaultDataShareExtAbility;
        let (code1, code2) = ability.insert_ex("datashare:///test", "{}");
        assert_eq!(code1, 0);
        assert_eq!(code2, 0);
    }

    #[test]
    fn test_default_update_ex() {
        let ability = DefaultDataShareExtAbility;
        let (code1, code2) = ability.update_ex("datashare:///test", "id=1", "{}");
        assert_eq!(code1, 0);
        assert_eq!(code2, 0);
    }

    #[test]
    fn test_default_delete_ex() {
        let ability = DefaultDataShareExtAbility;
        let (code1, code2) = ability.delete_ex("datashare:///test", "id=1");
        assert_eq!(code1, 0);
        assert_eq!(code2, 0);
    }

    #[test]
    fn test_default_batch_update() {
        let ability = DefaultDataShareExtAbility;
        assert_eq!(ability.batch_update(&[]), 0);
    }

    #[test]
    fn test_default_execute_batch() {
        let ability = DefaultDataShareExtAbility;
        assert_eq!(ability.execute_batch(&[]), -1);
    }

    #[test]
    fn test_default_register_observer_ext_provider() {
        let ability = DefaultDataShareExtAbility;
        assert!(ability.register_observer_ext_provider("datashare:///test", 1, true));
    }

    #[test]
    fn test_default_unregister_observer_ext_provider() {
        let ability = DefaultDataShareExtAbility;
        assert!(ability.unregister_observer_ext_provider("datashare:///test", 1));
    }

    #[test]
    fn test_default_notify_change_ext_provider() {
        let ability = DefaultDataShareExtAbility;
        assert!(ability.notify_change_ext_provider("{}"));
    }

    #[test]
    fn test_lifecycle_hooks() {
        let mut ability = DefaultDataShareExtAbility;
        ability.on_create();
        ability.on_destroy();
    }
}
