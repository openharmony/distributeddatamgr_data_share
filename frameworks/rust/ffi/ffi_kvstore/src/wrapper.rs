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

//! CXX bridge wrapper for GRD (Gauss Relational Database) document API.
//!
//! The data_share service uses GRD as the underlying document store for
//! KV-style operations (upsert, delete, find) via kv_delegate.

/// FFI declarations for GRD document database operations.
#[cxx::bridge(namespace = "OHOS::GRD")]
pub mod ffi {
    unsafe extern "C++" {
        include!("ffi_kvstore_bridge.h");

        /// Opaque C++ wrapper for GRD_DB database handle.
        type GrdDbCpp;

        /// Opaque C++ wrapper for GRD_ResultSet cursor handle.
        type GrdResultSetCpp;

        // ---- Database lifecycle ----

        /// Opens or creates a GRD database at the given path.
        /// Pass empty string for `config_str` to use default configuration.
        /// Returns null on failure.
        fn grd_db_open(db_path: &str, config_str: &str, flags: u32) -> UniquePtr<GrdDbCpp>;

        /// Explicitly closes the database with the given flags.
        /// After this call, the underlying handle is released and the destructor
        /// becomes a no-op.
        fn grd_db_close(db: Pin<&mut GrdDbCpp>, flags: u32) -> i32;

        /// Flushes pending writes to disk.
        fn grd_flush(db: &GrdDbCpp, flags: u32) -> i32;

        // ---- Collection management ----

        /// Creates a document collection.
        /// Pass empty string for `option_str` to use default options.
        fn grd_create_collection(
            db: &GrdDbCpp,
            collection_name: &str,
            option_str: &str,
            flags: u32,
        ) -> i32;

        /// Drops a document collection.
        fn grd_drop_collection(db: &GrdDbCpp, collection_name: &str, flags: u32) -> i32;

        // ---- Document operations ----

        /// Inserts or updates a document in the collection.
        /// Returns the number of affected documents on success, or negative error code.
        fn grd_upsert_doc(
            db: &GrdDbCpp,
            collection_name: &str,
            filter: &str,
            document: &str,
            flags: u32,
        ) -> i32;

        /// Deletes documents matching the filter.
        /// Returns the number of deleted documents on success, or negative error code.
        fn grd_delete_doc(
            db: &GrdDbCpp,
            collection_name: &str,
            filter: &str,
            flags: u32,
        ) -> i32;

        /// Finds documents matching the filter and projection.
        /// Returns null on failure.
        fn grd_find_doc(
            db: &GrdDbCpp,
            collection_name: &str,
            filter: &str,
            projection: &str,
            flags: u32,
        ) -> UniquePtr<GrdResultSetCpp>;

        // ---- ResultSet navigation ----

        /// Advances the cursor to the next row.
        /// Returns GRD_OK (0) on success, or error code when no more rows.
        fn grd_next(result_set: &GrdResultSetCpp) -> i32;

        /// Gets the JSON document value at the current cursor position.
        /// Returns empty string on failure.
        fn grd_get_value(result_set: &GrdResultSetCpp) -> String;
    }
}

// -- GRD database open flags --

/// Open existing database only; fail if it does not exist.
pub const GRD_DB_OPEN_ONLY: u32 = 0x00;

/// Create the database if it does not exist.
pub const GRD_DB_OPEN_CREATE: u32 = 0x01;

/// Open with abnormal-state recovery check (GRD_DB_OPEN_CHECK_FOR_ABNORMAL = 0x02).
pub const GRD_DB_OPEN_CHECK_FOR_ABNORMAL: u32 = 0x02;

/// Open with integrity check (GRD_DB_OPEN_CHECK = 0x04).
pub const GRD_DB_OPEN_CHECK: u32 = 0x04;

// -- GRD database close flags --

/// Standard close flag (GRD_DB_CLOSE = 0x00 per grd_type_export.h).
pub const GRD_DB_CLOSE: u32 = 0x00;

// -- GRD flush flags --

/// Asynchronous flush (GRD_DB_FLUSH_ASYNC = 0x00 per grd_type_export.h).
pub const GRD_DB_FLUSH_ASYNC: u32 = 0x00;

/// Synchronous flush (GRD_DB_FLUSH_SYNC = 0x01 per grd_type_export.h).
pub const GRD_DB_FLUSH_SYNC: u32 = 0x01;

// -- GRD find flags --

/// Include the document ID field in query results.
pub const GRD_DOC_ID_DISPLAY: u32 = 0x01;

// -- GRD document upsert flags --

/// Append mode for upsert (GRD_DOC_APPEND = 0 per grd_type_export.h).
pub const GRD_DOC_APPEND: u32 = 0;

/// Replace mode for upsert (GRD_DOC_REPLACE = 1 per grd_type_export.h).
pub const GRD_DOC_REPLACE: u32 = 1;

// -- GRD status codes --

/// Operation succeeded.
pub const GRD_OK: i32 = 0;

/// Safe Rust wrapper for GRD database handle.
pub struct GrdDb {
    inner: cxx::UniquePtr<ffi::GrdDbCpp>,
}

impl GrdDb {
    /// Opens or creates a GRD database.
    /// Pass empty string for `config_str` to use default configuration.
    pub fn open(db_path: &str, config_str: &str, flags: u32) -> Option<Self> {
        let inner = ffi::grd_db_open(db_path, config_str, flags);
        if inner.is_null() {
            None
        } else {
            Some(Self { inner })
        }
    }

    /// Explicitly closes the database with the specified flags.
    /// Consumes the handle. If not called, the destructor closes with flags=0.
    pub fn close(mut self, flags: u32) -> i32 {
        ffi::grd_db_close(self.inner.pin_mut(), flags)
    }

    /// Flushes pending writes to disk.
    pub fn flush(&self, flags: u32) -> i32 {
        ffi::grd_flush(&self.inner, flags)
    }

    /// Creates a document collection.
    pub fn create_collection(&self, name: &str, option_str: &str, flags: u32) -> i32 {
        ffi::grd_create_collection(&self.inner, name, option_str, flags)
    }

    /// Drops a document collection.
    pub fn drop_collection(&self, name: &str, flags: u32) -> i32 {
        ffi::grd_drop_collection(&self.inner, name, flags)
    }

    /// Inserts or updates a document.
    pub fn upsert_doc(&self, collection: &str, filter: &str, doc: &str, flags: u32) -> i32 {
        ffi::grd_upsert_doc(&self.inner, collection, filter, doc, flags)
    }

    /// Deletes documents matching the filter.
    pub fn delete_doc(&self, collection: &str, filter: &str, flags: u32) -> i32 {
        ffi::grd_delete_doc(&self.inner, collection, filter, flags)
    }

    /// Finds documents matching the filter and projection.
    pub fn find_doc(
        &self,
        collection: &str,
        filter: &str,
        projection: &str,
        flags: u32,
    ) -> Option<GrdResultSet> {
        let rs = ffi::grd_find_doc(&self.inner, collection, filter, projection, flags);
        if rs.is_null() {
            None
        } else {
            Some(GrdResultSet { inner: rs })
        }
    }
}

/// Safe Rust wrapper for GRD result set cursor.
pub struct GrdResultSet {
    inner: cxx::UniquePtr<ffi::GrdResultSetCpp>,
}

impl GrdResultSet {
    /// Advances to the next row. Returns true if a row is available.
    pub fn next(&self) -> bool {
        ffi::grd_next(&self.inner) == GRD_OK
    }

    /// Gets the JSON document string at the current cursor position.
    pub fn get_value(&self) -> String {
        ffi::grd_get_value(&self.inner)
    }
}

#[cfg(test)]
mod ut_wrapper {
    include!("../tests/ut/ut_wrapper.rs");
}
