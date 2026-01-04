// Copyright (C) 2025 Huawei Device Co., Ltd.
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

pub struct BusinessError {
    code: i32,
    message: Message,
}

enum Message {
    S(String),
    Str(&'static str),
}

impl BusinessError {
    pub const PERMISSION: Self = BusinessError {
        code: 201,
        message: Message::Str("Permission denied"),
    };

    pub const PARAMETER: Self = BusinessError {
        code: 401,
        message: Message::Str("Parameter error"),
    };

    pub fn new(code: i32, message: String) -> Self {
        BusinessError {
            code,
            message: Message::S(message),
        }
    }

    pub const fn new_static(code: i32, message: &'static str) -> Self {
        BusinessError {
            code,
            message: Message::Str(message),
        }
    }

    pub fn code(&self) -> i32 {
        self.code
    }

    pub fn message(&self) -> &str {
        match &self.message {
            Message::S(s) => s,
            Message::Str(s) => s,
        }
    }
}
