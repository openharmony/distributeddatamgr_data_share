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

#include "datashare_result_set_proxy.h"

#include <functional>
#include <chrono>
#include <cinttypes>

#include "datashare_result_set.h"
#include "datashare_js_utils.h"
#include "string_ex.h"
#include "datashare_log.h"

namespace OHOS {
namespace DataShare {
using namespace std::chrono;
constexpr int MAX_INPUT_COUNT = 10;
static napi_ref __thread ctorRef_ = nullptr;
napi_value DataShareResultSetProxy::NewInstance(napi_env env, std::shared_ptr<DataShareResultSet> resultSet)
{
    napi_value cons = GetConstructor(env);
    if (cons == nullptr) {
        LOG_ERROR("GetConstructor is nullptr!");
        return nullptr;
    }
    napi_value instance;
    napi_status status = napi_new_instance(env, cons, 0, nullptr, &instance);
    if (status != napi_ok) {
        LOG_ERROR("napi_new_instance failed! code:%{public}d!", status);
        return nullptr;
    }

    DataShareResultSetProxy *proxy = nullptr;
    status = napi_unwrap(env, instance, reinterpret_cast<void **>(&proxy));
    if (proxy == nullptr) {
        LOG_ERROR("native instance is nullptr! code:%{public}d!", status);
        return instance;
    }

    *proxy = std::move(resultSet);
    return instance;
}

std::shared_ptr<DataShareResultSet> DataShareResultSetProxy::GetNativeObject(
    napi_env const &env, napi_value const &arg)
{
    if (arg == nullptr) {
        LOG_ERROR("arg is null.");
        return nullptr;
    }
    DataShareResultSetProxy *proxy = nullptr;
    napi_unwrap(env, arg, reinterpret_cast<void **>(&proxy));
    if (proxy == nullptr) {
        LOG_ERROR("proxy is null.");
        return nullptr;
    }
    return proxy->GetInstance();
}

napi_value DataShareResultSetProxy::GetConstructor(napi_env env)
{
    napi_value cons;
    if (ctorRef_ != nullptr) {
        NAPI_CALL(env, napi_get_reference_value(env, ctorRef_, &cons));
        return cons;
    }
    LOG_DEBUG("Get DataShareResultSet constructor");
    napi_property_descriptor clzDes[] = {
        DECLARE_NAPI_FUNCTION("goToFirstRow", GoToFirstRow),
        DECLARE_NAPI_FUNCTION("goToLastRow", GoToLastRow),
        DECLARE_NAPI_FUNCTION("goToNextRow", GoToNextRow),
        DECLARE_NAPI_FUNCTION("goToPreviousRow", GoToPreviousRow),
        DECLARE_NAPI_FUNCTION("goTo", GoTo),
        DECLARE_NAPI_FUNCTION("goToRow", GoToRow),
        DECLARE_NAPI_FUNCTION("getBlob", GetBlob),
        DECLARE_NAPI_FUNCTION("getString", GetString),
        DECLARE_NAPI_FUNCTION("getLong", GetLong),
        DECLARE_NAPI_FUNCTION("getDouble", GetDouble),
        DECLARE_NAPI_FUNCTION("close", Close),
        DECLARE_NAPI_FUNCTION("getColumnIndex", GetColumnIndex),
        DECLARE_NAPI_FUNCTION("getColumnName", GetColumnName),
        DECLARE_NAPI_FUNCTION("getDataType", GetDataType),

        DECLARE_NAPI_GETTER("columnNames", GetAllColumnNames),
        DECLARE_NAPI_GETTER("columnCount", GetColumnCount),
        DECLARE_NAPI_GETTER("rowCount", GetRowCount),
        DECLARE_NAPI_GETTER("isClosed", IsClosed),
    };
    NAPI_CALL(env, napi_define_class(env, "DataShareResultSet", NAPI_AUTO_LENGTH, Initialize, nullptr,
        sizeof(clzDes) / sizeof(napi_property_descriptor), clzDes, &cons));
    NAPI_CALL(env, napi_create_reference(env, cons, 1, &ctorRef_));
    return cons;
}

napi_value DataShareResultSetProxy::Initialize(napi_env env, napi_callback_info info)
{
    napi_value self = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &self, nullptr));
    auto *proxy = new (std::nothrow) DataShareResultSetProxy();
    if (proxy == nullptr) {
        LOG_ERROR("DataShareResultSetProxy::Initialize new DataShareResultSetProxy error.");
        return nullptr;
    }
    auto finalize = [](napi_env env, void *data, void *hint) {
        DataShareResultSetProxy *proxy = reinterpret_cast<DataShareResultSetProxy *>(data);
        if (proxy != nullptr) {
            delete proxy;
        }
    };
    napi_status status = napi_wrap(env, self, proxy, finalize, nullptr, nullptr);
    if (status != napi_ok) {
        LOG_ERROR("napi_wrap failed! code:%{public}d!", status);
        finalize(env, proxy, nullptr);
        return nullptr;
    }
    return self;
}

DataShareResultSetProxy::~DataShareResultSetProxy()
{
    LOG_DEBUG("DataShareResultSetProxy destructor!");
    if (GetInstance() != nullptr && !GetInstance()->IsClosed()) {
        GetInstance()->Close();
    }
}

DataShareResultSetProxy::DataShareResultSetProxy(std::shared_ptr<DataShareResultSet> resultSet)
{
    if (GetInstance() == resultSet) {
        return;
    }
    SetInstance(resultSet);
}

DataShareResultSetProxy &DataShareResultSetProxy::operator=(std::shared_ptr<DataShareResultSet> resultSet)
{
    if (GetInstance() == resultSet) {
        return *this;
    }
    SetInstance(resultSet);
    return *this;
}

std::shared_ptr<DataShareResultSet> DataShareResultSetProxy::GetInnerResultSet(napi_env env,
    napi_callback_info info)
{
    DataShareResultSetProxy *resultSet = nullptr;
    napi_value self = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &self, nullptr);
    napi_unwrap(env, self, reinterpret_cast<void **>(&resultSet));
    return resultSet->GetInstance();
}

napi_value DataShareResultSetProxy::GoToFirstRow(napi_env env, napi_callback_info info)
{
    int errCode = E_ERROR;
    std::shared_ptr<DataShareResultSet> innerResultSet = GetInnerResultSet(env, info);
    if (innerResultSet != nullptr) {
        errCode = innerResultSet->GoToFirstRow();
        if (errCode != E_OK) {
            LOG_ERROR("failed code:%{public}d", errCode);
        }
    } else {
        LOG_ERROR("GetInnerResultSet failed.");
    }
    return DataShareJSUtils::Convert2JSValue(env, (errCode == E_OK));
}

napi_value DataShareResultSetProxy::GoToLastRow(napi_env env, napi_callback_info info)
{
    int errCode = E_ERROR;
    std::shared_ptr<DataShareResultSet> innerResultSet = GetInnerResultSet(env, info);
    if (innerResultSet != nullptr) {
        errCode = innerResultSet->GoToLastRow();
        if (errCode != E_OK) {
            LOG_ERROR("failed code:%{public}d", errCode);
        }
    } else {
        LOG_ERROR("GetInnerResultSet failed.");
    }
    return DataShareJSUtils::Convert2JSValue(env, (errCode == E_OK));
}

napi_value DataShareResultSetProxy::GoToNextRow(napi_env env, napi_callback_info info)
{
    int errCode = E_ERROR;
    std::shared_ptr<DataShareResultSet> innerResultSet = GetInnerResultSet(env, info);
    if (innerResultSet != nullptr) {
        errCode = innerResultSet->GoToNextRow();
        if (errCode != E_OK) {
            LOG_ERROR("failed code:%{public}d", errCode);
        }
    } else {
        LOG_ERROR("GetInnerResultSet failed.");
    }
    return DataShareJSUtils::Convert2JSValue(env, (errCode == E_OK));
}

napi_value DataShareResultSetProxy::GoToPreviousRow(napi_env env, napi_callback_info info)
{
    int errCode = E_ERROR;
    std::shared_ptr<DataShareResultSet> innerResultSet = GetInnerResultSet(env, info);
    if (innerResultSet != nullptr) {
        errCode = innerResultSet->GoToPreviousRow();
        if (errCode != E_OK) {
            LOG_ERROR("failed code:%{public}d", errCode);
        }
    } else {
        LOG_ERROR("GetInnerResultSet failed.");
    }
    return DataShareJSUtils::Convert2JSValue(env, (errCode == E_OK));
}

napi_value DataShareResultSetProxy::GoTo(napi_env env, napi_callback_info info)
{
    int32_t offset = -1;
    size_t argc = MAX_INPUT_COUNT;
    napi_value args[MAX_INPUT_COUNT] = { 0 };
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    NAPI_ASSERT(env, argc > 0, "Invalid argvs!");
    NAPI_CALL(env, napi_get_value_int32(env, args[0], &offset));
    int errCode = E_ERROR;
    std::shared_ptr<DataShareResultSet> innerResultSet = GetInnerResultSet(env, info);
    if (innerResultSet != nullptr) {
        errCode = innerResultSet->GoTo(offset);
        if (errCode != E_OK) {
            LOG_ERROR("failed code:%{public}d", errCode);
        }
    } else {
        LOG_ERROR("GetInnerResultSet failed.");
    }
    return DataShareJSUtils::Convert2JSValue(env, (errCode == E_OK));
}

napi_value DataShareResultSetProxy::GoToRow(napi_env env, napi_callback_info info)
{
    int32_t position = -1;
    size_t argc = MAX_INPUT_COUNT;
    napi_value args[MAX_INPUT_COUNT] = { 0 };
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    NAPI_ASSERT(env, argc > 0, "Invalid argvs!");
    NAPI_CALL(env, napi_get_value_int32(env, args[0], &position));
    int errCode = E_ERROR;
    std::shared_ptr<DataShareResultSet> innerResultSet = GetInnerResultSet(env, info);
    if (innerResultSet != nullptr) {
        errCode = innerResultSet->GoToRow(position);
        if (errCode != E_OK) {
            LOG_ERROR("failed code:%{public}d", errCode);
        }
    } else {
        LOG_ERROR("GetInnerResultSet failed.");
    }
    return DataShareJSUtils::Convert2JSValue(env, (errCode == E_OK));
}

napi_value DataShareResultSetProxy::GetBlob(napi_env env, napi_callback_info info)
{
    int32_t columnIndex = -1;
    std::vector<uint8_t> blob;
    size_t argc = MAX_INPUT_COUNT;
    napi_value args[MAX_INPUT_COUNT] = { 0 };
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    NAPI_ASSERT(env, argc > 0, "Invalid argvs!");
    NAPI_CALL(env, napi_get_value_int32(env, args[0], &columnIndex));
    if (columnIndex == -1) {
        return DataShareJSUtils::Convert2JSValue(env, blob);
    }
    std::shared_ptr<DataShareResultSet> innerResultSet = GetInnerResultSet(env, info);
    if (innerResultSet != nullptr) {
        int errCode = innerResultSet->GetBlob(columnIndex, blob);
        if (errCode != E_OK) {
            LOG_ERROR("failed code:%{public}d", errCode);
        }
    } else {
        LOG_ERROR("GetInnerResultSet failed.");
    }
    return DataShareJSUtils::Convert2JSValue(env, blob);
}

napi_value DataShareResultSetProxy::GetString(napi_env env, napi_callback_info info)
{
    int32_t columnIndex = -1;
    std::string value;
    size_t argc = MAX_INPUT_COUNT;
    napi_value args[MAX_INPUT_COUNT] = { 0 };
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    NAPI_ASSERT(env, argc > 0, "Invalid argvs!");
    NAPI_CALL(env, napi_get_value_int32(env, args[0], &columnIndex));
    if (columnIndex == -1) {
        return DataShareJSUtils::Convert2JSValue(env, value);
    }
    std::shared_ptr<DataShareResultSet> innerResultSet = GetInnerResultSet(env, info);
    if (innerResultSet != nullptr) {
        innerResultSet->GetString(columnIndex, value);
    } else {
        LOG_ERROR("GetInnerResultSet failed.");
    }
    return DataShareJSUtils::Convert2JSValue(env, value);
}

napi_value DataShareResultSetProxy::GetLong(napi_env env, napi_callback_info info)
{
    int32_t columnIndex = -1;
    int64_t value = -1;
    size_t argc = MAX_INPUT_COUNT;
    napi_value args[MAX_INPUT_COUNT] = { 0 };
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    NAPI_ASSERT(env, argc > 0, "Invalid argvs!");
    NAPI_CALL(env, napi_get_value_int32(env, args[0], &columnIndex));
    if (columnIndex == -1) {
        return DataShareJSUtils::Convert2JSValue(env, value);
    }
    std::shared_ptr<DataShareResultSet> innerResultSet = GetInnerResultSet(env, info);
    if (innerResultSet != nullptr) {
        int errCode = innerResultSet->GetLong(columnIndex, value);
        if (errCode != E_OK) {
            LOG_ERROR("failed code:%{public}d", errCode);
        }
    } else {
        LOG_ERROR("GetInnerResultSet failed.");
    }
    return DataShareJSUtils::Convert2JSValue(env, value);
}

napi_value DataShareResultSetProxy::GetDouble(napi_env env, napi_callback_info info)
{
    int32_t columnIndex = -1;
    double value = 0.0;
    size_t argc = MAX_INPUT_COUNT;
    napi_value args[MAX_INPUT_COUNT] = { 0 };
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    NAPI_ASSERT(env, argc > 0, "Invalid argvs!");
    NAPI_CALL(env, napi_get_value_int32(env, args[0], &columnIndex));
    if (columnIndex == -1) {
        return DataShareJSUtils::Convert2JSValue(env, value);
    }
    std::shared_ptr<DataShareResultSet> innerResultSet = GetInnerResultSet(env, info);
    if (innerResultSet != nullptr) {
        int errCode = innerResultSet->GetDouble(columnIndex, value);
        if (errCode != E_OK) {
            LOG_ERROR("failed code:%{public}d", errCode);
        }
    } else {
        LOG_ERROR("GetInnerResultSet failed.");
    }
    return DataShareJSUtils::Convert2JSValue(env, value);
}

napi_value DataShareResultSetProxy::Close(napi_env env, napi_callback_info info)
{
    int errCode = E_ERROR;
    DataShareResultSetProxy *resultSet = nullptr;
    napi_value self = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &self, nullptr);
    napi_unwrap(env, self, reinterpret_cast<void **>(&resultSet));
    if (resultSet == nullptr) {
        return DataShareJSUtils::Convert2JSValue(env, (errCode == E_OK));
    }
    auto innerResultSet = resultSet->GetInstance();
    if (innerResultSet != nullptr) {
        errCode = innerResultSet->Close();
        if (errCode != E_OK) {
            LOG_ERROR("failed code:%{public}d", errCode);
        }
        resultSet->SetInstance(nullptr);
    } else {
        LOG_ERROR("GetInnerResultSet failed.");
    }
    return DataShareJSUtils::Convert2JSValue(env, (errCode == E_OK));
}

napi_value DataShareResultSetProxy::GetColumnIndex(napi_env env, napi_callback_info info)
{
    int32_t columnIndex = -1;
    size_t argc = MAX_INPUT_COUNT;
    napi_value args[MAX_INPUT_COUNT] = { 0 };
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    NAPI_ASSERT(env, argc > 0, "Invalid argvs!");
    std::string columnName = DataShareJSUtils::Convert2String(env, args[0], DataShareJSUtils::DEFAULT_BUF_SIZE);
    std::shared_ptr<DataShareResultSet> innerResultSet = GetInnerResultSet(env, info);
    if (innerResultSet != nullptr) {
        int errCode = innerResultSet->GetColumnIndex(columnName, columnIndex);
        if (errCode != E_OK) {
            auto time = static_cast<uint64_t>(duration_cast<milliseconds>(
                system_clock::now().time_since_epoch()).count());
            LOG_ERROR("failed code:%{public}d columnIndex: %{public}d. times %{public}" PRIu64 ".",
                errCode, columnIndex, time);
        }
    } else {
        LOG_ERROR("GetInnerResultSet failed.");
    }
    return DataShareJSUtils::Convert2JSValue(env, columnIndex);
}

napi_value DataShareResultSetProxy::GetColumnName(napi_env env, napi_callback_info info)
{
    int32_t columnIndex = -1;
    std::string columnName;
    size_t argc = MAX_INPUT_COUNT;
    napi_value args[MAX_INPUT_COUNT] = { 0 };
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    NAPI_ASSERT(env, argc > 0, "Invalid argvs!");
    NAPI_CALL(env, napi_get_value_int32(env, args[0], &columnIndex));
    std::shared_ptr<DataShareResultSet> innerResultSet = GetInnerResultSet(env, info);
    if (innerResultSet != nullptr) {
        int errCode = innerResultSet->GetColumnName(columnIndex, columnName);
        if (errCode != E_OK) {
            LOG_ERROR("failed code:%{public}d", errCode);
        }
    } else {
        LOG_ERROR("GetInnerResultSet failed.");
    }
    return DataShareJSUtils::Convert2JSValue(env, columnName);
}

napi_value DataShareResultSetProxy::GetDataType(napi_env env, napi_callback_info info)
{
    int32_t columnIndex = -1;
    DataType dataType = DataType::TYPE_NULL;
    size_t argc = MAX_INPUT_COUNT;
    napi_value args[MAX_INPUT_COUNT] = { 0 };
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    NAPI_ASSERT(env, argc > 0, "Invalid argvs!");
    NAPI_CALL(env, napi_get_value_int32(env, args[0], &columnIndex));
    if (columnIndex == -1) {
        return DataShareJSUtils::Convert2JSValue(env, int32_t(dataType));
    }
    std::shared_ptr<DataShareResultSet> innerResultSet = GetInnerResultSet(env, info);
    if (innerResultSet != nullptr) {
        int errCode = innerResultSet->GetDataType(columnIndex, dataType);
        if (errCode != E_OK) {
            LOG_ERROR("failed code:%{public}d", errCode);
        }
    } else {
        LOG_ERROR("GetInnerResultSet failed.");
    }
    return DataShareJSUtils::Convert2JSValue(env, int32_t(dataType));
}

napi_value DataShareResultSetProxy::GetAllColumnNames(napi_env env, napi_callback_info info)
{
    std::vector<std::string> columnNames;
    std::shared_ptr<DataShareResultSet> innerResultSet = GetInnerResultSet(env, info);
    if (innerResultSet != nullptr) {
        int errCode = innerResultSet->GetAllColumnNames(columnNames);
        if (errCode != E_OK) {
            LOG_ERROR("failed code:%{public}d", errCode);
        }
    } else {
        LOG_ERROR("GetInnerResultSet failed.");
    }
    return DataShareJSUtils::Convert2JSValue(env, columnNames);
}

napi_value DataShareResultSetProxy::GetColumnCount(napi_env env, napi_callback_info info)
{
    int32_t count = -1;
    std::shared_ptr<DataShareResultSet> innerResultSet = GetInnerResultSet(env, info);
    if (innerResultSet != nullptr) {
        int errCode = innerResultSet->GetColumnCount(count);
        if (errCode != E_OK) {
            LOG_ERROR("failed code:%{public}d", errCode);
        }
    } else {
        LOG_ERROR("GetInnerResultSet failed.");
    }
    return DataShareJSUtils::Convert2JSValue(env, count);
}

napi_value DataShareResultSetProxy::GetRowCount(napi_env env, napi_callback_info info)
{
    int32_t count = -1;
    std::shared_ptr<DataShareResultSet> innerResultSet = GetInnerResultSet(env, info);
    if (innerResultSet != nullptr) {
        int errCode = innerResultSet->GetRowCount(count);
        if (errCode != E_OK) {
            LOG_ERROR("failed code:%{public}d", errCode);
        }
    } else {
        LOG_ERROR("GetInnerResultSet failed.");
    }
    return DataShareJSUtils::Convert2JSValue(env, count);
}

napi_value DataShareResultSetProxy::IsClosed(napi_env env, napi_callback_info info)
{
    bool result = false;
    std::shared_ptr<DataShareResultSet> innerResultSet = GetInnerResultSet(env, info);
    if (innerResultSet != nullptr) {
        result = innerResultSet->IsClosed();
    } else {
        LOG_ERROR("GetInnerResultSet failed.");
    }
    napi_value output;
    napi_get_boolean(env, result, &output);
    return output;
}
} // namespace DataShare
} // namespace OHOS
