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
 
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect, TestType, Size, Level } from '../../../hypium/index';
import dataShare from '@ohos.data.dataShare';
import dataSharePredicates from '@ohos.data.dataSharePredicates';
import DataShareResultSet,{DataType} from '@ohos.data.DataShareResultSet';
import common from '@ohos.app.ability.common';
import { ValuesBucket } from "@ohos.data.ValuesBucket";
import Utils from './Util.test';
import { BusinessError } from '@ohos.base';
import { AppStorage } from '@ohos.arkui.stateManagement';
import UIAbility from '@ohos.app.ability.UIAbility';
import abilityDelegatorRegistry from '@ohos.app.ability.abilityDelegatorRegistry';
 
let context: common.UIAbilityContext;
let uri = ("datashareproxy://com.acts.ohos.data.datasharetest.static/test");
let dataShareHelper: dataShare.DataShareHelper;
const TAG = '[ttt] '
 
export default function DataShareResultSetTest() {
  describe('DataShareResultSetTest', () => {
    beforeAll(async () => {
      console.info(TAG + "beforeAll")
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
        abilityDelegator.executeShellCommand("aa start -a EntryAbility -b com.acts.datasharestabilityclient_rdb_resultset.static");
        await Utils.msSleep(2000);
        dataShareHelper = await dataShare.createDataShareHelper(context, uri, {isProxy: true});
        expect(dataShareHelper != undefined).assertTrue();
        console.info(TAG + " createDataShareHelper succeed, dataShareHelper : " + dataShareHelper);
        let predicates = new dataSharePredicates.DataSharePredicates();
        let res = await dataShareHelper!.delete(uri, predicates);
        console.info(TAG + "beforeAll delete success:" + res);
      } catch (err: BusinessError) {
        console.error(TAG + ` createDataShareHelper error: code: ${err.code}, message: ${err.message} `);
      }
    })
 
    beforeEach(async () => {
      for (let i = 0; i < 10; i++) {
        let vb: ValuesBucket = {
          "name0": "zhangSan" + i,
          "age": i as Long,
          "isStudent":true,
          "phoneNumber": new Uint8Array([i, i, i])
        };
        try {
          let ret = await dataShareHelper!.insert(uri, vb);
          console.log(TAG + "beforeEach insert end rowNum is " + ret);
        } catch (err: BusinessError) {
          console.error(TAG + 'beforeEach insert error' + err.code + "  " + err.message);
          expect().assertFail()
        }
      }
    })
 
    afterEach(async () => {
      let predicates = new dataSharePredicates.DataSharePredicates();
      let res = await dataShareHelper!.delete(uri, predicates);
      console.info(TAG + "afterEach delete success:" + res);
    })
 
    afterAll(async () => {
      await dataShareHelper.close();
    })
 
    /**
     * @tc.name Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0340
     * @tc.number SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0340
     * @tc.desc DataShareResultSet loop row operation
     * @tc.level: Level 2
     * @tc.type: RELIABILITY
     * @tc.size: MediumTest
     */
    it("Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0340", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL2, async () => {
      console.info(TAG + "************* Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0340 start *************");
      let columns = ["*"];
      let da = new dataSharePredicates.DataSharePredicates();
      try {
        let resultSet: DataShareResultSet = await dataShareHelper.query(uri, da, columns);
        expect(resultSet.goToFirstRow()).assertTrue();
        console.info(TAG + "goToFirstRow success");
        expect(resultSet.goToLastRow()).assertTrue();
        console.info(TAG + "goToLastRow success");
        expect(resultSet.goToNextRow()).assertFalse();
        console.info(TAG + "goToNextRow fail");
        expect(resultSet.goToPreviousRow()).assertTrue();
        console.info(TAG + "goToPreviousRow success");
        expect(resultSet.goTo(-9)).assertTrue();
        console.info(TAG + "goTo -9 success");
        expect(resultSet.goToRow(9)).assertTrue();
        console.info(TAG + "goToRow 9 success" );
        expect(resultSet.goToRow(10)).assertFalse();
        console.info(TAG + "goToRow 10 fail");
        let vb: ValuesBucket = {
          "name0": "lisi",
          "age": 99 as Long,
          "isStudent":false,
          "phoneNumber": 99.01
        };
        let ret = await dataShareHelper.insert(uri, vb);
        console.log(TAG + " insert end rowNum is " + ret);
        resultSet.close();
        resultSet = await dataShareHelper.query(uri, da, columns);
        expect(resultSet.goToFirstRow()).assertTrue();
        console.info(TAG + "goToFirstRow success");
        expect(resultSet.goToPreviousRow()).assertFalse();
        console.info(TAG + "goToPreviousRow fail");
        expect(resultSet.goToNextRow()).assertTrue();
        console.info(TAG + "goToNextRow success");
        expect(resultSet.goToLastRow()).assertTrue();
        console.info(TAG + "goToLastRow success");
        expect(resultSet.goTo(1)).assertFalse();
        console.info(TAG + "goTo 1 fail");
        resultSet.close();
      } catch (err: BusinessError) {
        console.error(TAG + 'query error' + err.code + "  " + err.message);
        expect().assertFail()
      }
 
      console.info(TAG + "************* Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0340 end   *************");
    })
 
    /**
     * @tc.name Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0350
     * @tc.number SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0350
     * @tc.desc DataShareResultSet loop get operation
     * @tc.level: Level 2
     * @tc.type: RELIABILITY
     * @tc.size: MediumTest
     */
    it("Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0350", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL2, async () => {
      console.info(TAG + "************* Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0350 start *************");
      let columns = ["*"];
      let da = new dataSharePredicates.DataSharePredicates();
      try {
        let resultSet: DataShareResultSet = await dataShareHelper.query(uri, da, columns);
        expect(resultSet.goToFirstRow()).assertTrue();
        let blob1 = resultSet.getBlob(resultSet.getColumnIndex("phoneNumber"));
        console.info(TAG + "blob1:" + JSON.stringify(blob1));
        expect(JSON.stringify(new Uint8Array([0, 0, 0]))).assertEqual(JSON.stringify(blob1));
        let str1 = resultSet.getString(resultSet.getColumnIndex("name0"));
        console.info(TAG + "str1:" + JSON.stringify(str1));
        expect("zhangSan0").assertEqual(str1);
        let long1 = resultSet.getLong(resultSet.getColumnIndex("age"));
        console.info(TAG + "long1:" + JSON.stringify(long1));
        expect(0).assertEqual(long1);
        let double1 = resultSet.getDouble(resultSet.getColumnIndex("age"));
        console.info(TAG + "double1:" + JSON.stringify(double1));
        expect(0).assertEqual(double1);
        expect(resultSet.goToLastRow()).assertTrue();
        let blob2 = resultSet.getBlob(resultSet.getColumnIndex("phoneNumber"));
        console.info(TAG + "blob2:" + JSON.stringify(blob2));
        expect(JSON.stringify(new Uint8Array([9, 9, 9]))).assertEqual(JSON.stringify(blob2));
        let str2 = resultSet.getString(resultSet.getColumnIndex("name0"));
        console.info(TAG + "str2:" + JSON.stringify(str2));
        expect("zhangSan9").assertEqual(str2);
        let long2 = resultSet.getLong(resultSet.getColumnIndex("age"));
        console.info(TAG + "long2:" + JSON.stringify(long2));
        expect(9).assertEqual(long2);
        let double2 = resultSet.getDouble(resultSet.getColumnIndex("age"));
        console.info(TAG + "double2:" + JSON.stringify(double2));
        expect(9).assertEqual(double2);
        expect(3).assertEqual(resultSet.getDataType(resultSet.getColumnIndex("name0")))
        expect("name0").assertEqual(resultSet.getColumnName(1));
        resultSet.close();
        let vb: ValuesBucket = {
          "name0": "lisi",
          "age": 99 as Long,
          "isStudent":false,
          "phoneNumber": new Uint8Array([1, 2, 3])
        };
        let ret = await dataShareHelper.insert(uri, vb);
        resultSet = await dataShareHelper.query(uri, da, columns);
        expect(resultSet.goToLastRow()).assertTrue();
        let blob3 = resultSet.getBlob(resultSet.getColumnIndex("phoneNumber"));
        console.info(TAG + "blob3:" + JSON.stringify(blob3));
        expect(JSON.stringify(new Uint8Array([1, 2, 3]))).assertEqual(JSON.stringify(blob3));
        let str3 = resultSet.getString(resultSet.getColumnIndex("name0"));
        console.info(TAG + "str3:" + JSON.stringify(str3));
        expect("lisi").assertEqual(str3);
        let long3 = resultSet.getLong(resultSet.getColumnIndex("age"));
        console.info(TAG + "long3:" + JSON.stringify(long3));
        expect(99).assertEqual(long3);
        let double3 = resultSet.getDouble(resultSet.getColumnIndex("age"));
        console.info(TAG + "double3:" + JSON.stringify(double3));
        expect(99).assertEqual(double3);
        resultSet.close();
      } catch (err: BusinessError) {
        console.error(TAG + 'query error' + err.code + "  " + err.message);
        expect().assertFail()
      }
 
      console.info(TAG + "************* Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0350 end   *************");
    })
 
    /**
     * @tc.name Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0360
     * @tc.number SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0360
     * @tc.desc goToFirstRow
     * @tc.level: Level 2
     * @tc.type: RELIABILITY
     * @tc.size: MediumTest
     */
    it("Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0360", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL2, async () => {
      console.info(TAG + "************* Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0360 start *************");
      let columns = ["*"];
      let da = new dataSharePredicates.DataSharePredicates();
      try {
        let resultSet: DataShareResultSet = await dataShareHelper.query(uri, da, columns);
        expect(resultSet.goToFirstRow()).assertTrue();
        console.info(TAG + "goToFirstRow success");
        let res = await dataShareHelper.delete(uri, da);
        console.info(TAG + "Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0360 delete success:" + res);
        console.info(TAG + "Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0360 resultSet rowCount:" + resultSet.rowCount);
        expect(resultSet.rowCount).assertEqual(10);
        resultSet.close();
        console.info(TAG + "Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0360 resultSet close success");
        let resultSet2: DataShareResultSet = await dataShareHelper.query(uri, da, columns);
        console.info(TAG + "Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0360 resultSet2 rowCount:" + resultSet2.rowCount);
        expect(resultSet2.rowCount).assertEqual(0);
        expect(resultSet2.goToFirstRow()).assertFalse();
        console.info(TAG + "goToFirstRow fail");
      } catch (err: BusinessError) {
        console.error(TAG + 'goToFirstRow' + err.code + "  " + err.message);
        expect().assertFail()
      }
      console.info(TAG + "************* Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0360 end   *************");
    })
 
    /**
     * @tc.name Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0370
     * @tc.number SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0370
     * @tc.desc goToLastRow
     * @tc.level: Level 2
     * @tc.type: RELIABILITY
     * @tc.size: MediumTest
     */
    it("Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0370", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL2, async () => {
      console.info(TAG + "************* Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0370 start *************");
      let columns = ["*"];
      let da = new dataSharePredicates.DataSharePredicates();
      try {
        let resultSet: DataShareResultSet = await dataShareHelper.query(uri, da, columns);
        expect(resultSet.goToLastRow()).assertTrue();
        console.info(TAG + "goToLastRow success");
        let res = await dataShareHelper.delete(uri, da);
        console.info(TAG + "Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0370 delete success:" + res);
        console.info(TAG + "Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0370 delete success:" + resultSet.rowCount);
        expect(resultSet.rowCount).assertEqual(10);
        resultSet.close();
        console.info(TAG + "Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0370 resultSet close success");
        let resultSet2: DataShareResultSet = await dataShareHelper.query(uri, da, columns);
        console.info(TAG + "Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0370 resultSet2 rowCount:" + resultSet2.rowCount);
        expect(resultSet2.rowCount).assertEqual(0);
        expect(resultSet.goToLastRow()).assertFalse();
        console.info(TAG + "goToLastRow fail");
      } catch (err: BusinessError) {
        console.error(TAG + 'query error' + err.code + "  " + err.message);
        expect().assertFail()
      }
      console.info(TAG + "************* Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0370 end   *************");
    })
 
    /**
     * @tc.name Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0380
     * @tc.number SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0380
     * @tc.desc goToNextRow
     * @tc.level: Level 2
     * @tc.type: RELIABILITY
     * @tc.size: MediumTest
     */
    it("Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0380", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL2, async () => {
      console.info(TAG + "************* Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0380 start *************");
      let columns = ["*"];
      let da = new dataSharePredicates.DataSharePredicates();
      try {
        let resultSet: DataShareResultSet = await dataShareHelper.query(uri, da, columns);
        expect(resultSet.goToFirstRow()).assertTrue();
        console.info(TAG + "goToFirstRow success");
        expect(resultSet.goToNextRow()).assertTrue();
        console.info(TAG + "goToNextRow success");
        let res = await dataShareHelper.delete(uri, da);
        console.info(TAG + "Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0380 delete success:" + res);
        console.info(TAG + "Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0380 delete success:" + resultSet.rowCount);
        expect(resultSet.rowCount).assertEqual(10);
        resultSet.close();
        console.info(TAG + "Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0380 resultSet close success");
        let resultSet2: DataShareResultSet = await dataShareHelper.query(uri, da, columns);
        console.info(TAG + "Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0380 resultSet2 rowCount:" + resultSet2.rowCount);
        expect(resultSet2.rowCount).assertEqual(0);
        expect(resultSet.goToNextRow()).assertFalse();
        console.info(TAG + "goToNextRow fail");
      } catch (err: BusinessError) {
        console.error(TAG + 'query error' + err.code + "  " + err.message);
        expect().assertFail()
      }
      console.info(TAG + "************* Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0380 end   *************");
    })
 
    /**
     * @tc.name Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0390
     * @tc.number SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0390
     * @tc.desc goToPreviousRow
     * @tc.level: Level 2
     * @tc.type: RELIABILITY
     * @tc.size: MediumTest
     */
    it("Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0390", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL2, async () => {
      console.info(TAG + "************* Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0390 start *************");
      let columns = ["*"];
      let da = new dataSharePredicates.DataSharePredicates();
      try {
        let resultSet: DataShareResultSet = await dataShareHelper.query(uri, da, columns);
        expect(resultSet.goToLastRow()).assertTrue();
        console.info(TAG + "goToLastRow success");
        expect(resultSet.goToPreviousRow()).assertTrue();
        console.info(TAG + "goToPreviousRow success");
        let res = await dataShareHelper.delete(uri, da);
        console.info(TAG + "Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0390 delete success:" + res);
        console.info(TAG + "Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0390 delete success:" + resultSet.rowCount);
        expect(resultSet.rowCount).assertEqual(10);
        resultSet.close();
        console.info(TAG + "Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0390 resultSet close success");
        let resultSet2: DataShareResultSet = await dataShareHelper.query(uri, da, columns);
        console.info(TAG + "Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0390 resultSet2 rowCount:" + resultSet2.rowCount);
        expect(resultSet2.rowCount).assertEqual(0);
        expect(resultSet.goToPreviousRow()).assertFalse();
        console.info(TAG + "goToPreviousRow fail");
      } catch (err: BusinessError) {
        console.error(TAG + 'query error' + err.code + "  " + err.message);
        expect().assertFail()
      }
      console.info(TAG + "************* Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0390 end   *************");
    })
 
    /**
     * @tc.name Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0400
     * @tc.number SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0400
     * @tc.desc goTo
     * @tc.level: Level 2
     * @tc.type: RELIABILITY
     * @tc.size: MediumTest
     */
    it("Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0400", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL2, async () => {
      console.info(TAG + "************* Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0400 start *************");
      let columns = ["*"];
      let da = new dataSharePredicates.DataSharePredicates();
      try {
        let resultSet: DataShareResultSet = await dataShareHelper.query(uri, da, columns);
        expect(resultSet.goToFirstRow()).assertTrue();
        console.info(TAG + "goToFirstRow success");
        expect(resultSet.goTo(1)).assertTrue();
        console.info(TAG + "goTo 1  success");
        let res = await dataShareHelper.delete(uri, da);
        console.info(TAG + "Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0400 delete success:" + res);
        console.info(TAG + "Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0400 delete success:" + resultSet.rowCount);
        expect(resultSet.rowCount).assertEqual(10);
        resultSet.close();
        console.info(TAG + "Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0400 resultSet close success");
        let resultSet2: DataShareResultSet = await dataShareHelper.query(uri, da, columns);
        console.info(TAG + "Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0400 resultSet2 rowCount:" + resultSet2.rowCount);
        expect(resultSet2.rowCount).assertEqual(0);
        expect(resultSet.goTo(1)).assertFalse();
        console.info(TAG + "goTo 1 fail");
      } catch (err: BusinessError) {
        console.error(TAG + 'query error' + err.code + "  " + err.message);
        expect().assertFail()
      }
      console.info(TAG + "************* Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0400 end   *************");
    })
 
    /**
     * @tc.name Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0410
     * @tc.number SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0410
     * @tc.desc goToRow
     * @tc.level: Level 2
     * @tc.type: RELIABILITY
     * @tc.size: MediumTest
     */
    it("Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0410", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL2, async () => {
      console.info(TAG + "************* Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0410 start *************");
      let columns = ["*"];
      let da = new dataSharePredicates.DataSharePredicates();
      try {
        let resultSet: DataShareResultSet = await dataShareHelper.query(uri, da, columns);
        expect(resultSet.goToRow(0)).assertTrue();
        console.info(TAG + "goToRow 0  success");
        let res = await dataShareHelper.delete(uri, da);
        console.info(TAG + "Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0410 delete success:" + res);
        console.info(TAG + "Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0410 delete success:" + resultSet.rowCount);
        expect(resultSet.rowCount).assertEqual(10);
        resultSet.close();
        console.info(TAG + "Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0410 resultSet close success");
        let resultSet2: DataShareResultSet = await dataShareHelper.query(uri, da, columns);
        console.info(TAG + "Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0410 resultSet2 rowCount:" + resultSet2.rowCount);
        expect(resultSet2.rowCount).assertEqual(0);
        expect(resultSet.goToRow(0)).assertFalse();
        console.info(TAG + "goToRow 0 fail");
      } catch (err: BusinessError) {
        console.error(TAG + 'query error' + err.code + "  " + err.message);
        expect().assertFail()
      }
      console.info(TAG + "************* Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0410 end   *************");
    })
 
    /**
     * @tc.name Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0420
     * @tc.number SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0420
     * @tc.desc 200k
     * @tc.level: Level 2
     * @tc.type: RELIABILITY
     * @tc.size: MediumTest
     */
//    it("Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0420", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL2, async () => {
//      console.info(TAG + "************* Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0420 start *************");
//      let columns = ["*"];
//      let da = new dataSharePredicates.DataSharePredicates();
//      try {
//        let vb: ValuesBucket = {
//          "name0": "a".repeat(204607),
//          "age": 111 as Long
//        };
//        let ret = await dataShareHelper.insert(uri, vb);
//        da.equalTo("age", 111 as Long)
//        let resultSet: DataShareResultSet = await dataShareHelper.query(uri, da, columns);
//        expect(resultSet.rowCount).assertEqual(1)
//        resultSet.close();
//        console.info(TAG + "Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0420 resultSet close success");
//      }catch (err: BusinessError) {
//        console.error(TAG + 'query error' + err.code + "  " + err.message);
//        expect().assertFail()
//      }
//      console.info(TAG + "************* Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0420 end   *************");
//    })
 
    /**
     * @tc.name Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0430
     * @tc.number SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0430
     * @tc.desc  > 200k
     * @tc.level: Level 2
     * @tc.type: RELIABILITY
     * @tc.size: MediumTest
     */
    it("Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0430", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL2, async () => {
      console.info(TAG + "************* Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0430 start *************");
      let columns = ["*"];
      let da = new dataSharePredicates.DataSharePredicates();
      try {
        let vb: ValuesBucket = {
          "name0": "a".repeat(1024*201),
          "age": 111 as Long
        };
        let ret = await dataShareHelper.insert(uri, vb);
        da.equalTo("age", 111 as Long)
        let resultSet: DataShareResultSet = await dataShareHelper.query(uri, da, columns);
        resultSet.close();
        console.info(TAG + "Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0430 resultSet close success");
        expect().assertFail();
      } catch (err: BusinessError) {
        console.error(TAG + 'query error' + err.code + "  " + err.message);
        expect(err.code).assertEqual(15700000)
      }
      console.info(TAG + "************* Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0430 end   *************");
    })
 
    /**
     * @tc.name Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0450
     * @tc.number SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0450
     * @tc.desc  insert 200k 20items
     * @tc.level Level 2
     * @tc.type Function
     * @tc.size MediumTest
     */
//    it("Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0450", TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL2, async () => {
//      console.info(TAG + "************* Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0450 start *************");
//      let columns = ["*"];
//      let da = new dataSharePredicates.DataSharePredicates();
//      try {
//        console.info(TAG + "Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0450");
//        for (let i = 0; i < 20; i++) {
//          let vb: ValuesBucket = {
//            "name0": "a".repeat(204605) + i,
//            "age": 100 as Long
//          };
//          await dataShareHelper.insert(uri, vb);
//        }
//        console.info(TAG + "Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0450 insert success");
//        da.equalTo("age", 100 as Long)
//        let resultSet: DataShareResultSet = await dataShareHelper.query(uri, da, columns);
//        expect(resultSet.rowCount).assertEqual(20);
//        for(let j = 0; j < resultSet.rowCount; j++){
//          resultSet.goToRow(j);
//          let valueStr:string = resultSet.getString(1);
//          console.info(TAG + "valueStr is: " + valueStr);
//          expect(JSON.stringify(valueStr).includes("a".repeat(204605) + j)).assertTrue();
//        }
//        resultSet.close();
//        console.info(TAG + "Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0450 resultSet close success");
//      } catch (err: BusinessError) {
//        console.error(TAG + 'query error' + err.code + "  " + err.message);
//        expect().assertFail()
//      }
//      console.info(TAG + "************* Test_SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0450 end   *************");
//    })
  })
}