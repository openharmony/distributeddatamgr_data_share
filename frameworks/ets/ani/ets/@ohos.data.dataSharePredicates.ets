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
loadLibrary("datashare_ani_rs")
class Cleaner {
  private nativePtr: long = 0
  constructor(ptr:long) {
    this.nativePtr = ptr
  }
  native native_clean(): void
}

let destroyRegister = new FinalizationRegistry<Cleaner>((cleaner: Cleaner) => {cleaner.native_clean()})
let unregisterToken = new object()

export native function native_create(): long
export class DataSharePredicates {

  static { loadLibrary("datashare_ani_rs") }

  private nativePtr:long = 0;
  private cleaner: Cleaner | null = null;

  constructor() {
      if (this.nativePtr == 0) {
          this.nativePtr = native_create();
      }
      this.registerCleaner(this.nativePtr)
  }

  constructor(nativePtr: long) {
    if(this.nativePtr == 0){
        this.nativePtr = nativePtr;
    }
    this.registerCleaner(this.nativePtr)
  }

  getNativePtr():long {
      return this.nativePtr;
  }

  registerCleaner(ptr: long): void {
    this.cleaner = new Cleaner(ptr)
    destroyRegister.register(this, this.cleaner!, unregisterToken);
  }

  unregisterCleaner(): void {
    destroyRegister.unregister(unregisterToken);
  }

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

  native limit(total: int, offset: int): DataSharePredicates;

  native groupBy(fields: Array<string>): DataSharePredicates;

  native inValues(field: string, value: Array<ValueType>): DataSharePredicates;

  native notInValues(field: string, value: Array<ValueType>): DataSharePredicates;
}
}
