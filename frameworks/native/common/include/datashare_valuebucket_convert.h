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
#include "datashare_observer.h"
#include "traits.h"

namespace OHOS::DataShare {
using Value = DataShareObserver::ChangeInfo::Value;
using VBucket = DataShareObserver::ChangeInfo::VBucket;
using VBuckets = DataShareObserver::ChangeInfo::VBuckets;

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
    static VBuckets Convert(std::vector<DataShare::DataShareValuesBucket> &&dataShareValuesBuckets);
    static std::vector<DataShare::DataShareValuesBucket> Convert(VBuckets &&vBuckets);
};
}
#endif // DATASHARE_VALUEBUCKET_CONVERT_H