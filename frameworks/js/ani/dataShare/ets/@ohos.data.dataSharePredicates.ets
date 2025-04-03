
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

import { ValueType } from "@ohos.data.ValuesBucket"

export default namespace dataSharePredicates{
export class DataSharePredicates{
    static { loadLibrary("datasharepredicates_ani") }
    private nativePtr:long = 0;

    constructor() {
        console.log("enter constructor")
        if (this.nativePtr == 0) {
            console.log("start create")
            this.nativePtr = DataSharePredicates.create();
            console.log("stop create")
        }
    }

    getNativePtr():long {
        return this.nativePtr;
    }

    native static create(): long;
    native equalTo(field: string, value: ValueType):DataSharePredicates;
    native notEqualTo(field: string, value: ValueType):DataSharePredicates;
    native orderByDesc(field: string):DataSharePredicates;
    native orderByAsc(field: string):DataSharePredicates;
    native and():DataSharePredicates;
    native limit(total: number, offset: number):DataSharePredicates;
    native lessThan(field: string, value: ValueType): DataSharePredicates;
    native like(field: string, value: string): DataSharePredicates;
    native endWrap(): DataSharePredicates;
    native greaterThanOrEqualTo(field: string, value: ValueType): DataSharePredicates;
    native contains(field: string, value: string): DataSharePredicates;
    native or(): DataSharePredicates;
    native beginWrap(): DataSharePredicates;
    native greaterThan(field: string, value: ValueType): DataSharePredicates;
    native groupBy(fields:Array<string>): DataSharePredicates;
    native in(field: string, value: Array<ValueType>): DataSharePredicates;
    native notIn(field: string, value: Array<ValueType>): DataSharePredicates;
}
}
