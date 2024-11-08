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

#include "data_share_predicates_ffi.h"
#include "data_share_predicates_impl.h"
#include "data_share_predicates_utils.h"

using namespace OHOS::FFI;

namespace OHOS {
namespace DataShare {
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif
int64_t FfiOHOSDataSharePredicatesCreateDataSharePredicates()
{
    auto impl = FFIData::Create<DataSharePredicatesImpl>();
    if (impl == nullptr) {
        return -1;
    }
    return impl->GetID();
}

int32_t FfiOHOSDataSharePredicatesEqualTo(int64_t id, const char *field, CValueType value)
{
    auto impl = FFIData::GetData<DataSharePredicatesImpl>(id);
    if (impl == nullptr) {
        return -1;
    }
    impl->EqualTo(field, value);
    return 0;
}

int32_t FfiOHOSDataSharePredicatesAnd(int64_t id)
{
    auto impl = FFIData::GetData<DataSharePredicatesImpl>(id);
    if (impl == nullptr) {
        return -1;
    }
    impl->And();
    return 0;
}

int32_t FfiOHOSDataSharePredicatesOrderByAsc(int64_t id, const char *field)
{
    auto impl = FFIData::GetData<DataSharePredicatesImpl>(id);
    if (impl == nullptr) {
        return -1;
    }
    impl->OrderByAsc(field);
    return 0;
}

int32_t FfiOHOSDataSharePredicatesOrderByDesc(int64_t id, const char *field)
{
    auto impl = FFIData::GetData<DataSharePredicatesImpl>(id);
    if (impl == nullptr) {
        return -1;
    }
    impl->OrderByDesc(field);
    return 0;
}

int32_t FfiOHOSDataSharePredicatesLimit(int64_t id, const int32_t field, const int32_t value)
{
    auto impl = FFIData::GetData<DataSharePredicatesImpl>(id);
    if (impl == nullptr) {
        return -1;
    }
    impl->Limit(field, value);
    return 0;
}

int32_t FfiOHOSDataSharePredicatesIn(int64_t id, const char *field, CValueType *values, int64_t valuesSize)
{
    auto impl = FFIData::GetData<DataSharePredicatesImpl>(id);
    if (impl == nullptr) {
        return -1;
    }
    impl->In(field, values, valuesSize);
    return 0;
}

int32_t FfiOHOSDataSharePredicatesOr(int64_t id)
{
    auto impl = FFIData::GetData<DataSharePredicatesImpl>(id);
    if (impl == nullptr) {
        return -1;
    }
    impl->Or();
    return 0;
}

int32_t FfiOHOSDataSharePredicatesBeginWrap(int64_t id)
{
    auto impl = FFIData::GetData<DataSharePredicatesImpl>(id);
    if (impl == nullptr) {
        return -1;
    }
    impl->BeginWrap();
    return 0;
}

int32_t FfiOHOSDataSharePredicatesEndWrap(int64_t id)
{
    auto impl = FFIData::GetData<DataSharePredicatesImpl>(id);
    if (impl == nullptr) {
        return -1;
    }
    impl->EndWrap();
    return 0;
}
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif
} // namespace DataShare
} // namespace OHOS
