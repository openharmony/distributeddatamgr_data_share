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
 
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect, Level, Size, TestType } from "../../../hypium/index";
import hilog from '@ohos.hilog';
import Utils from './Util.test';
import dataShare from '@ohos.data.dataShare'
import dataSharePredicates from '@ohos.data.dataSharePredicates';
import common from '@ohos.app.ability.common';
import { ValuesBucket } from '@ohos.data.ValuesBucket';
import { BusinessError } from '@ohos.base';
import DataShareResultSet from '@ohos.data.DataShareResultSet';
import { AppStorage } from '@ohos.arkui.stateManagement';
 
let domain: int = 0x0000; //日志标识,0x0000作为测试框架的业务标识
let tag: string = 'testTag'; //日志标识字符串,作为tag标识当前runner类下的测试行为
 
let dseUri = "datashare:///com.acts.datasharetest.DataShare";
 
let context: common.UIAbilityContext;
let datashareHelper: dataShare.DataShareHelper | undefined = undefined;
let dsHelper: dataShare.DataShareHelper;
let TAG = "[batchUpdateTestClient]";
 
export default function dataShareBatchUpdateTest() {
  describe('dataShareBatchUpdateTest', (): void => {
    beforeAll(async (done: () => void) => {
      context = AppStorage.get<common.UIAbilityContext>("context") as common.UIAbilityContext;
      try {
        datashareHelper = await dataShare.createDataShareHelper(context, dseUri);
        await Utils.msSleep(2000);
        dsHelper = datashareHelper as dataShare.DataShareHelper;
        let ret = 0;
        for (let i = 1; i <= 10; i++) {
          let vb: ValuesBucket = {
            "name": "ZhangSan" + i,
            "age": i as long
          }
          ret = await dsHelper.insert(dseUri, vb);
        }
        console.log(TAG + " insert count is " + ret);
      } catch(err: BusinessError) {
        let code = (err as BusinessError).code;
        let message = (err as BusinessError).message;
        console.error(`beforeAll error: code: ${code}, message: ${message} `);
        expect(true).assertFail();
      }
      hilog.info(domain, tag, '%{public}s', 'beforeAll end');
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0120
     * @tc.name      : dataShareBatchUpdateTest001
     * @tc.desc      : batchUpdate api update two record
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    it('dataShareBatchUpdateTest001', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
      console.log(TAG + "dataShareBatchUpdateTest001 start");
      if (datashareHelper == undefined) {
        console.log(TAG + "DSHelper is null");
        expect(true).assertFail();
      }
      let Record: Record<string, Array<dataShare.UpdateOperation>> = {};
      let operations1: Array<dataShare.UpdateOperation> = [];
      let operations2: Array<dataShare.UpdateOperation> = [];
      let pre1: dataSharePredicates.DataSharePredicates = new dataSharePredicates.DataSharePredicates();
      pre1.equalTo("name", "ZhangSan1");
      let vb1: ValuesBucket = {
        "name": "Lisi",
        "phoneNumber": 15126324856,
      }
      let operation1: dataShare.UpdateOperation = {
        values: vb1,
        predicates: pre1
      };
      operations1.push(operation1)
      let pre2: dataSharePredicates.DataSharePredicates = new dataSharePredicates.DataSharePredicates();
      pre2.equalTo("name", "ZhangSan2");
      let vb2: ValuesBucket = {
        "name": "Tom",
        "phoneNumber": 9999999999,
        "age": 30 as long,
        "isStudent": true
      }
      let operation2: dataShare.UpdateOperation = {
        values: vb2,
        predicates: pre2
      };
      operations2.push(operation2)
      Record["uri1"] = operations1;
      Record["uri2"] = operations2;
 
      try {
        let data = await dsHelper.batchUpdate(Record);
        data.forEach((values, key) => {
          for(let value of values) {
            expect(value != 0).assertTrue();
          }
        })
        console.info(TAG + `batchUpdate success`);
 
        let da = new dataSharePredicates.DataSharePredicates();
        da.equalTo("name", "Tom");
        let result = await dsHelper.query(dseUri, da, ["*"]);
        console.info(TAG + `ttt data.rowCount is `+ result.rowCount);
        expect(result.rowCount==1).assertTrue();done();
      } catch (err: BusinessError) {
        let code = err.code;
        let message = err.message;
        console.error(TAG + `Batch update error: code: ${code}, message: ${message} `);
        expect().assertFail();
      }
      done();
      console.log(TAG + "dataShareBatchUpdateTest001 end");
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0130
     * @tc.name      : dataShareBatchUpdateTest002
     * @tc.desc      : batchUpdate api update one record
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    it('dataShareBatchUpdateTest002', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
      console.log(TAG + "dataShareBatchUpdateTest002 start");
      if (datashareHelper == undefined) {
        console.log(TAG + "DSHelper is null");
        expect(true).assertFail();
      }
      let Record: Record<string, Array<dataShare.UpdateOperation>> = {};
      let operations1: Array<dataShare.UpdateOperation> = [];
      let pre1: dataSharePredicates.DataSharePredicates = new dataSharePredicates.DataSharePredicates();
      pre1.equalTo("name", "ZhangSan3");
      let vb1: ValuesBucket = {
        "name": "Amy",
        "phoneNumber": 15126324856,
      }
      let operation1: dataShare.UpdateOperation = {
        values: vb1,
        predicates: pre1
      };
      operations1.push(operation1)
      Record["uri1"] = operations1;
 
      try {
        let data = await dsHelper.batchUpdate(Record);
        data.forEach((values, key) => {
          for(let value of values) {
            expect(value != 0).assertTrue();
          }
        })
        console.info(TAG + `batchUpdate success`);
 
        let da = new dataSharePredicates.DataSharePredicates();
        da.equalTo("name", "Amy");
        let result = await dsHelper.query(dseUri, da, ["*"]);
        console.info(TAG + `ttt data.rowCount is `+ result.rowCount);
        expect(result.rowCount==1).assertTrue();
      } catch (err: BusinessError) {
        let code = err.code;
        let message = err.message;
        console.error(TAG + `Batch update error: code: ${code}, message: ${message} `);
        expect().assertFail();
      }
      done();
      console.log(TAG + "dataShareBatchUpdateTest002 end");
    })
 
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0140
     * @tc.name      : dataShareBatchUpdateTest003
     * @tc.desc      : batchUpdate api update 0 record
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    it('dataShareBatchUpdateTest003', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
      console.log(TAG + "dataShareBatchUpdateTest003 start");
      if (datashareHelper == undefined) {
        console.log(TAG + "DSHelper is null");
        expect(true).assertFail();
      }
      let Record: Record<string, Array<dataShare.UpdateOperation>> = {};
      let operations1: Array<dataShare.UpdateOperation> = [];
      let pre1: dataSharePredicates.DataSharePredicates = new dataSharePredicates.DataSharePredicates();
      pre1.equalTo("name", "Summer");
      let vb1: ValuesBucket = {
        "name": "WangWu",
        "phoneNumber": 15126324856,
      }
      let operation1: dataShare.UpdateOperation = {
        values: vb1,
        predicates: pre1
      };
      operations1.push(operation1)
      Record["uri1"] = operations1;
 
      try {
        let data = await dsHelper.batchUpdate(Record);
        data.forEach((values, key) => {
          for(let value of values) {
            expect(value != 0).assertTrue();
          }
        })
        console.info(TAG + `batchUpdate success`);
 
        let da = new dataSharePredicates.DataSharePredicates();
        da.equalTo("name", "WangWu");
        let result = await dsHelper.query(dseUri, da, ["*"]);
        console.info(TAG + `ttt data.rowCount is `+ result.rowCount);
        expect(result.rowCount==0).assertTrue();
      } catch (err: BusinessError) {
        let code = err.code;
        let message = err.message;
        console.error(TAG + `Batch update error: code: ${code}, message: ${message} `);
        expect().assertFail();
      }
      done();
      console.log(TAG + "dataShareBatchUpdateTest003 end");
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0150
     * @tc.name      : dataShareBatchUpdateTest004
     * @tc.desc      : batchUpdate api parameter is null
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    // it('dataShareBatchUpdateTest004', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
    //   console.log(TAG + "dataShareBatchUpdateTest004 start");
    //   if (datashareHelper == undefined) {
    //     console.log(TAG + "DSHelper is null");
    //     expect(true).assertFail();
    //   }
    //
    //   try {
    //     let data = await dsHelper.batchUpdate(null);
    //     console.info(TAG + `batchUpdate success`);
    //     expect(true).assertFail();
    //   } catch (err: BusinessError) {
    //     let code = err.code;
    //     let message = err.message;
    //     console.error(TAG + `Batch update error: code: ${code}, message: ${message} `);
    //     expect(code == 401).assertTrue();
    //   }
    //   done();
    //   console.log(TAG + "dataShareBatchUpdateTest004 end");
    // })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0160
     * @tc.name      : dataShareBatchUpdateTest005
     * @tc.desc      : batchUpdate api parameter is undefined
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    // it('dataShareBatchUpdateTest005', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
    //   console.log(TAG + "dataShareBatchUpdateTest005 start");
    //   if (datashareHelper == undefined) {
    //     console.log(TAG + "DSHelper is null");
    //     expect(true).assertFail();
    //   }
    //
    //   try {
    //     await dsHelper.batchUpdate(undefined);
    //     console.info(TAG + `batchUpdate success`);
    //     expect(true).assertFail();
    //   } catch (err: BusinessError) {
    //     let code = err.code;
    //     let message = err.message;
    //     console.error(TAG + `Batch update error: code: ${code}, message: ${message} `);
    //     expect(code == 401).assertTrue();
    //   };
    //   done();
    //   console.log(TAG + "dataShareBatchUpdateTest005 end");
    // })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0170
     * @tc.name      : dataShareBatchUpdateTest006
     * @tc.desc      : batchUpdate api UpdateOperation is over 4000
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    it('dataShareBatchUpdateTest006', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
      console.log(TAG + "dataShareBatchUpdateTest006 start");
      if (datashareHelper == undefined) {
        console.log(TAG + "DSHelper is null");
        expect(true).assertFail();
      }
      let Record: Record<string, Array<dataShare.UpdateOperation>> = {};
      for (let i = 10; i < 4020; i++) {
        let operations1: Array<dataShare.UpdateOperation> = [];
        let pre1: dataSharePredicates.DataSharePredicates = new dataSharePredicates.DataSharePredicates();
        pre1.equalTo("name", "ZhangSan" + i);
        let vb1: ValuesBucket = {
          "name": "Amy" + i,
          "phoneNumber": 9856899941,
          "age": 30 as long,
          "isStudent": true
        }
        let operation1: dataShare.UpdateOperation = {
          values: vb1,
          predicates: pre1
        };
        operations1.push(operation1)
        Record["uri"+i] = operations1;
      }
 
      try {
        let data = await dsHelper.batchUpdate(Record)
        console.info(TAG + `batchUpdate success`);
        expect(true).assertFail();
      } catch (err: BusinessError) {
        let code = err.code;
        let message = err.message;
        console.error(TAG + `Batch update error: code: ${code}, message: ${message} `);
        expect(code == 15700000).assertTrue();
      }
      done();
      console.log(TAG + "dataShareBatchUpdateTest006 end");
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0180
     * @tc.name      : dataShareBatchUpdateTest007
     * @tc.desc      : batchUpdate api Record is over 900K
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    it('dataShareBatchUpdateTest007', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
      console.log(TAG + "dataShareBatchUpdateTest007 start");
      if (datashareHelper == undefined) {
        console.log(TAG + "DSHelper is null");
        expect(true).assertFail();
      }
      let Record: Record<string, Array<dataShare.UpdateOperation>> = {};
      for (let i = 10; i < 3010; i++) {
        let operations1: Array<dataShare.UpdateOperation> = [];
        let pre1: dataSharePredicates.DataSharePredicates = new dataSharePredicates.DataSharePredicates();
        pre1.equalTo("name", "ZhangSan" + i);
        let vb1: ValuesBucket = {
          "name": "ZhangSan".repeat(20) + i,
          "phoneNumber": 9856899941,
          "age": 30 as long,
          "isStudent": true
        }
        let operation1: dataShare.UpdateOperation = {
          values: vb1,
          predicates: pre1
        };
        operations1.push(operation1)
        Record["uri"+i] = operations1;
      }
 
      try {
        let data = await dsHelper.batchUpdate(Record);
        console.info(TAG + `batchUpdate success`);
        expect(true).assertFail();
      } catch (err: BusinessError) {
        let code = err.code;
        let message = err.message;
        console.error(TAG + `Batch update error: code: ${code}, message: ${message} `);
        expect(err.code == 15700000).assertTrue();
      };
      done();
      console.log(TAG + "dataShareBatchUpdateTest007 end");
    })
  })
}