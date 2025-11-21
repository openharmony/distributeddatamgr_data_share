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

import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect, Level, Size, TestType } from "@ohos/hypium";
import relationalStore from "@ohos.data.relationalStore";
import dataSharePredicates from "@ohos.data.dataSharePredicates";
import common from '@ohos.app.ability.common';
import AbilityDelegatorRegistry from '@ohos.app.ability.abilityDelegatorRegistry';
import { BusinessError } from "@ohos.base";
import dataShare from '@ohos.data.dataShare'
import { ValuesBucket } from '@ohos.data.ValuesBucket';

let TAG = "[dataShareSubscribeTest_test]";
let context: common.UIAbilityContext;
let uri1 = ("datashare:///com.example.myapplication/test");
let uri2 = ("datashareproxy://com.example.myapplication/entry/DB00/TBL00");
let dataShareHelper: dataShare.DataShareHelper | undefined = undefined
let da = new dataSharePredicates.DataSharePredicates();
da.equalTo("name", "WangWu");
let va: ValuesBucket = {
  "name": "WangWu",
  "age": 20
}

let va_change: ValuesBucket = {
  "name": "LiSi",
  "age": 18
}
let people: Array<ValuesBucket> = new Array<ValuesBucket>(1);
people[0] = {
  name: 'ZhangSan', age: 31
};
let changeData: dataShare.ChangeInfo = {
  type: dataShare.ChangeType.DELETE, uri: uri1, values: people
};
let TEST_TIMEOUT = 2000 //means 2000ms.timeout occurs,testcases failed

export default function DataShareSubscribeTest() {
  describe('DataShareSubscribeTest', () => {
    beforeAll(async () => {
      context = AppStorage.get<common.UIAbilityContext>('TestAbilityContext') as common.UIAbilityContext;
      console.info(TAG + 'beforeAll');
    })
    beforeEach(async () => {
      console.info(TAG + 'beforeEach');
    })
    afterEach(async () => {
      console.info(TAG + 'afterEach');
    })
    afterAll(async () => {
      console.info(TAG + 'afterAll');
    })

    /**
    * @tc.name: testNonSilentSubscribe001
    * @tc.desc: test Non-silent subscribe, then notifyChange uri
    * @tc.type: FUNC
    * @tc.require:issue
    * @tc.precon: None
    * @tc.experct: trigger the callback
    */
    it('testNonSilentSubscribe001', 0, async (done:Function) => {
      console.log(TAG + "************* testNonSilentSubscribe001 start *************");
      const timeoutTimer = setTimeout(() => {
        console.log(TAG + "************* testNonSilentSubscribe001 timeout *************");
        try {
          expect(false).assertTrue();
        } catch (err) {
          done(err);
        }
      }, TEST_TIMEOUT);
      let onCallback: () => void = (): void => {
        console.log(TAG + "************* testNonSilentSubscribe001 callback   *************");
        clearTimeout(timeoutTimer);
        expect(true).assertTrue();
        done();
      }
      try {
        dataShareHelper = await dataShare.createDataShareHelper(context, uri1);
        await dataShareHelper.on("dataChange", uri1, onCallback);
        await dataShareHelper.notifyChange(uri1);
      } catch (err) {
        clearTimeout(timeoutTimer);
        console.error(`testNonSilentSubscribe001 false,error: code: ${err.code}, message: ${err.message} `);
        expect(false).assertTrue();
        done();
      }
      console.log(TAG + "************* testNonSilentSubscribe001 end   *************");
    })

    /**
    * @tc.name: testNonSilentSubscribe002
    * @tc.desc: test Non-silent subscribe with info, then notifyChange uri
    * @tc.type: FUNC
    * @tc.require:issue
    * @tc.precon: None
    * @tc.experct: trigger the callback with info
    */
    it('testNonSilentSubscribe002', 0, async (done:Function) => {
      console.log(TAG + "************* testNonSilentSubscribe002 start *************");
      const timeoutTimer = setTimeout(() => {
        try {
          expect(false).assertTrue();
        } catch (err) {
          done(err);
        }
      }, TEST_TIMEOUT);
      let onCallback = (error:BusinessError, ChangeInfo:dataShare.ChangeInfo) => {
        console.log(TAG + "************* testNonSilentSubscribe002 callback   *************");
        clearTimeout(timeoutTimer);
        expect(true).assertTrue();
        done();
      }
      try {
        dataShareHelper = await dataShare.createDataShareHelper(context, uri1);
        await dataShareHelper.on('dataChange', dataShare.SubscriptionType.SUBSCRIPTION_TYPE_EXACT_URI, uri1, onCallback);
        await dataShareHelper.notifyChange(changeData);
      } catch (err) {
        clearTimeout(timeoutTimer);
        console.error(`testNonSilentSubscribe002 false,error: code: ${err.code}, message: ${err.message} `);
        expect(false).assertTrue();
        done();
      }
      console.log(TAG + "************* testNonSilentSubscribe002 end   *************");
    })

    /**
    * @tc.name: testNonSilentSubscribe003
    * @tc.desc: test Non-silent subscribe then cancel, and then notifyChange uri
    * @tc.type: FUNC
    * @tc.require:issue
    * @tc.precon: None
    * @tc.experct: no trigger the callback
    */
    it('testNonSilentSubscribe003', 0, async (done:Function) => {
      console.log(TAG + "************* testNonSilentSubscribe003 start *************");
      const timeoutTimer = setTimeout(() => {
        expect(true).assertTrue();
        done();
      }, TEST_TIMEOUT);
      let onCallback: () => void = (): void => {
        console.log(TAG + "************* testNonSilentSubscribe003 callback   *************");
        clearTimeout(timeoutTimer);
        expect(false).assertTrue();
        done();
      }
      try {
        dataShareHelper = await dataShare.createDataShareHelper(context, uri1);
        await dataShareHelper.on("dataChange", uri1, onCallback);
        await dataShareHelper.off("dataChange", uri1, onCallback);
        await dataShareHelper.notifyChange(uri1);
      } catch (err) {
        clearTimeout(timeoutTimer);
        console.error(`testNonSilentSubscribe003 false,error: code: ${err.code}, message: ${err.message} `);
        expect(false).assertTrue();
        done();
      }
      console.log(TAG + "************* testNonSilentSubscribe003 end   *************");
    })

    /**
    * @tc.name: testNonSilentSubscribe004
    * @tc.desc: test Non-silent subscribe with info then cancel, and then notifyChange uri
    * @tc.type: FUNC
    * @tc.require:issue
    * @tc.precon: None
    * @tc.experct: no trigger the callback
    */
    it('testNonSilentSubscribe004', 0, async (done:Function) => {
      console.log(TAG + "************* testNonSilentSubscribe004 start *************");
      const timeoutTimer = setTimeout(() => {
        expect(true).assertTrue();
        done();
      }, TEST_TIMEOUT);
      let onCallback = (error:BusinessError, ChangeInfo:dataShare.ChangeInfo) => {
        console.log(TAG + "************* testNonSilentSubscribe004 callback   *************");
        clearTimeout(timeoutTimer);
        expect(false).assertTrue();
        done();
      }
      try {
        dataShareHelper = await dataShare.createDataShareHelper(context, uri1);
        await dataShareHelper.on('dataChange', dataShare.SubscriptionType.SUBSCRIPTION_TYPE_EXACT_URI, uri1, onCallback);
        await dataShareHelper.off('dataChange', dataShare.SubscriptionType.SUBSCRIPTION_TYPE_EXACT_URI, uri1, onCallback);
        await dataShareHelper.notifyChange(changeData);
      } catch (err) {
        clearTimeout(timeoutTimer);
        console.error(`testNonSilentSubscribe004 false,error: code: ${err.code}, message: ${err.message} `);
        expect(false).assertTrue();
        done();
      }
      console.log(TAG + "************* testNonSilentSubscribe004 end   *************");
    })

    /**
    * @tc.name: testNonSilentSubscribe005
    * @tc.desc: test Non-silent subscribe with info and without info, and then notifyChange uri
    * @tc.type: FUNC
    * @tc.require:issue
    * @tc.precon: None
    * @tc.experct: trigger the callback
    */
    it('testNonSilentSubscribe005', 0, async (done:Function) => {
      console.log(TAG + "************* testNonSilentSubscribe005 start *************");
      const timeoutTimer = setTimeout(() => {
        try {
          expect(false).assertTrue();
        } catch (err) {
          done(err);
        }
      }, TEST_TIMEOUT);
      let onCallback1: () => void = (): void => {
        clearTimeout(timeoutTimer);
        console.log(TAG + "************* testNonSilentSubscribe005 callback   *************");
        expect(true).assertTrue();
        done();
      }
      let onCallback2 = (error:BusinessError, ChangeInfo:dataShare.ChangeInfo) => {
        console.log(TAG + "************* testNonSilentSubscribe005 callback   *************");
      }
      try {
        dataShareHelper = await dataShare.createDataShareHelper(context, uri1);
        await dataShareHelper.on("dataChange", uri1, onCallback1);
        await dataShareHelper.on('dataChange', dataShare.SubscriptionType.SUBSCRIPTION_TYPE_EXACT_URI, uri1, onCallback2);
        await dataShareHelper.notifyChange(uri1);
      } catch (err) {
        clearTimeout(timeoutTimer);
        console.error(`testNonSilentSubscribe005 false,error: code: ${err.code}, message: ${err.message} `);
        expect(false).assertTrue();
        done();
      }
      console.log(TAG + "************* testNonSilentSubscribe005 end   *************");
    })

    /**
    * @tc.name: testNonSilentSubscribe005
    * @tc.desc: test Non-silent subscribe with info and without info, and then notifyChange uri
    * @tc.type: FUNC
    * @tc.require:issue
    * @tc.precon: None
    * @tc.experct: trigger the callback whit info
    */
    it('testNonSilentSubscribe006', 0, async (done:Function) => {
      console.log(TAG + "************* testNonSilentSubscribe006 start *************");
      const timeoutTimer = setTimeout(() => {
        try {
          expect(false).assertTrue();
        } catch (err) {
          done(err);
        }
      }, TEST_TIMEOUT);
      let onCallback1: () => void = (): void => {
        expect(true).assertTrue();
      }
      let onCallback2 = (error:BusinessError, ChangeInfo:dataShare.ChangeInfo) => {
        clearTimeout(timeoutTimer);
        console.error(`**** Observer callback **** type: ${ChangeInfo.type}, uri: ${ChangeInfo.uri},values: ${ChangeInfo.values} `);
        expect(ChangeInfo.type).assertEqual(1);
        expect(ChangeInfo.uri).assertEqual(uri1);
        done();
      }
      try {
        dataShareHelper = await dataShare.createDataShareHelper(context, uri1);
        await dataShareHelper.on("dataChange", uri1, onCallback1);
        await dataShareHelper.on('dataChange', dataShare.SubscriptionType.SUBSCRIPTION_TYPE_EXACT_URI, uri1, onCallback2);
        await dataShareHelper.notifyChange(changeData);
      } catch (err) {
        console.error(`testNonSilentSubscribe006 false,error: code: ${err.code}, message: ${err.message} `);
        expect(false).assertTrue();
      }
      console.log(TAG + "************* testNonSilentSubscribe006 end   *************");
    })

    /**
    * @tc.name: testSilentSubscribe001
    * @tc.desc: test silent subscribe, and then insert data
    * @tc.type: FUNC
    * @tc.require:issue
    * @tc.precon: None
    * @tc.experct: trigger the callback
    */
    it('testSilentSubscribe001', 0, async (done:Function) => {
      console.log(TAG + "************* testSilentSubscribe001 start *************");
      const timeoutTimer = setTimeout(() => {
        try {
          expect(false).assertTrue();
        } catch (err) {
          done(err);
        }
      }, TEST_TIMEOUT);
      let onCallback: () => void = (): void => {
        clearTimeout(timeoutTimer);
        expect(true).assertTrue();
        done();
      }
      try {
        dataShareHelper = await dataShare.createDataShareHelper(context, uri2, {isProxy : true});
        await dataShareHelper.on("dataChange", uri2, onCallback);
        await dataShareHelper.insert(uri2, va);
      } catch (err) {
        clearTimeout(timeoutTimer);
        console.error(`testSilentSubscribe001 false,error: code: ${err.code}, message: ${err.message} `);
        expect(false).assertTrue();
        done();
      }
      console.log(TAG + "************* testSilentSubscribe001 end   *************");
    })

    /**
    * @tc.name: testSilentSubscribe002
    * @tc.desc: test silent subscribe, and then update data
    * @tc.type: FUNC
    * @tc.require:issue
    * @tc.precon: None
    * @tc.experct: trigger the callback
    */
    it('testSilentSubscribe002', 0, async (done:Function) => {
      console.log(TAG + "************* testSilentSubscribe002 start *************");
      const timeoutTimer = setTimeout(() => {
        try {
          expect(false).assertTrue();
        } catch (err) {
          done(err);
        }
      }, TEST_TIMEOUT);
      let onCallback: () => void = (): void => {
        clearTimeout(timeoutTimer);
        expect(true).assertTrue();
        done();
      }
      try {
        dataShareHelper = await dataShare.createDataShareHelper(context, uri2, {isProxy : true});
        await dataShareHelper.insert(uri2, va);
        await dataShareHelper.on("dataChange", uri2, onCallback);
        await dataShareHelper.update(uri2, da, va);
      } catch (err) {
        clearTimeout(timeoutTimer);
        console.error(`testSilentSubscribe002 false,error: code: ${err.code}, message: ${err.message} `);
        expect(false).assertTrue();
        done();
      }
      console.log(TAG + "************* testSilentSubscribe002 end   *************");
    })

    /**
    * @tc.name: testSilentSubscribe003
    * @tc.desc: test silent subscribe, and then delete data
    * @tc.type: FUNC
    * @tc.require:issue
    * @tc.precon: None
    * @tc.experct: trigger the callback
    */
    it('testSilentSubscribe003', 0, async (done:Function) => {
      console.log(TAG + "************* testSilentSubscribe003 start *************");
      const timeoutTimer = setTimeout(() => {
        try {
          expect(false).assertTrue();
        } catch (err) {
          done(err);
        }
      }, TEST_TIMEOUT);
      let onCallback: () => void = (): void => {
        clearTimeout(timeoutTimer);
        expect(true).assertTrue();
        done();
      }
      try {
        dataShareHelper = await dataShare.createDataShareHelper(context, uri2, {isProxy : true});
        await dataShareHelper.insert(uri2, va);
        await dataShareHelper.on("dataChange", uri2, onCallback);
        await dataShareHelper.delete(uri2, da);
      } catch (err) {
        clearTimeout(timeoutTimer);
        console.error(`testSilentSubscribe003 false,error: code: ${err.code}, message: ${err.message} `);
        expect(false).assertTrue();
        done();
      }
      console.log(TAG + "************* testSilentSubscribe003 end   *************");
    })

    /**
    * @tc.name: testSilentSubscribe004
    * @tc.desc: test silent subscribe then cancel, and then insert data
    * @tc.type: FUNC
    * @tc.require:issue
    * @tc.precon: None
    * @tc.experct: no trigger the callback
    */
    it('testSilentSubscribe004', 0, async (done:Function) => {
      console.log(TAG + "************* testSilentSubscribe004 start *************");
      const timeoutTimer = setTimeout(() => {
        expect(true).assertTrue();
        done();
      }, TEST_TIMEOUT);
      let onCallback: () => void = (): void => {
        clearTimeout(timeoutTimer);
        expect(false).assertTrue();
        done();
      }
      try {
        dataShareHelper = await dataShare.createDataShareHelper(context, uri2, {isProxy : true});
        await dataShareHelper.on("dataChange", uri2, onCallback);
        await dataShareHelper.off("dataChange", uri2, onCallback);
        await dataShareHelper.insert(uri2, va);
      } catch (err) {
        clearTimeout(timeoutTimer);
        console.error(`testSilentSubscribe004 false,error: code: ${err.code}, message: ${err.message} `);
        expect(false).assertTrue();
        done();
      }
      console.log(TAG + "************* testSilentSubscribe004 end   *************");
    })

    /**
    * @tc.name: testSilentSubscribe005
    * @tc.desc: test silent subscribe then cancel, and then update data
    * @tc.type: FUNC
    * @tc.require:issue
    * @tc.precon: None
    * @tc.experct: no trigger the callback
    */
    it('testSilentSubscribe005', 0, async (done:Function) => {
      console.log(TAG + "************* testSilentSubscribe005 start *************");
      const timeoutTimer = setTimeout(() => {
        expect(true).assertTrue();
        done();
      }, TEST_TIMEOUT);
      let onCallback: () => void = (): void => {
        clearTimeout(timeoutTimer);
        expect(false).assertTrue();
        done();
      }
      try {
        dataShareHelper = await dataShare.createDataShareHelper(context, uri2, {isProxy : true});
        await dataShareHelper.insert(uri2, va);
        await dataShareHelper.on("dataChange", uri2, onCallback);
        await dataShareHelper.off("dataChange", uri2, onCallback);
        await dataShareHelper.update(uri2, da, va);
      } catch (err) {
        clearTimeout(timeoutTimer);
        console.error(`testSilentSubscribe005 false,error: code: ${err.code}, message: ${err.message} `);
        expect(false).assertTrue();
        done();
      }
      console.log(TAG + "************* testSilentSubscribe005 end   *************");
    })

    /**
    * @tc.name: testSilentSubscribe006
    * @tc.desc: test silent subscribe then cancel, and then delete data
    * @tc.type: FUNC
    * @tc.require:issue
    * @tc.precon: None
    * @tc.experct: no trigger the callback
    */
    it('testSilentSubscribe006', 0, async (done:Function) => {
      console.log(TAG + "************* testSilentSubscribe006 start *************");
      const timeoutTimer = setTimeout(() => {
        expect(true).assertTrue();
        done();
      }, TEST_TIMEOUT);
      let onCallback: () => void = (): void => {
        clearTimeout(timeoutTimer);
        expect(false).assertTrue();
        done();
      }
      try {
        dataShareHelper = await dataShare.createDataShareHelper(context, uri2, {isProxy : true});
        await dataShareHelper.insert(uri2, va);
        await dataShareHelper.on("dataChange", uri2, onCallback);
        await dataShareHelper.off("dataChange", uri2, onCallback);
        await dataShareHelper.delete(uri2, da);
      } catch (err) {
        clearTimeout(timeoutTimer);
        console.error(`testSilentSubscribe006 false,error: code: ${err.code}, message: ${err.message} `);
        expect(false).assertTrue();
        done();
      }
      console.log(TAG + "************* testSilentSubscribe006 end   *************");
    })

    /**
    * @tc.name: testSilentSubscribe007
    * @tc.desc: test silent subscribe whit info and without info then cancel, and then insert data
    * @tc.type: FUNC
    * @tc.require:issue
    * @tc.precon: None
    * @tc.experct: no trigger the callback
    */
    it('testSilentSubscribe007', 0, async (done:Function) => {
      console.log(TAG + "************* testSilentSubscribe007 start *************");
      const timeoutTimer = setTimeout(() => {
        expect(true).assertTrue();
        done();
      }, TEST_TIMEOUT);
      let onCallback1: () => void = (): void => {
        clearTimeout(timeoutTimer);
        expect(false).assertTrue();
        done();
      }
      let onCallback2: () => void = (): void => {
        clearTimeout(timeoutTimer);
        expect(false).assertTrue();
        done();
      }
      try {
        dataShareHelper = await dataShare.createDataShareHelper(context, uri2, {isProxy : true});
        await dataShareHelper.on("dataChange", uri2, onCallback1);
        await dataShareHelper.on("dataChange", uri2, onCallback2);
        await dataShareHelper.off("dataChange", uri2);
        await dataShareHelper.insert(uri2, va);
      } catch (err) {
        clearTimeout(timeoutTimer);
        console.error(`testSilentSubscribe007 false,error: code: ${err.code}, message: ${err.message} `);
        expect(false).assertTrue();
        done();
      }
      console.log(TAG + "************* testSilentSubscribe007 end   *************");
    })

    /**
    * @tc.name: testSilentSubscribe008
    * @tc.desc: test silent subscribe whit info and without info then cancel, and then updata data
    * @tc.type: FUNC
    * @tc.require:issue
    * @tc.precon: None
    * @tc.experct: no trigger the callback
    */
    it('testSilentSubscribe008', 0, async (done:Function) => {
      console.log(TAG + "************* testSilentSubscribe008 start *************");
      const timeoutTimer = setTimeout(() => {
        expect(true).assertTrue();
        done();
      }, TEST_TIMEOUT);
      let onCallback1: () => void = (): void => {
        clearTimeout(timeoutTimer);
        expect(false).assertTrue();
        done();
      }
      let onCallback2: () => void = (): void => {
        clearTimeout(timeoutTimer);
        expect(false).assertTrue();
        done();
      }
      try {
        dataShareHelper = await dataShare.createDataShareHelper(context, uri2, {isProxy : true});
        await dataShareHelper.insert(uri2, va);
        await dataShareHelper.on("dataChange", uri2, onCallback1);
        await dataShareHelper.on("dataChange", uri2, onCallback2);
        await dataShareHelper.off("dataChange", uri2);
        await dataShareHelper.update(uri2, da, va);
      } catch (err) {
        clearTimeout(timeoutTimer);
        console.error(`testSilentSubscribe008 false,error: code: ${err.code}, message: ${err.message} `);
        expect(false).assertTrue();
        done();
      }
      console.log(TAG + "************* testSilentSubscribe008 end   *************");
    })

    /**
    * @tc.name: testSilentSubscribe009
    * @tc.desc: test silent subscribe whit info and without info then cancel, and then delete data
    * @tc.type: FUNC
    * @tc.require:issue
    * @tc.precon: None
    * @tc.experct: no trigger the callback
    */
    it('testSilentSubscribe009', 0, async (done:Function) => {
      console.log(TAG + "************* testSilentSubscribe009 start *************");
      const timeoutTimer = setTimeout(() => {
        expect(true).assertTrue();
        done();
      }, TEST_TIMEOUT);
      let onCallback1: () => void = (): void => {
        clearTimeout(timeoutTimer);
        expect(false).assertTrue();
        done();
      }
      let onCallback2: () => void = (): void => {
        clearTimeout(timeoutTimer);
        expect(false).assertTrue();
        done();
      }
      try {
        dataShareHelper = await dataShare.createDataShareHelper(context, uri2, {isProxy : true});
        await dataShareHelper.insert(uri2, va);
        await dataShareHelper.on("dataChange", uri2, onCallback1);
        await dataShareHelper.on("dataChange", uri2, onCallback2);
        await dataShareHelper.off("dataChange", uri2);
        await dataShareHelper.delete(uri2, da);
      } catch (err) {
        clearTimeout(timeoutTimer);
        console.error(`testSilentSubscribe009 false,error: code: ${err.code}, message: ${err.message} `);
        expect(false).assertTrue();
        done();
      }
      console.log(TAG + "************* testSilentSubscribe009 end   *************");
    })

  })
}