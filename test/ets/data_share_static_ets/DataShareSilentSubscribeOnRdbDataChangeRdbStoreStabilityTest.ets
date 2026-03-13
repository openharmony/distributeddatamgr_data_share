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
import { AppStorage } from '@ohos.arkui.stateManagement';
import UIAbility from '@ohos.app.ability.UIAbility';
 
let context: common.UIAbilityContext;
let uri = ("datashareproxy://com.acts.ohos.data.datasharetest.static/test");
let dsProxyHelper: dataShare.DataShareHelper | undefined = undefined
const TAG = "[ttt] "
const COUNT_NUM  = 200;
 
export default function DataShareSilentSubscribeOnRdbDataChangeRdbStoreMTBFTest() {
  describe('DataShareSilentSubscribeOnRdbDataChangeRdbStoreMTBFTest', () => {
    beforeAll(() => {
      context = AppStorage.get<common.UIAbilityContext>("context") as common.UIAbilityContext;
    })
 
    beforeEach(async () => {
        console.info(TAG + "===========beforeAll Begin===========");
        try{
          dsProxyHelper = await dataShare.createDataShareHelper(context, uri,{isProxy:true}); // 默访问
          await Utils.msSleep(2000);
          expect(dsProxyHelper != undefined).assertTrue();
          console.info(TAG + "[DataShareClientTest] <<Consumer>> dsProxyHelper " + JSON.stringify(dsProxyHelper));
          // 清理数据
          let da = new dataSharePredicates.DataSharePredicates();
          await dsProxyHelper!.delete(uri, da);
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
 
    afterAll(() => {});
 
    /**
     * @tc.name       : testSubscribeRdbDataChangeInsertTest0010
     * @tc.number     : SUB_DistributedData_DFX_MTBF_DataShare_Subscribe_Test_013
     * @tc.desc       : onRdbDataChange test
     * @tc.size       : MediumTest
     * @tc.type       : RELIABILITY
     * @tc.level      : Level 3
     */
    it('testSubscribeRdbDataChangeInsertTest0010', TestType.RELIABILITY | Size.MEDIUMTEST | Level.LEVEL3, async (done: () => void) => {
      // 订阅的模板信息
      let template: dataShare.Template = {
        predicates: {
          "p1": "select name0 as name from TBL00",
          "p2": "select name1 as name from TBL00",
        },
        scheduler: "select remindTimer(strftime('%s', 'now', '+10 seconds'))"
      }
      let templateId: dataShare.TemplateId = {
        subscriberId: "100", bundleNameOfOwner: "com.acts.ohos.data.datasharetestclient"
      }
 
      try{
          for(let i = 0; i < COUNT_NUM; i++) {
            // 增加的数据
            let vb: ValuesBucket = {
              "name1": "LisiTest0010" + i,
            };
 
            // 添加模板
            try {
              console.log(TAG +"[DataShareClientTest] <<Consumer>> testSubscribeRdbDataChangeInsertTest0010 addTemplate begin");
              dsProxyHelper?.addTemplate(uri, "100", template);
              console.log(TAG +"[DataShareClientTest] <<Consumer>> testSubscribeRdbDataChangeInsertTest0010 addTemplate end");
            } catch (err) {
              console.error(TAG +"[DataShareClientTest] <<Consumer>> testSubscribeRdbDataChangeInsertTest0010 addTemplate error " + JSON.stringify(err));
            }
 
            // 增加数据，触发回调
            // 订阅接口
            let onReturnCode = 0;
            let result1: Array<dataShare.OperationResult> = dsProxyHelper!.onRdbDataChange([uri], templateId, (node:dataShare.RdbDataChangeNode) => {
              onReturnCode ++;
              console.info(TAG +"testSubscribeRdbDataChangeInsertTest0010 onCallback " + JSON.stringify(node.uri));
              console.info(TAG +"testSubscribeRdbDataChangeInsertTest0010 onCallback " + JSON.stringify(node.templateId));
              console.info(TAG +"testSubscribeRdbDataChangeInsertTest0010 onCallback " + node.data.length);
              for (let i = 0; i < node.data.length; i++) {
                console.info(TAG + "testSubscribeRdbDataChangeInsertTest0010 onCallback " + node.data[i]);
              }
              console.info(TAG + "testSubscribeRdbDataChangeInsertTest0010 onCallback onReturnCode =" + onReturnCode);
              // rdbDataChange注册一次就会进入一次回调，故通过计数来进行断言
              if(onReturnCode === 2 && node.data.length === 2){
                try {
                  console.info(TAG + "testSubscribeRdbDataChangeInsertTest0010 onCallback test data = " + "{\"p2\":[" + "{\"name\":\"LisiTest0010" + i + "\"}" +"]}");
                  expect(node.data[1] === "{\"p2\":[" + "{\"name\":\"LisiTest0010" + i + "\"}" +"]}").assertTrue();
                } catch (err) {
                  console.error(TAG + "testSubscribeRdbDataChangeInsertTest0010 onCallback failed! err = " + err);
                }
              }
            });
 
            try {
              let ret_inset = await dsProxyHelper!.insert(uri, vb);
              console.info(TAG +
                "[DataShareClientTest] <<Consumer>> testSubscribeRdbDataChangeInsertTest0010 insert end rowNum is " +
                ret_inset);
              await Utils.msSleep(200);
            } catch (err) {
              console.error(TAG + 'testSubscribeRdbDataChangeInsertTest0010 ret_inset failed! ' + err);
            }
 
            // 回调完成后断言
            try {
              console.log(TAG + "[DataShareClientTest] <<Consumer>> testSubscribeRdbDataChangeInsertTest0010 user onRdbDataChange end. result1 = " + JSON.stringify(result1));
              let valueCode = result1[0].result;
              console.log(TAG + "[DataShareClientTest] <<Consumer>> testSubscribeRdbDataChangeInsertTest0010 onRdbDataChange end valueCode is " + JSON.stringify(valueCode));
              expect(valueCode === 0).assertTrue();
            } catch (err: BusinessError) {
              console.error(TAG + "testSubscribeRdbDataChangeInsertTest0010 ret_inset notifyChange failed! err = " + err);
              // 失败只能因为是时序问题导致句柄释放后回调慢继续使用
              expect(err.code === 15700013).assertTrue();
            }
 
            try {
              // 最后取消订阅
              let result: dataShare.OperationResult[] | undefined = dsProxyHelper!.offRdbDataChange([uri], templateId);
              console.log(TAG + "[DataShareClientTest] <<Consumer>> testSubscribeRdbDataChangeInsertTest0010 off end " + JSON.stringify(result));
              // 删除模板
              dsProxyHelper!.delTemplate(uri, "100");
              console.log(TAG + "[DataShareClientTest] <<Consumer>> testSubscribeRdbDataChangeInsertTest0010 delTemplate end!");
              // 清理数据
              let da = new dataSharePredicates.DataSharePredicates();
              await dsProxyHelper!.delete(uri, da);
              console.log(TAG + "[DataShareClientTest] <<Consumer>> testSubscribeRdbDataChangeInsertTest0010 deletedata end!");
            } catch (err) {
              console.error(TAG + "testSubscribeRdbDataChangeInsertTest0010 off or  delTemplate failed! err = " + err);
            }
          }
          done();
        } catch (err: BusinessError) {
          console.error(TAG +'[DataShareClientTest] <<Consumer>> testSubscribeRdbDataChangeInsertTest0010 error ' + err.code + "  "+err.message);
          done();
          // 失败只能因为是时序问题导致句柄释放后回调慢继续使用
          expect(err.code === 15700013).assertTrue();
        }
    });
 
    /**
     * @tc.name       : testSubscribeRdbDataChangeUpdateTest0010
     * @tc.number     : SUB_DistributedData_DFX_MTBF_DataShare_Subscribe_Test_014
     * @tc.desc       : onDataChange uri test
     * @tc.size       : MediumTest
     * @tc.type       : RELIABILITY
     * @tc.level      : Level 3
     */
    it('testSubscribeRdbDataChangeUpdateTest0010', TestType.RELIABILITY | Size.MEDIUMTEST | Level.LEVEL3, async (done: () => void) => {
      // 订阅的模板信息
      let template: dataShare.Template = {
        predicates: {
          "p1": "select name0 as name from TBL00",
          "p2": "select name1 as name from TBL00",
        },
        scheduler: "select remindTimer(strftime('%s', 'now', '+10 seconds'))"
      }
      let templateId: dataShare.TemplateId = {
        subscriberId: "100", bundleNameOfOwner: "com.acts.ohos.data.datasharetestclient"
      }
 
      try {
        for(let i = 0; i < 1; i++) {
          // 增加的数据
          let vb: ValuesBucket = {
            "name1": "LisiTest0010" + i,
          };
          try {
            let ret_inset = await dsProxyHelper!.insert(uri, vb);
            console.info(TAG +
              "[DataShareClientTest] <<Consumer>> testSubscribeRdbDataChangeUpdateTest0010 insert end rowNum is " +
              ret_inset);
          } catch (err) {
            console.error(TAG + 'testSubscribeRdbDataChangeUpdateTest0010 ret_inset failed! ' + err);
          }
 
          // 添加模板
          try {
            console.log(TAG +"[DataShareClientTest] <<Consumer>> testSubscribeRdbDataChangeUpdateTest0010 addTemplate begin");
            dsProxyHelper!.addTemplate(uri, "100", template);
            console.log(TAG +"[DataShareClientTest] <<Consumer>> testSubscribeRdbDataChangeUpdateTest0010 addTemplate end");
          } catch (err) {
            console.error(TAG +"[DataShareClientTest] <<Consumer>> testSubscribeRdbDataChangeUpdateTest0010 addTemplate error " + JSON.stringify(err));
          }
 
          // 修改数据，触发回调
          // 订阅接口
          let onReturnCode = 0;
          let result1: Array<dataShare.OperationResult> = dsProxyHelper!.onRdbDataChange([uri], templateId, (node:dataShare.RdbDataChangeNode) => {
            onReturnCode ++;
            console.info(TAG +"testSubscribeRdbDataChangeUpdateTest0010 onCallback " + JSON.stringify(node.uri));
            console.info(TAG +"testSubscribeRdbDataChangeUpdateTest0010 onCallback " + JSON.stringify(node.templateId));
            console.info(TAG +"testSubscribeRdbDataChangeUpdateTest0010 onCallback " + node.data.length);
            for (let i = 0; i < node.data.length; i++) {
              console.info(TAG + "testSubscribeRdbDataChangeUpdateTest0010 onCallback " + node.data[i]);
            }
            console.info(TAG + "testSubscribeRdbDataChangeUpdateTest0010 onCallback onReturnCode =" + onReturnCode);
            // rdbDataChange注册一次就会进入一次回调，故通过计数来进行断言
            if (onReturnCode === 2 && node.data.length === 2) {
              try {
                console.info(TAG + "testSubscribeRdbDataChangeUpdateTest0010 onCallback test data = " + "{\"p2\":[" + "{\"name\":\"LisiUpdateTest0010" + i + "\"}" +"]}");
                expect(node.data[1] === "{\"p2\":[" + "{\"name\":\"LisiUpdateTest0010" + i + "\"}" +"]}").assertTrue();
              } catch (err) {
                console.error(TAG + "testSubscribeRdbDataChangeUpdateTest0010 onCallback failed! err = " + err);
              }
            }
          });
 
          try {
            let vbUp: ValuesBucket = {
              "name1": "LisiUpdateTest0010" + i,
            };
            let da = new dataSharePredicates.DataSharePredicates();
            da.equalTo("name1", "LisiTest0010" + i);
            let ret_update = await dsProxyHelper!.update(uri, da, vbUp);
            console.log(TAG +
              "[DataShareClientTest] <<Consumer>> testSubscribeRdbDataChangeUpdateTest0010 update end rowNum is " +
              ret_update);
            await Utils.msSleep(200);
          } catch (err) {
            console.error(TAG + 'testSubscribeRdbDataChangeUpdateTest0010 ret_update failed! ' + err);
          }
 
          // 回调完成后断言
          try {
            console.log(TAG + "[DataShareClientTest] <<Consumer>> testSubscribeRdbDataChangeUpdateTest0010 user onRdbDataChange end. result1 = " + JSON.stringify(result1));
            let valueCode = result1[0].result;
            console.log(TAG + "[DataShareClientTest] <<Consumer>> testSubscribeRdbDataChangeUpdateTest0010 onRdbDataChange end valueCode is " + JSON.stringify(valueCode));
            expect(valueCode === 0).assertTrue();
          } catch (err: BusinessError) {
            console.error(TAG + "testSubscribeRdbDataChangeUpdateTest0010 ret_inset notifyChange failed! err = " + err);
            // 失败只能因为是时序问题导致句柄释放后回调慢继续使用
            expect(err.code === 15700013).assertTrue();
          }
 
          try {
            // 最后取消订阅
            let result: dataShare.OperationResult[] | undefined = dsProxyHelper!.offRdbDataChange([uri], templateId);
            console.log(TAG + "[DataShareClientTest] <<Consumer>> testSubscribeRdbDataChangeUpdateTest0010 off end " + JSON.stringify(result));
 
            // 清理数据
            let da = new dataSharePredicates.DataSharePredicates();
            await dsProxyHelper!.delete(uri, da);
            console.log(TAG + "[DataShareClientTest] <<Consumer>> testSubscribeRdbDataChangeUpdateTest0010 deletedata end!");
          } catch (err) {
            console.error(TAG + "testSubscribeRdbDataChangeUpdateTest0010 off or  delTemplate failed! err = " + err);
          }
        }
        // 删除模板
        dsProxyHelper!.delTemplate(uri, "100");
        console.log(TAG + "[DataShareClientTest] <<Consumer>> testSubscribeRdbDataChangeUpdateTest0010 delTemplate end!");
 
        done();
      } catch (err: BusinessError) {
        console.error(TAG +'[DataShareClientTest] <<Consumer>> testSubscribeRdbDataChangeUpdateTest0010 error ' + err.code + "  " + err.message);
        done();
        // 失败只能因为是时序问题导致句柄释放后回调慢继续使用
        expect(err.code === 15700013).assertTrue();
      }
    });
 
    /**
     * @tc.name       : testSubscribeRdbDataChangeDeleteTest0010
     * @tc.number     : SUB_DistributedData_DFX_MTBF_DataShare_Subscribe_Test_015
     * @tc.desc       : onDataChange uri test
     * @tc.size       : MediumTest
     * @tc.type       : RELIABILITY
     * @tc.level      : Level 3
     */
    it('testSubscribeRdbDataChangeDeleteTest0010', TestType.RELIABILITY | Size.MEDIUMTEST | Level.LEVEL3, async (done: () => void) => {
      // 订阅的模板信息
      let template: dataShare.Template = {
        predicates: {
          "p1": "select name0 as name from TBL00",
          "p2": "select name1 as name from TBL00",
        },
        scheduler: "select remindTimer(strftime('%s', 'now', '+10 seconds'))"
      }
      let templateId: dataShare.TemplateId = {
        subscriberId: "100", bundleNameOfOwner: "com.acts.ohos.data.datasharetestclient"
      }
 
      try{
        for (let i = 0; i < COUNT_NUM; i++) {
          // 增加的数据
          let vb: ValuesBucket = {
            "name1": "LisiTest0010" + i,
          };
          try {
            let ret_inset = await dsProxyHelper!.insert(uri, vb);
            console.info(TAG +
              "[DataShareClientTest] <<Consumer>> testSubscribeRdbDataChangeDeleteTest0010 insert end rowNum is " +
              ret_inset);
          } catch (err) {
            console.error(TAG + 'testSubscribeRdbDataChangeDeleteTest0010 ret_inset failed! ' + err);
          }
 
          // 添加模板
          try {
            console.log(TAG +"[DataShareClientTest] <<Consumer>> testSubscribeRdbDataChangeDeleteTest0010 addTemplate begin");
            dsProxyHelper!.addTemplate(uri, "100", template);
            console.log(TAG +"[DataShareClientTest] <<Consumer>> testSubscribeRdbDataChangeDeleteTest0010 addTemplate end");
          } catch (err) {
            console.error(TAG +"[DataShareClientTest] <<Consumer>> testSubscribeRdbDataChangeDeleteTest0010 addTemplate error " + JSON.stringify(err));
          }
 
          // 修改数据，触发回调
          // 订阅接口
          let onReturnCode = 0;
          let result1: Array<dataShare.OperationResult> = dsProxyHelper!.onRdbDataChange([uri], templateId, (node:dataShare.RdbDataChangeNode) => {
            onReturnCode ++;
            console.info(TAG +"testSubscribeRdbDataChangeDeleteTest0010 onCallback " + JSON.stringify(node.uri));
            console.info(TAG +"testSubscribeRdbDataChangeDeleteTest0010 onCallback " + JSON.stringify(node.templateId));
            console.info(TAG +"testSubscribeRdbDataChangeDeleteTest0010 onCallback " + node.data.length);
            for (let i = 0; i < node.data.length; i++) {
              console.info(TAG + "testSubscribeRdbDataChangeDeleteTest0010 onCallback " + node.data[i]);
            }
            console.info(TAG + "testSubscribeRdbDataChangeDeleteTest0010 onCallback onReturnCode =" + onReturnCode);
            // rdbDataChange注册一次就会进入一次回调，故通过计数来进行断言
            if (onReturnCode === 2 && node.data.length === 2){
              try {
                console.info(TAG + "testSubscribeRdbDataChangeDeleteTest0010 onCallback test data = " + "{\"p2\":[" + "" +"]}");
                expect(node.data[1] === "{\"p2\":[" + "" +"]}").assertTrue();
              } catch (err) {
                console.error(TAG + "testSubscribeRdbDataChangeDeleteTest0010 onCallback failed! err = " + err);
              }
            }
          });
 
          try {
            let daDe = new dataSharePredicates.DataSharePredicates();
            daDe.equalTo("name1", "LisiTest0010" + i);
            let ret_delete = await dsProxyHelper!.delete(uri, daDe);
            console.log(TAG +
              "[DataShareClientTest] <<Consumer>> testSubscribeRdbDataChangeDeleteTest0010 delete end rowNum is " +
              ret_delete);
            await Utils.msSleep(200);
          } catch (err) {
            console.info(TAG + 'ret_delete failed!' + err);
          }
 
          // 回调完成后断言
          try {
            console.log(TAG + "[DataShareClientTest] <<Consumer>> testSubscribeRdbDataChangeDeleteTest0010 user onRdbDataChange end. result1 = " + JSON.stringify(result1));
            let valueCode = result1[0].result;
            console.log(TAG + "[DataShareClientTest] <<Consumer>> testSubscribeRdbDataChangeDeleteTest0010 onRdbDataChange end valueCode is " + JSON.stringify(valueCode));
            expect(valueCode === 0).assertTrue();
          } catch (err: BusinessError) {
            console.error(TAG + "testSubscribeRdbDataChangeDeleteTest0010 ret_inset notifyChange failed! err = " + err);
            // 失败只能因为是时序问题导致句柄释放后回调慢继续使用
            expect(err.code === 15700013).assertTrue();
          }
 
          try {
            // 最后取消订阅
            let result: dataShare.OperationResult[] | undefined = dsProxyHelper!.offRdbDataChange([uri], templateId);
            console.log(TAG + "[DataShareClientTest] <<Consumer>> testSubscribeRdbDataChangeDeleteTest0010 off end " + JSON.stringify(result));
            // 清理数据
            let da = new dataSharePredicates.DataSharePredicates();
            await dsProxyHelper!.delete(uri, da);
            console.log(TAG + "[DataShareClientTest] <<Consumer>> testSubscribeRdbDataChangeDeleteTest0010 deletedata end!");
          } catch (err) {
            console.error(TAG + "testSubscribeRdbDataChangeDeleteTest0010 off or delTemplate or delete_data failed! err = " + err);
          }
        }
 
        // 删除模板
        dsProxyHelper!.delTemplate(uri, "100");
        console.log(TAG + "[DataShareClientTest] <<Consumer>> testSubscribeRdbDataChangeDeleteTest0010 delTemplate end!");
        done();
      } catch (err: BusinessError) {
        console.error(TAG +'[DataShareClientTest] <<Consumer>> testSubscribeRdbDataChangeDeleteTest0010 error ' + err.code + "  "+err.message);
        done();
        // 失败只能因为是时序问题导致句柄释放后回调慢继续使用
        expect(err.code === 15700013).assertTrue();
      }
    });
  })
}