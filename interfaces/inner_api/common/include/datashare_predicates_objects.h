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

#ifndef DATASHARE_PREDICATES_OBJECTS_H
#define DATASHARE_PREDICATES_OBJECTS_H

#include <variant>
#include <string>
#include <vector>

namespace OHOS {
namespace DataShare {
enum class DataSharePredicatesObjectsType {
    TYPE_NULL = 0x06,
    TYPE_INT_VECTOR,
    TYPE_LONG_VECTOR,
    TYPE_DOUBLE_VECTOR,
    TYPE_STRING_VECTOR,
};

using ObjectsType = DataSharePredicatesObjectsType;
class DataSharePredicatesObjects {
public:
    DataSharePredicatesObjects() : type(ObjectsType::TYPE_NULL) {}
    ~DataSharePredicatesObjects() = default;
    DataSharePredicatesObjects(DataSharePredicatesObjects &&val) noexcept : type(val.type), value(std::move(val.value))
    {
        val.type = ObjectsType::TYPE_NULL;
    }
    DataSharePredicatesObjects(const DataSharePredicatesObjects &val) : type(val.type), value(val.value) {}
    DataSharePredicatesObjects &operator=(DataSharePredicatesObjects &&object) noexcept
    {
        if (this == &object) {
            return *this;
        }
        type = object.type;
        value = std::move(object.value);
        object.type = ObjectsType::TYPE_NULL;
        return *this;
    }
    DataSharePredicatesObjects &operator=(const DataSharePredicatesObjects &object)
    {
        if (this == &object) {
            return *this;
        }
        type = object.type;
        value = object.value;
        return *this;
    }
    DataSharePredicatesObjects(const std::vector<int> &val) : type(ObjectsType::TYPE_INT_VECTOR), value(val) {}
    DataSharePredicatesObjects(const std::vector<int64_t> &val) : type(ObjectsType::TYPE_LONG_VECTOR), value(val) {}
    DataSharePredicatesObjects(const std::vector<double> &val) : type(ObjectsType::TYPE_DOUBLE_VECTOR), value(val) {}
    DataSharePredicatesObjects(const std::vector<std::string> &val)
        : type(ObjectsType::TYPE_STRING_VECTOR), value(val) {}

    DataSharePredicatesObjectsType GetType() const
    {
        return type;
    }

    DataSharePredicatesObjectsType type;
    std::variant<std::monostate, std::vector<int>, std::vector<int64_t>,
        std::vector<std::string>, std::vector<double>> value;

    operator std::vector<int> () const
    {
        if (std::get_if<std::vector<int>>(&value) != nullptr) {
            return std::get<std::vector<int>>(value);
        } else {
            return {};
        }
    }
    operator std::vector<int64_t> () const
    {
        if (std::get_if<std::vector<int64_t>>(&value) != nullptr) {
            return std::get<std::vector<int64_t>>(value);
        } else {
            return {};
        }
    }
    operator std::vector<double> () const
    {
        if (std::get_if<std::vector<double>>(&value) != nullptr) {
            return std::get<std::vector<double>>(value);
        } else {
            return {};
        }
    }
    operator std::vector<std::string> () const
    {
        if (std::get_if<std::vector<std::string>>(&value) != nullptr) {
            return std::get<std::vector<std::string>>(value);
        } else {
            return {};
        }
    }
};
} // namespace DataShare
} // namespace OHOS
#endif
