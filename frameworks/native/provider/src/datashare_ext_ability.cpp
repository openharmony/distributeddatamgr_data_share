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

#include "datashare_ext_ability.h"

#include "ability_loader.h"
#include "connection_manager.h"
#include "datashare_log.h"
#include "js_datashare_ext_ability.h"
#include "runtime.h"
#include "datashare_ext_ability_context.h"

namespace OHOS {
namespace DataShare {
using namespace OHOS::AppExecFwk;

CreatorFunc DataShareExtAbility::creator_ = nullptr;
void DataShareExtAbility::SetCreator(const CreatorFunc& creator)
{
    creator_ = creator;
}

DataShareExtAbility* DataShareExtAbility::Create(const std::unique_ptr<Runtime>& runtime)
{
    if (!runtime) {
        LOG_INFO("runtime is null");
        return nullptr;
    }

    if (creator_) {
        return creator_(runtime);
    }

    LOG_DEBUG("DataShareExtAbility::Create runtime");
    switch (runtime->GetLanguage()) {
        case Runtime::Language::JS:
            return JsDataShareExtAbility::Create(runtime);

        default:
            return new DataShareExtAbility();
    }
}

void DataShareExtAbility::Init(const std::shared_ptr<AbilityLocalRecord> &record,
    const std::shared_ptr<OHOSApplication> &application,
    std::shared_ptr<AbilityHandler> &handler,
    const sptr<IRemoteObject> &token)
{
    ExtensionBase<DataShareExtAbilityContext>::Init(record, application, handler, token);
    LOG_INFO("DataShareExtAbility begin init context");
}

std::vector<std::string> DataShareExtAbility::GetFileTypes(const Uri &uri, const std::string &mimeTypeFilter)
{
    std::vector<std::string> ret;
    return ret;
}

int DataShareExtAbility::OpenFile(const Uri &uri, const std::string &mode)
{
    return -1;
}

int DataShareExtAbility::OpenRawFile(const Uri &uri, const std::string &mode)
{
    return -1;
}

int DataShareExtAbility::Insert(const Uri &uri, const DataShareValuesBucket &value)
{
    return -1;
}

int DataShareExtAbility::InsertExt(const Uri &uri, const DataShareValuesBucket &value, std::string &result)
{
    return -1;
}

int DataShareExtAbility::Update(const Uri &uri, const DataSharePredicates &predicates,
    const DataShareValuesBucket &value)
{
    return -1;
}

int DataShareExtAbility::BatchUpdate(const UpdateOperations &operations, std::vector<BatchUpdateResult> &results)
{
    return 0;
}

int DataShareExtAbility::Delete(const Uri &uri, const DataSharePredicates &predicates)
{
    return -1;
}

std::shared_ptr<DataShareResultSet> DataShareExtAbility::Query(const Uri &uri,
    const DataSharePredicates &predicates, std::vector<std::string> &columns, DatashareBusinessError &businessError)
{
    return nullptr;
}

std::string DataShareExtAbility::GetType(const Uri &uri)
{
    return "";
}

int DataShareExtAbility::BatchInsert(const Uri &uri, const std::vector<DataShareValuesBucket> &values)
{
    return -1;
}

int DataShareExtAbility::ExecuteBatch(const std::vector<OperationStatement> &statements, ExecResultSet &result)
{
    return -1;
}

bool DataShareExtAbility::RegisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver)
{
    return true;
}

bool DataShareExtAbility::UnregisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver)
{
    return true;
}

bool DataShareExtAbility::NotifyChange(const Uri &uri)
{
    return true;
}

Uri DataShareExtAbility::NormalizeUri(const Uri &uri)
{
    Uri urivalue("");
    return uri;
}

Uri DataShareExtAbility::DenormalizeUri(const Uri &uri)
{
    Uri urivalue("");
    return uri;
}
} // namespace DataShare
} // namespace OHOS