/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
export class DataSharePredicates {

  static { loadLibrary("datashare_ani") }

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

  native equalTo(field: string, value: ValueType): DataSharePredicates;

  native notEqualTo(field: string, value: ValueType): DataSharePredicates;

  native beginWrap(): DataSharePredicates;

  native endWrap(): DataSharePredicates;

  native or(): DataSharePredicates;

  native and(): DataSharePredicates;

  native contains(field: string, value: string): DataSharePredicates;

  native isNull(field: string): DataSharePredicates;

  native isNotNull(field: string): DataSharePredicates;

  native like(field: string, value: string): DataSharePredicates;

  native between(field: string, low: ValueType, high: ValueType): DataSharePredicates;

  native greaterThan(field: string, value: ValueType): DataSharePredicates;

  native greaterThanOrEqualTo(field: string, value: ValueType): DataSharePredicates;

  native lessThanOrEqualTo(field: string, value: ValueType): DataSharePredicates;

  native lessThan(field: string, value: ValueType): DataSharePredicates;

  native orderByAsc(field: string): DataSharePredicates;

  native orderByDesc(field: string): DataSharePredicates;

  native limit(total: double, offset: double): DataSharePredicates;

  native groupBy(fields: Array<string>): DataSharePredicates;

  native in(field: string, value: Array<ValueType>): DataSharePredicates;

  native notIn(field: string, value: Array<ValueType>): DataSharePredicates;
}
}
