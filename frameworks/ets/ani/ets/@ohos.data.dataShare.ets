/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

import { BusinessError } from '@ohos.base'
import type { AsyncCallback, Callback } from '@ohos.base';
import Context from 'application.Context';
import DataShareResultSet from './@ohos.data.DataShareResultSet';
import dataSharePredicates from './@ohos.data.dataSharePredicates';
import { ValuesBucket } from '@ohos.data.ValuesBucket';

export namespace dataShare {
  loadLibrary("datashare_ani")

  class Cleaner {
    private nativePtr: long = 0
    constructor(ptr:long) {
      this.nativePtr = ptr
    }
    native native_clean(): void
  }

  let destroyRegister = new FinalizationRegistry<Cleaner>((cleaner: Cleaner) => {cleaner.native_clean()})
  let unregisterToken = new object()

  export interface DataShareHelperOptions {
    isProxy?: boolean;
  }

  export interface TemplateId {
    subscriberId: string;
    bundleNameOfOwner: string;
  }

  export interface PublishedItem {
    key: string;
    data: string | ArrayBuffer;
    subscriberId: string;
  }

  export interface RdbDataChangeNode {
    uri: string;
    templateId: TemplateId;
    data: Array<string>;
  }

  export interface PublishedDataChangeNode {
    bundleName: string;
    data: Array<PublishedItem>;
  }

  export interface Template {
    predicates: Record<string, string>;

    scheduler: string;

    update?: string;
  }

  export interface OperationResult {
    key: string;

    result: int;
  }

  export enum ChangeType {
    INSERT = 0,
    DELETE,
    UPDATE
  }

  export enum SubscriptionType {
      SUBSCRIPTION_TYPE_EXACT_URI = 0,
  }

  export interface ChangeInfo {
      changeType: ChangeType;

      uri: string;
      values: Array<ValuesBucket>;
  }

  export interface DataShareHelper {
    on(type: 'dataChange', uri: string, callback: Callback<void>): void;

    off(type: 'dataChange', uri: string, callback?: Callback<void>): void;

    on(event: 'dataChange', type: SubscriptionType, uri: string, callback: Callback<ChangeInfo>): void;

    off(event: 'dataChange', type: SubscriptionType, uri: string, callback?: Callback<ChangeInfo>): void;

    addTemplate(uri: string, subscriberId: string, template: Template): void;

    delTemplate(uri: string, subscriberId: string): void;

    on(
      type: 'rdbDataChange',
      uris: Array<string>,
      templateId: TemplateId,
      callback: Callback<RdbDataChangeNode>
    ): Array<OperationResult>;

    off(
      type: 'rdbDataChange',
      uris: Array<string>,
      templateId: TemplateId,
      callback?: Callback<RdbDataChangeNode>
    ): Array<OperationResult>;

    on(
      type: 'publishedDataChange',
      uris: Array<string>,
      subscriberId: string,
      callback: Callback<PublishedDataChangeNode>
    ): Array<OperationResult>;

    off(
      type: 'publishedDataChange',
      uris: Array<string>,
      subscriberId: string,
      callback?: Callback<PublishedDataChangeNode>
    ): Array<OperationResult>;

    publish(
      data: Array<PublishedItem>,
      bundleName: string,
      version: int,
      callback: AsyncCallback<Array<OperationResult>>
    ): void;

    publish(
      data: Array<PublishedItem>,
      bundleName: string,
      callback: AsyncCallback<Array<OperationResult>>
    ): void;

    publish(data: Array<PublishedItem>, bundleName: string, version?: int): Promise<Array<OperationResult>>;

    getPublishedData(bundleName: string, callback: AsyncCallback<Array<PublishedItem>>): void;

    getPublishedData(bundleName: string): Promise<Array<PublishedItem>>;

    insert(uri: string, value: ValuesBucket, callback: AsyncCallback<int>): void;

    insert(uri: string, value: ValuesBucket): Promise<int>;

    delete(uri: string, predicates: dataSharePredicates.DataSharePredicates, callback: AsyncCallback<int>): void;

    delete(uri: string, predicates: dataSharePredicates.DataSharePredicates): Promise<int>;

    query(
      uri: string,
      predicates: dataSharePredicates.DataSharePredicates,
      columns: Array<string>,
      callback: AsyncCallback<DataShareResultSet>
    ): void;

    query(
      uri: string,
      predicates: dataSharePredicates.DataSharePredicates,
      columns: Array<string>
    ): Promise<DataShareResultSet>;

    update(
      uri: string,
      predicates: dataSharePredicates.DataSharePredicates,
      value: ValuesBucket,
      callback: AsyncCallback<int>
    ): void;

    update(uri: string, predicates: dataSharePredicates.DataSharePredicates, value: ValuesBucket): Promise<int>;

    batchInsert(uri: string, values: Array<ValuesBucket>, callback: AsyncCallback<int>): void;

    batchInsert(uri: string, values: Array<ValuesBucket>): Promise<int>;

    close(): Promise<void>;
  }

  export native function native_create(context: Context, uri: string, options?: DataShareHelperOptions): DataShareHelper
  export native function native_query(obj: DataShareHelper, uri: string, predicates: dataSharePredicates.DataSharePredicates, columns: Array<string>): DataShareResultSet
  export native function native_update(obj: DataShareHelper, uri: string, predicates: dataSharePredicates.DataSharePredicates, value: ValuesBucket): int
  export native function native_publish(obj: DataShareHelper, data: Array<PublishedItem>, bundleName: string, version?: int): Array<OperationResult>
  export native function native_get_published_data(obj: DataShareHelper, bundleName: string): Array<PublishedItem>
  export native function native_insert(obj: DataShareHelper, uri: string, value: ValuesBucket): int
  export native function native_batch_insert(obj: DataShareHelper, uri: string, values: Array<ValuesBucket>): int
  export native function native_delete(obj: DataShareHelper, uri: string, predicates: dataSharePredicates.DataSharePredicates): int
  export native function native_close(obj: DataShareHelper):void
  export native function native_on(obj: DataShareHelper, type: string, uri: string, callback: Callback<void>): void
  export native function native_off(obj: DataShareHelper, type: string, uri: string, callback?: Callback<void>): void
  export native function native_on_changeinfo(obj: DataShareHelper, event: string, type: SubscriptionType, uri: string, callback: Callback<ChangeInfo>): void
  export native function native_off_changeinfo(obj: DataShareHelper, event: string, type: SubscriptionType, uri: string, callback?: Callback<ChangeInfo>): void
  export native function native_on_rdb_data_change(obj: DataShareHelper, type: string, uris: Array<string>, templateId: TemplateId, callback: Callback<RdbDataChangeNode>): Array<OperationResult>
  export native function native_off_rdb_data_change(obj: DataShareHelper, type: string, uris: Array<string>, templateId: TemplateId, callback?: Callback<RdbDataChangeNode>): Array<OperationResult>
  export native function native_on_published_data_change(obj: DataShareHelper, type: string, uris: Array<string>, subscriberId: string, callback: Callback<PublishedDataChangeNode>): Array<OperationResult>
  export native function native_off_published_data_change(obj: DataShareHelper, type: string, uris: Array<string>, subscriberId: string, callback?: Callback<PublishedDataChangeNode>): Array<OperationResult>

  export function createDataShareHelper(context: Context, uri: string, callback: AsyncCallback<DataShareHelper>): void {
        let p = taskpool.execute(native_create, context, uri, undefined)
        p.then((e: NullishType) => {
            let r = e as DataShareHelper
            let err: BusinessError<void>
            callback(err, r)
        })
  }

  export function createDataShareHelper(
    context: Context,
    uri: string,
    options: DataShareHelperOptions,
    callback: AsyncCallback<DataShareHelper>
  ): void {
        let p = taskpool.execute(native_create, context, uri, options)
        p.then((e: NullishType) => {
            let r = e as DataShareHelper
            let err: BusinessError<void>
            callback(err, r)
        })
  }

  export function createDataShareHelper(
    context: Context,
    uri: string,
    options?: DataShareHelperOptions
  ): Promise<DataShareHelper>  {
      return new Promise<DataShareHelper>((resolve, reject) => {
          let p = taskpool.execute(native_create, context, uri, options)
          p.then((e: NullishType) => {
              let r = e as DataShareHelper
              resolve(r)
          }).catch((e: Error): void => {
              reject(e)
          })
      })
  }

  class DataShareHelperInner implements DataShareHelper {
    private nativePtr: long = 0
    private cleaner: Cleaner | null = null;

    constructor(nativePtr: long) {
        if(this.nativePtr == 0){
            this.nativePtr = nativePtr;
        }
        this.registerCleaner(this.nativePtr)
    }

    registerCleaner(ptr: long): void {
        this.cleaner = new Cleaner(ptr)
        destroyRegister.register(this, this.cleaner!, unregisterToken);
    }

    unregisterCleaner(): void {
        destroyRegister.unregister(unregisterToken);
    }

    native addTemplate(uri: string, subscriberId: string, template: Template): void;
    native delTemplate(uri: string, subscriberId: string): void;

    query(uri: string, predicates: dataSharePredicates.DataSharePredicates, columns: Array<string>, callback: AsyncCallback<DataShareResultSet>): void {
        let p = taskpool.execute(native_query, this, uri, predicates, columns)
        p.then((e: NullishType) => {
            let r = e as DataShareResultSet
            let err: BusinessError<void>
            callback(err, r)
        })
    }

    query(uri: string, predicates: dataSharePredicates.DataSharePredicates, columns: Array<string>): Promise<DataShareResultSet> {
        return new Promise<DataShareResultSet>((resolve, reject) => {
            let p = taskpool.execute(native_query, this, uri, predicates, columns)
            p.then((e: NullishType) => {
                let r = e as DataShareResultSet
                resolve(r)
            }).catch((e: Error): void => {
                reject(e)
            })
        })
    }

    update(
      uri: string,
      predicates: dataSharePredicates.DataSharePredicates,
      value: ValuesBucket,
      callback: AsyncCallback<int>
    ): void {
        let p = taskpool.execute(native_update, this, uri, predicates, value)
        p.then((e: NullishType) => {
            let r = e as int
            let err: BusinessError<void>
            callback(err, r)
        })
    }

    update(uri: string, predicates: dataSharePredicates.DataSharePredicates, value: ValuesBucket): Promise<int> {
        return new Promise<int>((resolve, reject) => {
            let p = taskpool.execute(native_update, this, uri, predicates, value)
            p.then((e: NullishType) => {
                let r = e as int
                resolve(r)
            }).catch((e: Error): void => {
                reject(e)
            })
        })
    }

    publish(
      data: Array<PublishedItem>,
      bundleName: string,
      version: int,
      callback: AsyncCallback<Array<OperationResult>>
    ): void {
        let p = taskpool.execute(native_publish, this, data, bundleName, version)
        p.then((e: NullishType) => {
            let r = e as Array<OperationResult>
            let err: BusinessError<void>
            callback(err, r)
        })
    }

    publish(
      data: Array<PublishedItem>,
      bundleName: string,
      callback: AsyncCallback<Array<OperationResult>>
    ): void {
        let p = taskpool.execute(native_publish, this, data, bundleName, undefined)
        p.then((e: NullishType) => {
            let r = e as Array<OperationResult>
            let err: BusinessError<void>
            callback(err, r)
        })
    }

    publish(data: Array<PublishedItem>, bundleName: string, version?: int): Promise<Array<OperationResult>> {
        return new Promise<Array<OperationResult>>((resolve, reject) => {
            let p = taskpool.execute(native_publish, this, data, bundleName, version)
            p.then((e: NullishType) => {
                let r = e as Array<OperationResult>
                resolve(r)
            }).catch((e: Error): void => {
                reject(e)
            })
        })
    }

    getPublishedData(bundleName: string, callback: AsyncCallback<Array<PublishedItem>>): void {
        let p = taskpool.execute(native_get_published_data, this, bundleName)
        p.then((e: NullishType) => {
            let r = e as Array<PublishedItem>
            let err: BusinessError<void>
            callback(err, r)
        })
    }

    getPublishedData(bundleName: string): Promise<Array<PublishedItem>> {
        return new Promise<Array<PublishedItem>>((resolve, reject) => {
            let p = taskpool.execute(native_get_published_data, this, bundleName)
            p.then((e: NullishType) => {
                let r = e as Array<PublishedItem>
                resolve(r)
            }).catch((e: Error): void => {
                reject(e)
            })
        })
    }

    insert(uri: string, value: ValuesBucket, callback: AsyncCallback<int>): void {
        let p = taskpool.execute(native_insert, this, uri, value)
        p.then((e: NullishType) => {
            let r = e as int
            let err: BusinessError<void>
            callback(err, r)
        })
    }

    insert(uri: string, value: ValuesBucket): Promise<int> {
        return new Promise<int>((resolve, reject) => {
            let p = taskpool.execute(native_insert, this, uri, value)
            p.then((e: NullishType) => {
                let r = e as int
                resolve(r)
            }).catch((e: Error): void => {
                reject(e)
            })
        })
    }

    batchInsert(uri: string, values: Array<ValuesBucket>, callback: AsyncCallback<int>): void {
        let p = taskpool.execute(native_batch_insert, this, uri, values)
        p.then((e: NullishType) => {
            let r = e as int
            let err: BusinessError<void>
            callback(err, r)
        })
    }

    batchInsert(uri: string, values: Array<ValuesBucket>): Promise<int> {
        return new Promise<int>((resolve, reject) => {
            let p = taskpool.execute(native_batch_insert, this, uri, values)
            p.then((e: NullishType) => {
                let r = e as int
                resolve(r)
            }).catch((e: Error): void => {
                reject(e)
            })
        })
    }

    delete(uri: string, predicates: dataSharePredicates.DataSharePredicates, callback: AsyncCallback<int>): void {
        let p = taskpool.execute(native_delete, this, uri, predicates)
        p.then((e: NullishType) => {
            let r = e as int
            let err: BusinessError<void>
            callback(err, r)
        })
    }

    delete(uri: string, predicates: dataSharePredicates.DataSharePredicates): Promise<int> {
        return new Promise<int>((resolve, reject) => {
            let p = taskpool.execute(native_delete, this, uri, predicates)
            p.then((e: NullishType) => {
                let r = e as int
                resolve(r)
            }).catch((e: Error): void => {
                reject(e)
            })
        })
    }

    close(): Promise<void> {
        return new Promise<void>((resolve, reject) => {
            let p = taskpool.execute(native_close, this)
            p.then(() => {
                resolve(undefined);
            }).catch((e: Error): void => {
                reject(e)
            })
        })
    }

    on(type: 'dataChange', uri: string, callback: Callback<void>): void {
        native_on(this, type, uri, callback)
    }

    off(type: 'dataChange', uri: string, callback?: Callback<void>): void {
        native_off(this, type, uri, callback)
    }

    on(event: 'dataChange', type: SubscriptionType, uri: string, callback: Callback<ChangeInfo>): void {
        native_on_changeinfo(this, event, type, uri, callback)
    }

    off(event: 'dataChange', type: SubscriptionType, uri: string, callback?: Callback<ChangeInfo>): void {
        native_off_changeinfo(this, event, type, uri, callback)
    }

    on(
      type: 'rdbDataChange',
      uris: Array<string>,
      templateId: TemplateId,
      callback: Callback<RdbDataChangeNode>
    ): Array<OperationResult> {
        return native_on_rdb_data_change(this, type, uris, templateId, callback);
    }

    off(
      type: 'rdbDataChange',
      uris: Array<string>,
      templateId: TemplateId,
      callback?: Callback<RdbDataChangeNode>
    ): Array<OperationResult> {
        return native_off_rdb_data_change(this, type, uris, templateId, callback);
    }

    on(
      type: 'publishedDataChange',
      uris: Array<string>,
      subscriberId: string,
      callback: Callback<PublishedDataChangeNode>
    ): Array<OperationResult> {
        return native_on_published_data_change(this, type, uris, subscriberId, callback)
    }

    off(
      type: 'publishedDataChange',
      uris: Array<string>,
      subscriberId: string,
      callback?: Callback<PublishedDataChangeNode>
    ): Array<OperationResult> {
        return native_off_published_data_change(this, type, uris, subscriberId, callback)
    }
  }

  class DataShareHelperOptionsInner implements DataShareHelperOptions {
      isProxy?: boolean;
  }

  class PublishedItemInner implements PublishedItem {
      key: string;
      data: string | ArrayBuffer;
      subscriberId: string;
  }

  class OperationResultInner implements OperationResult {
      key: string;
      result: int;
  }

  class TemplateInner implements Template {
    predicates: Record<string, string>;
    scheduler: string;
    update?: string;
  }

  class ChangeInfoInner implements ChangeInfo {
      changeType: ChangeType;
      uri: string;
      values: Array<ValuesBucket>;
  }

  class TemplateIdInner implements TemplateId {
    subscriberId: string;
    bundleNameOfOwner: string;
  }

  class RdbDataChangeNodeInner implements RdbDataChangeNode {
    uri: string;
    templateId: TemplateId;
    data: Array<string>;
  }

  class PublishedDataChangeNodeInner implements PublishedDataChangeNode {
    bundleName: string;
    data: Array<PublishedItem>;
  }
}
