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

#include "js_datashare_ext_ability.h"

#include "ability_info.h"
#include "dataobs_mgr_client.h"
#include "datashare_log.h"
#include "datashare_predicates_proxy.h"
#include "datashare_stub_impl.h"
#include "ikvstore_data_service.h"
#include "idata_share_service.h"
#include "iservice_registry.h"
#include "js_datashare_ext_ability_context.h"
#include "js_proxy.h"
#include "js_runtime.h"
#include "js_runtime_utils.h"
#include "napi_common_util.h"
#include "napi_common_want.h"
#include "napi_datashare_values_bucket.h"
#include "napi_remote_object.h"
#include "system_ability_definition.h"

using namespace OHOS::DistributedShare::DataShare;

namespace OHOS {
namespace DataShare {
using namespace AbilityRuntime;
namespace {
constexpr int INVALID_VALUE = -1;
static constexpr int32_t MAX_ARGC = 6;
static constexpr int32_t MIN_ARGC = 2;
constexpr const char ASYNC_CALLBACK_NAME[] = "AsyncCallback";
constexpr int CALLBACK_LENGTH = sizeof(ASYNC_CALLBACK_NAME) - 1;
}

bool MakeNapiColumn(napi_env env, napi_value &napiColumns, const std::vector<std::string> &columns);

using namespace OHOS::AppExecFwk;
using DataObsMgrClient = OHOS::AAFwk::DataObsMgrClient;

JsDataShareExtAbility* JsDataShareExtAbility::Create(const std::unique_ptr<Runtime>& runtime)
{
    return new JsDataShareExtAbility(static_cast<JsRuntime&>(*runtime));
}

JsDataShareExtAbility::JsDataShareExtAbility(JsRuntime& jsRuntime) : jsRuntime_(jsRuntime) {}

JsDataShareExtAbility::~JsDataShareExtAbility()
{
    LOG_DEBUG("Js datashare extension destructor.");
    jsRuntime_.FreeNativeReference(std::move(jsObj_));
}

void JsDataShareExtAbility::Init(const std::shared_ptr<AbilityLocalRecord> &record,
    const std::shared_ptr<OHOSApplication> &application, std::shared_ptr<AbilityHandler> &handler,
    const sptr<IRemoteObject> &token)
{
    DataShareExtAbility::Init(record, application, handler, token);
    std::string srcPath = "";
    GetSrcPath(srcPath);
    if (srcPath.empty()) {
        LOG_ERROR("Failed to get srcPath");
        return;
    }

    std::string moduleName(Extension::abilityInfo_->moduleName);
    moduleName.append("::").append(abilityInfo_->name);
    LOG_DEBUG("module:%{public}s, srcPath:%{public}s.", moduleName.c_str(), srcPath.c_str());
    HandleScope handleScope(jsRuntime_);
    napi_env env = jsRuntime_.GetNapiEnv();

    jsObj_ = jsRuntime_.LoadModule(
        moduleName, srcPath, abilityInfo_->hapPath, abilityInfo_->compileMode == CompileMode::ES_MODULE);
    if (jsObj_ == nullptr) {
        LOG_ERROR("Failed to get jsObj_, moduleName:%{public}s.", moduleName.c_str());
        return;
    }
    napi_value obj = jsObj_->GetNapiValue();
    if (obj == nullptr) {
        LOG_ERROR("Failed to get JsDataShareExtAbility object, moduleName:%{public}s.", moduleName.c_str());
        return;
    }

    auto context = GetContext();
    if (context == nullptr) {
        LOG_ERROR("Failed to get context, moduleName:%{public}s.", moduleName.c_str());
        return;
    }
    napi_value contextObj = CreateJsDataShareExtAbilityContext(env, context);
    auto contextRef = jsRuntime_.LoadSystemModule("application.DataShareExtensionAbilityContext", &contextObj, 1);
    contextObj = contextRef->GetNapiValue();
    context->Bind(jsRuntime_, contextRef.release());
    napi_set_named_property(env, obj, "context", contextObj);
    napi_wrap(env, contextObj, new std::weak_ptr<AbilityRuntime::Context>(context),
        [](napi_env, void *data, void *) {
            LOG_INFO("Finalizer for weak_ptr datashare extension ability context is called");
            delete static_cast<std::weak_ptr<AbilityRuntime::Context>*>(data);
        }, nullptr, nullptr);
}

void JsDataShareExtAbility::OnStart(const AAFwk::Want &want)
{
    Extension::OnStart(want);
    HandleScope handleScope(jsRuntime_);
    napi_env env = jsRuntime_.GetNapiEnv();
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(env, &scope);
    if (scope == nullptr) {
        return;
    }
    napi_value napiWant = OHOS::AppExecFwk::WrapWant(env, want);
    napi_value argv[] = {napiWant};
    std::shared_ptr<AsyncContext> context = std::make_shared<AsyncContext>();
    context->isNeedNotify_ = true;
    CallObjectMethod("onCreate", argv, sizeof(argv)/sizeof(argv[0]), context);
    napi_close_handle_scope(env, scope);
}

sptr<IRemoteObject> JsDataShareExtAbility::OnConnect(const AAFwk::Want &want)
{
    Extension::OnConnect(want);
    sptr<DataShareStubImpl> remoteObject = new (std::nothrow) DataShareStubImpl(
        std::static_pointer_cast<JsDataShareExtAbility>(shared_from_this()),
        jsRuntime_.GetNapiEnv());
    if (remoteObject == nullptr) {
        LOG_ERROR("No memory allocated for DataShareStubImpl");
        return nullptr;
    }
    return remoteObject->AsObject();
}

bool JsDataShareExtAbility::UnwrapBatchUpdateResult(napi_env env, napi_value &info,
    std::vector<BatchUpdateResult> &results)
{
    napi_value keys = 0;
    if (napi_get_property_names(env, info, &keys) != napi_ok) {
        LOG_ERROR("napi_get_property_names failed");
        return false;
    }

    uint32_t arrLen = 0;
    if (napi_get_array_length(env, keys, &arrLen) != napi_ok) {
        LOG_ERROR("napi_get_array_length failed");
        return false;
    }
    for (size_t i = 0; i < arrLen; i++) {
        napi_value key = 0;
        if (napi_get_element(env, keys, i, &key) != napi_ok) {
            LOG_ERROR("napi_get_element failed");
            return false;
        }
        BatchUpdateResult batchUpdateResult;
        batchUpdateResult.uri = DataShareJSUtils::UnwrapStringFromJS(env, key);
        napi_value value = 0;
        if (napi_get_property(env, info, key, &value) != napi_ok) {
            LOG_ERROR("napi_get_property failed");
            return false;
        }
        if (!UnwrapArrayInt32FromJS(env, value, batchUpdateResult.codes)) {
            LOG_ERROR("UnwrapArrayInt32FromJS failed");
            return false;
        }
        results.push_back(std::move(batchUpdateResult));
    }
    return true;
}

void JsDataShareExtAbility::CheckAndSetAsyncResult(napi_env env)
{
    napi_valuetype type = napi_undefined;
    auto result = GetAsyncResult();
    napi_typeof(env, result, &type);
    if (type == napi_valuetype::napi_number) {
        int32_t value = OHOS::AppExecFwk::UnwrapInt32FromJS(env, result);
        SetResult(value);
    } else if (type == napi_valuetype::napi_string) {
        std::string value = OHOS::AppExecFwk::UnwrapStringFromJS(env, result);
        SetResult(value);
    } else if (type == napi_valuetype::napi_object) {
        JSProxy::JSCreator<ResultSetBridge> *proxy = nullptr;
        napi_unwrap(env, result, reinterpret_cast<void **>(&proxy));
        if (proxy == nullptr) {
            std::vector<BatchUpdateResult> results;
            if (UnwrapBatchUpdateResult(env, result, results)) {
                SetResult(results);
                return;
            }
            std::vector<std::string> value;
            OHOS::AppExecFwk::UnwrapArrayStringFromJS(env, result, value);
            SetResult(value);
        } else {
            std::shared_ptr<ResultSetBridge> value = proxy->Create();
            std::shared_ptr<DataShareResultSet> resultSet = std::make_shared<DataShareResultSet>(value);
            SetResultSet(resultSet);
        }
    } else {
        callbackResultNumber_ = -1;
        callbackResultString_ = "";
        callbackResultStringArr_ = {};
        SetResultSet(nullptr);
    }
}

napi_value JsDataShareExtAbility::AsyncCallback(napi_env env, napi_callback_info info)
{
    if (env == nullptr || info == nullptr) {
        LOG_ERROR("invalid param.");
        return nullptr;
    }
    napi_value self = nullptr;
    size_t argc = MAX_ARGC;
    napi_value argv[MAX_ARGC] = { nullptr };
    void* data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &self, &data));
    if (argc < MIN_ARGC || argv[0] == nullptr || argv[1] == nullptr) {
        LOG_ERROR("invalid args, argc : %{public}zu.", argc);
        return CreateJsUndefined(env);
    }
    if (data == nullptr) {
        LOG_ERROR("invalid object.");
        return CreateJsUndefined(env);
    }

    AsyncCallBackPoint* point = static_cast<AsyncCallBackPoint*>(data);
    auto instance = point->extAbility.lock();
    if (!instance) {
        LOG_ERROR("extension ability has been destroyed.");
        return CreateJsUndefined(env);
    }
    DatashareBusinessError businessError;
    napi_valuetype type = napi_undefined;
    napi_typeof(env, argv[0], &type);
    if (type == napi_valuetype::napi_object) {
        LOG_INFO("Error in callback");
        UnWrapBusinessError(env, argv[0], businessError);
    }
    if (instance != nullptr) {
        instance->SetBusinessError(businessError);
        instance->SetAsyncResult(argv[1]);
        instance->CheckAndSetAsyncResult(env);
        instance->SetRecvReply(true);
    }
    return CreateJsUndefined(env);
}

napi_value JsDataShareExtAbility::AsyncCallbackWithContext(napi_env env, napi_callback_info info)
{
    if (env == nullptr || info == nullptr) {
        LOG_ERROR("invalid param.");
        return nullptr;
    }

    void* data = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, nullptr, &data);
    if (data == nullptr) {
        LOG_ERROR("invalid object.");
        return CreateJsUndefined(env);
    }

    AsyncPoint* instance = static_cast<AsyncPoint*>(data);
    if (instance != nullptr) {
        if (instance->context->isNeedNotify_) {
            NotifyToDataShareService();
        }
    }
    return CreateJsUndefined(env);
}

napi_value JsDataShareExtAbility::CallObjectMethod(
    const char *name, napi_value const *argv, size_t argc, std::shared_ptr<AsyncContext> asyncContext)
{
    if (!jsObj_) {
        LOG_WARN("Not found DataShareExtAbility.js");
        return nullptr;
    }

    HandleEscape handleEscape(jsRuntime_);
    napi_env env = jsRuntime_.GetNapiEnv();
    napi_value obj = jsObj_->GetNapiValue();
    if (obj == nullptr) {
        LOG_ERROR("Failed to get DataShareExtAbility object");
        return nullptr;
    }

    napi_value method = nullptr;
    napi_get_named_property(env, obj, name, &method);
    if (method == nullptr) {
        LOG_ERROR("Failed to get '%{public}s' from DataShareExtAbility object", name);
        return nullptr;
    }

    AsyncPoint *point = new (std::nothrow)AsyncPoint();
    if (point == nullptr) {
        LOG_ERROR("JsDataShareExtAbility::CallObjectMethod new AsyncPoint error.");
        return nullptr;
    }
    point->context = asyncContext;
    size_t count = argc + 1;
    napi_value *args = new (std::nothrow) napi_value [count];
    if (args == nullptr) {
        LOG_ERROR("JsDataShareExtAbility::CallObjectMethod new NapiValue error.");
        delete point;
        return nullptr;
    }
    for (size_t i = 0; i < argc; i++) {
        args[i] = argv[i];
    }

    napi_create_function(env, ASYNC_CALLBACK_NAME, CALLBACK_LENGTH,
        JsDataShareExtAbility::AsyncCallbackWithContext, point, &args[argc]);
    napi_value callResult = nullptr;
    napi_call_function(env, obj, method, count, args, &callResult);
    auto result = handleEscape.Escape(callResult);
    napi_add_finalizer(env, args[argc], point,
        [](napi_env env, void* point, void* finalize_hint) {
            delete static_cast<AsyncPoint *>(point);
            }, nullptr, nullptr);
    delete []args;
    return result;
}

napi_value JsDataShareExtAbility::CallObjectMethod(const char* name, napi_value const *argv,
    size_t argc, bool isAsync)
{
    if (!jsObj_) {
        LOG_WARN("Not found DataShareExtAbility.js");
        return nullptr;
    }

    HandleEscape handleEscape(jsRuntime_);
    napi_env env = jsRuntime_.GetNapiEnv();
    napi_value obj = jsObj_->GetNapiValue();
    if (obj == nullptr) {
        LOG_ERROR("Failed to get DataShareExtAbility object");
        return nullptr;
    }

    napi_value method = nullptr;
    napi_get_named_property(env, obj, name, &method);
    if (method == nullptr) {
        LOG_ERROR("Failed to get '%{public}s' from DataShareExtAbility object", name);
        return nullptr;
    }

    size_t count = argc + 1;
    napi_value *args = new (std::nothrow) napi_value[count];
    if (args == nullptr) {
        LOG_ERROR("JsDataShareExtAbility::CallObjectMethod new napivalue error.");
        return nullptr;
    }
    for (size_t i = 0; i < argc; i++) {
        args[i] = argv[i];
    }

    if (isAsync) {
        auto ret = InitAsyncCallParams(argc, env, args);
        if (ret != E_OK) {
            LOG_ERROR("Failed to InitAsyncCallParams in isAsync.");
            delete [] args;
            return nullptr;
        }
    } else {
        args[argc] = nullptr;
    }

    napi_value remoteNapi = nullptr;
    NAPI_CallingInfo oldCallingInfo;
    NAPI_RemoteObject_saveOldCallingInfo(env, oldCallingInfo);
    SaveNewCallingInfo(env);
    napi_status status = napi_call_function(env, obj, method, count, args, &remoteNapi);
    NAPI_RemoteObject_resetOldCallingInfo(env, oldCallingInfo);
    delete []args;
    if (status != napi_ok) {
        return nullptr;
    }
    return handleEscape.Escape(remoteNapi);
}

void JsDataShareExtAbility::SaveNewCallingInfo(napi_env &env)
{
    auto newCallingInfo = GetCallingInfo();
    if (newCallingInfo == nullptr) {
        LOG_ERROR("newCallingInfo is null.");
        return;
    }
    CallingInfo callingInfo {
        .callingPid = newCallingInfo->callingPid,
        .callingUid = newCallingInfo->callingUid,
        .callingTokenId = newCallingInfo->callingTokenId,
        .activeStatus = ACTIVE_INVOKER,
    };
    NAPI_RemoteObject_setNewCallingInfo(env, callingInfo);
}

int32_t JsDataShareExtAbility::InitAsyncCallParams(size_t argc, napi_env &env, napi_value *args)
{
    AsyncCallBackPoint *point = new (std::nothrow)AsyncCallBackPoint();
    if (point == nullptr) {
        return E_ERROR;
    }
    callbackResultNumber_ = -1;
    callbackResultString_ = "";
    callbackResultStringArr_ = {};
    SetResultSet(nullptr);
    SetRecvReply(false);
    point->extAbility = std::static_pointer_cast<JsDataShareExtAbility>(shared_from_this());
    napi_create_function(env, ASYNC_CALLBACK_NAME, CALLBACK_LENGTH,
        JsDataShareExtAbility::AsyncCallback, point, &args[argc]);
    napi_add_finalizer(env, args[argc], point,
        [](napi_env env, void* point, void* finalize_hint) {
            delete static_cast<AsyncCallBackPoint *>(point);
        }, nullptr, nullptr);
    return E_OK;
}

void JsDataShareExtAbility::GetSrcPath(std::string &srcPath)
{
    if (!Extension::abilityInfo_->isStageBasedModel) {
        /* temporary compatibility api8 + config.json */
        srcPath.append(Extension::abilityInfo_->package);
        srcPath.append("/assets/js/");
        if (!Extension::abilityInfo_->srcPath.empty()) {
            srcPath.append(Extension::abilityInfo_->srcPath);
        }
        srcPath.append("/").append(Extension::abilityInfo_->name).append(".abc");
        return;
    }

    if (!Extension::abilityInfo_->srcEntrance.empty()) {
        srcPath.append(Extension::abilityInfo_->moduleName + "/");
        srcPath.append(Extension::abilityInfo_->srcEntrance);
        srcPath.erase(srcPath.rfind('.'));
        srcPath.append(".abc");
    }
}

std::vector<std::string> JsDataShareExtAbility::GetFileTypes(const Uri &uri, const std::string &mimeTypeFilter)
{
    auto ret = DataShareExtAbility::GetFileTypes(uri, mimeTypeFilter);
    HandleScope handleScope(jsRuntime_);
    napi_env env = jsRuntime_.GetNapiEnv();
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(env, &scope);
    if (scope == nullptr) {
        return ret;
    }
    napi_value napiUri = nullptr;
    napi_status status = napi_create_string_utf8(env, uri.ToString().c_str(), NAPI_AUTO_LENGTH, &napiUri);
    if (status != napi_ok) {
        LOG_ERROR("napi_create_string_utf8 status : %{public}d", status);
        napi_close_handle_scope(env, scope);
        return ret;
    }
    napi_value napiMimeTypeFilter = nullptr;
    status = napi_create_string_utf8(env, mimeTypeFilter.c_str(), NAPI_AUTO_LENGTH, &napiMimeTypeFilter);
    if (status != napi_ok) {
        LOG_ERROR("napi_create_string_utf8 status : %{public}d", status);
        napi_close_handle_scope(env, scope);
        return ret;
    }
    napi_value argv[] = {napiUri, napiMimeTypeFilter};
    //represents this function has 2 parameters
    CallObjectMethod("getFileTypes", argv, 2);
    napi_close_handle_scope(env, scope);
    return ret;
}

int JsDataShareExtAbility::OpenFile(const Uri &uri, const std::string &mode)
{
    auto ret = DataShareExtAbility::OpenFile(uri, mode);
    HandleScope handleScope(jsRuntime_);
    napi_env env = jsRuntime_.GetNapiEnv();
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(env, &scope);
    if (scope == nullptr) {
        return ret;
    }
    napi_value napiUri = nullptr;
    napi_status status = napi_create_string_utf8(env, uri.ToString().c_str(), NAPI_AUTO_LENGTH, &napiUri);
    if (status != napi_ok) {
        LOG_ERROR("napi_create_string_utf8 status : %{public}d", status);
        napi_close_handle_scope(env, scope);
        return ret;
    }
    napi_value napiMode = nullptr;
    status = napi_create_string_utf8(env, mode.c_str(), NAPI_AUTO_LENGTH, &napiMode);
    if (status != napi_ok) {
        LOG_ERROR("napi_create_string_utf8 status : %{public}d", status);
        napi_close_handle_scope(env, scope);
        return ret;
    }
    napi_value argv[] = {napiUri, napiMode};
    //represents this function has 2 parameters
    CallObjectMethod("openFile", argv, 2);
    napi_close_handle_scope(env, scope);
    return ret;
}

int JsDataShareExtAbility::OpenRawFile(const Uri &uri, const std::string &mode)
{
    auto ret = DataShareExtAbility::OpenRawFile(uri, mode);
    HandleScope handleScope(jsRuntime_);
    napi_env env = jsRuntime_.GetNapiEnv();
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(env, &scope);
    if (scope == nullptr) {
        return ret;
    }
    napi_value napiUri = nullptr;
    napi_status status = napi_create_string_utf8(env, uri.ToString().c_str(), NAPI_AUTO_LENGTH, &napiUri);
    if (status != napi_ok) {
        LOG_ERROR("napi_create_string_utf8 status : %{public}d", status);
        napi_close_handle_scope(env, scope);
        return ret;
    }
    napi_value napiMode = nullptr;
    status = napi_create_string_utf8(env, mode.c_str(), NAPI_AUTO_LENGTH, &napiMode);
    if (status != napi_ok) {
        LOG_ERROR("napi_create_string_utf8 status : %{public}d", status);
        napi_close_handle_scope(env, scope);
        return ret;
    }
    napi_value argv[] = {napiUri, napiMode};
    //represents this function has 2 parameters
    CallObjectMethod("openRawFile", argv, 2, false);
    napi_close_handle_scope(env, scope);
    return ret;
}

int JsDataShareExtAbility::Insert(const Uri &uri, const DataShareValuesBucket &value)
{
    int ret = INVALID_VALUE;
    ret = DataShareExtAbility::Insert(uri, value);
    HandleScope handleScope(jsRuntime_);
    napi_env env = jsRuntime_.GetNapiEnv();
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(env, &scope);
    if (scope == nullptr) {
        return ret;
    }
    napi_value napiUri = nullptr;
    napi_status status = napi_create_string_utf8(env, uri.ToString().c_str(), NAPI_AUTO_LENGTH, &napiUri);
    if (status != napi_ok) {
        LOG_ERROR("napi_create_string_utf8 status : %{public}d", status);
        napi_close_handle_scope(env, scope);
        return ret;
    }
    napi_value napiValue = NewInstance(env, const_cast<DataShareValuesBucket&>(value));
    if (napiValue == nullptr) {
        LOG_ERROR("failed to make new instance of rdbValueBucket.");
        napi_close_handle_scope(env, scope);
        return ret;
    }
    napi_value argv[] = {napiUri, napiValue};
    //represents this function has 2 parameters
    CallObjectMethod("insert", argv, 2);
    napi_close_handle_scope(env, scope);
    return ret;
}

int JsDataShareExtAbility::Update(const Uri &uri, const DataSharePredicates &predicates,
    const DataShareValuesBucket &value)
{
    int ret = INVALID_VALUE;
    ret = DataShareExtAbility::Update(uri, predicates, value);
    HandleScope handleScope(jsRuntime_);
    napi_env env = jsRuntime_.GetNapiEnv();
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(env, &scope);
    if (scope == nullptr) {
        return ret;
    }
    napi_value napiUri = nullptr;
    napi_status status = napi_create_string_utf8(env, uri.ToString().c_str(), NAPI_AUTO_LENGTH, &napiUri);
    if (status != napi_ok) {
        LOG_ERROR("napi_create_string_utf8 status : %{public}d", status);
        napi_close_handle_scope(env, scope);
        return ret;
    }

    napi_value napiPredicates = MakePredicates(env, predicates);
    if (napiPredicates == nullptr) {
        napi_close_handle_scope(env, scope);
        return ret;
    }

    napi_value napiValue = NewInstance(env, const_cast<DataShareValuesBucket&>(value));
    if (napiValue == nullptr) {
        LOG_ERROR("failed to make new instance of rdbValueBucket.");
        napi_close_handle_scope(env, scope);
        return ret;
    }

    napi_value argv[] = {napiUri, napiPredicates, napiValue};
    //represents this function has 3 parameters
    CallObjectMethod("update", argv, 3);
    napi_close_handle_scope(env, scope);
    return ret;
}

int JsDataShareExtAbility::BatchUpdate(const UpdateOperations &operations,
    std::vector<BatchUpdateResult> &results)
{
    int ret = DataShareExtAbility::BatchUpdate(operations, results);
    HandleScope handleScope(jsRuntime_);
    napi_env env = jsRuntime_.GetNapiEnv();
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(env, &scope);
    if (scope == nullptr) {
        return ret;
    }
    napi_value jsMap = nullptr;
    napi_status status = napi_create_object(env, &jsMap);
    if (status != napi_ok) {
        LOG_ERROR("napi_create_object : %{public}d", status);
        napi_close_handle_scope(env, scope);
        return ret;
    }
    for (const auto &valueArray : operations) {
        napi_value napiValues = nullptr;
        status = napi_create_array(env, &napiValues);
        if (status != napi_ok) {
            LOG_ERROR("napi_create_array status : %{public}d", status);
            napi_close_handle_scope(env, scope);
            return ret;
        }
        int32_t index = 0;
        for (const auto &value : valueArray.second) {
            napi_value jsUpdateOperation = MakeUpdateOperation(env, value);
            if (jsUpdateOperation == nullptr) {
                LOG_ERROR("MakeUpdateOperation failed");
                napi_close_handle_scope(env, scope);
                return ret;
            }
            napi_set_element(env, napiValues, index++, jsUpdateOperation);
        }
        napi_set_named_property(env, jsMap, valueArray.first.c_str(), napiValues);
    }
    napi_value argv[] = { jsMap };
    CallObjectMethod("batchUpdate", argv, 1);
    napi_close_handle_scope(env, scope);
    return ret;
}

int JsDataShareExtAbility::Delete(const Uri &uri, const DataSharePredicates &predicates)
{
    int ret = INVALID_VALUE;
    ret = DataShareExtAbility::Delete(uri, predicates);
    HandleScope handleScope(jsRuntime_);
    napi_env env = jsRuntime_.GetNapiEnv();
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(env, &scope);
    if (scope == nullptr) {
        return ret;
    }
    napi_value napiUri = nullptr;
    napi_status status = napi_create_string_utf8(env, uri.ToString().c_str(), NAPI_AUTO_LENGTH, &napiUri);
    if (status != napi_ok) {
        LOG_ERROR("napi_create_string_utf8 status : %{public}d", status);
        napi_close_handle_scope(env, scope);
        return ret;
    }

    napi_value napiPredicates = MakePredicates(env, predicates);
    if (napiPredicates == nullptr) {
        napi_close_handle_scope(env, scope);
        return ret;
    }

    napi_value argv[] = {napiUri, napiPredicates};
    //represents this function has 2 parameters
    CallObjectMethod("delete", argv, 2);
    napi_close_handle_scope(env, scope);
    return ret;
}

std::shared_ptr<DataShareResultSet> JsDataShareExtAbility::Query(const Uri &uri,
    const DataSharePredicates &predicates, std::vector<std::string> &columns, DatashareBusinessError &businessError)
{
    std::shared_ptr<DataShareResultSet> ret;
    ret = DataShareExtAbility::Query(uri, predicates, columns, businessError);

    HandleScope handleScope(jsRuntime_);
    napi_env env = jsRuntime_.GetNapiEnv();
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(env, &scope);
    if (scope == nullptr) {
        return ret;
    }
    napi_value napiUri = nullptr;
    napi_status status = napi_create_string_utf8(env, uri.ToString().c_str(), NAPI_AUTO_LENGTH, &napiUri);
    if (status != napi_ok) {
        LOG_ERROR("napi_create_string_utf8 status : %{public}d", status);
        napi_close_handle_scope(env, scope);
        return ret;
    }

    napi_value napiPredicates = MakePredicates(env, predicates);
    if (napiPredicates == nullptr) {
        napi_close_handle_scope(env, scope);
        return ret;
    }

    napi_value napiColumns = nullptr;
    if (!MakeNapiColumn(env, napiColumns, columns)) {
        LOG_ERROR("MakeNapiColumn failed");
        napi_close_handle_scope(env, scope);
        return ret;
    }

    napi_value argv[] = {napiUri, napiPredicates, napiColumns};
    //represents this function has 3 parameters
    CallObjectMethod("query", argv, 3);
    napi_close_handle_scope(env, scope);
    return ret;
}

std::string JsDataShareExtAbility::GetType(const Uri &uri)
{
    auto ret = DataShareExtAbility::GetType(uri);
    HandleScope handleScope(jsRuntime_);
    napi_env env = jsRuntime_.GetNapiEnv();
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(env, &scope);
    if (scope == nullptr) {
        return ret;
    }
    napi_value napiUri = nullptr;
    napi_status status = napi_create_string_utf8(env, uri.ToString().c_str(), NAPI_AUTO_LENGTH, &napiUri);
    if (status != napi_ok) {
        LOG_ERROR("napi_create_string_utf8 status : %{public}d", status);
        napi_close_handle_scope(env, scope);
        return ret;
    }
    napi_value argv[] = {napiUri};
    //represents this function has 1 parameter
    CallObjectMethod("getType", argv, 1);
    napi_close_handle_scope(env, scope);
    return ret;
}

int JsDataShareExtAbility::BatchInsert(const Uri &uri, const std::vector<DataShareValuesBucket> &values)
{
    int ret = INVALID_VALUE;
    ret = DataShareExtAbility::BatchInsert(uri, values);

    HandleScope handleScope(jsRuntime_);
    napi_env env = jsRuntime_.GetNapiEnv();
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(env, &scope);
    if (scope == nullptr) {
        return ret;
    }
    napi_value napiUri = nullptr;
    napi_status status = napi_create_string_utf8(env, uri.ToString().c_str(), NAPI_AUTO_LENGTH, &napiUri);
    if (status != napi_ok) {
        LOG_ERROR("napi_create_string_utf8 status : %{public}d", status);
        napi_close_handle_scope(env, scope);
        return ret;
    }

    napi_value napiValues = nullptr;
    status = napi_create_array(env, &napiValues);
    if (status != napi_ok) {
        LOG_ERROR("napi_create_array status : %{public}d", status);
        napi_close_handle_scope(env, scope);
        return ret;
    }
    bool isArray = false;
    if (napi_is_array(env, napiValues, &isArray) != napi_ok || !isArray) {
        LOG_ERROR("JsDataShareExtAbility create array failed");
        napi_close_handle_scope(env, scope);
        return ret;
    }
    int32_t index = 0;
    for (const auto &value : values) {
        napi_value result = NewInstance(env, const_cast<DataShareValuesBucket&>(value));
        if (result == nullptr) {
            LOG_ERROR("failed to make new instance of rdbValueBucket.");
            napi_close_handle_scope(env, scope);
            return ret;
        }
        napi_set_element(env, napiValues, index++, result);
    }
    napi_value argv[] = {napiUri, napiValues};
    //represents this function has 2 parameters
    CallObjectMethod("batchInsert", argv, 2);
    napi_close_handle_scope(env, scope);
    return ret;
}

bool JsDataShareExtAbility::RegisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver)
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

bool JsDataShareExtAbility::UnregisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver)
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

bool JsDataShareExtAbility::NotifyChange(const Uri &uri)
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

Uri JsDataShareExtAbility::NormalizeUri(const Uri &uri)
{
    auto ret = DataShareExtAbility::NormalizeUri(uri);
    HandleScope handleScope(jsRuntime_);
    napi_env env = jsRuntime_.GetNapiEnv();
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(env, &scope);
    if (scope == nullptr) {
        return ret;
    }
    napi_value napiUri = nullptr;
    napi_status status = napi_create_string_utf8(env, uri.ToString().c_str(), NAPI_AUTO_LENGTH, &napiUri);
    if (status != napi_ok) {
        LOG_ERROR("napi_create_string_utf8 status : %{public}d", status);
        napi_close_handle_scope(env, scope);
        return ret;
    }
    napi_value argv[] = {napiUri};
    //represents this function has 1 parameter
    CallObjectMethod("normalizeUri", argv, 1);
    napi_close_handle_scope(env, scope);
    return ret;
}

Uri JsDataShareExtAbility::DenormalizeUri(const Uri &uri)
{
    auto ret = DataShareExtAbility::DenormalizeUri(uri);
    HandleScope handleScope(jsRuntime_);
    napi_env env = jsRuntime_.GetNapiEnv();
    napi_handle_scope scope = nullptr;
    napi_open_handle_scope(env, &scope);
    if (scope == nullptr) {
        return ret;
    }
    napi_value napiUri = nullptr;
    napi_status status = napi_create_string_utf8(env, uri.ToString().c_str(), NAPI_AUTO_LENGTH, &napiUri);
    if (status != napi_ok) {
        LOG_ERROR("napi_create_string_utf8 status : %{public}d", status);
        napi_close_handle_scope(env, scope);
        return ret;
    }
    napi_value argv[] = {napiUri};
    //represents this function has 1 parameter
    CallObjectMethod("denormalizeUri", argv, 1);
    napi_close_handle_scope(env, scope);
    return ret;
}

napi_value JsDataShareExtAbility::MakePredicates(napi_env env, const DataSharePredicates &predicates)
{
    std::shared_ptr<DataSharePredicates> predicatesPtr = std::make_shared<DataSharePredicates>(predicates);
    if (predicatesPtr == nullptr) {
        LOG_ERROR("No memory allocated for predicates");
        return nullptr;
    }
    napi_value napiPredicates = DataSharePredicatesProxy::NewInstance(env, predicatesPtr);
    if (napiPredicates == nullptr) {
        LOG_ERROR("failed to make new instance of DataSharePredicates.");
    }
    return napiPredicates;
}

void JsDataShareExtAbility::UnWrapBusinessError(napi_env env, napi_value info,
    DatashareBusinessError& businessError)
{
    std::string code = UnWrapProperty(env, info, "code");
    businessError.SetCode(code);
    std::string message = UnWrapProperty(env, info, "message");
    businessError.SetMessage(message);
}

std::string JsDataShareExtAbility::UnWrapProperty(napi_env env, napi_value info, const std::string &key)
{
    napi_valuetype type = UnWrapPropertyType(env, info, key);
    if (type == napi_valuetype::napi_number) {
        int value;
        UnwrapInt32ByPropertyName(env, info, key.c_str(), value);
        return std::to_string(value);
    } else if (type == napi_valuetype::napi_string) {
        std::string value;
        UnwrapStringByPropertyName(env, info, key.c_str(), value);
        return value;
    } else {
        LOG_ERROR("ValueType should be napi_number or napi_string, property is %{public}s", key.c_str());
        return "";
    }
}

napi_valuetype JsDataShareExtAbility::UnWrapPropertyType(napi_env env, napi_value info,
    const std::string &propertyKey)
{
    napi_value key = nullptr;
    napi_status status = napi_create_string_utf8(env, propertyKey.c_str(), propertyKey.size(), &key);
    if (status != napi_ok) {
        LOG_ERROR("napi_create_string_utf8 failed, status is %{public}d, propertyKey is %{public}s",
            status, propertyKey.c_str());
        return napi_undefined;
    }

    bool result = false;
    napi_has_property(env, info, key, &result);
    if (!result) {
        LOG_WARN("not contains property is %{public}s", propertyKey.c_str());
        return napi_undefined;
    }

    napi_value value = nullptr;
    status = napi_get_property(env, info, key, &value);
    if (status != napi_ok) {
        LOG_ERROR("failed to napi_get_property, status is %{public}d, propertyKey is %{public}s",
            status, propertyKey.c_str());
        return napi_undefined;
    }

    napi_valuetype type = napi_undefined;
    napi_typeof(env, value, &type);
    return type;
}

void JsDataShareExtAbility::NotifyToDataShareService()
{
    auto manager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (manager == nullptr) {
        LOG_ERROR("get system ability manager failed");
        return;
    }
    auto remoteObject = manager->CheckSystemAbility(DISTRIBUTED_KV_DATA_SERVICE_ABILITY_ID);
    if (remoteObject == nullptr) {
        LOG_ERROR("CheckSystemAbility failed");
        return;
    }
    auto serviceProxy = std::make_shared<DataShareKvServiceProxy>(remoteObject);
    if (serviceProxy == nullptr) {
        LOG_ERROR("make_shared failed");
        return;
    }
    auto remote = serviceProxy->GetFeatureInterface("data_share");
    if (remote == nullptr) {
        LOG_ERROR("Get DataShare service failed!");
        return;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(IDataShareService::GetDescriptor())) {
        LOG_ERROR("Write descriptor failed!");
        return;
    }
    remote->SendRequest(
        static_cast<uint32_t>(DataShareServiceInterfaceCode::DATA_SHARE_SERVICE_CMD_NOTIFY), data, reply, option);
}

napi_value JsDataShareExtAbility::MakeUpdateOperation(napi_env env, const UpdateOperation &updateOperation)
{
    napi_value jsValueBucket = NewInstance(env, const_cast<DataShareValuesBucket&>(updateOperation.valuesBucket));
    napi_value jsPredicates = MakePredicates(env, updateOperation.predicates);
    if (jsValueBucket == nullptr || jsPredicates == nullptr) {
        LOG_ERROR("failed to make new instance of UpdateOperation.");
        return nullptr;
    }
    napi_value jsUpdateOperation = nullptr;
    napi_status status = napi_create_object(env, &jsUpdateOperation);
    if (status != napi_ok) {
        LOG_ERROR("JsDataShareExtAbility create object failed");
        return nullptr;
    }
    std::string valuesKey = "values";
    std::string presKey = "predicates";
    napi_value jsValueKey = DataShareJSUtils::Convert2JSValue(env, valuesKey);
    napi_value jsPresKey = DataShareJSUtils::Convert2JSValue(env, presKey);
    napi_set_property(env, jsUpdateOperation, jsValueKey, jsValueBucket);
    napi_set_property(env, jsUpdateOperation, jsPresKey, jsPredicates);
    return jsUpdateOperation;
}

bool MakeNapiColumn(napi_env env, napi_value &napiColumns, const std::vector<std::string> &columns)
{
    napi_status status = napi_create_array(env, &napiColumns);
    if (status != napi_ok) {
        LOG_ERROR("napi_create_array status : %{public}d", status);
        return false;
    }

    bool isArray = false;
    if (napi_is_array(env, napiColumns, &isArray) != napi_ok || !isArray) {
        LOG_ERROR("JsDataShareExtAbility create array failed");
        return false;
    }

    int32_t index = 0;
    for (const auto &column : columns) {
        napi_value result = nullptr;
        napi_create_string_utf8(env, column.c_str(), column.length(), &result);
        napi_set_element(env, napiColumns, index++, result);
    }

    return true;
}
} // namespace DataShare
} // namespace OHOS