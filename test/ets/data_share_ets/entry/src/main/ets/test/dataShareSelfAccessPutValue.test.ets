/*
 * Copyright (C) 2026 Huawei Device Co., Ltd.
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

import { describe, beforeAll, afterEach, afterAll, it, expect, Level, Size, TestType } from '@ohos/hypium';
import dataShare from '@ohos.data.dataShare'
import { BusinessError } from "@ohos.base";
import { ValueType } from '@ohos.data.ValuesBucket';

let g_handle: dataShare.DataProxyHandle | undefined = undefined;

let g_config_default: dataShare.DataProxyConfig = {
  type: dataShare.DataProxyType.SHARED_CONFIG
}

let g_config_4K: dataShare.DataProxyConfig = {
  type: dataShare.DataProxyType.SHARED_CONFIG,
  maxValueLength: dataShare.DataProxyMaxValueLength.MAX_LENGTH_4K
}

let g_config_100K: dataShare.DataProxyConfig = {
  type: dataShare.DataProxyType.SHARED_CONFIG,
  maxValueLength: dataShare.DataProxyMaxValueLength.MAX_LENGTH_100K
}

let TAG = "SelfAccessPutValue ";

export default function DataShareSelfAccessPutValueTest() {
  describe('DataShareSelfAccessPutValueTest', () => {

    beforeAll(async () => {
      await dataShare.createDataProxyHandle().then((handle: dataShare.DataProxyHandle) => {
        if (handle == null || handle == undefined) {
          console.log(TAG + "createDataProxyHandle failed, handle is null");
          return;
        }
        g_handle = handle;
        console.log(TAG + "createDataProxyHandle success");
      }).catch((err: BusinessError) => {
        console.error(TAG + `createDataProxyHandle failed: code:${err.code},message:${err.message}`);
      })
    });

    afterAll(async () => {
      await g_handle!.deleteMyPublishedData(g_config_default);
      console.info(TAG + 'afterAll');
    });

    /**
     * @tc.name: ShareConfig_SelfAccess_PutValue_Test001
     * @tc.desc: Verify publisher can putValue to add a new key on multi-value config
     * @tc.type: FUNC
     * @tc.precon: DataProxyHandle is created, publisher publishes multi-value
     * @tc.step:
     *   1. Publish a ProxyData with isMultiValues=true, values={ 0: "init" }
     *   2. Call putValue(uri, 1, "hello", g_config_4K)
     *   3. Verify operation resolves without exception
     * @tc.expect:
     *   1. putValue resolves successfully
     */
    it('ShareConfig_SelfAccess_PutValue_Test001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      console.log(TAG + "************* ShareConfig_SelfAccess_PutValue_Test001 start *************");
      let testUri = "datashareproxy://com.example.myapplication/test/pv/001";
      try {
        let proxyData: dataShare.ProxyData[] = [{
          uri: testUri,
          values: { 0: "init" },
          isMultiValues: true,
          allowList: [],
          trustProviders: []
        }];
        await g_handle!.publish(proxyData, g_config_4K);
        await g_handle!.putValue(testUri, 1, "hello", g_config_4K);
      } catch (err) {
        console.error(TAG + `ShareConfig_SelfAccess_PutValue_Test001 failed: ${JSON.stringify(err)}`);
        expect().assertFail();
      }
      await g_handle!.delete([testUri], g_config_4K);
      console.log(TAG + "************* ShareConfig_SelfAccess_PutValue_Test001 end *************");
    });

    /**
     * @tc.name: ShareConfig_SelfAccess_PutValue_Test002
     * @tc.desc: Verify publisher can putValue to update an existing key value
     * @tc.type: FUNC
     * @tc.precon: DataProxyHandle is created, publisher publishes multi-value config with key=1 value="old"
     * @tc.step:
     *   1. Publish multi-value config with values={ 1: "old" }
     *   2. Call putValue(uri, 1, "new", g_config_4K)
     *   3. Call getValues to verify the value is updated
     * @tc.expect:
     *   1. putValue resolves successfully
     *   2. getValues returns ["new"]
     */
    it('ShareConfig_SelfAccess_PutValue_Test002', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      console.log(TAG + "************* ShareConfig_SelfAccess_PutValue_Test002 start *************");
      let testUri = "datashareproxy://com.example.myapplication/test/pv/002";
      try {
        let proxyData: dataShare.ProxyData[] = [{
          uri: testUri,
          values: { 1: "old" },
          isMultiValues: true,
          allowList: [],
          trustProviders: []
        }];
        await g_handle!.publish(proxyData, g_config_4K);
        await g_handle!.putValue(testUri, 1, "new", g_config_4K);
        let values: ValueType[] = await g_handle!.getValues(testUri, g_config_4K);
        expect(values.length).assertEqual(1);
        expect(values[0]).assertEqual("new");
      } catch (err) {
        console.error(TAG + `ShareConfig_SelfAccess_PutValue_Test002 failed: ${JSON.stringify(err)}`);
        expect().assertFail();
      }
      await g_handle!.delete([testUri], g_config_4K);
      console.log(TAG + "************* ShareConfig_SelfAccess_PutValue_Test002 end *************");
    });

    /**
     * @tc.name: ShareConfig_SelfAccess_PutValue_Test003
     * @tc.desc: Verify publisher can putValue with int type value
     * @tc.type: FUNC
     * @tc.precon: DataProxyHandle is created, multi-value config published with seed key
     * @tc.step:
     *   1. Publish multi-value config with values={ 0: "seed" }
     *   2. Call putValue(uri, 1, 42, g_config_4K)
     *   3. Verify getValues contains 42
     * @tc.expect:
     *   1. putValue resolves successfully
     *   2. getValues returns 2 elements including 42
     */
    it('ShareConfig_SelfAccess_PutValue_Test003', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      console.log(TAG + "************* ShareConfig_SelfAccess_PutValue_Test003 start *************");
      let testUri = "datashareproxy://com.example.myapplication/test/pv/003";
      try {
        let proxyData: dataShare.ProxyData[] = [{
          uri: testUri,
          values: { 0: "seed" },
          isMultiValues: true,
          allowList: [],
          trustProviders: []
        }];
        await g_handle!.publish(proxyData, g_config_4K);
        await g_handle!.putValue(testUri, 1, 42, g_config_4K);
        let values: ValueType[] = await g_handle!.getValues(testUri, g_config_4K);
        expect(values.length).assertEqual(2);
        let has42 = false;
        for (let i = 0; i < values.length; i++) {
          if (values[i] === 42) { has42 = true; }
        }
        expect(has42).assertTrue();
      } catch (err) {
        console.error(TAG + `ShareConfig_SelfAccess_PutValue_Test003 failed: ${JSON.stringify(err)}`);
        expect().assertFail();
      }
      await g_handle!.delete([testUri], g_config_4K);
      console.log(TAG + "************* ShareConfig_SelfAccess_PutValue_Test003 end *************");
    });

    /**
     * @tc.name: ShareConfig_SelfAccess_PutValue_Test004
     * @tc.desc: Verify publisher can putValue with double type value
     * @tc.type: FUNC
     * @tc.precon: DataProxyHandle is created, multi-value config published with seed key
     * @tc.step:
     *   1. Publish multi-value config with values={ 0: "seed" }
     *   2. Call putValue(uri, 1, 3.14, g_config_4K)
     *   3. Verify getValues contains 3.14
     * @tc.expect:
     *   1. putValue resolves successfully
     *   2. getValues returns 2 elements including 3.14
     */
    it('ShareConfig_SelfAccess_PutValue_Test004', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      console.log(TAG + "************* ShareConfig_SelfAccess_PutValue_Test004 start *************");
      let testUri = "datashareproxy://com.example.myapplication/test/pv/004";
      try {
        let proxyData: dataShare.ProxyData[] = [{
          uri: testUri,
          values: { 0: "seed" },
          isMultiValues: true,
          allowList: [],
          trustProviders: []
        }];
        await g_handle!.publish(proxyData, g_config_4K);
        await g_handle!.putValue(testUri, 1, 3.14, g_config_4K);
        let values: ValueType[] = await g_handle!.getValues(testUri, g_config_4K);
        expect(values.length).assertEqual(2);
        let has314 = false;
        for (let i = 0; i < values.length; i++) {
          if (values[i] === 3.14) { has314 = true; }
        }
        expect(has314).assertTrue();
      } catch (err) {
        console.error(TAG + `ShareConfig_SelfAccess_PutValue_Test004 failed: ${JSON.stringify(err)}`);
        expect().assertFail();
      }
      await g_handle!.delete([testUri], g_config_4K);
      console.log(TAG + "************* ShareConfig_SelfAccess_PutValue_Test004 end *************");
    });

    /**
     * @tc.name: ShareConfig_SelfAccess_PutValue_Test005
     * @tc.desc: Verify publisher can putValue with string type value
     * @tc.type: FUNC
     * @tc.precon: DataProxyHandle is created, multi-value config published with seed key
     * @tc.step:
     *   1. Publish multi-value config with values={ 0: "seed" }
     *   2. Call putValue(uri, 1, "test", g_config_4K)
     *   3. Verify getValues contains "test"
     * @tc.expect:
     *   1. putValue resolves successfully
     *   2. getValues returns 2 elements including "test"
     */
    it('ShareConfig_SelfAccess_PutValue_Test005', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      console.log(TAG + "************* ShareConfig_SelfAccess_PutValue_Test005 start *************");
      let testUri = "datashareproxy://com.example.myapplication/test/pv/005";
      try {
        let proxyData: dataShare.ProxyData[] = [{
          uri: testUri,
          values: { 0: "seed" },
          isMultiValues: true,
          allowList: [],
          trustProviders: []
        }];
        await g_handle!.publish(proxyData, g_config_4K);
        await g_handle!.putValue(testUri, 1, "test", g_config_4K);
        let values: ValueType[] = await g_handle!.getValues(testUri, g_config_4K);
        expect(values.length).assertEqual(2);
        let hasTest = false;
        for (let i = 0; i < values.length; i++) {
          if (values[i] === "test") { hasTest = true; }
        }
        expect(hasTest).assertTrue();
      } catch (err) {
        console.error(TAG + `ShareConfig_SelfAccess_PutValue_Test005 failed: ${JSON.stringify(err)}`);
        expect().assertFail();
      }
      await g_handle!.delete([testUri], g_config_4K);
      console.log(TAG + "************* ShareConfig_SelfAccess_PutValue_Test005 end *************");
    });

    /**
     * @tc.name: ShareConfig_SelfAccess_PutValue_Test006
     * @tc.desc: Verify publisher can putValue with boolean type value
     * @tc.type: FUNC
     * @tc.precon: DataProxyHandle is created, multi-value config published with seed key
     * @tc.step:
     *   1. Publish multi-value config with values={ 0: "seed" }
     *   2. Call putValue(uri, 1, false, g_config_4K)
     *   3. Verify getValues contains false
     * @tc.expect:
     *   1. putValue resolves successfully
     *   2. getValues returns 2 elements including false
     */
    it('ShareConfig_SelfAccess_PutValue_Test006', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      console.log(TAG + "************* ShareConfig_SelfAccess_PutValue_Test006 start *************");
      let testUri = "datashareproxy://com.example.myapplication/test/pv/006";
      try {
        let proxyData: dataShare.ProxyData[] = [{
          uri: testUri,
          values: { 0: "seed" },
          isMultiValues: true,
          allowList: [],
          trustProviders: []
        }];
        await g_handle!.publish(proxyData, g_config_4K);
        await g_handle!.putValue(testUri, 1, false, g_config_4K);
        let values: ValueType[] = await g_handle!.getValues(testUri, g_config_4K);
        expect(values.length).assertEqual(2);
        let hasFalse = false;
        for (let i = 0; i < values.length; i++) {
          if (values[i] === false) { hasFalse = true; }
        }
        expect(hasFalse).assertTrue();
      } catch (err) {
        console.error(TAG + `ShareConfig_SelfAccess_PutValue_Test006 failed: ${JSON.stringify(err)}`);
        expect().assertFail();
      }
      await g_handle!.delete([testUri], g_config_4K);
      console.log(TAG + "************* ShareConfig_SelfAccess_PutValue_Test006 end *************");
    });

    /**
     * @tc.name: ShareConfig_SelfAccess_PutValue_Test007
     * @tc.desc: Verify publisher can putValue with negative key
     * @tc.type: FUNC
     * @tc.precon: DataProxyHandle is created, multi-value config published with seed key
     * @tc.step:
     *   1. Publish multi-value config with values={ 0: "seed" }
     *   2. Call putValue(uri, -1, "negative", g_config_4K)
     *   3. Verify getValues contains "negative"
     * @tc.expect:
     *   1. putValue resolves successfully (NAPI layer converts int32 to string "-1")
     *   2. getValues returns 2 elements including "negative"
     */
    it('ShareConfig_SelfAccess_PutValue_Test007', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      console.log(TAG + "************* ShareConfig_SelfAccess_PutValue_Test007 start *************");
      let testUri = "datashareproxy://com.example.myapplication/test/pv/007";
      try {
        let proxyData: dataShare.ProxyData[] = [{
          uri: testUri,
          values: { 0: "seed" },
          isMultiValues: true,
          allowList: [],
          trustProviders: []
        }];
        await g_handle!.publish(proxyData, g_config_4K);
        await g_handle!.putValue(testUri, -1, "negative", g_config_4K);
        let values: ValueType[] = await g_handle!.getValues(testUri, g_config_4K);
        expect(values.length).assertEqual(2);
        let hasNeg = false;
        for (let i = 0; i < values.length; i++) {
          if (values[i] === "negative") { hasNeg = true; }
        }
        expect(hasNeg).assertTrue();
      } catch (err) {
        console.error(TAG + `ShareConfig_SelfAccess_PutValue_Test007 failed: ${JSON.stringify(err)}`);
        expect().assertFail();
      }
      await g_handle!.delete([testUri], g_config_4K);
      console.log(TAG + "************* ShareConfig_SelfAccess_PutValue_Test007 end *************");
    });

    /**
     * @tc.name: ShareConfig_SelfAccess_PutValue_Test101
     * @tc.desc: Verify putValue fails with INCOMPATIBLE_CONFIG_TYPE on non-multi-value config
     * @tc.type: FUNC
     * @tc.precon: DataProxyHandle is created, normal (non-multi-value) config published
     * @tc.step:
     *   1. Publish a normal ProxyData (isMultiValues=false)
     *   2. Call putValue(uri, 1, "v", g_config_4K)
     *   3. Verify BusinessError 15700014 is thrown
     * @tc.expect:
     *   1. putValue throws BusinessError with code 15700014 (INCOMPATIBLE_CONFIG_TYPE)
     */
    it('ShareConfig_SelfAccess_PutValue_Test101', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL2, async () => {
      console.log(TAG + "************* ShareConfig_SelfAccess_PutValue_Test101 start *************");
      let testUri = "datashareproxy://com.example.myapplication/test/pv/norm101";
      try {
        let proxyData: dataShare.ProxyData[] = [{
          uri: testUri,
          value: "normal_value",
          allowList: []
        }];
        await g_handle!.publish(proxyData, g_config_4K);
        await g_handle!.putValue(testUri, 1, "v", g_config_4K);
        expect().assertFail();
      } catch (err) {
        let businessErr = err as BusinessError;
        expect(businessErr.code).assertEqual(15700014);
      }
      await g_handle!.delete([testUri], g_config_4K);
      console.log(TAG + "************* ShareConfig_SelfAccess_PutValue_Test101 end *************");
    });

    /**
     * @tc.name: ShareConfig_SelfAccess_PutValue_Test102
     * @tc.desc: Verify putValue fails with OVER_LIMIT when per-app key count exceeds 10
     * @tc.type: FUNC
     * @tc.precon: DataProxyHandle is created, publisher has 1 seed key + 9 putValue keys (total 10, at limit)
     * @tc.step:
     *   1. Publish multi-value config with values={ 0: "init" } (1 seed key)
     *   2. putValue 9 keys (key=1~9) to reach 10-key per-app limit
     *   3. Call putValue(uri, 10, "over", g_config_100K) for 11th key
     *   4. Verify BusinessError 15700014 is thrown
     * @tc.expect:
     *   1. putValue for 11th key throws BusinessError with code 15700014 (OVER_LIMIT)
     */
    it('ShareConfig_SelfAccess_PutValue_Test102', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL2, async () => {
      console.log(TAG + "************* ShareConfig_SelfAccess_PutValue_Test102 start *************");
      let testUri = "datashareproxy://com.example.myapplication/test/pv/102";
      try {
        let proxyData: dataShare.ProxyData[] = [{
          uri: testUri,
          values: { 0: "init" },
          isMultiValues: true,
          allowList: [],
          trustProviders: []
        }];
        await g_handle!.publish(proxyData, g_config_100K);
        for (let i = 1; i <= 9; i++) {
          await g_handle!.putValue(testUri, i, `val_${i}`, g_config_100K);
        }
        await g_handle!.putValue(testUri, 10, "over", g_config_100K);
        expect().assertFail();
      } catch (err) {
        let businessErr = err as BusinessError;
        expect(businessErr.code).assertEqual(15700014);
      }
      await g_handle!.delete([testUri], g_config_100K);
      console.log(TAG + "************* ShareConfig_SelfAccess_PutValue_Test102 end *************");
    });

    /**
     * @tc.name: ShareConfig_SelfAccess_PutValue_Test103
     * @tc.desc: Verify putValue fails with OVER_LIMIT when single value exceeds 4096 bytes
     * @tc.type: FUNC
     * @tc.precon: DataProxyHandle is created, multi-value config published with seed key
     * @tc.step:
     *   1. Publish multi-value config with values={ 0: "init" }
     *   2. Call putValue(uri, 1, string > 4096 chars, g_config_100K)
     *   3. Verify BusinessError 15700014 is thrown
     * @tc.expect:
     *   1. putValue throws BusinessError with code 15700014 (OVER_LIMIT, MAX_SINGLE_MULTIVALUE_LENGTH)
     */
    it('ShareConfig_SelfAccess_PutValue_Test103', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL2, async () => {
      console.log(TAG + "************* ShareConfig_SelfAccess_PutValue_Test103 start *************");
      let testUri = "datashareproxy://com.example.myapplication/test/pv/103";
      try {
        let proxyData: dataShare.ProxyData[] = [{
          uri: testUri,
          values: { 0: "init" },
          isMultiValues: true,
          allowList: [],
          trustProviders: []
        }];
        await g_handle!.publish(proxyData, g_config_100K);
        let overSizeStr = 'A'.repeat(4097);
        await g_handle!.putValue(testUri, 1, overSizeStr, g_config_100K);
        expect().assertFail();
      } catch (err) {
        let businessErr = err as BusinessError;
        expect(businessErr.code).assertEqual(15700014);
      }
      await g_handle!.delete([testUri], g_config_100K);
      console.log(TAG + "************* ShareConfig_SelfAccess_PutValue_Test103 end *************");
    });

    /**
     * @tc.name: ShareConfig_SelfAccess_PutValue_Test104
     * @tc.desc: Verify putValue fails with OVER_LIMIT when total size exceeds 4K maxValueLength
     * @tc.type: FUNC
     * @tc.precon: DataProxyHandle is created, multi-value config published with seed key and 4K maxValueLength
     * @tc.step:
     *   1. Publish multi-value config with values={ 0: "init" }, g_config_4K
     *   2. putValue keys until total size approaches 4096
     *   3. putValue another key that causes total size to exceed 4096
     *   4. Verify BusinessError 15700014 is thrown
     * @tc.expect:
     *   1. Final putValue throws BusinessError with code 15700014 (OVER_LIMIT, total size)
     */
    it('ShareConfig_SelfAccess_PutValue_Test104', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL2, async () => {
      console.log(TAG + "************* ShareConfig_SelfAccess_PutValue_Test104 start *************");
      let testUri = "datashareproxy://com.example.myapplication/test/pv/104";
      try {
        let proxyData: dataShare.ProxyData[] = [{
          uri: testUri,
          values: { 0: "init" },
          isMultiValues: true,
          allowList: [],
          trustProviders: []
        }];
        await g_handle!.publish(proxyData, g_config_4K);
        let nearLimitStr = 'B'.repeat(1000);
        await g_handle!.putValue(testUri, 1, nearLimitStr, g_config_4K);
        await g_handle!.putValue(testUri, 2, nearLimitStr, g_config_4K);
        await g_handle!.putValue(testUri, 3, nearLimitStr, g_config_4K);
        let overLimitStr = 'C'.repeat(1100);
        await g_handle!.putValue(testUri, 4, overLimitStr, g_config_4K);
        expect().assertFail();
      } catch (err) {
        let businessErr = err as BusinessError;
        expect(businessErr.code).assertEqual(15700014);
      }
      await g_handle!.delete([testUri], g_config_4K);
      console.log(TAG + "************* ShareConfig_SelfAccess_PutValue_Test104 end *************");
    });

    /**
     * @tc.name: ShareConfig_SelfAccess_PutValue_Test106
     * @tc.desc: Verify putValue fails with URI_NOT_EXIST when URI does not exist
     * @tc.type: FUNC
     * @tc.precon: DataProxyHandle is created, no data published for the URI
     * @tc.step:
     *   1. Call putValue with a non-existent URI
     *   2. Verify BusinessError 15700011 is thrown
     * @tc.expect:
     *   1. putValue throws BusinessError with code 15700011 (URI_NOT_EXIST)
     */
    it('ShareConfig_SelfAccess_PutValue_Test106', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL2, async () => {
      console.log(TAG + "************* ShareConfig_SelfAccess_PutValue_Test106 start *************");
      try {
        let notExistUri = "datashareproxy://com.example.myapplication/test/pv/notexist106";
        await g_handle!.putValue(notExistUri, 1, "v", g_config_4K);
        expect().assertFail();
      } catch (err) {
        let businessErr = err as BusinessError;
        expect(businessErr.code).assertEqual(15700011);
      }
      console.log(TAG + "************* ShareConfig_SelfAccess_PutValue_Test106 end *************");
    });

    /**
     * @tc.name: ShareConfig_SelfAccess_PutValue_Test107
     * @tc.desc: Verify putValue fails with 15700014 when URI length exceeds 256 bytes (NAPI layer)
     * @tc.type: FUNC
     * @tc.precon: DataProxyHandle is created
     * @tc.step:
     *   1. Call putValue with URI longer than 256 bytes
     *   2. Verify BusinessError 15700014 is thrown (NAPI layer interception)
     * @tc.expect:
     *   1. putValue throws BusinessError with code 15700014
     */
    it('ShareConfig_SelfAccess_PutValue_Test107', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL2, async () => {
      console.log(TAG + "************* ShareConfig_SelfAccess_PutValue_Test107 start *************");
      try {
        let overLengthUri = "datashareproxy://com.example.myapplication/" + 'X'.repeat(256);
        await g_handle!.putValue(overLengthUri, 1, "v", g_config_4K);
        expect().assertFail();
      } catch (err) {
        let businessErr = err as BusinessError;
        expect(businessErr.code).assertEqual('15700014');
      }
      console.log(TAG + "************* ShareConfig_SelfAccess_PutValue_Test107 end *************");
    });

    // ===== flow tests (putValue lifecycle) =====

    /**
     * @tc.name: ShareConfig_SelfAccess_PutValue_Test201
     * @tc.desc: Verify putValue fails with URI_NOT_EXIST after deleting config
     * @tc.type: FUNC
     * @tc.precon: DataProxyHandle is created, multi-value config published then deleted
     * @tc.step:
     *   1. Publish multi-value config with values={ 0: "init" }
     *   2. delete([uri], g_config_4K) to remove the data
     *   3. Call putValue(uri, 1, "v", g_config_4K)
     *   4. Verify BusinessError 15700011 is thrown
     * @tc.expect:
     *   1. Step 3 throws BusinessError with code 15700011 (URI_NOT_EXIST)
     */
    it('ShareConfig_SelfAccess_PutValue_Test201', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL2, async () => {
      console.log(TAG + "************* ShareConfig_SelfAccess_PutValue_Test201 start *************");
      let testUri = "datashareproxy://com.example.myapplication/test/pv/flow201";
      try {
        let proxyData: dataShare.ProxyData[] = [{
          uri: testUri,
          values: { 0: "init" },
          isMultiValues: true,
          allowList: [],
          trustProviders: []
        }];
        await g_handle!.publish(proxyData, g_config_4K);
        await g_handle!.delete([testUri], g_config_4K);

        await g_handle!.putValue(testUri, 1, "v", g_config_4K);
        expect().assertFail();
      } catch (err) {
        let businessErr = err as BusinessError;
        expect(businessErr.code).assertEqual(15700011);
      }
      console.log(TAG + "************* ShareConfig_SelfAccess_PutValue_Test201 end *************");
    });

    /**
     * @tc.name: ShareConfig_SelfAccess_PutValue_Test202
     * @tc.desc: Verify re-publish multi-value config allows putValue after delete
     * @tc.type: FUNC
     * @tc.precon: DataProxyHandle is created
     * @tc.step:
     *   1. Publish multi-value config with values={ 0: "init" } → delete([uri]) → re-publish
     *   2. Call putValue(uri, 1, "v", g_config_4K)
     *   3. Verify putValue resolves successfully
     * @tc.expect:
     *   1. putValue resolves successfully after re-publish
     */
    it('ShareConfig_SelfAccess_PutValue_Test202', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL2, async () => {
      console.log(TAG + "************* ShareConfig_SelfAccess_PutValue_Test202 start *************");
      let testUri = "datashareproxy://com.example.myapplication/test/pv/flow202";
      try {
        let proxyData: dataShare.ProxyData[] = [{
          uri: testUri,
          values: { 0: "init" },
          isMultiValues: true,
          allowList: [],
          trustProviders: []
        }];
        await g_handle!.publish(proxyData, g_config_4K);
        await g_handle!.delete([testUri], g_config_4K);
        await g_handle!.publish(proxyData, g_config_4K);
        await g_handle!.putValue(testUri, 1, "v", g_config_4K);
      } catch (err) {
        console.error(TAG + `ShareConfig_SelfAccess_PutValue_Test202 failed: ${JSON.stringify(err)}`);
        expect().assertFail();
      }
      await g_handle!.delete([testUri], g_config_4K);
      console.log(TAG + "************* ShareConfig_SelfAccess_PutValue_Test202 end *************");
    });

    /**
     * @tc.name: ShareConfig_SelfAccess_PutValue_Test203
     * @tc.desc: Verify publish non-multi → putValue fails → delete → publish multi → putValue succeeds
     * @tc.type: FUNC
     * @tc.precon: DataProxyHandle is created
     * @tc.step:
     *   1. Publish normal config (isMultiValues=false)
     *   2. Call putValue → expect 15700014
     *   3. delete([uri], g_config_4K)
     *   4. Re-publish multi-value config (isMultiValues=true, values={ 0: "init" })
     *   5. Call putValue(uri, 1, "v") → expect success
     * @tc.expect:
     *   1. Step 2 fails with 15700014
     *   2. Step 5 succeeds
     */
    it('ShareConfig_SelfAccess_PutValue_Test203', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL2, async () => {
      console.log(TAG + "************* ShareConfig_SelfAccess_PutValue_Test203 start *************");
      let testUri = "datashareproxy://com.example.myapplication/test/pv/flow203";
      try {
        let normalProxyData: dataShare.ProxyData[] = [{
          uri: testUri,
          value: "static_value",
          allowList: []
        }];
        await g_handle!.publish(normalProxyData, g_config_4K);

        await g_handle!.putValue(testUri, 1, "v", g_config_4K);
        expect().assertFail();
      } catch (err) {
        let businessErr = err as BusinessError;
        expect(businessErr.code).assertEqual(15700014);
      }
      await g_handle!.delete([testUri], g_config_4K);

      try {
        let multiProxyData: dataShare.ProxyData[] = [{
          uri: testUri,
          values: { 0: "init" },
          isMultiValues: true,
          allowList: [],
          trustProviders: []
        }];
        await g_handle!.publish(multiProxyData, g_config_4K);

        await g_handle!.putValue(testUri, 1, "v", g_config_4K);
      } catch (err) {
        console.error(TAG + `ShareConfig_SelfAccess_PutValue_Test203 second phase failed: ${JSON.stringify(err)}`);
        expect().assertFail();
      }
      await g_handle!.delete([testUri], g_config_4K);
      console.log(TAG + "************* ShareConfig_SelfAccess_PutValue_Test203 end *************");
    });

    // ===== boundary tests =====

    /**
     * @tc.name: ShareConfig_SelfAccess_PutValue_Test301
     * @tc.desc: Verify putValue succeeds when URI length is exactly 256 bytes
     * @tc.type: FUNC
     * @tc.precon: DataProxyHandle is created
     * @tc.step:
     *   1. Publish multi-value config with URI of exactly 256 bytes and values={ 0: "init" }
     *   2. Call putValue with that URI
     *   3. Verify operation succeeds
     * @tc.expect:
     *   1. putValue resolves successfully (256-byte URI is within limit)
     */
    it('ShareConfig_SelfAccess_PutValue_Test301', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL2, async () => {
      console.log(TAG + "************* ShareConfig_SelfAccess_PutValue_Test301 start *************");
      let base = "datashareproxy://com.example.myapplication/";
      let exact256Uri = base + 'Y'.repeat(256 - base.length);
      expect(exact256Uri.length).assertEqual(256);

      try {
        let proxyData: dataShare.ProxyData[] = [{
          uri: exact256Uri,
          values: { 0: "init" },
          isMultiValues: true,
          allowList: [],
          trustProviders: []
        }];
        await g_handle!.publish(proxyData, g_config_4K);
        await g_handle!.putValue(exact256Uri, 1, "v", g_config_4K);
      } catch (err) {
        console.error(TAG + `ShareConfig_SelfAccess_PutValue_Test301 failed: ${JSON.stringify(err)}`);
        expect().assertFail();
      }
      await g_handle!.delete([exact256Uri], g_config_4K);
      console.log(TAG + "************* ShareConfig_SelfAccess_PutValue_Test301 end *************");
    });

    /**
     * @tc.name: ShareConfig_SelfAccess_PutValue_Test302
     * @tc.desc: Verify putValue fails when URI length is exactly 257 bytes
     * @tc.type: FUNC
     * @tc.precon: DataProxyHandle is created
     * @tc.step:
     *   1. Call putValue with URI of exactly 257 bytes
     *   2. Verify BusinessError 15700014 is thrown (NAPI layer)
     * @tc.expect:
     *   1. putValue throws BusinessError with code 15700014
     */
    it('ShareConfig_SelfAccess_PutValue_Test302', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL2, async () => {
      console.log(TAG + "************* ShareConfig_SelfAccess_PutValue_Test302 start *************");
      try {
        let base = "datashareproxy://com.example.myapplication/";
        let over257Uri = base + 'Z'.repeat(257 - base.length);
        await g_handle!.putValue(over257Uri, 1, "v", g_config_4K);
        expect().assertFail();
      } catch (err) {
        let businessErr = err as BusinessError;
        expect(businessErr.code).assertEqual('15700014');
      }
      console.log(TAG + "************* ShareConfig_SelfAccess_PutValue_Test302 end *************");
    });

    /**
     * @tc.name: ShareConfig_SelfAccess_PutValue_Test303
     * @tc.desc: Verify putValue succeeds when value length is exactly 4096 bytes
     * @tc.type: FUNC
     * @tc.precon: DataProxyHandle is created, multi-value config published with seed key
     * @tc.step:
     *   1. Publish multi-value config with values={ 0: "init" }
     *   2. Call putValue with string value of exactly 4096 bytes
     *   3. Verify operation succeeds
     * @tc.expect:
     *   1. putValue resolves successfully (4096-byte value is within limit)
     */
    it('ShareConfig_SelfAccess_PutValue_Test303', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL2, async () => {
      console.log(TAG + "************* ShareConfig_SelfAccess_PutValue_Test303 start *************");
      let testUri = "datashareproxy://com.example.myapplication/test/pv/bnd303";
      try {
        let proxyData: dataShare.ProxyData[] = [{
          uri: testUri,
          values: { 0: "init" },
          isMultiValues: true,
          allowList: [],
          trustProviders: []
        }];
        await g_handle!.publish(proxyData, g_config_100K);
        let exact4096Str = 'D'.repeat(4096);
        await g_handle!.putValue(testUri, 1, exact4096Str, g_config_100K);
      } catch (err) {
        console.error(TAG + `ShareConfig_SelfAccess_PutValue_Test303 failed: ${JSON.stringify(err)}`);
        expect().assertFail();
      }
      await g_handle!.delete([testUri], g_config_100K);
      console.log(TAG + "************* ShareConfig_SelfAccess_PutValue_Test303 end *************");
    });

    /**
     * @tc.name: ShareConfig_SelfAccess_PutValue_Test304
     * @tc.desc: Verify putValue fails when value length is exactly 4097 bytes
     * @tc.type: FUNC
     * @tc.precon: DataProxyHandle is created, multi-value config published with seed key
     * @tc.step:
     *   1. Publish multi-value config with values={ 0: "init" }
     *   2. Call putValue with string value of exactly 4097 bytes
     *   3. Verify BusinessError 15700014 is thrown
     * @tc.expect:
     *   1. putValue throws BusinessError with code 15700014 (OVER_LIMIT)
     */
    it('ShareConfig_SelfAccess_PutValue_Test304', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL2, async () => {
      console.log(TAG + "************* ShareConfig_SelfAccess_PutValue_Test304 start *************");
      let testUri = "datashareproxy://com.example.myapplication/test/pv/bnd304";
      try {
        let proxyData: dataShare.ProxyData[] = [{
          uri: testUri,
          values: { 0: "init" },
          isMultiValues: true,
          allowList: [],
          trustProviders: []
        }];
        await g_handle!.publish(proxyData, g_config_100K);
        let over4097Str = 'E'.repeat(4097);
        await g_handle!.putValue(testUri, 1, over4097Str, g_config_100K);
        expect().assertFail();
      } catch (err) {
        let businessErr = err as BusinessError;
        expect(businessErr.code).assertEqual(15700014);
      }
      await g_handle!.delete([testUri], g_config_100K);
      console.log(TAG + "************* ShareConfig_SelfAccess_PutValue_Test304 end *************");
    });

    /**
     * @tc.name: ShareConfig_SelfAccess_PutValue_Test305
     * @tc.desc: Verify putValue succeeds with exactly 10 keys per app (maximum allowed)
     * @tc.type: FUNC
     * @tc.precon: DataProxyHandle is created, multi-value config published with 1 seed key
     * @tc.step:
     *   1. Publish multi-value config with values={ 0: "init" } (1 seed key)
     *   2. Call putValue 9 times (key=1~9) to reach 10-key per-app limit
     *   3. Verify all putValue calls succeed
     * @tc.expect:
     *   1. All 9 putValue calls resolve successfully (total 10 keys including seed)
     */
    it('ShareConfig_SelfAccess_PutValue_Test305', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL2, async () => {
      console.log(TAG + "************* ShareConfig_SelfAccess_PutValue_Test305 start *************");
      let testUri = "datashareproxy://com.example.myapplication/test/pv/bnd305";
      try {
        let proxyData: dataShare.ProxyData[] = [{
          uri: testUri,
          values: { 0: "init" },
          isMultiValues: true,
          allowList: [],
          trustProviders: []
        }];
        await g_handle!.publish(proxyData, g_config_100K);
        for (let i = 1; i <= 9; i++) {
          await g_handle!.putValue(testUri, i, `key_${i}`, g_config_100K);
        }
        let values: ValueType[] = await g_handle!.getValues(testUri, g_config_100K);
        expect(values.length).assertEqual(10);
      } catch (err) {
        console.error(TAG + `ShareConfig_SelfAccess_PutValue_Test305 failed: ${JSON.stringify(err)}`);
        expect().assertFail();
      }
      await g_handle!.delete([testUri], g_config_100K);
      console.log(TAG + "************* ShareConfig_SelfAccess_PutValue_Test305 end *************");
    });

    /**
     * @tc.name: ShareConfig_SelfAccess_PutValue_Test306
     * @tc.desc: Verify putValue succeeds with key=0 (minimum non-negative int32)
     * @tc.type: FUNC
     * @tc.precon: DataProxyHandle is created, multi-value config published with seed key
     * @tc.step:
     *   1. Publish multi-value config with values={ 1: "seed" }
     *   2. Call putValue(uri, 0, "v", g_config_4K)
     *   3. Verify operation succeeds (NAPI: 0 → to_string("0"))
     * @tc.expect:
     *   1. putValue resolves successfully
     */
    it('ShareConfig_SelfAccess_PutValue_Test306', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.log(TAG + "************* ShareConfig_SelfAccess_PutValue_Test306 start *************");
      let testUri = "datashareproxy://com.example.myapplication/test/pv/bnd306";
      try {
        let proxyData: dataShare.ProxyData[] = [{
          uri: testUri,
          values: { 1: "seed" },
          isMultiValues: true,
          allowList: [],
          trustProviders: []
        }];
        await g_handle!.publish(proxyData, g_config_4K);
        await g_handle!.putValue(testUri, 0, "v", g_config_4K);
      } catch (err) {
        console.error(TAG + `ShareConfig_SelfAccess_PutValue_Test306 failed: ${JSON.stringify(err)}`);
        expect().assertFail();
      }
      await g_handle!.delete([testUri], g_config_4K);
      console.log(TAG + "************* ShareConfig_SelfAccess_PutValue_Test306 end *************");
    });

    /**
     * @tc.name: ShareConfig_SelfAccess_PutValue_Test307
     * @tc.desc: Verify putValue succeeds with key=INT32_MAX (2147483647)
     * @tc.type: FUNC
     * @tc.precon: DataProxyHandle is created, multi-value config published with seed key
     * @tc.step:
     *   1. Publish multi-value config with values={ 0: "seed" }
     *   2. Call putValue(uri, 2147483647, "v", g_config_4K)
     *   3. Verify operation succeeds
     * @tc.expect:
     *   1. putValue resolves successfully (NAPI: int32 → to_string)
     */
    it('ShareConfig_SelfAccess_PutValue_Test307', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      console.log(TAG + "************* ShareConfig_SelfAccess_PutValue_Test307 start *************");
      let testUri = "datashareproxy://com.example.myapplication/test/pv/bnd307";
      try {
        let proxyData: dataShare.ProxyData[] = [{
          uri: testUri,
          values: { 0: "seed" },
          isMultiValues: true,
          allowList: [],
          trustProviders: []
        }];
        await g_handle!.publish(proxyData, g_config_4K);
        await g_handle!.putValue(testUri, 2147483647, "v", g_config_4K);
      } catch (err) {
        console.error(TAG + `ShareConfig_SelfAccess_PutValue_Test307 failed: ${JSON.stringify(err)}`);
        expect().assertFail();
      }
      await g_handle!.delete([testUri], g_config_4K);
      console.log(TAG + "************* ShareConfig_SelfAccess_PutValue_Test307 end *************");
    });

  });
}
