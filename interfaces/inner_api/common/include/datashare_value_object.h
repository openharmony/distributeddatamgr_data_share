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

#ifndef DATASHARE_VALUE_OBJECT_H
#define DATASHARE_VALUE_OBJECT_H

#include <variant>
#include <string>
#include <vector>
namespace OHOS {
namespace DataShare {
constexpr int INVALID_TYPE = -1;
constexpr int DATA_SHARE_NO_ERROR = 0;
enum DataShareValueObjectType : int32_t {
    TYPE_NULL = 0,
    TYPE_INT,
    TYPE_INT64,
    TYPE_DOUBLE,
    TYPE_STRING,
    TYPE_BOOL,
    TYPE_BLOB,
};

class DataShareValueObject {
public:
    DataShareValueObject() : type(TYPE_NULL) {};
    ~DataShareValueObject() = default;
    DataShareValueObject(DataShareValueObject &&object) noexcept : type(object.type), value(std::move(object.value))
    {
        object.type = DataShareValueObjectType::TYPE_NULL;
    };
    DataShareValueObject(const DataShareValueObject &object) : type(object.type), value(object.value) {};
    DataShareValueObject(int val) : type(TYPE_INT), value(static_cast<int64_t>(val)) {};
    DataShareValueObject(int64_t val) : type(TYPE_INT64), value(val) {};
    DataShareValueObject(double val) : type(TYPE_DOUBLE), value(val) {};
    DataShareValueObject(bool val) : type(TYPE_BOOL), value(val) {};
    DataShareValueObject(std::string val) : type(TYPE_STRING), value(std::move(val)) {};
    DataShareValueObject(const char *val) : DataShareValueObject(std::string(val)) {};
    DataShareValueObject(std::vector<uint8_t> blob) : type(TYPE_BLOB), value(std::move(blob)) {};
    DataShareValueObject &operator=(DataShareValueObject &&object) noexcept
    {
        if (this == &object) {
            return *this;
        }
        type = object.type;
        value = std::move(object.value);
        object.type = TYPE_NULL;
        return *this;
    };
    DataShareValueObject &operator=(const DataShareValueObject &object)
    {
        if (this == &object) {
            return *this;
        }
        type = object.type;
        value = object.value;
        return *this;
    }

    DataShareValueObjectType GetType() const
    {
        return type;
    }

    operator int () const
    {
        if (std::get_if<int64_t>(&value) != nullptr) {
            return static_cast<int>(std::get<int64_t>(value));
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
    operator std::vector<uint8_t> () const
    {
        if (std::get_if<std::vector<uint8_t>>(&value) != nullptr) {
            return std::get<std::vector<uint8_t>>(value);
        } else {
            return {};
        }
    }
    DataShareValueObjectType type;
    std::variant<std::monostate, int, int64_t, double, std::string, bool, std::vector<uint8_t>> value;
};
} // namespace DataShare
} // namespace OHOS
#endif
