/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License"),
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

/**
 * @file
 * @kit ArkData
 */

import { BusinessError } from '@ohos.base'
import type { AsyncCallback, Callback } from '@ohos.base';
import ExtensionContext from 'application.ExtensionContext';
import Want from '@ohos.app.ability.Want';
import dataSharePredicates from './@ohos.data.dataSharePredicates';
import { ValuesBucket } from '@ohos.data.ValuesBucket';

export interface UpdateOperation {
    values: ValuesBucket;
    predicates: dataSharePredicates.DataSharePredicates;
}

export namespace dataShareExtensionAbilityHelper {
    loadLibrary("datashare_ani_rs")

    export native function nativeExtensionCallbackInt(errorCode: double, errMsg: string, data: int, nativePtr: long): void;

    export native function nativeExtensionCallbackObject(errorCode: double, errMsg: string, data: Object, nativePtr: long): void;

    export native function nativeExtensionCallbackVoid(errorCode: double, errMsg: string, nativePtr: long): void;

    export native function nativeExtensionCallbackString(errorCode: double, errMsg: string, data: string, nativePtr: long): void;

    export native function nativeExtensionCallbackBatchUpdate(errorCode: double, errMsg: string, data: Record<string, Array<int>>, nativePtr: long): void;

    export class CallbackWrap {
        private nativeObject: DataShareExtensionAbility
        private nativePtr: long

        constructor(nativeObject: DataShareExtensionAbility, nativePtr: long) {
            this.nativeObject = nativeObject;
            this.nativePtr = nativePtr;
        }

        doExtensionCallbackInt(err: BusinessError|null, data: int|undefined) {
            if ((err instanceof BusinessError) && (data instanceof int)) {
                let error: BusinessError = err as BusinessError
                let dataVal: int = data as int
                nativeExtensionCallbackInt(error.code, error.message, dataVal, this.nativePtr)
            } else {
                console.error(`invalid type param`)
            }
        }

        doExtensionCallbackObject(err: BusinessError|null, data: Object|undefined) {
            if ((err instanceof BusinessError) && (data instanceof Object)) {
                let error: BusinessError = err as BusinessError
                let dataVal: Object = data as Object
                nativeExtensionCallbackObject(error.code, error.message, dataVal, this.nativePtr)
            } else {
                console.error(`invalid type param`)
            }
        }

        doExtensionCallbackVoid(err: BusinessError|null) {
            if (err instanceof BusinessError) {
                let error: BusinessError = err as BusinessError
                nativeExtensionCallbackVoid(error.code, error.message, this.nativePtr)
            } else {
                console.error(`invalid type param`)
            }
        }

        doExtensionCallbackString(err: BusinessError | null, data: string | undefined) {
            if ((err instanceof BusinessError) && (data instanceof string)) {
                let error: BusinessError = err as BusinessError
                let dataVal: string = data as string
                nativeExtensionCallbackString(error.code, error.message, dataVal, this.nativePtr)
            } else {
                console.error(`invalid type param`)
            }
        }

        doExtensionCallbackBatchUpdate(err: BusinessError | null, data: Record<string, Array<int>> | undefined) {
            if ((err instanceof BusinessError) && (data instanceof Record)) {
                let error: BusinessError = err as BusinessError
                let dataVal: Record<string, Array<int>> = data as Record<string, Array<int>>
                nativeExtensionCallbackBatchUpdate(error.code, error.message, dataVal, this.nativePtr)
            } else {
                console.error(`invalid type param`)
            }
        }

        doInsert(uri: string, valueBucket: ValuesBucket): void {
            if (this.nativeObject.insert != undefined) {
                this.nativeObject.insert!(uri, valueBucket, this.doExtensionCallbackInt);
            } else {
                console.error(`insert is undefined.`);
            }
        }

        doUpdate(
            uri: string,
            predicates: dataSharePredicates.DataSharePredicates,
            valueBucket: ValuesBucket,
        ): void {
            if (this.nativeObject.update != undefined) {
                this.nativeObject.update!(uri, predicates, valueBucket, this.doExtensionCallbackInt);
            } else {
                console.error(`update is undefined.`);
            }
        }

        doDelete(uri: string, predicates: dataSharePredicates.DataSharePredicates): void {
            if (this.nativeObject.delete != undefined) {
                this.nativeObject.delete!(uri, predicates, this.doExtensionCallbackInt);
            } else {
                console.error(`delete is undefined.`);
            }
        }

        doQuery(
            uri: string,
            predicates: dataSharePredicates.DataSharePredicates,
            columns: Array<string>,
        ): void {
            if (this.nativeObject.query != undefined) {
                this.nativeObject.query!(uri, predicates, columns, this.doExtensionCallbackObject);
            } else {
                console.error(`query is undefined.`);
            }
        }

        doBatchInsert(uri: string, valueBuckets: Array<ValuesBucket>): void {
            if (this.nativeObject.batchInsert != undefined) {
                this.nativeObject.batchInsert!(uri, valueBuckets, this.doExtensionCallbackInt);
            } else {
                console.error(`batchInsert is undefined.`);
            }
        }

        doOnCreate(want: Want): void {
            if (this.nativeObject.onCreate != undefined) {
                this.nativeObject.onCreate!(want, this.doExtensionCallbackVoid);
            } else {
                console.error(`onCreate is undefined.`);
            }
        }

        doBatchUpdate(operations: Record<string, Array<UpdateOperation>>): void {
            if (this.nativeObject.batchUpdate != undefined) {
                this.nativeObject.batchUpdate!(operations, this.doExtensionCallbackBatchUpdate);
            } else {
                console.error(`batchUpdate is undefined.`);
            }
        }

        doNormalizeUri(uri: string): void {
            if (this.nativeObject.normalizeUri != undefined) {
                this.nativeObject.normalizeUri!(uri, this.doExtensionCallbackString);
            } else {
                console.error(`normalizeUri is undefined.`);
            }    
        }
        
        doDenormalizeUri(uri: string): void {
            if (this.nativeObject.denormalizeUri != undefined) {
                this.nativeObject.denormalizeUri!(uri, this.doExtensionCallbackString);
            } else {
                console.error(`denormalizeUri is undefined.`);
            }    
        }
    }
}

type OnCreateFn = (want: Want, callback: AsyncCallback<void>) => void;

type InsertFn = (uri: string, valueBucket: ValuesBucket, callback: AsyncCallback<int>) => void;

type UpdateFn = (
  uri: string,
  predicates: dataSharePredicates.DataSharePredicates,
  valueBucket: ValuesBucket,
  callback: AsyncCallback<int>
) => void;

type DeleteFn = (
  uri: string,
  predicates: dataSharePredicates.DataSharePredicates,
  callback: AsyncCallback<int>
) => void;

type QueryFn = (
  uri: string,
  predicates: dataSharePredicates.DataSharePredicates,
  columns: Array<string>,
  callback: AsyncCallback<Object>
) => void;

type BatchInsertFn = (uri: string, valueBuckets: Array<ValuesBucket>, callback: AsyncCallback<int>) => void;

type BatchUpdateFn = (
    operations: Record<string,
    Array<UpdateOperation>>,
    callback: AsyncCallback<Record<string, Array<int>>>
) => void;

type NormalizeUriFn = (uri: string, callback: AsyncCallback<string>) => void;

type DenormalizeUriFn = (uri: string, callback: AsyncCallback<string>) => void;

export default class DataShareExtensionAbility {

    context: ExtensionContext;

    constructor(context: ExtensionContext) {
        this.context = context;
    }

    onCreate?: OnCreateFn;

    insert?: InsertFn;

    update?: UpdateFn;

    delete?: DeleteFn;

    query?: QueryFn;

    batchInsert?: BatchInsertFn;

    batchUpdate?: BatchUpdateFn;

    normalizeUri?: NormalizeUriFn;

    denormalizeUri?: DenormalizeUriFn;
}
