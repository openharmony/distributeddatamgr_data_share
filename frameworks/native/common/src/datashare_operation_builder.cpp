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

#include "datashare_operation_builder.h"
#include "datashare_log.h"
#include "datashare_operation.h"
#include "datashare_value_object.h"
#include "datashare_values_bucket.h"
namespace OHOS::DataShare { class DataSharePredicates; }

namespace OHOS {
namespace DataShare {
DataShareOperationBuilder::DataShareOperationBuilder(const int type, const std::shared_ptr<Uri> &uri)
{
    type_ = type;
    uri_ = uri;
    expectedCount_ = 0;
    interrupted_ = false;
    valuesBucket_ = nullptr;
    dataSharePredicates_ = nullptr;
    valuesBucketReferences_ = nullptr;
    dataSharePredicatesBackReferences_.clear();
}
DataShareOperationBuilder::~DataShareOperationBuilder()
{
    dataSharePredicatesBackReferences_.clear();
}

std::shared_ptr<DataShareOperation> DataShareOperationBuilder::Build()
{
    LOG_DEBUG("Start");
    if (type_ != DataShareOperation::TYPE_UPDATE || (valuesBucket_ != nullptr && !valuesBucket_->IsEmpty())) {
        std::shared_ptr<DataShareOperation> operation = std::make_shared<DataShareOperation>(shared_from_this());
        LOG_DEBUG("return operation");
        return operation;
    }
    LOG_ERROR("return nullptr");
    return nullptr;
}
std::shared_ptr<DataShareOperationBuilder> DataShareOperationBuilder::WithValuesBucket(
    std::shared_ptr<DataShareValuesBucket> &values)
{
    LOG_DEBUG("Start");
    if (type_ != DataShareOperation::TYPE_INSERT && type_ != DataShareOperation::TYPE_UPDATE &&
        type_ != DataShareOperation::TYPE_ASSERT) {
        LOG_ERROR("Only inserts, updates can have values, type=%{public}d", type_);
        return nullptr;
    }

    valuesBucket_ = std::make_shared<DataShareValuesBucket>(values->valuesMap);
    LOG_DEBUG("End successfully");
    return shared_from_this();
}

std::shared_ptr<DataShareOperationBuilder> DataShareOperationBuilder::WithPredicates(
    std::shared_ptr<DataSharePredicates> &predicates)
{
    LOG_DEBUG("Start");
    if (type_ != DataShareOperation::TYPE_DELETE && type_ != DataShareOperation::TYPE_UPDATE &&
        type_ != DataShareOperation::TYPE_ASSERT) {
        LOG_ERROR("Only deletes and updates can have selections, type=%{public}d", type_);
        return nullptr;
    }
    dataSharePredicates_ = predicates;
    LOG_DEBUG("End successfully");
    return shared_from_this();
}
std::shared_ptr<DataShareOperationBuilder> DataShareOperationBuilder::WithExpectedCount(int count)
{
    LOG_DEBUG("Start");
    LOG_INFO("expectedCount:%{public}d", count);
    if (type_ != DataShareOperation::TYPE_UPDATE && type_ != DataShareOperation::TYPE_DELETE &&
        type_ != DataShareOperation::TYPE_ASSERT) {
        LOG_ERROR("Only updates, deletes can have expected counts, type=%{public}d", type_);
        return nullptr;
    }
    expectedCount_ = count;
    LOG_DEBUG("End successfully");
    return shared_from_this();
}
std::shared_ptr<DataShareOperationBuilder> DataShareOperationBuilder::WithPredicatesBackReference(
    int requestArgIndex, int previousResult)
{
    LOG_DEBUG("Start");
    LOG_INFO("requestArgIndex=%{public}d, previousResult=%{public}d", requestArgIndex, previousResult);
    if (type_ != DataShareOperation::TYPE_UPDATE && type_ != DataShareOperation::TYPE_DELETE &&
        type_ != DataShareOperation::TYPE_ASSERT) {
        LOG_ERROR("Only updates, deletes, and asserts can have select back-references, type=%{public}d", type_);
        return nullptr;
    }
    dataSharePredicatesBackReferences_.insert(std::make_pair(requestArgIndex, previousResult));
    LOG_DEBUG("End successfully");
    return shared_from_this();
}
std::shared_ptr<DataShareOperationBuilder> DataShareOperationBuilder::WithValueBackReferences(
    std::shared_ptr<DataShareValuesBucket> &backReferences)
{
    LOG_DEBUG("Start");
    if (type_ != DataShareOperation::TYPE_INSERT && type_ != DataShareOperation::TYPE_UPDATE &&
        type_ != DataShareOperation::TYPE_ASSERT) {
        LOG_ERROR("Only inserts, updates, and asserts can have value back-references, type=%{public}d", type_);
        return nullptr;
    }
    valuesBucketReferences_ = backReferences;
    LOG_DEBUG("End successfully");
    return shared_from_this();
}
std::shared_ptr<DataShareOperationBuilder> DataShareOperationBuilder::WithInterruptionAllowed(bool interrupted)
{
    LOG_DEBUG("Start");
    LOG_INFO("interrupted=%{public}d", interrupted);
    if (type_ != DataShareOperation::TYPE_INSERT && type_ != DataShareOperation::TYPE_UPDATE &&
        type_ != DataShareOperation::TYPE_ASSERT && type_ != DataShareOperation::TYPE_DELETE) {
        LOG_ERROR("Only inserts, updates, delete, and asserts can have value back-references, type=%{public}d", type_);
        return nullptr;
    }
    interrupted_ = interrupted;
    LOG_DEBUG("End successfully");
    return shared_from_this();
}
}  // namespace DataShare
}  // namespace OHOS
