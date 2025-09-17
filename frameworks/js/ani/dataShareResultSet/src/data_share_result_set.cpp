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

#include <ani_utils.h>
#include <array>
#include <iostream>
#include <vector>
#include <functional>
#include <chrono>
#include <cinttypes>

#include "datashare_errno.h"
#include "datashare_log.h"
#include "datashare_result_set.h"

using namespace OHOS::DataShare;

static std::shared_ptr<DataShareResultSet> GetResultSet(ani_env *env, ani_object obj)
{
    auto holder = AniObjectUtils::Unwrap<SharedPtrHolder<DataShareResultSet>>(env, obj);
    if (holder == nullptr) {
        LOG_ERROR("SharedPtrHolder is NULL");
        return nullptr;
    }
    if (holder->Get() == nullptr) {
        LOG_ERROR("Holder shared_ptr is NULL");
        return nullptr;
    }

    return holder->Get();
}

static void Close([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object obj)
{
    static const char *className = "@ohos.data.DataShareResultSet.DataShareResultSetImpl";
    auto classObj = GetResultSet(env, obj);
    if (classObj == nullptr) {
        LOG_ERROR("DataShareResultSet is NULL");
        return;
    }
    int errCode = classObj->Close();
    if (errCode != E_OK) {
        LOG_ERROR("failed code:%{public}d", errCode);
    }
    ani_class cls;
    if (ANI_OK != env->FindClass(className, &cls)) {
        LOG_ERROR("[ANI] Not found class %{public}s", className);
        return;
    }

    ani_method setNaptr;
    if (ANI_OK != env->Class_FindMethod(cls, "clearNativePtr", nullptr, &setNaptr)) {
        LOG_ERROR("[ANI] Not found clearNativePtr for class %{public}s", className);
        return;
    }

    if (ANI_OK != env->Object_CallMethod_Void(obj, setNaptr)) {
        LOG_ERROR("[ANI] Object_CallMethod_Void failed");
        return;
    }
    return;
}

static ani_double GetColumnIndex([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object obj, ani_string columnName)
{
    int32_t columnIndex = -1;
    if (columnName == nullptr) {
        return static_cast<ani_double>(columnIndex);
    }
    auto columnNameStr = AniStringUtils::ToStd(env, columnName);

    auto classObj = GetResultSet(env, obj);
    if (classObj == nullptr) {
        LOG_ERROR("DataShareResultSet is NULL");
        return static_cast<ani_double>(columnIndex);
    }
    int errCode = classObj->GetColumnIndex(columnNameStr, columnIndex);
    if (errCode != E_OK) {
        auto time = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count());
        LOG_ERROR("failed code:%{public}d columnIndex: %{public}d. times %{public}" PRIu64 ".", errCode, columnIndex,
                  time);
    }
    return static_cast<ani_double>(columnIndex);
}

static ani_string GetString([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object obj, ani_double columnIndex)
{
    std::string value;
    double columnIndexD = static_cast<double>(columnIndex);
    auto classObj = GetResultSet(env, obj);
    if (classObj == nullptr) {
        LOG_ERROR("DataShareResultSet is NULL");
        return nullptr;
    }
    classObj->GetString(columnIndexD, value);
    return AniStringUtils::ToAni(env, value);
}

static bool GoToFirstRow([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object obj)
{
    auto classObj = GetResultSet(env, obj);
    if (classObj == nullptr) {
        LOG_ERROR("DataShareResultSet is NULL");
        return false;
    }

    int errCode = classObj->GoToFirstRow();
    if (errCode != E_OK) {
        LOG_ERROR("failed code:%{public}d", errCode);
        return false;
    }
    return true;
}

static ani_double GetRowCount([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object obj)
{
    int32_t count = -1;
    auto classObj = GetResultSet(env, obj);
    if (classObj == nullptr) {
        LOG_ERROR("DataShareResultSet is NULL");
        return static_cast<ani_double>(count);
    }
    classObj->GetRowCount(count);
    return static_cast<ani_double>(count);
}

ANI_EXPORT ani_status ANI_Constructor(ani_vm *vm, uint32_t *result)
{
    LOG_INFO("enter ANI_Constructor func");
    ani_env *env;
    if (ANI_OK != vm->GetEnv(ANI_VERSION_1, &env)) {
        LOG_ERROR("Unsupported ANI_VERSION_1");
        return ANI_ERROR;
    }

    ani_class cls;
    static const char *className = "@ohos.data.DataShareResultSet.DataShareResultSetImpl";
    if (ANI_OK != env->FindClass(className, &cls)) {
        LOG_ERROR("Not found class:%{public}s", className);
        return ANI_ERROR;
    }

    std::array methods = {
        ani_native_function {"close", nullptr, reinterpret_cast<void *>(Close)},
        ani_native_function {"getColumnIndex", nullptr, reinterpret_cast<void *>(GetColumnIndex)},
        ani_native_function {"getString", nullptr, reinterpret_cast<void *>(GetString)},
        ani_native_function {"goToFirstRow", nullptr, reinterpret_cast<void *>(GoToFirstRow)},
        ani_native_function {"getRowCount", nullptr, reinterpret_cast<void *>(GetRowCount)},
    };

    if (ANI_OK != env->Class_BindNativeMethods(cls, methods.data(), methods.size())) {
        LOG_ERROR("Cannot bind native methods to {%public}s", className);
        return ANI_ERROR;
    };

    *result = ANI_VERSION_1;
    return ANI_OK;
}