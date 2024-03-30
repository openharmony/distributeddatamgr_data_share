/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#define LOG_TAG "DataShareConstProperties"

#include "napi_datashare_const_properties.h"

#include <string>

#include "datashare_helper.h"
#include "napi_base_context.h"
#include "napi_common_util.h"

namespace OHOS::DataShare {

static napi_status SetNamedProperty(napi_env env, napi_value &obj, const std::string &name, int32_t value)
{
    napi_value property = nullptr;
    napi_status status = napi_create_int32(env, value, &property);
    NAPI_ASSERT_BASE(env, status == napi_ok, "int32_t to napi_value failed!", status);
    napi_set_named_property(env, obj, name.c_str(), property);
    return status;
}

static napi_value ExportChangeType(napi_env env)
{
    napi_value changeType = nullptr;
    napi_create_object(env, &changeType);
    SetNamedProperty(env, changeType, "INSERT", static_cast<int32_t>(DataShareObserver::ChangeType::INSERT));
    SetNamedProperty(env, changeType, "DELETE", static_cast<int32_t>(DataShareObserver::ChangeType::DELETE));
    SetNamedProperty(env, changeType, "UPDATE", static_cast<int32_t>(DataShareObserver::ChangeType::UPDATE));
    napi_object_freeze(env, changeType);
    return changeType;
}

static napi_value ExportSubscriptionType(napi_env env)
{
    napi_value SubscriptionType = nullptr;
    napi_create_object(env, &SubscriptionType);
    SetNamedProperty(env, SubscriptionType, "SUBSCRIPTION_TYPE_EXACT_URI",
                     static_cast<int32_t>(DataShareObserver::SubscriptionType::SUBSCRIPTION_TYPE_EXACT_URI));
    napi_object_freeze(env, SubscriptionType);
    return SubscriptionType;
}
napi_status InitConstProperties(napi_env env, napi_value exports)
{
    const napi_property_descriptor properties[] = {
        DECLARE_NAPI_PROPERTY("ChangeType", ExportChangeType(env)),
        DECLARE_NAPI_PROPERTY("SubscriptionType", ExportSubscriptionType(env)),
    };
    size_t count = sizeof(properties) / sizeof(properties[0]);

    return napi_define_properties(env, exports, count, properties);
}
} // namespace OHOS::DataShare