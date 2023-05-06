/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

import Ability from '@ohos.app.ability.UIAbility'
import dataShare from '@ohos.data.dataShare'
import dataSharePredicates from '@ohos.data.dataSharePredicates'
import rdb from '@ohos.data.rdb';

let cardUri = ("datashareproxy://com.acts.ohos.data.datasharetest/test");
let dsProxyHelper:dataShare.DataShareHelper;
let dbHelper:dataShare.DataShareHelper;
let DB_NAME = "DB00.db";
let TBL_NAME = "TBL00";
let DDL_TBL_CREATE = "CREATE TABLE IF NOT EXISTS "
+ TBL_NAME
+ " (id INTEGER PRIMARY KEY AUTOINCREMENT, name0 TEXT, name1 TEXT, name2 TEXT, name3 TEXT, name4 TEXT, name5 TEXT, name6 TEXT, " +
"name7 TEXT, name8 TEXT, name9 TEXT, time INTEGER, age INTEGER, phoneNumber DOUBLE, isStudent BOOLEAN)";
let rdbStore;

export default class MainAbility extends Ability {
    async onCreate(want, launchParam) {
        // Ability is creating, initialize resources for this ability
        console.log("[ttt] [DataShareTest] <<Consumer>> MainAbility onCreate")
        globalThis.abilityWant = want;
        globalThis.abilityContext = this.context;
        console.log('[ttt] [DataShareTest] <<Provider>> DataShareExtAbility onCreate, want:' + want.abilityName);
        console.log("[ttt] [DataShareTest] DataShareExtAbility onCreate this.context.databaseDir:" + this.context.databaseDir);

        rdbStore = await rdb.getRdbStore(this.context, { name: DB_NAME }, 1);
        console.log('[ttt] [DataShareTest] <<Provider>> DataShareExtAbility getRdbStore done');
        await rdbStore.executeSql(DDL_TBL_CREATE, []);
        console.log('[ttt] [DataShareTest] <<Provider>> DataShareExtAbility executeSql done');
    }

    onDestroy() {
        // Ability is destroying, release resources for this ability
        console.log("[ttt] [DataShareTest] <<Consumer>> MainAbility onDestroy")
    }

    onWindowStageCreate(windowStage) {
        // Main window is created, set main page for this ability
        console.log("[ttt] [DataShareTest] <<Consumer>> MainAbility onWindowStageCreate")
        globalThis.abilityContext = this.context;
        console.log("[ttt] [DataShareTest] <<Consumer>> MainAbility this.context.databaseDir:" + this.context.databaseDir);
        globalThis.connectDataShareExtAbility = (async () => {
            console.log("[ttt] [DataShareTest] <<Consumer>> connectDataShareExtAbility begin");
            // @ts-ignore
            dsProxyHelper = await dataShare.createDataShareHelper(this.context, cardUri, {isProxy : true});
        })

        globalThis.disconnectDataShareExtAbility = (async () => {
            console.log("[ttt] [DataShareTest] <<Consumer>> disconnectDataShareExtAbility begin");
            dsProxyHelper = null;
            console.log("[ttt] [DataShareTest] <<Consumer>> disconnectDataShareExtAbility end");
        })

        globalThis.insert = (async () => {
            console.log("[ttt] [DataShareTest] <<Consumer>> insert begin");
            if (dsProxyHelper == null) {
                console.log("[ttt] [DataShareTest] <<Consumer>> insert end, DSHelper is null");
                return;
            }

            let i = 0;
            let ret;
            for (i = 0; i < 2; i++) {
                let vb = {
                    "name0": "name0" + i,
                    "name1": "name0",
                    "name2": "name0",//"roe11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111roe11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111roe11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111roe11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111" + i,
                    "name3": "name0",//"roe11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111roe11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111roe11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111roe11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111" + i,
                    "name4": "name0",//"roe1122222222222222222222222222222222222222222222222222222222222222222222222222222222222222222roe11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111roe11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111roe11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111" + i,
                    "name5": "name0",//"roe112222222222222222222222222222222222222222222222222222222222222222222222222222222222222222roe11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111roe11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111roe11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111" + i,
                    "name6": "name0",//"roe112222222222222222222222222222222222222222222222222222222222222222222222222222222222222222roe11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111roe11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111roe11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111" + i,
                    "name7": "name0",//"roe112222222222222222222222222222222222222222222222222222222222222222222222222222222222222222roe11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111roe11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111roe11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111" + i,
                    "name8": "name0",//"roe112222222222222222222222222222222222222222222222222222222222222222222222222222222222222222roe11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111roe11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111roe11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111" + i,
                    "name9": "name0",//"roe112222222222222222222222222222222222222222222222222222222222222222222222222222222222222222roe11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111roe11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111roe11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111" + i
                    "time": 50
                };
                ret = await dsProxyHelper.insert(cardUri, vb);
            }
            return ret;
        })

        windowStage.setUIContent(this.context, "pages/index", null)
    }

    onWindowStageDestroy() {
        // Main window is destroyed, release UI related resources
        console.log("[ttt] [DataShareTest] <<Consumer>> MainAbility onWindowStageDestroy")
    }

    onForeground() {
        // Ability has brought to foreground
        console.log("[ttt] [DataShareTest] <<Consumer>> MainAbility onForeground")
    }

    onBackground() {
        // Ability has back to background
        console.log("[ttt] [DataShareTest] <<Consumer>> MainAbility onBackground")
    }
};