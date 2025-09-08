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

#define LOG_TAG "datashare_stub_impl"

#include "datashare_stub_impl.h"
#include <string>

#include "accesstoken_kit.h"
#include "bundle_constants.h"
#include "bundle_mgr_helper.h"
#include "dataobs_mgr_client.h"
#include "data_share_config.h"
#include "datashare_errno.h"
#include "datashare_log.h"
#include "datashare_string_utils.h"
#include "hiview_datashare.h"
#include "ipc_skeleton.h"

namespace OHOS {
namespace DataShare {
using OHOS::Security::AccessToken::AccessTokenKit;

constexpr int DEFAULT_NUMBER = -1;
constexpr int PERMISSION_ERROR_NUMBER = -2;
std::shared_ptr<JsDataShareExtAbility> DataShareStubImpl::GetOwner()
{
    if (extension_ == nullptr) {
        LOG_ERROR("extension_ is nullptr.");
    }
    return extension_;
}

int CheckTrusts(uint32_t consumerToken, uint32_t providerToken)
{
    auto obsMgrClient = AAFwk::DataObsMgrClient::GetInstance();
    if (obsMgrClient == nullptr) {
        LOG_ERROR("obsMgrClient is nullptr");
        return E_DATA_OBS_NOT_READY;
    }
    ErrCode ret = obsMgrClient->CheckTrusts(consumerToken, providerToken);
    if (ret != E_OK) {
        LOG_ERROR("CheckTrusts error return %{public}d", ret);
        return ret;
    }
    return E_OK;
}

bool DataShareStubImpl::CheckCallingPermission(const std::string &permission)
{
    uint32_t token = IPCSkeleton::GetCallingTokenID();
    if (permission.empty() || AccessTokenKit::VerifyAccessToken(token, permission)
        == AppExecFwk::Constants::PERMISSION_GRANTED) {
        return true;
    }
    LOG_WARN("permission not granted. permission %{public}s, token %{public}d", permission.c_str(), token);
    return false;
}

std::vector<std::string> DataShareStubImpl::GetFileTypes(const Uri &uri, const std::string &mimeTypeFilter)
{
    CallingInfo info;
    GetCallingInfo(info);
    std::vector<std::string> ret;
    auto client = sptr<DataShareStubImpl>(this);
    auto extension = client->GetOwner();
    if (extension == nullptr) {
        return ret;
    }
    auto result = std::make_shared<JsResult>();
    std::function<void()> syncTaskFunc = [extension, info, uri, mimeTypeFilter, result]() {
        extension->SetCallingInfo(info);
        extension->InitResult(result);
        extension->GetFileTypes(uri, mimeTypeFilter);
    };
    std::function<bool()> getRetFunc = [result, &ret]() -> bool {
        if (result == nullptr) {
            return false;
        }
        bool isRecvReply = result->GetRecvReply();
        result->GetResult(ret);
        return isRecvReply;
    };
    std::lock_guard<std::mutex> lock(mutex_);
    uvQueue_->SyncCall(syncTaskFunc, getRetFunc);
    return ret;
}

int DataShareStubImpl::OpenFile(const Uri &uri, const std::string &mode)
{
    CallingInfo info;
    GetCallingInfo(info);
    auto client = sptr<DataShareStubImpl>(this);
    auto extension = client->GetOwner();
    if (extension == nullptr) {
        return DEFAULT_NUMBER;
    }
    auto result = std::make_shared<JsResult>();
    int ret = -1;
    std::function<void()> syncTaskFunc = [extension, info, uri, mode, result]() {
        extension->SetCallingInfo(info);
        extension->InitResult(result);
        extension->OpenFile(uri, mode);
    };
    std::function<bool()> getRetFunc = [result, &ret]() -> bool {
        if (result == nullptr) {
            return false;
        }
        bool isRecvReply = result->GetRecvReply();
        result->GetResult(ret);
        return isRecvReply;
    };
    std::lock_guard<std::mutex> lock(mutex_);
    uvQueue_->SyncCall(syncTaskFunc, getRetFunc);
    return ret;
}

int DataShareStubImpl::OpenRawFile(const Uri &uri, const std::string &mode)
{
    CallingInfo info;
    GetCallingInfo(info);
    auto client = sptr<DataShareStubImpl>(this);
    auto extension = client->GetOwner();
    if (extension == nullptr) {
        return DEFAULT_NUMBER;
    }

    std::shared_ptr<int> ret = std::make_shared<int>(-1);
    std::function<void()> syncTaskFunc = [extension, ret, info, uri, mode]() {
        extension->SetCallingInfo(info);
        *ret = extension->OpenRawFile(uri, mode);
    };
    std::lock_guard<std::mutex> lock(mutex_);
    uvQueue_->SyncCall(syncTaskFunc);
    return *ret;
}

int DataShareStubImpl::Insert(const Uri &uri, const DataShareValuesBucket &value)
{
    CallingInfo info;
    GetCallingInfo(info);

    auto client = sptr<DataShareStubImpl>(this);
    auto extension = client->GetOwner();
    if (extension == nullptr) {
        return DEFAULT_NUMBER;
    }

    if (!CheckCallingPermission(extension->abilityInfo_->writePermission)) {
        LOG_ERROR("Check calling permission failed.");
        return PERMISSION_ERROR_NUMBER;
    }

    auto result = std::make_shared<JsResult>();
    int ret = 0;
    std::function<void()> syncTaskFunc = [extension, info, uri, value, result]() {
        extension->SetCallingInfo(info);
        extension->InitResult(result);
        extension->Insert(uri, value);
    };
    std::function<bool()> getRetFunc = [result, &ret]() -> bool {
        if (result == nullptr) {
            return false;
        }
        bool isRecvReply = result->GetRecvReply();
        result->GetResult(ret);
        return isRecvReply;
    };
    std::lock_guard<std::mutex> lock(mutex_);
    uvQueue_->SyncCall(syncTaskFunc, getRetFunc);
    return ret;
}

int DataShareStubImpl::Update(const Uri &uri, const DataSharePredicates &predicates,
    const DataShareValuesBucket &value)
{
    CallingInfo info;
    GetCallingInfo(info);

    auto client = sptr<DataShareStubImpl>(this);
    auto extension = client->GetOwner();
    if (extension == nullptr) {
        return DEFAULT_NUMBER;
    }

    if (!CheckCallingPermission(extension->abilityInfo_->writePermission)) {
        LOG_ERROR("Check calling permission failed.");
        return PERMISSION_ERROR_NUMBER;
    }

    auto result = std::make_shared<JsResult>();
    int ret = 0;
    std::function<void()> syncTaskFunc = [extension, info, uri, predicates, value, result]() {
        extension->SetCallingInfo(info);
        extension->InitResult(result);
        extension->Update(uri, predicates, value);
    };
    std::function<bool()> getRetFunc = [result, &ret]() -> bool {
        if (result == nullptr) {
            return false;
        }
        bool isRecvReply = result->GetRecvReply();
        result->GetResult(ret);
        return isRecvReply;
    };
    std::lock_guard<std::mutex> lock(mutex_);
    uvQueue_->SyncCall(syncTaskFunc, getRetFunc);
    return ret;
}

int DataShareStubImpl::BatchUpdate(const UpdateOperations &operations, std::vector<BatchUpdateResult> &results)
{
    CallingInfo info;
    GetCallingInfo(info);
    auto client = sptr<DataShareStubImpl>(this);
    auto extension = client->GetOwner();
    if (extension == nullptr) {
        return DEFAULT_NUMBER;
    }
    if (!CheckCallingPermission(extension->abilityInfo_->writePermission)) {
        LOG_ERROR("Check calling permission failed.");
        return PERMISSION_ERROR_NUMBER;
    }
    auto result = std::make_shared<JsResult>();
    int ret = 0;
    std::function<void()> syncTaskFunc = [extension, operations, info, result]() {
        extension->SetCallingInfo(info);
        extension->InitResult(result);
        std::vector<BatchUpdateResult> tmp;
        extension->BatchUpdate(operations, tmp);
    };
    std::function<bool()> getRetFunc = [&results, result, &ret]() -> bool {
        if (result == nullptr) {
            return false;
        }
        bool isRecvReply = result->GetRecvReply();
        result->GetResult(results);
        result->GetResult(ret);
        return isRecvReply;
    };
    std::lock_guard<std::mutex> lock(mutex_);
    uvQueue_->SyncCall(syncTaskFunc, getRetFunc);
    return ret;
}

int DataShareStubImpl::Delete(const Uri &uri, const DataSharePredicates &predicates)
{
    CallingInfo info;
    GetCallingInfo(info);

    auto client = sptr<DataShareStubImpl>(this);
    auto extension = client->GetOwner();
    if (extension == nullptr) {
        return DEFAULT_NUMBER;
    }

    if (!CheckCallingPermission(extension->abilityInfo_->writePermission)) {
        LOG_ERROR("Check calling permission failed.");
        return PERMISSION_ERROR_NUMBER;
    }

    auto result = std::make_shared<JsResult>();
    int ret = 0;
    std::function<void()> syncTaskFunc = [extension, info, uri, predicates, result]() {
        extension->SetCallingInfo(info);
        extension->InitResult(result);
        extension->Delete(uri, predicates);
    };
    std::function<bool()> getRetFunc = [result, &ret]() -> bool {
        if (result == nullptr) {
            return false;
        }
        bool isRecvReply = result->GetRecvReply();
        result->GetResult(ret);
        return isRecvReply;
    };
    std::lock_guard<std::mutex> lock(mutex_);
    uvQueue_->SyncCall(syncTaskFunc, getRetFunc);
    return ret;
}

std::pair<int32_t, int32_t> DataShareStubImpl::InsertEx(const Uri &uri, const DataShareValuesBucket &value)
{
    CallingInfo info;
    GetCallingInfo(info);

    auto client = sptr<DataShareStubImpl>(this);
    auto extension = client->GetOwner();
    if (extension == nullptr) {
        return std::make_pair(DATA_SHARE_ERROR, 0);
    }

    if (!CheckCallingPermission(extension->abilityInfo_->writePermission)) {
        LOG_ERROR("Check calling permission failed.");
        return std::make_pair(PERMISSION_ERROR_NUMBER, 0);
    }

    auto result = std::make_shared<JsResult>();
    int ret = 0;
    std::function<void()> syncTaskFunc = [extension, info, uri, value, result]() {
        extension->SetCallingInfo(info);
        extension->InitResult(result);
        extension->Insert(uri, value);
    };
    std::function<bool()> getRetFunc = [result, &ret]() -> bool {
        if (result == nullptr) {
            return false;
        }
        bool isRecvReply = result->GetRecvReply();
        result->GetResult(ret);
        return isRecvReply;
    };
    std::lock_guard<std::mutex> lock(mutex_);
    uvQueue_->SyncCall(syncTaskFunc, getRetFunc);
    return std::make_pair(E_OK, ret);
}

std::pair<int32_t, int32_t> DataShareStubImpl::UpdateEx(const Uri &uri, const DataSharePredicates &predicates,
    const DataShareValuesBucket &value)
{
    CallingInfo info;
    GetCallingInfo(info);

    auto client = sptr<DataShareStubImpl>(this);
    auto extension = client->GetOwner();
    if (extension == nullptr) {
        return std::make_pair(DATA_SHARE_ERROR, 0);
    }

    if (!CheckCallingPermission(extension->abilityInfo_->writePermission)) {
        LOG_ERROR("Check calling permission failed.");
        return std::make_pair(PERMISSION_ERROR_NUMBER, 0);
    }

    auto result = std::make_shared<JsResult>();
    int ret = 0;
    std::function<void()> syncTaskFunc = [extension, info, uri, predicates, value, result]() {
        extension->SetCallingInfo(info);
        extension->InitResult(result);
        extension->Update(uri, predicates, value);
    };
    std::function<bool()> getRetFunc = [result, &ret]() -> bool {
        if (result == nullptr) {
            return false;
        }
        bool isRecvReply = result->GetRecvReply();
        result->GetResult(ret);
        return isRecvReply;
    };
    std::lock_guard<std::mutex> lock(mutex_);
    uvQueue_->SyncCall(syncTaskFunc, getRetFunc);
    return std::make_pair(E_OK, ret);
}

std::pair<int32_t, int32_t> DataShareStubImpl::DeleteEx(const Uri &uri, const DataSharePredicates &predicates)
{
    CallingInfo info;
    GetCallingInfo(info);

    auto client = sptr<DataShareStubImpl>(this);
    auto extension = client->GetOwner();
    if (extension == nullptr) {
        return std::make_pair(DATA_SHARE_ERROR, 0);
    }

    if (!CheckCallingPermission(extension->abilityInfo_->writePermission)) {
        LOG_ERROR("Check calling permission failed.");
        return std::make_pair(PERMISSION_ERROR_NUMBER, 0);
    }

    int ret = 0;
    auto result = std::make_shared<JsResult>();
    std::function<void()> syncTaskFunc = [extension, info, uri, predicates, result]() {
        extension->SetCallingInfo(info);
        extension->InitResult(result);
        extension->Delete(uri, predicates);
    };
    std::function<bool()> getRetFunc = [result, &ret]() -> bool {
        if (result == nullptr) {
            return false;
        }
        bool isRecvReply = result->GetRecvReply();
        result->GetResult(ret);
        return isRecvReply;
    };
    std::lock_guard<std::mutex> lock(mutex_);
    uvQueue_->SyncCall(syncTaskFunc, getRetFunc);
    return std::make_pair(E_OK, ret);
}

std::shared_ptr<DataShareResultSet> DataShareStubImpl::Query(const Uri &uri,
    const DataSharePredicates &predicates, std::vector<std::string> &columns, DatashareBusinessError &businessError)
{
    CallingInfo info;
    GetCallingInfo(info);
    std::shared_ptr<DataShareResultSet> resultSet = nullptr;
    auto client = sptr<DataShareStubImpl>(this);
    auto extension = client->GetOwner();
    if (extension == nullptr) {
        return resultSet;
    }

    if (!CheckCallingPermission(extension->abilityInfo_->readPermission)) {
        LOG_ERROR("Check calling permission failed.");
        businessError.SetCode(PERMISSION_ERROR_NUMBER);
        return resultSet;
    }
    auto result = std::make_shared<JsResult>();
    std::function<void()> syncTaskFunc = [extension, info, uri, predicates, columns, result]() mutable {
        extension->SetCallingInfo(info);
        extension->InitResult(result);
        DatashareBusinessError businessErr;
        extension->Query(uri, predicates, columns, businessErr);
    };
    std::function<bool()> getRetFunc = [result, &resultSet, &businessError]() -> bool {
        if (result == nullptr) {
            return false;
        }
        auto isRecvReply = result->GetRecvReply();
        result->GetResultSet(resultSet);
        result->GetBusinessError(businessError);
        return isRecvReply;
    };
    std::lock_guard<std::mutex> lock(mutex_);
    uvQueue_->SyncCall(syncTaskFunc, getRetFunc);
    return resultSet;
}

std::string DataShareStubImpl::GetType(const Uri &uri)
{
    CallingInfo info;
    GetCallingInfo(info);
    std::string ret = "";
    auto client = sptr<DataShareStubImpl>(this);
    auto extension = client->GetOwner();
    if (extension == nullptr) {
        return ret;
    }
    auto result = std::make_shared<JsResult>();
    std::function<void()> syncTaskFunc = [extension, info, uri, result]() {
        if (extension == nullptr) {
            return;
        }
        extension->SetCallingInfo(info);
        extension->InitResult(result);
        extension->GetType(uri);
    };
    std::function<bool()> getRetFunc = [result, &ret]() -> bool {
        if (result == nullptr) {
            return false;
        }
        bool isRecvReply = result->GetRecvReply();
        result->GetResult(ret);
        return isRecvReply;
    };
    std::lock_guard<std::mutex> lock(mutex_);
    uvQueue_->SyncCall(syncTaskFunc, getRetFunc);
    return ret;
}

int DataShareStubImpl::BatchInsert(const Uri &uri, const std::vector<DataShareValuesBucket> &values)
{
    CallingInfo info;
    GetCallingInfo(info);
    auto client = sptr<DataShareStubImpl>(this);
    auto extension = client->GetOwner();
    if (extension == nullptr) {
        return DEFAULT_NUMBER;
    }

    if (!CheckCallingPermission(extension->abilityInfo_->writePermission)) {
        LOG_ERROR("Check calling permission failed.");
        return PERMISSION_ERROR_NUMBER;
    }

    auto result = std::make_shared<JsResult>();
    int ret = 0;
    std::function<void()> syncTaskFunc = [extension, info, uri, values, result]() {
        extension->SetCallingInfo(info);
        extension->InitResult(result);
        extension->BatchInsert(uri, values);
    };
    std::function<bool()> getRetFunc = [result, &ret]() -> bool {
        if (result == nullptr) {
            return false;
        }
        bool isRecvReply = result->GetRecvReply();
        result->GetResult(ret);
        return isRecvReply;
    };
    std::lock_guard<std::mutex> lock(mutex_);
    uvQueue_->SyncCall(syncTaskFunc, getRetFunc);
    return ret;
}

int32_t DataShareStubImpl::GetCallingUserId()
{
    uint32_t tokenId = IPCSkeleton::GetCallingTokenID();
    auto type = Security::AccessToken::AccessTokenKit::GetTokenTypeFlag(tokenId);
    if (type == Security::AccessToken::TOKEN_NATIVE || type == Security::AccessToken::TOKEN_SHELL) {
        return 0;
    } else {
        int32_t callingUid = IPCSkeleton::GetCallingUid();
        int32_t userId = callingUid / Constants::BASE_USER_RANGE;
        return userId;
    }
}

bool DataShareStubImpl::RegisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver)
{
    auto extension = GetOwner();
    if (extension == nullptr) {
        return false;
    }
    uint32_t callingToken = IPCSkeleton::GetCallingTokenID();
    bool isSuccess = CheckCallingPermission(extension->abilityInfo_->readPermission);
    if (!isSuccess) {
        uint32_t selfToken = IPCSkeleton::GetSelfTokenID();
        if (CheckTrusts(callingToken, selfToken) != E_OK) {
            LOG_ERROR("Register observer check permission failed. uri: %{public}s, token:%{public}d",
                DataShareStringUtils::Anonymous(uri.ToString()).c_str(), callingToken);
            // just log
        }
    }
    return extension->RegisterObserver(uri, dataObserver);
}

bool DataShareStubImpl::UnregisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver)
{
    auto extension = GetOwner();
    if (extension == nullptr) {
        return false;
    }
    uint32_t callingToken = IPCSkeleton::GetCallingTokenID();
    bool isSuccess = CheckCallingPermission(extension->abilityInfo_->readPermission);
    if (!isSuccess) {
        uint32_t selfToken = IPCSkeleton::GetSelfTokenID();
        if (CheckTrusts(callingToken, selfToken) != E_OK) {
            LOG_ERROR("UnRegister observer check permission failed. uri: %{public}s, token:%{public}d",
                DataShareStringUtils::Anonymous(uri.ToString()).c_str(), callingToken);
            // just log
        }
    }
    
    return extension->UnregisterObserver(uri, dataObserver);
}

bool DataShareStubImpl::NotifyChange(const Uri &uri)
{
    std::shared_ptr<bool> ret = std::make_shared<bool>(false);
    auto client = sptr<DataShareStubImpl>(this);
    auto extension = client->GetOwner();
    if (extension == nullptr) {
        return *ret;
    }
    uint32_t callingToken = IPCSkeleton::GetCallingTokenID();
    bool isSuccess = CheckCallingPermission(extension->abilityInfo_->writePermission);
    if (!isSuccess) {
        uint32_t selfToken = IPCSkeleton::GetSelfTokenID();
        if (CheckTrusts(callingToken, selfToken) != E_OK) {
            LOG_ERROR("extension NotifyChange check permission failed. uri: %{public}s token %{public}d",
                uri.ToString().c_str(), IPCSkeleton::GetCallingTokenID());
            // just log
        }
    }

    int32_t callingUserId = GetCallingUserId();
    std::function<void()> syncTaskFunc = [extension, ret, uri, callingUserId]() {
        *ret = extension->NotifyChangeWithUser(uri, callingUserId);
    };
    std::lock_guard<std::mutex> lock(mutex_);
    uvQueue_->SyncCall(syncTaskFunc);
    return *ret;
}

Uri DataShareStubImpl::NormalizeUri(const Uri &uri)
{
    CallingInfo info;
    GetCallingInfo(info);
    Uri normalizeUri("");
    auto client = sptr<DataShareStubImpl>(this);
    auto extension = client->GetOwner();
    if (extension == nullptr) {
        return normalizeUri;
    }

    auto result = std::make_shared<JsResult>();
    std::function<void()> syncTaskFunc = [extension, info, uri, result]() {
        extension->SetCallingInfo(info);
        extension->InitResult(result);
        extension->NormalizeUri(uri);
    };
    std::function<bool()> getRetFunc = [result, &normalizeUri]() -> bool {
        if (result == nullptr) {
            return false;
        }
        bool isRecvReply = result->GetRecvReply();
        std::string ret;
        result->GetResult(ret);
        Uri tmp(ret);
        normalizeUri = tmp;
        return isRecvReply;
    };
    std::lock_guard<std::mutex> lock(mutex_);
    uvQueue_->SyncCall(syncTaskFunc, getRetFunc);
    return normalizeUri;
}

Uri DataShareStubImpl::DenormalizeUri(const Uri &uri)
{
    CallingInfo info;
    GetCallingInfo(info);
    Uri denormalizedUri("");
    auto client = sptr<DataShareStubImpl>(this);
    auto extension = client->GetOwner();
    if (extension == nullptr) {
        return denormalizedUri;
    }
    auto result = std::make_shared<JsResult>();
    std::function<void()> syncTaskFunc = [extension, info, uri, result]() {
        extension->SetCallingInfo(info);
        extension->InitResult(result);
        extension->DenormalizeUri(uri);
    };
    std::function<bool()> getRetFunc = [result, &denormalizedUri]() -> bool {
        if (result == nullptr) {
            return false;
        }
        bool isRecvReply = result->GetRecvReply();
        std::string ret;
        result->GetResult(ret);
        Uri tmp(ret);
        denormalizedUri = tmp;
        return isRecvReply;
    };
    std::lock_guard<std::mutex> lock(mutex_);
    uvQueue_->SyncCall(syncTaskFunc, getRetFunc);
    return denormalizedUri;
}

void DataShareStubImpl::GetCallingInfo(CallingInfo& callingInfo)
{
    callingInfo.callingTokenId = GetCallingTokenID();
    callingInfo.callingPid = GetCallingPid();
    callingInfo.callingUid = GetCallingUid();
}
} // namespace DataShare
} // namespace OHOS