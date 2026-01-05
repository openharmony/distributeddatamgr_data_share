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

use std::{
    error::Error,
    ffi::FromBytesWithNulError,
    fmt::{Debug, Display},
};

use crate::business_error::BusinessError;

#[derive(Debug)]
pub struct AniError {
    code: Option<AniErrorCode>,
    message: Msg,
}

pub(crate) enum Msg {
    Literal(&'static str),
    Temp(String),
}

impl Display for Msg {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        Debug::fmt(&self, f)
    }
}

impl Debug for Msg {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            Msg::Literal(s) => write!(f, "{}", s),
            Msg::Temp(s) => write!(f, "{}", s),
        }
    }
}

impl AniError {
    pub fn message(s: String) -> Self {
        Self {
            code: None,
            message: Msg::Temp(s),
        }
    }

    pub const fn literal(s: &'static str) -> Self {
        Self {
            code: None,
            message: Msg::Literal(s),
        }
    }

    pub(crate) fn from_code(message: String, code: u32) -> Self {
        let code = match code {
            1 => Some(AniErrorCode::Error),
            2 => Some(AniErrorCode::InvalidArgs),
            3 => Some(AniErrorCode::InvalidType),
            4 => Some(AniErrorCode::InvalidDescriptor),
            5 => Some(AniErrorCode::IncorrectRef),
            6 => Some(AniErrorCode::PendingError),
            7 => Some(AniErrorCode::NotFound),
            8 => Some(AniErrorCode::AlreadyBinded),
            9 => Some(AniErrorCode::OutOfRef),
            10 => Some(AniErrorCode::OutOfMemory),
            11 => Some(AniErrorCode::OutOfRange),
            12 => Some(AniErrorCode::BufferTooSmall),
            13 => Some(AniErrorCode::InvalidVersion),
            _ => None,
        };
        Self {
            code,
            message: Msg::Temp(message),
        }
    }
}

#[derive(Clone, Debug)]
pub enum AniErrorCode {
    Error = 1,
    InvalidArgs,
    InvalidType,
    InvalidDescriptor,
    IncorrectRef,
    PendingError,
    NotFound,
    AlreadyBinded,
    OutOfRef,
    OutOfMemory,
    OutOfRange,
    BufferTooSmall,
    InvalidVersion,
}

impl Display for AniErrorCode {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            AniErrorCode::Error => write!(f, "Error"),
            AniErrorCode::InvalidArgs => write!(f, "InvalidArgs"),
            AniErrorCode::InvalidType => write!(f, "InvalidType"),
            AniErrorCode::InvalidDescriptor => write!(f, "InvalidDescriptor"),
            AniErrorCode::IncorrectRef => write!(f, "IncorrectRef"),
            AniErrorCode::PendingError => write!(f, "PendingError"),
            AniErrorCode::NotFound => write!(f, "NotFound"),
            AniErrorCode::AlreadyBinded => write!(f, "AlreadyBinded"),
            AniErrorCode::OutOfRef => write!(f, "OutOfRef"),
            AniErrorCode::OutOfMemory => write!(f, "OutOfMemory"),
            AniErrorCode::OutOfRange => write!(f, "OutOfRange"),
            AniErrorCode::BufferTooSmall => write!(f, "BufferTooSmall"),
            AniErrorCode::InvalidVersion => write!(f, "InvalidVersion"),
        }
    }
}

impl Display for AniError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        if let Some(code) = &self.code {
            write!(f, "{}: {}", code, self.message)
        } else {
            write!(f, "{}", self.message)
        }
    }
}

impl Error for AniError {}

impl serde::ser::Error for AniError {
    fn custom<T>(msg: T) -> Self
    where
        T: Display,
    {
        AniError {
            code: None,
            message: Msg::Temp(msg.to_string()),
        }
    }
}

impl serde::de::Error for AniError {
    fn custom<T>(msg: T) -> Self
    where
        T: Display,
    {
        AniError {
            code: None,
            message: Msg::Temp(msg.to_string()),
        }
    }
}

impl From<FromBytesWithNulError> for AniError {
    fn from(value: FromBytesWithNulError) -> Self {
        AniError {
            code: None,
            message: Msg::Temp(format!("{}", value)),
        }
    }
}

impl From<AniError> for BusinessError {
    fn from(value: AniError) -> Self {
        let code = value.code.as_ref().map_or(-1, |ani_code| {
            ani_code.clone() as i32
        });
        let msg = match value.message {
            Msg::Literal(s) => s.to_string(),
            Msg::Temp(s) => s,
        };
        BusinessError::new(code, msg)
    }
}