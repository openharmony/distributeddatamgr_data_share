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
import Utils from './Util.test';
import common from '@ohos.app.ability.common';
import abilityDelegatorRegistry from '@ohos.app.ability.abilityDelegatorRegistry';
import { AppStorage } from '@ohos.arkui.stateManagement'
import UIAbility from '@ohos.app.ability.UIAbility';
import dataShare from '@ohos.data.dataShare';
import { BusinessError } from '@ohos.base';

let g_handle: dataShare.DataProxyHandle | undefined;
let g_config: dataShare.DataProxyConfig = {
  type: dataShare.DataProxyType.SHARED_CONFIG
}
let testUriStr1 = "datashareproxy://com.example.myapplication.static/staticConfiguration1";
let testUriStr2 = "datashareproxy://com.example.myapplication.static/staticConfiguration2";
let testUriStr3 = "datashareproxy://com.example.myapplication.static/staticConfiguration3";
let testUriStr4 = "datashareproxy://com.example.myapplication.static/staticConfiguration4";
let testUriStr5 = "datashareproxy://com.example.myapplication.static/staticConfiguration5";
let testUriStr6 = "datashareproxy://com.example.myapplication.static/staticConfiguration6";
let testUriStr7 = "datashareproxy://com.example.myapplication.static/ ";
let testUriStr8 = "datashareproxy://com.example.myapplication.static/";
let testUriStr9 = "datashareproxy://com.example.myapplication.static/all";
let testUriStr10 = "datashareproxy://com.example.myapplication.static/*&^%$#";
let testUriStr11 = "datashareproxy://com.acts.ohos.data.dataproxy/staticConfiguration7";
let testUriStr12 = "datashareproxy://com.example.myapplication.static/staticConfiguration12datashareproxy://com.example.myapplication/staticConfiguration12datashareproxy://com.example.myapplication/staticConfiguration12datashareproxy://com.example.myapplication/staticConfigur";
//uri长度超过257
let testUriStr13 = "datashareproxy://com.example.myapplication.static/staticConfiguration12datashareproxyqwecom.example.myapplication.static/staticConfiguration12datashareproxyqwecom.example.myapplication.static/staticConfiguration12datashareproxyqwecom.example.myapplication.static/staticConfiguration129";
let testUriStr14 = "datashareproxy://com.example.myapplication.static/staticConfiguration14";
let testUriStr15 = "datashareproxy://com.example.myapplication.static/staticConfiguration15";
let testUriStr16 = "datashareproxy://com.example.myapplication.static/staticConfiguration16";
let testUriStr17 = "datashareproxy://com.example.myapplication.static/staticConfiguration17";
let testUriStr18 = "datashareproxy://com.example.myapplication.static/staticConfiguration18";

let TAG = "ttt ";

export default function dataShareStaticConfigGetTest() {
  describe("dataShareStaticConfigGetTest", () => {
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
    })

    afterAll(async () => {
      console.info(TAG + "afterAll---------------------------------");
      try {
        let uris = [
          testUriStr1,
          testUriStr2,
          testUriStr3,
          testUriStr4,
          testUriStr5,
          testUriStr6,
          testUriStr7,
          testUriStr8,
          testUriStr9,
          testUriStr10,
          testUriStr11,
          testUriStr12,
          testUriStr14,
          testUriStr15,
          testUriStr16,
          testUriStr17,
          testUriStr18
        ]
        await g_handle!.delete(uris, g_config).then((results: dataShare.DataProxyResult[]) => {
          results.forEach((result) => {
            console.info(TAG + `[DataShareProvider] URI:${result.uri},Result:${result.result}`);
          });
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] delete failed,err =" + JSON.stringify(err));
          expect().assertFail();
        })
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareConfigDeleteTest001 failed.code =" + err.code + ",message=" + err.message);
        expect().assertFail();
      }
    })

    /**
     * @tc.name   dataShareStaticConfigGetTest001
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_1250
     * @tc.desc   uri repeat
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareStaticConfigGetTest001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      let uris = [
        testUriStr1
      ]
      try {
        await g_handle!.get(uris, g_config).then((results: dataShare.DataProxyGetResult[]) => {
          results.forEach((result) => {
            console.info(TAG + `[DataShareProvider] URI:${result.uri},Value:${result.value},Result:${result.result},AllowList:${result.allowList}`);
            expect(result.result).assertEqual(0);
            expect(result.uri).assertEqual(testUriStr1);
            expect(result.value).assertEqual("niuma");
          });
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] get failed.err =" + JSON.stringify(err));
          expect().assertFail();
        })
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareStaticConfigGetTest001 failed,code =" + err.code + ",message=" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareStaticConfigGetTest002
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_1260
     * @tc.desc   uri is number
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareStaticConfigGetTest002', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      let uris = [
        testUriStr2
      ]
      try {
        await g_handle!.get(uris, g_config).then((results: dataShare.DataProxyGetResult[]) => {
          results.forEach((result) => {
            console.info(TAG + `[DataShareProvider] URI:${result.uri},Value:${result.value},Result:${result.result},AllowList:${result.allowList}`);
            expect(result.result).assertEqual(0);
            expect(result.uri).assertEqual(testUriStr2);
            expect(result.value).assertEqual("");
          });
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] get failed.err =" + JSON.stringify(err));
          expect().assertFail();
        })
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareStaticConfigGetTest002 failed,code =" + err.code + ",message=" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareStaticConfigGetTest003
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_1070
     * @tc.desc   uri is chinese
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareStaticConfigGetTest003', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      let uris = [
        testUriStr3
      ]
      try {
        await g_handle!.get(uris, g_config).then((results: dataShare.DataProxyGetResult[]) => {
          results.forEach((result) => {
            console.info(TAG + `[DataShareProvider] URI:${result.uri},Value:${result.value},Result:${result.result},AllowList:${result.allowList}`);
            expect(result.result).assertEqual(0);
            expect(result.uri).assertEqual(testUriStr3);
            expect(result.value).assertEqual("恶魔");
          });
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] get failed.err =" + JSON.stringify(err));
          expect().assertFail();
        })
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareStaticConfigGetTest003 failed,code =" + err.code + ",message=" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareStaticConfigGetTest004
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_1080
     * @tc.desc   value is false
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareStaticConfigGetTest004', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      let uris = [
        testUriStr4
      ]
      try {
        await g_handle!.get(uris, g_config).then((results: dataShare.DataProxyGetResult[]) => {
          results.forEach((result) => {
            console.info(TAG + `[DataShareProvider] URI:${result.uri},Value:${result.value},Result:${result.result},AllowList:${result.allowList}`);
            expect(result.result).assertEqual(0);
            expect(result.uri).assertEqual(testUriStr4);
            expect(result.value).assertEqual("");
          });
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] get failed.err =" + JSON.stringify(err));
          expect().assertFail();
        })
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareStaticConfigGetTest004 failed,code =" + err.code + ",message=" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareStaticConfigGetTest005
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_1090
     * @tc.desc   value is true
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareStaticConfigGetTest005', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      let uris = [
        testUriStr5
      ]
      try {
        await g_handle!.get(uris, g_config).then((results: dataShare.DataProxyGetResult[]) => {
          results.forEach((result) => {
            console.info(TAG + `[DataShareProvider] URI:${result.uri},Value:${result.value},Result:${result.result},AllowList:${result.allowList}`);
            expect(result.result).assertEqual(0);
            expect(result.uri).assertEqual(testUriStr5);
            expect(result.value).assertEqual("");
          });
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] get failed.err =" + JSON.stringify(err));
          expect().assertFail();
        })
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareStaticConfigGetTest005 failed,code =" + err.code + ",message=" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareStaticConfigGetTest006
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_1100
     * @tc.desc   value is special character
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareStaticConfigGetTest006', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      let uris = [
        testUriStr6
      ]
      try {
        await g_handle!.get(uris, g_config).then((results: dataShare.DataProxyGetResult[]) => {
          results.forEach((result) => {
            console.info(TAG + `[DataShareProvider] URI:${result.uri},Value:${result.value},Result:${result.result},AllowList:${result.allowList}`);
            expect(result.result).assertEqual(0);
            expect(result.uri).assertEqual(testUriStr6);
            expect(result.value).assertEqual("@#$%^&*");
          });
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] get failed.err =" + JSON.stringify(err));
          expect().assertFail();
        })
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareStaticConfigGetTest006 failed,code =" + err.code + ",message=" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareStaticConfigGetTest007
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_1110
     * @tc.desc   uri path is space
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareStaticConfigGetTest007', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      let uris = [
        testUriStr7
      ]
      try {
        await g_handle!.get(uris, g_config).then((results: dataShare.DataProxyGetResult[]) => {
          results.forEach((result) => {
            console.info(TAG + `[DataShareProvider] URI:${result.uri},Value:${result.value},Result:${result.result},AllowList:${result.allowList}`);
            expect(result.result).assertEqual(0);
            expect(result.uri).assertEqual(testUriStr7);
            expect(result.value).assertEqual("staticSpace");
          });
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] get failed.err =" + JSON.stringify(err));
          expect().assertFail();
        })
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareStaticConfigGetTest007 failed,code =" + err.code + ",message=" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareStaticConfigGetTest008
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_1120
     * @tc.desc   uri path is null
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareStaticConfigGetTest008', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      let uris = [
        testUriStr8
      ]
      try {
        await g_handle!.get(uris, g_config).then((results: dataShare.DataProxyGetResult[]) => {
          results.forEach((result) => {
            console.info(TAG + `[DataShareProvider] URI:${result.uri},Value:${result.value},Result:${result.result},AllowList:${result.allowList}`);
            expect(result.result).assertEqual(0);
            expect(result.uri).assertEqual(testUriStr8);
            expect(result.value).assertEqual("staticNull");
          });
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] get failed.err =" + JSON.stringify(err));
          expect().assertFail();
        })
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareStaticConfigGetTest008 failed,code =" + err.code + ",message=" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareStaticConfigGetTest009
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_1130
     * @tc.desc   uri path is all
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareStaticConfigGetTest009', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      let uris = [
        testUriStr9
      ]
      try {
        await g_handle!.get(uris, g_config).then((results: dataShare.DataProxyGetResult[]) => {
          results.forEach((result) => {
            console.info(TAG + `[DataShareProvider] URI:${result.uri},Value:${result.value},Result:${result.result},AllowList:${result.allowList}`);
            expect(result.result).assertEqual(0);
            expect(result.uri).assertEqual(testUriStr9);
            expect(result.value).assertEqual("staticall");
          });
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] get failed.err =" + JSON.stringify(err));
          expect().assertFail();
        })
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareStaticConfigGetTest009 failed,code =" + err.code + ",message=" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareStaticConfigGetTest010
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_1140
     * @tc.desc   uri path is special character
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareStaticConfigGetTest010', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      let uris = [
        testUriStr10
      ]
      try {
        await g_handle!.get(uris, g_config).then((results: dataShare.DataProxyGetResult[]) => {
          results.forEach((result) => {
            console.info(TAG + `[DataShareProvider] URI:${result.uri},Value:${result.value},Result:${result.result},AllowList:${result.allowList}`);
            expect(result.result).assertEqual(0);
            expect(result.uri).assertEqual(testUriStr10);
            expect(result.value).assertEqual("staticChacrater");
          });
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] get failed.err =" + JSON.stringify(err));
          expect().assertFail();
        })
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareStaticConfigGetTest010 failed,code =" + err.code + ",message=" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareStaticConfigGetTest011
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_1150
     * @tc.desc   uri bundle name is error
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareStaticConfigGetTest011', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      let uris = [
        testUriStr11
      ]
      try {
        await g_handle!.get(uris, g_config).then((results: dataShare.DataProxyGetResult[]) => {
          results.forEach((result) => {
            console.info(TAG + `[DataShareProvider] URI:${result.uri},Value:${result.value},Result:${result.result},AllowList:${result.allowList}`)
            expect(result.result).assertEqual(1);
            expect(result.uri).assertEqual(testUriStr11);
            expect(result.value).assertEqual(undefined);
          });
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] get failed.err =" + JSON.stringify(err));
          expect().assertFail();
        })
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareStaticConfigGetTest011 failed,code =" + err.code + ",message=" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareStaticConfigGetTest012
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_1160
     * @tc.desc   uri can delete
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareStaticConfigGetTest012', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      let uris = [
        testUriStr1
      ]
      try {
        await g_handle!.delete(uris, g_config).then((results: dataShare.DataProxyResult[]) => {
          results.forEach((result) => {
            console.info(TAG + `[DataShareProvider] URI:${result.uri},Result:${result.result}`);
            expect(result.result).assertEqual(0);
          });
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] delete failed.err =" + JSON.stringify(err));
          expect().assertFail();
        })

        await g_handle!.get(uris, g_config).then((results: dataShare.DataProxyGetResult[]) => {
          results.forEach((result) => {
            console.info(TAG + `[DataShareProvider] URI:${result.uri},Value:${result.value},Result:${result.result},AllowList:${result.allowList}`)
            expect(result.result).assertEqual(1);
          });
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] get failed.err =" + JSON.stringify(err));
          expect().assertFail();
        })
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareStaticConfigGetTest012 failed,code =" + err.code + ",message=" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareStaticConfigGetTest013
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_1170
     * @tc.desc   uri length is 256
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareStaticConfigGetTest013', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      let uris = [
        testUriStr12
      ]
      try {
        await g_handle!.get(uris, g_config).then((results: dataShare.DataProxyGetResult[]) => {
          results.forEach((result) => {
            console.info(TAG + `[DataShareProvider] URI:${result.uri},Value:${result.value},Result:${result.result},AllowList:${result.allowList}`)
            expect(result.result).assertEqual(0);
            expect(result.uri).assertEqual(testUriStr12);
            expect(result.value).assertEqual("statictest12");
          });
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] get failed.err =" + JSON.stringify(err));
          expect().assertFail();
        })
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareStaticConfigGetTest013 failed,code =" + err.code + ",message=" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareStaticConfigGetTest014
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_1180
     * @tc.desc   uri length is 257
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareStaticConfigGetTest014', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      let uris = [
        testUriStr13
      ]
      try {
        await g_handle!.get(uris, g_config).then((results: dataShare.DataProxyGetResult[]) => {
          expect().assertFail();
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] get failed.err =" + JSON.stringify(err));
          expect().assertFail();
        })
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareStaticConfigGetTest014 failed,code =" + err.code + ",message=" + err.message);
        expect(err.code).assertEqual(15700014);
      }
    });

    /**
     * @tc.name   dataShareStaticConfigGetTest015
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_1190
     * @tc.desc   static config can be published
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareStaticConfigGetTest015', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      let uris = [
        testUriStr3
      ]
      try {
        await g_handle!.get(uris, g_config).then((results: dataShare.DataProxyGetResult[]) => {
          results.forEach((result) => {
            console.info(TAG + `[DataShareProvider] URI:${result.uri},Value:${result.value},Result:${result.result},AllowList:${result.allowList}`)
            expect(result.result).assertEqual(0);
            expect(result.uri).assertEqual(testUriStr3);
            expect(result.value).assertEqual("恶魔");
          });
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] get failed.err =" + JSON.stringify(err));
          expect().assertFail();
        })
        let proxyData: dataShare.ProxyData[] = [];
        let g_proxyDataAllowTest1: dataShare.ProxyData = {
          uri: testUriStr3,
          value: "value is ok",
          allowList: []
        }
        proxyData.push(g_proxyDataAllowTest1);
        await g_handle!.publish(proxyData, g_config).then((results: dataShare.DataProxyResult[]) => {
          console.log(TAG + "[DataShareProvider] dataShareStaticConfigGetTest015 publish success,results:" + JSON.stringify(results));
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] dataShareStaticConfigGetTest015 publish failed.code =" + err.code + ",message=" + err.message);
          expect().assertFail();
        })
        await g_handle!.get(uris, g_config).then((results: dataShare.DataProxyGetResult[]) => {
          results.forEach((result) => {
            console.info(TAG + `[DataShareProvider] URI:${result.uri},Value:${result.value},Result:${result.result},AllowList:${result.allowList}`)
            expect(result.result).assertEqual(0);
            expect(result.uri).assertEqual(testUriStr3);
            expect(result.value).assertEqual("value is ok");
          });
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] dataShareStaticConfigGetTest015 get failed.err =" + JSON.stringify(err));
          expect().assertFail();
        })
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareStaticConfigGetTest015 failed,code =" + err.code + ",message=" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareStaticConfigGetTest016
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_1200
     * @tc.desc   value Equal 4096
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareStaticConfigGetTest016', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      let uris = [
        testUriStr14
      ]
      try {
        await g_handle!.get(uris, g_config).then((results: dataShare.DataProxyGetResult[]) => {
          results.forEach((result) => {
            console.info(TAG + `[DataShareProvider] URI:${result.uri},Value:${result.value},Result:${result.result},AllowList:${result.allowList}`)
            expect(result.result).assertEqual(0);
            expect(result.uri).assertEqual(testUriStr14);
          });
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] get failed.err =" + JSON.stringify(err));
          expect().assertFail();
        })
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareStaticConfigGetTest016 failed,code =" + err.code + ",message=" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareStaticConfigGetTest017
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_1210
     * @tc.desc   value more than 4096
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareStaticConfigGetTest017', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      let uris = [
        testUriStr15
      ]
      try {
        await g_handle!.get(uris, g_config).then((results: dataShare.DataProxyGetResult[]) => {
          results.forEach((result) => {
            console.info(TAG + `[DataShareProvider] URI:${result.uri},Value:${result.value},Result:${result.result},AllowList:${result.allowList}`);
            console.log(TAG + "[DataShareProvider] get value length is:" + result.value?.toString().length);
            expect(result.result).assertEqual(1);
          });
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] get failed.err =" + JSON.stringify(err));
          expect().assertFail();
        })
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareStaticConfigGetTest017 failed,code =" + err.code + ",message=" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareStaticConfigGetTest018
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_1220
     * @tc.desc   allowList is 128
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareStaticConfigGetTest018', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      let uris = [
        testUriStr16
      ]
      try {
        await g_handle!.get(uris, g_config).then((results: dataShare.DataProxyGetResult[]) => {
          results.forEach((result) => {
            console.info(TAG + `[DataShareProvider] URI:${result.uri},Value:${result.value},Result:${result.result},AllowList:${result.allowList}`);
            console.log(TAG + "[DataShareProvider] get allowList length is:" + result.allowList?.length);
            expect(result.result).assertEqual(0);
          });
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] get failed.err =" + JSON.stringify(err));
          expect().assertFail();
        })
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareStaticConfigGetTest018 failed,code =" + err.code + ",message=" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareStaticConfigGetTest019
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_1230
     * @tc.desc   allowList is 129
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareStaticConfigGetTest019', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      let uris = [
        testUriStr17
      ]
      try {
        await g_handle!.get(uris, g_config).then((results: dataShare.DataProxyGetResult[]) => {
          results.forEach((result) => {
            console.info(TAG + `[DataShareProvider] URI:${result.uri},Value:${result.value},Result:${result.result},AllowList:${result.allowList}`);
            console.log(TAG + "[DataShareProvider] get allowList length is:" + result.allowList?.length);
            // expect(result.result).assertEqual(dataShare.DataProxyErrorCode.SUCCESS);
            expect(result.result).assertEqual(0);
            expect(result.allowList?.length).assertEqual(0);
          });
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] get failed.err =" + JSON.stringify(err));
          expect().assertFail();
        })
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareStaticConfigGetTest019 failed,code =" + err.code + ",message=" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareStaticConfigGetTest020
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_1240
     * @tc.desc   [allowList] more than 256
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareStaticConfigGetTest020', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      let uris = [
        testUriStr18
      ]
      try {
        await g_handle!.get(uris, g_config).then((results: dataShare.DataProxyGetResult[]) => {
          results.forEach((result) => {
            console.info(TAG + `[DataShareProvider] URI:${result.uri},Value:${result.value},Result:${result.result},AllowList:${result.allowList}`);
            console.log(TAG + "[DataShareProvider] get allowList length is:" + result.allowList?.length);
            expect(result.result).assertEqual(0);
            expect(result.allowList?.length).assertEqual(256);
          });
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] get failed.err =" + JSON.stringify(err));
          expect().assertFail();
        })
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareStaticConfigGetTest020 failed,code =" + err.code + ",message=" + err.message);
        expect().assertFail();
      }
    });
  })
}