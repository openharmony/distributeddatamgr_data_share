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
let uris = [
  testUri1,
]
let TAG = "ttt ";

export default function dataShareConfigUnSubscribeTest() {
  describe("dataShareConfigUnSubscribeTest", () => {
    /**
     * 发布配置
     */
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
      try {
        let proxyData: dataShare.ProxyData[] = [];
        let g_proxyDataAllowTest1: dataShare.ProxyData = {
          uri: testUri1,
          value: "value is ok beforeAll",
          allowList: ["all"]
        }
        proxyData.push(g_proxyDataAllowTest1);
        await g_handle!.publish(proxyData, g_config);
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] beforeEach publish failed, code=" + err.code + ",message=" + err.message);
        expect().assertFail();
      }
    })

    /**
     * 订阅uri
     */
    beforeEach(async () => {
      console.info(TAG + "beforeEach---------------------------------");
      try {
        console.error(TAG + "[DataShareProvider] onProxyData ----------- 1=")
        let results = g_handle!.onDataChange(uris, g_config, (changeInfo: Array<dataShare.DataProxyChangeInfo>) => {
          console.log(TAG + "[DataShareProvider] onProxyData receive change:" + JSON.stringify(changeInfo));
        });
        console.error(TAG + "[DataShareProvider] onProxyData ----------- 2")

        console.log(TAG + "[DataShareProvider] onProxyData success:" + JSON.stringify(results));
        results.forEach((result) => {
          console.info(TAG + `[DataShareProvider] URI:${result.uri},Result:${result.result}`);
          expect(result.result).assertEqual(0);
        });
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] onProxyData ----------- failed.code =" + err.code + ",message=" + err.message);
        expect().assertFail();
      }
    })

    /**
     * 取消订阅uri
     */
    afterEach(async () => {
      console.info(TAG + "afterEach---------------------------------");
      g_handle!.offDataChange(uris, g_config, undefined);
    })

    /**
     * 删除配置
     */
    afterAll(async () => {
      console.info(TAG + "afterAll---------------------------------");
      try {
        await g_handle!.delete(uris, g_config).then((results: dataShare.DataProxyResult[]) => {
          console.log("[ttt] [DataShareProvider] afterEach delete success,results:" + JSON.stringify(results));
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] delete failed.code=" + err.code + ",message=" + err.message);
          expect().assertFail();
        })
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] afterEach delete failed.code =" + err.code + ",message=" + err.message);
        expect().assertFail();
      }
    })

    /**
     * @tc.name   dataShareConfigUnSubscribeTest001
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_0890
     * @tc.desc   single uri
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareConfigUnSubscribeTest001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      let testUri = "datashareproxy://com.acts.dataShare.config.static/test1";
      let uris = [
        testUri
      ]
      try {
        let results = g_handle!.offDataChange(uris, g_config, undefined);
        expect(results.length > 0).assertTrue();
        results.forEach((result) => {
          console.info(TAG + `[DataShareProvider] off URI: ${result.uri}, Result: ${result.result}`);
          expect(result.result).assertEqual(0);
        });
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + '[DataShareProvider] dataShareConfigUnSubscribeTest001 failed.code =' + err.code + ",message =" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareConfigUnSubscribeTest008
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_0960
     * @tc.desc   single uri
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareConfigUnSubscribeTest008', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      let testUri = "";
      let uris = [
        testUri
      ]
      try {
        let results = g_handle!.offDataChange(uris, g_config);
        expect(results.length > 0).assertTrue();
        console.log(TAG + '[DataShareProvider] onProxyData success:' + JSON.stringify(results));
        results.forEach((result) => {
          console.info(TAG + `[DataShareProvider] URI: ${result.uri}, Result: ${result.result}`);
          expect(result.result).assertEqual(1);
        });
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + '[DataShareProvider] dataShareConfigUnSubscribeTest008 failed.code =' + err.code + ",message =" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareConfigUnSubscribeTest009
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_0970
     * @tc.desc   single uri
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareConfigUnSubscribeTest009', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      let tmpStr = "123456".repeat(33);
      let testUri = "datashareproxy://com.acts.dataShare.config.static/test1123" + tmpStr;
      let uris = [
        testUri
      ]
      try {
        let results = g_handle!.offDataChange(uris, g_config);
        console.log(TAG + '[DataShareProvider] onProxyData success:' + JSON.stringify(results));
        results.forEach((result) => {
          console.info(TAG + `[DataShareProvider] URI: ${result.uri}, Result: ${result.result}`);
          expect(result.result).assertEqual(1);
        });
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + '[DataShareProvider] dataShareConfigUnSubscribeTest009 failed.code =' + err.code + ",message =" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareConfigUnSubscribeTest010
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_0980
     * @tc.desc   single uri
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareConfigUnSubscribeTest010', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      console.log(TAG + '[DataShareProvider] dataShareConfigUnSubscribeTest010 start');
      let tmpStr = "123456".repeat(34);
      let testUri = "datashareproxy://com.acts.dataShare.config.static/test11234" + tmpStr + "9";
      let uris = [
        testUri
      ]
      try {
        let results = g_handle!.offDataChange(uris, g_config);
        console.log(TAG + '[DataShareProvider] dataShareConfigUnSubscribeTest010 offProxyData success:' + JSON.stringify(results));
        expect().assertFail();
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + '[DataShareProvider] dataShareConfigUnSubscribeTest010 failed.code =' + err.code + ",message =" + err.message);
        expect(err.code).assertEqual(15700014);
      }
    });

    /**
     * @tc.name   dataShareConfigUnSubscribeTest011
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_0990
     * @tc.desc   single uri
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareConfigUnSubscribeTest011', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      let testUri = " ";
      let uris = [
        testUri
      ]
      try {
        let results = g_handle!.offDataChange(uris, g_config);
        console.log(TAG + '[DataShareProvider] offProxyData success:' + JSON.stringify(results));
        results.forEach((result) => {
          console.info(TAG + `[DataShareProvider] URI: ${result.uri}, Result: ${result.result}`);
          expect(result.result).assertEqual(1);
        });
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + '[DataShareProvider] dataShareConfigUnSubscribeTest011 failed.code =' + err.code + ",message =" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareConfigUnSubscribeTest012
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_1000
     * @tc.desc   single uri
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareConfigUnSubscribeTest012', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      let testUri = "@#￥%^&*";
      let uris = [
        testUri
      ]
      try {
        let results = g_handle!.offDataChange(uris, g_config);
        console.log(TAG + '[DataShareProvider] offProxyData success:' + JSON.stringify(results));
        results.forEach((result) => {
          console.info(TAG + `[DataShareProvider] URI: ${result.uri}, Result: ${result.result}`);
          expect(result.result).assertEqual(1);
        });
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + '[DataShareProvider] dataShareConfigUnSubscribeTest012 failed.code =' + err.code + ",message =" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareConfigUnSubscribeTest015
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_1030
     * @tc.desc   single uri
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareConfigUnSubscribeTest015', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      let testUri = "datashareproxy://com.acts.dataShare.config.static/ ";
      let uris = [
        testUri
      ]
      try {
        let results = g_handle!.offDataChange(uris, g_config);
        console.log(TAG + '[DataShareProvider] onProxyData success:' + JSON.stringify(results));
        results.forEach((result) => {
          console.info(TAG + `[DataShareProvider] URI: ${result.uri}, Result: ${result.result}`);
          expect(result.result).assertEqual(1);
        });
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + '[DataShareProvider] dataShareConfigUnSubscribeTest015 failed.code =' + err.code + ",message =" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareConfigUnSubscribeTest016
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_1050
     * @tc.desc   single uri
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareConfigUnSubscribeTest016', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      let testUri = "datashareproxy://com.acts.dataShare.config.static/路径";
      let uris = [
        testUri
      ]
      try {
        let results = g_handle!.offDataChange(uris, g_config);
        console.log(TAG + '[DataShareProvider] onProxyData success:' + JSON.stringify(results));
        results.forEach((result) => {
          console.info(TAG + `[DataShareProvider] URI: ${result.uri}, Result: ${result.result}`);
          expect(result.result).assertEqual(1);
        });
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + '[DataShareProvider] dataShareConfigUnSubscribeTest016 failed.code =' + err.code + ",message =" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareConfigUnSubscribeTest017
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_1060
     * @tc.desc   single uri
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareConfigUnSubscribeTest017', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      console.error(TAG + '[DataShareProvider] dataShareConfigUnSubscribeTest017 start');
      let testUri = "datashareproxy://com.acts.dataShare.config.static/test1";
      let uris = [
        testUri
      ]
      try {
        console.error(TAG + '[DataShareProvider] dataShareConfigUnSubscribeTest017 start 0');
        let results = g_handle!.offDataChange(uris, g_config);
        console.error(TAG + '[DataShareProvider] dataShareConfigUnSubscribeTest017 start 1');
        console.log(TAG + '[DataShareProvider] onProxyData success:' + JSON.stringify(results));
        results.forEach((result) => {
          console.info(TAG + `[DataShareProvider] URI: ${result.uri}, Result: ${result.result}`);
          expect(result.result).assertEqual(0);
          expect(result.uri).assertEqual(testUri);
        });
        console.error(TAG + '[DataShareProvider] dataShareConfigUnSubscribeTest017 start 2');
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + '[DataShareProvider] dataShareConfigUnSubscribeTest017 failed.code =' + err.code + ",message =" + err.message);
        expect().assertFail();
      }
    });
  })
}