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

#ifndef DATASHARE_COMMON_ITYPES_UTIL_H
#define DATASHARE_COMMON_ITYPES_UTIL_H

#include "parcel.h"
#include "datashare_predicates.h"
#include "datashare_values_bucket.h"


namespace OHOS::DataShare {
class ITypesUtil final {
public:
    static bool Marshalling(const DataSharePredicates &predicates, Parcel &parcel);
    static bool Unmarshalling(Parcel &parcel, DataSharePredicates &predicates);

    static bool Marshalling(const DataShareValuesBucket &valuesBucket, Parcel &parcel);
    static bool Unmarshalling(Parcel &parcel, DataShareValuesBucket &valuesBucket);

    static bool Marshalling(const OperationItem &operationItem, Parcel &parcel);
    static bool Unmarshalling(Parcel &parcel, OperationItem &operationItem);

    static bool Marshalling(const DataSharePredicatesObject &predicatesObject, Parcel &parcel);
    static bool Unmarshalling(Parcel &parcel, DataSharePredicatesObject &predicatesObject);

    static bool Marshalling(const DataSharePredicatesObjects &predicatesObject, Parcel &parcel);
    static bool Unmarshalling(Parcel &parcel, DataSharePredicatesObjects &predicatesObject);

    static bool Marshalling(const DataShareValueObject &valueObject, Parcel &parcel);
    static bool Unmarshalling(Parcel &parcel, DataShareValueObject &valueObject);

    template <typename T>
    static bool Marshalling(const std::vector<T> &params, Parcel &parcel);
    template <typename T>
    static bool Unmarshalling(Parcel &parcel, std::vector<T> &params);
};
} // namespace OHOS::DataShare
#endif
