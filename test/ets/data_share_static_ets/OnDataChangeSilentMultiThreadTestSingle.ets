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
export default function onDataChangeSilentMultiThreadTestSingle() {
  describe('onDataChangeSilentMultiThreadTestSingle', () => {
    beforeAll(async () => {
      try{
        context = AppStorage.get<common.UIAbilityContext>("context") as common.UIAbilityContext;
        dsProxyHelper = await dataShare.createDataShareHelper(context, uri, {isProxy:true});
        console.log("[ttt] [DataShareClientTest] <<Consumer>> dsProxyHelper " + JSON.stringify(dsProxyHelper));
      } catch (err: BusinessError) {
        console.error('[ttt] [DataShareClientTest] <<Consumer>> createDataShareHelper error' + err.code+ "  " + err.message);
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
      dsProxyHelper?.offDataChange(dataShare.SubscriptionType.SUBSCRIPTION_TYPE_EXACT_URI, uri);
      let predicates = new dataSharePredicates.DataSharePredicates();
      let res = await dsProxyHelper!.delete(uri, predicates);
      console.info("[ttt] afterEach delete success:" + res);
    })
 
    afterAll(async () => {
      await dsProxyHelper!.close();
    })
 
    /**
     * @tc.name       : testSubscribeMultiThreadTest0200
     * @tc.number     : SUB_DistributedData_DFX_Concurrent_DataShare_SubscribeMultiThreadTest_0200
     * @tc.desc       : onDataChange and onDataChange thread
     * @tc.size       : MediumTest
     * @tc.type       : Function
     * @tc.level      : Level 3
     */
    it('testSubscribeMultiThreadTest0200', TestType.RELIABILITY | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      for(let i = 0; i < 10; i++){
        let value: Array<int> = new Array<int>();
        let taskGroupName: string = "SubscribeMultiThread0200";
        let taskCloseGroup: taskpool.TaskGroup = new taskpool.TaskGroup(taskGroupName);
        let task9_onDataChange1: taskpool.Task = new taskpool.Task(onDataChangeTest1, context, value); // 订阅数据
        let task9_dataCRUDTest: taskpool.Task = new taskpool.Task(dataInsertTest, context, i);// 新增单条数据
        let task9_onDataChange2: taskpool.Task = new taskpool.Task(onDataChangeTest1, context, value);// 订阅数据
        taskCloseGroup.addTask(task9_onDataChange1)
        taskCloseGroup.addTask(task9_dataCRUDTest)
        taskCloseGroup.addTask(task9_onDataChange2)
        await taskpool.execute(taskCloseGroup).then((data: Array<Any>) => {
          //data是线程调用后的返回值，按照添加任务时的顺序一次放进数组中，再取出来进行断言
          console.info(TAG + "seqrunner: testSubscribeMultiThreadTest0200 done.data is :"+JSON.stringify(data));
          let a1 = data[0] as Object[];
          let a3 = data[2] as Object[];
          expect(a1[0] == 0 || a1[0] == -1).assertTrue()
          expect(a3[0] == 0 || a3[0] == -1).assertTrue()
          console.info(TAG + "seqrunner: testSubscribeMultiThreadTest0200 done.");
        }).catch((err) => {
          console.info(TAG + "Failed to testSubscribeMultiThreadTest0200. err " + JSON.stringify(err));
          expect().assertFail()
        })
      }
    });
 
    /**
     * @tc.name       : testSubscribeMultiThreadTest0210
     * @tc.number     : SUB_DistributedData_DFX_Concurrent_DataShare_SubscribeMultiThreadTest_0210
     * @tc.desc       : onDataChange and onDataChange thread
     * @tc.size       : MediumTest
     * @tc.type       : Function
     * @tc.level      : Level 3
     */
    it('testSubscribeMultiThreadTest0210', TestType.RELIABILITY | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      for(let i = 0; i < 10; i++){
        let value: Array<int> = new Array<int>();
        let taskGroupName: string = "SubscribeMultiThread0210";
        let taskCloseGroup: taskpool.TaskGroup = new taskpool.TaskGroup(taskGroupName);
        let task9_onDataChange1: taskpool.Task = new taskpool.Task(onDataChangeTest1,context,value); // 订阅数据
        let task9_dataCRUDTest: taskpool.Task = new taskpool.Task(dataUpdateTest,context,i);// 更新单条数据
        let task9_onDataChange2: taskpool.Task = new taskpool.Task(onDataChangeTest1,context,value);// 订阅数据
        taskCloseGroup.addTask(task9_onDataChange1)
        taskCloseGroup.addTask(task9_dataCRUDTest)
        taskCloseGroup.addTask(task9_onDataChange2)
        await taskpool.execute(taskCloseGroup).then((data: Array<Any>) => {
          //data是线程调用后的返回值，按照添加任务时的顺序一次放进数组中，再取出来进行断言
          console.info(TAG + "seqrunner: testSubscribeMultiThreadTest0210 done.data is :" + JSON.stringify(data));
          let a1 =data[0] as Object[];
          let a3 =data[2] as Object[];
          expect(a1[0] == 0 || a1[0] == -1 ).assertTrue()
          expect(a3[0] == 0 || a3[0] == -1 ).assertTrue()
        }).catch((err) => {
          console.info(TAG + "Failed to testSubscribeMultiThreadTest0210. err " + JSON.stringify(err));
          expect().assertFail()
        })
      }
    });
 
    /**
     * @tc.name       : testSubscribeMultiThreadTest0220
     * @tc.number     : SUB_DistributedData_DFX_Concurrent_DataShare_SubscribeMultiThreadTest_0220
     * @tc.desc       : onDataChange and onDataChange thread
     * @tc.size       : MediumTest
     * @tc.type       : Function
     * @tc.level      : Level 3
     */
    it('testSubscribeMultiThreadTest0220', TestType.RELIABILITY | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      for(let i = 0; i < 10; i++){
        let value: Array<int> = new Array<int>();
        let taskGroupName: string = "SubscribeMultiThread0220";
        let taskCloseGroup: taskpool.TaskGroup = new taskpool.TaskGroup(taskGroupName);
        let task9_onDataChange1: taskpool.Task = new taskpool.Task(onDataChangeTest1, context, value); // 订阅数据
        let task9_dataCRUDTest: taskpool.Task = new taskpool.Task(dataDeleteTest, context, i);// 删除数据
        let task9_onDataChange2: taskpool.Task = new taskpool.Task(onDataChangeTest1, context, value);// 订阅数据
        taskCloseGroup.addTask(task9_onDataChange1)
        taskCloseGroup.addTask(task9_dataCRUDTest)
        taskCloseGroup.addTask(task9_onDataChange2)
        await taskpool.execute(taskCloseGroup).then((data: Array<Any>) => {
          //data是线程调用后的返回值，按照添加任务时的顺序一次放进数组中，再取出来进行断言
          console.info(TAG + "seqrunner: testSubscribeMultiThreadTest0220 done.data is :" + JSON.stringify(data));
          let a1 = data[0] as Object[];
          let a3 = data[2] as Object[];
          expect(a1[0] == 0 || a1[0] == -1).assertTrue()
          expect(a3[0] == 0 || a3[0] == -1).assertTrue()
        }).catch((err) => {
          console.info(TAG + "Failed to testSubscribeMultiThreadTest0220. err " + JSON.stringify(err));
          expect().assertFail()
        })
      }
    });
  })
}
 
/**
 * onDataChange支持静默访问
 * @param context
 * @param value:用来保存on、off的调用结果
 * @returns :将on、off的调用结果返回到线程执行完的回调函数中
 */
async function onDataChangeTest1(context: common.UIAbilityContext, value: Array<int>): Promise<Array<int>> {
  const TAG = "[ttt-onDataChangeTest] ";
  value = new Array<int>();
  let dsProxyHelper: dataShare.DataShareHelper | undefined = undefined;
  let uri = ("datashareproxy://com.acts.ohos.data.datasharetest.static/test");
  try {
    dsProxyHelper = await dataShare.createDataShareHelper(context, uri, {isProxy:true});
    console.log(TAG + "[DataShareClientTest] <<Consumer>> dsProxyHelper " + JSON.stringify(dsProxyHelper));
  } catch (err: BusinessError) {
    console.error(TAG + '[DataShareClientTest] <<Consumer>> createDataShareHelper error' + err.code+ "  " + err.message);
  }
  let onReturnCode = -1; //on接口的默认返回值为-1，如果进入回调则返回值为0
  let callback = () => {
    console.info(TAG + '**** Observer onTest1 callback **** change ini:');
    onReturnCode = 0;
  }
  try {
    dsProxyHelper?.onDataChange(uri, callback);
    console.log(TAG + "[DataShareClientTest] <<Consumer>> on end value is " + JSON.stringify(value));
    await new Promise<string>((resolve) => {setTimeout(() => {resolve("ok")}, 1000)});
    value?.push(onReturnCode);
  } catch (err: BusinessError) {
    console.error(TAG + '[DataShareClientTest] <<Consumer>> onDataChangeTest1 error' + err.code + "  " + err.message);
  }
  try {
    let offReturnCode = -1; //on接口的默认返回值为-1，如果进入回调则返回值为0
    dsProxyHelper?.offDataChange(uri,callback);
    value.push(offReturnCode);
    console.log(TAG + "[DataShareClientTest] <<Consumer>> off end value is " + JSON.stringify(value));
  } catch (err: BusinessError) {
    console.error(TAG + '[DataShareClientTest] <<Consumer>> testSubscribe001 error' + err.code + "  " + err.message);
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
  }
  let vb: ValuesBucket = {
    "name0": "TinaInsert",
  };
  try {
    let ret = await dsProxyHelper!.insert(uri, vb);
    console.log(TAG + "[DataShareClientTest] <<Consumer>> insert end rowNum is " + ret);
  } catch (err: BusinessError) {
    console.error(TAG + '[DataShareClientTest] <<Consumer>> insert error' + err.code + "  " + err.message);
    expect().assertFail()
  }
  dsProxyHelper?.close();
}
 
async function dataUpdateTest(context: common.UIAbilityContext,index: int): Promise<void> {
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
    da.equalTo("name0", "wangwu" + index);
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
    daDe.equalTo("name0", "wangwu" + (index + 100));
    let ret = await dsProxyHelper!.delete(uri, daDe);
    console.log(TAG + "[DataShareClientTest] <<Consumer>> delete end rowNum is " + ret);
  } catch (err: BusinessError) {
    console.error(TAG + '[DataShareClientTest] <<Consumer>> delete error' + err.code + "  " + err.message);
  }
  dsProxyHelper?.close();
}