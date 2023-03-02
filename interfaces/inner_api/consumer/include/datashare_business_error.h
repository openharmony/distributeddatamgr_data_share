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

#ifndef DATASHARE_BUSINESS_ERROR_H
#define DATASHARE_BUSINESS_ERROR_H

namespace OHOS {
namespace DataShare {
class DatashareBusinessError {
public:
    DatashareBusinessError() = default;
    ~DatashareBusinessError() = default;

    int GetCode()
    {
        return code_;
    }

    void SetCode(const std::string &code)
    {
        if (!code.empty()) {
            code_ = atoi(code.c_str());
        }
    }

    void SetCode(int code)
    {
        code_ = code;
    }

    std::string GetMessage()
    {
        return message_;
    }
    
    void SetMessage(const std::string &message)
    {
        message_ = message;
    }

    static constexpr int DB_NOT_EXIST_ERR = 14800045;

private:
    int code_ = 0;
    std::string message_ = "";
};
}  // namespace DataShare
}  // namespace OHOS
#endif  // DATASHARE_BUSINESS_ERROR_H