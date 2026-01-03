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

#ifndef DATASHARE_PREDICATES_VERIFY_H
#define DATASHARE_PREDICATES_VERIFY_H

#include <utility>
#include <set>
#include <string>

#include "datashare_predicates.h"

namespace OHOS {
namespace DataShare {

class DataSharePredicatesVerify {
public:
    DataSharePredicatesVerify() = default;
    ~DataSharePredicatesVerify() = default;
    std::pair<int, int> VerifyPredicates(const DataSharePredicates &predicates);

private:
    enum class PredicatesVerifyType {
        VERIFY_DEFAULT = 0x00,
        SINGLE_2_PARAMS_PUBLIC,
        SINGLE_3_PARAMS_PUBLIC,
        SINGLE_2_PARAMS_SYS,
        SINGLE_3_PARAMS_SYS,
        MULTI_2_PARAMS_SYS
    };

    // verify singleParams
    bool VerifyField(const std::string &field);
    // verify multiParams
    bool VerifyFields(const std::vector<std::string> &fields);
    DataSharePredicatesVerify::PredicatesVerifyType GetPredicatesVerifyType(const int32_t type);
    int VerifyPredicatesByType(const PredicatesVerifyType &verifyType, const OperationItem &item);
    bool CheckParamNum(const PredicatesVerifyType &verifyType, const OperationItem &item);
};
} // namespace DataSharePredicatesVerify
} // namespace OHOS
#endif // DATASHARE_PREDICATES_VERIFY_H
