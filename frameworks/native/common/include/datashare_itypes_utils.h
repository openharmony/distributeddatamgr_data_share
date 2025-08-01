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

#include "dataproxy_handle_common.h"
#include "datashare_operation_statement.h"
#include "datashare_common.h"
#include "datashare_predicates.h"
#include "datashare_template.h"
#include "datashare_value_object.h"
#include "datashare_values_bucket.h"
#include "itypes_util.h"
#include "uri.h"

namespace OHOS::ITypesUtil {
using Predicates = DataShare::DataSharePredicates;
using Operation = DataShare::OperationItem;
using PublishedDataItem = DataShare::PublishedDataItem;
using Data = DataShare::Data;
using TemplateId = DataShare::TemplateId;
using PredicateTemplateNode = DataShare::PredicateTemplateNode;
using RdbChangeNode = DataShare::RdbChangeNode;
using PublishedDataChangeNode = DataShare::PublishedDataChangeNode;
using OperationResult = DataShare::OperationResult;
using DataShareValuesBucket = DataShare::DataShareValuesBucket;
using AshmemNode = DataShare::AshmemNode;
using OperationStatement = DataShare::OperationStatement;
using ExecResult = DataShare::ExecResult;
using ExecResultSet = DataShare::ExecResultSet;
using UpdateOperation = DataShare::UpdateOperation;
using BatchUpdateResult = DataShare::BatchUpdateResult;
using DataShareProxyData = DataShare::DataShareProxyData;
using DataProxyConfig = DataShare::DataProxyConfig;
using DataProxyType = DataShare::DataProxyType;
using DataProxyResult = DataShare::DataProxyResult;
using DataProxyErrorCode = DataShare::DataProxyErrorCode;
using DataShareValueObject = DataShare::DataShareValueObject;
using DataProxyValue = DataShare::DataProxyValue;
using DataProxyGetResult = DataShare::DataProxyGetResult;
using DataProxyChangeInfo = DataShare::DataProxyChangeInfo;
using DataShareParamSet = DataShare::DataShareParamSet;
using RegisterOption = DataShare::RegisterOption;

template<>
bool Marshalling(const BatchUpdateResult &result, MessageParcel &parcel);

template<>
bool Unmarshalling(BatchUpdateResult &result, MessageParcel &parcel);

template<>
bool Marshalling(const UpdateOperation &operation, MessageParcel &parcel);

template<>
bool Unmarshalling(UpdateOperation &operation, MessageParcel &parcel);

template<>
bool Marshalling(const Predicates &bucket, MessageParcel &parcel);

template<>
bool Unmarshalling(Predicates &predicates, MessageParcel &parcel);

template<>
bool Marshalling(const Operation &operation, MessageParcel &parcel);

template<>
bool Unmarshalling(Operation &operation, MessageParcel &parcel);

template<>
bool Unmarshalling(PublishedDataItem &dataItem, MessageParcel &parcel);

template<>
bool Unmarshalling(PublishedDataItem &dataItem, MessageParcel &parcel);

template<>
bool Marshalling(const PublishedDataItem &templateId, MessageParcel &parcel);

template<>
bool Marshalling(const Data &data, MessageParcel &parcel);

template<>
bool Unmarshalling(Data &data, MessageParcel &parcel);

template<>
bool Marshalling(const DataShareValuesBucket &bucket, MessageParcel &parcel);

template<>
bool Unmarshalling(DataShareValuesBucket &bucket, MessageParcel &parcel);

template<>
bool Marshalling(const AshmemNode &node, MessageParcel &parcel);

template<>
bool Unmarshalling(AshmemNode &node, MessageParcel &parcel);

template<>
bool Marshalling(const Uri &node, MessageParcel &parcel);

template<>
bool Unmarshalling(Uri &node, MessageParcel &parcel);

template<>
bool Unmarshalling(TemplateId &templateId, MessageParcel &parcel);

template<>
bool Marshalling(const PredicateTemplateNode &predicateTemplateNode, MessageParcel &parcel);

template<>
bool Unmarshalling(PredicateTemplateNode &predicateTemplateNode, MessageParcel &parcel);

template<>
bool Marshalling(const RdbChangeNode &changeNode, MessageParcel &parcel);

template<>
bool Unmarshalling(RdbChangeNode &changeNode, MessageParcel &parcel);

template<>
bool Marshalling(const PublishedDataChangeNode &changeNode, MessageParcel &parcel);

template<>
bool Unmarshalling(PublishedDataChangeNode &changeNode, MessageParcel &parcel);

template<>
bool Marshalling(const OperationResult &operationResult, MessageParcel &parcel);

template<>
bool Unmarshalling(OperationResult &predicateTemplateNode, MessageParcel &parcel);

template<>
bool Marshalling(const TemplateId &changeNode, MessageParcel &parcel);

template<>
bool Marshalling(const OperationStatement &operationStatement, MessageParcel &parcel);

template<>
bool Unmarshalling(OperationStatement &operationStatement, MessageParcel &parcel);

template<>
bool Marshalling(const ExecResult &execResult, MessageParcel &parcel);

template<>
bool Unmarshalling(ExecResult &execResult, MessageParcel &parcel);

template<>
bool Marshalling(const ExecResultSet &execResultSet, MessageParcel &parcel);

template<>
bool Unmarshalling(ExecResultSet &execResultSet, MessageParcel &parcel);

/**
 * @brief The following four functions are used for serializing and deserializing objects
 * to and from shared memory during Query and BatchInsert operations,
 * which has a 128M upper limit. The upper limit of other method is 200k.
 * Other methods remain unchanged.
 */
bool MarshalPredicates(const Predicates &predicates, MessageParcel &parcel);

bool UnmarshalPredicates(Predicates &predicates, MessageParcel &parcel);

bool MarshalValuesBucketVec(const std::vector<DataShareValuesBucket> &values, MessageParcel &parcel);

bool UnmarshalValuesBucketVec(std::vector<DataShareValuesBucket> &values, MessageParcel &parcel);

template<>
bool Marshalling(const DataShareProxyData &proxyData, MessageParcel &parcel);

template<>
bool Unmarshalling(DataShareProxyData &proxyData, MessageParcel &parcel);

template<>
bool Marshalling(const DataProxyConfig &proxyData, MessageParcel &parcel);

template<>
bool Unmarshalling(DataProxyConfig &proxyData, MessageParcel &parcel);

template<>
bool Marshalling(const DataProxyResult &result, MessageParcel &parcel);

template<>
bool Unmarshalling(DataProxyResult &result, MessageParcel &parcel);

template<>
bool Marshalling(const DataShareValueObject &result, MessageParcel &parcel);

template<>
bool Unmarshalling(DataShareValueObject &result, MessageParcel &parcel);

template<>
bool Marshalling(const DataProxyValue &result, MessageParcel &parcel);

template<>
bool Unmarshalling(DataProxyValue &result, MessageParcel &parcel);

template<>
bool Marshalling(const DataProxyGetResult &result, MessageParcel &parcel);

template<>
bool Unmarshalling(DataProxyGetResult &result, MessageParcel &parcel);

template<>
bool Marshalling(const DataProxyChangeInfo &result, MessageParcel &parcel);

template<>
bool Unmarshalling(DataProxyChangeInfo &result, MessageParcel &parcel);

template<>
bool Marshalling(const DataShareParamSet &result, MessageParcel &parcel);

template<>
bool Unmarshalling(DataShareParamSet &result, MessageParcel &parcel);

template<>
bool Marshalling(const RegisterOption &option, MessageParcel &parcel);

template<>
bool Unmarshalling(RegisterOption &option, MessageParcel &parcel);
}
#endif
