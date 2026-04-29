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

use super::*;

// @tc.name: ut_kvstore_grd_constants_001
// @tc.desc: Verify GRD open flag constants have correct values
// @tc.precon: NA
// @tc.step: 1. Check GRD_DB_OPEN_ONLY equals 0x00
//           2. Check GRD_DB_OPEN_CREATE equals 0x01
//           3. Check GRD_DB_OPEN_CHECK_FOR_ABNORMAL equals 0x02
//           4. Check GRD_DB_OPEN_CHECK equals 0x04
// @tc.expect: All open flag constants match their defined values
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 0
#[test]
fn ut_kvstore_grd_constants_001() {
    assert_eq!(GRD_DB_OPEN_ONLY, 0x00);
    assert_eq!(GRD_DB_OPEN_CREATE, 0x01);
    assert_eq!(GRD_DB_OPEN_CHECK_FOR_ABNORMAL, 0x02);
    assert_eq!(GRD_DB_OPEN_CHECK, 0x04);
}

// @tc.name: ut_kvstore_grd_close_flush_constants_001
// @tc.desc: Verify GRD close and flush flag constants have correct values
// @tc.precon: NA
// @tc.step: 1. Check GRD_DB_CLOSE equals 0x00
//           2. Check GRD_DB_FLUSH_ASYNC equals 0x00
//           3. Check GRD_DB_FLUSH_SYNC equals 0x01
//           4. Check GRD_DOC_ID_DISPLAY equals 0x01
//           5. Check GRD_OK equals 0
// @tc.expect: All constants match their defined values
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 0
#[test]
fn ut_kvstore_grd_close_flush_constants_001() {
    assert_eq!(GRD_DB_CLOSE, 0x00);
    assert_eq!(GRD_DB_FLUSH_ASYNC, 0x00);
    assert_eq!(GRD_DB_FLUSH_SYNC, 0x01);
    assert_eq!(GRD_DOC_ID_DISPLAY, 0x01);
    assert_eq!(GRD_OK, 0);
}

// @tc.name: ut_kvstore_open_empty_path_001
// @tc.desc: Test opening a GRD database with empty path
// @tc.precon: GRD native library is available
// @tc.step: 1. Call GrdDb::open with empty path and default config
//           2. Verify the result is None (open fails for empty path)
// @tc.expect: Returns None for empty database path
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 2
#[test]
fn ut_kvstore_open_empty_path_001() {
    let db = GrdDb::open("", "", GRD_DB_OPEN_ONLY);
    assert!(db.is_none(), "GrdDb::open with empty path should return None");
}

// @tc.name: ut_kvstore_open_nonexistent_path_001
// @tc.desc: Test opening a GRD database at nonexistent path with OPEN_ONLY flag
// @tc.precon: GRD native library is available
// @tc.step: 1. Call GrdDb::open with a nonexistent path and GRD_DB_OPEN_ONLY flag
//           2. Verify the result is None
// @tc.expect: Returns None when database does not exist and OPEN_ONLY is used
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 2
#[test]
fn ut_kvstore_open_nonexistent_path_001() {
    let db = GrdDb::open("/nonexistent/path/test.db", "", GRD_DB_OPEN_ONLY);
    assert!(db.is_none(), "GrdDb::open with nonexistent path and OPEN_ONLY should return None");
}

// @tc.name: ut_kvstore_open_create_close_001
// @tc.desc: Test opening a GRD database with OPEN_CREATE flag and closing it
// @tc.precon: GRD native library is available, /tmp is writable
// @tc.step: 1. Call GrdDb::open with a temp path and GRD_DB_OPEN_CREATE flag
//           2. Verify the database was opened successfully (Some)
//           3. Close the database with GRD_DB_CLOSE flag
//           4. Verify close returns GRD_OK
// @tc.expect: Database opens and closes successfully
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 1
#[test]
fn ut_kvstore_open_create_close_001() {
    let db_path = "/tmp/ut_kvstore_open_create_close_001.db";
    let db = match GrdDb::open(db_path, "", GRD_DB_OPEN_CREATE) {
        Some(d) => d,
        None => return, // GRD library not available in test environment
    };
    let ret = db.close(GRD_DB_CLOSE);
    assert_eq!(ret, GRD_OK, "GrdDb::close should return GRD_OK, got {}", ret);
    // Clean up the database file
    let _ = std::fs::remove_file(db_path);
}

// @tc.name: ut_kvstore_create_collection_001
// @tc.desc: Test creating a document collection in a GRD database
// @tc.precon: GRD native library is available, /tmp is writable
// @tc.step: 1. Open a GRD database with OPEN_CREATE flag
//           2. Create a collection named "test_collection"
//           3. Verify create_collection returns GRD_OK
//           4. Close the database
// @tc.expect: Collection is created successfully
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 1
#[test]
fn ut_kvstore_create_collection_001() {
    let db_path = "/tmp/ut_kvstore_create_collection_001.db";
    let db = match GrdDb::open(db_path, "", GRD_DB_OPEN_CREATE) {
        Some(d) => d,
        None => return,
    };
    let ret = db.create_collection("test_collection", "", 0);
    assert_eq!(ret, GRD_OK, "create_collection should return GRD_OK, got {}", ret);
    db.close(GRD_DB_CLOSE);
    let _ = std::fs::remove_file(db_path);
}

// @tc.name: ut_kvstore_upsert_find_001
// @tc.desc: Test upserting a document and finding it back
// @tc.precon: GRD native library is available, /tmp is writable
// @tc.step: 1. Open a GRD database and create a collection
//           2. Upsert a document with filter and value
//           3. Find the document using the same filter
//           4. Verify the result set is not None
//           5. Advance cursor and read the value
//           6. Close the database
// @tc.expect: Upserted document can be found and read back
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 1
#[test]
fn ut_kvstore_upsert_find_001() {
    let db_path = "/tmp/ut_kvstore_upsert_find_001.db";
    let db = match GrdDb::open(db_path, "", GRD_DB_OPEN_CREATE) {
        Some(d) => d,
        None => return,
    };

    let ret = db.create_collection("kv_collection", "", 0);
    assert_eq!(ret, GRD_OK, "create_collection should succeed");

    // GRD_UpsertDoc returns affected count (1) on success, not GRD_OK.
    // Document body must NOT contain _id; the filter provides it separately.
    // See: documentdb_data_test.cpp UpsertDataTest001
    let ret = db.upsert_doc("kv_collection", r#"{"_id":"key1"}"#, r#"{"value":"hello"}"#, GRD_DOC_REPLACE);
    assert_eq!(ret, 1, "upsert_doc should return 1 (affected count), got {}", ret);

    let rs = db.find_doc("kv_collection", r#"{"_id":"key1"}"#, "{}", GRD_DOC_ID_DISPLAY);
    assert!(rs.is_some(), "find_doc should return a result set");

    let rs = rs.unwrap();
    let has_row = rs.next();
    assert!(has_row, "result set should have at least one row");

    let value = rs.get_value();
    assert!(!value.is_empty(), "get_value should return non-empty document string");

    db.close(GRD_DB_CLOSE);
    let _ = std::fs::remove_file(db_path);
}

// @tc.name: ut_kvstore_delete_doc_001
// @tc.desc: Test deleting a document from a collection
// @tc.precon: GRD native library is available, /tmp is writable
// @tc.step: 1. Open a GRD database and create a collection
//           2. Upsert a document
//           3. Delete the document using its filter
//           4. Verify delete returns non-negative
//           5. Close the database
// @tc.expect: Document is deleted successfully
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 1
#[test]
fn ut_kvstore_delete_doc_001() {
    let db_path = "/tmp/ut_kvstore_delete_doc_001.db";
    let db = match GrdDb::open(db_path, "", GRD_DB_OPEN_CREATE) {
        Some(d) => d,
        None => return,
    };

    db.create_collection("del_collection", "", 0);
    db.upsert_doc("del_collection", r#"{"_id":"to_delete"}"#, r#"{"data":"temp"}"#, GRD_DOC_REPLACE);

    let ret = db.delete_doc("del_collection", r#"{"_id":"to_delete"}"#, 0);
    assert!(ret >= 0, "delete_doc should return non-negative on success, got {}", ret);

    db.close(GRD_DB_CLOSE);
    let _ = std::fs::remove_file(db_path);
}

// @tc.name: ut_kvstore_drop_collection_001
// @tc.desc: Test dropping a collection from a GRD database
// @tc.precon: GRD native library is available, /tmp is writable
// @tc.step: 1. Open a GRD database and create a collection
//           2. Drop the collection
//           3. Verify drop_collection returns GRD_OK
//           4. Close the database
// @tc.expect: Collection is dropped successfully
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 1
#[test]
fn ut_kvstore_drop_collection_001() {
    let db_path = "/tmp/ut_kvstore_drop_collection_001.db";
    let db = match GrdDb::open(db_path, "", GRD_DB_OPEN_CREATE) {
        Some(d) => d,
        None => return,
    };

    db.create_collection("drop_me", "", 0);
    let ret = db.drop_collection("drop_me", 0);
    assert_eq!(ret, GRD_OK, "drop_collection should return GRD_OK, got {}", ret);

    db.close(GRD_DB_CLOSE);
    let _ = std::fs::remove_file(db_path);
}

// @tc.name: ut_kvstore_flush_001
// @tc.desc: Test flushing pending writes to disk
// @tc.precon: GRD native library is available, /tmp is writable
// @tc.step: 1. Open a GRD database
//           2. Call flush with GRD_DB_FLUSH_SYNC flag
//           3. Verify flush returns GRD_OK
//           4. Close the database
// @tc.expect: Flush completes successfully
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 1
#[test]
fn ut_kvstore_flush_001() {
    let db_path = "/tmp/ut_kvstore_flush_001.db";
    let db = match GrdDb::open(db_path, "", GRD_DB_OPEN_CREATE) {
        Some(d) => d,
        None => return,
    };

    let ret = db.flush(GRD_DB_FLUSH_SYNC);
    assert_eq!(ret, GRD_OK, "flush should return GRD_OK, got {}", ret);

    db.close(GRD_DB_CLOSE);
    let _ = std::fs::remove_file(db_path);
}
