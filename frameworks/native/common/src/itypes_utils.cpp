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

#include "itypes_utils.h"

#include "datashare_log.h"
namespace OHOS::DataShare {
bool ITypesUtils::Marshal(Parcel &data)
{
    return true;
}

bool ITypesUtils::Unmarshal(Parcel &data)
{
    return true;
}

bool ITypesUtils::Marshalling(bool input, Parcel &data)
{
    return data.WriteBool(input);
}

bool ITypesUtils::Unmarshalling(Parcel &data, bool &output)
{
    return data.ReadBool(output);
}

bool ITypesUtils::Marshalling(const char *input, Parcel &data)
{
    return data.WriteString(input);
}

bool ITypesUtils::Marshalling(const std::string &input, Parcel &data)
{
    return data.WriteString(input);
}

bool ITypesUtils::Unmarshalling(Parcel &data, std::string &output)
{
    return data.ReadString(output);
}

bool ITypesUtils::Marshalling(int16_t input, Parcel &data)
{
    return data.WriteInt16(input);
}

bool ITypesUtils::Unmarshalling(Parcel &data, int16_t &output)
{
    return data.ReadInt16(output);
}

bool ITypesUtils::Marshalling(int32_t input, Parcel &data)
{
    return data.WriteInt32(input);
}

bool ITypesUtils::Unmarshalling(Parcel &data, int32_t &output)
{
    return data.ReadInt32(output);
}

bool ITypesUtils::Marshalling(int64_t input, Parcel &data)
{
    return data.WriteInt64(input);
}
bool ITypesUtils::Unmarshalling(Parcel &data, int64_t &output)
{
    return data.ReadInt64(output);
}

bool ITypesUtils::Marshalling(double input, Parcel &data)
{
    return data.WriteDouble(input);
}

bool ITypesUtils::Unmarshalling(Parcel &data, double &output)
{
    return data.ReadDouble(output);
}

bool ITypesUtils::Marshalling(const std::monostate &input, Parcel &data)
{
    return true;
}

bool ITypesUtils::Unmarshalling(Parcel &data, std::monostate &output)
{
    return true;
}

bool ITypesUtils::Marshalling(const std::vector<uint8_t> &input, Parcel &data)
{
    return data.WriteUInt8Vector(input);
}

bool ITypesUtils::Unmarshalling(Parcel &data, std::vector<uint8_t> &output)
{
    return data.ReadUInt8Vector(&output);
}

bool ITypesUtils::Marshalling(const DataSharePredicates &predicates, Parcel &parcel)
{
    LOG_DEBUG("Marshalling DataSharePredicates Start");
    const auto &operations = predicates.GetOperationList();
    int64_t mode = static_cast<int64_t>(predicates.GetSettingMode());
    return ITypesUtils::Marshal(parcel, operations, predicates.GetWhereClause(), predicates.GetWhereArgs(),
        predicates.GetOrder(), mode);
}

bool ITypesUtils::Unmarshalling(Parcel &parcel, DataSharePredicates &predicates)
{
    LOG_DEBUG("Unmarshalling DataSharePredicates Start");
    std::vector<OperationItem> operations{};
    std::string whereClause = "";
    std::vector<std::string> whereArgs;
    std::string order = "";
    int64_t mode = INVALID_MODE;
    if (!ITypesUtils::Unmarshal(parcel, operations, whereClause, whereArgs, order, mode)) {
        LOG_ERROR("read predicate failed");
        return false;
    }
    DataSharePredicates tmpPredicates(std::move(operations));
    tmpPredicates.SetWhereClause(whereClause);
    tmpPredicates.SetWhereArgs(whereArgs);
    tmpPredicates.SetOrder(order);
    tmpPredicates.SetSettingMode(static_cast<SettingMode>(mode));
    predicates = tmpPredicates;
    return true;
}

bool ITypesUtils::Marshalling(const DataShareValuesBucket &valuesBucket, Parcel &parcel)
{
    return ITypesUtils::Marshal(parcel, valuesBucket.valuesMap);
}

bool ITypesUtils::Unmarshalling(Parcel &parcel, DataShareValuesBucket &valuesBucket)
{
    return ITypesUtils::Unmarshal(parcel, valuesBucket.valuesMap);
}

bool ITypesUtils::Marshalling(const OperationItem &operationItem, Parcel &parcel)
{
    return ITypesUtils::Marshal(parcel, operationItem.operation, operationItem.singleParams, operationItem.multiParams);
}

bool ITypesUtils::Unmarshalling(Parcel &parcel, OperationItem &operationItem)
{
    return ITypesUtils::Unmarshal(parcel, operationItem.operation, operationItem.singleParams,
        operationItem.multiParams);
}

bool ITypesUtils::Marshalling(const DataShareValueObject &valueObject, Parcel &parcel)
{
    return ITypesUtils::Marshal(parcel, valueObject.value);
}

bool ITypesUtils::Unmarshalling(Parcel &parcel, DataShareValueObject &valueObject)
{
    return ITypesUtils::Unmarshal(parcel, valueObject.value);
}
} // namespace OHOS::DataShare
