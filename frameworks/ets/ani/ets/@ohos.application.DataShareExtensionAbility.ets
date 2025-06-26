/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

export namespace dataShareExtensionAbilityHelper {
    loadLibrary("datashare_ani")

    export native function nativeExtensionCallbackInt(errorCode: double, errMsg: string, data: int, nativePtr: long): void;

    export native function nativeExtensionCallbackObject(errorCode: double, errMsg: string, data: Object, nativePtr: long): void;

    export native function nativeExtensionCallbackVoid(errorCode: double, errMsg: string, nativePtr: long): void;

    export class CallbackWrap {
        private nativeObject: DataShareExtensionAbility
        private nativePtr:long

        constructor(nativeObject: DataShareExtensionAbility, nativePtr: long) {
            this.nativeObject = nativeObject;
            this.nativePtr = nativePtr;
        }

        doExtensionCallbackInt(err: BusinessError, data: int) {
            nativeExtensionCallbackInt(err.code, err.message, data, this.nativePtr)
        }

        doExtensionCallbackObject(err: BusinessError, data: Object) {
            nativeExtensionCallbackObject(err.code, err.message, data, this.nativePtr)
        }

        doExtensionCallbackVoid(err: BusinessError) {
            nativeExtensionCallbackVoid(err.code, err.message, this.nativePtr)
        }

        doInsert(uri: string, valueBucket: ValuesBucket): void {
            this.nativeObject.insert(uri, valueBucket, this.doExtensionCallbackInt);
        }

        doUpdate(
            uri: string,
            predicates: dataSharePredicates.DataSharePredicates,
            valueBucket: ValuesBucket,
        ): void {
            this.nativeObject.update(uri, predicates, valueBucket, this.doExtensionCallbackInt);
        }

        doDelete(uri: string, predicates: dataSharePredicates.DataSharePredicates): void {
            this.nativeObject.delete(uri, predicates, this.doExtensionCallbackInt);
        }

        doQuery(
            uri: string,
            predicates: dataSharePredicates.DataSharePredicates,
            columns: Array<string>,
        ): void {
            this.nativeObject.query(uri, predicates, columns, this.doExtensionCallbackObject);
        }

        doBatchInsert(uri: string, valueBuckets: Array<ValuesBucket>): void {
            this.nativeObject.batchInsert(uri, valueBuckets, this.doExtensionCallbackInt);
        }

        doOnCreate(want: Want): void {
            this.nativeObject.onCreate(want, this.doExtensionCallbackVoid);
        }
    }
}

export default class DataShareExtensionAbility {

    context: ExtensionContext;

    constructor(context: ExtensionContext) {
        this.context = context;
    }

    onCreate(want: Want, callback: AsyncCallback<void>): void {
        console.info("basic onCreate");
    }

    insert(uri: string, valueBucket: ValuesBucket, callback: AsyncCallback<int>): void {
        console.info("basic insert");
    }

    update(
        uri: string,
        predicates: dataSharePredicates.DataSharePredicates,
        valueBucket: ValuesBucket,
        callback: AsyncCallback<int>
    ): void {
        console.info("basic update");
    }

    delete(uri: string, predicates: dataSharePredicates.DataSharePredicates, callback: AsyncCallback<int>): void {
        console.info("basic delete");
    }

    query(
        uri: string,
        predicates: dataSharePredicates.DataSharePredicates,
        columns: Array<string>,
        callback: AsyncCallback<Object>
    ): void {
        console.info("basic query");
    }

    batchInsert(uri: string, valueBuckets: Array<ValuesBucket>, callback: AsyncCallback<int>): void {
        console.info("basic batchInsert");
    }
}
