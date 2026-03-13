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
 
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect, Level, Size, TestType } from "../../../hypium/index";
import dataShare from '@ohos.data.dataShare'
import { BusinessError } from '@ohos.base';
import Utils from './Util.test'
 
let g_handle : dataShare.DataProxyHandle;
let g_config : dataShare.DataProxyConfig = {
  type : dataShare.DataProxyType.SHARED_CONFIG
}
let testUri1 = "datashareproxy://ohos.acts.datashareconfigtest.static/test1";
let testUri2 = "datashareproxy://ohos.acts.datashareconfigtest.static/test1";
let TAG = "[ttt]";
export default function dataShareConfigMtbfTest() {
  describe("dataShareConfigMtbfTest", (): void => {
    beforeAll(async () => {
      console.info(TAG + "beforeAll -----------------------------------");
      await dataShare.createDataProxyHandle().then((handle: dataShare.DataProxyHandle) => {
        if (handle == null || handle == undefined) {
          console.log(TAG + "[DataShareProvider] createDataProxyHandle failed, handle is null");
        } else {
          g_handle = handle;
          console.log(TAG + "[DataShareProvider] createDataProxyHandle success");
        }
      }).catch((err : Error) => {
        console.error(TAG + `[DataShareProvider] createDataProxyHandle failed: code: ${err.code}, message: ${err.message} `);
      })
    })
    beforeEach(async () => {
      console.info(TAG + "beforeEach -----------------------------------");
      try {
        let proxyData : dataShare.ProxyData[] = [];
        let g_proxyDataAllowTest1 : dataShare.ProxyData = {
          uri : testUri1,
          value : "value is ok beforeEach",
          allowList : ["all"]
        }
        proxyData.push(g_proxyDataAllowTest1);
        await g_handle!.publish(proxyData, g_config);
      } catch(err: BusinessError) {
        console.error(TAG + "[DataShareProvider] beforeEach publish failed, code =" + err.code + ", message =" + err.message);
        expect().assertFail();
      }
    })
    afterEach(async () => {
      console.info(TAG + "afterEach -----------------------------------");
      let uris = [
        testUri1,testUri2
      ]
      try{
        await g_handle!.delete(uris, g_config).then((results : dataShare.DataProxyResult[]) => {
          console.log(TAG + "[DataShareProvider] afterEach delete success, results: " + JSON.stringify(results));
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] delete failed. code =" + err.code + ", message =" + err.message);
          expect().assertFail();
        })
      }catch (err: BusinessError) {
        console.error(TAG + "[DataShareProvider] afterEach delete failed. code =" + err.code + ", message =" + err.message);
        expect().assertFail();
      }
    })
    afterAll(async () => {
      console.info(TAG + "afterAll -----------------------------------");
    })
 
    /**
     * @tc.name dataShareConfigMtbfTest001
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_0520
     * @tc.desc get configuration 2000 times
     * @tc.size MediumTest
     * @tc.type RELIABILITY
     * @tc.level Level 3
     */
    it('dataShareConfigMtbfTest001', TestType.RELIABILITY | Size.MEDIUMTEST | Level.LEVEL3, async (done: () => void): Promise<void> => {
      for(let i=0;i<2000;i++){
        let uris:string[] = [];
        uris.push(testUri1);
        try{
          await g_handle!.get(uris, g_config).then((results : dataShare.DataProxyGetResult[]) => {
            results.forEach((result) => {
              console.info(TAG + `[DataShareProvider] URI: ${result.uri}, Value: ${result.value}, Result: ${result.result}, AllowList: ${result.allowList}`);
              expect(result.result).assertEqual(dataShare.DataProxyErrorCode.SUCCESS);
              expect(result.value).assertEqual("value is ok beforeEach");
              expect(result.allowList?.toString()).assertEqual("all");
            })
          }).catch((err : Error) => {
            console.log(TAG + "[DataShareProvider] get failed, err : " + JSON.stringify(err));
            expect().assertFail();
          })
        }catch (err: BusinessError) {
          console.error(TAG + "[DataShareProvider] dataShareConfigMtbfTest001 failed. code =" + err.code + ", message =" + err.message);
          expect().assertFail();
        }
      }
      await Utils.msSleep(10000);
      done();
    });
 
    /**
     * @tc.name dataShareConfigMtbfTest002
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_0520
     * @tc.desc publish configuration 2000 times
     * @tc.size MediumTest
     * @tc.type RELIABILITY
     * @tc.level Level 3
     */
    it('dataShareConfigMtbfTest002', TestType.RELIABILITY | Size.MEDIUMTEST | Level.LEVEL3, async (done: () => void): Promise<void> => {
      for(let i=0;i<2000;i++){
        let testUri1 = "datashareproxy://ohos.acts.datashareconfigtest.static/test2";
        try {
          let proxyData : dataShare.ProxyData[] = [];
          let g_proxyDataAllowTest1 : dataShare.ProxyData = {
            uri : testUri1,
            value : "value is ok"+i,
            allowList : []
          }
          proxyData.push(g_proxyDataAllowTest1);
          await g_handle!.publish(proxyData, g_config).then((results: dataShare.DataProxyResult[]) => {
            results.forEach((result) => {
              console.log(TAG + "[DataShareProvider] dataShareConfigMtbfTest002 success, results: " + JSON.stringify(results));
              expect(result.result).assertEqual(dataShare.DataProxyErrorCode.SUCCESS);
              expect(result.uri).assertEqual(testUri1);
            })
          }).catch((err: Error) => {
            console.error(TAG + "[DataShareProvider] publish failed. code =" + err.code + ", message =" + err.message);
            expect().assertFail();
          })
        } catch(err: BusinessError) {
          console.error(TAG + "[DataShareProvider] dataShareConfigMtbfTest002 failed, code =" + err.code + ", message =" + err.message);
          expect().assertFail();
        }
      }
      await Utils.msSleep(10000);
      done();
    });
 
    /**
     * @tc.name dataShareConfigMtbfTest002
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_0520
     * @tc.desc publish and delete configuration 2000 times
     * @tc.size MediumTest
     * @tc.type RELIABILITY
     * @tc.level Level 3
     */
    it('dataShareConfigMtbfTest003', TestType.RELIABILITY | Size.MEDIUMTEST | Level.LEVEL3, async (done: () => void): Promise<void> => {
      for(let i=0;i<2000;i++){
        let testUri = "datashareproxy://ohos.acts.datashareconfigtest.static/test"+i;
        try {
          let proxyData : dataShare.ProxyData[] = [];
          let g_proxyDataAllowTest1 : dataShare.ProxyData = {
            uri : testUri,
            value : "value is ok"+i,
            allowList : []
          }
          proxyData.push(g_proxyDataAllowTest1);
          await g_handle!.publish(proxyData, g_config).then((results: dataShare.DataProxyResult[]) => {
            console.log(TAG + "[DataShareProvider] dataShareConfigMtbfTest003 publish success, results: " + JSON.stringify(results));
            results.forEach((result) => {
              expect(result.result).assertEqual(dataShare.DataProxyErrorCode.SUCCESS);
              expect(result.uri).assertEqual(testUri);
            })
          }).catch((err: Error) => {
            console.error(TAG + "[DataShareProvider] publish failed. code =" + err.code + ", message =" + err.message);
            expect().assertFail();
          })
          await g_handle!.delete([testUri], g_config).then((results : dataShare.DataProxyResult[]) => {
            console.log(TAG + "[DataShareProvider] dataShareConfigMtbfTest003 delete success, results: " + JSON.stringify(results));
            results.forEach((result) => {
              expect(result.result).assertEqual(dataShare.DataProxyErrorCode.SUCCESS);
              expect(result.uri).assertEqual(testUri);
            })
          }).catch((err: Error) => {
            console.error(TAG + "[DataShareProvider] dataShareConfigMtbfTest003 delete failed. code =" + err.code + ", message =" + err.message);
            expect().assertFail();
          })
        } catch(err: BusinessError) {
          console.error(TAG + "[DataShareProvider] dataShareConfigMtbfTest003 failed, code =" + err.code + ", message =" + err.message);
          expect().assertFail();
        }
      }
      done();
    });
  })
}