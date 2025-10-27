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
import dataSharePredicates from '@ohos.data.dataSharePredicates'
import { BusinessError } from '@ohos.base';
import { ValuesBucket } from '@ohos.data.ValuesBucket';
import rpc from '@ohos.rpc';
import common from "@ohos.app.ability.common"
import bundleManager from '@ohos.bundle.bundleManager';

let cardUri = ("datashareproxy://com.acts.ohos.data.datasharetest/test");
let cloneUri = ("datashareproxy://com.acts.ohos.data.datasharetest/test?appIndex=abcd");

let dsProxyHelper: dataShare.DataShareHelper | undefined = undefined
let dsProxyHelper2: dataShare.DataShareHelper | undefined = undefined

let context: common.UIAbilityContext
context = AppStorage.get<common.UIAbilityContext>("TestAbilityContext") as common.UIAbilityContext

export async function connectDataShareExtAbility() {
    console.log("[ttt] [DataShareClientTest] <<Consumer>> connectDataShareExtAbility1 begin ");
    dsProxyHelper = await dataShare.createDataShareHelper(context, cardUri, {isProxy : true});
    if (dsProxyHelper == null) {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> DSHelper1 is null");
    }
    console.log("[ttt] [DataShareClientTest] <<Consumer>> dsProxyHelper1 " + JSON.stringify(dsProxyHelper));
}

export async function disconnectDataShareExtAbility() {
    console.log("[ttt] [DataShareClientTest] <<Consumer>> disconnectDataShareExtAbility begin");
    dsProxyHelper = undefined;
    console.log("[ttt] [DataShareClientTest] <<Consumer>> disconnectDataShareExtAbility end");
}

export async function insertTest() {
    console.info("[xxxTime] [qosTest] insert begin");
    // dsProxyHelper = await dataShare.createDataShareHelper(context, cardUri, {isProxy : true});
    if (dsProxyHelper == null) {
        console.info("[xxxTime] [qosTest] insert end, DSHelper is null");
        return;
    }
    let ret = 0;
    for (let i = 0; i < 2000; ++i) {
        let name = "name" + i
        let vb: ValuesBucket = {
            "name0": name
        };
        ret = await dsProxyHelper.insert(cardUri, vb);
        dsProxyHelper.insert(cardUri, vb, (err: BusinessError, result: DataShareResultSet) => {
            if (err != undefined) {
                console.error(`update error: code: ${err.code}, message: ${err.message} `);
                return;
            }
        });
    }
    return ret;
}