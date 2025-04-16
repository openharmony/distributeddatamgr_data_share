/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not \tuse this file except in compliance with the License.
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

const STORE_CONFIG = {
    name: "DB00.db",
    securityLevel: rdb.SecurityLevel.S1,
}
let TBL_NAME = "TBL00";
let DROP_TBL = "DROP TABLE IF EXISTS " + TBL_NAME;
let DDL_TBL_CREATE = "CREATE TABLE IF NOT EXISTS "
+ TBL_NAME
+ " (id INTEGER PRIMARY KEY AUTOINCREMENT, name0 TEXT, name1 TEXT, name2 TEXT, name3 TEXT, name4 TEXT, name5 TEXT, name6 TEXT, " +
"name7 TEXT, name8 TEXT, name9 TEXT, age INTEGER, phoneNumber DOUBLE, isStudent BOOLEAN, Binary BINARY)";

let rdbStore;

export default class DataShareExtAbility extends Extension {

    async onCreate(want, callback) {
        console.log("[ttt] [DataShareTest] DataShareExtAbility onCreate this.context.databaseDir:" + this.context.databaseDir);
        rdbStore = await rdb.getRdbStore(this.context.getApplicationContext(), STORE_CONFIG);
        await rdbStore.executeSql(DROP_TBL, []);
        await rdbStore.executeSql(DDL_TBL_CREATE, []);
        let err = {"code":0};
        callback(err);
    }
};