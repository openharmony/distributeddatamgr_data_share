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

use std::ffi::CStr;
use std::collections::HashMap;
use ani_rs::{business_error::BusinessError, objects::AniObject, AniEnv, error::AniError};

use crate::{
    get_native_ptr, wrapper::{self, ValuesBucketArrayWrap, ValuesBucketHashWrap,
    ExtensionBatchUpdateParamIn, ExtensionBatchUpdateParamOut},
    DATA_SHARE_EXTENSION_HELPER, DATA_SHARE_PREDICATES, datashare::UpdateOperation,datashare_error
};

fn call_arkts_insert_inner(
    extension_ability_ptr: i64,
    env_ptr: i64,
    uri: String,
    value_bucket: &ValuesBucketHashWrap,
    native_ptr: i64,
) -> Result<(), AniError> {
    let extension_ability_obj = AniObject::from_raw(extension_ability_ptr as _);
    let env = AniEnv::from_raw(env_ptr as _);

    let helper_class = env.find_class(DATA_SHARE_EXTENSION_HELPER)?;
    let instance = env.new_object(&helper_class, (extension_ability_obj, native_ptr))?;
    let do_insert_method_name = unsafe { CStr::from_bytes_with_nul_unchecked(b"doInsert\0") };
    let do_insert_method = env.find_method(&helper_class, do_insert_method_name)?;

    let arg1 = env.serialize(&uri)?;
    let arg2 = env.serialize(value_bucket.as_ref())?;
    env.call_method(&instance, &do_insert_method, (arg1, arg2))
    .map_err(|err| {
        datashare_error!("Panic occurred: doInsert err: {}", err);
        env.exist_unhandled_error().map(|is_unhandled| {
            if is_unhandled {
                let _ = env.describe_error();
            }
        })
    });
    Ok(())
}

pub fn call_arkts_insert(
    extension_ability_ptr: i64,
    env_ptr: i64,
    uri: String,
    value_bucket: &ValuesBucketHashWrap,
    native_ptr: i64,
) {
    call_arkts_insert_inner(
        extension_ability_ptr,
        env_ptr,
        uri,
        value_bucket,
        native_ptr,
    )
    .map_err(|err| {
        datashare_error!("Panic occurred: call arkts insert failed, err: {}", err);
    });
}

fn call_arkts_batch_insert_inner(
    extension_ability_ptr: i64,
    env_ptr: i64,
    uri: String,
    value_buckets: &ValuesBucketArrayWrap,
    native_ptr: i64,
) -> Result<(), AniError> {
    let extension_ability_obj = AniObject::from_raw(extension_ability_ptr as _);
    let env = AniEnv::from_raw(env_ptr as _);

    let helper_class = env.find_class(DATA_SHARE_EXTENSION_HELPER)?;
    let instance = env.new_object(&helper_class, (extension_ability_obj, native_ptr))?;
    let do_batch_insert_method_name =
        unsafe { CStr::from_bytes_with_nul_unchecked(b"doBatchInsert\0") };
    let do_batch_insert_method = env.find_method(&helper_class, do_batch_insert_method_name)?;

    let arg1 = env.serialize(&uri)?;
    let arg2 = env.serialize(value_buckets.as_ref())?;

    env.call_method(&instance, &do_batch_insert_method, (arg1, arg2))
    .map_err(|err| {
        datashare_error!("Panic occurred: doBatchInsert err: {}", err);
        env.exist_unhandled_error().map(|is_unhandled| {
            if is_unhandled {
                let _ = env.describe_error();
            }
        })
    });
    Ok(())
}

pub fn call_arkts_batch_insert(
    extension_ability_ptr: i64,
    env_ptr: i64,
    uri: String,
    value_buckets: &ValuesBucketArrayWrap,
    native_ptr: i64,
) {
    call_arkts_batch_insert_inner(
        extension_ability_ptr,
        env_ptr,
        uri,
        value_buckets,
        native_ptr,
    )
    .map_err(|err| {
        datashare_error!("Panic occurred: call arkts batch insert failed, err: {}", err);
    });
}

fn call_arkts_update_inner(
    extension_ability_ptr: i64,
    env_ptr: i64,
    uri: String,
    predicates_ptr: i64,
    value_bucket: &ValuesBucketHashWrap,
    native_ptr: i64,
) -> Result<(), AniError> {
    let extension_ability_obj = AniObject::from_raw(extension_ability_ptr as _);
    let env = AniEnv::from_raw(env_ptr as _);

    let helper_class = env.find_class(DATA_SHARE_EXTENSION_HELPER)?;
    let instance = env.new_object(&helper_class, (extension_ability_obj, native_ptr))?;
    let do_update_method_name = unsafe { CStr::from_bytes_with_nul_unchecked(b"doUpdate\0") };
    let do_update_method = env.find_method(&helper_class, do_update_method_name)?;

    let ctor_signature = unsafe { CStr::from_bytes_with_nul_unchecked(b"J:V\0") };
    let predicates_class = env.find_class(DATA_SHARE_PREDICATES)?;
    let arg1 = env.serialize(&uri)?;
    let arg2 = env.new_object_with_signature(&predicates_class, ctor_signature, (predicates_ptr,))?;
    let arg3 = env.serialize(value_bucket.as_ref())?;
    env.call_method(&instance, &do_update_method, (arg1, arg2, arg3))
    .map_err(|err| {
        datashare_error!("Panic occurred: doUpdate err: {}", err);
        env.exist_unhandled_error().map(|is_unhandled| {
            if is_unhandled {
                let _ = env.describe_error();
            }
        })
    });
    Ok(())
}

pub fn call_arkts_update(
    extension_ability_ptr: i64,
    env_ptr: i64,
    uri: String,
    predicates_ptr: i64,
    value_bucket: &ValuesBucketHashWrap,
    native_ptr: i64,
) {
    call_arkts_update_inner(
        extension_ability_ptr,
        env_ptr,
        uri,
        predicates_ptr,
        value_bucket,
        native_ptr,
    )
    .map_err(|err| {
        datashare_error!("Panic occurred: call arkts update failed, err: {}", err);
    });
}

fn call_arkts_delete_inner(
    extension_ability_ptr: i64,
    env_ptr: i64,
    uri: String,
    predicates_ptr: i64,
    native_ptr: i64,
) -> Result<(), AniError> {
    let extension_ability_obj = AniObject::from_raw(extension_ability_ptr as _);
    let env = AniEnv::from_raw(env_ptr as _);

    let helper_class = env.find_class(DATA_SHARE_EXTENSION_HELPER)?;
    let instance = env.new_object(&helper_class, (extension_ability_obj, native_ptr))?;
    let do_delete_method_name = unsafe { CStr::from_bytes_with_nul_unchecked(b"doDelete\0") };
    let do_delete_method = env.find_method(&helper_class, do_delete_method_name)?;

    let arg1 = env.serialize(&uri)?;
    let ctor_signature = unsafe { CStr::from_bytes_with_nul_unchecked(b"J:V\0") };
    let predicates_class = env.find_class(DATA_SHARE_PREDICATES)?;
    let arg2 = env.new_object_with_signature(&predicates_class, ctor_signature, (predicates_ptr,))?;

    env.call_method(&instance, &do_delete_method, (arg1, arg2))
    .map_err(|err| {
        datashare_error!("Panic occurred: doDelete err: {}", err);
        env.exist_unhandled_error().map(|is_unhandled| {
            if is_unhandled {
                let _ = env.describe_error();
            }
        })
    });
    Ok(())
}

pub fn call_arkts_delete(
    extension_ability_ptr: i64,
    env_ptr: i64,
    uri: String,
    predicates_ptr: i64,
    native_ptr: i64,
) {
    call_arkts_delete_inner(
        extension_ability_ptr,
        env_ptr,
        uri,
        predicates_ptr,
        native_ptr,
    )
    .map_err(|err| {
        datashare_error!("Panic occurred: call arkts delete failed, err: {}", err);
    });
}

fn call_arkts_query_inner(
    extension_ability_ptr: i64,
    env_ptr: i64,
    uri: String,
    predicates_ptr: i64,
    columns: Vec<String>,
    native_ptr: i64,
) -> Result<(), AniError> {
    let extension_ability_obj = AniObject::from_raw(extension_ability_ptr as _);
    let env = AniEnv::from_raw(env_ptr as _);

    let helper_class = env.find_class(DATA_SHARE_EXTENSION_HELPER)?;
    let instance = env.new_object(&helper_class, (extension_ability_obj, native_ptr))?;
    let do_query_method_name = unsafe { CStr::from_bytes_with_nul_unchecked(b"doQuery\0") };
    let do_query_method = env.find_method(&helper_class, do_query_method_name)?;

    let arg1 = env.serialize(&uri)?;
    let ctor_signature = unsafe { CStr::from_bytes_with_nul_unchecked(b"J:V\0") };
    let predicates_class = env.find_class(DATA_SHARE_PREDICATES)?;
    let arg2 = env.new_object_with_signature(&predicates_class, ctor_signature, (predicates_ptr,))?;
    let arg3 = env.serialize(&columns)?;
    env.call_method(&instance, &do_query_method, (arg1, arg2, arg3))
    .map_err(|err| {
        datashare_error!("Panic occurred: doQuery err: {}", err);
        env.exist_unhandled_error().map(|is_unhandled| {
            if is_unhandled {
                let _ = env.describe_error();
            }
        })
    });
    Ok(())
}

pub fn call_arkts_query(
    extension_ability_ptr: i64,
    env_ptr: i64,
    uri: String,
    predicates_ptr: i64,
    columns: Vec<String>,
    native_ptr: i64,
) {
    call_arkts_query_inner(
        extension_ability_ptr,
        env_ptr,
        uri,
        predicates_ptr,
        columns,
        native_ptr,
    )
    .map_err(|err| {
        datashare_error!("Panic occurred: call arkts query failed, err: {}", err);
    });
}

fn call_arkts_on_create_inner(
    extension_ability_ptr: i64,
    env_ptr: i64,
    ani_want: i64,
    native_ptr: i64
) -> Result<(), AniError> {
    let extension_ability_obj = AniObject::from_raw(extension_ability_ptr as _);
    let env = AniEnv::from_raw(env_ptr as _);

    let helper_class = env.find_class(DATA_SHARE_EXTENSION_HELPER)?;
    let instance = env.new_object(&helper_class, (extension_ability_obj, native_ptr))?;
    let do_on_create_method_name = unsafe { CStr::from_bytes_with_nul_unchecked(b"doOnCreate\0") };
    let do_on_create_method = env.find_method(&helper_class, do_on_create_method_name)?;

    let want_object = AniObject::from_raw(ani_want as _);

    env.call_method(&instance, &do_on_create_method, (want_object,))
    .map_err(|err| {
        datashare_error!("Panic occurred: doOnCreate err: {}", err);
        env.exist_unhandled_error().map(|is_unhandled| {
            if is_unhandled {
                let _ = env.describe_error();
            }
        })
    });
    Ok(())
}

pub fn call_arkts_on_create(
    extension_ability_ptr: i64,
    env_ptr: i64,
    ani_want: i64,
    native_ptr: i64
) {
    call_arkts_on_create_inner(
        extension_ability_ptr,
        env_ptr,
        ani_want,
        native_ptr,
    )
    .map_err(|err| {
        datashare_error!("Panic occurred: call arkts on create failed, err: {}", err);
    });
}

fn call_arkts_normalize_uri_inner(
    extension_ability_ptr: i64,
    env_ptr: i64,
    uri: String,
    native_ptr: i64,
) -> Result<(), AniError> {
    let extension_ability_obj = AniObject::from_raw(extension_ability_ptr as _);
    let env = AniEnv::from_raw(env_ptr as _);
    let helper_class = env.find_class(DATA_SHARE_EXTENSION_HELPER)?;
    let instance = env.new_object(&helper_class, (extension_ability_obj, native_ptr))?;
    let do_normalize_method_name = unsafe { CStr::from_bytes_with_nul_unchecked(b"doNormalizeUri\0") };
    let do_normalize_method = env.find_method(&helper_class, do_normalize_method_name)?;
    let arg = env.serialize(&uri)?;
    env.call_method(&instance, &do_normalize_method, (arg,))
    .map_err(|err| {
        datashare_error!("Panic occurred: doNormalizeUri err: {}", err);
        env.exist_unhandled_error().map(|is_unhandled| {
            if is_unhandled {
                let _ = env.describe_error();
            }
        })
    });
    Ok(())
}

pub fn call_arkts_normalize_uri(
    extension_ability_ptr: i64,
    env_ptr: i64,
    uri: String,
    native_ptr: i64,
) {
    call_arkts_normalize_uri_inner(
        extension_ability_ptr,
        env_ptr,
        uri,
        native_ptr,
    )
    .map_err(|err| {
        datashare_error!("Panic occurred: call arkts normalize uri failed, err: {}", err);
    });
}

fn call_arkts_denormalize_uri_inner(
    extension_ability_ptr: i64,
    env_ptr: i64,
    uri: String,
    native_ptr: i64,
) -> Result<(), AniError> {
    let extension_ability_obj = AniObject::from_raw(extension_ability_ptr as _);
    let env = AniEnv::from_raw(env_ptr as _);
    let helper_class = env.find_class(DATA_SHARE_EXTENSION_HELPER)?;
    let instance = env.new_object(&helper_class, (extension_ability_obj, native_ptr))?;
    let do_denormalize_method_name = unsafe { CStr::from_bytes_with_nul_unchecked(b"doDenormalizeUri\0") };
    let do_denormalize_method = env.find_method(&helper_class, do_denormalize_method_name)?;
    let arg = env.serialize(&uri)?;
    env.call_method(&instance, &do_denormalize_method, (arg,))
    .map_err(|err| {
        datashare_error!("Panic occurred: doDenormalizeUri err: {}", err);
        env.exist_unhandled_error().map(|is_unhandled| {
            if is_unhandled {
                let _ = env.describe_error();
            }
        })
    });
    Ok(())
}

pub fn call_arkts_denormalize_uri(
    extension_ability_ptr: i64,
    env_ptr: i64,
    uri: String,
    native_ptr: i64,
) {
    call_arkts_denormalize_uri_inner(
        extension_ability_ptr,
        env_ptr,
        uri,
        native_ptr,
    )
    .map_err(|err| {
        datashare_error!("Panic occurred: call arkts denormalize uri failed, err: {}", err);
    });
}

fn call_arkts_batch_update_inner(
    extension_ability_ptr: i64,
    env_ptr: i64,
    param_out: &ExtensionBatchUpdateParamOut,
    native_ptr: i64,
) -> Result<(), AniError> {
    let extension_ability_obj = AniObject::from_raw(extension_ability_ptr as _);
    let env = AniEnv::from_raw(env_ptr as _);
    let helper_class = env.find_class(DATA_SHARE_EXTENSION_HELPER)?;
    let instance = env.new_object(&helper_class, (extension_ability_obj, native_ptr))?;
    let do_batch_update_method_name = unsafe { CStr::from_bytes_with_nul_unchecked(b"doBatchUpdate\0") };
    let do_batch_update_method = env.find_method(&helper_class, do_batch_update_method_name)?;
    let arg = env.serialize(&param_out.operations)?;
    env.call_method(&instance, &do_batch_update_method, (arg,))
    .map_err(|err| {
        datashare_error!("Panic occurred: doBatchUpdate err: {}", err);
        env.exist_unhandled_error().map(|is_unhandled| {
            if is_unhandled {
                let _ = env.describe_error();
            }
        })
    });
    Ok(())
}

pub fn call_arkts_batch_update(
    extension_ability_ptr: i64,
    env_ptr: i64,
    param_out: &ExtensionBatchUpdateParamOut,
    native_ptr: i64,
) {
    call_arkts_batch_update_inner(
        extension_ability_ptr,
        env_ptr,
        param_out,
        native_ptr,
    )
    .map_err(|err| {
        datashare_error!("Panic occurred: call arkts batch update failed, err: {}", err);
    });
}

#[ani_rs::native]
pub fn native_extension_callback_int(
    error_code: f64,
    error_msg: String,
    data: i32,
    native_ptr: i64
) -> Result<(), BusinessError> {
    wrapper::ffi::DataShareNativeExtensionCallbackInt(error_code, error_msg, data, native_ptr);
    Ok(())
}

#[ani_rs::native]
pub fn native_extension_callback_object(
    env: &AniEnv,
    error_code: f64,
    error_msg: String,
    data: AniObject,
    native_ptr: i64,
) -> Result<(), BusinessError> {
    let rdb_cls_name = unsafe { 
        CStr::from_bytes_with_nul_unchecked(b"L@ohos/data/relationalStore/relationalStore/_taihe_ResultSet_inner;\0")
    };
    let rdb_cls = env.find_class(rdb_cls_name).ok();
    let kv_cls_name = unsafe {
        CStr::from_bytes_with_nul_unchecked(b"L@ohos/data/distributedKVStore/distributedKVStore/_taihe_KVStoreResultSet_inner;\0")
    };
    let kv_cls = env.find_class(kv_cls_name).ok();

    let cls = if rdb_cls.is_some() && env.instance_of(&data, rdb_cls.as_ref().unwrap())? {
        rdb_cls
    } else if kv_cls.is_some() && env.instance_of(&data, kv_cls.as_ref().unwrap())? {
        kv_cls
    } else {
        None
    };

    if let Some(cls) = cls {
        let get_inner_method_name = unsafe { CStr::from_bytes_with_nul_unchecked(b"getProxy\0") };
        let get_inner_method = env
            .find_method(&cls, get_inner_method_name)?;
        let object_ptr = env
            .call_method_long(&data, &get_inner_method, ())
            .unwrap_or(0);
        wrapper::ffi::DataShareNativeExtensionCallbackObject(error_code, error_msg, object_ptr, native_ptr);
    } else {
        datashare_error!("Panic occurred: data does not match any available class");
    }
    Ok(())
}

#[ani_rs::native]
pub fn native_extension_callback_void(
    error_code: f64,
    error_msg: String,
    native_ptr: i64
) -> Result<(), BusinessError> {
    wrapper::ffi::DataShareNativeExtensionCallbackVoid(error_code, error_msg, native_ptr);
    Ok(())
}

#[ani_rs::native]
pub fn native_extension_callback_string(
    error_code: f64,
    error_msg: String,
    data: String,
    native_ptr: i64,
) -> Result<(), BusinessError> {
    wrapper::ffi::DataShareNativeExtensionCallbackString(error_code, error_msg, data, native_ptr);
    Ok(())
}

#[ani_rs::native]
pub fn native_extension_callback_batch_update(
    error_code: f64,
    error_msg: String,
    data: HashMap<String, Vec<i32>>,
    native_ptr: i64,
) -> Result<(), BusinessError> {
    let mut param_in: ExtensionBatchUpdateParamIn = ExtensionBatchUpdateParamIn::new();
    for (key, value) in data {
        param_in.data.insert(key, value);
    }
    wrapper::ffi::DataShareNativeExtensionCallbackBatchUpdate(error_code, error_msg, &param_in, native_ptr);
    Ok(())
}
