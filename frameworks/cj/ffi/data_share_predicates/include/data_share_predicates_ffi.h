/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef DATA_SHARE_PREDICATES_FFI_H
#define DATA_SHARE_PREDICATES_FFI_H

#include "data_share_predicates_utils.h"
#include "ffi_remote_data.h"

namespace OHOS {
namespace DataShare {
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

FFI_EXPORT int64_t FfiOHOSDataSharePredicatesCreateDataSharePredicates();

FFI_EXPORT int32_t FfiOHOSDataSharePredicatesEqualTo(int64_t id, const char *field, CValueType value);

FFI_EXPORT int32_t FfiOHOSDataSharePredicatesAnd(int64_t id);

FFI_EXPORT int32_t FfiOHOSDataSharePredicatesOrderByAsc(int64_t id, const char *field);

FFI_EXPORT int32_t FfiOHOSDataSharePredicatesOrderByDesc(int64_t id, const char *field);

FFI_EXPORT int32_t FfiOHOSDataSharePredicatesLimit(int64_t id, const int32_t field, const int32_t value);

FFI_EXPORT int32_t FfiOHOSDataSharePredicatesIn(int64_t id, const char *field, CValueType *values, int64_t valuesSize);

FFI_EXPORT int32_t FfiOHOSDataSharePredicatesOr(int64_t id);

FFI_EXPORT int32_t FfiOHOSDataSharePredicatesBeginWrap(int64_t id);

FFI_EXPORT int32_t FfiOHOSDataSharePredicatesEndWrap(int64_t id);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
} // namespace DataShare
} // namespace OHOS

#endif // endif DATA_SHARE_PREDICATES_FFI_H
