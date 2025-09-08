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
#define LOG_TAG "ani_datashare_observer"

#include "ani_datashare_observer.h"

#include "adaptor.h"
#include <memory>
#include "datashare_log.h"

namespace OHOS {
namespace DataShare {
ANIDataShareObserver::~ANIDataShareObserver() {}

void ANIDataShareObserver::OnChange()
{
    DISTRIBUTED_DATA_HITRACE(std::string(LOG_TAG) + "::" + std::string(__FUNCTION__));
    observer_->OnChange();
}

void ANIDataShareObserver::OnChange(const ChangeInfo &changeInfo)
{
    LOG_DEBUG("ANIDataShareObserver ChangeInfo Start");
    observer_->OnChange(changeInfo, true);
}
}  // namespace DataShare
}  // namespace OHOS
