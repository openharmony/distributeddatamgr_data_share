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

#ifndef DATA_SHARE_PREDICATES_UTILS_H
#define DATA_SHARE_PREDICATES_UTILS_H

#include <cstdint>
#include <memory>

#include "datashare_predicates.h"
#include "datashare_value_object.h"
#include "value_object.h"

namespace OHOS {
namespace DataShare {
struct CValueType {
    int64_t integer;
    double dou;
    bool boolean;
    char *string;
    uint8_t tag;
};

SingleValue::Type parseValueType(const CValueType &value);
MutliValue::Type parseValueTypeArray(const CValueType *array, int64_t size);
} // namespace DataShare
} // namespace OHOS

#endif // endif DATA_SHARE_PREDICATES_UTILS_H
