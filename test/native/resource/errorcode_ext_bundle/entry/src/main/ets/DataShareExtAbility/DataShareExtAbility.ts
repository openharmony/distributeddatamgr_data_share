/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import Extension from '@ohos.application.DataShareExtensionAbility'
import rdb from '@ohos.data.relationalStore';
import rpc from '@ohos.rpc';

let DB_NAME = "DB00.db";
let TBL_NAME = "TBL00";
let DDL_TBL_CREATE = "CREATE TABLE IF NOT EXISTS "
+ TBL_NAME
+ " (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT, age INTEGER, phoneNumber DOUBLE, isStudent BOOLEAN, Binary BINARY)";

let rdbStore;

export default class
extends Extension {
    async onCreate(want, callback) {
        console.log('[ttt] [DataShareTest] DataShareExtAbility onCreate, want:' + want.abilityName);
        console.log("[ttt] [DataShareTest] DataShareExtAbility onCreate this.context.databaseDir:" + this.context.databaseDir);
        // @ts-ignore
        rdbStore = await rdb.getRdbStore(this.context, { name: DB_NAME, securityLevel: rdb.SecurityLevel.S1 });
        console.log('[ttt] [DataShareTest] DataShareExtAbility getRdbStore done');
        await rdbStore.executeSql(DDL_TBL_CREATE, []);
        console.log('[ttt] [DataShareTest] DataShareExtAbility executeSql done');
        let err = {"code":0};
        callback(err);
        console.log('[ttt] [DataShareTest] DataShareExtAbility onCreate end');
    }

    async getFileTypes(uri: string, mimeTypeFilter: string, callback) {
        console.info('[ttt] [DataShareTest] [getFileTypes] enter');
        let ret = new Array("type01", "type00", "type03");
        console.info('[ttt] [DataShareTest] [getFileTypes] leave, ret:' + ret);
        let err = {"code":0};
        await callback(err,ret);
        return ret;
    }

    async openFile(uri: string, mode: string, callback) {
        console.info('[ttt] [DataShareTest] [openFile] enter');
        let ret = 12345;
        let err = {"code":0};
        await callback(err,ret);
        console.info('[ttt] [DataShareTest] [openFile] leave, ret:' + ret);
    }

    async insert(uri, value, callback) {
        console.info('[ttt] [DataShareTest] [insert] enter');
        if (value == null) {
            console.info('[ttt] [DataShareTest] [insert] invalid valueBuckets');
            return;
        }

        console.info('[ttt] [DataShareTest] [insert] getCallingTokenId:' + rpc.IPCSkeleton.getCallingTokenId());
        console.info('[ttt] [DataShareTest] [insert]  value = ' + value);
        console.info('[ttt] [DataShareTest] [insert]  value = ' + JSON.stringify(value));
        await rdbStore.insert(TBL_NAME, value, function (err, ret) {
            console.info('[ttt] [DataShareTest] [insert] callback ret:' + ret);

            if (callback != undefined) {
                callback(err, ret);
            }
        });
        console.info('[ttt] [DataShareTest] [insert] leave');
    }

    async update(uri, predicates, value, callback) {
        console.info('[ttt] [DataShareTest] [update] enter');
        if (predicates == null || predicates == undefined) {
            console.info('[ttt] [DataShareTest]  [update] invalid predicates');
            return;
        }
        console.info('[ttt] [DataShareTest] [update]  values = ' + value);
        console.info('[ttt] [DataShareTest] [update]  values = ' + JSON.stringify(value));
        console.info('[ttt] [DataShareTest] [update]  predicates = ' + predicates);
        console.info('[ttt] [DataShareTest] [update]  predicates = ' + JSON.stringify(predicates));
        try {
            await rdbStore.update(TBL_NAME,value, predicates, function (err, ret) {
                console.info('[ttt] [DataShareTest] [update] callback ret:' + ret);
                console.info('[ttt] [DataShareTest] [update] callback err:' + err);
                if (callback != undefined) {
                    callback(err, ret);
                }
            });
        } catch (err) {
            console.error('[ttt] [DataShareTest] [update] error' + err);
        }
        console.info('[ttt] [DataShareTest] [update] leave');
    }

    async delete(uri, predicates, callback) {
        console.info('[ttt] [DataShareTest] [delete] enter');
        if (predicates == null || predicates == undefined) {
            console.info('[ttt] [DataShareTest] [delete] invalid predicates');
            return;
        }
        console.info('[ttt] [DataShareTest] [delete]  predicates = ' + predicates);
        console.info('[ttt] [DataShareTest] [delete]  predicates = ' + JSON.stringify(predicates));
        try {
            await rdbStore.delete(TBL_NAME,predicates, function (err, ret) {
                console.info('[ttt] [DataShareTest] [delete] ret:' + ret);
                if (callback != undefined) {
                    callback(err, ret);
                }
            });
        } catch (err) {
            console.error('[ttt] [DataShareTest] [delete] error' + err);
        }
        console.info('[ttt] [DataShareTest] [delete] leave');
    }

    async query(uri, predicates, columns,  callback) {
        if (predicates == null || predicates == undefined) {
            console.info('[ttt] [DataShareTest] [query] invalid predicates');
        }
        try {
            console.info('[ttt] [DataShareTest] [query] for errorcode test, sissing parameter: TBL_NAME ');
            await rdbStore.query(columns, function (err, resultSet) {
                console.info('[ttt] [DataShareTest] [query] ret: ' + resultSet);
                if (resultSet != undefined) {
                    console.info('[ttt] [DataShareTest] [query] resultSet.rowCount: ' + resultSet.rowCount);
                }
                if (callback != undefined) {
                    callback(err, resultSet);
                }
            });
        } catch (err) {
            console.error(`[ttt] [DataShareTest] [query] error: code: ${err.code}, message: ${err.message} `);
            callback(err, undefined);
        }
        console.info('[ttt] [DataShareTest] [query] leave');
    }

    async getType(uri: string,callback) {
        console.info('[ttt] [DataShareTest] [getType] enter');
        let ret = "image";
        console.info('[ttt] [DataShareTest] [getType] leave, ret:' + ret);
        let err = {"code":0};
        await callback(err,ret);
        return ret;
    }

    async batchInsert(uri: string, valueBuckets, callback) {
        console.info('[ttt] [DataShareTest] [batchInsert] enter');
        if (valueBuckets == null || valueBuckets.length == undefined) {
            console.info('[ttt] [DataShareTest] [batchInsert] invalid valueBuckets');
            return;
        }
        console.info('[ttt] [DataShareTest] [batchInsert] valueBuckets.length:' + valueBuckets.length);
        let resultNum = valueBuckets.length
        await rdbStore.batchinsert(TBL_NAME, valueBuckets, function (err, ret) {
            console.info('[ttt] [DataShareTest] [batchInsert] callback ret:' + ret);
            if (callback != undefined) {
                callback(err, ret);
            }
        });

        console.info('[ttt] [DataShareTest] [batchInsert] leave');
    }

    async normalizeUri(uri: string, callback) {
        console.info('[ttt] [DataShareTest] [normalizeUri] enter');
        let ret = "normalize+" + uri;
        let err = {"code":0};
        await callback(err, ret);
        console.info('[ttt] [DataShareTest] [normalizeUri] leave, ret:' + ret);
    }

    async denormalizeUri(uri: string, callback) {
        console.info('[ttt] [DataShareTest] [denormalizeUri] enter');
        let ret = "denormalize+" + uri;
        let err = {"code":0};
        await callback(err, ret);
        console.info('[ttt] [DataShareTest] [denormalizeUri] leave, ret:' + ret);
    }
};