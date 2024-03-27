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

#ifndef DATASHARE_VALUEBUCKET_CONVERT_H
#define DATASHARE_VALUEBUCKET_CONVERT_H

#include "datashare_value_object.h"
#include "datashare_values_bucket.h"
#include "traits.h"

namespace OHOS::CommonType {
using Value = std::variant<std::monostate, int64_t, double, std::string, bool, std::vector<uint8_t>>;
using VBucket = std::map<std::string, Value>;
using VBuckets = std::vector<VBucket>;

template<typename T>
static inline constexpr size_t TYPE_INDEX = Traits::variant_index_of_v<T, CommonType::Value>;
static inline constexpr size_t TYPE_MAX = Traits::variant_size_of_v<CommonType::Value>;

template<typename T, typename O>
static bool GetItem(T&& input, O& output)
{
    return false;
}

template<typename T, typename O, typename First, typename... Rest>
static bool GetItem(T&& input, O& output)
{
    auto val = Traits::get_if<First>(&input);
    if (val != nullptr) {
        output = std::move(*val);
        return true;
    }
    return GetItem<T, O, Rest...>(std::move(input), output);
}

template<typename T, typename... Types>
static bool Convert(T&& input, std::variant<Types...>& output)
{
    return GetItem<T, decltype(output), Types...>(std::move(input), output);
}
}

namespace OHOS::DataShare {
class ValueProxy {
public:
    static CommonType::VBuckets Convert(std::vector<DataShare::DataShareValuesBucket> &&dataShareValuesBuckets);
    static std::vector<DataShare::DataShareValuesBucket> Convert(CommonType::VBuckets &&vBuckets);
};
}
#endif // DATASHARE_VALUEBUCKET_CONVERT_H