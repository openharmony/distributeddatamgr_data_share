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

#[macro_export]
macro_rules! bind {
    (namespace $en:ident $path:literal [$($func_path:literal : $func:path ),* $(,)?]) => {{
        let functions = [
            $(
                (
                    unsafe { std::ffi::CStr::from_bytes_with_nul_unchecked(concat!($func_path, "\0").as_bytes()) },
                    $func as _,
                ),
            )*
        ];

        let path = unsafe { std::ffi::CStr::from_bytes_with_nul_unchecked(concat!($path, "\0").as_bytes()) };

        let namespace = $en.find_namespace(path).unwrap();
        $en.bind_namespace_functions(namespace, &functions).unwrap();
    }};

    (class $en:ident $path:literal [$($func_path:literal : $func:path ),* $(,)?]) => {{
        let functions = [
            $(
                (
                    unsafe { std::ffi::CStr::from_bytes_with_nul_unchecked(concat!($func_path, "\0").as_bytes()) },
                    $func as _,
                ),
            )*
        ];

        let path = unsafe { std::ffi::CStr::from_bytes_with_nul_unchecked(concat!($path, "\0").as_bytes()) };
        let class = $en.find_class(path).unwrap();
        $en.bind_class_methods(class, &functions).unwrap();
    }};
}

#[macro_export]
macro_rules! ani_constructor {
    ($($type:tt $path:literal [$($func_path:literal : $func:path ),* $(,)?])*) => {
        #[no_mangle]
        pub extern "C" fn ANI_Constructor(vm: ani_rs::AniVm, result: *mut u32) -> std::ffi::c_uint {
            let env = vm.get_env().unwrap();
            ani_rs::AniVm::init(vm);
            $(
                ani_rs::bind!($type env $path [$($func_path : $func),*]);
            )*
            unsafe {*result = 1;}
            0
        }
    };
}
