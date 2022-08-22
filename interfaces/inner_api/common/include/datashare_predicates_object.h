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
class DataSharePredicatesObject {
public:
    DataSharePredicatesObject() : type(ObjectType::TYPE_NULL) {}
    ~DataSharePredicatesObject() = default;
    DataSharePredicatesObject(DataSharePredicatesObject &&val) noexcept : type(val.type), value(std::move(val.value))
    {
        val.type = ObjectType::TYPE_NULL;
    }
    DataSharePredicatesObject(const DataSharePredicatesObject &val) : type(val.type), value(val.value) {}
    DataSharePredicatesObject &operator=(DataSharePredicatesObject &&object) noexcept
    {
        if (this == &object) {
            return *this;
        }
        type = object.type;
        value = std::move(object.value);
        object.type = ObjectType::TYPE_NULL;
        return *this;
    }
    DataSharePredicatesObject &operator=(const DataSharePredicatesObject &object)
    {
        if (this == &object) {
            return *this;
        }
        type = object.type;
        value = object.value;
        return *this;
    }
    DataSharePredicatesObject(int val) : type(ObjectType::TYPE_INT), value(val) {}
    DataSharePredicatesObject(int64_t val) : type(ObjectType::TYPE_LONG), value(val) {}
    DataSharePredicatesObject(double val) : type(ObjectType::TYPE_DOUBLE), value(val) {}
    DataSharePredicatesObject(bool val) : type(ObjectType::TYPE_BOOL), value(val) {}
    DataSharePredicatesObject(const char *val) : type(ObjectType::TYPE_STRING), value(std::string(val)) {}
    DataSharePredicatesObject(std::string val) : type(ObjectType::TYPE_STRING), value(std::move(val)) {}

    DataSharePredicatesObjectType GetType() const
    {
        return type;
    }

    DataSharePredicatesObjectType type;
    std::variant<std::monostate, int, int64_t, double, std::string, bool> value;

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
