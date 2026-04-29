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

// @tc.name: ut_want_new_001
// @tc.desc: Test creating a new Want object
// @tc.precon: NA
// @tc.step: 1. Call Want::new()
//           2. Verify the initial URI is empty
// @tc.expect: New Want has empty URI
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 0
#[test]
fn ut_want_new_001() {
    let want = Want::new();
    let uri = want.get_uri();
    assert!(uri.is_empty(), "New Want should have empty URI, got '{}'", uri);
}

// @tc.name: ut_want_set_get_uri_001
// @tc.desc: Test setting and getting a URI on a Want
// @tc.precon: NA
// @tc.step: 1. Create a new Want
//           2. Set URI to "https://example.com/test"
//           3. Get the URI back
//           4. Verify it matches
// @tc.expect: URI is correctly stored and retrieved
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 0
#[test]
fn ut_want_set_get_uri_001() {
    let mut want = Want::new();
    want.set_uri("https://example.com/test");
    let uri = want.get_uri();
    assert_eq!(uri, "https://example.com/test");
}

// @tc.name: ut_want_set_get_param_001
// @tc.desc: Test setting and getting a string parameter on a Want
// @tc.precon: NA
// @tc.step: 1. Create a new Want
//           2. Set parameter "key1" = "value1"
//           3. Get parameter "key1"
//           4. Verify it returns "value1"
// @tc.expect: Parameter is correctly stored and retrieved
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 0
#[test]
fn ut_want_set_get_param_001() {
    let mut want = Want::new();
    want.set_param("key1", "value1");
    let value = want.get_param("key1");
    assert_eq!(value, "value1");
}

// @tc.name: ut_want_set_multiple_params_001
// @tc.desc: Test setting multiple parameters on a Want
// @tc.precon: NA
// @tc.step: 1. Create a new Want
//           2. Set parameters "param_a" = "aaa" and "param_b" = "bbb"
//           3. Get both parameters
//           4. Verify each value matches
// @tc.expect: Multiple parameters can coexist on the same Want
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 1
#[test]
fn ut_want_set_multiple_params_001() {
    let mut want = Want::new();
    want.set_param("param_a", "aaa");
    want.set_param("param_b", "bbb");
    assert_eq!(want.get_param("param_a"), "aaa");
    assert_eq!(want.get_param("param_b"), "bbb");
}

// @tc.name: ut_want_get_nonexistent_param_001
// @tc.desc: Test getting a parameter that was not set
// @tc.precon: NA
// @tc.step: 1. Create a new Want
//           2. Get a parameter that was never set
// @tc.expect: Returns empty string for nonexistent parameter
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 2
#[test]
fn ut_want_get_nonexistent_param_001() {
    let want = Want::new();
    let value = want.get_param("nonexistent");
    assert!(value.is_empty(), "Nonexistent param should return empty, got '{}'", value);
}

// @tc.name: ut_want_overwrite_param_001
// @tc.desc: Test overwriting an existing parameter
// @tc.precon: NA
// @tc.step: 1. Create a new Want
//           2. Set parameter "key" = "old_value"
//           3. Set parameter "key" = "new_value"
//           4. Get parameter "key"
// @tc.expect: Parameter is overwritten with the new value
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 1
#[test]
fn ut_want_overwrite_param_001() {
    let mut want = Want::new();
    want.set_param("key", "old_value");
    want.set_param("key", "new_value");
    assert_eq!(want.get_param("key"), "new_value");
}

// @tc.name: ut_uri_parse_valid_001
// @tc.desc: Test parsing a valid URI string
// @tc.precon: NA
// @tc.step: 1. Call Uri::parse() with "https://example.com/path?q=1"
//           2. Verify it returns Some
//           3. Verify the string representation matches
// @tc.expect: Valid URI is parsed successfully
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 0
#[test]
fn ut_uri_parse_valid_001() {
    let uri = Uri::parse("https://example.com/path?q=1");
    assert!(uri.is_some(), "Valid URI should parse successfully");

    let uri = uri.unwrap();
    let uri_str = uri.to_string();
    assert!(!uri_str.is_empty(), "URI string should not be empty");
}

// @tc.name: ut_uri_get_scheme_001
// @tc.desc: Test getting the scheme component of a URI
// @tc.precon: NA
// @tc.step: 1. Parse "https://example.com/path"
//           2. Call get_scheme()
//           3. Verify it returns "https"
// @tc.expect: Scheme is correctly extracted from URI
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 0
#[test]
fn ut_uri_get_scheme_001() {
    let uri = Uri::parse("https://example.com/path").unwrap();
    assert_eq!(uri.get_scheme(), "https");
}

// @tc.name: ut_uri_get_host_001
// @tc.desc: Test getting the host component of a URI
// @tc.precon: NA
// @tc.step: 1. Parse "https://example.com/path"
//           2. Call get_host()
//           3. Verify it returns "example.com"
// @tc.expect: Host is correctly extracted from URI
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 0
#[test]
fn ut_uri_get_host_001() {
    let uri = Uri::parse("https://example.com/path").unwrap();
    assert_eq!(uri.get_host(), "example.com");
}

// @tc.name: ut_uri_get_path_001
// @tc.desc: Test getting the path component of a URI
// @tc.precon: NA
// @tc.step: 1. Parse "https://example.com/path/to/resource"
//           2. Call get_path()
//           3. Verify it returns "/path/to/resource"
// @tc.expect: Path is correctly extracted from URI
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 0
#[test]
fn ut_uri_get_path_001() {
    let uri = Uri::parse("https://example.com/path/to/resource").unwrap();
    assert_eq!(uri.get_path(), "/path/to/resource");
}

// @tc.name: ut_uri_get_query_001
// @tc.desc: Test getting the query component of a URI
// @tc.precon: NA
// @tc.step: 1. Parse "https://example.com/path?key=value&foo=bar"
//           2. Call get_query()
//           3. Verify it contains the query string
// @tc.expect: Query is correctly extracted from URI
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 0
#[test]
fn ut_uri_get_query_001() {
    let uri = Uri::parse("https://example.com/path?key=value&foo=bar").unwrap();
    let query = uri.get_query();
    assert!(
        query.contains("key=value"),
        "Query should contain 'key=value', got '{}'",
        query
    );
}

// @tc.name: ut_uri_datashare_scheme_001
// @tc.desc: Test parsing a datashare URI (common in OpenHarmony)
// @tc.precon: NA
// @tc.step: 1. Parse "datashare:///com.example.app/table1"
//           2. Verify scheme is "datashare"
// @tc.expect: Datashare URI is parsed correctly
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 1
#[test]
fn ut_uri_datashare_scheme_001() {
    let uri = Uri::parse("datashare:///com.example.app/table1");
    assert!(uri.is_some(), "Datashare URI should parse successfully");
    let uri = uri.unwrap();
    assert_eq!(uri.get_scheme(), "datashare");
}

// @tc.name: ut_uri_to_string_001
// @tc.desc: Test converting a parsed URI back to string
// @tc.precon: NA
// @tc.step: 1. Parse a URI string
//           2. Convert back to string with to_string()
//           3. Verify the round-trip
// @tc.expect: URI string round-trips correctly
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 1
#[test]
fn ut_uri_to_string_001() {
    let original = "https://example.com/path";
    let uri = Uri::parse(original).unwrap();
    let result = uri.to_string();
    assert_eq!(result, original, "URI should round-trip: expected '{}', got '{}'", original, result);
}

// @tc.name: ut_want_get_action_empty_001
// @tc.desc: Test getting action from a new Want (should be empty)
// @tc.precon: NA
// @tc.step: 1. Create a new Want
//           2. Call get_action()
//           3. Verify it returns empty string
// @tc.expect: New Want should have empty action
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 0
#[test]
fn ut_want_get_action_empty_001() {
    let want = Want::new();
    let action = want.get_action();
    assert!(action.is_empty(), "New Want should have empty action, got '{}'", action);
}

// @tc.name: ut_want_set_get_action_001
// @tc.desc: Test setting and getting an action on a Want
// @tc.precon: NA
// @tc.step: 1. Create a new Want
//           2. Set action to "ohos.want.action.viewData"
//           3. Get the action back
//           4. Verify it matches
// @tc.expect: Action is correctly stored and retrieved
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 0
#[test]
fn ut_want_set_get_action_001() {
    let mut want = Want::new();
    want.set_action("ohos.want.action.viewData");
    let action = want.get_action();
    assert_eq!(action, "ohos.want.action.viewData");
}

// @tc.name: ut_want_set_element_001
// @tc.desc: Test setting and getting element on a Want
// @tc.precon: NA
// @tc.step: 1. Create a new Want
//           2. Set element with bundle "com.example.app" and ability "MainAbility"
//           3. Get bundle name and ability name back
//           4. Verify they match
// @tc.expect: Element bundle and ability names are correctly stored and retrieved
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 0
#[test]
fn ut_want_set_element_001() {
    let mut want = Want::new();
    want.set_element("com.example.app", "MainAbility");
    assert_eq!(want.get_element_bundle_name(), "com.example.app");
    assert_eq!(want.get_element_ability_name(), "MainAbility");
}

// @tc.name: ut_want_element_empty_001
// @tc.desc: Test getting element from a new Want (should be empty)
// @tc.precon: NA
// @tc.step: 1. Create a new Want
//           2. Get element bundle name and ability name
//           3. Verify both are empty
// @tc.expect: New Want should have empty element names
// @tc.type: FUNC
// @tc.require: issueNumber
// @tc.level: Level 1
#[test]
fn ut_want_element_empty_001() {
    let want = Want::new();
    assert!(
        want.get_element_bundle_name().is_empty(),
        "New Want should have empty element bundle name"
    );
    assert!(
        want.get_element_ability_name().is_empty(),
        "New Want should have empty element ability name"
    );
}
