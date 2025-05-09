/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include <iostream>
#include "datashare_ani.h"
#include "wrapper.rs.h"

#define UNIMPL_RET_CODE 0

namespace OHOS {
using namespace DataShare;

namespace DataShareAni {

// @ohos.data.DataShareResultSet.d.ets
bool GoToFirstRow(int64_t resultSetPtr)
{
    return true;
}

bool GoToLastRow(int64_t resultSetPtr)
{
    return true;
}

bool GoToNextRow(int64_t resultSetPtr)
{
    return true;
}

rust::String GetString(int64_t resultSetPtr, int columnIndex)
{
    return rust::String("hello world");
}

int64_t GetLong(int64_t resultSetPtr, int columnIndex)
{
    return UNIMPL_RET_CODE;
}

void Close(int64_t resultSetPtr)
{
}

int GetColumnIndex(int64_t resultSetPtr, rust::String columnName)
{
    return UNIMPL_RET_CODE;
}

// @ohos.data.dataSharePredicates.d.ets
int64_t DataSharePredicatesNew()
{
    return UNIMPL_RET_CODE;
}

void DataSharePredicatesClean(int64_t predicatesPtr)
{
}

void DataSharePredicatesEqualTo(int64_t predicatesPtr, rust::String field, const ValueType& value)
{
}

void DataSharePredicatesNotEqualTo(int64_t predicatesPtr, rust::String field, const ValueType& value)
{
}

void DataSharePredicatesBeginWrap(int64_t predicatesPtr)
{
}

void DataSharePredicatesEndWrap(int64_t predicatesPtr)
{
}

void DataSharePredicatesOr(int64_t predicatesPtr)
{
}

void DataSharePredicatesAnd(int64_t predicatesPtr)
{
}

void DataSharePredicatesContains(int64_t predicatesPtr, rust::String field, rust::String value)
{
}

void DataSharePredicatesIsNull(int64_t predicatesPtr, rust::String field)
{
}

void DataSharePredicatesIsNotNull(int64_t predicatesPtr, rust::String field)
{
}

void DataSharePredicatesLike(int64_t predicatesPtr, rust::String field, rust::String value)
{
}

void DataSharePredicatesBetween(int64_t predicatesPtr, rust::String field,
                                const ValueType& low, const ValueType& high)
{
}

void DataSharePredicatesGreaterThan(int64_t predicatesPtr, rust::String field, const ValueType& value)
{
}

void DataSharePredicatesGreaterThanOrEqualTo(int64_t predicatesPtr, rust::String field, const ValueType& value)
{
}

void DataSharePredicatesLessThanOrEqualTo(int64_t predicatesPtr, rust::String field, const ValueType& value)
{
}

void DataSharePredicatesLessThan(int64_t predicatesPtr, rust::String field, const ValueType& value)
{
}

void DataSharePredicatesOrderByAsc(int64_t predicatesPtr, rust::String field)
{
}

void DataSharePredicatesOrderByDesc(int64_t predicatesPtr, rust::String field)
{
}

void DataSharePredicatesLimit(int64_t predicatesPtr, double total, double offset)
{
}

void DataSharePredicatesGroupBy(int64_t predicatesPtr, rust::Vec<rust::String> field)
{
}

void DataSharePredicatesIn(int64_t predicatesPtr, rust::String field,  rust::Vec<ValueType> value)
{
}

void DataSharePredicatesNotIn(int64_t predicatesPtr, rust::String field,  rust::Vec<ValueType> value)
{
}

// @ohos.data.dataShare.d.ets
int64_t DataShareNativeCreate(int64_t context, rust::String strUri,
                                bool optionIsUndefined, bool isProxy)
{
    return UNIMPL_RET_CODE;
}

void DataShareNativeClean(int64_t dataShareHelperPtr)
{
}

int64_t DataShareNativeQuery(int64_t dataShareHelperPtr, rust::String strUri,
                               int64_t dataSharePredicatesPtr, rust::Vec<rust::String> columns)
{
    return UNIMPL_RET_CODE;
}

int DataShareNativeUpdate(int64_t dataShareHelperPtr, rust::String strUri,
                          int64_t dataSharePredicatesPtr, rust::Vec<ValuesBucketKvItem> bucket)
{
    return UNIMPL_RET_CODE;
}

void DataShareNativePublish(int64_t dataShareHelperPtr, rust::Vec<PublishedItem> data,
                            rust::String bundleName, VersionWrap version, PublishSretParam& sret)
{
}

void DataShareNativeGetPublishedData(int64_t dataShareHelperPtr, rust::String bundleName,
                                     GetPublishedDataSretParam& sret)
{
}

void DataShareNativeAddTemplate(int64_t dataShareHelperPtr, rust::String uri,
                                rust::String subscriberId, const Template& temp)
{
}

void DataShareNativeDelTemplate(int64_t dataShareHelperPtr, rust::String uri, rust::String subscriberId)
{
}

int DataShareNativeInsert(int64_t dataShareHelperPtr, rust::String strUri,
                          rust::Vec<ValuesBucketKvItem> bucket)
{
    return UNIMPL_RET_CODE;
}

int DataShareNativeBatchInsert(int64_t dataShareHelperPtr, rust::String strUri,
                               rust::Vec<ValuesBucketWrap> buckets)
{
    return UNIMPL_RET_CODE;
}

int DataShareNativeDelete(int64_t dataShareHelperPtr, rust::String strUri, int64_t dataSharePredicatesPtr)
{
    return UNIMPL_RET_CODE;
}

void DataShareNativeClose(int64_t dataShareHelperPtr)
{
}

void DataShareNativeOn(EnvPtrWrap envPtrWrap, rust::String strType, rust::String strUri)
{
}

void DataShareNativeOnChangeinfo(EnvPtrWrap envPtrWrap, rust::String event,
                                 int32_t arktype, rust::String strUri)
{
}

void DataShareNativeOnRdbDataChange(EnvPtrWrap envPtrWrap, rust::String arktype,
                                    rust::Vec<rust::String> uris, const TemplateId& templateId,
                                    PublishSretParam& sret)
{
}

void DataShareNativeOnPublishedDataChange(EnvPtrWrap envPtrWrap, rust::String arktype,
                                          rust::Vec<rust::String> uris, rust::String subscriberId,
                                          PublishSretParam& sret)
{
}

void DataShareNativeOff(EnvPtrWrap envPtrWrap, rust::String strType, rust::String strUri)
{
}

void DataShareNativeOffChangeinfo(EnvPtrWrap envPtrWrap, rust::String event,
                                  int32_t arktype, rust::String strUri)
{
}

void DataShareNativeOffRdbDataChange(EnvPtrWrap envPtrWrap, rust::String arktype, rust::Vec<rust::String> uris,
                                     const TemplateId& templateId, PublishSretParam& sret)
{
}

void DataShareNativeOffPublishedDataChange(EnvPtrWrap envPtrWrap, rust::String arktype,
                                           rust::Vec<rust::String> uris, rust::String subscriberId,
                                           PublishSretParam& sret)
{
}

} // namespace DataShareAni
} // namespace OHOS
