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

#ifndef DATASHARE_I_SHARED_RESULT_SET_STUB_H
#define DATASHARE_I_SHARED_RESULT_SET_STUB_H
#include <functional>
#include <memory>

#include "safe_block_queue.h"
#include "ishared_result_set.h"
#include "iremote_stub.h"

namespace OHOS::DataShare {
class ISharedResultSetStub : public IRemoteStub<ISharedResultSet> {
public:
    explicit ISharedResultSetStub(std::shared_ptr<DataShareResultSet> resultSet);
    ~ISharedResultSetStub();
    static sptr<ISharedResultSet> CreateStub(std::shared_ptr<DataShareResultSet> resultSet,
                                            MessageParcel &parcel);
    int OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

protected:
    int HandleGetRowCountRequest(MessageParcel &data, MessageParcel &reply);
    int HandleGetAllColumnNamesRequest(MessageParcel &data, MessageParcel &reply);
    int HandleOnGoRequest(MessageParcel &data, MessageParcel &reply);
    int HandleCloseRequest(MessageParcel &data, MessageParcel &reply);

private:
    using Handler = int(ISharedResultSetStub::*)(MessageParcel &request, MessageParcel &reply);
    std::shared_ptr<DataShareResultSet> resultSet_;
    static constexpr Handler handlers[FUNC_BUTT] {
        &ISharedResultSetStub::HandleGetRowCountRequest,
        &ISharedResultSetStub::HandleGetAllColumnNamesRequest,
        &ISharedResultSetStub::HandleOnGoRequest,
        &ISharedResultSetStub::HandleCloseRequest,
    };
};
} // namespace OHOS::DataShare

#endif // DATASHARE_I_SHARED_RESULT_SET_STUB_H
