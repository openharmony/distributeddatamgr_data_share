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

#include "napi_datashare_values_bucket.h"

#include "datashare_log.h"
#include "datashare_js_utils.h"
#include "datashare_value_object.h"

#include "securec.h"

namespace OHOS {
namespace DataShare {
napi_value NewInstance(napi_env env, const DataShareValuesBucket &valuesBucket)
{
    napi_value ret;
    NAPI_CALL(env, napi_create_object(env, &ret));
    const auto &valuesMap = valuesBucket.valuesMap;
    for (auto it = valuesMap.begin(); it != valuesMap.end(); ++it) {
        std::string key = it->first;
        auto valueObject = it->second;
        napi_value value = DataShareJSUtils::Convert2JSValue(env, valueObject);
        if (value == nullptr) {
            continue;
        }
        NAPI_CALL(env, napi_set_named_property(env, ret, key.c_str(), value));
    }

    return ret;
}

bool UnWrapValuesBucket(DataShareValuesBucket &valuesBucket, const napi_env &env, const napi_value &arg)
{
    napi_value keys = 0;
    napi_get_property_names(env, arg, &keys);
    uint32_t arrLen = 0;
    napi_status status = napi_get_array_length(env, keys, &arrLen);
    if (status != napi_ok || arrLen == 0) {
        LOG_ERROR("ValuesBucket err");
        return false;
    }
    for (size_t i = 0; i < arrLen; ++i) {
        napi_value key = 0;
        status = napi_get_element(env, keys, i, &key);
        if (status != napi_ok) {
            LOG_ERROR("ValuesBucket err");
            return false;
        }
        std::string keyStr = DataShareJSUtils::UnwrapStringFromJS(env, key);
        napi_value value = 0;
        napi_get_property(env, arg, key, &value);

        bool ret;
        DataShareValueObject valueObject = DataShareJSUtils::Convert2ValueObject(env, value, ret);
        if (!ret) {
            LOG_ERROR("ValuesBucket err");
            return false;
        }
        valuesBucket.Put(keyStr, valueObject);
    }
    return true;
}

bool GetValueBucketObject(DataShareValuesBucket &valuesBucket, const napi_env &env, const napi_value &arg)
{
    return UnWrapValuesBucket(valuesBucket, env, arg);
}
} // namespace DataShare
} // namespace OHOS