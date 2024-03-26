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
using Values = std::vector<Value>;
using VBucket = std::map<std::string, Value>;
using VBuckets = std::vector<VBucket>;
}

namespace OHOS::DataShare {
class ValueProxy final {
public:
    template<typename T>
    static inline constexpr size_t TYPE_INDEX = Traits::variant_index_of_v<T, CommonType::Value>;
    static inline constexpr size_t TYPE_MAX = Traits::variant_size_of_v<CommonType::Value>;

    template<class T>
        static inline constexpr uint32_t INDEX = TYPE_INDEX<T>;
    static inline constexpr uint32_t MAX = TYPE_MAX;

    template<typename T, typename... Types>
    struct variant_cvt_of {
        static constexpr size_t value = std::is_class_v<T> ? Traits::convertible_index_of_v<T, Types...>
        : Traits::same_index_of_v<T, Types...>;
    };

    template<typename T, typename... Types>
    static variant_cvt_of<T, Types...> variant_cvt_test(const T &, const std::variant<Types...> &);

    template<class T, class V>
        static inline constexpr uint32_t CVT_INDEX =
            decltype(variant_cvt_test(std::declval<T>(), std::declval<V>()))::value;

    using Proxy = std::variant<std::monostate, int64_t, double, std::string, bool, std::vector<uint8_t>>;

    class Value {
    public:
        Value() = default;
        Value(Value &&value) noexcept
        {
            *this = std::move(value);
        };
        Value &operator=(Value &&value) noexcept;
        operator DataShareValueObject();
        operator CommonType::Value();

        template<typename T>
        operator T() noexcept
        {
            auto val = Traits::get_if<T>(&value_);
            if (val != nullptr) {
                return T(std::move(*val));
            }
            return T();
        };

    private:
        friend ValueProxy;
        Proxy value_;
    };
    class Values {
    public:
        Values() = default;
        Values(Values &&values) noexcept
        {
            *this = std::move(values);
        };
        Values &operator=(Values &&values) noexcept;
        template<typename T>
        operator std::vector<T>()
        {
            std::vector<T> objects;
            objects.reserve(value_.size());
            for (auto &proxy : value_) {
                objects.emplace_back(std::move(proxy));
            }
            value_.clear();
            return objects;
        }

    private:
        friend ValueProxy;
        std::vector<Value> value_;
    };
    class Bucket {
    public:
        Bucket() = default;
        Bucket(Bucket &&bucket) noexcept
        {
            *this = std::move(bucket);
        };
        Bucket &operator=(Bucket &&bucket) noexcept;
        template<typename T>
        operator std::map<std::string, T>()
        {
            std::map<std::string, T> bucket;
            for (auto &[key, value] : value_) {
                bucket.insert_or_assign(key, std::move(static_cast<T>(value)));
            }
            value_.clear();
            return bucket;
        }
        operator DataShareValuesBucket();
        operator CommonType::VBucket();

    private:
        friend ValueProxy;
        std::map<std::string, Value> value_;
    };
    class Buckets {
    public:
        Buckets() = default;
        Buckets(Buckets &&buckets) noexcept
        {
            *this = std::move(buckets);
        };
        Buckets &operator=(Buckets &&buckets) noexcept;
        template<typename T>
        operator std::vector<T>()
        {
            std::vector<T> buckets;
            buckets.reserve(value_.size());
            for (auto &bucket : value_) {
                buckets.emplace_back(std::move(bucket));
            }
            value_.clear();
            return buckets;
        }

    private:
        friend ValueProxy;
        std::vector<Bucket> value_;
    };

    static Value Convert(CommonType::Value &&value);
    static Value Convert(DataShareValueObject &&value);
    static Values Convert(std::vector<CommonType::Value> &&values);
    static Values Convert(std::vector<DataShareValueObject> &&values);
    static Bucket Convert(DataShareValuesBucket &&bucket);
    static Bucket Convert(CommonType::VBucket &&bucket);
    static Buckets Convert(std::vector<DataShareValuesBucket> &&buckets);
    static Buckets Convert(std::vector<CommonType::VBucket> &&buckets);

    template<typename T>
    static std::enable_if_t < CVT_INDEX<T, Proxy><MAX, Bucket>
    Convert(const std::map<std::string, T> &values)
    {
        Bucket bucket;
        for (auto &[key, value] : values) {
            bucket.value_[key].value_ = static_cast<std::variant_alternative_t<CVT_INDEX<T, Proxy>, Proxy>>(value);
        }
        return bucket;
    }

    template<typename T>
    static std::enable_if_t < CVT_INDEX<T, Proxy><MAX, Values>
    Convert(const std::vector<T> &values)
    {
        Values proxy;
        proxy.value_.resize(values.size());
        for (size_t i = 0; i < values.size(); i++) {
            proxy.value_[i].value_ = static_cast<std::variant_alternative_t<CVT_INDEX<T, Proxy>, Proxy>>(values[i]);
        }
        return proxy;
    }

    template<typename T, typename O>
    static bool GetItem(T &&input, O &output)
    {
        return false;
    }

    template<typename T, typename O, typename First, typename... Rest>
    static bool GetItem(T &&input, O &output)
    {
        auto val = Traits::get_if<First>(&input);
        if (val != nullptr) {
            output = std::move(*val);
            return true;
        }
        return GetItem<T, O, Rest...>(std::move(input), output);
    }

    template<typename T, typename... Types>
    static bool Convert(T &&input, std::variant<Types...> &output)
    {
        return GetItem<T, decltype(output), Types...>(std::move(input), output);
    }

private:
    ValueProxy() = delete;
    ~ValueProxy() = delete;
};
} // namespace OHOS::DataShare
#endif // DATASHARE_VALUEBUCKET_CONVERT_H