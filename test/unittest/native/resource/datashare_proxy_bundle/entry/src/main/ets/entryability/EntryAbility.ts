/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

import { AbilityConstant, ConfigurationConstant, UIAbility, Want } from '@kit.AbilityKit';
import { hilog } from '@kit.PerformanceAnalysisKit';
import { window } from '@kit.ArkUI';
import Ability from '@ohos.app.ability.UIAbility'
import dataShare from '@ohos.data.dataShare'
import dataSharePredicates from '@ohos.data.dataSharePredicates'
import rdb from '@ohos.data.rdb';
const DOMAIN = 0x0000;
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

export default class EntryAbility extends UIAbility {
  async onCreate(want: Want, launchParam: AbilityConstant.LaunchParam) {
    // Ability is creating, initialize resources for this ability
    console.log("[ttt] [datashareproxyTest] <<Consumer>> MainAbility onCreate")
    globalThis.abilityWant = want;
    globalThis.abilityContext = this.context;
    console.log('[ttt] [DataShareTest] DataShareExtAbility onCreate, want:' + want.abilityName);
    console.log("[ttt] [DataShareTest] DataShareExtAbility onCreate this.context.databaseDir:" + this.context.databaseDir);

    rdbStore = await rdb.getRdbStore(this.context, { name: DB_NAME }, 1);
    console.log('[ttt] [DataShareTest] DataShareExtAbility getRdbStore done');
    await rdbStore.executeSql(DDL_TBL_CREATE, []);
    console.log('[ttt] [DataShareTest] DataShareExtAbility executeSql done');
  }

  onDestroy(): void {
    hilog.info(DOMAIN, 'testTag', '%{public}s', 'Ability onDestroy');
  }

  onWindowStageCreate(windowStage: window.WindowStage): void {
    // Main window is created, set main page for this ability
    console.log("[ttt] [datashareproxyTest] <<Consumer>> MainAbility onWindowStageCreate")
    globalThis.abilityContext = this.context;
    console.log("[ttt] [datashareproxyTest] <<Consumer>> MainAbility this.context.databaseDir:" + this.context.databaseDir);
    globalThis.connectDataShareExtAbility = (async () => {
      console.log("[ttt] [datashareproxyTest] <<Consumer>> connectDataShareExtAbility begin");
      dsProxyHelper = await dataShare.createDataShareHelper(this.context, cardUri, {isProxy : true});
    })

    globalThis.disconnectDataShareExtAbility = (async () => {
      console.log("[ttt] [datashareproxyTest] <<Consumer>> disconnectDataShareExtAbility begin");
      dsProxyHelper = null;
      console.log("[ttt] [datashareproxyTest] <<Consumer>> disconnectDataShareExtAbility end");
    })

    globalThis.insert = (async () => {
      console.log("[ttt] [datashareproxyTest] <<Consumer>> insert begin");
      if (dsProxyHelper == null) {
        console.log("[ttt] [datashareproxyTest] <<Consumer>> insert end, DSHelper is null");
        return;
      }

      let i = 0;
      let ret;
      for (i = 0; i < 2; i++) {
        let vb = {
          "name0": "name0" + i,
          "name1": "name0",
          "name2": "name0",
          "name3": "name0",
          "name4": "name0",
          "name5": "name0",
          "name6": "name0",
          "name7": "name0",
          "name8": "name0",
          "name9": "name0",
          "time": 50
        };
        ret = await dsProxyHelper.insert(cardUri, vb);
      }
      return ret;
    })
    windowStage.loadContent("pages/index", (err, data) => {
      if (err.code) {
        console.error('Failed to load the content. Cause:' + JSON.stringify(err));
        return;
      }
      console.info('Succeeded in loading the content. Data: ' + JSON.stringify(data))
    });
  }

  onWindowStageDestroy(): void {
    // Main window is destroyed, release UI related resources
    hilog.info(DOMAIN, 'testTag', '%{public}s', 'Ability onWindowStageDestroy');
  }

  onForeground(): void {
    // Ability has brought to foreground
    hilog.info(DOMAIN, 'testTag', '%{public}s', 'Ability onForeground');
  }

  onBackground(): void {
    // Ability has back to background
    hilog.info(DOMAIN, 'testTag', '%{public}s', 'Ability onBackground');
  }
}