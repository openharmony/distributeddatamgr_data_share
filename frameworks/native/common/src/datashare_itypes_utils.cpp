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

#include "datashare_itypes_utils.h"

#include <cstdint>
#include <iomanip>
#include <sstream>
#include "datashare_log.h"

namespace OHOS::ITypesUtil {
using namespace OHOS::DataShare;

// Maximum value of IPC shared memory
static const size_t MAX_IPC_SIZE = 128 * 1024 * 1024;

template<>
bool Marshalling(const Predicates &predicates, MessageParcel &parcel)
{
    const auto &operations = predicates.GetOperationList();
    int16_t mode = predicates.GetSettingMode();
    return ITypesUtil::Marshal(parcel, operations, predicates.GetWhereClause(), predicates.GetWhereArgs(),
                               predicates.GetOrder(), mode);
}

template<>
bool Unmarshalling(Predicates &predicates, MessageParcel &parcel)
{
    std::vector<Operation> operations{};
    std::string whereClause = "";
    std::vector<std::string> whereArgs;
    std::string order = "";
    int16_t mode = DataShare::INVALID_MODE;
    if (!ITypesUtil::Unmarshal(parcel, operations, whereClause, whereArgs, order, mode)) {
        return false;
    }
    DataShare::DataSharePredicates tmpPredicates(operations);
    tmpPredicates.SetWhereClause(whereClause);
    tmpPredicates.SetWhereArgs(whereArgs);
    tmpPredicates.SetOrder(order);
    tmpPredicates.SetSettingMode(mode);
    predicates = tmpPredicates;
    return true;
}

template<>
bool Marshalling(const BatchUpdateResult &result, MessageParcel &parcel)
{
    return ITypesUtil::Marshal(parcel, result.uri, result.codes);
}

template<>
bool Unmarshalling(BatchUpdateResult &result, MessageParcel &parcel)
{
    return ITypesUtil::Unmarshal(parcel, result.uri, result.codes);
}

template<>
bool Marshalling(const UpdateOperation &operation, MessageParcel &parcel)
{
    return ITypesUtil::Marshal(parcel, operation.valuesBucket, operation.predicates);
}

template<>
bool Unmarshalling(UpdateOperation &operation, MessageParcel &parcel)
{
    return ITypesUtil::Unmarshal(parcel, operation.valuesBucket, operation.predicates);
}

template<>
bool Marshalling(const Operation &operation, MessageParcel &parcel)
{
    return ITypesUtil::Marshal(parcel, operation.operation, operation.singleParams, operation.multiParams);
}

template<>
bool Unmarshalling(Operation &operation, MessageParcel &parcel)
{
    return ITypesUtil::Unmarshal(parcel, operation.operation, operation.singleParams, operation.multiParams);
}

template<>
bool Unmarshalling(PublishedDataItem &dataItem, MessageParcel &parcel)
{
    return ITypesUtil::Unmarshal(parcel, dataItem.key_, dataItem.subscriberId_, dataItem.value_);
}

template<>
bool Marshalling(const PublishedDataItem &dataItem, MessageParcel &parcel)
{
    return ITypesUtil::Marshal(parcel, dataItem.key_, dataItem.subscriberId_, dataItem.value_);
}

template<>
bool Marshalling(const Data &data, MessageParcel &parcel)
{
    return ITypesUtil::Marshal(parcel, data.datas_, data.version_);
}

template<>
bool Unmarshalling(Data &data, MessageParcel &parcel)
{
    return ITypesUtil::Unmarshal(parcel, data.datas_, data.version_);
}

template<>
bool Unmarshalling(TemplateId &templateId, MessageParcel &parcel)
{
    return ITypesUtil::Unmarshal(parcel, templateId.subscriberId_, templateId.bundleName_);
}

template<>
bool Marshalling(const TemplateId &templateId, MessageParcel &parcel)
{
    return ITypesUtil::Marshal(parcel, templateId.subscriberId_, templateId.bundleName_);
}

template<>
bool Marshalling(const PredicateTemplateNode &predicateTemplateNode, MessageParcel &parcel)
{
    return ITypesUtil::Marshal(parcel, predicateTemplateNode.key_, predicateTemplateNode.selectSql_);
}

template<>
bool Unmarshalling(PredicateTemplateNode &predicateTemplateNode, MessageParcel &parcel)
{
    return ITypesUtil::Unmarshal(parcel, predicateTemplateNode.key_, predicateTemplateNode.selectSql_);
}

template<>
bool Marshalling(const RdbChangeNode &changeNode, MessageParcel &parcel)
{
    bool firstPart = ITypesUtil::Marshal(
        parcel, changeNode.uri_, changeNode.templateId_, changeNode.data_, changeNode.isSharedMemory_);
    if (!firstPart) {
        return false;
    }
    if (changeNode.isSharedMemory_) {
        if (changeNode.memory_ == nullptr) {
            LOG_ERROR("Used shared memory but ashmem is nullptr.");
            return false;
        }
        if (!parcel.WriteAshmem(changeNode.memory_)) {
            return false;
        }
    }
    return ITypesUtil::Marshal(parcel, changeNode.size_);
}

template<>
bool Unmarshalling(RdbChangeNode &changeNode, MessageParcel &parcel)
{
    bool firstPart = ITypesUtil::Unmarshal(
        parcel, changeNode.uri_, changeNode.templateId_, changeNode.data_, changeNode.isSharedMemory_);
    if (!firstPart) {
        return false;
    }
    if (changeNode.isSharedMemory_) {
        changeNode.memory_ = parcel.ReadAshmem();
    } else {
        changeNode.memory_ = nullptr;
    }
    return ITypesUtil::Unmarshal(parcel, changeNode.size_);
}

template<>
bool Marshalling(const PublishedDataChangeNode &changeNode, MessageParcel &parcel)
{
    return ITypesUtil::Marshal(parcel, changeNode.ownerBundleName_, changeNode.datas_);
}

template<>
bool Unmarshalling(PublishedDataChangeNode &changeNode, MessageParcel &parcel)
{
    return ITypesUtil::Unmarshal(parcel, changeNode.ownerBundleName_, changeNode.datas_);
}

template<>
bool Marshalling(const OperationResult &operationResult, MessageParcel &parcel)
{
    return ITypesUtil::Marshal(parcel, operationResult.key_, operationResult.errCode_);
}

template<>
bool Unmarshalling(OperationResult &predicateTemplateNode, MessageParcel &parcel)
{
    return ITypesUtil::Unmarshal(parcel, predicateTemplateNode.key_, predicateTemplateNode.errCode_);
}

template<>
bool Unmarshalling(AshmemNode &node, MessageParcel &parcel)
{
    node.isManaged = true;
    node.ashmem = parcel.ReadAshmem();
    return true;
}

template<>
bool Marshalling(const AshmemNode &node, MessageParcel &parcel)
{
    return parcel.WriteAshmem(node.ashmem);
}

template<>
bool Marshalling(const Uri &node, MessageParcel &parcel)
{
    return parcel.WriteParcelable(&node);
}

template<>
bool Unmarshalling(Uri &node, MessageParcel &parcel)
{
    auto uri = std::shared_ptr<Uri>(parcel.ReadParcelable<Uri>());
    if (uri == nullptr) {
        return false;
    }
    node = *uri;
    return true;
}

template<>
bool Marshalling(const DataShareValuesBucket &bucket, MessageParcel &parcel)
{
    return ITypesUtil::Marshal(parcel, bucket.valuesMap);
}

template<>
bool Unmarshalling(DataShareValuesBucket &bucket, MessageParcel &parcel)
{
    return ITypesUtil::Unmarshal(parcel, bucket.valuesMap);
}

template<>
bool Marshalling(const OperationStatement &operationStatement, MessageParcel &parcel)
{
    return ITypesUtil::Marshal(parcel, static_cast<int32_t>(operationStatement.operationType),
        operationStatement.uri, operationStatement.valuesBucket, operationStatement.predicates);
}

template<>
bool Unmarshalling(OperationStatement &operationStatement, MessageParcel &parcel)
{
    int type;
    if (!ITypesUtil::Unmarshalling(type, parcel)) {
        return false;
    }
    operationStatement.operationType = static_cast<DataShare::Operation>(type);
    return ITypesUtil::Unmarshal(parcel, operationStatement.uri,
        operationStatement.valuesBucket, operationStatement.predicates);
}

template<>
bool Marshalling(const ExecResult &execResult, MessageParcel &parcel)
{
    return ITypesUtil::Marshal(parcel, static_cast<int32_t>(execResult.operationType), execResult.code,
        execResult.message);
}

template<>
bool Unmarshalling(ExecResult &execResult, MessageParcel &parcel)
{
    int type;
    if (!ITypesUtil::Unmarshalling(type, parcel)) {
        return false;
    }
    execResult.operationType = static_cast<DataShare::Operation>(type);
    return ITypesUtil::Unmarshal(parcel, execResult.code, execResult.message);
}

template<>
bool Marshalling(const ExecResultSet &execResultSet, MessageParcel &parcel)
{
    return ITypesUtil::Marshal(parcel, static_cast<int32_t>(execResultSet.errorCode), execResultSet.results);
}

template<>
bool Unmarshalling(ExecResultSet &execResultSet, MessageParcel &parcel)
{
    int errorCode;
    if (!ITypesUtil::Unmarshalling(errorCode, parcel)) {
        return false;
    }
    execResultSet.errorCode = static_cast<DataShare::ExecErrorCode>(errorCode);
    return ITypesUtil::Unmarshal(parcel, execResultSet.results);
}

template <typename T>
bool MarshalBasicTypeToBuffer(std::ostringstream &oss, const T &value)
{
    oss.write(reinterpret_cast<const char *>(&value), sizeof(value));
    return oss.good();
}

template <typename T>
bool MarshalBasicTypeVecToBuffer(std::ostringstream &oss, const std::vector<T> &values)
{
    size_t valSize = values.size();
    if (!MarshalBasicTypeToBuffer(oss, valSize)) {
        return false;
    }
    if (valSize > 0)
        oss.write(reinterpret_cast<const char *>(values.data()), valSize * sizeof(T));
    return oss.good();
}

bool MarshalStringToBuffer(std::ostringstream &oss, const std::string &value)
{
    // write string length
    size_t len = value.length();
    if (!MarshalBasicTypeToBuffer(oss, len)) {
        return false;
    }
    // write string data
    if (len > 0) {
        oss.write(value.data(), len);
    }
    return oss.good();
}

bool MarshalStringVecToBuffer(std::ostringstream &oss, const std::vector<std::string> &values)
{
    // write vector size
    size_t len = values.size();
    if (!MarshalBasicTypeToBuffer(oss, len)) {
        return false;
    }
    for (const auto &it : values) {
        if (!MarshalStringToBuffer(oss, it)) {
            return false;
        }
    }
    return oss.good();
}

bool MarshalSingleTypeToBuffer(std::ostringstream &oss, const SingleValue::Type &value)
{
    // write typeId
    uint8_t typeId = value.index();
    if (!MarshalBasicTypeToBuffer(oss, typeId)) {
        return false;
    }
    switch (typeId) {
        case static_cast<uint8_t>(DataSharePredicatesObjectType::TYPE_NULL): {
            return oss.good();
        }
        case static_cast<uint8_t>(DataSharePredicatesObjectType::TYPE_INT): {
            int val = std::get<int>(value);
            if (!MarshalBasicTypeToBuffer(oss, val)) { return false; }
            break;
        }
        case static_cast<uint8_t>(DataSharePredicatesObjectType::TYPE_DOUBLE): {
            double val = std::get<double>(value);
            if (!MarshalBasicTypeToBuffer(oss, val)) { return false; }
            break;
        }
        case static_cast<uint8_t>(DataSharePredicatesObjectType::TYPE_STRING): {
            std::string val = std::get<std::string>(value);
            if (!MarshalStringToBuffer(oss, val)) { return false; }
            break;
        }
        case static_cast<uint8_t>(DataSharePredicatesObjectType::TYPE_BOOL): {
            bool val = std::get<bool>(value);
            if (!MarshalBasicTypeToBuffer(oss, val)) { return false; }
            break;
        }
        case static_cast<uint8_t>(DataSharePredicatesObjectType::TYPE_LONG): {
            int64_t val = std::get<int64_t>(value);
            if (!MarshalBasicTypeToBuffer(oss, val)) { return false; }
            break;
        }
        default:
            LOG_ERROR("MarshalSingleTypeToBuffer: unknown typeId");
            return false;
    }
    return oss.good();
}

bool MarshalSingleTypeVecToBuffer(std::ostringstream &oss, const std::vector<SingleValue::Type> &values)
{
    // write vector size
    size_t len = values.size();
    if (!MarshalBasicTypeToBuffer(oss, len)) {
        return false;
    }
    for (const auto &it : values) {
        if (!MarshalSingleTypeToBuffer(oss, it)) {
            return false;
        }
    }
    return oss.good();
}

bool MarshalMultiTypeToBuffer(std::ostringstream &oss, const MutliValue::Type &value)
{
    uint8_t typeId = value.index();
    if (!MarshalBasicTypeToBuffer(oss, typeId)) {
        return false;
    }
    // add offset of TYPE_NULL
    typeId += static_cast<uint8_t>(DataSharePredicatesObjectsType::TYPE_NULL);
    switch (typeId) {
        case static_cast<uint8_t>(DataSharePredicatesObjectsType::TYPE_NULL): {
            return oss.good();
        }
        case static_cast<uint8_t>(DataSharePredicatesObjectsType::TYPE_INT_VECTOR): {
            std::vector<int> val = std::get<std::vector<int>>(value);
            if (!MarshalBasicTypeVecToBuffer(oss, val)) { return false; }
            break;
        }
        case static_cast<uint8_t>(DataSharePredicatesObjectsType::TYPE_DOUBLE_VECTOR): {
            std::vector<double> val = std::get<std::vector<double>>(value);
            if (!MarshalBasicTypeVecToBuffer(oss, val)) { return false; }
            break;
        }
        case static_cast<uint8_t>(DataSharePredicatesObjectsType::TYPE_LONG_VECTOR): {
            std::vector<int64_t> val = std::get<std::vector<int64_t>>(value);
            if (!MarshalBasicTypeVecToBuffer(oss, val)) { return false; }
            break;
        }
        case static_cast<uint8_t>(DataSharePredicatesObjectsType::TYPE_STRING_VECTOR): {
            auto val = std::get<std::vector<std::string>>(value);
            if (!MarshalStringVecToBuffer(oss, val)) { return false; }
            break;
        }
        default:
            LOG_ERROR("MarshalMultiTypeToBuffer: unknown typeId");
            return false;
    }
    return oss.good();
}

bool MarshalMultiTypeVecToBuffer(std::ostringstream &oss, const std::vector<MutliValue::Type> &values)
{
    size_t len = values.size();
    if (!MarshalBasicTypeToBuffer(oss, len)) {
        return false;
    }
    for (const auto &it : values) {
        if (!MarshalMultiTypeToBuffer(oss, it)) {
            return false;
        }
    }
    return oss.good();
}

bool MarshalOperationItemToBuffer(std::ostringstream &oss, const OperationItem &value)
{
    int32_t operation = value.operation;
    std::vector<SingleValue::Type> singleParams = value.singleParams;
    std::vector<MutliValue::Type> multiParams = value.multiParams;

    // Serialize operation
    if (!MarshalBasicTypeToBuffer(oss, operation)) {
        return false;
    }
    // Serialize singleParams
    if (!MarshalSingleTypeVecToBuffer(oss, singleParams)) {
        return false;
    }
    // Serialize multiParams
    if (!MarshalMultiTypeVecToBuffer(oss, multiParams)) {
        return false;
    }
    return oss.good();
}

bool MarshalOperationItemVecToBuffer(std::ostringstream &oss, const std::vector<OperationItem> &values)
{
    size_t len = values.size();
    if (!MarshalBasicTypeToBuffer(oss, len)) {
        return false;
    }
    for (const auto &it : values) {
        if (!MarshalOperationItemToBuffer(oss, it)) {
            return false;
        }
    }
    return oss.good();
}

bool MarshalPredicatesToBuffer(std::ostringstream &oss, const DataSharePredicates &predicates)
{
    // Extract all members of predicates
    const std::vector<OperationItem> &operations = predicates.GetOperationList();
    std::string whereClause = predicates.GetWhereClause();
    std::vector<std::string> whereArgs = predicates.GetWhereArgs();
    std::string order = predicates.GetOrder();
    short mode = predicates.GetSettingMode();

    // Serialize operations
    if (!MarshalOperationItemVecToBuffer(oss, operations)) {
        return false;
    }
    // Serialize whereClause
    if (!MarshalStringToBuffer(oss, whereClause)) {
        return false;
    }
    // Serialize whereArgs
    if (!MarshalStringVecToBuffer(oss, whereArgs)) {
        return false;
    }
    // Serialize order
    if (!MarshalStringToBuffer(oss, order)) {
        return false;
    }
    // Serialize mode
    if (!MarshalBasicTypeToBuffer(oss, mode)) {
        return false;
    }
    return oss.good();
}

bool MarshalValuesBucketToBuffer(std::ostringstream &oss, const DataShareValuesBucket &bucket)
{
    for (const auto &[key, value] : bucket.valuesMap) {
        // write key
        if (!MarshalStringToBuffer(oss, key)) { return false; }
        // write typeId
        uint8_t typeId = value.index();
        if (!MarshalBasicTypeToBuffer(oss, typeId)) { return false; }
        switch (typeId) {
            case static_cast<uint8_t>(DataShareValueObjectType::TYPE_NULL): {
                continue;
            }
            case static_cast<uint8_t>(DataShareValueObjectType::TYPE_INT): {
                int64_t val = std::get<int64_t>(value);
                if (!MarshalBasicTypeToBuffer(oss, val)) { return false; }
                break;
            }
            case static_cast<uint8_t>(DataShareValueObjectType::TYPE_DOUBLE): {
                double val = std::get<double>(value);
                if (!MarshalBasicTypeToBuffer(oss, val)) { return false; }
                break;
            }
            case static_cast<uint8_t>(DataShareValueObjectType::TYPE_STRING): {
                std::string val = std::get<std::string>(value);
                if (!MarshalStringToBuffer(oss, val)) { return false; }
                break;
            }
            case static_cast<uint8_t>(DataShareValueObjectType::TYPE_BOOL): {
                bool val = std::get<bool>(value);
                if (!MarshalBasicTypeToBuffer(oss, val)) { return false; }
                break;
            }
            case static_cast<uint8_t>(DataShareValueObjectType::TYPE_BLOB): {
                std::vector<uint8_t> val = std::get<std::vector<uint8_t>>(value);
                if (!MarshalBasicTypeVecToBuffer(oss, val)) { return false; }
                break;
            }
            default:
                LOG_ERROR("MarshalValuesBucketToBuffer: unknown typeId");
                return false;
        }
    }
    return oss.good();
}

bool MarshalValuesBucketVecToBuffer(std::ostringstream &oss, const std::vector<DataShareValuesBucket> &values)
{
    size_t size = values.size();
    if (!MarshalBasicTypeToBuffer(oss, size)) {
        return false;
    }
    for (const auto &bucket : values) {
        size_t mapSize = bucket.valuesMap.size();
        if (!MarshalBasicTypeToBuffer(oss, mapSize)) {
            return false;
        }
        if (!MarshalValuesBucketToBuffer(oss, bucket)) {
            return false;
        }
    }
    return oss.good();
}

template <typename T>
bool UnmarshalBasicTypeToBuffer(std::istringstream &iss, T &value)
{
    iss.read(reinterpret_cast<char *>(&value), sizeof(value));
    return iss.good();
}

template <typename T>
bool UnmarshalBasicTypeVecToBuffer(std::istringstream &iss, std::vector<T> &values)
{
    size_t valSize = 0;
    if (!UnmarshalBasicTypeToBuffer(iss, valSize)) {
        return false;
    }
    if (valSize > 0) {
        values.resize(valSize);
        iss.read(reinterpret_cast<char *>(values.data()), valSize * sizeof(T));
    }
    return iss.good();
}

bool UnmarshalStringToBuffer(std::istringstream &iss, std::string &value)
{
    // Get string length
    size_t len;
    if (!UnmarshalBasicTypeToBuffer(iss, len)) {
        return false;
    }
    // Get string content
    if (len > 0) {
        value.resize(len, '\0');
        iss.read(value.data(), len);
    }
    return iss.good();
}

bool UnmarshalStringVecToBuffer(std::istringstream &iss, std::vector<std::string> &values)
{
    // Get vec length
    size_t len;
    if (!UnmarshalBasicTypeToBuffer(iss, len)) {
        return false;
    }
    for (size_t i = 0; i < len; i++) {
        std::string value;
        if (!UnmarshalStringToBuffer(iss, value)) {
            return false;
        }
        values.push_back(value);
    }
    return iss.good();
}

bool UnmarshalSingleTypeToBuffer(std::istringstream &iss, SingleValue::Type &value)
{
    // Get type of value
    uint8_t typeId;
    if (!UnmarshalBasicTypeToBuffer(iss, typeId)) {
        return false;
    }
    // Deserialize according to the type
    switch (typeId) {
        case static_cast<uint8_t>(DataSharePredicatesObjectType::TYPE_NULL): {
            return iss.good();
        }
        case static_cast<uint8_t>(DataSharePredicatesObjectType::TYPE_INT): {
            int intVal;
            if (!UnmarshalBasicTypeToBuffer(iss, intVal)) { return false; }
            value = intVal;
            break;
        }
        case static_cast<uint8_t>(DataSharePredicatesObjectType::TYPE_DOUBLE): {
            double doubleVal;
            if (!UnmarshalBasicTypeToBuffer(iss, doubleVal)) { return false; }
            value = doubleVal;
            break;
        }
        case static_cast<uint8_t>(DataSharePredicatesObjectType::TYPE_STRING): {
            std::string str;
            if (!UnmarshalStringToBuffer(iss, str)) { return false; }
            value = str;
            break;
        }
        case static_cast<uint8_t>(DataSharePredicatesObjectType::TYPE_BOOL): {
            bool val;
            if (!UnmarshalBasicTypeToBuffer(iss, val)) { return false; }
            value = val;
            break;
        }
        case static_cast<uint8_t>(DataSharePredicatesObjectType::TYPE_LONG): {
            int64_t longVal;
            if (!UnmarshalBasicTypeToBuffer(iss, longVal)) { return false; }
            value = longVal;
            break;
        }
        default:
            LOG_ERROR("UnmarshalSingleTypeToBuffer: unknown typeId");
            return false;
        }
    return iss.good();
}

bool UnmarshalSingleTypeVecToBuffer(std::istringstream &iss, std::vector<SingleValue::Type> &values)
{
    // Get vec length
    size_t len;
    if (!UnmarshalBasicTypeToBuffer(iss, len)) {
        return false;
    }
    for (size_t i = 0; i < len; i++) {
        SingleValue::Type value;
        if (!UnmarshalSingleTypeToBuffer(iss, value)) {
            return false;
        }
        values.push_back(value);
    }
    return iss.good();
}

bool UnmarshalMultiTypeToBuffer(std::istringstream &iss, MutliValue::Type &value)
{
    // Get type of value
    uint8_t typeId;
    if (!UnmarshalBasicTypeToBuffer(iss, typeId)) {
        return false;
    }
    // add offset of TYPE_NULL
    typeId += static_cast<uint8_t>(DataSharePredicatesObjectsType::TYPE_NULL);
    switch (typeId) {
        case static_cast<uint8_t>(DataSharePredicatesObjectsType::TYPE_NULL): {
            return iss.good();
        }
        case static_cast<uint8_t>(DataSharePredicatesObjectsType::TYPE_INT_VECTOR): {
            std::vector<int> intVector;
            if (!UnmarshalBasicTypeVecToBuffer(iss, intVector)) { return false; }
            value = intVector;
            break;
        }
        case static_cast<uint8_t>(DataSharePredicatesObjectsType::TYPE_LONG_VECTOR): {
            std::vector<int64_t> longVector;
            if (!UnmarshalBasicTypeVecToBuffer(iss, longVector)) { return false; }
            value = longVector;
            break;
        }
        case static_cast<uint8_t>(DataSharePredicatesObjectsType::TYPE_DOUBLE_VECTOR): {
            std::vector<double> doubleVector;
            if (!UnmarshalBasicTypeVecToBuffer(iss, doubleVector)) { return false; }
            value = doubleVector;
            break;
        }
        case static_cast<uint8_t>(DataSharePredicatesObjectsType::TYPE_STRING_VECTOR): {
            std::vector<std::string> strVector;
            if (!UnmarshalStringVecToBuffer(iss, strVector)) { return false; }
            value = strVector;
            break;
        }
        default:
            LOG_ERROR("UnmarshalMultiTypeToBuffer: unknown typeId");
            return false;
    }
    return iss.good();
}

bool UnmarshalMultiTypeVecToBuffer(std::istringstream &iss, std::vector<MutliValue::Type> &values)
{
    size_t len;
    if (!UnmarshalBasicTypeToBuffer(iss, len)) {
        return false;
    }
    for (size_t i = 0; i < len; i++) {
        MutliValue::Type typ;
        if (!UnmarshalMultiTypeToBuffer(iss, typ)) {
            return false;
        }
        values.push_back(typ);
    }
    return iss.good();
}

bool UnmarshalOperationItemToBuffer(std::istringstream &iss, OperationItem &value)
{
    // Deserialize operation
    if (!UnmarshalBasicTypeToBuffer(iss, value.operation)) {
        return false;
    }
    // Deserialize singleParams
    if (!UnmarshalSingleTypeVecToBuffer(iss, value.singleParams)) {
        return false;
    }
    // Deserialize multiParams
    if (!UnmarshalMultiTypeVecToBuffer(iss, value.multiParams)) {
        return false;
    }

    return iss.good();
}

bool UnmarshalOperationItemVecToBuffer(std::istringstream &iss, std::vector<OperationItem> &values)
{
    size_t len;
    if (!UnmarshalBasicTypeToBuffer(iss, len)) {
        return false;
    }
    for (size_t i = 0; i < len; i++) {
        OperationItem item;
        if (!UnmarshalOperationItemToBuffer(iss, item)) {
            return false;
        }
        values.push_back(item);
    }
    return iss.good();
}

bool UnmarshalPredicatesToBuffer(std::istringstream &iss, DataSharePredicates &predicates)
{
    std::vector<OperationItem> operations = {};
    std::string whereClause = "";
    std::vector<std::string> whereArgs = {};
    std::string order = "";
    short mode = 0;

    // Deserialize operations
    if (!UnmarshalOperationItemVecToBuffer(iss, operations)) {
        return false;
    }
    // Deserialize whereClause
    if (!UnmarshalStringToBuffer(iss, whereClause)) {
        return false;
    }
    // Deserialize whereArgs
    if (!UnmarshalStringVecToBuffer(iss, whereArgs)) {
        return false;
    }
    // Deserialize order
    if (!UnmarshalStringToBuffer(iss, order)) {
        return false;
    }
    // Deserialize mode
    if (!UnmarshalBasicTypeToBuffer(iss, mode)) {
        return false;
    }

    predicates.SetOperationList(operations);
    predicates.SetWhereClause(whereClause);
    predicates.SetWhereArgs(whereArgs);
    predicates.SetOrder(order);
    predicates.SetSettingMode(mode);

    return iss.good();
}

bool UnmarshalValuesMapToBuffer(std::istringstream &iss,
    std::map<std::string, DataShareValueObject::Type> &valuesMap)
{
    std::string key;
    UnmarshalStringToBuffer(iss, key);
    uint8_t typeId;
    UnmarshalBasicTypeToBuffer(iss, typeId);
    DataShareValueObject::Type value;
    switch (typeId) {
        case static_cast<uint8_t>(DataShareValueObjectType::TYPE_NULL): { return iss.good(); }
        case static_cast<uint8_t>(DataShareValueObjectType::TYPE_INT): {
            int64_t val;
            UnmarshalBasicTypeToBuffer(iss, val);
            value = val;
            break;
        }
        case static_cast<uint8_t>(DataShareValueObjectType::TYPE_DOUBLE): {
            double val;
            UnmarshalBasicTypeToBuffer(iss, val);
            value = val;
            break;
        }
        case static_cast<uint8_t>(DataShareValueObjectType::TYPE_STRING): {
            std::string val;
            UnmarshalStringToBuffer(iss, val);
            value = val;
            break;
        }
        case static_cast<uint8_t>(DataShareValueObjectType::TYPE_BOOL): {
            bool val;
            UnmarshalBasicTypeToBuffer(iss, val);
            value = val;
            break;
        }
        case static_cast<uint8_t>(DataShareValueObjectType::TYPE_BLOB): {
            std::vector<uint8_t> val;
            UnmarshalBasicTypeVecToBuffer(iss, val);
            value = val;
            break;
        }
        default:
            LOG_ERROR("UnmarshalValuesMapToBuffer: unknown typeId");
            return false;
    }
    valuesMap.insert(std::make_pair(key, value));
    return iss.good();
}

bool UnmarshalValuesBucketVecToBuffer(std::istringstream &iss, std::vector<DataShareValuesBucket> &values)
{
    size_t size;
    if (!UnmarshalBasicTypeToBuffer(iss, size)) { return false; }
    for (size_t i = 0; i < size; i++) {
        size_t mapSize;
        if (!UnmarshalBasicTypeToBuffer(iss, mapSize)) { return false; }
        std::map<std::string, DataShareValueObject::Type> valuesMap;
        for (size_t j = 0; j < mapSize; j++) {
            if (!UnmarshalValuesMapToBuffer(iss, valuesMap)) {
                return false;
            }
        }
        DataShareValuesBucket value(valuesMap);
        values.push_back(value);
    }
    return iss.good();
}

bool MarshalPredicates(const Predicates &predicates, MessageParcel &parcel)
{
    std::ostringstream oss;
    if (!MarshalPredicatesToBuffer(oss, predicates)) {
        LOG_ERROR("MarshalPredicatesToBuffer failed.");
        return false;
    }
    std::string str = oss.str();
    size_t size = str.length();
    if (size > MAX_IPC_SIZE) {
        LOG_ERROR("Size of predicates is too large.");
        return false;
    }
    if (!parcel.WriteInt32(size)) {
        LOG_ERROR("Write size failed.");
        return false;
    }
    return parcel.WriteRawData(reinterpret_cast<const void *>(str.data()), size);
}

bool UnmarshalPredicates(Predicates &predicates, MessageParcel &parcel)
{
    int32_t length = parcel.ReadInt32();
    if (length < 1) {
        LOG_ERROR("Length of predicates is invalid.");
        return false;
    }
    const char *buffer = reinterpret_cast<const char *>(parcel.ReadRawData(static_cast<size_t>(length)));
    if (buffer == nullptr) {
        LOG_ERROR("ReadRawData failed.");
        return false;
    }
    std::istringstream iss(std::string(buffer, length));
    return UnmarshalPredicatesToBuffer(iss, predicates);
}

bool MarshalValuesBucketVec(const std::vector<DataShareValuesBucket> &values, MessageParcel &parcel)
{
    std::ostringstream oss;
    if (!MarshalValuesBucketVecToBuffer(oss, values)) {
        LOG_ERROR("MarshalValuesBucketVecToBuffer failed.");
        return false;
    }
    std::string str = oss.str();
    size_t size = str.length();
    if (size > MAX_IPC_SIZE) {
        LOG_ERROR("Size of ValuesBucketVec is too large.");
        return false;
    }
    if (!parcel.WriteInt32(size)) {
        LOG_ERROR("Write size failed.");
        return false;
    }
    return parcel.WriteRawData(reinterpret_cast<const void *>(str.data()), size);
}

bool UnmarshalValuesBucketVec(std::vector<DataShareValuesBucket> &values, MessageParcel &parcel)
{
    int32_t length = parcel.ReadInt32();
    if (length < 1) {
        LOG_ERROR("Length of ValuesBucketVec is invalid.");
        return false;
    }
    const char *buffer = reinterpret_cast<const char *>(parcel.ReadRawData(static_cast<size_t>(length)));
    if (buffer == nullptr) {
        LOG_ERROR("ReadRawData failed.");
        return false;
    }
    std::istringstream iss(std::string(buffer, length));
    return UnmarshalValuesBucketVecToBuffer(iss, values);
}
}  // namespace OHOS::ITypesUtil