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

import { describe, beforeAll, afterAll, it, expect, Level, Size, TestType } from '@ohos/hypium';
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

let TAG = "SelfAccessGetValues ";

export default function DataShareSelfAccessGetValuesTest() {
  describe('DataShareSelfAccessGetValuesTest', () => {

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

    // ===== getValues — normal branch =====

    /**
     * @tc.name: ShareConfig_SelfAccess_GetValues_Test001
     * @tc.desc: Verify publisher getValues returns all values under multi-value config URI
     * @tc.type: FUNC
     * @tc.precon: DataProxyHandle is created, publisher publishes multi-value config with values={ 1: "v1", 2: "v2" }
     * @tc.step:
     *   1. Publish multi-value config with values={ 1: "v1", 2: "v2" }
     *   2. Call getValues(uri, g_config_4K)
     *   3. Verify returned array contains ["v1", "v2"]
     * @tc.expect:
     *   1. getValues returns ValueType[] with 2 elements matching all values under the URI
     */
    it('ShareConfig_SelfAccess_GetValues_Test001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      console.log(TAG + "************* ShareConfig_SelfAccess_GetValues_Test001 start *************");
      let testUri = "datashareproxy://com.example.myapplication/test/gv/001";
      try {
        let proxyData: dataShare.ProxyData[] = [{
          uri: testUri,
          values: { 1: "v1", 2: "v2" },
          isMultiValues: true,
          allowList: [],
          trustProviders: []
        }];
        await g_handle!.publish(proxyData, g_config_4K);
        let values: ValueType[] = await g_handle!.getValues(testUri, g_config_4K);
        expect(values.length).assertEqual(2);
        let hasV1 = false;
        let hasV2 = false;
        for (let i = 0; i < values.length; i++) {
          if (values[i] === "v1") { hasV1 = true; }
          if (values[i] === "v2") { hasV2 = true; }
        }
        expect(hasV1).assertTrue();
        expect(hasV2).assertTrue();
      } catch (err) {
        console.error(TAG + `ShareConfig_SelfAccess_GetValues_Test001 failed: ${JSON.stringify(err)}`);
        expect().assertFail();
      }
      await g_handle!.delete([testUri], g_config_4K);
      console.log(TAG + "************* ShareConfig_SelfAccess_GetValues_Test001 end *************");
    });

    /**
     * @tc.name: ShareConfig_SelfAccess_GetValues_Test002
     * @tc.desc: Verify getValues returns mixed type values (string, int, boolean)
     * @tc.type: FUNC
     * @tc.precon: DataProxyHandle is created, publisher publishes with mixed-type values
     * @tc.step:
     *   1. Publish multi-value config with values={ 1: "str", 2: 42, 3: true }
     *   2. Call getValues(uri, g_config_4K)
     *   3. Verify returned array contains all 3 values with correct types
     * @tc.expect:
     *   1. getValues returns ValueType[] with 3 elements of mixed types
     */
    it('ShareConfig_SelfAccess_GetValues_Test002', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      console.log(TAG + "************* ShareConfig_SelfAccess_GetValues_Test002 start *************");
      let testUri = "datashareproxy://com.example.myapplication/test/gv/002";
      try {
        let proxyData: dataShare.ProxyData[] = [{
          uri: testUri,
          values: { 1: "str", 2: 42, 3: true },
          isMultiValues: true,
          allowList: [],
          trustProviders: []
        }];
        await g_handle!.publish(proxyData, g_config_4K);
        let values: ValueType[] = await g_handle!.getValues(testUri, g_config_4K);
        expect(values.length).assertEqual(3);
        let hasStr = false;
        let has42 = false;
        let hasTrue = false;
        for (let i = 0; i < values.length; i++) {
          if (values[i] === "str") { hasStr = true; }
          if (values[i] === 42) { has42 = true; }
          if (values[i] === true) { hasTrue = true; }
        }
        expect(hasStr).assertTrue();
        expect(has42).assertTrue();
        expect(hasTrue).assertTrue();
      } catch (err) {
        console.error(TAG + `ShareConfig_SelfAccess_GetValues_Test002 failed: ${JSON.stringify(err)}`);
        expect().assertFail();
      }
      await g_handle!.delete([testUri], g_config_4K);
      console.log(TAG + "************* ShareConfig_SelfAccess_GetValues_Test002 end *************");
    });

    /**
     * @tc.name: ShareConfig_SelfAccess_GetValues_Test003
     * @tc.desc: Verify getValues returns empty array after all keys are removed
     * @tc.type: FUNC
     * @tc.precon: DataProxyHandle is created, multi-value config published then all keys removed via removeValue
     * @tc.step:
     *   1. Publish multi-value config with values={ 1: "v" }
     *   2. Call removeValue(uri, 1) to remove all keys
     *   3. Call getValues(uri, g_config_4K) to verify returned array is empty
     * @tc.expect:
     *   1. getValues returns [] (empty array after all keys removed)
     */
    it('ShareConfig_SelfAccess_GetValues_Test003', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      console.log(TAG + "************* ShareConfig_SelfAccess_GetValues_Test003 start *************");
      let testUri = "datashareproxy://com.example.myapplication/test/gv/003";
      try {
        let proxyData: dataShare.ProxyData[] = [{
          uri: testUri,
          values: { 1: "v" },
          isMultiValues: true,
          allowList: [],
          trustProviders: []
        }];
        await g_handle!.publish(proxyData, g_config_4K);
        await g_handle!.removeValue(testUri, 1, g_config_4K);
        let values: ValueType[] = await g_handle!.getValues(testUri, g_config_4K);
        expect(values.length).assertEqual(0);
      } catch (err) {
        console.error(TAG + `ShareConfig_SelfAccess_GetValues_Test003 failed: ${JSON.stringify(err)}`);
        expect().assertFail();
      }
      await g_handle!.delete([testUri], g_config_4K);
      console.log(TAG + "************* ShareConfig_SelfAccess_GetValues_Test003 end *************");
    });

    // ===== getValues — error branch =====

    /**
     * @tc.name: ShareConfig_SelfAccess_GetValues_Test101
     * @tc.desc: Verify getValues fails with INCOMPATIBLE_CONFIG_TYPE on non-multi-value config
     * @tc.type: FUNC
     * @tc.precon: DataProxyHandle is created, normal (non-multi-value) config published
     * @tc.step:
     *   1. Publish a normal ProxyData (isMultiValues=false)
     *   2. Call getValues(uri, g_config_4K)
     *   3. Verify BusinessError 15700014 is thrown
     * @tc.expect:
     *   1. getValues throws BusinessError with code 15700014 (INCOMPATIBLE_CONFIG_TYPE)
     */
    it('ShareConfig_SelfAccess_GetValues_Test101', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL2, async () => {
      console.log(TAG + "************* ShareConfig_SelfAccess_GetValues_Test101 start *************");
      let testUri = "datashareproxy://com.example.myapplication/test/gv/norm101";
      try {
        let proxyData: dataShare.ProxyData[] = [{
          uri: testUri,
          value: "normal_value",
          allowList: []
        }];
        await g_handle!.publish(proxyData, g_config_4K);
        await g_handle!.getValues(testUri, g_config_4K);
        expect().assertFail();
      } catch (err) {
        let businessErr = err as BusinessError;
        expect(businessErr.code).assertEqual(15700014);
      }
      await g_handle!.delete([testUri], g_config_4K);
      console.log(TAG + "************* ShareConfig_SelfAccess_GetValues_Test101 end *************");
    });

    /**
     * @tc.name: ShareConfig_SelfAccess_GetValues_Test102
     * @tc.desc: Verify getValues fails with URI_NOT_EXIST when URI does not exist
     * @tc.type: FUNC
     * @tc.precon: DataProxyHandle is created, no data published for the URI
     * @tc.step:
     *   1. Call getValues with a non-existent URI
     *   2. Verify BusinessError 15700011 is thrown
     * @tc.expect:
     *   1. getValues throws BusinessError with code 15700011 (URI_NOT_EXIST)
     */
    it('ShareConfig_SelfAccess_GetValues_Test102', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL2, async () => {
      console.log(TAG + "************* ShareConfig_SelfAccess_GetValues_Test102 start *************");
      try {
        let notExistUri = "datashareproxy://com.example.myapplication/test/gv/notexist102";
        await g_handle!.getValues(notExistUri, g_config_4K);
        expect().assertFail();
      } catch (err) {
        let businessErr = err as BusinessError;
        expect(businessErr.code).assertEqual(15700011);
      }
      console.log(TAG + "************* ShareConfig_SelfAccess_GetValues_Test102 end *************");
    });

    /**
     * @tc.name: ShareConfig_SelfAccess_GetValues_Test103
     * @tc.desc: Verify getValues fails with 15700014 when URI length exceeds 256 bytes (NAPI layer)
     * @tc.type: FUNC
     * @tc.precon: DataProxyHandle is created
     * @tc.step:
     *   1. Call getValues with URI longer than 256 bytes
     *   2. Verify BusinessError 15700014 is thrown (NAPI layer interception)
     * @tc.expect:
     *   1. getValues throws BusinessError with code 15700014
     */
    it('ShareConfig_SelfAccess_GetValues_Test103', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL2, async () => {
      console.log(TAG + "************* ShareConfig_SelfAccess_GetValues_Test103 start *************");
      try {
        let overLengthUri = "datashareproxy://com.example.myapplication/" + 'X'.repeat(256);
        await g_handle!.getValues(overLengthUri, g_config_4K);
        expect().assertFail();
      } catch (err) {
        let businessErr = err as BusinessError;
        expect(businessErr.code).assertEqual('15700014');
      }
      console.log(TAG + "************* ShareConfig_SelfAccess_GetValues_Test103 end *************");
    });

    // ===== combined scenarios (Flow) =====

    /**
     * @tc.name: ShareConfig_SelfAccess_Flow_Test001
     * @tc.desc: Verify full lifecycle: publish → getValues → removeValue → getValues
     * @tc.type: FUNC
     * @tc.precon: DataProxyHandle is created, publisher publishes with values={ 1: "v1", 2: "v2" }
     * @tc.step:
     *   1. Publish multi-value config with values={ 1: "v1", 2: "v2" }
     *   2. getValues → verify ["v1", "v2"]
     *   3. removeValue(uri, 1)
     *   4. getValues → verify ["v2"]
     * @tc.expect:
     *   1. Step 2 returns ["v1", "v2"]
     *   2. Step 4 returns ["v2"]
     */
    it('ShareConfig_SelfAccess_Flow_Test001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      console.log(TAG + "************* ShareConfig_SelfAccess_Flow_Test001 start *************");
      let testUri = "datashareproxy://com.example.myapplication/test/gv/flow001";
      try {
        let proxyData: dataShare.ProxyData[] = [{
          uri: testUri,
          values: { 1: "v1", 2: "v2" },
          isMultiValues: true,
          allowList: [],
          trustProviders: []
        }];
        await g_handle!.publish(proxyData, g_config_4K);

        let values1: ValueType[] = await g_handle!.getValues(testUri, g_config_4K);
        expect(values1.length).assertEqual(2);
        let hasV1 = false;
        let hasV2 = false;
        for (let i = 0; i < values1.length; i++) {
          if (values1[i] === "v1") { hasV1 = true; }
          if (values1[i] === "v2") { hasV2 = true; }
        }
        expect(hasV1).assertTrue();
        expect(hasV2).assertTrue();

        await g_handle!.removeValue(testUri, 1, g_config_4K);

        let values2: ValueType[] = await g_handle!.getValues(testUri, g_config_4K);
        expect(values2.length).assertEqual(1);
        expect(values2[0]).assertEqual("v2");
      } catch (err) {
        console.error(TAG + `ShareConfig_SelfAccess_Flow_Test001 failed: ${JSON.stringify(err)}`);
        expect().assertFail();
      }
      await g_handle!.delete([testUri], g_config_4K);
      console.log(TAG + "************* ShareConfig_SelfAccess_Flow_Test001 end *************");
    });

    /**
     * @tc.name: ShareConfig_SelfAccess_Flow_Test002
     * @tc.desc: Verify putValue update → getValues reflects value change
     * @tc.type: FUNC
     * @tc.precon: DataProxyHandle is created, publisher publishes with values={ 1: "old" }
     * @tc.step:
     *   1. Publish multi-value config with values={ 1: "old" }
     *   2. getValues → verify ["old"]
     *   3. putValue(uri, 1, "new") → getValues → verify ["new"]
     * @tc.expect:
     *   1. Step 2 returns ["old"]
     *   2. Step 3 returns ["new"]
     */
    it('ShareConfig_SelfAccess_Flow_Test002', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      console.log(TAG + "************* ShareConfig_SelfAccess_Flow_Test002 start *************");
      let testUri = "datashareproxy://com.example.myapplication/test/gv/flow002";
      try {
        let proxyData: dataShare.ProxyData[] = [{
          uri: testUri,
          values: { 1: "old" },
          isMultiValues: true,
          allowList: [],
          trustProviders: []
        }];
        await g_handle!.publish(proxyData, g_config_4K);
        let values1: ValueType[] = await g_handle!.getValues(testUri, g_config_4K);
        expect(values1.length).assertEqual(1);
        expect(values1[0]).assertEqual("old");

        await g_handle!.putValue(testUri, 1, "new", g_config_4K);
        let values2: ValueType[] = await g_handle!.getValues(testUri, g_config_4K);
        expect(values2.length).assertEqual(1);
        expect(values2[0]).assertEqual("new");
      } catch (err) {
        console.error(TAG + `ShareConfig_SelfAccess_Flow_Test002 failed: ${JSON.stringify(err)}`);
        expect().assertFail();
      }
      await g_handle!.delete([testUri], g_config_4K);
      console.log(TAG + "************* ShareConfig_SelfAccess_Flow_Test002 end *************");
    });

});
}
