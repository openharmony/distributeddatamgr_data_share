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
import { ValuesBucket } from '@ohos.data.ValuesBucket';
import common from "@ohos.app.ability.common"

let cardUri = ("datashareproxy://com.acts.ohos.data.datasharetest/test?user=101");

let dsProxyHelper: dataShare.DataShareHelper | undefined = undefined


let context: common.UIAbilityContext
context = AppStorage.get<common.UIAbilityContext>("TestAbilityContext") as common.UIAbilityContext

export async function connectDataShareExtAbility001() {
    console.log("[ttt] [DataShareClientTest] <<Consumer>> connectDataShareExtAbility1 begin ");
    dsProxyHelper = await dataShare.createDataShareHelper(context, cardUri, {isProxy : true});
    if (dsProxyHelper == null) {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> DSHelper1 is null");
    }
    console.log("[ttt] [DataShareClientTest] <<Consumer>> dsProxyHelper1 " + JSON.stringify(dsProxyHelper));
}

export async function insertTest1() {
    console.log("[ttt] [DataShareClientTest] <<Consumer>> insert1 begin");
    // dsProxyHelper = await dataShare.createDataShareHelper(context, cardUri, {isProxy : true});
    if (dsProxyHelper == null) {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> insert1 end, DSHelper is null");
        return;
    }

    let i = 0;
    let ret = 0;
    for (i = 0; i < 2; i++) {
        let vb: ValuesBucket = {
            "name0": "name0" + i,
            "name1": "name1" + i,
            "name2": "name2" + i,
            "name3": "name3" + i,
            "name4": "name4" + i,
            "name5": "name5" + i,
            "name6": "name6" + i,
            "name7": "name7" + i,
            "name8": "name8" + i,
            "name9": "name9" + i,
        };
        try {
            ret = await dsProxyHelper.insert(cardUri, vb);
            console.log("[ttt] [DataShareClientTest] <<Consumer>> insert1 end rowNum is " + ret);
        } catch (e) {
            console.log("[ttt] error " + JSON.stringify(e));
        }

    }
    return ret;
}