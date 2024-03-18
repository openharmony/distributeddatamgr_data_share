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

#ifndef DATASHARE_ERROR_H
#define DATASHARE_ERROR_H

#include "datashare_js_utils.h"

namespace OHOS {
namespace DataShare {
class Error {
public:
    static const int E_OK = 0;
    static const int EXCEPTION_PARAMETER_CHECK = 401;
    static const int EXCEPTION_INNER = 15700000;
    static const int EXCEPTION_HELPER_UNINITIALIZED = 15700010;
    static const int EXCEPTION_URI_NOT_EXIST = 15700011;
    static const int EXCEPTION_DATA_AREA_NOT_EXIST = 15700012;
    static const int EXCEPTION_HELPER_CLOSED = 15700013;
    virtual ~Error() {};
    virtual std::string GetMessage() const = 0;
    virtual int GetCode() const = 0;
};
} // namespace DataShare
} // namespace OHOS

#endif // DATASHARE_ERROR_H