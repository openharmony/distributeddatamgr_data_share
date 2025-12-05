// Copyright (c) 2025 Huawei Device Co., Ltd.
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

use crate::predicates::ValueType;

use super::ffi;

// called by c++, get ValueType's type.
pub fn value_type_get_type(v: &ValueType) -> ffi::EnumType {
    match v {
        ValueType::S(_) => ffi::EnumType::StringType,
        ValueType::F64(_) => ffi::EnumType::F64Type,
        ValueType::Boolean(_) => ffi::EnumType::BooleanType,
    }
}

// called by c++, if ValueType is String, get String.
pub fn value_type_get_string(v: &ValueType) -> String {
    if let ValueType::S(s) = v {
        return s.clone();
    }

    panic!("Not String Type!!!");
}

// called by c++, if ValueType is f64, get 64.
pub fn value_type_get_f64(v: &ValueType) -> f64 {
    if let ValueType::F64(f) = v {
        return *f;
    }

    panic!("Not F64 Type!!!");
}

// called by c++, if ValueType is bool, get bool.
pub fn value_type_get_bool(v: &ValueType) -> bool {
    if let ValueType::Boolean(b) = v {
        return *b;
    }

    panic!("Not Boolean Type!!!");
}
