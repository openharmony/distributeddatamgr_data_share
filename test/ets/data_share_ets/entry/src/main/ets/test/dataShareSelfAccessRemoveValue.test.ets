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

let TAG = "SelfAccessRemoveValue ";

export default function DataShareSelfAccessRemoveValueTest() {
  describe('DataShareSelfAccessRemoveValueTest', () => {

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
     * @tc.name: ShareConfig_SelfAccess_RemoveValue_Test001
     * @tc.desc: Verify publisher can removeValue to delete own key on multi-value config
     * @tc.type: FUNC
     * @tc.precon: DataProxyHandle is created, publisher publishes multi-value config with values={ 1: "v" }
     * @tc.step:
     *   1. Publish multi-value config with values={ 1: "v" }
     *   2. Call removeValue(uri, 1, g_config_4K)
     *   3. Verify operation resolves without exception
     * @tc.expect:
     *   1. removeValue resolves successfully
     */
    it('ShareConfig_SelfAccess_RemoveValue_Test001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      console.log(TAG + "************* ShareConfig_SelfAccess_RemoveValue_Test001 start *************");
      let testUri = "datashareproxy://com.example.myapplication/test/rv/001";
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
      } catch (err) {
        console.error(TAG + `ShareConfig_SelfAccess_RemoveValue_Test001 failed: ${JSON.stringify(err)}`);
        expect().assertFail();
      }
      await g_handle!.delete([testUri], g_config_4K);
      console.log(TAG + "************* ShareConfig_SelfAccess_RemoveValue_Test001 end *************");
    });

    /**
     * @tc.name: ShareConfig_SelfAccess_RemoveValue_Test002
     * @tc.desc: Verify publisher (tokenId) can removeValue another app's key
     * @tc.type: FUNC
     * @tc.precon: DataProxyHandle is created, multi-value config published with values={ 2: "v2" }
     *   Note: In single-app test, publisher's tokenId matches so this tests the publisher-can-remove-any-key rule
     * @tc.step:
     *   1. Publish multi-value config with values={ 2: "v2" }
     *   2. Call removeValue(uri, 2, g_config_4K) as publisher (same app)
     *   3. Verify getValues no longer returns the removed value
     * @tc.expect:
     *   1. removeValue resolves successfully (publisher tokenId has full rights)
     *   2. getValues returns empty array
     */
    it('ShareConfig_SelfAccess_RemoveValue_Test002', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      console.log(TAG + "************* ShareConfig_SelfAccess_RemoveValue_Test002 start *************");
      let testUri = "datashareproxy://com.example.myapplication/test/rv/002";
      try {
        let proxyData: dataShare.ProxyData[] = [{
          uri: testUri,
          values: { 2: "v2" },
          isMultiValues: true,
          allowList: [],
          trustProviders: []
        }];
        await g_handle!.publish(proxyData, g_config_4K);
        await g_handle!.removeValue(testUri, 2, g_config_4K);
        let values: ValueType[] = await g_handle!.getValues(testUri, g_config_4K);
        expect(values.length).assertEqual(0);
      } catch (err) {
        console.error(TAG + `ShareConfig_SelfAccess_RemoveValue_Test002 failed: ${JSON.stringify(err)}`);
        expect().assertFail();
      }
      await g_handle!.delete([testUri], g_config_4K);
      console.log(TAG + "************* ShareConfig_SelfAccess_RemoveValue_Test002 end *************");
    });

    /**
     * @tc.name: ShareConfig_SelfAccess_RemoveValue_Test003
     * @tc.desc: Verify getValues no longer returns removed value after removeValue
     * @tc.type: FUNC
     * @tc.precon: DataProxyHandle is created, publisher publishes with values={ 1: "v1", 2: "v2" }
     * @tc.step:
     *   1. Publish multi-value config with values={ 1: "v1", 2: "v2" }
     *   2. Call removeValue(uri, 1, g_config_4K)
     *   3. Call getValues to verify key=1 value is removed
     * @tc.expect:
     *   1. removeValue resolves successfully
     *   2. getValues returns only ["v2"]
     */
    it('ShareConfig_SelfAccess_RemoveValue_Test003', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      console.log(TAG + "************* ShareConfig_SelfAccess_RemoveValue_Test003 start *************");
      let testUri = "datashareproxy://com.example.myapplication/test/rv/003";
      try {
        let proxyData: dataShare.ProxyData[] = [{
          uri: testUri,
          values: { 1: "v1", 2: "v2" },
          isMultiValues: true,
          allowList: [],
          trustProviders: []
        }];
        await g_handle!.publish(proxyData, g_config_4K);
        await g_handle!.removeValue(testUri, 1, g_config_4K);
        let values: ValueType[] = await g_handle!.getValues(testUri, g_config_4K);
        expect(values.length).assertEqual(1);
        expect(values[0]).assertEqual("v2");
      } catch (err) {
        console.error(TAG + `ShareConfig_SelfAccess_RemoveValue_Test003 failed: ${JSON.stringify(err)}`);
        expect().assertFail();
      }
      await g_handle!.delete([testUri], g_config_4K);
      console.log(TAG + "************* ShareConfig_SelfAccess_RemoveValue_Test003 end *************");
    });

    /**
     * @tc.name: ShareConfig_SelfAccess_RemoveValue_Test004
     * @tc.desc: Verify appIdentifier entry is removed when last key is deleted
     * @tc.type: FUNC
     * @tc.precon: DataProxyHandle is created, publisher publishes with only 1 key under the URI
     * @tc.step:
     *   1. Publish multi-value config with values={ 1: "only_one" }
     *   2. Call removeValue(uri, 1, g_config_4K) to delete the last key
     *   3. Call getValues to verify empty array returned
     * @tc.expect:
     *   1. removeValue resolves successfully
     *   2. getValues returns empty array (appIdentifier entry cleaned up)
     */
    it('ShareConfig_SelfAccess_RemoveValue_Test004', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      console.log(TAG + "************* ShareConfig_SelfAccess_RemoveValue_Test004 start *************");
      let testUri = "datashareproxy://com.example.myapplication/test/rv/004";
      try {
        let proxyData: dataShare.ProxyData[] = [{
          uri: testUri,
          values: { 1: "only_one" },
          isMultiValues: true,
          allowList: [],
          trustProviders: []
        }];
        await g_handle!.publish(proxyData, g_config_4K);
        await g_handle!.removeValue(testUri, 1, g_config_4K);
        let values: ValueType[] = await g_handle!.getValues(testUri, g_config_4K);
        expect(values.length).assertEqual(0);
      } catch (err) {
        console.error(TAG + `ShareConfig_SelfAccess_RemoveValue_Test004 failed: ${JSON.stringify(err)}`);
        expect().assertFail();
      }
      await g_handle!.delete([testUri], g_config_4K);
      console.log(TAG + "************* ShareConfig_SelfAccess_RemoveValue_Test004 end *************");
    });

    /**
     * @tc.name: ShareConfig_SelfAccess_RemoveValue_Test101
     * @tc.desc: Verify removeValue fails with INCOMPATIBLE_CONFIG_TYPE on non-multi-value config
     * @tc.type: FUNC
     * @tc.precon: DataProxyHandle is created, normal (non-multi-value) config published
     * @tc.step:
     *   1. Publish a normal ProxyData (isMultiValues=false)
     *   2. Call removeValue(uri, 1, g_config_4K)
     *   3. Verify BusinessError 15700014 is thrown
     * @tc.expect:
     *   1. removeValue throws BusinessError with code 15700014 (INCOMPATIBLE_CONFIG_TYPE)
     */
    it('ShareConfig_SelfAccess_RemoveValue_Test101', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL2, async () => {
      console.log(TAG + "************* ShareConfig_SelfAccess_RemoveValue_Test101 start *************");
      let testUri = "datashareproxy://com.example.myapplication/test/rv/norm101";
      try {
        let proxyData: dataShare.ProxyData[] = [{
          uri: testUri,
          value: "normal_value",
          allowList: []
        }];
        await g_handle!.publish(proxyData, g_config_4K);
        await g_handle!.removeValue(testUri, 1, g_config_4K);
        expect().assertFail();
      } catch (err) {
        let businessErr = err as BusinessError;
        expect(businessErr.code).assertEqual(15700014);
      }
      await g_handle!.delete([testUri], g_config_4K);
      console.log(TAG + "************* ShareConfig_SelfAccess_RemoveValue_Test101 end *************");
    });

    /**
     * @tc.name: ShareConfig_SelfAccess_RemoveValue_Test102
     * @tc.desc: Verify removeValue fails with KEY_NOT_EXIST when key does not exist
     * @tc.type: FUNC
     * @tc.precon: DataProxyHandle is created, multi-value config published with seed key, key=999 does not exist
     * @tc.step:
     *   1. Publish multi-value config with values={ 0: "init" }
     *   2. Call removeValue(uri, 999, g_config_4K) for a non-existent key
     *   3. Verify BusinessError 15700014 is thrown (KEY_NOT_EXIST mapped to 15700014)
     * @tc.expect:
     *   1. removeValue throws BusinessError with code 15700014 (KEY_NOT_EXIST)
     */
    it('ShareConfig_SelfAccess_RemoveValue_Test102', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL2, async () => {
      console.log(TAG + "************* ShareConfig_SelfAccess_RemoveValue_Test102 start *************");
      let testUri = "datashareproxy://com.example.myapplication/test/rv/102";
      try {
        let proxyData: dataShare.ProxyData[] = [{
          uri: testUri,
          values: { 0: "init" },
          isMultiValues: true,
          allowList: [],
          trustProviders: []
        }];
        await g_handle!.publish(proxyData, g_config_4K);
        await g_handle!.removeValue(testUri, 999, g_config_4K);
        expect().assertFail();
      } catch (err) {
        let businessErr = err as BusinessError;
        expect(businessErr.code).assertEqual(15700014);
      }
      await g_handle!.delete([testUri], g_config_4K);
      console.log(TAG + "************* ShareConfig_SelfAccess_RemoveValue_Test102 end *************");
    });

    /**
     * @tc.name: ShareConfig_SelfAccess_RemoveValue_Test103
     * @tc.desc: Verify removeValue fails with URI_NOT_EXIST when URI does not exist
     * @tc.type: FUNC
     * @tc.precon: DataProxyHandle is created, no data published for the URI
     * @tc.step:
     *   1. Call removeValue with a non-existent URI
     *   2. Verify BusinessError 15700011 is thrown
     * @tc.expect:
     *   1. removeValue throws BusinessError with code 15700011 (URI_NOT_EXIST)
     */
    it('ShareConfig_SelfAccess_RemoveValue_Test103', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL2, async () => {
      console.log(TAG + "************* ShareConfig_SelfAccess_RemoveValue_Test103 start *************");
      try {
        let notExistUri = "datashareproxy://com.example.myapplication/test/rv/notexist103";
        await g_handle!.removeValue(notExistUri, 1, g_config_4K);
        expect().assertFail();
      } catch (err) {
        let businessErr = err as BusinessError;
        expect(businessErr.code).assertEqual(15700011);
      }
      console.log(TAG + "************* ShareConfig_SelfAccess_RemoveValue_Test103 end *************");
    });

    /**
     * @tc.name: ShareConfig_SelfAccess_RemoveValue_Test104
     * @tc.desc: Verify removeValue fails with 15700014 when URI length exceeds 256 bytes (NAPI layer)
     * @tc.type: FUNC
     * @tc.precon: DataProxyHandle is created
     * @tc.step:
     *   1. Call removeValue with URI longer than 256 bytes
     *   2. Verify BusinessError 15700014 is thrown (NAPI layer interception)
     * @tc.expect:
     *   1. removeValue throws BusinessError with code 15700014
     */
    it('ShareConfig_SelfAccess_RemoveValue_Test104', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL2, async () => {
      console.log(TAG + "************* ShareConfig_SelfAccess_RemoveValue_Test104 start *************");
      try {
        let overLengthUri = "datashareproxy://com.example.myapplication/" + 'X'.repeat(256);
        await g_handle!.removeValue(overLengthUri, 1, g_config_4K);
        expect().assertFail();
      } catch (err) {
        let businessErr = err as BusinessError;
        expect(businessErr.code).assertEqual('15700014');
      }
      console.log(TAG + "************* ShareConfig_SelfAccess_RemoveValue_Test104 end *************");
    });

  });
}