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
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect, TestType, Size, Level } from '../../../hypium/index';
import dataShare from '@ohos.data.dataShare'
import dataSharePredicates from '@ohos.data.dataSharePredicates'
import { ValuesBucket } from '@ohos.data.ValuesBucket';
import { BusinessError } from '@ohos.base';
import Utils from './Util.test';
import common from '@ohos.app.ability.common';
import { AppStorage } from '@ohos.arkui.stateManagement'
import UIAbility from '@ohos.app.ability.UIAbility';
 
let context: common.UIAbilityContext;
let uri = ("datashareproxy://com.acts.ohos.data.datasharetest.static/test");
let dsProxyHelper: dataShare.DataShareHelper | undefined = undefined
const TAG = "[ttt] "
const COUNT_NUM  = 10;
const ON_CALLBACK_WAITETIME = 1000;
 
export default function DataShareSilentSubscribeOnDataChangeRdbStoreMTBFTest() {
  describe('DataShareSilentSubscribeOnDataChangeRdbStoreMTBFTest', () => {
    beforeAll(() => {
      context = AppStorage.get<common.UIAbilityContext>("context") as common.UIAbilityContext;
    })
 
    beforeEach(async () => {
      console.info(TAG + "===========beforeAll Begin===========");
      try{
        dsProxyHelper = await dataShare.createDataShareHelper(context, uri, {isProxy:true}); // 默访问
        await Utils.msSleep(2000);
        console.info(TAG + "[DataShareClientTest] <<Consumer>> dsProxyHelper " + JSON.stringify(dsProxyHelper));
      } catch (err: BusinessError) {
        console.error(TAG + '[DataShareClientTest] <<Consumer>> createDataShareHelper error' + err.code+ "  " + err.message);
        expect().assertFail();
      }
      console.info(TAG + "===========beforeAll End===========");
    })
 
    afterEach(async () => {
      console.info(TAG + "===========afterAll Begin===========");
      let da = new dataSharePredicates.DataSharePredicates();
      await dsProxyHelper!.delete(uri, da);
      await dsProxyHelper!.close();
      console.info(TAG + "===========afterAll End===========");
    })
 
    afterAll(() => {});
 
    /**
     * @tc.name       : testSubscribeDataChangeInsertTest0010
     * @tc.number     : SUB_DistributedData_DFX_MTBF_DataShare_Subscribe_Test_007
     * @tc.desc       : onDataChange uri test
     * @tc.size       : MediumTest
     * @tc.type       : RELIABILITY
     * @tc.level      : Level 3
     */
    it('testSubscribeDataChangeInsertTest0010', TestType.RELIABILITY | Size.MEDIUMTEST | Level.LEVEL3, async (done: () => void) => {
      try{
        for(let i = 0; i < COUNT_NUM; i++) {
          // 增加数据，触发回调
          let onReturnCode = -1; //on接口的默认返回值为-1，如果进入回调则返回值为0
          // 订阅接口
          dsProxyHelper!.onDataChange(uri, () => {
            onReturnCode = 1;
            console.info(TAG + '>>>>>  testSubscribeDataChangeInsertTest0010 Observer callback >>>>> Info: ' +
              onReturnCode);
          });
 
          let vb: ValuesBucket = {
            "name0": "wangwuTest0010" + i,
          };
          try {
            let ret_inset = await dsProxyHelper!.insert(uri, vb);
            console.info(TAG +
              "[DataShareClientTest] <<Consumer>> testSubscribeDataChangeInsertTest0010 insert end rowNum is " +
              ret_inset);
          } catch (err) {
            console.error(TAG + 'testSubscribeDataChangeInsertTest0010 ret_inset failed! ' + err);
          }
          try {
            await Utils.msSleep(ON_CALLBACK_WAITETIME); // 预留时间进行回调
            // 异步回调中增加tag标识，断言tag标识确
            expect(onReturnCode === 1).assertTrue();
          } catch (err: BusinessError) {
            console.error(TAG + "testSubscribeDataChangeInsertTest0010 ret_inset notifyChange failed! onReturnCode = " +
              onReturnCode + "; err = " + err);
            // 失败只能因为是时序问题导致句柄释放后回调慢继续使用
            expect(err.code === 15700013).assertTrue();
          }
 
          // 最后取消订阅
          dsProxyHelper!.offDataChange(uri);
          await Utils.msSleep(ON_CALLBACK_WAITETIME); // 预留时间进行回调
        }
        done();
      } catch (err: BusinessError) {
        console.error(TAG +'[DataShareClientTest] <<Consumer>> testSubscribeDataChangeInsertTest0010 error ' + err.code + "  " + err.message);
        done();
        // 失败只能因为是时序问题导致句柄释放后回调慢继续使用
        expect(err.code === 15700013).assertTrue();
      }
    });
 
    /**
     * @tc.name       : testSubscribeDataChangeUpdateTest0010
     * @tc.number     : SUB_DistributedData_DFX_MTBF_DataShare_Subscribe_Test_008
     * @tc.desc       : onDataChange uri test
     * @tc.size       : MediumTest
     * @tc.type       : RELIABILITY
     * @tc.level      : Level 3
     */
    it('testSubscribeDataChangeUpdateTest0010', TestType.RELIABILITY | Size.MEDIUMTEST | Level.LEVEL3, async (done: () => void) => {
      try{
        for(let i = 0; i < COUNT_NUM; i++) {
          // 增加数据，触发回调
          let vb: ValuesBucket = {
            "name0": "wangwuTest0010" + i,
          };
          try {
            let ret_inset = await dsProxyHelper!.insert(uri, vb);
            console.info(TAG +
              "[DataShareClientTest] <<Consumer>> testSubscribeDataChangeUpdateTest0010 insert end rowNum is " +
              ret_inset);
          } catch (err) {
            console.error(TAG + 'testSubscribeDataChangeUpdateTest0010 ret_inset failed!' + err);
          }
 
          // 修改数据，触发回调
          let onReturnCode = -1; //on接口的默认返回值为-1，如果进入回调则返回值为0
          // 订阅接口
          dsProxyHelper!.onDataChange(uri, () => {
            onReturnCode = 1;
            console.info(TAG + '>>>>>  testSubscribeDataChangeUpdateTest0010 Observer callback >>>>> Info: ' +
              onReturnCode);
          });
 
          try {
            let vbUp: ValuesBucket = {
              "name0": "wangwuUpdateTest0010" + i,
            };
            let da = new dataSharePredicates.DataSharePredicates();
            da.equalTo("name0", "wangwuTest0010" + i);
            let ret_update = await dsProxyHelper!.update(uri, da, vbUp);
            console.log(TAG +
              "[DataShareClientTest] <<Consumer>> testSubscribeDataChangeUpdateTest0010 update end rowNum is " +
              ret_update);
          } catch (err) {
            console.error(TAG + 'testSubscribeDataChangeUpdateTest0010 ret_update failed! ' + err);
          }
          try {
            await Utils.msSleep(ON_CALLBACK_WAITETIME); // 预留时间进行回调
            // 异步回调中增加tag标识，断言tag标识确
            expect(onReturnCode === 1).assertTrue();
          } catch (err: BusinessError) {
            console.error(TAG + "testSubscribeDataChangeUpdateTest0010 ret_update notifyChange failed! onReturnCode = "  + onReturnCode + "; err = "+ err);
            // 失败只能因为是时序问题导致句柄释放后回调慢继续使用
            expect(err.code === 15700013).assertTrue();
          }
 
          // 最后取消订阅
          dsProxyHelper!.offDataChange(uri);
          await Utils.msSleep(ON_CALLBACK_WAITETIME);
        }
        done();
      } catch (err: BusinessError) {
        console.error(TAG +'[DataShareClientTest] <<Consumer>> testSubscribeDataChangeUpdateTest0010 error ' + err.code + "  " + err.message);
        done();
        // 失败只能因为是时序问题导致句柄释放后回调慢继续使用
        expect(err.code === 15700013).assertTrue();
      }
    });
 
    /**
     * @tc.name       : testSubscribeDataChangeDeleteTest0010
     * @tc.number     : SUB_DistributedData_DFX_MTBF_DataShare_Subscribe_Test_009
     * @tc.desc       : onDataChange uri test
     * @tc.size       : MediumTest
     * @tc.type       : RELIABILITY
     * @tc.level      : Level 3
     */
    it('testSubscribeDataChangeDeleteTest0010', TestType.RELIABILITY | Size.MEDIUMTEST | Level.LEVEL3, async (done: () => void) => {
      try{
        for(let i = 0; i < COUNT_NUM; i++) {
          // 增加数据
          let vb: ValuesBucket = {
            "name0": "wangwuTest0010" + i,
          };
          try {
            let ret_inset = await dsProxyHelper!.insert(uri, vb);
            console.info(TAG +
              "[DataShareClientTest] <<Consumer>> testSubscribeDataChangeDeleteTest0010 insert end rowNum is " +
              ret_inset);
          } catch (err) {
            console.error(TAG + 'testSubscribeDataChangeDeleteTest0010 ret_inset failed!' + err);
          }
 
          // 删除数据，触发回调
          let onReturnCode = -1; //on接口的默认返回值为-1，如果进入回调则返回值为0
          // 订阅接口
          dsProxyHelper!.onDataChange(uri, () => {
            onReturnCode = 1;
            console.info(TAG + '>>>>> testSubscribeDataChangeDeleteTest0010 Observer callback >>>>> Info: ' +
              onReturnCode);
          });
 
          try {
            let daDe = new dataSharePredicates.DataSharePredicates();
            daDe.equalTo("name0", "wangwuTest0010" + i);
            let ret_delete = await dsProxyHelper!.delete(uri, daDe)
            console.log(TAG +
              "[DataShareClientTest] <<Consumer>> testSubscribeDataChangeDeleteTest0010 delete end rowNum is " +
              ret_delete);
          } catch (err) {
            console.info(TAG + 'ret_delete failed!' + err);
          }
          try {
            await Utils.msSleep(ON_CALLBACK_WAITETIME); // 预留时间进行回调
            // 异步回调中增加tag标识，断言tag标识确
            expect(onReturnCode === 1).assertTrue();
          } catch (err: BusinessError) {
            console.error(TAG +
              "testSubscribeDataChangeDeleteTest0010 ret_update notifyChange failed! onReturnCode = " + onReturnCode +
              "; err = " + err);
            // 失败只能因为是时序问题导致句柄释放后回调慢继续使用
            expect(err.code === 15700013).assertTrue();
          }
          // 最后取消订阅
          dsProxyHelper!.offDataChange(uri);
          await Utils.msSleep(ON_CALLBACK_WAITETIME); // 预留时间进行回调
        }
        done();
      } catch (err: BusinessError) {
        console.error(TAG +'[DataShareClientTest] <<Consumer>> testSubscribeDataChangeDeleteTest0010 error ' + err.code + "  " + err.message);
        done();
        // 失败只能因为是时序问题导致句柄释放后回调慢继续使用
        expect(err.code === 15700013).assertTrue();
      }
    });
 
    /**
     * @tc.name       : testSubscribeDataChangeInsertTest0020
     * @tc.number     : SUB_DistributedData_DFX_MTBF_DataShare_Subscribe_Test_010
     * @tc.desc       : onDataChange uri and type test
     * @tc.size       : MediumTest
     * @tc.type       : RELIABILITY
     * @tc.level      : Level 3
     */
//    it('testSubscribeDataChangeInsertTest0020', TestType.RELIABILITY | Size.MEDIUMTEST | Level.LEVEL3, async (done: () => void) => {
//      try{
//        for(let i = 0; i < COUNT_NUM; i++) {
//          let onReturnCode = -1; //on接口的默认返回值为-1，如果进入回调则返回值为0
//          // 订阅接口
//          dsProxyHelper!.onDataChange(dataShare.SubscriptionType.SUBSCRIPTION_TYPE_EXACT_URI, uri,
//            async (ChangeInfo: dataShare.ChangeInfo) => {
//              try{
//                onReturnCode = 1;
//                console.info(TAG +
//                  '>>>> testSubscribeDataChangeInsertTest0020 Observer callback >>>> ChangeInfo values =:' +
//                  JSON.stringify(ChangeInfo));
//                expect(JSON.stringify(ChangeInfo.type) === "3").assertTrue();
//              } catch (err) {
//                console.error(TAG + 'testSubscribeDataChangeInsertTest0020 Observer callback failed! ' + err);
//              }
//            });
//
//          // 增加数据，触发回调
//          let vb: ValuesBucket = {
//            "name0": "wangwuTest0010",
//          };
//          try {
//            let ret_inset = await dsProxyHelper!.insert(uri, vb);
//            console.info(TAG +
//              "[DataShareClientTest] <<Consumer>> testSubscribeDataChangeInsertTest0020 insert end rowNum is " +
//              ret_inset);
//          } catch (err) {
//            console.error(TAG + 'testSubscribeDataChangeInsertTest0020 ret_inset failed! ' + err);
//          }
//
//          try {
//            // 通知回调后，返回flag断言
//            await Utils.msSleep(ON_CALLBACK_WAITETIME);
//            // 异步回调中增加tag标识，断言tag标识确
//            expect(onReturnCode === 1).assertTrue();
//          } catch (err: BusinessError) {
//            console.error(TAG + "testSubscribeDataChangeInsertTest0020 ret_inset notifyChange failed! onReturnCode = " + onReturnCode + "; err = " + err);
//            // 失败只能因为是时序问题导致句柄释放后回调慢继续使用
//            expect(err.code === 15700013).assertTrue();
//          }
//          // 回调之后再最后取消订阅
//          dsProxyHelper!.offDataChange(dataShare.SubscriptionType.SUBSCRIPTION_TYPE_EXACT_URI, uri);
//          // 通知回调后，返回flag断言
//          await Utils.msSleep(ON_CALLBACK_WAITETIME);
//        }
//        done();
//      } catch (err: BusinessError) {
//        console.error(TAG +'[DataShareClientTest] <<Consumer>> testSubscribeDataChangeInsertTest0020 error ' + err.code + "  "+err.message);
//        done();
//        // 失败只能因为是时序问题导致句柄释放后回调慢继续使用
//        expect(err.code === 15700013).assertTrue();
//      }
//    });
 
    /**
     * @tc.name       : testSubscribeDataChangeUpdateTest0020
     * @tc.number     : SUB_DistributedData_DFX_MTBF_DataShare_Subscribe_Test_011
     * @tc.desc       : onDataChange uri and type test
     * @tc.size       : MediumTest
     * @tc.type       : RELIABILITY
     * @tc.level      : Level 3
     */
//    it('testSubscribeDataChangeUpdateTest0020', TestType.RELIABILITY | Size.MEDIUMTEST | Level.LEVEL3, async (done: () => void) => {
//      try{
//        for(let i = 0; i < COUNT_NUM; i++) {
//          // 增加数据
//          let vb: ValuesBucket = {
//            "name0": "wangwuTest0010" + i,
//          };
//          try {
//            let ret_inset = await dsProxyHelper!.insert(uri, vb);
//            console.info(TAG +
//              "[DataShareClientTest] <<Consumer>> testSubscribeDataChangeUpdateTest0020 insert end rowNum is " +
//              ret_inset);
//          } catch (err) {
//            console.error(TAG + 'testSubscribeDataChangeUpdateTest0020 ret_inset failed! ' + err);
//          }
//
//          // 修改数据，触发回调
//          let onReturnCode = -1; //on接口的默认返回值为-1，如果进入回调则返回值为0
//          // 订阅接口
//          dsProxyHelper!.onDataChange(dataShare.SubscriptionType.SUBSCRIPTION_TYPE_EXACT_URI, uri,
//            async (ChangeInfo: dataShare.ChangeInfo) => {
//              try {
//                onReturnCode = 1;
//                console.info(TAG +
//                  '>>>> testSubscribeDataChangeUpdateTest0020 Observer callback >>>> ChangeInfo values =:' +
//                JSON.stringify(ChangeInfo));
//                expect(JSON.stringify(ChangeInfo.type) === "3").assertTrue();
//              } catch (err) {
//                console.error(TAG + 'testSubscribeDataChangeUpdateTest0020 Observer callback failed! ' + err);
//              }
//            });
//
//          let vbUp: ValuesBucket = {
//            "name0": "wangwuUpdateTest0010" + i,
//          };
//          try {
//            let da = new dataSharePredicates.DataSharePredicates();
//            da.equalTo("name0", "wangwuTest0010" + i);
//            let ret_update = await dsProxyHelper!.update(uri, da, vbUp);
//            console.log(TAG +
//              "[DataShareClientTest] <<Consumer>> testSubscribeDataChangeUpdateTest0020 update end rowNum is " +
//              ret_update);
//          } catch (err) {
//            console.error(TAG + 'testSubscribeDataChangeUpdateTest0020 ret_update failed!' + err);
//          }
//          try {
//            // 通知回调后，返回flag断言
//            await Utils.msSleep(ON_CALLBACK_WAITETIME);
//            // 异步回调中增加tag标识，断言tag标识确
//            expect(onReturnCode === 1).assertTrue();
//          } catch (err: BusinessError) {
//            console.error(TAG +
//              "testSubscribeDataChangeUpdateTest0020 ret_update notifyChange failed! onReturnCode = " + onReturnCode +
//              "; err = " + err);
//            // 失败只能因为是时序问题导致句柄释放后回调慢继续使用
//            expect(err.code === 15700013).assertTrue();
//          }
//          // 最后取消订阅
//          dsProxyHelper!.offDataChange(dataShare.SubscriptionType.SUBSCRIPTION_TYPE_EXACT_URI, uri);
//          // 通知回调后，返回flag断言
//          await Utils.msSleep(ON_CALLBACK_WAITETIME);
//        }
//        done();
//      } catch (err: BusinessError) {
//        console.error(TAG +'[DataShareClientTest] <<Consumer>> testSubscribeDataChangeUpdateTest0020 error ' + err.code + "  "+err.message);
//        done();
//        // 失败只能因为是时序问题导致句柄释放后回调慢继续使用
//        expect(err.code === 15700013).assertTrue();
//      }
//    });
 
    /**
     * @tc.name       : testSubscribeDataChangeDeleteTest0020
     * @tc.number     : SUB_DistributedData_DFX_MTBF_DataShare_Subscribe_Test_012
     * @tc.desc       : onDataChange uri and type test
     * @tc.size       : MediumTest
     * @tc.type       : RELIABILITY
     * @tc.level      : Level 3
     */
//    it('testSubscribeDataChangeDeleteTest0020', TestType.RELIABILITY | Size.MEDIUMTEST | Level.LEVEL3, async (done: () => void) => {
//      try {
//        for(let i = 0; i < COUNT_NUM; i++){
//          // 增加数据，触发回调
//          let vb: ValuesBucket = {
//            "name0": "wangwuTest0010" + i,
//          };
//          try {
//            let ret_inset = await dsProxyHelper!.insert(uri, vb);
//            console.info(TAG +
//              "[DataShareClientTest] <<Consumer>> testSubscribeDataChangeDeleteTest0020 insert end rowNum is " +
//              ret_inset);
//          } catch (err) {
//            console.error(TAG + 'testSubscribeDataChangeDeleteTest0020 ret_inset failed! ' + err);
//          }
//
//          // 删除数据，触发回调
//          let onReturnCode = -1; //on接口的默认返回值为-1，如果进入回调则返回值为0
//          // 订阅接口
//          dsProxyHelper!.onDataChange(dataShare.SubscriptionType.SUBSCRIPTION_TYPE_EXACT_URI, uri,
//            async (ChangeInfo: dataShare.ChangeInfo) => {
//              try {
//                onReturnCode = 1;
//                console.info(TAG +
//                  '>>>> testSubscribeDataChangeDeleteTest0020 Observer callback >>>> ChangeInfo values =:' +
//                JSON.stringify(ChangeInfo));
//                // 静默增删改的通知，不发送changeInfo，故type的类型默认为3
//                expect((JSON.stringify(ChangeInfo.type) === "3") || (JSON.stringify(ChangeInfo.type) === "1") ).assertTrue();
//              } catch (err) {
//                console.error(TAG + 'testSubscribeDataChangeDeleteTest0020 Observer callback failed! ' + err);
//              }
//            });
//
//          try {
//            let daDe = new dataSharePredicates.DataSharePredicates();
//            daDe.equalTo("name0", "wangwuTest0010" + i);
//            let ret_delete = await dsProxyHelper!.delete(uri, daDe);
//            console.log(TAG +
//              "[DataShareClientTest] <<Consumer>> testSubscribeDataChangeDeleteTest0020 delete end rowNum is " +
//              ret_delete);
//          } catch (err) {
//            console.error(TAG + 'testSubscribeDataChangeDeleteTest0020 ret_delete failed! ' + err);
//          }
//          try {
//            // 通知回调后，返回flag断言
//            await Utils.msSleep(ON_CALLBACK_WAITETIME)
//            // 异步回调中增加tag标识，断言tag标识确
//            expect(onReturnCode === 1).assertTrue();
//          } catch (err: BusinessError) {
//            console.error(TAG +
//              "testSubscribeDataChangeDeleteTest0020 ret_update notifyChange failed! onReturnCode = " + onReturnCode +
//              "; err = " + err);
//            // 失败只能因为是时序问题导致句柄释放后回调慢继续使用
//            expect(err.code === 15700013).assertTrue();
//          }
//
//          // 最后取消订阅
//          dsProxyHelper!.offDataChange(dataShare.SubscriptionType.SUBSCRIPTION_TYPE_EXACT_URI, uri);
//          // 通知回调后，返回flag断言
//          await Utils.msSleep(ON_CALLBACK_WAITETIME)
//        }
//        done();
//      } catch (err: BusinessError) {
//        console.error(TAG +'[DataShareClientTest] <<Consumer>> testSubscribeDataChangeDeleteTest0020 error ' + err.code + "  "+err.message);
//        done();
//        // 失败只能因为是时序问题导致句柄释放后回调慢继续使用
//        expect(err.code === 15700013).assertTrue();
//      }
//    });
  })
}