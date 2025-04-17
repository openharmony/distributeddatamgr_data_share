/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
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
import type { AsyncCallback } from '@ohos.base'
import Context from 'application.Context'
import DataShareResultSet from '@ohos.data.DataShareResultSet'
import dataSharePredicates from '@ohos.data.dataSharePredicates'
import { ValuesBucket } from '@ohos.data.ValuesBucket'

export namespace dataShare {
    loadLibrary("datashare")

    export interface DataShareHelperOptions {
        isProxy?: boolean;
    }

    enum ChangeType {
        INSERT = 0,
        DELETE,
        UPDATE
    }

    export enum SubscriptionType {
        SUBSCRIPTION_TYPE_EXACT_URI = 0,
    }

    export interface ChangeInfo {
        type: ChangeType
        uri: string
        values: Array<ValuesBucket>
    }

    export interface DataShareHelper {
        on(type: 'dataChange', uri: string, callback:AsyncCallback<void>):void
        off(type: 'dataChange', uri: string, callback:AsyncCallback<void>):void
        on(event: 'dataChange', type: SubscriptionType, uri: string, callback:AsyncCallback<ChangeInfo>):void
        off(event: 'dataChange', type: SubscriptionType, uri: string, callback:AsyncCallback<ChangeInfo>):void
        query(uri: string, predicates: dataSharePredicates.DataSharePredicates, columns: Array<string>, callback: AsyncCallback<DataShareResultSet>): void
        query(uri: string, predicates: dataSharePredicates.DataSharePredicates, columns: Array<string>): Promise<DataShareResultSet>
        update(uri: string, predicates: dataSharePredicates.DataSharePredicates, value: ValuesBucket, callback: AsyncCallback<number>): void
        update(uri: string, predicates: dataSharePredicates.DataSharePredicates, value: ValuesBucket): Promise<number>
    }

    export native function native_create(context: Context, uri: string, options?: DataShareHelperOptions): DataShareHelper
    native function native_on(obj: DataShareHelper, type: string, uri: string): void
    native function native_off(obj: DataShareHelper, type: string, uri: string): void
    native function native_onEvent(obj: DataShareHelper, event: string, type: SubscriptionType, uri: string): void
    native function native_offEvent(obj: DataShareHelper, event: string, type: SubscriptionType, uri: string): void
    export native function native_query(obj: DataShareHelper, uri: string, predicates: dataSharePredicates.DataSharePredicates, columns: Array<string>): DataShareResultSet
    export native function native_update(obj: DataShareHelper, uri: string, predicates: dataSharePredicates.DataSharePredicates, value: ValuesBucket): number

    export function createDataShareHelper(context: Context, uri: string, callback: AsyncCallback<DataShareHelper>): void {
        console.println("createDataShareHelper enter")
        let p = taskpool.execute(native_create, context, uri, undefined)
        p.then((e: NullishType) => {
            let r = e as DataShareHelper
            let err: BusinessError<void>
            callback(err, r)
        })
    }

    export function createDataShareHelper(context: Context, uri: string, options: DataShareHelperOptions,
        callback: AsyncCallback<DataShareHelper>): void {
        console.println("createDataShareHelper enter")
        let p = taskpool.execute(native_create, context, uri, options)
        p.then((e: NullishType) => {
            let r = e as DataShareHelper
            let err: BusinessError<void>
            callback(err, r)
        })
    }

    export function createDataShareHelper(context: Context, uri: string, options?: DataShareHelperOptions): Promise<DataShareHelper> {
        console.println("createDataShareHelper enter")
        return new Promise<DataShareHelper>((resolve: (v: DataShareHelper) => void, reject: (error: Error) => void) => {
            let p = taskpool.execute(native_create, context, uri, options)
            p.then((e: NullishType) => {
                let r = e as DataShareHelper
                resolve(r)
            }).catch((e: Error): void => {
                console.log("create failed:", e)
                reject(e)
            })
        })
    }

    class ChangeInfoInner implements ChangeInfo {
        type: ChangeType = ChangeType.INSERT
        uri: string = ""
        values: Array<ValuesBucket> = new Array<ValuesBucket>
    }

    class DataShareHelperInner implements DataShareHelper {
        private nativePtr: long = 0
        private onCallback: AsyncCallback<void> = () => {}
        private offCallback?: AsyncCallback<void>
        private onEventCallback: AsyncCallback<ChangeInfo> = () => {}
        private offEventCallback?: AsyncCallback<ChangeInfo>

        constructor(nativePtr:long) {
            console.log("in constructor, context is ", nativePtr)
            this.nativePtr = nativePtr
        }

        getNativePtr(): long {
            return this.nativePtr
        }

        on(type: 'dataChange', uri: string, callback: AsyncCallback<void>): void {
            this.onCallback = callback
            native_on(this, type, uri)
        }

        off(type: 'dataChange', uri: string, callback?: AsyncCallback<void>): void {
            if (callback != undefined) {
                this.offCallback = callback
            }
            native_off(this, type, uri)
        }

        on(event: 'dataChange', type: SubscriptionType, uri: string, callback: AsyncCallback<ChangeInfo>): void {
            this.onEventCallback = callback
            native_onEvent(this, event, type, uri)
        }

        off(event: 'dataChange', type: SubscriptionType, uri: string, callback?: AsyncCallback<ChangeInfo>): void {
            if (callback != undefined) {
                this.offEventCallback = callback
            }
            native_offEvent(this, event, type, uri)
        }

        query(uri: string, predicates: dataSharePredicates.DataSharePredicates, columns: Array<string>, callback: AsyncCallback<DataShareResultSet>): void {
            let p = taskpool.execute(native_query, this, uri, predicates, columns)
            p.then((e: NullishType) => {
                let r = e as DataShareResultSet
                let err: BusinessError<void>
                callback(err, r)
            })
        }

        query(uri: string, predicates: dataSharePredicates.DataSharePredicates, columns: Array<string>): Promise<DataShareResultSet> {
            return new Promise<DataShareResultSet>((resolve: (v: DataShareResultSet) => void, reject: (error: Error) => void) => {
                let p = taskpool.execute(native_query, this, uri, predicates, columns)
                p.then((e: NullishType) => {
                    let r = e as DataShareResultSet
                    resolve(r)
                }).catch((e: Error): void => {
                    console.log("query failed:", e)
                    reject(e)
                })
            })
        }

        update(uri: string, predicates: dataSharePredicates.DataSharePredicates, value: ValuesBucket, callback: AsyncCallback<number>): void {
            let p = taskpool.execute(native_update, this, uri, predicates, value)
            p.then((e: NullishType) => {
                let r = e as number
                let err: BusinessError<void>
                callback(err, r)
            })
        }

        update(uri: string, predicates: dataSharePredicates.DataSharePredicates, value: ValuesBucket): Promise<number> {
            return new Promise<number>((resolve: (v: number) => void, reject: (error: Error) => void) => {
                let p = taskpool.execute(native_update, this, uri, predicates, value)
                p.then((e: NullishType) => {
                    let r = e as number
                    resolve(r)
                }).catch((e: Error): void => {
                    console.log("update failed:", e)
                    reject(e)
                })
            })
        }
    }
}
