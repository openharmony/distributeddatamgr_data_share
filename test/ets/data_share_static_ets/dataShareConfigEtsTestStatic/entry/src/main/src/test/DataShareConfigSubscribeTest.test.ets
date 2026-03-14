/*
* Copyright (c) 2025 Huawei Device Co., Ltd.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect, Level, Size, TestType } from '../../../hypium/index';
import dataShare from '@ohos.data.dataShare';
import { BusinessError } from '@ohos.base';

let g_handle: dataShare.DataProxyHandle | undefined;
let g_config: dataShare.DataProxyConfig = {
  type: dataShare.DataProxyType.SHARED_CONFIG
}
let testUri1 = "datashareproxy://com.acts.dataShare.config.static/test1";
let testUri2 = "datashareproxy://com.acts.dataShare.config.static/test2";
let TAG = "ttt ";

export default function dataShareConfigSubscribeTest() {
  describe("dataShareConfigSubscribeTest", () => {
    beforeAll(async () => {
      console.info(TAG + "beforeAll---------------------------------");
      await dataShare.createDataProxyHandle().then((handle: dataShare.DataProxyHandle) => {
        if (handle == null || handle == undefined) {
          console.log(TAG + "[DataShareProvider] createDataProxyHandle failed,handle is null");
        } else {
          g_handle = handle;
          console.log(TAG + "[DataShareProvider] createDataProxyHandle success");
        }
      }).catch((err: Error) => {
        console.error(TAG + `[DataShareProvider] createDataProxyHandle failed: code:${err.code},message:${err.message}`)
      })
    })

    beforeEach(async () => {
      console.info(TAG + "beforeEach---------------------------------");
      try {
        let proxyData: dataShare.ProxyData[] = [];
        let g_proxyDataAllowTest1: dataShare.ProxyData = {
          uri: testUri1,
          value: "value is ok beforeEach",
          allowList: ["all"]
        }
        proxyData.push(g_proxyDataAllowTest1);
        await g_handle!.publish(proxyData, g_config);
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] beforeEach publish failed, code=" + err.code + ",message=" +
        err.message);
        expect().assertFail();
      }
    })

    afterEach(async () => {
      console.info(TAG + "afterEach---------------------------------");
      let uris = [
        testUri1,
        testUri2
      ]
      try {
        await g_handle!.delete(uris, g_config).then((results: dataShare.DataProxyResult[]) => {
          console.log("[ttt] [DataShareProvider] afterEach delete success,results:" + JSON.stringify(results));
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] delete failed.code=" + err.code + ",message=" + err.message);
          expect().assertFail();
        })
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] afterEach delete failed.code =" + err.code + ",message=" +
        err.message);
        expect().assertFail();
      }
    })

    afterAll(async () => {
      console.info(TAG + "afterAll---------------------------------");
    })

    /**
     * @tc.name   dataShareConfigSubscribeTest001
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_0680
     * @tc.desc   single uri
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareConfigSubscribeTest001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      let testUri = "";
      let uris = [
        testUri
      ]
      try {
        let results = g_handle!.onDataChange(uris, g_config, (changeInfo: Array<dataShare.DataProxyChangeInfo>) => {
          console.log(TAG + "[DataShareProvider] onProxyData receive change:" + JSON.stringify(changeInfo));
          expect().assertFail();
        });
        expect(results.length > 0).assertTrue();
        console.log(TAG + '[DataShareProvider] dataShareConfigSubscribeTest001 success:' + JSON.stringify(results));
        results.forEach((result) => {
          console.info(TAG + `[DataShareProvider] URI: ${result.uri}, Result: ${result.result}`);
          expect(result.result).assertEqual(1);
        });
      } catch (err) {
        console.error(TAG + "[DataShareProvider] dataShareConfigSubscribeTest001 failed,err =" + JSON.stringify(err));
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareConfigSubscribeTest002
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_0690
     * @tc.desc   single uri
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareConfigSubscribeTest002', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      let tmpStr = "123456".repeat(33);
      let testUri = "datashareproxy://com.acts.dataShare.config.static/test1123" + tmpStr;
      let uris = [
        testUri
      ]
      try {
        let results = g_handle!.onDataChange(uris, g_config, (changeInfo: dataShare.DataProxyChangeInfo[]) => {
          console.log(TAG + "[DataShareProvider] onProxyData receive change:" + JSON.stringify(changeInfo));
          expect().assertFail();
        });
        expect(results.length > 0).assertTrue();
        console.log(TAG + '[DataShareProvider] dataShareConfigSubscribeTest002 success:' + JSON.stringify(results));
        results.forEach((result) => {
          console.info(TAG + `[DataShareProvider] URI: ${result.uri}, Result: ${result.result}`);
          expect(result.result).assertEqual(1);
        });
      } catch (err) {
        console.error(TAG + "[DataShareProvider] dataShareConfigSubscribeTest002 failed,err =" + JSON.stringify(err));
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareConfigSubscribeTest003
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_0700
     * @tc.desc   single uri
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareConfigSubscribeTest003', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      let tmpStr = "123456".repeat(34);
      let testUri = "datashareproxy://com.acts.dataShare.config.static/test11234" + tmpStr + "9";
      let uris = [
        testUri
      ]
      try {
        let results = g_handle!.onDataChange(uris, g_config, (changeInfo: dataShare.DataProxyChangeInfo[]) => {
          console.log(TAG + "[DataShareProvider] onProxyData receive change:" + JSON.stringify(changeInfo));
          expect().assertFail();
        });
        expect(results.length > 0).assertTrue();
        console.log(TAG + '[DataShareProvider] dataShareConfigSubscribeTest003 success:' + JSON.stringify(results));
        results.forEach((result) => {
          console.info(TAG + `[DataShareProvider] URI: ${result.uri}, Result: ${result.result}`);
          expect(result.result).assertEqual(1);
        });
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareConfigSubscribeTest003 failed.code =" + err.code +
          ",message=" + err.message);
        expect(err.code).assertEqual(15700014);
      }
    });

    /**
     * @tc.name   dataShareConfigSubscribeTest004
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_0710
     * @tc.desc   single uri
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareConfigSubscribeTest004', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      let uris: Array<string> = []
      try {
        let results = g_handle!.onDataChange(uris, g_config, (changeInfo: dataShare.DataProxyChangeInfo[]) => {
          console.log(TAG + "[DataShareProvider] onProxyData receive change:" + JSON.stringify(changeInfo));
          expect().assertFail();
        });
        console.log(TAG + '[DataShareProvider] dataShareConfigSubscribeTest004 success:' + JSON.stringify(results));
        results.forEach((result) => {
          console.info(TAG + `[DataShareProvider] URI: ${result.uri}, Result: ${result.result}`);
          expect(result.result).assertEqual(1);
        });
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareConfigSubscribeTest004 failed.code =" + err.code +
          ",message=" + err.message);
        expect(err.code).assertEqual(401);
      }
    });

    /**
     * @tc.name   dataShareConfigSubscribeTest005
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_0720
     * @tc.desc   single uri
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareConfigSubscribeTest005', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      let testUri = " ";
      let uris = [
        testUri
      ]
      try {
        let results = g_handle!.onDataChange(uris, g_config, (changeInfo: dataShare.DataProxyChangeInfo[]) => {
          console.log(TAG + "[DataShareProvider] onProxyData receive change:" + JSON.stringify(changeInfo));
          expect().assertFail();
        });
        console.log(TAG + '[DataShareProvider] dataShareConfigSubscribeTest005 success:' + JSON.stringify(results));
        results.forEach((result) => {
          console.info(TAG + `[DataShareProvider] URI: ${result.uri}, Result: ${result.result}`);
          expect(result.result).assertEqual(1);
        });
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareConfigSubscribeTest005 failed.code =" + err.code +
          ",message=" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareConfigSubscribeTest006
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_0730
     * @tc.desc   single uri
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareConfigSubscribeTest006', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      let testUri = "@#$%^&*";
      let uris = [
        testUri
      ]
      try {
        let results = g_handle!.onDataChange(uris, g_config, (changeInfo: dataShare.DataProxyChangeInfo[]) => {
          console.log(TAG + "[DataShareProvider] onProxyData receive change:" + JSON.stringify(changeInfo));
          expect().assertFail();
        });
        console.log(TAG + '[DataShareProvider] dataShareConfigSubscribeTest006 success:' + JSON.stringify(results));
        results.forEach((result) => {
          console.info(TAG + `[DataShareProvider] URI: ${result.uri}, Result: ${result.result}`);
          expect(result.result).assertEqual(1);
        });
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareConfigSubscribeTest006 failed.code =" + err.code +
          ",message=" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareConfigSubscribeTest009
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_0760
     * @tc.desc   single uri
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareConfigSubscribeTest009', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      let testUri = "datashareproxy://com.acts.dataShare.config.static/ ";
      let uris = [
        testUri
      ]
      try {
        let results = g_handle!.onDataChange(uris, g_config, (changeInfo: dataShare.DataProxyChangeInfo[]) => {
          console.log(TAG + "[DataShareProvider] onProxyData receive change:" + JSON.stringify(changeInfo));
          expect().assertFail();
        });
        console.log(TAG + '[DataShareProvider] dataShareConfigSubscribeTest009 success:' + JSON.stringify(results));
        results.forEach((result) => {
          console.info(TAG + `[DataShareProvider] URI: ${result.uri}, Result: ${result.result}`);
          expect(result.result).assertEqual(1);
        });
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareConfigSubscribeTest009 failed.code =" + err.code +
          ",message=" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareConfigSubscribeTest010
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_0770
     * @tc.desc   single uri
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareConfigSubscribeTest010', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      let testUri = "datashareproxy://com.acts.dataShare.config.static/@#$%^&*";
      let uris = [
        testUri
      ]
      try {
        let results = g_handle!.onDataChange(uris, g_config, (changeInfo: dataShare.DataProxyChangeInfo[]) => {
          console.log(TAG + "[DataShareProvider] onProxyData receive change:" + JSON.stringify(changeInfo));
          expect().assertFail();
        });
        console.log(TAG + '[DataShareProvider] dataShareConfigSubscribeTest010 success:' + JSON.stringify(results));
        results.forEach((result) => {
          console.info(TAG + `[DataShareProvider] URI: ${result.uri}, Result: ${result.result}`);
          expect(result.result).assertEqual(1);
        });
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareConfigSubscribeTest010 failed.code =" + err.code +
          ",message=" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareConfigSubscribeTest011
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_0780
     * @tc.desc   single uri
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareConfigSubscribeTest011', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      let testUri = "datashareproxy://com.acts.dataShare.config.static/路径";
      let uris = [
        testUri
      ]
      try {
        let results = g_handle!.onDataChange(uris, g_config, (changeInfo: dataShare.DataProxyChangeInfo[]) => {
          console.log(TAG + "[DataShareProvider] onProxyData receive change:" + JSON.stringify(changeInfo));
          expect().assertFail();
        });
        console.log(TAG + '[DataShareProvider] dataShareConfigSubscribeTest011 success:' + JSON.stringify(results));
        results.forEach((result) => {
          console.info(TAG + `[DataShareProvider] URI: ${result.uri}, Result: ${result.result}`);
          expect(result.result).assertEqual(1);
        });
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareConfigSubscribeTest011 failed.code =" + err.code +
          ",message=" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareConfigSubscribeTest018
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_0850
     * @tc.desc   single uri
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareConfigSubscribeTest018', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1,
      async (done: () => void) => {
        let testUri = "datashareproxy://com.acts.dataShare.config.static/test1";
        let uris = [
          testUri
        ]
        try {
          let results = g_handle!.onDataChange(uris, g_config, (changeInfo: dataShare.DataProxyChangeInfo[]) => {
            console.log(TAG + "[DataShareProvider] onProxyData receive change:" + JSON.stringify(changeInfo));
            changeInfo.forEach((info) => {
              expect(info.type).assertEqual(2);
              expect(info.uri).assertEqual(testUri);
              expect(info.value).assertEqual("value is ok update");
            });
            g_handle!.offDataChange(uris, g_config);
            done();
          });
          console.log(TAG + "[DataShareProvider] dataShareConfigSubscribeTest018 success:" + JSON.stringify(results));
          results.forEach((result) => {
            console.info(TAG + `[DataShareProvider] URI: ${result.uri}, Result: ${result.result}`);
            expect(result.result).assertEqual(0);
          });
        } catch (err) {
          err = err as BusinessError;
          console.error(TAG + "[DataShareProvider] dataShareConfigSubscribeTest018 failed.code =" + err.code +
            ",message=" + err.message);
          expect().assertFail();
        }
        try {
          let proxyData: dataShare.ProxyData[] = [];
          let g_proxyDataAllowTest1: dataShare.ProxyData = {
            uri: testUri1,
            value: "value is ok update",
            allowList: ["all"]
          }
          proxyData.push(g_proxyDataAllowTest1);
          await g_handle!.publish(proxyData, g_config);
        } catch (err) {
          err = err as BusinessError;
          console.error(TAG + "[DataShareProvider] dataShareConfigSubscribeTest018 publish failed,code =" + err.code +
            ",message=" + err.message);
          expect().assertFail();
        }
      });

    /**
     * @tc.name   dataShareConfigSubscribeTest019
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_0860
     * @tc.desc   single uri
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareConfigSubscribeTest019', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1,
      async (done: () => void) => {
        let testUri = "datashareproxy://com.acts.dataShare.config.static/test1";
        let uris = [
          testUri
        ]
        try {
          let results = g_handle!.onDataChange(uris, g_config, (changeInfo: dataShare.DataProxyChangeInfo[]) => {
            console.log(TAG + "[DataShareProvider] onProxyData receive change:" + JSON.stringify(changeInfo));
            changeInfo.forEach((info) => {
              expect(info.type).assertEqual(1);
              expect(info.uri).assertEqual(testUri);
              expect(info.value).assertEqual("value is ok beforeEach");
            });
            g_handle!.offDataChange(uris, g_config);
            done();
          });
          console.log(TAG + "[DataShareProvider] dataShareConfigSubscribeTest019 success:" + JSON.stringify(results));
          results.forEach((result) => {
            console.info(TAG + `[DataShareProvider] URI: ${result.uri}, Result: ${result.result}`);
            expect(result.result).assertEqual(0);
          });
        } catch (err) {
          err = err as BusinessError;
          console.error(TAG + "[DataShareProvider] dataShareConfigSubscribeTest019 failed.code =" + err.code +
            ",message=" + err.message);
          expect().assertFail();
        }
        try {
          await g_handle!.delete(uris, g_config);
        } catch (err) {
          err = err as BusinessError;
          console.error(TAG + "[DataShareProvider] dataShareConfigSubscribeTest019 publish failed,code =" + err.code +
            ",message=" + err.message);
          expect().assertFail();
        }
      });
  })
}