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
 
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect, Level, Size, TestType } from "../../../hypium/index";
import hilog from '@ohos.hilog';
import Utils from './Util.test';
import dataShare from '@ohos.data.dataShare'
import common from '@ohos.app.ability.common';
import { ValuesBucket } from '@ohos.data.ValuesBucket';
import { BusinessError } from '@ohos.base';
import { AppStorage } from '@ohos.arkui.stateManagement';
 
let domain: int = 0x0000; //日志标识,0x0000作为测试框架的业务标识
let tag: string = 'testTag'; //日志标识字符串,作为tag标识当前runner类下的测试行为
 
let dseUri = "datashare:///com.acts.datasharetest.DataShare";
 
let context: common.UIAbilityContext;
let dataShareHelper: dataShare.DataShareHelper | undefined = undefined;
let dsHelper: dataShare.DataShareHelper;
let TAG = "[notifyChangeClient]";
 
export default function notifyChangeTest() {
  describe('notifyChangeTest', () => {
    beforeAll(async (done: () => void) => {
      context = AppStorage.get<common.UIAbilityContext>("context") as common.UIAbilityContext;
      try {
        dataShareHelper = await dataShare.createDataShareHelper(context, dseUri);
        await Utils.msSleep(2000);
        dsHelper = dataShareHelper as dataShare.DataShareHelper;
        let ret = 0;
        for (let i = 1; i <= 10; i++) {
          let vb: ValuesBucket = {
            "name": "ZhangSan" + i,
            "age": i as long
          }
          ret = await dsHelper.insert(dseUri, vb);
        }
        console.log(TAG + " insert count is " + ret);
      } catch(err: BusinessError) {
        let code = (err as BusinessError).code;
        let message = (err as BusinessError).message;
        console.error(`beforeAll error: code: ${code}, message: ${message} `);
        expect(true).assertFail();
      }
      hilog.info(domain, tag, '%{public}s', 'beforeAll end');
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0210
     * @tc.name      : notifyChangeTest001
     * @tc.desc      : close first one
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    it('notifyChangeTest001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
      console.log(TAG + "notifyChangeTest001 start");
      if (dataShareHelper != undefined) {
        try {
          dsHelper.onDataChange(dataShare.SubscriptionType.SUBSCRIPTION_TYPE_EXACT_URI, dseUri, (ChangeInfo: dataShare.ChangeInfo) => {
            // expect(error == undefined).assertTrue();
          })
 
        } catch (err: BusinessError) {
          hilog.info(domain, tag, '%{public}s', `[on] <<Consumer>> on1 err.code: ${err.code}, err.message: ${err.message}`);
          expect().assertFail();
        }
      }
      done();
      console.log(TAG + "notifyChangeTest001 end");
    })
 
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0210
     * @tc.name      : notifyChangeTest002
     * @tc.desc      : close first one
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    it('notifyChangeTest002', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
      console.log(TAG + "notifyChangeTest002 start");
      if (dataShareHelper != undefined) {
        try {
          dsHelper.onDataChange(dataShare.SubscriptionType.SUBSCRIPTION_TYPE_EXACT_URI, "", (ChangeInfo: dataShare.ChangeInfo) => {
            // expect(error.code == undefined).assertTrue();
          })
 
        } catch (err: BusinessError) {
          hilog.info(domain, tag, '%{public}s', `[on] <<Consumer>> on1 err.code: ${err.code}, err.message: ${err.message}`);
          expect().assertFail();
        }
      }
      done();
      console.log(TAG + "notifyChangeTest002 end");
    })
 
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0210
     * @tc.name      : notifyChangeTest003
     * @tc.desc      : close first one
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    it('notifyChangeTest003', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
      console.log(TAG + "notifyChangeTest003 start");
      if (dataShareHelper != undefined) {
        try {
          dsHelper.offDataChange(dataShare.SubscriptionType.SUBSCRIPTION_TYPE_EXACT_URI, "", (ChangeInfo: dataShare.ChangeInfo) => {
            // expect(error.code == undefined).assertTrue();
          })
        } catch (err: BusinessError) {
          hilog.info(domain, tag, '%{public}s', `[on] <<Consumer>> on1 err.code: ${err.code}, err.message: ${err.message}`);
          expect().assertFail();
        }
      }
      done();
      console.log(TAG + "notifyChangeTest003 end");
    })
 
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0210
     * @tc.name      : notifyChangeTest004
     * @tc.desc      : close first one
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    it('notifyChangeTest004', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
      console.log(TAG + "notifyChangeTest004 start");
      if (dataShareHelper != undefined) {
        try {
          dsHelper.offDataChange(dataShare.SubscriptionType.SUBSCRIPTION_TYPE_EXACT_URI, "")
          console.log(TAG + "[off] success ");
        } catch (err: BusinessError) {
          hilog.info(domain, tag, '%{public}s', `[on] <<Consumer>> on1 err.code: ${err.code}, err.message: ${err.message}`);
          expect().assertFail();
        }
      }
      done();
      console.log(TAG + "notifyChangeTest004 end");
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0210
     * @tc.name      : notifyChangeTest005
     * @tc.desc      : close first one
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    it('notifyChangeTest005', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
      console.log(TAG + "notifyChangeTest005 start");
      if (dataShareHelper != undefined) {
        try {
          dsHelper.offDataChange(dataShare.SubscriptionType.SUBSCRIPTION_TYPE_EXACT_URI, dseUri)
          console.log(TAG + "[off] success ");
        } catch (err: BusinessError) {
          hilog.info(domain, tag, '%{public}s', `[on] <<Consumer>> on1 err.code: ${err.code}, err.message: ${err.message}`);
          expect().assertFail();
        }
      }
      done();
      console.log(TAG + "notifyChangeTest005 end");
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0210
     * @tc.name      : notifyChangeTest006
     * @tc.desc      : close first one
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    it('notifyChangeTest006', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
      console.log(TAG + "notifyChangeTest006 start");
      if (dataShareHelper != undefined) {
        try {
          dsHelper.offDataChange(dataShare.SubscriptionType.SUBSCRIPTION_TYPE_EXACT_URI, dseUri, undefined)
          console.log(TAG + "[off] success ");
        } catch (err: BusinessError) {
          hilog.info(domain, tag, '%{public}s', `[on] <<Consumer>> on1 err.code: ${err.code}, err.message: ${err.message}`);
          expect().assertFail();
        }
      }
      done();
      console.log(TAG + "notifyChangeTest006 end");
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0210
     * @tc.name      : notifyChangeTest007
     * @tc.desc      : close first one
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    it('notifyChangeTest007', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
      console.log(TAG + "notifyChangeTest007 start");
      if (dataShareHelper != undefined) {
        try {
          let people: Array<ValuesBucket> = [ {'name': 'ZhangSan', 'age': 31 as long} ];
          let changeData: dataShare.ChangeInfo = {
            type: dataShare.ChangeType.UPDATE, uri: dseUri, values: people
          };
          await dsHelper.notifyChange(changeData);
          console.log(TAG + "[notifyChange] success ");
        } catch (err: BusinessError) {
          hilog.info(domain, tag, '%{public}s', `[on] <<Consumer>> on1 err.code: ${err.code}, err.message: ${err.message}`);
          expect().assertFail();
        }
      }
      done();
      console.log(TAG + "notifyChangeTest007 end");
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0210
     * @tc.name      : notifyChangeTest008
     * @tc.desc      : close first one
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    it('notifyChangeTest008', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
      console.log(TAG + "notifyChangeTest008 start");
      if (dataShareHelper != undefined) {
        try {
          let people: Array<ValuesBucket> = [ {'name': 'ZhangSan', 'age': 31 as long} ];
          let changeData: dataShare.ChangeInfo = {
            type: dataShare.ChangeType.DELETE, uri: dseUri, values: people
          };
          await dsHelper.notifyChange(changeData);
          console.log(TAG + "[notifyChange] success ");
        } catch (err: BusinessError) {
          hilog.info(domain, tag, '%{public}s', `[on] <<Consumer>> on1 err.code: ${err.code}, err.message: ${err.message}`);
          expect().assertFail();
        }
      }
      done();
      console.log(TAG + "notifyChangeTest008 end");
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0210
     * @tc.name      : notifyChangeTest009
     * @tc.desc      : close first one
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    it('notifyChangeTest009', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
      console.log(TAG + "notifyChangeTest009 start");
      if (dataShareHelper != undefined) {
        try {
          let people: Array<ValuesBucket> = [ {'name': 'ZhangSan', 'age': 31 as long} ];
          let changeData: dataShare.ChangeInfo = {
            type: dataShare.ChangeType.INSERT, uri: dseUri, values: people
          };
          await dsHelper.notifyChange(changeData);
          console.log(TAG + "[notifyChange] success ");
        } catch (err: BusinessError) {
          hilog.info(domain, tag, '%{public}s', `[on] <<Consumer>> on1 err.code: ${err.code}, err.message: ${err.message}`);
          expect().assertFail();
        }
      }
      done();
      console.log(TAG + "notifyChangeTest009 end");
    })
  })
}