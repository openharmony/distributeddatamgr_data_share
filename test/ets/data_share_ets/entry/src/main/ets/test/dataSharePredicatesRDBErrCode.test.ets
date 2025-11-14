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
  name: "publicPredicatesRdbStore.db",
  securityLevel: relationalStore.SecurityLevel.S1
};
let rdbStore: relationalStore.RdbStore | undefined = undefined;
let table_name = "test";
let DROP_TBL = "DROP TABLE IF EXISTS " + table_name;
let CREATE_TABLE_TEST = "CREATE TABLE IF NOT EXISTS "
  + table_name
  + " (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT NOT NULL, age INTEGER, isStudent BOOLEAN)";
let TAG = "[dataSharePredicateRDBErrCode_test]";

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
    const valueBucket6: relationalStore.ValuesBucket = {
      name: "Error",
      age: 0,
      'isStudent': true,
    };
    await rdbStore.insert("test", valueBucket6);
    console.info(TAG + 'generateTable end');
  }
}

export default function DataSharePredicatesRDBErrCodeEtsTest() {
  describe('DataSharePredicatesRDBErrCodeEtsTest', () => {
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
     * @tc.name: NotEqualToErrCodeTest_0001
     * @tc.desc: Verify the behavior of the notEqualTo in DataSharePredicates with various input types
     * @tc.type: FUNC
     * @tc.precon: DataSharePredicates instance is created and table is populated with test data
     * @tc.step:
     *   1. Test notEqualTo with numeric field and expect row count 5
     *   2. Test notEqualTo with invalid parameters and expect an error
     * @tc.expect:
     *   1. Correct row count 5 for numeric field and verify specific row data
     *   2. Error is thrown for invalid parameters
     */
    it('NotEqualToErrCodeTest_0001', 0, async () => {
      console.log(TAG + "************* NotEqualToErrCodeTest_0001 start *************");
      try {
        let dp = new dataSharePredicates.DataSharePredicates();
        dp.notEqualTo('age', 20, 22);
        let resultSets = await (rdbStore as relationalStore.RdbStore).query(table_name, dp, ["*"]);
        expect(resultSets.rowCount).assertEqual(5);
        expect(resultSets.goToRow(0)).assertEqual(true);
        let id = resultSets.getLong(resultSets.getColumnIndex("id"));
        expect(id).assertEqual(2);
        resultSets.close();

        try {
          let dp1 = new dataSharePredicates.DataSharePredicates();
          dp1.notEqualTo('age');
          expect().assertFail();
        } catch (error1) {
          console.info(TAG + 'error1: ' + JSON.stringify(error1));
        }
      } catch (err) {
        console.error(TAG + `Query failed, code is ${err.code},message is ${err.message}`);
        if (rdbStore != undefined) {
          await rdbStore.executeSql(DROP_TBL);
        }
        expect().assertFail();
      }
      console.log(TAG + "************* NotEqualToErrCodeTest_0001 end *************");
    });

    /**
     * @tc.name: NotEqualToErrCodeTest_0002
     * @tc.desc: Verify the behavior of the notEqualTo in DataSharePredicates with various input types
     * @tc.type: FUNC
     * @tc.precon: DataSharePredicates instance is created and table is populated with test data
     * @tc.step:
     *   1. Test notEqualTo with numeric field
     *   2. Test notEqualTo with null and undefined values
     *   3. Test notEqualTo with string 'null'
     *   4. Test notEqualTo with empty string
     * @tc.expect:
     *   1. Correct row count 6 for numeric field
     *   2. Correct row count 6 for null and undefined values
     *   3. Correct row count 0 for string 'null'
     *   4. Correct row count 6 for empty string
     */
    it('NotEqualToErrCodeTest_0002', 0, async () => {
      console.log(TAG + "************* NotEqualToErrCodeTest_0002 start *************");
      try {
        let dp = new dataSharePredicates.DataSharePredicates();
        dp.notEqualTo(1, 22);
        let resultSets = await (rdbStore as relationalStore.RdbStore).query(table_name, dp, ["*"]);
        expect(resultSets.rowCount).assertEqual(6);
        resultSets.close();

        let dp1 = new dataSharePredicates.DataSharePredicates();
        dp1.notEqualTo(null, 22)
          .or()
          .notEqualTo(undefined, 22);
        let resultSets1 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp1, ["*"]);
        expect(resultSets1.rowCount).assertEqual(6);
        resultSets1.close();

        let dp2 = new dataSharePredicates.DataSharePredicates();
        dp2.notEqualTo('null', 22);
        let resultSets2 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp2, ["*"]);
        expect(resultSets2.rowCount).assertEqual(0);
        resultSets2.close();

        let dp3 = new dataSharePredicates.DataSharePredicates();
        dp3.notEqualTo('', 22);
        let resultSets3 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp3, ["*"]);
        expect(resultSets3.rowCount).assertEqual(6);
        resultSets3.close();
      } catch (err) {
        console.error(TAG + `Query failed, code is ${err.code},message is ${err.message}`);
        if (rdbStore != undefined) {
          await rdbStore.executeSql(DROP_TBL);
        }
        expect().assertFail();
      }
      console.log(TAG + "************* NotEqualToErrCodeTest_0002 end *************");
    });

    /**
     * @tc.name: NotEqualToErrCodeTest_0003
     * @tc.desc: Verify the behavior of the notEqualTo in DataSharePredicates with various input types
     * @tc.type: FUNC
     * @tc.precon: DataSharePredicates instance is created and table is populated with test data
     * @tc.step:
     *   1. Test notEqualTo with an array of values
     *   2. Test notEqualTo with null and undefined values combined
     *   3. Test notEqualTo with string number
     *   4. Test notEqualTo with empty string
     *   5. Test notEqualTo with boolean value
     * @tc.expect:
     *   1. Correct row count 6 for array of values
     *   2. Correct row count 6 for null and undefined values
     *   3. Correct row count 5 for string number
     *   4. Correct row count 6 for empty string
     *   5. Correct row count 5 for boolean value
     */
    it('NotEqualToErrCodeTest_0003', 0, async () => {
      console.log(TAG + "************* NotEqualToErrCodeTest_0003 start *************");
      try {
        let dp = new dataSharePredicates.DataSharePredicates();
        dp.notEqualTo('age', [20, 22]);
        let resultSets = await (rdbStore as relationalStore.RdbStore).query(table_name, dp, ["*"]);
        expect(resultSets.rowCount).assertEqual(6);
        resultSets.close();

        let dp1 = new dataSharePredicates.DataSharePredicates();
        dp1.notEqualTo('age', null)
          .or()
          .notEqualTo('name', undefined);
        let resultSets1 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp1, ["*"]);
        expect(resultSets1.rowCount).assertEqual(6);
        resultSets1.close();

        let dp2 = new dataSharePredicates.DataSharePredicates();
        dp2.notEqualTo('age', '20');
        let resultSets2 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp2, ["*"]);
        expect(resultSets2.rowCount).assertEqual(5);
        resultSets2.close();

        let dp3 = new dataSharePredicates.DataSharePredicates();
        dp3.notEqualTo('name', '');
        let resultSets3 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp3, ["*"]);
        expect(resultSets3.rowCount).assertEqual(6);
        resultSets3.close();

        let dp4 = new dataSharePredicates.DataSharePredicates();
        dp4.notEqualTo('age', false);
        let resultSets4 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp4, ["*"]);
        expect(resultSets4.rowCount).assertEqual(5);
        resultSets4.close();
      } catch (err) {
        console.error(TAG + `Query failed, code is ${err.code},message is ${err.message}`);
        if (rdbStore != undefined) {
          await rdbStore.executeSql(DROP_TBL);
        }
        expect().assertFail();
      }
      console.log(TAG + "************* NotEqualToErrCodeTest_0003 end *************");
    });

    /**
     * @tc.name: GreaterThanErrCodeTest_0001
     * @tc.desc: Verify the behavior of the greaterThan in DataSharePredicates with various input types
     * @tc.type: FUNC
     * @tc.precon: DataSharePredicates instance is created and table is populated with test data
     * @tc.step:
     *   1. Test greaterThan with numeric field and expect row count 2
     *   2. Test greaterThan with invalid parameters and expect an error
     * @tc.expect:
     *   1. Correct row count 2 for numeric field and verify specific row data
     *   2. Error is thrown for invalid parameters
     */
    it('GreaterThanErrCodeTest_0001', 0, async () => {
      console.log(TAG + "************* GreaterThanErrCodeTest_0001 start *************");
      try {
        let dp = new dataSharePredicates.DataSharePredicates();
        dp.greaterThan('age', 22, '20');
        let resultSets = await (rdbStore as relationalStore.RdbStore).query(table_name, dp, ["*"]);
        expect(resultSets.rowCount).assertEqual(2);
        expect(resultSets.goToRow(0)).assertEqual(true);
        let name = resultSets.getString(resultSets.getColumnIndex("name"));
        expect(name).assertEqual('SunLiu');
        resultSets.close();

        try {
          let dp1 = new dataSharePredicates.DataSharePredicates();
          dp1.greaterThan('age');
          expect().assertFail();
        } catch (error1) {
          console.info(TAG + 'error1: ' + JSON.stringify(error1));
        }
      } catch (err) {
        console.error(TAG + `Query failed, code is ${err.code},message is ${err.message}`);
        if (rdbStore != undefined) {
          await rdbStore.executeSql(DROP_TBL);
        }
        expect().assertFail();
      }
      console.log(TAG + "************* GreaterThanErrCodeTest_0001 end *************");
    });

    /**
     * @tc.name: GreaterThanErrCodeTest_0002
     * @tc.desc: Verify the behavior of the greaterThan in DataSharePredicates with various input types
     * @tc.type: FUNC
     * @tc.precon: DataSharePredicates instance is created and table is populated with test data
     * @tc.step:
     *   1. Test greaterThan with numeric field
     *   2. Test greaterThan with null and undefined values
     *   3. Test greaterThan with string 'null'
     *   4. Test greaterThan with empty string
     * @tc.expect:
     *   1. Correct row count 6 for numeric field
     *   2. Correct row count 6 for null and undefined values
     *   3. Correct row count 0 for string 'null'
     *   4. Correct row count 6 for empty string
     */
    it('GreaterThanErrCodeTest_0002', 0, async () => {
      console.log(TAG + "************* GreaterThanErrCodeTest_0002 start *************");
      try {
        let dp = new dataSharePredicates.DataSharePredicates();
        dp.greaterThan(1, 22);
        let resultSets = await (rdbStore as relationalStore.RdbStore).query(table_name, dp, ["*"]);
        expect(resultSets.rowCount).assertEqual(6);
        resultSets.close();

        let dp1 = new dataSharePredicates.DataSharePredicates();
        dp1.greaterThan(null, 22)
          .or()
          .greaterThan(undefined, 22);
        let resultSets1 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp1, ["*"]);
        expect(resultSets1.rowCount).assertEqual(6);
        resultSets1.close();

        let dp2 = new dataSharePredicates.DataSharePredicates();
        dp2.greaterThan('null', 22);
        let resultSets2 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp2, ["*"]);
        expect(resultSets2.rowCount).assertEqual(0);
        resultSets2.close();

        let dp3 = new dataSharePredicates.DataSharePredicates();
        dp3.greaterThan('', 22);
        let resultSets3 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp3, ["*"]);
        expect(resultSets3.rowCount).assertEqual(6);
        resultSets3.close();
      } catch (err) {
        console.error(TAG + `Query failed, code is ${err.code},message is ${err.message}`);
        if (rdbStore != undefined) {
          await rdbStore.executeSql(DROP_TBL);
        }
        expect().assertFail();
      }
      console.log(TAG + "************* GreaterThanErrCodeTest_0002 end *************");
    });

    /**
     * @tc.name: GreaterThanErrCodeTest_0003
     * @tc.desc: Verify the behavior of the greaterThan in DataSharePredicates with various input types
     * @tc.type: FUNC
     * @tc.precon: DataSharePredicates instance is created and table is populated with test data
     * @tc.step:
     *   1. Test greaterThan with array of numbers
     *   2. Test greaterThan with null and undefined values combined
     *   3. Test greaterThan with empty string
     *   4. Test greaterThan with string number
     *   5. Test greaterThan with boolean value
     * @tc.expect:
     *   1. Correct row count 6 for array of numbers
     *   2. Correct row count 6 for null and undefined values
     *   3. Correct row count 6 for empty string
     *   4. Correct row count 2 for string number
     *   5. Correct row count 6 for boolean value
     */
    it('GreaterThanErrCodeTest_0003', 0, async () => {
      console.log(TAG + "************* GreaterThanErrCodeTest_0003 start *************");
      try {
        let dp = new dataSharePredicates.DataSharePredicates();
        dp.greaterThan('age', [1, 2]);
        let resultSets = await (rdbStore as relationalStore.RdbStore).query(table_name, dp, ["*"]);
        expect(resultSets.rowCount).assertEqual(6);
        resultSets.close();

        let dp1 = new dataSharePredicates.DataSharePredicates();
        dp1.greaterThan('age', null)
          .or()
          .greaterThan('name', undefined);
        let resultSets1 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp1, ["*"]);
        expect(resultSets1.rowCount).assertEqual(6);
        resultSets1.close();

        let dp2 = new dataSharePredicates.DataSharePredicates();
        dp2.greaterThan('age', '');
        let resultSets2 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp2, ["*"]);
        expect(resultSets2.rowCount).assertEqual(6);
        resultSets2.close();

        let dp3 = new dataSharePredicates.DataSharePredicates();
        dp3.greaterThan('age', '22');
        let resultSets3 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp3, ["*"]);
        expect(resultSets3.rowCount).assertEqual(2);
        resultSets3.close();

        let dp4 = new dataSharePredicates.DataSharePredicates();
        dp4.greaterThan('age', true);
        let resultSets4 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp4, ["*"]);
        expect(resultSets4.rowCount).assertEqual(6);
        resultSets4.close();
      } catch (err) {
        console.error(TAG + `Query failed, code is ${err.code},message is ${err.message}`);
        if (rdbStore != undefined) {
          await rdbStore.executeSql(DROP_TBL);
        }
        expect().assertFail();
      }
      console.log(TAG + "************* GreaterThanErrCodeTest_0003 end *************");
    });

    /**
     * @tc.name: LessThanErrCodeTest_0001
     * @tc.desc: Verify the behavior of the lessThan in DataSharePredicates with various input types
     * @tc.type: FUNC
     * @tc.precon: DataSharePredicates instance is created and table is populated with test data
     * @tc.step:
     *   1. Test lessThan with numeric field and expect row count 3
     *   2. Test lessThan with invalid parameters and expect an error
     * @tc.expect:
     *   1. Correct row count 3 for numeric field and verify specific row data
     *   2. Error is thrown for invalid parameters
     */
    it('LessThanErrCodeTest_0001', 0, async () => {
      console.log(TAG + "************* LessThanErrCodeTest_0001 start *************");
      try {
        let dp = new dataSharePredicates.DataSharePredicates();
        dp.lessThan('age', 22, '20');
        let resultSets = await (rdbStore as relationalStore.RdbStore).query(table_name, dp, ["*"]);
        expect(resultSets.rowCount).assertEqual(3);
        expect(resultSets.goToRow(0)).assertEqual(true);
        let name = resultSets.getString(resultSets.getColumnIndex("name"));
        expect(name).assertEqual('ZhangSan');
        resultSets.close();

        try {
          let dp1 = new dataSharePredicates.DataSharePredicates();
          dp1.lessThan(20);
          expect().assertFail();
        } catch (error1) {
          console.info(TAG + 'error1: ' + JSON.stringify(error1));
        }
      } catch (err) {
        console.error(TAG + `Query failed, code is ${err.code},message is ${err.message}`);
        if (rdbStore != undefined) {
          await rdbStore.executeSql(DROP_TBL);
        }
        expect().assertFail();
      }
      console.log(TAG + "************* LessThanErrCodeTest_0001 end *************");
    });

    /**
     * @tc.name: LessThanErrCodeTest_0002
     * @tc.desc: Verify the behavior of the lessThan in DataSharePredicates with various input types
     * @tc.type: FUNC
     * @tc.precon: DataSharePredicates instance is created and table is populated with test data
     * @tc.step:
     *   1. Test lessThan with numeric field
     *   2. Test lessThan with null and undefined values
     *   3. Test lessThan with string 'null'
     *   4. Test lessThan with empty string
     * @tc.expect:
     *   1. Correct row count 6 for numeric field
     *   2. Correct row count 6 for null and undefined values
     *   3. Correct row count 0 for string 'null'
     *   4. Correct row count 6 for empty string
     */
    it('LessThanErrCodeTest_0002', 0, async () => {
      console.log(TAG + "************* LessThanErrCodeTest_0002 start *************");
      try {
        let dp = new dataSharePredicates.DataSharePredicates();
        dp.lessThan(1, 22);
        let resultSets = await (rdbStore as relationalStore.RdbStore).query(table_name, dp, ["*"]);
        expect(resultSets.rowCount).assertEqual(6);
        resultSets.close();

        let dp1 = new dataSharePredicates.DataSharePredicates();
        dp1.lessThan(null, 22)
          .or()
          .lessThan(undefined, 22);
        let resultSets1 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp1, ["*"]);
        expect(resultSets1.rowCount).assertEqual(6);
        resultSets1.close();

        let dp2 = new dataSharePredicates.DataSharePredicates();
        dp2.lessThan('null', 22);
        let resultSets2 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp2, ["*"]);
        expect(resultSets2.rowCount).assertEqual(0);
        resultSets2.close();

        let dp3 = new dataSharePredicates.DataSharePredicates();
        dp3.lessThan('', 22);
        let resultSets3 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp3, ["*"]);
        expect(resultSets3.rowCount).assertEqual(6);
        resultSets3.close();
      } catch (err) {
        console.error(TAG + `Query failed, code is ${err.code},message is ${err.message}`);
        if (rdbStore != undefined) {
          await rdbStore.executeSql(DROP_TBL);
        }
        expect().assertFail();
      }
      console.log(TAG + "************* LessThanErrCodeTest_0002 end *************");
    });

    /**
     * @tc.name: LessThanErrCodeTest_0003
     * @tc.desc: Verify the behavior of the lessThan in DataSharePredicates with various input types
     * @tc.type: FUNC
     * @tc.precon: DataSharePredicates instance is created and table is populated with test data
     * @tc.step:
     *   1. Test lessThan with array of numbers
     *   2. Test lessThan with null and undefined values combined
     *   3. Test lessThan with empty string
     *   4. Test lessThan with string number
     * @tc.expect:
     *   1. Correct row count 6 for array of numbers
     *   2. Correct row count 6 for null and undefined values
     *   3. Correct row count 6 for empty string
     *   4. Correct row count 3 for string number
     */
    it('LessThanErrCodeTest_0003', 0, async () => {
      console.log(TAG + "************* LessThanErrCodeTest_0003 start *************");
      try {
        let dp = new dataSharePredicates.DataSharePredicates();
        dp.lessThan('age', [1, 2]);
        let resultSets = await (rdbStore as relationalStore.RdbStore).query(table_name, dp, ["*"]);
        expect(resultSets.rowCount).assertEqual(6);
        resultSets.close();

        let dp1 = new dataSharePredicates.DataSharePredicates();
        dp1.lessThan('age', null)
          .or()
          .lessThan('name', undefined);
        let resultSets1 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp1, ["*"]);
        expect(resultSets1.rowCount).assertEqual(6);
        resultSets1.close();

        let dp2 = new dataSharePredicates.DataSharePredicates();
        dp2.lessThan('age', '');
        let resultSets2 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp2, ["*"]);
        expect(resultSets2.rowCount).assertEqual(6);
        resultSets2.close();

        let dp3 = new dataSharePredicates.DataSharePredicates();
        dp3.lessThan('age', '22');
        let resultSets3 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp3, ["*"]);
        expect(resultSets3.rowCount).assertEqual(3);
        resultSets3.close();
      } catch (err) {
        console.error(TAG + `Query failed, code is ${err.code},message is ${err.message}`);
        if (rdbStore != undefined) {
          await rdbStore.executeSql(DROP_TBL);
        }
        expect().assertFail();
      }
      console.log(TAG + "************* LessThanErrCodeTest_0003 end *************");
    });

    /**
     * @tc.name: GreaterThanOrEqualToErrCodeTest_0001
     * @tc.desc: Verify the behavior of the greaterThanOrEqualTo in DataSharePredicates with various input types
     * @tc.type: FUNC
     * @tc.precon: DataSharePredicates instance is created and table is populated with test data
     * @tc.step:
     *   1. Test greaterThanOrEqualTo with numeric field and expect row count 3
     *   2. Test greaterThanOrEqualTo with invalid parameters and expect an error
     * @tc.expect:
     *   1. Correct row count 3 for numeric field and verify specific row data
     *   2. Error is thrown for invalid parameters
     */
    it('GreaterThanOrEqualToErrCodeTest_0001', 0, async () => {
      console.log(TAG + "************* GreaterThanOrEqualToErrCodeTest_0001 start *************");
      try {
        let dp = new dataSharePredicates.DataSharePredicates();
        dp.greaterThanOrEqualTo('age', 22, '20');
        let resultSets = await (rdbStore as relationalStore.RdbStore).query(table_name, dp, ["*"]);
        expect(resultSets.rowCount).assertEqual(3);
        expect(resultSets.goToRow(0)).assertEqual(true);
        let name = resultSets.getString(resultSets.getColumnIndex("name"));
        expect(name).assertEqual('WangWu');
        resultSets.close();

        try {
          let dp1 = new dataSharePredicates.DataSharePredicates();
          dp1.greaterThanOrEqualTo(20);
          expect().assertFail();
        } catch (error1) {
          console.info(TAG + 'error1: ' + JSON.stringify(error1));
        }
      } catch (err) {
        console.error(TAG + `Query failed, code is ${err.code},message is ${err.message}`);
        if (rdbStore != undefined) {
          await rdbStore.executeSql(DROP_TBL);
        }
        expect().assertFail();
      }
      console.log(TAG + "************* GreaterThanOrEqualToErrCodeTest_0001 end *************");
    });

    /**
     * @tc.name: GreaterThanOrEqualToErrCodeTest_0002
     * @tc.desc: Verify the behavior of the greaterThanOrEqualTo in DataSharePredicates with various input types
     * @tc.type: FUNC
     * @tc.precon: DataSharePredicates instance is created and table is populated with test data
     * @tc.step:
     *   1. Test greaterThanOrEqualTo with numeric field
     *   2. Test greaterThanOrEqualTo with null and undefined values
     *   3. Test greaterThanOrEqualTo with string 'null'
     *   4. Test greaterThanOrEqualTo with empty string
     * @tc.expect:
     *   1. Correct row count 6 for numeric field
     *   2. Correct row count 6 for null and undefined values
     *   3. Correct row count 0 for string 'null'
     *   4. Correct row count 6 for empty string
     */
    it('GreaterThanOrEqualToErrCodeTest_0002', 0, async () => {
      console.log(TAG + "************* GreaterThanOrEqualToErrCodeTest_0002 start *************");
      try {
        let dp = new dataSharePredicates.DataSharePredicates();
        dp.greaterThanOrEqualTo(1, 22);
        let resultSets = await (rdbStore as relationalStore.RdbStore).query(table_name, dp, ["*"]);
        expect(resultSets.rowCount).assertEqual(6);
        resultSets.close();

        let dp1 = new dataSharePredicates.DataSharePredicates();
        dp1.greaterThanOrEqualTo(null, 22)
          .or()
          .greaterThanOrEqualTo(undefined, 22);
        let resultSets1 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp1, ["*"]);
        expect(resultSets1.rowCount).assertEqual(6);
        resultSets1.close();

        let dp2 = new dataSharePredicates.DataSharePredicates();
        dp2.greaterThanOrEqualTo('null', 22);
        let resultSets2 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp2, ["*"]);
        expect(resultSets2.rowCount).assertEqual(0);
        resultSets2.close();

        let dp3 = new dataSharePredicates.DataSharePredicates();
        dp3.greaterThanOrEqualTo('', 22);
        let resultSets3 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp3, ["*"]);
        expect(resultSets3.rowCount).assertEqual(6);
        resultSets3.close();
      } catch (err) {
        console.error(TAG + `Query failed, code is ${err.code},message is ${err.message}`);
        if (rdbStore != undefined) {
          await rdbStore.executeSql(DROP_TBL);
        }
        expect().assertFail();
      }
      console.log(TAG + "************* GreaterThanOrEqualToErrCodeTest_0002 end *************");
    });

    /**
     * @tc.name: GreaterThanOrEqualToErrCodeTest_0003
     * @tc.desc: Verify the behavior of the greaterThanOrEqualTo in DataSharePredicates with various input types
     * @tc.type: FUNC
     * @tc.precon: DataSharePredicates instance is created and table is populated with test data
     * @tc.step:
     *   1. Test greaterThanOrEqualTo with array of numbers
     *   2. Test greaterThanOrEqualTo with null and undefined values combined
     *   3. Test greaterThanOrEqualTo with empty string
     *   4. Test greaterThanOrEqualTo with string number
     *   5. Test greaterThanOrEqualTo with boolean value
     * @tc.expect:
     *   1. Correct row count 6 for array of numbers
     *   2. Correct row count 6 for null and undefined values
     *   3. Correct row count 6 for empty string
     *   4. Correct row count 5 for string number
     *   5. Correct row count 6 for boolean value
     */
    it('GreaterThanOrEqualToErrCodeTest_0003', 0, async () => {
      console.log(TAG + "************* GreaterThanOrEqualToErrCodeTest_0003 start *************");
      try {
        let dp = new dataSharePredicates.DataSharePredicates();
        dp.greaterThanOrEqualTo('age', [1, 2]);
        let resultSets = await (rdbStore as relationalStore.RdbStore).query(table_name, dp, ["*"]);
        expect(resultSets.rowCount).assertEqual(6);
        resultSets.close();

        let dp1 = new dataSharePredicates.DataSharePredicates();
        dp1.greaterThanOrEqualTo('age', null)
          .or()
          .greaterThanOrEqualTo('name', undefined);
        let resultSets1 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp1, ["*"]);
        expect(resultSets1.rowCount).assertEqual(6);
        resultSets1.close();

        let dp2 = new dataSharePredicates.DataSharePredicates();
        dp2.greaterThanOrEqualTo('age', '');
        let resultSets2 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp2, ["*"]);
        expect(resultSets2.rowCount).assertEqual(6);
        resultSets2.close();

        let dp3 = new dataSharePredicates.DataSharePredicates();
        dp3.greaterThanOrEqualTo('age', '20');
        let resultSets3 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp3, ["*"]);
        expect(resultSets3.rowCount).assertEqual(5);
        resultSets3.close();

        let dp4 = new dataSharePredicates.DataSharePredicates();
        dp4.greaterThanOrEqualTo('age', false);
        let resultSets4 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp4, ["*"]);
        expect(resultSets4.rowCount).assertEqual(6);
        resultSets4.close();
      } catch (err) {
        console.error(TAG + `Query failed, code is ${err.code},message is ${err.message}`);
        if (rdbStore != undefined) {
          await rdbStore.executeSql(DROP_TBL);
        }
        expect().assertFail();
      }
      console.log(TAG + "************* GreaterThanOrEqualToErrCodeTest_0003 end *************");
    });

    /**
     * @tc.name: LessThanOrEqualToErrCodeTest_0001
     * @tc.desc: Verify the behavior of the lessThanOrEqualTo in DataSharePredicates with various input types
     * @tc.type: FUNC
     * @tc.precon: DataSharePredicates instance is created and table is populated with test data
     * @tc.step:
     *   1. Test lessThanOrEqualTo with numeric field and expect row count 4
     *   2. Test lessThanOrEqualTo with invalid parameters and expect an error
     * @tc.expect:
     *   1. Correct row count 4 for numeric field and verify specific row data
     *   2. Error is thrown for invalid parameters
     */
    it('LessThanOrEqualToErrCodeTest_0001', 0, async () => {
      console.log(TAG + "************* LessThanOrEqualToErrCodeTest_0001 start *************");
      try {
        let dp = new dataSharePredicates.DataSharePredicates();
        dp.lessThanOrEqualTo('age', 22, '20');
        let resultSets = await (rdbStore as relationalStore.RdbStore).query(table_name, dp, ["*"]);
        expect(resultSets.rowCount).assertEqual(4);
        expect(resultSets.goToRow(0)).assertEqual(true);
        let name = resultSets.getString(resultSets.getColumnIndex("name"));
        expect(name).assertEqual('ZhangSan');
        resultSets.close();

        try {
          let dp1 = new dataSharePredicates.DataSharePredicates();
          dp1.lessThanOrEqualTo(20);
          expect().assertFail();
        } catch (error1) {
          console.info(TAG + 'error1: ' + JSON.stringify(error1));
        }
      } catch (err) {
        console.error(TAG + `Query failed, code is ${err.code},message is ${err.message}`);
        if (rdbStore != undefined) {
          await rdbStore.executeSql(DROP_TBL);
        }
        expect().assertFail();
      }
      console.log(TAG + "************* LessThanOrEqualToErrCodeTest_0001 end *************");
    });

    /**
     * @tc.name: LessThanOrEqualToErrCodeTest_0002
     * @tc.desc: Verify the behavior of the lessThanOrEqualTo in DataSharePredicates with various input types
     * @tc.type: FUNC
     * @tc.precon: DataSharePredicates instance is created and table is populated with test data
     * @tc.step:
     *   1. Test lessThanOrEqualTo with numeric field
     *   2. Test lessThanOrEqualTo with null and undefined values
     *   3. Test lessThanOrEqualTo with string 'null'
     *   4. Test lessThanOrEqualTo with empty string
     * @tc.expect:
     *   1. Correct row count 6 for numeric field
     *   2. Correct row count 6 for null and undefined values
     *   3. Correct row count 0 for string 'null'
     *   4. Correct row count 6 for empty string
     */
    it('LessThanOrEqualToErrCodeTest_0002', 0, async () => {
      console.log(TAG + "************* LessThanOrEqualToErrCodeTest_0002 start *************");
      try {
        let dp = new dataSharePredicates.DataSharePredicates();
        dp.lessThanOrEqualTo(1, 22);
        let resultSets = await (rdbStore as relationalStore.RdbStore).query(table_name, dp, ["*"]);
        expect(resultSets.rowCount).assertEqual(6);
        resultSets.close();

        let dp1 = new dataSharePredicates.DataSharePredicates();
        dp1.lessThanOrEqualTo(null, 22)
          .or()
          .lessThanOrEqualTo(undefined, 22);
        let resultSets1 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp1, ["*"]);
        expect(resultSets1.rowCount).assertEqual(6);
        resultSets1.close();

        let dp2 = new dataSharePredicates.DataSharePredicates();
        dp2.lessThanOrEqualTo('null', 22);
        let resultSets2 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp2, ["*"]);
        expect(resultSets2.rowCount).assertEqual(0);
        resultSets2.close();

        let dp3 = new dataSharePredicates.DataSharePredicates();
        dp3.lessThanOrEqualTo('', 22);
        let resultSets3 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp3, ["*"]);
        expect(resultSets3.rowCount).assertEqual(6);
        resultSets3.close();
      } catch (err) {
        console.error(TAG + `Query failed, code is ${err.code},message is ${err.message}`);
        if (rdbStore != undefined) {
          await rdbStore.executeSql(DROP_TBL);
        }
        expect().assertFail();
      }
      console.log(TAG + "************* LessThanOrEqualToErrCodeTest_0002 end *************");
    });

    /**
     * @tc.name: LessThanOrEqualToErrCodeTest_0003
     * @tc.desc: Verify the behavior of the lessThanOrEqualTo in DataSharePredicates with various input types
     * @tc.type: FUNC
     * @tc.precon: DataSharePredicates instance is created and table is populated with test data
     * @tc.step:
     *   1. Test lessThanOrEqualTo with array of numbers
     *   2. Test lessThanOrEqualTo with null and undefined values combined
     *   3. Test lessThanOrEqualTo with empty string
     *   4. Test lessThanOrEqualTo with string number
     *   5. Test lessThanOrEqualTo with boolean value
     * @tc.expect:
     *   1. Correct row count 6 for array of numbers
     *   2. Correct row count 6 for null and undefined values
     *   3. Correct row count 6 for empty string
     *   4. Correct row count 2 for string number
     *   5. Correct row count 6 for boolean value
     */
    it('LessThanOrEqualToErrCodeTest_0003', 0, async () => {
      console.log(TAG + "************* LessThanOrEqualToErrCodeTest_0003 start *************");
      try {
        let dp = new dataSharePredicates.DataSharePredicates();
        dp.lessThanOrEqualTo('age', [1, 2]);
        let resultSets = await (rdbStore as relationalStore.RdbStore).query(table_name, dp, ["*"]);
        expect(resultSets.rowCount).assertEqual(6);
        resultSets.close();

        let dp1 = new dataSharePredicates.DataSharePredicates();
        dp1.lessThanOrEqualTo('age', null)
          .or()
          .lessThanOrEqualTo('name', undefined);
        let resultSets1 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp1, ["*"]);
        expect(resultSets1.rowCount).assertEqual(6);
        resultSets1.close();

        let dp2 = new dataSharePredicates.DataSharePredicates();
        dp2.lessThanOrEqualTo('age', '');
        let resultSets2 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp2, ["*"]);
        expect(resultSets2.rowCount).assertEqual(6);
        resultSets2.close();

        let dp3 = new dataSharePredicates.DataSharePredicates();
        dp3.lessThanOrEqualTo('age', '20');
        let resultSets3 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp3, ["*"]);
        expect(resultSets3.rowCount).assertEqual(2);
        resultSets3.close();

        let dp4 = new dataSharePredicates.DataSharePredicates();
        dp4.lessThanOrEqualTo('age', true);
        let resultSets4 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp4, ["*"]);
        expect(resultSets4.rowCount).assertEqual(6);
        resultSets4.close();
      } catch (err) {
        console.error(TAG + `Query failed, code is ${err.code},message is ${err.message}`);
        if (rdbStore != undefined) {
          await rdbStore.executeSql(DROP_TBL);
        }
        expect().assertFail();
      }
      console.log(TAG + "************* LessThanOrEqualToErrCodeTest_0003 end *************");
    });

    /**
     * @tc.name: LikeErrCodeTest_0001
     * @tc.desc: Verify the behavior of the like in DataSharePredicates with various input types
     * @tc.type: FUNC
     * @tc.precon: DataSharePredicates instance is created and table is populated with test data
     * @tc.step:
     *   1. Test like with string pattern and expect row count 2
     *   2. Test like with invalid parameters and expect an error
     * @tc.expect:
     *   1. Correct row count 2 for string pattern and verify specific row data
     *   2. Error is thrown for invalid parameters
     */
    it('LikeErrCodeTest_0001', 0, async () => {
      console.log(TAG + "************* LikeErrCodeTest_0001 start *************");
      try {
        let dp = new dataSharePredicates.DataSharePredicates();
        dp.like('name', '%ng%', 24);
        let resultSets = await (rdbStore as relationalStore.RdbStore).query(table_name, dp, ["*"]);
        expect(resultSets.rowCount).assertEqual(2);
        expect(resultSets.goToRow(0)).assertEqual(true);
        let id = resultSets.getLong(resultSets.getColumnIndex("id"));
        expect(id).assertEqual(1);
        resultSets.close();

        try {
          let dp1 = new dataSharePredicates.DataSharePredicates();
          dp1.like(20);
          expect().assertFail();
        } catch (error1) {
          console.info(TAG + 'error1: ' + JSON.stringify(error1));
        }
      } catch (err) {
        console.error(TAG + `Query failed, code is ${err.code},message is ${err.message}`);
        if (rdbStore != undefined) {
          await rdbStore.executeSql(DROP_TBL);
        }
        expect().assertFail();
      }
      console.log(TAG + "************* LikeErrCodeTest_0001 end *************");
    });

    /**
     * @tc.name: LikeErrCodeTest_0002
     * @tc.desc: Verify the behavior of the like in DataSharePredicates with various input types
     * @tc.type: FUNC
     * @tc.precon: DataSharePredicates instance is created and table is populated with test data
     * @tc.step:
     *   1. Test like with numeric field
     *   2. Test like with null and undefined values
     *   3. Test like with string 'null'
     *   4. Test like with empty string
     * @tc.expect:
     *   1. Correct row count 6 for numeric field
     *   2. Correct row count 6 for null and undefined values
     *   3. Correct row count 0 for string 'null'
     *   4. Correct row count 6 for empty string
     */
    it('LikeErrCodeTest_0002', 0, async () => {
      console.log(TAG + "************* LikeErrCodeTest_0002 start *************");
      try {
        let dp = new dataSharePredicates.DataSharePredicates();
        dp.like(1, '%ng%');
        let resultSets = await (rdbStore as relationalStore.RdbStore).query(table_name, dp, ["*"]);
        expect(resultSets.rowCount).assertEqual(6);
        resultSets.close();

        let dp1 = new dataSharePredicates.DataSharePredicates();
        dp1.like(null, '%ng%')
          .or()
          .like(undefined, '%ng%');
        let resultSets1 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp1, ["*"]);
        expect(resultSets1.rowCount).assertEqual(6);
        resultSets1.close();

        let dp2 = new dataSharePredicates.DataSharePredicates();
        dp2.like('null', '%ng%');
        let resultSets2 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp2, ["*"]);
        expect(resultSets2.rowCount).assertEqual(0);
        resultSets2.close();

        let dp3 = new dataSharePredicates.DataSharePredicates();
        dp3.like('', '%ng%');
        let resultSets3 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp3, ["*"]);
        expect(resultSets3.rowCount).assertEqual(6);
        resultSets3.close();
      } catch (err) {
        console.error(TAG + `Query failed, code is ${err.code},message is ${err.message}`);
        if (rdbStore != undefined) {
          await rdbStore.executeSql(DROP_TBL);
        }
        expect().assertFail();
      }
      console.log(TAG + "************* LikeErrCodeTest_0002 end *************");
    });

    /**
     * @tc.name: likeErrCodeTest_0003
     * @tc.desc: Verify the behavior of the like in DataSharePredicates with various input types
     * @tc.type: FUNC
     * @tc.precon: DataSharePredicates instance is created and table is populated with test data
     * @tc.step:
     *   1. Test like with boolean value on 'age' field
     *   2. Test like with null value on 'age' field
     *   3. Test like with undefined value on 'age' field
     *   4. Test like with array of numbers on 'age' field
     *   5. Test like with empty string on 'age' field
     *   6. Test like with boolean value on 'name' field
     * @tc.expect:
     *   1. Correct row count 1 for boolean value on 'age' field
     *   2. Correct row count 0 for null value on 'age' field
     *   3. Correct row count 0 for undefined value on 'age' field
     *   4. Correct row count 6 for array of numbers on 'age' field
     *   5. Correct row count 6 for empty string on 'age' field
     *   6. Correct row count 0 for boolean value on 'name' field
     */
    it('likeErrCodeTest_0003', 0, async () => {
      console.log(TAG + "************* likeErrCodeTest_0003 start *************");
      try {
        let dp = new dataSharePredicates.DataSharePredicates();
        dp.like('age', false);
        let resultSets = await (rdbStore as relationalStore.RdbStore).query(table_name, dp, ["*"]);
        expect(resultSets.rowCount).assertEqual(1);
        resultSets.close();

        let dp1 = new dataSharePredicates.DataSharePredicates();
        dp1.like('age', null);
        let resultSets1 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp1, ["*"]);
        expect(resultSets1.rowCount).assertEqual(0);
        resultSets1.close();

        let dp2 = new dataSharePredicates.DataSharePredicates();
        dp2.like('age', undefined);
        let resultSets2 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp2, ["*"]);
        expect(resultSets2.rowCount).assertEqual(0);
        resultSets2.close();

        let dp3 = new dataSharePredicates.DataSharePredicates();
        dp3.like('age', [20, 22]);
        let resultSets3 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp3, ["*"]);
        expect(resultSets3.rowCount).assertEqual(6);
        resultSets3.close();

        let dp4 = new dataSharePredicates.DataSharePredicates();
        dp4.like('age', '');
        let resultSets4 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp4, ["*"]);
        expect(resultSets4.rowCount).assertEqual(6);
        resultSets4.close();

        let dp5 = new dataSharePredicates.DataSharePredicates();
        dp5.like('name', false);
        let resultSets5 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp5, ["*"]);
        expect(resultSets5.rowCount).assertEqual(0);
        resultSets5.close();
      } catch (err) {
        console.error(TAG + `Query failed, code is ${err.code},message is ${err.message}`);
        if (rdbStore != undefined) {
          await rdbStore.executeSql(DROP_TBL);
        }
        expect().assertFail();
      }
      console.log(TAG + "************* likeErrCodeTest_0003 end *************");
    });

    /**
     * @tc.name: NotInErrCodeTest_0001
     * @tc.desc: Verify the behavior of the notIn in DataSharePredicates with various input types
     * @tc.type: FUNC
     * @tc.precon: DataSharePredicates instance is created and table is populated with test data
     * @tc.step:
     *   1. Test notIn with numeric array and expect row count 4
     *   2. Test notIn with invalid parameters and expect an error
     * @tc.expect:
     *   1. Correct row count 4 for numeric array and verify specific row data
     *   2. Error is thrown for invalid parameters
     */
    it('NotInErrCodeTest_0001', 0, async () => {
      console.log(TAG + "************* NotInErrCodeTest_0001 start *************");
      try {
        let dp = new dataSharePredicates.DataSharePredicates();
        dp.notIn('age', [20, 24], [22, 24]);
        let resultSets = await (rdbStore as relationalStore.RdbStore).query(table_name, dp, ["*"]);
        expect(resultSets.rowCount).assertEqual(4);
        expect(resultSets.goToRow(0)).assertEqual(true);
        let id = resultSets.getLong(resultSets.getColumnIndex("id"));
        expect(id).assertEqual(2);
        resultSets.close();

        try {
          let dp1 = new dataSharePredicates.DataSharePredicates();
          dp1.notIn(20);
          expect().assertFail();
        } catch (error1) {
          console.info(TAG + 'error1: ' + JSON.stringify(error1));
        }
      } catch (err) {
        console.error(TAG + `Query failed, code is ${err.code},message is ${err.message}`);
        if (rdbStore != undefined) {
          await rdbStore.executeSql(DROP_TBL);
        }
        expect().assertFail();
      }
      console.log(TAG + "************* NotInErrCodeTest_0001 end *************");
    });

    /**
     * @tc.name: notInErrCodeTest_0002
     * @tc.desc: Verify the behavior of the notIn in DataSharePredicates with various input types
     * @tc.type: FUNC
     * @tc.precon: DataSharePredicates instance is created and table is populated with test data
     * @tc.step:
     *   1. Test notIn with numeric field
     *   2. Test notIn with null and undefined values
     *   3. Test notIn with string 'null'
     *   4. Test notIn with empty string
     * @tc.expect:
     *   1. Correct row count 6 for numeric field
     *   2. Correct row count 6 for null and undefined values
     *   3. Correct row count 0 for string 'null'
     *   4. Correct row count 6 for empty string
     */
    it('notInErrCodeTest_0002', 0, async () => {
      console.log(TAG + "************* notInErrCodeTest_0002 start *************");
      try {
        let dp = new dataSharePredicates.DataSharePredicates();
        dp.notIn(1, [20, 24]);
        let resultSets = await (rdbStore as relationalStore.RdbStore).query(table_name, dp, ["*"]);
        expect(resultSets.rowCount).assertEqual(6);
        resultSets.close();

        let dp1 = new dataSharePredicates.DataSharePredicates();
        dp1.notIn(null, [20, 24])
          .or()
          .notIn(undefined, [20, 24]);
        let resultSets1 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp1, ["*"]);
        expect(resultSets1.rowCount).assertEqual(6);
        resultSets1.close();

        let dp2 = new dataSharePredicates.DataSharePredicates();
        dp2.notIn('null', [20, 24]);
        let resultSets2 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp2, ["*"]);
        expect(resultSets2.rowCount).assertEqual(0);
        resultSets2.close();

        let dp3 = new dataSharePredicates.DataSharePredicates();
        dp3.notIn('', [20, 24]);
        let resultSets3 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp3, ["*"]);
        expect(resultSets3.rowCount).assertEqual(6);
        resultSets3.close();
      } catch (err) {
        console.error(TAG + `Query failed, code is ${err.code},message is ${err.message}`);
        if (rdbStore != undefined) {
          await rdbStore.executeSql(DROP_TBL);
        }
        expect().assertFail();
      }
      console.log(TAG + "************* notInErrCodeTest_0002 end *************");
    });

    /**
     * @tc.name: notInErrCodeTest_0003
     * @tc.desc: Verify the behavior of the notIn in DataSharePredicates with various input types
     * @tc.type: FUNC
     * @tc.precon: DataSharePredicates instance is created and table is populated with test data
     * @tc.step:
     *   1. Test notIn with boolean value on 'age' field
     *   2. Test notIn with null value on 'age' field
     *   3. Test notIn with undefined value on 'age' field
     *   4. Test notIn with nested array on 'age' field
     *   5. Test notIn with empty string on 'age' field
     *   6. Test notIn with string array on 'age' field
     * @tc.expect:
     *   1. Correct row count 6 for boolean value on 'age' field
     *   2. Correct row count 6 for null value on 'age' field
     *   3. Correct row count 6 for undefined value on 'age' field
     *   4. Correct row count 6 for nested array on 'age' field
     *   5. Correct row count 6 for empty string on 'age' field
     *   6. Correct row count 5 for string array on 'age' field
     */
    it('notInErrCodeTest_0003', 0, async () => {
      console.log(TAG + "************* notInErrCodeTest_0003 start *************");
      try {
        let dp = new dataSharePredicates.DataSharePredicates();
        dp.notIn('age', false);
        let resultSets = await (rdbStore as relationalStore.RdbStore).query(table_name, dp, ["*"]);
        expect(resultSets.rowCount).assertEqual(6);
        resultSets.close();

        let dp1 = new dataSharePredicates.DataSharePredicates();
        dp1.notIn('age', null);
        let resultSets1 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp1, ["*"]);
        expect(resultSets1.rowCount).assertEqual(6);
        resultSets1.close();

        let dp2 = new dataSharePredicates.DataSharePredicates();
        dp2.notIn('age', undefined);
        let resultSets2 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp2, ["*"]);
        expect(resultSets2.rowCount).assertEqual(6);
        resultSets2.close();

        let dp3 = new dataSharePredicates.DataSharePredicates();
        dp3.notIn('age', [[20, 22]]);
        let resultSets3 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp3, ["*"]);
        expect(resultSets3.rowCount).assertEqual(6);
        resultSets3.close();

        let dp4 = new dataSharePredicates.DataSharePredicates();
        dp4.notIn('age', '');
        let resultSets4 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp4, ["*"]);
        expect(resultSets4.rowCount).assertEqual(6);
        resultSets4.close();

        let dp5 = new dataSharePredicates.DataSharePredicates();
        dp5.notIn('age', ['ZhangSan', '20']);
        let resultSets5 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp5, ["*"]);
        expect(resultSets5.rowCount).assertEqual(5);
        resultSets5.close();
      } catch (err) {
        console.error(TAG + `Query failed, code is ${err.code},message is ${err.message}`);
        if (rdbStore != undefined) {
          await rdbStore.executeSql(DROP_TBL);
        }
        expect().assertFail();
      }
      console.log(TAG + "************* notInErrCodeTest_0003 end *************");
    });

    /**
     * @tc.name: BetweenErrCodeTest_0001
     * @tc.desc: Verify the behavior of the between in DataSharePredicates with various input types
     * @tc.type: FUNC
     * @tc.precon: DataSharePredicates instance is created and table is populated with test data
     * @tc.step:
     *   1. Test between with numeric range and expect row count 4
     *   2. Test between with invalid parameters and expect an error
     * @tc.expect:
     *   1. Correct row count 4 for numeric range and verify specific row data
     *   2. Error is thrown for invalid parameters
     */
    it('BetweenErrCodeTest_0001', 0, async () => {
      console.log(TAG + "************* BetweenErrCodeTest_0001 start *************");
      try {
        let dp = new dataSharePredicates.DataSharePredicates();
        dp.between('age', 20, 23, false);
        let resultSets = await (rdbStore as relationalStore.RdbStore).query(table_name, dp, ["*"]);
        expect(resultSets.rowCount).assertEqual(4);
        expect(resultSets.goToRow(0)).assertEqual(true);
        let id = resultSets.getLong(resultSets.getColumnIndex("id"));
        expect(id).assertEqual(1);
        resultSets.close();

        try {
          let dp1 = new dataSharePredicates.DataSharePredicates();
          dp1.between(20);
          expect().assertFail();
        } catch (error1) {
          console.info(TAG + 'error1: ' + JSON.stringify(error1));
        }
      } catch (err) {
        console.error(TAG + `Query failed, code is ${err.code},message is ${err.message}`);
        if (rdbStore != undefined) {
          await rdbStore.executeSql(DROP_TBL);
        }
        expect().assertFail();
      }
      console.log(TAG + "************* BetweenErrCodeTest_0001 end *************");
    });

    /**
     * @tc.name: BetweenErrCodeTest_0002
     * @tc.desc: Verify the behavior of the between in DataSharePredicates with various input types
     * @tc.type: FUNC
     * @tc.precon: DataSharePredicates instance is created and table is populated with test data
     * @tc.step:
     *   1. Test between with numeric field
     *   2. Test between with null and undefined values
     *   3. Test between with string 'null'
     *   4. Test between with empty string
     * @tc.expect:
     *   1. Correct row count 6 for numeric field
     *   2. Correct row count 6 for null and undefined values
     *   3. Correct row count 0 for string 'null'
     *   4. Correct row count 6 for empty string
     */
    it('BetweenErrCodeTest_0002', 0, async () => {
      console.log(TAG + "************* BetweenErrCodeTest_0002 start *************");
      try {
        let dp = new dataSharePredicates.DataSharePredicates();
        dp.between(1, 20, 23);
        let resultSets = await (rdbStore as relationalStore.RdbStore).query(table_name, dp, ["*"]);
        expect(resultSets.rowCount).assertEqual(6);
        resultSets.close();

        let dp1 = new dataSharePredicates.DataSharePredicates();
        dp1.between(null, 20, 23)
          .or()
          .between(undefined, 20, 23);
        let resultSets1 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp1, ["*"]);
        expect(resultSets1.rowCount).assertEqual(6);
        resultSets1.close();

        let dp2 = new dataSharePredicates.DataSharePredicates();
        dp2.between('null', 20, 23);
        let resultSets2 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp2, ["*"]);
        expect(resultSets2.rowCount).assertEqual(0);
        resultSets2.close();

        let dp3 = new dataSharePredicates.DataSharePredicates();
        dp3.between('', 20, 23);
        let resultSets3 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp3, ["*"]);
        expect(resultSets3.rowCount).assertEqual(6);
        resultSets3.close();
      } catch (err) {
        console.error(TAG + `Query failed, code is ${err.code},message is ${err.message}`);
        if (rdbStore != undefined) {
          await rdbStore.executeSql(DROP_TBL);
        }
        expect().assertFail();
      }
      console.log(TAG + "************* BetweenErrCodeTest_0002 end *************");
    });

    /**
     * @tc.name: BetweenErrCodeTest_0003
     * @tc.desc: Verify the behavior of the between in DataSharePredicates with various input types
     * @tc.type: FUNC
     * @tc.precon: DataSharePredicates instance is created and table is populated with test data
     * @tc.step:
     *   1. Test between with boolean value on 'age' field
     *   2. Test between with null value on 'age' field
     *   3. Test between with undefined value on 'age' field
     *   4. Test between with string number on 'age' field
     *   5. Test between with empty string on 'age' field
     *   6. Test between with reversed range on 'age' field
     * @tc.expect:
     *   1. Correct row count 4 for boolean value on 'age' field
     *   2. Correct row count 0 for null value on 'age' field
     *   3. Correct row count 0 for undefined value on 'age' field
     *   4. Correct row count 5 for string number on 'age' field
     *   5. Correct row count 6 for empty string on 'age' field
     *   6. Correct row count 0 for reversed range on 'age' field
     */
    it('BetweenErrCodeTest_0003', 0, async () => {
      console.log(TAG + "************* BetweenErrCodeTest_0003 start *************");
      try {
        let dp = new dataSharePredicates.DataSharePredicates();
        dp.between('age', false, 22);
        let resultSets = await (rdbStore as relationalStore.RdbStore).query(table_name, dp, ["*"]);
        expect(resultSets.rowCount).assertEqual(4);
        resultSets.close();

        let dp1 = new dataSharePredicates.DataSharePredicates();
        dp1.between('age', null, 24);
        let resultSets1 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp1, ["*"]);
        expect(resultSets1.rowCount).assertEqual(0);
        resultSets1.close();

        let dp2 = new dataSharePredicates.DataSharePredicates();
        dp2.between('age', undefined, 24);
        let resultSets2 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp2, ["*"]);
        expect(resultSets2.rowCount).assertEqual(0);
        resultSets2.close();

        let dp3 = new dataSharePredicates.DataSharePredicates();
        dp3.between('age', '20', 24);
        let resultSets3 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp3, ["*"]);
        expect(resultSets3.rowCount).assertEqual(5);
        resultSets3.close();

        let dp4 = new dataSharePredicates.DataSharePredicates();
        dp4.between('age', '', 24);
        let resultSets4 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp4, ["*"]);
        expect(resultSets4.rowCount).assertEqual(6);
        resultSets4.close();

        let dp5 = new dataSharePredicates.DataSharePredicates();
        dp5.between('age', 24, 20);
        let resultSets5 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp5, ["*"]);
        expect(resultSets5.rowCount).assertEqual(0);
        resultSets5.close();
      } catch (err) {
        console.error(TAG + `Query failed, code is ${err.code},message is ${err.message}`);
        if (rdbStore != undefined) {
          await rdbStore.executeSql(DROP_TBL);
        }
        expect().assertFail();
      }
      console.log(TAG + "************* BetweenErrCodeTest_0003 end *************");
    });

    /**
     * @tc.name: NotBetweenErrCodeTest_0001
     * @tc.desc: Verify the behavior of the notBetween in DataSharePredicates with various input types
     * @tc.type: FUNC
     * @tc.precon: DataSharePredicates instance is created and table is populated with test data
     * @tc.step:
     *   1. Test notBetween with numeric range and expect row count 2
     *   2. Test notBetween with invalid parameters and expect an error
     * @tc.expect:
     *   1. Correct row count 2 for numeric range and verify specific row data
     *   2. Error is thrown for invalid parameters
     */
    it('NotBetweenErrCodeTest_0001', 0, async () => {
      console.log(TAG + "************* NotBetweenErrCodeTest_0001 start *************");
      try {
        let dp = new dataSharePredicates.DataSharePredicates();
        dp.notBetween('age', 20, 23, false);
        let resultSets = await (rdbStore as relationalStore.RdbStore).query(table_name, dp, ["*"]);
        expect(resultSets.rowCount).assertEqual(2);
        expect(resultSets.goToRow(0)).assertEqual(true);
        let id = resultSets.getLong(resultSets.getColumnIndex("id"));
        expect(id).assertEqual(5);
        resultSets.close();

        try {
          let dp1 = new dataSharePredicates.DataSharePredicates();
          dp1.notBetween(20);
          expect().assertFail();
        } catch (error1) {
          console.info(TAG + 'error1: ' + JSON.stringify(error1));
        }
      } catch (err) {
        console.error(TAG + `Query failed, code is ${err.code},message is ${err.message}`);
        if (rdbStore != undefined) {
          await rdbStore.executeSql(DROP_TBL);
        }
        expect().assertFail();
      }
      console.log(TAG + "************* NotBetweenErrCodeTest_0001 end *************");
    });

    /**
     * @tc.name: NotBetweenErrCodeTest_0002
     * @tc.desc: Verify the behavior of the notBetween in DataSharePredicates with various input types
     * @tc.type: FUNC
     * @tc.precon: DataSharePredicates instance is created and table is populated with test data
     * @tc.step:
     *   1. Test notBetween with numeric field
     *   2. Test notBetween with null and undefined values
     *   3. Test notBetween with string 'null'
     *   4. Test notBetween with empty string
     * @tc.expect:
     *   1. Correct row count 6 for numeric field
     *   2. Correct row count 6 for null and undefined values
     *   3. Correct row count 0 for string 'null'
     *   4. Correct row count 6 for empty string
     */
    it('NotBetweenErrCodeTest_0002', 0, async () => {
      console.log(TAG + "************* NotBetweenErrCodeTest_0002 start *************");
      try {
        let dp = new dataSharePredicates.DataSharePredicates();
        dp.notBetween(1, 20, 23);
        let resultSets = await (rdbStore as relationalStore.RdbStore).query(table_name, dp, ["*"]);
        expect(resultSets.rowCount).assertEqual(6);
        resultSets.close();

        let dp1 = new dataSharePredicates.DataSharePredicates();
        dp1.notBetween(null, 20, 23)
          .or()
          .notBetween(undefined, 20, 23);
        let resultSets1 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp1, ["*"]);
        expect(resultSets1.rowCount).assertEqual(6);
        resultSets1.close();

        let dp2 = new dataSharePredicates.DataSharePredicates();
        dp2.notBetween('null', 20, 23);
        let resultSets2 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp2, ["*"]);
        expect(resultSets2.rowCount).assertEqual(0);
        resultSets2.close();

        let dp3 = new dataSharePredicates.DataSharePredicates();
        dp3.notBetween('', 20, 23);
        let resultSets3 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp3, ["*"]);
        expect(resultSets3.rowCount).assertEqual(6);
        resultSets3.close();
      } catch (err) {
        console.error(TAG + `Query failed, code is ${err.code},message is ${err.message}`);
        if (rdbStore != undefined) {
          await rdbStore.executeSql(DROP_TBL);
        }
        expect().assertFail();
      }
      console.log(TAG + "************* NotBetweenErrCodeTest_0002 end *************");
    });

    /**
     * @tc.name: NotBetweenErrCodeTest_0003
     * @tc.desc: Verify the behavior of the notBetween in DataSharePredicates with various input types
     * @tc.type: FUNC
     * @tc.precon: DataSharePredicates instance is created and table is populated with test data
     * @tc.step:
     *   1. Test notBetween with boolean value on 'age' field
     *   2. Test notBetween with null value on 'age' field
     *   3. Test notBetween with undefined value on 'age' field
     *   4. Test notBetween with string number on 'age' field
     *   5. Test notBetween with empty string on 'age' field
     *   6. Test notBetween with reversed range on 'age' field
     * @tc.expect:
     *   1. Correct row count 2 for boolean value on 'age' field
     *   2. Correct row count 6 for null value on 'age' field
     *   3. Correct row count 6 for undefined value on 'age' field
     *   4. Correct row count 3 for string number on 'age' field
     *   5. Correct row count 6 for empty string on 'age' field
     *   6. Correct row count 6 for reversed range on 'age' field
     */
    it('NotBetweenErrCodeTest_0003', 0, async () => {
      console.log(TAG + "************* NotBetweenErrCodeTest_0003 start *************");
      try {
        let dp = new dataSharePredicates.DataSharePredicates();
        dp.notBetween('age', false, 22);
        let resultSets = await (rdbStore as relationalStore.RdbStore).query(table_name, dp, ["*"]);
        expect(resultSets.rowCount).assertEqual(2);
        resultSets.close();

        let dp1 = new dataSharePredicates.DataSharePredicates();
        dp1.notBetween('age', null, 22);
        let resultSets1 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp1, ["*"]);
        expect(resultSets1.rowCount).assertEqual(6);
        resultSets1.close();

        let dp2 = new dataSharePredicates.DataSharePredicates();
        dp2.notBetween('age', undefined, 22);
        let resultSets2 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp2, ["*"]);
        expect(resultSets2.rowCount).assertEqual(6);
        resultSets2.close();

        let dp3 = new dataSharePredicates.DataSharePredicates();
        dp3.notBetween('age', '20', 22);
        let resultSets3 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp3, ["*"]);
        expect(resultSets3.rowCount).assertEqual(3);
        resultSets3.close();

        let dp4 = new dataSharePredicates.DataSharePredicates();
        dp4.notBetween('age', '', 22);
        let resultSets4 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp4, ["*"]);
        expect(resultSets4.rowCount).assertEqual(6);
        resultSets4.close();

        let dp5 = new dataSharePredicates.DataSharePredicates();
        dp5.notBetween('age', 24, 20);
        let resultSets5 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp5, ["*"]);
        expect(resultSets5.rowCount).assertEqual(6);
        resultSets5.close();
      } catch (err) {
        console.error(TAG + `Query failed, code is ${err.code},message is ${err.message}`);
        if (rdbStore != undefined) {
          await rdbStore.executeSql(DROP_TBL);
        }
        expect().assertFail();
      }
      console.log(TAG + "************* NotBetweenErrCodeTest_0003 end *************");
    });

    /**
     * @tc.name: BeginWrapEndWrapOrErrCodeTest_0001
     * @tc.desc: Test logical operations and wrapping in DataSharePredicates
     * @tc.type: FUNC
     * @tc.precon: DataSharePredicates instance and populated table
     * @tc.step:
     *   1. Create predicates with notEqualTo('age', 14)
     *   2. Begin wrap, add isStudent = true
     *   3. Add OR operator, then name = 'SunLiu'
     *   4. End wrap
     *   5. Query and verify result set
     * @tc.expect:
     *   1. Correct predicates setup
     *   2. Successful query with expected row count
     *   3. Correct first row id
     */
    it('BeginWrapEndWrapOrErrCodeTest_0001', 0, async () => {
      console.log(TAG + "************* BeginWrapEndWrapOrErrCodeTest_0001 start *************");
      try {
        let dp = new dataSharePredicates.DataSharePredicates();
        dp.notEqualTo('age', 14)
          .beginWrap('111')
          .equalTo('isStudent', true)
          .or('222')
          .equalTo('name', 'SunLiu')
          .endWrap('333');
        let resultSets = await (rdbStore as relationalStore.RdbStore).query(table_name, dp, ["*"]);
        expect(resultSets.rowCount).assertEqual(5);
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
      console.log(TAG + "************* BeginWrapEndWrapOrErrCodeTest_0001 end *************");
    });

    /**
     * @tc.name: BeginWrapEndWrapOrErrCodeTest_0002
     * @tc.desc: Verify query behavior with beginWrap, endWrap, and OR conditions
     * @tc.type: FUNC
     * @tc.precon: RdbStore instance is created and table is initialized
     * @tc.step:
     *   1. Create a query with notEqualTo and beginWrap conditions
     *   2. Execute the query and verify the result set
     *   3. Create another query with endWrap and OR conditions
     *   4. Execute the second query and verify the result set
     * @tc.expect: Query executes successfully and returns expected results
     */
    it('BeginWrapEndWrapOrErrCodeTest_0002', 0, async () => {
      console.log(TAG + "************* BeginWrapEndWrapOrErrCodeTest_0002 start *************");
      try {
        let dp = new dataSharePredicates.DataSharePredicates();
        dp.notEqualTo('age', 14)
          .beginWrap('111')
          .equalTo('isStudent', true)
          .or('222')
          .equalTo('name', 'SunLiu');
        let resultSets = await (rdbStore as relationalStore.RdbStore).query(table_name, dp, ["*"]);
        expect(resultSets.rowCount).assertEqual(-1);
        resultSets.close();

        let dp1 = new dataSharePredicates.DataSharePredicates();
        dp1.notEqualTo('age', 14)
          .equalTo('isStudent', true)
          .or('222')
          .equalTo('name', 'SunLiu')
          .endWrap(22);
        let resultSets1 = await (rdbStore as relationalStore.RdbStore).query(table_name, dp1, ["*"]);
        expect(resultSets1.rowCount).assertEqual(-1);
        resultSets.close();
      } catch (err) {
        console.error(TAG + `Query failed, code is ${err.code},message is ${err.message}`);
        if (rdbStore != undefined) {
          await rdbStore.executeSql(DROP_TBL);
        }
        expect().assertFail();
      }
      console.log(TAG + "************* BeginWrapEndWrapOrErrCodeTest_0002 end *************");
    });
  });
}