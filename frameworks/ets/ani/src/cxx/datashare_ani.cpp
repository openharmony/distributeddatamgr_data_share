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
bool GoToFirstRow(long long resultSetPtr)
{
    return true;
}

bool GoToLastRow(long long resultSetPtr)
{
    return true;
}

bool GoToNextRow(long long resultSetPtr)
{
    return true;
}

rust::String GetString(long long resultSetPtr, int columnIndex)
{
    return rust::String("hello world");
}

long long GetLong(long long resultSetPtr, int columnIndex)
{
    return UNIMPL_RET_CODE;
}

void Close(long long resultSetPtr)
{
}

int GetColumnIndex(long long resultSetPtr, rust::String columnName)
{
    return UNIMPL_RET_CODE;
}

// @ohos.data.dataSharePredicates.d.ets
long long DataSharePredicatesNew()
{
    return UNIMPL_RET_CODE;
}

void DataSharePredicatesClean(long long predicatesPtr)
{
}

void DataSharePredicatesEqualTo(long long predicatesPtr, rust::String field, const ValueType& value)
{
}

void DataSharePredicatesNotEqualTo(long long predicatesPtr, rust::String field, const ValueType& value)
{
}

void DataSharePredicatesBeginWrap(long long predicatesPtr)
{
}

void DataSharePredicatesEndWrap(long long predicatesPtr)
{
}

void DataSharePredicatesOr(long long predicatesPtr)
{
}

void DataSharePredicatesAnd(long long predicatesPtr)
{
}

void DataSharePredicatesContains(long long predicatesPtr, rust::String field, rust::String value)
{
}

void DataSharePredicatesIsNull(long long predicatesPtr, rust::String field)
{
}

void DataSharePredicatesIsNotNull(long long predicatesPtr, rust::String field)
{
}

void DataSharePredicatesLike(long long predicatesPtr, rust::String field, rust::String value)
{
}

void DataSharePredicatesBetween(long long predicatesPtr, rust::String field,
                                const ValueType& low, const ValueType& high)
{
}

void DataSharePredicatesGreaterThan(long long predicatesPtr, rust::String field, const ValueType& value)
{
}

void DataSharePredicatesGreaterThanOrEqualTo(long long predicatesPtr, rust::String field, const ValueType& value)
{
}

void DataSharePredicatesLessThanOrEqualTo(long long predicatesPtr, rust::String field, const ValueType& value)
{
}

void DataSharePredicatesLessThan(long long predicatesPtr, rust::String field, const ValueType& value)
{
}

void DataSharePredicatesOrderByAsc(long long predicatesPtr, rust::String field)
{
}

void DataSharePredicatesOrderByDesc(long long predicatesPtr, rust::String field)
{
}

void DataSharePredicatesLimit(long long predicatesPtr, double total, double offset)
{
}

void DataSharePredicatesGroupBy(long long predicatesPtr, rust::Vec<rust::String> field)
{
}

void DataSharePredicatesIn(long long predicatesPtr, rust::String field,  rust::Vec<ValueType> value)
{
}

void DataSharePredicatesNotIn(long long predicatesPtr, rust::String field,  rust::Vec<ValueType> value)
{
}

// @ohos.data.dataShare.d.ets
long long DataShareNativeCreate(long long context, rust::String strUri,
                                bool optionIsUndefined, bool isProxy)
{
    return UNIMPL_RET_CODE;
}

void DataShareNativeClean(long long dataShareHelperPtr)
{
}

long long DataShareNativeQuery(long long dataShareHelperPtr, rust::String strUri,
                               long long dataSharePredicatesPtr, rust::Vec<rust::String> columns)
{
    return UNIMPL_RET_CODE;
}

int DataShareNativeUpdate(long long dataShareHelperPtr, rust::String strUri,
                          long long dataSharePredicatesPtr, rust::Vec<ValuesBucketKvItem> bucket)
{
    return UNIMPL_RET_CODE;
}

void DataShareNativePublish(long long dataShareHelperPtr, rust::Vec<PublishedItem> data,
                            rust::String bundleName, VersionWrap version, PublishSretParam& sret)
{
}

void DataShareNativeGetPublishedData(long long dataShareHelperPtr, rust::String bundleName,
                                     GetPublishedDataSretParam& sret)
{
}

void DataShareNativeAddTemplate(long long dataShareHelperPtr, rust::String uri,
                                rust::String subscriberId, const Template& temp)
{
}

void DataShareNativeDelTemplate(long long dataShareHelperPtr, rust::String uri, rust::String subscriberId)
{
}

int DataShareNativeInsert(long long dataShareHelperPtr, rust::String strUri,
                          rust::Vec<ValuesBucketKvItem> bucket)
{
    return UNIMPL_RET_CODE;
}

int DataShareNativeBatchInsert(long long dataShareHelperPtr, rust::String strUri,
                               rust::Vec<ValuesBucketWrap> buckets)
{
    return UNIMPL_RET_CODE;
}

int DataShareNativeDelete(long long dataShareHelperPtr, rust::String strUri, long long dataSharePredicatesPtr)
{
    return UNIMPL_RET_CODE;
}

void DataShareNativeClose(long long dataShareHelperPtr)
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
