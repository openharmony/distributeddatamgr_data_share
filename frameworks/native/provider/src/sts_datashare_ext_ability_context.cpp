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
#include "sts_datashare_ext_ability_context.h"
#include "ui_extension_context.h"
#include "ability_manager_client.h"
#include "sts_context_utils.h"
#include "sts_error_utils.h"
#include "ets_extension_context.h"

namespace OHOS {
namespace DataShare {
using namespace AbilityRuntime;
constexpr const char* CONTEXT_CLASS_NAME = "application.ExtensionContext.ExtensionContext";
namespace {
class StsDataShareExtAbilityContext final {
public:
    explicit StsDataShareExtAbilityContext(const std::shared_ptr<DataShareExtAbilityContext>& context)
        : context_(context) {}
    ~StsDataShareExtAbilityContext() = default;

private:
    std::weak_ptr<DataShareExtAbilityContext> context_;
};
} // namespace

ani_object CreateStsDataShareExtAbilityContext(ani_env *env, std::shared_ptr<DataShareExtAbilityContext> context,
    const std::shared_ptr<AppExecFwk::OHOSApplication> &application)
{
    LOG_DEBUG("CreateStsDataShareExtAbilityContext begin");
    if (env == nullptr) {
        LOG_ERROR("Failed to create sts extension ability context, env is null");
        return nullptr;
    }

    ani_class cls = nullptr;
    ani_object contextObj = nullptr;
    ani_field field = nullptr;
    ani_status status = ANI_ERROR;
    if ((env->FindClass(CONTEXT_CLASS_NAME, &cls)) != ANI_OK) {
        LOG_ERROR("Failed to find class %{public}s, status: %{public}d", CONTEXT_CLASS_NAME, status);
        return nullptr;
    }

    if ((status = env->Class_FindMethod(cls, "<ctor>", ":", &method)) != ANI_OK) {
        LOG_ERROR("Failed to find constructor of %{public}s, status: %{public}d", CONTEXT_CLASS_NAME, status);
        return nullptr;
    }
    if ((status = env->Object_New(cls, method, &contextObj)) != ANI_OK) {
        LOG_ERROR("Failed to create context object, status: %{public}d", status);
        return nullptr;
    }
    if ((status = env->Class_FindField(cls, "nativeContext", &field)) != ANI_OK) {
        LOG_ERROR("Failed to find field nativeContext, status: %{public}d", status);
        return nullptr;
    }
    std::unique_ptr<StsDataShareExtAbilityContext> stsContext =
        std::make_unique<StsDataShareExtAbilityContext>(context);
    ani_long nativeContextLong = (ani_long)stsContext.get();
    if ((status = env->Object_SetField_Long(contextObj, field, nativeContextLong)) != ANI_OK) {
        LOG_ERROR("Failed to set field, status: %{public}d", status);
        return nullptr;
    }
    if (application == nullptr) {
        LOG_ERROR("Failed to create sts extension ability context, application is null");
        return nullptr;
    }
    ContextUtil::StsCreatContext(env, cls, contextObj, application->GetApplicationCtxObjRef(), context);
    CreatEtsExtensionContext(env, cls, contextObj, context, context->GetAbilityInfo());
    return contextObj;
}
}  // namespace DataShare
}  // namespace OHOS