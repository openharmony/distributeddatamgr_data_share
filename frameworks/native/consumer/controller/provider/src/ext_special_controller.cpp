/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "ext_special_controller.h"
#include "datashare_log.h"

namespace OHOS {
namespace DataShare {
constexpr int INVALID_VALUE = -1;
int ExtSpecialController::OpenFile(const Uri &uri, const std::string &mode)
{
    auto connection = connection_;
    if (connection == nullptr) {
        LOG_ERROR("connection is nullptr");
        return INVALID_VALUE;
    }
    auto proxy = connection->GetDataShareProxy(uri_, token_);
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return INVALID_VALUE;
    }
    return proxy->OpenFile(uri, mode);
}

int ExtSpecialController::OpenRawFile(const Uri &uri, const std::string &mode)
{
    auto connection = connection_;
    if (connection == nullptr) {
        LOG_ERROR("connection is nullptr");
        return INVALID_VALUE;
    }
    auto proxy = connection->GetDataShareProxy(uri_, token_);
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return INVALID_VALUE;
    }
    return proxy->OpenRawFile(uri, mode);
}

std::string ExtSpecialController::GetType(const Uri &uri)
{
    auto connection = connection_;
    if (connection == nullptr) {
        LOG_ERROR("connection is nullptr");
        return "";
    }
    auto proxy = connection->GetDataShareProxy(uri_, token_);
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return "";
    }
    return proxy->GetType(uri);
}

int ExtSpecialController::BatchInsert(const Uri &uri, const std::vector<DataShareValuesBucket> &values)
{
    auto connection = connection_;
    if (connection == nullptr) {
        LOG_ERROR("connection is nullptr");
        return INVALID_VALUE;
    }
    auto proxy = connection->GetDataShareProxy(uri_, token_);
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return INVALID_VALUE;
    }
    return proxy->BatchInsert(uri, values);
}

int ExtSpecialController::BatchUpdate(const UpdateOperations &operations,
    std::vector<BatchUpdateResult> &results)
{
    auto connection = connection_;
    if (connection == nullptr) {
        LOG_ERROR("connection is nullptr");
        return INVALID_VALUE;
    }
    auto proxy = connection->GetDataShareProxy(uri_, token_);
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return INVALID_VALUE;
    }
    return proxy->BatchUpdate(operations, results);
}

int ExtSpecialController::InsertExt(Uri &uri, const DataShareValuesBucket &value, std::string &result)
{
    auto connection = connection_;
    if (connection == nullptr) {
        LOG_ERROR("connection is nullptr");
        return INVALID_VALUE;
    }
    auto proxy = connection->GetDataShareProxy(uri_, token_);
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return INVALID_VALUE;
    }
    return proxy->InsertExt(uri, value, result);
}

int ExtSpecialController::ExecuteBatch(const std::vector<OperationStatement> &statements, ExecResultSet &result)
{
    auto connection = connection_;
    if (connection == nullptr) {
        LOG_ERROR("connection is nullptr");
        return INVALID_VALUE;
    }
    auto proxy = connection->GetDataShareProxy(uri_, token_);
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return INVALID_VALUE;
    }
    return proxy->ExecuteBatch(statements, result);
}

Uri ExtSpecialController::NormalizeUri(const Uri &uri)
{
    auto connection = connection_;
    if (connection == nullptr) {
        LOG_ERROR("connection is nullptr");
        return Uri("");
    }
    auto proxy = connection->GetDataShareProxy(uri_, token_);
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return Uri("");
    }
    return proxy->NormalizeUri(uri);
}

Uri ExtSpecialController::DenormalizeUri(const Uri &uri)
{
    auto connection = connection_;
    if (connection == nullptr) {
        LOG_ERROR("connection is nullptr");
        return Uri("");
    }
    auto proxy = connection->GetDataShareProxy(uri_, token_);
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return Uri("");
    }
    return proxy->DenormalizeUri(uri);
}

std::vector<std::string> ExtSpecialController::GetFileTypes(const Uri &uri, const std::string &mimeTypeFilter)
{
    auto connection = connection_;
    if (connection == nullptr) {
        LOG_ERROR("connection is nullptr");
        return std::vector<std::string>();
    }
    auto proxy = connection->GetDataShareProxy(uri_, token_);
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr");
        return std::vector<std::string>();
    }
    return proxy->GetFileTypes(uri, mimeTypeFilter);
}

ExtSpecialController::ExtSpecialController(std::shared_ptr<DataShareConnection> connection, const Uri &uri,
    const sptr<IRemoteObject> &token)
    : connection_(connection), token_(token), uri_(uri)
{
}
} // namespace DataShare
} // namespace OHOS
