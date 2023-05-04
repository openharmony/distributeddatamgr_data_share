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
    const auto &operations = predicates.GetOperationList();
    int16_t mode = predicates.GetSettingMode();
    return ITypesUtils::Marshal(parcel, operations, predicates.GetWhereClause(), predicates.GetWhereArgs(),
        predicates.GetOrder(), mode);
}

bool ITypesUtils::Unmarshalling(Parcel &parcel, DataSharePredicates &predicates)
{
    std::vector<OperationItem> operations{};
    std::string whereClause = "";
    std::vector<std::string> whereArgs;
    std::string order = "";
    int16_t mode = INVALID_MODE;
    if (!ITypesUtils::Unmarshal(parcel, operations, whereClause, whereArgs, order, mode)) {
        LOG_ERROR("read predicate failed");
        return false;
    }
    DataSharePredicates tmpPredicates(std::move(operations));
    tmpPredicates.SetWhereClause(whereClause);
    tmpPredicates.SetWhereArgs(whereArgs);
    tmpPredicates.SetOrder(order);
    tmpPredicates.SetSettingMode(mode);
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

bool ITypesUtils::Marshalling(const PredicateTemplateNode &node, Parcel &parcel)
{
    return ITypesUtils::Marshal(parcel, node.key_, node.selectSql_);
}

bool ITypesUtils::Unmarshalling(Parcel &parcel, PredicateTemplateNode &node)
{
    return ITypesUtils::Unmarshal(parcel, node.key_, node.selectSql_);
}

bool ITypesUtils::Marshalling(const Template &templat, Parcel &parcel)
{
    return ITypesUtils::Marshal(parcel, templat.predicates_, templat.scheduler_);
}

bool ITypesUtils::Unmarshalling(Parcel &parcel, Template &templat)
{
    return ITypesUtils::Unmarshal(parcel, templat.predicates_, templat.scheduler_);
}

bool ITypesUtils::Marshalling(const Data &data, MessageParcel &parcel)
{
    if (!parcel.WriteInt32(data.datas_.size())) {
        return false;
    }
    for (const auto & dataItem : data.datas_) {
        if (!ITypesUtils::Marshalling(dataItem, parcel)) {
            return false;
        }
    }
    return parcel.WriteInt32(data.version_);
}

bool ITypesUtils::Marshalling(const PublishedDataItem &dataItem, MessageParcel &parcel)
{
    if (!parcel.WriteString(dataItem.key_)) {
        return false;
    }
    if (!parcel.WriteInt64(dataItem.subscriberId_)) {
        return false;
    }
    auto index = static_cast<uint32_t>(dataItem.value_.index());
    if (!parcel.WriteUint32(index)) {
        return false;
    }
    if (index == 0) {
        return parcel.WriteAshmem(std::get<sptr<Ashmem>>(dataItem.value_));
    }
    return parcel.WriteString(std::get<std::string>(dataItem.value_));
}

bool ITypesUtils::Unmarshalling(MessageParcel &parcel, PublishedDataItem &dataItem)
{
    std::string key = parcel.ReadString();
    auto subscriberId = parcel.ReadInt64();
    auto index = parcel.ReadUint32();
    std::variant<sptr<Ashmem>, std::string> value;
    if (index == 0) {
        value = parcel.ReadAshmem();
    } else {
        value = parcel.ReadString();
    }
    dataItem.key_ = key;
    dataItem.subscriberId_ = subscriberId;
    dataItem.value_ = value;
    return true;
}

bool ITypesUtils::Unmarshalling(MessageParcel &parcel, std::vector<PublishedDataItem> &publishedDataItems)
{
    int32_t len = parcel.ReadInt32();
    if (len < 0) {
        return false;
    }
    size_t size = static_cast<size_t>(len);
    size_t readAbleSize = parcel.GetReadableBytes();
    if ((size > readAbleSize) || (size > publishedDataItems.max_size())) {
        return false;
    }

    for (size_t i = 0; i < size; i++) {
        PublishedDataItem value;
        if (!ITypesUtils::Unmarshalling(parcel, value)) {
            return false;
        }
        publishedDataItems.emplace_back(std::move(value));
    }
    return true;
}

bool ITypesUtils::Unmarshalling(Parcel &parcel, RdbChangeNode &changeNode)
{
    return ITypesUtils::Unmarshal(parcel, changeNode.uri_, changeNode.templateId_.subscriberId_,
                                  changeNode.templateId_.bundleName_, changeNode.data_);
}

bool ITypesUtils::Unmarshalling(MessageParcel &parcel, PublishedDataChangeNode &changeNode)
{
    auto bundleName = parcel.ReadString();
    if (!Unmarshalling(parcel, changeNode.datas_ )) {
        return false;
    }
    changeNode.ownerBundleName_ = bundleName;
    return true;
}

bool ITypesUtils::Unmarshalling(Parcel &parcel, OperationResult &result)
{
    return ITypesUtils::Unmarshal(parcel, result.key_, result.errCode_);
}
} // namespace OHOS::DataShare
