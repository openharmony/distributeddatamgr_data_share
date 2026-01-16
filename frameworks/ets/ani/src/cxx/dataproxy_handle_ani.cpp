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
    PtrWrap ptrWrap, rust::Vec<rust::String> uris, AniDataProxyResultSetParam& param)
{
    auto proxyHandleHolder = reinterpret_cast<DataProxyHandleHolder *>(ptrWrap.dataShareHelperPtr);
    if (proxyHandleHolder == nullptr || proxyHandleHolder->dataProxyHandle_ == nullptr) {
        LOG_ERROR("DataShareNativeDataProxyHandleOnDataProxy failed, helper is nullptr.");
        return EXCEPTION_HELPER_CLOSED;
    }
    std::vector<DataProxyResult> results;
    auto curis = convert_rust_vec_to_cpp_vector(uris);
    if (proxyHandleHolder->jsProxyDataObsManager_ == nullptr) {
        LOG_ERROR("proxyHandleHolder->jsProxyDataObsManager_ is nullptr.");
        return E_OK;
    }
    results = proxyHandleHolder->jsProxyDataObsManager_->AddObservers(ptrWrap.callback, curis);

    for (const auto &result : results) {
        data_proxy_result_set_push(param, rust::String(result.uri_), (int32_t)result.result_);
    }
    return E_OK;
}

int DataShareNativeDataProxyHandleOffDataProxy(PtrWrap ptrWrap, rust::Vec<rust::String> uris,
    AniDataProxyResultSetParam& param)
{
    auto proxyHandleHolder = reinterpret_cast<DataProxyHandleHolder *>(ptrWrap.dataShareHelperPtr);
    if (proxyHandleHolder == nullptr || proxyHandleHolder->dataProxyHandle_ == nullptr) {
        LOG_ERROR("DataShareNativeDataProxyHandleOffDataProxy failed, helper is nullptr.");
        return EXCEPTION_HELPER_CLOSED;
    }
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
    rust::Vec<rust::String> uris, AniDataProxyResultSetParam& param)
{
    auto proxyHandleHolder = reinterpret_cast<DataProxyHandleHolder *>(dataShareProxyHandlePtr);
    if (proxyHandleHolder == nullptr || proxyHandleHolder->dataProxyHandle_ == nullptr) {
        LOG_ERROR("DataShareNativeDataProxyHandleOffDataProxyNone failed, helper is nullptr.");
        return EXCEPTION_HELPER_CLOSED;
    }
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

std::vector<DataShareProxyData> ConvertProxyDataVec(const rust::Vec<AniProxyData>& proxydata)
{
    std::vector<DataShareProxyData> vec_proxyData;
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
        rust::Vec<rust::String> data;
        dspd.isAllowListUndefined = ani_proxy_data_get_data(item, data);
        dspd.allowList_ = convert_rust_vec_to_cpp_vector(data);
        auto it = dspd.allowList_.begin();
        while (it != dspd.allowList_.end()) {
            if (it->size() > APPIDENTIFIER_MAX_SIZE) {
                LOG_WARN("appIdentifier is over limit");
                it = dspd.allowList_.erase(it);
            } else {
                ++it;
            }
        }
        if (dspd.allowList_.size() > ALLOW_LIST_MAX_COUNT) {
            LOG_WARN("ProxyData's allowList is over limit, uri: %{public}s", dspd.uri_.c_str());
            dspd.allowList_.resize(ALLOW_LIST_MAX_COUNT);
        }
        vec_proxyData.push_back(dspd);
    }
    return vec_proxyData;
}

int DataShareNativeDataProxyHandlePublish(int64_t dataShareProxyHandlePtr, rust::Vec<AniProxyData> proxydata,
    const AniDataProxyConfig& config, AniDataProxyResultSetParam& param)
{
    std::vector<DataShareProxyData> vec_proxyData = ConvertProxyDataVec(proxydata);
    DataProxyConfig proxyConfig;
    int32_t type = data_share_data_proxy_config_get_type(config);
    proxyConfig.type_ = static_cast<DataProxyType>(type);
    auto handlePtr = reinterpret_cast<DataProxyHandleHolder*>(dataShareProxyHandlePtr);
    if (handlePtr == nullptr || handlePtr->dataProxyHandle_ == nullptr) {
        LOG_ERROR("DataShareNativeDataProxyHandlePublish failed, dataShareProxyHandlePtr is nullptr.");
        return EXCEPTION_INNER;
    }
    auto results = handlePtr->dataProxyHandle_->PublishProxyData(vec_proxyData, proxyConfig);
    for (const auto &result : results) {
        data_proxy_result_set_push(param, rust::String(result.uri_), static_cast<int32_t>(result.result_));
    }
    return E_OK;
}

int DataShareNativeDataProxyHandleDelete(int64_t dataShareProxyHandlePtr, rust::Vec<rust::String> uris,
    const AniDataProxyConfig& config, AniDataProxyResultSetParam& param)
{
    auto curis = convert_rust_vec_to_cpp_vector(uris);
    DataProxyConfig proxyConfig;
    int32_t type = data_share_data_proxy_config_get_type(config);
    proxyConfig.type_ = (DataProxyType)type;
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

int DataShareNativeDataProxyHandleGet(int64_t dataShareProxyHandlePtr, rust::Vec<rust::String> uris,
    const AniDataProxyConfig& config, AniDataProxyGetResultSetParam& param)
{
    auto curis = convert_rust_vec_to_cpp_vector(uris);
    DataProxyConfig proxyConfig;
    int32_t type = data_share_data_proxy_config_get_type(config);
    proxyConfig.type_ = (DataProxyType)type;
    auto results = DataProxyHandle::GetProxyData(curis, proxyConfig);
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

} // namespace DataShareAni
} // namespace OHOS
