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

#ifndef DATASHARE_ERROR_IMPL_H
#define DATASHARE_ERROR_IMPL_H

#include "datashare_error.h"

namespace OHOS {
namespace DataShare {
class ParametersTypeError : public Error {
public:
    ParametersTypeError(const std::string &name, const std::string &wantType) : name(name), wantType(wantType) {};
    std::string GetMessage() override;
    int GetCode() override;
private:
    std::string name;
    std::string wantType;
};

class ParametersNumError : public Error {
public:
    ParametersNumError(const std::string &wantNum) : wantNum(wantNum) {};
    std::string GetMessage() override;
    int GetCode() override;
private:
    std::string wantNum;
};

class DataShareHelperInitError : public Error {
public:
    DataShareHelperInitError() = default;
    std::string GetMessage() override;
    int GetCode() override;
};

class InnerError : public Error {
public:
    InnerError() = default;
    std::string GetMessage() override;
    int GetCode() override;
};
} // namespace DataShare
} // namespace OHOS
#endif // DATASHARE_ERROR_IMPL_H
