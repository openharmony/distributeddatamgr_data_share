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
import { BusinessError } from '@ohos.base';
import dataShare from '@ohos.data.dataShare'
import { ValuesBucket } from '@ohos.data.ValuesBucket';
import dataSharePredicates from '@ohos.data.dataSharePredicates';
import DataShareResultSet from '@ohos.data.DataShareResultSet';
import common from '@ohos.app.ability.common';
import { ValuesBucket } from "@ohos.data.ValuesBucket";
import { AppStorage } from '@ohos.arkui.stateManagement';
import Utils from './Util.test';
import abilityDelegatorRegistry from '@ohos.app.ability.abilityDelegatorRegistry';
import UIAbility from '@ohos.app.ability.UIAbility';
 
const TAG = "[ttt]";
let dataShareHelper: dataShare.DataShareHelper;
let context: common.UIAbilityContext;
let dseUri = "datashare:///com.acts.datasharekvencryptclient";
 
export default function DataSharePredicatesKvdbMetaService() {
  describe('DataSharePredicatesKvdbMetaService', () => {
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
      abilityDelegator.executeShellCommand("aa start -a EntryAbility -b com.acts.datasharekvencryptclient.static");
      await Utils.msSleep(2000);
 
      try {
        console.info(TAG + "before createDataShareHelper");
        dataShareHelper = await dataShare.createDataShareHelper(context, dseUri);
        await Utils.msSleep(500);
        if (dataShareHelper == undefined) {
          console.info(TAG + "createDataShareHelper failed");
        }
        console.info(TAG + "createDataShareHelper success")
      } catch (err: BusinessError) {
        console.error(TAG + "error. err.code:" + err.code + "err.message:" + err.message)
        expect().assertFail();
      }
    });
 
    beforeEach(async () => {
      console.info(TAG + "beforeEach");
    });
 
    afterEach(async () => {
      console.info(TAG + "afterEach");
    });
 
    afterAll(async () => {
      try {
        await dataShareHelper.close();
        console.info(TAG + "closeDataShareHelper success")
      } catch (err: BusinessError) {
        console.error(TAG + "error. err.code:" + err.code + "err.message:" + err.message)
        expect().assertFail();
      }
    });
 
    /**
     * @tc.name testDataShareKvEncryptNoSilent_0010
     * @tc.number SUB_DistributedData_DataShare_SDK_KVEncryptNoSilentAPITest_0010
     * @tc.desc insert、delete、update、query promise test
     * @tc.level: Level 2
     * @tc.type: Function
     * @tc.size: MediumTest
     */
    it('testDataShareKvEncryptNoSilent_0010', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL2, async () => {
      console.info(TAG + "testDataShareKvEncryptNoSilent_0010 start")
      let resultSet: DataShareResultSet | undefined;
      try {
        let vb: ValuesBucket = { "name": "zhangsan", "age": 18 as Long, "isStudent": true };
        let vb1: ValuesBucket = { "name": "lisi", "age": 18 as Long, "isStudent": true };
        let data = await dataShareHelper.insert(dseUri, vb);
        console.info(TAG + "insertData:" + data)
        let predicates = new dataSharePredicates.DataSharePredicates();
        predicates.equalTo("name", "zhangsan");
        resultSet= await dataShareHelper.query(dseUri, predicates, ["*"]);
        expect(resultSet.rowCount).assertEqual(1);
        resultSet.close();
        resultSet = undefined;
        let data2 = await dataShareHelper.update(dseUri, predicates, vb1)
        console.info(TAG + "update success, data2:" + data2)
        expect(data2).assertEqual(1);
        let predicates1 = new dataSharePredicates.DataSharePredicates();
        predicates1.inKeys(["testkey0"]);
        let res = await dataShareHelper.delete(dseUri, predicates1)
        expect(res).assertEqual(1)
        console.info(TAG + "delete success" + res)
      } catch (err: BusinessError) {
        console.error(TAG + "catch error. err.code:" + err.code + "err.message:" + err.message)
        expect().assertFail();
      }
      console.info(TAG + "testDataShareKvEncryptNoSilent_0010 end")
    })
 
    /**
     * @tc.name testDataShareKvEncryptNoSilent_0020
     * @tc.number SUB_DistributedData_DataShare_SDK_KVEncryptNoSilentAPITest_0020
     * @tc.desc insert、delete、update、query callback test
     * @tc.level: Level 2
     * @tc.type: Function
     * @tc.size: MediumTest
     */
    it('testDataShareKvEncryptNoSilent_0020', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL2, async (done: () => void) => {
      console.info(TAG + "testDataShareKvEncryptNoSilent_0020 start")
      try {
        let vb: ValuesBucket = { "name": "zhangsan", "age": 18 as Long, "isStudent": true };
        let vb1: ValuesBucket = { "name": "lisi", "age": 18 as Long, "isStudent": true };
        dataShareHelper.insert(dseUri, vb, (err: BusinessError | null, data: int | undefined) => {
          if (err) {
            console.error(TAG + "insert error. err.code:" + err.code + "err.message:" + err.message)
            expect().assertFail();
          } else {
            console.info(TAG + "insertData:" + data)
            let predicates = new dataSharePredicates.DataSharePredicates();
            predicates.equalTo("name", "zhangsan");
            dataShareHelper.query(dseUri, predicates, ["*"], (err: BusinessError | null, resultSet: DataShareResultSet | undefined) => {
              if (err) {
                console.error(TAG + "query error. err.code:" + err.code + "err.message:" + err.message)
                expect().assertFail();
              } else {
                expect(resultSet!.rowCount).assertEqual(1);
                resultSet!.close();
                resultSet = undefined;
                dataShareHelper.update(dseUri, predicates, vb1, (err, data2) => {
                  if (err) {
                    console.error(TAG + "update error. err.code:" + err.code + "err.message:" + err.message)
                    expect().assertFail();
                  } else {
                    console.info(TAG + "update success, data2:" + data2)
                    expect(data2).assertEqual(1);
                    let predicates1 = new dataSharePredicates.DataSharePredicates();
                    predicates1.inKeys(["testkey0"]);
                    dataShareHelper.delete(dseUri, predicates1, (err: BusinessError | null, res: int | undefined) => {
                      if (err) {
                        console.error(TAG + "delete error. err.code:" + err.code + "err.message:" + err.message)
                        expect().assertFail();
                      } else {
                        expect(res).assertEqual(1);
                        console.info(TAG + "delete success" + res);
                        console.info(TAG + "testDataShareKvEncryptNoSilent_0020 end");
                        done();
                      }
                    })
                  }
                })
              }
            })
          }
        })
      } catch (err: BusinessError) {
        console.error(TAG + "catch error. err.code:" + err.code + "err.message:" + err.message)
        expect().assertFail();
      }
    })
 
    /**
     * @tc.name testDataShareKvEncryptNoSilent_0030
     * @tc.number SUB_DistributedData_DataShare_SDK_KVEncryptNoSilentAPITest_0030
     * @tc.desc on('dataChange')、off('dataChange')、notifyChange test
     * @tc.level: Level 2
     * @tc.type: Function
     * @tc.size: MediumTest
     */
    it('testDataShareKvEncryptNoSilent_0030', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL2, async (done: () => void) => {
      console.info(TAG + "testDataShareKvEncryptNoSilent_0030 start")
      try {
        let vb: ValuesBucket = { "name": "zhangsan", "age": 18 as Long, "isStudent": true };
        let onCallback: () => void = (): void => {
          console.info(TAG + "**** Observer on callback ****");
          dataShareHelper!.offDataChange(dseUri);
          console.info(TAG + "testDataShareKvEncryptNoSilent_0030 end");
          done();
        }
        dataShareHelper!.onDataChange(dseUri, onCallback);
        let data = await dataShareHelper.insert(dseUri, vb);
        console.info(TAG + "insertData:" + data)
        let predicates1 = new dataSharePredicates.DataSharePredicates();
        predicates1.inKeys(["testkey0"]);
        let res = await dataShareHelper.delete(dseUri, predicates1)
        expect(res).assertEqual(1)
        console.info(TAG + "delete success" + res);
        await dataShareHelper.notifyChange(dseUri);
      } catch (err: BusinessError) {
        console.error(TAG + "catch error. err.code:" + err.code + "err.message:" + err.message)
        expect().assertFail();
      }
    })
 
    /**
     * @tc.name testDataShareKvEncryptNoSilent_0040
     * @tc.number SUB_DistributedData_DataShare_SDK_KVEncryptNoSilentAPITest_0040
     * @tc.desc on('dataChange')、off('dataChange')、notifyChange test
     * @tc.level: Level 2
     * @tc.type: Function
     * @tc.size: MediumTest
     */
    // it('testDataShareKvEncryptNoSilent_0040', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL2, async (done: () => void) => {
    //   console.info(TAG + "testDataShareKvEncryptNoSilent_0040 start")
    //   try {
    //     let vb: ValuesBucket = { "name": "zhangsan", "age": 18 as Long, "isStudent": true };
    //     let onCallback = (changeInfo: dataShare.ChangeInfo): void => {
    //       console.info(TAG + "**** Observer on callback ****");
    //       console.info(TAG + "**** Observer on callback ****" + JSON.stringify(changeInfo));
    //       expect(changeInfo.type).assertEqual(3);
    //       dataShareHelper!.offDataChange(dataShare.SubscriptionType.SUBSCRIPTION_TYPE_EXACT_URI, dseUri);
    //       console.info(TAG + "testDataShareKvEncryptNoSilent_0040 end");
    //       done();
    //     }
    //     dataShareHelper!.onDataChange(dataShare.SubscriptionType.SUBSCRIPTION_TYPE_EXACT_URI, dseUri, onCallback);
    //     let data = await dataShareHelper.insert(dseUri, vb);
    //     console.info(TAG + "insertData:" + data)
    //     let predicates1 = new dataSharePredicates.DataSharePredicates();
    //     predicates1.inKeys(["testkey0"]);
    //     let res = await dataShareHelper.delete(dseUri, predicates1)
    //     expect(res).assertEqual(1)
    //     console.info(TAG + "delete success" + res);
    //     dataShareHelper.notifyChange(dseUri, () => {
    //       console.info("***** notifyChange *****");
    //     });
    //   } catch (err: BusinessError) {
    //     console.error(TAG + "catch error. err.code:" + err.code + "err.message:" + err.message)
    //     expect().assertFail();
    //   }
    // })
 
    /**
     * @tc.name testDataShareKvEncryptNoSilent_0050
     * @tc.number SUB_DistributedData_DataShare_SDK_KVEncryptNoSilentAPITest_0050
     * @tc.desc on('dataChange')、off('dataChange')、notifyChange test
     * @tc.level: Level 2
     * @tc.type: Function
     * @tc.size: MediumTest
     */
    it('testDataShareKvEncryptNoSilent_0050', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL2, async (done: () => void) => {
      console.info(TAG + "testDataShareKvEncryptNoSilent_0050 start")
      try {
        let vb: ValuesBucket = { "name": "zhangsan", "age": 18 as Long, "isStudent": true };
        let onCallback = (changeInfo:dataShare.ChangeInfo): void => {
          console.info(TAG + "**** Observer on callback ****");
          console.info(TAG + "**** Observer on callback ****" + JSON.stringify(changeInfo));
          expect(changeInfo.type).assertEqual(2);
          let predicates1 = new dataSharePredicates.DataSharePredicates();
          predicates1.inKeys(["testkey0"]);
          dataShareHelper.delete(dseUri, predicates1, (err, res) => {
            expect(res).assertEqual(1)
            console.info(TAG + "delete success" + res);
            dataShareHelper!.offDataChange(dataShare.SubscriptionType.SUBSCRIPTION_TYPE_EXACT_URI, dseUri);
            console.info(TAG + "testDataShareKvEncryptNoSilent_0050 end");
            done();
          })
        }
        dataShareHelper!.onDataChange(dataShare.SubscriptionType.SUBSCRIPTION_TYPE_EXACT_URI, dseUri, onCallback);
        let data = await dataShareHelper.insert(dseUri, vb);
        console.info(TAG + "insertData:" + data);
        let people: Array<ValuesBucket> = new Array<ValuesBucket>(vb);
        let changeData:dataShare.ChangeInfo= { type:dataShare.ChangeType.UPDATE, uri:dseUri, values:people};
        await dataShareHelper!.notifyChange(changeData);
        console.info(TAG + "***** notifyChange *****");
      } catch (err: BusinessError) {
        console.error(TAG + "catch error. err.code:" + err.code + "err.message:" + err.message)
        expect().assertFail();
      }
    })
 
    /**
     * @tc.name testDataShareKvEncryptNoSilent_0060
     * @tc.number SUB_DistributedData_DataShare_SDK_KVEncryptNoSilentAPITest_0060
     * @tc.desc normalizeUri、denormalizeUri promise test
     * @tc.size MediumTest
     * @tc.type Function
     * @tc.level Level 1
     */
    it('testDataShareKvEncryptNoSilent_0060', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      console.info(TAG + "testDataShareKvEncryptNoSilent_0060 start");
      try {
        let data = await dataShareHelper!.normalizeUri(dseUri);
        console.info(TAG + "normalizeUri = " + JSON.stringify(data));
        expect(data).assertEqual("normalize+" + dseUri)
        let data1 = await dataShareHelper!.denormalizeUri(dseUri);
        console.info(TAG + "denormalizeUri = " + JSON.stringify(data1));
        expect(data1).assertEqual("denormalize+" + dseUri)
      } catch (err: BusinessError) {
        console.error(TAG+ 'testDataShareKvEncryptNoSilent_0060 error' + err.code + "  " + err.message);
        expect().assertFail();
      }
      console.info(TAG + "testDataShareKvEncryptNoSilent_0060 end");
    });
 
    /**
     * @tc.name testDataShareKvEncryptNoSilent_0070
     * @tc.number SUB_DistributedData_DataShare_SDK_KVEncryptNoSilentAPITest_0070
     * @tc.desc normalizeUri、denormalizeUri callback test
     * @tc.size MediumTest
     * @tc.type Function
     * @tc.level Level 1
     */
    it('testDataShareKvEncryptNoSilent_0070', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void) => {
      console.info(TAG + "testDataShareKvEncryptNoSilent_0070 start");
      try {
        dataShareHelper?.normalizeUri(dseUri, (err: BusinessError | null, data: string | undefined) => {
          console.info(TAG + "normalizeUri = " + JSON.stringify(data));
          expect(data).assertEqual("normalize+" + dseUri)
          dataShareHelper?.denormalizeUri(dseUri, (err, data1) => {
            console.info(TAG + "denormalizeUri = " + JSON.stringify(data1));
            expect(data1).assertEqual("denormalize+" + dseUri)
            console.info(TAG + "testDataShareKvEncryptNoSilent_0070 end");
            done();
          })
        })
      } catch (err: BusinessError) {
        console.error(TAG+ 'testDataShareKvEncryptNoSilent_0070 error' + err.code + "  " + err.message);
        expect().assertFail();
      }
    });
 
    /**
     * @tc.name testDataShareKvEncryptNoSilent_0080
     * @tc.number SUB_DistributedData_DataShare_SDK_KVEncryptNoSilentAPITest_0080
     * @tc.desc batchInsert promise test
     * @tc.size MediumTest
     * @tc.type Function
     * @tc.level Level 1
     */
    it('testDataShareKvEncryptNoSilent_0080', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async () => {
      console.info(TAG + "testDataShareKvEncryptNoSilent_0080 start");
      try {
        let vbs: ValuesBucket[] = [
          { "name": "zhangsan", "age": 18 as Long, "isStudent": true },
          { "name": "zhangsan", "age": 17 as Long, "isStudent": true },
          { "name": "wangwu", "age": 18 as Long, "isStudent": true }
        ];
        let data = await dataShareHelper.batchInsert(dseUri, vbs);
        console.info(TAG + "batchInsertData:" + data)
        expect(data).assertEqual(1);
        let predicates1 = new dataSharePredicates.DataSharePredicates();
        predicates1.inKeys(["testkey0", "testkey1", "testkey2"]);
        let res = await dataShareHelper.delete(dseUri, predicates1)
        console.info(TAG + "delete success" + res);
        expect(res).assertEqual(1)
      }catch (err: BusinessError) {
        console.error(TAG+ 'testDataShareKvEncryptNoSilent_0080 error' + err.code + "  " + err.message);
        expect().assertFail();
      }
      console.info(TAG + "testDataShareKvEncryptNoSilent_0080 end");
    });
 
    /**
     * @tc.name testDataShareKvEncryptNoSilent_0090
     * @tc.number SUB_DistributedData_DataShare_SDK_KVEncryptNoSilentAPITest_0090
     * @tc.desc batchInsert promise test
     * @tc.size MediumTest
     * @tc.type Function
     * @tc.level Level 1
     */
    it('testDataShareKvEncryptNoSilent_0090', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void) => {
      console.info(TAG + "testDataShareKvEncryptNoSilent_0090 start");
      try {
        let vbs: ValuesBucket[] = [
          { "name": "zhangsan", "age": 18 as Long, "isStudent": true },
          { "name": "zhangsan", "age": 17 as Long, "isStudent": true },
          { "name": "wangwu", "age": 18 as Long, "isStudent": true }
        ];
        dataShareHelper.batchInsert(dseUri, vbs, (err: BusinessError | null, data: int | undefined) => {
          if (err) {
            console.error(TAG+ 'batchInsert error' + err.code + "  " + err.message);
            expect().assertFail();
          } else {
            console.info(TAG + "batchInsertData:" + data)
            expect(data).assertEqual(1);
            let predicates1 = new dataSharePredicates.DataSharePredicates();
            predicates1.inKeys(["testkey0", "testkey1", "testkey2"]);
            dataShareHelper.delete(dseUri, predicates1, (err: BusinessError | null, res: int | undefined) => {
              if (err) {
                console.error(TAG+ 'delete error' + err.code + "  " + err.message);
                expect().assertFail();
                done();
              } else {
                console.info(TAG + "delete success" + res);
                expect(res).assertEqual(1);
                console.info(TAG + "testDataShareKvEncryptNoSilent_0090 end");
                done();
              }
            })
          }
        })
      } catch (err: BusinessError) {
        console.error(TAG+ 'testDataShareKvEncryptNoSilent_0090 error' + err.code + "  " + err.message);
        expect().assertFail();
        done();
      }
    });
  })
}