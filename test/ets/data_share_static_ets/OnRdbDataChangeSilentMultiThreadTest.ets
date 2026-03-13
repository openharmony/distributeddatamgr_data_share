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
import { describe, it, expect, TestType, Size, Level, beforeAll, beforeEach, afterEach, afterAll } from "../../../hypium/index";
import dataShare from '@ohos.data.dataShare'
import { ValuesBucket } from '@ohos.data.ValuesBucket';
import dataSharePredicates from '@ohos.data.dataSharePredicates'
import { BusinessError } from '@ohos.base';
import Utils from './Util.test';
import { AppStorage } from '@ohos.arkui.stateManagement'
import common from '@ohos.app.ability.common';
import UIAbility from '@ohos.app.ability.UIAbility';
// import { collections } from '@kit.ArkTS';
 
let context: common.UIAbilityContext;
let uri = ("datashareproxy://com.acts.ohos.data.datasharetest.static/test");
let dsProxyHelper: dataShare.DataShareHelper | undefined = undefined
const TAG = "[ttt] "
 
export default function onRdbDataChangeSilentMultiThreadTest() {
  describe('onRdbDataChangeSilentMultiThreadTest', () => {
    beforeAll(async () => {
      try {
        context = AppStorage.get<common.UIAbilityContext>("context") as common.UIAbilityContext;
        dsProxyHelper = await dataShare.createDataShareHelper(context, uri, { isProxy: true });
        console.log("[ttt] [DataShareClientTest] <<Consumer>> dsProxyHelper " + JSON.stringify(dsProxyHelper));
      } catch (err: BusinessError) {
        console.error('[ttt] [DataShareClientTest] <<Consumer>> createDataShareHelper error' + err.code + "  " + err.message);
        expect().assertFail();
      }
      await Utils.msSleep(4000);
      for (let i = 0; i < 200; i++) {
        let vb: ValuesBucket = {
          "name0": "wangwu" + i,
        };
        try {
          let ret = await dsProxyHelper!.insert(uri, vb);
          console.log("[ttt-main] [DataShareClientTest] <<Consumer>> insert end rowNum is " + ret);
        } catch (err: BusinessError) {
          console.error('[ttt-main] [DataShareClientTest] <<Consumer>> insert error' + err.code + "  " + err.message);
          expect().assertFail()
        }
      }
    })
 
    beforeEach(() => {
    })
 
    afterEach(() => {
      let templateId:dataShare.TemplateId = {subscriberId:"100", bundleNameOfOwner:"com.acts.ohos.data.datasharetestclient"}
      dsProxyHelper?.offRdbDataChange([uri], templateId);
    })
 
    afterAll(async () => {
      await dsProxyHelper!.close();
    })
 
    /**
     * @tc.name       : testSubscribeMultiThreadTest0080
     * @tc.number     : SUB_DistributedData_DFX_Concurrent_DataShare_SubscribeMultiThreadTest_0080
     * @tc.desc       : onDataChange and onDataChange thread
     * @tc.size       : MediumTest
     * @tc.type       : Function
     * @tc.level      : Level 3
     */
//    it('testSubscribeMultiThreadTest0080', TestType.RELIABILITY | Size.MEDIUMTEST | Level.LEVEL3, async () => { // impl problem
//      for (let i = 0; i < 10; i++) {
//        // let value: collections.Array<number> = new collections.Array<number>();
//        let value: Array<int> = new Array<int>();
//        let taskGroupName: string = "SubscribeMultiThread0080";
//        let taskCloseGroup: taskpool.TaskGroup = new taskpool.TaskGroup(taskGroupName);
//        let task9_onDataChange1: taskpool.Task = new taskpool.Task(onRdbDataChangeTest1, context, value); // 订阅数据
//        let task9_onDataChange2: taskpool.Task = new taskpool.Task(onRdbDataChangeTest1, context, value); // 订阅数据
//        let task9_dataCRUDTest: taskpool.Task = new taskpool.Task(dataCRUDTest, context, i); // 订阅数据
//        let task9_offDataChangeTest1: taskpool.Task = new taskpool.Task(offRdbDataChangeTest1, context, value); // 订阅数据
//        taskCloseGroup.addTask(task9_onDataChange1)
//        taskCloseGroup.addTask(task9_onDataChange2)
//        taskCloseGroup.addTask(task9_dataCRUDTest)
//        taskCloseGroup.addTask(task9_offDataChangeTest1)
//        await taskpool.execute(taskCloseGroup).then((data: Array<Any>) => {
//          //data是线程调用后的返回值，按照添加任务时的顺序一次放进数组中，再取出来进行断言
//          console.info(TAG + "seqrunner: testSubscribeMultiThreadTest0080 done. data is :" + JSON.stringify(data));
//          let a1 = data[0] as Object[];
//          let a2 = data[1] as Object[];
//          let a4 = data[3] as Object[];
//          expect(a1[0] == 0).assertTrue()
//          expect(a2[0] == 0).assertTrue()
//          expect(a4[0] == 1003).assertTrue()
//        }).catch((err) => {
//          console.info(TAG + "Failed to testSubscribeMultiThreadTest0080. err " + JSON.stringify(err));
//          expect().assertFail()
//        })
//      }
//    });
  })
}
 
/**
 * onRdbDataChange支持静默访问
 * @param context
 * @param value:用来保存on、off的调用结果
 * @returns :将on、off的调用结果返回到线程执行完的回调函数中
 */
async function onRdbDataChangeTest1(context: common.UIAbilityContext, value:Array<int>): Promise<Array<int>> {
  const TAG = "[ttt-onRdbDataChangeTest] "
  value = new Array<int>();
  let dsProxyHelper: dataShare.DataShareHelper | undefined = undefined
  let uri = ("datashareproxy://com.acts.ohos.data.datasharetest.static/test");
  try {
    dsProxyHelper = await dataShare.createDataShareHelper(context, uri, { isProxy: true });
    console.log(TAG +"[DataShareClientTest] <<Consumer>> dsProxyHelper " + JSON.stringify(dsProxyHelper));
  } catch (err: BusinessError) {
    console.error(TAG +'[DataShareClientTest] <<Consumer>> createDataShareHelper error' + err.code + "  " + err.message);
    expect().assertFail();
  }
  let template: dataShare.Template = {
    predicates: {
      "p1": "select name0 as name from TBL00",
      "p2": "select name1 as name from TBL00",
    },
    scheduler: "select remindTimer(strftime('%s', 'now', '+10 seconds'))"
  }
  try {
    console.log(TAG +"[DataShareClientTest] <<Consumer>> addTemplate begin");
    dsProxyHelper!.addTemplate(uri, "100", template);
    console.log(TAG +"[DataShareClientTest] <<Consumer>> addTemplate end");
  } catch (err: BusinessError) {
    console.error(TAG +"[DataShareClientTest] <<Consumer>> addTemplate error " + JSON.stringify(err));
    expect().assertFail()
  }
  let templateId: dataShare.TemplateId = {
    subscriberId: "100", bundleNameOfOwner: "com.acts.ohos.data.datasharetestclient"
  }
 
  try {
    let result1: Array<dataShare.OperationResult> = dsProxyHelper!.onRdbDataChange([uri], templateId, (node:dataShare.RdbDataChangeNode) => {
      console.info(TAG +"***** notifyChange *****");
      console.info(TAG +"onCallback " + JSON.stringify(node.uri));
      console.info(TAG +"onCallback " + JSON.stringify(node.templateId));
      console.info(TAG +"onCallback " + node.data.length);
      for (let i = 0; i < node.data.length; i++) {
        console.info("onCallback " + typeof node.data[i] + " " + node.data[i]);
      }
    });
    console.log("[DataShareClientTest] <<Consumer>> user onRdbDataChange end " + JSON.stringify(result1));
    let valueCode = result1[0].result;
    console.log(TAG + "[DataShareClientTest] <<Consumer>> onRdbDataChange end valueCode is " + JSON.stringify(valueCode));
    if(value != undefined){
      value.push(valueCode)
      console.log(TAG + "[DataShareClientTest] <<Consumer>> onRdbDataChange end value is " + JSON.stringify(value));
    }
  } catch (err: BusinessError) {
    console.error(TAG + '[DataShareClientTest] <<Consumer>> onRdbDataChange error code is ' + err.code + " message is " + err.message);
  }
  await new Promise<string>((resolve) => {
    setTimeout(() => {
      resolve("ok")
    }, 3000)
  });
  dsProxyHelper?.close();
  return value;
}
 
/**
 * 子线程中调用off接口
 * @param context
 * @param value
 * @returns
 */
async function offRdbDataChangeTest1(context: common.UIAbilityContext, value: Array<int>): Promise<Array<int>> {
  const TAG = "[ttt-offRdbDataChangeTest] "
  value = new Array<int>();
  let dsProxyHelper: dataShare.DataShareHelper | undefined = undefined;
  let uri = ("datashareproxy://com.acts.ohos.data.datasharetest.static/test");
  try {
    dsProxyHelper = await dataShare.createDataShareHelper(context, uri, { isProxy: true });
    console.log(TAG + "[DataShareClientTest] <<Consumer>> dsProxyHelper " + JSON.stringify(dsProxyHelper));
  } catch (err: BusinessError) {
    console.error(TAG + '[DataShareClientTest] <<Consumer>> createDataShareHelper error' + err.code + " message is " + err.message);
  }
  try{
    let templateId:dataShare.TemplateId = {subscriberId:"100", bundleNameOfOwner:"com.acts.ohos.data.datasharetestclient"}
    let result: dataShare.OperationResult[] | undefined = dsProxyHelper!.offRdbDataChange([uri], templateId);
    console.log(TAG + "[DataShareClientTest] <<Consumer>> off end " + JSON.stringify(result));
    let valueCode = result[0].result;
    console.log(TAG + "[DataShareClientTest] <<Consumer>> onRdbDataChange end valueCode is " + JSON.stringify(valueCode));
    if(value != undefined){
      value.push(valueCode)
      console.log(TAG + "[DataShareClientTest] <<Consumer>> onRdbDataChange end value is " + JSON.stringify(value));
    }
  } catch (err: BusinessError) {
    console.error(TAG + '[DataShareClientTest] <<Consumer>> off error' + err.code + "  "+err.message);
  }
  dsProxyHelper?.close();
  return value;
}
 
/**
 * 静默增删改查数据
 * @param context
 * @param index
 */
async function dataCRUDTest(context: common.UIAbilityContext, index: int): Promise<void> {
  let dsProxyHelper: dataShare.DataShareHelper | undefined = undefined
  let uri = ("datashareproxy://com.acts.ohos.data.datasharetest.static/test");
  try {
    dsProxyHelper = await dataShare.createDataShareHelper(context, uri, { isProxy: true });
    console.log("[ttt-DataCRUDTest] [DataShareClientTest] <<Consumer>> dsProxyHelper " + JSON.stringify(dsProxyHelper));
  } catch (err: BusinessError) {
    console.error('[ttt-DataCRUDTest] [DataShareClientTest] <<Consumer>> createDataShareHelper error' + err.code + "  " + err.message);
    expect().assertFail();
  }
  let vb: ValuesBucket = {
    "name0": "Tina" + index,
  };
  try {
    let ret = await dsProxyHelper!.insert(uri, vb);
    console.log("[ttt-DataCRUDTest] [DataShareClientTest] <<Consumer>> insert end rowNum is " + ret);
  } catch (err: BusinessError) {
    console.error('[ttt-DataCRUDTest] [DataShareClientTest] <<Consumer>> insert error' + err.code + "  " + err.message);
    expect().assertFail()
  }
 
  let vbUp: ValuesBucket = {
    "name0": "Tina",
  };
  try {
    let da = new dataSharePredicates.DataSharePredicates();
    da.equalTo("name0", "wangwu" + (index));
    let ret = await dsProxyHelper!.update(uri, da, vbUp);
    console.log("[ttt-DataCRUDTest] [DataShareClientTest] <<Consumer>> update end rowNum is " + ret);
  } catch (err: BusinessError) {
    console.error('[ttt-DataCRUDTest] [DataShareClientTest] <<Consumer>> update error' + err.code + "  " + err.message);
    expect().assertFail()
  }
 
  try {
    let daDe = new dataSharePredicates.DataSharePredicates();
    daDe.equalTo("name0", "wangwu" + (index + 100));
    let ret = await dsProxyHelper!.delete(uri, daDe);
    console.log("[ttt-DataCRUDTest] [DataShareClientTest] <<Consumer>> delete end rowNum is " + ret);
  } catch (err: BusinessError) {
    console.error('[ttt-DataCRUDTest] [DataShareClientTest] <<Consumer>> delete error' + err.code + "  " + err.message);
    expect().assertFail()
  }
  dsProxyHelper?.close();
}
 