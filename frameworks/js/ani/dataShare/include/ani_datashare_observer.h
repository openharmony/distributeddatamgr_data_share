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

#ifndef ANI_DATASHARE_OBSERVER_H
#define ANI_DATASHARE_OBSERVER_H

#include "data_ability_observer_stub.h"
#include "ani_datashare_inner_observer.h"

namespace OHOS {
namespace DataShare {
class ANIDataShareObserver : public AAFwk::DataAbilityObserverStub,
                            public DataShareObserver {
public:
    explicit ANIDataShareObserver(const std::shared_ptr<ANIInnerObserver> observer) : observer_(observer){};
    virtual ~ANIDataShareObserver();
    void OnChange() override;
    void OnChange(const ChangeInfo &changeInfo) override;
    std::shared_ptr<ANIInnerObserver> observer_ = nullptr;
};
}  // namespace DataShare
}  // namespace OHOS
#endif /* ANI_DATASHARE_OBSERVER_H */
 