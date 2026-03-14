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
let TAG = "ttt ";

export default function dataShareConfigPublishTest() {
  describe("dataShareConfigPublishTest", () => {
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
        console.error(TAG + `[DataShareProvider] createDataProxyHandle failed: code:${err.code},message:${err.message}`);
      })
    })

    beforeEach(async () => {
      console.info(TAG + "beforeEach---------------------------------");
    })

    afterEach(async () => {
      console.info(TAG + "afterEach---------------------------------");
      let uris: string[] = [];
      for (let i = 0; i < 32; i++) {
        let testUri = "datashareproxy://com.acts.dataShare.config.static/test" + i;
        uris.push(testUri);
      }
      await g_handle!.delete(uris, g_config);
      let tmpStr = "123456".repeat(33);
      let testUri2 = "datashareproxy://com.acts.dataShare.config.static/test1123" + tmpStr;
      let uris2 = [testUri2];
      await g_handle!.delete(uris2, g_config);
    })

    afterAll(async () => {
      console.info(TAG + "afterAll---------------------------------");
    })

    /**
     * @tc.name   dataShareConfigPublishTest001
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_0010
     * @tc.desc   single uri
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareConfigPublishTest001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      let testUri1 = "datashareproxy://com.acts.dataShare.config.static/test1";
      try {
        let proxyData: dataShare.ProxyData[] = [];
        let g_proxyDataAllowTest1: dataShare.ProxyData = {
          uri: testUri1,
          value: "value is ok",
          allowList: []
        }
        proxyData.push(g_proxyDataAllowTest1);
        await g_handle!.publish(proxyData, g_config).then((results: dataShare.DataProxyResult[]) => {
          expect(results.length > 0).assertTrue();
          console.log(TAG + "[DataShareProvider] publishProxyDataTest001 success,proxyData allowList length is:" + JSON.stringify(proxyData[0].allowList?.length));
          console.log(TAG + "[DataShareProvider] publishProxyDataTest001 success,results:" + JSON.stringify(results));
          let uri = results[0].uri;
          let rtnCode = results[0].result;
          expect(uri).assertEqual(testUri1);
          expect(rtnCode).assertEqual(dataShare.DataProxyErrorCode.SUCCESS);
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] publish failed.code =" + err.code + ",message=" + err.message);
          expect().assertFail();
        })
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] publishProxyDataTest001 failed,code =" + err.code + ",message=" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareConfigPublishTest002
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_0070
     * @tc.desc   uri is 256
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareConfigPublishTest002', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      let tmpStr = "123456".repeat(33);
      let testUri1 = "datashareproxy://com.acts.dataShare.config.static/test1123" + tmpStr;
      let uris = [testUri1];
      try {
        let proxyData: dataShare.ProxyData[] = [];
        let g_proxyDataAllowTest1: dataShare.ProxyData = {
          uri: testUri1,
          value: "value is ok",
          allowList: []
        }
        proxyData.push(g_proxyDataAllowTest1);
        await g_handle!.publish(proxyData, g_config).then((results: dataShare.DataProxyResult[]) => {
          expect(results.length > 0).assertTrue();
          console.log(TAG + "[DataShareProvider] dataShareConfigPublishTest002 success,results:" + JSON.stringify(results));
          console.log(TAG + "[DataShareProvider] dataShareConfigPublishTest002 success,results uri.length:" + results[0].uri.toString().length);
          let uri = results[0].uri;
          let rtnCode = results[0].result;
          expect(uri).assertEqual(testUri1);
          expect(rtnCode).assertEqual(0);
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] publish failed.code =" + err.code + ",message=" + err.message);
          expect().assertFail();
        })
        await g_handle!.delete(uris, g_config);
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareConfigPublishTest002 failed.code =" + err.code + ",message=" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareConfigPublishTest003
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_0110
     * @tc.desc   depth is 2,UPSTREAM
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareConfigPublishTest003', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      try {
        let testUri1 = "";
        let proxyData: dataShare.ProxyData[] = [];
        let g_proxyDataAllowTest1: dataShare.ProxyData = {
          uri: testUri1,
          value: "value is ok",
          allowList: []
        }
        proxyData.push(g_proxyDataAllowTest1);
        await g_handle!.publish(proxyData, g_config).then((results: dataShare.DataProxyResult[]) => {
          expect(results.length > 0).assertTrue();
          console.log(TAG + "[DataShareProvider] dataShareConfigPublishTest003 success,proxyData allowList length is:" + JSON.stringify(proxyData[0].allowList?.length));
          console.log(TAG + "[DataShareProvider] dataShareConfigPublishTest003 success,results:" + JSON.stringify(results));
          let uri = results[0].uri;
          let rtnCode = results[0].result;
          expect(uri).assertEqual(testUri1);
          expect(rtnCode).assertEqual(dataShare.DataProxyErrorCode.NO_PERMISSION);
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] publish failed.code =" + err.code + ",message=" + err.message);
          expect().assertFail();
        })
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareConfigPublishTest003 failed.code =" + err.code + ",message=" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareConfigPublishTest004
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_0060
     * @tc.desc   depth is 2,UPSTREAM
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareConfigPublishTest004', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      try {
        let tmpStr = "123456".repeat(34);
        let testUri1 = "datashareproxy://com.acts.dataShare.config.static/test11234" + tmpStr + "9";
        let proxyData: dataShare.ProxyData[] = [];
        let g_proxyDataAllowTest1: dataShare.ProxyData = {
          uri: testUri1,
          value: "value is ok",
          allowList: []
        }
        proxyData.push(g_proxyDataAllowTest1);
        await g_handle!.publish(proxyData, g_config).then((results: dataShare.DataProxyResult[]) => {
          console.log(TAG + "[DataShareProvider] dataShareConfigPublishTest004 success,proxyData allowList length is:" + JSON.stringify(proxyData[0].allowList?.length));
          console.log(TAG + "[DataShareProvider] dataShareConfigPublishTest004 success,results:" + JSON.stringify(results));
          expect().assertFail();
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] publish failed.code =" + err.code + ",message=" + err.message);
          expect().assertFail();
        })
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareConfigPublishTest004 failed.code =" + err.code + ",message=" + err.message);
        expect(err.code).assertEqual(15700014);
      }
    });

    /**
     * @tc.name   dataShareConfigPublishTest005
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_0130
     * @tc.desc   depth is 2,UPSTREAM
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareConfigPublishTest005', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      try {
        let testUri1 = "";
        let proxyData: dataShare.ProxyData[] = [];
        let g_proxyDataAllowTest1: dataShare.ProxyData = {
          uri: testUri1,
          value: "value is ok",
          allowList: []
        }
        proxyData.push(g_proxyDataAllowTest1);
        await g_handle!.publish(proxyData, g_config).then((results: dataShare.DataProxyResult[]) => {
          console.log(TAG + "[DataShareProvider] dataShareConfigPublishTest005 success,proxyData allowList length is:" + JSON.stringify(proxyData[0].allowList?.length));
          console.log(TAG + "[DataShareProvider] dataShareConfigPublishTest005 success,results:" + JSON.stringify(results));
          let uri = results[0].uri;
          let rtnCode = results[0].result;
          expect(uri).assertEqual(testUri1);
          expect(rtnCode).assertEqual(2);
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] publish failed.code =" + err.code + ",message=" + err.message);
          expect().assertFail();
        })
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareConfigPublishTest005 failed.code =" + err.code + ",message=" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareConfigPublishTest006
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_0140
     * @tc.desc   depth is 2,UPSTREAM
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareConfigPublishTest006', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      try {
        let testUri1 = "@#$%^&*";
        let proxyData: dataShare.ProxyData[] = [];
        let g_proxyDataAllowTest1: dataShare.ProxyData = {
          uri: testUri1,
          value: "value is ok",
          allowList: []
        }
        proxyData.push(g_proxyDataAllowTest1);
        await g_handle!.publish(proxyData, g_config).then((results: dataShare.DataProxyResult[]) => {
          console.log(TAG + "[DataShareProvider] dataShareConfigPublishTest006 success,proxyData allowList length is:" + JSON.stringify(proxyData[0].allowList?.length));
          console.log(TAG + "[DataShareProvider] dataShareConfigPublishTest006 success,results:" + JSON.stringify(results));
          let uri = results[0].uri;
          let rtnCode = results[0].result;
          expect(uri).assertEqual(testUri1);
          expect(rtnCode).assertEqual(2);
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] publish failed.code =" + err.code + ",message=" + err.message);
          expect().assertFail();
        })
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareConfigPublishTest006 failed.code =" + err.code + ",message=" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareConfigPublishTest007
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_0170
     * @tc.desc   depth is 2,UPSTREAM
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareConfigPublishTest007', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      try {
        let testUri1 = "datashareproxy:// /test1";
        let proxyData: dataShare.ProxyData[] = [];
        let g_proxyDataAllowTest1: dataShare.ProxyData = {
          uri: testUri1,
          value: "value is ok",
          allowList: []
        }
        proxyData.push(g_proxyDataAllowTest1);
        await g_handle!.publish(proxyData, g_config).then((results: dataShare.DataProxyResult[]) => {
          console.log(TAG + "[DataShareProvider] dataShareConfigPublishTest007 success,proxyData allowList length is:" + JSON.stringify(proxyData[0].allowList?.length));
          console.log(TAG + "[DataShareProvider] dataShareConfigPublishTest007 success,results:" + JSON.stringify(results));
          let uri = results[0].uri;
          let rtnCode = results[0].result;
          expect(uri).assertEqual(testUri1);
          expect(rtnCode).assertEqual(2);
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] publish failed.code =" + err.code + ",message=" + err.message);
          expect().assertFail();
        })
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareConfigPublishTest007 failed.code =" + err.code + ",message=" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareConfigPublishTest008
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_0180
     * @tc.desc   depth is 2,UPSTREAM
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareConfigPublishTest008', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      try {
        let testUri1 = "datashareproxy://@#$%^*/test1";
        let proxyData: dataShare.ProxyData[] = [];
        let g_proxyDataAllowTest1: dataShare.ProxyData = {
          uri: testUri1,
          value: "value is ok",
          allowList: []
        }
        proxyData.push(g_proxyDataAllowTest1);
        await g_handle!.publish(proxyData, g_config).then((results: dataShare.DataProxyResult[]) => {
          console.log(TAG + "[DataShareProvider] dataShareConfigPublishTest008 success,proxyData allowList length is:" + JSON.stringify(proxyData[0].allowList?.length));
          console.log(TAG + "[DataShareProvider] dataShareConfigPublishTest008 success,results:" + JSON.stringify(results));
          let uri = results[0].uri;
          let rtnCode = results[0].result;
          expect(uri).assertEqual(testUri1);
          expect(rtnCode).assertEqual(2);
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] publish failed.code =" + err.code + ",message=" + err.message);
          expect().assertFail();
        })
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareConfigPublishTest008 failed.code =" + err.code + ",message=" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareConfigPublishTest009
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_0190
     * @tc.desc   depth is 2,UPSTREAM
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareConfigPublishTest009', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      try {
        let testUri1 = "datashareproxy://动态/test1";
        let proxyData: dataShare.ProxyData[] = [];
        let g_proxyDataAllowTest1: dataShare.ProxyData = {
          uri: testUri1,
          value: "value is ok",
          allowList: []
        }
        proxyData.push(g_proxyDataAllowTest1);
        await g_handle!.publish(proxyData, g_config).then((results: dataShare.DataProxyResult[]) => {
          console.log(TAG + "[DataShareProvider] dataShareConfigPublishTest009 success,proxyData allowList length is:" + JSON.stringify(proxyData[0].allowList?.length));
          console.log(TAG + "[DataShareProvider] dataShareConfigPublishTest009 success,results:" + JSON.stringify(results));
          let uri = results[0].uri;
          let rtnCode = results[0].result;
          expect(uri).assertEqual(testUri1);
          expect(rtnCode).assertEqual(2);
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] publish failed.code =" + err.code + ",message=" + err.message);
          expect().assertFail();
        })
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareConfigPublishTest009 failed.code =" + err.code + ",message=" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareConfigPublishTest010
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_0200
     * @tc.desc   depth is 2,UPSTREAM
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareConfigPublishTest010', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      try {
        let testUri1 = "datashareproxy://1254qwedfsfwe/test1";
        let proxyData: dataShare.ProxyData[] = [];
        let g_proxyDataAllowTest1: dataShare.ProxyData = {
          uri: testUri1,
          value: "value is ok",
          allowList: []
        }
        proxyData.push(g_proxyDataAllowTest1);
        await g_handle!.publish(proxyData, g_config).then((results: dataShare.DataProxyResult[]) => {
          console.log(TAG + "[DataShareProvider] dataShareConfigPublishTest010 success,proxyData allowList length is:" + JSON.stringify(proxyData[0].allowList?.length));
          console.log(TAG + "[DataShareProvider] dataShareConfigPublishTest010 success,results:" + JSON.stringify(results));
          let uri = results[0].uri;
          let rtnCode = results[0].result;
          expect(uri).assertEqual(testUri1);
          expect(rtnCode).assertEqual(2);
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] publish failed.code =" + err.code + ",message=" + err.message);
          expect().assertFail();
        })
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareConfigPublishTest010 failed.code =" + err.code + ",message=" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareConfigPublishTest014
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_0260
     * @tc.desc   depth is 2,UPSTREAM
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareConfigPublishTest014', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      try {
        let testUri1 = "datashareproxy://com.acts.dataShare.config.static/test1";
        let proxyData: dataShare.ProxyData[] = [];
        let g_proxyDataAllowTest1: dataShare.ProxyData = {
          uri: testUri1,
          value: "value is ok",
          allowList: []
        }
        proxyData.push(g_proxyDataAllowTest1);
        await g_handle!.publish(proxyData, g_config).then((results: dataShare.DataProxyResult[]) => {
          console.log(TAG + "[DataShareProvider] dataShareConfigPublishTest014 success,proxyData allowList length is:" + JSON.stringify(proxyData[0].allowList?.length));
          console.log(TAG + "[DataShareProvider] dataShareConfigPublishTest014 success,results:" + JSON.stringify(results));
          let uri = results[0].uri;
          let rtnCode = results[0].result;
          expect(uri).assertEqual(testUri1);
          expect(rtnCode).assertEqual(0);
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] publish failed.code =" + err.code + ",message=" + err.message);
          expect().assertFail();
        })
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareConfigPublishTest014 failed.code =" + err.code + ",message=" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareConfigPublishTest015
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_0280
     * @tc.desc   value length is 4096
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareConfigPublishTest015', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      try {
        let testUri1 = "datashareproxy://com.acts.dataShare.config.static/test1";
        let proxyData: dataShare.ProxyData[] = [];
        let g_proxyDataAllowTest1: dataShare.ProxyData = {
          uri: testUri1,
          value: "valueisokgoodddd".repeat(256),
          allowList: []
        }
        proxyData.push(g_proxyDataAllowTest1);
        await g_handle!.publish(proxyData, g_config).then((results: dataShare.DataProxyResult[]) => {
          console.log(TAG + "[DataShareProvider] dataShareConfigPublishTest015 success,proxyData allowList length is:" + JSON.stringify(proxyData[0].allowList?.length));
          console.log(TAG + "[DataShareProvider] dataShareConfigPublishTest015 success,results:" + JSON.stringify(results));
          let uri = results[0].uri;
          let rtnCode = results[0].result;
          expect(uri).assertEqual(testUri1);
          expect(rtnCode).assertEqual(0);
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] publish failed.code =" + err.code + ",message=" + err.message);
          expect().assertFail();
        })
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareConfigPublishTest015 failed.code =" + err.code + ",message=" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareConfigPublishTest016
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_0270
     * @tc.desc   value length is 4097
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareConfigPublishTest016', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      try {
        let testUri1 = "datashareproxy://com.acts.dataShare.config.static/test1";
        let proxyData: dataShare.ProxyData[] = [];
        let g_proxyDataAllowTest1: dataShare.ProxyData = {
          uri: testUri1,
          value: "valueisokgoodddd".repeat(256) + "9",
          allowList: []
        }
        proxyData.push(g_proxyDataAllowTest1);
        await g_handle!.publish(proxyData, g_config).then((results: dataShare.DataProxyResult[]) => {
          expect().assertFail();
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] publish failed.code =" + err.code + ",message=" + err.message);
          expect().assertFail();
        })
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareConfigPublishTest016 failed.code =" + err.code + ",message=" + err.message);
        expect(err.code).assertEqual(15700014);
      }
    });

    /**
     * @tc.name   dataShareConfigPublishTest017
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_0220
     * @tc.desc   depth is 2,UPSTREAM
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareConfigPublishTest017', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      try {
        let testUri1 = "datashareproxy://com.acts.dataShare.config.static/test1";
        let proxyData: dataShare.ProxyData[] = [];
        let g_proxyDataAllowTest1: dataShare.ProxyData = {
          uri: testUri1,
          value: "",
          allowList: []
        }
        proxyData.push(g_proxyDataAllowTest1);
        await g_handle!.publish(proxyData, g_config).then((results: dataShare.DataProxyResult[]) => {
          console.log(TAG + "[DataShareProvider] dataShareConfigPublishTest017 success,proxyData allowList length is:" + JSON.stringify(proxyData[0].allowList?.length));
          console.log(TAG + "[DataShareProvider] dataShareConfigPublishTest017 success,results:" + JSON.stringify(results));
          let uri = results[0].uri;
          let rtnCode = results[0].result;
          expect(uri).assertEqual(testUri1);
          expect(rtnCode).assertEqual(0);
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] publish failed.code =" + err.code + ",message=" + err.message);
          expect().assertFail();
        })
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareConfigPublishTest017 failed.code =" + err.code + ",message=" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareConfigPublishTest018
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_0230
     * @tc.desc   depth is 2,UPSTREAM
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareConfigPublishTest018', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      try {
        let testUri1 = "datashareproxy://com.acts.dataShare.config.static/test1";
        let proxyData: dataShare.ProxyData[] = [];
        let g_proxyDataAllowTest1: dataShare.ProxyData = {
          uri: testUri1,
          value: "@#$%^&*",
          allowList: []
        }
        proxyData.push(g_proxyDataAllowTest1);
        await g_handle!.publish(proxyData, g_config).then((results: dataShare.DataProxyResult[]) => {
          console.log(TAG + "[DataShareProvider] dataShareConfigPublishTest018 success,proxyData allowList length is:" + JSON.stringify(proxyData[0].allowList?.length));
          console.log(TAG + "[DataShareProvider] dataShareConfigPublishTest018 success,results:" + JSON.stringify(results));
          let uri = results[0].uri;
          let rtnCode = results[0].result;
          expect(uri).assertEqual(testUri1);
          expect(rtnCode).assertEqual(0);
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] publish failed.code =" + err.code + ",message=" + err.message);
          expect().assertFail();
        })
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareConfigPublishTest018 failed.code =" + err.code + ",message=" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareConfigPublishTest019
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_0250
     * @tc.desc   depth is 2,UPSTREAM
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareConfigPublishTest019', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      try {
        let testUri1 = "datashareproxy://com.acts.dataShare.config.static/test1";
        let proxyData: dataShare.ProxyData[] = [];
        let g_proxyDataAllowTest1: dataShare.ProxyData = {
          uri: testUri1,
          value: undefined,
          allowList: []
        }
        proxyData.push(g_proxyDataAllowTest1);
        await g_handle!.publish(proxyData, g_config).then((results: dataShare.DataProxyResult[]) => {
          console.log(TAG + "[DataShareProvider] dataShareConfigPublishTest019 success,proxyData allowList length is:" + JSON.stringify(proxyData[0].allowList?.length));
          console.log(TAG + "[DataShareProvider] dataShareConfigPublishTest019 success,results:" + JSON.stringify(results));
          let uri = results[0].uri;
          let rtnCode = results[0].result;
          expect(uri).assertEqual(testUri1);
          expect(rtnCode).assertEqual(0);
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] publish failed.code =" + err.code + ",message=" + err.message);
          expect().assertFail();
        })
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareConfigPublishTest019 failed.code =" + err.code + ",message=" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareConfigPublishTest020
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_0290
     * @tc.desc   depth is 2,UPSTREAM
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareConfigPublishTest020', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      try {
        let testUri1 = "datashareproxy://com.acts.dataShare.config.static/test1";
        let proxyData: dataShare.ProxyData[] = [];
        let allowListArr: Array<string> = [];
        for (let i = 0; i < 256; i++) {
          let appIdentifier = "6918702203883852463" + i;
          allowListArr.push(appIdentifier);
        }
        let g_proxyDataAllowTest1: dataShare.ProxyData = {
          uri: testUri1,
          value: "value is ok",
          allowList: allowListArr
        }
        proxyData.push(g_proxyDataAllowTest1);
        await g_handle!.publish(proxyData, g_config).then((results: dataShare.DataProxyResult[]) => {
          console.log(TAG + "[DataShareProvider] dataShareConfigPublishTest020 success,proxyData allowList length is:" + JSON.stringify(proxyData[0].allowList?.length));
          console.log(TAG + "[DataShareProvider] dataShareConfigPublishTest020 success,results:" + JSON.stringify(results));
          let uri = results[0].uri;
          let rtnCode = results[0].result;
          expect(uri).assertEqual(testUri1);
          expect(rtnCode).assertEqual(0);
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] publish failed.code =" + err.code + ",message=" + err.message);
          expect().assertFail();
        })
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareConfigPublishTest020 failed.code =" + err.code + ",message=" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareConfigPublishTest021
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_0300
     * @tc.desc   allowList length is 256
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareConfigPublishTest021', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      try {
        let testUri1 = "datashareproxy://com.acts.dataShare.config.static/test1";
        let proxyData: dataShare.ProxyData[] = [];
        let allowListArr: Array<string> = [];
        for (let i = 0; i < 257; i++) {
          let appIdentifier = "6918702203883852463" + i;
          allowListArr.push(appIdentifier);
        }
        let g_proxyDataAllowTest1: dataShare.ProxyData = {
          uri: testUri1,
          value: "value is ok",
          allowList: allowListArr
        }
        proxyData.push(g_proxyDataAllowTest1);
        await g_handle!.publish(proxyData, g_config).then((results: dataShare.DataProxyResult[]) => {
          console.log(TAG + "[DataShareProvider] dataShareConfigPublishTest021 success,proxyData allowList length is:" + JSON.stringify(proxyData[0].allowList?.length));
          console.log(TAG + "[DataShareProvider] dataShareConfigPublishTest021 success,results:" + JSON.stringify(results));
          let rtnCode = results[0].result;
          expect(rtnCode).assertEqual(0);
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] publish failed.code =" + err.code + ",message=" + err.message);
          expect().assertFail();
        })
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareConfigPublishTest021 failed.code =" + err.code + ",message=" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareConfigPublishTest022
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_0330
     * @tc.desc   depth is 2,UPSTREAM
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareConfigPublishTest022', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      try {
        let testUri1 = "datashareproxy://com.acts.dataShare.config.static/test1";
        let proxyData: dataShare.ProxyData[] = [];
        let g_proxyDataAllowTest1: dataShare.ProxyData = {
          uri: testUri1,
          value: "value is ok",
          allowList: [""]
        }
        proxyData.push(g_proxyDataAllowTest1);
        await g_handle!.publish(proxyData, g_config).then((results: dataShare.DataProxyResult[]) => {
          console.log(TAG + "[DataShareProvider] dataShareConfigPublishTest022 success,proxyData allowList length is:" + JSON.stringify(proxyData[0].allowList?.length));
          console.log(TAG + "[DataShareProvider] dataShareConfigPublishTest022 success,results:" + JSON.stringify(results));
          let uri = results[0].uri;
          let rtnCode = results[0].result;
          expect(uri).assertEqual(testUri1);
          expect(rtnCode).assertEqual(0);
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] publish failed.code =" + err.code + ",message=" + err.message);
          expect().assertFail();
        })
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareConfigPublishTest022 failed.code =" + err.code + ",message=" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareConfigPublishTest023
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_0340
     * @tc.desc   depth is 2,UPSTREAM
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareConfigPublishTest023', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      try {
        let testUri1 = "datashareproxy://com.acts.dataShare.config.static/test1";
        let proxyData: dataShare.ProxyData[] = [];
        let g_proxyDataAllowTest1: dataShare.ProxyData = {
          uri: testUri1,
          value: "value is ok",
          allowList: ["@#$%^&*"]
        }
        proxyData.push(g_proxyDataAllowTest1);
        await g_handle!.publish(proxyData, g_config).then((results: dataShare.DataProxyResult[]) => {
          console.log(TAG + "[DataShareProvider] dataShareConfigPublishTest023 success,proxyData allowList length is:" + JSON.stringify(proxyData[0].allowList?.length));
          console.log(TAG + "[DataShareProvider] dataShareConfigPublishTest023 success,results:" + JSON.stringify(results));
          let uri = results[0].uri;
          let rtnCode = results[0].result;
          expect(uri).assertEqual(testUri1);
          expect(rtnCode).assertEqual(0);
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] publish failed.code =" + err.code + ",message=" + err.message);
          expect().assertFail();
        })
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareConfigPublishTest023 failed.code =" + err.code + ",message=" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareConfigPublishTest024
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_0360
     * @tc.desc   depth is 2,UPSTREAM
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareConfigPublishTest024', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      try {
        let testUri1 = "datashareproxy://com.acts.dataShare.config.static/test1";
        let proxyData: dataShare.ProxyData[] = [];
        let g_proxyDataAllowTest1: dataShare.ProxyData = {
          uri: testUri1,
          value: "value is ok",
          allowList: undefined
        }
        proxyData.push(g_proxyDataAllowTest1);
        await g_handle!.publish(proxyData, g_config).then((results: dataShare.DataProxyResult[]) => {
          console.log(TAG + "[DataShareProvider] dataShareConfigPublishTest024 success,proxyData allowList length is:" + JSON.stringify(proxyData[0].allowList?.length));
          console.log(TAG + "[DataShareProvider] dataShareConfigPublishTest024 success,results:" + JSON.stringify(results));
          let uri = results[0].uri;
          let rtnCode = results[0].result;
          expect(uri).assertEqual(testUri1);
          expect(rtnCode).assertEqual(0);
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] publish failed.code =" + err.code + ",message=" + err.message);
          expect().assertFail();
        })
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareConfigPublishTest024 failed.code =" + err.code + ",message=" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareConfigPublishTest027
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_0120
     * @tc.desc   proxyData is []
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareConfigPublishTest027', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      try {
        let proxyData: dataShare.ProxyData[] = [];
        await g_handle!.publish(proxyData, g_config).then((results: dataShare.DataProxyResult[]) => {
          console.log(TAG + "[DataShareProvider] dataShareConfigPublishTest027 success,results:" + JSON.stringify(results));
          expect(results.length).assertEqual(0);
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] publish failed.code =" + err.code + ",message=" + err.message);
          expect().assertFail();
        })
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareConfigPublishTest027 failed.code =" + err.code + ",message=" + err.message);
        expect(err.code).assertEqual(15700014);
      }
    });

    /**
     * @tc.name   dataShareConfigPublishTest028
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_0080
     * @tc.desc   uri is 32
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareConfigPublishTest028', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      try {
        let proxyData: dataShare.ProxyData[] = [];
        for (let i = 0; i < 32; i++) {
          let testUri = "datashareproxy://com.acts.dataShare.config.static/test" + i;
          let g_proxyDataAllowTest: dataShare.ProxyData = {
            uri: testUri,
            value: "value is less" + i,
            allowList: []
          }
          proxyData.push(g_proxyDataAllowTest);
        }
        await g_handle!.publish(proxyData, g_config).then((results: dataShare.DataProxyResult[]) => {
          console.log(TAG + "[DataShareProvider] dataShareConfigPublishTest028 success,proxyData allowList length is:" + JSON.stringify(proxyData[0].allowList?.length));
          console.log(TAG + "[DataShareProvider] dataShareConfigPublishTest028 success,results:" + JSON.stringify(results));
          results.forEach((result) => {
            let rtnCode = result.result;
            expect(rtnCode).assertEqual(0);
          });
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] publish failed.code =" + err.code + ",message=" + err.message);
          expect().assertFail();
        })
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareConfigPublishTest028 failed.code =" + err.code + ",message=" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareConfigPublishTest029
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_0100
     * @tc.desc   uri is 33
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareConfigPublishTest029', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      try {
        let proxyData: dataShare.ProxyData[] = [];
        for (let i = 0; i < 33; i++) {
          let testUri = "datashareproxy://com.acts.dataShare.config.static/test" + i;
          let g_proxyDataAllowTest: dataShare.ProxyData = {
            uri: testUri,
            value: "value is less" + i,
            allowList: []
          }
          proxyData.push(g_proxyDataAllowTest);
        }
        await g_handle!.publish(proxyData, g_config).then((results: dataShare.DataProxyResult[]) => {
          console.log(TAG + "[DataShareProvider] dataShareConfigPublishTest029 success,results:" + JSON.stringify(results));
          expect().assertFail();
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] publish failed.code =" + err.code + ",message=" + err.message);
          expect().assertFail();
        })
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareConfigPublishTest029 failed.code =" + err.code + ",message=" + err.message);
        expect(err.code).assertEqual(15700014);
      }
    });

    /**
     * @tc.name   dataShareConfigPublishTest030
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_0090
     * @tc.desc   uri is 32,then add uri
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareConfigPublishTest030', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      try {
        let proxyData: dataShare.ProxyData[] = [];
        for (let i = 0; i < 32; i++) {
          let testUri = "datashareproxy://com.acts.dataShare.config.static/test" + i;
          let g_proxyDataAllowTest: dataShare.ProxyData = {
            uri: testUri,
            value: "value is less" + i,
            allowList: []
          }
          proxyData.push(g_proxyDataAllowTest);
        }
        await g_handle!.publish(proxyData, g_config).then((results: dataShare.DataProxyResult[]) => {
          console.log(TAG + "[DataShareProvider] dataShareConfigPublishTest030 success,proxyData allowList length is:" + JSON.stringify(proxyData[0].allowList?.length));
          console.log(TAG + "[DataShareProvider] dataShareConfigPublishTest030 success,results:" + JSON.stringify(results));
          results.forEach((result) => {
            let rtnCode = result.result;
            expect(rtnCode).assertEqual(0);
          });
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] publish1 failed.code =" + err.code + ",message=" + err.message);
          expect().assertFail();
        })
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareConfigPublishTest030 failed.code =" + err.code + ",message=" + err.message);
        expect().assertFail();
      }

      try {
        let testUri1 = "datashareproxy://com.acts.dataShare.config.static/test99";
        let proxyData: dataShare.ProxyData[] = [];
        let g_proxyDataAllowTest1: dataShare.ProxyData = {
          uri: testUri1,
          value: "value is ok",
          allowList: []
        }
        proxyData.push(g_proxyDataAllowTest1);
        await g_handle!.publish(proxyData, g_config).then((results: dataShare.DataProxyResult[]) => {
          console.log(TAG + "[DataShareProvider] publish2 end, results:" + JSON.stringify(results));
          results.forEach((result) => {
            expect(result.result).assertEqual(dataShare.DataProxyErrorCode.OVER_LIMIT);
          })
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] publish2 failed.code =" + err.code + ",message=" + err.message);
          expect().assertFail();
        })
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareConfigPublishTest030 failed.code =" + err.code + ",message=" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareConfigPublishTest031
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_0350
     * @tc.desc   allowList length is 256
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareConfigPublishTest031', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      try {
        let testUri1 = "datashareproxy://com.acts.dataShare.config.static/test1";
        let proxyData: dataShare.ProxyData[] = [];
        let allowListArr: Array<string> = [];
        for (let i = 0; i < 257; i++) {
          let appIdentifier = "6918702203883852463" + i;
          allowListArr.push(appIdentifier);
        }
        let g_proxyDataAllowTest1: dataShare.ProxyData = {
          uri: testUri1,
          value: "value is ok",
          allowList: allowListArr
        }
        proxyData.push(g_proxyDataAllowTest1);
        await g_handle!.publish(proxyData, g_config).then((results: dataShare.DataProxyResult[]) => {
          g_handle!.get([testUri1], g_config).then((results: dataShare.DataProxyGetResult[]) => {
            results.forEach((result) => {
              console.info(TAG + `[DataShareProvider] URI:${result.uri},Value:${result.value},Result:${result.result},AllowList:${result.allowList}`);
              expect(result.allowList?.length).assertEqual(256);
            });
          }).catch((err: Error) => {
            console.error(TAG + "[DataShareProvider] get failed.err =" + JSON.stringify(err));
            expect().assertFail();
          })
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] publish failed.code =" + err.code + ",message=" + err.message);
          expect().assertFail();
        })
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareConfigPublishTest031 failed.code =" + err.code + ",message=" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareConfigPublishTest032
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_0020
     * @tc.desc   three different uri
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareConfigPublishTest032', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      try {
        let testUri1 = "datashareproxy://com.acts.dataShare.config.static/test1";
        let testUri2 = "datashareproxy://com.acts.dataShare.config.static/test2";
        let testUri3 = "datashareproxy://com.acts.dataShare.config.static/test3";
        let proxyData: dataShare.ProxyData[] = [];
        let g_proxyDataAllowTest1: dataShare.ProxyData = {
          uri: testUri1,
          value: true,
          allowList: []
        }
        let g_proxyDataAllowTest2: dataShare.ProxyData = {
          uri: testUri2,
          value: 253689.256,
          allowList: []
        }
        let g_proxyDataAllowTest3: dataShare.ProxyData = {
          uri: testUri3,
          value: "value is ok la",
          allowList: []
        }
        proxyData.push(g_proxyDataAllowTest1);
        proxyData.push(g_proxyDataAllowTest2);
        proxyData.push(g_proxyDataAllowTest3);
        await g_handle!.publish(proxyData, g_config).then((results: dataShare.DataProxyResult[]) => {
          console.log(TAG + "[DataShareProvider] dataShareConfigPublishTest032 success,results:" + JSON.stringify(results));
          results.forEach((result) => {
            let rtnCode = result.result;
            expect(rtnCode).assertEqual(0);
          });
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] publish failed.code =" + err.code + ",message=" + err.message);
          expect().assertFail();
        })
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareConfigPublishTest032 failed.code =" + err.code + ",message=" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareConfigPublishTest033
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_0030
     * @tc.desc   three same uri
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareConfigPublishTest033', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      try {
        let testUri1 = "datashareproxy://com.acts.dataShare.config.static/test1";
        let proxyData: dataShare.ProxyData[] = [];
        let g_proxyDataAllowTest1: dataShare.ProxyData = {
          uri: testUri1,
          value: "value is ok 1",
          allowList: []
        }
        let g_proxyDataAllowTest2: dataShare.ProxyData = {
          uri: testUri1,
          value: "value is ok 2",
          allowList: []
        }
        let g_proxyDataAllowTest3: dataShare.ProxyData = {
          uri: testUri1,
          value: "value is ok 3",
          allowList: []
        }
        proxyData.push(g_proxyDataAllowTest1);
        proxyData.push(g_proxyDataAllowTest2);
        proxyData.push(g_proxyDataAllowTest3);
        await g_handle!.publish(proxyData, g_config).then((results: dataShare.DataProxyResult[]) => {
          console.log(TAG + "[DataShareProvider] dataShareConfigPublishTest033 success,results:" + JSON.stringify(results));
          results.forEach((result) => {
            let rtnCode = result.result;
            expect(rtnCode).assertEqual(0);
          });
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] publish failed.code =" + err.code + ",message=" + err.message);
          expect().assertFail();
        })
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareConfigPublishTest033 failed.code =" + err.code + ",message=" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareConfigPublishTest034
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_0050
     * @tc.desc   contains error bundle name
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareConfigPublishTest034', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      try {
        let testUri1 = "datashareproxy://com.example.myappli234cation/test1";
        let testUri2 = "datashareproxy://com.acts.dataShare.config.static/test1";
        let proxyData: dataShare.ProxyData[] = [];
        let g_proxyDataAllowTest1: dataShare.ProxyData = {
          uri: testUri1,
          value: "value is ok la",
          allowList: []
        }
        let g_proxyDataAllowTest2: dataShare.ProxyData = {
          uri: testUri2,
          value: "value is ok la",
          allowList: []
        }
        proxyData.push(g_proxyDataAllowTest1);
        proxyData.push(g_proxyDataAllowTest2);
        await g_handle!.publish(proxyData, g_config).then((results: dataShare.DataProxyResult[]) => {
          console.log(TAG + "[DataShareProvider] dataShareConfigPublishTest034 success,results:" + JSON.stringify(results));
          expect(results[0].result).assertEqual(2);
          expect(results[1].result).assertEqual(0);
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] publish failed.code =" + err.code + ",message=" + err.message);
          expect().assertFail();
        })
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareConfigPublishTest034 failed.code =" + err.code + ",message=" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareConfigPublishTest035
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_0040
     * @tc.desc   error bundle name
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareConfigPublishTest035', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      try {
        let testUri1 = "datashareproxy://com.example.myappli234cation/test1";
        let proxyData: dataShare.ProxyData[] = [];
        let g_proxyDataAllowTest1: dataShare.ProxyData = {
          uri: testUri1,
          value: "value is ok la",
          allowList: []
        }
        proxyData.push(g_proxyDataAllowTest1);
        await g_handle!.publish(proxyData, g_config).then((results: dataShare.DataProxyResult[]) => {
          console.log(TAG + "[DataShareProvider] dataShareConfigPublishTest035 success,results:" + JSON.stringify(results));
          results.forEach((result) => {
            let rtnCode = result.result;
            expect(rtnCode).assertEqual(2);
          });
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] publish failed.code =" + err.code + ",message=" + err.message);
          expect().assertFail();
        })
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareConfigPublishTest035 failed.code =" + err.code + ",message=" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareConfigPublishTest036
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_1250
     * @tc.desc   error bundle name
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareConfigPublishTest036', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      try {
        let testUri1 = "datashareproxy://com.acts.dataShare.config.static/test1";
        let uris = [
          testUri1
        ]
        let proxyData: dataShare.ProxyData[] = [];
        let g_proxyDataAllowTest1: dataShare.ProxyData = {
          uri: testUri1,
          value: "value is ok la",
          allowList: []
        }
        proxyData.push(g_proxyDataAllowTest1);
        await g_handle!.publish(proxyData, g_config).then((results: dataShare.DataProxyResult[]) => {
          console.log(TAG + "[DataShareProvider] dataShareConfigPublishTest036 success,results:" + JSON.stringify(results));
          results.forEach((result) => {
            expect(result.result).assertEqual(0);
          });
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] publish failed.code =" + err.code + ",message=" + err.message);
          expect().assertFail();
        })

        await g_handle!.get(uris, g_config).then((results: dataShare.DataProxyGetResult[]) => {
          results.forEach((result) => {
            expect(result.result).assertEqual(0);
            expect(result.value).assertEqual("value is ok la");
          });
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] get failed.err =" + JSON.stringify(err));
          expect().assertFail();
        })

        //更新是value缺省，则不更新value的值
        let proxyData2: dataShare.ProxyData[] = [];
        let g_proxyDataAllowTest2: dataShare.ProxyData = {
          uri: testUri1,
          allowList: []
        }
        proxyData2.push(g_proxyDataAllowTest2);
        await g_handle!.publish(proxyData2, g_config).then((results: dataShare.DataProxyResult[]) => {
          console.log(TAG + "[DataShareProvider] dataShareConfigPublishTest036 success,results:" + JSON.stringify(results));
          results.forEach((result) => {
            expect(result.result).assertEqual(0);
          });
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] publish failed.code =" + err.code + ",message=" + err.message);
          expect().assertFail();
        })

        await g_handle!.get(uris, g_config).then((results: dataShare.DataProxyGetResult[]) => {
          results.forEach((result) => {
            expect(result.result).assertEqual(0);
            expect(result.value).assertEqual("value is ok la");
            console.log(TAG + "[DataShareProvider] dataShareConfigPublishTest036 g_handle!.get,results.value:" + result.value);
          });
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] get failed.err =" + JSON.stringify(err));
          expect().assertFail();
        })

      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareConfigPublishTest036 failed.code =" + err.code + ",message=" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareConfigPublishTest037
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_1260
     * @tc.desc   error bundle name
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareConfigPublishTest037', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      try {
        let testUri1 = "datashareproxy://com.acts.dataShare.config.static/test1";
        let uris = [
          testUri1
        ]
        let proxyData: dataShare.ProxyData[] = [];
        let g_proxyDataAllowTest1: dataShare.ProxyData = {
          uri: testUri1,
          value: "value is ok one",
          allowList: ["all"]
        }
        proxyData.push(g_proxyDataAllowTest1);
        await g_handle!.publish(proxyData, g_config).then((results: dataShare.DataProxyResult[]) => {
          console.log(TAG + "[DataShareProvider] dataShareConfigPublishTest037 publish1 success,results:" + JSON.stringify(results));
          results.forEach((result) => {
            expect(result.result).assertEqual(0);
          });
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] publish failed.code =" + err.code + ",message=" + err.message);
          expect().assertFail();
        })

        await g_handle!.get(uris, g_config).then((results: dataShare.DataProxyGetResult[]) => {
          console.log(TAG + "[DataShareProvider] dataShareConfigPublishTest037 get1 success,results:" + JSON.stringify(results));
          results.forEach((result) => {
            expect(result.result).assertEqual(0);
            expect(result.value).assertEqual("value is ok one");
          });
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] get failed.err =" + JSON.stringify(err));
          expect().assertFail();
        })

        //更新时allowlist缺省，则不更新allowlist的值
        let proxyData2: dataShare.ProxyData[] = [];
        let g_proxyDataAllowTest2: dataShare.ProxyData = {
          uri: testUri1,
          value: "value is update"
        }
        proxyData2.push(g_proxyDataAllowTest2);
        await g_handle!.publish(proxyData2, g_config).then((results: dataShare.DataProxyResult[]) => {
          console.log(TAG + "[DataShareProvider] dataShareConfigPublishTest037 publish2 success,results:" + JSON.stringify(results));
          results.forEach((result) => {
            expect(result.result).assertEqual(0);
          });
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] publish failed.code =" + err.code + ",message=" + err.message);
          expect().assertFail();
        })

        await g_handle!.get(uris, g_config).then((results: dataShare.DataProxyGetResult[]) => {
          console.log(TAG + "[DataShareProvider] dataShareConfigPublishTest037 get2 success,results:" + JSON.stringify(results));
          results.forEach((result) => {
            expect(result.result).assertEqual(0);
            expect(result.value).assertEqual("value is update");
            expect(result.allowList?.toString()).assertEqual("all");
          });
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] get failed.err =" + JSON.stringify(err));
          expect().assertFail();
        })
        await g_handle!.delete(uris, g_config);
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareConfigPublishTest037 failed,code =" + err.code + ",message=" + err.message);
        expect().assertFail();
      }
    });
  })
}