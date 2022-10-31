/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef DATASHARE_PREDICATES_OBJECT_H
#define DATASHARE_PREDICATES_OBJECT_H

#include <variant>
#include <string>
#include <vector>

namespace OHOS {
namespace DataShare {
enum class DataSharePredicatesObjectType {
    TYPE_NULL = 0x00,
    TYPE_INT,
    TYPE_DOUBLE,
    TYPE_STRING,
    TYPE_BOOL,
    TYPE_LONG,
};

using ObjectType = DataSharePredicatesObjectType;

class SingleValue {
public:
    using Type = std::variant<std::monostate, int, int64_t, double, std::string, bool>;
    Type value;
    SingleValue() = default;
    ~SingleValue() = default;
    SingleValue(Type val) noexcept : value(std::move(val))
    {
    }
    SingleValue(SingleValue &&val) noexcept :value(std::move(val.value))
    {
    }
    SingleValue(const SingleValue &val) : value(val.value) {}
    SingleValue &operator=(SingleValue &&object) noexcept
    {
        if (this == &object) {
            return *this;
        }
        value = std::move(object.value);
        return *this;
    }

    SingleValue &operator=(const SingleValue &object)
    {
        if (this == &object) {
            return *this;
        }
        value = object.value;
        return *this;
    }

    SingleValue(int val) : value(val) {}
    SingleValue(int64_t val) : value(val) {}
    SingleValue(double val) : value(val) {}
    SingleValue(bool val) : value(val) {}
    SingleValue(const char *val) : value(std::string(val)) {}
    SingleValue(std::string val) : value(std::move(val)) {}
    operator int () const
    {
        if (std::get_if<int>(&value) != nullptr) {
            return std::get<int>(value);
        } else {
            return {};
        }
    }
    operator int64_t () const
    {
        if (std::get_if<int64_t>(&value) != nullptr) {
            return std::get<int64_t>(value);
        } else {
            return {};
        }
    }
    operator double () const
    {
        if (std::get_if<double>(&value) != nullptr) {
            return std::get<double>(value);
        } else {
            return {};
        }
    }
    operator bool () const
    {
        if (std::get_if<bool>(&value) != nullptr) {
            return std::get<bool>(value);
        } else {
            return {};
        }
    }
    operator std::string () const
    {
        if (std::get_if<std::string>(&value) != nullptr) {
            return std::get<std::string>(value);
        } else {
            return {};
        }
    }
};
} // namespace DataShare
} // namespace OHOS
#endif
