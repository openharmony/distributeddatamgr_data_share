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

#include "datashare_predicates.h"
#include "datashare_values_bucket.h"
#include "parcel.h"
namespace OHOS::DataShare {
class ITypesUtils final {
public:
    static bool Marshal(Parcel &data);
    static bool Unmarshal(Parcel &data);

    static bool Marshalling(bool input, Parcel &data);
    static bool Unmarshalling(Parcel &data, bool &output);

    static bool Marshalling(const char *input, Parcel &data);
    static bool Marshalling(const std::string &input, Parcel &data);
    static bool Unmarshalling(Parcel &data, std::string &output);

    static bool Marshalling(int16_t input, Parcel &data);
    static bool Unmarshalling(Parcel &data, int16_t &output);

    static bool Marshalling(int32_t input, Parcel &data);
    static bool Unmarshalling(Parcel &data, int32_t &output);

    static bool Marshalling(int64_t input, Parcel &data);
    static bool Unmarshalling(Parcel &data, int64_t &output);

    static bool Marshalling(double input, Parcel &data);
    static bool Unmarshalling(Parcel &data, double &output);

    static bool Marshalling(const std::monostate &input, Parcel &data);
    static bool Unmarshalling(Parcel &data, std::monostate &output);

    static bool Marshalling(const std::vector<uint8_t> &input, Parcel &data);
    static bool Unmarshalling(Parcel &data, std::vector<uint8_t> &output);

    static bool Marshalling(const DataSharePredicates &predicates, Parcel &parcel);
    static bool Unmarshalling(Parcel &parcel, DataSharePredicates &predicates);

    static bool Marshalling(const DataShareValuesBucket &valuesBucket, Parcel &parcel);
    static bool Unmarshalling(Parcel &parcel, DataShareValuesBucket &valuesBucket);

    static bool Marshalling(const OperationItem &operationItem, Parcel &parcel);
    static bool Unmarshalling(Parcel &parcel, OperationItem &operationItem);

    static bool Marshalling(const SingleValue &predicatesObject, Parcel &parcel);
    static bool Unmarshalling(Parcel &parcel, SingleValue &predicatesObject);

    static bool Marshalling(const MutliValue &predicatesObject, Parcel &parcel);
    static bool Unmarshalling(Parcel &parcel, MutliValue &predicatesObject);

    static bool Marshalling(const DataShareValueObject &valueObject, Parcel &parcel);
    static bool Unmarshalling(Parcel &parcel, DataShareValueObject &valueObject);

    template<typename T>
    static bool Marshalling(const std::vector<T> &params, Parcel &parcel);
    template<typename T>
    static bool Unmarshalling(Parcel &parcel, std::vector<T> &params);

    template<typename T>
    static bool Marshalling(const std::list<T> &params, Parcel &parcel);
    template<typename T>
    static bool Unmarshalling(Parcel &parcel, std::list<T> &params);

    template<class K, class V>
    static bool Marshalling(const std::map<K, V> &val, Parcel &parcel);
    template<class K, class V>
    static bool Unmarshalling(Parcel &parcel, std::map<K, V> &val);

    template<typename... _Types>
    static bool Marshalling(const std::variant<_Types...> &input, Parcel &data);

    template<typename... _Types>
    static bool Unmarshalling(Parcel &data, std::variant<_Types...> &output);

    template<typename T, typename... Types>
    static bool Marshal(Parcel &parcel, const T &first, const Types &...others);
    template<typename T, typename... Types>
    static bool Unmarshal(Parcel &parcel, T &first, Types &...others);

private:
    template<typename T>
    static bool Marshalling(T *input, Parcel &data) = delete;
    template<typename T>
    static bool Unmarshalling(T *&output, Parcel &data) = delete;
    template<typename _OutTp>
    static bool ReadVariant(uint32_t step, uint32_t index, const _OutTp &output, Parcel &data)
    {
        return false;
    }

    template<typename _OutTp, typename _First, typename... _Rest>
    static bool ReadVariant(uint32_t step, uint32_t index, const _OutTp &output, Parcel &data)
    {
        if (step == index) {
            _First value{};
            auto success = ITypesUtils::Unmarshalling(data, value);
            output = value;
            return success;
        }
        return ReadVariant<_OutTp, _Rest...>(step + 1, index, output, data);
    }

    template<typename _InTp>
    static bool WriteVariant(uint32_t step, const _InTp &input, Parcel &data)
    {
        return false;
    }

    template<typename _InTp, typename _First, typename... _Rest>
    static bool WriteVariant(uint32_t step, const _InTp &input, Parcel &data)
    {
        if (step == input.index()) {
            return ITypesUtils::Marshalling(std::get<_First>(input), data);
        }
        return WriteVariant<_InTp, _Rest...>(step + 1, input, data);
    }
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

template<typename T>
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

template<typename T>
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

template<typename T>
bool ITypesUtils::Marshalling(const std::list<T> &params, Parcel &parcel)
{
    if (!parcel.WriteInt32(params.size())) {
        return false;
    }
    for (auto &item : params) {
        if (!Marshalling(item, parcel)) {
            return false;
        }
    }
    return true;
}

template<typename T>
bool ITypesUtils::Unmarshalling(Parcel &parcel, std::list<T> &params)
{
    size_t size = static_cast<size_t>(parcel.ReadInt32());
    if (static_cast<int32_t>(size) < 0) {
        return false;
    }
    if ((size > parcel.GetReadableBytes()) || (params.max_size() < size)) {
        return false;
    }
    for (size_t i = 0; i < size; i++) {
        T param;
        if (!Unmarshalling(parcel, param)) {
            return false;
        }
        params.push_back(std::move(param));
    }
    return true;
}
template<class K, class V>
bool ITypesUtils::Marshalling(const std::map<K, V> &result, Parcel &parcel)
{
    if (!parcel.WriteInt32(static_cast<int32_t>(result.size()))) {
        return false;
    }
    for (const auto &entry : result) {
        if (!Marshalling(entry.first, parcel)) {
            return false;
        }
        if (!Marshalling(entry.second, parcel)) {
            return false;
        }
    }
    return true;
}

template<class K, class V>
bool ITypesUtils::Unmarshalling(Parcel &parcel, std::map<K, V> &val)
{
    int32_t size = 0;
    if (!parcel.ReadInt32(size)) {
        return false;
    }
    if (size < 0) {
        return false;
    }

    size_t readAbleSize = parcel.GetReadableBytes();
    size_t len = static_cast<size_t>(size);
    if ((len > readAbleSize) || len > val.max_size()) {
        return false;
    }

    for (int32_t i = 0; i < size; i++) {
        K key;
        if (!Unmarshalling(parcel, key)) {
            return false;
        }
        V value;
        if (!Unmarshalling(parcel, value)) {
            return false;
        }
        val.insert({ std::move(key), std::move(value) });
    }
    return true;
}
template<typename... _Types>
bool ITypesUtils::Marshalling(const std::variant<_Types...> &input, Parcel &data)
{
    uint32_t index = static_cast<uint32_t>(input.index());
    if (!data.WriteUint32(index)) {
        return false;
    }

    return WriteVariant<decltype(input), _Types...>(0, input, data);
}

template<typename... _Types>
bool ITypesUtils::Unmarshalling(Parcel &data, std::variant<_Types...> &output)
{
    uint32_t index = data.ReadUint32();
    if (index >= sizeof...(_Types)) {
        return false;
    }

    return ReadVariant<decltype(output), _Types...>(0, index, output, data);
}
} // namespace OHOS::DataShare
#endif
