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
    std::vector<std::string> ret;
    std::function<void()> syncTaskFunc = [=, &ret, client = sptr<DataShareStubImpl>(this)]() {
        auto extension = client->GetOwner();
        if (extension == nullptr) {
            return;
        }
        ret = extension->GetFileTypes(uri, mimeTypeFilter);
    };
    std::function<bool()> getRetFunc = [=, &ret, client = sptr<DataShareStubImpl>(this)]() -> bool {
        auto extension = client->GetOwner();
        if (extension == nullptr) {
            return false;
        }
        extension->GetResult(ret);
        return (ret.size() != 0);
    };
    uvQueue_->SyncCall(syncTaskFunc, getRetFunc);
    return ret;
}

int DataShareStubImpl::OpenFile(const Uri &uri, const std::string &mode)
{
    int ret = -1;
    std::function<void()> syncTaskFunc = [=, &ret, client = sptr<DataShareStubImpl>(this)]() {
        auto extension = client->GetOwner();
        if (extension == nullptr) {
            return;
        }
        ret = extension->OpenFile(uri, mode);
    };
    std::function<bool()> getRetFunc = [=, &ret, client = sptr<DataShareStubImpl>(this)]() -> bool {
        auto extension = client->GetOwner();
        if (extension == nullptr) {
            return false;
        }
        extension->GetResult(ret);
        return (ret != DEFAULT_NUMBER);
    };
    uvQueue_->SyncCall(syncTaskFunc, getRetFunc);
    return ret;
}

int DataShareStubImpl::OpenRawFile(const Uri &uri, const std::string &mode)
{
    int ret = -1;
    std::function<void()> syncTaskFunc = [=, &ret, client = sptr<DataShareStubImpl>(this)]() {
        auto extension = client->GetOwner();
        if (extension == nullptr) {
            return;
        }
        ret = extension->OpenRawFile(uri, mode);
    };
    uvQueue_->SyncCall(syncTaskFunc);
    return ret;
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
    std::function<void()> syncTaskFunc = [=, &ret, &extension]() {
        extension->SetCallingInfo(info);
        ret = extension->Insert(uri, value);
    };
    std::function<bool()> getRetFunc = [=, &ret, client = sptr<DataShareStubImpl>(this)]() -> bool {
        auto extension = client->GetOwner();
        if (extension == nullptr) {
            return false;
        }
        extension->GetResult(ret);
        return (ret != DEFAULT_NUMBER);
    };
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
    std::function<void()> syncTaskFunc = [=, &ret, &extension]() {
        extension->SetCallingInfo(info);
        ret = extension->Update(uri, predicates, value);
    };
    std::function<bool()> getRetFunc = [=, &ret, client = sptr<DataShareStubImpl>(this)]() -> bool {
        auto extension = client->GetOwner();
        if (extension == nullptr) {
            return false;
        }
        extension->GetResult(ret);
        return (ret != DEFAULT_NUMBER);
    };
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

    int ret = 0;
    std::function<void()> syncTaskFunc = [=, &ret, &extension]() {
        extension->SetCallingInfo(info);
        ret = extension->Delete(uri, predicates);
    };
    std::function<bool()> getRetFunc = [=, &ret, client = sptr<DataShareStubImpl>(this)]() -> bool {
        auto extension = client->GetOwner();
        if (extension == nullptr) {
            return false;
        }
        extension->GetResult(ret);
        return (ret != DEFAULT_NUMBER);
    };
    uvQueue_->SyncCall(syncTaskFunc, getRetFunc);
    return ret;
}

std::shared_ptr<DataShareResultSet> DataShareStubImpl::Query(const Uri &uri,
    const DataSharePredicates &predicates, std::vector<std::string> &columns)
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

    std::function<void()> syncTaskFunc = [=, &columns, &resultSet, &extension]() {
        resultSet = extension->Query(uri, predicates, columns);
    };
    std::function<bool()> getRetFunc = [=, &resultSet, client = sptr<DataShareStubImpl>(this)]() -> bool {
        auto extension = client->GetOwner();
        if (extension == nullptr) {
            return false;
        }
        extension->SetCallingInfo(info);
        extension->GetResult(resultSet);
        return (resultSet != nullptr);
    };
    std::lock_guard<std::mutex> lock(mutex_);
    uvQueue_->SyncCall(syncTaskFunc, getRetFunc);
    return resultSet;
}

std::string DataShareStubImpl::GetType(const Uri &uri)
{
    std::string ret = "";
    std::function<void()> syncTaskFunc = [=, &ret, client = sptr<DataShareStubImpl>(this)]() {
        auto extension = client->GetOwner();
        if (extension == nullptr) {
            return;
        }
        ret = extension->GetType(uri);
    };
    std::function<bool()> getRetFunc = [=, &ret, client = sptr<DataShareStubImpl>(this)]() -> bool {
        auto extension = client->GetOwner();
        if (extension == nullptr) {
            return false;
        }
        extension->GetResult(ret);
        return (ret != "");
    };
    uvQueue_->SyncCall(syncTaskFunc, getRetFunc);
    return ret;
}

int DataShareStubImpl::BatchInsert(const Uri &uri, const std::vector<DataShareValuesBucket> &values)
{
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
    std::function<void()> syncTaskFunc = [=, &ret, &extension]() {
        auto extension = client->GetOwner();
        if (extension == nullptr) {
            return;
        }
        ret = extension->BatchInsert(uri, values);
    };
    std::function<bool()> getRetFunc = [=, &ret, client = sptr<DataShareStubImpl>(this)]() -> bool {
        auto extension = client->GetOwner();
        if (extension == nullptr) {
            return false;
        }
        extension->GetResult(ret);
        return (ret != DEFAULT_NUMBER);
    };
    uvQueue_->SyncCall(syncTaskFunc, getRetFunc);
    return ret;
}

bool DataShareStubImpl::RegisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver)
{
    auto extension = GetOwner();
    if (extension == nullptr) {
        return false;
    }
    return extension->RegisterObserver(uri, dataObserver);
}

bool DataShareStubImpl::UnregisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver)
{
    auto extension = GetOwner();
    if (extension == nullptr) {
        return false;
    }
    return extension->UnregisterObserver(uri, dataObserver);
}

bool DataShareStubImpl::NotifyChange(const Uri &uri)
{
    bool ret = false;
    std::function<void()> syncTaskFunc = [=, &ret, client = sptr<DataShareStubImpl>(this)]() {
        auto extension = client->GetOwner();
        if (extension == nullptr) {
            return;
        }
        ret = extension->NotifyChange(uri);
    };
    uvQueue_->SyncCall(syncTaskFunc);
    return ret;
}

Uri DataShareStubImpl::NormalizeUri(const Uri &uri)
{
    Uri urivalue("");
    std::function<void()> syncTaskFunc = [=, &urivalue, client = sptr<DataShareStubImpl>(this)]() {
        auto extension = client->GetOwner();
        if (extension == nullptr) {
            return;
        }
        urivalue = extension->NormalizeUri(uri);
    };
    std::function<bool()> getRetFunc = [=, &urivalue, client = sptr<DataShareStubImpl>(this)]() -> bool {
        auto extension = client->GetOwner();
        if (extension == nullptr) {
            return false;
        }
        std::string ret;
        extension->GetResult(ret);
        Uri tmp(ret);
        urivalue = tmp;
        return (urivalue.ToString() != "");
    };
    uvQueue_->SyncCall(syncTaskFunc, getRetFunc);
    return urivalue;
}

Uri DataShareStubImpl::DenormalizeUri(const Uri &uri)
{
    Uri urivalue("");
    std::function<void()> syncTaskFunc = [=, &urivalue, client = sptr<DataShareStubImpl>(this)]() {
        auto extension = client->GetOwner();
        if (extension == nullptr) {
            return;
        }
        urivalue = extension->DenormalizeUri(uri);
    };
    std::function<bool()> getRetFunc = [=, &urivalue, client = sptr<DataShareStubImpl>(this)]() -> bool {
        auto extension = client->GetOwner();
        if (extension == nullptr) {
            return false;
        }
        std::string ret;
        extension->GetResult(ret);
        Uri tmp(ret);
        urivalue = tmp;
        return (urivalue.ToString() != "");
    };
    uvQueue_->SyncCall(syncTaskFunc, getRetFunc);
    return urivalue;
}

void DataShareStubImpl::GetCallingInfo(CallingInfo& callingInfo)
{
    callingInfo.callingTokenId = GetCallingTokenID();
    callingInfo.callingPid = GetCallingPid();
    callingInfo.callingUid = GetCallingUid();
}
} // namespace DataShare
} // namespace OHOS