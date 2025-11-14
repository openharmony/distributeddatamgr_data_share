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
const TEST_STORE_ID = 'SingleKVStoreReturnValueId';
let kvManager: distributedKVStore.KVManager | null = null;
let kvStore: distributedKVStore.SingleKVStore | undefined = undefined;

let TAG = "[dataSharePredicateKV_test]";

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
const value6 = '{"id":-1, "name":"Error", "age":-1, "isStudent":false}';

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

export default function DataSharePredicatesKVEtsTest() {
  describe('DataSharePredicatesKVEtsTest', () => {

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
        console.error(TAG + `Failed to close and delete kvStore code ${e.code},message ${e.message}`);
        expect().assertFail();
      }
      kvManager = null;
      kvStore = undefined;
    });

    /**
     * @tc.name: NotEqualToTest_0001
     * @tc.desc: Verify notEqualTo predicate for different data types in key-value store
     * @tc.type: FUNC
     * @tc.precon: kvStore is initialized and populated with test data
     * @tc.step:
     *   1. Test notEqualTo on string field (name != 'ZhangSan') - expect 5 non-matching entries
     *   2. Test notEqualTo on numeric field (age != 20) - expect 4 non-matching entries
     *   3. Test notEqualTo on boolean field (isStudent != false) - expect 3 non-matching entries
     * @tc.expect:
     *   1. Correct result set for string field
     *   2. Correct result set for numeric field
     *   3. Correct result set for boolean field
     */
    it('NotEqualToTest_0001', 0, async () => {
      console.log(TAG + "************* NotEqualToTest_0001 start *************");
      try {
        let predicates = new dataSharePredicates.DataSharePredicates();
        predicates.notEqualTo('name', 'ZhangSan');
        let resultSet = await kvStore?.getResultSet(predicates);
        expect(resultSet?.getCount()).assertEqual(5);
        await kvStore?.closeResultSet(resultSet);

        let predicates1 = new dataSharePredicates.DataSharePredicates();
        predicates1.notEqualTo('$.age', 20);
        let resultSet1 = await kvStore?.getResultSet(predicates1);
        expect(resultSet1?.getCount()).assertEqual(4);
        await kvStore?.closeResultSet(resultSet1);

        let predicates2 = new dataSharePredicates.DataSharePredicates();
        predicates2.notEqualTo('$.isStudent', false);
        let resultSet2 = await kvStore?.getResultSet(predicates2);
        expect(resultSet2?.getCount()).assertEqual(3);
        await kvStore?.closeResultSet(resultSet2);
      } catch (e) {
        console.error(TAG + `get resultSet failed errCode:${e.code}, message${e.message}`);
        expect().assertFail();
      }
      console.log(TAG + "************* NotEqualToTest_0001 end *************");
    });

    /**
     * @tc.name: greaterThanTest_0001
     * @tc.desc: Verify greaterThan predicate for different data types in key-value store
     * @tc.type: FUNC
     * @tc.precon: kvStore is initialized and populated with test data
     * @tc.step:
     *   1. Test greaterThan on string field (name > 'MaQi') - expect 3 matching entries
     *   2. Test greaterThan on numeric field (age > 21) - expect 2 matching entries
     *   3. Test greaterThan on boolean field (isStudent > true) - expect 6 matching entries
     * @tc.expect:
     *   1. Correct result set for string field
     *   2. Correct result set for numeric field
     *   3. Correct result set for boolean field
     */
    it('greaterThanTest_0001', 0, async () => {
      console.log(TAG + "************* greaterThanTest_0001 start *************");
      try {
        let predicates = new dataSharePredicates.DataSharePredicates();
        predicates.greaterThan('$.name', 'MaQi');
        let resultSet = await kvStore?.getResultSet(predicates);
        expect(resultSet?.getCount()).assertEqual(3);
        await kvStore?.closeResultSet(resultSet);

        let predicates1 = new dataSharePredicates.DataSharePredicates();
        predicates1.greaterThan('age', 21);
        let resultSet1 = await kvStore?.getResultSet(predicates1);
        expect(resultSet1?.getCount()).assertEqual(2);
        await kvStore?.closeResultSet(resultSet1);

        let predicates2 = new dataSharePredicates.DataSharePredicates();
        predicates2.greaterThan('$.isStudent', true);
        let resultSet2 = await kvStore?.getResultSet(predicates2);
        expect(resultSet2?.getCount()).assertEqual(6);
        await kvStore?.closeResultSet(resultSet2);
      } catch (e) {
        console.error(TAG + `get resultSet failed errCode:${e.code}, message${e.message}`);
        expect().assertFail();
      }
      console.log(TAG + "************* greaterThanTest_0001 end *************");
    });

    /**
     * @tc.name: lessThanTest_0001
     * @tc.desc: Verify lessThan predicate for different data types in key-value store
     * @tc.type: FUNC
     * @tc.precon: kvStore is initialized and populated with test data
     * @tc.step:
     *   1. Test lessThan on string field (name < 'MaQi') - expect 2 matching entries
     *   2. Test lessThan on numeric field (age < 21) - expect 3 matching entries
     *   3. Test lessThan on boolean field (isStudent < false) - expect 6 matching entries
     * @tc.expect:
     *   1. Correct result set for string field
     *   2. Correct result set for numeric field
     *   3. Correct result set for boolean field
     */
    it('lessThanTest_0001', 0, async () => {
      console.log(TAG + "************* lessThanTest_0001 start *************");
      try {
        let predicates = new dataSharePredicates.DataSharePredicates();
        predicates.lessThan('$.name', 'MaQi');
        let resultSet = await kvStore?.getResultSet(predicates);
        expect(resultSet?.getCount()).assertEqual(2);
        await kvStore?.closeResultSet(resultSet);

        let predicates1 = new dataSharePredicates.DataSharePredicates();
        predicates1.lessThan('$.age', 21);
        let resultSet1 = await kvStore?.getResultSet(predicates1);
        expect(resultSet1?.getCount()).assertEqual(3);
        await kvStore?.closeResultSet(resultSet1);

        let predicates2 = new dataSharePredicates.DataSharePredicates();
        predicates2.lessThan('isStudent', false);
        let resultSet2 = await kvStore?.getResultSet(predicates2);
        expect(resultSet2?.getCount()).assertEqual(6);
        await kvStore?.closeResultSet(resultSet2);
      } catch (e) {
        console.error(TAG + `get resultSet failed errCode:${e.code}, message${e.message}`);
        expect().assertFail();
      }
      console.log(TAG + "************* lessThanTest_0001 end *************");
    });

    /**
     * @tc.name: greaterThanOrEqualToTest_0001
     * @tc.desc: Verify greaterThanOrEqualTo predicate for different data types in key-value store
     * @tc.type: FUNC
     * @tc.precon: kvStore is initialized and populated with test data
     * @tc.step:
     *   1. Test greaterThanOrEqualTo on string field (name >= 'MaQi') - expect 4 matching entries
     *   2. Test greaterThanOrEqualTo on numeric field (age >= 21) - expect 3 matching entries
     *   3. Test greaterThanOrEqualTo on boolean field (isStudent >= true) - expect 6 matching entries
     * @tc.expect:
     *   1. Correct result set for string field
     *   2. Correct result set for numeric field
     *   3. Correct result set for boolean field
     */
    it('greaterThanOrEqualToTest_0001', 0, async () => {
      console.log(TAG + "************* greaterThanOrEqualToTest_0001 start *************");
      try {
        let predicates = new dataSharePredicates.DataSharePredicates();
        predicates.greaterThanOrEqualTo('$.name', 'MaQi');
        let resultSet = await kvStore?.getResultSet(predicates);
        expect(resultSet?.getCount()).assertEqual(4);
        await kvStore?.closeResultSet(resultSet);

        let predicates1 = new dataSharePredicates.DataSharePredicates();
        predicates1.greaterThanOrEqualTo('$.age', 21);
        let resultSet1 = await kvStore?.getResultSet(predicates1);
        expect(resultSet1?.getCount()).assertEqual(3);
        await kvStore?.closeResultSet(resultSet1);

        let predicates2 = new dataSharePredicates.DataSharePredicates();
        predicates2.greaterThanOrEqualTo('$.isStudent', true);
        let resultSet2 = await kvStore?.getResultSet(predicates2);
        expect(resultSet2?.getCount()).assertEqual(6);
        await kvStore?.closeResultSet(resultSet2);
      } catch (e) {
        console.error(TAG + `get resultSet failed errCode:${e.code}, message${e.message}`);
        expect().assertFail();
      }
      console.log(TAG + "************* greaterThanOrEqualToTest_0001 end *************");
    });

    /**
     * @tc.name: lessThanOrEqualToTest_0001
     * @tc.desc: Verify lessThanOrEqualTo predicate for different data types in key-value store
     * @tc.type: FUNC
     * @tc.precon: kvStore is initialized and populated with test data
     * @tc.step:
     *   1. Test lessThanOrEqualTo on string field (name <= 'MaQi') - expect 3 matching entries
     *   2. Test lessThanOrEqualTo on numeric field (age <= 21) - expect 4 matching entries
     *   3. Test lessThanOrEqualTo on boolean field (isStudent <= true) - expect 6 matching entries
     * @tc.expect:
     *   1. Correct result set for string field
     *   2. Correct result set for numeric field
     *   3. Correct result set for boolean field
     */
    it('lessThanOrEqualToTest_0001', 0, async () => {
      console.log(TAG + "************* lessThanOrEqualToTest_0001 start *************");
      try {
        let predicates = new dataSharePredicates.DataSharePredicates();
        predicates.lessThanOrEqualTo('$.name', 'MaQi');
        let resultSet = await kvStore?.getResultSet(predicates);
        expect(resultSet?.getCount()).assertEqual(3);
        await kvStore?.closeResultSet(resultSet);

        let predicates1 = new dataSharePredicates.DataSharePredicates();
        predicates1.lessThanOrEqualTo('$.age', 21);
        let resultSet1 = await kvStore?.getResultSet(predicates1);
        expect(resultSet1?.getCount()).assertEqual(4);
        await kvStore?.closeResultSet(resultSet1);

        let predicates2 = new dataSharePredicates.DataSharePredicates();
        predicates2.lessThanOrEqualTo('$.isStudent', true);
        let resultSet2 = await kvStore?.getResultSet(predicates2);
        expect(resultSet2?.getCount()).assertEqual(6);
        await kvStore?.closeResultSet(resultSet2);
      } catch (e) {
        console.error(TAG + `get resultSet failed errCode:${e.code}, message${e.message}`);
        expect().assertFail();
      }
      console.log(TAG + "************* lessThanOrEqualToTest_0001 end *************");
    });

    /**
     * @tc.name: notInTest_0001
     * @tc.desc: Verify notIn predicate for different data types in key-value store
     * @tc.type: FUNC
     * @tc.precon: kvStore is initialized and populated with test data
     * @tc.step:
     *   1. Test notIn on string field (name not in ['MaQi', 'ZhangSan']) - expect 4 non-matching entries
     *   2. Test notIn on numeric field (age not in [21, 20]) - expect 3 non-matching entries
     *   3. Test notIn on boolean field (isStudent not in [true]) - expect 3 non-matching entries
     * @tc.expect:
     *   1. Correct result set for string field
     *   2. Correct result set for numeric field
     *   3. Correct result set for boolean field
     */
    it('notInTest_0001', 0, async () => {
      console.log(TAG + "************* notInTest_0001 start *************");
      try {
        let predicates = new dataSharePredicates.DataSharePredicates();
        predicates.notIn('$.name', ['MaQi', 'ZhangSan']);
        let resultSet = await kvStore?.getResultSet(predicates);
        expect(resultSet?.getCount()).assertEqual(4);
        await kvStore?.closeResultSet(resultSet);

        let predicates1 = new dataSharePredicates.DataSharePredicates();
        predicates1.notIn('$.age', [21, 20]);
        let resultSet1 = await kvStore?.getResultSet(predicates1);
        expect(resultSet1?.getCount()).assertEqual(3);
        await kvStore?.closeResultSet(resultSet1);

        let predicates2 = new dataSharePredicates.DataSharePredicates();
        predicates2.notIn('$.isStudent', [true]);
        let resultSet2 = await kvStore?.getResultSet(predicates2);
        expect(resultSet2?.getCount()).assertEqual(3);
        await kvStore?.closeResultSet(resultSet2);
      } catch (e) {
        console.error(TAG + `get resultSet failed errCode:${e.code}, message${e.message}`);
        expect().assertFail();
      }
      console.log(TAG + "************* notInTest_0001 end *************");
    });

    /**
     * @tc.name: likeTest_0001
     * @tc.desc: Verify like predicate for different data types in key-value store
     * @tc.type: FUNC
     * @tc.precon: kvStore is initialized and populated with test data
     * @tc.step:
     *   1. Test like on string field (name LIKE '%an%') - expect 2 matching entries
     *   2. Test like on numeric field (age LIKE '20') - expect 2 matching entries
     *   3. Test like on boolean field (isStudent LIKE 'false') - expect 0 matching entries
     *   4. Test like on boolean field (isStudent LIKE '0') - expect 3 matching entries
     * @tc.expect:
     *   1. Correct result set for string field
     *   2. Correct result set for numeric field
     *   3. Correct result set for boolean field (string representation)
     *   4. Correct result set for boolean field (numeric representation)
     */
    it('likeTest_0001', 0, async () => {
      console.log(TAG + "************* likeTest_0001 start *************");
      try {
        let predicates = new dataSharePredicates.DataSharePredicates();
        predicates.like('name', '%an%');
        let resultSet = await kvStore?.getResultSet(predicates);
        expect(resultSet?.getCount()).assertEqual(2);
        await kvStore?.closeResultSet(resultSet);

        let predicates1 = new dataSharePredicates.DataSharePredicates();
        predicates1.like('age', '20');
        let resultSet1 = await kvStore?.getResultSet(predicates1);
        expect(resultSet1?.getCount()).assertEqual(2);
        await kvStore?.closeResultSet(resultSet1);

        let predicates2 = new dataSharePredicates.DataSharePredicates();
        predicates2.like('isStudent', 'false');
        let resultSet2 = await kvStore?.getResultSet(predicates2);
        expect(resultSet2?.getCount()).assertEqual(0);
        await kvStore?.closeResultSet(resultSet2);

        let predicates3 = new dataSharePredicates.DataSharePredicates();
        predicates3.like('isStudent', '0');
        let resultSet3 = await kvStore?.getResultSet(predicates3);
        expect(resultSet3?.getCount()).assertEqual(3);
        await kvStore?.closeResultSet(resultSet3);
      } catch (e) {
        console.error(TAG + `get resultSet failed errCode:${e.code}, message${e.message}`);
        expect().assertFail();
      }
      console.log(TAG + "************* likeTest_0001 end *************");
    });

    /**
     * @tc.name: orTest_0001
     * @tc.desc: Verify OR logical operator in predicates with different field path notations
     * @tc.type: FUNC
     * @tc.precon: kvStore is initialized and populated with test data
     * @tc.step:
     *   1. Create predicates with OR condition: (name LIKE '%an%') OR (age = -1) using $.fieldPath notation
     *   2. Create predicates with OR condition: (name LIKE '%an%') OR (age = -1) using fieldPath notation
     * @tc.expect:
     *   1. Correct result set with 3 matching entries for$.fieldPath notation
     *   2. Correct result set with 3 matching entries for fieldPath notation
     */
    it('orTest_0001', 0, async () => {
      console.log(TAG + "************* orTest_0001 start *************");
      try {
        let predicates = new dataSharePredicates.DataSharePredicates();
        predicates.like('$.name', '%an%');
        predicates.or().equalTo('$.age', -1);
        let resultSet = await kvStore?.getResultSet(predicates);
        expect(resultSet?.getCount()).assertEqual(3);
        await kvStore?.closeResultSet(resultSet);

        let predicates1 = new dataSharePredicates.DataSharePredicates();
        predicates1.like('name', '%an%');
        predicates1.or().equalTo('age', -1);
        let resultSet1 = await kvStore?.getResultSet(predicates1);
        expect(resultSet1?.getCount()).assertEqual(3);
        await kvStore?.closeResultSet(resultSet1);
      } catch (e) {
        console.error(TAG + `get resultSet failed errCode:${e.code}, message${e.message}`);
        expect().assertFail();
      }
      console.log(TAG + "************* orTest_0001 end *************");
    });
  });
}