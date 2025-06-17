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

#include "datashare_predicates.h"
#include "datashare_itypes_utils.h"

namespace OHOS {
namespace DataShare {

bool DataSharePredicates::Unmarshal(DataSharePredicates &predicates, MessageParcel &parcel)
{
    return ITypesUtil::UnmarshalPredicates(predicates, parcel);
}

bool DataSharePredicates::Marshal(const DataSharePredicates &predicates, MessageParcel &parcel)
{
    return ITypesUtil::MarshalPredicates(predicates, parcel);
}
}  // namespace DataShare
}  // namespace OHOS