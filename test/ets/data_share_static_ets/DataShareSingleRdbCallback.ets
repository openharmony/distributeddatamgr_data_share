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
import dataShare from '@ohos.data.dataShare';
import dataSharePredicates from '@ohos.data.dataSharePredicates';
import * as pubfun from './DataSharePublicfunction_Callback';
import common from '@ohos.app.ability.common';
import { BusinessError } from '@ohos.base';
import { ValuesBucket } from '@ohos.data.ValuesBucket';
import UIAbility from '@ohos.app.ability.UIAbility';
import abilityDelegatorRegistry from '@ohos.app.ability.abilityDelegatorRegistry';
import { AppStorage } from '@ohos.arkui.stateManagement';
import { GlobalThis } from '../../ets/entryability/EntryAbility';
 
let domain: int = 0x0000; //日志标识,0x0000作为测试框架的业务标识
let tag: string = 'testTag'; //日志标识字符串,作为tag标识当前runner类下的测试行为
 
let context: common.UIAbilityContext;
//CreateDataShareHelper
let uri = ("datashare:///com.acts.datasharetest.DataShare");
 
//InsertValuesBucket
let InsertNumber: ValuesBucket = { "age": 1 as long };
let InsertString: ValuesBucket = { "name": "sun" };
let InsertBoolean: ValuesBucket = { "isStudent": true };
let InsertUint8Array: ValuesBucket = { "Binary": new Uint8Array([1, 2, 3]) };
let InsertNull: ValuesBucket = { "name": "" };
let BatchInsert: Array<ValuesBucket> =[
  { "age": 1 as long },
  { "name": "sun" },
  { "isStudent": true },
  { "Binary": new Uint8Array([1, 2, 3]) }
];
 
//UpdateValuesBucket
let UpdateNumber: ValuesBucket = { "age": 11 as long };
let UpdateString: ValuesBucket = { "name": "suns" };
let UpdateBoolean: ValuesBucket = { "isStudent": false };
let UpdateUint8Array: ValuesBucket = { "Binary": new Uint8Array([1, 2, 3, 4]) };
let UpdateNull: ValuesBucket = { "name": "1" };
 
//Predicates
let PredicatesNumber = new dataSharePredicates.DataSharePredicates();
let PredicatesString = new dataSharePredicates.DataSharePredicates();
let PredicatesBoolean = new dataSharePredicates.DataSharePredicates();
let PredicatesUint8Array = new dataSharePredicates.DataSharePredicates();
let PredicatesNull = new dataSharePredicates.DataSharePredicates();
let PredicatesAll = new dataSharePredicates.DataSharePredicates();
 
//getColumnIndex
const IndexNumber = "age";
const IndexString = "name";
const IndexBoolean = "isStudent";
const IndexUint8Array = "Binary";
const IndexNull = "name";
 
//Return Expect
const DataProcessResultOne = 1;
const DataProcessResultFour = 4;
const GetType = "image";
// const GetFileTypes: Array<string> = ["type01", "type02", "type03"];
const Normalize = "normalize+" + uri;
const Denormalize = "denormalize+" + uri;
const ResultSetNumber = 1;
const ResultSetString = "sun";
const ResultSetBoolean = 1;
const ResultSetUint8Array = new Uint8Array([1, 2, 3]);
const ResultSetNull = "";
const Mode = "rwt";
 
function onChangeNotify() {
  console.info("===>onChangeNotify===>");
};
 
export default function DataShareSingleRdb_Callback() {
  describe('DataShareSingleRdb_Callback', (): void => {
    const globalThis: GlobalThis = GlobalThis.getInstance();
    beforeAll(async () => {
      hilog.info(domain, tag, '%{public}s', 'beforeAll start');
      let abilityDelegator: abilityDelegatorRegistry.AbilityDelegator = abilityDelegatorRegistry.getAbilityDelegator()
      abilityDelegator.addAbilityMonitor({
        abilityName: "EntryAbility",
        moduleName: "entry",
        onAbilityCreate: (ability: UIAbility) : void => {
          context = ability.context;
          AppStorage.setOrCreate("context", ability.context);
          hilog.info(domain, tag, '%{public}s', '[ttt] ability.context.databaseDir:' + context.databaseDir);
          hilog.info(domain, tag, '%{public}s', '[ttt] ability.context.area:' + context.area);
          hilog.info(domain, tag, '%{public}s', 'onAbilityCreate end');
        },
      }, (err : BusinessError | null) : void => {
        if (err && err.code != 0 ) {
          hilog.info(domain, tag, '%{public}s', '-----' + err.code);
        }
        hilog.info(domain, tag, '%{public}s', 'BusinessError  end');
      });
      await Utils.msSleep(2000);
      abilityDelegator.executeShellCommand("aa start -a EntryAbility -b com.acts.datasharerdbclient.static");
      await Utils.msSleep(2000);
 
      // globalThis.Uint8 = new Uint8Array([1, 2, 3]);
      PredicatesNumber.equalTo("age", 1 as long);
      PredicatesString.equalTo("name", "sun");
      PredicatesBoolean.equalTo("isStudent", true);
      // PredicatesUint8Array.equalTo("Binary", globalThis.Uint8!);
      PredicatesNull.equalTo("name", "");
      try {
        context = AppStorage.get<common.UIAbilityContext>("context") as common.UIAbilityContext;
        await globalThis.connectDataShareExtAbility(context);
        await Utils.msSleep(2000);
        hilog.info(domain, tag, '%{public}s', `[ttt] <<Consumer>> beforeAll helper: ` + JSON.stringify(globalThis.helper!));
        await pubfun.publicinsert(globalThis.helper!, uri, InsertNumber).then((data) => {
          console.info("TestDataShare going insert = " + data);
          globalThis.Count = data;
        })
        await pubfun.publicdelete(globalThis.helper!, uri, PredicatesAll).then((data) => {
          console.info("TestDataShare going delete = " + data);
        })
      } catch (err: BusinessError) {
        hilog.error(domain, tag, '%{public}s', `[ttt] <<Consumer>> beforeAll error: code: ${err.code}, message: ${err.message}`);
      }
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0200
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : Insert number type data in Rdb
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_DDM_DataShareInsertRdb0100', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL0, async (done: () => void): Promise<void> => {
      try {
        await pubfun.publicinsert(globalThis.helper!, uri, InsertNumber).then((data) => {
          console.info("TestDataShare going insert = " + data);
          let count = globalThis.Count as int;
          globalThis.Count = count + 1;
          expect(globalThis.Count!).assertEqual(data);
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataShareInsertRdb0100 fail err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicdelete(globalThis.helper!, uri, PredicatesAll).then((data) => {
          console.info("TestDataShare going delete = " + data);
          expect(DataProcessResultOne).assertEqual(data);
          done();
        })
      } catch (err) {
        console.info("SUB_DDM_DataShareInsertRdb0100 fail " + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0100
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : Insert string type data in Rdb
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_DDM_DataShareInsertRdb0200', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL0, async (done: () => void): Promise<void> => {
      try {
        await pubfun.publicinsert(globalThis.helper!, uri, InsertString).then((data) => {
          console.info("TestDataShare going insert = " + data);
          let count = globalThis.Count as int;
          globalThis.Count = count + 1;
          expect(globalThis.Count!).assertEqual(data);
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataShareInsertRdb0200 fail err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicdelete(globalThis.helper!, uri, PredicatesAll).then((data) => {
          console.info("TestDataShare going delete = " + data);
          expect(DataProcessResultOne).assertEqual(data);
          done();
        })
      } catch (err) {
        console.info("SUB_DDM_DataShareInsertRdb0200 fail " + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0300
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : Insert boolean type data in Rdb
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_DDM_DataShareInsertRdb0300', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL0, async (done: () => void): Promise<void> => {
      try {
        await pubfun.publicinsert(globalThis.helper!, uri, InsertBoolean).then((data) => {
          console.info("TestDataShare going insert = " + data);
          let count = globalThis.Count as int;
          globalThis.Count = count + 1;
          expect(globalThis.Count!).assertEqual(data);
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataShareInsertRdb0300 fail err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicdelete(globalThis.helper!, uri, PredicatesAll).then((data) => {
          console.info("TestDataShare going delete = " + data);
          expect(DataProcessResultOne).assertEqual(data);
          done();
        })
      } catch (err) {
        console.info("SUB_DDM_DataShareInsertRdb0300 fail " + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0400
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : Insert Uint8Array type data in Rdb
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_DDM_DataShareInsertRdb0400', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL0, async (done: () => void): Promise<void> => {
      try {
        await pubfun.publicinsert(globalThis.helper!, uri, InsertUint8Array).then((data) => {
          console.info("TestDataShare going insert = " + data);
          let count = globalThis.Count as int;
          globalThis.Count = count + 1;
          expect(globalThis.Count!).assertEqual(data);
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataShareInsertRdb0400 fail err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicdelete(globalThis.helper!, uri, PredicatesAll).then((data) => {
          console.info("TestDataShare going delete = " + data);
          expect(DataProcessResultOne).assertEqual(data);
          done();
        })
      } catch (err) {
        console.info("SUB_DDM_DataShareInsertRdb0400 fail " + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0500
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : Insert null type data in Rdb
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_DDM_DataShareInsertRdb0500', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL0, async (done: () => void): Promise<void> => {
      try {
        await pubfun.publicinsert(globalThis.helper!, uri, InsertNull).then((data) => {
          console.info("TestDataShare going insert = " + data);
          let count = globalThis.Count as int;
          globalThis.Count = count + 1;
          expect(globalThis.Count!).assertEqual(data);
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataShareInsertRdb0500 fail err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicdelete(globalThis.helper!, uri, PredicatesAll).then((data) => {
          console.info("TestDataShare going delete = " + data);
          expect(DataProcessResultOne).assertEqual(data);
          done();
        })
      } catch (err) {
        console.info("SUB_DDM_DataShareInsertRdb0500 fail " + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0600
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : Update number type data in Rdb
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_DDM_DataShareUpdateRdb0100', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL0, async (done: () => void): Promise<void> => {
      try {
        await pubfun.publicinsert(globalThis.helper!, uri, InsertNumber).then((data) => {
          console.info("TestDataShare going insert = " + data);
          let count = globalThis.Count as int;
          globalThis.Count = count + 1;
          expect(globalThis.Count!).assertEqual(data);
        })
        await pubfun.publicupdate(globalThis.helper!, uri, PredicatesNumber, UpdateNumber).then((data) => {
          console.info("TestDataShare going update = " + data);
          expect(DataProcessResultOne).assertEqual(data);
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataShareUpdateRdb0100 fail err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicdelete(globalThis.helper!, uri, PredicatesAll).then((data) => {
          console.info("TestDataShare going delete = " + data);
          expect(DataProcessResultOne).assertEqual(data);
          done();
        })
      } catch (err) {
        console.info("SUB_DDM_DataShareUpdateRdb0100 fail " + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0700
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : Update string type data in Rdb
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_DDM_DataShareUpdateRdb0200', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL0, async (done: () => void): Promise<void> => {
      try {
        await pubfun.publicinsert(globalThis.helper!, uri, InsertString).then((data) => {
          console.info("TestDataShare going insert = " + data);
          let count = globalThis.Count as int;
          globalThis.Count = count + 1;
          expect(globalThis.Count!).assertEqual(data);
        })
        await pubfun.publicupdate(globalThis.helper!, uri, PredicatesString, UpdateString).then((data) => {
          console.info("TestDataShare going update = " + data);
          expect(DataProcessResultOne).assertEqual(data);
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataShareUpdateRdb0200 fail err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicdelete(globalThis.helper!, uri, PredicatesAll).then((data) => {
          console.info("TestDataShare going delete = " + data);
          expect(DataProcessResultOne).assertEqual(data);
          done();
        })
      } catch (err) {
        console.info("SUB_DDM_DataShareUpdateRdb0200 fail " + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0800
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : Update boolean type data in Rdb
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_DDM_DataShareUpdateRdb0300', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL0, async (done: () => void): Promise<void> => {
      try {
        await pubfun.publicinsert(globalThis.helper!, uri, InsertBoolean).then((data) => {
          console.info("TestDataShare going insert = " + data);
          let count = globalThis.Count as int;
          globalThis.Count = count + 1;
          expect(globalThis.Count!).assertEqual(data);
        })
        await pubfun.publicupdate(globalThis.helper!, uri, PredicatesBoolean, UpdateBoolean).then((data) => {
          console.info("TestDataShare going update = " + data);
          expect(DataProcessResultOne).assertEqual(data);
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataShareUpdateRdb0300 fail err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicdelete(globalThis.helper!, uri, PredicatesAll).then((data) => {
          console.info("TestDataShare going delete = " + data);
          expect(DataProcessResultOne).assertEqual(data);
          done();
        })
      } catch (err) {
        console.info("SUB_DDM_DataShareUpdateRdb0300 fail " + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_0900
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : Update Uint8Array type data in Rdb
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    // it('SUB_DDM_DataShareUpdateRdb0400', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL0, async (done: () => void): Promise<void> => {
    //   try {
    //     await pubfun.publicinsert(globalThis.helper!, uri, InsertUint8Array).then((data) => {
    //       console.info("TestDataShare going insert = " + data);
    //       let count = globalThis.Count as int;
    //       globalThis.Count = count + 1;
    //       expect(globalThis.Count!).assertEqual(data);
    //     })
    //     await pubfun.publicupdate(globalThis.helper!, uri, PredicatesUint8Array, UpdateUint8Array).then((data) => {
    //       console.info("TestDataShare going update = " + data);
    //       expect(DataProcessResultOne).assertEqual(data);
    //     }).catch((error) => {
    //       let err = error as BusinessError;
    //       console.error(`SUB_DDM_DataShareUpdateRdb0400 fail err: code: ${err.code}, message: ${err.message}`);
    //       // console.info("SUB_DDM_DataShareUpdateRdb0400 err " + JSON.stringify(err));
    //       expect(err).assertFail();
    //     })
    //     await pubfun.publicdelete(globalThis.helper!, uri, PredicatesAll).then((data) => {
    //       console.info("TestDataShare going delete = " + data);
    //       expect(DataProcessResultOne).assertEqual(data);
    //       done();
    //     })
    //   } catch (err) {
    //     console.info("SUB_DDM_DataShareUpdateRdb0400 fail " + JSON.stringify(err));
    //     expect(err).assertFail();
    //     done();
    //   }
    // })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_1000
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : Update null type data in Rdb
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_DDM_DataShareUpdateRdb0500', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL0, async (done: () => void): Promise<void> => {
      try {
        await pubfun.publicinsert(globalThis.helper!, uri, InsertNull).then((data) => {
          console.info("TestDataShare going insert = " + data);
          let count = globalThis.Count as int;
          globalThis.Count = count + 1;
          expect(globalThis.Count!).assertEqual(data);
        })
        await pubfun.publicupdate(globalThis.helper!, uri, PredicatesNull, UpdateNull).then((data) => {
          console.info("TestDataShare going update = " + data);
          expect(DataProcessResultOne).assertEqual(data);
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataShareUpdateRdb0500 fail err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicdelete(globalThis.helper!, uri, PredicatesAll).then((data) => {
          console.info("TestDataShare going delete = " + data);
          expect(DataProcessResultOne).assertEqual(data);
          done();
        })
      } catch (err) {
        console.info("SUB_DDM_DataShareUpdateRdb0500 fail " + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_1100
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : Query number type data in Rdb
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_DDM_DataShareQueryRdb0100', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL0, async (done: () => void): Promise<void> => {
      try {
        await pubfun.publicinsert(globalThis.helper!, uri, InsertNumber).then((data) => {
          console.info("TestDataShare going insert = " + data);
          let count = globalThis.Count as int;
          globalThis.Count = count + 1;
          expect(globalThis.Count!).assertEqual(data);
        })
        await pubfun.publicquery(globalThis.helper!, uri, PredicatesNumber, ["*"]).then((data) => {
          console.info("TestDataShare going query = " + data);
          globalThis.ResultSet!.goToFirstRow();
          expect(JSON.stringify(ResultSetNumber))
            .assertEqual(globalThis.ResultSet!.getString(globalThis.ResultSet!.getColumnIndex(IndexNumber)));
          globalThis.ResultSet!.close();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataShareQueryRdb0100 fail err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicdelete(globalThis.helper!, uri, PredicatesAll).then((data) => {
          console.info("TestDataShare going delete = " + data);
          expect(DataProcessResultOne).assertEqual(data);
          done();
        })
      } catch (err) {
        console.info("SUB_DDM_DataShareQueryRdb0100 fail " + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_1200
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : Query string type data in Rdb
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_DDM_DataShareQueryRdb0200', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL0, async (done: () => void): Promise<void> => {
      try {
        await pubfun.publicinsert(globalThis.helper!, uri, InsertString).then((data) => {
          console.info("TestDataShare going insert = " + data);
          let count = globalThis.Count as int;
          globalThis.Count = count + 1;
          expect(globalThis.Count!).assertEqual(data);
        })
        await pubfun.publicquery(globalThis.helper!, uri, PredicatesString, ["*"]).then((data) => {
          console.info("TestDataShare going query = " + data);
          globalThis.ResultSet!.goToFirstRow();
          expect(ResultSetString)
            .assertEqual(globalThis.ResultSet!.getString(globalThis.ResultSet!.getColumnIndex(IndexString)));
          globalThis.ResultSet!.close();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataShareQueryRdb0200 fail err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicdelete(globalThis.helper!, uri, PredicatesAll).then((data) => {
          console.info("TestDataShare going delete = " + data);
          expect(DataProcessResultOne).assertEqual(data);
          done();
        })
      } catch (err) {
        console.info("SUB_DDM_DataShareQueryRdb0200 fail " + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_1300
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : Query boolean type data in Rdb
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_DDM_DataShareQueryRdb0300', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL0, async (done: () => void): Promise<void> => {
      try {
        await pubfun.publicinsert(globalThis.helper!, uri, InsertBoolean).then((data) => {
          console.info("TestDataShare going insert = " + data);
          let count = globalThis.Count as int;
          globalThis.Count = count + 1;
          expect(globalThis.Count!).assertEqual(data);
        })
        await pubfun.publicquery(globalThis.helper!, uri, PredicatesBoolean, ["*"]).then((data) => {
          console.info("TestDataShare going query = " + data);
          globalThis.ResultSet!.goToFirstRow();
          expect(JSON.stringify(ResultSetBoolean))
            .assertEqual(globalThis.ResultSet!.getString(globalThis.ResultSet!.getColumnIndex(IndexBoolean)));
          globalThis.ResultSet!.close();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataShareQueryRdb0300 fail err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicdelete(globalThis.helper!, uri, PredicatesAll).then((data) => {
          console.info("TestDataShare going delete = " + data);
          expect(DataProcessResultOne).assertEqual(data);
          done();
        })
      } catch (err) {
        console.info("SUB_DDM_DataShareQueryRdb0300 fail " + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })

    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_1400
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : Query Uint8Array type data in Rdb
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    // it('SUB_DDM_DataShareQueryRdb0400', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL0, async (done: () => void): Promise<void> => {
    //   try {
    //     await pubfun.publicinsert(globalThis.helper!, uri, InsertUint8Array).then((data) => {
    //       console.info("TestDataShare going insert = " + data);
    //       let count = globalThis.Count as int;
    //       globalThis.Count = count + 1;
    //       expect(globalThis.Count!).assertEqual(data);
    //     })
    //     await pubfun.publicquery(globalThis.helper!, uri, PredicatesUint8Array, ["*"]).then((data) => {
    //       console.info("TestDataShare going query = " + data);
    //       globalThis.ResultSet!.goToFirstRow();
    //       expect(JSON.stringify(ResultSetUint8Array))
    //         .assertEqual(
    //           JSON.stringify(globalThis.ResultSet!.getBlob(globalThis.ResultSet!.getColumnIndex(IndexUint8Array))));
    //       globalThis.ResultSet!.close();
    //     }).catch((error) => {
    //       let err = error as BusinessError;
    //       console.error(`SUB_DDM_DataShareQueryRdb0400 fail err: code: ${err.code}, message: ${err.message}`);
    //       // console.info("SUB_DDM_DataShareQueryRdb0400 err " + JSON.stringify(err));
    //       expect(err).assertFail();
    //     })
    //     await pubfun.publicdelete(globalThis.helper!, uri, PredicatesAll).then((data) => {
    //       console.info("TestDataShare going delete = " + data);
    //       expect(DataProcessResultOne).assertEqual(data);
    //       done();
    //     })
    //   } catch (err) {
    //     console.info("SUB_DDM_DataShareQueryRdb0400 fail " + JSON.stringify(err));
    //     expect(err).assertFail();
    //     done();
    //   }
    // })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_1500
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : Query null type data in Rdb
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_DDM_DataShareQueryRdb0500', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL0, async (done: () => void): Promise<void> => {
      try {
        await pubfun.publicinsert(globalThis.helper!, uri, InsertNull).then((data) => {
          console.info("TestDataShare going insert = " + data);
          let count = globalThis.Count as int;
          globalThis.Count = count + 1;
          expect(globalThis.Count!).assertEqual(data);
        })
        await pubfun.publicquery(globalThis.helper!, uri, PredicatesNull, ["*"]).then((data) => {
          console.info("TestDataShare going query = " + data);
          globalThis.ResultSet!.goToFirstRow();
          expect(ResultSetNull)
            .assertEqual(globalThis.ResultSet!.getString(globalThis.ResultSet!.getColumnIndex(IndexNull)));
          globalThis.ResultSet!.close();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataShareQueryRdb0500 fail err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicdelete(globalThis.helper!, uri, PredicatesAll).then((data) => {
          console.info("TestDataShare going delete = " + data);
          expect(DataProcessResultOne).assertEqual(data);
          done();
        })
      } catch (err) {
        console.info("SUB_DDM_DataShareQueryRdb0500 fail " + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_1600
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : Delete number type data in Rdb
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_DDM_DataShareDeleteRdb0100', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL0, async (done: () => void): Promise<void> => {
      try {
        await pubfun.publicinsert(globalThis.helper!, uri, InsertNumber).then((data) => {
          console.info("TestDataShare going insert = " + data);
          let count = globalThis.Count as int;
          globalThis.Count = count + 1;
          expect(globalThis.Count!).assertEqual(data);
        })
        await pubfun.publicdelete(globalThis.helper!, uri, PredicatesNumber).then((data) => {
          console.info("TestDataShare going delete = " + data);
          expect(DataProcessResultOne).assertEqual(data);
          done();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataShareDeleteRdb0100 fail err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
          done();
        })
      } catch (err) {
        console.info("SUB_DDM_DataShareDeleteRdb0100 fail " + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_1700
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : Delete string type data in Rdb
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_DDM_DataShareDeleteRdb0200', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL0, async (done: () => void): Promise<void> => {
      try {
        await pubfun.publicinsert(globalThis.helper!, uri, InsertString).then((data) => {
          console.info("TestDataShare going insert = " + data);
          let count = globalThis.Count as int;
          globalThis.Count = count + 1;
          expect(globalThis.Count!).assertEqual(data);
        })
        await pubfun.publicdelete(globalThis.helper!, uri, PredicatesString).then((data) => {
          console.info("TestDataShare going delete = " + data);
          expect(DataProcessResultOne).assertEqual(data);
          done();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataShareDeleteRdb0200 fail err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
          done();
        })
      } catch (err) {
        console.info("SUB_DDM_DataShareDeleteRdb0200 fail " + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_1800
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : Delete boolean type data in Rdb
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_DDM_DataShareDeleteRdb0300', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL0, async (done: () => void): Promise<void> => {
      try {
        await pubfun.publicinsert(globalThis.helper!, uri, InsertBoolean).then((data) => {
          console.info("TestDataShare going insert = " + data);
          let count = globalThis.Count as int;
          globalThis.Count = count + 1;
          expect(globalThis.Count!).assertEqual(data);
        })
        await pubfun.publicdelete(globalThis.helper!, uri, PredicatesBoolean).then((data) => {
          console.info("TestDataShare going delete = " + data);
          expect(DataProcessResultOne).assertEqual(data);
          done();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataShareDeleteRdb0300 fail err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
          done();
        })
      } catch (err) {
        console.info("SUB_DDM_DataShareDeleteRdb0300 fail " + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_1900
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : Delete Uint8Array type data in Rdb
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    // it('SUB_DDM_DataShareDeleteRdb0400', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL0, async (done: () => void): Promise<void> => {
    //   try {
    //     await pubfun.publicinsert(globalThis.helper!, uri, InsertUint8Array).then((data) => {
    //       console.info("TestDataShare going insert = " + data);
    //       let count = globalThis.Count as int;
    //       globalThis.Count = count + 1;
    //       expect(globalThis.Count!).assertEqual(data);
    //     })
    //     await pubfun.publicdelete(globalThis.helper!, uri, PredicatesUint8Array).then((data) => {
    //       console.info("TestDataShare going delete = " + data);
    //       expect(DataProcessResultOne).assertEqual(data);
    //       done();
    //     }).catch((error) => {
    //       let err = error as BusinessError;
    //       console.error(`SUB_DDM_DataShareDeleteRdb0400 fail err: code: ${err.code}, message: ${err.message}`);
    //       // console.info("SUB_DDM_DataShareDeleteRdb0400 err " + JSON.stringify(err));
    //       expect(err).assertFail();
    //       done();
    //     })
    //   } catch (err) {
    //     console.info("SUB_DDM_DataShareDeleteRdb0400 fail " + JSON.stringify(err));
    //     expect(err).assertFail();
    //     done();
    //   }
    // })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_2000
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : Delete nul type data in Rdb
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_DDM_DataShareDeleteRdb0500', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL0, async (done: () => void): Promise<void> => {
      try {
        await pubfun.publicinsert(globalThis.helper!, uri, InsertNull).then((data) => {
          console.info("TestDataShare going insert = " + data);
          let count = globalThis.Count as int;
          globalThis.Count = count + 1;
          expect(globalThis.Count!).assertEqual(data);
        })
        await pubfun.publicdelete(globalThis.helper!, uri, PredicatesNull).then((data) => {
          console.info("TestDataShare going delete = " + data);
          expect(DataProcessResultOne).assertEqual(data);
          done();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataShareDeleteRdb0500 fail err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
          done();
        })
      } catch (err) {
        console.info("SUB_DDM_DataShareDeleteRdb0500 fail " + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_2100
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : BatchInsert data in Rdb
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_DDM_DataShareBatchRdb0100', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL0, async (done: () => void): Promise<void> => {
      try {
        await pubfun.publicbatchInsert(globalThis.helper!, uri, BatchInsert).then((data) => {
          console.info("TestDataShare going batchInsert = " + data);
          globalThis.Count = globalThis.Count! + DataProcessResultFour;
          expect(DataProcessResultFour).assertEqual(data);
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataShareBatchRdb0100 fail err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicdelete(globalThis.helper!, uri, PredicatesAll).then((data) => {
          console.info("TestDataShare going delete = " + data);
          expect(DataProcessResultFour).assertEqual(data);
          done();
        })
      } catch (err) {
        console.info("SUB_DDM_DataShareBatchRdb0100 fail " + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_2200
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : Update data in Rdb
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_DDM_DataShareBatchRdb0200', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL0, async (done: () => void): Promise<void> => {
      try {
        await pubfun.publicbatchInsert(globalThis.helper!, uri, BatchInsert).then((data) => {
          console.info("TestDataShare going batchInsert = " + data);
          globalThis.Count = globalThis.Count! + DataProcessResultFour;
          expect(DataProcessResultFour).assertEqual(data);
        })
        await pubfun.publicupdate(globalThis.helper!, uri, PredicatesAll, UpdateNumber).then((data) => {
          console.info("TestDataShare going update = " + data);
          expect(DataProcessResultFour).assertEqual(data);
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataShareBatchRdb0200 fail err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
        })
        await pubfun.publicdelete(globalThis.helper!, uri, PredicatesAll).then((data) => {
          console.info("TestDataShare going delete = " + data);
          expect(DataProcessResultFour).assertEqual(data);
          done();
        })
      } catch (err) {
        console.info("SUB_DDM_DataShareBatchRdb0200 fail " + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_2300
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : Query data in Rdb
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    // it('SUB_DDM_DataShareBatchRdb0300', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL0, async (done: () => void): Promise<void> => {
    //   try {
    //     await pubfun.publicbatchInsert(globalThis.helper!, uri, BatchInsert).then((data) => {
    //       console.info("TestDataShare going batchInsert = " + data);
    //       globalThis.Count = globalThis.Count! + DataProcessResultFour;
    //       expect(DataProcessResultFour).assertEqual(data);
    //     })
    //     await pubfun.publicquery(globalThis.helper!, uri, PredicatesAll, ["*"]).then((data) => {
    //       console.info("TestDataShare going query = " + data);
    //       globalThis.ResultSet!.goToLastRow();
    //       expect(JSON.stringify(ResultSetUint8Array))
    //         .assertEqual(
    //           JSON.stringify(globalThis.ResultSet!.getBlob(globalThis.ResultSet!.getColumnIndex(IndexUint8Array))));
    //       globalThis.ResultSet!.close();
    //     }).catch((error) => {
    //       let err = error as BusinessError;
    //       console.error(`SUB_DDM_DataShareBatchRdb0300 fail err: code: ${err.code}, message: ${err.message}`);
    //       // console.info("SUB_DDM_DataShareBatchRdb0300 err " + JSON.stringify(err));
    //       expect(err).assertFail();
    //     })
    //     await pubfun.publicdelete(globalThis.helper!, uri, PredicatesAll).then((data) => {
    //       console.info("TestDataShare going delete = " + data);
    //       expect(DataProcessResultFour).assertEqual(data);
    //       done();
    //     })
    //   } catch (err) {
    //     console.info("SUB_DDM_DataShareBatchRdb0300 fail " + JSON.stringify(err));
    //     expect(err).assertFail();
    //     done();
    //   }
    // })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_2400
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : Delete data in Rdb
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_DDM_DataShareBatchRdb0400', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL0, async (done: () => void): Promise<void> => {
      try {
        await pubfun.publicbatchInsert(globalThis.helper!, uri, BatchInsert).then((data) => {
          console.info("TestDataShare going batchInsert = " + data);
          globalThis.Count = globalThis.Count! + DataProcessResultFour;
          expect(DataProcessResultFour).assertEqual(data);
        })
        await pubfun.publicdelete(globalThis.helper!, uri, PredicatesAll).then((data) => {
          console.info("TestDataShare going delete = " + data);
          expect(DataProcessResultFour).assertEqual(data);
          done();
        }).catch((error) => {
          let err = error as BusinessError;
          console.error(`SUB_DDM_DataShareBatchRdb0400 fail err: code: ${err.code}, message: ${err.message}`);
          expect(err).assertFail();
          done();
        })
      } catch (err) {
        console.info("SUB_DDM_DataShareBatchRdb0400 fail " + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_2600
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : Support NormalizeUri function
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    // it('SUB_DDM_DataShareNormalizeUri0100', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL0, async (done: () => void): Promise<void> => {
    //   try {
    //     await pubfun.publicnormalizeUri(globalThis.helper!, uri).then((data) => {
    //       console.info("TestDataShare going NormalizeUri = " + data);
    //       expect(Normalize).assertEqual(data);
    //       done();
    //     }).catch((error) => {
    //       let err = error as BusinessError;
    //       console.error(`SUB_DDM_DataShareNormalizeUri0100 fail err: code: ${err.code}, message: ${err.message}`);
    //       // console.info("SUB_DDM_DataShareNormalizeUri0100 err " + JSON.stringify(err));
    //       expect(err).assertFail();
    //       done();
    //     })
    //   } catch (err) {
    //     console.info("SUB_DDM_DataShareNormalizeUri0100 fail " + JSON.stringify(err));
    //     expect(err).assertFail();
    //     done();
    //   }
    // })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_2900
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : Support DenormalizeUri function
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    // it('SUB_DDM_DataShareDenormalizeUri0100', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL0, async (done: () => void): Promise<void> => {
    //   try {
    //     await pubfun.publicdenormalizeUri(globalThis.helper!, uri).then((data) => {
    //       console.info("TestDataShare going denormalizeUri = " + data);
    //       expect(Denormalize).assertEqual(data);
    //       done();
    //     }).catch((error) => {
    //       let err = error as BusinessError;
    //       console.error(`SUB_DDM_DataShareNormalizeUri0100 fail err: code: ${err.code}, message: ${err.message}`);
    //       // console.info("SUB_DDM_DataShareDenormalizeUri0100 err " + JSON.stringify(err));
    //       expect(err).assertFail();
    //       done();
    //     })
    //   } catch (err) {
    //     console.info("SUB_DDM_DataShareDenormalizeUri0100 fail " + JSON.stringify(err));
    //     expect(err).assertFail();
    //     done();
    //   }
    // })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_2500
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : Support On function
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_DDM_DataShareOnRdb0100', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL0, (): void => {
      try {
        globalThis.helper!.onDataChange(uri, onChangeNotify);
        console.info("TestDataShare going on");
        globalThis.helper!.offDataChange(uri);
        console.info("TestDataShare going off");
      } catch (err) {
        console.info("SUB_DDM_DataShareOnRdb0100 fail " + JSON.stringify(err));
        expect(err).assertFail();
      }
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_2800
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : Support Off function
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_DDM_DataShareOffRdb0100', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL0, (): void => {
      try {
        globalThis.helper!.offDataChange(uri, onChangeNotify);
        console.info("TestDataShare going off");
        globalThis.helper!.offDataChange(uri);
        console.info("TestDataShare going off");
      } catch (err) {
        console.info("SUB_DDM_DataShareOffRdb0100 fail " + JSON.stringify(err));
        let error = err as BusinessError;
        hilog.error(domain, tag, '%{public}s', ` SUB_DDM_DataShareOffRdb0100 error: code: ${error.code}, message: ${error.message}`);
        expect(err).assertFail();
      }
    })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_2700
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : Cannot on off operate
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 2
     */
    // it('SUB_DDM_DataShareTypeErr0100', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL2, (): void => {
    //   try {
    //     globalThis.helper!.on(null, uri, onChangeNotify);
    //     console.info("TestDataShare going on");
    //     globalThis.helper!.off(null, uri);
    //     console.info("TestDataShare going off");
    //   } catch (err) {
    //     console.info("SUB_DDM_DataShareTypeErr0100 fail" + JSON.stringify(err));
    //     expect(err).assertFail();
    //   }
    // })
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_3000
     * @tc.name      : Use getEntries get the value by mixing the string key
     * @tc.desc      : CreateDataShareHelperPromise
     * @tc.size      : MediumTest
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_DDM_CreateDataShareCallback0100', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL0, async (done: () => void): Promise<void> => {
      try {
        let data = await dataShare.createDataShareHelper(context, uri);
        console.info("TestDataShare CreateDataShareHelper = " + data);
        expect(true).assertEqual(data != undefined);
        await data.close();
        done();
      } catch(err: BusinessError) {
        hilog.error(domain, tag, '%{public}s', ` SUB_DDM_CreateDataSharePromise0100 error: code: ${err.code}, message: ${err.message}`);
        expect(err).assertFail();
        done();
      }
    })
 
  })
}