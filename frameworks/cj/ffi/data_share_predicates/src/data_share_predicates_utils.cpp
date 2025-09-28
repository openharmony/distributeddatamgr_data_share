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

#include "data_share_predicates_utils.h"
#include "datashare_log.h"

namespace OHOS {
namespace DataShare {

SingleValue::Type parseValueType(const CValueType &value)
{
    SingleValue::Type ret;
    switch (static_cast<int32_t>(value.tag)) {
        case DataShareValueObjectType::TYPE_INT: {
            ret = value.integer;
            break;
        }
        case DataShareValueObjectType::TYPE_DOUBLE: {
            ret = value.dou;
            break;
        }
        case DataShareValueObjectType::TYPE_STRING: {
            ret = std::string(value.string);
            break;
        }
        case DataShareValueObjectType::TYPE_BOOL: {
            ret = value.boolean;
            break;
        }
        default:
            ret = 0;
            break;
    }
    return ret;
}

MutliValue::Type parseValueTypeArray(const CValueType *array, int64_t size)
{
    MutliValue::Type ret;
    if (array == nullptr) {
        LOG_ERROR("array is nullptr");
        return ret;
    }
    CValueType value = array[0];
    switch (static_cast<int32_t>(value.tag)) {
        case DataShareValueObjectType::TYPE_INT: {
            std::vector<int> arr(size);
            for (int64_t i = 0; i < size; ++i) {
                arr[i] = array[i].integer;
            }
            ret = arr;
            break;
        }
        case DataShareValueObjectType::TYPE_DOUBLE: {
            std::vector<double> arr(size);
            for (int64_t i = 0; i < size; ++i) {
                arr[i] = array[i].dou;
            }
            ret = arr;
            break;
        }
        case DataShareValueObjectType::TYPE_STRING: {
            std::vector<std::string> arr(size);
            for (int64_t i = 0; i < size; ++i) {
                arr[i] = std::string(array[i].string);
            }
            ret = arr;
            break;
        }
        default:
            break;
    }
    return ret;
}

} // namespace DataShare
} // namespace OHOS