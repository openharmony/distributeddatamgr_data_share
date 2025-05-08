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

import hilog from '@ohos.hilog'
import { BusinessError } from '@ohos.base'
import type { AsyncCallback } from '@ohos.base'
import Context from 'application.Context'
import DataShareResultSet from '@ohos.data.DataShareResultSet'
import dataSharePredicates from '@ohos.data.dataSharePredicates'
import { ValuesBucket } from '@ohos.data.ValuesBucket'

export namespace dataShare {
    loadLibrary("datashare_ani")

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
        on(type: 'dataChange', uri: string, callback: AsyncCallback<void>): void
        off(type: 'dataChange', uri: string, callback?: AsyncCallback<void>): void
        on(event: 'dataChange', type: SubscriptionType, uri: string, callback: AsyncCallback<ChangeInfo>): void
        off(event: 'dataChange', type: SubscriptionType, uri: string, callback?: AsyncCallback<ChangeInfo>): void
        query(uri: string, predicates: dataSharePredicates.DataSharePredicates, columns: Array<string>, callback: AsyncCallback<DataShareResultSet>): void
        query(uri: string, predicates: dataSharePredicates.DataSharePredicates, columns: Array<string>): Promise<DataShareResultSet>
        update(uri: string, predicates: dataSharePredicates.DataSharePredicates, value: ValuesBucket, callback: AsyncCallback<number>): void
        update(uri: string, predicates: dataSharePredicates.DataSharePredicates, value: ValuesBucket): Promise<number>
    }

    export native function create(context: Context, uri: string, options?: DataShareHelperOptions): DataShareHelper

    export function createDataShareHelper(context: Context, uri: string, callback: AsyncCallback<DataShareHelper>): void {
        let p = taskpool.execute(create, context, uri, undefined)
        p.then((e: NullishType) => {
            let r = e as DataShareHelper
            let err: BusinessError<void>
            callback(err, r)
        }).catch((error: NullishType) => {
            let r = new DataShareHelperInner(0)
            let err =  error as BusinessError<void>
            hilog.error(0x0000, 'DataShareHelper', "create failed:" + err)
            callback(err, r)
        })
    }

    export function createDataShareHelper(context: Context, uri: string, options: DataShareHelperOptions, callback: AsyncCallback<DataShareHelper>): void {
        let p = taskpool.execute(create, context, uri, options)
        p.then((e: NullishType) => {
            let r = e as DataShareHelper
            let err: BusinessError<void>
            callback(err, r)
        }).catch((error: NullishType) => {
            let r = new DataShareHelperInner(0)
            let err =  error as BusinessError<void>
            hilog.error(0x0000, 'DataShareHelper', "create failed:" + err)
            callback(err, r)
        })
    }

    export function createDataShareHelper(context: Context, uri: string, options?: DataShareHelperOptions): Promise<DataShareHelper> {
        console.println("createDataShareHelper enter")
        return new Promise<DataShareHelper>((resolve: (v: DataShareHelper) => void, reject: (error: Error) => void) => {
            let p = taskpool.execute(create, context, uri, options)
            p.then((e: NullishType) => {
                let r = e as DataShareHelper
                resolve(r)
            }).catch((e: Error): void => {
                hilog.error(0x0000, 'DataShareHelper', "create failed:" + e)
                reject(e)
            })
        })
    }

    class ChangeInfoInner implements ChangeInfo {
        type: ChangeType
        uri: string
        values: Array<ValuesBucket>
    }

    class Cleaner {
    static callback(cleaner: Cleaner): void {
        console.println("enter Cleaner.callback");
        cleaner.clean()
    }

    constructor(targetPtr: long) {
        this.targetPtr = targetPtr
    }

    native clean(): void

    private targetPtr: long = 0
    }

    class FinalizationAgent<T extends Object> {
    constructor(obj: T, ptr: long) {
        this.register(obj, ptr);
    }

    register(obj: T, ptr: long): void {
        this.unregisterToken = {};
        this.cleaner = new Cleaner(ptr);
        finalizer.register(obj, this.cleaner!, this.unregisterToken);
    }

    unregister(): void {
        finalizer.unregister(this.unregisterToken);
    }

    private cleaner: Cleaner | null = null;
    private unregisterToken: object;
    }

    let finalizer = new FinalizationRegistry<Cleaner>(Cleaner.callback)

    class DataShareHelperInner implements DataShareHelper {
        private nativePtr: long = 0
        private fzAgent: FinalizationAgent<DataShareHelperInner>;
        private onCallback: AsyncCallback<void> = () => {}
        private offCallback?: AsyncCallback<void>
        private onEventCallback: AsyncCallback<ChangeInfo> = () => {}
        private offEventCallback?: AsyncCallback<ChangeInfo>

        constructor(nativePtr:long) {
            hilog.info(0x0000, 'DataShareHelper', "in constructor, context ：" + nativePtr)
            this.nativePtr = nativePtr
            this.fzAgent = new FinalizationAgent<DataShareHelperInner>(this, this.nativePtr);
        }

        unregisterCleaner(): void {
            this.fzAgent.unregister();
    }

        getNativePtr(): long {
            return this.nativePtr
        }

        native on(type: 'dataChange', uri: string, callback: AsyncCallback<void>): void
        native off(type: 'dataChange', uri: string, callback?: AsyncCallback<void>): void
        native on(event: 'dataChange', type: SubscriptionType, uri: string, callback: AsyncCallback<ChangeInfo>): void
        native off(event: 'dataChange', type: SubscriptionType, uri: string, callback?: AsyncCallback<ChangeInfo>): void
        native ani_query(uri: string, predicates: dataSharePredicates.DataSharePredicates, columns: Array<string>): DataShareResultSet
        native ani_update(uri: string, predicates: dataSharePredicates.DataSharePredicates, value: ValuesBucket): number

        query(uri: string, predicates: dataSharePredicates.DataSharePredicates, columns: Array<string>, callback: AsyncCallback<DataShareResultSet>): void {
            let p = taskpool.execute(this.ani_query, uri, predicates, columns)
            p.then((e: NullishType) => {
                let r = e as DataShareResultSet
                let err: BusinessError<void>
                callback(err, r)
            }).catch((error: NullishType) => {
                let err =  error as BusinessError<void>
                hilog.error(0x0000, 'DataShareHelper', "query failed:" + err)
            })
        }

        query(uri: string, predicates: dataSharePredicates.DataSharePredicates, columns: Array<string>): Promise<DataShareResultSet> {
            return new Promise<DataShareResultSet>((resolve: (v: DataShareResultSet) => void, reject: (error: Error) => void) => {
                let p = taskpool.execute(this.ani_query, uri, predicates, columns)
                p.then((e: NullishType) => {
                    let r = e as DataShareResultSet
                    resolve(r)
                }).catch((e: Error): void => {
                    hilog.error(0x0000, 'DataShareHelper', "query failed:" + e)
                    reject(e)
                })
            })
        }

        update(uri: string, predicates: dataSharePredicates.DataSharePredicates, value: ValuesBucket, callback: AsyncCallback<number>): void {
            let p = taskpool.execute(this.ani_update, uri, predicates, value)
            p.then((e: NullishType) => {
                let r = e as number
                let err: BusinessError<void>
                callback(err, r)
            }).catch((error: NullishType) => {
                let err =  error as BusinessError<void>
                hilog.error(0x0000, 'DataShareHelper', "create failed:" + err)
            })
        }

        update(uri: string, predicates: dataSharePredicates.DataSharePredicates, value: ValuesBucket): Promise<number> {
            return new Promise<number>((resolve: (v: number) => void, reject: (error: Error) => void) => {
                let p = taskpool.execute(this.ani_update, uri, predicates, value)
                p.then((e: NullishType) => {
                    let r = e as number
                    resolve(r)
                }).catch((e: Error): void => {
                    hilog.error(0x0000, 'DataShareHelper', "update failed:" + e)
                    reject(e)
                })
            })
        }
    }
}
