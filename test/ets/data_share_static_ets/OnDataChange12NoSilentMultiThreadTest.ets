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
import Utils from './Util.test';
import common from '@ohos.app.ability.common';
import abilityDelegatorRegistry from '@ohos.app.ability.abilityDelegatorRegistry';
import { AppStorage } from '@ohos.arkui.stateManagement'
import UIAbility from '@ohos.app.ability.UIAbility';
import { BusinessError } from '@ohos.base';
import dataShare from '@ohos.data.dataShare'
import { ValuesBucket } from '@ohos.data.ValuesBucket';
 
let domain: number = 0x0000; //日志标识,0x0000作为测试框架的业务标识
let tag: string = 'testTag'; //日志标识字符串,作为tag标识当前runner类下的测试行为
let context: common.UIAbilityContext;
let uri = ("datashareproxy://com.acts.ohos.data.datasharetest.static/test");
let dsProxyHelper: dataShare.DataShareHelper | undefined = undefined
const TAG = "[ttt] "
 
export default function onDataChange12NoSilentMultiThreadTest() {
  describe("onDataChange12NoSilentMultiThreadTest", (): void => {
    beforeAll(async () => {
      try {
        let abilityDelegator: abilityDelegatorRegistry.AbilityDelegator = abilityDelegatorRegistry.getAbilityDelegator()
        abilityDelegator.addAbilityMonitor({
          abilityName: "EntryAbility",
          moduleName: "entry",
          onAbilityCreate: (ability: UIAbility) : void => {
            context = ability.context;
            AppStorage.setOrCreate("context", ability.context);
            console.log('[ttt] ability.context.databaseDir:' + context.databaseDir);
            console.log('[ttt] ability.context.area:' + context.area);
            console.log('onAbilityCreate end');
          },
        }, (err : BusinessError | null) : void => {
          if (err && err.code != 0 ) {
            console.log('-----' + err.code);
          }
          console.log('BusinessError  end');
        });
        await Utils.msSleep(2000);
        abilityDelegator.executeShellCommand("aa start -a EntryAbility -b com.acts.datasharestabilityclient.static");
        await Utils.msSleep(2000);
        dsProxyHelper = await dataShare.createDataShareHelper(context, uri, { isProxy: false });
        // console.log("[ttt] [DataShareClientTest] <<Consumer>> dsProxyHelper " + JSON.stringify(dsProxyHelper));
      } catch(err: BusinessError) {
        console.error('[ttt] [DataShareClientTest] <<Consumer>> createDataShareHelper error' + err.code + "  " + err.message);
        expect().assertFail();
      }
    });
 
    beforeEach(() => {
    })
 
    afterEach(() => {
      dsProxyHelper?.offDataChange(dataShare.SubscriptionType.SUBSCRIPTION_TYPE_EXACT_URI,uri);
    })
 
    afterAll(async () => {
      dsProxyHelper?.close(); // dsProxyHelper?.close();
    })
 
    /**
     * @tc.name       : testSubscribeMultiThreadTest0010
     * @tc.number     : SUB_DistributedData_DFX_Concurrent_DataShare_SubscribeMultiThreadTest_0010
     * @tc.desc       : onDataChange and onDataChange thread
     * @tc.size       : MediumTest
     * @tc.type       : Function
     * @tc.level      : Level 3
     */
    it('testSubscribeMultiThreadTest0010', TestType.RELIABILITY | Size.MEDIUMTEST | Level.LEVEL3, async () => {
      for (let i = 0; i < 10; i++) {
        let taskGroupName: string = "SubscribeMultiThread0010";
        let value: Array<int> = new Array<int>();
        let taskCloseGroup: taskpool.TaskGroup = new taskpool.TaskGroup(taskGroupName);
        let task9_onDataChange1: taskpool.Task = new taskpool.Task(onDataChangeTest1, context, value); // 订阅数据
        let task9_dataCRUDTest: taskpool.Task = new taskpool.Task(dataCRUDTest,context, i, value); // 订阅数据
        taskCloseGroup.addTask(task9_onDataChange1)
        taskCloseGroup.addTask(task9_dataCRUDTest)
        await taskpool.execute(taskCloseGroup).then((data: Array<Any>) => {
          //data是线程调用后的返回值，按照添加任务时的顺序一次放进数组中，再取出来进行断言
          console.info(TAG + "seqrunner: testSubscribeMultiThreadTest0010 done.data is :" + JSON.stringify(data));
          let a1 = data[0] as Object[];
          expect(a1[0] == 0 || a1[0] == -1).assertTrue()
          expect(a1[1] == 0 || a1[1] == -1).assertTrue()
        }).catch((err) => {
          console.info(TAG + "Failed to testSubscribeMultiThreadTest0010 err " + JSON.stringify(err));
          expect().assertFail();
        })
      }
    });
 
  })
}
 
/**
 * onDataChange支持非静默访问
 * @param context
 * @param value:用来保存on、off的调用结果
 * @returns :将on、off的调用结果返回到线程执行完的回调函数中
 */
async function onDataChangeTest1(context: common.UIAbilityContext, value: Array<int>): Promise<Array<int>> {
  const TAG = "[ttt-onDataChangeTest] "
  value = new Array<int>();
  let dsProxyHelper: dataShare.DataShareHelper | undefined = undefined
  let uri = ("datashareproxy://com.acts.ohos.data.datasharetest.static/test");
  try{
    dsProxyHelper = await dataShare.createDataShareHelper(context, uri, {isProxy:false});
    console.log(TAG +"[DataShareClientTest] <<Consumer>> dsProxyHelper " + JSON.stringify(dsProxyHelper));
  } catch (err: BusinessError) {
    console.error(TAG + '[DataShareClientTest] <<Consumer>> createDataShareHelper error ' + err.code + "  " + err.message);
  }
  let onReturnCode = -1; //on接口的默认返回值为-1，如果进入回调则返回值为0
  let callback = (ChangeInfo:dataShare.ChangeInfo)=> {
    console.info(TAG + '**** Observer callback **** ChangeInfo:' + JSON.stringify(ChangeInfo));
    onReturnCode = 0;
  }
  try {
    dsProxyHelper?.onDataChange(dataShare.SubscriptionType.SUBSCRIPTION_TYPE_EXACT_URI, uri, callback);
    console.log(TAG + "[DataShareClientTest] <<Consumer>> on end value is " + JSON.stringify(value));
  } catch (err: BusinessError) {
    console.error(TAG + '[DataShareClientTest] <<Consumer>> testSubscribe001 error' + err.code + "  " + err.message);
  }
  await new Promise<string>((resolve) => {setTimeout(() => {resolve("ok")}, 3000)});
  value?.push(onReturnCode);
  try{
    let offReturnCode = -1; //on接口的默认返回值为-1，如果进入回调则返回值为0
    dsProxyHelper?.offDataChange(dataShare.SubscriptionType.SUBSCRIPTION_TYPE_EXACT_URI,uri, callback);
    value?.push(offReturnCode);
    console.log(TAG + "[DataShareClientTest] <<Consumer>> off end value is " + JSON.stringify(value));
  } catch (err: BusinessError) {
    console.error(TAG + '[DataShareClientTest] <<Consumer>> testSubscribe001 error' + err.code + "  " + err.message);
  }
  try {
    dsProxyHelper?.close();
  } catch (err: BusinessError) {
    console.error(TAG + '[DataShareClientTest] <<Consumer>> testSubscribe001 error' + err.code + "  " + err.message);
  }
 
  return value;
}
 
async function dataCRUDTest(context: common.UIAbilityContext, index: int): Promise<void> {
  const TAG = "[ttt-dataCRUDTest]"
  let dsProxyHelper: dataShare.DataShareHelper | undefined = undefined
  let uri = ("datashareproxy://com.acts.ohos.data.datasharetest.static/test");
  try{
    dsProxyHelper = await dataShare.createDataShareHelper(context, uri,{isProxy:false});
    console.log(TAG +"[DataShareClientTest] <<Consumer>> dsProxyHelper " + JSON.stringify(dsProxyHelper));
  } catch (err: BusinessError) {
    console.error(TAG +'[DataShareClientTest] <<Consumer>> createDataShareHelper error' + err.code+ "  "+err.message);
  }
  let vb: ValuesBucket = {
    "name0": "Tina"+index,
  };
  try {
    let ret = await dsProxyHelper!.insert(uri, vb);
    dsProxyHelper?.notifyChange(uri);
    console.log(TAG + "[DataShareClientTest] <<Consumer>> insert end rowNum is " + ret);
    dsProxyHelper?.close();
  } catch (err: BusinessError) {
    console.error(TAG + '[DataShareClientTest] <<Consumer>> insert error' + err.code + "  " + err.message);
  }
}