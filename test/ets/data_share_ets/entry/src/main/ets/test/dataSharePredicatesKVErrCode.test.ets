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
import distributedKVStore from '@ohos.data.distributedKVStore';
import dataSharePredicates from "@ohos.data.dataSharePredicates";
import AbilityDelegatorRegistry from '@ohos.app.ability.abilityDelegatorRegistry';
import { BusinessError } from "@ohos.base";

const delegator = AbilityDelegatorRegistry.getAbilityDelegator();
const context = delegator.getAppContext().getApplicationContext();

const TEST_BUNDLE_NAME = 'com.acts.ohos.data.datasharetestclient1';
const TEST_STORE_ID = 'SingleKVStoreErrCodeTest';

let TAG = "[dataSharePredicateKVErrCode_test]";

let kvManager: distributedKVStore.KVManager | null = null;
let kvStore: distributedKVStore.SingleKVStore | undefined = undefined;

const key1 = 'key_test_string1';
const value1 = '{"id":1, "name":"ZhangSan", "age":20, "isStudent":false}';

const key2 = 'key_test_string2';
const value2 = '{"id":2, "name":"LiSi", "age":20, "isStudent":true}';

const key3 = 'key_test_string3';
const value3 = '{"id":3, "name":"WangWu", "age":21, "isStudent":true}';

const key4 = 'key_test_string4';
const value4 = '{"id":4, "name":"SunLiu", "age":22, "isStudent":false}';

const key5 = 'key_test_string5';
const value5 = '{"id":5, "name":"MaQi", "age":23, "isStudent":true}';

const key6 = 'key_test_string6';
const value6 = '{"id":0, "name":"Error", "age":0, "isStudent":false}';


function sleep(ms: number) {
  return new Promise<ESObject>(resolve => setTimeout(resolve, ms));
}

const config: distributedKVStore.KVManagerConfig = {
  bundleName: TEST_BUNDLE_NAME,
  context: context
};

async function createKVStore() {
  try {
    let child1 = new distributedKVStore.FieldNode('id');
    child1.type = distributedKVStore.ValueType.INTEGER;
    child1.nullable = false;
    child1.default = '0';
    let child2 = new distributedKVStore.FieldNode('name');
    child2.type = distributedKVStore.ValueType.STRING;
    child2.nullable = false;
    child2.default = 'name';
    let child3 = new distributedKVStore.FieldNode('age');
    child3.type = distributedKVStore.ValueType.INTEGER;
    child3.nullable = false;
    child3.default = '0';
    let child4 = new distributedKVStore.FieldNode('isStudent');
    child4.type = distributedKVStore.ValueType.BOOLEAN;
    child4.nullable = false;
    child4.default = 'false';

    let schema = new distributedKVStore.Schema();
    schema.root.appendChild(child1);
    schema.root.appendChild(child2);
    schema.root.appendChild(child3);
    schema.root.appendChild(child4);
    schema.indexes = ['$.id', '$.name', '$.age', '$.isStudent'];
    schema.mode = 1;
    schema.skip = 0;

    const options: distributedKVStore.Options = {
      createIfMissing: true,
      encrypt: true,
      backup: false,
      autoSync: false,
      kvStoreType: distributedKVStore.KVStoreType.SINGLE_VERSION,
      schema: schema,
      securityLevel: distributedKVStore.SecurityLevel.S3
    };
    try {
      kvStore = await kvManager?.getKVStore<distributedKVStore.SingleKVStore>(TEST_STORE_ID, options);
    } catch (error) {
      console.error(TAG + `get store failed, errCode:${error.code}, message:${error.message}`);
      expect().assertFail();
    }
  } catch (e) {
    let error = e as BusinessError;
    console.error(`An unexpected error occurred. Code:${error.code},message:${error.message}`);
  }
}

async function generateKVStore() {
  console.info(TAG + 'generateKVStore');
  await createKVStore();
  if (kvStore == undefined) {
    console.error(TAG + 'kvStore is undefined');
    return;
  }

  try {
    await kvStore?.put(key1, value1);
    await kvStore?.put(key2, value2);
    await kvStore?.put(key3, value3);
    await kvStore?.put(key4, value4);
    await kvStore?.put(key5, value5);
    await kvStore?.put(key6, value6);
  } catch (e) {
    console.error(TAG + `insert data failed, errCode: ${e.code}, message: ${e.message}`);
    expect().assertFail();
  }
}

export default function DataSharePredicatesKVErrCodeEtsTest() {
  describe('DataSharePredicatesKVErrCodeEtsTest', () => {

    beforeAll(async () => {
      console.info(TAG + 'beforeAll');
      try {
        kvManager = distributedKVStore.createKVManager(config);
      } catch (e) {
        console.error(TAG + `Failed to create KVManager, code ${e.code},message ${e.message}`);
        expect().assertFail();
      }
      try {
        await sleep(300);
        await generateKVStore();
      } catch (e) {
        console.error(TAG + `Failed to create kvStore, code ${e.code},message ${e.message}`);
        expect().assertFail();
      }
    });

    beforeEach(async () => {
      console.info(TAG + 'beforeEach');
    });

    afterEach(async () => {
      console.info(TAG + 'afterEach');
    });

    afterAll(async () => {
      console.info(TAG + 'afterAll');
      try {
        await sleep(300);
        await kvManager!.closeKVStore(TEST_BUNDLE_NAME, TEST_STORE_ID);
        await kvManager!.deleteKVStore(TEST_BUNDLE_NAME, TEST_STORE_ID);
      } catch (e) {
        console.info(TAG + `Failed to close and delete kvStore code ${e.code},message ${e.message}`);
      }
      kvManager = null;
      kvStore = undefined;
    });

    /**
     * @tc.name: notEqualToErrCodeTest_0001
     * @tc.desc: Verify the behavior of the notEqualTo in DataSharePredicates with various input types
     * @tc.type: FUNC
     * @tc.precon: DataSharePredicates instance is created and table is populated with test data
     * @tc.step:
     *   1. Test notEqualTo with string field and expect row count 5
     *   2. Test notEqualTo with numeric field and expect row count 4
     *   3. Test notEqualTo with boolean field and expect row count 3
     *   4. Test notEqualTo with invalid parameters and expect an error
     * @tc.expect:
     *   1. Correct row count 5 for string field
     *   2. Correct row count 4 for numeric field
     *   3. Correct row count 3 for boolean field
     *   4. Error is thrown for invalid parameters
     */
    it('notEqualToErrCodeTest_0001', 0, async () => {
      console.log(TAG + "************* notEqualToErrCodeTest_0001 start *************");
      try {
        let predicates = new dataSharePredicates.DataSharePredicates();
        predicates.notEqualTo('name', 'ZhangSan', 'LiSi');
        let resultSet = await kvStore?.getResultSet(predicates);
        expect(resultSet?.getCount()).assertEqual(5);
        await kvStore?.closeResultSet(resultSet);

        let predicates1 = new dataSharePredicates.DataSharePredicates();
        predicates1.notEqualTo('$.age', 20, 22);
        let resultSet1 = await kvStore?.getResultSet(predicates1);
        expect(resultSet1?.getCount()).assertEqual(4);
        await kvStore?.closeResultSet(resultSet1);

        let predicates2 = new dataSharePredicates.DataSharePredicates();
        predicates2.notEqualTo('$.isStudent', false, true);
        let resultSet2 = await kvStore?.getResultSet(predicates2);
        expect(resultSet2?.getCount()).assertEqual(3);
        await kvStore?.closeResultSet(resultSet2);

        try {
          let predicates3 = new dataSharePredicates.DataSharePredicates();
          predicates3.notEqualTo('$.isStudent');
          expect().assertFail();
        } catch (error1) {
          console.info(TAG + 'error1: ' + JSON.stringify(error1));
        }
      } catch (e) {
        console.error(TAG + `get resultSet failed errCode:${e.code}, message${e.message}`);
        expect().assertFail();
      }
      console.log(TAG + "************* notEqualToErrCodeTest_0001 end *************");
    });

    /**
     * @tc.name: notEqualToErrCodeTest_0002
     * @tc.desc: Verify the behavior of the notEqualTo in DataSharePredicates with various input types
     * @tc.type: FUNC
     * @tc.precon: DataSharePredicates instance is created and table is populated with test data
     * @tc.step:
     *   1. Test notEqualTo with numeric field - expect row count 6
     *   2. Test notEqualTo with null value - expect row count 6
     *   3. Test notEqualTo with string 'null' - expect row count 0
     *   4. Test notEqualTo with empty string - expect row count 6
     *   5. Test notEqualTo with undefined value - expect row count 6
     * @tc.expect:
     *   1. Correct row count 6 for numeric field
     *   2. Correct row count 6 for null value
     *   3. Correct row count 0 for string 'null'
     *   4. Correct row count 6 for empty string
     *   5. Correct row count 6 for undefined value
     */
    it('notEqualToErrCodeTest_0002', 0, async () => {
      console.log(TAG + "************* notEqualToErrCodeTest_0002 start *************");
      try {
        let predicates = new dataSharePredicates.DataSharePredicates();
        predicates.notEqualTo(1, 22);
        let resultSet = await kvStore?.getResultSet(predicates);
        expect(resultSet?.getCount()).assertEqual(6);
        await kvStore?.closeResultSet(resultSet);

        let predicates1 = new dataSharePredicates.DataSharePredicates();
        predicates1.notEqualTo(null, 22);
        let resultSet1 = await kvStore?.getResultSet(predicates1);
        expect(resultSet1?.getCount()).assertEqual(6);
        await kvStore?.closeResultSet(resultSet1);

        try {
          let predicates2 = new dataSharePredicates.DataSharePredicates();
          predicates2.notEqualTo('null', 22);
          let resultSet2 = await kvStore?.getResultSet(predicates2);
          expect(resultSet2?.getCount()).assertEqual(0);
          await kvStore?.closeResultSet(resultSet2);
          expect().assertFail();
        } catch (error1) {
          console.info(TAG + `error1 errCode: ${error1.code}, message: ${error1.message}`);
        }

        let predicates3 = new dataSharePredicates.DataSharePredicates();
        predicates3.notEqualTo('', 22);
        let resultSet3 = await kvStore?.getResultSet(predicates3);
        expect(resultSet3?.getCount()).assertEqual(6);
        await kvStore?.closeResultSet(resultSet3);

        try {
          let predicates4 = new dataSharePredicates.DataSharePredicates();
          predicates4.notEqualTo(undefined, 22);
          let resultSet4 = await kvStore?.getResultSet(predicates4);
          expect(resultSet4?.getCount()).assertEqual(6);
          await kvStore?.closeResultSet(resultSet4);
        } catch (error2) {
          console.info(TAG + `error2 errCode: ${error2.code}, message: ${error2.message}`);
          expect().assertFail();
        }
      } catch (e) {
        console.error(TAG + `get resultSet failed errCode: ${e.code}, message: ${e.message}`);
        expect().assertFail();
      }
      console.log(TAG + "************* notEqualToErrCodeTest_0002 end *************");
    });

    /**
     * @tc.name: notEqualToErrCodeTest_0003
     * @tc.desc: Verify the behavior of the notEqualTo in DataSharePredicates with various input types
     * @tc.type: FUNC
     * @tc.precon: DataSharePredicates instance is created and table is populated with test data
     * @tc.step:
     *   1. Test notEqualTo with numeric array - expect row count 6
     *   2. Test notEqualTo with null value - expect row count 6
     *   3. Test notEqualTo with empty string for age - expect row count 6
     *   4. Test notEqualTo with boolean false for age - expect row count 5
     *   5. Test notEqualTo with string "20" for age - expect row count 4
     * @tc.expect:
     *   1. Correct row count 6 for numeric array
     *   2. Correct row count 6 for null value
     *   3. Correct row count 6 for empty string in age
     *   4. Correct row count 5 for boolean false in age
     *   5. Correct row count 4 for string "20" in age
     */
    it('notEqualToErrCodeTest_0003', 0, async () => {
      console.log(TAG + "************* notEqualToErrCodeTest_0003 start *************");
      try {
        let predicates = new dataSharePredicates.DataSharePredicates();
        predicates.notEqualTo('age', [20, 22]);
        let resultSet = await kvStore?.getResultSet(predicates);
        expect(resultSet?.getCount()).assertEqual(6);
        await kvStore?.closeResultSet(resultSet);

        let predicates1 = new dataSharePredicates.DataSharePredicates();
        predicates1.notEqualTo('age', null);
        let resultSet1 = await kvStore?.getResultSet(predicates1);
        expect(resultSet1?.getCount()).assertEqual(6);
        await kvStore?.closeResultSet(resultSet1);

        let predicates2 = new dataSharePredicates.DataSharePredicates();
        predicates2.notEqualTo('age', '');
        let resultSet2 = await kvStore?.getResultSet(predicates2);
        expect(resultSet2?.getCount()).assertEqual(6);
        await kvStore?.closeResultSet(resultSet2);

        let predicates3 = new dataSharePredicates.DataSharePredicates();
        predicates3.notEqualTo('age', false);
        let resultSet3 = await kvStore?.getResultSet(predicates3);
        expect(resultSet3?.getCount()).assertEqual(5);
        await kvStore?.closeResultSet(resultSet3);

        let predicates4 = new dataSharePredicates.DataSharePredicates();
        predicates4.notEqualTo('age', '20');
        let resultSet4 = await kvStore?.getResultSet(predicates4);
        expect(resultSet4?.getCount()).assertEqual(4);
        await kvStore?.closeResultSet(resultSet4);
      } catch (e) {
        console.error(TAG + `get resultSet failed errCode:${e.code}, message:${e.message}`);
        expect().assertFail();
      }
      console.log(TAG + "************* notEqualToErrCodeTest_0003 end *************");
    });

    /**
     * @tc.name: greaterThanErrCodeTest_0001
     * @tc.desc: Verify the behavior of the greaterThan in DataSharePredicates with various input types
     * @tc.type: FUNC
     * @tc.precon: DataSharePredicates instance is created and table is populated with test data
     * @tc.step:
     *   1. Test greaterThan with string field and expect row count 3
     *   2. Test greaterThan with numeric field and expect row count 2
     *   3. Test greaterThan with boolean field and expect row count 6
     *   4. Test greaterThan with invalid parameters and expect an error
     * @tc.expect:
     *   1. Correct row count 3 for string field
     *   2. Correct row count 2 for numeric field
     *   3. Correct row count 6 for boolean field
     *   4. Error is thrown for invalid parameters
     */
    it('greaterThanErrCodeTest_0001', 0, async () => {
      console.log(TAG + "************* greaterThanErrCodeTest_0001 start *************");
      try {
        let predicates = new dataSharePredicates.DataSharePredicates();
        predicates.greaterThan('$.name', 'MaQi', 'LiSi');
        let resultSet = await kvStore?.getResultSet(predicates);
        expect(resultSet?.getCount()).assertEqual(3);
        await kvStore?.closeResultSet(resultSet);

        let predicates1 = new dataSharePredicates.DataSharePredicates();
        predicates1.greaterThan('age', 21, 22);
        let resultSet1 = await kvStore?.getResultSet(predicates1);
        expect(resultSet1?.getCount()).assertEqual(2);
        await kvStore?.closeResultSet(resultSet1);

        let predicates2 = new dataSharePredicates.DataSharePredicates();
        predicates2.greaterThan('$.isStudent', true, false);
        let resultSet2 = await kvStore?.getResultSet(predicates2);
        expect(resultSet2?.getCount()).assertEqual(6);
        await kvStore?.closeResultSet(resultSet2);

        try {
          let predicates3 = new dataSharePredicates.DataSharePredicates();
          predicates3.greaterThan(-1);
          expect().assertFail();
        } catch (error1) {
          console.info(TAG + 'error1: ' + JSON.stringify(error1));
        }
      } catch (e) {
        console.error(TAG + `get resultSet failed errCode:${e.code}, message:${e.message}`);
        expect().assertFail();
      }
      console.log(TAG + "************* greaterThanErrCodeTest_0001 end *************");
    });

    /**
     * @tc.name: greaterThanErrCodeTest_0002
     * @tc.desc: Verify the behavior of the greaterThan in DataSharePredicates with various input types
     * @tc.type: FUNC
     * @tc.precon: DataSharePredicates instance is created and table is populated with test data
     * @tc.step:
     *   1. Test greaterThan with numeric field - expect row count 6
     *   2. Test greaterThan with null value - expect row count 6
     *   3. Test greaterThan with string 'null' - expect row count 0
     *   4. Test greaterThan with empty string - expect row count 6
     *   5. Test greaterThan with undefined value - expect row count 6
     * @tc.expect:
     *   1. Correct row count 6 for numeric field
     *   2. Correct row count 6 for null value
     *   3. Correct row count 0 for string 'null'
     *   4. Correct row count 6 for empty string
     *   5. Correct row count 6 for undefined value
     */
    it('greaterThanErrCodeTest_0002', 0, async () => {
      console.log(TAG + "************* greaterThanErrCodeTest_0002 start *************");
      try {
        let predicates = new dataSharePredicates.DataSharePredicates();
        predicates.greaterThan(1, 22);
        let resultSet = await kvStore?.getResultSet(predicates);
        expect(resultSet?.getCount()).assertEqual(6);
        await kvStore?.closeResultSet(resultSet);

        let predicates1 = new dataSharePredicates.DataSharePredicates();
        predicates1.greaterThan(null, 22);
        let resultSet1 = await kvStore?.getResultSet(predicates1);
        expect(resultSet1?.getCount()).assertEqual(6);
        await kvStore?.closeResultSet(resultSet1);

        try {
          let predicates2 = new dataSharePredicates.DataSharePredicates();
          predicates2.greaterThan('null', 22);
          let resultSet2 = await kvStore?.getResultSet(predicates2);
          expect(resultSet2?.getCount()).assertEqual(0);
          await kvStore?.closeResultSet(resultSet2);
          expect().assertFail();
        } catch (error1) {
          console.info(TAG + `error1 errCode: ${error1.code}, message: ${error1.message}`);
        }

        let predicates3 = new dataSharePredicates.DataSharePredicates();
        predicates3.greaterThan('', 22);
        let resultSet3 = await kvStore?.getResultSet(predicates3);
        expect(resultSet3?.getCount()).assertEqual(6);
        await kvStore?.closeResultSet(resultSet3);

        let predicates4 = new dataSharePredicates.DataSharePredicates();
        predicates4.greaterThan(undefined, 'LiSi');
        let resultSet4 = await kvStore?.getResultSet(predicates4);
        expect(resultSet4?.getCount()).assertEqual(6);
        await kvStore?.closeResultSet(resultSet4);
      } catch (e) {
        console.error(TAG + `get resultSet failed errCode:${e.code}, message${e.message}`);
        expect().assertFail();
      }
      console.log(TAG + "************* greaterThanErrCodeTest_0002 end *************");
    });

    /**
     * @tc.name: greaterThanErrCodeTest_0003
     * @tc.desc: Verify the behavior of the greaterThan in DataSharePredicates with various input types
     * @tc.type: FUNC
     * @tc.precon: DataSharePredicates instance is created and table is populated with test data
     * @tc.step:
     *   1. Test greaterThan with numeric array - expect row count 6
     *   2. Test greaterThan with null value - expect row count 6
     *   3. Test greaterThan with empty string for age - expect row count 0
     *   4. Test greaterThan with boolean true for age - expect row count 6
     *   5. Test greaterThan with string "20" for age - expect row count 3
     * @tc.expect:
     *   1. Correct row count 6 for numeric array
     *   2. Correct row count 6 for null value
     *   3. Correct row count 0 for empty string in age
     *   4. Correct row count 6 for boolean true in age
     *   5. Correct row count 3 for string "20" in age
     */
    it('greaterThanErrCodeTest_0003', 0, async () => {
      console.log(TAG + "************* greaterThanErrCodeTest_0003 start *************");
      try {
        let predicates = new dataSharePredicates.DataSharePredicates();
        predicates.greaterThan('age', [1, 2]);
        let resultSet = await kvStore?.getResultSet(predicates);
        expect(resultSet?.getCount()).assertEqual(6);
        await kvStore?.closeResultSet(resultSet);

        let predicates1 = new dataSharePredicates.DataSharePredicates();
        predicates1.greaterThan('age', null);
        let resultSet1 = await kvStore?.getResultSet(predicates1);
        expect(resultSet1?.getCount()).assertEqual(6);
        await kvStore?.closeResultSet(resultSet1);

        let predicates2 = new dataSharePredicates.DataSharePredicates();
        predicates2.greaterThan('age', '');
        let resultSet2 = await kvStore?.getResultSet(predicates2);
        expect(resultSet2?.getCount()).assertEqual(0);
        await kvStore?.closeResultSet(resultSet2);

        let predicates3 = new dataSharePredicates.DataSharePredicates();
        predicates3.greaterThan('age', true);
        let resultSet3 = await kvStore?.getResultSet(predicates3);
        expect(resultSet3?.getCount()).assertEqual(6);
        await kvStore?.closeResultSet(resultSet3);

        let predicates4 = new dataSharePredicates.DataSharePredicates();
        predicates4.greaterThan('age', '20');
        let resultSet4 = await kvStore?.getResultSet(predicates4);
        expect(resultSet4?.getCount()).assertEqual(3);
        await kvStore?.closeResultSet(resultSet4);
      } catch (e) {
        console.error(TAG + `get resultSet failed errCode:${e.code}, message${e.message}`);
        expect().assertFail();
      }
      console.log(TAG + "************* greaterThanErrCodeTest_0003 end *************");
    });

    /**
     * @tc.name: lessThanErrCodeTest_0001
     * @tc.desc: Verify the behavior of the lessThan in DataSharePredicates with various input types
     * @tc.type: FUNC
     * @tc.precon: DataSharePredicates instance is created and table is populated with test data
     * @tc.step:
     *   1. Test lessThan with string field and expect row count 2
     *   2. Test lessThan with numeric field and expect row count 3
     *   3. Test lessThan with boolean field and expect row count 6
     *   4. Test lessThan with invalid parameters and expect an error
     * @tc.expect:
     *   1. Correct row count 2 for string field
     *   2. Correct row count 3 for numeric field
     *   3. Correct row count 6 for boolean field
     *   4. Error is thrown for invalid parameters
     */
    it('lessThanErrCodeTest_0001', 0, async () => {
      console.log(TAG + "************* lessThanErrCodeTest_0001 start *************");
      try {
        let predicates = new dataSharePredicates.DataSharePredicates();
        predicates.lessThan('$.name', 'MaQi', 'SunLiu');
        let resultSet = await kvStore?.getResultSet(predicates);
        expect(resultSet?.getCount()).assertEqual(2);
        await kvStore?.closeResultSet(resultSet);

        let predicates1 = new dataSharePredicates.DataSharePredicates();
        predicates1.lessThan('$.age', 21, -1);
        let resultSet1 = await kvStore?.getResultSet(predicates1);
        expect(resultSet1?.getCount()).assertEqual(3);
        await kvStore?.closeResultSet(resultSet1);

        let predicates2 = new dataSharePredicates.DataSharePredicates();
        predicates2.lessThan('isStudent', false, 3);
        let resultSet2 = await kvStore?.getResultSet(predicates2);
        expect(resultSet2?.getCount()).assertEqual(6);
        await kvStore?.closeResultSet(resultSet2);

        try {
          let predicates3 = new dataSharePredicates.DataSharePredicates();
          predicates3.lessThan(false);
          expect().assertFail();
        } catch (error1) {
          console.info(TAG + 'error1: ' + JSON.stringify(error1));
        }
      } catch (e) {
        console.error(TAG + `get resultSet failed errCode:${e.code}, message${e.message}`);
        expect().assertFail();
      }
      console.log(TAG + "************* lessThanErrCodeTest_0001 end *************");
    });

    /**
     * @tc.name: lessThanErrCodeTest_0002
     * @tc.desc: Verify the behavior of the lessThan in DataSharePredicates with various input types
     * @tc.type: FUNC
     * @tc.precon: DataSharePredicates instance is created and table is populated with test data
     * @tc.step:
     *   1. Test lessThan with numeric field - expect row count 6
     *   2. Test lessThan with null value - expect row count 6
     *   3. Test lessThan with string 'null' - expect row count 0
     *   4. Test lessThan with empty string - expect row count 6
     *   5. Test lessThan with undefined value - expect row count 6
     * @tc.expect:
     *   1. Correct row count 6 for numeric field
     *   2. Correct row count 6 for null value
     *   3. Correct row count 0 for string 'null'
     *   4. Correct row count 6 for empty string
     *   5. Correct row count 6 for undefined value
     */
    it('lessThanErrCodeTest_0002', 0, async () => {
      console.log(TAG + "************* lessThanErrCodeTest_0002 start *************");
      try {
        let predicates = new dataSharePredicates.DataSharePredicates();
        predicates.lessThan(1, 22);
        let resultSet = await kvStore?.getResultSet(predicates);
        expect(resultSet?.getCount()).assertEqual(6);
        await kvStore?.closeResultSet(resultSet);

        let predicates1 = new dataSharePredicates.DataSharePredicates();
        predicates1.lessThan(null, 22);
        let resultSet1 = await kvStore?.getResultSet(predicates1);
        expect(resultSet1?.getCount()).assertEqual(6);
        await kvStore?.closeResultSet(resultSet1);

        try {
          let predicates2 = new dataSharePredicates.DataSharePredicates();
          predicates2.lessThan('null', 22);
          let resultSet2 = await kvStore?.getResultSet(predicates2);
          expect(resultSet2?.getCount()).assertEqual(0);
          await kvStore?.closeResultSet(resultSet2);
          expect().assertFail();
        } catch (error1) {
          console.info(TAG + `error1 errCode: ${error1.code}, message: ${error1.message}`);
        }

        let predicates3 = new dataSharePredicates.DataSharePredicates();
        predicates3.lessThan('', 22);
        let resultSet3 = await kvStore?.getResultSet(predicates3);
        expect(resultSet3?.getCount()).assertEqual(6);
        await kvStore?.closeResultSet(resultSet3);

        let predicates4 = new dataSharePredicates.DataSharePredicates();
        predicates4.lessThan(undefined, 22);
        let resultSet4 = await kvStore?.getResultSet(predicates4);
        expect(resultSet4?.getCount()).assertEqual(6);
        await kvStore?.closeResultSet(resultSet4);
      } catch (e) {
        console.error(TAG + `get resultSet failed errCode:${e.code}, message${e.message}`);
        expect().assertFail();
      }
      console.log(TAG + "************* lessThanErrCodeTest_0002 end *************");
    });

    /**
     * @tc.name: lessThanErrCodeTest_0003
     * @tc.desc: Verify the behavior of the lessThan in DataSharePredicates with various input types
     * @tc.type: FUNC
     * @tc.precon: DataSharePredicates instance is created and table is populated with test data
     * @tc.step:
     *   1. Test lessThan with numeric array - expect row count 6
     *   2. Test lessThan with null value - expect row count 6
     *   3. Test lessThan with empty string for age - expect row count 6
     *   4. Test lessThan with boolean true for age - expect row count 6
     *   5. Test lessThan with string "20" for age - expect row count 1
     * @tc.expect:
     *   1. Correct row count 6 for numeric array
     *   2. Correct row count 6 for null value
     *   3. Correct row count 6 for empty string in age
     *   4. Correct row count 6 for boolean true in age
     *   5. Correct row count 1 for string "20" in age
     */
    it('lessThanErrCodeTest_0003', 0, async () => {
      console.log(TAG + "************* lessThanErrCodeTest_0003 start *************");
      try {
        let predicates = new dataSharePredicates.DataSharePredicates();
        predicates.lessThan('age', [1, 2]);
        let resultSet = await kvStore?.getResultSet(predicates);
        expect(resultSet?.getCount()).assertEqual(6);
        await kvStore?.closeResultSet(resultSet);

        let predicates1 = new dataSharePredicates.DataSharePredicates();
        predicates1.lessThan('age', null);
        let resultSet1 = await kvStore?.getResultSet(predicates1);
        expect(resultSet1?.getCount()).assertEqual(6);
        await kvStore?.closeResultSet(resultSet1);

        let predicates2 = new dataSharePredicates.DataSharePredicates();
        predicates2.lessThan('age', '');
        let resultSet2 = await kvStore?.getResultSet(predicates2);
        expect(resultSet2?.getCount()).assertEqual(6);
        await kvStore?.closeResultSet(resultSet2);

        let predicates3 = new dataSharePredicates.DataSharePredicates();
        predicates3.lessThan('age', true);
        let resultSet3 = await kvStore?.getResultSet(predicates3);
        expect(resultSet3?.getCount()).assertEqual(6);
        await kvStore?.closeResultSet(resultSet3);

        let predicates4 = new dataSharePredicates.DataSharePredicates();
        predicates4.lessThan('age', '20');
        let resultSet4 = await kvStore?.getResultSet(predicates4);
        expect(resultSet4?.getCount()).assertEqual(1);
        await kvStore?.closeResultSet(resultSet4);
      } catch (e) {
        console.error(TAG + `get resultSet failed errCode:${e.code}, message${e.message}`);
        expect().assertFail();
      }
      console.log(TAG + "************* lessThanErrCodeTest_0003 end *************");
    });

    /**
     * @tc.name: greaterThanOrEqualToErrCodeTest_0001
     * @tc.desc: Verify the behavior of the greaterThanOrEqualTo in DataSharePredicates with various input types
     * @tc.type: FUNC
     * @tc.precon: DataSharePredicates instance is created and table is populated with test data
     * @tc.step:
     *   1. Test greaterThanOrEqualTo with string field - expect row count 4
     *   2. Test greaterThanOrEqualTo with numeric field - expect row count 3
     *   3. Test greaterThanOrEqualTo with boolean field - expect row count 6
     *   4. Test greaterThanOrEqualTo with invalid parameters and expect an error
     * @tc.expect:
     *   1. Correct row count 4 for string field
     *   2. Correct row count 3 for numeric field
     *   3. Correct row count 6 for boolean field
     *   4. Error is thrown for invalid parameters
     */
    it('greaterThanOrEqualToErrCodeTest_0001', 0, async () => {
      console.log(TAG + "************* greaterThanOrEqualToErrCodeTest_0001 start *************");
      try {
        let predicates = new dataSharePredicates.DataSharePredicates();
        predicates.greaterThanOrEqualTo('$.name', 'MaQi', 4);
        let resultSet = await kvStore?.getResultSet(predicates);
        expect(resultSet?.getCount()).assertEqual(4);
        await kvStore?.closeResultSet(resultSet);

        let predicates1 = new dataSharePredicates.DataSharePredicates();
        predicates1.greaterThanOrEqualTo('$.age', 21, null);
        let resultSet1 = await kvStore?.getResultSet(predicates1);
        expect(resultSet1?.getCount()).assertEqual(3);
        await kvStore?.closeResultSet(resultSet1);

        let predicates2 = new dataSharePredicates.DataSharePredicates();
        predicates2.greaterThanOrEqualTo('$.isStudent', true, '');
        let resultSet2 = await kvStore?.getResultSet(predicates2);
        expect(resultSet2?.getCount()).assertEqual(6);
        await kvStore?.closeResultSet(resultSet2);

        try {
          let predicates3 = new dataSharePredicates.DataSharePredicates();
          predicates3.greaterThanOrEqualTo('e');
          expect().assertFail();
        } catch (error1) {
          console.info(TAG + 'error1: ' + JSON.stringify(error1));
        }
      } catch (e) {
        console.error(TAG + `get resultSet failed errCode:${e.code}, message${e.message}`);
        expect().assertFail();
      }
      console.log(TAG + "************* greaterThanOrEqualToErrCodeTest_0001 end *************");
    });

    /**
     * @tc.name: greaterThanOrEqualToErrCodeTest_0002
     * @tc.desc: Verify the behavior of the greaterThanOrEqualTo in DataSharePredicates with various input types
     * @tc.type: FUNC
     * @tc.precon: DataSharePredicates instance is created and table is populated with test data
     * @tc.step:
     *   1. Test greaterThanOrEqualTo with numeric field - expect row count 6
     *   2. Test greaterThanOrEqualTo with null value - expect row count 6
     *   3. Test greaterThanOrEqualTo with string 'null' - expect row count 0
     *   4. Test greaterThanOrEqualTo with empty string - expect row count 6
     *   5. Test greaterThanOrEqualTo with undefined value - expect row count 6
     * @tc.expect:
     *   1. Correct row count 6 for numeric field
     *   2. Correct row count 6 for null value
     *   3. Correct row count 0 for string 'null'
     *   4. Correct row count 6 for empty string
     *   5. Correct row count 6 for undefined value
     */
    it('greaterThanOrEqualToErrCodeTest_0002', 0, async () => {
      console.log(TAG + "************* greaterThanOrEqualToErrCodeTest_0002 start *************");
      try {
        let predicates = new dataSharePredicates.DataSharePredicates();
        predicates.greaterThanOrEqualTo(1, 22);
        let resultSet = await kvStore?.getResultSet(predicates);
        expect(resultSet?.getCount()).assertEqual(6);
        await kvStore?.closeResultSet(resultSet);

        let predicates1 = new dataSharePredicates.DataSharePredicates();
        predicates1.greaterThanOrEqualTo(null, 22);
        let resultSet1 = await kvStore?.getResultSet(predicates1);
        expect(resultSet1?.getCount()).assertEqual(6);
        await kvStore?.closeResultSet(resultSet1);

        try {
          let predicates2 = new dataSharePredicates.DataSharePredicates();
          predicates2.greaterThanOrEqualTo('null', 22);
          let resultSet2 = await kvStore?.getResultSet(predicates2);
          expect(resultSet2?.getCount()).assertEqual(0);
          await kvStore?.closeResultSet(resultSet2);
          expect().assertFail();
        } catch (error1) {
          console.info(TAG + `error1 errCode: ${error1.code}, message: ${error1.message}`);
        }

        let predicates3 = new dataSharePredicates.DataSharePredicates();
        predicates3.greaterThanOrEqualTo('', 22);
        let resultSet3 = await kvStore?.getResultSet(predicates3);
        expect(resultSet3?.getCount()).assertEqual(6);
        await kvStore?.closeResultSet(resultSet3);

        let predicates4 = new dataSharePredicates.DataSharePredicates();
        predicates4.greaterThanOrEqualTo(undefined, 22);
        let resultSet4 = await kvStore?.getResultSet(predicates4);
        expect(resultSet4?.getCount()).assertEqual(6);
        await kvStore?.closeResultSet(resultSet4);
      } catch (e) {
        console.error(TAG + `get resultSet failed errCode:${e.code}, message${e.message}`);
        expect().assertFail();
      }
      console.log(TAG + "************* greaterThanOrEqualToErrCodeTest_0002 end *************");
    });

    /**
     * @tc.name: greaterThanOrEqualToErrCodeTest_0003
     * @tc.desc: Verify the behavior of the greaterThanOrEqualTo in DataSharePredicates with various input types
     * @tc.type: FUNC
     * @tc.precon: DataSharePredicates instance is created and table is populated with test data
     * @tc.step:
     *   1. Test greaterThanOrEqualTo with numeric array - expect row count 6
     *   2. Test greaterThanOrEqualTo with null value - expect row count 6
     *   3. Test greaterThanOrEqualTo with empty string for age - expect row count 0
     *   4. Test greaterThanOrEqualTo with empty string for name - expect row count 6
     *   5. Test greaterThanOrEqualTo with undefined value for name - expect row count 6
     * @tc.expect:
     *   1. Correct row count 6 for numeric array
     *   2. Correct row count 6 for null value
     *   3. Correct row count 0 for empty string in age
     *   4. Correct row count 6 for empty string in name
     *   5. Correct row count 6 for undefined value in name
     */
    it('greaterThanOrEqualToErrCodeTest_0003', 0, async () => {
      console.log(TAG + "************* greaterThanOrEqualToErrCodeTest_0003 start *************");
      try {
        let predicates = new dataSharePredicates.DataSharePredicates();
        predicates.greaterThanOrEqualTo('age', [1, 2]);
        let resultSet = await kvStore?.getResultSet(predicates);
        expect(resultSet?.getCount()).assertEqual(6);
        await kvStore?.closeResultSet(resultSet);

        let predicates1 = new dataSharePredicates.DataSharePredicates();
        predicates1.greaterThanOrEqualTo('age', null);
        let resultSet1 = await kvStore?.getResultSet(predicates1);
        expect(resultSet1?.getCount()).assertEqual(6);
        await kvStore?.closeResultSet(resultSet1);

        let predicates2 = new dataSharePredicates.DataSharePredicates();
        predicates2.greaterThanOrEqualTo('age', '');
        let resultSet2 = await kvStore?.getResultSet(predicates2);
        expect(resultSet2?.getCount()).assertEqual(0);
        await kvStore?.closeResultSet(resultSet2);

        let predicates3 = new dataSharePredicates.DataSharePredicates();
        predicates3.greaterThanOrEqualTo('name', '');
        let resultSet3 = await kvStore?.getResultSet(predicates3);
        expect(resultSet3?.getCount()).assertEqual(6);
        await kvStore?.closeResultSet(resultSet3);

        let predicates4 = new dataSharePredicates.DataSharePredicates();
        predicates4.greaterThanOrEqualTo('name', undefined);
        let resultSet4 = await kvStore?.getResultSet(predicates4);
        expect(resultSet4?.getCount()).assertEqual(6);
      } catch (e) {
        console.error(TAG + `get resultSet failed errCode:${e.code}, message${e.message}`);
        expect().assertFail();
      }
      console.log(TAG + "************* greaterThanOrEqualToErrCodeTest_0003 end *************");
    });

    /**
     * @tc.name: lessThanOrEqualToErrCodeTest_0001
     * @tc.desc: Verify the behavior of the lessThanOrEqualTo in DataSharePredicates with various input types
     * @tc.type: FUNC
     * @tc.precon: DataSharePredicates instance is created and table is populated with test data
     * @tc.step:
     *   1. Test lessThanOrEqualTo with string field and string third parameter - expect row count 3
     *   2. Test lessThanOrEqualTo with numeric field and numeric third parameter - expect row count 4
     *   3. Test lessThanOrEqualTo with boolean field and numeric third parameter - expect row count 6
     *   4. Test lessThanOrEqualTo with invalid parameters and expect an error
     * @tc.expect:
     *   1. Correct row count 3 for string field with string third parameter
     *   2. Correct row count 4 for numeric field with numeric third parameter
     *   3. Correct row count 6 for boolean field with numeric third parameter
     *   4. Error is thrown for invalid parameters
     */
    it('lessThanOrEqualToErrCodeTest_0001', 0, async () => {
      console.log(TAG + "************* lessThanOrEqualToErrCodeTest_0001 start *************");
      try {
        let predicates = new dataSharePredicates.DataSharePredicates();
        predicates.lessThanOrEqualTo('$.name', 'MaQi', 'test');
        let resultSet = await kvStore?.getResultSet(predicates);
        expect(resultSet?.getCount()).assertEqual(3);
        await kvStore?.closeResultSet(resultSet);

        let predicates1 = new dataSharePredicates.DataSharePredicates();
        predicates1.lessThanOrEqualTo('$.age', 21, 0);
        let resultSet1 = await kvStore?.getResultSet(predicates1);
        expect(resultSet1?.getCount()).assertEqual(4);
        await kvStore?.closeResultSet(resultSet1);

        let predicates2 = new dataSharePredicates.DataSharePredicates();
        predicates2.lessThanOrEqualTo('$.isStudent', true, 1);
        let resultSet2 = await kvStore?.getResultSet(predicates2);
        expect(resultSet2?.getCount()).assertEqual(6);
        await kvStore?.closeResultSet(resultSet2);

        try {
          let predicates3 = new dataSharePredicates.DataSharePredicates();
          predicates3.lessThanOrEqualTo('e');
          expect().assertFail();
        } catch (error1) {
          console.info(TAG + 'error1: ' + JSON.stringify(error1));
        }
      } catch (e) {
        console.error(TAG + `get resultSet failed errCode:${e.code}, message${e.message}`);
        expect().assertFail();
      }
      console.log(TAG + "************* lessThanOrEqualToErrCodeTest_0001 end *************");
    });

    /**
     * @tc.name: lessThanOrEqualToErrCodeTest_0002
     * @tc.desc: Verify the behavior of the lessThanOrEqualTo in DataSharePredicates with various input types
     * @tc.type: FUNC
     * @tc.precon: DataSharePredicates instance is created and table is populated with test data
     * @tc.step:
     *   1. Test lessThanOrEqualTo with numeric field - expect row count 6
     *   2. Test lessThanOrEqualTo with null value - expect row count 6
     *   3. Test lessThanOrEqualTo with string 'null' - expect row count 0
     *   4. Test lessThanOrEqualTo with empty string - expect row count 6
     *   5. Test lessThanOrEqualTo with undefined value - expect row count 6
     * @tc.expect:
     *   1. Correct row count 6 for numeric field
     *   2. Correct row count 6 for null value
     *   3. Correct row count 0 for string 'null'
     *   4. Correct row count 6 for empty string
     *   5. Correct row count 6 for undefined value
     */
    it('lessThanOrEqualToErrCodeTest_0002', 0, async () => {
      console.log(TAG + "************* lessThanOrEqualToErrCodeTest_0002 start *************");
      try {
        let predicates = new dataSharePredicates.DataSharePredicates();
        predicates.lessThanOrEqualTo(1, 22);
        let resultSet = await kvStore?.getResultSet(predicates);
        expect(resultSet?.getCount()).assertEqual(6);
        await kvStore?.closeResultSet(resultSet);

        let predicates1 = new dataSharePredicates.DataSharePredicates();
        predicates1.lessThanOrEqualTo(null, 22);
        let resultSet1 = await kvStore?.getResultSet(predicates1);
        expect(resultSet1?.getCount()).assertEqual(6);
        await kvStore?.closeResultSet(resultSet1);

        try {
          let predicates2 = new dataSharePredicates.DataSharePredicates();
          predicates2.lessThanOrEqualTo('null', 22);
          let resultSet2 = await kvStore?.getResultSet(predicates2);
          expect(resultSet2?.getCount()).assertEqual(0);
          await kvStore?.closeResultSet(resultSet2);
          expect().assertFail();
        } catch (error1) {
          console.info(TAG + `error1 errCode: ${error1.code}, message: ${error1.message}`);
        }
        let predicates3 = new dataSharePredicates.DataSharePredicates();
        predicates3.lessThanOrEqualTo('', 22);
        let resultSet3 = await kvStore?.getResultSet(predicates3);
        expect(resultSet3?.getCount()).assertEqual(6);
        await kvStore?.closeResultSet(resultSet3);

        let predicates4 = new dataSharePredicates.DataSharePredicates();
        predicates4.lessThanOrEqualTo(undefined, 22);
        let resultSet4 = await kvStore?.getResultSet(predicates4);
        expect(resultSet4?.getCount()).assertEqual(6);
        await kvStore?.closeResultSet(resultSet4);
      } catch (e) {
        console.error(TAG + `get resultSet failed errCode:${e.code}, message${e.message}`);
        expect().assertFail();
      }
      console.log(TAG + "************* lessThanOrEqualToErrCodeTest_0002 end *************");
    });

    /**
     * @tc.name: lessThanOrEqualToErrCodeTest_0003
     * @tc.desc: Verify the behavior of the lessThanOrEqualTo in DataSharePredicates with various input types
     * @tc.type: FUNC
     * @tc.precon: DataSharePredicates instance is created and table is populated with test data
     * @tc.step:
     *   1. Test lessThanOrEqualTo with numeric array - expect row count 6
     *   2. Test lessThanOrEqualTo with null value - expect row count 6
     *   3. Test lessThanOrEqualTo with empty string for age - expect row count 6
     *   4. Test lessThanOrEqualTo with empty string for name - expect row count 0
     *   5. Test lessThanOrEqualTo with undefined value for name - expect row count 6
     * @tc.expect:
     *   1. Correct row count 6 for numeric array
     *   2. Correct row count 6 for null value
     *   3. Correct row count 6 for empty string in age
     *   4. Correct row count 0 for empty string in name
     *   5. Correct row count 6 for undefined value in name
     */
    it('lessThanOrEqualToErrCodeTest_0003', 0, async () => {
      console.log(TAG + "************* lessThanOrEqualToErrCodeTest_0003 start *************");
      try {
        let predicates = new dataSharePredicates.DataSharePredicates();
        predicates.lessThanOrEqualTo('age', [1, 2]);
        let resultSet = await kvStore?.getResultSet(predicates);
        expect(resultSet?.getCount()).assertEqual(6);
        await kvStore?.closeResultSet(resultSet);

        let predicates1 = new dataSharePredicates.DataSharePredicates();
        predicates1.lessThanOrEqualTo('age', null);
        let resultSet1 = await kvStore?.getResultSet(predicates1);
        expect(resultSet1?.getCount()).assertEqual(6);
        await kvStore?.closeResultSet(resultSet1);

        let predicates2 = new dataSharePredicates.DataSharePredicates();
        predicates2.lessThanOrEqualTo('age', '');
        let resultSet2 = await kvStore?.getResultSet(predicates2);
        expect(resultSet2?.getCount()).assertEqual(6);
        await kvStore?.closeResultSet(resultSet2);

        let predicates3 = new dataSharePredicates.DataSharePredicates();
        predicates3.lessThanOrEqualTo('name', '');
        let resultSet3 = await kvStore?.getResultSet(predicates3);
        expect(resultSet3?.getCount()).assertEqual(0);
        await kvStore?.closeResultSet(resultSet3);

        let predicates4 = new dataSharePredicates.DataSharePredicates();
        predicates4.lessThanOrEqualTo('name', undefined);
        let resultSet4 = await kvStore?.getResultSet(predicates4);
        expect(resultSet4?.getCount()).assertEqual(6);
        await kvStore?.closeResultSet(resultSet4);
      } catch (e) {
        console.error(TAG + `get resultSet failed errCode:${e.code}, message${e.message}`);
        expect().assertFail();
      }
      console.log(TAG + "************* lessThanOrEqualToErrCodeTest_0003 end *************");
    });

    /**
     * @tc.name: notInErrCodeTest_0001
     * @tc.desc: Verify the behavior of the notIn in DataSharePredicates with various input types
     * @tc.type: FUNC
     * @tc.precon: DataSharePredicates instance is created and table is populated with test data
     * @tc.step:
     *   1. Test notIn with string field and string array - expect row count 4
     *   2. Test notIn with numeric field and numeric array - expect row count 3
     *   3. Test notIn with boolean field and mixed boolean array - expect row count 3
     *   4. Test notIn with invalid parameters and expect an error
     * @tc.expect:
     *   1. Correct row count 4 for string field with string array
     *   2. Correct row count 3 for numeric field with numeric array
     *   3. Correct row count 3 for boolean field with mixed boolean array
     *   4. Error is thrown for invalid parameters
     */
    it('notInErrCodeTest_0001', 0, async () => {
      console.log(TAG + "************* notInErrCodeTest_0001 start *************");
      try {
        let predicates = new dataSharePredicates.DataSharePredicates();
        predicates.notIn('$.name', ['MaQi', 'ZhangSan'], ['test']);
        let resultSet = await kvStore?.getResultSet(predicates);
        expect(resultSet?.getCount()).assertEqual(4);
        await kvStore?.closeResultSet(resultSet);

        let predicates1 = new dataSharePredicates.DataSharePredicates();
        predicates1.notIn('$.age', [21, 20], [1, 2]);
        let resultSet1 = await kvStore?.getResultSet(predicates1);
        expect(resultSet1?.getCount()).assertEqual(3);
        await kvStore?.closeResultSet(resultSet1);

        let predicates2 = new dataSharePredicates.DataSharePredicates();
        predicates2.notIn('$.isStudent', [true], [true, false]);
        let resultSet2 = await kvStore?.getResultSet(predicates2);
        expect(resultSet2?.getCount()).assertEqual(3);
        await kvStore?.closeResultSet(resultSet2);

        try {
          let predicates3 = new dataSharePredicates.DataSharePredicates();
          predicates3.notIn(222);
          expect().assertFail();
        } catch (error1) {
          console.info(TAG + 'error1: ' + JSON.stringify(error1));
        }
      } catch (e) {
        console.error(TAG + `get resultSet failed errCode:${e.code}, message${e.message}`);
        expect().assertFail();
      }
      console.log(TAG + "************* notInErrCodeTest_0001 end *************");
    });

    /**
     * @tc.name: notInErrCodeTest_0002
     * @tc.desc: Verify the behavior of the notIn in DataSharePredicates with various input types
     * @tc.type: FUNC
     * @tc.precon: DataSharePredicates instance is created and table is populated with test data
     * @tc.step:
     *   1. Test notIn with numeric field - expect row count 6
     *   2. Test notIn with null value - expect row count 6
     *   3. Test notIn with string 'null' - expect row count 0
     *   4. Test notIn with empty string - expect row count 6
     *   5. Test notIn with undefined value - expect row count 6
     * @tc.expect:
     *   1. Correct row count 6 for numeric field
     *   2. Correct row count 6 for null value
     *   3. Correct row count 0 for string 'null'
     *   4. Correct row count 6 for empty string
     *   5. Correct row count 6 for undefined value
     */
    it('notInErrCodeTest_0002', 0, async () => {
      console.log(TAG + "************* notInErrCodeTest_0002 start *************");
      try {
        let predicates = new dataSharePredicates.DataSharePredicates();
        predicates.notIn(1, [20, 24]);
        let resultSet = await kvStore?.getResultSet(predicates);
        expect(resultSet?.getCount()).assertEqual(6);
        await kvStore?.closeResultSet(resultSet);

        let predicates1 = new dataSharePredicates.DataSharePredicates();
        predicates1.notIn(null, [20, 24]);
        let resultSet1 = await kvStore?.getResultSet(predicates1);
        expect(resultSet1?.getCount()).assertEqual(6);
        await kvStore?.closeResultSet(resultSet1);

        try {
          let predicates2 = new dataSharePredicates.DataSharePredicates();
          predicates2.notIn('null', [20, 24]);
          let resultSet2 = await kvStore?.getResultSet(predicates2);
          expect(resultSet2?.getCount()).assertEqual(0);
          await kvStore?.closeResultSet(resultSet2);
          expect().assertFail();
        } catch (error1) {
          console.info(TAG + `error1 errCode: ${error1.code}, message: ${error1.message}`);
        }

        let predicates3 = new dataSharePredicates.DataSharePredicates();
        predicates3.notIn('', [20, 24]);
        let resultSet3 = await kvStore?.getResultSet(predicates3);
        expect(resultSet3?.getCount()).assertEqual(6);
        await kvStore?.closeResultSet(resultSet3);

        let predicates4 = new dataSharePredicates.DataSharePredicates();
        predicates4.notIn(undefined, [20, 24]);
        let resultSet4 = await kvStore?.getResultSet(predicates4);
        expect(resultSet4?.getCount()).assertEqual(6);
        await kvStore?.closeResultSet(resultSet4);
      } catch (e) {
        console.error(TAG + `get resultSet failed errCode:${e.code}, message${e.message}`);
        expect().assertFail();
      }
      console.log(TAG + "************* notInErrCodeTest_0002 end *************");
    });

    /**
     * @tc.name: notInErrCode_0003
     * @tc.desc: Verify the behavior of the notIn in DataSharePredicates with various input types
     * @tc.type: FUNC
     * @tc.precon: DataSharePredicates instance is created and table is populated with test data
     * @tc.step:
     *   1. Test notIn with boolean false for age - expect row count 6
     *   2. Test notIn with null value for age - expect row count 6
     *   3. Test notIn with undefined value for age - expect row count 6
     *   4. Test notIn with nested array for age - expect row count 6
     *   5. Test notIn with nested array for age - expect row count 5
     *   6. Test notIn with empty string for age - expect row count 6
     * @tc.expect:
     *   1. Correct row count 6 for boolean false in age
     *   2. Correct row count 6 for null value in age
     *   3. Correct row count 6 for undefined value in age
     *   4. Correct row count 6 for nested array in age
     *   5. Correct row count 5 for nested array in age
     *   6. Correct row count 6 for empty string in age
     */
    it('notInErrCodeTest_0003', 0, async () => {
      console.log(TAG + "************* notInErrCodeTest_0003 start *************");
      try {
        let predicates = new dataSharePredicates.DataSharePredicates();
        predicates.notIn('age', false);
        let resultSet = await kvStore?.getResultSet(predicates);
        expect(resultSet?.getCount()).assertEqual(6);
        await kvStore?.closeResultSet(resultSet);

        let predicates1 = new dataSharePredicates.DataSharePredicates();
        predicates1.notIn('age', null);
        let resultSet1 = await kvStore?.getResultSet(predicates1);
        expect(resultSet1?.getCount()).assertEqual(6);
        await kvStore?.closeResultSet(resultSet1);

        let predicates2 = new dataSharePredicates.DataSharePredicates();
        predicates2.notIn('age', undefined);
        let resultSet2 = await kvStore?.getResultSet(predicates2);
        expect(resultSet2?.getCount()).assertEqual(6);
        await kvStore?.closeResultSet(resultSet2);

        let predicates3 = new dataSharePredicates.DataSharePredicates();
        predicates3.notIn('age', [[20, 22]]);
        let resultSet3 = await kvStore?.getResultSet(predicates3);
        expect(resultSet3?.getCount()).assertEqual(6);
        await kvStore?.closeResultSet(resultSet3);

        let predicates4 = new dataSharePredicates.DataSharePredicates();
        predicates4.notIn('age', [true, false]);
        let resultSet4 = await kvStore?.getResultSet(predicates4);
        expect(resultSet4?.getCount()).assertEqual(5);
        await kvStore?.closeResultSet(resultSet4);

        let predicates5 = new dataSharePredicates.DataSharePredicates();
        predicates5.notIn('age', '');
        let resultSet5 = await kvStore?.getResultSet(predicates5);
        expect(resultSet5?.getCount()).assertEqual(6);
        await kvStore?.closeResultSet(resultSet5);
      } catch (e) {
        console.error(TAG + `get resultSet failed errCode:${e.code}, message${e.message}`);
        expect().assertFail();
      }
      console.log(TAG + "************* notInErrCodeTest_0003 end *************");
    });

    /**
     * @tc.name: likeErrCodeTest_0001
     * @tc.desc: Verify the behavior of the like in DataSharePredicates with various input types
     * @tc.type: FUNC
     * @tc.precon: DataSharePredicates instance is created and table is populated with test data
     * @tc.step:
     *   1. Test like with string field and pattern - expect row count 2
     *   2. Test like with numeric field and string pattern - expect row count 2
     *   3. Test like with boolean field and string pattern - expect row count 0
     *   4. Test like with boolean field and numeric pattern - expect row count 3
     *   5. Test like with invalid parameters and expect an error
     * @tc.expect:
     *   1. Correct row count 2 for string field with pattern
     *   2. Correct row count 2 for numeric field with string pattern
     *   3. Correct row count 0 for boolean field with string pattern
     *   4. Correct row count 3 for boolean field with numeric pattern
     *   5. Error is thrown for invalid parameters
     */
    it('likeErrCodeTest_0001', 0, async () => {
      console.log(TAG + "************* likeErrCodeTest_0001 start *************");
      try {
        let predicates = new dataSharePredicates.DataSharePredicates();
        predicates.like('name', '%an%', 3);
        let resultSet = await kvStore?.getResultSet(predicates);
        expect(resultSet?.getCount()).assertEqual(2);
        await kvStore?.closeResultSet(resultSet);

        let predicates1 = new dataSharePredicates.DataSharePredicates();
        predicates1.like('age', '20', false);
        let resultSet1 = await kvStore?.getResultSet(predicates1);
        expect(resultSet1?.getCount()).assertEqual(2);
        await kvStore?.closeResultSet(resultSet1);

        let predicates2 = new dataSharePredicates.DataSharePredicates();
        predicates2.like('isStudent', 'false', 1);
        let resultSet2 = await kvStore?.getResultSet(predicates2);
        expect(resultSet2?.getCount()).assertEqual(0);
        await kvStore?.closeResultSet(resultSet2);

        let predicates3 = new dataSharePredicates.DataSharePredicates();
        predicates3.like('isStudent', '0', true);
        let resultSet3 = await kvStore?.getResultSet(predicates3);
        expect(resultSet3?.getCount()).assertEqual(3);
        await kvStore?.closeResultSet(resultSet3);

        try {
          let predicates4 = new dataSharePredicates.DataSharePredicates();
          predicates4.like(222);
          expect().assertFail();
        } catch (error1) {
          console.info(TAG + 'error1: ' + JSON.stringify(error1));
        }
      } catch (e) {
        console.error(TAG + `get resultSet failed errCode:${e.code}, message${e.message}`);
        expect().assertFail();
      }
      console.log(TAG + "************* likeErrCodeTest_0001 end *************");
    });

    /**
     * @tc.name: likeErrCodeTest_0002
     * @tc.desc: Verify the behavior of the like in DataSharePredicates with various input types
     * @tc.type: FUNC
     * @tc.precon: DataSharePredicates instance is created and table is populated with test data
     * @tc.step:
     *   1. Test like with numeric field - expect row count 6
     *   2. Test like with null value - expect row count 6
     *   3. Test like with string 'null' - expect row count 0
     *   4. Test like with empty string - expect row count 6
     *   5. Test like with undefined value - expect row count 6
     * @tc.expect:
     *   1. Correct row count 6 for numeric field
     *   2. Correct row count 6 for null value
     *   3. Correct row count 0 for string 'null'
     *   4. Correct row count 6 for empty string
     *   5. Correct row count 6 for undefined value
     */
    it('likeErrCodeTest_0002', 0, async () => {
      console.log(TAG + "************* likeErrCodeTest_0002 start *************");
      try {
        let predicates = new dataSharePredicates.DataSharePredicates();
        predicates.like(1, '%ng%');
        let resultSet = await kvStore?.getResultSet(predicates);
        expect(resultSet?.getCount()).assertEqual(6);
        await kvStore?.closeResultSet(resultSet);

        let predicates1 = new dataSharePredicates.DataSharePredicates();
        predicates1.like(null, '%ng%');
        let resultSet1 = await kvStore?.getResultSet(predicates1);
        expect(resultSet1?.getCount()).assertEqual(6);
        await kvStore?.closeResultSet(resultSet1);

        try {
          let predicates2 = new dataSharePredicates.DataSharePredicates();
          predicates2.like('null', '%ng%');
          let resultSet2 = await kvStore?.getResultSet(predicates2);
          expect(resultSet2?.getCount()).assertEqual(0);
          await kvStore?.closeResultSet(resultSet2);
          expect().assertFail();
        } catch (error1) {
          console.info(TAG + `error1 errCode: ${error1.code}, message: ${error1.message}`);
        }
        let predicates3 = new dataSharePredicates.DataSharePredicates();
        predicates3.like('', '%ng%');
        let resultSet3 = await kvStore?.getResultSet(predicates3);
        expect(resultSet3?.getCount()).assertEqual(6);
        await kvStore?.closeResultSet(resultSet3);

        let predicates4 = new dataSharePredicates.DataSharePredicates();
        predicates4.like(undefined, '%ng%');
        let resultSet4 = await kvStore?.getResultSet(predicates4);
        expect(resultSet4?.getCount()).assertEqual(6);
        await kvStore?.closeResultSet(resultSet4);
      } catch (e) {
        console.error(TAG + `get resultSet failed errCode:${e.code}, message${e.message}`);
        expect().assertFail();
      }
      console.log(TAG + "************* likeErrCodeTest_0002 end *************");
    });

    /**
     * @tc.name: likeErrCodeTest_0003
     * @tc.desc: Verify the behavior of the like in DataSharePredicates with various input types
     * @tc.type: FUNC
     * @tc.precon: DataSharePredicates instance is created and table is populated with test data
     * @tc.step:
     *   1. Test like with boolean false for age - expect row count 1
     *   2. Test like with null value for age - expect row count 0
     *   3. Test like with undefined value for age - expect row count 0
     *   4. Test like with array for age - expect row count 0
     *   5. Test like with empty string for age - expect row count 0
     *   6. Test like with empty string for name - expect row count 0
     * @tc.expect:
     *   1. Correct row count 1 for boolean false in age
     *   2. Correct row count 0 for null value in age
     *   3. Correct row count 0 for undefined value in age
     *   4. Correct row count 0 for array in age
     *   5. Correct row count 0 for empty string in age
     *   6. Correct row count 0 for empty string in name
     */
    it('likeErrCodeTest_0003', 0, async () => {
      console.log(TAG + "************* likeErrCodeTest_0003 start *************");
      try {
        let predicates = new dataSharePredicates.DataSharePredicates();
        predicates.like('age', false);
        let resultSet = await kvStore?.getResultSet(predicates);
        expect(resultSet?.getCount()).assertEqual(1);
        await kvStore?.closeResultSet(resultSet);

        let predicates1 = new dataSharePredicates.DataSharePredicates();
        predicates1.like('age', null);
        let resultSet1 = await kvStore?.getResultSet(predicates1);
        expect(resultSet1?.getCount()).assertEqual(0);
        await kvStore?.closeResultSet(resultSet1);

        let predicates2 = new dataSharePredicates.DataSharePredicates();
        predicates2.like('age', undefined);
        let resultSet2 = await kvStore?.getResultSet(predicates2);
        expect(resultSet2?.getCount()).assertEqual(0);
        await kvStore?.closeResultSet(resultSet2);

        let predicates3 = new dataSharePredicates.DataSharePredicates();
        predicates3.like('age', [20, 22]);
        let resultSet3 = await kvStore?.getResultSet(predicates3);
        expect(resultSet3?.getCount()).assertEqual(0);
        await kvStore?.closeResultSet(resultSet3);

        let predicates4 = new dataSharePredicates.DataSharePredicates();
        predicates4.like('age', '');
        let resultSet4 = await kvStore?.getResultSet(predicates4);
        expect(resultSet4?.getCount()).assertEqual(0);
        await kvStore?.closeResultSet(resultSet4);

        let predicates5 = new dataSharePredicates.DataSharePredicates();
        predicates5.like('name', '');
        let resultSet5 = await kvStore?.getResultSet(predicates5);
        expect(resultSet5?.getCount()).assertEqual(0);
        await kvStore?.closeResultSet(resultSet5);
      } catch (e) {
        console.error(TAG + `get resultSet failed errCode:${e.code}, message${e.message}`);
        expect().assertFail();
      }
      console.log(TAG + "************* likeErrCodeTest_0003 end *************");
    });

    /**
     * @tc.name: orErrCodeTest_0001
     * @tc.desc: Verify the behavior of the or operator in DataSharePredicates with like and equalTo predicates
     * @tc.type: FUNC
     * @tc.precon: DataSharePredicates instance is created and table is populated with test data
     * @tc.step:
     *   1. Test like with name field and pattern, combined with or and equalTo on age field
     *   2. Test like with name field and pattern, combined with or and equalTo on age field (without JSON path)
     * @tc.expect:
     *   1. Correct row count 3 for combination of like and equalTo predicates with or operator
     *   2. Correct row count 3 for combination of like and equalTo predicates with or operator (without JSON path)
     */
    it('orErrCodeTest_0001', 0, async () => {
      console.log(TAG + "************* orErrCodeTest_0001 start *************");
      try {
        let predicates = new dataSharePredicates.DataSharePredicates();
        predicates.like('$.name', '%an%');
        predicates.or(666).equalTo('$.age', 0);
        let resultSet = await kvStore?.getResultSet(predicates);
        expect(resultSet?.getCount()).assertEqual(3);
        await kvStore?.closeResultSet(resultSet);

        let predicates1 = new dataSharePredicates.DataSharePredicates();
        predicates1.like('name', '%an%');
        predicates1.or(666).equalTo('age', 0);
        let resultSet1 = await kvStore?.getResultSet(predicates1);
        expect(resultSet1?.getCount()).assertEqual(3);
        await kvStore?.closeResultSet(resultSet1);
      } catch (e) {
        console.error(TAG + `get resultSet failed errCode:${e.code}, message${e.message}`);
        expect().assertFail();
      }
      console.log(TAG + "************* orErrCodeTest_0001 end *************");
    });
  });
}