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

#define LOG_TAG "ANIDatashare"

#include "ani_base_context.h"
#include "datashare_ani.h"
#include "datashare_business_error.h"
#include "datashare_errno.h"
#include "datashare_log.h"
#include "datashare_predicates.h"
#include "datashare_result.h"
#include "datashare_string_utils.h"
#include "datashare_value_object.h"
#include "datashare_values_bucket.h"
#include "ikvstore_data_service.h"
#include "idata_share_service.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "js_proxy.h"
#include "system_ability_definition.h"
#include "tokenid_kit.h"
#include "wrapper.rs.h"
#include <map>
#define UNIMPL_RET_CODE 0

namespace OHOS {
using namespace DataShare;
using namespace DistributedShare::DataShare;
namespace DataShareAni {

static std::vector<std::string> convert_rust_vec_to_cpp_vector(const rust::Vec<rust::String>& rust_vec)
{
    std::vector<std::string> cpp_vector;
    for (const auto& rust_str : rust_vec) {
        cpp_vector.push_back(std::string(rust_str));
    }
    return cpp_vector;
}

static rust::Vec<rust::String> convert_cpp_vector_to_rust_vec(const std::vector<std::string>& cpp_vec)
{
    rust::Vec<rust::String> rust_vec;
    for (const auto &cpp_data : cpp_vec) {
        rust_vec.push_back(rust::String(cpp_data));
    }
    return rust_vec;
}

I64ResultWrap DataProxyHandleNativeCreate()
{
    auto ret = DataProxyHandle::Create();
    if (ret.second == nullptr || ret.first != E_OK) {
        LOG_ERROR("create dataProxyHandle failed, ret.first is %{public}d.", ret.first);
        return I64ResultWrap{0, EXCEPTION_INNER};
    }
    auto dataProxyHandleHolder = new DataProxyHandleHolder(ret.second);
    if (dataProxyHandleHolder == nullptr) {
        LOG_ERROR("new DataProxyHandleHolder failed.");
        return I64ResultWrap{0, EXCEPTION_INNER};
    }
    dataProxyHandleHolder->jsProxyDataObsManager_ =
        std::make_shared<AniProxyDataSubscriberManager>(dataProxyHandleHolder->dataProxyHandle_);
    return I64ResultWrap{reinterpret_cast<long long>(dataProxyHandleHolder), E_OK};
}

void CleanupDataProxyHandle(int64_t dataProxyHandlePtr)
{
    delete reinterpret_cast<DataProxyHandleHolder *>(dataProxyHandlePtr);
}

int DataShareNativeDataProxyHandleOnDataProxy(
    PtrWrap ptrWrap, rust::Vec<rust::String> uris, const AniDataProxyConfig& config, AniDataProxyResultSetParam& param)
{
    auto proxyHandleHolder = reinterpret_cast<DataProxyHandleHolder *>(ptrWrap.dataShareHelperPtr);
    if (proxyHandleHolder == nullptr || proxyHandleHolder->dataProxyHandle_ == nullptr) {
        LOG_ERROR("DataShareNativeDataProxyHandleOnDataProxy failed, helper is nullptr.");
        return EXCEPTION_HELPER_CLOSED;
    }
    int32_t type = data_share_data_proxy_config_get_type(config);
    int32_t maxValueLength = data_share_data_proxy_config_get_max_value_length(config);
    if (maxValueLength == INVALID_MAX_VALUE_LENGTH) {
        LOG_ERROR("Invalid maxValueLength");
        return EXCEPTION_PROXY_PARAMETER_CHECK;
    }
    DataProxyConfig proxyConfig;
    proxyConfig.type_ = static_cast<DataProxyType>(type);
    proxyConfig.maxValueLength_ = static_cast<DataProxyMaxValueLength>(maxValueLength);
    std::vector<DataProxyResult> results;
    auto curis = convert_rust_vec_to_cpp_vector(uris);
    if (proxyHandleHolder->jsProxyDataObsManager_ == nullptr) {
        LOG_ERROR("proxyHandleHolder->jsProxyDataObsManager_ is nullptr.");
        return E_OK;
    }
    results = proxyHandleHolder->jsProxyDataObsManager_->AddObservers(ptrWrap.callback, curis, proxyConfig);

    for (const auto &result : results) {
        data_proxy_result_set_push(param, rust::String(result.uri_), (int32_t)result.result_);
    }
    return E_OK;
}

int DataShareNativeDataProxyHandleOffDataProxy(PtrWrap ptrWrap, rust::Vec<rust::String> uris,
    const AniDataProxyConfig& config, AniDataProxyResultSetParam& param)
{
    auto proxyHandleHolder = reinterpret_cast<DataProxyHandleHolder *>(ptrWrap.dataShareHelperPtr);
    if (proxyHandleHolder == nullptr || proxyHandleHolder->dataProxyHandle_ == nullptr) {
        LOG_ERROR("DataShareNativeDataProxyHandleOffDataProxy failed, helper is nullptr.");
        return EXCEPTION_HELPER_CLOSED;
    }
    int32_t type = data_share_data_proxy_config_get_type(config);
    int32_t maxValueLength = data_share_data_proxy_config_get_max_value_length(config);
    if (maxValueLength == INVALID_MAX_VALUE_LENGTH) {
        LOG_ERROR("Invalid maxValueLength");
        return EXCEPTION_PROXY_PARAMETER_CHECK;
    }
    DataProxyConfig proxyConfig;
    proxyConfig.type_ = static_cast<DataProxyType>(type);
    proxyConfig.maxValueLength_ = static_cast<DataProxyMaxValueLength>(maxValueLength);
    auto curis = convert_rust_vec_to_cpp_vector(uris);
    std::vector<DataProxyResult> results;
    if (proxyHandleHolder->jsProxyDataObsManager_ == nullptr) {
        LOG_ERROR("proxyHandleHolder->jsProxyDataObsManager_ is nullptr.");
        return E_OK;
    }
    results = proxyHandleHolder->jsProxyDataObsManager_->DelObservers(ptrWrap.callback, curis);
    for (const auto &result : results) {
        data_proxy_result_set_push(param, rust::String(result.uri_), (int32_t)result.result_);
    }
    return E_OK;
}

int DataShareNativeDataProxyHandleOffDataProxyNone(int64_t dataShareProxyHandlePtr,
    rust::Vec<rust::String> uris, const AniDataProxyConfig& config, AniDataProxyResultSetParam& param)
{
    auto proxyHandleHolder = reinterpret_cast<DataProxyHandleHolder *>(dataShareProxyHandlePtr);
    if (proxyHandleHolder == nullptr || proxyHandleHolder->dataProxyHandle_ == nullptr) {
        LOG_ERROR("DataShareNativeDataProxyHandleOffDataProxyNone failed, helper is nullptr.");
        return EXCEPTION_HELPER_CLOSED;
    }
    int32_t type = data_share_data_proxy_config_get_type(config);
    int32_t maxValueLength = data_share_data_proxy_config_get_max_value_length(config);
    if (maxValueLength == INVALID_MAX_VALUE_LENGTH) {
        LOG_ERROR("Invalid maxValueLength");
        return EXCEPTION_PROXY_PARAMETER_CHECK;
    }
    DataProxyConfig proxyConfig;
    proxyConfig.type_ = static_cast<DataProxyType>(type);
    proxyConfig.maxValueLength_ = static_cast<DataProxyMaxValueLength>(maxValueLength);
    auto curis = convert_rust_vec_to_cpp_vector(uris);
    std::vector<DataProxyResult> results;
    if (proxyHandleHolder->jsProxyDataObsManager_ == nullptr) {
        LOG_ERROR("proxyHandleHolder->jsProxyDataObsManager_ is nullptr.");
        return E_OK;
    }
    results = proxyHandleHolder->jsProxyDataObsManager_->DelObservers(curis);
    for (const auto &result : results) {
        data_proxy_result_set_push(param, rust::String(result.uri_), (int32_t)result.result_);
    }
    return E_OK;
}

static void ValidateAllowListProperty(std::vector<std::string> &list, const std::string &propertyName,
    const std::string &uri)
{
    auto it = list.begin();
    while (it != list.end()) {
        if (it->size() > APPIDENTIFIER_MAX_SIZE) {
            LOG_WARN("%{public}s item is over limit", propertyName.c_str());
            it = list.erase(it);
        } else {
            ++it;
        }
    }
    if (list.size() > ALLOW_LIST_MAX_COUNT) {
        LOG_WARN("ProxyData's %{public}s is over limit, uri: %{public}s",
            propertyName.c_str(), DataShareStringUtils::Anonymous(uri).c_str());
        list.resize(ALLOW_LIST_MAX_COUNT);
    }
}

bool ConvertMultiValues(const AniProxyData &item, std::map<std::string, DataProxyValue>& innerMap)
{
    size_t valuesSize = ani_proxy_data_get_values_size(item);
    for (size_t i = 0; i < valuesSize; ++i) {
        std::string key = std::string(ani_proxy_data_get_values_key_at(item, i));
        EnumType valueType = ani_proxy_data_get_values_type_at(item, i);
        DataProxyValue proxyValue;
        switch (valueType) {
            case EnumType::StringType:
                proxyValue = std::string(ani_proxy_data_get_values_string_at(item, i));
                break;
            case EnumType::F64Type:
                proxyValue = static_cast<double>(ani_proxy_data_get_values_f64_at(item, i));
                break;
            case EnumType::BooleanType:
                proxyValue = static_cast<bool>(ani_proxy_data_get_values_bool_at(item, i));
                break;
            case EnumType::I64Type:
                proxyValue = static_cast<int64_t>(ani_proxy_data_get_values_i64_at(item, i));
                break;
            default:
                LOG_ERROR("ConvertMultiValues type err at index %{public}zu", i);
                return false;
        }
        innerMap[key] = proxyValue;
    }
    return true;
}

bool ConvertProxyDataVec(const rust::Vec<AniProxyData>& proxydata,
    std::vector<DataShareProxyData>& vec_proxyData)
{
    for (const auto &item : proxydata) {
        EnumType type = ani_proxy_data_get_enum_type(item);
        DataShareProxyData dspd;
        switch (type) {
            case EnumType::StringType:
                dspd.value_ = std::string(ani_proxy_data_get_value_string(item));
                break;
            case EnumType::F64Type:
                dspd.value_ = static_cast<double>(ani_proxy_data_get_value_f64(item));
                break;
            case EnumType::BooleanType:
                dspd.value_ = static_cast<bool>(ani_proxy_data_get_value_boolean(item));
                break;
            case EnumType::I64Type:
                dspd.value_ = static_cast<int64_t>(ani_proxy_data_get_value_i64(item));
                break;
            case EnumType::NullType:
                dspd.isValueUndefined = true;
                break;
            default:
                dspd.isValueUndefined = true;
                break;
        }
        dspd.uri_ = std::string(ani_proxy_data_get_uri(item));

        rust::Vec<rust::String> allowListData;
        dspd.isAllowListUndefined = ani_proxy_data_get_data(item, allowListData);
        dspd.allowList_ = convert_rust_vec_to_cpp_vector(allowListData);
        ValidateAllowListProperty(dspd.allowList_, "allowList", dspd.uri_);

        dspd.isMultiValues_ = ani_proxy_data_get_is_multi_values(item);
        if (dspd.isMultiValues_) {
            if (!ConvertMultiValues(item, dspd.multiValues_["appIdentifier"])) {
                return false;
            }
        }

        rust::Vec<rust::String> trustProvidersData;
        dspd.isTrustProvidersUndefined = ani_proxy_data_get_trust_providers(item, trustProvidersData);
        dspd.trustProviders_ = convert_rust_vec_to_cpp_vector(trustProvidersData);
        ValidateAllowListProperty(dspd.trustProviders_, "trustProviders", dspd.uri_);

        vec_proxyData.push_back(dspd);
    }
    return true;
}

int DataShareNativeDataProxyHandlePublish(int64_t dataShareProxyHandlePtr, rust::Vec<AniProxyData> proxydata,
    const AniDataProxyConfig& config, AniDataProxyResultSetParam& param)
{
    std::vector<DataShareProxyData> vec_proxyData;
    if (!ConvertProxyDataVec(proxydata, vec_proxyData)) {
        LOG_ERROR("ConvertProxyDataVec failed");
        return EXCEPTION_PROXY_PARAMETER_CHECK;
    }
    int32_t type = data_share_data_proxy_config_get_type(config);
    int32_t maxValueLength = data_share_data_proxy_config_get_max_value_length(config);
    if (maxValueLength == INVALID_MAX_VALUE_LENGTH) {
        LOG_ERROR("Invalid maxValueLength");
        return EXCEPTION_PROXY_PARAMETER_CHECK;
    }
    DataProxyConfig proxyConfig;
    proxyConfig.type_ = static_cast<DataProxyType>(type);
    proxyConfig.maxValueLength_ = static_cast<DataProxyMaxValueLength>(maxValueLength);
    auto handlePtr = reinterpret_cast<DataProxyHandleHolder*>(dataShareProxyHandlePtr);
    if (handlePtr == nullptr || handlePtr->dataProxyHandle_ == nullptr) {
        LOG_ERROR("DataShareNativeDataProxyHandlePublish failed, dataShareProxyHandlePtr is nullptr.");
        return EXCEPTION_INNER;
    }
    auto results = handlePtr->dataProxyHandle_->PublishProxyData(vec_proxyData, proxyConfig);
    for (const auto &result : results) {
        data_proxy_result_set_push(param, rust::String(result.uri_), static_cast<int32_t>(result.result_));
    }
    for (const auto &result : results) {
        auto code = static_cast<DataProxyErrorCode>(result.result_);
        if (code > DataProxyErrorCode::OVER_LIMIT) {
            if (code == DataProxyErrorCode::INCOMPATIBLE_CONFIG_TYPE
                || code == DataProxyErrorCode::KEY_NOT_EXIST) {
                return EXCEPTION_PROXY_PARAMETER_CHECK;
            }
            return EXCEPTION_INNER;
        }
    }
    return E_OK;
}

int DataShareNativeDataProxyHandleDelete(int64_t dataShareProxyHandlePtr, rust::Vec<rust::String> uris,
    const AniDataProxyConfig& config, AniDataProxyResultSetParam& param)
{
    auto curis = convert_rust_vec_to_cpp_vector(uris);
    int32_t type = data_share_data_proxy_config_get_type(config);
    int32_t maxValueLength = data_share_data_proxy_config_get_max_value_length(config);
    if (maxValueLength == INVALID_MAX_VALUE_LENGTH) {
        LOG_ERROR("Invalid maxValueLength");
        return EXCEPTION_PROXY_PARAMETER_CHECK;
    }
    DataProxyConfig proxyConfig;
    proxyConfig.type_ = static_cast<DataProxyType>(type);
    proxyConfig.maxValueLength_ = static_cast<DataProxyMaxValueLength>(maxValueLength);
    auto handlePtr = reinterpret_cast<DataProxyHandleHolder*>(dataShareProxyHandlePtr);
    if (handlePtr == nullptr || handlePtr->dataProxyHandle_ == nullptr) {
        LOG_ERROR("DataShareNativeDataProxyHandleDelete failed, dataShareProxyHandlePtr is nullptr.");
        return EXCEPTION_INNER;
    }
    auto results = handlePtr->dataProxyHandle_->DeleteProxyData(curis, proxyConfig);
    for (const auto &result : results) {
        data_proxy_result_set_push(param, rust::String(result.uri_), (int32_t)result.result_);
    }
    return E_OK;
}

int DataShareNativeDataProxyHandleDeleteAll(int64_t dataShareProxyHandlePtr, const AniDataProxyConfig& config,
    AniDataProxyResultSetParam& param)
{
    int32_t type = data_share_data_proxy_config_get_type(config);
    int32_t maxValueLength = data_share_data_proxy_config_get_max_value_length(config);
    if (maxValueLength == INVALID_MAX_VALUE_LENGTH) {
        LOG_ERROR("Invalid maxValueLength");
        return EXCEPTION_PROXY_PARAMETER_CHECK;
    }
    DataProxyConfig proxyConfig;
    proxyConfig.type_ = static_cast<DataProxyType>(type);
    proxyConfig.maxValueLength_ = static_cast<DataProxyMaxValueLength>(maxValueLength);
    auto handlePtr = reinterpret_cast<DataProxyHandleHolder*>(dataShareProxyHandlePtr);
    if (handlePtr == nullptr || handlePtr->dataProxyHandle_ == nullptr) {
        LOG_ERROR("DataShareNativeDataProxyHandleDeleteAll failed, dataShareProxyHandlePtr is nullptr.");
        return EXCEPTION_INNER;
    }
    auto results = handlePtr->dataProxyHandle_->DeleteProxyData(proxyConfig);
    for (const auto &result : results) {
        data_proxy_result_set_push(param, rust::String(result.uri_), (int32_t)result.result_);
    }
    return E_OK;
}

int DataShareNativeDataProxyHandleGet(int64_t dataShareProxyHandlePtr, rust::Vec<rust::String> uris,
    const AniDataProxyConfig& config, AniDataProxyGetResultSetParam& param)
{
    auto handlePtr = reinterpret_cast<DataProxyHandleHolder*>(dataShareProxyHandlePtr);
    if (handlePtr == nullptr || handlePtr->dataProxyHandle_ == nullptr) {
        LOG_ERROR("DataShareNativeDataProxyHandleGet failed, dataShareProxyHandlePtr is nullptr.");
        return EXCEPTION_INNER;
    }
    auto curis = convert_rust_vec_to_cpp_vector(uris);
    int32_t type = data_share_data_proxy_config_get_type(config);
    int32_t maxValueLength = data_share_data_proxy_config_get_max_value_length(config);
    if (maxValueLength == INVALID_MAX_VALUE_LENGTH) {
        LOG_ERROR("Invalid maxValueLength");
        return EXCEPTION_PROXY_PARAMETER_CHECK;
    }
    DataProxyConfig proxyConfig;
    proxyConfig.type_ = static_cast<DataProxyType>(type);
    proxyConfig.maxValueLength_ = static_cast<DataProxyMaxValueLength>(maxValueLength);
    auto results = handlePtr->dataProxyHandle_->GetProxyData(curis, proxyConfig);
    for (const auto &result : results) {
        if (std::holds_alternative<int64_t>(result.value_)) {
            data_proxy_get_result_set_push_i64(param, result.uri_, (int32_t)result.result_,
                std::get<int64_t>(result.value_), convert_cpp_vector_to_rust_vec(result.allowList_));
        } else if (std::holds_alternative<double>(result.value_)) {
            data_proxy_get_result_set_push_f64(param, result.uri_, (int32_t)result.result_,
                std::get<double>(result.value_), convert_cpp_vector_to_rust_vec(result.allowList_));
        } else if (std::holds_alternative<bool>(result.value_)) {
            data_proxy_get_result_set_push_bool(param, result.uri_, (int32_t)result.result_,
                std::get<bool>(result.value_), convert_cpp_vector_to_rust_vec(result.allowList_));
        } else if (std::holds_alternative<std::string>(result.value_)) {
            data_proxy_get_result_set_push_string(param, result.uri_, (int32_t)result.result_,
                rust::String(std::get<std::string>(result.value_)), convert_cpp_vector_to_rust_vec(result.allowList_));
        }
    }
    return E_OK;
}

int ResolveDataProxyErrorCode(DataProxyErrorCode err)
{
    if (err == DataProxyErrorCode::SUCCESS) {
        return E_OK;
    }
    if (err == DataProxyErrorCode::URI_NOT_EXIST) {
        return EXCEPTION_URI_NOT_EXIST;
    }
    if (err == DataProxyErrorCode::NO_PERMISSION) {
        return EXCEPTION_NO_PERMISSION_ACCESS_URI;
    }
    if (err == DataProxyErrorCode::OVER_LIMIT || err == DataProxyErrorCode::INCOMPATIBLE_CONFIG_TYPE
        || err == DataProxyErrorCode::KEY_NOT_EXIST) {
        return EXCEPTION_PROXY_PARAMETER_CHECK;
    }
    if (err == DataProxyErrorCode::INNER_ERROR) {
        return EXCEPTION_INNER;
    }
    LOG_ERROR("Unknown DataProxyErrorCode: %{public}d", static_cast<int32_t>(err));
    return EXCEPTION_INNER;
}

static bool ConvertValueTypeToDataProxyValue(const ValueType& value, DataProxyValue& proxyValue)
{
    EnumType type = value_type_get_type(value);
    switch (type) {
        case EnumType::StringType:
            proxyValue = std::string(value_type_get_string(value));
            return true;
        case EnumType::F64Type:
            proxyValue = static_cast<double>(value_type_get_f64(value));
            return true;
        case EnumType::BooleanType:
            proxyValue = static_cast<bool>(value_type_get_bool(value));
            return true;
        case EnumType::I64Type:
            proxyValue = static_cast<int64_t>(value_type_get_i64(value));
            return true;
        default:
            LOG_ERROR("ConvertValueTypeToDataProxyValue type err: %{public}d", static_cast<int32_t>(type));
            return false;
    }
}

int DataShareNativeDataProxyHandlePutValue(int64_t dataShareProxyHandlePtr, rust::String uri,
    int32_t key, const ValueType& value, const AniDataProxyConfig& config)
{
    auto handlePtr = reinterpret_cast<DataProxyHandleHolder*>(dataShareProxyHandlePtr);
    if (handlePtr == nullptr || handlePtr->dataProxyHandle_ == nullptr) {
        LOG_ERROR("DataShareNativeDataProxyHandlePutValue failed, dataShareProxyHandlePtr is nullptr.");
        return EXCEPTION_INNER;
    }
    int32_t type = data_share_data_proxy_config_get_type(config);
    int32_t maxValueLength = data_share_data_proxy_config_get_max_value_length(config);
    if (maxValueLength == INVALID_MAX_VALUE_LENGTH) {
        LOG_ERROR("Invalid maxValueLength");
        return EXCEPTION_PROXY_PARAMETER_CHECK;
    }
    DataProxyConfig proxyConfig;
    proxyConfig.type_ = static_cast<DataProxyType>(type);
    proxyConfig.maxValueLength_ = static_cast<DataProxyMaxValueLength>(maxValueLength);
    DataProxyValue cppValue;
    if (!ConvertValueTypeToDataProxyValue(value, cppValue)) {
        LOG_ERROR("ConvertValueTypeToDataProxyValue failed");
        return EXCEPTION_PROXY_PARAMETER_CHECK;
    }
    auto result = handlePtr->dataProxyHandle_->PutValue(
        std::string(uri), std::to_string(key), cppValue, proxyConfig);
    return ResolveDataProxyErrorCode(result.result_);
}

int DataShareNativeDataProxyHandleRemoveValue(int64_t dataShareProxyHandlePtr, rust::String uri,
    int32_t key, const AniDataProxyConfig& config)
{
    auto handlePtr = reinterpret_cast<DataProxyHandleHolder*>(dataShareProxyHandlePtr);
    if (handlePtr == nullptr || handlePtr->dataProxyHandle_ == nullptr) {
        LOG_ERROR("DataShareNativeDataProxyHandleRemoveValue failed, dataShareProxyHandlePtr is nullptr.");
        return EXCEPTION_INNER;
    }
    int32_t type = data_share_data_proxy_config_get_type(config);
    int32_t maxValueLength = data_share_data_proxy_config_get_max_value_length(config);
    if (maxValueLength == INVALID_MAX_VALUE_LENGTH) {
        LOG_ERROR("Invalid maxValueLength");
        return EXCEPTION_PROXY_PARAMETER_CHECK;
    }
    DataProxyConfig proxyConfig;
    proxyConfig.type_ = static_cast<DataProxyType>(type);
    proxyConfig.maxValueLength_ = static_cast<DataProxyMaxValueLength>(maxValueLength);
    auto result = handlePtr->dataProxyHandle_->RemoveValue(
        std::string(uri), std::to_string(key), proxyConfig);
    return ResolveDataProxyErrorCode(result.result_);
}

int DataShareNativeDataProxyHandleGetValues(int64_t dataShareProxyHandlePtr, rust::String uri,
    const AniDataProxyConfig& config, AniDataProxyGetValuesResultParam& param)
{
    auto handlePtr = reinterpret_cast<DataProxyHandleHolder*>(dataShareProxyHandlePtr);
    if (handlePtr == nullptr || handlePtr->dataProxyHandle_ == nullptr) {
        LOG_ERROR("DataShareNativeDataProxyHandleGetValues failed, dataShareProxyHandlePtr is nullptr.");
        return EXCEPTION_INNER;
    }
    int32_t type = data_share_data_proxy_config_get_type(config);
    int32_t maxValueLength = data_share_data_proxy_config_get_max_value_length(config);
    if (maxValueLength == INVALID_MAX_VALUE_LENGTH) {
        LOG_ERROR("Invalid maxValueLength");
        return EXCEPTION_PROXY_PARAMETER_CHECK;
    }
    DataProxyConfig proxyConfig;
    proxyConfig.type_ = static_cast<DataProxyType>(type);
    proxyConfig.maxValueLength_ = static_cast<DataProxyMaxValueLength>(maxValueLength);
    auto result = handlePtr->dataProxyHandle_->GetValues(std::string(uri), proxyConfig);
    int32_t errCode = ResolveDataProxyErrorCode(result.result_);
    if (errCode != E_OK) {
        return errCode;
    }
    for (const auto &value : result.multiValues_) {
        if (std::holds_alternative<int64_t>(value)) {
            data_proxy_get_values_push_i64(param, std::get<int64_t>(value));
        } else if (std::holds_alternative<double>(value)) {
            data_proxy_get_values_push_f64(param, std::get<double>(value));
        } else if (std::holds_alternative<bool>(value)) {
            data_proxy_get_values_push_bool(param, std::get<bool>(value));
        } else if (std::holds_alternative<std::string>(value)) {
            data_proxy_get_values_push_string(param, rust::String(std::get<std::string>(value)));
        }
    }
    return E_OK;
}

} // namespace DataShareAni
} // namespace OHOS
