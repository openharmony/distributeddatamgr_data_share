/*
* Copyright (c) 2025-2026 Huawei Device Co., Ltd.
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
let context: common.UIAbilityContext;
let testUri1 = "datashareproxy://com.acts.dataShare.config.static/test1";
let TAG = "ttt ";

export default function dataShareConfigDeleteTest() {
  describe('dataShareConfigDeleteTest', (): void => {
    beforeAll(async () => {
      console.info(TAG + "beforeAll---------------------------------");
      let abilityDelegator: abilityDelegatorRegistry.AbilityDelegator = abilityDelegatorRegistry.getAbilityDelegator()
      abilityDelegator.addAbilityMonitor({
        abilityName: "EntryAbility",
        moduleName: "entry",
        onAbilityCreate: (ability: UIAbility) : void => {
          context = ability.context;
          AppStorage.setOrCreate("context", ability.context);
          console.log('[ttt] ability.context.databaseDir:' + context.databaseDir);
          console.log('[ttt] ability.context.area:' + context.area);
          console.log('onAbilityCreate end');
        },
      }, (err : BusinessError | null) : void => {
        if (err && err.code != 0 ) {
          console.log('-----' + err.code);
        }
        console.log('BusinessError  end');
      });
      await Utils.msSleep(2000);
      abilityDelegator.executeShellCommand("aa start -a EntryAbility -b com.acts.datasharestabilityclient");
      await Utils.msSleep(2000);
      await dataShare.createDataProxyHandle().then((handle: dataShare.DataProxyHandle) => {
        if (handle == null || handle == undefined) {
          console.log(TAG + "[DataShareProvider] createDataProxyHandle failed, handle is null");
        } else {
          g_handle = handle;
          console.log(TAG + "[DataShareProvider] createDataProxyHandle success");
        }
      }).catch((err: Error) => {
        console.error(TAG + `[DataShareProvider] createDataProxyHandle failed: code: ${err.code}, message: ${err.message} `);
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
        console.error(TAG + "[DataShareProvider] beforeEach publish failed, code =" + err.code + ", message =" + err.message);
        expect().assertFail();
      }
    })

    afterEach(async () => {
      console.info(TAG + "afterEach---------------------------------");
      let uris = [
        testUri1
      ]
      try {
        await g_handle!.delete(uris, g_config).then((results: dataShare.DataProxyResult[]) => {
          console.log("[ttt] [DataShareProvider] afterEach delete success, results: " + JSON.stringify(results));
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] delete failed. code =" + err.code + ", message =" + err.message);
          expect().assertFail();
        })
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] afterEach delete failed. code =" + err.code + ", message =" + err.message);
        expect().assertFail();
      }
    })
    afterAll(async () => {
      console.info(TAG + "afterAll---------------------------------");
    })

    /**
     * @tc.name   dataShareConfigDeleteTest003
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_0570
     * @tc.desc   single uri
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareConfigDeleteTest003', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      let tmpStr = "123456".repeat(34);
      let testUri = "datashareproxy://com.acts.dataShare.config.static/test11234" + tmpStr + "9";
      let uris = [
        testUri
      ]
      try {
        await g_handle!.delete(uris, g_config).then((results: dataShare.DataProxyResult[]) => {
          expect(results.length > 0).assertTrue();
          results.forEach((result) => {
            console.info(TAG + `[DataShareProvider] URI: ${result.uri}, Result: ${result.result}`);
            expect().assertFail();
          });
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] dataShareConfigDeleteTest003 delete failed, err =" + JSON.stringify(err));
          expect().assertFail();
        })
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareConfigDeleteTest003 failed. code =" + err.code + ",message=" + err.message);
        expect(err.code).assertEqual(15700014);
      }
    });

    /**
     * @tc.name   dataShareConfigDeleteTest004
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_0580
     * @tc.desc   single uri
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareConfigDeleteTest004', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      let uris: Array<string> = [];
      try {
        await g_handle!.delete(uris, g_config).then((results: dataShare.DataProxyResult[]) => {
          results.forEach((result) => {
            console.info(TAG + `[DataShareProvider] URI: ${result.uri}, Result: ${result.result}`);
            expect().assertFail();
          });
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] delete failed,err =" + JSON.stringify(err));
          expect().assertFail();
        })
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareConfigDeleteTest004 failed.code =" + err.code + ",message=" + err.message);
        expect(err.code).assertEqual(401);
      }
    });

    /**
     * @tc.name   dataShareConfigDeleteTest005
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_0590
     * @tc.desc   single uri
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareConfigDeleteTest005', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      let testUri = " ";
      let uris = [
        testUri
      ]
      try {
        await g_handle!.delete(uris, g_config).then((results: dataShare.DataProxyResult[]) => {
          results.forEach((result) => {
            console.info(TAG + `[DataShareProvider] URI: ${result.uri}, Result: ${result.result}`);
            expect(result.result).assertEqual(2);
          });
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] delete failed,err =" + JSON.stringify(err));
          expect().assertFail();
        })
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareConfigDeleteTest005 failed.code =" + err.code + ",message=" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareConfigDeleteTest006
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_0600
     * @tc.desc   single uri
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareConfigDeleteTest006', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      let testUri = "@#$%^&*";
      let uris = [
        testUri
      ]
      try {
        await g_handle!.delete(uris, g_config).then((results: dataShare.DataProxyResult[]) => {
          results.forEach((result) => {
            console.info(TAG + `[DataShareProvider] URI: ${result.uri}, Result: ${result.result}`);
            expect(result.result).assertEqual(2);
            expect(result.uri).assertEqual(testUri);
          });
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] delete failed,err =" + JSON.stringify(err));
          expect().assertFail();
        })
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareConfigDeleteTest006 failed.code =" + err.code + ",message=" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareConfigDeleteTest009
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_0630
     * @tc.desc   single uri
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareConfigDeleteTest009', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      let testUri = "datashareproxy://com.acts.dataShare.config.static/";
      let uris = [
        testUri
      ]
      try {
        await g_handle!.delete(uris, g_config).then((results: dataShare.DataProxyResult[]) => {
          results.forEach((result) => {
            console.info(TAG + `[DataShareProvider] URI:${result.uri},Result:${result.result}`);
            expect(result.result).assertEqual(1);
            expect(result.uri).assertEqual(testUri);
          });
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] delete failed,err =" + JSON.stringify(err));
          expect().assertFail();
        })
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareConfigDeleteTest009 failed.code =" + err.code + ",message=" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareConfigDeleteTest010
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_0640
     * @tc.desc   single uri
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareConfigDeleteTest010', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      let testUri = "datashareproxy://com.acts.dataShare.config.static/@#$%^&*";
      let uris = [
        testUri
      ]
      try {
        await g_handle!.delete(uris, g_config).then((results: dataShare.DataProxyResult[]) => {
          results.forEach((result) => {
            console.info(TAG + `[DataShareProvider] URI:${result.uri},Result:${result.result}`);
            expect(result.result).assertEqual(1);
            expect(result.uri).assertEqual(testUri);
          });
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] delete failed,err =" + JSON.stringify(err));
          expect().assertFail();
        })
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareConfigDeleteTest010 failed.code =" + err.code + ",message=" + err.message);
        expect().assertFail();
      }
    });

    /**
     * @tc.name   dataShareConfigDeleteTest011
     * @tc.number SUB_DistributedData_DataShare_SDK_ShareConfig_0650
     * @tc.desc   single uri
     * @tc.type   FUNCTION
     * @tc.size   MEDIUMTEST
     * @tc.level  LEVEL1
     */
    it('dataShareConfigDeleteTest011', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      let testUri = "datashareproxy://com.acts.dataShare.config.static/路径";
      let uris = [
        testUri
      ]
      try {
        await g_handle!.delete(uris, g_config).then((results: dataShare.DataProxyResult[]) => {
          results.forEach((result) => {
            console.info(TAG + `[DataShareProvider] URI:${result.uri},Result:${result.result}`);
            expect(result.result).assertEqual(1);
            expect(result.uri).assertEqual(testUri);
          });
        }).catch((err: Error) => {
          console.error(TAG + "[DataShareProvider] delete failed,err =" + JSON.stringify(err));
          expect().assertFail();
        })
      } catch (err) {
        err = err as BusinessError;
        console.error(TAG + "[DataShareProvider] dataShareConfigDeleteTest011 failed.code =" + err.code + ",message=" + err.message);
        expect().assertFail();
      }
    });
  })
}