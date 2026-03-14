/*
* Copyright (c) 2026 Huawei Device Co., Ltd.
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
 
// import file from '@system.file';
import { describe, it, expect, TestType, Size, Level, beforeAll, afterAll} from "../../../hypium/index";
// import { dataShare, dataSharePredicates, DataShareResultSet} from '@kit.ArkData';
import dataShare from '@ohos.data.dataShare';
import dataSharePredicates from '@ohos.data.dataSharePredicates';
import DataShareResultSet from '@ohos.data.DataShareResultSet';
import { BusinessError } from '@ohos.base';
import { ValuesBucket } from '@ohos.data.ValuesBucket';
import { GlobalThis } from '../../ets/entryability/EntryAbility'
 
const globalThis: GlobalThis = GlobalThis.getInstance();
 
// insert公共方法
function publicinsert(helper: dataShare.DataShareHelper, uri: string, DataShareValuesBucket: ValuesBucket): Promise<int>
{
  return new Promise<int>((resolve, reject) => {
    helper.insert(uri, DataShareValuesBucket).then((data: int) => {
      console.info("[ttt] [TestDataShare]  publicinsert Callback=" + JSON.stringify(data));
      globalThis.insertData = data;
      resolve(data);
    }).catch((error) => {
      let err = error as BusinessError;
      console.error(`test insert err information: code: ${err.code}, message: ${err.message}`);
      reject(err);
    })
  })
}
 
//delete公共方法
function publicdelete(helper: dataShare.DataShareHelper, uri: string, DataSharePredicates: dataSharePredicates.DataSharePredicates): Promise<int>
{
  return new Promise<int>((resolve, reject) => {
    helper.delete(uri, DataSharePredicates).then((data: int) => {
      console.info("[ttt] [TestDataShare]  publicdelete Callback=" + JSON.stringify(data));
      resolve(data);
    }).catch((error) => {
      let err = error as BusinessError;
      console.error(`test delete err information: code: ${err.code}, message: ${err.message}`);
      reject(err);
    })
  })
}
 
//query公共方法
function publicquery(helper: dataShare.DataShareHelper, uri: string, DataSharePredicates: dataSharePredicates.DataSharePredicates, columns: Array<string>): Promise<int>
{
  return new Promise<int>((resolve, reject) => {
    helper.query(uri, DataSharePredicates, columns).then((data) => {
      console.info("[ttt] [TestDataShare]  publicquery Callback=" + JSON.stringify(data));
      globalThis.ResultSet = data;
      console.info("[ttt] [TestDataShare]  publicquery globalThis.ResultSet: " + JSON.stringify(globalThis.ResultSet));
      resolve(1);
    }).catch((error) => {
      let err = error as BusinessError;
      console.error(`test query err information: code: ${err.code}, message: ${err.message}`);
      reject(err);
    })
  })
}
 
//update公共方法
function publicupdate(helper: dataShare.DataShareHelper, uri: string, DataSharePredicates: dataSharePredicates.DataSharePredicates, DataShareValuesBucket: ValuesBucket): Promise<int>
{
  return new Promise<int>((resolve, reject) => {
    helper.update(uri, DataSharePredicates, DataShareValuesBucket).then((data: int) => {
      console.info("[ttt] [TestDataShare]  publicupdate Callback=" + JSON.stringify(data));
      resolve(data);
    }).catch((error) => {
      let err = error as BusinessError;
      console.error(`test delete err information: code: ${err.code}, message: ${err.message}`);
      reject(err);
    })
  })
}
 
//batchInsert公共方法
function publicbatchInsert(helper: dataShare.DataShareHelper, uri: string, DataShareValuesBucket: Array<ValuesBucket>): Promise<int>
{
  return new Promise<int>((resolve, reject) => {
    helper.batchInsert(uri, DataShareValuesBucket).then((data: int) => {
      console.info("[ttt] [TestDataShare]  publicbatchInsert Callback=" + JSON.stringify(data));
      resolve(data);
    }).catch((error) => {
      let err = error as BusinessError;
      console.error(`test batchInsert err information: code: ${err.code}, message: ${err.message}`);
      reject(err);
    })
  })
}
 
//getType公共方法
// function publicgetType(helper, uri) {
//   return new Promise(function (resolve, reject) {
//     helper.getType(uri).then((data) => {
//       console.info("[ttt] [TestDataShare]  publicgetType Callback=" + JSON.stringify(data));
//       resolve(data);
//     }).catch((error) => {
//       let err = error as BusinessError;
//       console.error(`test getType err information: code: ${err.code}, message: ${err.message}`);
//       // console.info("test getType err information: " + JSON.stringify(err));
//       reject(err);
//     })
//   })
// }
 
//getFileTypes公共方法
// function publicgetFileTypes(helper, uri, mimeTypeFilter) {
//   return new Promise(function (resolve, reject) {
//     helper.getFileTypes(uri, mimeTypeFilter).then((data) => {
//       console.info("[ttt] [TestDataShare]  publicgetFileTypes Callback=" + JSON.stringify(data));
//       resolve(data);
//     }).catch((error) => {
//       let err = error as BusinessError;
//       console.error(`test getFileTypes err information: code: ${err.code}, message: ${err.message}`);
//       // console.info("test getFileTypes err information: " + JSON.stringify(err));
//       reject(err);
//     })
//   })
// }
 
//normalizeUri公共方法
// function publicnormalizeUri(helper, uri) {
//   return new Promise(function (resolve, reject) {
//     helper.normalizeUri(uri).then((data) => {
//       console.info("[ttt] [TestDataShare]  publicnormalizeUri Callback=" + JSON.stringify(data));
//       resolve(data);
//     }).catch((error) => {
//       let err = error as BusinessError;
//       console.error(`test normalizeUri err information: code: ${err.code}, message: ${err.message}`);
//       // console.info("test normalizeUri err information: " + JSON.stringify(err));
//       reject(err);
//     })
//   })
// }
 
//denormalizeUri公共方法
// function publicdenormalizeUri(helper, uri) {
//   return new Promise(function (resolve, reject) {
//     helper.denormalizeUri(uri).then((data) => {
//       console.info("[ttt] [TestDataShare]  publicdenormalizeUri Callback=" + JSON.stringify(data));
//       resolve(data);
//     }).catch((error) => {
//       let err = error as BusinessError;
//       console.error(`test denormalizeUri err information: code: ${err.code}, message: ${err.message}`);
//       // console.info("test denormalizeUri err information: " + JSON.stringify(err));
//       reject(err);
//     })
//   })
// }
 
//denormalizeUri公共方法
// function publicopenFile(helper, uri, mode) {
//   return new Promise(function (resolve, reject) {
//     helper.openFile(uri, mode).then((data) => {
//       console.info("[ttt] [TestDataShare]  publicopenFile Callback=" + JSON.stringify(data));
//       resolve(data);
//     }).catch((error) => {
//       let err = error as BusinessError;
//       console.error(`test openFile err information: code: ${err.code}, message: ${err.message}`);
//       // console.info("test openFile err information: " + JSON.stringify(err));
//       reject(err);
//     })
//   })
// }
 
export { publicinsert, publicdelete, publicquery, publicupdate, publicbatchInsert }