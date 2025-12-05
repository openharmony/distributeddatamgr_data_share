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
};
let rdbStore: relationalStore.RdbStore | undefined = undefined;
let table_name = "test";
let DROP_TBL = "DROP TABLE IF EXISTS " + table_name;
let CREATE_TABLE_TEST = "CREATE TABLE IF NOT EXISTS "
  + table_name
  + " (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT NOT NULL, age INTEGER, isStudent BOOLEAN)";
let TAG = "[dataSharePredicateRDB_test]";

const delegator = AbilityDelegatorRegistry.getAbilityDelegator();
const context = delegator.getAppContext().getApplicationContext();

async function generateTable() {
  console.info(TAG + 'generateTable');
  if (rdbStore != undefined) {
    await rdbStore.executeSql(CREATE_TABLE_TEST);
    const valueBucket1: relationalStore.ValuesBucket = {
      name: "ZhangSan",
      age: 20,
      'isStudent': true,
    };
    await rdbStore.insert("test", valueBucket1);
    const valueBucket2: relationalStore.ValuesBucket = {
      name: "LiSi",
      age: 21,
      'isStudent': true,
    };
    await rdbStore.insert("test", valueBucket2);
    const valueBucket3: relationalStore.ValuesBucket = {
      name: "WangWu",
      age: 22,
      'isStudent': false,
    };
    await rdbStore.insert("test", valueBucket3);
    const valueBucket4: relationalStore.ValuesBucket = {
      name: "SunLiu",
      age: 23,
      'isStudent': false,
    };
    await rdbStore.insert("test", valueBucket4);
    const valueBucket5: relationalStore.ValuesBucket = {
      name: "MaQi",
      age: 24,
      'isStudent': true,
    };
    await rdbStore.insert("test", valueBucket5);
    console.info(TAG + 'generateTable end');
  }
}

async function insertMoreData() {
  console.info(TAG + 'insertMoreData');
  if (rdbStore != undefined) {
    const valueBucket1: relationalStore.ValuesBucket = {
      name: "Steven",
      age: 17,
      'isStudent': true,
    };
    await rdbStore.insert("test", valueBucket1);
    const valueBucket2: relationalStore.ValuesBucket = {
      name: "Jax",
      age: 17,
      'isStudent': true,
    };
    await rdbStore.insert("test", valueBucket2);
    const valueBucket3: relationalStore.ValuesBucket = {
      name: "Green",
      age: 17,
      'isStudent': false,
    };
    await rdbStore.insert("test", valueBucket3);
    const valueBucket4: relationalStore.ValuesBucket = {
      name: "List",
      age: 14,
      'isStudent': false,
    };
    await rdbStore.insert("test", valueBucket4);
    const valueBucket5: relationalStore.ValuesBucket = {
      name: "Alice",
      age: 24,
      'isStudent': true,
    };
    await rdbStore.insert("test", valueBucket5);
    const valueBucket6: relationalStore.ValuesBucket = {
      name: "Chary",
      age: 24,
      'isStudent': true,
    };
    await rdbStore.insert("test", valueBucket6);
    console.info(TAG + 'insertMoreData end');
  }
}

export default function DataSharePredicatesRDBEtsTest() {
  describe('DataSharePredicatesRDBEtsTest', () => {

    beforeAll(async () => {
      console.info(TAG + 'beforeAll');
      await relationalStore.getRdbStore(context, STORE_CONFIG).then(async (store: relationalStore.RdbStore) => {
        rdbStore = store;
        console.info('ttt Get RdbStore successfully.');
      }).catch((err: BusinessError) => {
        console.error(`ttt Get RdbStore failed, code is ${err.code},message is ${err.message}`);
      });
    });

    beforeEach(async () => {
      console.info(TAG + 'beforeEach');
      console.info(TAG + 'createTest data start');
      await generateTable();
      console.info(TAG + 'createTest data end');
    });

    afterEach(async () => {
      console.info(TAG + 'afterEach');
      if (rdbStore != undefined) {
        await rdbStore.executeSql(DROP_TBL);
      }
    });

    afterAll(async () => {
      console.info(TAG + 'afterAll');
      relationalStore.deleteRdbStore(context, STORE_CONFIG.name).then(() => {
        rdbStore = undefined;
        console.info(TAG + 'Delete RdbStore successfully.');
      }).catch((err: BusinessError) => {
        console.error(TAG + `Delete RdbStore failed, code is ${err.code},message is ${err.message}`);
      });
    });

    /**
     * @tc.name: testDataSharePredicate_0001
     * @tc.desc: Verify the func of DataSharePredicates equalTo
     * @tc.type: FUNC
     * @tc.expect:
     *   1. The resultSet should contain 1 row with the correct data
     */
    it('testDataSharePredicate_0001', 0, async () => {
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
        if (rdbStore != undefined) {
          await rdbStore.executeSql(DROP_TBL);
        }
        expect().assertFail();
      }

      console.log(TAG + "************* testDataSharePredicate_0001 end *************");
    });

    /**
     * @tc.name: testDataSharePredicate_0002
     * @tc.desc: Verify the func of DataSharePredicates equalTo and orderByAsc
     * @tc.type: FUNC
     * @tc.expect:
     *   1. The resultSet should contain 2 rows with the correct data and ordered by name
     */
    it('testDataSharePredicate_0002', 0, async () => {
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
        if (rdbStore != undefined) {
          await rdbStore.executeSql(DROP_TBL);
        }
        expect().assertFail();
      }

      console.log(TAG + "************* testDataSharePredicate_0002 end *************");
    });

    /**
     * @tc.name: testDataSharePredicate_0003
     * @tc.desc: Verify the func of DataSharePredicates equalTo and AND
     * @tc.type: FUNC
     * @tc.expect:
     *   1. The resultSet should contain 1 row with the correct data
     */
    it('testDataSharePredicate_0003', 0, async () => {
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
        resultSets.close();
      } catch (err) {
        console.error(TAG + `Query failed, code is ${err.code},message is ${err.message}`);
        if (rdbStore != undefined) {
          await rdbStore.executeSql(DROP_TBL);
        }
        expect().assertFail();
      }

      console.log(TAG + "************* testDataSharePredicate_0003 end *************");
    });

    /**
     * @tc.name: testDataSharePredicate_0004
     * @tc.desc: Verify the func of DataSharePredicates in and limit
     * @tc.type: FUNC
     * @tc.expect:
     *   1. The resultSet should contain 5 rows
     */
    it('testDataSharePredicate_0004', 0, async () => {
      console.log(TAG + "************* testDataSharePredicate_0004 start *************");
      try {
        let dp = new dataSharePredicates.DataSharePredicates();
        dp.in("isStudent", [true, false]).limit(0, -1);
        let resultSets = await (rdbStore as relationalStore.RdbStore).query(table_name, dp, ["*"]);

        expect(resultSets.rowCount).assertEqual(5);
        resultSets.close();
      } catch (err) {
        console.error(TAG + `Query failed, code is ${err.code},message is ${err.message}`);
        if (rdbStore != undefined) {
          await rdbStore.executeSql(DROP_TBL);
        }
        expect().assertFail();
      }

      console.log(TAG + "************* testDataSharePredicate_0004 end *************");
    });

    /**
     * @tc.name: testDataSharePredicate_0005
     * @tc.desc: Verify the func of DataSharePredicates limit with offset
     * @tc.type: FUNC
     * @tc.expect:
     *   1. The resultSet should contain 1 row with the correct data
     */
    it('testDataSharePredicate_0005', 0, async () => {
      console.log(TAG + "************* testDataSharePredicate_0005 start *************");
      if (rdbStore != undefined) {
        await insertMoreData();
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
        resultSets.close();
      } catch (err) {
        console.error(TAG + `Query failed, code is ${err.code},message is ${err.message}`);
        if (rdbStore != undefined) {
          await rdbStore.executeSql(DROP_TBL);
        }
        expect().assertFail();
      }

      console.log(TAG + "************* testDataSharePredicate_0005 end *************");
    });

    /**
     * @tc.name: testDataSharePredicate_0006
     * @tc.desc: Verify the func of DataSharePredicates limit with specific offset
     * @tc.type: FUNC
     * @tc.expect:
     *   1. The resultSet should contain 1 row with the correct data
     */
    it('testDataSharePredicate_0006', 0, async () => {
      console.log(TAG + "************* testDataSharePredicate_0006 start *************");
      if (rdbStore != undefined) {
        await insertMoreData();
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
        resultSets.close();
      } catch (err) {
        console.error(TAG + `Query failed, code is ${err.code},message is ${err.message}`);
        if (rdbStore != undefined) {
          await rdbStore.executeSql(DROP_TBL);
        }
        expect().assertFail();
      }

      console.log(TAG + "************* testDataSharePredicate_0006 end *************");
    });

    /**
     * @tc.name: testDataSharePredicate_0007
     * @tc.desc: Verify the func of DataSharePredicates limit exceeding record count
     * @tc.type: FUNC
     * @tc.expect:
     *   1. The resultSet should be empty
     */
    it('testDataSharePredicate_0007', 0, async () => {
      console.log(TAG + "************* testDataSharePredicate_0007 start *************");
      if (rdbStore != undefined) {
        await insertMoreData();
      }
      try {
        let dp = new dataSharePredicates.DataSharePredicates();
        dp.limit(1, 12);
        let resultSets = await (rdbStore as relationalStore.RdbStore).query(table_name, dp, ["*"]);
        expect(resultSets.rowCount).assertEqual(0);
        resultSets.close();
      } catch (err) {
        console.error(TAG + `Query failed, code is ${err.code},message is ${err.message}`);
        if (rdbStore != undefined) {
          await rdbStore.executeSql(DROP_TBL);
        }
        expect().assertFail();
      }

      console.log(TAG + "************* testDataSharePredicate_0007 end *************");
    });

    /**
     * @tc.name: testDataSharePredicate_0008
     * @tc.desc: Verify the behavior of the notEqualTo in DataSharePredicates with boolean field
     * @tc.type: FUNC
     * @tc.precon: DataSharePredicates instance is created and table is populated with test data
     * @tc.step:
     *   1. Create a DataSharePredicates instance and set notEqualTo condition on isStudent field
     *   2. Query the database with the predicates and verify the result set
     *   3. Check the row count and column values for the returned result set
     * @tc.expect:
     *   1. Correct predicates setup with notEqualTo condition
     *   2. Correct query execution and result set retrieval
     *   3. Correct row count (2) and column values (id=3, name="WangWu", age=22, isStudent=0)
     */
    it('testDataSharePredicate_0008', 0, async () => {
      console.log(TAG + "************* testDataSharePredicate_0008 start *************");
      try {
        let dp = new dataSharePredicates.DataSharePredicates();
        dp.notEqualTo('isStudent', true);
        let resultSets = await (rdbStore as relationalStore.RdbStore).query(table_name, dp, ["*"]);
        expect(resultSets.rowCount).assertEqual(2);
        expect(resultSets.goToRow(0)).assertEqual(true);
        let id = resultSets.getLong(resultSets.getColumnIndex("id"));
        let name = resultSets.getString(resultSets.getColumnIndex("name"));
        let age = resultSets.getLong(resultSets.getColumnIndex("age"));
        let isStudent = resultSets.getValue(resultSets.getColumnIndex("isStudent"));
        expect(id).assertEqual(3);
        expect(name).assertEqual("WangWu");
        expect(age).assertEqual(22);
        expect(isStudent).assertEqual(0);
        resultSets.close();
      } catch (err) {
        console.error(TAG + `Query failed, code is ${err.code},message is ${err.message}`);
        if (rdbStore != undefined) {
          await rdbStore.executeSql(DROP_TBL);
        }
        expect().assertFail();
      }
      console.log(TAG + "************* testDataSharePredicate_0008 end *************");
    });

    /**
     * @tc.name: testDataSharePredicate_0009
     * @tc.desc: Verify the behavior of complex predicates with notEqualTo, beginWrap, or, and endWrap in DataSharePredicates
     * @tc.type: FUNC
     * @tc.precon: DataSharePredicates instance is created and table is populated with test data
     * @tc.step:
     *   1. Insert additional test data into the database
     *   2. Create a DataSharePredicates instance and set complex predicates conditions:
     *      - notEqualTo on age field
     *      - beginWrap with equalTo on age field and or with equalTo on name field
     *      - endWrap
     *   3. Query the database with the predicates and verify the result set
     *   4. Check the row count and column values for the returned result set
     * @tc.expect:
     *   1. Additional data inserted successfully
     *   2. Correct predicates setup with complex conditions
     *   3. Correct query execution and result set retrieval
     *   4. Correct row count (4) and column values (id=5, name="MaQi", age=24, isStudent=1)
     */
    it('testDataSharePredicate_0009', 0, async () => {
      console.log(TAG + "************* testDataSharePredicate_0009 start *************");
      if (rdbStore != undefined) {
        await insertMoreData();
      }
      try {
        let dp = new dataSharePredicates.DataSharePredicates();
        dp.notEqualTo('age', 14)
          .beginWrap()
          .equalTo('age', 17)
          .or()
          .equalTo('name', 'MaQi')
          .endWrap();
        let resultSets = await (rdbStore as relationalStore.RdbStore).query(table_name, dp, ["*"]);
        expect(resultSets.rowCount).assertEqual(4);
        expect(resultSets.goToRow(0)).assertEqual(true);
        let id = resultSets.getLong(resultSets.getColumnIndex("id"));
        let name = resultSets.getString(resultSets.getColumnIndex("name"));
        let age = resultSets.getLong(resultSets.getColumnIndex("age"));
        let isStudent = resultSets.getValue(resultSets.getColumnIndex("isStudent"));
        expect(id).assertEqual(5);
        expect(name).assertEqual("MaQi");
        expect(age).assertEqual(24);
        expect(isStudent).assertEqual(1);
        resultSets.close();
      } catch (err) {
        console.error(TAG + `Query failed, code is ${err.code},message is ${err.message}`);
        if (rdbStore != undefined) {
          await rdbStore.executeSql(DROP_TBL);
        }
        expect().assertFail();
      }
      console.log(TAG + "************* testDataSharePredicate_0009 end *************");
    });

    /**
     * @tc.name: testDataSharePredicate_0010
     * @tc.desc: Verify the behavior of the contains in DataSharePredicates with string field
     * @tc.type: FUNC
     * @tc.precon: DataSharePredicates instance is created and table is populated with test data
     * @tc.step:
     *   1. Insert additional test data into the database
     *   2. Create a DataSharePredicates instance and set contains condition on name field
     *   3. Query the database with the predicates and verify the result set
     *   4. Check the row count and column values for the returned result set
     * @tc.expect:
     *   1. Additional data inserted successfully
     *   2. Correct predicates setup with contains condition
     *   3. Correct query execution and result set retrieval
     *   4. Correct row count (2) and column values (id=6, name="Steven", age=17, isStudent=1)
     */
    it('testDataSharePredicate_0010', 0, async () => {
      console.log(TAG + "************* testDataSharePredicate_0010 start *************");
      if (rdbStore != undefined) {
        await insertMoreData();
      }
      try {
        let dp = new dataSharePredicates.DataSharePredicates();
        dp.contains('name', 'en');
        let resultSets = await (rdbStore as relationalStore.RdbStore).query(table_name, dp, ["*"]);
        expect(resultSets.rowCount).assertEqual(2);
        expect(resultSets.goToRow(0)).assertEqual(true);
        let id = resultSets.getLong(resultSets.getColumnIndex("id"));
        let name = resultSets.getString(resultSets.getColumnIndex("name"));
        let age = resultSets.getLong(resultSets.getColumnIndex("age"));
        let isStudent = resultSets.getValue(resultSets.getColumnIndex("isStudent"));
        expect(id).assertEqual(6);
        expect(name).assertEqual('Steven');
        expect(age).assertEqual(17);
        expect(isStudent).assertEqual(1);
        resultSets.close();
      } catch (err) {
        console.error(TAG + `Query failed, code is ${err.code},message is ${err.message}`);
        if (rdbStore != undefined) {
          await rdbStore.executeSql(DROP_TBL);
        }
        expect().assertFail();
      }
      console.log(TAG + "************* testDataSharePredicate_0010 end *************");
    });

    /**
     * @tc.name: testDataSharePredicate_0011
     * @tc.desc: Verify the behavior of beginsWith and endsWith in DataSharePredicates with string field
     * @tc.type: FUNC
     * @tc.precon: DataSharePredicates instance is created and table is populated with test data
     * @tc.step:
     *   1. Insert additional test data into the database
     *   2. Create a DataSharePredicates instance and set conditions:
     *      - beginsWith on name field
     *      - or with endsWith on name field
     *   3. Query the database with the predicates and verify the result set
     *   4. Check the row count and column values for the returned result set
     * @tc.expect:
     *   1. Additional data inserted successfully
     *   2. Correct predicates setup with beginsWith and endsWith conditions
     *   3. Correct query execution and result set retrieval
     *   4. Correct row count (3) and column values (id=4, name="SunLiu", age=23, isStudent=0)
     */
    it('testDataSharePredicate_0011', 0, async () => {
      console.log(TAG + "************* testDataSharePredicate_0011 start *************");
      if (rdbStore != undefined) {
        await insertMoreData();
      }
      try {
        let dp = new dataSharePredicates.DataSharePredicates();
        dp.beginsWith('name', 'S')
          .or()
          .endsWith('name', 'en');
        let resultSets = await (rdbStore as relationalStore.RdbStore).query(table_name, dp, ["*"]);
        expect(resultSets.rowCount).assertEqual(3);
        expect(resultSets.goToRow(0)).assertTrue();
        let id = resultSets.getLong(resultSets.getColumnIndex("id"));
        let name = resultSets.getString(resultSets.getColumnIndex("name"));
        let age = resultSets.getLong(resultSets.getColumnIndex("age"));
        let isStudent = resultSets.getValue(resultSets.getColumnIndex("isStudent"));
        expect(id).assertEqual(4);
        expect(name).assertEqual("SunLiu");
        expect(age).assertEqual(23);
        expect(isStudent).assertEqual(0);
        resultSets.close();
      } catch (err) {
        console.error(TAG + `Query failed, code is ${err.code},message is ${err.message}`);
        if (rdbStore != undefined) {
          await rdbStore.executeSql(DROP_TBL);
        }
        expect().assertFail();
      }
      console.log(TAG + "************* testDataSharePredicate_0011 end *************");
    });

    /**
     * @tc.name: testDataSharePredicate_0012
     * @tc.desc: Test isNull and isNotNull predicates in DataSharePredicates
     * @tc.type: FUNC
     * @tc.precon: RdbStore is initialized and table exists
     * @tc.step:
     *   1. Insert row with null age
     *   2. Query using isNull('age') - expect 1 row with null age
     *   3. Query using isNotNull('age') - expect 5 rows with non-null age
     * @tc.expect:
     *   1. Row inserted successfully
     *   2. Correct result set for isNull query
     *   3. Correct result set for isNotNull query
     */
    it('testDataSharePredicate_0012', 0, async () => {
      console.log(TAG + "************* testDataSharePredicate_0012 start *************");
      const valueBucket: relationalStore.ValuesBucket = {
        'name': 'Steven',
        'age': null,
        'isStudent': false,
      };
      await rdbStore?.insert(table_name, valueBucket);
      try {
        console.info(TAG + "isNull start");
        let dp = new dataSharePredicates.DataSharePredicates();
        dp.isNull('age');
        let resultSets = await (rdbStore as relationalStore.RdbStore).query(table_name, dp, ["*"]);
        expect(resultSets.rowCount).assertEqual(1);
        expect(resultSets.goToRow(0)).assertTrue();
        let id = resultSets.getLong(resultSets.getColumnIndex("id"));
        let name = resultSets.getString(resultSets.getColumnIndex("name"));
        let age = resultSets.getLong(resultSets.getColumnIndex("age"));
        let isStudent = resultSets.getValue(resultSets.getColumnIndex("isStudent"));
        expect(id).assertEqual(6);
        expect(name).assertEqual("Steven");
        expect(age).assertEqual(0);
        expect(isStudent).assertEqual(0);
        resultSets.close();

        console.info(TAG + "isNotNull start");
        let dp1 = new dataSharePredicates.DataSharePredicates();
        dp1.isNotNull('age');
        let resultSets1 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp1, ["*"]);
        expect(resultSets1.rowCount).assertEqual(5);
        resultSets1.close();
      } catch (err) {
        console.error(TAG + `Query failed, code is ${err.code},message is ${err.message}`);
        if (rdbStore != undefined) {
          await rdbStore.executeSql(DROP_TBL);
        }
        expect().assertFail();
      }
      console.log(TAG + "************* testDataSharePredicate_0012 end *************");
    });

    /**
     * @tc.name: testDataSharePredicate_0013
     * @tc.desc: Verify like and unlike predicates in DataSharePredicates
     * @tc.type: FUNC
     * @tc.precon: RdbStore is initialized and table is populated
     * @tc.step:
     *   1. Insert additional test data
     *   2. Test like predicate with '%en%' pattern on name field - expect 2 matching rows
     *   3. Test unlike predicate with '%en%' pattern on name field - expect 9 non-matching rows
     * @tc.expect:
     *   1. Additional data inserted successfully
     *   2. Correct result set for like query (id=6)
     *   3. Correct result set for unlike query (first row name='ZhangSan')
     */
    it('testDataSharePredicate_0013', 0, async () => {
      console.log(TAG + "************* testDataSharePredicate_0013 start *************");
      if (rdbStore != undefined) {
        await insertMoreData();
      }
      try {
        console.info(TAG + "like start");
        let dp = new dataSharePredicates.DataSharePredicates();
        dp.like('name', '%en%');
        let resultSets = await (rdbStore as relationalStore.RdbStore).query(table_name, dp, ["*"]);
        expect(resultSets.rowCount).assertEqual(2);
        expect(resultSets.goToRow(0)).assertEqual(true);
        let id = resultSets.getLong(resultSets.getColumnIndex("id"));
        expect(id).assertEqual(6);
        resultSets.close();

        console.info(TAG + "unlike start");
        let dp1 = new dataSharePredicates.DataSharePredicates();
        dp1.unlike('name', '%en%');
        let resultSets1 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp1, ["*"]);
        expect(resultSets1.rowCount).assertEqual(9);
        expect(resultSets1.goToRow(0)).assertEqual(true);
        let name = resultSets1.getString(resultSets1.getColumnIndex("name"));
        expect(name).assertEqual('ZhangSan');
        resultSets1.close();
      } catch (err) {
        console.error(TAG + `Query failed, code is ${err.code},message is ${err.message}`);
        if (rdbStore != undefined) {
          await rdbStore.executeSql(DROP_TBL);
        }
        expect().assertFail();
      }
      console.log(TAG + "************* testDataSharePredicate_0013 end *************");
    });

    /**
     * @tc.name: testDataSharePredicate_0014
     * @tc.desc: Verify glob predicate in DataSharePredicates with wildcard pattern
     * @tc.type: FUNC
     * @tc.precon: RdbStore is initialized and table exists
     * @tc.step:
     *   1. Insert new test row with name='Seen'
     *   2. Create predicates with glob pattern '*n' on name field
     *   3. Query table and verify result set
     * @tc.expect:
     *   1. New row inserted successfully
     *   2. Correct predicates setup with glob pattern
     *   3. Correct result set with 2 matching rows (id=1)
     */
    it('testDataSharePredicate_0014', 0, async () => {
      console.log(TAG + "************* testDataSharePredicate_0014 start *************");
      const valueBucket: relationalStore.ValuesBucket = {
        'name': 'Seen',
        'age': 66,
        'isStudent': false,
      };
      await rdbStore?.insert(table_name, valueBucket);
      try {
        let dp = new dataSharePredicates.DataSharePredicates();
        dp.glob('name', '*n');
        let resultSets = await (rdbStore as relationalStore.RdbStore).query(table_name, dp, ["*"]);
        expect(resultSets.rowCount).assertEqual(2);
        expect(resultSets.goToRow(0)).assertEqual(true);
        let id = resultSets.getLong(resultSets.getColumnIndex("id"));
        expect(id).assertEqual(1);
        resultSets.close();
      } catch (err) {
        console.error(TAG + `Query failed, code is ${err.code},message is ${err.message}`);
        if (rdbStore != undefined) {
          await rdbStore.executeSql(DROP_TBL);
        }
        expect().assertFail();
      }
      console.log(TAG + "************* testDataSharePredicate_0014 end *************");
    });

    /**
     * @tc.name: testDataSharePredicate_0015
     * @tc.desc: Verify between and notBetween predicates in DataSharePredicates
     * @tc.type: FUNC
     * @tc.precon: RdbStore is initialized and table is populated
     * @tc.step:
     *   1. Insert additional test data
     *   2. Test between predicate on age field (20-23) - expect 4 matching rows
     *   3. Test notBetween predicate on age field (20-23) - expect 7 non-matching rows
     * @tc.expect:
     *   1. Additional data inserted successfully
     *   2. Correct result set for between query (first row id=1)
     *   3. Correct result set for notBetween query (first row id=5)
     */
    it('testDataSharePredicate_0015', 0, async () => {
      console.log(TAG + "************* testDataSharePredicate_0015 start *************");
      if (rdbStore != undefined) {
        await insertMoreData();
      }
      try {
        let dp = new dataSharePredicates.DataSharePredicates();
        dp.between('age', 20, 23);
        let resultSets = await (rdbStore as relationalStore.RdbStore).query(table_name, dp, ["*"]);
        expect(resultSets.rowCount).assertEqual(4);
        expect(resultSets.goToRow(0)).assertEqual(true);
        let id = resultSets.getLong(resultSets.getColumnIndex("id"));
        expect(id).assertEqual(1);
        resultSets.close();

        let dp1 = new dataSharePredicates.DataSharePredicates();
        dp1.notBetween('age', 20, 23);
        let resultSets1 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp1, ["*"]);
        expect(resultSets1.rowCount).assertEqual(7);
        expect(resultSets1.goToRow(0)).assertEqual(true);
        let id1 = resultSets1.getLong(resultSets1.getColumnIndex("id"));
        expect(id1).assertEqual(5);
        resultSets1.close();
      } catch (err) {
        console.error(TAG + `Query failed, code is ${err.code},message is ${err.message}`);
        if (rdbStore != undefined) {
          await rdbStore.executeSql(DROP_TBL);
        }
        expect().assertFail();
      }
      console.log(TAG + "************* testDataSharePredicate_0015 end *************");
    });

    /**
     * @tc.name: testDataSharePredicate_0016
     * @tc.desc: Verify greaterThan and lessThan predicates for different data types
     * @tc.type: FUNC
     * @tc.precon: RdbStore is initialized and table is populated
     * @tc.step:
     *   1. Insert additional test data
     *   2. Test greaterThan on numeric age field (age > 23) - expect 3 rows
     *   3. Test greaterThan on string name field (name > 'WangWu') - expect 1 row
     *   4. Test greaterThan on boolean isStudent field (isStudent > true) - expect 11 rows
     *   5. Test lessThan on numeric age field (age < 17) - expect 1 row
     *   6. Test lessThan on string name field (name < 'Chary') - expect 1 row
     * @tc.expect:
     *   1. Additional data inserted successfully
     *   2-6. Correct result set for each comparison type and data type
     */
    it('testDataSharePredicate_0016', 0, async () => {
      console.log(TAG + "************* testDataSharePredicate_0016 start *************");
      if (rdbStore != undefined) {
        await insertMoreData();
      }
      try {
        console.info(TAG + "greaterThan number start");
        let dp = new dataSharePredicates.DataSharePredicates();
        dp.greaterThan('age', 23);
        let resultSets = await (rdbStore as relationalStore.RdbStore).query(table_name, dp, ["*"]);
        expect(resultSets.rowCount).assertEqual(3);
        resultSets.close();

        console.info(TAG + "greaterThan string start");
        let dp1 = new dataSharePredicates.DataSharePredicates();
        dp1.greaterThan('name', 'WangWu');
        let resultSets1 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp1, ["*"]);
        expect(resultSets1.rowCount).assertEqual(1);
        resultSets1.close();

        console.info(TAG + "greaterThan bool start");
        let dp2 = new dataSharePredicates.DataSharePredicates();
        dp2.greaterThan('isStudent', true);
        let resultSets2 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp2, ["*"]);
        expect(resultSets2.rowCount).assertEqual(11);
        resultSets2.close();

        console.info(TAG + "lessThan number start");
        let dp3 = new dataSharePredicates.DataSharePredicates();
        dp3.lessThan('age', 17);
        let resultSets3 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp3, ["*"]);
        expect(resultSets3.rowCount).assertEqual(1);
        resultSets3.close();

        console.info(TAG + "lessThan string start");
        let dp4 = new dataSharePredicates.DataSharePredicates();
        dp4.lessThan('name', 'Chary');
        let resultSets4 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp4, ["*"]);
        expect(resultSets4.rowCount).assertEqual(1);
        resultSets4.close();
      } catch (err) {
        console.error(TAG + `Query failed, code is ${err.code},message is ${err.message}`);
        if (rdbStore != undefined) {
          await rdbStore.executeSql(DROP_TBL);
        }
        expect().assertFail();
      }
      console.log(TAG + "************* testDataSharePredicate_0016 end *************");
    });

    /**
     * @tc.name: testDataSharePredicate_0017
     * @tc.desc: Verify greaterThanOrEqualTo and lessThanOrEqualTo predicates for different data types
     * @tc.type: FUNC
     * @tc.precon: RdbStore is initialized and table is populated
     * @tc.step:
     *   1. Insert additional test data
     *   2. Test greaterThanOrEqualTo on numeric age field (age >= 23) - expect 4 rows
     *   3. Test greaterThanOrEqualTo on string name field (name >= 'WangWu') - expect 2 rows
     *   4. Test greaterThanOrEqualTo on boolean isStudent field (isStudent >= true) - expect 11 rows
     *   5. Test lessThanOrEqualTo on numeric age field (age <= 17) - expect 4 rows
     *   6. Test lessThanOrEqualTo on string name field (name <= 'Chary') - expect 2 rows
     * @tc.expect:
     *   1. Additional data inserted successfully
     *   2-6. Correct result set for each comparison type and data type
     */
    it('testDataSharePredicate_0017', 0, async () => {
      console.log(TAG + "************* testDataSharePredicate_0017 start *************");
      if (rdbStore != undefined) {
        await insertMoreData();
      }
      try {
        let dp = new dataSharePredicates.DataSharePredicates();
        dp.greaterThanOrEqualTo('age', 23);
        let resultSets = await (rdbStore as relationalStore.RdbStore).query(table_name, dp, ["*"]);
        expect(resultSets.rowCount).assertEqual(4);
        resultSets.close();

        let dp1 = new dataSharePredicates.DataSharePredicates();
        dp1.greaterThanOrEqualTo('name', 'WangWu');
        let resultSets1 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp1, ["*"]);
        expect(resultSets1.rowCount).assertEqual(2);
        resultSets1.close();

        let dp2 = new dataSharePredicates.DataSharePredicates();
        dp2.greaterThanOrEqualTo('isStudent', true);
        let resultSets2 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp2, ["*"]);
        expect(resultSets2.rowCount).assertEqual(11);
        resultSets2.close();

        let dp3 = new dataSharePredicates.DataSharePredicates();
        dp3.lessThanOrEqualTo('age', 17);
        let resultSets3 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp3, ["*"]);
        expect(resultSets3.rowCount).assertEqual(4);
        resultSets3.close();

        let dp4 = new dataSharePredicates.DataSharePredicates();
        dp4.lessThanOrEqualTo('name', 'Chary');
        let resultSets4 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp4, ["*"]);
        expect(resultSets4.rowCount).assertEqual(2);
        resultSets4.close();
      } catch (err) {
        console.error(TAG + `Query failed, code is ${err.code},message is ${err.message}`);
        if (rdbStore != undefined) {
          await rdbStore.executeSql(DROP_TBL);
        }
        expect().assertFail();
      }
      console.log(TAG + "************* testDataSharePredicate_0017 end *************");
    });

    /**
     * @tc.name: testDataSharePredicate_0018
     * @tc.desc: Verify distinct() method in combination with greaterThanOrEqualTo predicate
     * @tc.type: FUNC
     * @tc.precon: RdbStore is initialized and table is populated
     * @tc.step:
     *   1. Insert additional test data twice
     *   2. Create predicates with:
     *      - greaterThanOrEqualTo on age field (age >= 23)
     *      - distinct() to remove duplicate rows
     *   3. Query table and verify result set
     * @tc.expect:
     *   1. Additional data inserted successfully
     *   2. Correct predicates setup with distinct
     *   3. Correct result set with 6 unique rows (since id is autoincrement, no duplicates expected)
     */
    it('testDataSharePredicate_0018', 0, async () => {
      console.log(TAG + "************* testDataSharePredicate_0018 start *************");
      if (rdbStore != undefined) {
        await insertMoreData();
        await insertMoreData();
      }
      try {
        let dp = new dataSharePredicates.DataSharePredicates();
        dp.greaterThanOrEqualTo('age', 23)
          .distinct();
        let resultSets = await (rdbStore as relationalStore.RdbStore).query(table_name, dp, ["*"]);
        // id INTEGER PRIMARY KEY AUTOINCREMENT, so records are not duplicate
        expect(resultSets.rowCount).assertEqual(6);
        resultSets.close();
      } catch (err) {
        console.error(TAG + `Query failed, code is ${err.code},message is ${err.message}`);
        if (rdbStore != undefined) {
          await rdbStore.executeSql(DROP_TBL);
        }
        expect().assertFail();
      }
      console.log(TAG + "************* testDataSharePredicate_0018 end *************");
    });

    /**
     * @tc.name: testDataSharePredicate_0019
     * @tc.desc: Verify groupBy and orderByAsc predicates in combination with lessThanOrEqualTo
     * @tc.type: FUNC
     * @tc.precon: RdbStore is initialized and table is populated
     * @tc.step:
     *   1. Insert additional test data
     *   2. Create predicates with:
     *      - lessThanOrEqualTo on age field (age <= 17)
     *      - groupBy on isStudent field
     *      - orderByAsc on age field
     *   3. Query table and verify result set
     * @tc.expect:
     *   1. Additional data inserted successfully
     *   2. Correct predicates setup with groupBy and orderBy
     *   3. Correct result set with:
     *      - 2 groups (one for each isStudent value)
     *      - First row id=8 (Green, age=16)
     *      - Second row id=6 (Steven, age=17)
     */
    it('testDataSharePredicate_0019', 0, async () => {
      console.log(TAG + "************* testDataSharePredicate_0019 start *************");
      if (rdbStore != undefined) {
        await insertMoreData();
      }
      try {
        let dp = new dataSharePredicates.DataSharePredicates();
        dp.lessThanOrEqualTo('age', 17)
          .groupBy(['isStudent'])
          .orderByAsc('age');
        let resultSets = await (rdbStore as relationalStore.RdbStore).query(table_name, dp, ["*"]);
        // 2 groups
        expect(resultSets.rowCount).assertEqual(2);
        expect(resultSets.goToRow(0)).assertEqual(true);
        let id = resultSets.getLong(resultSets.getColumnIndex("id"));
        // groupBy returns 2 groups, orderBy age means (Green, List) age is lessThan (Steven, Jax)
        // the actually order is not changed, row[0].id == Green.id row[1].id == Steven.id
        expect(id).assertEqual(8);
        expect(resultSets.goToRow(1)).assertEqual(true);
        id = resultSets.getLong(resultSets.getColumnIndex("id"));
        expect(id).assertEqual(6);
        resultSets.close();
      } catch (err) {
        console.error(TAG + `Query failed, code is ${err.code},message is ${err.message}`);
        if (rdbStore != undefined) {
          await rdbStore.executeSql(DROP_TBL);
        }
        expect().assertFail();
      }
      console.log(TAG + "************* testDataSharePredicate_0019 end *************");
    });

    /**
     * @tc.name: testDataSharePredicate_0020
     * @tc.desc: Verify in and notIn predicates with vector values
     * @tc.type: FUNC
     * @tc.precon: RdbStore is initialized and table is populated
     * @tc.step:
     *   1. Insert additional test data
     *   2. Test in predicate on age field (age in [20, 24]) - expect 4 matching rows
     *   3. Test notIn predicate on age field (age not in [20, 24]) - expect 7 non-matching rows
     * @tc.expect:
     *   1. Additional data inserted successfully
     *   2. Correct result set for in query (first row id=1)
     *   3. Correct result set for notIn query (first row id=2)
     */
    it('testDataSharePredicate_0020', 0, async () => {
      console.log(TAG + "************* testDataSharePredicate_0020 start *************");
      if (rdbStore != undefined) {
        await insertMoreData();
      }
      try {
        let dp = new dataSharePredicates.DataSharePredicates();
        // in vector
        dp.in('age', [20, 24]);
        let resultSets = await (rdbStore as relationalStore.RdbStore).query(table_name, dp, ["*"]);
        expect(resultSets.rowCount).assertEqual(4);
        expect(resultSets.goToRow(0)).assertEqual(true);
        let id = resultSets.getLong(resultSets.getColumnIndex("id"));
        expect(id).assertEqual(1);
        resultSets.close();

        let dp1 = new dataSharePredicates.DataSharePredicates();
        // not in vector
        dp1.notIn('age', [20, 24]);
        let resultSets1 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp1, ["*"]);
        expect(resultSets1.rowCount).assertEqual(7);
        expect(resultSets1.goToRow(0)).assertEqual(true);
        let id1 = resultSets1.getLong(resultSets1.getColumnIndex("id"));
        expect(id1).assertEqual(2);
        resultSets1.close();
      } catch (err) {
        console.error(TAG + `Query failed, code is ${err.code},message is ${err.message}`);
        if (rdbStore != undefined) {
          await rdbStore.executeSql(DROP_TBL);
        }
        expect().assertFail();
      }
      console.log(TAG + "************* testDataSharePredicate_0020 end *************");
    });

    /**
     * @tc.name: testDataSharePredicate_0021
     * @tc.desc: Verify in and notIn predicates with string vector values
     * @tc.type: FUNC
     * @tc.precon: RdbStore is initialized and table is populated
     * @tc.step:
     *   1. Insert additional test data
     *   2. Test in predicate on name field (name in ['ZhangSan', 'LiSi']) - expect 2 matching rows
     *   3. Test notIn predicate on name field (name not in ['ZhangSan', 'LiSi']) - expect 9 non-matching rows
     * @tc.expect:
     *   1. Additional data inserted successfully
     *   2. Correct result set for in query (first row id=1)
     *   3. Correct result set for notIn query (first row id=3)
     */
    it('testDataSharePredicate_0021', 0, async () => {
      console.log(TAG + "************* testDataSharePredicate_0021 start *************");
      if (rdbStore != undefined) {
        await insertMoreData();
      }
      try {
        let dp = new dataSharePredicates.DataSharePredicates();
        // in number vector
        dp.in('name', ['ZhangSan', 'LiSi']);
        let resultSets = await (rdbStore as relationalStore.RdbStore).query(table_name, dp, ["*"]);
        expect(resultSets.rowCount).assertEqual(2);
        expect(resultSets.goToRow(0)).assertEqual(true);
        let id = resultSets.getLong(resultSets.getColumnIndex("id"));
        expect(id).assertEqual(1);
        resultSets.close();

        let dp1 = new dataSharePredicates.DataSharePredicates();
        // not in number vector
        dp1.notIn('name', ['ZhangSan', 'LiSi']);
        let resultSets1 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp1, ["*"]);
        expect(resultSets1.rowCount).assertEqual(9);
        expect(resultSets1.goToRow(0)).assertEqual(true);
        let id1 = resultSets1.getLong(resultSets1.getColumnIndex("id"));
        expect(id1).assertEqual(3);
        resultSets1.close();
      } catch (err) {
        console.error(TAG + `Query failed, code is ${err.code},message is ${err.message}`);
        if (rdbStore != undefined) {
          await rdbStore.executeSql(DROP_TBL);
        }
        expect().assertFail();
      }
      console.log(TAG + "************* testDataSharePredicate_0021 end *************");
    });

    /**
     * @tc.name: testDataSharePredicate_0022
     * @tc.desc: Verify in and notIn predicates with boolean vector values
     * @tc.type: FUNC
     * @tc.precon: RdbStore is initialized and table is populated
     * @tc.step:
     *   1. Insert additional test data
     *   2. Test in predicate on isStudent field (isStudent in [false]) - expect 4 matching rows
     *   3. Test notIn predicate on isStudent field (isStudent not in [false]) - expect 7 non-matching rows
     * @tc.expect:
     *   1. Additional data inserted successfully
     *   2. Correct result set for in query (first row id=3)
     *   3. Correct result set for notIn query (first row id=1)
     */
    it('testDataSharePredicate_0022', 0, async () => {
      console.log(TAG + "************* testDataSharePredicate_0022 start *************");
      if (rdbStore != undefined) {
        await insertMoreData();
      }
      try {
        let dp = new dataSharePredicates.DataSharePredicates();
        // in number vector
        dp.in('isStudent', [false]);
        let resultSets = await (rdbStore as relationalStore.RdbStore).query(table_name, dp, ["*"]);
        expect(resultSets.rowCount).assertEqual(4);
        expect(resultSets.goToRow(0)).assertEqual(true);
        let id = resultSets.getLong(resultSets.getColumnIndex("id"));
        expect(id).assertEqual(3);
        resultSets.close();

        let dp1 = new dataSharePredicates.DataSharePredicates();
        // not in number vector
        dp1.notIn('isStudent', [false]);
        let resultSets1 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp1, ["*"]);
        expect(resultSets1.rowCount).assertEqual(7);
        expect(resultSets1.goToRow(0)).assertEqual(true);
        let id1 = resultSets1.getLong(resultSets1.getColumnIndex("id"));
        expect(id1).assertEqual(1);
        resultSets1.close();
      } catch (err) {
        console.error(TAG + `Query failed, code is ${err.code},message is ${err.message}`);
        if (rdbStore != undefined) {
          await rdbStore.executeSql(DROP_TBL);
        }
        expect().assertFail();
      }
      console.log(TAG + "************* testDataSharePredicate_0022 end *************");
    });
  });
}