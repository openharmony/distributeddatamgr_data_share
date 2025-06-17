
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

import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect, Level, Size, TestType } from "@ohos/hypium";
import relationalStore from "@ohos.data.relationalStore";
import dataSharePredicates from "@ohos.data.dataSharePredicates";
import common from '@ohos.app.ability.common';
import AbilityDelegatorRegistry from '@ohos.app.ability.abilityDelegatorRegistry';
import { BusinessError } from "@ohos.base";


const STORE_CONFIG: relationalStore.StoreConfig = {
  name: "rdbStore.db",
  securityLevel: relationalStore.SecurityLevel.S1
}
let rdbStore:relationalStore.RdbStore | undefined = undefined;
let table_name = "test";
let DROP_TBL = "DROP TABLE IF EXISTS " + table_name;
let CREATE_TABLE_TEST = "CREATE TABLE IF NOT EXISTS "
  + table_name
  + " (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT NOT NULL, age INTEGER, isStudent BOOLEAN)";
let TAG = "[dataSharePredicate_test]";

const delegator = AbilityDelegatorRegistry.getAbilityDelegator();
const context = delegator.getAppContext().getApplicationContext();

async function generateTable() {
  console.info(TAG + 'generateTable')
  if (rdbStore != undefined) {
    await rdbStore.executeSql(CREATE_TABLE_TEST);
    const valueBucket1: relationalStore.ValuesBucket = {
      name: "ZhangSan",
      age: 20,
      'isStudent': true,
    }
    await rdbStore.insert("test", valueBucket1)
    const valueBucket2: relationalStore.ValuesBucket = {
      name: "LiSi",
      age: 21,
      'isStudent': true,
    }
    await rdbStore.insert("test", valueBucket2)
    const valueBucket3: relationalStore.ValuesBucket = {
      name: "WangWu",
      age: 22,
      'isStudent': false,
    }
    await rdbStore.insert("test", valueBucket3)
    const valueBucket4: relationalStore.ValuesBucket = {
      name: "SunLiu",
      age: 23,
      'isStudent': false,
    }
    await rdbStore.insert("test", valueBucket4)
    const valueBucket5: relationalStore.ValuesBucket = {
      name: "MaQi",
      age: 24,
      'isStudent': true,
    }
    await rdbStore.insert("test", valueBucket5)
    console.info(TAG + 'generateTable end')
  }
}

async function insertMoreData() {
  console.info(TAG + 'insertMoreData')
  if (rdbStore != undefined) {
    const valueBucket1: relationalStore.ValuesBucket = {
      name: "Steven",
      age: 17,
      'isStudent': true,
    }
    await rdbStore.insert("test", valueBucket1)
    const valueBucket2: relationalStore.ValuesBucket = {
      name: "Jax",
      age: 17,
      'isStudent': true,
    }
    await rdbStore.insert("test", valueBucket2)
    const valueBucket3: relationalStore.ValuesBucket = {
      name: "Green",
      age: 17,
      'isStudent': false,
    }
    await rdbStore.insert("test", valueBucket3)
    const valueBucket4: relationalStore.ValuesBucket = {
      name: "List",
      age: 14,
      'isStudent': false,
    }
    await rdbStore.insert("test", valueBucket4)
    const valueBucket5: relationalStore.ValuesBucket = {
      name: "Alice",
      age: 24,
      'isStudent': true,
    }
    await rdbStore.insert("test", valueBucket5)
    const valueBucket6: relationalStore.ValuesBucket = {
      name: "Chary",
      age: 24,
      'isStudent': true,
    }
    await rdbStore.insert("test", valueBucket6)
    console.info(TAG + 'insertMoreData end')
  }
}

export default function DataSharePredicatesEtsTest() {
  describe('DataSharePredicatesEtsTest', () => {
    beforeAll(async () => {
      console.info(TAG + 'beforeAll');
      await relationalStore.getRdbStore(context, STORE_CONFIG).then(async (store: relationalStore.RdbStore) => {
        rdbStore = store;
        console.info('ttt Get RdbStore successfully.')
      }).catch((err: BusinessError) => {
        console.error(`ttt Get RdbStore failed, code is ${err.code},message is ${err.message}`);
      })
    });
    beforeEach(async () => {
      console.info(TAG + 'beforeEach');
      console.info(TAG + 'createTest data start');
      await generateTable();
      console.info(TAG + 'createTest data end');
    })
    afterEach(async () => {
      console.info(TAG + 'afterEach');
      if (rdbStore != undefined) {
        await rdbStore.executeSql(DROP_TBL);
      }
    })
    afterAll(async () => {
      console.info(TAG + 'afterAll');
      relationalStore.deleteRdbStore(context, STORE_CONFIG.name).then(() => {
        rdbStore = undefined;
        console.info(TAG + 'Delete RdbStore successfully.');
      }).catch((err: BusinessError) => {
        console.error(TAG + `Delete RdbStore failed, code is ${err.code},message is ${err.message}`);
      })
    });



    it('testDataSharePredicate_0001',  0, async () => {
      console.log(TAG + "************* testDataSharePredicate_0001 start *************");
      const valueBucket: relationalStore.ValuesBucket = {
        'name': 'Steven',
        'age': 28,
        'isStudent': false,
      };
      await rdbStore?.insert(table_name, valueBucket);
      try {
        let dp = new dataSharePredicates.DataSharePredicates();
        dp.equalTo("age", 28);
        let resultSets = await (rdbStore as relationalStore.RdbStore).query(table_name, dp, ["*"]);
        console.log(TAG + "rowCount is : " + resultSets.rowCount);
        expect(resultSets.goToFirstRow()).assertEqual(true);
        const id = resultSets.getLong(resultSets.getColumnIndex("id"));
        const name = resultSets.getString(resultSets.getColumnIndex("name"));
        const age = resultSets.getLong(resultSets.getColumnIndex("age"));
        const isStudent = resultSets.getValue(resultSets.getColumnIndex("isStudent"));
        expect(id).assertEqual(6);
        expect(name).assertEqual("Steven");
        expect(age).assertEqual(28);
        expect(isStudent).assertEqual(0);

        resultSets.close();
      } catch (err) {
        console.error(TAG + `Query failed, code is ${err.code},message is ${err.message}`);
      }

      console.log(TAG + "************* testDataSharePredicate_0001 end *************");
    })

    it('testDataSharePredicate_0002',  0, async () => {
      console.log(TAG + "************* testDataSharePredicate_0002 start *************");
      const valueBucket: relationalStore.ValuesBucket = {
        'name': 'Steven',
        'age': 20,
        'isStudent': false,
      };
      let rowId = await rdbStore?.insert(table_name, valueBucket);
      try {
        let dp = new dataSharePredicates.DataSharePredicates();
        dp.equalTo("age", 20).orderByAsc("name");
        let resultSets = await (rdbStore as relationalStore.RdbStore).query(table_name, dp, ["*"]);

        expect(resultSets.rowCount).assertEqual(2);
        expect(resultSets.goToRow(0)).assertEqual(true);
        let id = resultSets.getLong(resultSets.getColumnIndex("id"));
        let name = resultSets.getString(resultSets.getColumnIndex("name"));
        let age = resultSets.getLong(resultSets.getColumnIndex("age"));
        let isStudent = resultSets.getValue(resultSets.getColumnIndex("isStudent"));
        expect(id).assertEqual(6);
        expect(name).assertEqual("Steven");
        expect(age).assertEqual(20);
        expect(isStudent).assertEqual(0);
        expect(resultSets.goToNextRow()).assertEqual(true);
        id = resultSets.getLong(resultSets.getColumnIndex("id"));
        name = resultSets.getString(resultSets.getColumnIndex("name"));
        age = resultSets.getLong(resultSets.getColumnIndex("age"));
        isStudent = resultSets.getValue(resultSets.getColumnIndex("isStudent"));
        expect(id).assertEqual(1);
        expect(name).assertEqual("ZhangSan");
        expect(age).assertEqual(20);
        expect(isStudent).assertEqual(1);
        resultSets.close();
      } catch (err) {
        console.error(TAG + `Query failed, code is ${err.code},message is ${err.message}`);
      }

      console.log(TAG + "************* testDataSharePredicate_0002 end *************");
    })

    it('testDataSharePredicate_0003',  0, async () => {
      console.log(TAG + "************* testDataSharePredicate_0003 start *************");
      let dp = new dataSharePredicates.DataSharePredicates();
      dp.equalTo("name", "LiSi").and();
      const valueBucket: relationalStore.ValuesBucket = {
        'name': 'Steven',
        'age': 20,
        'isStudent': false,
      };
      const valueBucket1: relationalStore.ValuesBucket = {
        'name': 'Steven',
        'age': 24,
        'isStudent': true,
      };
      if (rdbStore != undefined) {
        await rdbStore.update(table_name, valueBucket, dp);
        await rdbStore.insert(table_name, valueBucket1);
      }

      try {
        dp = new dataSharePredicates.DataSharePredicates();
        dp.equalTo("name", "Steven").and().equalTo("age", 24);
        let resultSets = await (rdbStore as relationalStore.RdbStore).query(table_name, dp, ["*"]);

        expect(resultSets.rowCount).assertEqual(1);
        expect(resultSets.goToRow(0)).assertEqual(true);
        let id = resultSets.getLong(resultSets.getColumnIndex("id"));
        let name = resultSets.getString(resultSets.getColumnIndex("name"));
        let age = resultSets.getLong(resultSets.getColumnIndex("age"));
        let isStudent = resultSets.getValue(resultSets.getColumnIndex("isStudent"));
        expect(id).assertEqual(6);
        expect(name).assertEqual("Steven");
        expect(age).assertEqual(24);
        expect(isStudent).assertEqual(1);
      } catch (err) {
        console.error(TAG + `Query failed, code is ${err.code},message is ${err.message}`);
      }

      console.log(TAG + "************* testDataSharePredicate_0003 end *************");
    })

    it('testDataSharePredicate_0004',  0, async () => {
      console.log(TAG + "************* testDataSharePredicate_0004 start *************");
      try {
        let dp = new dataSharePredicates.DataSharePredicates();
        dp.in("isStudent", [true, false]).limit(0, -1);
        let resultSets = await (rdbStore as relationalStore.RdbStore).query(table_name, dp, ["*"]);

        expect(resultSets.rowCount).assertEqual(5);
      } catch (err) {
        console.error(TAG + `Query failed, code is ${err.code},message is ${err.message}`);
      }

      console.log(TAG + "************* testDataSharePredicate_0004 end *************");
    })

    it('testDataSharePredicate_0005',  0, async () => {
      console.log(TAG + "************* testDataSharePredicate_0005 start *************");
      if (rdbStore != undefined) {
        await insertMoreData;
      }
      try {
        let dp = new dataSharePredicates.DataSharePredicates();
        dp.limit(1, -1);
        let resultSets = await (rdbStore as relationalStore.RdbStore).query(table_name, dp, ["*"]);
        expect(resultSets.rowCount).assertEqual(1);
        expect(resultSets.goToRow(0)).assertEqual(true);
        let id = resultSets.getLong(resultSets.getColumnIndex("id"));
        let name = resultSets.getString(resultSets.getColumnIndex("name"));
        let age = resultSets.getLong(resultSets.getColumnIndex("age"));
        let isStudent = resultSets.getValue(resultSets.getColumnIndex("isStudent"));
        expect(id).assertEqual(1);
        expect(name).assertEqual("ZhangSan");
        expect(age).assertEqual(20);
        expect(isStudent).assertEqual(1);
      } catch (err) {
        console.error(TAG + `Query failed, code is ${err.code},message is ${err.message}`);
      }

      console.log(TAG + "************* testDataSharePredicate_0005 end *************");
    })

    it('testDataSharePredicate_0006',  0, async () => {
      console.log(TAG + "************* testDataSharePredicate_0006 start *************");
      if (rdbStore != undefined) {
        await insertMoreData;
      }
      try {
        let dp = new dataSharePredicates.DataSharePredicates();
        dp.limit(1, 3);
        let resultSets = await (rdbStore as relationalStore.RdbStore).query(table_name, dp, ["*"]);
        expect(resultSets.rowCount).assertEqual(1);
        expect(resultSets.goToRow(0)).assertEqual(true);
        let id = resultSets.getLong(resultSets.getColumnIndex("id"));
        let name = resultSets.getString(resultSets.getColumnIndex("name"));
        let age = resultSets.getLong(resultSets.getColumnIndex("age"));
        let isStudent = resultSets.getValue(resultSets.getColumnIndex("isStudent"));
        expect(id).assertEqual(4);
        expect(name).assertEqual("SunLiu");
        expect(age).assertEqual(23);
        expect(isStudent).assertEqual(0);
      } catch (err) {
        console.error(TAG + `Query failed, code is ${err.code},message is ${err.message}`);
      }

      console.log(TAG + "************* testDataSharePredicate_0006 end *************");
    })

    it('testDataSharePredicate_0007',  0, async () => {
      console.log(TAG + "************* testDataSharePredicate_0007 start *************");
      if (rdbStore != undefined) {
        await insertMoreData;
      }
      try {
        let dp = new dataSharePredicates.DataSharePredicates();
        dp.limit(1, 12);
        let resultSets = await (rdbStore as relationalStore.RdbStore).query(table_name, dp, ["*"]);
        expect(resultSets.rowCount).assertEqual(0);
      } catch (err) {
        console.error(TAG + `Query failed, code is ${err.code},message is ${err.message}`);
      }

      console.log(TAG + "************* testDataSharePredicate_0007 end *************");
    })
  })
}