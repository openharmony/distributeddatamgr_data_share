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
 
let context: common.UIAbilityContext;
let uri = ("datashareproxy://com.acts.ohos.data.datasharetest.static/test");
let dsProxyHelper: dataShare.DataShareHelper | undefined = undefined
const TAG = "[ttt] "
export default function onRdbDataChangeSilentMultiThreadTestSingle() {
  describe('onRdbDataChangeSilentMultiThreadTestSingle', () => {
    beforeAll(async () => {
      try {
        context = AppStorage.get<common.UIAbilityContext>("context") as common.UIAbilityContext;
        dsProxyHelper = await dataShare.createDataShareHelper(context, uri, { isProxy: true });
        console.log("[ttt] [DataShareClientTest] <<Consumer>> dsProxyHelper " + JSON.stringify(dsProxyHelper));
      } catch (err: BusinessError) {
        console.error('[ttt] [DataShareClientTest] <<Consumer>> createDataShareHelper error' + err.code + "  " + err.message);
        expect().assertFail();
      }
      await Utils.msSleep(2000);
    })
 
    beforeEach(async () => {
      for (let i = 0; i < 200; i++) {
        let vb: ValuesBucket = {
          "name0": "wangwu" + i,
          "name1": "wangwu",
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
 
    afterEach(async () => {
      let templateId:dataShare.TemplateId = {subscriberId:"100", bundleNameOfOwner:"com.acts.ohos.data.datasharetestclient"}
      dsProxyHelper?.offRdbDataChange([uri], templateId);
      let predicates = new dataSharePredicates.DataSharePredicates();
      let res = await dsProxyHelper!.delete(uri, predicates);
      console.info("[ttt] afterEach delete success:" + res);
    })
 
    afterAll(async () => {
      await dsProxyHelper!.close();
    })
 
    /**
     * @tc.name       : testSubscribeMultiThreadTest0120
     * @tc.number     : SUB_DistributedData_DFX_Concurrent_DataShare_SubscribeMultiThreadTest_0120
     * @tc.desc       : onDataChange and onDataChange thread
     * @tc.size       : MediumTest
     * @tc.type       : Function
     * @tc.level      : Level 3
     */
    it('testSubscribeMultiThreadTest0120', TestType.RELIABILITY | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      for (let i = 0; i < 10; i++) {
        let value: Array<int> = new Array<int>();
        let taskGroupName: string = "SubscribeMultiThread0120";
        let taskCloseGroup: taskpool.TaskGroup = new taskpool.TaskGroup(taskGroupName);
        let task9_onDataChange1: taskpool.Task = new taskpool.Task(onRdbDataChangeTest1, context, value); // 订阅数据
        let task9_onDataChange2: taskpool.Task = new taskpool.Task(onRdbDataChangeTest1, context, value); // 订阅数据
        let task9_dataCRUDTest: taskpool.Task = new taskpool.Task(dataInsertTest, context); // 订阅数据
        taskCloseGroup.addTask(task9_onDataChange1)
        taskCloseGroup.addTask(task9_onDataChange2)
        taskCloseGroup.addTask(task9_dataCRUDTest)
        await taskpool.execute(taskCloseGroup).then((data: Array<Any>) => {
          //data是线程调用后的返回值，按照添加任务时的顺序一次放进数组中，再取出来进行断言
          console.info(TAG + "seqrunner: testSubscribeMultiThreadTest0120 done.data is :" + JSON.stringify(data));
          let a1 = data[0] as Object[];
          let a2 = data[1] as Object[];
          expect(a1[0] == 0).assertTrue()
          expect(a1[1] == 0 || a1[1] == 1003 || a1[1] == 1047).assertTrue() //未找到on调用off，客户端拦截报1003状态码，服务端拦截报1047错误码
          expect(a2[0] == 0).assertTrue()
          expect(a2[1] == 0 || a2[1] == 1003 || a2[1] == 1047).assertTrue()
        }).catch((err) => {
          console.info(TAG + "Failed to testSubscribeMultiThreadTest0120. err " + JSON.stringify(err));
          expect().assertFail()
        })
      }
    });
 
    /**
     * @tc.name       : testSubscribeMultiThreadTest0130
     * @tc.number     : SUB_DistributedData_DFX_Concurrent_DataShare_SubscribeMultiThreadTest_0130
     * @tc.desc       : onDataChange and onDataChange thread
     * @tc.size       : MediumTest
     * @tc.type       : Function
     * @tc.level      : Level 3
     */
    it('testSubscribeMultiThreadTest0130', TestType.RELIABILITY | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      for (let i = 0; i < 10; i++) {
        let value: Array<int> = new Array<int>();
        let taskGroupName: string = "SubscribeMultiThread0130";
        let taskCloseGroup: taskpool.TaskGroup = new taskpool.TaskGroup(taskGroupName);
        let task9_onDataChange1: taskpool.Task = new taskpool.Task(onRdbDataChangeTest1, context, value); // 订阅数据
        let task9_onDataChange2: taskpool.Task = new taskpool.Task(onRdbDataChangeTest1, context, value); // 订阅数据
        let task9_dataCRUDTest: taskpool.Task = new taskpool.Task(dataUpdateTest, context, i); // 订阅数据
        taskCloseGroup.addTask(task9_onDataChange1)
        taskCloseGroup.addTask(task9_onDataChange2)
        taskCloseGroup.addTask(task9_dataCRUDTest)
        await taskpool.execute(taskCloseGroup).then((data: Array<Any>) => {
          //data是线程调用后的返回值，按照添加任务时的顺序一次放进数组中，再取出来进行断言
          console.info(TAG + "seqrunner: testSubscribeMultiThreadTest0130 done.data is :" + JSON.stringify(data));
          let a1 = data[0] as Object[];
          let a2 = data[1] as Object[];
          expect(a1[0] == 0).assertTrue()
          expect(a1[1] == 0 || a1[1] == 1003 || a1[1] == 1047).assertTrue() //未找到on直接调用off，客户端拦截报1003状态码，服务端拦截报1047错误码
          expect(a2[0] == 0).assertTrue()
          expect(a2[1] == 0 || a2[1] == 1003 || a2[1] == 1047).assertTrue()
        }).catch((err) => {
          console.info(TAG + "Failed to testSubscribeMultiThreadTest0130. err " + JSON.stringify(err));
          expect().assertFail()
        })
      }
    });
 
    /**
     * @tc.name       : testSubscribeMultiThreadTest0140
     * @tc.number     : SUB_DistributedData_DFX_Concurrent_DataShare_SubscribeMultiThreadTest_0140
     * @tc.desc       : onDataChange and onDataChange thread
     * @tc.size       : MediumTest
     * @tc.type       : Function
     * @tc.level      : Level 3
     */
    it('testSubscribeMultiThreadTest0140', TestType.RELIABILITY | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      for (let i = 0; i < 10; i++) {
        let value: Array<int> = new Array<int>();
        let taskGroupName: string = "SubscribeMultiThread0140";
        let taskCloseGroup: taskpool.TaskGroup = new taskpool.TaskGroup(taskGroupName);
        let task9_onDataChange1: taskpool.Task = new taskpool.Task(onRdbDataChangeTest1, context, value); // 订阅数据
        let task9_onDataChange2: taskpool.Task = new taskpool.Task(onRdbDataChangeTest1, context, value); // 订阅数据
        let task9_dataCRUDTest: taskpool.Task = new taskpool.Task(dataDeleteTest, context, i); // 订阅数据
        taskCloseGroup.addTask(task9_onDataChange1)
        taskCloseGroup.addTask(task9_onDataChange2)
        taskCloseGroup.addTask(task9_dataCRUDTest)
        await taskpool.execute(taskCloseGroup).then((data: Array<Any>) => {
          //data是线程调用后的返回值，按照添加任务时的顺序一次放进数组中，再取出来进行断言
          console.info(TAG + "seqrunner: testSubscribeMultiThreadTest0140 done.data is :"+JSON.stringify(data));
          let a1 = data[0] as Object[];
          let a2 = data[1] as Object[];
          expect(a1[0] == 0).assertTrue()
          expect(a1[1] == 0 || a1[1] == 1003 || a1[1] == 1047).assertTrue() //未找到on直接调用off，客户端拦截报1003状态码，服务端拦截报1047错误码
          expect(a2[0] == 0).assertTrue()
          expect(a2[1] == 0 || a2[1] == 1003 || a2[1] == 1047).assertTrue()
        }).catch((err) => {
          console.info(TAG + "Failed to testSubscribeMultiThreadTest0140. err " + JSON.stringify(err));
          expect().assertFail()
        })
      }
    });
  })
}
 
/**
 * onRdbDataChange支持静默访问
 * @param context
 * @param value:用来保存on、off的调用结果
 * @returns :将on、off的调用结果返回到线程执行完的回调函数中
 */
async function onRdbDataChangeTest1(context: common.UIAbilityContext, value: Array<int>): Promise<Array<int>> {
  const TAG = "[ttt-onRdbDataChangeTest] "
  value = new Array<int>();
  let dsProxyHelper: dataShare.DataShareHelper | undefined = undefined
  let uri = ("datashareproxy://com.acts.ohos.data.datasharetest.static/test");
  try {
    dsProxyHelper = await dataShare.createDataShareHelper(context, uri, { isProxy: true });
    console.log(TAG + "[DataShareClientTest] <<Consumer>> dsProxyHelper " + JSON.stringify(dsProxyHelper));
  } catch (err: BusinessError) {
    console.error(TAG + '[DataShareClientTest] <<Consumer>> createDataShareHelper error' + err.code + "  " + err.message);
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
    console.log(TAG + "[DataShareClientTest] <<Consumer>> addTemplate begin");
    dsProxyHelper!.addTemplate(uri, "100", template);
    console.log(TAG + "[DataShareClientTest] <<Consumer>> addTemplate end");
  } catch (err: BusinessError) {
    console.error(TAG + "[DataShareClientTest] <<Consumer>> addTemplate error " + JSON.stringify(err));
    expect().assertFail()
  }
  let templateId: dataShare.TemplateId = {
    subscriberId: "100", bundleNameOfOwner: "com.acts.ohos.data.datasharetestclient"
  }
 
  try {
    let result1: Array<dataShare.OperationResult> = dsProxyHelper!.onRdbDataChange([uri], templateId, (node:dataShare.RdbDataChangeNode) => {
      console.info(TAG + "***** notifyChange *****");
      console.info(TAG + "onCallback " + JSON.stringify(node.uri));
      console.info(TAG + "onCallback " + JSON.stringify(node.templateId));
      console.info(TAG + "onCallback " + node.data.length);
      for (let i = 0; i < node.data.length; i++) {
        console.info("onCallback " + typeof node.data[i] + " " + node.data[i]);
      }
    });
    console.log("TAG + [DataShareClientTest] <<Consumer>> onRdbDataChange end " + JSON.stringify(result1));
    let valueCode = result1[0].result;
    console.log(TAG + "[DataShareClientTest] <<Consumer>> onRdbDataChange end valueCode is " + JSON.stringify(valueCode));
    if(value != undefined){
      value.push(valueCode)
      console.log(TAG + "[DataShareClientTest] <<Consumer>> onRdbDataChange end value is " + JSON.stringify(value));
    }
  } catch (err: BusinessError) {
    console.error(TAG + '[DataShareClientTest] <<Consumer>> onRdbDataChange error code is ' + err.code + " message is " + err.message);
  }
 
  try{
    let templateId:dataShare.TemplateId = {subscriberId: "100", bundleNameOfOwner: "com.acts.ohos.data.datasharetestclient"}
    let result2: dataShare.OperationResult[] | undefined = dsProxyHelper!.offRdbDataChange([uri], templateId);
    console.log(TAG + "[DataShareClientTest] <<Consumer>> offRdbDataChang end " + JSON.stringify(result2));
    let valueCode2 = result2[0].result;
    console.log(TAG + "[DataShareClientTest] <<Consumer>> offRdbDataChang end valueCode is " + JSON.stringify(valueCode2));
    if(value != undefined){
      value.push(valueCode2)
      console.log(TAG + "[DataShareClientTest] <<Consumer>> offRdbDataChang end value is " + JSON.stringify(value));
    }
  } catch (err: BusinessError) {
    console.error(TAG + '[DataShareClientTest] <<Consumer>> offRdbDataChang error' + err.code + "  "+err.message);
  }
  dsProxyHelper?.close();
  return value;
}
 
 
async function dataInsertTest(context: common.UIAbilityContext) {
  const TAG = "[ttt-dataInsertTest] "
  let dsProxyHelper: dataShare.DataShareHelper | undefined = undefined
  let uri = ("datashareproxy://com.acts.ohos.data.datasharetest.static/test");
  try {
    dsProxyHelper = await dataShare.createDataShareHelper(context, uri, { isProxy: true });
    console.log(TAG + "[DataShareClientTest] <<Consumer>> dsProxyHelper " + JSON.stringify(dsProxyHelper));
  } catch (err: BusinessError) {
    console.error(TAG + '[DataShareClientTest] <<Consumer>> createDataShareHelper error' + err.code + "  " + err.message);
    expect().assertFail();
  }
  let vb: ValuesBucket = {
    "name0": "TinaInsert",
  };
  try {
    let ret = await dsProxyHelper!.insert(uri, vb);
    console.log(TAG + "[DataShareClientTest] <<Consumer>> insert end rowNum is " + ret);
  } catch (err: BusinessError) {
    console.error(TAG + '[DataShareClientTest] <<Consumer>> insert error' + err.code + "  " + err.message);
  }
  dsProxyHelper?.close();
}
 
async function dataUpdateTest(context: common.UIAbilityContext, index: int): Promise<void> {
  const TAG = "[ttt-dataUpdateTest] "
  let dsProxyHelper: dataShare.DataShareHelper | undefined = undefined
  let uri = ("datashareproxy://com.acts.ohos.data.datasharetest.static/test");
  try {
    dsProxyHelper = await dataShare.createDataShareHelper(context, uri, { isProxy: true });
    console.log(TAG + "[DataShareClientTest] <<Consumer>> dsProxyHelper " + JSON.stringify(dsProxyHelper));
  } catch (err: BusinessError) {
    console.error(TAG + '[DataShareClientTest] <<Consumer>> createDataShareHelper error' + err.code + "  " + err.message);
  }
  let vbUp: ValuesBucket = {
    "name1": "wangwuUpToTina",
  };
  try {
    let da = new dataSharePredicates.DataSharePredicates();
    da.equalTo("name0", "wangwu"+index);
    let ret = await dsProxyHelper!.update(uri, da, vbUp);
    console.log(TAG + "[DataShareClientTest] <<Consumer>> update end rowNum is " + ret);
  } catch (err: BusinessError) {
    console.error(TAG + '[DataShareClientTest] <<Consumer>> update error' + err.code + "  " + err.message);
  }
  dsProxyHelper?.close();
}
 
async function dataDeleteTest(context: common.UIAbilityContext, index: int): Promise<void> {
  const TAG = "[ttt-dataDeleteTest] "
  let dsProxyHelper: dataShare.DataShareHelper | undefined = undefined
  let uri = ("datashareproxy://com.acts.ohos.data.datasharetest.static/test");
  try {
    dsProxyHelper = await dataShare.createDataShareHelper(context, uri, { isProxy: true });
    console.log(TAG + "[DataShareClientTest] <<Consumer>> dsProxyHelper " + JSON.stringify(dsProxyHelper));
  } catch (err: BusinessError) {
    console.error(TAG + '[DataShareClientTest] <<Consumer>> createDataShareHelper error' + err.code + "  " + err.message);
  }
  try {
    let daDe = new dataSharePredicates.DataSharePredicates();
    daDe.equalTo("name0", "wangwu"+(index+100));
    let ret = await dsProxyHelper!.delete(uri, daDe);
    console.log(TAG + "[DataShareClientTest] <<Consumer>> delete end rowNum is " + ret);
  } catch (err: BusinessError) {
    console.error(TAG + '[DataShareClientTest] <<Consumer>> delete error' + err.code + "  " + err.message);
  }
  dsProxyHelper?.close();
}