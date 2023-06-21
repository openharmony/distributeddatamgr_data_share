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

#ifndef PERSISTENT_DATA_CONTROLLER_H
#define PERSISTENT_DATA_CONTROLLER_H

#include "data_share_manager_impl.h"
#include "datashare_template.h"

namespace OHOS {
namespace DataShare {
class PersistentDataController {
public:
    PersistentDataController() = default;

    virtual ~PersistentDataController() = default;

    int AddQueryTemplate(const std::string &uri, int64_t subscriberId, Template &tpl);

    int DelQueryTemplate(const std::string &uri, int64_t subscriberId);

    std::vector<OperationResult> SubscribeRdbData(void *subscriber, const std::vector<std::string> &uris,
        const TemplateId &templateId, std::function<void(const RdbChangeNode &)> callback);

    std::vector<OperationResult> UnSubscribeRdbData(void *subscriber, const std::vector<std::string> &uris,
        const TemplateId &templateId);

    std::vector<OperationResult> EnableSubscribeRdbData(void *subscriber, const std::vector<std::string> &uris,
        const TemplateId &templateId);

    std::vector<OperationResult> DisableSubscribeRdbData(void *subscriber, const std::vector<std::string> &uris,
        const TemplateId &templateId);
};
} // namespace DataShare
} // namespace OHOS

#endif // PERSISTENT_DATA_CONTROLLER_H
