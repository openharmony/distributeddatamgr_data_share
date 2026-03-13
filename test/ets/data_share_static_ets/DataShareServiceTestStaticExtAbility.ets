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
// import { DataShareExtensionAbility, dataSharePredicates } from '@kit.ArkData';
import DataShareExtensionAbility from '@ohos.application.DataShareExtensionAbility';
import dataSharePredicates from '@ohos.data.dataSharePredicates';
import DataShareResultSet from '@ohos.data.DataShareResultSet';
import relationalStore from '@ohos.data.relationalStore';
import { ValuesBucket } from '@ohos.data.ValuesBucket';
import { AsyncCallback, BusinessError } from '@ohos.base';
import Want from '@ohos.app.ability.Want';
import hilog from '@ohos.hilog'
 
let domain: int = 0x0000;
let tag: string = 'testTag';
 
const STORE_CONFIG:relationalStore.StoreConfig = {
  name: 'DB00.db',
  securityLevel: relationalStore.SecurityLevel.S1,
};
let TBL_NAME = 'TBL00';
let DROP_TBL = 'DROP TABLE IF EXISTS ' + TBL_NAME;
let DDL_TBL_CREATE = "CREATE TABLE IF NOT EXISTS "
  + TBL_NAME
  + " (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT, age INTEGER, phoneNumber DOUBLE, isStudent BOOLEAN, Binary BINARY)";
 
let rdbStore:relationalStore.RdbStore | undefined = undefined;
 
function valueConvert(valueBucket: ValuesBucket): relationalStore.ValuesBucket
{
  let rdbValuesBucket: relationalStore.ValuesBucket = {};
  valueBucket.forEach((value, key) => {
    rdbValuesBucket[key] = value;
  })
  return rdbValuesBucket;
}
 
export default class DataShareExtAbility extends DataShareExtensionAbility {
  onCreateTest(want:Want, callback:AsyncCallback<void>): void
  {
    hilog.info(domain, tag, '%{public}s', '[ttt] [DataShareTest] <<Provider>> DataShareExtAbility onCreate, want:' + want.abilityName);
 
    hilog.info(domain, tag, '%{public}s', '[ttt] [DataShareTest] DataShareExtAbility onCreate this.context.databaseDir:' + this.context.databaseDir);
    hilog.info(domain, tag, '%{public}s', '[ttt] [DataShareTest] DataShareExtAbility onCreate this.context.area:' + this.context.area);
 
    rdbStore = await relationalStore.getRdbStore(this.context, STORE_CONFIG);
    await (rdbStore as relationalStore.RdbStore).execute(DROP_TBL, []);
    hilog.info(domain, tag, '%{public}s', '[ttt] [DataShareTest] <<Provider>> DataShareExtAbility getRdbStore done');
    await (rdbStore as relationalStore.RdbStore).execute(DDL_TBL_CREATE, []);
    hilog.info(domain, tag, '%{public}s', '[ttt] [DataShareTest] <<Provider>> DataShareExtAbility executeSql done');
    callback({'code': 0}, undefined);
    hilog.info(domain, tag, '%{public}s', '[ttt] [DataShareTest] <<Provider>> DataShareExtAbility onCreate end');
  }
 
  insertTest(uri:string, value:ValuesBucket, callback:AsyncCallback<int>): void
  {
    hilog.info(domain, tag, '%{public}s', '[ttt] [DataShareTest] <<Provider>> [insert] enter');
    if (value == null) {
      hilog.info(domain, tag, '%{public}s', '[ttt] [DataShareTest] <<Provider>> [insert] invalid valueBuckets');
      return;
    }
    hilog.info(domain, tag, '%{public}s', '[ttt] [DataShareTest] <<Provider>> [insert]  value = ' + value);
    hilog.info(domain, tag, '%{public}s', '[ttt] [DataShareTest] <<Provider>> [insert]  value = ' + JSON.stringify(value));
    let conflict = relationalStore.ConflictResolution.ON_CONFLICT_REPLACE;
    (rdbStore as relationalStore.RdbStore).insert(TBL_NAME, valueConvert(value), conflict).then(async (ret: long) => {
      hilog.info(domain, tag, '%{public}s', '[ttt] [DataShareTest] <<Provider>> [insert] callback ret:' + ret);
      if (callback != undefined) {
        callback({'code': 0}, ret as int);
      }
    }).catch((err) => {
      hilog.error(domain, tag, '%{public}s', `[ttt] [DataShareTest] <<Provider>> [insert] error: code: ${err.code}, message: ${err.message}`);
    });
    hilog.info(domain, tag, '%{public}s', '[ttt] [DataShareTest] <<Provider>> [insert] leave');
  }
 
  updateTest(uri:string, predicates:dataSharePredicates.DataSharePredicates, value:ValuesBucket, callback:AsyncCallback<int>): void
  {
    hilog.info(domain, tag, '%{public}s', '[ttt] [DataShareTest] <<Provider>> [update] enter');
    if (predicates == null || predicates == undefined) {
      hilog.info(domain, tag, '%{public}s', '[ttt] [DataShareTest] <<Provider>> [update] invalid predicates');
      return;
    }
    hilog.info(domain, tag, '%{public}s', '[ttt] [DataShareTest] <<Provider>> [update]  values = ' + value);
    hilog.info(domain, tag, '%{public}s', '[ttt] [DataShareTest] <<Provider>> [update]  values = ' + JSON.stringify(value));
    hilog.info(domain, tag, '%{public}s', '[ttt] [DataShareTest] <<Provider>> [update]  predicates = ' + predicates);
    hilog.info(domain, tag, '%{public}s', '[ttt] [DataShareTest] <<Provider>> [update]  predicates = ' + JSON.stringify(predicates));
    (rdbStore as relationalStore.RdbStore).update(TBL_NAME, valueConvert(value), predicates).then(async (ret: long)  => {
      hilog.info(domain, tag, '%{public}s', '[ttt] [DataShareTest] <<Provider>> [update] callback ret:' + ret);
      if (callback != undefined) {
        callback({'code': 0}, ret as int);
      }
    }).catch((err) => {
      hilog.error(domain, tag, '%{public}s', `[ttt] [DataShareTest] <<Provider>> [update] error: code: ${err.code}, message: ${err.message}`);
    });
    hilog.info(domain, tag, '%{public}s', '[ttt] [DataShareTest] <<Provider>> [update] leave');
  }
 
  deleteTest(uri:string, predicates:dataSharePredicates.DataSharePredicates, callback:AsyncCallback<int>): void
  {
    hilog.info(domain, tag, '%{public}s', '[ttt] [DataShareTest] <<Provider>> [delete] enter');
    if (predicates == null || predicates == undefined) {
      hilog.info(domain, tag, '%{public}s', '[ttt] [DataShareTest] <<Provider>> [delete] invalid predicates');
      return;
    }
    hilog.info(domain, tag, '%{public}s', '[ttt] [DataShareTest] <<Provider>> [delete]  predicates = ' + predicates);
    hilog.info(domain, tag, '%{public}s', '[ttt] [DataShareTest] <<Provider>> [delete]  predicates = ' + JSON.stringify(predicates));
    (rdbStore as relationalStore.RdbStore).delete(TBL_NAME, predicates).then(async (ret: long) => {
      hilog.info(domain, tag, '%{public}s', '[ttt] [DataShareTest] <<Provider>> [delete] ret:' + ret);
      if (callback != undefined) {
        callback({'code': 0}, ret as int);
      }
      // let rdbPredicate = new relationalStore.RdbPredicates('TBL00');
      // rdbPredicate.inAllDevices();
      // hilog.info(domain, tag, '%{public}s', "[ttt] [DataShareTest] <<Provider>> [sync] begin");
      // (rdbStore as relationalStore.RdbStore).sync(relationalStore.SyncMode.SYNC_MODE_PUSH, rdbPredicate).then((data) => {
      //   hilog.info(domain, tag, '%{public}s', "[ttt] [DataShareTest] <<Provider>> [sync] data = " + JSON.stringify(data));
      // }).catch((err) => {
      //   hilog.error(domain, tag, '%{public}s', `[ttt] [DataShareTest] <<Provider>> [sync] error: code: ${err.code}, message: ${err.message}`);
      // });
    }).catch((err) => {
      hilog.error(domain, tag, '%{public}s', `[ttt] [DataShareTest] <<Provider>> [delete] error: code: ${err.code}, message: ${err.message}`);
    });
    hilog.info(domain, tag, '%{public}s', '[ttt] [DataShareTest] <<Provider>> [delete] leave');
  }
 
  queryTest(uri:string, predicates:dataSharePredicates.DataSharePredicates, columns:Array<string>,  callback:AsyncCallback<relationalStore.ResultSet>): void
  {
    hilog.info(domain, tag, '%{public}s', '[ttt] [DataShareTest] <<Provider>> [query] enter');
    if (predicates == null || predicates == undefined) {
      hilog.info(domain, tag, '%{public}s', '[ttt] [DataShareTest] <<Provider>> [query] invalid predicates');
    }
    hilog.info(domain, tag, '%{public}s', "[ttt] [DataShareTest] <<Provider>> [query] local query begin " );
    (rdbStore as relationalStore.RdbStore).query(TBL_NAME, predicates, columns).then(async (resultSet: relationalStore.ResultSet) => {
      hilog.info(domain, tag, '%{public}s', '[ttt] [DataShareTest] <<Provider>> [query] local query ret: ' + resultSet);
      if (resultSet != undefined) {
        // hilog.info(domain, tag, '%{public}s', '[ttt] [DataShareTest] <<Provider>> [query] local query resultSet.rowCount: ' + resultSet.rowCount);
        hilog.info(domain, tag, '%{public}s', "[ttt] [DataShareTest] <<Provider>> [query] local query end ");
      }
      if (callback != undefined) {
        callback({'code': 0}, resultSet);
      }
    }).catch((err) => {
      hilog.error(domain, tag, '%{public}s', `[ttt] [DataShareTest] <<Provider>> [query] error: code: ${err.code}, message: ${err.message}`);
    });
    hilog.info(domain, tag, '%{public}s', '[ttt] [DataShareTest] <<Provider>> [query] leave');
  }
 
  constructor()
  {
    this.onCreate = this.onCreateTest;
    this.insert = this.insertTest;
    this.update = this.updateTest;
    this.delete = this.deleteTest;
    this.query = this.queryTest;
    hilog.error(domain, tag, '%{public}s', `[ttt] <<Provider>> DataShareExtAbility constructor end.`);
  }
}