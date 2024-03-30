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

#include "datashare_valuebucket_convert.h"

namespace OHOS::DataShare {

VBuckets ValueProxy::Convert(std::vector<DataShare::DataShareValuesBucket> &&dataShareValuesBuckets)
{
    size_t length = dataShareValuesBuckets.size();
    VBuckets res;
    res.reserve(length);
    for (auto &dataShareValuesBucket : dataShareValuesBuckets) {
        VBucket vBucket;
        for (auto &[k, v] : dataShareValuesBucket.valuesMap) {
            Value value;
            DataShare::Convert<DataShareValueObject::Type>(std::move(v), value);
            vBucket.emplace(k, std::move(value));
        }
        res.emplace_back(std::move(vBucket));
    }
    return res;
}

std::vector<DataShareValuesBucket> ValueProxy::Convert(VBuckets &&vBuckets)
{
    size_t length = vBuckets.size();
    std::vector<DataShareValuesBucket> res;
    res.reserve(length);
    for (auto &vBucket : vBuckets) {
        DataShareValuesBucket dataShareValuesBucket;
        for (auto &[k, v] : vBucket) {
            DataShareValueObject::Type dataShareValueObject;
            DataShare::Convert<Value>(std::move(v), dataShareValueObject);
            dataShareValuesBucket.valuesMap.emplace(k, std::move(dataShareValueObject));
        }
        res.emplace_back(std::move(dataShareValuesBucket));
    }
    return res;
}
} // namespace OHOS::DataShare