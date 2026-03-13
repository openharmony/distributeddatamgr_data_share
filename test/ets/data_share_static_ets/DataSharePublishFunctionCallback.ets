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
import hilog from '@ohos.hilog'
import dataShare from '@ohos.data.dataShare'
import dataSharePredicates from '@ohos.data.dataSharePredicates'
import DataShareResultSet from '@ohos.data.DataShareResultSet'
import { BusinessError } from '@ohos.base';
import { ValuesBucket } from '@ohos.data.ValuesBucket';
import { GlobalThis } from '../../ets/entryability/EntryAbility'
 
let domain: int = 0x0000; //日志标识,0x0000作为测试框架的业务标识
let tag: string = 'testTag'; //日志标识字符串,作为tag标识当前runner类下的测试行为
 
const globalThis: GlobalThis = GlobalThis.getInstance();
 
//insert公共方法
function publicinsert(helper: dataShare.DataShareHelper, uri: string, DataShareValuesBucket: ValuesBucket): Promise<int>
{
  return new Promise<int>((resolve, reject) => {
    helper.insert(uri, DataShareValuesBucket, (err: BusinessError | null, data: int | undefined) => {
      console.info("[ttt] [TestDataShare]  publicinsert Callback=" + JSON.stringify(data));
      globalThis.insertData = data as int;
      if (err?.message) {
        hilog.error(domain, tag, '%{public}s', `test insert err information:  ${err?.code}, message: ${err?.message}`);
        reject(err as BusinessError);
      } else {
        resolve(data as int);
      }
    })
  })
}
 
//delete公共方法
async function publicdelete(helper: dataShare.DataShareHelper, uri: string, DataSharePredicates: dataSharePredicates.DataSharePredicates): Promise<int>
{
  return new Promise<int>((resolve, reject) => {
    console.info("[ttt] [DataShareTest] <<XTS>> 12345648564185645314");
    helper.delete(uri, DataSharePredicates, (err: BusinessError | null, data: int | undefined) => {
      console.info("[ttt] [TestDataShare] publicdelete Callback=" + JSON.stringify(data));
      if (err?.message) {
        hilog.error(domain, tag, '%{public}s', `test delete err information:  ${err?.code}, message: ${err?.message}`);
        reject(err as BusinessError);
      } else {
        resolve(data as int);
      }
    })
  })
}
 
//query公共方法
function publicquery(helper: dataShare.DataShareHelper, uri: string, DataSharePredicates: dataSharePredicates.DataSharePredicates, columns: Array<string>): Promise<int>
{
  return new Promise<int>((resolve, reject) => {
    helper.query(uri, DataSharePredicates, columns, (err: BusinessError | null, data: DataShareResultSet | undefined) => {
      console.info("[ttt] [TestDataShare]  publicquery Callback=" + JSON.stringify(data));
      globalThis.ResultSet = data as DataShareResultSet;
      if (err?.message) {
        hilog.error(domain, tag, '%{public}s', `test query err information:  ${err?.code}, message: ${err?.message}`);
        reject(err as BusinessError);
      } else {
        resolve(1);
      }
    })
  })
}
 
//update公共方法
function publicupdate(helper: dataShare.DataShareHelper, uri: string, DataSharePredicates: dataSharePredicates.DataSharePredicates, DataShareValuesBucket: ValuesBucket): Promise<int>
{
  return new Promise<int>((resolve, reject) => {
    helper.update(uri, DataSharePredicates, DataShareValuesBucket, (err: BusinessError | null, data: int | undefined) => {
      console.info("[ttt] [TestDataShare]  publicupdate Callback=" + JSON.stringify(data));
      if (err?.message) {
        hilog.error(domain, tag, '%{public}s', `test update err information:  ${err?.code}, message: ${err?.message}`);
        reject(err as BusinessError);
      } else {
        resolve(data as int);
      }
    })
  })
}
 
//batchInsert公共方法
function publicbatchInsert(helper: dataShare.DataShareHelper, uri: string, DataShareValuesBucket: Array<ValuesBucket>): Promise<int>
{
  return new Promise<int>((resolve, reject) => {
    helper.batchInsert(uri, DataShareValuesBucket, (err: BusinessError | null, data: int | undefined) => {
      console.info("[ttt] [TestDataShare]  publicbatchInsert Callback=" + JSON.stringify(data));
      if (err?.message) {
        hilog.error(domain, tag, '%{public}s', `test batchInsert err information:  ${err?.code}, message: ${err?.message}`);
        reject(err as BusinessError);
      } else {
        resolve(data as int);
      }
    })
  })
}
 
// //getType公共方法
// function publicgetType(helper, uri) {
//   return new Promise(function (resolve, reject) {
//     helper.getType(uri, (err, data) => {
//       console.info("[ttt] [TestDataShare]  publicgetType Callback=" + JSON.stringify(data));
//       if (err) {
//         console.info("test getType err information: " + JSON.stringify(err));
//         reject(err);
//       } else {
//         resolve(data);
//       }
//     })
//   })
// }
//
// //getFileTypes公共方法
// function publicgetFileTypes(helper, uri, mimeTypeFilter) {
//   return new Promise(function (resolve, reject) {
//     helper.getFileTypes(uri, mimeTypeFilter, (err, data) => {
//       console.info("[ttt] [TestDataShare]  publicgetFileTypes Callback=" + JSON.stringify(data));
//       if (err) {
//         console.info("test getFileTypes err information: " + JSON.stringify(err));
//         reject(err);
//       } else {
//         resolve(data);
//       }
//     })
//   })
// }
//
// //normalizeUri公共方法
// function publicnormalizeUri(helper, uri) {
//   return new Promise(function (resolve, reject) {
//     helper.normalizeUri(uri, (err, data) => {
//       console.info("[ttt] [TestDataShare]  publicnormalizeUri Callback=" + JSON.stringify(data));
//       if (err) {
//         console.info("test normalizeUri err information: " + JSON.stringify(err));
//         reject(err);
//       } else {
//         resolve(data);
//       }
//     })
//   })
// }
//
// //denormalizeUri公共方法
// function publicdenormalizeUri(helper, uri) {
//   return new Promise(function (resolve, reject) {
//     helper.denormalizeUri(uri, (err, data) => {
//       console.info("[ttt] [TestDataShare]  publicdenormalizeUri Callback=" + JSON.stringify(data));
//       if (err) {
//         console.info("test denormalizeUri err information: " + JSON.stringify(err));
//         reject(err);
//       } else {
//         resolve(data);
//       }
//     })
//   })
// }
//
// //denormalizeUri公共方法
// function publicopenFile(helper, uri, mode) {
//   return new Promise(function (resolve, reject) {
//     helper.denormalizeUri(uri, mode, (err, data) => {
//       console.info("[ttt] [TestDataShare]  publicopenFile Callback=" + JSON.stringify(data));
//       if (err) {
//         console.info("test openFile err information: " + JSON.stringify(err));
//         reject(err);
//       } else {
//         resolve(data);
//       }
//     })
//   })
// }
 
export { publicinsert, publicdelete, publicquery, publicupdate, publicbatchInsert }