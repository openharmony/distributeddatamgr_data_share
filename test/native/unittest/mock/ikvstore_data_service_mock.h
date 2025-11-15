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

#ifndef IKVSTORE_DATA_SERVICE_MOCK_H
#define IKVSTORE_DATA_SERVICE_MOCK_H

#include <gtest/gtest.h>
#include "gmock/gmock.h"
#include "ikvstore_data_service.h"

namespace OHOS {
namespace DataShare {
class MockDataShareKvServiceProxy : public DataShareKvServiceProxy {
public:
    explicit MockDataShareKvServiceProxy(const sptr<IRemoteObject> &impl)
        : DataShareKvServiceProxy(impl) {}
    ~MockDataShareKvServiceProxy() = default;
    MOCK_METHOD(sptr<IRemoteObject>, GetFeatureInterface, (const std::string &name), (override));
    MOCK_METHOD(int32_t, RegisterClientDeathObserver, (const std::string &appId, sptr<IRemoteObject> observer),
        (override));
};
}
}
#endif // IKVSTORE_DATA_SERVICE_MOCK_H