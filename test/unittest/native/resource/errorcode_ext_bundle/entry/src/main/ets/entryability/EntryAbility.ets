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
import dataShare from '@ohos.data.dataShare'
import dataSharePredicates from '@ohos.data.dataSharePredicates'
import { window } from '@kit.ArkUI';

let dseUri = ("datashare:///com.acts.errorcodetest");
let uri = ("datashare://com.acts.errorcodetest/entry/DB00/TBL00?Proxy=true");

export default class MainAbility extends UIAbility {
  onCreate(want: Want, launchParam: AbilityConstant.LaunchParam) {
    console.log("[ttt] [DataShareTest] <<Consumer>> MainAbility onCreate")
    globalThis.abilityWant = want;
  }

  onDestroy() {
    console.log("[ttt] [DataShareTest] <<Consumer>> MainAbility onDestroy")
  }

  onWindowStageCreate(windowStage: window.WindowStage) {
    globalThis.abilityContext = this.context;
    console.log("[ttt] [DataShareTest] <<Consumer>> MainAbility this.context.databaseDir:" + this.context.databaseDir);
    globalThis.connectDataShareExtAbility = (async () => {
      console.log("[ttt] [DataShareTest] <<Consumer>> connectDataShareExtAbility begin");
      globalThis.dsHelper = await dataShare.createDataShareHelper(this.context, dseUri);
      console.log("[ttt] [DataShareTest] <<Consumer>> connectDataShareExtAbility end");
    })
    windowStage.loadContent("pages/index", (err, data) => {
      if (err.code) {
        console.error('Failed to load the content. Cause:' + JSON.stringify(err));
        return;
      }
      console.info('Succeeded in loading the content. Data: ' + JSON.stringify(data))
    });
  }

  onWindowStageDestroy() {
    console.log("[ttt] [DataShareTest] <<Consumer>> MainAbility onWindowStageDestroy")
  }

  onForeground() {
    console.log("[ttt] [DataShareTest] <<Consumer>> MainAbility onForeground")
  }

  onBackground() {
    console.log("[ttt] [DataShareTest] <<Consumer>> MainAbility onBackground")
  }
};