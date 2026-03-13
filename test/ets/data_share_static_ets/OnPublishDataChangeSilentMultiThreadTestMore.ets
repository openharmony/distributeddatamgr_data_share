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
 
export default function onPublishDataChangeSilentMultiThreadTestMore() {
  describe('onPublishDataChangeSilentMultiThreadTestMore', () => {
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
 
    beforeEach(() => {
    })
 
    afterEach(() => {
      dsProxyHelper?.offPublishedDataChange([uri], "11");
    })
 
    afterAll(async () => {
      await dsProxyHelper!.close();
    })
 
    /**
     * @tc.name       : testSubscribeMultiThreadTest0150
     * @tc.number     : SUB_DistributedData_DFX_Concurrent_DataShare_SubscribeMultiThreadTest_0150
     * @tc.desc       : onDataChange and onDataChange thread
     * @tc.size       : MediumTest
     * @tc.type       : Function
     * @tc.level      : Level 3
     */
    it('testSubscribeMultiThreadTest0150', TestType.RELIABILITY | Size.MEDIUMTEST | Level.LEVEL3, async () => { //impl problem
      for (let i = 0; i < 10; i++) {
        let value: Array<int> = new Array<int>();
        let taskGroupName: string = "SubscribeMultiThread0150";
        let taskCloseGroup: taskpool.TaskGroup = new taskpool.TaskGroup(taskGroupName);
        let task9_onDataChange1: taskpool.Task = new taskpool.Task(onPublishDataChangeTest1, context,value); // 订阅数据
        let task9_dataCRUDTest: taskpool.Task = new taskpool.Task(dataCRUDTest, context); // 订阅数据
        taskCloseGroup.addTask(task9_onDataChange1)
        taskCloseGroup.addTask(task9_dataCRUDTest)
        await taskpool.execute(taskCloseGroup).then((data: Array<Any>) => {
          //data是线程调用后的返回值，按照添加任务时的顺序一次放进数组中，再取出来进行断言
          console.info(TAG + "seqrunner: testSubscribeMultiThreadTest0150 done.data is :" + JSON.stringify(data));
          let a1 = data[0] as Object[];
          expect(a1[0] == 0).assertTrue()
          expect(a1[1] == 0 || a1[1] == 1003 || a1[1] == 1047).assertTrue() //未找到on调用off，客户端拦截报1003状态码，服务端拦截报1047错误码
        }).catch((err) => {
          console.info(TAG + "Failed to testSubscribeMultiThreadTest0150. err " + JSON.stringify(err));
          expect().assertFail()
        })
      }
    });
  })
}
 
/**
 * onPublishDataChange支持静默访问
 * @param context
 * @param value:用来保存on、off的调用结果
 * @returns :将on、off的调用结果返回到线程执行完的回调函数中
 */
async function onPublishDataChangeTest1(context: common.UIAbilityContext, value: Array<int>): Promise<Array<int>> {
  const TAG = "[ttt-onPublishDataChangeTest] "
  value = new Array<int>();
  let dsProxyHelper: dataShare.DataShareHelper | undefined = undefined
  let uri = ("datashareproxy://com.acts.ohos.data.datasharetest.static/test");
  try {
    dsProxyHelper = await dataShare.createDataShareHelper(context, uri, { isProxy: true });
    console.log(TAG + "[DataShareClientTest] <<Consumer>> dsProxyHelper " + JSON.stringify(dsProxyHelper));
  } catch (err: BusinessError) {
    console.error(TAG + '[DataShareClientTest] <<Consumer>> createDataShareHelper error' + err.code + "  " + err.message);
  }
  let uris:Array<string> = ["city", uri];
  try {
    console.log(TAG + "[DataShareClientTest] <<Consumer>> on begin");
    let result1: Array<dataShare.OperationResult> = dsProxyHelper!.onPublishedDataChange(uris, "11", (node:dataShare.PublishedDataChangeNode) => {
      console.info(TAG + "[DataShareClientTest] [on callback] publishedData change ININ ");
      console.info(TAG + "[DataShareClientTest] [on callback] data size" + node.data.length);
    });
    console.log(TAG + "[DataShareClientTest] <<Consumer>> on end " + JSON.stringify(result1));
    let valueCode = result1[0].result;
    console.log(TAG + "[DataShareClientTest] <<Consumer>> on end valueCode is " + JSON.stringify(valueCode));
    if(value != undefined){
      value.push(valueCode)
      console.log(TAG + "[DataShareClientTest] <<Consumer>> on end value is " + JSON.stringify(value));
    }
  } catch (err: BusinessError) {
    console.error(TAG + "[DataShareClientTest] <<Consumer>> on err " + err.code + "  " + err.message);
  }
  await new Promise<string>((resolve) => {
    setTimeout(() => {
      resolve("ok")
    }, 3000)
  });
 
  try{
    let result2: Array<dataShare.OperationResult> = dsProxyHelper!.offPublishedDataChange([uri], "11");
    console.log(TAG + "[DataShareClientTest] <<Consumer>> off end " + JSON.stringify(result2));
    let valueCode = result2[0].result;
    console.log(TAG + "[DataShareClientTest] <<Consumer>> off end valueCode is " + JSON.stringify(valueCode));
    if(value != undefined){
      value.push(valueCode)
      console.log(TAG + "[DataShareClientTest] <<Consumer>> off end value is " + JSON.stringify(value));
    }
  }catch(err: BusinessError){
    console.error(TAG + `[DataShareClientTest] off error: code: ${err.code}, message: ${err.message} `);
  }
  dsProxyHelper?.close();
  return value;
}
 
/**
 * publish接口发布多条数据
 * @param context
 */
async function dataCRUDTest(context: common.UIAbilityContext): Promise<void> {
  const TAG = "[ttt-dataCRUDTest] "
  let dsProxyHelper: dataShare.DataShareHelper | undefined = undefined
  let uri = ("datashareproxy://com.acts.ohos.data.datasharetest.static/test");
  try {
    dsProxyHelper = await dataShare.createDataShareHelper(context, uri, { isProxy: true });
    console.log(TAG + "[DataShareClientTest] <<Consumer>> dsProxyHelper " + JSON.stringify(dsProxyHelper));
  } catch (err: BusinessError) {
    console.error(TAG + '[DataShareClientTest] <<Consumer>> createDataShareHelper error' + err.code + "  " + err.message);
  }
  let data : Array<dataShare.PublishedItem> = [
    {key: uri, subscriberId:"11", data:"nobody sub " + Math.random()},
    {key: "city1", subscriberId:"11", data:"nobody sub " + Math.random()},
    {key: "city2", subscriberId:"11", data:"nobody sub " + Math.random()},
    {key: "city3", subscriberId:"11", data:"nobody sub " + Math.random()}];
  try {
    console.log(TAG + "[DataShareClientTest] <<Consumer>> data length is:", data.length);
    let result: Array<dataShare.OperationResult> = await dsProxyHelper!.publish(data, "com.acts.ohos.data.datasharetest");
    console.log(TAG + "[DataShareClientTest] <<Consumer>> publish end " + JSON.stringify(result));
    let publishedData:Array<dataShare.PublishedItem> = await dsProxyHelper!.getPublishedData("com.acts.ohos.data.datasharetest")
    console.log(TAG + "[DataShareClientTest] <<Consumer>> getPublishedData end " + JSON.stringify(publishedData));
  } catch (err: BusinessError) {
    console.error(TAG + "[DataShareClientTest] <<Consumer>> publish error " + err.code + "  " + err.message);
  }
  dsProxyHelper?.close();
}
 