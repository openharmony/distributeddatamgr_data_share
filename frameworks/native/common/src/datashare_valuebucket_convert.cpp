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

ValueProxy::Value ValueProxy::Convert(DataShareValueObject &&value)
{
    Value proxy;
    Convert(std::move(value.value), proxy.value_);
    return proxy;
}

ValueProxy::Value ValueProxy::Convert(CommonType::Value &&value)
{
    Value proxy;
    Convert(std::move(value), proxy.value_);
    return proxy;
}

ValueProxy::Values ValueProxy::Convert(std::vector<DataShareValueObject> &&values)
{
    Values proxy;
    proxy.value_.reserve(values.size());
    for (auto &value : values) {
        proxy.value_.emplace_back(Convert(std::move(value)));
    }
    return proxy;
}

ValueProxy::Values ValueProxy::Convert(std::vector<CommonType::Value> &&values)
{
    Values proxy;
    proxy.value_.reserve(values.size());
    for (auto &value : values) {
        proxy.value_.emplace_back(Convert(std::move(value)));
    }
    return proxy;
}

ValueProxy::Bucket ValueProxy::Convert(DataShareValuesBucket &&bucket)
{
    ValueProxy::Bucket proxy;
    for (auto &[key, value] : bucket.valuesMap) {
        proxy.value_.insert_or_assign(key, Convert(std::move(value)));
    }
    return proxy;
}

ValueProxy::Bucket ValueProxy::Convert(CommonType::VBucket &&bucket)
{
    ValueProxy::Bucket proxy;
    for (auto &[key, value] : bucket) {
        proxy.value_.insert_or_assign(key, Convert(std::move(value)));
    }
    return proxy;
}

ValueProxy::Buckets ValueProxy::Convert(std::vector<DataShareValuesBucket> &&buckets)
{
    ValueProxy::Buckets proxy;
    proxy.value_.reserve(buckets.size());
    for (auto &bucket : buckets) {
        proxy.value_.emplace_back(Convert(std::move(bucket)));
    }
    return proxy;
}

ValueProxy::Buckets ValueProxy::Convert(std::vector<CommonType::VBucket> &&buckets)
{
    ValueProxy::Buckets proxy;
    proxy.value_.reserve(buckets.size());
    for (auto &bucket : buckets) {
        proxy.value_.emplace_back(Convert(std::move(bucket)));
    }
    return proxy;
}

ValueProxy::Value& ValueProxy::Value::operator=(ValueProxy::Value&& value) noexcept
{
    if (this == &value) {
        return *this;
    }
    value_ = std::move(value.value_);
    return *this;
}

ValueProxy::Value::operator DataShareValueObject()
{
    DataShareValueObject object;
    Convert(std::move(value_), object.value);
    return object;
}

ValueProxy::Value::operator CommonType::Value()
{
    CommonType::Value object;
    Convert(std::move(value_), object);
    return object;
}

ValueProxy::Values& ValueProxy::Values::operator=(ValueProxy::Values&& values) noexcept
{
    if (this == &values) {
        return *this;
    }
    value_ = std::move(values.value_);
    return *this;
}

ValueProxy::Bucket& ValueProxy::Bucket::operator=(Bucket&& bucket) noexcept
{
    if (this == &bucket) {
        return *this;
    }
    value_ = std::move(bucket.value_);
    return *this;
}

ValueProxy::Bucket::operator DataShareValuesBucket()
{
    DataShareValuesBucket bucket;
    for (auto &[key, value] : value_) {
        bucket.valuesMap.insert_or_assign(key, std::move(value));
    }
    value_.clear();
    return bucket;
}

ValueProxy::Bucket::operator CommonType::VBucket()
{
    CommonType::VBucket bucket;
    for (auto &[key, value] : value_) {
        bucket.insert_or_assign(key, std::move(value));
    }
    value_.clear();
    return bucket;
}

ValueProxy::Buckets &ValueProxy::Buckets::operator=(Buckets &&buckets) noexcept
    {
    if (this == &buckets) {
        return *this;
    }
    value_ = std::move(buckets.value_);
    return *this;
    }
} // namespace OHOS::DistributedData