/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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
import DataShareExtensionAbility from '@ohos.application.DataShareExtensionAbility'
import relationalStore from '@ohos.data.relationalStore';
import Want from '@ohos.app.ability.Want';
import dataSharePredicates from '@ohos.data.dataSharePredicates';
import { ValuesBucket } from '@ohos.data.ValuesBucket';
import { BusinessError } from '@ohos.base';
import dataShare from '@ohos.data.dataShare';
import { AsyncCallback } from '@ohos.base';
 
const STORE_CONFIG: relationalStore.StoreConfig = {
  name: "DB00.db",
  securityLevel: relationalStore.SecurityLevel.S1,
}
let TBL_NAME = "TBL00";
let DDL_TBL_CREATE = "CREATE TABLE IF NOT EXISTS "
  + TBL_NAME
  + " (id INTEGER PRIMARY KEY AUTOINCREMENT, name0 TEXT, name1 TEXT, name2 TEXT, name3 TEXT, name4 TEXT, name5 TEXT, name6 TEXT, " +
  "name7 TEXT, name8 TEXT, name9 TEXT, age INTEGER, phoneNumber DOUBLE, isStudent BOOLEAN, Binary BINARY)";
 
let rdbStore: relationalStore.RdbStore | undefined;
 
function convertToRdbValuesBucket(valueBucket: ValuesBucket): relationalStore.ValuesBucket
{
  let rdbValuesBucket: relationalStore.ValuesBucket = {};
  valueBucket.forEach((value, key) => {
    rdbValuesBucket[key] = value;
  })
  return rdbValuesBucket;
}
 
export default class DataShareExtAbility extends DataShareExtensionAbility {
  onCreateTest(want: Want, callback: AsyncCallback<void>): void {
    console.log('[ttt] [DataShareTest] <<Provider>> DataShareExtAbility onCreate, want:' + want.abilityName);
    console.log("[ttt] [DataShareTest] DataShareExtAbility onCreate this.context.databaseDir:" + this.context.databaseDir);
 
    rdbStore = await relationalStore.getRdbStore(this.context, STORE_CONFIG);
    console.log('[ttt] [DataShareTest] <<Provider>> DataShareExtAbility getRdbStore done');
    await (rdbStore as relationalStore.RdbStore).execute(DDL_TBL_CREATE, []); // await rdbStore.executeSql(DDL_TBL_CREATE, []);
    console.log('[ttt] [DataShareTest] <<Provider>> DataShareExtAbility executeSql done');
    // let ret1 = await rdbStore?.insert(TBL_NAME, {"name0": "lisa1",});
    // console.log('[ttt] [DataShareTest] <<Provider>> DataShareExtAbility insert done ' + ret1);
    callback({"code": 0}, undefined);
    console.log('[ttt] [DataShareTest] <<Provider>> DataShareExtAbility onCreate end');
  }
 
  insertTest(uri: string, value: ValuesBucket, callback: AsyncCallback<int>): void {
    console.info('[ttt] [DataShareTest] <<Provider>> [insert] enter');
    if (value == null) {
      console.info('[ttt] [DataShareTest] <<Provider>> [insert] invalid valueBuckets');
      return;
    }
    console.info('[ttt] [DataShareTest] <<Provider>> [insert]  value = ' + value);
    console.info('[ttt] [DataShareTest] <<Provider>> [insert]  value = ' + JSON.stringify(value));
    let conflict = relationalStore.ConflictResolution.ON_CONFLICT_REPLACE;
    rdbStore?.insert(TBL_NAME, convertToRdbValuesBucket(value), conflict).then((ret: long) => {
      console.info('[ttt] [DataShareTest] <<Provider>> [insert] callback ret:' + ret);
      if (callback != undefined) {
        callback({'code': 0}, ret as int);
      }
    }).catch((err) => {
      console.info(`[ttt] [DataShareTest] <<Provider>> [insert] rdb insert error : code: ${err.code}, message: ${err.message}`);
    });
    console.info('[ttt] [DataShareTest] <<Provider>> [insert] leave');
  }
 
  updateTest(uri: string, predicates: dataSharePredicates.DataSharePredicates, value: ValuesBucket, callback: AsyncCallback<int>): void {
    console.info('[ttt] [DataShareTest] <<Provider>> [update] enter');
    if (predicates == null || predicates == undefined) {
      console.info('[ttt] [DataShareTest] <<Provider>> [update] invalid predicates');
      return;
    }
    console.info('[ttt] [DataShareTest] <<Provider>> [update]  values = ' + value);
    console.info('[ttt] [DataShareTest] <<Provider>> [update]  values = ' + JSON.stringify(value));
    console.info('[ttt] [DataShareTest] <<Provider>> [update]  predicates = ' + predicates);
    console.info('[ttt] [DataShareTest] <<Provider>> [update]  predicates = ' + JSON.stringify(predicates));
    try {
      rdbStore?.update(TBL_NAME, convertToRdbValuesBucket(value), predicates).then(async (ret: long)  => {
        console.info('[ttt] [DataShareTest] <<Provider>> [update] callback ret:' + ret);
        // console.info('[ttt] [DataShareTest] <<Provider>> [update] callback err:' + err);
        if (callback != undefined) {
          callback({'code': 0}, ret as int);
        }
      });
    } catch (err) {
      console.error('[ttt] [DataShareTest] <<Provider>> [update] error' + err);
    }
    console.info('[ttt] [DataShareTest] <<Provider>> [update] leave');
  }
 
  deleteTest(uri: string, predicates: dataSharePredicates.DataSharePredicates, callback: AsyncCallback<int>): void {
    console.info('[ttt] [DataShareTest] <<Provider>> [delete] enter');
    if (predicates == null || predicates == undefined) {
      console.info('[ttt] [DataShareTest] <<Provider>> [delete] invalid predicates');
      return;
    }
    console.info('[ttt] [DataShareTest] <<Provider>> [delete]  predicates = ' + predicates);
    console.info('[ttt] [DataShareTest] <<Provider>> [delete]  predicates = ' + JSON.stringify(predicates));
    rdbStore?.delete(TBL_NAME, predicates).then(async (ret: long) => {
      console.log('[ttt] [DataShareTest] <<Provider>> [delete] ret:' + ret);
      if (callback != undefined) {
        callback({'code': 0}, ret as int);
      }
    }).catch((err) => {
      console.log(`[ttt] [DataShareTest] <<Provider>> [delete] error: code: ${err.code}, message: ${err.message}`);
    });
    console.info('[ttt] [DataShareTest] <<Provider>> [delete] leave');
  }
 
  queryTest(uri: string, predicates: dataSharePredicates.DataSharePredicates, columns: Array<string>,  callback: AsyncCallback<Object>): void {
    console.info('[ttt] [DataShareTest] <<Provider>> [query] enter');
    if (predicates == null || predicates == undefined) {
      console.info('[ttt] [DataShareTest] <<Provider>> [query] invalid predicates');
    }
    try {
      console.info("[ttt] [DataShareTest] <<Provider>> [query] local query begin " );
      rdbStore?.query(TBL_NAME, predicates, columns).then(async (resultSet: relationalStore.ResultSet) => {
        if (resultSet != undefined) {
          console.info('[ttt] [DataShareTest] <<Provider>> [query] local query resultSet.rowCount: ' + resultSet.rowCount);
          console.info("[ttt] [DataShareTest] <<Provider>> [query] local query end ");
        }
        if (callback != undefined) {
          callback({'code': 0}, resultSet);
        }
      }).catch((err) => {
        console.info(`[ttt] [DataShareTest] <<Provider>> [query] error: code: ${err.code}, message: ${err.message}`);
      });
    } catch (err) {
      console.error('[ttt] [DataShareTest] <<Provider>> [query] local query error' + err);
    }
    console.info('[ttt] [DataShareTest] <<Provider>> [query] leave');
  }
 
  batchInsertTest(uri: string, valueBuckets: Array<ValuesBucket>, callback: AsyncCallback<int>): void {
    console.info('[ttt] [DataShareTest] <<Provider>> [batchInsert] enter');
    let tableTmp = TBL_NAME;
    if (valueBuckets == null || valueBuckets.length == undefined) {
      console.info('[ttt] [DataShareTest] <<Provider>> [batchInsert] invalid valueBuckets');
      return;
    }
    console.info('[ttt] [DataShareTest] <<Provider>> [batchInsert] valueBuckets.length:' + valueBuckets.length);
    let rdbValuesBuckets: Array<relationalStore.ValuesBucket> = [];
    valueBuckets.forEach(item => {
      rdbValuesBuckets.push(convertToRdbValuesBucket(item));
      console.info(`[ttt] [DataShareTest] <<Provider>> [batchInsert] valueBuckets, item: ${item}`);
    })
    try{
      rdbStore?.batchInsert(TBL_NAME, rdbValuesBuckets).then(async (ret: long) => {
        console.info('[ttt] [DataShareTest] <<Provider>> [batchInsert] callback ret:' + ret);
        let err: BusinessError = {"code": 0};
        if (callback != undefined) {
          callback(err, ret as int);
        }
      }).catch((err) => {
        console.info(`[ttt] [DataShareTest] <<Provider>> [query] error: code: ${err.code}, message: ${err.message}`);
      });
    } catch (err) {
      console.error('[ttt] [DataShareTest] <<Provider>> [batchInsert] local batchInsert error' + err);
    }
    console.info('[ttt] [DataShareTest] <<Provider>> [batchInsert] leave');
  }
 
  // 重写batchUpdate接口
  batchUpdateTest(operations: Record<string, Array<dataShare.UpdateOperation>>, callback: AsyncCallback<Record<string, Array<int>>>): void {
    console.info('[ttt] [DataShareTest] <<Provider>> [batchUpdate] enter');
    let recordOps : Record<string, Array<dataShare.UpdateOperation>> = operations;
    let results : Record<string, Array<int>> = {};
    let countRows: long = 0;
    try {
      recordOps.forEach((values, key) => {
        const result : Array<int> = [];
        for (const value of values) {
          console.info('[ttt] Update TBL_NAME is ' + TBL_NAME);
          console.info('[ttt] Update value is ' + JSON.stringify(value.values));
          console.info('[ttt] Update predicates is ' + value.predicates);
          await (rdbStore as relationalStore.RdbStore).update(TBL_NAME, convertToRdbValuesBucket(value.values), value.predicates).then(async (rows: long) => {
            console.info('[ttt] Update row count is ' + rows);
            result.push(rows as int);
            countRows = countRows + rows;
          }).catch((err) => {
            console.info(`[ttt] Update failed, err is code: ${err.code}, message: ${err.message}`);
            result.push(-1)
          });
        }
        results[key] = result;
      })
 
      if (callback != undefined) {
        console.info('[ttt] [DataShareTest] <<Provider>> [batchUpdate] callback results is ' + JSON.stringify(results));
        callback({'code': 0}, results);
      }
    } catch (err) {
      console.error('[ttt] [DataShareTest] <<Provider>> [batchUpdate] local batchUpdate error' + err);
    }
    console.info('[ttt] [DataShareTest] <<Provider>> [batchUpdate] leave');
  }
 
  normalizeUriTest(uri: string, callback: AsyncCallback<string>): void {
    console.info('[ttt] [DataShareTest] <<Provider>> [normalizeUri] enter');
    let ret = "normalize+" + uri;
    callback({"code":0}, ret);
    console.info('[ttt] [DataShareTest] <<Provider>> [normalizeUri] leave, ret:' + ret);
  }
 
  denormalizeUriTest(uri: string, callback: AsyncCallback<string>): void {
    console.info('[ttt] [DataShareTest] <<Provider>> [denormalizeUri] enter');
    let ret = "denormalize+" + uri;
    callback({"code":0}, ret);
    console.info('[ttt] [DataShareTest] <<Provider>> [denormalizeUri] leave, ret:' + ret);
  }
 
  constructor() {
    this.onCreate = this.onCreateTest;
    this.insert = this.insertTest;
    this.update = this.updateTest;
    this.delete = this.deleteTest;
    this.query = this.queryTest;
    this.batchInsert = this.batchInsertTest;
    this.batchUpdate = this.batchUpdateTest;
    this.normalizeUri = this.denormalizeUriTest;
    this.denormalizeUri = this.denormalizeUriTest;
    console.log(`[ttt] [DataShareTest] <<Provider>> DataShareExtAbility constructor end.`);
  }
};