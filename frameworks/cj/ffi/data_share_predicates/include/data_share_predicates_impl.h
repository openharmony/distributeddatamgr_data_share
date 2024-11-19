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

#ifndef DATA_SHARE_PREDICATES_IMPL_PREDICATES_H
#define DATA_SHARE_PREDICATES_IMPL_PREDICATES_H

#include <memory>

#include "cj_common_ffi.h"
#include "data_share_predicates_utils.h"
#include "ffi_remote_data.h"

namespace OHOS {
namespace DataShare {

class DataSharePredicatesImpl : public OHOS::FFI::FFIData {
    DECL_TYPE(DataSharePredicatesImpl, OHOS::FFI::FFIData)
public:
    explicit DataSharePredicatesImpl();

    std::shared_ptr<DataSharePredicates> GetPredicates();

    void EqualTo(const char *field, CValueType value);

    void And();

    void OrderByAsc(const char *field);

    void OrderByDesc(const char *field);

    void Limit(const int32_t total, const int32_t offset);

    void In(const char *field, CValueType *values, int64_t valuesSize);

    void Or();

    void BeginWrap();

    void EndWrap();

private:
    std::shared_ptr<DataSharePredicates> predicates_;
};
} // namespace DataShare
} // namespace OHOS

#endif // endif DATA_SHARE_PREDICATES_IMPL_H
