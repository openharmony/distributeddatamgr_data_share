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
import { AppStorage } from '@ohos.arkui.stateManagement'
import common from '@ohos.app.ability.common';
import UIAbility from '@ohos.app.ability.UIAbility';
import abilityDelegatorRegistry from '@ohos.app.ability.abilityDelegatorRegistry';
 
const TAG = "[ttt]";
let dataShareHelper: dataShare.DataShareHelper;
let context: common.UIAbilityContext;
let dseUri = "datashare:///com.acts.ohos.data.datashareresultkv.static";
 
export default function DataShareResultSetKvdbTestMtbf() {
  describe('DataShareResultSetKvdbTestMtbf', () => {
    beforeAll(async () => {
      console.info(TAG + "beforeAll");
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
      abilityDelegator.executeShellCommand("aa start -a EntryAbility -b com.acts.datashareresultsetstabilityclient.static");
      await Utils.msSleep(2000);
    });
 
    beforeEach(async () => {
      console.info(TAG + "beforeEach");
      try {
        dataShareHelper = await dataShare.createDataShareHelper(context, dseUri);
        await Utils.msSleep(1000)
        console.info(TAG + "createDataShareHelper success")
      } catch (err: BusinessError) {
        console.error(TAG + "error. err.code:" + err.code + "err.message:" + err.message)
        expect().assertFail();
      }
    });
 
    afterEach(async () => {
      console.info(TAG + "afterEach");
      let da = new dataSharePredicates.DataSharePredicates();
      da.inKeys(["testInsertKey"]);
      await dataShareHelper.delete(dseUri, da);
    });
 
    afterAll(async () => {
      await dataShareHelper.close();
    });
 
    /**
     * @tc.name testDataShareResultKv0010
     * @tc.number SUB_DistributedData_DFX_MTBF_DataShare_ResultSetKVTest_0010
     * @tc.desc ResultSet test
     * @tc.level: Level 3
     * @tc.type: RELIABILITY
     * @tc.size: MediumTest
     */
//    it('testDataShareResultKv0010', TestType.RELIABILITY | Size.MEDIUMTEST | Level.LEVEL3, async () => {
//      console.info(TAG + "testDataShareResultKv0010 start")
//      try{
//        for (let i = 0; i < 2000; i++) {
//          let resultSet: DataShareResultSet | undefined = undefined;
//          let resultSet2: DataShareResultSet | undefined = undefined;
//          let vbs: ValuesBucket = { "name": "zhangsan", "age": 99 as Long, "isStudent": true, "key":"testInsertKey" };
//          await dataShareHelper.insert(dseUri, vbs);
//          let predicates = new dataSharePredicates.DataSharePredicates();
//          predicates.equalTo("name", "zhangsan");
//          resultSet = await dataShareHelper.query(dseUri, predicates, ["*"]);
//          console.info(TAG + "rowCount1 is " + resultSet.rowCount);
//          expect(resultSet.rowCount).assertEqual(1);
//          expect(resultSet.goToFirstRow()).assertTrue();
//          console.info(TAG + "goToFirstRow success");
//          resultSet.close();
//          //删除一条数据
//          let da = new dataSharePredicates.DataSharePredicates();
//          da.inKeys(["testInsertKey"]);
//          await dataShareHelper.delete(dseUri, da);
//
//          let predicates2 = new dataSharePredicates.DataSharePredicates();
//          predicates2.equalTo("name", "zhangsan");
//          resultSet2 = await dataShareHelper.query(dseUri, predicates2, ["*"]);
//          console.info(TAG + "rowCount2 is " + resultSet2.rowCount);
//          expect(resultSet2.rowCount).assertEqual(0);
//          expect(resultSet.goToFirstRow()).assertFalse();
//          console.info(TAG + "goToFirstRow fail");
//          expect(resultSet.goToNextRow()).assertFalse();
//          console.info(TAG + "goToNextRow fail");
//          expect(resultSet.goToLastRow()).assertFalse();
//          console.info(TAG + "goToLastRow fail");
//          expect(resultSet.goToPreviousRow()).assertFalse();
//          console.info(TAG + "goToPreviousRow fail");
//          expect(resultSet.goTo(-1)).assertFalse();
//          console.info(TAG + "goTo -1 fail"); //向前1行
//          expect(resultSet.goToRow(2)).assertFalse();
//          console.info(TAG + "goToRow 0 fail"); //下标从0开始,跳转最后一行
//          expect(resultSet.goToRow(1)).assertFalse();
//          console.info(TAG + "goToRow 1 fail"); //下标从0开始,跳转第二行
//          resultSet2.close();
//        }
//      } catch (err: BusinessError) {
//        console.error(TAG + "testDataShareResultKv0010 error. err.code:" + err.code + "err.message:" + err.message)
//        expect().assertFail();
//      }
//      console.info(TAG + "testDataShareResultKv0010 end")
//    })
 
    /**
     * @tc.name testDataShareResultKv0020
     * @tc.number SUB_DistributedData_DFX_MTBF_DataShare_ResultSetKVTest_0020
     * @tc.desc ResultSet test
     * @tc.level: Level 3
     * @tc.type: RELIABILITY
     * @tc.size: MediumTest
     */
//    it('testDataShareResultKv0020', TestType.RELIABILITY | Size.MEDIUMTEST | Level.LEVEL3, async () => {
//      console.info(TAG + "testDataShareResultKv0020 start")
//      try{
//        for (let i = 0; i < 2000; i++) {
//          let resultSet: DataShareResultSet | undefined = undefined;
//          let resultSet2: DataShareResultSet | undefined = undefined;
//          let vbs: ValuesBucket = { "name": "zhangsan", "age": 88 as Long, "isStudent": true,"key":"testInsertKey" };
//          await dataShareHelper.insert(dseUri, vbs);
//          let predicates = new dataSharePredicates.DataSharePredicates();
//          predicates.equalTo("name", "zhangsan");
//          resultSet = await dataShareHelper.query(dseUri, predicates, ["*"]);
//          expect(resultSet.rowCount).assertEqual(1);
//          expect(resultSet.goToFirstRow()).assertTrue();
//          let str1 = resultSet.getString(resultSet.getColumnIndex("key"));
//          console.info(TAG + "str1:" + JSON.stringify(str1));
//          expect(str1).assertEqual("testInsertKey");
//          let str2 = resultSet.getColumnName(1);
//          console.info(TAG + "str2:" + JSON.stringify(str2));
//          expect(str2).assertEqual("value");
//          let long1 = resultSet.getLong(resultSet.getColumnIndex("key"));
//          console.info(TAG + "long1:" + JSON.stringify(long1));
//          expect(long1).assertEqual(0);
//          let type = resultSet.getDataType(resultSet.getColumnIndex("key"));
//          console.info(TAG + "type:" + JSON.stringify(type));
//          expect(type).assertEqual(3)
//          resultSet.close();
//
//          let vb: ValuesBucket = { "name": "linjunjie", "age": 35 as Long, "isStudent": false,"key":"testInsertKey" };
//          let data = await dataShareHelper.insert(dseUri, vb);
//          console.info(TAG + "insert data is: " + JSON.stringify(data));
//
//          let predicates2 = new dataSharePredicates.DataSharePredicates();
//          predicates2.equalTo("name", "linjunjie");
//          resultSet2 = await dataShareHelper.query(dseUri, predicates2, ["*"]);
//          expect(resultSet2.rowCount).assertEqual(1);
//          let str3 = resultSet2.getString(resultSet2.getColumnIndex("key"));
//          console.info(TAG + "str3:" + JSON.stringify(str3));
//          expect(str1).assertEqual("testInsertKey");
//          let str4 = resultSet2.getColumnName(1);
//          console.info(TAG + "str4:" + JSON.stringify(str4));
//          expect(str2).assertEqual("value");
//          let long2 = resultSet2.getLong(resultSet2.getColumnIndex("key"));
//          console.info(TAG + "long1:" + JSON.stringify(long2));
//          expect(long1).assertEqual(0);
//          let type2 = resultSet2.getDataType(resultSet2.getColumnIndex("key"));
//          console.info(TAG + "type:" + JSON.stringify(type2));
//          expect(type).assertEqual(3)
//          resultSet2.close();
//        }
//      } catch (err: BusinessError) {
//        console.error(TAG + "testDataShareResultKv0020 error. err.code:" + err.code + "err.message:" + err.message)
//        expect().assertFail();
//      }
//      console.info(TAG + "testDataShareResultKv0020 end");
//    })
 
    /**
     * @tc.name testDataShareResultKv0030
     * @tc.number SUB_DistributedData_DFX_MTBF_DataShare_ResultSetKVTest_0030
     * @tc.desc ResultSet test
     * @tc.level: Level 3
     * @tc.type: RELIABILITY
     * @tc.size: MediumTest
     */
//    it('testDataShareResultKv0030', TestType.RELIABILITY | Size.MEDIUMTEST | Level.LEVEL3, async () => {
//      console.info(TAG + "testDataShareResultKv0030 start")
//      try {
//        for (let i = 0; i < 2000; i++) {
//          let resultSet: DataShareResultSet | undefined = undefined;
//          let nameStr = "zhangsan".repeat(26214); //小于1M
//          let vbs: ValuesBucket = { "name": nameStr, "age": 88 as Long, "isStudent": true,"key":"testInsertKey" };
//          await dataShareHelper.insert(dseUri, vbs);
//          let predicates = new dataSharePredicates.DataSharePredicates();
//          predicates.equalTo("age", "88");
//          resultSet = await dataShareHelper.query(dseUri, predicates, ["*"]);
//          expect(resultSet.rowCount).assertEqual(1);
//          resultSet.close();
//        }
//      } catch (err: BusinessError) {
//        console.error(TAG + "insert error. err.code:" + err.code + "err.message:" + err.message)
//        expect().assertFail();
//      }
//      console.info(TAG + "testDataShareResultKv0030 end");
//    })
  })
}