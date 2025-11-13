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

import { BusinessError } from '@ohos.base'
import type { Callback, AsyncCallback } from '@ohos.base';
import Context from 'application.Context';
import DataShareResultSet from './@ohos.data.DataShareResultSet';
import dataSharePredicates from './@ohos.data.dataSharePredicates';
import { ValuesBucket } from '@ohos.data.ValuesBucket';

export namespace dataShare {
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
    type: ChangeType;

    uri: string;
    values: Array<ValuesBucket>;
  }

  export interface DataShareHelper {
    onDataChange(uri: string, callback: Callback<void>): void;

    offDataChange(uri: string, callback?: Callback<void>): void;

    onDataChange(type: SubscriptionType, uri: string, callback: Callback<ChangeInfo>): void;

    offDataChange(type: SubscriptionType, uri: string, callback?: Callback<ChangeInfo>): void;

    addTemplate(uri: string, subscriberId: string, template: Template): void;

    delTemplate(uri: string, subscriberId: string): void;

    onRdbDataChange(
      uris: Array<string>,
      templateId: TemplateId,
      callback: Callback<RdbDataChangeNode>
    ): Array<OperationResult>;

    offRdbDataChange(
      uris: Array<string>,
      templateId: TemplateId,
      callback?: Callback<RdbDataChangeNode>
    ): Array<OperationResult>;

    onPublishedDataChange(
      uris: Array<string>,
      subscriberId: string,
      callback: Callback<PublishedDataChangeNode>
    ): Array<OperationResult>;

    offPublishedDataChange(
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
  export native function native_on(obj: DataShareHelper, uri: string, callback: Callback<void>): void
  export native function native_off(obj: DataShareHelper, uri: string, callback?: Callback<void>): void
  export native function native_on_changeinfo(obj: DataShareHelper, type: SubscriptionType, uri: string, callback: Callback<ChangeInfo>): void
  export native function native_off_changeinfo(obj: DataShareHelper, type: SubscriptionType, uri: string, callback?: Callback<ChangeInfo>): void
  export native function native_on_rdb_data_change(obj: DataShareHelper, uris: Array<string>, templateId: TemplateId, callback: Callback<RdbDataChangeNode>): Array<OperationResult>
  export native function native_off_rdb_data_change(obj: DataShareHelper, uris: Array<string>, templateId: TemplateId, callback?: Callback<RdbDataChangeNode>): Array<OperationResult>
  export native function native_on_published_data_change(obj: DataShareHelper, uris: Array<string>, subscriberId: string, callback: Callback<PublishedDataChangeNode>): Array<OperationResult>
  export native function native_off_published_data_change(obj: DataShareHelper, uris: Array<string>, subscriberId: string, callback?: Callback<PublishedDataChangeNode>): Array<OperationResult>

  export function createDataShareHelper(context: Context, uri: string, callback: AsyncCallback<DataShareHelper>): void {
        let p = taskpool.execute(native_create, context, uri, undefined)
        p.then((r: NullishType) => {
            callback(null, r as DataShareHelper);
        }, (err: Error): void => {
            callback(err as BusinessError, undefined);
        });
  }

  export function createDataShareHelper(
    context: Context,
    uri: string,
    options: DataShareHelperOptions,
    callback: AsyncCallback<DataShareHelper>
  ): void {
        let p = taskpool.execute(native_create, context, uri, options)
        p.then((r: NullishType) => {
            callback(null, r as DataShareHelper);
        }, (err: Error): void => {
            callback(err as BusinessError, undefined);
        });
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
        p.then((r: NullishType) => {
            callback(null, r as DataShareResultSet);
        }, (err: Error): void => {
            callback(err as BusinessError, undefined);
        });
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
        p.then((r: NullishType) => {
            callback(null, r as int);
        }, (err: Error): void => {
            callback(err as BusinessError, undefined);
        });
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
        p.then((r: NullishType) => {
            callback(null, r as Array<OperationResult>);
        }, (err: Error): void => {
            callback(err as BusinessError, undefined);
        });
    }

    publish(
      data: Array<PublishedItem>,
      bundleName: string,
      callback: AsyncCallback<Array<OperationResult>>
    ): void {
        let p = taskpool.execute(native_publish, this, data, bundleName, undefined)
        p.then((r: NullishType) => {
            callback(null, r as Array<OperationResult>);
        }, (err: Error): void => {
            callback(err as BusinessError, undefined);
        });
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
        p.then((r: NullishType) => {
            callback(null, r as Array<PublishedItem>);
        }, (err: Error): void => {
            callback(err as BusinessError, undefined);
        });
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
        p.then((r: NullishType) => {
            callback(null, r as int);
        }, (err: Error): void => {
            callback(err as BusinessError, undefined);
        });
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
        p.then((r: NullishType) => {
            callback(null, r as int);
        }, (err: Error): void => {
            callback(err as BusinessError, undefined);
        });
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
        p.then((r: NullishType) => {
            callback(null, r as int);
        }, (err: Error): void => {
            callback(err as BusinessError, undefined);
        });
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

    onDataChange(uri: string, callback: Callback<void>): void {
        native_on(this, uri, callback)
    }

    offDataChange(uri: string, callback?: Callback<void>): void {
        native_off(this, uri, callback)
    }

    onDataChange(type: SubscriptionType, uri: string, callback: Callback<ChangeInfo>): void {
        native_on_changeinfo(this, type, uri, callback)
    }

    offDataChange(type: SubscriptionType, uri: string, callback?: Callback<ChangeInfo>): void {
        native_off_changeinfo(this, type, uri, callback)
    }

    onRdbDataChange(
      uris: Array<string>,
      templateId: TemplateId,
      callback: Callback<RdbDataChangeNode>
    ): Array<OperationResult> {
        return native_on_rdb_data_change(this, uris, templateId, callback);
    }

    offRdbDataChange(
      uris: Array<string>,
      templateId: TemplateId,
      callback?: Callback<RdbDataChangeNode>
    ): Array<OperationResult> {
        return native_off_rdb_data_change(this, uris, templateId, callback);
    }

    onPublishedDataChange(
      uris: Array<string>,
      subscriberId: string,
      callback: Callback<PublishedDataChangeNode>
    ): Array<OperationResult> {
        return native_on_published_data_change(this, uris, subscriberId, callback)
    }

    offPublishedDataChange(
      uris: Array<string>,
      subscriberId: string,
      callback?: Callback<PublishedDataChangeNode>
    ): Array<OperationResult> {
        return native_off_published_data_change(this, uris, subscriberId, callback)
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
    type: ChangeType;
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
