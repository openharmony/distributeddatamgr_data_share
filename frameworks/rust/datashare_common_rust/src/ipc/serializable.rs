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

//! JSON serialization framework for DataShare.
//!
//! Equivalent to C++ `Serializable` base class using nlohmann::json.
//! Since serde_json is not available in OpenHarmony, this module provides
//! a lightweight JSON value type and parser/serializer.

use std::collections::BTreeMap;
use std::fmt;

/// A JSON value, equivalent to nlohmann::json.
#[derive(Debug, Clone, PartialEq, Default)]
pub enum JsonValue {
    #[default]
    Null,
    Bool(bool),
    Int(i64),
    Uint(u64),
    Float(f64),
    String(String),
    Array(Vec<JsonValue>),
    Object(BTreeMap<String, JsonValue>),
}

impl JsonValue {
    pub fn is_null(&self) -> bool {
        matches!(self, JsonValue::Null)
    }

    pub fn is_bool(&self) -> bool {
        matches!(self, JsonValue::Bool(_))
    }

    pub fn is_string(&self) -> bool {
        matches!(self, JsonValue::String(_))
    }

    pub fn is_array(&self) -> bool {
        matches!(self, JsonValue::Array(_))
    }

    pub fn is_object(&self) -> bool {
        matches!(self, JsonValue::Object(_))
    }

    pub fn is_number(&self) -> bool {
        matches!(
            self,
            JsonValue::Int(_) | JsonValue::Uint(_) | JsonValue::Float(_)
        )
    }

    pub fn is_number_integer(&self) -> bool {
        matches!(self, JsonValue::Int(_) | JsonValue::Uint(_))
    }

    pub fn is_number_unsigned(&self) -> bool {
        match self {
            JsonValue::Uint(_) => true,
            JsonValue::Int(v) => *v >= 0,
            _ => false,
        }
    }

    pub fn as_bool(&self) -> Option<bool> {
        match self {
            JsonValue::Bool(b) => Some(*b),
            _ => None,
        }
    }

    pub fn as_i64(&self) -> Option<i64> {
        match self {
            JsonValue::Int(v) => Some(*v),
            JsonValue::Uint(v) => i64::try_from(*v).ok(),
            _ => None,
        }
    }

    pub fn as_u64(&self) -> Option<u64> {
        match self {
            JsonValue::Uint(v) => Some(*v),
            JsonValue::Int(v) if *v >= 0 => Some(*v as u64),
            _ => None,
        }
    }

    pub fn as_f64(&self) -> Option<f64> {
        match self {
            JsonValue::Float(v) => Some(*v),
            JsonValue::Int(v) => Some(*v as f64),
            JsonValue::Uint(v) => Some(*v as f64),
            _ => None,
        }
    }

    pub fn as_str(&self) -> Option<&str> {
        match self {
            JsonValue::String(s) => Some(s),
            _ => None,
        }
    }

    pub fn as_array(&self) -> Option<&Vec<JsonValue>> {
        match self {
            JsonValue::Array(a) => Some(a),
            _ => None,
        }
    }

    pub fn as_array_mut(&mut self) -> Option<&mut Vec<JsonValue>> {
        match self {
            JsonValue::Array(a) => Some(a),
            _ => None,
        }
    }

    pub fn as_object(&self) -> Option<&BTreeMap<String, JsonValue>> {
        match self {
            JsonValue::Object(o) => Some(o),
            _ => None,
        }
    }

    pub fn as_object_mut(&mut self) -> Option<&mut BTreeMap<String, JsonValue>> {
        match self {
            JsonValue::Object(o) => Some(o),
            _ => None,
        }
    }

    /// Get a sub-node by name. Returns Null if not found or name is empty.
    /// When name is empty, returns self (matching C++ GetSubNode behavior).
    pub fn get(&self, name: &str) -> &JsonValue {
        static NULL: JsonValue = JsonValue::Null;
        if name.is_empty() {
            return self;
        }
        match self {
            JsonValue::Object(map) => map.get(name).unwrap_or(&NULL),
            _ => &NULL,
        }
    }

    /// Set a value at the given key. Converts self to Object if not already.
    pub fn set(&mut self, key: &str, value: JsonValue) {
        if !self.is_object() {
            *self = JsonValue::Object(BTreeMap::new());
        }
        if let JsonValue::Object(map) = self {
            map.insert(key.to_string(), value);
        }
    }

    /// Serialize this JSON value to a string.
    pub fn dump(&self) -> String {
        let mut buf = String::new();
        self.write_to(&mut buf);
        buf
    }

    fn write_to(&self, buf: &mut String) {
        match self {
            JsonValue::Null => buf.push_str("null"),
            JsonValue::Bool(b) => buf.push_str(if *b { "true" } else { "false" }),
            JsonValue::Int(n) => {
                let s = n.to_string();
                buf.push_str(&s);
            }
            JsonValue::Uint(n) => {
                let s = n.to_string();
                buf.push_str(&s);
            }
            JsonValue::Float(f) => {
                if f.is_infinite() || f.is_nan() {
                    buf.push_str("null");
                } else if *f == (*f as i64) as f64 && f.abs() < 1e15 {
                    // Write as integer-like float to match nlohmann behavior
                    let s = format!("{:.1}", f);
                    buf.push_str(&s);
                } else {
                    let s = format!("{}", f);
                    buf.push_str(&s);
                }
            }
            JsonValue::String(s) => {
                write_json_string(buf, s);
            }
            JsonValue::Array(arr) => {
                buf.push('[');
                for (i, v) in arr.iter().enumerate() {
                    if i > 0 {
                        buf.push(',');
                    }
                    v.write_to(buf);
                }
                buf.push(']');
            }
            JsonValue::Object(map) => {
                buf.push('{');
                for (i, (k, v)) in map.iter().enumerate() {
                    if i > 0 {
                        buf.push(',');
                    }
                    write_json_string(buf, k);
                    buf.push(':');
                    v.write_to(buf);
                }
                buf.push('}');
            }
        }
    }

    /// Parse a JSON string. Returns None on failure.
    pub fn parse(input: &str) -> Option<JsonValue> {
        let trimmed = input.trim();
        if trimmed.is_empty() {
            return None;
        }
        let bytes = trimmed.as_bytes();
        let (val, pos) = parse_value(bytes, 0)?;
        // Ensure we consumed all input (after trailing whitespace)
        let end = skip_whitespace(bytes, pos);
        if end == bytes.len() {
            Some(val)
        } else {
            None
        }
    }

    /// Check if a string is valid JSON.
    pub fn accept(input: &str) -> bool {
        JsonValue::parse(input).is_some()
    }
}

fn write_json_string(buf: &mut String, s: &str) {
    buf.push('"');
    for c in s.chars() {
        match c {
            '"' => buf.push_str("\\\""),
            '\\' => buf.push_str("\\\\"),
            '\n' => buf.push_str("\\n"),
            '\r' => buf.push_str("\\r"),
            '\t' => buf.push_str("\\t"),
            c if (c as u32) < 0x20 => {
                let _ = fmt::write(buf, format_args!("\\u{:04x}", c as u32));
            }
            c => buf.push(c),
        }
    }
    buf.push('"');
}

// --- Minimal JSON parser ---

fn skip_whitespace(bytes: &[u8], mut pos: usize) -> usize {
    while pos < bytes.len() && matches!(bytes[pos], b' ' | b'\t' | b'\n' | b'\r') {
        pos += 1;
    }
    pos
}

fn parse_value(bytes: &[u8], pos: usize) -> Option<(JsonValue, usize)> {
    let pos = skip_whitespace(bytes, pos);
    if pos >= bytes.len() {
        return None;
    }
    match bytes[pos] {
        b'"' => parse_string(bytes, pos).map(|(s, p)| (JsonValue::String(s), p)),
        b'{' => parse_object(bytes, pos),
        b'[' => parse_array(bytes, pos),
        b't' | b'f' => parse_bool(bytes, pos),
        b'n' => parse_null(bytes, pos),
        b'-' | b'0'..=b'9' => parse_number(bytes, pos),
        _ => None,
    }
}

fn parse_string(bytes: &[u8], pos: usize) -> Option<(String, usize)> {
    if bytes[pos] != b'"' {
        return None;
    }
    let mut i = pos + 1;
    let mut s = String::new();
    while i < bytes.len() {
        match bytes[i] {
            b'"' => return Some((s, i + 1)),
            b'\\' => {
                i += 1;
                if i >= bytes.len() {
                    return None;
                }
                match bytes[i] {
                    b'"' => s.push('"'),
                    b'\\' => s.push('\\'),
                    b'/' => s.push('/'),
                    b'n' => s.push('\n'),
                    b'r' => s.push('\r'),
                    b't' => s.push('\t'),
                    b'b' => s.push('\u{08}'),
                    b'f' => s.push('\u{0c}'),
                    b'u' => {
                        if i + 4 >= bytes.len() {
                            return None;
                        }
                        let hex = std::str::from_utf8(&bytes[i + 1..i + 5]).ok()?;
                        let cp = u16::from_str_radix(hex, 16).ok()?;
                        // Handle surrogate pairs
                        if (0xD800..=0xDBFF).contains(&cp) {
                            if i + 10 < bytes.len() && bytes[i + 5] == b'\\' && bytes[i + 6] == b'u'
                            {
                                let hex2 = std::str::from_utf8(&bytes[i + 7..i + 11]).ok()?;
                                let cp2 = u16::from_str_radix(hex2, 16).ok()?;
                                let codepoint =
                                    0x10000 + ((cp as u32 - 0xD800) << 10) + (cp2 as u32 - 0xDC00);
                                s.push(char::from_u32(codepoint)?);
                                i += 10;
                            } else {
                                s.push(char::REPLACEMENT_CHARACTER);
                                i += 4;
                            }
                        } else {
                            s.push(char::from_u32(cp as u32)?);
                            i += 4;
                        }
                    }
                    _ => return None,
                }
                i += 1;
            }
            _ => {
                // Handle UTF-8 multi-byte
                let remaining = &bytes[i..];
                let ch = std::str::from_utf8(remaining)
                    .ok()
                    .and_then(|s| s.chars().next())?;
                s.push(ch);
                i += ch.len_utf8();
            }
        }
    }
    None
}

fn parse_number(bytes: &[u8], pos: usize) -> Option<(JsonValue, usize)> {
    let start = pos;
    let mut i = pos;
    let negative = if i < bytes.len() && bytes[i] == b'-' {
        i += 1;
        true
    } else {
        false
    };

    // Integer part
    if i >= bytes.len() {
        return None;
    }
    if bytes[i] == b'0' {
        i += 1;
    } else if bytes[i].is_ascii_digit() {
        while i < bytes.len() && bytes[i].is_ascii_digit() {
            i += 1;
        }
    } else {
        return None;
    }

    let mut is_float = false;

    // Fraction
    if i < bytes.len() && bytes[i] == b'.' {
        is_float = true;
        i += 1;
        if i >= bytes.len() || !bytes[i].is_ascii_digit() {
            return None;
        }
        while i < bytes.len() && bytes[i].is_ascii_digit() {
            i += 1;
        }
    }

    // Exponent
    if i < bytes.len() && (bytes[i] == b'e' || bytes[i] == b'E') {
        is_float = true;
        i += 1;
        if i < bytes.len() && (bytes[i] == b'+' || bytes[i] == b'-') {
            i += 1;
        }
        if i >= bytes.len() || !bytes[i].is_ascii_digit() {
            return None;
        }
        while i < bytes.len() && bytes[i].is_ascii_digit() {
            i += 1;
        }
    }

    let num_str = std::str::from_utf8(&bytes[start..i]).ok()?;
    if is_float {
        let f: f64 = num_str.parse().ok()?;
        Some((JsonValue::Float(f), i))
    } else if negative {
        let n: i64 = num_str.parse().ok()?;
        Some((JsonValue::Int(n), i))
    } else {
        let n: u64 = num_str.parse().ok()?;
        // Store small non-negative integers as Uint for C++ compatibility
        Some((JsonValue::Uint(n), i))
    }
}

fn parse_bool(bytes: &[u8], pos: usize) -> Option<(JsonValue, usize)> {
    if bytes[pos..].starts_with(b"true") {
        Some((JsonValue::Bool(true), pos + 4))
    } else if bytes[pos..].starts_with(b"false") {
        Some((JsonValue::Bool(false), pos + 5))
    } else {
        None
    }
}

fn parse_null(bytes: &[u8], pos: usize) -> Option<(JsonValue, usize)> {
    if bytes[pos..].starts_with(b"null") {
        Some((JsonValue::Null, pos + 4))
    } else {
        None
    }
}

fn parse_array(bytes: &[u8], pos: usize) -> Option<(JsonValue, usize)> {
    let mut i = pos + 1; // skip '['
    let mut arr = Vec::new();
    i = skip_whitespace(bytes, i);
    if i < bytes.len() && bytes[i] == b']' {
        return Some((JsonValue::Array(arr), i + 1));
    }
    loop {
        let (val, next) = parse_value(bytes, i)?;
        arr.push(val);
        i = skip_whitespace(bytes, next);
        if i >= bytes.len() {
            return None;
        }
        match bytes[i] {
            b']' => return Some((JsonValue::Array(arr), i + 1)),
            b',' => i += 1,
            _ => return None,
        }
    }
}

fn parse_object(bytes: &[u8], pos: usize) -> Option<(JsonValue, usize)> {
    let mut i = pos + 1; // skip '{'
    let mut map = BTreeMap::new();
    i = skip_whitespace(bytes, i);
    if i < bytes.len() && bytes[i] == b'}' {
        return Some((JsonValue::Object(map), i + 1));
    }
    loop {
        i = skip_whitespace(bytes, i);
        let (key, next) = parse_string(bytes, i)?;
        i = skip_whitespace(bytes, next);
        if i >= bytes.len() || bytes[i] != b':' {
            return None;
        }
        i += 1;
        let (val, next) = parse_value(bytes, i)?;
        map.insert(key, val);
        i = skip_whitespace(bytes, next);
        if i >= bytes.len() {
            return None;
        }
        match bytes[i] {
            b'}' => return Some((JsonValue::Object(map), i + 1)),
            b',' => i += 1,
            _ => return None,
        }
    }
}

// --- Serializable trait and helpers ---

/// Trait for types that can be serialized to/from JSON.
///
/// Equivalent to C++ `Serializable` base struct.
pub trait Serializable {
    /// Serialize this object into a JSON node.
    fn marshal(&self, node: &mut JsonValue) -> bool;

    /// Deserialize this object from a JSON node.
    fn unmarshal(&mut self, node: &JsonValue) -> bool;

    /// Serialize to a JSON value (convenience).
    fn marshall(&self) -> JsonValue {
        let mut root = JsonValue::Null;
        self.marshal(&mut root);
        root
    }

    /// Deserialize from a JSON string.
    /// Quirk: if initial parse fails, tries parsing from index 1 (C++ compatibility).
    fn unmarshall(&mut self, json_str: &str) -> bool {
        let json_obj = match JsonValue::parse(json_str) {
            Some(v) => v,
            None => {
                if json_str.is_empty() {
                    return false;
                }
                // Drop first char to adapt A's value (C++ quirk)
                match JsonValue::parse(&json_str[1..]) {
                    Some(v) => v,
                    None => return false,
                }
            }
        };
        self.unmarshal(&json_obj)
    }
}

/// Parse a JSON string. Tries dropping first char if initial parse fails.
/// Equivalent to C++ `Serializable::ToJson`.
pub fn to_json(json_str: &str) -> JsonValue {
    match JsonValue::parse(json_str) {
        Some(v) => v,
        None => {
            if json_str.is_empty() {
                return JsonValue::Null;
            }
            match JsonValue::parse(&json_str[1..]) {
                Some(v) => v,
                None => JsonValue::Null,
            }
        }
    }
}

/// Check if a string is valid JSON. Tries dropping first char if needed.
/// Equivalent to C++ `Serializable::IsJson`.
pub fn is_json(json_str: &str) -> bool {
    if JsonValue::accept(json_str) {
        return true;
    }
    if json_str.len() > 1 {
        return JsonValue::accept(&json_str[1..]);
    }
    false
}

/// Get a sub-node from a JSON object. Returns a reference to Null if not found.
/// When name is empty, returns the node itself.
pub fn get_sub_node<'a>(node: &'a JsonValue, name: &str) -> &'a JsonValue {
    static NULL: JsonValue = JsonValue::Null;
    if node.is_null() {
        return &NULL;
    }
    if name.is_empty() {
        return node;
    }
    match node {
        JsonValue::Object(map) => map.get(name).unwrap_or(&NULL),
        _ => &NULL,
    }
}

// --- GetValue overloads ---

pub fn get_string(node: &JsonValue, name: &str) -> Option<String> {
    let sub = get_sub_node(node, name);
    sub.as_str().map(|s| s.to_string())
}

pub fn get_i32(node: &JsonValue, name: &str) -> Option<i32> {
    let sub = get_sub_node(node, name);
    if !sub.is_number_integer() {
        return None;
    }
    sub.as_i64().and_then(|v| i32::try_from(v).ok())
}

pub fn get_u32(node: &JsonValue, name: &str) -> Option<u32> {
    let sub = get_sub_node(node, name);
    if !sub.is_number_unsigned() {
        return None;
    }
    sub.as_u64().and_then(|v| u32::try_from(v).ok())
}

pub fn get_i64(node: &JsonValue, name: &str) -> Option<i64> {
    let sub = get_sub_node(node, name);
    if !sub.is_number_integer() {
        return None;
    }
    sub.as_i64()
}

pub fn get_u64(node: &JsonValue, name: &str) -> Option<u64> {
    let sub = get_sub_node(node, name);
    if !sub.is_number_unsigned() {
        return None;
    }
    sub.as_u64()
}

pub fn get_u16(node: &JsonValue, name: &str) -> Option<u16> {
    let sub = get_sub_node(node, name);
    if !sub.is_number_unsigned() {
        return None;
    }
    sub.as_u64().and_then(|v| u16::try_from(v).ok())
}

pub fn get_bool(node: &JsonValue, name: &str) -> Option<bool> {
    let sub = get_sub_node(node, name);
    // C++ behavior: accept boolean or unsigned integer (0 = false, non-0 = true)
    if let Some(b) = sub.as_bool() {
        return Some(b);
    }
    if sub.is_number_unsigned() {
        return sub.as_u64().map(|v| v != 0);
    }
    None
}

pub fn get_blob(node: &JsonValue, name: &str) -> Option<Vec<u8>> {
    let sub = get_sub_node(node, name);
    let arr = sub.as_array()?;
    let mut result = Vec::with_capacity(arr.len());
    for v in arr {
        let byte = v.as_u64().and_then(|n| u8::try_from(n).ok())?;
        result.push(byte);
    }
    Some(result)
}

pub fn get_serializable<T: Serializable>(node: &JsonValue, name: &str, value: &mut T) -> bool {
    let sub = get_sub_node(node, name);
    if sub.is_null() || !sub.is_object() {
        return false;
    }
    value.unmarshal(sub)
}

pub fn get_vec<T, F>(node: &JsonValue, name: &str, parse_elem: F) -> Option<Vec<T>>
where
    F: Fn(&JsonValue, &str) -> Option<T>,
{
    let sub = get_sub_node(node, name);
    let arr = sub.as_array()?;
    let mut result = Vec::with_capacity(arr.len());
    for v in arr {
        result.push(parse_elem(v, "")?);
    }
    Some(result)
}

// --- SetValue helpers ---

pub fn set_string(node: &mut JsonValue, key: &str, value: &str) {
    node.set(key, JsonValue::String(value.to_string()));
}

pub fn set_i32(node: &mut JsonValue, key: &str, value: i32) {
    node.set(key, JsonValue::Int(value as i64));
}

pub fn set_u32(node: &mut JsonValue, key: &str, value: u32) {
    node.set(key, JsonValue::Uint(value as u64));
}

pub fn set_i64(node: &mut JsonValue, key: &str, value: i64) {
    node.set(key, JsonValue::Int(value));
}

pub fn set_u64(node: &mut JsonValue, key: &str, value: u64) {
    node.set(key, JsonValue::Uint(value));
}

pub fn set_u16(node: &mut JsonValue, key: &str, value: u16) {
    node.set(key, JsonValue::Uint(value as u64));
}

pub fn set_bool(node: &mut JsonValue, key: &str, value: bool) {
    node.set(key, JsonValue::Bool(value));
}

pub fn set_f64(node: &mut JsonValue, key: &str, value: f64) {
    node.set(key, JsonValue::Float(value));
}

pub fn set_blob(node: &mut JsonValue, key: &str, value: &[u8]) {
    let arr: Vec<JsonValue> = value.iter().map(|b| JsonValue::Uint(*b as u64)).collect();
    node.set(key, JsonValue::Array(arr));
}

pub fn set_serializable(node: &mut JsonValue, key: &str, value: &dyn Serializable) {
    let mut sub = JsonValue::Null;
    value.marshal(&mut sub);
    node.set(key, sub);
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_parse_null() {
        assert_eq!(JsonValue::parse("null"), Some(JsonValue::Null));
    }

    #[test]
    fn test_parse_bool() {
        assert_eq!(JsonValue::parse("true"), Some(JsonValue::Bool(true)));
        assert_eq!(JsonValue::parse("false"), Some(JsonValue::Bool(false)));
    }

    #[test]
    fn test_parse_numbers() {
        assert_eq!(JsonValue::parse("42"), Some(JsonValue::Uint(42)));
        assert_eq!(JsonValue::parse("-7"), Some(JsonValue::Int(-7)));
        assert_eq!(JsonValue::parse("3.14"), Some(JsonValue::Float(3.14)));
        assert_eq!(JsonValue::parse("1e10"), Some(JsonValue::Float(1e10)));
    }

    #[test]
    fn test_parse_string() {
        assert_eq!(
            JsonValue::parse(r#""hello""#),
            Some(JsonValue::String("hello".to_string()))
        );
        assert_eq!(
            JsonValue::parse(r#""he\"llo""#),
            Some(JsonValue::String("he\"llo".to_string()))
        );
        assert_eq!(
            JsonValue::parse(r#""line\nbreak""#),
            Some(JsonValue::String("line\nbreak".to_string()))
        );
    }

    #[test]
    fn test_parse_array() {
        let val = JsonValue::parse("[1,2,3]").unwrap();
        let arr = val.as_array().unwrap();
        assert_eq!(arr.len(), 3);
        assert_eq!(arr[0], JsonValue::Uint(1));
    }

    #[test]
    fn test_parse_object() {
        let val = JsonValue::parse(r#"{"key":"value","num":42}"#).unwrap();
        assert_eq!(get_string(&val, "key"), Some("value".to_string()));
        assert_eq!(get_u32(&val, "num"), Some(42));
    }

    #[test]
    fn test_parse_nested() {
        let val = JsonValue::parse(r#"{"a":{"b":[1,2]}}"#).unwrap();
        let a = val.get("a");
        assert!(a.is_object());
        let b = a.get("b");
        assert!(b.is_array());
        assert_eq!(b.as_array().unwrap().len(), 2);
    }

    #[test]
    fn test_dump_roundtrip() {
        let original = r#"{"arr":[1,2,3],"bool":true,"null":null,"str":"hello"}"#;
        let val = JsonValue::parse(original).unwrap();
        let dumped = val.dump();
        let reparsed = JsonValue::parse(&dumped).unwrap();
        assert_eq!(val, reparsed);
    }

    #[test]
    fn test_is_json() {
        assert!(is_json(r#"{"key":"value"}"#));
        assert!(!is_json("not json"));
        // C++ quirk: try dropping first char
        assert!(is_json(&format!("x{}", r#"{"key":"value"}"#)));
    }

    #[test]
    fn test_to_json_with_prefix() {
        let json_str = format!("x{}", r#"{"key":"value"}"#);
        let val = to_json(&json_str);
        assert_eq!(get_string(&val, "key"), Some("value".to_string()));
    }

    #[test]
    fn test_get_bool_from_number() {
        let val = JsonValue::parse(r#"{"flag":1}"#).unwrap();
        assert_eq!(get_bool(&val, "flag"), Some(true));
        let val = JsonValue::parse(r#"{"flag":0}"#).unwrap();
        assert_eq!(get_bool(&val, "flag"), Some(false));
    }

    #[test]
    fn test_get_blob() {
        let val = JsonValue::parse(r#"{"data":[72,101,108,108,111]}"#).unwrap();
        assert_eq!(get_blob(&val, "data"), Some(vec![72, 101, 108, 108, 111]));
    }

    #[test]
    fn test_set_and_get() {
        let mut node = JsonValue::Null;
        set_string(&mut node, "name", "test");
        set_i32(&mut node, "count", -5);
        set_u32(&mut node, "size", 100);
        set_bool(&mut node, "active", true);
        set_f64(&mut node, "ratio", 3.14);

        assert_eq!(get_string(&node, "name"), Some("test".to_string()));
        assert_eq!(get_i32(&node, "count"), Some(-5));
        assert_eq!(get_u32(&node, "size"), Some(100));
        assert_eq!(get_bool(&node, "active"), Some(true));
    }

    #[test]
    fn test_serializable_trait() {
        struct TestObj {
            name: String,
            value: i32,
        }

        impl Serializable for TestObj {
            fn marshal(&self, node: &mut JsonValue) -> bool {
                set_string(node, "name", &self.name);
                set_i32(node, "value", self.value);
                true
            }
            fn unmarshal(&mut self, node: &JsonValue) -> bool {
                if let Some(n) = get_string(node, "name") {
                    self.name = n;
                } else {
                    return false;
                }
                if let Some(v) = get_i32(node, "value") {
                    self.value = v;
                } else {
                    return false;
                }
                true
            }
        }

        let obj = TestObj {
            name: "hello".to_string(),
            value: 42,
        };
        let json = obj.marshall();
        let json_str = json.dump();

        let mut obj2 = TestObj {
            name: String::new(),
            value: 0,
        };
        assert!(obj2.unmarshall(&json_str));
        assert_eq!(obj2.name, "hello");
        assert_eq!(obj2.value, 42);
    }

    #[test]
    fn test_unmarshall_with_prefix() {
        struct Simple {
            val: i32,
        }
        impl Serializable for Simple {
            fn marshal(&self, node: &mut JsonValue) -> bool {
                set_i32(node, "val", self.val);
                true
            }
            fn unmarshal(&mut self, node: &JsonValue) -> bool {
                self.val = get_i32(node, "val").unwrap_or(0);
                true
            }
        }

        let mut s = Simple { val: 0 };
        // Prefix 'x' before valid JSON -- C++ quirk
        assert!(s.unmarshall(r#"x{"val":99}"#));
        assert_eq!(s.val, 99);
    }

    #[test]
    fn test_empty_object() {
        let val = JsonValue::parse("{}").unwrap();
        assert!(val.is_object());
        assert_eq!(val.as_object().unwrap().len(), 0);
    }

    #[test]
    fn test_empty_array() {
        let val = JsonValue::parse("[]").unwrap();
        assert!(val.is_array());
        assert_eq!(val.as_array().unwrap().len(), 0);
    }

    #[test]
    fn test_unicode_escape() {
        let val = JsonValue::parse(r#""\u0041\u0042""#).unwrap();
        assert_eq!(val.as_str(), Some("AB"));
    }

    #[test]
    fn test_whitespace_handling() {
        let val = JsonValue::parse("  { \"a\" : 1 , \"b\" : 2 }  ").unwrap();
        assert!(val.is_object());
    }

    #[test]
    fn test_get_sub_node_empty_name() {
        let val = JsonValue::parse(r#"{"key":"value"}"#).unwrap();
        let sub = get_sub_node(&val, "");
        assert!(sub.is_object());
    }

    #[test]
    fn test_invalid_json() {
        assert!(JsonValue::parse("").is_none());
        assert!(JsonValue::parse("{invalid}").is_none());
        assert!(JsonValue::parse("[1,]").is_none());
    }

    #[test]
    fn test_dump_special_chars() {
        let val = JsonValue::String("hello\nworld\t\"quoted\"".to_string());
        let dumped = val.dump();
        assert_eq!(dumped, r#""hello\nworld\t\"quoted\"""#);
        let reparsed = JsonValue::parse(&dumped).unwrap();
        assert_eq!(reparsed.as_str(), Some("hello\nworld\t\"quoted\""));
    }
}
