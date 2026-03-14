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
import abilityDelegatorRegistry from '@ohos.app.ability.abilityDelegatorRegistry';
 
let context: common.UIAbilityContext;
let uri = ("datashareproxy://com.acts.ohos.data.datasharetest.static/test");
let dsProxyHelper: dataShare.DataShareHelper | undefined = undefined
const TAG = "[ttt] "
const COUNT_NUM  = 10;
const ON_CALLBACK_WAITETIME = 1000;
 
export default function DataShareNoSilentSubscribeRdbStoreMTBFTest() {
  describe('DataShareNoSilentSubscribeRdbStoreMTBFTest', () => {
    beforeAll(() => {
      try {
        let abilityDelegator: abilityDelegatorRegistry.AbilityDelegator = abilityDelegatorRegistry.getAbilityDelegator()
        console.log('czydebug here1');
        abilityDelegator.addAbilityMonitor({
          abilityName: "EntryAbility",
          moduleName: "entry",
          onAbilityCreate: (ability: UIAbility): void => {
            console.log('czydebug here2');
            context = ability.context;
            AppStorage.setOrCreate("context", ability.context);
            console.log('[ttt] ability.context.databaseDir:' + context.databaseDir);
            console.log('[ttt] ability.context.area:' + context.area);
            console.log('onAbilityCreate end');
          },
        }, (err: BusinessError | null): void => {
          if (err && err.code != 0) {
            console.log('-----' + err.code);
          }
          console.log('BusinessError  end');
        });
        await Utils.msSleep(2000);
        abilityDelegator.executeShellCommand("aa start -a EntryAbility -b com.acts.datasharestabilityclient_subscribe_mtbf.static");
        await Utils.msSleep(2000);
      } catch(err: BusinessError) {
        console.error('[ttt] [DataShareClientTest] <<Consumer>> createDataShareHelper error' + err.code + "  " + err.message);
        expect().assertFail();
      }
    })
 
    beforeEach(async () => {
      console.info(TAG + "===========beforeAll Begin===========");
      try{
        dsProxyHelper = await dataShare.createDataShareHelper(context, uri, {isProxy: false}); // 非静默访问
        await Utils.msSleep(2000);
        expect(dsProxyHelper != undefined).assertTrue();
        console.info(TAG + "[DataShareClientTest] <<Consumer>> dsProxyHelper " + JSON.stringify(dsProxyHelper));
      } catch (err: BusinessError) {
        console.error(TAG + '[DataShareClientTest] <<Consumer>> createDataShareHelper error' + err.code + "  " + err.message);
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
 
    afterAll(()=>{});
 
    /**
     * @tc.name       : testNoSilentSubscribeDataChangeInsertTest0010
     * @tc.number     : SUB_DistributedData_DFX_MTBF_DataShare_Subscribe_Test_001
     * @tc.desc       : onDataChange uri test
     * @tc.size       : MediumTest
     * @tc.type       : RELIABILITY
     * @tc.level      : Level 3
     */
    it('testNoSilentSubscribeDataChangeInsertTest0010', TestType.RELIABILITY | Size.MEDIUMTEST | Level.LEVEL3, async (done: () => void) => {
      try{
        for(let i = 0; i < COUNT_NUM; i++) {
          let onReturnCode = -1; //on接口的默认返回值为-1，如果进入回调则返回值为0
          // 增加数据，触发回调
          dsProxyHelper!.onDataChange(uri, () => {
            onReturnCode = 1;
            console.info(TAG + '>>>>>  testNoSilent' +
              'SubscribeDataChangeInsertTest0010 Observer callback >>>>> Info: ' +
              onReturnCode);
          });
 
          let vb: ValuesBucket = {
            "name0": "wangwuTest0010" + i,
          };
          try {
            let ret_inset = await dsProxyHelper!.insert(uri, vb);
            console.info(TAG +
              "[DataShareClientTest] <<Consumer>> testNoSilent" +
              "SubscribeDataChangeInsertTest0010 insert end rowNum is " +
              ret_inset);
          } catch (err) {
            console.error(TAG + 'testNoSilent' +
              'SubscribeDataChangeInsertTest0010 ret_inset failed! ' + err);
          }
          try {
            await dsProxyHelper!.notifyChange(uri);
            await Utils.msSleep(ON_CALLBACK_WAITETIME); // 预留时间进行回调
            // 异步回调中增加tag标识，断言tag标识确
            expect(onReturnCode === 1).assertTrue();
          } catch (err: BusinessError) {
            console.error(TAG + "testNoSilent" +
              "SubscribeDataChangeInsertTest0010 ret_inset notifyChange failed! onReturnCode = " +
              onReturnCode + "; err = " + err);
            // 失败只能因为是时序问题导致句柄释放后回调慢继续使用
            expect(err.code === 15700013).assertTrue();
          }
 
          // 最后取消订阅
          dsProxyHelper?.offDataChange(uri);
          // 通知回调后，返回flag断言
          await Utils.msSleep(ON_CALLBACK_WAITETIME);
        }
        done();
      } catch (err: BusinessError) {
        console.error(TAG +'[DataShareClientTest] <<Consumer>> testNoSilent' +
          'SubscribeDataChangeInsertTest0010 error ' + err.code + "  " + err.message);
        done();
        // 失败只能因为是时序问题导致句柄释放后回调慢继续使用
        expect(err.code === 15700013).assertTrue();
      }
    });
 
    /**
     * @tc.name       : testNoSilentSubscribeDataChangeUpdateTest0010
     * @tc.number     : SUB_DistributedData_DFX_MTBF_DataShare_Subscribe_Test_002
     * @tc.desc       : onDataChange uri test
     * @tc.size       : MediumTest
     * @tc.type       : RELIABILITY
     * @tc.level      : Level 3
     */
    it('testNoSilentSubscribeDataChangeUpdateTest0010', TestType.RELIABILITY | Size.MEDIUMTEST | Level.LEVEL3, async (done: () => void) => {
      try{
        for(let i = 0; i < COUNT_NUM; i++) {
          // 增加数据，触发回调
          let vb: ValuesBucket = {
            "name0": "wangwuTest0010" + i,
          };
          try {
            let ret_inset = await dsProxyHelper!.insert(uri, vb);
            console.info(TAG +
              "[DataShareClientTest] <<Consumer>> testNoSilent" +
              "SubscribeDataChangeUpdateTest0010 insert end rowNum is " +
              ret_inset);
          } catch (err) {
            console.error(TAG + 'testNoSilent' +
              'SubscribeDataChangeUpdateTest0010 ret_inset failed!' + err);
          }
 
          // 修改数据，触发回调
          let onReturnCode = -1; //on接口的默认返回值为-1，如果进入回调则返回值为0
          // 订阅接口
          dsProxyHelper!.onDataChange(uri, () => {
            onReturnCode = 1;
            console.info(TAG + '>>>>>  testNoSilent' +
              'SubscribeDataChangeUpdateTest0010 Observer callback >>>>> Info: ' +
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
              "[DataShareClientTest] <<Consumer>> testNoSilent" +
              "SubscribeDataChangeUpdateTest0010 update end rowNum is " +
              ret_update);
          } catch (err) {
            console.error(TAG + 'testNoSilent' +
              'SubscribeDataChangeUpdateTest0010 ret_update failed! ' + err);
          }
          try {
            await dsProxyHelper!.notifyChange(uri);
            await Utils.msSleep(ON_CALLBACK_WAITETIME)
            // 异步回调中增加tag标识，断言tag标识确
            expect(onReturnCode === 1).assertTrue();
          } catch (err: BusinessError) {
            console.error(TAG + "testNoSilent" +
              "SubscribeDataChangeUpdateTest0010 ret_update notifyChange failed! onReturnCode = "  + onReturnCode + "; err = "+ err);
            // 失败只能因为是时序问题导致句柄释放后回调慢继续使用
            expect(err.code === 15700013).assertTrue();
          }
 
          // 最后取消订阅
          dsProxyHelper!.offDataChange(uri);
          // 通知回调后，返回flag断言
          await Utils.msSleep(ON_CALLBACK_WAITETIME);
        }
        done();
      } catch (err: BusinessError) {
        console.error(TAG +'[DataShareClientTest] <<Consumer>> testNoSilent' +
          'SubscribeDataChangeUpdateTest0010 error ' + err.code + "  " + err.message);
        done();
        // 失败只能因为是时序问题导致句柄释放后回调慢继续使用
        expect(err.code === 15700013).assertTrue();
      }
    });
 
    /**
     * @tc.name       : testNoSilentSubscribeDataChangeDeleteTest0010
     * @tc.number     : SUB_DistributedData_DFX_MTBF_DataShare_Subscribe_Test_003
     * @tc.desc       : onDataChange uri test
     * @tc.size       : MediumTest
     * @tc.type       : RELIABILITY
     * @tc.level      : Level 3
     */
    it('testNoSilentSubscribeDataChangeDeleteTest0010', TestType.RELIABILITY | Size.MEDIUMTEST | Level.LEVEL3, async (done: () => void) => {
      try{
        for(let i = 0; i < COUNT_NUM; i++) {
          // 增加数据
          let vb: ValuesBucket = {
            "name0": "wangwuTest0010" + i,
          };
          try {
            let ret_inset = await dsProxyHelper!.insert(uri, vb);
            console.info(TAG +
              "[DataShareClientTest] <<Consumer>> testNoSilent" +
              "SubscribeDataChangeDeleteTest0010 insert end rowNum is " +
              ret_inset);
          } catch (err) {
            console.error(TAG + 'testNoSilent' +
              'SubscribeDataChangeDeleteTest0010 ret_inset failed!' + err);
          }
 
          // 删除数据，触发回调
          let onReturnCode = -1; //on接口的默认返回值为-1，如果进入回调则返回值为0
          // 订阅接口
          dsProxyHelper!.onDataChange(uri, () => {
            onReturnCode = 1;
            console.info(TAG + '>>>>> testNoSilent' +
              'SubscribeDataChangeDeleteTest0010 Observer callback >>>>> Info: ' +
              onReturnCode);
          });
 
          try {
            let daDe = new dataSharePredicates.DataSharePredicates();
            daDe.equalTo("name0", "wangwuTest0010" + i);
            let ret_delete = await dsProxyHelper!.delete(uri, daDe)
            console.log(TAG +
              "[DataShareClientTest] <<Consumer>> testNoSilent" +
              "SubscribeDataChangeDeleteTest0010 delete end rowNum is " +
              ret_delete);
          } catch (err) {
            console.info(TAG + 'ret_delete failed!' + err);
          }
 
          try {
            await dsProxyHelper!.notifyChange(uri);
            await Utils.msSleep(ON_CALLBACK_WAITETIME); // 预留时间进行回调
            // 异步回调中增加tag标识，断言tag标识确
            expect(onReturnCode === 1).assertTrue();
          } catch (err: BusinessError) {
            console.error(TAG +
              "testNoSilent" +
              "SubscribeDataChangeDeleteTest0010 ret_update notifyChange failed! onReturnCode = " + onReturnCode +
              "; err = " + err);
            // 失败只能因为是时序问题导致句柄释放后回调慢继续使用
            expect(err.code === 15700013).assertTrue();
          }
          // 最后取消订阅
          dsProxyHelper!.offDataChange(uri);
          // 通知回调后，返回flag断言
          await Utils.msSleep(ON_CALLBACK_WAITETIME);
        }
        done();
      } catch (err: BusinessError) {
        console.error(TAG +'[DataShareClientTest] <<Consumer>> testNoSilent' +
          'SubscribeDataChangeDeleteTest0010 error ' + err.code + "  " + err.message);
        done();
        // 失败只能因为是时序问题导致句柄释放后回调慢继续使用
        expect(err.code === 15700013).assertTrue();
      }
    });
 
    /**
     * @tc.name       : testNoSilentSubscribeDataChangeInsertTest0020
     * @tc.number     : SUB_DistributedData_DFX_MTBF_DataShare_Subscribe_Test_004
     * @tc.desc       : onDataChange uri and type test
     * @tc.size       : MediumTest
     * @tc.type       : RELIABILITY
     * @tc.level      : Level 3
     */
    // it('testNoSilentSubscribeDataChangeInsertTest0020', TestType.RELIABILITY | Size.MEDIUMTEST | Level.LEVEL3, async (done: () => void) => {
    //   try{
    //     console.log("czydbeug here1");
    //     for(let i = 0; i < COUNT_NUM; i++) {
    //       let onReturnCode = -1; // on接口的默认返回值为-1，如果进入回调则返回值为0
    //       // 订阅接口
    //       console.log("czydbeug here2");
    //       dsProxyHelper!.onDataChange(dataShare.SubscriptionType.SUBSCRIPTION_TYPE_EXACT_URI, uri,
    //         (ChangeInfo: dataShare.ChangeInfo) => {
    //           console.log("czydebug here3");
    //           // try{
    //           //   onReturnCode = 1;
    //           //   console.info(TAG +
    //           //     '>>>> testNoSilent' +
    //           //     'SubscribeDataChangeInsertTest0020 Observer callback >>>> ChangeInfo values =:' +
    //           //   JSON.stringify(ChangeInfo));
    //           //   expect(JSON.stringify(ChangeInfo.type) === "0").assertTrue();
    //           // } catch (err) {
    //           //   console.error(TAG + 'testNoSilent' +
    //           //     'SubscribeDataChangeInsertTest0020 Observer callback failed! ' + err);
    //           // }
    //         });
    //
    //       // 增加数据，触发回调
    //       let vb: ValuesBucket = {
    //         "name0": "wangwuTest0010",
    //       };
    //       try {
    //         let ret_inset = await dsProxyHelper!.insert(uri, vb);
    //         console.info(TAG +
    //           "[DataShareClientTest] <<Consumer>> testNoSilent" +
    //           "SubscribeDataChangeInsertTest0020 insert end rowNum is " +
    //           ret_inset);
    //       } catch (err) {
    //         console.error(TAG + 'testNoSilent' +
    //           'SubscribeDataChangeInsertTest0020 ret_inset failed! ' + err);
    //       }
    //       try {
    //         let inset_values: Array<ValuesBucket> = new Array<ValuesBucket>();
    //         inset_values.push(vb);
    //         await dsProxyHelper!.notifyChange({ type: dataShare.ChangeType.INSERT, uri: uri, values: inset_values });
    //         // 通知回调后，返回flag断言
    //         await Utils.msSleep(ON_CALLBACK_WAITETIME);
    //         // 异步回调中增加tag标识，断言tag标识确
    //         expect(onReturnCode === 1).assertTrue();
    //       } catch (err: BusinessError) {
    //         console.error(TAG + "testNoSilent" +
    //           "SubscribeDataChangeInsertTest0020 ret_inset notifyChange failed! onReturnCode = " + onReturnCode + "; err = " + err);
    //         // 失败只能因为是时序问题导致句柄释放后回调慢继续使用
    //         expect(err.code === 15700013).assertTrue();
    //       }
    //
    //       // 回调之后再最后取消订阅
    //       dsProxyHelper!.offDataChange(dataShare.SubscriptionType.SUBSCRIPTION_TYPE_EXACT_URI, uri);
    //       // 通知回调后，返回flag断言
    //       await Utils.msSleep(ON_CALLBACK_WAITETIME);
    //     }
    //     done();
    //   } catch (err: BusinessError) {
    //     console.error(TAG +'[DataShareClientTest] <<Consumer>> testNoSilent' +
    //       'SubscribeDataChangeInsertTest0020 error ' + err.code + "  " + err.message);
    //     done();
    //     // 失败只能因为是时序问题导致句柄释放后回调慢继续使用
    //     expect(err.code === 15700013).assertTrue();
    //   }
    // });
 
    /**
     * @tc.name       : testNoSilentSubscribeDataChangeUpdateTest0020
     * @tc.number     : SUB_DistributedData_DFX_MTBF_DataShare_Subscribe_Test_005
     * @tc.desc       : onDataChange uri and type test
     * @tc.size       : MediumTest
     * @tc.type       : RELIABILITY
     * @tc.level      : Level 3
     */
    it('testNoSilentSubscribeDataChangeUpdateTest0020', TestType.RELIABILITY | Size.MEDIUMTEST | Level.LEVEL3, async (done: () => void) => {
      try{
        for(let i = 0; i < COUNT_NUM; i++) {
          // 增加数据
          let vb: ValuesBucket = {
            "name0": "wangwuTest0010" + i,
          };
          try {
            let ret_inset = await dsProxyHelper!.insert(uri, vb);
            console.info(TAG +
              "[DataShareClientTest] <<Consumer>> testNoSilent" +
              "SubscribeDataChangeUpdateTest0020 insert end rowNum is " +
              ret_inset);
          } catch (err) {
            console.error(TAG + 'testNoSilent' +
              'SubscribeDataChangeUpdateTest0020 ret_inset failed! ' + err);
          }
 
          // 修改数据，触发回调
          let onReturnCode = -1; // on接口的默认返回值为-1，如果进入回调则返回值为0
          // 订阅接口
          dsProxyHelper!.onDataChange(dataShare.SubscriptionType.SUBSCRIPTION_TYPE_EXACT_URI, uri,
            async (ChangeInfo: dataShare.ChangeInfo) => {
              try{
                onReturnCode = 1;
                console.info(TAG +
                  '>>>> testNoSilent' +
                  'SubscribeDataChangeUpdateTest0020 Observer callback >>>> ChangeInfo values = ' +
                JSON.stringify(ChangeInfo));
                expect(JSON.stringify(ChangeInfo.type) === "2").assertTrue();
              }catch (err) {
                console.error(TAG + 'testNoSilent' +
                  'SubscribeDataChangeUpdateTest0020 Observer callback failed! ' + err);
              }
            });
 
          let vbUp: ValuesBucket = {
            "name0": "wangwuUpdateTest0010" + i,
          };
          try {
            let da = new dataSharePredicates.DataSharePredicates();
            da.equalTo("name0", "wangwuTest0010" + i);
            let ret_update = await dsProxyHelper!.update(uri, da, vbUp);
            console.log(TAG +
              "[DataShareClientTest] <<Consumer>> testNoSilent" +
              "SubscribeDataChangeUpdateTest0020 update end rowNum is " +
              ret_update);
          } catch (err) {
            console.error(TAG + 'testNoSilent' +
              'SubscribeDataChangeUpdateTest0020 ret_update failed!' + err);
          }
          try {
            let update_values: Array<ValuesBucket> = new Array<ValuesBucket>();
            update_values.push(vbUp);
            await dsProxyHelper!.notifyChange({ type: dataShare.ChangeType.UPDATE, uri: uri, values: update_values });
            // 通知回调后，返回flag断言
            await Utils.msSleep(ON_CALLBACK_WAITETIME);
            // 异步回调中增加tag标识，断言tag标识确
            expect(onReturnCode === 1).assertTrue();
          } catch (err: BusinessError) {
            console.error(TAG +
              "testNoSilent" +
              "SubscribeDataChangeUpdateTest0020 ret_update notifyChange failed! onReturnCode = " + onReturnCode +
              "; err = " + err);
            // 失败只能因为是时序问题导致句柄释放后回调慢继续使用
            expect(err.code === 15700013).assertTrue();
          }
          // 最后取消订阅
          dsProxyHelper!.offDataChange(dataShare.SubscriptionType.SUBSCRIPTION_TYPE_EXACT_URI, uri);
          // 通知回调后，返回flag断言
          await Utils.msSleep(ON_CALLBACK_WAITETIME);
        }
        done();
      } catch (err: BusinessError) {
        console.error(TAG +'[DataShareClientTest] <<Consumer>> testNoSilent' +
          'SubscribeDataChangeUpdateTest0020 error ' + err.code + "  " + err.message);
        done();
        // 失败只能因为是时序问题导致句柄释放后回调慢继续使用
        expect(err.code === 15700013).assertTrue();
      }
    });
 
    /**
     * @tc.name       : testNoSilentSubscribeDataChangeDeleteTest0020
     * @tc.number     : SUB_DistributedData_DFX_MTBF_DataShare_Subscribe_Test_006
     * @tc.desc       : onDataChange uri and type test
     * @tc.size       : MediumTest
     * @tc.type       : RELIABILITY
     * @tc.level      : Level 3
     */
    it('testNoSilentSubscribeDataChangeDeleteTest0020', TestType.RELIABILITY | Size.MEDIUMTEST | Level.LEVEL3, async (done: () => void) => {
      try {
        for(let i = 0; i < COUNT_NUM; i++){
          // 增加数据
          let vb: ValuesBucket = {
            "name0": "wangwuTest0010" + i,
          };
          try {
            let ret_inset = await dsProxyHelper!.insert(uri, vb);
            console.info(TAG +
              "[DataShareClientTest] <<Consumer>> testNoSilent" +
              "SubscribeDataChangeDeleteTest0020 insert end rowNum is " +
              ret_inset);
          } catch (err) {
            console.error(TAG + 'testNoSilent' +
              'SubscribeDataChangeDeleteTest0020 ret_inset failed! ' + err);
          }
 
          // 删除数据，触发回调
          let onReturnCode = -1; //on接口的默认返回值为-1，如果进入回调则返回值为0
          // 订阅接口
          dsProxyHelper!.onDataChange(dataShare.SubscriptionType.SUBSCRIPTION_TYPE_EXACT_URI, uri,
            async (ChangeInfo: dataShare.ChangeInfo) => {
              try{
                onReturnCode = 1;
                console.info(TAG +
                  '>>>> testNoSilent' +
                  'SubscribeDataChangeDeleteTest0020 Observer callback >>>> ChangeInfo values =:' +
                JSON.stringify(ChangeInfo));
                expect((JSON.stringify(ChangeInfo.type) === "3") || (JSON.stringify(ChangeInfo.type) === "1")).assertTrue();
              } catch (err) {
                console.error(TAG + 'testNoSilent' +
                  'SubscribeDataChangeDeleteTest0020 Observer callback failed! ' + err);
                expect().assertFail();
              }
            });
 
          try {
            let daDe = new dataSharePredicates.DataSharePredicates();
            daDe.equalTo("name0", "wangwuTest0010" + i);
            let ret_delete = await dsProxyHelper!.delete(uri, daDe);
            console.log(TAG +
              "[DataShareClientTest] <<Consumer>> testNoSilent" +
              "SubscribeDataChangeDeleteTest0020 delete end rowNum is " +
              ret_delete);
          } catch (err) {
            console.error(TAG + 'testNoSilent' +
              'SubscribeDataChangeDeleteTest0020 ret_delete failed! ' + err);
          }
          try {
            let delete_values: Array<ValuesBucket> = new Array<ValuesBucket>();
            delete_values.push(vb);
            await dsProxyHelper!.notifyChange({ type: dataShare.ChangeType.DELETE, uri: uri, values: delete_values });
            // 通知回调后，返回flag断言
            await Utils.msSleep(ON_CALLBACK_WAITETIME)
            // 异步回调中增加tag标识，断言tag标识确
            expect(onReturnCode === 1).assertTrue();
          } catch (err: BusinessError) {
            console.error(TAG +
              "testNoSilent" +
              "SubscribeDataChangeDeleteTest0020 ret_update notifyChange failed! onReturnCode = " + onReturnCode +
              "; err = " + err);
            // 失败只能因为是时序问题导致句柄释放后回调慢继续使用
            expect(err.code === 15700013).assertTrue();
          }
 
          // 最后取消订阅
          dsProxyHelper!.offDataChange(dataShare.SubscriptionType.SUBSCRIPTION_TYPE_EXACT_URI, uri);
          // 通知回调后，返回flag断言
          await Utils.msSleep(ON_CALLBACK_WAITETIME);
        }
        done();
      } catch (err: BusinessError) {
        console.error(TAG +'[DataShareClientTest] <<Consumer>> testNoSilent' +
          'SubscribeDataChangeDeleteTest0020 error ' + err.code + "  " + err.message);
        done();
        // 失败只能因为是时序问题导致句柄释放后回调慢继续使用
        expect(err.code === 15700013).assertTrue();
      }
    });
  })
}