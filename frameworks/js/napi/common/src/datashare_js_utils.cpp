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

#include "datashare_js_utils.h"

#include "datashare_log.h"
#include "datashare_predicates_proxy.h"
#include "datashare_valuebucket_convert.h"
#include "napi/native_common.h"
#include "napi_datashare_values_bucket.h"
#include "securec.h"

namespace OHOS {
namespace DataShare {
std::string DataShareJSUtils::Convert2String(napi_env env, napi_value jsStr, const size_t max)
{
    size_t str_buffer_size = max;
    napi_get_value_string_utf8(env, jsStr, nullptr, 0, &str_buffer_size);
    char *buf = new (std::nothrow) char[str_buffer_size + 1];
    if (buf == nullptr) {
        return "";
    }
    size_t len = 0;
    napi_get_value_string_utf8(env, jsStr, buf, str_buffer_size + 1, &len);
    buf[len] = 0;
    std::string value(buf);
    delete[] buf;
    return value;
}

std::vector<std::string> DataShareJSUtils::Convert2StrVector(napi_env env, napi_value value, const size_t strMax)
{
    NAPI_ASSERT_BASE(env, strMax > 0, "failed on strMax > 0",  std::vector<std::string>());
    uint32_t arrLen = 0;
    napi_get_array_length(env, value, &arrLen);
    if (arrLen == 0) {
        return {};
    }
    std::vector<std::string> result;
    for (size_t i = 0; i < arrLen; ++i) {
        napi_value element;
        if (napi_get_element(env, value, i, &element) != napi_ok) {
            return {};
        }
        result.push_back(ConvertAny2String(env, element));
    }
    return result;
}

std::vector<uint8_t> DataShareJSUtils::Convert2U8Vector(napi_env env, napi_value input_array)
{
    bool isTypedArray = false;
    bool isArrayBuffer = false;
    napi_is_typedarray(env, input_array, &isTypedArray);
    if (!isTypedArray) {
        napi_is_arraybuffer(env, input_array, &isArrayBuffer);
        if (!isArrayBuffer) {
            LOG_ERROR("unknow type");
            return {};
        }
    }
    size_t length = 0;
    void *data = nullptr;
    if (isTypedArray) {
        napi_typedarray_type type;
        napi_value input_buffer = nullptr;
        size_t byte_offset = 0;
        napi_get_typedarray_info(env, input_array, &type, &length, &data, &input_buffer, &byte_offset);
        if (type != napi_uint8_array || data == nullptr) {
            LOG_ERROR("napi_get_typedarray_info err");
            return {};
        }
    } else {
        napi_get_arraybuffer_info(env, input_array, &data, &length);
        if (data == nullptr || length <= 0) {
            LOG_ERROR("napi_get_arraybuffer_info err");
            return {};
        }
    }
    return std::vector<uint8_t>((uint8_t *)data, ((uint8_t *)data) + length);
}

std::vector<uint8_t> DataShareJSUtils::ConvertU8Vector(napi_env env, napi_value jsValue)
{
    bool isTypedArray = false;
    if (napi_is_typedarray(env, jsValue, &isTypedArray) != napi_ok || !isTypedArray) {
        return {};
    }

    napi_typedarray_type type;
    size_t length = 0;
    napi_value buffer = nullptr;
    size_t offset = 0;
    NAPI_CALL_BASE(env, napi_get_typedarray_info(env, jsValue, &type, &length, nullptr, &buffer, &offset), {});
    if (type != napi_uint8_array) {
        return {};
    }
    uint8_t *data = nullptr;
    size_t total = 0;
    NAPI_CALL_BASE(env, napi_get_arraybuffer_info(env, buffer, reinterpret_cast<void **>(&data), &total), {});
    length = std::min<size_t>(length, total - offset);
    std::vector<uint8_t> result(sizeof(uint8_t) + length);
    int retCode = memcpy_s(result.data(), result.size(), &data[offset], length);
    if (retCode != 0) {
        return {};
    }
    return result;
}

std::string DataShareJSUtils::ConvertAny2String(napi_env env, napi_value jsValue)
{
    napi_valuetype valueType = napi_undefined;
    NAPI_CALL_BASE(env, napi_typeof(env, jsValue, &valueType), "napi_typeof failed");
    if (valueType == napi_string) {
        return DataShareJSUtils::Convert2String(env, jsValue, DataShareJSUtils::DEFAULT_BUF_SIZE);
    } else if (valueType == napi_number) {
        double valueNumber;
        napi_get_value_double(env, jsValue, &valueNumber);
        return std::to_string(valueNumber);
    } else if (valueType == napi_boolean) {
        bool valueBool = false;
        napi_get_value_bool(env, jsValue, &valueBool);
        return std::to_string(valueBool);
    } else if (valueType == napi_null) {
        return "null";
    } else if (valueType == napi_object) {
        std::vector<uint8_t> bytes = DataShareJSUtils::Convert2U8Vector(env, jsValue);
        std::string ret(bytes.begin(), bytes.end());
        return ret;
    }

    return "invalid type";
}

napi_value DataShareJSUtils::Convert2JSValue(napi_env env, const std::monostate &value)
{
    return nullptr;
}

napi_value DataShareJSUtils::Convert2JSValue(napi_env env, const std::vector<std::string> &value)
{
    napi_value jsValue;
    napi_status status = napi_create_array_with_length(env, value.size(), &jsValue);
    if (status != napi_ok) {
        return nullptr;
    }

    for (size_t i = 0; i < value.size(); ++i) {
        napi_set_element(env, jsValue, i, Convert2JSValue(env, value[i]));
    }
    return jsValue;
}

napi_value DataShareJSUtils::Convert2JSValue(napi_env env, const std::string &value)
{
    napi_value jsValue;
    napi_status status = napi_create_string_utf8(env, value.c_str(), value.size(), &jsValue);
    if (status != napi_ok) {
        return nullptr;
    }
    return jsValue;
}

napi_value DataShareJSUtils::Convert2JSValue(napi_env env, const std::vector<uint8_t> &value, bool isTypedArray)
{
    void *native = nullptr;
    napi_value buffer = nullptr;
    if (value.empty()) {
        LOG_DEBUG("vector is empty");
        return nullptr;
    }
    napi_status status = napi_create_arraybuffer(env, value.size(), &native, &buffer);
    if (status != napi_ok) {
        return nullptr;
    }
    if (memcpy_s(native, value.size(), value.data(), value.size()) != EOK && value.size() > 0) {
        return nullptr;
    }
    if (!isTypedArray) {
        return buffer;
    }
    napi_value jsValue;
    status = napi_create_typedarray(env, napi_uint8_array, value.size(), buffer, 0, &jsValue);
    if (status != napi_ok) {
        return nullptr;
    }
    return jsValue;
}

napi_value DataShareJSUtils::Convert2JSValue(napi_env env, int32_t value)
{
    napi_value jsValue;
    napi_status status = napi_create_int32(env, value, &jsValue);
    if (status != napi_ok) {
        return nullptr;
    }
    return jsValue;
}

napi_value DataShareJSUtils::Convert2JSValue(napi_env env, int64_t value)
{
    napi_value jsValue;
    napi_status status = napi_create_int64(env, value, &jsValue);
    if (status != napi_ok) {
        return nullptr;
    }
    return jsValue;
}

napi_value DataShareJSUtils::Convert2JSValue(napi_env env, uint32_t value)
{
    napi_value jsValue;
    napi_status status = napi_create_uint32(env, value, &jsValue);
    if (status != napi_ok) {
        return nullptr;
    }
    return jsValue;
}

napi_value DataShareJSUtils::Convert2JSValue(napi_env env, double value)
{
    napi_value jsValue;
    napi_status status = napi_create_double(env, value, &jsValue);
    if (status != napi_ok) {
        return nullptr;
    }
    return jsValue;
}

napi_value DataShareJSUtils::Convert2JSValue(napi_env env, bool value)
{
    napi_value jsValue;
    napi_status status = napi_get_boolean(env, value, &jsValue);
    if (status != napi_ok) {
        return nullptr;
    }
    return jsValue;
}

napi_value DataShareJSUtils::Convert2JSValue(napi_env env, const std::map<std::string, int> &value)
{
    napi_value jsValue;
    napi_status status = napi_create_array_with_length(env, value.size(), &jsValue);
    if (status != napi_ok) {
        return nullptr;
    }

    int index = 0;
    for (const auto& [device, result] : value) {
        napi_value jsElement;
        status = napi_create_array_with_length(env, SYNC_RESULT_ELEMNT_NUM, &jsElement);
        if (status != napi_ok) {
            return nullptr;
        }
        napi_set_element(env, jsElement, 0, Convert2JSValue(env, device));
        napi_set_element(env, jsElement, 1, Convert2JSValue(env, result));
        napi_set_element(env, jsValue, index++, jsElement);
    }

    return jsValue;
}
std::string DataShareJSUtils::UnwrapStringFromJS(napi_env env, napi_value param, const std::string &defaultValue)
{
    size_t size = 0;
    if (napi_get_value_string_utf8(env, param, nullptr, 0, &size) != napi_ok) {
        return defaultValue;
    }

    std::string value("");
    if (size == 0) {
        return defaultValue;
    }

    char *buf = new (std::nothrow) char[size + 1];
    if (buf == nullptr) {
        return value;
    }
    (void)memset_s(buf, size + 1, 0, size + 1);

    bool rev = napi_get_value_string_utf8(env, param, buf, size + 1, &size) == napi_ok;
    if (rev) {
        value = buf;
    } else {
        value = defaultValue;
    }

    if (buf != nullptr) {
        delete[] buf;
        buf = nullptr;
    }
    return value;
}

napi_value DataShareJSUtils::Convert2JSValue(napi_env env, const DataShareValuesBucket &valueBucket)
{
    napi_value res = NewInstance(env, valueBucket);
    if (res == nullptr) {
        LOG_ERROR("failed to make new instance of DataShareValueBucket.");
    }
    return res;
}

DataShareValueObject DataShareJSUtils::Convert2ValueObject(napi_env env, napi_value value, bool &status)
{
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, value, &valueType);
    status = true;
    if (valueType == napi_string) {
        std::string valueString = DataShareJSUtils::UnwrapStringFromJS(env, value);
        return valueString;
    } else if (valueType == napi_number) {
        double valueNumber = 0;
        napi_get_value_double(env, value, &valueNumber);
        return valueNumber;
    } else if (valueType == napi_boolean) {
        bool valueBool = false;
        napi_get_value_bool(env, value, &valueBool);
        return valueBool;
    } else if (valueType == napi_null) {
        return {};
    } else if (valueType == napi_object) {
        std::vector<uint8_t> valueBlob = DataShareJSUtils::Convert2U8Vector(env, value);
        return valueBlob;
    } else {
        LOG_ERROR("valuesBucket error");
        status = false;
        return {};
    }
}

bool DataShareJSUtils::Equals(napi_env env, napi_value value, napi_ref copy)
{
    if (copy == nullptr) {
        return (value == nullptr);
    }

    napi_value copyValue = nullptr;
    napi_get_reference_value(env, copy, &copyValue);

    bool isEqual = false;
    napi_strict_equals(env, value, copyValue, &isEqual);
    return isEqual;
}

napi_value DataShareJSUtils::Convert2JSValue(napi_env env, const TemplateId &templateId)
{
    napi_value tplId = nullptr;
    napi_create_object(env, &tplId);
    napi_value subscriberId = Convert2JSValue(env, std::to_string(templateId.subscriberId_));
    if (subscriberId == nullptr) {
        return nullptr;
    }
    napi_value bundleName = nullptr;
    bundleName = Convert2JSValue(env, templateId.bundleName_);
    if (bundleName == nullptr) {
        return nullptr;
    }
    napi_set_named_property(env, tplId, "subscriberId", subscriberId);
    napi_set_named_property(env, tplId, "bundleName", bundleName);
    return tplId;
}

napi_value DataShareJSUtils::Convert2JSValue(napi_env env, const RdbChangeNode &changeNode)
{
    napi_value jsRdbChangeNode = nullptr;
    napi_create_object(env, &jsRdbChangeNode);

    napi_value uri = nullptr;
    uri = Convert2JSValue(env, changeNode.uri_);
    if (uri == nullptr) {
        return nullptr;
    }
    napi_value templateId = nullptr;
    templateId = Convert2JSValue(env, changeNode.templateId_);
    if (templateId == nullptr) {
        return nullptr;
    }
    napi_value data = Convert2JSValue(env, changeNode.data_);
    if (data == nullptr) {
        return nullptr;
    }
    napi_set_named_property(env, jsRdbChangeNode, "uri", uri);
    napi_set_named_property(env, jsRdbChangeNode, "templateId", templateId);
    napi_set_named_property(env, jsRdbChangeNode, "data", data);
    return jsRdbChangeNode;
}

napi_value DataShareJSUtils::Convert2JSValue(napi_env env, PublishedDataItem &publishedDataItem)
{
    napi_value jsPublishedDataItem = nullptr;
    napi_create_object(env, &jsPublishedDataItem);

    napi_value key = Convert2JSValue(env, publishedDataItem.key_);
    if (key == nullptr) {
        return nullptr;
    }

    napi_value subscriberId = nullptr;
    subscriberId = Convert2JSValue(env, std::to_string(publishedDataItem.subscriberId_));
    if (subscriberId == nullptr) {
        return nullptr;
    }

    napi_value data = nullptr;
    if (publishedDataItem.IsAshmem()) {
        data = Convert2JSValue(env, std::get<std::vector<uint8_t>>(publishedDataItem.GetData()), false);
    } else {
        data = Convert2JSValue(env, std::get<std::string>(publishedDataItem.GetData()));
    }
    if (data == nullptr) {
        return nullptr;
    }

    napi_set_named_property(env, jsPublishedDataItem, "key", key);
    napi_set_named_property(env, jsPublishedDataItem, "subscriberId", subscriberId);
    napi_set_named_property(env, jsPublishedDataItem, "data", data);
    return jsPublishedDataItem;
}

napi_value DataShareJSUtils::Convert2JSValue(napi_env env, std::vector<PublishedDataItem> &publishedDataItems)
{
    napi_value jsValue;
    napi_status status = napi_create_array_with_length(env, publishedDataItems.size(), &jsValue);
    if (status != napi_ok) {
        return nullptr;
    }

    for (size_t i = 0; i < publishedDataItems.size(); ++i) {
        napi_set_element(env, jsValue, i, Convert2JSValue(env, publishedDataItems[i]));
    }
    return jsValue;
}

napi_value DataShareJSUtils::Convert2JSValue(napi_env env, PublishedDataChangeNode &changeNode)
{
    napi_value jsPublishedDataChangeNode = nullptr;
    napi_create_object(env, &jsPublishedDataChangeNode);

    napi_value bundleName = nullptr;
    bundleName = Convert2JSValue(env, changeNode.ownerBundleName_);
    if (bundleName == nullptr) {
        return nullptr;
    }
    napi_value data = nullptr;
    data = Convert2JSValue(env, changeNode.datas_);
    if (data == nullptr) {
        return nullptr;
    }
    napi_set_named_property(env, jsPublishedDataChangeNode, "bundleName", bundleName);
    napi_set_named_property(env, jsPublishedDataChangeNode, "data", data);
    return jsPublishedDataChangeNode;
}

napi_value DataShareJSUtils::Convert2JSValue(napi_env env, const OperationResult &results)
{
    napi_value jsOperationResult = nullptr;
    napi_create_object(env, &jsOperationResult);

    napi_value key = nullptr;
    key = Convert2JSValue(env, results.key_);
    if (key == nullptr) {
        return nullptr;
    }

    napi_value result = nullptr;
    result = Convert2JSValue(env, results.errCode_);
    if (result == nullptr) {
        return nullptr;
    }
    napi_set_named_property(env, jsOperationResult, "key", key);
    napi_set_named_property(env, jsOperationResult, "result", result);
    return jsOperationResult;
}

napi_value DataShareJSUtils::Convert2JSValue(napi_env env, const std::vector<OperationResult> &results)
{
    napi_value jsValue;
    napi_status status = napi_create_array_with_length(env, results.size(), &jsValue);
    if (status != napi_ok) {
        return nullptr;
    }

    for (size_t i = 0; i < results.size(); ++i) {
        napi_set_element(env, jsValue, i, Convert2JSValue(env, results[i]));
    }
    return jsValue;
}

bool DataShareJSUtils::UnwrapTemplatePredicates(napi_env env, napi_value jsPredicates,
    std::vector<PredicateTemplateNode> &predicates)
{
    napi_value keys = nullptr;
    napi_get_property_names(env, jsPredicates, &keys);
    uint32_t arrLen = 0;
    napi_status status = napi_get_array_length(env, keys, &arrLen);
    if (status != napi_ok) {
        LOG_ERROR("UnwrapTemplatePredicates error");
        return false;
    }
    LOG_DEBUG("TemplatePredicates length : %{public}u", arrLen);
    for (size_t i = 0; i < arrLen; ++i) {
        napi_value key = nullptr;
        status = napi_get_element(env, keys, i, &key);
        if (status != napi_ok) {
            LOG_ERROR("UnwrapTemplatePredicates err");
            return false;
        }
        napi_value value = nullptr;
        status = napi_get_property(env, jsPredicates, key, &value);
        if (status != napi_ok) {
            LOG_ERROR("UnwrapTemplatePredicates err");
            return false;
        }
        std::string keyStr = UnwrapStringFromJS(env, key);
        std::string valueStr = UnwrapStringFromJS(env, value);
        PredicateTemplateNode node(keyStr, valueStr);
        predicates.emplace_back(node);
    }
    return true;
}

Template DataShareJSUtils::Convert2Template(napi_env env, napi_value value)
{
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, value, &valueType);
    if (valueType != napi_object) {
        LOG_ERROR("Convert2Template error, value is not object");
        return {};
    }
    napi_value jsPredicates;
    auto status =  napi_get_named_property(env, value, "predicates", &jsPredicates);
    if (status != napi_ok) {
        LOG_ERROR("Convert predicates failed");
        return {};
    }
    std::vector<PredicateTemplateNode> predicates;
    if (!UnwrapTemplatePredicates(env, jsPredicates, predicates)) {
        LOG_ERROR("UnwrapTemplateNodeVector failed");
        return {};
    }

    std::string scheduler;
    if (!UnwrapStringByPropertyName(env, value, "scheduler", scheduler)) {
        LOG_ERROR("Convert scheduler failed");
        return {};
    }
    Template tpl(predicates, scheduler);
    return tpl;
}

TemplateId DataShareJSUtils::Convert2TemplateId(napi_env env, napi_value value)
{
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, value, &valueType);
    if (valueType != napi_object) {
        LOG_ERROR("Convert2TemplateId error, value is not object");
        return {};
    }

    TemplateId templateId;
    std::string strSubId;
    if (!UnwrapStringByPropertyName(env, value, "subscriberId", strSubId)) {
        LOG_ERROR("Convert subscriberId failed");
        return {};
    }
    templateId.subscriberId_ = atoll(strSubId.c_str());
    if (!UnwrapStringByPropertyName(env, value, "bundleNameOfOwner", templateId.bundleName_)) {
        LOG_ERROR("Convert bundleNameOfOwner failed");
        return {};
    }
    return templateId;
}

bool DataShareJSUtils::UnwrapPublishedDataItem(napi_env env, napi_value jsObject, PublishedDataItem &publishedDataItem)
{
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, jsObject, &valueType);
    if (valueType != napi_object) {
        LOG_ERROR("UnwrapPublishedDataItem error, value is not object");
        return false;
    }

    if (!UnwrapStringByPropertyName(env, jsObject, "key", publishedDataItem.key_)) {
        LOG_ERROR("Convert key failed");
        return false;
    }
    std::string keyStr = "data";
    napi_value jsDataKey = Convert2JSValue(env, keyStr);
    napi_value jsDataValue = nullptr;
    napi_get_property(env, jsObject, jsDataKey, &jsDataValue);
    napi_typeof(env, jsDataValue, &valueType);
    PublishedDataItem::DataType value;
    if (valueType == napi_object) {
        value = Convert2U8Vector(env, jsDataValue);
        publishedDataItem.Set(value);
    } else if (valueType == napi_string) {
        value = Convert2String(env, jsDataValue);
        publishedDataItem.Set(value);
    } else {
        LOG_ERROR("Convert dataValue failed, type is %{public}d", valueType);
        return false;
    }
    std::string strSubId;
    if (!UnwrapStringByPropertyName(env, jsObject, "subscriberId", strSubId)) {
        LOG_ERROR("Convert subscriberId failed");
        return false;
    }
    publishedDataItem.subscriberId_ = atoll(strSubId.c_str());
    return true;
}

bool DataShareJSUtils::IsArrayForNapiValue(napi_env env, napi_value param, uint32_t &arraySize)
{
    bool isArray = false;
    arraySize = 0;

    if (napi_is_array(env, param, &isArray) != napi_ok || isArray == false) {
        return false;
    }

    if (napi_get_array_length(env, param, &arraySize) != napi_ok) {
        return false;
    }
    return true;
}

bool DataShareJSUtils::UnwrapPublishedDataItemVector(napi_env env, napi_value value,
    std::vector<PublishedDataItem> &publishedDataItems)
{
    uint32_t arraySize = 0;

    if (!IsArrayForNapiValue(env, value, arraySize)) {
        LOG_ERROR("IsArrayForNapiValue is false");
        return false;
    }

    for (uint32_t i = 0; i < arraySize; i++) {
        napi_value jsValue = nullptr;
        if (napi_get_element(env, value, i, &jsValue) != napi_ok) {
            LOG_ERROR("napi_get_element is false");
            return false;
        }

        PublishedDataItem publishedDataItem;
        if (!UnwrapPublishedDataItem(env, jsValue, publishedDataItem)) {
            LOG_ERROR("UnwrapPublishedDataItem failed");
            return false;
        }
        publishedDataItems.emplace_back(std::move(publishedDataItem));
    }
    return true;
}

Data DataShareJSUtils::Convert2PublishedData(napi_env env, napi_value value)
{
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, value, &valueType);
    if (valueType != napi_object) {
        LOG_ERROR("Convert2PublishedData error, value is not object");
        return {};
    }
    Data data;
    if (!UnwrapPublishedDataItemVector(env, value,  data.datas_)) {
        LOG_ERROR("UnwrapPublishedDataItems failed");
        return {};
    }
    return data;
}

bool DataShareJSUtils::UnwrapStringByPropertyName(
    napi_env env, napi_value jsObject, const char *propertyName, std::string &value)
{
    napi_value jsResult = nullptr;
    auto status = napi_get_named_property(env, jsObject, propertyName, &jsResult);
    if ((status != napi_ok) || (jsResult == nullptr)) {
        LOG_ERROR("Convert bundleNameOfOwner failed");
        return false;
    }
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, jsResult, &valueType);
    if (valueType != napi_string) {
        LOG_ERROR("Convert2PublishedData error, value is not object");
        return false;
    }
    value = DataShareJSUtils::Convert2String(env, jsResult);
    return true;
}

napi_value DataShareJSUtils::Convert2JSValue(napi_env env, const std::vector<BatchUpdateResult> &result)
{
    napi_value jsResult = nullptr;
    napi_status status = napi_create_object(env, &jsResult);
    if (status != napi_ok) {
        LOG_ERROR("Create object failed, ret : %{public}d", status);
        return nullptr;
    }
    for (const auto &valueArray : result) {
        napi_value values;
        if (napi_create_array(env, &values) != napi_ok) {
            LOG_ERROR("Create array failed");
            return nullptr;
        }
        uint32_t index = 0;
        for (const auto &value : valueArray.codes) {
            napi_value jsValue = Convert2JSValue(env, value);
            if (napi_set_element(env, values, index++, jsValue) != napi_ok) {
                LOG_ERROR("Set to array failed");
                return nullptr;
            }
        }
        if (napi_set_named_property(env, jsResult, valueArray.uri.c_str(), values) != napi_ok) {
            LOG_ERROR("Set to map failed");
            return nullptr;
        }
    }
    return jsResult;
}

int32_t DataShareJSUtils::Convert2Value(napi_env env, napi_value input, UpdateOperation& operation)
{
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, input, &valueType);
    if (valueType != napi_object) {
        LOG_ERROR("value is not object");
        return napi_invalid_arg;
    }
    if (Convert2Value(env, input, "predicates", operation.predicates) != napi_ok) {
        return napi_invalid_arg;
    }
    if (Convert2Value(env, input, "values", operation.valuesBucket) != napi_ok) {
        return napi_invalid_arg;
    }
    return napi_ok;
}

int32_t DataShareJSUtils::Convert2Value(napi_env env, napi_value input, std::string &str)
{
    size_t strBufferSize = DEFAULT_BUF_SIZE;
    napi_get_value_string_utf8(env, input, nullptr, 0, &strBufferSize);
    char *buf = new (std::nothrow) char[strBufferSize + 1];
    if (buf == nullptr) {
        return napi_invalid_arg;
    }
    size_t len = 0;
    napi_get_value_string_utf8(env, input, buf, strBufferSize + 1, &len);
    buf[len] = 0;
    str = std::string(buf);
    delete[] buf;
    return napi_ok;
}

int32_t DataShareJSUtils::Convert2Value(napi_env env, napi_value input,
    OHOS::DataShare::DataShareObserver::ChangeType &changeType)
{
    uint32_t number = 0;
    napi_status status = napi_get_value_uint32(env, input, &number);
    changeType = static_cast<OHOS::DataShare::DataShareObserver::ChangeType>(number);
    return status;
}

int32_t DataShareJSUtils::Convert2Value(napi_env env, napi_value input, DataShareObserver::ChangeInfo &changeInfo)
{
    napi_valuetype type = napi_undefined;
    napi_typeof(env, input, &type);
    if (type != napi_object) {
        LOG_ERROR("ChangeInfo is not object");
        return napi_invalid_arg;
    }
    std::string uriStr;
    std::vector<DataShareValuesBucket> valuebuckets = {};
    if (Convert2Value(env, input, "type", changeInfo.changeType_) != napi_ok) {
        return napi_invalid_arg;
    }
    if (Convert2Value(env, input, "uri", uriStr) != napi_ok) {
        return napi_invalid_arg;
    }
    if (Convert2Value(env, input, "values", valuebuckets) != napi_ok) {
        return napi_invalid_arg;
    }

    Uri uri(uriStr);
    changeInfo.uris_.push_back(uri);
    changeInfo.valueBuckets_ = ValueProxy::Convert(std::move(valuebuckets));
    return napi_ok;
}

napi_value DataShareJSUtils::Convert2JSValue(napi_env env, const DataShareObserver::ChangeInfo &changeInfo)
{
    napi_value napiValue = nullptr;
    napi_create_object(env, &napiValue);
    napi_value changeType = Convert2JSValue(env, changeInfo.changeType_);
    if (changeType == nullptr) {
        return nullptr;
    }
    napi_value uri = Convert2JSValue(env, changeInfo.uris_.front().ToString());
    if (uri == nullptr) {
        return nullptr;
    }
    auto &valBucket = const_cast<DataShareObserver::ChangeInfo &>(changeInfo);
    std::vector<DataShareValuesBucket> VBuckets = ValueProxy::Convert(std::move(valBucket.valueBuckets_));
    napi_value valueBuckets = Convert2JSValue(env, VBuckets);
    if (valueBuckets == nullptr) {
        return nullptr;
    }
    napi_set_named_property(env, napiValue, "type", changeType);
    napi_set_named_property(env, napiValue, "uri", uri);
    napi_set_named_property(env, napiValue, "values", valueBuckets);
    return napiValue;
}

bool DataShareJSUtils::UnwrapDataSharePredicates(napi_env env, napi_value value,
    DataSharePredicates &dataSharePredicates)
{
    auto predicates = DataSharePredicatesProxy::GetNativePredicates(env, value);
    if (predicates == nullptr) {
        LOG_ERROR("GetNativePredicates is nullptr.");
        return false;
    }
    dataSharePredicates = DataSharePredicates(predicates->GetOperationList());
    return true;
}

int32_t DataShareJSUtils::Convert2Value(napi_env env, napi_value input, DataSharePredicates &predicates)
{
    if (!UnwrapDataSharePredicates(env, input, predicates)) {
        LOG_ERROR("get predicates from js failed");
        return napi_invalid_arg;
    }
    return napi_ok;
}

int32_t DataShareJSUtils::Convert2Value(napi_env env, napi_value input, DataShareValuesBucket &valueBucket)
{
    if (!GetValueBucketObject(valueBucket, env, input)) {
        LOG_ERROR("get valueBucketObject from js failed");
        return napi_invalid_arg;
    }
    return napi_ok;
}
} // namespace DataShare
} // namespace OHOS