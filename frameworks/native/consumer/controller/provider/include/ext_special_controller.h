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

#ifndef EXT_SPECIAL_CONTROLLER_H
#define EXT_SPECIAL_CONTROLLER_H

#include "datashare_connection.h"
#include "datashare_operation_statement.h"
#include "datashare_values_bucket.h"
#include "uri.h"

namespace OHOS {
namespace DataShare {
class ExtSpecialController {
public:
    ExtSpecialController(std::shared_ptr<DataShareConnection> connection, const Uri &uri,
        const sptr<IRemoteObject> &token);
    virtual ~ExtSpecialController() = default;

    int OpenFile(const Uri &uri, const std::string &mode);

    int OpenRawFile(const Uri &uri, const std::string &mode);

    int BatchUpdate(const UpdateOperations &operations, std::vector<BatchUpdateResult> &results);

    std::string GetType(const Uri &uri);

    int BatchInsert(const Uri &uri, const std::vector<DataShareValuesBucket> &values);
	
    int ExecuteBatch(const std::vector<OperationStatement> &statements, ExecResultSet &result);

    int InsertExt(Uri &uri, const DataShareValuesBucket &value, std::string &result);

    Uri NormalizeUri(const Uri &uri);

    Uri DenormalizeUri(const Uri &uri);

    std::vector<std::string> GetFileTypes(const Uri &uri, const std::string &mimeTypeFilter);

private:
    std::shared_ptr<DataShareConnection> connection_ = nullptr;
    sptr<IRemoteObject> token_ = {};
    Uri uri_ = Uri("");
};
}
}

#endif // EXT_SPECIAL_CONTROLLER_H
