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

import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect, Level, Size, TestType } from '@ohos/hypium';
import dataShare from '@ohos.data.dataShare'
import { BusinessError } from "@ohos.base";

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

let staticUri = "datashareproxy://com.example.myapplication/static_config";
let uri4K = "datashareproxy://com.example.myapplication/uri_4K";
let uri100K = "datashareproxy://com.example.myapplication/uri_100K";

let g_proxyData_4K: dataShare.ProxyData = {
  uri: uri4K,
  value : '1'.repeat(1024 * 4),
  allowList: []
}

let g_proxyData_100K: dataShare.ProxyData = {
  uri: uri100K,
  value : '1'.repeat(1024 * 4 * 25),
  allowList: []
}
let TAG = "ttt ";

export default function DataShareConfigEtsTest() {
  describe('DataShareConfigEtsTest', () => {

    beforeAll(async () => {
      await dataShare.createDataProxyHandle().then((handle: dataShare.DataProxyHandle) => {
        if (handle == null || handle == undefined) {
          console.log(TAG + "createDataProxyHandle failed,handle is null");
          return;
        }
        g_handle = handle;
        console.log(TAG + "createDataProxyHandle success");
      }).catch((err: BusinessError) => {
        console.error(TAG + `createDataProxyHandle failed: code:${err.code},message:${err.message}`);
      })
    });

    beforeEach(async () => {
      console.info(TAG + 'beforeEach');
    });

    afterEach(async () => {
      // delete all data
      await g_handle!.delete(g_config_default);
    });

    afterAll(async () => {
      console.info(TAG + 'afterAll');
    });

    /**
     * @tc.name: ShareConfig_Static100K_Get_Test001
     * @tc.desc: Verify get operation with MAX_LENGTH_100K config after publishing data
     * @tc.type: FUNC
     * @tc.precon: DataProxyHandle is created
     * @tc.step:
     *   1. Publish g_proxyData_4K and g_proxyData_100K with g_config_100K
     *   2. Create URI array containing staticUri, uri4K, uri100K
     *   3. Call get() with g_config_100K (maxValueLength: 100K)
     *   4. Verify results for uri4K and uri100K have SUCCESS code and non-empty value
     * @tc.expect:
     *   1. Results array length equals 3
     *   2. uri4K and uri100K results have SUCCESS code
     *   3. Each result.value has non-zero length
     */
    it('ShareConfig_Static100K_Get_Test001', 0, async () => {
      console.log(TAG + "************* ShareConfig_Static100K_Get_Test001 start *************");
      let uris = [
        staticUri,
        uri4K,
        uri100K,
      ]
      try {
        let proxyData: dataShare.ProxyData[] = [];
        proxyData.push(g_proxyData_4K);
        proxyData.push(g_proxyData_100K);
        await g_handle!.publish(proxyData, g_config_100K);

        await g_handle!.get(uris, g_config_100K).then((results: dataShare.DataProxyGetResult[]) => {
          expect(results.length).assertEqual(3);
          results.forEach((result) => {
            console.info(TAG + `URI:${result.uri},Value:${result.value},Result:${result.result},AllowList:${result.allowList}`);
            expect(result.result).assertEqual(dataShare.DataProxyErrorCode.SUCCESS);
            expect(JSON.stringify(result.value).length > 0).assertTrue();
          });
        }).catch((err: BusinessError) => {
          console.error(TAG + "get failed.err =" + JSON.stringify(err));
          expect().assertFail();
        })
      } catch (err) {
        console.error(TAG + "ShareConfig_Static100K_Get_Test001 failed,code =" + err.code + ",message=" + err.message);
        expect().assertFail();
      }
      console.log(TAG + "************* ShareConfig_Static100K_Get_Test001 end *************");
    });

    /**
     * @tc.name: ShareConfig_Static100K_Get_Test002
     * @tc.desc: Verify get operation with default config returns URI_NOT_EXIST when no matching data exists
     * @tc.type: FUNC
     * @tc.precon: DataProxyHandle is created, no data exists with 4K maxValueLength config
     * @tc.step:
     *   1. Create URI array containing staticUri, uri4K, uri100K
     *   2. Call get() with g_config_default (default maxValueLength: 4K)
     *   3. Verify each result has URI_NOT_EXIST code
     * @tc.expect:
     *   1. Results array length equals 3
     *   2. Each result.result equals DataProxyErrorCode.URI_NOT_EXIST
     *   3. Each result.value and result.allowList are undefined
     */
    it('ShareConfig_Static100K_Get_Test002', 0, async () => {
      console.log(TAG + "************* ShareConfig_Static100K_Get_Test002 start *************");
      let uris = [
        staticUri,
        uri4K,
        uri100K,
      ]
      try {
        await g_handle!.get(uris, g_config_default).then((results: dataShare.DataProxyGetResult[]) => {
          expect(results.length).assertEqual(3);
          results.forEach((result) => {
            console.info(TAG + `URI:${result.uri},Value:${result.value},Result:${result.result},AllowList:${result.allowList}`);
            expect(result.result).assertEqual(dataShare.DataProxyErrorCode.URI_NOT_EXIST);
            expect(result.value).assertEqual(undefined);
            expect(result.allowList).assertEqual(undefined);
          });
        }).catch((err: BusinessError) => {
          console.error(TAG + "get failed.err =" + JSON.stringify(err));
          expect().assertFail();
        })
      } catch (err) {
        console.error(TAG + "ShareConfig_Static100K_Get_Test002 failed,code =" + err.code + ",message=" + err.message);
        expect().assertFail();
      }
      console.log(TAG + "************* ShareConfig_Static100K_Get_Test002 end *************");
    });

    /**
     * @tc.name: ShareConfig_Static100K_Get_Test003
     * @tc.desc: Verify get operation with 4K config returns URI_NOT_EXIST when no matching data exists
     * @tc.type: FUNC
     * @tc.precon: DataProxyHandle is created, no data exists with 4K maxValueLength config
     * @tc.step:
     *   1. Create URI array containing staticUri, uri4K, uri100K
     *   2. Call get() with g_config_4K (maxValueLength: 4K)
     *   3. Verify each result has URI_NOT_EXIST code
     * @tc.expect:
     *   1. Results array length equals 3
     *   2. Each result.result equals DataProxyErrorCode.URI_NOT_EXIST
     *   3. Each result.value and result.allowList are undefined
     */
    it('ShareConfig_Static100K_Get_Test003', 0, async () => {
      console.log(TAG + "************* ShareConfig_Static100K_Get_Test003 start *************");
      let uris = [
        staticUri,
        uri4K,
        uri100K,
      ]
      try {
        await g_handle!.get(uris, g_config_4K).then((results: dataShare.DataProxyGetResult[]) => {
          expect(results.length).assertEqual(3);
          results.forEach((result) => {
            console.info(TAG + `URI:${result.uri},Value:${result.value},Result:${result.result},AllowList:${result.allowList}`);
            expect(result.result).assertEqual(dataShare.DataProxyErrorCode.URI_NOT_EXIST);
            expect(result.value).assertEqual(undefined);
            expect(result.allowList).assertEqual(undefined);
          });
        }).catch((err: BusinessError) => {
          console.error(TAG + "get failed.err =" + JSON.stringify(err));
          expect().assertFail();
        })
      } catch (err) {
        console.error(TAG + "ShareConfig_Static100K_Get_Test003 failed,code =" + err.code + ",message=" + err.message);
        expect().assertFail();
      }
      console.log(TAG + "************* ShareConfig_Static100K_Get_Test003 end *************");
    });

    /**
     * @tc.name: ShareConfig_Normal100K_Publish_Test001
     * @tc.desc: Verify publish operation fails when maxValueLength config mismatch with data size
     * @tc.type: FUNC
     * @tc.precon: DataProxyHandle is created, g_proxyData_4K (4K string) and g_proxyData_100K (100K string) prepared
     * @tc.step:
     *   1. Create ProxyData array with 4K and 100K sized values
     *   2. Call publish() with g_config_default (4K maxValueLength)
     *   3. Verify operation throws exception with code "15700014"
     * @tc.expect:
     *   1. Publish operation fails with exception
     *   2. Exception code equals "15700014" (OVER_LIMIT)
     */
    it('ShareConfig_Normal100K_Publish_Test001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      try {
        let proxyData: dataShare.ProxyData[] = [];
        proxyData.push(g_proxyData_4K);
        proxyData.push(g_proxyData_100K);
        await g_handle!.publish(proxyData, g_config_default).then((results: dataShare.DataProxyResult[]) => {
          console.log(TAG + "ShareConfig_Normal100K_Publish_Test001 success,results:" + JSON.stringify(results));
          expect().assertFail();
        }).catch((err: BusinessError) => {
          console.error(TAG + "publish failed.code =" + err.code + ",message=" + err.message);
          expect().assertFail();
        })
      } catch (err) {
        console.error(TAG + "ShareConfig_Normal100K_Publish_Test001 failed.code =" + err.code + ",message=" + err.message);
        expect(err.code).assertEqual("15700014");
      }
    });

    /**
     * @tc.name: ShareConfig_Normal100K_Publish_Test002
     * @tc.desc: Verify publish operation fails when 100K data exceeds 4K maxValueLength config
     * @tc.type: FUNC
     * @tc.precon: DataProxyHandle is created, g_proxyData_4K (4K string) and g_proxyData_100K (100K string) prepared
     * @tc.step:
     *   1. Create ProxyData array with 4K and 100K sized values
     *   2. Call publish() with g_config_4K (maxValueLength: 4K)
     *   3. Verify operation throws exception with code "15700014"
     * @tc.expect:
     *   1. Publish operation fails with exception
     *   2. Exception code equals "15700014" (OVER_LIMIT)
     */
    it('ShareConfig_Normal100K_Publish_Test002', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      try {
        let proxyData: dataShare.ProxyData[] = [];
        proxyData.push(g_proxyData_4K);
        proxyData.push(g_proxyData_100K);
        await g_handle!.publish(proxyData, g_config_4K).then((results: dataShare.DataProxyResult[]) => {
          console.log(TAG + "ShareConfig_Normal100K_Publish_Test002 success,results:" + JSON.stringify(results));
          expect().assertFail();
        }).catch((err: BusinessError) => {
          console.error(TAG + "publish failed.code =" + err.code + ",message=" + err.message);
          expect().assertFail();
        })
      } catch (err) {
        console.error(TAG + "ShareConfig_Normal100K_Publish_Test002 failed.code =" + err.code + ",message=" + err.message);
        expect(err.code).assertEqual("15700014");
      }
    });

    /**
     * @tc.name: ShareConfig_Normal100K_Publish_Test003
     * @tc.desc: Verify publish operation succeeds when 100K data matches 100K maxValueLength config
     * @tc.type: FUNC
     * @tc.precon: DataProxyHandle is created, g_proxyData_4K (4K string) and g_proxyData_100K (100K string) prepared
     * @tc.step:
     *   1. Create ProxyData array with 4K and 100K sized values
     *   2. Call publish() with g_config_100K (maxValueLength: 100K)
     *   3. Verify publish succeeds, results[1].uri equals uri100K
     *   4. Call get() to verify published data
     * @tc.expect:
     *   1. Publish succeeds, results[1].uri equals uri100K, results[1].result equals 0
     *   2. Get returns 2 results with SUCCESS code
     *   3. getResults[1].value length equals 102400
     */
    it('ShareConfig_Normal100K_Publish_Test003', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      try {
        let proxyData: dataShare.ProxyData[] = [];
        proxyData.push(g_proxyData_4K);
        proxyData.push(g_proxyData_100K);
        await g_handle!.publish(proxyData, g_config_100K).then((results: dataShare.DataProxyResult[]) => {
          console.log(TAG + "ShareConfig_Normal100K_Publish_Test003 success,results:" + JSON.stringify(results));
          let uri = results[1].uri;
          let rtnCode = results[1].result;
          expect(uri).assertEqual(uri100K);
          expect(rtnCode).assertEqual(0);
        }).catch((err: BusinessError) => {
          console.error(TAG + "publish failed.code =" + err.code + ",message=" + err.message);
          expect().assertFail();
        })
        let uris = [
          uri4K,
          uri100K,
        ]
        let getResults = await g_handle!.get(uris, g_config_100K);
        expect(getResults.length).assertEqual(2);
        expect(getResults[1].result).assertEqual(0);
        expect((getResults[1].value as string).length).assertEqual(102400);
      } catch (err) {
        console.error(TAG + "ShareConfig_Normal100K_Publish_Test003 failed.code =" + err.code + ",message=" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name: ShareConfig_Size64_Publish_Test001
     * @tc.desc: Verify publish operation respects maximum count limit of 64 items per batch
     * @tc.type: FUNC
     * @tc.precon: DataProxyHandle is created
     * @tc.step:
     *   1. Create ProxyData array with 64 items (within limit)
     *   2. Call publish() with g_config_default - expect success for all 64 items
     *   3. Add one more item to array (total 65)
     *   4. Call publish() with 65 items - expect failure with exception code "15700014"
     * @tc.expect:
     *   1. First publish succeeds with SUCCESS for all 64 items
     *   2. Second publish fails with exception code "15700014" (OVER_LIMIT)
     */
    it('ShareConfig_Size64_Publish_Test001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      try {
        let proxyData: dataShare.ProxyData[] = [];
        for (let i = 0; i < 64; i++) {
          let testUri = "datashareproxy://com.example.myapplication/test" + i;
          let g_proxyDataAllowTest: dataShare.ProxyData = {
            uri: testUri,
            value: "value is less" + i,
            allowList: []
          }
          proxyData.push(g_proxyDataAllowTest);
        }
        await g_handle!.publish(proxyData, g_config_default).then((results: dataShare.DataProxyResult[]) => {
          console.log(TAG + "ShareConfig_Size64_Publish_Test001 success,proxyData allowList length is:" + JSON.stringify(proxyData[0].allowList?.length));
          console.log(TAG + "ShareConfig_Size64_Publish_Test001 success,results:" + JSON.stringify(results));
          results.forEach((result) => {
            let rtnCode = result.result;
            expect(rtnCode).assertEqual(0);
          });
        }).catch((err: BusinessError) => {
          console.error(TAG + "publish failed.code =" + err.code + ",message=" + err.message);
          expect().assertFail();
        })
        
        let testUri1 = "datashareproxy://com.example.myapplication/test99";
        let g_proxyDataAllowTest1: dataShare.ProxyData = {
          uri: testUri1,
          value: "value is ok",
          allowList: []
        }
        proxyData.push(g_proxyDataAllowTest1);
        let results1 = await g_handle!.publish(proxyData, g_config_default);
        expect().assertFail();
      } catch (err) {
        console.error(TAG + "ShareConfig_Size64_Publish_Test001 failed.code =" + err.code + ",message=" + err.message);
        expect(err.code).assertEqual("15700014");
      }
    });

    /**
     * @tc.name: ShareConfig_Normal_DeleteAll_Test001
     * @tc.desc: Verify delete all operation removes all published data
     * @tc.type: FUNC
     * @tc.precon: DataProxyHandle is created, g_proxyData_4K and g_proxyData_100K prepared
     * @tc.step:
     *   1. Publish 4K and 100K data with g_config_100K
     *   2. Verify publish succeeds, results[1].uri equals uri100K
     *   3. Call delete(g_config_default) to delete all data
     *   4. Call get() to verify data is deleted
     * @tc.expect:
     *   1. Publish succeeds, results[1].uri equals uri100K, results[1].result equals SUCCESS
     *   2. Delete operation completes
     *   3. Get returns URI_NOT_EXIST for both uri4K and uri100K
     */
    it('ShareConfig_Normal_DeleteAll_Test001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      try {
        let proxyData: dataShare.ProxyData[] = [];
        proxyData.push(g_proxyData_4K);
        proxyData.push(g_proxyData_100K);
        await g_handle!.publish(proxyData, g_config_100K).then((results: dataShare.DataProxyResult[]) => {
          console.log(TAG + "ShareConfig_Normal_DeleteAll_Test001 success,results:" + JSON.stringify(results));
          let uri = results[1].uri;
          let rtnCode = results[1].result;
          expect(uri).assertEqual(uri100K);
          expect(rtnCode).assertEqual(dataShare.DataProxyErrorCode.SUCCESS);
        }).catch((err: BusinessError) => {
          console.error(TAG + "publish failed.code =" + err.code + ",message=" + err.message);
          expect().assertFail();
        })

        await g_handle!.delete(g_config_default).then((results : dataShare.DataProxyResult[]) => {
            results.forEach((result) => {
                console.info(TAG + ` URI: ${result.uri}, Result: ${result.result}`);
            });
        }).catch((error: BusinessError) => {
            console.error('Error deleting config:', error);
        })
        let uris = [
          uri4K,
          uri100K,
        ]
        let getResults = await g_handle!.get(uris, g_config_100K);
        expect(getResults.length).assertEqual(2);
        expect(getResults[0].result).assertEqual(dataShare.DataProxyErrorCode.URI_NOT_EXIST);
        expect(getResults[1].result).assertEqual(dataShare.DataProxyErrorCode.URI_NOT_EXIST);
      } catch (err) {
        console.error(TAG + "ShareConfig_Normal_DeleteAll_Test001 failed.code =" + err.code + ",message=" + err.message);
        expect().assertFail();
      }
    });
  });
}