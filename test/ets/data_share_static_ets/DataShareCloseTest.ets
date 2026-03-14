/*
* Copyright (c) 2026 Huawei Device Co., Ltd.
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
 
import { describe, it, expect, TestType, Size, Level, beforeAll, afterAll} from "../../../hypium/index";
import hilog from '@ohos.hilog'
import Utils from './Util.test';
import common from "@ohos.app.ability.common";
import { BusinessError } from '@ohos.base';
import { ValuesBucket } from '@ohos.data.ValuesBucket';
import dataShare from '@ohos.data.dataShare';
import dataSharePredicates from '@ohos.data.dataSharePredicates';
import DataShareResultSet from '@ohos.data.DataShareResultSet';
import { AppStorage } from '@ohos.arkui.stateManagement';
 
let domain: int = 0x0000; //日志标识,0x0000作为测试框架的业务标识
let tag: string = 'testTag'; //日志标识字符串,作为tag标识当前runner类下的测试行为
 
let dseUri = "datashare:///com.acts.datasharetest.DataShare";
 
let context: common.UIAbilityContext;
let datashareHelper: dataShare.DataShareHelper | undefined = undefined;
let dsHelper: dataShare.DataShareHelper;
let TAG = "[closeTestClient]";
 
export default function dataShareCloseTest() {
  describe('dataShareCloseTest', () => {
    beforeAll(async () => {
      context = AppStorage.get<common.UIAbilityContext>("context") as common.UIAbilityContext;
      try {
        datashareHelper = await dataShare.createDataShareHelper(context, dseUri);
        await Utils.msSleep(2000);
        dsHelper = datashareHelper as dataShare.DataShareHelper;
        let ret = 0;
        for (let i = 1; i <= 10; i++) {
          let vb: ValuesBucket = {
            "name": "Lisi" + i,
            "age": i as long
          }
          ret = await dsHelper.insert(dseUri, vb);
        }
        console.log(TAG + " insert count is " + ret);
      } catch(err) {
        let code = (err as BusinessError).code;
        let message = (err as BusinessError).message;
        console.error(`beforeAll error: code: ${code}, message: ${message} `);
        expect(true).assertFail();
      }
      hilog.info(domain, tag, '%{public}s', 'beforeAll end');
    })
 
    afterAll(async () => {
      try {
        dsHelper = await dataShare.createDataShareHelper(context, dseUri) as dataShare.DataShareHelper;
        let da = new dataSharePredicates.DataSharePredicates();
        await dsHelper.delete(dseUri, da);
        await dsHelper.close();
      } catch (err) {
        let code = (err as BusinessError).code;
        let message = (err as BusinessError).message;
        hilog.info(domain, tag, '%{public}s', `[ttt] afterAll error: code: ${code}, message: ${message}`);
        expect(true).assertFail();
      }
    })
 
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0210
     * @tc.name      : dataShareCloseTest001
     * @tc.desc      : close first one
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    it('dataShareCloseTest001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
      console.log(TAG + "dataShareCloseTest001 start");
      if (datashareHelper != undefined) {
        try {
          await dsHelper.close();
          console.error(`[ttt] close success `);
        } catch (err) {
          let code = (err as BusinessError).code;
          let message = (err as BusinessError).message;
          console.error(`[ttt] close error: code: ${code}, message: ${message} `);
          expect(true).assertFail();
        }
      } else {
        console.log(TAG + "datashareHelper is undefined");
        expect(true).assertFail();
      }
      done();
      console.log(TAG + "dataShareCloseTest001 end");
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0220
     * @tc.name      : dataShareCloseTest002
     * @tc.desc      : insert after close
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    it('dataShareCloseTest002', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
      console.log(TAG + "dataShareCloseTest002 start");
      if (datashareHelper != undefined) {
        try {
          await dsHelper.close();
          let vb: ValuesBucket = {
            "name": "Lisi",
            "age": 22 as long
          }
          dsHelper.insert(dseUri, vb).then((data: int) => {
            expect(true).assertFail();
          }).catch((error) => {
            let err = error as BusinessError;
            console.error(`[ttt] insert error: code: ${err.code}, message: ${err.message} `);
            expect(err.code == 15700013).assertTrue();
          });
        } catch (err) {
          let code = (err as BusinessError).code;
          let message = (err as BusinessError).message;
          console.error(`[ttt] insert error: code: ${code}, message: ${message} `);
          expect(true).assertFail();
        }
      } else {
        console.log(TAG + "datashareHelper is undefined");
        expect(true).assertFail();
      }
      done();
      console.log(TAG + "dataShareCloseTest002 end");
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0230
     * @tc.name      : dataShareCloseTest003
     * @tc.desc      : update after close
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    it('dataShareCloseTest003', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
      console.log(TAG + "dataShareCloseTest003 start");
      if (datashareHelper != undefined) {
        try {
          await dsHelper.close();
          let vb: ValuesBucket = {
            "name": "Lisi",
            "age": 22 as long
          }
          let da = new dataSharePredicates.DataSharePredicates();
          dsHelper.update(dseUri, da, vb).then((data: int) => {
            console.info("[ttt] update succeed, data : " + data);
            expect(true).assertFail();
          }).catch((error) => {
            let err = error as BusinessError;
            console.error(`[ttt] update error: code: ${err.code}, message: ${err.message} `);
            expect(err.code == 15700013).assertTrue();
          });
 
        } catch (err) {
          let code = (err as BusinessError).code;
          let message = (err as BusinessError).message;
          console.error(`[ttt] insert error: code: ${code}, message: ${message} `);
          expect(true).assertFail();
        }
      } else {
        console.log(TAG + "datashareHelper is undefined");
        expect(true).assertFail();
      }
      done();
      console.log(TAG + "dataShareCloseTest003 end");
    })
 
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0240
     * @tc.name      : dataShareCloseTest004
     * @tc.desc      : query after close
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    it('dataShareCloseTest004', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
      console.log(TAG + "dataShareCloseTest004 start");
      if (datashareHelper != undefined) {
        try {
          await dsHelper.close();
          let da = new dataSharePredicates.DataSharePredicates();
          let columns = ["*"];
          dsHelper.query(dseUri, da, columns).then((data: DataShareResultSet) => {
            if (data == undefined) {
              console.info("query failed");
            } else {
              console.info("query succeed, data : " + data);
              expect(true).assertFail();
            }
          }).catch((error) => {
            let err = error as BusinessError;
            console.error(`query error: code: ${err.code}, message: ${err.message} `);
            expect(err.code == 15700013).assertTrue();
          });
 
        } catch (err) {
          let code = (err as BusinessError).code;
          let message = (err as BusinessError).message;
          console.error(`[ttt] insert error: code: ${code}, message: ${message} `);
          expect(true).assertFail();
        }
      } else {
        console.log(TAG + "datashareHelper is undefined");
        expect(true).assertFail();
      }
      done();
      console.log(TAG + "dataShareCloseTest004 end");
    })
 
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0250
     * @tc.name      : dataShareCloseTest005
     * @tc.desc      : delete after close
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    it('dataShareCloseTest005', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
      console.log(TAG + "dataShareCloseTest005 start");
      if (datashareHelper != undefined) {
        try {
          await dsHelper.close();
          let da = new dataSharePredicates.DataSharePredicates();
          dsHelper.delete(dseUri, da).then((data: int) => {
            console.info("[ttt] delete succeed, data : " + data);
            expect(true).assertFail();
          }).catch((error) => {
            let err = error as BusinessError;
            console.error(`[ttt] delete error: code: ${err.code}, message: ${err.message} `);
            expect(err.code == 15700013).assertTrue();
          });
 
        } catch (err) {
          let code = (err as BusinessError).code;
          let message = (err as BusinessError).message;
          console.error(`[ttt] insert error: code: ${code}, message: ${message} `);
          expect(true).assertFail();
        }
      } else {
        console.log(TAG + "datashareHelper is undefined");
        expect(true).assertFail();
      }
      done();
      console.log(TAG + "dataShareCloseTest005 end");
    })
  })
}