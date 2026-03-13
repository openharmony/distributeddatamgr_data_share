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
import * as pubfun from './DataSharePublicfunction_Promise'
import dataShare from '@ohos.data.dataShare';
import dataSharePredicates from '@ohos.data.dataSharePredicates';
import DataShareResultSet from '@ohos.data.DataShareResultSet';
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
let InsertNumber: ValuesBucket = { "age": 1 as long };
let BatchInsert: Array<ValuesBucket> = [
  { "age": 1 as long },
  { "name": "sun" },
  { "isStudent": true },
  { "Binary": new Uint8Array([1, 2, 3]) }
];
let ErrBatchInsert: Array<ValuesBucket> = [
  { "age": 1 as long },
  { "age": true },
  { "name": "sun" }
];
let Insert100: Array<ValuesBucket> = [];
for (let i = 0; i < 100; i++) {
  Insert100.push({ "name": "sun", "age": (i + 1) as long });
}
 
let InsertString: ValuesBucket = { "name": "sun" };
let InsertLong: ValuesBucket = { "age": 1234 as long };
let InsertDouble: ValuesBucket = { "phoneNumber": 1.0 };
let InsertBlob: ValuesBucket = { "Binary": new Uint8Array([1, 2, 3]) };
 
//UpdateValuesBucket
let UpdateNumber: ValuesBucket = { "age": 11 as long };
let Update100: ValuesBucket = { "name": "suns", "age": 1 as long };
 
//Predicates
let PredicatesNumber = new dataSharePredicates.DataSharePredicates();
PredicatesNumber.equalTo("age", 1 as long);
let PredicatesAll = new dataSharePredicates.DataSharePredicates();
 
//getColumnIndex
const IndexNumber = "age";
const IndexString = "name";
const IndexUint8Array = "Binary";
 
//Return Expect
const ResultSetNumber = 11;
const ResultSetDouble = "phoneNumber";
const ResultSetUpdate100 = "suns";
const DataProcessResultZero = 0;
const DataProcessResultOne = 1;
const DataProcessResultTwo = 2;
const DataProcessResultThree = 3;
const DataProcessResultFour = 4;
const DataProcessResultOnehundred = 100;
const DataProcessResultString = "sun";
const DataProcessResultLong = 1234;
const DataProcessResultBlob = new Uint8Array([1, 2, 3]);
const DataProcessResultDouble = 1.0;
const column = 1;
 
let Column: int;
 
function onChangeNotify() {
  console.info("===>onChangeNotify===>");
};
 
function notifyChangeCallback() {
  console.info("===>notifyChangeCallback===>");
};
 
export default function DataShareCombinationRdb() {
  describe('DataShareCombinationRdb', (): void => {
    const globalThis: GlobalThis = GlobalThis.getInstance();
    beforeAll(async () => {
      try {
        context = AppStorage.get<common.UIAbilityContext>("context") as common.UIAbilityContext;
        await globalThis.connectDataShareExtAbility(context);
        await Utils.msSleep(2000);
        hilog.info(domain, tag, '%{public}s', `[ttt] <<Consumer>> beforeAll helper: ` + JSON.stringify(globalThis.helper));
      } catch (err: BusinessError) {
        hilog.error(domain, tag, '%{public}s', `[ttt] <<Consumer>> beforeAll error: code: ${err.code}, message: ${err.message}`);
      }
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_8600
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : Insert Update Query Delete in Rdb
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    it('SUB_DDM_DataShareCombinationRdb0100', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
      try {
        await pubfun.publicinsert(globalThis.helper!, uri, InsertNumber).then((data) => {
          console.info("TestDataShare going insert = " + data);
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataShareCombinationRdb0100 insert err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicupdate(globalThis.helper!, uri, PredicatesNumber, UpdateNumber).then((data) => {
          console.info("TestDataShare going update = " + data);
          expect(DataProcessResultOne).assertEqual(data);
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataShareCombinationRdb0100 update err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicquery(globalThis.helper!, uri, PredicatesAll, ["*"]).then((data) => {
          console.info("TestDataShare going query = " + data);
          globalThis.ResultSet!.goToFirstRow();
          Column = globalThis.ResultSet!.getColumnIndex(IndexNumber);
          expect(JSON.stringify(ResultSetNumber)).assertEqual(globalThis.ResultSet!.getString(Column));
          globalThis.ResultSet!.close();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataShareCombinationRdb0100 query err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicdelete(globalThis.helper!, uri, PredicatesAll).then((data) => {
          console.info("TestDataShare going delete = " + data);
          expect(DataProcessResultOne).assertEqual(data);
          done();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataShareCombinationRdb0100 delete err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
          done();
        })
      } catch (err) {
        console.info("SUB_DDM_DataShareCombinationRdb0100 fail" + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_8700
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : BatchInsert Update Query Delete in Rdb
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    it('SUB_DDM_DataShareCombinationRdb0200', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
      try {
        await pubfun.publicbatchInsert(globalThis.helper!, uri, BatchInsert).then((data) => {
          console.info("TestDataShare going batchInsert = " + data);
          expect(DataProcessResultFour).assertEqual(data);
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataShareCombinationRdb0200 batchInsert err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicupdate(globalThis.helper!, uri, PredicatesAll, UpdateNumber).then((data) => {
          console.info("TestDataShare going update = " + data);
          expect(DataProcessResultFour).assertEqual(data);
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataShareCombinationRdb0200 update err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicquery(globalThis.helper!, uri, PredicatesAll, ["*"]).then((data) => {
          console.info("TestDataShare going query = " + data);
          globalThis.ResultSet!.goToLastRow();
          Column = globalThis.ResultSet!.getColumnIndex(IndexNumber)
          expect(JSON.stringify(ResultSetNumber)).assertEqual(globalThis.ResultSet!.getString(Column));
          globalThis.ResultSet!.close();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataShareCombinationRdb0200 query err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicdelete(globalThis.helper!, uri, PredicatesAll).then((data) => {
          console.info("TestDataShare going delete = " + data);
          expect(DataProcessResultFour).assertEqual(data);
          done();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataShareCombinationRdb0200 delete err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
          done();
        })
      } catch (err) {
        console.info("SUB_DDM_DataShareCombinationRdb0200 fail" + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_8800
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : On on
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    it('SUB_DDM_DataShareCombinationRdb0300', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, (): void => {
      try {
        globalThis.helper!.onDataChange(uri, onChangeNotify);
        console.info("TestDataShare going On 1");
        globalThis.helper!.onDataChange(uri, onChangeNotify);
        console.info("TestDataShare going On 2");
      } catch (err) {
        console.info("SUB_DDM_DataShareCombinationRdb0300 fail" + JSON.stringify(err));
        expect(err).assertFail();
      }
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_8900
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : Off off
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    it('SUB_DDM_DataShareCombinationRdb0400', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, (): void => {
      try {
        globalThis.helper!.onDataChange(uri, onChangeNotify);
        console.info("TestDataShare going On 1");
        globalThis.helper!.offDataChange(uri);
        console.info("TestDataShare going Off 1");
        globalThis.helper!.offDataChange(uri);
        console.info("TestDataShare going Off 2");
      } catch (err) {
        console.info("SUB_DDM_DataShareCombinationRdb0400  fail" + JSON.stringify(err));
        expect(err).assertFail();
      }
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_9000
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : BatchInsert Update Query Delete in Rdb
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 2
     */
    it('SUB_DDM_DataShareCombinationRdb0500', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL2, async (done: () => void): Promise<void> => {
      try {
        await pubfun.publicbatchInsert(globalThis.helper!, uri, Insert100).then((data) => {
          console.info("TestDataShare going batchInsert = " + data);
          expect(DataProcessResultOnehundred).assertEqual(data);
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataShareCombinationRdb0500 batchInsert err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicupdate(globalThis.helper!, uri, PredicatesAll, Update100).then((data) => {
          console.info("TestDataShare going update = " + data);
          expect(DataProcessResultOnehundred).assertEqual(data);
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataShareCombinationRdb0500 update err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await Utils.msSleep(1000);
        await pubfun.publicquery(globalThis.helper!, uri, PredicatesAll, ["*"]).then((data) => {
          console.info("TestDataShare going query = " + data);
          globalThis.ResultSet!.goToLastRow();
          Column = globalThis.ResultSet!.getColumnIndex(IndexString);
          expect(ResultSetUpdate100).assertEqual(globalThis.ResultSet!.getString(Column));
          globalThis.ResultSet!.close();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataShareCombinationRdb0500 query err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicdelete(globalThis.helper!, uri, PredicatesAll).then((data) => {
          console.info("TestDataShare going delete = " + data);
          expect(DataProcessResultOnehundred).assertEqual(data);
          done();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataShareCombinationRdb0500 delete err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
          done();
        })
      } catch (err) {
        console.info("SUB_DDM_DataShareCombinationRdb0500 fail" + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_9100
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : NotifyChange
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    it('SUB_DDM_DataShareNotifyChangeRdb0100', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
      try {
        let Count = 1;
        globalThis.helper!.onDataChange(dataShare.SubscriptionType.SUBSCRIPTION_TYPE_EXACT_URI, uri, (data: dataShare.ChangeInfo) => {
          if (Count < 4) {
            console.info("TestDataShare notifyChange data = " + data);
            expect(true).assertEqual(data == null);
            Count = Count + 1;
          } else {
            console.info("TestDataShare notifyChange data = " + data);
            expect(true).assertEqual(data == null);
            globalThis.helper!.offDataChange(uri);
            console.info("TestDataShare off");
            done();
          }
        })
        await pubfun.publicinsert(globalThis.helper!, uri, InsertNumber).then((data) => {
          console.info("TestDataShare going insert = " + data);
          // globalThis.helper!.notifyChange(uri, notifyChangeCallback);
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataShareNotifyChangeRdb0100 insert err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicupdate(globalThis.helper!, uri, PredicatesAll, UpdateNumber).then((data) => {
          console.info("TestDataShare going update = " + data);
          // globalThis.helper!.notifyChange(uri, notifyChangeCallback);
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataShareNotifyChangeRdb0100 update err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicquery(globalThis.helper!, uri, PredicatesAll, ["*"]).then((data) => {
          console.info("TestDataShare going query = " + data);
          // globalThis.helper!.notifyChange(uri);
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataShareNotifyChangeRdb0100 query err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicdelete(globalThis.helper!, uri, PredicatesAll).then((data) => {
          console.info("TestDataShare going delete = " + data);
          // globalThis.helper!.notifyChange(uri);
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataShareNotifyChangeRdb0100 delete err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        done();
      } catch (err) {
        console.info("SUB_DDM_DataShareNotifyChangeRdb0100 fail" + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_9200
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : DataShare Supports ResultSet
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    it('SUB_DDM_DataShareResultSetRdb0100', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
      try {
        await pubfun.publicbatchInsert(globalThis.helper!, uri, BatchInsert).then((data) => {
          console.info("TestDataShare going batchInsert = " + data);
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataShareResultSetRdb0100 batchInsert err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicquery(globalThis.helper!, uri, PredicatesAll, ["*"]).then((data) => {
          console.info("TestDataShare going query = " + data);
          expect(true).assertEqual(globalThis.ResultSet!.goToFirstRow());
          expect(true).assertEqual(globalThis.ResultSet!.goToNextRow());
          expect(true).assertEqual(globalThis.ResultSet!.goToLastRow());
          // expect(true).assertEqual(globalThis.ResultSet!.goToPreviousRow());
          // expect(true).assertEqual(globalThis.ResultSet!.goTo(column));
          // expect(true).assertEqual(globalThis.ResultSet!.goToRow(column));
          globalThis.ResultSet!.close()
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataShareResultSetRdb0100 query err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicdelete(globalThis.helper!, uri, PredicatesAll).then((data) => {
          console.info("TestDataShare going deleteall = " + data);
          done();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataShareResultSetRdb0100 deleteall err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
          done();
        })
      } catch (err) {
        console.info("SUB_DDM_DataShareResultSetRdb0100 fail" + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_9300
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : DataShare Supports ResultSet
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    it('SUB_DDM_DataShareResultSetRdb0200', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
      try {
        await pubfun.publicquery(globalThis.helper!, uri, PredicatesAll, ["*"]).then((data) => {
          console.info("TestDataShare going query = " + data);
          expect(false).assertEqual(globalThis.ResultSet!.goToFirstRow());
          expect(false).assertEqual(globalThis.ResultSet!.goToNextRow());
          expect(false).assertEqual(globalThis.ResultSet!.goToLastRow());
          // expect(false).assertEqual(globalThis.ResultSet!.goToPreviousRow());
          // expect(false).assertEqual(globalThis.ResultSet!.goTo(column));
          // expect(false).assertEqual(globalThis.ResultSet!.goToRow(column));
          globalThis.ResultSet!.close();
          done();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataShareResultSetRdb0200 query err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
          done();
        })
      } catch (err) {
        console.info("SUB_DDM_DataShareResultSetRdb0200 fail" + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_9400
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : DataShare Supports ResultSet
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    it('SUB_DDM_DataShareResultSetRdb0300', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
      try {
        await pubfun.publicinsert(globalThis.helper!, uri, InsertString).then((data) => {
          console.info("TestDataShare going insert = " + data);
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataShareResultSetRdb0300 insert err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicquery(globalThis.helper!, uri, PredicatesAll, ["*"]).then((data) => {
          console.info("TestDataShare going query = " + data);
          globalThis.ResultSet!.goToFirstRow();
          Column = globalThis.ResultSet!.getColumnIndex(IndexString);
          expect(DataProcessResultString).assertEqual(globalThis.ResultSet!.getString(Column));
          // expect(DataProcessResultThree).assertEqual(globalThis.ResultSet!.getDataType(Column));
          globalThis.ResultSet!.close();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataShareResultSetRdb0300 query err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicdelete(globalThis.helper!, uri, PredicatesAll).then((data) => {
          console.info("TestDataShare going deleteall = " + data);
          expect(DataProcessResultOne).assertEqual(data);
          done();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataShareResultSetRdb0300 deleteall err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
          done();
        })
      } catch (err) {
        console.info("SUB_DDM_DataShareResultSetRdb0300 fail" + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_9500
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : DataShare Supports ResultSet
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    it('SUB_DDM_DataShareResultSetRdb0400', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
      try {
        await pubfun.publicinsert(globalThis.helper!, uri, InsertLong).then((data) => {
          console.info("TestDataShare going insert = " + data);
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataShareResultSetRdb0400 insert err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicquery(globalThis.helper!, uri, PredicatesAll, ["*"]).then((data) => {
          console.info("TestDataShare going query = " + data);
          globalThis.ResultSet!.goToFirstRow();
          Column = globalThis.ResultSet!.getColumnIndex(IndexNumber);
          expect(DataProcessResultLong).assertEqual(globalThis.ResultSet!.getLong(Column));
          // expect(DataProcessResultOne).assertEqual(globalThis.ResultSet!.getDataType(Column));
          globalThis.ResultSet!.close();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataShareResultSetRdb0400 query err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicdelete(globalThis.helper!, uri, PredicatesAll).then((data) => {
          console.info("TestDataShare going deleteall = " + data);
          expect(DataProcessResultOne).assertEqual(data);
          done();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataShareResultSetRdb0400 deleteall err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
          done();
        })
      } catch (err) {
        console.info("SUB_DDM_DataShareResultSetRdb0400 fail" + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_9600
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : DataShare Supports ResultSet
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    it('SUB_DDM_DataShareResultSetRdb0500', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
      try {
        await pubfun.publicinsert(globalThis.helper!, uri, InsertDouble).then((data) => {
          console.info("TestDataShare going insert = " + data);
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataShareResultSetRdb0500 insert err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicquery(globalThis.helper!, uri, PredicatesAll, ["*"]).then((data) => {
          console.info("TestDataShare going query = " + data);
          globalThis.ResultSet!.goToFirstRow();
          Column = globalThis.ResultSet!.getColumnIndex(ResultSetDouble)
          // expect(DataProcessResultDouble).assertEqual(globalThis.ResultSet!.getDouble(Column));
          // expect(DataProcessResultTwo).assertEqual(globalThis.ResultSet!.getDataType(Column));
          globalThis.ResultSet!.close();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataShareResultSetRdb0500 query err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicdelete(globalThis.helper!, uri, PredicatesAll).then((data) => {
          console.info("TestDataShare going deleteall = " + data);
          expect(DataProcessResultOne).assertEqual(data);
          done();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataShareResultSetRdb0500 deleteall err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
          done();
        })
      } catch (err) {
        console.info("SUB_DDM_DataShareResultSetRdb0500 fail" + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_9700
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : DataShare Supports ResultSet
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    it('SUB_DDM_DataShareResultSetRdb0600', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
      try {
        await pubfun.publicinsert(globalThis.helper!, uri, InsertBlob).then((data) => {
          console.info("TestDataShare going insert = " + data);
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataShareResultSetRdb0600 insert err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicquery(globalThis.helper!, uri, PredicatesAll, ["*"]).then((data) => {
          console.info("TestDataShare going query = " + data);
          globalThis.ResultSet!.goToFirstRow();
          Column = globalThis.ResultSet!.getColumnIndex(IndexUint8Array)
          expect(JSON.stringify(DataProcessResultBlob))
          // .assertEqual(JSON.stringify(globalThis.ResultSet!.getBlob(Column)));
          // expect(DataProcessResultFour).assertEqual(globalThis.ResultSet!.getDataType(Column));
          globalThis.ResultSet!.close();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataShareResultSetRdb0600 query err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicdelete(globalThis.helper!, uri, PredicatesAll).then((data) => {
          console.info("TestDataShare going deleteall = " + data);
          expect(DataProcessResultOne).assertEqual(data);
          done();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataShareResultSetRdb0600 deleteall err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
          done();
        })
      } catch (err) {
        console.info("SUB_DDM_DataShareResultSetRdb0600 fail" + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_9800
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : DataShare Supports ResultSet
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    it('SUB_DDM_DataShareResultSetRdb0700', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
      try {
        await pubfun.publicinsert(globalThis.helper!, uri, InsertString).then((data) => {
          console.info("TestDataShare going insert = " + data);
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataShareResultSetRdb0700 insert err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicquery(globalThis.helper!, uri, PredicatesAll, ["*"]).then((data) => {
          console.info("TestDataShare going query = " + data);
          globalThis.ResultSet!.goToFirstRow();
          Column = globalThis.ResultSet!.getColumnIndex(IndexNumber)
          // expect(DataProcessResultZero).assertEqual(globalThis.ResultSet!.getDataType(Column));
          globalThis.ResultSet!.close();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataShareResultSetRdb0700 query err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicdelete(globalThis.helper!, uri, PredicatesAll).then((data) => {
          console.info("TestDataShare going deleteall = " + data);
          expect(DataProcessResultOne).assertEqual(data);
          done();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataShareResultSetRdb0700 deleteall err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
          done();
        })
      } catch (err) {
        console.info("SUB_DDM_DataShareResultSetRdb0700 fail" + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_9900
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : DataShare Supports ResultSet
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    it('SUB_DDM_DataShareResultSetRdb0800', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
      try {
        await pubfun.publicinsert(globalThis.helper!, uri, InsertNumber).then((data) => {
          console.info("TestDataShare going insert = " + data);
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataShareResultSetRdb0800 insert err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicquery(globalThis.helper!, uri, PredicatesAll, ["*"]).then((data) => {
          console.info("TestDataShare going query = " + data);
          globalThis.ResultSet!.goToFirstRow();
          Column = globalThis.ResultSet!.getColumnIndex(IndexNumber);
          expect(DataProcessResultTwo).assertEqual(Column);
          // expect(IndexNumber).assertEqual(globalThis.ResultSet!.getColumnName(Column));
          globalThis.ResultSet!.close();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataShareResultSetRdb0800 query err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicdelete(globalThis.helper!, uri, PredicatesAll).then((data) => {
          console.info("TestDataShare going deleteall = " + data);
          expect(DataProcessResultOne).assertEqual(data);
          done();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataShareResultSetRdb0800 deleteall err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
          done();
        })
      } catch (err) {
        console.info("SUB_DDM_DataShareResultSetRdb0800 fail" + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })
  })
}