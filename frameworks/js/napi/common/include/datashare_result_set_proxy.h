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

#ifndef DATASHARE_RESULT_SET_PROXY
#define DATASHARE_RESULT_SET_PROXY

#include <memory>
#include "datashare_result_set.h"
#include "js_proxy.h"
#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace DataShare {
class DataShareResultSetProxy final : public JSProxy::JSProxy<DataShareResultSet> {
public:
    DataShareResultSetProxy() = default;
    ~DataShareResultSetProxy();
    explicit DataShareResultSetProxy(std::shared_ptr<DataShareResultSet> resultSet);
    DataShareResultSetProxy &operator=(std::shared_ptr<DataShareResultSet> resultSet);
    static napi_value NewInstance(napi_env env, std::shared_ptr<DataShareResultSet> resultSet);
    static std::shared_ptr<DataShareResultSet> GetNativeObject(
        const napi_env &env, const napi_value &arg);
    static napi_value GetConstructor(napi_env env);

private:
    static std::shared_ptr<DataShareResultSet> GetInnerResultSet(napi_env env, napi_callback_info info);
    static napi_value Initialize(napi_env env, napi_callback_info info);

    static napi_value GoToFirstRow(napi_env env, napi_callback_info info);
    static napi_value GoToLastRow(napi_env env, napi_callback_info info);
    static napi_value GoToNextRow(napi_env env, napi_callback_info info);
    static napi_value GoToPreviousRow(napi_env env, napi_callback_info info);
    static napi_value GoTo(napi_env env, napi_callback_info info);
    static napi_value GoToRow(napi_env env, napi_callback_info info);
    static napi_value GetBlob(napi_env env, napi_callback_info info);
    static napi_value GetString(napi_env env, napi_callback_info info);
    static napi_value GetLong(napi_env env, napi_callback_info info);
    static napi_value GetDouble(napi_env env, napi_callback_info info);
    static napi_value Close(napi_env env, napi_callback_info info);
    static napi_value GetColumnIndex(napi_env env, napi_callback_info info);
    static napi_value GetColumnName(napi_env env, napi_callback_info info);
    static napi_value GetDataType(napi_env env, napi_callback_info info);

    static napi_value GetAllColumnNames(napi_env env, napi_callback_info info);
    static napi_value GetColumnCount(napi_env env, napi_callback_info info);
    static napi_value GetRowCount(napi_env env, napi_callback_info info);
    static napi_value IsClosed(napi_env env, napi_callback_info info);
};
} // namespace DataShare
} // namespace OHOS
#endif // DATASHARE_RESULT_SET_PROXY
