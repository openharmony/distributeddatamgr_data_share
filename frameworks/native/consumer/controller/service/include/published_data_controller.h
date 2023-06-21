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

#ifndef PUBLISHED_DATA_CONTROLLER_H
#define PUBLISHED_DATA_CONTROLLER_H

#include "datashare_template.h"
#include "data_share_manager_impl.h"

namespace OHOS {
namespace DataShare {
class PublishedDataController {
public:
    PublishedDataController() = default;

    virtual ~PublishedDataController() = default;

    std::vector<OperationResult> Publish(const Data &data, const std::string &bundleName);

    Data GetPublishedData(const std::string &bundleName, int &resultCode);

    std::vector<OperationResult> SubscribePublishedData(void *subscriber, const std::vector<std::string> &uris,
        int64_t subscriberId, const std::function<void(const PublishedDataChangeNode &changeNode)> &callback);

    std::vector<OperationResult> UnSubscribePublishedData(void *subscriber, const std::vector<std::string> &uris,
        int64_t subscriberId);

    std::vector<OperationResult> EnableSubscribePublishedData(void *subscriber, const std::vector<std::string> &uris,
        int64_t subscriberId);

    std::vector<OperationResult> DisableSubscribePublishedData(void *subscriber, const std::vector<std::string> &uris,
        int64_t subscriberId);
};

} // namespace DataShare
} // namespace OHOS

#endif // PUBLISHED_DATA_CONTROLLER_H
