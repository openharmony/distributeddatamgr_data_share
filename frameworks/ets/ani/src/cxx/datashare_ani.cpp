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

#include "ani_base_context.h"
#include "datashare_ani.h"
#include "datashare_business_error.h"
#include "datashare_log.h"
#include "datashare_predicates.h"
#include "datashare_value_object.h"
#include "datashare_values_bucket.h"
#include "wrapper.rs.h"

#define UNIMPL_RET_CODE 0

namespace OHOS {
using namespace DataShare;

namespace DataShareAni {

std::mutex listMutex_{};

std::vector<std::string> convert_rust_vec_to_cpp_vector(const rust::Vec<rust::String>& rust_vec)
{
    std::vector<std::string> cpp_vector;
    for (const auto& rust_str : rust_vec) {
        cpp_vector.push_back(std::string(rust_str));
    }
    return cpp_vector;
}

std::vector<uint8_t> convert_rust_vec_to_cpp_vector(const rust::Vec<uint8_t>& rust_vec)
{
    std::vector<uint8_t> cpp_vector;
    for (const auto& rust_data : rust_vec) {
        cpp_vector.push_back(rust_data);
    }
    return cpp_vector;
}

// @ohos.data.DataShareResultSet.d.ets
bool GoToFirstRow(int64_t resultSetPtr)
{
    return reinterpret_cast<DataShareResultSet*>(resultSetPtr)->GoToFirstRow();
}

bool GoToLastRow(int64_t resultSetPtr)
{
    return reinterpret_cast<DataShareResultSet*>(resultSetPtr)->GoToLastRow();
}

bool GoToNextRow(int64_t resultSetPtr)
{
    return reinterpret_cast<DataShareResultSet*>(resultSetPtr)->GoToNextRow();
}

rust::String GetString(int64_t resultSetPtr, int columnIndex)
{
    std::string strValue;
    reinterpret_cast<DataShareResultSet*>(resultSetPtr)->GetString(columnIndex, strValue);
    return rust::String(strValue);
}

int64_t GetLong(int64_t resultSetPtr, int columnIndex)
{
    int64_t value = -1;
    int errorCode = reinterpret_cast<DataShareResultSet*>(resultSetPtr)->GetLong(columnIndex, value);
    if (errorCode != E_OK) {
        LOG_ERROR("failed code:%{public}d", errorCode);
    }
    return value;
}

void Close(int64_t resultSetPtr)
{
    reinterpret_cast<DataShareResultSet*>(resultSetPtr)->Close();
}

int GetColumnIndex(int64_t resultSetPtr, rust::String columnName)
{
    std::string name = std::string(columnName);
    int32_t columnIndex = -1;
    int errorCode = reinterpret_cast<DataShareResultSet*>(resultSetPtr)->GetColumnIndex(name, columnIndex);
    if (errorCode != E_OK) {
        LOG_ERROR("failed code:%{public}d columnIndex:%{public}d", errorCode, columnIndex);
    }
    return columnIndex;
}

// @ohos.data.dataSharePredicates.d.ets
int64_t DataSharePredicatesNew()
{
    return reinterpret_cast<long long>(new DataSharePredicates);
}

void DataSharePredicatesClean(int64_t predicatesPtr)
{
    std::string strFiled = std::string(field);
    EnumType type = value_type_get_type(value);
    switch (type) {
        case EnumType::StringType: {
            rust::String str = value_type_get_string(value);
            reinterpret_cast<DataSharePredicates*>(predicatesPtr)->EqualTo(strFiled, std::string(str));
            break;
        }
        case EnumType::F64Type: {
            double data = value_type_get_f64(value);
            reinterpret_cast<DataSharePredicates*>(predicatesPtr)->EqualTo(strFiled, data);
            break;
        }
        case EnumType::BooleanType: {
            bool data = value_type_get_bool(value);
            reinterpret_cast<DataSharePredicates*>(predicatesPtr)->EqualTo(strFiled, data);
            break;
        }
        default: {
            break;
        }
    }
}

void DataSharePredicatesEqualTo(int64_t predicatesPtr, rust::String field, const ValueType& value)
{
    std::string strFiled = std::string(field);
    EnumType type = value_type_get_type(value);
    switch (type) {
        case EnumType::StringType: {
            rust::String str = value_type_get_string(value);
            reinterpret_cast<DataSharePredicates*>(predicatesPtr)->NotEqualTo(strFiled, std::string(str));
            break;
        }
        case EnumType::F64Type: {
            double data = value_type_get_f64(value);
            reinterpret_cast<DataSharePredicates*>(predicatesPtr)->NotEqualTo(strFiled, data);
            break;
        }
        case EnumType::BooleanType: {
            bool data = value_type_get_bool(value);
            reinterpret_cast<DataSharePredicates*>(predicatesPtr)->NotEqualTo(strFiled, data);
            break;
        }
        default: {
            break;
        }
    }
}

void DataSharePredicatesNotEqualTo(int64_t predicatesPtr, rust::String field, const ValueType& value)
{
    (void)(*reinterpret_cast<DataSharePredicates*>(predicatesPtr)->BeginWrap());
}

void DataSharePredicatesBeginWrap(int64_t predicatesPtr)
{
    (void)(*reinterpret_cast<DataSharePredicates*>(predicatesPtr)->EndWrap());
}

void DataSharePredicatesEndWrap(int64_t predicatesPtr)
{
    (void)(*reinterpret_cast<DataSharePredicates*>(predicatesPtr)->Or());
}

void DataSharePredicatesOr(int64_t predicatesPtr)
{
    (void)(*reinterpret_cast<DataSharePredicates*>(predicatesPtr)->And());
}

void DataSharePredicatesAnd(int64_t predicatesPtr)
{
    (void)(*reinterpret_cast<DataSharePredicates*>(predicatesPtr)->Contains(std::string(field), std::string(value)));
}

void DataSharePredicatesContains(int64_t predicatesPtr, rust::String field, rust::String value)
{
    (void)(*reinterpret_cast<DataSharePredicates*>(predicatesPtr)->IsNull(std::string(field)));
}

void DataSharePredicatesIsNull(int64_t predicatesPtr, rust::String field)
{
    (void)(*reinterpret_cast<DataSharePredicates*>(predicatesPtr)->IsNotNull(std::string(field)));
}

void DataSharePredicatesIsNotNull(int64_t predicatesPtr, rust::String field)
{
    (void)(*reinterpret_cast<DataSharePredicates*>(predicatesPtr)->Like(std::string(field), std::string(value)));
}

void DataSharePredicatesLike(int64_t predicatesPtr, rust::String field, rust::String value)
{
}

void DataSharePredicatesBetween(int64_t predicatesPtr, rust::String field,
                                const ValueType& low, const ValueType& high)
{
    std::string strLow;
    std::string strHigh;
    EnumType type = value_type_get_type(low);
    switch (type) {
        case EnumType::StringType: {
            rust::String str = value_type_get_string(low);
            strLow = std::string(str);
            break;
        }
        case EnumType::F64Type: {
            double data = value_type_get_f64(low);
            strLow = std::to_string(data);
            break;
        }
        case EnumType::BooleanType: {
            bool data = value_type_get_bool(low);
            strLow = std::to_string(data);
            break;
        }
        default: {
            break;
        }
    }

    type = value_type_get_type(high);
    switch (type) {
        case EnumType::StringType: {
            rust::String str = value_type_get_string(high);
            strHigh = std::string(str);
            break;
        }
        case EnumType::F64Type: {
            double data = value_type_get_f64(high);
            strHigh = std::to_string(data);
            break;
        }
        case EnumType::BooleanType: {
            bool data = value_type_get_bool(high);
            strHigh = std::to_string(data);
            break;
        }
        default: {
            break;
        }
    }
    *reinterpret_cast<DataSharePredicates*>(predicatesPtr)->Between(std::string(field), strLow, strHigh);
}

void DataSharePredicatesGreaterThan(int64_t predicatesPtr, rust::String field, const ValueType& value)
{
    EnumType type = value_type_get_type(value);
    switch (type) {
        case EnumType::StringType: {
            rust::String str = value_type_get_string(value);
            (void)(*reinterpret_cast<DataSharePredicates*>(predicatesPtr)->GreaterThan(
                std::string(field), std::string(str)));
            break;
        }
        case EnumType::F64Type: {
            double data = value_type_get_f64(value);
            (void)(*reinterpret_cast<DataSharePredicates*>(predicatesPtr)->GreaterThan(std::string(field), data));
            break;
        }
        case EnumType::BooleanType: {
            bool data = value_type_get_bool(value);
            (void)(*reinterpret_cast<DataSharePredicates*>(predicatesPtr)->GreaterThan(std::string(field), data));
            break;
        }
        default: {
            break;
        }
    }
}

void DataSharePredicatesGreaterThanOrEqualTo(int64_t predicatesPtr, rust::String field, const ValueType& value)
{
    EnumType type = value_type_get_type(value);
    switch (type) {
        case EnumType::StringType: {
            rust::String str = value_type_get_string(value);
            (void)(*reinterpret_cast<DataSharePredicates*>(predicatesPtr)->GreaterThanOrEqualTo(
                std::string(field), std::string(str)));
            break;
        }
        case EnumType::F64Type: {
            double data = value_type_get_f64(value);
            (void)(*reinterpret_cast<DataSharePredicates*>(predicatesPtr)->GreaterThanOrEqualTo(
                std::string(field), data));
            break;
        }
        case EnumType::BooleanType: {
            bool data = value_type_get_bool(value);
            (void)(*reinterpret_cast<DataSharePredicates*>(predicatesPtr)->GreaterThanOrEqualTo(
                std::string(field), data));
            break;
        }
        default: {
            break;
        }
    }
}

void DataSharePredicatesLessThanOrEqualTo(int64_t predicatesPtr, rust::String field, const ValueType& value)
{
    EnumType type = value_type_get_type(value);
    switch (type) {
        case EnumType::StringType: {
            rust::String str = value_type_get_string(value);
            (void)(*reinterpret_cast<DataSharePredicates*>(predicatesPtr)->LessThanOrEqualTo(std::string(field),
                std::string(str)));
            break;
        }
        case EnumType::F64Type: {
            double data = value_type_get_f64(value);
            (void)(*reinterpret_cast<DataSharePredicates*>(predicatesPtr)->LessThanOrEqualTo(
                std::string(field), data));
            break;
        }
        case EnumType::BooleanType: {
            bool data = value_type_get_bool(value);
            (void)(*reinterpret_cast<DataSharePredicates*>(predicatesPtr)->LessThanOrEqualTo(
                std::string(field), data));
            break;
        }
        default: {
            break;
        }
    }
}

void DataSharePredicatesLessThan(int64_t predicatesPtr, rust::String field, const ValueType& value)
{
    EnumType type = value_type_get_type(value);
    switch (type) {
        case EnumType::StringType: {
            rust::String str = value_type_get_string(value);
            (void)(*reinterpret_cast<DataSharePredicates*>(predicatesPtr)->LessThan(
                std::string(field), std::string(str)));
            break;
        }
        case EnumType::F64Type: {
            double data = value_type_get_f64(value);
            (void)(*reinterpret_cast<DataSharePredicates*>(predicatesPtr)->LessThan(std::string(field), data));
            break;
        }
        case EnumType::BooleanType: {
            bool data = value_type_get_bool(value);
            (void)(*reinterpret_cast<DataSharePredicates*>(predicatesPtr)->LessThan(std::string(field), data));
            break;
        }
        default: {
            break;
        }
    }
}

void DataSharePredicatesOrderByAsc(int64_t predicatesPtr, rust::String field)
{
    (void)(*reinterpret_cast<DataSharePredicates*>(predicatesPtr)->OrderByAsc(std::string(field)));
}

void DataSharePredicatesOrderByDesc(int64_t predicatesPtr, rust::String field)
{
    (void)(*reinterpret_cast<DataSharePredicates*>(predicatesPtr)->OrderByDesc(std::string(field)));
}

void DataSharePredicatesLimit(int64_t predicatesPtr, double total, double offset)
{
    (void)(*reinterpret_cast<DataSharePredicates*>(predicatesPtr)->Limit(total, offset));
}

void DataSharePredicatesGroupBy(int64_t predicatesPtr, rust::Vec<rust::String> field)
{
    (void)(*reinterpret_cast<DataSharePredicates*>(predicatesPtr)->GroupBy(std::string(field)));
}

void DataSharePredicatesIn(int64_t predicatesPtr, rust::String field,  rust::Vec<ValueType> value)
{
    std::vector<std::string> values;
    for (const ValueType& v : value) {
        EnumType type = value_type_get_type(v);
        switch (type) {
            case EnumType::StringType: {
                rust::String str = value_type_get_string(v);
                values.push_back(std::string(str));
                break;
            }
            case EnumType::F64Type: {
                double data = value_type_get_f64(v);
                values.push_back(std::to_string(data));
                break;
            }
            case EnumType::BooleanType: {
                bool data = value_type_get_bool(v);
                values.push_back(std::to_string(data));
                break;
            }
            default: {
                break;
            }
        }
    }
    (void)(*reinterpret_cast<DataSharePredicates*>(predicatesPtr)->In(std::string(field), values));
}

void DataSharePredicatesNotIn(int64_t predicatesPtr, rust::String field,  rust::Vec<ValueType> value)
{
    std::vector<std::string> values;
    for (const ValueType& v : value) {
        EnumType type = value_type_get_type(v);
        switch (type) {
            case EnumType::StringType: {
                rust::String str = value_type_get_string(v);
                values.push_back(std::string(str));
                break;
            }
            case EnumType::F64Type: {
                double data = value_type_get_f64(v);
                values.push_back(std::to_string(data));
                break;
            }
            case EnumType::BooleanType: {
                bool data = value_type_get_bool(v);
                values.push_back(std::to_string(data));
                break;
            }
            default: {
                break;
            }
        }
    }
    (void)(*reinterpret_cast<DataSharePredicates*>(predicatesPtr)->NotIn(std::string(field), values));
}

// @ohos.data.dataShare.d.ets
int64_t DataShareNativeCreate(int64_t context, rust::String strUri,
                                bool optionIsUndefined, bool isProxy)
{
    std::string stdStrUri = std::string(strUri);
    std::shared_ptr<AbilityRuntime::Context> weakContext =
        reinterpret_cast<std::weak_ptr<AbilityRuntime::Context>*>(context)->lock();
    std::shared_ptr<DataShareHelper> dataShareHelper;
    if (optionIsUndefined) {
        dataShareHelper = DataShareHelper::Creator(weakContext->GetToken(), stdStrUri);
    } else {
        CreateOptions options = {
            isProxy,
            weakContext->GetToken(),
            Uri(stdStrUri).GetScheme() == "datashareproxy",
        };
        dataShareHelper = DataShareHelper::Creator(stdStrUri, options);
    }
    jsRdbObsManager_ = std::make_shared<AniRdbSubscriberManager>(dataShareHelper);
    jsPublishedObsManager_ = std::make_shared<AniPublishedSubscriberManager>(dataShareHelper);
    return reinterpret_cast<long long>(dataShareHelper.get());
}

void DataShareNativeClean(int64_t dataShareHelperPtr)
{
    delete reinterpret_cast<DataShareHelper*>(dataShareHelperPtr);
}

int64_t DataShareNativeQuery(int64_t dataShareHelperPtr, rust::String strUri,
                               int64_t dataSharePredicatesPtr, rust::Vec<rust::String> columns)
{
    std::string stdStrUri = std::string(strUri);
    std::vector<std::string> std_vector = convert_rust_vec_to_cpp_vector(columns);
    DatashareBusinessError businessError;
    Uri uri(stdStrUri);
    std::shared_ptr<DataShareResultSet> resultSet = reinterpret_cast<DataShareHelper*>(dataShareHelperPtr)->Query(uri,
        *reinterpret_cast<DataSharePredicates*>(dataSharePredicatesPtr), std_vector, &businessError);
    if (resultSet == nullptr) {
        LOG_ERROR("query failed, resultSet is null!");
        return {};
    }

    if (businessError.GetCode() != 0) {
        LOG_ERROR("query falied, errorCode: %{public}d", businessError.GetCode());
        return {};
    }
    return reinterpret_cast<long long>(resultSet.get());
}

int DataShareNativeUpdate(int64_t dataShareHelperPtr, rust::String strUri,
                          int64_t dataSharePredicatesPtr, rust::Vec<ValuesBucketKvItem> bucket)
{
    std::string stdStrUri = std::string(strUri);
    DataShareValuesBucket valuesBucket;
    for (const ValuesBucketKvItem& cpp_bucket : bucket) {
        rust::String bucket_key = value_bucket_get_key(cpp_bucket);
        std::string key = std::string(bucket_key);
        EnumType bucket_type = value_bucket_get_vtype(cpp_bucket);
        switch (bucket_type) {
            case EnumType::StringType: {
                rust::String str = value_bucket_get_string(cpp_bucket);
                std::string stdStr = std::string(str);
                valuesBucket.Put(key, stdStr);
                break;
            }
            case EnumType::F64Type: {
                double data = value_bucket_get_f64(cpp_bucket);
                valuesBucket.Put(key, data);
                break;
            }
            case EnumType::BooleanType: {
                bool data = value_bucket_get_bool(cpp_bucket);
                valuesBucket.Put(key, data);
                break;
            }
            case EnumType::Uint8ArrayType: {
                rust::Vec<uint8_t> data = value_bucket_get_uint8array(cpp_bucket);
                std::vector<uint8_t> std_vector = convert_rust_vec_to_cpp_vector(data);
                valuesBucket.Put(key, std_vector);
                break;
            }
            case EnumType::NullType: {
                valuesBucket.Put(key, {});
                break;
            }
            default: {
                break;
            }
        }
    }

    Uri uri(stdStrUri);
    int resultNumber = reinterpret_cast<DataShareHelper*>(dataShareHelperPtr)->Update(uri,
        *reinterpret_cast<DataSharePredicates*>(dataSharePredicatesPtr), valuesBucket);
    if (resultNumber < 0) {
        LOG_ERROR("Update failed");
    }
    return resultNumber;
}

void DataShareNativePublish(int64_t dataShareHelperPtr, rust::Vec<PublishedItem> data,
                            rust::String bundleName, VersionWrap version, PublishSretParam& sret)
{
    std::string stdBundleName = std::string(bundleName);
    Data PublishData;
    for (PublishedItem& item : data) {
        rust::String key = published_item_get_key(item);
        std::string strKey = std::string(published_item_get_data_string(item));
        rust::String subscriberId = published_item_get_subscriber_id(item);
        std::string strSubscriberId = std::string(published_item_get_subscriber_id(item));
        int64_t llSubscriberId = atoll(strSubscriberId.c_str());
        EnumType data_type = published_item_get_data_type(item);
        switch (data_type) {
            case EnumType::StringType: {
                rust::String data_str = published_item_get_data_string(item);
                std::string stdStr = std::string(data_str);
                PublishData.datas_.emplace_back(strKey, llSubscriberId, stdStr);
                break;
            }
            case EnumType::ArrayBufferType: {
                rust::Slice<const uint8_t> data_arraybuffer = published_item_get_data_arraybuffer(item);
                std::vector<uint8_t> std_vec;
                for (const auto &dataItem : data_arraybuffer) {
                    std_vec.push_back(dataItem);
                }
                PublishData.datas_.emplace_back(strKey, llSubscriberId, std_vec);
                break;
            }
            default: {
                break;
            }
        }
    }

    std::vector<OperationResult> results = reinterpret_cast<DataShareHelper*>(dataShareHelperPtr)->Publish(PublishData,
        stdBundleName);
    for (const auto &result : results) {
        publish_sret_push(sret, rust::String(result.key_), result.errCode_);
    }
}

void DataShareNativeGetPublishedData(int64_t dataShareHelperPtr, rust::String bundleName,
                                     GetPublishedDataSretParam& sret)
{
    std::string stdBundleName = std::string(bundleName);
    int errorCode = 0;
    Data publishData = reinterpret_cast<DataShareHelper*>(dataShareHelperPtr)->GetPublishedData(stdBundleName,
        errorCode);
    for (const auto &data : publishData.datas_) {
        DataShare::PublishedDataItem::DataType dataItem = data.GetData();
        if (dataItem.index() == 0) {
            std::vector std_vec = std::get<std::vector<uint8_t>>(dataItem);
            rust::Vec<uint8_t> rust_vec;
            for (const auto &u8data : std_vec) {
                rust_vec.push_back(u8data);
            }
            published_data_sret_push_array(sret,
                rust::String(data.key_), rust::String(std::to_string(data.subscriberId_)), rust_vec);
        } else {
            std::string strData = std::get<std::string>(dataItem);
            published_data_sret_push_str(sret,
                rust::String(data.key_), rust::String(std::to_string(data.subscriberId_)), rust::String(strData));
        }
    }
}

void DataShareNativeAddTemplate(int64_t dataShareHelperPtr, rust::String uri,
                                rust::String subscriberId, const Template& temp)
{
    rust::String scheduler = template_get_scheduler(temp);
    rust::String update = template_get_update(temp);
    rust::Vec<TemplatePredicatesKvItem> predicates = template_get_predicates(temp);
    std::vector<PredicateTemplateNode> stdPredicates;
    for (TemplatePredicatesKvItem& kv : predicates) {
        rust::String key = template_predicates_get_key(kv);
        rust::String value = template_predicates_get_value(kv);
        stdPredicates.emplace_back(std::string(key), std::string(value));
    }

    std::string tplUri = std::string(uri);
    int64_t llSubscriberId = atoll(std::string(subscriberId).c_str());
    DataShare::Template tpl(std::string(update), stdPredicates, std::string(scheduler));
    auto result = reinterpret_cast<DataShareHelper*>(dataShareHelperPtr)->AddQueryTemplate(tplUri, llSubscriberId, tpl);
    if (result != E_OK) {
        LOG_ERROR("AddTemplate failed, result: %{public}d", result);
    }
}

void DataShareNativeDelTemplate(int64_t dataShareHelperPtr, rust::String uri, rust::String subscriberId)
{
    std::string tplUri = std::string(uri);
    int64_t llSubscriberId = atoll(std::string(subscriberId).c_str());
    auto result = reinterpret_cast<DataShareHelper*>(dataShareHelperPtr)->DelQueryTemplate(tplUri, llSubscriberId);
    if (result != E_OK) {
        LOG_ERROR("AddTemplate failed, result: %{public}d", result);
    }
}

int DataShareNativeInsert(int64_t dataShareHelperPtr, rust::String strUri,
                          rust::Vec<ValuesBucketKvItem> bucket)
{
    std::string stdStrUri = std::string(strUri);
    DataShareValuesBucket valuesBucket;
    for (const ValuesBucketKvItem& cpp_bucket : bucket) {
        rust::String bucket_key = value_bucket_get_key(cpp_bucket);
        std::string key = std::string(bucket_key);
        EnumType bucket_type = value_bucket_get_vtype(cpp_bucket);
        switch (bucket_type) {
            case EnumType::StringType: {
                rust::String str = value_bucket_get_string(cpp_bucket);
                valuesBucket.Put(key, std::string(str));
                break;
            }
            case EnumType::F64Type: {
                double data = value_bucket_get_f64(cpp_bucket);
                valuesBucket.Put(key, data);
                break;
            }
            case EnumType::BooleanType: {
                bool data = value_bucket_get_bool(cpp_bucket);
                valuesBucket.Put(key, data);
                break;
            }
            case EnumType::Uint8ArrayType: {
                rust::Vec<uint8_t> data = value_bucket_get_uint8array(cpp_bucket);
                std::vector<uint8_t> std_vector = convert_rust_vec_to_cpp_vector(data);
                valuesBucket.Put(key, std_vector);
                break;
            }
            case EnumType::NullType: {
                valuesBucket.Put(key, {});
                break;
            }
            default: {
                break;
            }
        }
    }

    Uri uri(std::string(strUri).c_str());
    int resultNumber = 0;
    resultNumber = reinterpret_cast<DataShareHelper*>(dataShareHelperPtr)->Insert(uri, valuesBucket);
    if (resultNumber < 0) {
        LOG_ERROR("Insert failed");
    }
    return resultNumber;
}

int DataShareNativeBatchInsert(int64_t dataShareHelperPtr, rust::String strUri,
                               rust::Vec<ValuesBucketWrap> buckets)
{
    std::string stdStrUri = std::string(strUri);
    std::vector<DataShareValuesBucket> valuesBuckets;
    for (ValuesBucketWrap& wrapBuckets : buckets) {
        rust::Vec<ValuesBucketKvItem> const &bucket = values_bucket_wrap_inner(wrapBuckets);
        DataShareValuesBucket valuesBucket;
        for (const ValuesBucketKvItem& cpp_bucket : bucket) {
            std::string key = std::string(value_bucket_get_key(cpp_bucket));
            EnumType bucket_type = value_bucket_get_vtype(cpp_bucket);
            switch (bucket_type) {
                case EnumType::StringType: {
                    rust::String str = value_bucket_get_string(cpp_bucket);
                    valuesBucket.Put(key, std::string(str));
                    break;
                }
                case EnumType::F64Type: {
                    double data = value_bucket_get_f64(cpp_bucket);
                    valuesBucket.Put(key, data);
                    break;
                }
                case EnumType::BooleanType: {
                    bool data = value_bucket_get_bool(cpp_bucket);
                    valuesBucket.Put(key, data);
                    break;
                }
                case EnumType::Uint8ArrayType: {
                    rust::Vec<uint8_t> data = value_bucket_get_uint8array(cpp_bucket);
                    std::vector<uint8_t> std_vec = convert_rust_vec_to_cpp_vector(data);
                    valuesBucket.Put(key, std_vec);
                    break;
                }
                case EnumType::NullType: {
                    valuesBucket.Put(key, {});
                    break;
                }
                default: {
                    break;
                }
            }
        }
        valuesBuckets.push_back(valuesBucket);
    }

    Uri uri(std::string(strUri).c_str());
    int resultNumber = 0;
    resultNumber = reinterpret_cast<DataShareHelper*>(dataShareHelperPtr)->BatchInsert(uri, valuesBuckets);
    if (resultNumber < 0) {
        LOG_ERROR("BatchInsert failed");
    }
    return resultNumber;
}

int DataShareNativeDelete(int64_t dataShareHelperPtr, rust::String strUri, int64_t dataSharePredicatesPtr)
{
    int resultNumber = 0;
    Uri uri(std::string(strUri).c_str());
    resultNumber = reinterpret_cast<DataShareHelper*>(dataShareHelperPtr)->Delete(uri,
        *reinterpret_cast<DataSharePredicates*>(dataSharePredicatesPtr));
    if (resultNumber < 0) {
        LOG_ERROR("BatchInsert failed");
    }
    return resultNumber;
}

void DataShareNativeClose(int64_t dataShareHelperPtr)
{
    reinterpret_cast<DataShareHelper*>(dataShareHelperPtr)->Release();
}

void ANIRegisterObserver(const std::string &strUri, long long dataShareHelperPtr, long long envPtr,
    long long callbackPtr, bool isNotifyDetails)
{
    std::lock_guard<std::mutex> lck(listMutex_);
    observerMap_.try_emplace(strUri);
    auto &list = observerMap_.find(strUri)->second;
    for (const auto &item : list) {
        if (callbackPtr == item->observer_->GetCallback()) {
            LOG_ERROR("observer has already subscribed.");
            return;
        }
    }

    auto innerObserver = std::make_shared<ANIInnerDataShareObserver>(envPtr, callbackPtr);
    sptr<ANIDataShareObserver> observer(new (std::nothrow) ANIDataShareObserver(innerObserver));
    if (observer == nullptr) {
        LOG_ERROR("observer is nullptr");
        return;
    }
    Uri uri(strUri);
    if (!isNotifyDetails) {
        reinterpret_cast<DataShareHelper*>(dataShareHelperPtr)->RegisterObserver(uri, observer);
    } else {
        reinterpret_cast<DataShareHelper*>(dataShareHelperPtr)->RegisterObserverExt(uri,
            std::shared_ptr<DataShareObserver>(observer.GetRefPtr(), [holder = observer](const auto*) {}), false);
    }
    list.push_back(observer);
}

void ANIUnRegisterObserver(const std::string &strUri, long long dataShareHelperPtr, long long envPtr,
    long long callbackPtr, bool isNotifyDetails)
{
    std::lock_guard<std::mutex> lck(listMutex_);
    auto &list = observerMap_.find(strUri)->second;
    auto it = list.begin();
    Uri uri(strUri);
    while (it != list.end()) {
        if (callbackPtr != (*it)->observer_->GetCallback()) {
            ++it;
            continue;
        }
        if (!isNotifyDetails) {
            reinterpret_cast<DataShareHelper*>(dataShareHelperPtr)->UnregisterObserver(uri, *it);
        } else {
            reinterpret_cast<DataShareHelper*>(dataShareHelperPtr)->UnregisterObserverExt(uri,
                std::shared_ptr<DataShareObserver>((*it).GetRefPtr(), [holder = *it](const auto*) {}));
        }
        it = list.erase(it);
        break;
    }
    if (list.empty()) {
        observerMap_.erase(strUri);
    }
}

void ANIUnRegisterObserver(const std::string &strUri, long long dataShareHelperPtr, long long envPtr,
    bool isNotifyDetails)
{
    std::lock_guard<std::mutex> lck(listMutex_);
    auto &list = observerMap_.find(strUri)->second;
    auto it = list.begin();
    Uri uri(strUri);
    while (it != list.end()) {
        if (!isNotifyDetails) {
            reinterpret_cast<DataShareHelper*>(dataShareHelperPtr)->UnregisterObserver(uri, *it);
        } else {
            reinterpret_cast<DataShareHelper*>(dataShareHelperPtr)->UnregisterObserverExt(uri,
                std::shared_ptr<DataShareObserver>((*it).GetRefPtr(), [holder = *it](const auto*) {}));
        }
        it = list.erase(it);
    }
    observerMap_.erase(strUri);
}

void DataShareNativeOn(EnvPtrWrap envPtrWrap, rust::String strType, rust::String strUri)
{
    ANIRegisterObserver(std::string(strUri), envPtrWrap.dataShareHelperPtr, envPtrWrap.envPtr, envPtrWrap.callbackPtr);
}

void DataShareNativeOnChangeinfo(EnvPtrWrap envPtrWrap, rust::String event,
                                 int32_t arktype, rust::String strUri)
{
    ANIRegisterObserver(std::string(strUri),
        envPtrWrap.dataShareHelperPtr, envPtrWrap.envPtr, envPtrWrap.callbackPtr, true);
}

void DataShareNativeOnRdbDataChange(EnvPtrWrap envPtrWrap, rust::String arktype,
                                    rust::Vec<rust::String> uris, const TemplateId& templateId,
                                    PublishSretParam& sret)
{
    std::vector<OperationResult> results;
    std::vector<std::string> stdUris;
    for (const auto &uri : uris) {
        stdUris.push_back(std::string(uri));
    }
    DataShare::TemplateId tplId;
    tplId.subscriberId_ = atoll(std::string(template_id_get_subscriber_id(templateId)).c_str());
    tplId.bundleName_ = std::string(template_id_get_bundle_name_of_owner(templateId));
    std::shared_ptr<AniRdbSubscriberManager> jsRdbObsManager =
        std::make_shared<AniRdbSubscriberManager>(reinterpret_cast<DataShareHelper*>(envPtrWrap.dataShareHelperPtr));
    if (jsRdbObsManager == nullptr) {
        LOG_ERROR("OnRdbDataChange failed, jsRdbObsManager is null");
        return;
    }
    results = jsRdbObsManager->AddObservers(envPtrWrap.envPtr, envPtrWrap.callbackPtr, stdUris, tplId);
    for (const auto &result : results) {
        publish_sret_push(sret, rust::String(result.key_), result.errCode_);
    }
}

void DataShareNativeOnPublishedDataChange(EnvPtrWrap envPtrWrap, rust::String arktype,
                                          rust::Vec<rust::String> uris, rust::String subscriberId,
                                          PublishSretParam& sret)
{
    std::vector<OperationResult> results;
    std::vector<std::string> stdUris;
    for (const auto &uri : uris) {
        stdUris.push_back(std::string(uri));
    }

    int64_t innerSubscriberId = atoll(std::string(subscriberId).c_str());
    auto jsPublishedObsManager = std::make_shared<AniPublishedSubscriberManager>(
        reinterpret_cast<DataShareHelper*>(envPtrWrap.dataShareHelperPtr));
    if (jsPublishedObsManager == nullptr) {
        LOG_ERROR("OnPublishedDataChange failed, jsPublishedObsManager is null");
        return;
    }
    results = jsPublishedObsManager->AddObservers(envPtrWrap.envPtr,
        envPtrWrap.callbackPtr, stdUris, innerSubscriberId);
    for (const auto &result : results) {
        publish_sret_push(sret, rust::String(result.key_), result.errCode_);
    }
}

void DataShareNativeOff(EnvPtrWrap envPtrWrap, rust::String strType, rust::String strUri)
{
    if (envPtrWrap.callbackPtr != 0) {
        ANIUnRegisterObserver(std::string(strUri), envPtrWrap.dataShareHelperPtr,
            envPtrWrap.envPtr, envPtrWrap.callbackPtr);
    }
    ANIUnRegisterObserver(std::string(strUri), envPtrWrap.dataShareHelperPtr, envPtrWrap.envPtr);
}

void DataShareNativeOffChangeinfo(EnvPtrWrap envPtrWrap, rust::String event,
                                  int32_t arktype, rust::String strUri)
{
    if (envPtrWrap.callbackPtr != 0) {
        ANIUnRegisterObserver(std::string(strUri), envPtrWrap.dataShareHelperPtr,
            envPtrWrap.envPtr, envPtrWrap.callbackPtr, true);
    }
    ANIUnRegisterObserver(std::string(strUri), envPtrWrap.dataShareHelperPtr, envPtrWrap.envPtr, true);
}

void DataShareNativeOffRdbDataChange(EnvPtrWrap envPtrWrap, rust::String arktype, rust::Vec<rust::String> uris,
                                     const TemplateId& templateId, PublishSretParam& sret)
{
    std::vector<OperationResult> results;
    std::vector<std::string> stdUris;
    for (const auto &uri : uris) {
        stdUris.push_back(std::string(uri));
    }
    DataShare::TemplateId tplId;
    tplId.subscriberId_ = atoll(std::string(template_id_get_subscriber_id(templateId)).c_str());
    tplId.bundleName_ = std::string(template_id_get_bundle_name_of_owner(templateId));
    std::shared_ptr<AniRdbSubscriberManager> jsRdbObsManager =
        std::make_shared<AniRdbSubscriberManager>(reinterpret_cast<DataShareHelper*>(envPtrWrap.dataShareHelperPtr));
    if (jsRdbObsManager == nullptr) {
        LOG_ERROR("OffRdbDataChange failed, jsRdbObsManager is null");
        return;
    }
    if (envPtrWrap.callbackPtr == 0) {
        results = jsRdbObsManager->DelObservers(envPtrWrap.envPtr, 0, stdUris, tplId);
    }
    results = jsRdbObsManager->DelObservers(envPtrWrap.envPtr, envPtrWrap.callbackPtr, stdUris, tplId);
    for (const auto &result : results) {
        publish_sret_push(sret, rust::String(result.key_), result.errCode_);
    }
}

void DataShareNativeOffPublishedDataChange(EnvPtrWrap envPtrWrap, rust::String arktype,
                                           rust::Vec<rust::String> uris, rust::String subscriberId,
                                           PublishSretParam& sret)
{
    std::vector<OperationResult> results;
    std::vector<std::string> stdUris;
    for (const auto &uri : uris) {
        stdUris.push_back(std::string(uri));
    }
    int64_t innerSubscriberId = atoll(std::string(subscriberId).c_str());
    auto jsPublishedObsManager = std::make_shared<AniPublishedSubscriberManager>(
        reinterpret_cast<DataShareHelper*>(envPtrWrap.dataShareHelperPtr));
    if (jsPublishedObsManager == nullptr) {
        LOG_ERROR("OffPublishedDataChange failed, jsPublishedObsManager is null");
        return;
    }
    if (envPtrWrap.callbackPtr == 0) {
        results = jsPublishedObsManager->DelObservers(envPtrWrap.envPtr, 0, stdUris, innerSubscriberId);
    }
    results = jsPublishedObsManager->DelObservers(envPtrWrap.envPtr,
        envPtrWrap.callbackPtr, stdUris, innerSubscriberId);
    for (const auto &result : results) {
        publish_sret_push(sret, rust::String(result.key_), result.errCode_);
    }
}

void DataShareNativeExtensionCallbackInt(double errorCode, rust::string errorMsg, int32_t data, int64_t nativePtr)
{
    //todo
}

void DataShareNativeExtensionCallbackObject(double errorCode, rust::string errorMsg, int64_t ptr, int64_t nativePtr)
{
    //todo
}

void DataShareNativeExtensionCallbackVoid(double errorCode, rust::string errorMsg, int64_t nativePtr)
{
    //todo
}

} // namespace DataShareAni
} // namespace OHOS
