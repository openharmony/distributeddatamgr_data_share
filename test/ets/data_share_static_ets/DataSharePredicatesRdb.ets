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
import hilog from '@ohos.hilog'
import Utils from './Util.test';
import common from '@ohos.app.ability.common';
import dataShare from '@ohos.data.dataShare'
import dataSharePredicates from '@ohos.data.dataSharePredicates'
import * as pubfun from './DataSharePublicfunction_Promise'
import { BusinessError } from '@ohos.base';
import { ValuesBucket } from '@ohos.data.ValuesBucket';
import { AppStorage } from '@ohos.arkui.stateManagement';
import { GlobalThis } from '../../ets/entryability/EntryAbility';
 
let domain: int = 0x0000; //日志标识,0x0000作为测试框架的业务标识
let tag: string = 'testTag'; //日志标识字符串,作为tag标识当前runner类下的测试行为
let context: common.UIAbilityContext;
 
//CreateDataShareHelper
let uri = "datashare:///com.acts.datasharetest.DataShare";
 
//InsertValuesBucket
let BatchInsert: Array<ValuesBucket> = [
  { "name": "LiSi", "age": 18 as long },
  { "name": "WangWu", "age": 20 as long },
  { "name": "ZhaoLiu"}
];
let Insert: ValuesBucket = { "Name": "LiSi", "age": 18 as long };
 
//UpdateValuesBucket
let UpdateLiSi: ValuesBucket = { "name": "LiSii" };
let UpdateWangWu: ValuesBucket = { "name": "WangWuu" };
let UpdateZhaoLiu: ValuesBucket = { "name": "ZhaoLiuu" };
 
//Predicates
let EqualTo = new dataSharePredicates.DataSharePredicates();
let NotEqualTo = new dataSharePredicates.DataSharePredicates();
let GreaterThan = new dataSharePredicates.DataSharePredicates();
let LessThan = new dataSharePredicates.DataSharePredicates();
let GreaterThanOrEqualTo = new dataSharePredicates.DataSharePredicates();
let LessThanOrEqualTo = new dataSharePredicates.DataSharePredicates();
let IsNull = new dataSharePredicates.DataSharePredicates();
let IsNotNull = new dataSharePredicates.DataSharePredicates();
let InValues = new dataSharePredicates.DataSharePredicates();
let NotInValues = new dataSharePredicates.DataSharePredicates();
let Like = new dataSharePredicates.DataSharePredicates();
let And = new dataSharePredicates.DataSharePredicates();
let Or = new dataSharePredicates.DataSharePredicates();
let OrderByAsc = new dataSharePredicates.DataSharePredicates();
let OrderByDesc = new dataSharePredicates.DataSharePredicates();
let Wrap = new dataSharePredicates.DataSharePredicates();
let Contains = new dataSharePredicates.DataSharePredicates();
// let BeginsWith = new dataSharePredicates.DataSharePredicates();
// BeginsWith.beginsWith("name", "Li");
// let EndsWith = new dataSharePredicates.DataSharePredicates();
// EndsWith.endsWith("name", "i");
// let Glob = new dataSharePredicates.DataSharePredicates();
// Glob.glob("name", "*i");
let Between = new dataSharePredicates.DataSharePredicates();
// let NotBetween = new dataSharePredicates.DataSharePredicates();
// NotBetween.notBetween("age", 19, 20);
// let Distinct = new dataSharePredicates.DataSharePredicates();
// Distinct.equalTo("age", 18).distinct();
let GroupBy = new dataSharePredicates.DataSharePredicates();
let IndexedBy = new dataSharePredicates.DataSharePredicates();
let PredicatesAll = new dataSharePredicates.DataSharePredicates();
 
//Return Expect
const DataProcessResultOne = 1;
const DataProcessResultTwo = 2;
const RetLiSii = "LiSii";
const RetLiSi = "LiSi";
const RetWangWuu = "WangWuu";
const RetWangWu = "WangWu";
const RetZhaoLiuu = "ZhaoLiuu";
const RetZhaoLiu = "ZhaoLiu";
 
const Name = "name";
 
let Column: int;
 
export default function DataSharePredicatesRdb() {
  describe('DataSharePredicatesRdb', (): void => {
    const globalThis: GlobalThis = GlobalThis.getInstance();
    beforeAll(async () => {
      EqualTo.equalTo("age", 18 as long);
      NotEqualTo.notEqualTo("age", 18 as long);
      GreaterThan.greaterThan("age", 18 as long);
      LessThan.lessThan("age", 20 as long);
      GreaterThanOrEqualTo.greaterThanOrEqualTo("age", 20 as long);
      LessThanOrEqualTo.lessThanOrEqualTo("age", 18 as long);
      IsNull.isNull("age");
      IsNotNull.isNotNull("age");
      InValues.inValues("age", [18 as long]);
      NotInValues.notInValues("age", [20 as long]);
      Like.like("name", "%iS%");
      And.contains("name", "Li").and().equalTo("age", 18 as long);
      Or.contains("name", "Li").or().equalTo("age", 18 as long);
      OrderByAsc.orderByAsc("age");
      OrderByDesc.orderByDesc("age");
      Wrap.contains("name", "Li").beginWrap().equalTo("age", 18 as long).endWrap();
      Contains.contains("name", "LiSi");
      Between.between("age", 18 as long, 19 as long)
      GroupBy.groupBy(["name"]);
      try {
        context = AppStorage.get<common.UIAbilityContext>("context") as common.UIAbilityContext;
        await globalThis.connectDataShareExtAbility(context);
        await Utils.msSleep(2000);
        hilog.info(domain, tag, '%{public}s', `[ttt] <<Consumer>> beforeAll helper: ` + JSON.stringify(globalThis.helper!));
      } catch (err: BusinessError) {
        hilog.error(domain, tag, '%{public}s', `[ttt] <<Consumer>> beforeAll error: code: ${err.code}, message: ${err.message}`);
      }
    })
 
    beforeEach(async () => {
      await pubfun.publicbatchInsert(globalThis.helper!, uri, BatchInsert).then((data) => {
        console.info("TestDataShare going batchInsert = " + data);
      })
    })
    afterEach(async () => {
      await pubfun.publicdelete(globalThis.helper!, uri, PredicatesAll).then((data) => {
        console.info("TestDataShare going deleteall = " + data);
      })
    })
 
    /**
     * @tc.number: SUB_Ability_Ability_Static_0100
     * @tc.name: SUB_Ability_Ability_Static_0100
     * @tc.desc:  Check start ability by bundl
     * @tc.level: LEVEL1
     * @tc.size: MEDIUMTEST
     * @tc.type: FUNCTION
     */
    it("SUB_Ability_Ability_Static_0100", TestType.FUNCTION|Size.MEDIUMTEST|Level.LEVEL1, async (done: () => void): Promise<void> => {
      await Utils.msSleep(1000);
      hilog.info(domain, tag, '%{public}s', 'abilityTest001 start');
      let a:int = 1;
      let b:int = 1;
      expect(a).assertEqual(b);
      done()
      hilog.info(domain, tag, '%{public}s', 'abilityTest001 end');
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_6100
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : DataShare Supports Predicates
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    it('SUB_DDM_DataSharePredicatesRdb0100', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
      try {
        await pubfun.publicupdate(globalThis.helper!, uri, EqualTo, UpdateLiSi).then((data) => {
          console.info("TestDataShare going update = " + data);
          expect(DataProcessResultOne).assertEqual(data);
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataShareResultSetRdb0100 Update err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicquery(globalThis.helper!, uri, EqualTo, ["*"]).then((data) => {
          console.info("TestDataShare going query = " + data);
          globalThis.ResultSet!.goToFirstRow();
          Column = globalThis.ResultSet!.getColumnIndex(Name);
          expect(RetLiSii).assertEqual(globalThis.ResultSet!.getString(Column));
          globalThis.ResultSet!.close();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataShareResultSetRdb0100 query err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicdelete(globalThis.helper!, uri, EqualTo).then((data) => {
          console.info("TestDataShare going delete = " + data);
          expect(DataProcessResultOne).assertEqual(data);
          done();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataShareResultSetRdb0100 delete err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
          done();
        })
      } catch (err) {
        console.info("SUB_DDM_DataSharePredicatesRdb0100 fail" + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_6200
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : DataShare Supports Predicates
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    it('SUB_DDM_DataSharePredicatesRdb0200', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
      try {
        await pubfun.publicupdate(globalThis.helper!, uri, NotEqualTo, UpdateWangWu).then((data) => {
          console.info("TestDataShare going update = " + data);
          expect(DataProcessResultOne).assertEqual(data);
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataSharePredicatesRdb0200 Update err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicquery(globalThis.helper!, uri, NotEqualTo, ["*"]).then((data) => {
          console.info("TestDataShare going query = " + data);
          globalThis.ResultSet!.goToFirstRow();
          Column = globalThis.ResultSet!.getColumnIndex(Name);
          expect(RetWangWuu).assertEqual(globalThis.ResultSet!.getString(Column));
          globalThis.ResultSet!.close();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataSharePredicatesRdb0200 query err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicdelete(globalThis.helper!, uri, NotEqualTo).then((data) => {
          console.info("TestDataShare going delete = " + data);
          expect(DataProcessResultOne).assertEqual(data);
          done();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataSharePredicatesRdb0200 delete err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
          done();
        })
      } catch (err) {
        console.info("SUB_DDM_DataSharePredicatesRdb0200 fail" + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_6300
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : DataShare Supports Predicates
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    it('SUB_DDM_DataSharePredicatesRdb0300', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
      try {
        await pubfun.publicupdate(globalThis.helper!, uri, GreaterThan, UpdateWangWu).then((data) => {
          console.info("TestDataShare going update = " + data);
          expect(DataProcessResultOne).assertEqual(data);
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataSharePredicatesRdb0300 Update err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicquery(globalThis.helper!, uri, GreaterThan, ["*"]).then((data) => {
          console.info("TestDataShare going query = " + data);
          globalThis.ResultSet!.goToFirstRow();
          Column = globalThis.ResultSet!.getColumnIndex(Name);
          expect(RetWangWuu).assertEqual(globalThis.ResultSet!.getString(Column));
          globalThis.ResultSet!.close();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataSharePredicatesRdb0300 query err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicdelete(globalThis.helper!, uri, GreaterThan).then((data) => {
          console.info("TestDataShare going delete = " + data);
          expect(DataProcessResultOne).assertEqual(data);
          done();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataSharePredicatesRdb0300 delete err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
          done();
        })
      } catch (err) {
        console.info("SUB_DDM_DataSharePredicatesRdb0300 fail" + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })
     
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_6400
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : DataShare Supports Predicates
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    it('SUB_DDM_DataSharePredicatesRdb0400', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
      try {
        await pubfun.publicupdate(globalThis.helper!, uri, LessThan, UpdateLiSi).then((data) => {
          console.info("TestDataShare going update = " + data);
          expect(DataProcessResultOne).assertEqual(data);
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataSharePredicatesRdb0400 Update err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicquery(globalThis.helper!, uri, LessThan, ["*"]).then((data) => {
          console.info("TestDataShare going query = " + data);
          globalThis.ResultSet!.goToFirstRow();
          Column = globalThis.ResultSet!.getColumnIndex(Name);
          expect(RetLiSii).assertEqual(globalThis.ResultSet!.getString(Column));
          globalThis.ResultSet!.close();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataSharePredicatesRdb0400 query err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicdelete(globalThis.helper!, uri, LessThan).then((data) => {
          console.info("TestDataShare going delete = " + data);
          expect(DataProcessResultOne).assertEqual(data);
          done();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataSharePredicatesRdb0400 delete err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
          done();
        })
      } catch (err) {
        console.info("SUB_DDM_DataSharePredicatesRdb0400 fail" + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_6500
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : DataShare Supports Predicates
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    it('SUB_DDM_DataSharePredicatesRdb0500', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
      try {
        await pubfun.publicupdate(globalThis.helper!, uri, GreaterThanOrEqualTo, UpdateWangWu).then((data) => {
          console.info("TestDataShare going update = " + data);
          expect(DataProcessResultOne).assertEqual(data);
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataSharePredicatesRdb0500 Update err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicquery(globalThis.helper!, uri, GreaterThanOrEqualTo, ["*"]).then((data) => {
          console.info("TestDataShare going query = " + data);
          globalThis.ResultSet!.goToFirstRow();
          Column = globalThis.ResultSet!.getColumnIndex(Name);
          expect(RetWangWuu).assertEqual(globalThis.ResultSet!.getString(Column));
          globalThis.ResultSet!.close();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataSharePredicatesRdb0500 query err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicdelete(globalThis.helper!, uri, GreaterThanOrEqualTo).then((data) => {
          console.info("TestDataShare going delete = " + data);
          expect(DataProcessResultOne).assertEqual(data);
          done();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataSharePredicatesRdb0500 delete err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
          done();
        })
      } catch (err) {
        console.info("SUB_DDM_DataSharePredicatesRdb0500 fail" + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_6600
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : DataShare Supports Predicates
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    it('SUB_DDM_DataSharePredicatesRdb0600', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
      try {
        await pubfun.publicupdate(globalThis.helper!, uri, LessThanOrEqualTo, UpdateLiSi).then((data) => {
          console.info("TestDataShare going update = " + data);
          expect(DataProcessResultOne).assertEqual(data);
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataSharePredicatesRdb0600 Update err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicquery(globalThis.helper!, uri, LessThanOrEqualTo, ["*"]).then((data) => {
          console.info("TestDataShare going query = " + data);
          globalThis.ResultSet!.goToFirstRow();
          Column = globalThis.ResultSet!.getColumnIndex(Name);
          expect(RetLiSii).assertEqual(globalThis.ResultSet!.getString(Column));
          globalThis.ResultSet!.close();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataSharePredicatesRdb0600 query err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicdelete(globalThis.helper!, uri, LessThanOrEqualTo).then((data) => {
          console.info("TestDataShare going delete = " + data);
          expect(DataProcessResultOne).assertEqual(data);
          done();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataSharePredicatesRdb0600 delete err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
          done();
        })
      } catch (err) {
        console.info("SUB_DDM_DataSharePredicatesRdb0600 fail" + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_6700
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : DataShare Supports Predicates
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    it('SUB_DDM_DataSharePredicatesRdb0700', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
      try {
        await pubfun.publicupdate(globalThis.helper!, uri, IsNull, UpdateZhaoLiu).then((data) => {
          console.info("TestDataShare going update = " + data);
          expect(DataProcessResultOne).assertEqual(data);
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataSharePredicatesRdb0700 Update err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicquery(globalThis.helper!, uri, IsNull, ["*"]).then((data) => {
          console.info("TestDataShare going query = " + data);
          globalThis.ResultSet!.goToLastRow();
          Column = globalThis.ResultSet!.getColumnIndex(Name);
          expect(RetZhaoLiuu).assertEqual(globalThis.ResultSet!.getString(Column));
          globalThis.ResultSet!.close();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataSharePredicatesRdb0700 query err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicdelete(globalThis.helper!, uri, IsNull).then((data) => {
          console.info("TestDataShare going delete = " + data);
          expect(DataProcessResultOne).assertEqual(data);
          done();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataSharePredicatesRdb0700 delete err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
          done();
        })
      } catch (err) {
        console.info("SUB_DDM_DataSharePredicatesRdb0700 fail" + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_6800
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : DataShare Supports Predicates
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    it('SUB_DDM_DataSharePredicatesRdb0800', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
      try {
        await pubfun.publicupdate(globalThis.helper!, uri, IsNotNull, UpdateLiSi).then((data) => {
          console.info("TestDataShare going update = " + data);
          expect(DataProcessResultTwo).assertEqual(data);
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataSharePredicatesRdb0800 Update err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicquery(globalThis.helper!, uri, IsNotNull, ["*"]).then((data) => {
          console.info("TestDataShare going query = " + data);
          globalThis.ResultSet!.goToFirstRow();
          Column = globalThis.ResultSet!.getColumnIndex(Name);
          expect(RetLiSii).assertEqual(globalThis.ResultSet!.getString(Column));
          globalThis.ResultSet!.close();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataSharePredicatesRdb0800 query err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicdelete(globalThis.helper!, uri, IsNotNull).then((data) => {
          console.info("TestDataShare going delete = " + data);
          expect(DataProcessResultTwo).assertEqual(data);
          done();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataSharePredicatesRdb0800 delete err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
          done();
        })
      } catch (err) {
        console.info("SUB_DDM_DataSharePredicatesRdb0800 fail" + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_6900
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : DataShare Supports Predicates
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    it('SUB_DDM_DataSharePredicatesRdb0900', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
      try {
        await pubfun.publicupdate(globalThis.helper!, uri, InValues, UpdateLiSi).then((data) => {
          console.info("TestDataShare going update = " + data);
          expect(DataProcessResultOne).assertEqual(data);
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataSharePredicatesRdb0900 Update err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicquery(globalThis.helper!, uri, InValues, ["*"]).then((data) => {
          console.info("TestDataShare going query = " + data);
          globalThis.ResultSet!.goToFirstRow();
          Column = globalThis.ResultSet!.getColumnIndex(Name);
          expect(RetLiSii).assertEqual(globalThis.ResultSet!.getString(Column));
          globalThis.ResultSet!.close();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataSharePredicatesRdb0900 query err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicdelete(globalThis.helper!, uri, InValues).then((data) => {
          console.info("TestDataShare going delete = " + data);
          expect(DataProcessResultOne).assertEqual(data);
          done();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataSharePredicatesRdb0900 delete err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
          done();
        })
      } catch (err) {
        console.info("SUB_DDM_DataSharePredicatesRdb0900 fail" + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_7000
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : DataShare Supports Predicates
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    it('SUB_DDM_DataSharePredicatesRdb1000', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
      try {
        await pubfun.publicupdate(globalThis.helper!, uri, NotInValues, UpdateLiSi).then((data) => {
          console.info("TestDataShare going update = " + data);
          expect(DataProcessResultOne).assertEqual(data);
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataSharePredicatesRdb1000 Update err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicquery(globalThis.helper!, uri, NotInValues, ["*"]).then((data) => {
          console.info("TestDataShare going query = " + data);
          globalThis.ResultSet!.goToFirstRow();
          Column = globalThis.ResultSet!.getColumnIndex(Name);
          expect(RetLiSii).assertEqual(globalThis.ResultSet!.getString(Column));
          globalThis.ResultSet!.close();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataSharePredicatesRdb1000 query err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicdelete(globalThis.helper!, uri, NotInValues).then((data) => {
          console.info("TestDataShare going delete = " + data);
          expect(DataProcessResultOne).assertEqual(data);
          done();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataSharePredicatesRdb1000 delete err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
          done();
        })
      } catch (err) {
        console.info("SUB_DDM_DataSharePredicatesRdb1000 fail" + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_7100
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : DataShare Supports Predicates
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    it('SUB_DDM_DataSharePredicatesRdb1100', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
      try {
        await pubfun.publicupdate(globalThis.helper!, uri, Like, UpdateLiSi).then((data) => {
          console.info("TestDataShare going update = " + data);
          expect(DataProcessResultOne).assertEqual(data);
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataSharePredicatesRdb1100 Update err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicquery(globalThis.helper!, uri, Like, ["*"]).then((data) => {
          console.info("TestDataShare going query = " + data);
          globalThis.ResultSet!.goToFirstRow();
          Column = globalThis.ResultSet!.getColumnIndex(Name);
          expect(RetLiSii).assertEqual(globalThis.ResultSet!.getString(Column));
          globalThis.ResultSet!.close();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataSharePredicatesRdb1100 query err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicdelete(globalThis.helper!, uri, Like).then((data) => {
          console.info("TestDataShare going delete = " + data);
          expect(DataProcessResultOne).assertEqual(data);
          done();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataSharePredicatesRdb1100 delete err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
          done();
        })
      } catch (err) {
        console.info("SUB_DDM_DataSharePredicatesRdb1100 fail" + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })

        /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_7200
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : DataShare Supports Predicates
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    it('SUB_DDM_DataSharePredicatesRdb1200', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
      try {
        await pubfun.publicupdate(globalThis.helper!, uri, And, UpdateLiSi).then((data) => {
          console.info("TestDataShare going update = " + data);
          expect(DataProcessResultOne).assertEqual(data);
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataSharePredicatesRdb1200 Update err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicquery(globalThis.helper!, uri, And, ["*"]).then((data) => {
          console.info("TestDataShare going query = " + data);
          globalThis.ResultSet!.goToFirstRow();
          Column = globalThis.ResultSet!.getColumnIndex(Name);
          expect(RetLiSii).assertEqual(globalThis.ResultSet!.getString(Column));
          globalThis.ResultSet!.close();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataSharePredicatesRdb1200 query err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicdelete(globalThis.helper!, uri, And).then((data) => {
          console.info("TestDataShare going delete = " + data);
          expect(DataProcessResultOne).assertEqual(data);
          done();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataSharePredicatesRdb1200 delete err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
          done();
        })
      } catch (err) {
        console.info("SUB_DDM_DataSharePredicatesRdb1200 fail" + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_7300
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : DataShare Supports Predicates
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    it('SUB_DDM_DataSharePredicatesRdb1300', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
      try {
        await pubfun.publicupdate(globalThis.helper!, uri, Or, UpdateLiSi).then((data) => {
          console.info("TestDataShare going update = " + data);
          expect(DataProcessResultTwo).assertEqual(data);
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataSharePredicatesRdb1300 Update err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicquery(globalThis.helper!, uri, Or, ["*"]).then((data) => {
          console.info("TestDataShare going query = " + data);
          globalThis.ResultSet!.goToFirstRow();
          hilog.error(domain, tag, '%{public}s', `globalThis.ResultSet: ` + JSON.stringify(globalThis.ResultSet!))
          Column = globalThis.ResultSet!.getColumnIndex(Name);
          expect(RetLiSii).assertEqual(globalThis.ResultSet!.getString(Column));
          globalThis.ResultSet!.close();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataSharePredicatesRdb1300 query err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicdelete(globalThis.helper!, uri, Or).then((data) => {
          console.info("TestDataShare going delete = " + data);
          expect(DataProcessResultTwo).assertEqual(data);
          done();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataSharePredicatesRdb1300 delete err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
          done();
        })
      } catch (err) {
        console.info("SUB_DDM_DataSharePredicatesRdb1300 fail" + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_7400
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : DataShare Supports Predicates
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    it('SUB_DDM_DataSharePredicatesRdb1400', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
      try {
        await pubfun.publicquery(globalThis.helper!, uri, OrderByAsc, ["*"]).then((data) => {
          console.info("TestDataShare going query = " + data);
          globalThis.ResultSet!.goToFirstRow();
          Column = globalThis.ResultSet!.getColumnIndex(Name);
          expect(RetZhaoLiu).assertEqual(globalThis.ResultSet!.getString(Column));
          globalThis.ResultSet!.close();
          done();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataSharePredicatesRdb1400 query err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
          done();
        })
      } catch (err) {
        console.info("SUB_DDM_DataSharePredicatesRdb1400 fail" + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_7500
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : DataShare Supports Predicates
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    it('SUB_DDM_DataSharePredicatesRdb1500', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
      try {
        await pubfun.publicquery(globalThis.helper!, uri, OrderByDesc, ["*"]).then((data) => {
          console.info("TestDataShare going query = " + data);
          globalThis.ResultSet!.goToFirstRow();
          Column = globalThis.ResultSet!.getColumnIndex(Name);
          expect(RetWangWu).assertEqual(globalThis.ResultSet!.getString(Column));
          globalThis.ResultSet!.close();
          done();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataSharePredicatesRdb1500 query err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
          done();
        })
      } catch (err) {
        console.info("SUB_DDM_DataSharePredicatesRdb1500 fail" + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_7600
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : DataShare Supports Predicates
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    it('SUB_DDM_DataSharePredicatesRdb1600', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
      try {
        await pubfun.publicquery(globalThis.helper!, uri, Wrap, ["*"]).then((data) => {
          console.info("TestDataShare going query = " + data);
          globalThis.ResultSet!.goToFirstRow();
          Column = globalThis.ResultSet!.getColumnIndex(Name);
          expect(RetLiSi).assertEqual(globalThis.ResultSet!.getString(Column));
          globalThis.ResultSet!.close();
          done();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataSharePredicatesRdb1600 query err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
          done();
        })
      } catch (err) {
        console.info("SUB_DDM_DataSharePredicatesRdb1600 fail" + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_7700
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : DataShare Supports Predicates
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    it('SUB_DDM_DataSharePredicatesRdb1700', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
      try {
        await pubfun.publicupdate(globalThis.helper!, uri, Contains, UpdateLiSi).then((data) => {
          console.info("TestDataShare going update = " + data);
          expect(DataProcessResultOne).assertEqual(data);
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataSharePredicatesRdb1700 Update err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicquery(globalThis.helper!, uri, Contains, ["*"]).then((data) => {
          console.info("TestDataShare going query = " + data);
          globalThis.ResultSet!.goToFirstRow();
          Column = globalThis.ResultSet!.getColumnIndex(Name);
          expect(RetLiSii).assertEqual(globalThis.ResultSet!.getString(Column));
          globalThis.ResultSet!.close();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataSharePredicatesRdb1700 query err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicdelete(globalThis.helper!, uri, Contains).then((data) => {
          console.info("TestDataShare going delete = " + data);
          expect(DataProcessResultOne).assertEqual(data);
          done();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataSharePredicatesRdb1700 delete err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
          done();
        })
      } catch (err) {
        console.info("SUB_DDM_DataSharePredicatesRdb1700 fail" + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_7800
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : DataShare Supports Predicates
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    // it('SUB_DDM_DataSharePredicatesRdb1800', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
    //   try {
    //     await pubfun.publicupdate(globalThis.helper!, uri, BeginsWith, UpdateLiSi).then((data) => {
    //       console.info("TestDataShare going update = " + data);
    //       expect(DataProcessResultOne).assertEqual(data);
    //     }).catch((error) => {
    //       let err = error as BusinessError;
    //       console.error(`SUB_DDM_DataSharePredicatesRdb1800 Update err: code: ${err.code}, message: ${err.message}`);
    //       // console.info("SUB_DDM_DataSharePredicatesRdb1800 Update err" + JSON.stringify(err));
    //       expect(err).assertFail();
    //     })
    //     await pubfun.publicquery(globalThis.helper!, uri, BeginsWith, ["*"]).then((data) => {
    //       console.info("TestDataShare going query = " + data);
    //       globalThis.ResultSet!.goToFirstRow();
    //       Column = globalThis.ResultSet!.getColumnIndex(Name);
    //       expect(RetLiSii).assertEqual(globalThis.ResultSet!.getString(Column));
    //       globalThis.ResultSet!.close();
    //     }).catch((error) => {
    //       let err = error as BusinessError;
    //       console.error(`SUB_DDM_DataSharePredicatesRdb1800 query err: code: ${err.code}, message: ${err.message}`);
    //       // console.info("SUB_DDM_DataSharePredicatesRdb1800 query err" + JSON.stringify(err));
    //       expect(err).assertFail();
    //     })
    //     await pubfun.publicdelete(globalThis.helper!, uri, BeginsWith).then((data) => {
    //       console.info("TestDataShare going delete = " + data);
    //       expect(DataProcessResultOne).assertEqual(data);
    //       done();
    //     }).catch((error) => {
    //       let err = error as BusinessError;
    //       console.error(`SUB_DDM_DataSharePredicatesRdb1800 delete err: code: ${err.code}, message: ${err.message}`);
    //       // console.info("SUB_DDM_DataSharePredicatesRdb1800 delete err" + JSON.stringify(err));
    //       expect(err).assertFail();
    //       done();
    //     })
    //   } catch (err) {
    //     console.info("SUB_DDM_DataSharePredicatesRdb1800 fail" + JSON.stringify(err));
    //     expect(err).assertFail();
    //     done();
    //   }
    // })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_7900
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : DataShare Supports Predicates
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    // it('SUB_DDM_DataSharePredicatesRdb1900', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
    //   try {
    //     await pubfun.publicupdate(globalThis.helper!, uri, EndsWith, UpdateLiSi).then((data) => {
    //       console.info("TestDataShare going update = " + data);
    //       expect(DataProcessResultOne).assertEqual(data);
    //     }).catch((error) => {
    //       let err = error as BusinessError;
    //       console.error(`SUB_DDM_DataSharePredicatesRdb1900 Update err: code: ${err.code}, message: ${err.message}`);
    //       // console.info("SUB_DDM_DataSharePredicatesRdb1900 Update err" + JSON.stringify(err));
    //       expect(err).assertFail();
    //     })
    //     await pubfun.publicquery(globalThis.helper!, uri, EndsWith, ["*"]).then((data) => {
    //       console.info("TestDataShare going query = " + data);
    //       globalThis.ResultSet!.goToFirstRow();
    //       Column = globalThis.ResultSet!.getColumnIndex(Name);
    //       expect(RetLiSii).assertEqual(globalThis.ResultSet!.getString(Column));
    //       globalThis.ResultSet!.close();
    //     }).catch((error) => {
    //       let err = error as BusinessError;
    //       console.error(`SUB_DDM_DataSharePredicatesRdb1900 query err: code: ${err.code}, message: ${err.message}`);
    //       // console.info("SUB_DDM_DataSharePredicatesRdb1900 query err" + JSON.stringify(err));
    //       expect(err).assertFail();
    //     })
    //     await pubfun.publicdelete(globalThis.helper!, uri, EndsWith).then((data) => {
    //       console.info("TestDataShare going delete = " + data);
    //       expect(DataProcessResultOne).assertEqual(data);
    //       done();
    //     }).catch((error) => {
    //       let err = error as BusinessError;
    //       console.error(`SUB_DDM_DataSharePredicatesRdb1900 delete err: code: ${err.code}, message: ${err.message}`);
    //       // console.info("SUB_DDM_DataSharePredicatesRdb1900 delete err" + JSON.stringify(err));
    //       expect(err).assertFail();
    //       done();
    //     })
    //   } catch (err) {
    //     console.info("SUB_DDM_DataSharePredicatesRdb1900 fail" + JSON.stringify(err));
    //     expect(err).assertFail();
    //     done();
    //   }
    // })
 /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_8000
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : DataShare Supports Predicates
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    // it('SUB_DDM_DataSharePredicatesRdb2000', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
    //   try {
    //     await pubfun.publicupdate(globalThis.helper!, uri, Glob, UpdateLiSi).then((data) => {
    //       console.info("TestDataShare going update = " + data);
    //       expect(DataProcessResultOne).assertEqual(data);
    //     }).catch((error) => {
    //       let err = error as BusinessError;
    //       console.error(`SUB_DDM_DataSharePredicatesRdb2000 Update err: code: ${err.code}, message: ${err.message}`);
    //       // console.info("SUB_DDM_DataSharePredicatesRdb2000 Update err" + JSON.stringify(err));
    //       expect(err).assertFail();
    //     })
    //     await pubfun.publicquery(globalThis.helper!, uri, Glob, ["*"]).then((data) => {
    //       console.info("TestDataShare going query = " + data);
    //       globalThis.ResultSet!.goToFirstRow();
    //       Column = globalThis.ResultSet!.getColumnIndex(Name);
    //       expect(RetLiSii).assertEqual(globalThis.ResultSet!.getString(Column));
    //       globalThis.ResultSet!.close();
    //     }).catch((error) => {
    //       let err = error as BusinessError;
    //       console.error(`SUB_DDM_DataSharePredicatesRdb2000 query err: code: ${err.code}, message: ${err.message}`);
    //       // console.info("SUB_DDM_DataSharePredicatesRdb2000 query err" + JSON.stringify(err));
    //       expect(err).assertFail();
    //     })
    //     await pubfun.publicdelete(globalThis.helper!, uri, Glob).then((data) => {
    //       console.info("TestDataShare going delete = " + data);
    //       expect(DataProcessResultOne).assertEqual(data);
    //       done();
    //     }).catch((error) => {
    //       let err = error as BusinessError;
    //       console.error(`SUB_DDM_DataSharePredicatesRdb2000 delete err: code: ${err.code}, message: ${err.message}`);
    //       // console.info("SUB_DDM_DataSharePredicatesRdb2000 delete err" + JSON.stringify(err));
    //       expect(err).assertFail();
    //       done();
    //     })
    //   } catch (err) {
    //     console.info("SUB_DDM_DataSharePredicatesRdb2000 fail" + JSON.stringify(err));
    //     expect(err).assertFail();
    //     done();
    //   }
    // })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_8100
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : DataShare Supports Predicates
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    it('SUB_DDM_DataSharePredicatesRdb2100', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
      try {
        await pubfun.publicupdate(globalThis.helper!, uri, Between, UpdateLiSi).then((data) => {
          console.info("TestDataShare going update = " + data);
          expect(DataProcessResultOne).assertEqual(data);
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataSharePredicatesRdb2100 Update err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicquery(globalThis.helper!, uri, Between, ["*"]).then((data) => {
          console.info("TestDataShare going query = " + data);
          globalThis.ResultSet!.goToFirstRow();
          Column = globalThis.ResultSet!.getColumnIndex(Name);
          expect(RetLiSii).assertEqual(globalThis.ResultSet!.getString(Column));
          globalThis.ResultSet!.close();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataSharePredicatesRdb2100 query err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicdelete(globalThis.helper!, uri, Between).then((data) => {
          console.info("TestDataShare going delete = " + data);
          expect(DataProcessResultOne).assertEqual(data);
          done();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataSharePredicatesRdb2100 delete err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
          done();
        })
      } catch (err) {
        console.info("SUB_DDM_DataSharePredicatesRdb2100 fail" + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_8200
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : DataShare Supports Predicates
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    // it('SUB_DDM_DataSharePredicatesRdb2200', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
    //   try {
    //     await pubfun.publicupdate(globalThis.helper!, uri, NotBetween, UpdateLiSi).then((data) => {
    //       console.info("TestDataShare going update = " + data);
    //       expect(DataProcessResultOne).assertEqual(data);
    //     }).catch((error) => {
    //       let err = error as BusinessError;
    //       console.error(`SUB_DDM_DataSharePredicatesRdb2200 Update err: code: ${err.code}, message: ${err.message}`);
    //       // console.info("SUB_DDM_DataSharePredicatesRdb2200 Update err" + JSON.stringify(err));
    //       expect(err).assertFail();
    //     })
    //     await pubfun.publicquery(globalThis.helper!, uri, NotBetween, ["*"]).then((data) => {
    //       console.info("TestDataShare going query = " + data);
    //       globalThis.ResultSet!.goToFirstRow();
    //       Column = globalThis.ResultSet!.getColumnIndex(Name);
    //       expect(RetLiSii).assertEqual(globalThis.ResultSet!.getString(Column));
    //       globalThis.ResultSet!.close();
    //     }).catch((error) => {
    //       let err = error as BusinessError;
    //       console.error(`SUB_DDM_DataSharePredicatesRdb2200 query err: code: ${err.code}, message: ${err.message}`);
    //       // console.info("SUB_DDM_DataSharePredicatesRdb2200 query err" + JSON.stringify(err));
    //       expect(err).assertFail();
    //     })
    //     await pubfun.publicdelete(globalThis.helper!, uri, NotBetween).then((data) => {
    //       console.info("TestDataShare going delete = " + data);
    //       expect(DataProcessResultOne).assertEqual(data);
    //       done();
    //     }).catch((error) => {
    //       let err = error as BusinessError;
    //       console.error(`SUB_DDM_DataSharePredicatesRdb2200 delete err: code: ${err.code}, message: ${err.message}`);
    //       // console.info("SUB_DDM_DataSharePredicatesRdb2200 delete err" + JSON.stringify(err));
    //       expect(err).assertFail();
    //       done();
    //     })
    //   } catch (err) {
    //     console.info("SUB_DDM_DataSharePredicatesRdb2200 fail" + JSON.stringify(err));
    //     expect(err).assertFail();
    //     done();
    //   }
    // })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_8300
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : DataShare Supports Predicates
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    // it('SUB_DDM_DataSharePredicatesRdb2300', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
    //   try {
    //     await pubfun.publicquery(globalThis.helper!, uri, Distinct, ["*"]).then((data) => {
    //       console.info("TestDataShare going query = " + data);
    //       globalThis.ResultSet!.goToFirstRow();
    //       Column = globalThis.ResultSet!.getColumnIndex(Name);
    //       expect(RetLiSi).assertEqual(globalThis.ResultSet!.getString(Column));
    //       globalThis.ResultSet!.close();
    //       done();
    //     }).catch((error) => {
    //       let err = error as BusinessError;
    //       console.error(`SUB_DDM_DataSharePredicatesRdb2300 query err: code: ${err.code}, message: ${err.message}`);
    //       // console.info("SUB_DDM_DataSharePredicatesRdb2300 query err" + JSON.stringify(err));
    //       expect(err).assertFail();
    //       done();
    //     })
    //   } catch (err) {
    //     console.info("SUB_DDM_DataSharePredicatesRdb2300 fail" + JSON.stringify(err));
    //     expect(err).assertFail();
    //     done();
    //   }
    // })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_8400
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : DataShare Supports Predicates
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    it('SUB_DDM_DataSharePredicatesRdb2400', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
      try {
        pubfun.publicinsert(globalThis.helper!, uri, Insert).then((data) => {
          console.info("TestDataShare going batchInsert = " + data);
        })
        await pubfun.publicquery(globalThis.helper!, uri, GroupBy, ["*"]).then((data) => {
          console.info("TestDataShare going query = " + data);
          globalThis.ResultSet!.goToFirstRow();
          Column = globalThis.ResultSet!.getColumnIndex(Name);
          expect(RetLiSi).assertEqual(globalThis.ResultSet!.getString(Column));
          globalThis.ResultSet!.close();
          done();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataSharePredicatesRdb2400 query err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
          done();
        })
      } catch (err) {
        console.info("SUB_DDM_DataSharePredicatesRdb2400 fail" + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_8500
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : DataShare Supports Predicates
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    // it('SUB_DDM_DataSharePredicatesRdb2500', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
    //   try {
    //     await pubfun.publicinsert(globalThis.helper!, uri, Insert).then((data) => {
    //       console.info("TestDataShare going batchInsert = " + data);
    //       IndexedBy.indexedBy(globalThis.insertdata);
    //     })
    //     await pubfun.publicquery(globalThis.helper!, uri, IndexedBy, ["*"]).then((data) => {
    //       console.info("TestDataShare going query = " + data);
    //       globalThis.ResultSet!.goToLastRow();
    //       Column = globalThis.ResultSet!.getColumnIndex(Name);
    //       expect(RetLiSi).assertEqual(globalThis.ResultSet!.getString(Column));
    //       globalThis.ResultSet!.close();
    //       done();
    //     }).catch((error) => {
    //       let err = error as BusinessError;
    //       console.error(`SUB_DDM_DataSharePredicatesRdb2500 query err: code: ${err.code}, message: ${err.message}`);
    //       // console.info("SUB_DDM_DataSharePredicatesRdb2500 query err" + JSON.stringify(err));
    //       expect(err).assertFail();
    //       done();
    //     })
    //   } catch (err) {
    //     console.info("SUB_DDM_DataSharePredicatesRdb2500 fail" + JSON.stringify(err));
    //     expect(err).assertFail();
    //     done();
    //   }
    // })
  })
}