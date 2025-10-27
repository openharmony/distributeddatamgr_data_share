/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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
import dataShare from '@ohos.data.dataShare'
import common from "@ohos.app.ability.common"

let extUri = ("datashare:///com.acts.ohos.data.datasharetest?Proxy=false");

let dsProxyHelper: dataShare.DataShareHelper | undefined = undefined
let dsProxyHelper2: dataShare.DataShareHelper | undefined = undefined


let context: common.UIAbilityContext
context = AppStorage.get<common.UIAbilityContext>("TestAbilityContext") as common.UIAbilityContext

export async function connectDataShareExtension100() {
  console.log("[ttt] [DataShareClientTest] <<Consumer>> connectDataShareExtension begin ");
  let uri = extUri + "&user=100"
  dsProxyHelper = await dataShare.createDataShareHelper(context, uri);
  if (dsProxyHelper == null) {
    console.log("[ttt] [DataShareClientTest] <<Consumer>> DSHelper1 is null");
  }
  console.log("[ttt] [DataShareClientTest] <<Consumer>> dsProxyHelper1 " + JSON.stringify(dsProxyHelper));
}

export async function connectDataShareExtension101() {
  console.log("[ttt] [DataShareClientTest] <<Consumer>> connectDataShareExtension begin ");
  let uri = extUri + "&user=101"
  dsProxyHelper = await dataShare.createDataShareHelper(context, uri);
  if (dsProxyHelper == null) {
    console.log("[ttt] [DataShareClientTest] <<Consumer>> DSHelper1 is null");
  }
  console.log("[ttt] [DataShareClientTest] <<Consumer>> dsProxyHelper1 " + JSON.stringify(dsProxyHelper));
}

export async function disconnectDataShareExtension() {
  console.log("[ttt] [DataShareClientTest] <<Consumer>> disconnectDataShareExtAbility begin");
  if (dsProxyHelper == null) {
    console.log("[ttt] [DataShareClientTest] <<Consumer>> on end, DSHelper is null");
    return;
  }
  dsProxyHelper.close();
  console.log("[ttt] [DataShareClientTest] <<Consumer>> disconnectDataShareExtAbility end");
}

export async function onDataExt() {
  console.log("[ttt] [DataShareClientTest] <<Consumer>> onDataChange ext begin");
  if (dsProxyHelper == null) {
    console.log("[ttt] [DataShareClientTest] <<Consumer>> uonDataChange ext end, DSHelper is null");
    return;
  }

  try {
    console.log("[ttt] [DataShareClientTest] <<Consumer>> onDataChange ext begin");
    let result = dsProxyHelper.on("dataChange", extUri, (err) => {
      console.info("[ttt] [DataShareClientTest] [callback] onDataChange ext err" + JSON.stringify(err));
      console.info("[ttt] [DataShareClientTest] [callback] receive ext dataChange");
    });
    console.log("[ttt] [DataShareClientTest] <<Consumer>> onDataChange ext end " + JSON.stringify(result));
  } catch (e) {
    console.log("[ttt] [DataShareClientTest] <<Consumer>> onDataChange ext err " + JSON.stringify(e));
  }
}

export async function offDataExt() {
  console.log("[ttt] [DataShareClientTest] <<Consumer>> offDataChange begin");
  if (dsProxyHelper == null) {
    console.log("[ttt] [DataShareClientTest] <<Consumer>> offDataChange end, DSHelper is null");
    return;
  }

  try {
    let result = dsProxyHelper.off("dataChange", extUri);
    console.log("[ttt] [DataShareClientTest] <<Consumer>> offDataChange end " + JSON.stringify(result));
  } catch (e) {
    console.log("[ttt] [DataShareClientTest] <<Consumer>> offDataChange err " + JSON.stringify(e));
  }
}

export async function onNotifyExt() {
  console.log("[ttt] [DataShareClientTest] <<Consumer>> Ext Notify begin");
  if (dsProxyHelper == null) {
    console.log("[ttt] [DataShareClientTest] <<Consumer>> Ext Notify end, DSHelper is null");
    return;
  }

  try {
    console.log("[ttt] [DataShareClientTest] <<Consumer>> Ext Notify begin");
    let result = dsProxyHelper.notifyChange(extUri);
    console.log("[ttt] [DataShareClientTest] <<Consumer>> user100 onDataChange ext end " + JSON.stringify(result));
  } catch (e) {
    console.log("[ttt] [DataShareClientTest] <<Consumer>> Ext Notify err " + JSON.stringify(e));
  }
}