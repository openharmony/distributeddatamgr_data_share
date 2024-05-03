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

#include "datashare_stub_impl.h"

#include "accesstoken_kit.h"
#include "datashare_log.h"
#include "datashare_string_utils.h"
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

bool DataShareStubImpl::CheckCallingPermission(const std::string &permission)
{
    if (!permission.empty() && AccessTokenKit::VerifyAccessToken(IPCSkeleton::GetCallingTokenID(), permission)
        != AppExecFwk::Constants::PERMISSION_GRANTED) {
        LOG_ERROR("permission not granted.");
        return false;
    }
    return true;
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
    std::function<void()> syncTaskFunc = [extension, info, uri, mimeTypeFilter]() {
        extension->SetCallingInfo(info);
        extension->GetFileTypes(uri, mimeTypeFilter);
    };
    std::function<bool()> getRetFunc = [extension, &ret]() -> bool {
        if (extension == nullptr) {
            return false;
        }
        extension->GetResult(ret);
        return extension->GetRecvReply();
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
    int ret = -1;
    std::function<void()> syncTaskFunc = [extension, info, uri, mode]() {
        extension->SetCallingInfo(info);
        extension->OpenFile(uri, mode);
    };
    std::function<bool()> getRetFunc = [extension, &ret]() -> bool {
        if (extension == nullptr) {
            return false;
        }
        extension->GetResult(ret);
        return extension->GetRecvReply();
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

    int ret = 0;
    std::function<void()> syncTaskFunc = [extension, info, uri, value]() {
        extension->SetCallingInfo(info);
        extension->Insert(uri, value);
    };
    std::function<bool()> getRetFunc = [extension, &ret]() -> bool {
        if (extension == nullptr) {
            return false;
        }
        extension->GetResult(ret);
        return extension->GetRecvReply();
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

    int ret = 0;
    std::function<void()> syncTaskFunc = [extension, info, uri, predicates, value]() {
        extension->SetCallingInfo(info);
        extension->Update(uri, predicates, value);
    };
    std::function<bool()> getRetFunc = [extension, &ret]() -> bool {
        if (extension == nullptr) {
            return false;
        }
        extension->GetResult(ret);
        return extension->GetRecvReply();
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
    std::shared_ptr<int> ret = std::make_shared<int>(0);
    std::function<void()> syncTaskFunc = [extension, ret, operations, info]() {
        extension->SetCallingInfo(info);
        std::vector<BatchUpdateResult> tmp;
        *ret = extension->BatchUpdate(operations, tmp);
    };
    std::function<bool()> getRetFunc = [&results, extension]() -> bool {
        if (extension == nullptr) {
            return false;
        }
        extension->GetResult(results);
        return extension->GetRecvReply();
    };
    std::lock_guard<std::mutex> lock(mutex_);
    uvQueue_->SyncCall(syncTaskFunc, getRetFunc);
    return *ret;
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

    int ret = 0;
    std::function<void()> syncTaskFunc = [extension, info, uri, predicates]() {
        extension->SetCallingInfo(info);
        extension->Delete(uri, predicates);
    };
    std::function<bool()> getRetFunc = [extension, &ret]() -> bool {
        if (extension == nullptr) {
            return false;
        }
        extension->GetResult(ret);
        return extension->GetRecvReply();
    };
    std::lock_guard<std::mutex> lock(mutex_);
    uvQueue_->SyncCall(syncTaskFunc, getRetFunc);
    return ret;
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
        return resultSet;
    }
    
    std::function<void()> syncTaskFunc = [extension, info, uri, predicates, columns]() mutable {
        extension->SetCallingInfo(info);
        DatashareBusinessError businessErr;
        extension->Query(uri, predicates, columns, businessErr);
    };
    std::function<bool()> getRetFunc = [extension, &resultSet, &businessError]() -> bool {
        if (extension == nullptr) {
            return false;
        }
        extension->GetResultSet(resultSet);
        extension->GetBusinessError(businessError);
        return extension->GetRecvReply();
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
    std::function<void()> syncTaskFunc = [extension, info, uri]() {
        if (extension == nullptr) {
            return;
        }
        extension->SetCallingInfo(info);
        extension->GetType(uri);
    };
    std::function<bool()> getRetFunc = [extension, &ret]() -> bool {
        if (extension == nullptr) {
            return false;
        }
        extension->GetResult(ret);
        return extension->GetRecvReply();
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

    int ret = 0;
    std::function<void()> syncTaskFunc = [extension, info, uri, values]() {
        extension->SetCallingInfo(info);
        extension->BatchInsert(uri, values);
    };
    std::function<bool()> getRetFunc = [extension, &ret]() -> bool {
        if (extension == nullptr) {
            return false;
        }
        extension->GetResult(ret);
        return extension->GetRecvReply();
    };
    std::lock_guard<std::mutex> lock(mutex_);
    uvQueue_->SyncCall(syncTaskFunc, getRetFunc);
    return ret;
}

bool DataShareStubImpl::RegisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver)
{
    auto extension = GetOwner();
    if (extension == nullptr) {
        return false;
    }
    if (!CheckCallingPermission(extension->abilityInfo_->readPermission)) {
        LOG_ERROR("Register observer check permission failed. uri: %{public}s",
            DataShareStringUtils::Anonymous(uri.ToString()).c_str());
        return PERMISSION_ERROR_NUMBER;
    }
    return extension->RegisterObserver(uri, dataObserver);
}

bool DataShareStubImpl::UnregisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver)
{
    auto extension = GetOwner();
    if (extension == nullptr) {
        return false;
    }
    if (!CheckCallingPermission(extension->abilityInfo_->readPermission)) {
        LOG_ERROR("UnRegister observer check permission failed. uri: %{public}s",
            DataShareStringUtils::Anonymous(uri.ToString()).c_str());
        return PERMISSION_ERROR_NUMBER;
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

    std::function<void()> syncTaskFunc = [extension, ret, uri]() {
        *ret = extension->NotifyChange(uri);
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

    std::function<void()> syncTaskFunc = [extension, info, uri]() {
        extension->SetCallingInfo(info);
        extension->NormalizeUri(uri);
    };
    std::function<bool()> getRetFunc = [extension, &normalizeUri]() -> bool {
        if (extension == nullptr) {
            return false;
        }
        std::string ret;
        extension->GetResult(ret);
        Uri tmp(ret);
        normalizeUri = tmp;
        return extension->GetRecvReply();
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
    std::function<void()> syncTaskFunc = [extension, info, uri]() {
        extension->SetCallingInfo(info);
        extension->DenormalizeUri(uri);
    };
    std::function<bool()> getRetFunc = [extension, &denormalizedUri]() -> bool {
        if (extension == nullptr) {
            return false;
        }
        std::string ret;
        extension->GetResult(ret);
        Uri tmp(ret);
        denormalizedUri = tmp;
        return extension->GetRecvReply();
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