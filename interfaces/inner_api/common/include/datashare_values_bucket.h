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

#ifndef DATASHARE_VALUES_BUCKET_H
#define DATASHARE_VALUES_BUCKET_H

#include "datashare_value_object.h"

#include <map>
#include <set>

namespace OHOS {
namespace DataShare {
class DataShareValuesBucket {
public:
    DataShareValuesBucket() = default;
    explicit DataShareValuesBucket(std::map<std::string, DataShareValueObject> &values) : valuesMap(values){};
    ~DataShareValuesBucket() = default;

    void Put(const std::string &columnName, const DataShareValueObject &value = {})
    {
        valuesMap.insert(std::make_pair(columnName, value));
    }
    void PutString(const std::string &columnName, const std::string &value)
    {
        valuesMap.insert(std::make_pair(columnName, DataShareValueObject(value)));
    }
    void PutInt(const std::string &columnName, int value)
    {
        valuesMap.insert(std::make_pair(columnName, DataShareValueObject(value)));
    }
    void PutLong(const std::string &columnName, int64_t value)
    {
        valuesMap.insert(std::make_pair(columnName, DataShareValueObject(value)));
    }
    void PutDouble(const std::string &columnName, double value)
    {
        valuesMap.insert(std::make_pair(columnName, DataShareValueObject(value)));
    }
    void PutBool(const std::string &columnName, bool value)
    {
        valuesMap.insert(std::make_pair(columnName, DataShareValueObject(value)));
    }
    void PutBlob(const std::string &columnName, const std::vector<uint8_t> &value)
    {
        valuesMap.insert(std::make_pair(columnName, DataShareValueObject(value)));
    }
    void PutNull(const std::string &columnName)
    {
        valuesMap.insert(std::make_pair(columnName, DataShareValueObject()));
    }

    void Delete(const std::string &columnName)
    {
        valuesMap.erase(columnName);
    }

    void Clear()
    {
        valuesMap.clear();
    }

    int Size() const
    {
        return valuesMap.size();
    }

    bool IsEmpty() const
    {
        return valuesMap.empty();
    }

    bool GetObject(const std::string &columnName, DataShareValueObject &value) const
    {
        auto iter = valuesMap.find(columnName);
        if (iter == valuesMap.end()) {
            return false;
        }
        value = iter->second;
        return true;
    }

    std::map<std::string, DataShareValueObject> valuesMap;
};
} // namespace DataShare
} // namespace OHOS
#endif
