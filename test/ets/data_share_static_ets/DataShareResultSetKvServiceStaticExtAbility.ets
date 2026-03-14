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
 
import DataShareExtensionAbility from '@ohos.application.DataShareExtensionAbility'
import distributedKVStore from '@ohos.data.distributedKVStore';
import dataSharePredicates from '@ohos.data.dataSharePredicates';
// import fileIo from '@ohos.fileio';
import dataShare from '@ohos.data.dataShare';
import Want from '@ohos.app.ability.Want';
import { AsyncCallback } from '@ohos.base';
import { BusinessError } from '@ohos.base';
import { ValuesBucket } from '@ohos.data.ValuesBucket';
 
let kvManager: distributedKVStore.KVManager;
let kvdbStore: distributedKVStore.SingleKVStore;
let kvResultSet: distributedKVStore.KVStoreResultSet;
let result: string;
let resultSetCnt = 0;
const BUNDLE_NAME = 'com.acts.datasharetest';
const STORE_ID = 'storeId';
 
export default class DataShareExtAbility extends DataShareExtensionAbility {
  // private rdbStore_;
  // private kvdbStore_;
  onCreateTest(want: Want, callback: AsyncCallback<void>): void {
    console.info('[ttt] [DataShareTest] <<Provider>> DataShareExtAbility onCreate, want:' + want.abilityName);
    let mKVMgrConfig: distributedKVStore.KVManagerConfig = {
      bundleName: BUNDLE_NAME,
      context: this.context
    }
    let options: distributedKVStore.Options = {
      createIfMissing : true,
      encrypt : false,
      backup : false,
      autoSync : true,
      kvStoreType : distributedKVStore.KVStoreType.SINGLE_VERSION,
      schema : {},
      securityLevel : distributedKVStore.SecurityLevel.S1
    }
    let name: distributedKVStore.FieldNode = new distributedKVStore.FieldNode('name');
    name.type = distributedKVStore.ValueType.STRING;
    name.nullable = false;
    name.defaultValue = 'name';
 
    let age: distributedKVStore.FieldNode = new distributedKVStore.FieldNode('age');
    age.type = distributedKVStore.ValueType.LONG;
    age.nullable = false;
    age.defaultValue = '0';
 
    let isStudent = new distributedKVStore.FieldNode('isStudent');
    isStudent.type = distributedKVStore.ValueType.STRING;
    isStudent.nullable = false;
    isStudent.defaultValue = 'false';
 
    let schema1 = new distributedKVStore.Schema();
    schema1.root.appendChild(name);
    schema1.root.appendChild(age);
    schema1.root.appendChild(isStudent);
    schema1.indexes = ['$.name', "$.age", "$.isStudent"];
    options.schema = schema1;
 
    distributedKVStore.createKVManager(mKVMgrConfig);
    console.info("[ttt] [DataShareTest] <<Provider>> createKVManager success");
    kvManager =  distributedKVStore.createKVManager(mKVMgrConfig);
    console.info('[ttt] [DataShareTest] <<Provider>> createKVManager kvManager = ' + kvManager);
    kvManager.getKVStore(STORE_ID, options, (err: BusinessError | null, store: distributedKVStore.SingleKVStore | undefined) => {
      if (err) {
        console.info("[ttt] [DataShareTest] <<Provider>> getKVStore err: "  + err);
        return;
      }
      console.info("[ttt] [DataShareTest] <<Provider>> getKVStore success");
      kvdbStore = store as distributedKVStore.SingleKVStore;
      console.info('[ttt] [DataShareTest] <<Provider>> createKVManager kvdbStore = ' + kvdbStore);
    });
    result = this.context.cacheDir + '/datashare.txt'
    console.info('[ttt] [DataAbilityTest] <<Provider>> resultpath = ' + result);
  }
 
  // getFileTypes(uri: string, mimeTypeFilter: string, callback) {
  //   console.info('[ttt] [DataShareTest] <<Provider>> [getFileTypes] enter');
  //   let ret = new Array("type01", "type02", "type03");
  //   console.info('[ttt] [DataShareTest] <<Provider>> [getFileTypes] leave, ret:' + ret);
  //   let err = {"code":2};
  //   callback(err,ret);
  //   return ret;
  // }
 
  insertTest(uri: string, value: ValuesBucket, callback: AsyncCallback<int>): void {
    console.info('[ttt] [DataShareTest] <<Provider>> [insert] enterkvdb');
    if (value == null) {
      console.info('[ttt] [DataShareTest] <<Provider>> [insert] invalid valueBuckets');
      return;
    }
    console.info('[ttt] [DataShareTest] <<Provider>> [insert] kvdb value = ' + JSON.stringify(value));
    let vbArr: Array<ValuesBucket> = new Array<ValuesBucket>();
    let insertBucket: ValuesBucket = {"name": value["name"]!, "age": value["age"]!, "isStudent":String(value["isStudent"])}
    console.info('[ttt] [DataShareTest] <<Provider>> [insert] kvdb insertBucket = ' + JSON.stringify(insertBucket));
    // vbArr.push({"key": value.at(0), "value": JSON.stringify(insertBucket)});
    vbArr.push(value);
    console.info('[ttt] [DataShareTest] <<Provider>> [insert] kvdb vbArr = ' + JSON.stringify(vbArr) + "?????????????");
    await kvdbStore.putValuesBuckets(vbArr).then(async (err: Error | undefined) => {
      if (err != undefined) {
        console.info("[ttt] [DataShareTest] <<Provider>> [insert kvdb put] callback err: " + err);
        if (callback != undefined) {
          callback({'code': -1}, -1);
        }
      } else {
        if (callback != undefined) {
          callback({'code': 0}, 1);
        }
      }
    });
    console.info('[ttt] [DataShareTest] <<Provider>> [insert] leave');
  }
 
  updateTest(uri: string, predicates: dataSharePredicates.DataSharePredicates, value: ValuesBucket, callback: AsyncCallback<int>): void {
    console.info('[ttt] [DataShareTest] <<Provider>> [update] enter');
    console.info('[ttt] [DataShareTest] <<Provider>> [update] value = ' + JSON.stringify(value));
    console.info('[ttt] [DataShareTest] <<Provider>> [update] predicates = ' + predicates);
    if (predicates == null || predicates == undefined) {
      console.info('[ttt] [DataShareTest] <<Provider>> [update] invalid predicates');
      return;
    }
    let vbArr: Array<ValuesBucket> = new Array<ValuesBucket>();
    let keyName = "testkey0";
    let updateBucket: ValuesBucket = {"name":value["name"]!, "age":value["age"]!, "isStudent":String(value["isStudent"])};
    console.info('[ttt] [DataShareTest] <<Provider>> [update] updateBucket = ' + JSON.stringify(updateBucket));
    console.info('[ttt] [DataShareTest] <<Provider>> [update] keyName = ' + keyName);
    vbArr.push({"key": keyName, "value": JSON.stringify(updateBucket)});
    console.info('[ttt] [DataShareTest] <<Provider>> [update] vbArr = ' + JSON.stringify(vbArr));
    await kvdbStore.putValuesBuckets(vbArr).then(async (err: Error | undefined) => {
      if (err != undefined) {
        console.info("[ttt] [DataShareTest] <<Provider>> [update kvdb put] callback err: " + JSON.stringify(err));
        if (callback != undefined) {
          callback({'code': -1}, -1);
        }
      } else {
        if (callback != undefined) {
          callback({'code': 0}, 1);
        }
      }
    });
    console.info('[ttt] [DataShareTest] <<Provider>> [update] leave');
  }
 
  deleteTest(uri: string, predicates: dataSharePredicates.DataSharePredicates, callback: AsyncCallback<int>): void {
    console.info('[ttt] [DataShareTest] <<Provider>> [delete] enter');
    if (predicates == null || predicates == undefined) {
      console.info('[ttt] [DataShareTest] <<Provider>> [delete] invalid predicates');
      return;
    }
    await kvdbStore.delete(predicates).then(() => {
      console.info("[ttt] [DataShareTest] <<Provider>> [delete kvdb] callback data");
      if (callback != undefined) {
        callback({'code': -1}, 1);
      }
    }).catch((err) => {
      console.info("[ttt] [DataShareTest] <<Provider>> [delete kvdb] callback err: " + err);
      if (callback != undefined) {
        callback({'code': 0}, -1);
      }
    });
    console.info('[ttt] [DataShareTest] <<Provider>> [delete] leave');
  }
 
  queryTest(uri: string, predicates: dataSharePredicates.DataSharePredicates, columns: Array<string>, callback: AsyncCallback<distributedKVStore.KVStoreResultSet>): void {
    console.info('[ttt] [DataShareTest] <<Provider>> [query] enter');
    if (predicates == null || predicates == undefined) {
      console.info('[ttt] [DataShareTest] <<Provider>> [query] invalid predicates');
    }
    console.info("[ttt] [DataShareTest] <<Provider>> [query] getDADADADADADADA!!!");
    if (resultSetCnt > 0) {
      await kvdbStore.closeResultSet(kvResultSet).then(() => {
        console.info("[ttt] [DataShareTest] <<Provider>> [query kvdb closeResultSet] success: ");
        --resultSetCnt;
      }).catch((err) => {
        console.info("[ttt] [DataShareTest] <<Provider>> [query kvdb closeResultSet] callback err: " + err);
      })
    }
    console.info("[ttt] [DataShareTest] <<Provider>> [RPF66666] ");
    kvdbStore.getResultSet(predicates).then((data: distributedKVStore.KVStoreResultSet) => {
      kvResultSet = data;
      console.info("[ttt] [DataShareTest] <<Provider>> [query kvdb get] getCount = " + kvResultSet.getCount())
      console.info("[ttt] [DataShareTest] <<Provider>> [query kvdb get] callback data:" + JSON.stringify(data));
      ++resultSetCnt;
      if (callback != undefined) {
        callback({'code': 0}, data);
      }
    }).catch((err) => {
      console.info("[ttt] [DataShareTest] <<Provider>> [query kvdb get] callback err: " + err);
      callback(err, undefined);
    })
    console.info('[ttt] [DataShareTest] <<Provider>> [query] leave');
  }
 
  // getType(uri: string,callback) {
  //   console.info('[ttt] [DataShareTest] <<Provider>> [getType] enter');
  //   let ret = "image";
  //   console.info('[ttt] [DataShareTest] <<Provider>> [getType] leave, ret:' + ret);
  //   let err = {"code":2};
  //   callback(err,ret);
  //   return ret;
  // }
 
  batchInsertTest(uri: string, valueBuckets: Array<ValuesBucket>, callback: AsyncCallback<int>): void {
    console.info('[ttt] [DataShareTest] <<Provider>> [batchInsert] enter');
    if (valueBuckets == null || valueBuckets.length == undefined) {
      console.info('[ttt] [DataShareTest] <<Provider>> [batchInsert] invalid valueBuckets');
      return;
    }
    console.info('[ttt] [DataShareTest] <<Provider>> [batchInsert] valueBuckets.length:' + valueBuckets.length);
    console.info('[ttt] [DataShareTest] <<Provider>> [batchInsert] valueBuckets.isStudent :' + JSON.stringify(valueBuckets[0]["isStudent"]));
    let vbArr: Array<ValuesBucket> = new Array<ValuesBucket>();
    let batchInsertBucket: Array<ValuesBucket> = new Array<ValuesBucket>();
    let num = 0;
    for (let i = 0; i < valueBuckets.length; i++) {
      console.info('[ttt] [DataShareTest] <<Provider>> [batchInsert] valueBuckets.boolean :' + JSON.stringify(valueBuckets[i]["isStudent"]));
      batchInsertBucket.push({"name": valueBuckets[i]["name"]!, "age": valueBuckets[i]["age"]!, "isStudent":String(valueBuckets[i]["isStudent"]!)});
      vbArr.push({"key": "testkey" + num, "value": JSON.stringify(batchInsertBucket[i])});
      num++;
    }
    console.info('[ttt] [DataShareTest] <<Provider>> [batchInsert] valueBuckets :' + JSON.stringify(batchInsertBucket));
    await kvdbStore.putValuesBuckets(vbArr).then(async (err: Error | undefined) => {
      if (err != undefined) {
        console.info("[ttt] [DataShareTest] <<Provider>> [batchInsert kvdb] callback err: " + err);
        if (callback != undefined) {
          callback({'code': -1}, -1);
        }
        // return;
      } else {
        if (callback != undefined) {
          callback({'code': 0}, 1);
        }
      }
    });
    console.info('[ttt] [DataShareTest] <<Provider>> [batchInsert] leave');
  }
 
  batchUpdateTest(operations: Record<string, Array<dataShare.UpdateOperation>>, callback: AsyncCallback<Record<string, Array<int>>>): void {
    let recordOps : Record<string, Array<dataShare.UpdateOperation>> = operations;
    let results : Record<string, Array<int>> = {};
    let a = Object.entries(recordOps);
    let num = 0;
    // for (let i = 0; i < a.length; i++) {
    //   let values = a[i][1];
    //   let vbArr = new Array();
    //   for (const value of values) {
    //     console.info('[ttt] [DataShareTest] <<Provider>> [batchUpdate] value: '+JSON.stringify(value.values));
    //     vbArr.push({
    //       "key": "testkey" + num,
    //       "value": JSON.stringify(value.values)
    //     });
    //     num++;
    //   }
    //   await kvdbStore.putBatch(vbArr);
    // }
    callback({'code': -1}, results);
  }
 
  // openFile(uri: string, mode: string, callback) {
  //   console.info('[ttt] [DataAbilityTest] <<Provider>> DataAbility openFile in');
  //   fileIo.open(result, 0o2 | 0o100, 0o666, function(err, fd) {
  //     if(err) {
  //       console.info('[ttt] [DataAbilityTest] <<Provider>> OpenFile err = ' + err);
  //       console.info('[ttt] [DataAbilityTest] <<Provider>> OpenFile err = ' + JSON.stringify(err));
  //     }
  //     console.log('[ttt] [DataAbilityTest] <<Provider>> EtsFileIo fd =' + fd);
  //     let num = fileIo.writeSync(fd, "this is a boring test of DataShare");
  //     console.info("[ttt] [DataAbillityTest] <<Provider>> WriteSync length = " + num);
  //     console.info("[ttt] [DataAbillityTest] <<Provider>> WriteSync fd = " + fd);
  //     callback(err,fd);
  //   })
  // }
 
  normalizeUriTest(uri: string, callback: AsyncCallback<string>): void {
    console.info('[ttt] [DataShareTest] <<Provider>> [normalizeUri] enter');
    let ret = "normalize+" + uri;
    console.info('[ttt] [DataShareTest] <<Provider>> [normalizeUri] leave, ret:' + ret);
    let err: BusinessError = {"code": 2};
    console.info('[ttt] [DataShareTest] <<Provider>> [normalizeUri] enter!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!');
    callback(err, ret);
    console.info('[ttt] [DataShareTest] <<Provider>> [normalizeUri] leave!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!');
  }
 
  denormalizeUriTest(uri: string, callback: AsyncCallback<string>): void {
    console.info('[ttt] [DataShareTest] <<Provider>> [denormalizeUri] enter');
    let ret = "denormalize+" + uri;
    console.info('[ttt] [DataShareTest] <<Provider>> [denormalizeUri] leave, ret:' + ret);
    let err: BusinessError = {"code": 2};
    callback(err, ret);
    console.info('[ttt] [DataShareTest] <<Provider>> [denormalizeUri] leave!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!');
  }
 
  constructor()
  {
    this.onCreate = this.onCreateTest;
    this.insert = this.insertTest;
    this.update = this.updateTest;
    this.delete = this.deleteTest;
    this.query = this.queryTest;
    this.batchInsert = this.batchInsertTest;
    this.normalizeUri = this.normalizeUriTest;
    this.denormalizeUri = this.denormalizeUriTest;
    console.log(`[ttt] <<Provider>> DataShareExtAbility constructor end.`);
  }
};