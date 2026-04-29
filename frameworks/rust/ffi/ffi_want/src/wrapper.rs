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

//! CXX bridge wrapper for Want and Uri types.

/// FFI declarations for Want and Uri operations.
#[cxx::bridge(namespace = "ffi_want")]
pub mod ffi {
    unsafe extern "C++" {
        include!("ffi_want_bridge.h");

        /// Opaque C++ Uri handle.
        type Uri;
        /// Opaque C++ Want handle.
        type Want;

        /// Parses a URI string and returns a Uri object.
        fn parse_uri(uri_str: &str) -> UniquePtr<Uri>;
        /// Converts a Uri to its string representation.
        fn uri_to_string(uri: &Uri) -> String;
        /// Returns the path component of the URI.
        fn uri_get_path(uri: &Uri) -> String;
        /// Returns the host component of the URI.
        fn uri_get_host(uri: &Uri) -> String;
        /// Returns the query component of the URI.
        fn uri_get_query(uri: &Uri) -> String;
        /// Returns the scheme component of the URI.
        fn uri_get_scheme(uri: &Uri) -> String;

        /// Creates a new empty Want object.
        fn create_want() -> UniquePtr<Want>;
        /// Sets the URI on a Want object.
        fn want_set_uri(want: Pin<&mut Want>, uri_str: &str);
        /// Returns the URI from a Want object.
        fn want_get_uri(want: &Want) -> String;
        /// Sets a string parameter on a Want object.
        fn want_set_param(want: Pin<&mut Want>, key: &str, value: &str);
        /// Returns a string parameter from a Want object.
        fn want_get_param(want: &Want, key: &str) -> String;
        /// Sets the element (bundle name and ability name) on a Want object.
        fn want_set_element(want: Pin<&mut Want>, bundle: &str, ability: &str);
        /// Returns the action string from a Want object.
        fn want_get_action(want: &Want) -> String;
        /// Sets the action string on a Want object.
        fn want_set_action(want: Pin<&mut Want>, action: &str);
        /// Returns the bundle name from the Want's element.
        fn want_get_element_bundle_name(want: &Want) -> String;
        /// Returns the ability name from the Want's element.
        fn want_get_element_ability_name(want: &Want) -> String;
    }
}

/// Safe wrapper around Uri
pub struct Uri {
    inner: cxx::UniquePtr<ffi::Uri>,
}

impl std::fmt::Display for Uri {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{}", ffi::uri_to_string(&self.inner))
    }
}

impl Uri {
    /// Parses a URI string into a structured representation.
    pub fn parse(uri_str: &str) -> Option<Self> {
        let inner = ffi::parse_uri(uri_str);
        if inner.is_null() {
            None
        } else {
            Some(Self { inner })
        }
    }

    /// Converts the URI to its string representation.
    pub fn as_string(&self) -> String {
        ffi::uri_to_string(&self.inner)
    }

    /// Returns the path component of the URI.
    pub fn get_path(&self) -> String {
        ffi::uri_get_path(&self.inner)
    }

    /// Returns the host component of the URI.
    pub fn get_host(&self) -> String {
        ffi::uri_get_host(&self.inner)
    }

    /// Returns the query component of the URI.
    pub fn get_query(&self) -> String {
        ffi::uri_get_query(&self.inner)
    }

    /// Returns the scheme component of the URI.
    pub fn get_scheme(&self) -> String {
        ffi::uri_get_scheme(&self.inner)
    }
}

/// Safe wrapper around Want
pub struct Want {
    inner: cxx::UniquePtr<ffi::Want>,
}

impl Default for Want {
    fn default() -> Self {
        Self {
            inner: ffi::create_want(),
        }
    }
}

impl Want {
    /// Creates a new instance.
    pub fn new() -> Self {
        Self::default()
    }

    /// Sets the URI string on this Want.
    pub fn set_uri(&mut self, uri_str: &str) {
        ffi::want_set_uri(self.inner.pin_mut(), uri_str);
    }

    /// Returns the URI string from this Want.
    pub fn get_uri(&self) -> String {
        ffi::want_get_uri(&self.inner)
    }

    /// Sets a key-value parameter on this Want.
    pub fn set_param(&mut self, key: &str, value: &str) {
        ffi::want_set_param(self.inner.pin_mut(), key, value);
    }

    /// Returns the value of a parameter by key.
    pub fn get_param(&self, key: &str) -> String {
        ffi::want_get_param(&self.inner, key)
    }

    /// Sets the element (bundle and ability) on this Want.
    pub fn set_element(&mut self, bundle: &str, ability: &str) {
        ffi::want_set_element(self.inner.pin_mut(), bundle, ability);
    }

    /// Returns the action string from this Want.
    pub fn get_action(&self) -> String {
        ffi::want_get_action(&self.inner)
    }

    /// Sets the action string on this Want.
    pub fn set_action(&mut self, action: &str) {
        ffi::want_set_action(self.inner.pin_mut(), action);
    }

    /// Returns the bundle name from the element of this Want.
    pub fn get_element_bundle_name(&self) -> String {
        ffi::want_get_element_bundle_name(&self.inner)
    }

    /// Returns the ability name from the element of this Want.
    pub fn get_element_ability_name(&self) -> String {
        ffi::want_get_element_ability_name(&self.inner)
    }
}

#[cfg(test)]
mod ut_wrapper {
    include!("../tests/ut/ut_wrapper.rs");
}
