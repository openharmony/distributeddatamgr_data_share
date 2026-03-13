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
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect, Level, Size, TestType } from "../../../hypium/index";
import dataShare from '@ohos.data.dataShare'
import { BusinessError } from '@kit.BasicServicesKit';
// import taskPool from '@ohos.taskpool';
 
 
let g_handle : dataShare.DataProxyHandle | undefined = undefined;
let g_config : dataShare.DataProxyConfig = {
  type : dataShare.DataProxyType.SHARED_CONFIG
}
let testUri1 = "datashareproxy://ohos.acts.datashareconfigtest.static/test1";
let testUri2 = "datashareproxy://ohos.acts.datashareconfigtest.static/test1";
let TAG = "[ttt]";
export default function dataShareConfigMoreThreadTest() {
  describe("dataShareConfigMoreThreadTest", () => {
    beforeAll(async () => {
      console.info(TAG + "beforeAll -----------------------------------");
    })
    beforeEach(async () => {
      console.info(TAG + "beforeEach -----------------------------------");
    })
    afterEach(async () => {
      console.info(TAG + "afterEach -----------------------------------");
    })
    afterAll(async () => {
      console.info(TAG + "afterAll -----------------------------------");
    })
 
    /**
     * @tc.name       : testSubscribeMultiThreadTest0230
     * @tc.number     : SUB_DistributedData_DFX_Concurrent_DataShare_SubscribeMultiThreadTest_0230
     * @tc.desc       : publish and delete and get config
     * @tc.size       : MediumTest
     * @tc.type       : RELIABILITY
     * @tc.level      : Level 3
     */
    it('testSubscribeMultiThreadTest0230', TestType.RELIABILITY | Size.MEDIUMTEST | Level.LEVEL3, async (done: () => void): Promise<void> => {
      console.info(TAG + "testSubscribeMultiThreadTest0230 start");
      for(let i=0;i<2000;i++){
        let value: Array<number> = new Array<number>();
        let taskGroupName: string = "SubscribeMultiThread0230";
        let taskCloseGroup: taskpool.TaskGroup = new taskpool.TaskGroup(taskGroupName);
        let task9_publishConfig: taskpool.Task = new taskpool.Task(publishConfig,value); // 发布配置
        let task9_deleteConfig: taskpool.Task = new taskpool.Task(deleteConfig,value);// 删除配置
        let task9_getConfig: taskpool.Task = new taskpool.Task(getConfig,value);// 查询配置
        taskCloseGroup.addTask(task9_publishConfig)
        taskCloseGroup.addTask(task9_deleteConfig)
        taskCloseGroup.addTask(task9_getConfig)
        await taskpool.execute(taskCloseGroup).then((data:Array<Any>) => {
          //data是线程调用后的返回值，按照添加任务时的顺序一次放进数组中，再取出来进行断言
          console.info(TAG + "seqrunner: testSubscribeMultiThreadTest0230 done.data is :"+JSON.stringify(data));
          let a1 =data[0] as Object[];
          let a2 =data[1] as Object[];
          let a3 =data[2] as Object[];
          expect(a1[0] == 0 || a1[0]==1).assertTrue();
          expect(a2[0] == 0 || a2[0]==1).assertTrue();
          expect(a3[0] == 0 || a3[0]==1).assertTrue();
        }).catch((err: Error) => {
          console.info(TAG + "Failed to testSubscribeMultiThreadTest0230. code =" + err.code + ", message =" + err.message);
          expect().assertFail()
        })
      }
      console.info(TAG + "testSubscribeMultiThreadTest0230 end");
      done();
    });
  })
}
 
// @Concurrent
async function publishConfig(value:Array<Double>):Promise<Array<Double>> {
  const TAG = "[ttt-publishConfig] "
  value = new Array<Double>();
  let g_config : dataShare.DataProxyConfig = {
    type : dataShare.DataProxyType.SHARED_CONFIG
  }
  let testUri = "datashareproxy://ohos.acts.datashareconfigtest.static/test";
  let g_handle =await dataShare.createDataProxyHandle()
  try {
    let proxyData : dataShare.ProxyData[] = [];
    let g_proxyDataAllowTest1 : dataShare.ProxyData = {
      uri : testUri,
      value : "value is ok",
      allowList : []
    }
    proxyData.push(g_proxyDataAllowTest1);
    await g_handle!.publish(proxyData, g_config).then((results: dataShare.DataProxyResult[]) => {
      console.log(TAG + "[DataShareProvider] publishConfig success, results: " + JSON.stringify(results));
      results.forEach((result) => {
        value.push(result.result);
      })
    }).catch((err: Error) => {
      console.error(TAG + "[DataShareProvider] publishConfig failed. code =" + err.code + ", message =" + err.message);
    })
  } catch(err: BusinessError) {
    console.error(TAG + "[DataShareProvider] publishConfig failed, code =" + err.code + ", message =" + err.message);
  }
  return value;
}
 
// @Concurrent
async function deleteConfig(value:Array<Double>):Promise<Array<Double>> {
  const TAG = "[ttt-deleteConfig] "
  value = new Array<Double>();
  let g_config : dataShare.DataProxyConfig = {
    type : dataShare.DataProxyType.SHARED_CONFIG
  }
  let testUri = "datashareproxy://ohos.acts.datashareconfigtest.static/test";
  let g_handle =await dataShare.createDataProxyHandle()
  try {
    await g_handle!.delete([testUri], g_config).then((results : dataShare.DataProxyResult[]) => {
      console.log(TAG + "[DataShareProvider] deleteConfig success, results: " + JSON.stringify(results));
      results.forEach((result) => {
        value.push(result.result);
      })
    }).catch((err: Error) => {
      console.error(TAG + "[DataShareProvider] deleteConfig failed. code =" + err.code + ", message =" + err.message);
    })
  } catch(err:BusinessError) {
    console.error(TAG + "[DataShareProvider] deleteConfig failed, code =" + err.code + ", message =" + err.message);
  }
  return value;
}
 
// @Concurrent
async function getConfig(value:Array<Double>):Promise<Array<Double>> {
  const TAG = "[ttt-getConfig] "
  value = new Array<Double>();
  let g_config : dataShare.DataProxyConfig = {
    type : dataShare.DataProxyType.SHARED_CONFIG
  }
  let testUri = "datashareproxy://ohos.acts.datashareconfigtest.static/test";
  let g_handle =await dataShare.createDataProxyHandle()
  try {
    await g_handle!.get([testUri], g_config).then((results : dataShare.DataProxyGetResult[]) => {
      results.forEach((result) => {
        console.info(TAG + `[DataShareProvider] URI: ${result.uri}, Value: ${result.value}, Result: ${result.result}, AllowList: ${result.allowList}`);
        value.push(result.result);
      })
    }).catch((err : Error) => {
      console.log(TAG + "[DataShareProvider] getConfig failed, err : " + JSON.stringify(err));
    })
  } catch(err:BusinessError) {
    console.error(TAG + "[DataShareProvider] getConfig failed, code =" + err.code + ", message =" + err.message);
  }
  return value;
}