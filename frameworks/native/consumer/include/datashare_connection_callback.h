/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef DATASHARE_CONNECTION_CALLBACK_H
#define DATASHARE_CONNECTION_CALLBACK_H

#include <memory>

#include "ability_connect_callback_stub.h"

namespace OHOS {
namespace DataShare {
class DataShareConnection;

class DataShareConnectionCallback : public AAFwk::AbilityConnectionStub {
public:
    DataShareConnectionCallback();
    void SetTarget(std::weak_ptr<DataShareConnection> target);
    bool TargetExpired() const;

    void OnAbilityConnectDone(const AppExecFwk::ElementName &element, const sptr<IRemoteObject> &remoteObject,
        int resultCode) override;
    void OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int resultCode) override;

private:
    std::weak_ptr<DataShareConnection> target_;
};
} // namespace DataShare
} // namespace OHOS
#endif // DATASHARE_CONNECTION_CALLBACK_H