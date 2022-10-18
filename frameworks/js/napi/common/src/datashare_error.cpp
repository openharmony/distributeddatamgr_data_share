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

#include "datashare_error.h"

namespace OHOS {
namespace DataShare {
std::string ParametersTypeError::GetMessage()
{
    return "Parameter error. The type of '" + name + "' must be '" + wantType + "'.";
}

int ParametersTypeError::GetCode()
{
    return EXCEPTION_PARAMETER_CHECK;
}

std::string ParametersNumError::GetMessage()
{
    return "Parameter error. Need " + wantNum + " parameters!";
}

int ParametersNumError::GetCode()
{
    return EXCEPTION_PARAMETER_CHECK;
}

std::string DataShareHelperInitError::GetMessage()
{
    return "The DataShareHelper is not initialized successfully.";
}

int DataShareHelperInitError::GetCode()
{
    return EXCEPTION_HELPER_UNINITIALIZED;
}

std::string InnerError::GetMessage()
{
    return "";
}

int InnerError::GetCode()
{
    return EXCEPTION_INNER;
}
} // namespace DataShare
} // namespace OHOS