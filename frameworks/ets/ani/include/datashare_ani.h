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

#ifndef DATASHARE_ANI_RESULT_SET_H
#define DATASHARE_ANI_RESULT_SET_H

#include "datashare_shared_result_set.h"
#include "ani_observer.h"
#include "ani_subscriber_manager.h"
#include "datashare_helper.h"
#include "dataproxy_handle.h"
#include "cxx.h"
namespace OHOS {
using namespace DataShare;

namespace DataShareAni {

struct PtrWrap;
struct VersionWrap;
struct I32ResultWrap;
struct StringResultWrap;
struct I64ResultWrap;
struct ValuesBucketKvItem;
struct ValueType;
struct PublishedItem;
struct PublishSretParam;
struct GetPublishedDataSretParam;
struct Template;
struct TemplatePredicatesKvItem;
struct ValuesBucketWrap;
struct ChangeInfo;
struct TemplateId;
struct RdbDataChangeNode;
struct PublishedDataChangeNode;
struct DataShareCallback;
struct AniProxyData;
struct AniDataProxyConfig;
struct AniDataProxyResultSetParam;
struct AniDataProxyGetResultSetParam;
struct AniDataProxyResult;
struct DataShareBatchUpdateParamIn;
struct DataShareBatchUpdateParamOut;
struct ExtensionBatchUpdateParamIn;
struct ExtensionBatchUpdateParamOut;

const int E_OK = 0;
const int EXCEPTION_SYSTEMAPP_CHECK = 202;
const int EXCEPTION_PARAMETER_CHECK = 401;
const int EXCEPTION_INNER = 15700000;
const int EXCEPTION_HELPER_UNINITIALIZED = 15700010;
const int EXCEPTION_URI_NOT_EXIST = 15700011;
const int EXCEPTION_DATA_AREA_NOT_EXIST = 15700012;
const int EXCEPTION_HELPER_CLOSED = 15700013;
const int EXCEPTION_PROXY_PARAMETER_CHECK = 15700014;
class SharedPtrHolder {
public:
    SharedPtrHolder(const std::shared_ptr<DataShareHelper> &datashareHelper) : datashareHelper_(datashareHelper)
    {
    }

public:
    std::shared_ptr<DataShareHelper> datashareHelper_ = nullptr;
    std::shared_ptr<AniRdbSubscriberManager> jsRdbObsManager_ = nullptr;
    std::shared_ptr<AniPublishedSubscriberManager> jsPublishedObsManager_ = nullptr;
};

class DataProxyHandleHolder {
public:
    DataProxyHandleHolder(const std::shared_ptr<DataProxyHandle> &dataProxyHandle) : dataProxyHandle_(dataProxyHandle)
    {
    }

public:
    std::shared_ptr<DataProxyHandle> dataProxyHandle_ = nullptr;
    std::shared_ptr<AniProxyDataSubscriberManager> jsProxyDataObsManager_ = nullptr;
};

class ResultSetHolder {
public:
    ResultSetHolder(const std::shared_ptr<DataShareResultSet> &resultSetPtr) : resultSetPtr_(resultSetPtr)
    {
    }

public:
    std::shared_ptr<DataShareResultSet> resultSetPtr_ = nullptr;
};

rust::Vec<rust::string> GetColumnNames(int64_t resultSetPtr);
int32_t GetColumnCount(int64_t resultSetPtr);
int32_t GetRowCount(int64_t resultSetPtr);
bool GetIsClosed(int64_t resultSetPtr);
bool GoToFirstRow(int64_t resultSetPtr);
bool GoToLastRow(int64_t resultSetPtr);
bool GoToNextRow(int64_t resultSetPtr);
bool GoToPreviousRow(int64_t resultSetPtr);
bool GoTo(int64_t resultSetPtr, int32_t offset);
bool GoToRow(int64_t resultSetPtr, int32_t position);
rust::Vec<uint8_t> GetBlob(int64_t resultSetPtr, int32_t columnIndex);
rust::String GetString(int64_t resultSetPtr, int columnIndex);
int64_t GetLong(int64_t resultSetPtr, int columnIndex);
double GetDouble(int64_t resultSetPtr, int columnIndex);
void Close(int64_t resultSetPtr);
int GetColumnIndex(int64_t resultSetPtr, rust::String columnName);
rust::String GetColumnName(int64_t resultSetPtr, int columnIndex);
int32_t GetDataType(int64_t resultSetPtr, int columnIndex);

int64_t DataSharePredicatesNew();
void DataSharePredicatesClean(int64_t predicatesPtr);
void DataSharePredicatesEqualTo(int64_t predicatesPtr, rust::String field, const ValueType& value);
void DataSharePredicatesNotEqualTo(int64_t predicatesPtr, rust::String field, const ValueType& value);
void DataSharePredicatesBeginWrap(int64_t predicatesPtr);
void DataSharePredicatesEndWrap(int64_t predicatesPtr);
void DataSharePredicatesOr(int64_t predicatesPtr);
void DataSharePredicatesAnd(int64_t predicatesPtr);
void DataSharePredicatesContains(int64_t predicatesPtr, rust::String field, rust::String value);
void DataSharePredicatesBeginsWith(int64_t predicatesPtr, rust::String field, rust::String value);
void DataSharePredicatesEndsWith(int64_t predicatesPtr, rust::String field, rust::String value);
void DataSharePredicatesIsNull(int64_t predicatesPtr, rust::String field);
void DataSharePredicatesIsNotNull(int64_t predicatesPtr, rust::String field);
void DataSharePredicatesLike(int64_t predicatesPtr, rust::String field, rust::String value);
void DataSharePredicatesUnlike(int64_t predicatesPtr, rust::String field, rust::String value);
void DataSharePredicatesGlob(int64_t predicatesPtr, rust::String field, rust::String value);
void DataSharePredicatesBetween(int64_t predicatesPtr, rust::String field, const ValueType& low, const ValueType& high);
void DataSharePredicatesNotBetween(int64_t predicatesPtr, rust::String field,
    const ValueType& low, const ValueType& high);
void DataSharePredicatesGreaterThan(int64_t predicatesPtr, rust::String field, const ValueType& value);
void DataSharePredicatesGreaterThanOrEqualTo(int64_t predicatesPtr, rust::String field, const ValueType& value);
void DataSharePredicatesLessThanOrEqualTo(int64_t predicatesPtr, rust::String field, const ValueType& value);
void DataSharePredicatesLessThan(int64_t predicatesPtr, rust::String field, const ValueType& value);
void DataSharePredicatesOrderByAsc(int64_t predicatesPtr, rust::String field);
void DataSharePredicatesOrderByDesc(int64_t predicatesPtr, rust::String field);
void DataSharePredicatesDistinct(int64_t predicatesPtr);
void DataSharePredicatesLimit(int64_t predicatesPtr, int total, int offset);
void DataSharePredicatesGroupBy(int64_t predicatesPtr, rust::Vec<rust::String> field);
void DataSharePredicatesIndexedBy(int64_t predicatesPtr, rust::String field);
void DataSharePredicatesIn(int64_t predicatesPtr, rust::String field, rust::Vec<ValueType> value);
void DataSharePredicatesNotIn(int64_t predicatesPtr, rust::String field, rust::Vec<ValueType> value);
void DataSharePredicatesPrefixKey(int64_t predicatesPtr, rust::String prefix);
void DataSharePredicatesInKeys(int64_t predicatesPtr, rust::Vec<rust::String> keys);

I64ResultWrap DataShareNativeCreate(int64_t context, rust::String strUri,
    bool optionIsUndefined, bool isProxy, int waitTime);

void DataShareNativeClean(int64_t dataShareHelperPtr);

int DataShareNativeEnableSilentProxy(int64_t context, rust::String strUri);
int DataShareNativeDisableSilentProxy(int64_t context, rust::String strUri);

I64ResultWrap DataShareNativeQuery(int64_t dataShareHelperPtr, rust::String strUri,
    int64_t dataSharePredicatesPtr, rust::Vec<rust::String> columns);

I32ResultWrap DataShareNativeUpdate(int64_t dataShareHelperPtr, rust::String strUri,
    int64_t dataSharePredicatesPtr, rust::Vec<ValuesBucketKvItem> bucket);

int DataShareNativePublish(int64_t dataShareHelperPtr, rust::Vec<PublishedItem> data,
    rust::String bundleName, VersionWrap version, PublishSretParam& sret);

int DataShareNativeGetPublishedData(int64_t dataShareHelperPtr, rust::String bundleName,
    GetPublishedDataSretParam& sret);

int DataShareNativeAddTemplate(int64_t dataShareHelperPtr, rust::String uri,
    rust::String subscriberId, const Template& temp);

int DataShareNativeDelTemplate(int64_t dataShareHelperPtr, rust::String uri, rust::String subscriberId);

I32ResultWrap DataShareNativeInsert(int64_t dataShareHelperPtr, rust::String strUri,
    rust::Vec<ValuesBucketKvItem> bucket);

I32ResultWrap DataShareNativeBatchInsert(int64_t dataShareHelperPtr, rust::String strUri,
    rust::Vec<ValuesBucketWrap> buckets);

void DataShareNativeBatchUpdate(int64_t dataShareHelperPtr, const DataShareBatchUpdateParamIn& param_in,
    DataShareBatchUpdateParamOut& param_out);

StringResultWrap DataShareNativeNormalizeUri(int64_t dataShareHelperPtr, rust::String strUri);

StringResultWrap DataShareNativeDeNormalizeUri(int64_t dataShareHelperPtr, rust::String strUri);

int DataShareNativeNotifyChange(int64_t dataShareHelperPtr, rust::String strUri);

int DataShareNativeNotifyChangeInfo(int64_t dataShareHelperPtr, int32_t changeType,
    rust::String strUri, rust::Vec<ValuesBucketWrap> buckets);

I32ResultWrap DataShareNativeDelete(int64_t dataShareHelperPtr, rust::String strUri, int64_t dataSharePredicatesPtr);

int DataShareNativeClose(int64_t dataShareHelperPtr);

int DataShareNativeOn(PtrWrap ptrWrap, rust::String strUri);

int DataShareNativeOnChangeinfo(PtrWrap ptrWrap, int32_t arktype, rust::String strUri);

int DataShareNativeOnRdbDataChange(PtrWrap ptrWrap, rust::Vec<rust::String> uris, const TemplateId& templateId,
    PublishSretParam& sret);

int DataShareNativeOnPublishedDataChange(PtrWrap ptrWrap, rust::Vec<rust::String> uris, rust::String subscriberId,
    PublishSretParam& sret);

int DataShareNativeOff(PtrWrap ptrWrap, rust::String strUri);

int DataShareNativeOffNone(int64_t dataShareHelperPtr, rust::String strUri);

int DataShareNativeOffChangeinfo(PtrWrap ptrWrap, int32_t arktype, rust::String strUri);

int DataShareNativeOffChangeinfoNone(int64_t dataShareHelperPtr, int32_t arktype, rust::String strUri);

int DataShareNativeOffRdbDataChange(PtrWrap ptrWrap, rust::Vec<rust::String> uris, const TemplateId& templateId,
    PublishSretParam& sret);

int DataShareNativeOffRdbDataChangeNone(int64_t dataShareHelperPtr, rust::Vec<rust::String> uris,
    const TemplateId& templateId, PublishSretParam& sret);
                                          
int DataShareNativeOffPublishedDataChange(PtrWrap PtrWrap, rust::Vec<rust::String> uris,
    rust::String subscriberId, PublishSretParam& sret);

int DataShareNativeOffPublishedDataChangeNone(int64_t dataShareHelperPtr, rust::Vec<rust::String> uris,
    rust::String subscriberId, PublishSretParam& sret);

int ANIRegisterObserver(const std::string &uri, long long dataShareHelperPtr, rust::Box<DataShareCallback> &callback,
    bool isNotifyDetails = false);

int ANIUnRegisterObserver(const std::string &uri, long long dataShareHelperPtr, bool isNotifyDetails = false);

int ANIUnRegisterObserver(const std::string &uri, long long dataShareHelperPtr,
    rust::Box<DataShareCallback> &callback, bool isNotifyDetails = false);

void DataShareNativeExtensionCallbackInt(double errorCode, rust::string errorMsg, int32_t data, int64_t nativePtr);

void DataShareNativeExtensionCallbackObject(double errorCode, rust::string errorMsg, int64_t ptr, int64_t nativePtr);

void DataShareNativeExtensionCallbackVoid(double errorCode, rust::string errorMsg, int64_t nativePtr);

void DataShareNativeExtensionCallbackString(double errorCode, rust::String errorMsg,
    rust::String value, int64_t nativePtr);

void DataShareNativeExtensionCallbackBatchUpdate(double errorCode, rust::String errorMsg,
    const ExtensionBatchUpdateParamIn& param_in, int64_t nativePtr);

int ValidateUrisForDataProxy(rust::Vec<rust::String> uris);

int ValidateDataShareNativePublishParameters(rust::Vec<AniProxyData> proxydata);

I64ResultWrap DataProxyHandleNativeCreate();

void CleanupDataProxyHandle(int64_t dataProxyHandlePtr);

int DataShareNativeDataProxyHandleOnDataProxy(
    PtrWrap ptrWrap, rust::Vec<rust::String> uris, AniDataProxyResultSetParam& param);

int DataShareNativeDataProxyHandleOffDataProxy(
    PtrWrap ptrWrap, rust::Vec<rust::String> uris, AniDataProxyResultSetParam& param);

int DataShareNativeDataProxyHandleOffDataProxyNone(
    int64_t dataShareProxyHandlePtr, rust::Vec<rust::String> uris, AniDataProxyResultSetParam& param);

int DataShareNativeDataProxyHandlePublish(int64_t dataShareProxyHandlePtr, rust::Vec<AniProxyData> proxydata,
    const AniDataProxyConfig& config, AniDataProxyResultSetParam& param);

int DataShareNativeDataProxyHandleDelete(int64_t dataShareProxyHandlePtr, rust::Vec<rust::String> uris,
    const AniDataProxyConfig& config, AniDataProxyResultSetParam& param);

int DataShareNativeDataProxyHandleGet(int64_t dataShareProxyHandlePtr, rust::Vec<rust::String> uris,
    const AniDataProxyConfig& config, AniDataProxyGetResultSetParam& param);
} // namespace DataShareAni
} // namespace OHOS

#endif
