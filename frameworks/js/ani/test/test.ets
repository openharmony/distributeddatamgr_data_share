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

import { dataSharePredicates } from "../ets/@ohos.data.dataSharePredicates.d.ets"
import { ValueType } from "../ets/@ohos.data.ValuesBucket.d.ets"

function main(){
    console.println("hello DataSharePredicates")
    let datashare = dataSharePredicates.DataSharePredicates.create();
    let result = datashare.equalTo("123", "abc")
    result.equalTo("456", true)
    result.equalTo("789", 123)
    result = datashare.orderByDesc("758")
    result = datashare.orderByAsc("668")
    result = datashare.and()
    result.limit(10, 10)
    console.println("result from native")

    console.println("------------------------------------------")
    datashare.lessThan("boy", "jack").lessThan("boy2", 120).lessThan("boy3", false);

    console.println("------------------------------------------")
    datashare.like("girl", "rose");

    console.println("------------------------------------------")
    datashare.endWrap();

    console.println("------------------------------------------")
    datashare.greaterThanOrEqualTo("cat", "higer").greaterThanOrEqualTo("cat2", 12.9)
        .greaterThanOrEqualTo("cat3", true);

    console.println("------------------------------------------")
    datashare.contains("name", "rose");

    console.println("------------------------------------------")
    datashare.or();

    console.println("------------------------------------------")
    datashare.beginWrap();

    console.println("------------------------------------------")
    datashare.greaterThan("age", "100").greaterThanOrEqualTo("age2", 150.01)
        .greaterThanOrEqualTo("age3", true);
    let fruits: Array<string> = new Array<string>("apple", "banana", "cherry");
    datashare.groupBy(fruits);
    console.println("--------------in or notin----------------------------")
    let numbersArr = new Array<ValueType>(255, 11.6, 13.0);
    let boolArr = new Array<ValueType>(true, false, true, false, true);
    let stringArr = new Array<ValueType>("cat", "dog", "pig");


    datashare.in("fz", numbersArr).in("fz", boolArr).in("fz", stringArr).notIn("fz", numbersArr).notIn("fz", boolArr).notIn("fz", stringArr);
}