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
import common from '@ohos.app.ability.common';
import { ValuesBucket } from "@ohos.data.ValuesBucket";
import { AppStorage } from '@ohos.arkui.stateManagement';
import Utils from './Util.test';
import { BusinessError } from '@ohos.base';
 
const TAG = "[ttt]";
let dataShareHelper: dataShare.DataShareHelper;
let context: common.UIAbilityContext;
let dseUri = "datashare:///com.acts.datasharekvencryptclient";
 
export default function DataShareResultSetKvdbTest() {
  describe('DataShareResultSetKvdbTest', () => {
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
      let vbs: Array<string>= []
      for(let i = 0; i < 20; i++){
        let vb = "testkey"+i;
        vbs.push(vb);
      }
      da.inKeys(vbs);
      await dataShareHelper.delete(dseUri, da);
    });
    afterAll(async () => {
      await dataShareHelper.close();
    });
 
    /**
     * @tc.name testDataShareResultKv0010
     * @tc.number SUB_DistributedData_DataShare_SDK_ResultSetKVTest_0010
     * @tc.desc ResultSet test
     * @tc.level: Level 2
     * @tc.type: Function
     * @tc.size: MediumTest
     */
    it('testDataShareResultKv0010', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL2, async () => {
      console.info(TAG + "testDataShareResultKv0010 start")
      let resultSet: DataShareResultSet | undefined = undefined;
      try {
        let predicates = new dataSharePredicates.DataSharePredicates();
        resultSet = await dataShareHelper.query(dseUri, predicates, ["*"]);
        expect(resultSet.rowCount).assertEqual(3);
        expect(resultSet.goToFirstRow()).assertTrue();
        console.info(TAG + "goToFirstRow success");
        expect(resultSet.goToNextRow()).assertTrue();
        console.info(TAG + "goToNextRow success");
        expect(resultSet.goToLastRow()).assertTrue();
        console.info(TAG + "goToLastRow success");
        expect(resultSet.goToPreviousRow()).assertTrue();
        console.info(TAG + "goToPreviousRow success");
        expect(resultSet.goTo(-1)).assertTrue();
        console.info(TAG + "goTo -1 success");//向前1行
        expect(resultSet.goToRow(2)).assertTrue();
        console.info(TAG + "goToRow 0 success" );//下标从0开始,跳转最后一行
        expect(resultSet.goToRow(1)).assertTrue();
        console.info(TAG + "goToRow 1 success");//下标从0开始,跳转第二行
        try {
          //删除一条数据
          let da = new dataSharePredicates.DataSharePredicates();
          da.inKeys(["testkey0"]);
          await dataShareHelper.delete(dseUri, da);
        } catch (err: BusinessError) {
          console.error(TAG + `delete error: code: ${err.code}, message: ${err.message} `);
          expect().assertFail();
        };
        let predicates2 = new dataSharePredicates.DataSharePredicates();
        resultSet= await dataShareHelper.query(dseUri, predicates2, ["*"]);
        console.info(TAG + "rowCount is " +resultSet.rowCount);
        expect(resultSet.rowCount).assertEqual(2);
        expect(resultSet.goToFirstRow()).assertTrue();
        console.info(TAG + "goToFirstRow success");
        expect(resultSet.goToNextRow()).assertTrue();
        console.info(TAG + "goToNextRow success");
        expect(resultSet.goToLastRow()).assertTrue();
        console.info(TAG + "goToLastRow success");
        expect(resultSet.goToPreviousRow()).assertTrue();
        console.info(TAG + "goToPreviousRow success");
        expect(resultSet.goTo(-1)).assertFalse();
        console.info(TAG + "goTo -1 fail");//向前1行
        expect(resultSet.goToRow(2)).assertFalse();
        console.info(TAG + "goToRow 0 fail" );//下标从0开始,跳转最后一行
        expect(resultSet.goToRow(1)).assertTrue();
        console.info(TAG + "goToRow 1 success");//下标从0开始,跳转第二行
        resultSet.close();
      } catch (err: BusinessError) {
        console.error(TAG + "testDataShareResultKv0010 error. err.code:" + err.code + "err.message:" + err.message)
        expect().assertFail();
      }
      console.info(TAG + "testDataShareResultKv0010 end")
    })
 
    /**
     * @tc.name testDataShareResultKv0020
     * @tc.number SUB_DistributedData_DataShare_SDK_ResultSetKVTest_0020
     * @tc.desc ResultSet test
     * @tc.level: Level 2
     * @tc.type: Function
     * @tc.size: MediumTest
     */
    it('testDataShareResultKv0020', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL2, async () => {
      console.info(TAG + "testDataShareResultKv0020 start")
      let resultSet: DataShareResultSet | undefined = undefined;
      try {
        let predicates = new dataSharePredicates.DataSharePredicates();
        predicates.equalTo("name", "zhangsan");
        resultSet= await dataShareHelper.query(dseUri, predicates, ["*"]);
        expect(resultSet.rowCount).assertEqual(1);
        expect(resultSet.goToFirstRow()).assertTrue();
        let str1 = resultSet.getString(resultSet.getColumnIndex("key"));
        console.info(TAG + "str1:" + JSON.stringify(str1));
        expect(str1).assertEqual("testkey0");
        let str2 = resultSet.getColumnName(1);
        console.info(TAG + "str2:" + JSON.stringify(str2));
        expect(str2).assertEqual("value");
        let long1 = resultSet.getLong(resultSet.getColumnIndex("key"));
        console.info(TAG + "long1:" + JSON.stringify(long1));
        expect(long1).assertEqual(0);
        let type = resultSet.getDataType(resultSet.getColumnIndex("key"));
        console.info(TAG + "type:" + JSON.stringify(type));
        expect(type).assertEqual(3)
 
        let vb: ValuesBucket = { "name": "linjunjie", "age": 35 as Long, "isStudent": false };
        let data = await dataShareHelper.insert(dseUri, vb);
        console.info(TAG + "insert data is: " + JSON.stringify(data));
 
        let predicates2 = new dataSharePredicates.DataSharePredicates();
        predicates2.equalTo("name", "linjunjie");
        resultSet = await dataShareHelper.query(dseUri, predicates2, ["*"]);
        expect(resultSet.rowCount).assertEqual(1);
        let str3 = resultSet.getString(resultSet.getColumnIndex("key"));
        console.info(TAG + "str3:" + JSON.stringify(str3));
        expect(str1).assertEqual("testkey0");
        let str4 = resultSet.getColumnName(1);
        console.info(TAG + "str4:" + JSON.stringify(str4));
        expect(str2).assertEqual("value");
        let long2 = resultSet.getLong(resultSet.getColumnIndex("key"));
        console.info(TAG + "long1:" + JSON.stringify(long2));
        expect(long1).assertEqual(0);
        let type2 = resultSet.getDataType(resultSet.getColumnIndex("key"));
        console.info(TAG + "type:" + JSON.stringify(type2));
        expect(type).assertEqual(3)
        resultSet.close();
      } catch (err: BusinessError) {
        console.error(TAG + "catch error. err.code:" + err.code + "err.message:" + err.message)
        expect().assertFail();
      }
      console.info(TAG + "testDataShareResultKv0020 end");
    })
 
    /**
     * @tc.name testDataShareResultKv0030
     * @tc.number SUB_DistributedData_DataShare_SDK_ResultSetKVTest_0030
     * @tc.desc ResultSet test
     * @tc.level: Level 2
     * @tc.type: Function
     * @tc.size: MediumTest
     */
    it('testDataShareResultKv0030', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL2, async () => {
      console.info(TAG + "testDataShareResultKv0030 start")
      let resultSet: DataShareResultSet | undefined = undefined;
      try {
        let predicates = new dataSharePredicates.DataSharePredicates();
        predicates.equalTo("name", "zhangsan");
        resultSet= await dataShareHelper.query(dseUri, predicates, ["*"]);
        console.info(TAG + "rowCount1 is " +resultSet.rowCount);
        expect(resultSet.rowCount).assertEqual(1);
        expect(resultSet.goToFirstRow()).assertTrue();
        console.info(TAG + "goToFirstRow success");
        try {
          //删除一条数据
          let da = new dataSharePredicates.DataSharePredicates();
          da.inKeys(["testkey0"]);
          await dataShareHelper.delete(dseUri, da);
        } catch (err: BusinessError) {
          console.error(TAG +`delete error: code: ${err.code}, message: ${err.message} `);
          expect().assertFail();
        };
        expect(resultSet.rowCount).assertEqual(1);//resultSet没有刷新
        let predicates2 = new dataSharePredicates.DataSharePredicates();
        predicates2.equalTo("name", "zhangsan");
        resultSet= await dataShareHelper.query(dseUri, predicates2, ["*"]);
        console.info(TAG + "rowCount2 is " +resultSet.rowCount);
        expect(resultSet.rowCount).assertEqual(0);
        expect(resultSet.goToFirstRow()).assertFalse();
        console.info(TAG + "goToFirstRow fail");
        expect(resultSet.goToNextRow()).assertFalse();
        console.info(TAG + "goToNextRow fail");
        expect(resultSet.goToLastRow()).assertFalse();
        console.info(TAG + "goToLastRow fail");
        expect(resultSet.goToPreviousRow()).assertFalse();
        console.info(TAG + "goToPreviousRow fail");
        expect(resultSet.goTo(-1)).assertFalse();
        console.info(TAG + "goTo -1 fail");//向前1行
        expect(resultSet.goToRow(2)).assertFalse();
        console.info(TAG + "goToRow 0 fail" );//下标从0开始,跳转最后一行
        expect(resultSet.goToRow(1)).assertFalse();
        console.info(TAG + "goToRow 1 fail");//下标从0开始,跳转第二行
        resultSet.close();
      } catch (err: BusinessError) {
        console.error(TAG + "testDataShareResultKv0030 error. err.code:" + err.code + "err.message:" + err.message)
        expect().assertFail();
      }
      console.info(TAG + "testDataShareResultKv0030 end");
    })
 
    /**
     * @tc.name testDataShareResultKv0040
     * @tc.number SUB_DistributedData_DataShare_SDK_ResultSetKVTest_0040
     * @tc.desc ResultSet test
     * @tc.level: Level 2
     * @tc.type: Function
     * @tc.size: MediumTest
     */
    it('testDataShareResultKv0040', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL2, async () => {
      console.info(TAG + "testDataShareResultKv0040 start")
      let resultSet: DataShareResultSet | undefined = undefined;
      try {
        let nameStr = "zhangsan".repeat(26214); //小于1M
        let vbs: ValuesBucket = { "name": nameStr, "age": 99 as Long, "isStudent": true };
        if (dataShareHelper != undefined) {
          await dataShareHelper.insert(dseUri, vbs);
        }
      } catch (err: BusinessError) {
        console.error(TAG + "insert error. err.code:" + err.code + "err.message:" + err.message)
        expect().assertFail();
      }
      try {
        let predicates = new dataSharePredicates.DataSharePredicates();
        predicates.equalTo("age", "99");
        resultSet= await dataShareHelper.query(dseUri, predicates, ["*"]);
        expect(resultSet.rowCount).assertEqual(1);
        resultSet.close();
      } catch (err: BusinessError) {
        console.error(TAG + "insert error. err.code:" + err.code + "err.message:" + err.message)
        expect().assertFail();
      }
      console.info(TAG + "testDataShareResultKv0040 end");
    })
 
    /**
     * @tc.name testDataShareResultKv0050
     * @tc.number SUB_DistributedData_DataShare_SDK_ResultSetKVTest_0050
     * @tc.desc ResultSet test
     * @tc.level: Level 2
     * @tc.type: Function
     * @tc.size: MediumTest
     */
    // it('testDataShareResultKv0050', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL2, async () => {
    //   console.info(TAG + "testDataShareResultKv0050 start")
    //   let resultSet: DataShareResultSet | undefined = undefined;
    //   try {
    //     let nameStr = "zhangsan".repeat(262144); //约2M
    //     let vbs: ValuesBucket = { "name": nameStr, "age": 99 as Long, "isStudent": true };
    //     if (dataShareHelper != undefined) {
    //       await dataShareHelper.insert(dseUri, vbs);
    //       expect().assertFail();
    //     }
    //   } catch (err: BusinessError) {
    //     console.error(TAG + "insert error. err.code:" + err.code + "err.message:" + err.message)
    //     expect(err.code).assertEqual(15700000);
    //   }
    //   try {
    //     let predicates = new dataSharePredicates.DataSharePredicates();
    //     predicates.equalTo("age", "99");
    //     resultSet= await dataShareHelper.query(dseUri, predicates, ["*"]);
    //     expect(resultSet.rowCount).assertEqual(0);
    //     resultSet.close();
    //   } catch (err: BusinessError) {
    //     console.error(TAG + "query error. err.code:" + err.code + "err.message:" + err.message)
    //     expect().assertFail();
    //   }
    //   console.info(TAG + "testDataShareResultKv0050 end");
    // })
 
    /**
     * @tc.name testDataShareResultKv0060
     * @tc.number SUB_DistributedData_DataShare_SDK_ResultSetKVTest_0060
     * @tc.desc ResultSet test
     * @tc.level: Level 2
     * @tc.type: Function
     * @tc.size: MediumTest
     */
    // it('testDataShareResultKv0060', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL2, async () => {
    //   console.info(TAG + "testDataShareResultKv0060 start")
    //   let resultSet: DataShareResultSet | undefined = undefined;
    //   let nameStr = "zhangsan".repeat(25000);//约200K
    //   try {
    //     let vbs: ValuesBucket[] = []
    //      for(let i = 0;i < 20; i++){
    //        let vb: ValuesBucket = { "name": nameStr+i, "age": 88 as Long, "isStudent": true };
    //        vbs.push(vb);
    //      }
    //     if (dataShareHelper != undefined) {
    //       await dataShareHelper.batchInsert(dseUri, vbs);
    //     }
    //   } catch (err: BusinessError) {
    //     console.error(TAG + "batchInsert error. err.code:" + err.code + "err.message:" + err.message)
    //     expect().assertFail();
    //   }
    //   try {
    //     let predicates = new dataSharePredicates.DataSharePredicates();
    //     predicates.equalTo("age", "88");
    //     resultSet= await dataShareHelper.query(dseUri, predicates, ["*"]);
    //     expect(resultSet.rowCount).assertEqual(20);
    //     for(let i=0;i<resultSet.rowCount;i++){
    //       resultSet.goToRow(i);
    //       let valueStr:string = resultSet.getString(1);
    //       // console.info(TAG + "valueStr is: "+valueStr);
    //       expect(JSON.stringify(valueStr).includes(nameStr+i)).assertTrue();
    //     }
    //     resultSet.close();
    //   } catch (err: BusinessError) {
    //     console.error(TAG + "query error. err.code:" + err.code + "err.message:" + err.message)
    //     expect().assertFail();
    //   }
    //   console.info(TAG + "testDataShareResultKv0060 end");
    // })
  })
}