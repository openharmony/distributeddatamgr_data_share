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

#define LOG_TAG "datashare_itypes_utils"

#include "datashare_itypes_utils.h"

#include <cstdint>
#include <iomanip>
#include <sstream>
#include <variant>
#include "dataproxy_handle_common.h"
#include "datashare_log.h"
#include "datashare_value_object.h"
#include "itypes_util.h"

namespace OHOS::ITypesUtil {
using namespace OHOS::DataShare;

// Maximum value of IPC shared memory
static const size_t MAX_IPC_SIZE = 128 * 1024 * 1024;
// Maximum count of ProxyDatas
static const size_t PROXY_DATA_MAX_COUNT = 64;

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
    if (!execResult.IsOperationTypeValid()) {
        // existing marshalling use static cast on enum, only log when operationType exceeds defined types
        LOG_ERROR("operationType Invalid:%{public}d", execResult.operationType);
    }
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
    if (!execResult.IsOperationTypeValid()) {
        // existing marshalling use static cast on enum, only log when operationType exceeds defined types
        LOG_ERROR("operationType Invalid:%{public}d", execResult.operationType);
    }
    return ITypesUtil::Unmarshal(parcel, execResult.code, execResult.message);
}

template<>
bool Marshalling(const ExecResultSet &execResultSet, MessageParcel &parcel)
{
    if (!execResultSet.IsErrorCodeValid()) {
        // existing marshalling use static cast on enum, only log when errorCode exceeds defined types
        LOG_ERROR("errorCode Invalid:%{public}d", execResultSet.errorCode);
    }
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
    if (!execResultSet.IsErrorCodeValid()) {
        // existing marshalling use static cast on enum, only log when errorCode exceeds defined types
        LOG_ERROR("errorCode Invalid:%{public}d", execResultSet.errorCode);
    }
    return ITypesUtil::Unmarshal(parcel, execResultSet.results);
}

template<>
bool Marshalling(const DataShareProxyData &proxyData, MessageParcel &parcel)
{
    return ITypesUtil::Marshal(parcel, proxyData.uri_,
        proxyData.value_, proxyData.isValueUndefined, proxyData.allowList_, proxyData.isAllowListUndefined);
}

template<>
bool Unmarshalling(DataShareProxyData &proxyData, MessageParcel &parcel)
{
    return ITypesUtil::Unmarshal(parcel, proxyData.uri_,
        proxyData.value_, proxyData.isValueUndefined, proxyData.allowList_, proxyData.isAllowListUndefined);
}

template<>
bool Marshalling(const DataProxyConfig &config, MessageParcel &parcel)
{
    return ITypesUtil::Marshal(parcel, static_cast<int32_t>(config.type_),
        static_cast<int32_t>(config.maxValueLength_));
}

template<>
bool Unmarshalling(DataProxyConfig &config, MessageParcel &parcel)
{
    int32_t type;
    int32_t maxValueLength;
    if (!ITypesUtil::Unmarshal(parcel, type, maxValueLength)) {
        return false;
    }
    config.type_ = static_cast<DataProxyType>(type);
    if (maxValueLength != static_cast<int32_t>(DataProxyMaxValueLength::MAX_LENGTH_4K) &&
        maxValueLength != static_cast<int32_t>(DataProxyMaxValueLength::MAX_LENGTH_100K)) {
        return false;
    } else {
        config.maxValueLength_ = static_cast<DataProxyMaxValueLength>(maxValueLength);
    }
    return true;
}

template<>
bool Marshalling(const DataProxyResult &result, MessageParcel &parcel)
{
    return ITypesUtil::Marshal(parcel, static_cast<int32_t>(result.result_), result.uri_);
}

template<>
bool Unmarshalling(DataProxyResult &result, MessageParcel &parcel)
{
    int errorCode;
    if (!ITypesUtil::Unmarshalling(errorCode, parcel)) {
        return false;
    }
    result.result_ = static_cast<DataProxyErrorCode>(errorCode);
    return ITypesUtil::Unmarshal(parcel, result.uri_);
}

template<>
bool Marshalling(const DataShareValueObject &value, MessageParcel &parcel)
{
    if (!ITypesUtil::Marshal(parcel, value.value.index())) {
        return false;
    }
    switch (value.value.index()) {
        case DataShareValueObjectType::TYPE_INT: {
            return ITypesUtil::Marshal(parcel, std::get<int64_t>(value.value));
        }
        case DataShareValueObjectType::TYPE_DOUBLE: {
            return ITypesUtil::Marshal(parcel, std::get<double>(value.value));
        }
        case DataShareValueObjectType::TYPE_STRING: {
            return ITypesUtil::Marshal(parcel, std::get<std::string>(value.value));
        }
        case DataShareValueObjectType::TYPE_BOOL: {
            return ITypesUtil::Marshal(parcel, std::get<bool>(value.value));
        }
        default: {
            LOG_ERROR("Marshal ValueObject: unknown typeId");
            return false;
        }
    }
}

template<>
bool Unmarshalling(DataShareValueObject &value, MessageParcel &parcel)
{
    int32_t index;
    if (!ITypesUtil::Unmarshal(parcel, index)) {
        return false;
    }
    bool ret = true;
    switch (index) {
        case static_cast<uint8_t>(DataShareValueObjectType::TYPE_INT): {
            int64_t val;
            ret = ITypesUtil::Unmarshal(parcel, val);
            value = val;
            break;
        }
        case static_cast<uint8_t>(DataShareValueObjectType::TYPE_DOUBLE): {
            double val;
            ret = ITypesUtil::Unmarshal(parcel, val);
            value = val;
            break;
        }
        case static_cast<uint8_t>(DataShareValueObjectType::TYPE_STRING): {
            std::string val;
            ret = ITypesUtil::Unmarshal(parcel, val);
            value = val;
            break;
        }
        case static_cast<uint8_t>(DataShareValueObjectType::TYPE_BOOL): {
            bool val;
            ret = ITypesUtil::Unmarshal(parcel, val);
            value = val;
            break;
        }
        default: {
            LOG_ERROR("Unmarshal ValueObject: unknown typeId");
            ret = false;
        }
    }
    return ret;
}

template<>
bool Marshalling(const DataProxyValue &value, MessageParcel &parcel)
{
    int32_t index = value.index();
    if (!ITypesUtil::Marshal(parcel, index)) {
        return false;
    }
    switch (value.index()) {
        case static_cast<uint8_t>(DataProxyValueType::VALUE_INT): {
            return ITypesUtil::Marshal(parcel, std::get<int64_t>(value));
        }
        case static_cast<uint8_t>(DataProxyValueType::VALUE_DOUBLE): {
            return ITypesUtil::Marshal(parcel, std::get<double>(value));
        }
        case static_cast<uint8_t>(DataProxyValueType::VALUE_STRING): {
            return ITypesUtil::Marshal(parcel, std::get<std::string>(value));
        }
        case static_cast<uint8_t>(DataProxyValueType::VALUE_BOOL): {
            return ITypesUtil::Marshal(parcel, std::get<bool>(value));
        }
        default: {
            LOG_ERROR("Marshal ValueObject: unknown typeId");
            return false;
        }
    }
}

template<>
bool Unmarshalling(DataProxyValue &value, MessageParcel &parcel)
{
    int32_t index;
    if (!ITypesUtil::Unmarshal(parcel, index)) {
        return false;
    }
    bool ret = true;
    switch (index) {
        case static_cast<uint8_t>(DataProxyValueType::VALUE_INT): {
            int64_t val;
            ret = ITypesUtil::Unmarshal(parcel, val);
            value = val;
            break;
        }
        case static_cast<uint8_t>(DataProxyValueType::VALUE_DOUBLE): {
            double val;
            ret = ITypesUtil::Unmarshal(parcel, val);
            value = val;
            break;
        }
        case static_cast<uint8_t>(DataProxyValueType::VALUE_STRING): {
            std::string val;
            ret = ITypesUtil::Unmarshal(parcel, val);
            value = val;
            break;
        }
        case static_cast<uint8_t>(DataProxyValueType::VALUE_BOOL): {
            bool val;
            ret = ITypesUtil::Unmarshal(parcel, val);
            value = val;
            break;
        }
        default: {
            LOG_ERROR("Unmarshal DataProxyValue: unknown typeId");
            ret = false;
        }
    }
    return ret;
}

template<>
bool Marshalling(const DataProxyGetResult &result, MessageParcel &parcel)
{
    return ITypesUtil::Marshal(parcel, static_cast<int32_t>(result.result_),
        result.uri_, result.value_, result.allowList_);
}

template<>
bool Unmarshalling(DataProxyGetResult &result, MessageParcel &parcel)
{
    int errorCode;
    if (!ITypesUtil::Unmarshalling(errorCode, parcel)) {
        return false;
    }
    result.result_ = static_cast<DataProxyErrorCode>(errorCode);
    return ITypesUtil::Unmarshal(parcel, result.uri_, result.value_, result.allowList_);
}

template<>
bool Marshalling(const DataProxyChangeInfo &changeInfo, MessageParcel &parcel)
{
    return ITypesUtil::Marshal(parcel, static_cast<int32_t>(changeInfo.changeType_),
        changeInfo.uri_, changeInfo.value_);
}

template<>
bool Unmarshalling(DataProxyChangeInfo &changeInfo, MessageParcel &parcel)
{
    int errorCode;
    if (!ITypesUtil::Unmarshalling(errorCode, parcel)) {
        return false;
    }
    changeInfo.changeType_ = static_cast<DataShareObserver::ChangeType>(errorCode);
    return ITypesUtil::Unmarshal(parcel, changeInfo.uri_, changeInfo.value_);
}

template<>
bool Marshalling(const TimedQueryUriInfo &result, MessageParcel &parcel)
{
    return ITypesUtil::Marshal(parcel, result.uri, result.extUri, result.option.timeout);
}

template<>
bool Unmarshalling(TimedQueryUriInfo &result, MessageParcel &parcel)
{
    return ITypesUtil::Unmarshal(parcel, result.uri, result.extUri, result.option.timeout);
}

template<>
bool Marshalling(const ConnectionInterfaceInfo &info, MessageParcel &parcel)
{
    return ITypesUtil::Marshal(parcel, info.descriptor_, info.code_);
}

template<>
bool Unmarshalling(ConnectionInterfaceInfo &info, MessageParcel &parcel)
{
    return ITypesUtil::Unmarshal(parcel, info.descriptor_, info.code_);
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
        if (valSize > MAX_IPC_SIZE / sizeof(T)) {
            LOG_ERROR("valSize of BasicType is too large.");
            return false;
        }
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
        if (len > MAX_IPC_SIZE) {
            LOG_ERROR("length of string is too large.");
            return false;
        }
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
        LOG_ERROR("Unmarshal operations failed.");
        return false;
    }
    // Deserialize whereClause
    if (!UnmarshalStringToBuffer(iss, whereClause)) {
        LOG_ERROR("Unmarshal whereClause failed.");
        return false;
    }
    // Deserialize whereArgs
    if (!UnmarshalStringVecToBuffer(iss, whereArgs)) {
        LOG_ERROR("Unmarshal whereArgs failed.");
        return false;
    }
    // Deserialize order
    if (!UnmarshalStringToBuffer(iss, order)) {
        LOG_ERROR("Unmarshal order failed.");
        return false;
    }
    // Deserialize mode
    if (!UnmarshalBasicTypeToBuffer(iss, mode)) {
        LOG_ERROR("Unmarshal mode failed.");
        return false;
    }

    predicates.SetOperationList(operations);
    predicates.SetWhereClause(whereClause);
    predicates.SetWhereArgs(whereArgs);
    predicates.SetOrder(order);
    predicates.SetSettingMode(mode);

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
    if (static_cast<size_t>(length) > MAX_IPC_SIZE) {
        LOG_ERROR("Length of predicates is too large.");
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

bool MarshalValuesBucketToBuffer(std::ostringstream &oss, const DataShareValuesBucket &bucket)
{
    // marshal valuesMap size
    size_t mapSize = bucket.valuesMap.size();
    if (!MarshalBasicTypeToBuffer(oss, mapSize)) {
        LOG_ERROR("Marshal valuesMap size failed");
        return false;
    }
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

bool UnmarshalValuesBucketToBuffer(std::istringstream &iss,
    DataShareValuesBucket &bucket)
{
    size_t mapSize;
    if (!UnmarshalBasicTypeToBuffer(iss, mapSize)) {
        LOG_ERROR("Unmarshal valuesMap size failed");
        return false;
    }
    for (size_t j = 0; j < mapSize; j++) {
        std::string key;
        UnmarshalStringToBuffer(iss, key);
        uint8_t typeId;
        UnmarshalBasicTypeToBuffer(iss, typeId);
        DataShareValueObject::Type value;
        switch (typeId) {
            case static_cast<uint8_t>(DataShareValueObjectType::TYPE_NULL): { continue; }
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
        bucket.valuesMap.insert(std::make_pair(key, value));
    }
    return iss.good();
}

bool MarshalValuesBucketVecToBuffer(std::ostringstream &oss, const std::vector<DataShareValuesBucket> &values)
{
    size_t size = values.size();
    if (!MarshalBasicTypeToBuffer(oss, size)) {
        return false;
    }
    for (const auto &bucket : values) {
        if (!MarshalValuesBucketToBuffer(oss, bucket)) {
            return false;
        }
    }
    return oss.good();
}

bool UnmarshalValuesBucketVecToBuffer(std::istringstream &iss, std::vector<DataShareValuesBucket> &values)
{
    size_t size;
    if (!UnmarshalBasicTypeToBuffer(iss, size)) { return false; }
    for (size_t i = 0; i < size; i++) {
        DataShareValuesBucket bucket;
        if (!UnmarshalValuesBucketToBuffer(iss, bucket)) {
            return false;
        }
        values.push_back(bucket);
    }
    return iss.good();
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
    if (static_cast<size_t>(length) > MAX_IPC_SIZE) {
        LOG_ERROR("Length of ValuesBucketVec is too large.");
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

template<>
bool Marshalling(const RegisterOption &option, MessageParcel &parcel)
{
    return ITypesUtil::Marshal(parcel, option.isReconnect);
}

template<>
bool Unmarshalling(RegisterOption &option, MessageParcel &parcel)
{
    return ITypesUtil::Unmarshal(parcel, option.isReconnect);
}

bool MarshalBackReferenceToBuffer(std::ostringstream &oss, const BackReference &backReference)
{
    if (!MarshalStringToBuffer(oss, backReference.GetColumn())) {
        LOG_ERROR("Marshal column failed.");
        return false;
    }
    if (!MarshalBasicTypeToBuffer(oss, backReference.GetFromIndex())) {
        LOG_ERROR("Marshal fromIndex failed.");
        return false;
    }
    return oss.good();
}

bool UnmarshalBackReferenceToBuffer(std::istringstream &iss, BackReference &backReference)
{
    std::string column = "";
    int32_t fromIndex;
    if (!UnmarshalStringToBuffer(iss, column)) {
        LOG_ERROR("Unmarshal column failed.");
        return false;
    }
    backReference.SetColumn(column);
    if (!UnmarshalBasicTypeToBuffer(iss, fromIndex)) {
        LOG_ERROR("Unmarshal fromIndex failed.");
        return false;
    }
    backReference.SetFromIndex(fromIndex);
    return iss.good();
}

bool MarshalOperationStatementVecToBuffer(std::ostringstream &oss,
                                          const std::vector<OperationStatement> &operationStatements)
{
    size_t size = operationStatements.size();
    if (!MarshalBasicTypeToBuffer(oss, size)) {
        LOG_ERROR("Marshal vec size failed.");
        return false;
    }
    size_t index = 0;
    for (const auto &statement : operationStatements) {
        if (!statement.IsOperationTypeValid()) {
            // existing marshalling use static cast on enum, only log when operationType exceeds defined types
            LOG_ERROR("operationType Invalid:%{public}d, index:%{public}zu", statement.operationType, index);
        }
        if (!MarshalBasicTypeToBuffer(oss, statement.operationType)) {
            LOG_ERROR("Marshal operationType failed.");
            return false;
        }
        if (!MarshalStringToBuffer(oss, statement.uri)) {
            LOG_ERROR("Marshal uri failed.");
            return false;
        }
        if (!MarshalPredicatesToBuffer(oss, statement.predicates)) {
            LOG_ERROR("Marshal predicates failed.");
            return false;
        }
        if (!MarshalValuesBucketToBuffer(oss, statement.valuesBucket)) {
            LOG_ERROR("Marshal valuesBucket failed.");
            return false;
        }
        if (!MarshalBackReferenceToBuffer(oss, statement.backReference)) {
            LOG_ERROR("Marshal backReference failed.");
            return false;
        }
        index += 1;
    }
    return oss.good();
}

bool UnmarshalOperationStatementVecToBuffer(std::istringstream &iss,
                                            std::vector<OperationStatement> &operationStatements)
{
    size_t size;
    if (!UnmarshalBasicTypeToBuffer(iss, size)) {
        LOG_ERROR("Unmarshal vec size failed.");
        return false;
    }
    
    for (size_t i = 0; i < size; i++) {
        OperationStatement statement;
        if (!UnmarshalBasicTypeToBuffer(iss, statement.operationType)) {
            LOG_ERROR("Unmarshal operationType failed.");
            return false;
        }
        if (!statement.IsOperationTypeValid()) {
            // existing marshalling use static cast on enum, only log when operationType exceeds defined types
            LOG_ERROR("operationType Invalid:%{public}d, index:%{public}zu", statement.operationType, i);
        }
        if (!UnmarshalStringToBuffer(iss, statement.uri)) {
            LOG_ERROR("Unmarshal uri failed.");
            return false;
        }
        if (!UnmarshalPredicatesToBuffer(iss, statement.predicates)) {
            LOG_ERROR("Unmarshal predicates failed.");
            return false;
        }
        if (!UnmarshalValuesBucketToBuffer(iss, statement.valuesBucket)) {
            LOG_ERROR("Unmarshal valuesBucket failed.");
            return false;
        }
        if (!UnmarshalBackReferenceToBuffer(iss, statement.backReference)) {
            LOG_ERROR("Unmarshal backReference failed.");
            return false;
        }
        operationStatements.push_back(std::move(statement));
    }
    return iss.good();
}

// Currently as a substitution for MarshalToBuffer
bool MarshalOperationStatementVec(const std::vector<OperationStatement> &operationStatements, MessageParcel &parcel)
{
    std::ostringstream oss;
    if (!MarshalOperationStatementVecToBuffer(oss, operationStatements)) {
        LOG_ERROR("MarshalOperationStatementVecToBuffer failed.");
        return false;
    }
    std::string str = oss.str();
    size_t size = str.length();
    if (size > MAX_IPC_SIZE) {
        LOG_ERROR("Size of OperationStatementVec exceed limit:%{public}zu", size);
        return false;
    }
    if (!parcel.WriteInt32(size)) {
        LOG_ERROR("Write size failed.");
        return false;
    }
    return parcel.WriteRawData(reinterpret_cast<const void *>(str.data()), size);
}

bool UnmarshalOperationStatementVec(std::vector<OperationStatement> &operationStatements, MessageParcel &parcel)
{
    int32_t length = parcel.ReadInt32();
    if (length < 1) {
        LOG_ERROR("Length of OperationStatementVec is invalid:%{public}d", length);
        return false;
    }
    if (static_cast<size_t>(length) > MAX_IPC_SIZE) {
        LOG_ERROR("Length of OperationStatementVec exceed limit:%{public}d", length);
        return false;
    }
    const char *buffer = reinterpret_cast<const char *>(parcel.ReadRawData(static_cast<size_t>(length)));
    if (buffer == nullptr) {
        LOG_ERROR("ReadRawData failed.");
        return false;
    }
    std::istringstream iss(std::string(buffer, length));
    return UnmarshalOperationStatementVecToBuffer(iss, operationStatements);
}

bool MarshalDataProxyValueToBuffer(std::ostringstream &oss, const DataProxyValue &value)
{
    size_t typeId = value.index();
    if (!MarshalBasicTypeToBuffer(oss, typeId)) {
        LOG_ERROR("Marshal dataProxyValue typeId failed");
        return false;
    }
    switch (static_cast<DataProxyValueType>(typeId)) {
        case VALUE_INT: {
            if (!MarshalBasicTypeToBuffer(oss, std::get<int64_t>(value))) {
                LOG_ERROR("Marshal int value failed");
                return false;
            }
            break;
        }
        case VALUE_DOUBLE: {
            if (!MarshalBasicTypeToBuffer(oss, std::get<double>(value))) {
                LOG_ERROR("Marshal double value failed");
                return false;
            }
            break;
        }
        case VALUE_STRING: {
            if (!MarshalStringToBuffer(oss, std::get<std::string>(value))) {
                LOG_ERROR("Marshal string value failed");
                return false;
            }
            break;
        }
        case VALUE_BOOL: {
            if (!MarshalBasicTypeToBuffer(oss, std::get<bool>(value))) {
                LOG_ERROR("Marshal bool value failed");
                return false;
            }
            break;
        }
        default: {
            LOG_ERROR("Marshal dataProxyValue: unknown typeId");
            return false;
        }
    }
    return true;
}

bool MarshalProxyDataToBuffer(std::ostringstream &oss, const DataShareProxyData &data)
{
    // Write uri_
    if (!MarshalStringToBuffer(oss, data.uri_)) {
        LOG_ERROR("Marshal uri failed");
        return false;
    }
    // Write isValueUndefined and isAllowListUndefined
    if (!MarshalBasicTypeToBuffer(oss, data.isValueUndefined) ||
        !MarshalBasicTypeToBuffer(oss, data.isAllowListUndefined)) {
        return false;
    }
    // Write value_ if it is not undefined
    if (!data.isValueUndefined) {
        if (!MarshalDataProxyValueToBuffer(oss, data.value_)) {
            LOG_ERROR("Marshal value failed");
            return false;
        }
    }
    // Write allowList_ if it is not undefined
    if (!data.isAllowListUndefined) {
        if (!MarshalStringVecToBuffer(oss, data.allowList_)) {
            LOG_ERROR("Marshal allowList failed");
            return false;
        }
    }

    return oss.good();
}

bool MarshalProxyDataVecToBuffer(std::ostringstream &oss, const std::vector<DataShareProxyData> &proxyDatas)
{
    size_t size = proxyDatas.size();
    if (!MarshalBasicTypeToBuffer(oss, size)) {
        LOG_ERROR("Marshal proxyData size failed");
        return false;
    }
    for (const auto &data : proxyDatas) {
        if (!MarshalProxyDataToBuffer(oss, data)) {
            LOG_ERROR("Marshal proxyData failed");
            return false;
        }
    }
    return oss.good();
}

bool MarshalDataProxyGetResultToBuffer(std::ostringstream &oss, const DataProxyGetResult &result)
{
    // Write uri_
    if (!MarshalStringToBuffer(oss, result.uri_)) {
        LOG_ERROR("Marshal getResult.uri_ failed.");
        return false;
    }
    // Write result_
    if (!MarshalBasicTypeToBuffer(oss, result.result_)) {
        LOG_ERROR("Marshal getResult.result_ failed.");
        return false;
    }
    // Write value_
    if (!MarshalDataProxyValueToBuffer(oss, result.value_)) {
        LOG_ERROR("Marshal getResult.value_ failed.");
        return false;
    }
    // Write allowList_
    if (!MarshalStringVecToBuffer(oss, result.allowList_)) {
        LOG_ERROR("Marshal getResult.allowList_ failed.");
        return false;
    }

    return oss.good();
}

bool MarshalDataProxyGetResultVecToBuffer(std::ostringstream &oss, const std::vector<DataProxyGetResult> &results)
{
    size_t size = results.size();
    if (!MarshalBasicTypeToBuffer(oss, size)) {
        LOG_ERROR("Marshal results.size failed");
        return false;
    }
    for (const auto &result : results) {
        if (!MarshalDataProxyGetResultToBuffer(oss, result)) {
            LOG_ERROR("Marshal results failed");
            return false;
        }
    }
    return oss.good();
}

bool MarshalDataProxyChangeInfoToBuffer(std::ostringstream &oss, const DataProxyChangeInfo &info)
{
    // Write changeType_
    if (!MarshalBasicTypeToBuffer(oss, info.changeType_)) {
        LOG_ERROR("Marshal info.changeType_ failed");
        return false;
    }
    // Write uri_
    if (!MarshalStringToBuffer(oss, info.uri_)) {
        LOG_ERROR("Marshal info.uri_ failed");
        return false;
    }
    // Write value_
    if (!MarshalDataProxyValueToBuffer(oss, info.value_)) {
        LOG_ERROR("Marshal info.value_ failed");
        return false;
    }

    return oss.good();
}

bool MarshalDataProxyChangeInfoVecToBuffer(std::ostringstream &oss, const std::vector<DataProxyChangeInfo> &changeInfos)
{
    size_t size = changeInfos.size();
    if (!MarshalBasicTypeToBuffer(oss, size)) {
        LOG_ERROR("Marshal changeInfo.size failed");
        return false;
    }
    for (const auto &changeInfo : changeInfos) {
        if (!MarshalDataProxyChangeInfoToBuffer(oss, changeInfo)) {
            LOG_ERROR("Marshal changeInfo failed");
            return false;
        }
    }
    return oss.good();
}

bool UnmarshalDataProxyValueFromBuffer(std::istringstream &iss, DataProxyValue &value)
{
    size_t typeId;
    if (!UnmarshalBasicTypeToBuffer(iss, typeId)) {
        LOG_ERROR("Unmarshal typeId failed");
        return false;
    }
    switch (static_cast<DataProxyValueType>(typeId)) {
        case VALUE_INT: {
            int64_t intVal;
            if (!UnmarshalBasicTypeToBuffer(iss, intVal)) {
                LOG_ERROR("Unmarshal int failed");
                return false;
            }
            value = intVal;
            break;
        }
        case VALUE_DOUBLE: {
            double doubleVal;
            if (!UnmarshalBasicTypeToBuffer(iss, doubleVal)) {
                LOG_ERROR("Unmarshal double failed");
                return false;
            }
            value = doubleVal;
            break;
        }
        case VALUE_STRING: {
            std::string stringVal;
            if (!UnmarshalStringToBuffer(iss, stringVal)) {
                LOG_ERROR("Unmarshal string failed");
                return false;
            }
            value = stringVal;
            break;
        }
        case VALUE_BOOL: {
            bool boolVal;
            if (!UnmarshalBasicTypeToBuffer(iss, boolVal)) {
                LOG_ERROR("Unmarshal bool failed");
                return false;
            }
            value = boolVal;
            break;
        }
        default: {
            LOG_ERROR("Unmarshal: unknown typeId");
            return false;
        }
    }
    return true;
}

bool UnmarshalProxyDataFromBuffer(std::istringstream &iss, DataShareProxyData &data)
{
    // Read uri_
    if (!UnmarshalStringToBuffer(iss, data.uri_)) {
        LOG_ERROR("Unmarshal data.uri_ failed");
        return false;
    }
    // Read isValueUndefined and isAllowListUndefined
    if (!UnmarshalBasicTypeToBuffer(iss, data.isValueUndefined) ||
        !UnmarshalBasicTypeToBuffer(iss, data.isAllowListUndefined)) {
        LOG_ERROR("Unmarshal isValueUndefined or isAllowListUndefined failed");
        return false;
    }
    // Read value_ if it is not undefined
    if (!data.isValueUndefined) {
        if (!UnmarshalDataProxyValueFromBuffer(iss, data.value_)) {
            LOG_ERROR("Unmarshal data.value_ failed");
            return false;
        }
    }
    // Read allowList_ if it is not undefined
    if (!data.isAllowListUndefined) {
        if (!UnmarshalStringVecToBuffer(iss, data.allowList_)) {
            LOG_ERROR("Unmarshal data.allowList_ failed");
            return false;
        }
    }
    return iss.good();
}

bool UnmarshalProxyDataVecToBuffer(std::istringstream &iss, std::vector<DataShareProxyData> &proxyDatas)
{
    size_t size;
    if (!UnmarshalBasicTypeToBuffer(iss, size)) {
        LOG_ERROR("Unmarshal vec size failed.");
        return false;
    }
    if (size > PROXY_DATA_MAX_COUNT) {
        LOG_ERROR("ProxyData vec size: %{public}zu over limit", size);
        return false;
    }
    for (size_t i = 0; i < size; i++) {
        DataShareProxyData data;
        if (!UnmarshalProxyDataFromBuffer(iss, data)) {
            LOG_ERROR("Unmarshal proxyData failed");
            return false;
        }
        proxyDatas.push_back(std::move(data));
    }
    return iss.good();
}

bool UnmarshalDataProxyGetResultFromBuffer(std::istringstream &iss, DataProxyGetResult &result)
{
    // Read uri_
    if (!UnmarshalStringToBuffer(iss, result.uri_)) {
        LOG_ERROR("Unmarshal uri failed");
        return false;
    }
    // Read result_
    if (!UnmarshalBasicTypeToBuffer(iss, result.result_)) {
        LOG_ERROR("Unmarshal result failed");
        return false;
    }
    // Read value_
    if (!UnmarshalDataProxyValueFromBuffer(iss, result.value_)) {
        LOG_ERROR("Unmarshal value failed");
        return false;
    }
    // Read allowList_
    if (!UnmarshalStringVecToBuffer(iss, result.allowList_)) {
        LOG_ERROR("Unmarshal allowList failed");
        return false;
    }
    return iss.good();
}

bool UnmarshalDataProxyGetResultVecToBuffer(std::istringstream &iss, std::vector<DataProxyGetResult> &results)
{
    size_t size;
    if (!UnmarshalBasicTypeToBuffer(iss, size)) {
        LOG_ERROR("Unmarshal vec size failed.");
        return false;
    }
    if (size > PROXY_DATA_MAX_COUNT) {
        LOG_ERROR("GetResult vec size: %{public}zu over limit", size);
        return false;
    }
    for (size_t i = 0; i < size; i++) {
        DataProxyGetResult result;
        if (!UnmarshalDataProxyGetResultFromBuffer(iss, result)) {
            LOG_ERROR("Unmarshal DataProxyGetResult failed");
            return false;
        }
        results.push_back(std::move(result));
    }
    return iss.good();
}

bool UnmarshalDataProxyChangeInfoFromBuffer(std::istringstream &iss, DataProxyChangeInfo &info)
{
    // Read changeType_
    if (!UnmarshalBasicTypeToBuffer(iss, info.changeType_)) {
        LOG_ERROR("Unmarshal info.changeType_ failed");
        return false;
    }
    // Read uri_
    if (!UnmarshalStringToBuffer(iss, info.uri_)) {
        LOG_ERROR("Unmarshal info.uri_ failed");
        return false;
    }
    // Read value_
    if (!UnmarshalDataProxyValueFromBuffer(iss, info.value_)) {
        LOG_ERROR("Unmarshal info.value_ failed");
        return false;
    }
    return iss.good();
}

bool UnmarshalDataProxyChangeInfoVecToBuffer(std::istringstream &iss, std::vector<DataProxyChangeInfo> &changeInfos)
{
    size_t size;
    if (!UnmarshalBasicTypeToBuffer(iss, size)) {
        LOG_ERROR("Unmarshal vec size failed.");
        return false;
    }
    for (size_t i = 0; i < size; i++) {
        DataProxyChangeInfo changeInfo;
        if (!UnmarshalDataProxyChangeInfoFromBuffer(iss, changeInfo)) {
            LOG_ERROR("Unmarshal changeInfo failed");
            return false;
        }
        changeInfos.push_back(std::move(changeInfo));
    }
    return iss.good();
}

bool MarshalProxyDataVec(const std::vector<DataShareProxyData> &proxyDatas, MessageParcel &parcel)
{
    std::ostringstream oss;
    if (!MarshalProxyDataVecToBuffer(oss, proxyDatas)) {
        LOG_ERROR("MarshalProxyDataVecToBuffer failed.");
        return false;
    }
    std::string str = oss.str();
    size_t size = str.length();
    if (size > MAX_IPC_SIZE) {
        LOG_ERROR("Size: %{public}zu of ProxyDataVec is too large.", size);
        return false;
    }
    if (!parcel.WriteInt32(size)) {
        LOG_ERROR("Write size failed.");
        return false;
    }
    return parcel.WriteRawData(reinterpret_cast<const void *>(str.data()), size);
}

bool MarshalDataProxyGetResultVec(const std::vector<DataProxyGetResult> &results, MessageParcel &parcel)
{
    std::ostringstream oss;
    if (!MarshalDataProxyGetResultVecToBuffer(oss, results)) {
        LOG_ERROR("DataProxyGetResultVecToBuffer failed.");
        return false;
    }
    std::string str = oss.str();
    size_t size = str.length();
    if (size > MAX_IPC_SIZE) {
        LOG_ERROR("Size of DataProxyGetResultVec is too large.");
        return false;
    }
    if (!parcel.WriteInt32(size)) {
        LOG_ERROR("Write size failed.");
        return false;
    }
    return parcel.WriteRawData(reinterpret_cast<const void *>(str.data()), size);
}

bool MarshalDataProxyChangeInfoVec(const std::vector<DataProxyChangeInfo> &changeInfos, MessageParcel &parcel)
{
    std::ostringstream oss;
    if (!MarshalDataProxyChangeInfoVecToBuffer(oss, changeInfos)) {
        LOG_ERROR("MarshalDataProxyChangeInfoVec failed.");
        return false;
    }
    std::string str = oss.str();
    size_t size = str.length();
    if (size > MAX_IPC_SIZE) {
        LOG_ERROR("Size of DataProxyChangeInfoVec is too large.");
        return false;
    }
    if (!parcel.WriteInt32(size)) {
        LOG_ERROR("Write size failed.");
        return false;
    }
    return parcel.WriteRawData(reinterpret_cast<const void *>(str.data()), size);
}

bool UnmarshalProxyDataVec(std::vector<DataShareProxyData> &proxyDatas, MessageParcel &parcel)
{
    int32_t length = parcel.ReadInt32();
    if (length < 1) {
        LOG_ERROR("Length of ProxyDataVec is invalid: %{public}d", length);
        return false;
    }
    if (static_cast<size_t>(length) > MAX_IPC_SIZE) {
        LOG_ERROR("Length of ProxyDataVec exceed limit: %{public}d", length);
        return false;
    }
    const char *buffer = reinterpret_cast<const char *>(parcel.ReadRawData(static_cast<size_t>(length)));
    if (buffer == nullptr) {
        LOG_ERROR("ReadRawData failed.");
        return false;
    }
    std::istringstream iss(std::string(buffer, length));
    std::string issStr = iss.str();
    return UnmarshalProxyDataVecToBuffer(iss, proxyDatas);
}

bool UnmarshalDataProxyGetResultVec(std::vector<DataProxyGetResult> &results, MessageParcel &parcel)
{
    int32_t length = parcel.ReadInt32();
    if (length < 1) {
        LOG_ERROR("Length of ProxyDataVec is invalid: %{public}d", length);
        return false;
    }
    if (static_cast<size_t>(length) > MAX_IPC_SIZE) {
        LOG_ERROR("Length of ProxyDataVec exceed limit: %{public}d", length);
        return false;
    }
    const char *buffer = reinterpret_cast<const char *>(parcel.ReadRawData(static_cast<size_t>(length)));
    if (buffer == nullptr) {
        LOG_ERROR("ReadRawData failed.");
        return false;
    }
    std::istringstream iss(std::string(buffer, length));
    return UnmarshalDataProxyGetResultVecToBuffer(iss, results);
}

bool UnmarshalDataProxyChangeInfoVec(std::vector<DataProxyChangeInfo> &changeInfos, MessageParcel &parcel)
{
    int32_t length = parcel.ReadInt32();
    if (length < 1) {
        LOG_ERROR("Length of DataProxyChangeInfo is invalid: %{public}d", length);
        return false;
    }
    if (static_cast<size_t>(length) > MAX_IPC_SIZE) {
        LOG_ERROR("Length of DataProxyChangeInfo exceed limit: %{public}d", length);
        return false;
    }
    const char *buffer = reinterpret_cast<const char *>(parcel.ReadRawData(static_cast<size_t>(length)));
    if (buffer == nullptr) {
        LOG_ERROR("ReadRawData failed.");
        return false;
    }
    std::istringstream iss(std::string(buffer, length));
    return UnmarshalDataProxyChangeInfoVecToBuffer(iss, changeInfos);
}

template<>
bool Marshalling(const SubscribeOption &subscribeOption, MessageParcel &parcel)
{
    return ITypesUtil::Marshal(parcel, subscribeOption.subscribeStatus);
}

template<>
bool Unmarshalling(SubscribeOption &subscribeOption, MessageParcel &parcel)
{
    return ITypesUtil::Unmarshal(parcel, subscribeOption.subscribeStatus);
}
}  // namespace OHOS::ITypesUtil