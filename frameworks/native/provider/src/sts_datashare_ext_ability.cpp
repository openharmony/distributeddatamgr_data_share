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

#include "sts_datashare_ext_ability.h"
#include "sts_datashare_ext_ability_context.h"
#include "datashare_stub_impl.h"
#include "datashare_log.h"
#include "datashare_predicates_proxy.h"
#include "dataobs_mgr_client.h"
#include "ability_context.h"
#include "ets_runtime.h"
#include "ani_common_want.h"
#include "wrapper.rs.h"

using namespace OHOS::DistributedShare::DataShare;

namespace OHOS {
namespace DataShare {
using namespace AbilityRuntime;
using namespace AppExecFwk;
using namespace DataShareAni;
using DataObsMgrClient = AAFwk::DataObsMgrClient;

namespace {
constexpr int INVALID_VALUE = -1;
}

StsDataShareExtAbility* StsDataShareExtAbility::Create(const std::unique_ptr<Runtime>& runtime)
{
    return new StsDataShareExtAbility(static_cast<ETSRuntime&>(*runtime));
}

StsDataShareExtAbility::StsDataShareExtAbility(ETSRuntime& stsRuntime) : stsRuntime_(stsRuntime) {}

StsDataShareExtAbility::~StsDataShareExtAbility()
{
    LOG_DEBUG("Js datashare extension destructor.");
}

void StsDataShareExtAbility::ResetEnv(ani_env *env)
{
    env->DescribeError();
    env->ResetError();
}

void StsDataShareExtAbility::Init(const std::shared_ptr<AbilityLocalRecord> &record,
    const std::shared_ptr<OHOSApplication> &application, std::shared_ptr<AbilityHandler> &handler,
    const sptr<IRemoteObject> &token)
{
    DataShareExtAbility::Init(record, application, handler, token);
    std::string srcPath(Extension::abilityInfo_->moduleName + "/");
    srcPath.append(Extension::abilityInfo_->srcEntrance);
    auto pos = srcPath.rfind(".");
    if (pos != std::string::npos) {
        srcPath.erase(pos);
        srcPath.append(".abc");
    }

    std::string moduleName(Extension::abilityInfo_->moduleName);
    moduleName.append("::").append(abilityInfo_->name);
    
    stsObj_ = stsRuntime_.LoadModule(
        moduleName, srcPath, abilityInfo_->hapPath, abilityInfo_->compileMode == CompileMode::ES_MODULE, false,
        abilityInfo_->srcEntrance);
    if (stsObj_ == nullptr) {
        LOG_ERROR("Failed to get stsObj_, moduleName:%{public}s.", moduleName.c_str());
        return;
    }

    auto env = stsRuntime_.GetAniEnv();
    if (env == nullptr) {
        LOG_ERROR("Failed to init ability, env is null");
        return;
    }

    auto context = GetContext();
    if (context == nullptr) {
        LOG_ERROR("Failed to get context, moduleName:%{public}s.", moduleName.c_str());
        return;
    }

    ani_object contextObj = CreateStsDataShareExtAbilityContext(env, context, application);
    if (contextObj == nullptr) {
        LOG_ERROR("Failed to get contextObj");
        return;
    }

    ani_field contextField;
    auto status = env->Class_FindField(stsObj_->aniCls, "context", &contextField);
    if (status != ANI_OK) {
        LOG_ERROR("Failed to get contextField, status:%{public}d", status);
        ResetEnv(env);
        return;
    }

    ani_ref contextRef = nullptr;
    if ((status = env->GlobalReference_Create(contextObj, &contextRef)) != ANI_OK) {
        LOG_ERROR("Failed to get contextRef, status:%{public}d", status);
        return;
    }

    if ((status = env->Object_SetField_Ref(stsObj_->aniObj, contextField, contextRef)) != ANI_OK) {
        LOG_ERROR("Failed to set contextField, status:%{public}d", status);
        ResetEnv(env);
    }
}

void StsDataShareExtAbility::OnStart(const AAFwk::Want &want)
{
    Extension::OnStart(want);
    ani_env *env = stsRuntime_.GetAniEnv();
    if (env == nullptr) {
        LOG_ERROR("OnUpdate failed, env is null");
        return;
    }
    std::shared_ptr<AsyncContext> asyncContext = std::make_shared<AsyncContext>();
    asyncContext->isNeedNotify_ = true;
    AsyncPoint *point = new (std::nothrow)AsyncPoint();
    if (point == nullptr) {
        LOG_ERROR("New AsyncPoint error.");
        return;
    }
    point->context = asyncContext;
    ani_object aniWant = WrapWant(env, want);

    call_arkts_on_create(reinterpret_cast<int64_t>(stsObj_->aniObj), reinterpret_cast<int64_t>(env),
        reinterpret_cast<int64_t>(aniWant), reinterpret_cast<int64_t>(point));
}

sptr<IRemoteObject> StsDataShareExtAbility::OnConnect(const AAFwk::Want &want)
{
    Extension::OnConnect(want);
    sptr<DataShareStubImpl> remoteObject = new (std::nothrow) DataShareStubImpl(
        std::static_pointer_cast<StsDataShareExtAbility>(shared_from_this()));
    if (remoteObject == nullptr) {
        LOG_ERROR("No memory allocated for DataShareStubImpl");
        return nullptr;
    }
    return remoteObject->AsObject();
}

void PushBucket(rust::Box<ValuesBucketHashWrap> &valuesBucket, const DataShareValuesBucket &inputVal)
{
    const auto &valuesMap = inputVal.valuesMap;
    for (auto it = valuesMap.begin(); it != valuesMap.end(); ++it) {
        std::string key = it->first;
        auto rawValue = it->second;
        auto index = rawValue.index();
        if (index == TYPE_NULL) {
            value_bucket_push_kv_null(*valuesBucket, rust::String(key));
            continue;
        }
        if (index == TYPE_INT) {
            auto value = std::get<int64_t>(rawValue);
            value_bucket_push_kv_f64(*valuesBucket, rust::String(key), value);
            continue;
        }
        if (index == TYPE_DOUBLE) {
            auto value = std::get<double>(rawValue);
            value_bucket_push_kv_f64(*valuesBucket, rust::String(key), value);
            continue;
        }
        if (index == TYPE_STRING) {
            auto value = std::get<std::string>(rawValue);
            value_bucket_push_kv_str(*valuesBucket, rust::String(key), rust::String(value));
            continue;
        }
        if (index == TYPE_BOOL) {
            auto value = std::get<bool>(rawValue);
            value_bucket_push_kv_boolean(*valuesBucket, rust::String(key), value);
            continue;
        }
        if (index == TYPE_BLOB) {
            auto value = std::get<std::vector<uint8_t>>(rawValue);
            rust::Vec<uint8_t> vec;
            for (auto val: value) {
                vec.push_back(val);
            }
            value_bucket_push_kv_uint8array(*valuesBucket, rust::String(key), vec);
            continue;
        }
        LOG_ERROR("Value type not supported.");
    }
}

void PushBucketsArray(rust::Box<ValuesBucketArrayWrap> &valueBucketsArray, const DataShareValuesBucket &inputVal)
{
    const auto &valuesMap = inputVal.valuesMap;
    for (auto it = valuesMap.begin(); it != valuesMap.end(); ++it) {
        std::string key = it->first;
        auto rawValue = it->second;
        auto index = rawValue.index();
        if (index == TYPE_NULL) {
            values_bucket_array_push_kv_null(*valueBucketsArray, rust::String(key), false);
            continue;
        }
        if (index == TYPE_INT) {
            auto value = std::get<int64_t>(rawValue);
            values_bucket_array_push_kv_f64(*valueBucketsArray, rust::String(key), (double)value, false);
            continue;
        }
        if (index == TYPE_DOUBLE) {
            auto value = std::get<double>(rawValue);
            values_bucket_array_push_kv_f64(*valueBucketsArray, rust::String(key), value, false);
            continue;
        }
        if (index == TYPE_STRING) {
            auto value = std::get<std::string>(rawValue);
            values_bucket_array_push_kv_str(*valueBucketsArray, rust::String(key), rust::String(value), false);
            continue;
        }
        if (index == TYPE_BOOL) {
            auto value = std::get<bool>(rawValue);
            values_bucket_array_push_kv_boolean(*valueBucketsArray, rust::String(key), value, false);
            continue;
        }
        if (index == TYPE_BLOB) {
            auto value = std::get<std::vector<uint8_t>>(rawValue);
            rust::Vec<uint8_t> vec;
            for (auto val: value) {
                vec.push_back(val);
            }
            values_bucket_array_push_kv_uint8array(*valueBucketsArray, rust::String(key), vec, false);
            continue;
        }
        LOG_ERROR("Value type not supported.");
    }
}

int StsDataShareExtAbility::Insert(const Uri &uri, const DataShareValuesBucket &value)
{
    int ret = INVALID_VALUE;
    ret = DataShareExtAbility::Insert(uri, value);
    ani_env *env = stsRuntime_.GetAniEnv();
    if (env == nullptr) {
        LOG_ERROR("Insert failed, env is null");
        return ret;
    }

    rust::Box<ValuesBucketHashWrap> valuesBucket = rust_create_values_bucket();
    PushBucket(valuesBucket, value);

    AsyncCallBackPoint *point = new (std::nothrow)AsyncCallBackPoint();
    if (point == nullptr) {
        LOG_ERROR("New AsyncCallBackPoint error.");
        return ret;
    }
    point->result = std::move(result_);
    call_arkts_insert(reinterpret_cast<int64_t>(stsObj_->aniObj), reinterpret_cast<int64_t>(env),
        rust::String(uri.ToString()), *valuesBucket, reinterpret_cast<int64_t>(point));
    return ret;
}

int StsDataShareExtAbility::Update(const Uri &uri, const DataSharePredicates &predicates,
    const DataShareValuesBucket &value)
{
    int ret = INVALID_VALUE;
    ret = DataShareExtAbility::Update(uri, predicates, value);
    ani_env *env = stsRuntime_.GetAniEnv();
    if (env == nullptr) {
        LOG_ERROR("Update failed, env is null");
        return ret;
    }

    DataSharePredicates *predicatesPtr = new (std::nothrow)DataSharePredicates(predicates);
    if (predicatesPtr == nullptr) {
        LOG_ERROR("Get predicatesPtr failed");
        return ret;
    }

    rust::Box<ValuesBucketHashWrap> valuesBucket = rust_create_values_bucket();
    PushBucket(valuesBucket, value);

    AsyncCallBackPoint *point = new (std::nothrow)AsyncCallBackPoint();
    if (point == nullptr) {
        LOG_ERROR("New AsyncCallBackPoint error.");
        delete predicatesPtr;
        return ret;
    }
    point->result = std::move(result_);

    call_arkts_update(reinterpret_cast<int64_t>(stsObj_->aniObj), reinterpret_cast<int64_t>(env),
        rust::String(uri.ToString()), reinterpret_cast<int64_t>(predicatesPtr), *valuesBucket,
        reinterpret_cast<int64_t>(point));
    return ret;
}

int StsDataShareExtAbility::Delete(const Uri &uri, const DataSharePredicates &predicates)
{
    int ret = INVALID_VALUE;
    ret = DataShareExtAbility::Delete(uri, predicates);
    ani_env *env = stsRuntime_.GetAniEnv();
    if (env == nullptr) {
        LOG_ERROR("Delete failed, env is null");
        return ret;
    }

    DataSharePredicates *predicatesPtr = new (std::nothrow)DataSharePredicates(predicates);
    if (predicatesPtr == nullptr) {
        LOG_ERROR("Get predicatesPtr failed");
        return ret;
    }

    AsyncCallBackPoint *point = new (std::nothrow)AsyncCallBackPoint();
    if (point == nullptr) {
        LOG_ERROR("New AsyncCallBackPoint error.");
        delete predicatesPtr;
        return ret;
    }
    point->result = std::move(result_);
    call_arkts_delete(reinterpret_cast<int64_t>(stsObj_->aniObj), reinterpret_cast<int64_t>(env),
        rust::String(uri.ToString()), reinterpret_cast<int64_t>(predicatesPtr), reinterpret_cast<int64_t>(point));
    return ret;
}

std::shared_ptr<DataShareResultSet> StsDataShareExtAbility::Query(const Uri &uri,
    const DataSharePredicates &predicates, std::vector<std::string> &columns, DatashareBusinessError &businessError)
{
    std::shared_ptr<DataShareResultSet> ret;
    ret = DataShareExtAbility::Query(uri, predicates, columns, businessError);
    ani_env *env = stsRuntime_.GetAniEnv();
    if (env == nullptr) {
        LOG_ERROR("Query failed, env is null");
        return ret;
    }

    DataSharePredicates *predicatesPtr = new (std::nothrow)DataSharePredicates(predicates);
    if (predicatesPtr == nullptr) {
        LOG_ERROR("Get predicatesPtr failed");
        return ret;
    }

    rust::Vec<rust::String> rustColumns;
    for (const auto &col : columns) {
        rustColumns.push_back(rust::String(col));
    }

    AsyncCallBackPoint *point = new (std::nothrow)AsyncCallBackPoint();
    if (point == nullptr) {
        LOG_ERROR("New AsyncCallBackPoint error.");
        delete predicatesPtr;
        return ret;
    }
    point->result = std::move(result_);
    call_arkts_query(reinterpret_cast<int64_t>(stsObj_->aniObj), reinterpret_cast<int64_t>(env),
        rust::String(uri.ToString()), reinterpret_cast<int64_t>(predicatesPtr), rustColumns,
        reinterpret_cast<int64_t>(point));
    return ret;
}

int StsDataShareExtAbility::BatchInsert(const Uri &uri, const std::vector<DataShareValuesBucket> &values)
{
    int ret = INVALID_VALUE;
    ret = DataShareExtAbility::BatchInsert(uri, values);
    ani_env *env = stsRuntime_.GetAniEnv();
    if (env == nullptr) {
        LOG_ERROR("BatchInsert failed, env is null");
        return ret;
    }

    rust::Box<ValuesBucketArrayWrap> valueBucketsArray = rust_create_values_bucket_array();
    for (const auto &value : values) {
        PushBucketsArray(valueBucketsArray, value);
    }

    AsyncCallBackPoint *point = new (std::nothrow)AsyncCallBackPoint();
    if (point == nullptr) {
        LOG_ERROR("New AsyncCallBackPoint error.");
        return ret;
    }
    point->result = std::move(result_);
    call_arkts_batch_insert(reinterpret_cast<int64_t>(stsObj_->aniObj), reinterpret_cast<int64_t>(env),
        rust::String(uri.ToString()), *valueBucketsArray, reinterpret_cast<int64_t>(point));
    return ret;
}


bool StsDataShareExtAbility::RegisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver)
{
    DataShareExtAbility::RegisterObserver(uri, dataObserver);
    auto obsMgrClient = DataObsMgrClient::GetInstance();
    if (obsMgrClient == nullptr) {
        LOG_ERROR("obsMgrClient is nullptr");
        return false;
    }

    ErrCode ret = obsMgrClient->RegisterObserver(uri, dataObserver);
    if (ret != ERR_OK) {
        LOG_ERROR("obsMgrClient->RegisterObserver error return %{public}d", ret);
        return false;
    }
    return true;
}

bool StsDataShareExtAbility::UnregisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver)
{
    DataShareExtAbility::UnregisterObserver(uri, dataObserver);
    auto obsMgrClient = DataObsMgrClient::GetInstance();
    if (obsMgrClient == nullptr) {
        LOG_ERROR("obsMgrClient is nullptr");
        return false;
    }

    ErrCode ret = obsMgrClient->UnregisterObserver(uri, dataObserver);
    if (ret != ERR_OK) {
        LOG_ERROR("obsMgrClient->UnregisterObserver error return %{public}d", ret);
        return false;
    }
    return true;
}

bool StsDataShareExtAbility::NotifyChange(const Uri &uri)
{
    DataShareExtAbility::NotifyChange(uri);
    auto obsMgrClient = DataObsMgrClient::GetInstance();
    if (obsMgrClient == nullptr) {
        LOG_ERROR("obsMgrClient is nullptr");
        return false;
    }

    ErrCode ret = obsMgrClient->NotifyChange(uri);
    if (ret != ERR_OK) {
        LOG_ERROR("obsMgrClient->NotifyChange error return %{public}d", ret);
        return false;
    }
    return true;
}

bool StsDataShareExtAbility::NotifyChangeWithUser(const Uri &uri, int32_t userId)
{
    auto obsMgrClient = DataObsMgrClient::GetInstance();
    if (obsMgrClient == nullptr) {
        LOG_ERROR("obsMgrClient is nullptr");
        return false;
    }
    ErrCode ret = obsMgrClient->NotifyChange(uri, userId);
    if (ret != ERR_OK) {
        LOG_ERROR("obsMgrClient->NotifyChange error return %{public}d", ret);
        return false;
    }
    return true;
}

void StsDataShareExtAbility::InitResult(std::shared_ptr<JsResult> result)
{
    result_ = result;
}

void StsDataShareExtAbility::SaveNewCallingInfo(ani_env *env)
{
    // todo
}

} // namespace DataShare
} // namespace OHOS