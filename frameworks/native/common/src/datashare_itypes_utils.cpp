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
#include "datashare_log.h"

namespace OHOS::ITypesUtil {
using namespace OHOS::DataShare;
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
    return ITypesUtil::Marshal(parcel, changeNode.uri_, changeNode.templateId_, changeNode.data_);
}

template<>
bool Unmarshalling(RdbChangeNode &changeNode, MessageParcel &parcel)
{
    return ITypesUtil::Unmarshal(parcel, changeNode.uri_, changeNode.templateId_, changeNode.data_);
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
} // namespace OHOS::ITypesUtil
