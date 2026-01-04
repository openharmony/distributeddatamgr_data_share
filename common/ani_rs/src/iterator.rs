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

use crate::{
    signature::{ITERATOR, NEXT, VALUE},
    error::AniError,
    objects::{AniObject, AniRef},
    AniEnv,
};

pub struct AniIter<'local> {
    env: AniEnv<'local>,
    iter: AniObject<'local>,
}

macro_rules! to_try {
    ($i:expr) => {
        match $i {
            Ok(value) => value,
            Err(err) => {
                return Some(Err(err));
            }
        }
    };
}

impl<'local> AniIter<'local> {
    pub fn new(env: &AniEnv<'local>, iter: AniObject<'local>) -> Self {
        AniIter {
            env: env.clone(),
            iter: iter,
        }
    }
}

impl<'local> Iterator for AniIter<'local> {
    type Item = Result<(AniRef<'local>, AniRef<'local>), AniError>;
    fn next(&mut self) -> Option<Result<(AniRef<'local>, AniRef<'local>), AniError>> {
        let class = to_try!(self.env.find_class(ITERATOR));
        let method = to_try!(self.env.find_method(&class, NEXT));
        let next = to_try!(self.env.call_method_ref(&self.iter, &method, ()));
        let value: AniRef<'local> = to_try!(self.env.get_property(&next.into(), VALUE));
        let value: AniObject<'local> = value.into();
        if to_try!(self.env.is_undefined(&value)) {
            return None;
        }
        let key = to_try!(self.env.get_tuple_value_ref(&value, 0));
        let val = to_try!(self.env.get_tuple_value_ref(&value, 1));
        Some(Ok((key, val)))
    }
}
