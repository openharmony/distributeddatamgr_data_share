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

#ifndef NAPI_DATASHARE_OBSERVER_H
#define NAPI_DATASHARE_OBSERVER_H

#include "data_ability_observer_stub.h"
#include "napi_datashare_inner_observer.h"

namespace OHOS {
namespace DataShare {
class NAPIDataShareObserver : public AAFwk::DataAbilityObserverStub,
                              public DataShareObserver {
public:
    explicit NAPIDataShareObserver(const std::shared_ptr<NAPIInnerObserver> observer) : observer_(observer){};
    virtual ~NAPIDataShareObserver();
    void OnChange() override;
    void OnChange(const ChangeInfo &changeInfo) override;
    std::shared_ptr<NAPIInnerObserver> observer_ = nullptr;
};
}  // namespace DataShare
}  // namespace OHOS
#endif /* DATASHARE_COMMON_H */
