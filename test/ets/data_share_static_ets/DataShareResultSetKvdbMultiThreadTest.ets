/*
* Copyright (c) 2025 Huawei Device Co., Ltd.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect, Level, Size, TestType } from "../../../hypium/index";
import dataShare from '@ohos.data.dataShare';
import dataSharePredicates from '@ohos.data.dataSharePredicates';
import DataShareResultSet from '@ohos.data.DataShareResultSet';
import { ValuesBucket } from "@ohos.data.ValuesBucket";
import { BusinessError } from '@ohos.base';
import Utils from './Util.test';
import { AppStorage } from '@ohos.arkui.stateManagement';
import common from '@ohos.app.ability.common';
import UIAbility from '@ohos.app.ability.UIAbility';
 
const TAG = "[ttt]";
let dataShareHelper: dataShare.DataShareHelper;
let dseUri = "datashare:///com.acts.ohos.data.datashareresultkv.static";
let context: common.UIAbilityContext;
 
export default function DataShareResultSetKvdbMultiThreadTest() {
  describe('DataShareResultSetKvdbMultiThreadTest', () => {
    beforeAll(async () => {
      console.info(TAG + "beforeAll");
      context = AppStorage.get<common.UIAbilityContext>("context") as common.UIAbilityContext;
    });
 
    beforeEach(async () => {
      console.info(TAG + "beforeEach");
      try {
        dataShareHelper = await dataShare.createDataShareHelper(context, dseUri);
        await Utils.msSleep(1000)
        console.info(TAG + "createDataShareHelper success")
        let vbs: ValuesBucket[] = [
          { "name": "zhangsan", "age": 18 as Long, "isStudent": true },
          { "name": "lisi", "age": 18 as Long, "isStudent": false },
          { "name": "amy", "age": 18 as Long, "isStudent": true }
        ]
        if (dataShareHelper != undefined) {
          await dataShareHelper.batchInsert(dseUri, vbs);
        }
      } catch (err: BusinessError) {
        console.error(TAG + "error. err.code:" + err.code + "err.message:" + err.message)
        expect().assertFail();
      }
    });
 
    afterEach(async () => {
      console.info(TAG + "afterEach");
      let da = new dataSharePredicates.DataSharePredicates();
      da.inKeys(["testkey0","testkey1","testkey2"]);
      await dataShareHelper.delete(dseUri, da);
    });
 
    afterAll(async () => {
      await dataShareHelper.close();
    });
 
    /**
     * @tc.name testDataShareResultKvMultiThreadTest0010
     * @tc.number SUB_DistributedData_DFX_MultiThread_DataShare_ResultSetKVTest_0010
     * @tc.desc ResultSet test
     * @tc.level: Level 2
     * @tc.type: Function
     * @tc.size: MediumTest
     */
//    it('testDataShareResultKvMultiThreadTest0010', TestType.RELIABILITY | Size.MEDIUMTEST | Level.LEVEL3, async () => {
//      for (let i = 0; i < 10; i++) {
//        let taskGroupName: string = "SubscribeMultiThread0010";
//        let value: Array<int> = new Array<int>();
//        let taskCloseGroup: taskpool.TaskGroup = new taskpool.TaskGroup(taskGroupName);
//        let task9_dataInsertTest: taskpool.Task = new taskpool.Task(dataInsertTest, context, value); // 插入数据
//        let task9_dataDeleteTest: taskpool.Task = new taskpool.Task(dataDeleteTest, context, value); // 删除数据
//        let task9_dataUpdateTest: taskpool.Task = new taskpool.Task(dataUpdateTest, context, value); // 更新数据
//        let task9_dataGotoFirstRow: taskpool.Task = new taskpool.Task(dataGotoFirstRow, context, value); // 更新数据
//        let task9_dataGotoLastRow: taskpool.Task = new taskpool.Task(dataGotoLastRow, context, value); // 更新数据
//        let task9_dataGotoNextRow: taskpool.Task = new taskpool.Task(dataGotoNextRow, context, value); // 更新数据
//        let task9_dataGotoPreviousRow: taskpool.Task = new taskpool.Task(dataGotoPreviousRow, context, value);
//        let task9_dataGoTo: taskpool.Task = new taskpool.Task(dataGoTo, context, value);
//        let task9_dataGoToRow: taskpool.Task = new taskpool.Task(dataGoToRow, context, value);
//        let task9_dataGetString: taskpool.Task = new taskpool.Task(dataGetString, context, value);
//        let task9_dataGetColumnIndex: taskpool.Task = new taskpool.Task(dataGetColumnIndex, context, value);
//        let task9_dataGetColumnName: taskpool.Task = new taskpool.Task(dataGetColumnName, context, value);
//        let task9_dataGetDataType: taskpool.Task = new taskpool.Task(dataGetDataType, context, value);
//
//        taskCloseGroup.addTask(task9_dataInsertTest)
//        taskCloseGroup.addTask(task9_dataDeleteTest)
//        taskCloseGroup.addTask(task9_dataUpdateTest)
//        taskCloseGroup.addTask(task9_dataGotoFirstRow)
//        taskCloseGroup.addTask(task9_dataGotoLastRow)
//        taskCloseGroup.addTask(task9_dataGotoNextRow)
//        taskCloseGroup.addTask(task9_dataGotoPreviousRow)
//        taskCloseGroup.addTask(task9_dataGoTo)
//        taskCloseGroup.addTask(task9_dataGoToRow)
//        taskCloseGroup.addTask(task9_dataGetString)
//        taskCloseGroup.addTask(task9_dataGetColumnIndex)
//        taskCloseGroup.addTask(task9_dataGetColumnName)
//        taskCloseGroup.addTask(task9_dataGetDataType)
//        await taskpool.execute(taskCloseGroup).then((data: Array<Any>) => {
//          //data是线程调用后的返回值，按照添加任务时的顺序一次放进数组中，再取出来进行断言
//          console.info(TAG + "seqrunner: testDataShareResultKvMultiThreadTest0010 done.data is :" + JSON.stringify(data));
//          let a1 = data[0] as Object[];
//          let a2 = data[1] as Object[];
//          let a3 = data[2] as Object[];
//          let a4 = data[3] as Object[];
//          let a5 = data[4] as Object[];
//          let a6 = data[5] as Object[];
//          let a7 = data[6] as Object[];
//          let a8 = data[7] as Object[];
//          let a9 = data[8] as Object[];
//          let a10 = data[9] as Object[];
//          let a11 = data[10] as Object[];
//          let a12 = data[11] as Object[];
//          let a13 = data[12] as Object[];
//          expect(a1[0] === 0 || a1[0] === 15700000).assertTrue()
//          expect(a2[0] === 0 || a2[0] === 15700000).assertTrue()
//          expect(a3[0] === 0 || a3[0] === 15700000).assertTrue()
//          expect(a4[0] === 0 || a4[0] === 1 || a4[0] === 15700000).assertTrue()
//          expect(a5[0] === 0 || a5[0] === 1 || a5[0] === 15700000).assertTrue()
//          expect(a6[0] === 0 || a6[0] === 1 || a6[0] === 15700000).assertTrue()
//          expect(a7[0] === 0 || a7[0] === 1 || a7[0] === 15700000).assertTrue()
//          expect(a8[0] === 0 || a8[0] === 1 || a8[0] === 15700000).assertTrue()
//          expect(a9[0] === 0 || a9[0] === 1 || a9[0] === 15700000).assertTrue()
//          expect(a10[0] === 0 || a10[0] === 1 || a10[0] === 15700000).assertTrue()
//          expect(a11[0] === 0 || a11[0] === 1 || a11[0] === 15700000).assertTrue()
//          expect(a12[0] === 0 || a12[0] === 1 || a12[0] === 15700000).assertTrue()
//          expect(a13[0] === 0 || a13[0] === 1 || a13[0] === 15700000).assertTrue()
//        }).catch((err: Error) => {
//          console.info(TAG + "Failed to testSubscribeMultiThreadTest0010. err =" + JSON.stringify(err));
//          expect().assertFail()
//        })
//      }
//    })
  })
}
 
/**
 * insert接口新增数据
 * @param context
 * @param value:用来保存接口调用结果
 * @returns :将接口调用结果返回到线程执行完的回调函数中
 */
async function dataInsertTest(context: common.UIAbilityContext, value: Array<int>): Promise<Array<int>> {
  const TAG = "[ttt-onDataChangeTest] "
  value = new Array<int>();
  let dsProxyHelper: dataShare.DataShareHelper | undefined = undefined;
  let uri = "datashare:///com.acts.ohos.data.datashareresultkv.static";
  let retCode = 0;
  try {
    dsProxyHelper = await dataShare.createDataShareHelper(context, uri, {isProxy:false});
    console.log(TAG +"[DataShareClientTest] <<Consumer>> dsProxyHelper " + JSON.stringify(dsProxyHelper));
    for(let i = 0; i < 10; i++){
      let vbs: ValuesBucket = { "name": "Candy", "age": 99 as Long, "isStudent": false, "key":"testInsertKey"+i };
      if (dsProxyHelper != undefined) {
        await dsProxyHelper.insert(uri, vbs);
      }
    }
 
  } catch (err: BusinessError) {
    console.error(TAG + '[DataShareClientTest] <<Consumer>> dataInsertTest error' + err.code + "  " + err.message);
    retCode = err.code;
  }
  value.push(retCode)
  return value;
}
 
/**
 * delete接口删除数据
 * @param context
 * @param value:用来保存接口调用结果
 * @returns :将接口调用结果返回到线程执行完的回调函数中
 */
async function dataDeleteTest(context: common.UIAbilityContext, value: Array<int>): Promise<Array<int>> {
  const TAG = "[ttt-onDataChangeTest] "
  value = new Array<int>();
  let dsProxyHelper: dataShare.DataShareHelper | undefined = undefined
  let uri = "datashare:///com.acts.ohos.data.datashareresultkv.static";
  let retCode = 0;
  try {
    dsProxyHelper = await dataShare.createDataShareHelper(context, uri, {isProxy:false});
    console.log(TAG +"[DataShareClientTest] <<Consumer>> dsProxyHelper " + JSON.stringify(dsProxyHelper));
    for(let i = 0; i < 10; i++){
      if (dsProxyHelper != undefined) {
        let da = new dataSharePredicates.DataSharePredicates();
        da.inKeys(["testInsertKey"+i]);
        await dsProxyHelper.delete(uri, da);
      }
    }
  } catch (err: BusinessError) {
    console.error(TAG +'[DataShareClientTest] <<Consumer>> dataDeleteTest error' + err.code+ "  "+err.message);
    retCode = err.code;
  }
  value.push(retCode)
  return value;
}
 
/**
 * update接口更新数据
 * @param context
 * @param value:用来保存接口调用结果
 * @returns :将接口调用结果返回到线程执行完的回调函数中
 */
async function dataUpdateTest(context: common.UIAbilityContext, value: Array<int>): Promise<Array<int>> {
  const TAG = "[ttt-onDataChangeTest] "
  value = new Array<int>();
  let dsProxyHelper: dataShare.DataShareHelper | undefined = undefined
  let uri = "datashare:///com.acts.ohos.data.datashareresultkv.static";
  let retCode = 0;
  try {
    dsProxyHelper = await dataShare.createDataShareHelper(context, uri,{isProxy:false});
    console.log(TAG +"[DataShareClientTest] <<Consumer>> dsProxyHelper " + JSON.stringify(dsProxyHelper));
    for(let i = 0; i < 10; i++){
      if (dsProxyHelper != undefined) {
        let da = new dataSharePredicates.DataSharePredicates();
        da.inKeys(["testInsertKey"+i]);
        let vbs: ValuesBucket = { "name": "Nessy", "age": 99 as Long, "isStudent": false, "key":"testUpdateKey" };
        await dsProxyHelper.update(uri, da,vbs);
      }
    }
  } catch (err: BusinessError) {
    console.error(TAG + '[DataShareClientTest] <<Consumer>> dataUpdateTest error' + err.code + "  " + err.message);
    retCode = err.code;
  }
  value.push(retCode)
  return value;
}
 
/**
 * goToFirstRow
 * @param context
 * @param value:用来保存接口调用结果
 * @returns :将接口调用结果返回到线程执行完的回调函数中
 */
async function dataGotoFirstRow(context: common.UIAbilityContext, value:Array<int>): Promise<Array<int>> {
  const TAG = "[ttt-onDataChangeTest] "
  value = new Array<int>();
  let dsProxyHelper: dataShare.DataShareHelper | undefined = undefined
  let uri = "datashare:///com.acts.ohos.data.datashareresultkv.static";
  let retCode = 999;
  try{
    dsProxyHelper = await dataShare.createDataShareHelper(context, uri, {isProxy:false});
    console.log(TAG +"[DataShareClientTest] <<Consumer>> dsProxyHelper " + JSON.stringify(dsProxyHelper));
    let predicates = new dataSharePredicates.DataSharePredicates();
    let resultSet= await dsProxyHelper.query(uri, predicates, ["*"]);
    resultSet.goToFirstRow();
    if (resultSet.goToFirstRow()) {
      retCode = 0;
    } else {
      retCode = 1;
    }
    resultSet.close();
  } catch (err: BusinessError) {
    console.error(TAG + '[DataShareClientTest] <<Consumer>> dataGotoFirstRow error' + err.code + "  " + err.message);
    retCode = err.code;
  }
  value.push(retCode);
  return value;
}
 
/**
 * goToLastRow
 * @param context
 * @param value:用来保存接口调用结果
 * @returns :将接口调用结果返回到线程执行完的回调函数中
 */
async function dataGotoLastRow(context: common.UIAbilityContext, value: Array<int>): Promise<Array<int>> {
  const TAG = "[ttt-onDataChangeTest] "
  value = new Array<int>();
  let dsProxyHelper: dataShare.DataShareHelper | undefined = undefined
  let uri = "datashare:///com.acts.ohos.data.datashareresultkv.static";
  let retCode = 999;
  try{
    dsProxyHelper = await dataShare.createDataShareHelper(context, uri, {isProxy:false});
    console.log(TAG +"[DataShareClientTest] <<Consumer>> dsProxyHelper " + JSON.stringify(dsProxyHelper));
    let predicates = new dataSharePredicates.DataSharePredicates();
    let resultSet= await dsProxyHelper.query(uri, predicates, ["*"]);
    resultSet.goToLastRow()
    if (resultSet.goToLastRow()) {
      retCode = 0;
    } else {
      retCode = 1;
    }
    resultSet.close();
  } catch (err: BusinessError) {
    console.error(TAG + '[DataShareClientTest] <<Consumer>> dataGotoLastRow error' + err.code + "  " + err.message);
    retCode = err.code;
  }
  value.push(retCode);
  return value;
}
 
/**
 * goToNextRow
 * @param context
 * @param value:用来保存接口调用结果
 * @returns :将接口调用结果返回到线程执行完的回调函数中
 */
async function dataGotoNextRow(context: common.UIAbilityContext, value: Array<int>): Promise<Array<int>> {
  const TAG = "[ttt-onDataChangeTest] "
  value = new Array<int>();
  let dsProxyHelper: dataShare.DataShareHelper | undefined = undefined
  let uri = "datashare:///com.acts.ohos.data.datashareresultkv.static";
  let retCode = 999;
  try{
    dsProxyHelper = await dataShare.createDataShareHelper(context, uri, {isProxy:false});
    console.log(TAG +"[DataShareClientTest] <<Consumer>> dsProxyHelper " + JSON.stringify(dsProxyHelper));
    let predicates = new dataSharePredicates.DataSharePredicates();
    let resultSet= await dsProxyHelper.query(uri, predicates, ["*"]);
    resultSet.goToNextRow();
    if (resultSet.goToNextRow()) {
      retCode = 0;
    } else {
      retCode = 1;
    }
    resultSet.close();
  } catch (err: BusinessError) {
    console.error(TAG + '[DataShareClientTest] <<Consumer>> dataGotoNextRow error' + err.code + "  " + err.message);
    retCode = err.code;
  }
  value.push(retCode);
  return value;
}
 
/**
 * goToPreviousRow
 * @param context
 * @param value:用来保存接口调用结果
 * @returns :将接口调用结果返回到线程执行完的回调函数中
 */
async function dataGotoPreviousRow(context: common.UIAbilityContext, value: Array<int>): Promise<Array<int>> {
  const TAG = "[ttt-onDataChangeTest] "
  value = new Array<int>();
  let dsProxyHelper: dataShare.DataShareHelper | undefined = undefined
  let uri = "datashare:///com.acts.ohos.data.datashareresultkv.static";
  let retCode = 999;
  try {
    dsProxyHelper = await dataShare.createDataShareHelper(context, uri, {isProxy:false});
    console.log(TAG +"[DataShareClientTest] <<Consumer>> dsProxyHelper " + JSON.stringify(dsProxyHelper));
    let predicates = new dataSharePredicates.DataSharePredicates();
    let resultSet= await dsProxyHelper.query(uri, predicates, ["*"]);
    resultSet.goToPreviousRow()
    if (resultSet.goToPreviousRow()) {
      retCode = 0;
    } else {
      retCode = 1;
    }
    resultSet.close();
  } catch (err: BusinessError) {
    console.error(TAG + '[DataShareClientTest] <<Consumer>> dataGotoPreviousRow error' + err.code + "  " + err.message);
    retCode = err.code;
  }
  value.push(retCode);
  return value;
}
 
/**
 * goTo
 * @param context
 * @param value:用来保存接口调用结果
 * @returns :将接口调用结果返回到线程执行完的回调函数中
 */
async function dataGoTo(context: common.UIAbilityContext, value: Array<int>): Promise<Array<int>> {
  const TAG = "[ttt-onDataChangeTest] "
  value = new Array<int>();
  let dsProxyHelper: dataShare.DataShareHelper | undefined = undefined
  let uri = "datashare:///com.acts.ohos.data.datashareresultkv.static";
  let retCode = 999;
  try{
    dsProxyHelper = await dataShare.createDataShareHelper(context, uri, {isProxy:false});
    console.log(TAG +"[DataShareClientTest] <<Consumer>> dsProxyHelper " + JSON.stringify(dsProxyHelper));
    let predicates = new dataSharePredicates.DataSharePredicates();
    let resultSet= await dsProxyHelper.query(uri, predicates, ["*"]);
    resultSet.goTo(1);
    if (resultSet.goTo(1)) {
      retCode = 0;
    } else {
      retCode = 1;
    }
    resultSet.close();
  } catch (err: BusinessError) {
    console.error(TAG +'[DataShareClientTest] <<Consumer>> dataGoTo error' + err.code+ "  "+err.message);
    retCode = err.code;
  }
  value.push(retCode);
  return value;
}
 
/**
 * goToRow
 * @param context
 * @param value:用来保存接口调用结果
 * @returns :将接口调用结果返回到线程执行完的回调函数中
 */
async function dataGoToRow(context: common.UIAbilityContext, value: Array<int>): Promise<Array<int>> {
  const TAG = "[ttt-onDataChangeTest] "
  value = new Array<int>();
  let dsProxyHelper: dataShare.DataShareHelper | undefined = undefined
  let uri = "datashare:///com.acts.ohos.data.datashareresultkv.static";
  let retCode = 999;
  try {
    dsProxyHelper = await dataShare.createDataShareHelper(context, uri, {isProxy:false});
    console.log(TAG +"[DataShareClientTest] <<Consumer>> dsProxyHelper " + JSON.stringify(dsProxyHelper));
    let predicates = new dataSharePredicates.DataSharePredicates();
    let resultSet= await dsProxyHelper.query(uri, predicates, ["*"]);
    resultSet.goToRow(1);
    if (resultSet.goToRow(1)) {
      retCode = 0;
    } else {
      retCode = 1;
    }
    resultSet.close();
  } catch (err: BusinessError) {
    console.error(TAG + '[DataShareClientTest] <<Consumer>> dataGoToRow error' + err.code + "  " + err.message);
    retCode = err.code;
  }
  value.push(retCode);
  return value;
}
 
/**
 * getString
 * @param context
 * @param value:用来保存接口调用结果
 * @returns :将接口调用结果返回到线程执行完的回调函数中
 */
async function dataGetString(context: common.UIAbilityContext, value: Array<int>): Promise<Array<int>> {
  const TAG = "[ttt-onDataChangeTest] "
  value = new Array<int>();
  let dsProxyHelper: dataShare.DataShareHelper | undefined = undefined
  let uri = "datashare:///com.acts.ohos.data.datashareresultkv.static";
  let retCode = 999;
  try{
    dsProxyHelper = await dataShare.createDataShareHelper(context, uri, {isProxy:false});
    console.log(TAG +"[DataShareClientTest] <<Consumer>> dsProxyHelper " + JSON.stringify(dsProxyHelper));
    let predicates = new dataSharePredicates.DataSharePredicates();
    let resultSet= await dsProxyHelper.query(uri, predicates, ["*"]);
    let str = resultSet.getString(resultSet.getColumnIndex("key"));
    retCode = 0;
    resultSet.close();
  } catch (err: BusinessError) {
    console.error(TAG + '[DataShareClientTest] <<Consumer>> dataGetString error' + err.code + "  " + err.message);
    retCode = err.code;
  }
  value.push(retCode);
  return value;
}
 
/**
 * getColumnIndex
 * @param context
 * @param value:用来保存接口调用结果
 * @returns :将接口调用结果返回到线程执行完的回调函数中
 */
async function dataGetColumnIndex(context: common.UIAbilityContext, value:Array<int>): Promise<Array<int>> {
  const TAG = "[ttt-onDataChangeTest] "
  value = new Array<int>();
  let dsProxyHelper: dataShare.DataShareHelper | undefined = undefined
  let uri = "datashare:///com.acts.ohos.data.datashareresultkv.static";
  let retCode = 999;
  try{
    dsProxyHelper = await dataShare.createDataShareHelper(context, uri,{isProxy:false});
    console.log(TAG +"[DataShareClientTest] <<Consumer>> dsProxyHelper " + JSON.stringify(dsProxyHelper));
    let predicates = new dataSharePredicates.DataSharePredicates();
    let resultSet= await dsProxyHelper.query(uri, predicates, ["*"]);
    let index = resultSet.getColumnIndex("key")
    retCode = index;
    resultSet.close();
  } catch (err: BusinessError) {
    console.error(TAG +'[DataShareClientTest] <<Consumer>> dataGetColumnIndex error' + err.code+ "  "+err.message);
    retCode = err.code;
  }
  value.push(retCode);
  return value;
}
 
/**
 * dataGetColumnName
 * @param context
 * @param value:用来保存接口调用结果
 * @returns :将接口调用结果返回到线程执行完的回调函数中
 */
async function dataGetColumnName(context: common.UIAbilityContext, value: Array<int>): Promise<Array<int>> {
  const TAG = "[ttt-onDataChangeTest] "
  value = new Array<int>();
  let dsProxyHelper: dataShare.DataShareHelper | undefined = undefined
  let uri = "datashare:///com.acts.ohos.data.datashareresultkv.static";
  let retCode = 999;
  try{
    dsProxyHelper = await dataShare.createDataShareHelper(context, uri, {isProxy:false});
    console.log(TAG +"[DataShareClientTest] <<Consumer>> dsProxyHelper " + JSON.stringify(dsProxyHelper));
    let predicates = new dataSharePredicates.DataSharePredicates();
    let resultSet= await dsProxyHelper.query(uri, predicates, ["*"]);
    resultSet.getColumnName(1);
    retCode = 0;
    resultSet.close();
  } catch (err: BusinessError) {
    console.error(TAG + '[DataShareClientTest] <<Consumer>> dataGetColumnName error' + err.code + "  " + err.message);
    retCode = err.code;
  }
  value.push(retCode);
  return value;
}
 
/**
 * getDataType
 * @param context
 * @param value:用来保存接口调用结果
 * @returns :将接口调用结果返回到线程执行完的回调函数中
 */
async function dataGetDataType(context: common.UIAbilityContext, value:Array<int>): Promise<Array<int>> {
  const TAG = "[ttt-onDataChangeTest] "
  value = new Array<int>();
  let dsProxyHelper: dataShare.DataShareHelper | undefined = undefined
  let uri = "datashare:///com.acts.ohos.data.datashareresultkv.static";
  let retCode = 999;
  try{
    dsProxyHelper = await dataShare.createDataShareHelper(context, uri,{isProxy:false});
    console.log(TAG +"[DataShareClientTest] <<Consumer>> dsProxyHelper " + JSON.stringify(dsProxyHelper));
    let predicates = new dataSharePredicates.DataSharePredicates();
    let resultSet= await dsProxyHelper.query(uri, predicates, ["*"]);
    let colType = resultSet.getDataType(1);
    if(colType === 3){
      retCode = 0;
    }else{
      retCode = 1;
    }
    resultSet.close();
  } catch (err: BusinessError) {
    console.error(TAG +'[DataShareClientTest] <<Consumer>> dataGetDataType error' + err.code+ "  "+err.message);
    retCode = err.code;
  }
  value.push(retCode);
  return value;
}