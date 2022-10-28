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

#ifndef DATASHARE_COMMON_ITYPES_UTIL_H
#define DATASHARE_COMMON_ITYPES_UTIL_H

#include "parcel.h"
#include "datashare_predicates.h"
#include "datashare_values_bucket.h"


namespace OHOS::DataShare {
class ITypesUtils final {
public:
    static bool Marshal(Parcel &data);
    static bool Unmarshal(Parcel &data);

    static bool Marshalling(const DataSharePredicates &predicates, Parcel &parcel);
    static bool Unmarshalling(Parcel &parcel, DataSharePredicates &predicates);

    static bool Marshalling(const DataShareValuesBucket &valuesBucket, Parcel &parcel);
    static bool Unmarshalling(Parcel &parcel, DataShareValuesBucket &valuesBucket);

    static bool Marshalling(const OperationItem &operationItem, Parcel &parcel);
    static bool Unmarshalling(Parcel &parcel, OperationItem &operationItem);

    static bool Marshalling(const DataSharePredicatesObject &predicatesObject, Parcel &parcel);
    static bool Unmarshalling(Parcel &parcel, DataSharePredicatesObject &predicatesObject);

    static bool Marshalling(const DataSharePredicatesObjects &predicatesObject, Parcel &parcel);
    static bool Unmarshalling(Parcel &parcel, DataSharePredicatesObjects &predicatesObject);

    static bool Marshalling(const DataShareValueObject &valueObject, Parcel &parcel);
    static bool Unmarshalling(Parcel &parcel, DataShareValueObject &valueObject);

    static bool Marshalling(const std::string &input, Parcel &data);
    static bool Unmarshalling(Parcel &data, std::string &output);

    template <typename T>
    static bool Marshalling(const std::vector<T> &params, Parcel &parcel);
    template <typename T>
    static bool Unmarshalling(Parcel &parcel, std::vector<T> &params);

    template<typename T, typename... Types>
    static bool Marshal(Parcel &parcel, const T &first, const Types &...others);
    template<typename T, typename... Types>
    static bool Unmarshal(Parcel &parcel, T &first, Types &...others);
};
template<typename T, typename... Types>
bool ITypesUtils::Marshal(Parcel &parcel, const T &first, const Types &...others)
{
    if (!Marshalling(first, parcel)) {
        return false;
    }
    return Marshal(parcel, others...);
}

template<typename T, typename... Types>
bool ITypesUtils::Unmarshal(Parcel &parcel, T &first, Types &...others)
{
    if (!Unmarshalling(parcel, first)) {
        return false;
    }
    return Unmarshal(parcel, others...);
}

template <typename T>
bool ITypesUtils::Marshalling(const std::vector<T> &params, Parcel &parcel)
{
    if (!parcel.WriteInt32(params.size())) {
        return false;
    }
    for (unsigned long i = 0; i < params.size(); i++) {
        if (!Marshalling(params[i], parcel)) {
            return false;
        }
    }
    return true;
}

template <typename T>
bool ITypesUtils::Unmarshalling(Parcel &parcel, std::vector<T> &params)
{
    size_t size = static_cast<size_t>(parcel.ReadInt32());
    if (static_cast<int32_t>(size) < 0) {
        return false;
    }
    if ((size > parcel.GetReadableBytes()) || (params.max_size() < size)) {
        return false;
    }
    params.resize(static_cast<int32_t>(size));
    for (size_t i = 0; i < size; i++) {
        T param;
        if (!Unmarshalling(parcel, param)) {
            return false;
        }
        params[static_cast<int32_t>(i)] = param;
    }
    return true;
}
} // namespace OHOS::DataShare
#endif