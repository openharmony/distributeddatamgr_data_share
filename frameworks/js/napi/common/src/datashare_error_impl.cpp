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

#include "datashare_error_impl.h"

namespace OHOS {
namespace DataShare {
std::string ParametersTypeError::GetMessage() const
{
    return "Parameter error. The type of '" + name + "' must be '" + wantType + "'.";
}

int ParametersTypeError::GetCode() const
{
    return EXCEPTION_PARAMETER_CHECK;
}

std::string ParametersNumError::GetMessage() const
{
    return "Parameter error. Need " + wantNum + " parameters!";
}

int ParametersNumError::GetCode() const
{
    return EXCEPTION_PARAMETER_CHECK;
}

std::string DataShareHelperInitError::GetMessage() const
{
    return "The DataShareHelper is not initialized successfully.";
}

int DataShareHelperInitError::GetCode() const
{
    return EXCEPTION_HELPER_UNINITIALIZED;
}

std::string InnerError::GetMessage() const
{
    return "Inner error.";
}

int InnerError::GetCode() const
{
    return EXCEPTION_INNER;
}

int BusinessError::GetCode() const
{
    return code_;
}

std::string BusinessError::GetMessage() const
{
    return message_;
}

std::string UriNotExistError::GetMessage() const
{
    return "The uri is not exist.";
}

int UriNotExistError::GetCode() const
{
    return EXCEPTION_URI_NOT_EXIST;
}

std::string DataAreaNotExistError::GetMessage() const
{
    return "The data area is not exist.";
}

int DataAreaNotExistError::GetCode() const
{
    return EXCEPTION_DATA_AREA_NOT_EXIST;
}

std::string HelperAlreadyClosedError::GetMessage() const
{
    return "The DataShareHelper instance is already closed.";
}

int HelperAlreadyClosedError::GetCode() const
{
    return EXCEPTION_HELPER_CLOSED;
}
} // namespace DataShare
} // namespace OHOS