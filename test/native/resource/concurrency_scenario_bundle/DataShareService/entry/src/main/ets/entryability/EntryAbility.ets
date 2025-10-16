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

import AbilityConstant from '@ohos.app.ability.AbilityConstant';
import hilog from '@ohos.hilog';
import UIAbility from '@ohos.app.ability.UIAbility';
import Want from '@ohos.app.ability.Want';
import window from '@ohos.window';
import common from '@ohos.app.ability.common';
import rdb from '@ohos.data.relationalStore';

interface GeneratedObjectLiteralInterface_1 {
  name: string;
  securityLevel: rdb.SecurityLevel;
}

const STORE_CONFIG: GeneratedObjectLiteralInterface_1 = {
  name: "DB00.db",
  securityLevel: rdb.SecurityLevel.S1,
}

let TBL_NAME = "TBL00";
let DROP_TBL = "DROP TABLE IF EXISTS " + TBL_NAME;
let DDL_TBL_CREATE = "CREATE TABLE IF NOT EXISTS "
  + TBL_NAME
  + " (id INTEGER PRIMARY KEY AUTOINCREMENT, name0 TEXT, name1 TEXT, name2 TEXT, name3 TEXT, name4 TEXT, name5 TEXT, name6 TEXT, " +
  "name7 TEXT, name8 TEXT, name9 TEXT, age INTEGER, phoneNumber DOUBLE, isStudent BOOLEAN, Binary BINARY)";

export default class EntryAbility extends UIAbility {
  onCreate(want: Want, launchParam: AbilityConstant.LaunchParam): void {
    hilog.info(0x0000, 'testTag', '%{public}s', 'Ability onCreate');
    console.log("[ttt] [DataShareServerTest]" + this.context.databaseDir);
    AppStorage.setOrCreate<common.UIAbilityContext>("TestAbilityContext",this.context)
    this.onCreate1();
  }

  async onCreate1() {
    console.log("[ttt] [DataShareTest] DataShareExtAbility onCreate this.context.databaseDir:" + this.context.databaseDir);
    let rdbStore = await rdb.getRdbStore(this.context.getApplicationContext(), STORE_CONFIG);
    await rdbStore.executeSql(DROP_TBL, []);
    await rdbStore.executeSql(DDL_TBL_CREATE, []);
  }

  onDestroy(): void {
    hilog.info(0x0000, 'testTag', '%{public}s', 'Ability onDestroy');
  }

  onWindowStageCreate(windowStage: window.WindowStage): void {
    // Main window is created, set main page for this ability
    hilog.info(0x0000, 'testTag', '%{public}s', 'Ability onWindowStageCreate');

    windowStage.loadContent('pages/Index', (err, data) => {
      if (err.code) {
        hilog.error(0x0000, 'testTag', 'Failed to load the content. Cause: %{public}s', JSON.stringify(err) ?? '');
        return;
      }
      hilog.info(0x0000, 'testTag', 'Succeeded in loading the content. Data: %{public}s', JSON.stringify(data) ?? '');
    });
  }

  onWindowStageDestroy(): void {
    // Main window is destroyed, release UI related resources
    hilog.info(0x0000, 'testTag', '%{public}s', 'Ability onWindowStageDestroy');
  }

  onForeground(): void {
    // Ability has brought to foreground
    hilog.info(0x0000, 'testTag', '%{public}s', 'Ability onForeground');
  }

  onBackground(): void {
    // Ability has back to background
    hilog.info(0x0000, 'testTag', '%{public}s', 'Ability onBackground');
  }
}
