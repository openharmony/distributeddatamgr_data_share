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

import distributedData from '@ohos.data.distributedKVStore';
import dataShare from '@ohos.data.dataShare'
import dataSharePredicates from '@ohos.data.dataSharePredicates'
import { BusinessError } from '@ohos.base';
import { ValuesBucket } from '@ohos.data.ValuesBucket';
import rpc from '@ohos.rpc';
import common from "@ohos.app.ability.common"
import bundleManager from '@ohos.bundle.bundleManager';
import { expect } from '@ohos/hypium';
import { accountManager } from '@kit.MDMKit';

let cardUri = ("datashareproxy://com.acts.ohos.data.datasharetest/test");
// let cardUri = ("datashare:///com.acts.ohos.data.datasharetest/test");
let cloneUri = ("datashareproxy://com.acts.ohos.data.datasharetest/test?appIndex=1");
let extUri = ("datashare:///com.acts.datasharetest");

let dsProxyHelper: dataShare.DataShareHelper | undefined = undefined
let dsProxyHelper2: dataShare.DataShareHelper | undefined = undefined


let context: common.UIAbilityContext
context = AppStorage.get<common.UIAbilityContext>("TestAbilityContext") as common.UIAbilityContext

const BUNDLE_NAME = 'com.acts.datasharetest';

export async function connectDataShareExtAbility001() {
    console.log("[ttt] [DataShareClientTest] <<Consumer>> connectDataShareExtAbility1 begin ");
    //dsProxyHelper = await dataShare.createDataShareHelper(context, testUri, {isProxy : true});
    dsProxyHelper = await dataShare.createDataShareHelper(context, cardUri, {isProxy : true});
    if (dsProxyHelper == null) {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> DSHelper1 is null");
    }
    console.log("[ttt] [DataShareClientTest] <<Consumer>> dsProxyHelper1 " + JSON.stringify(dsProxyHelper));
}

export async function connectDataShareExtension() {
    console.log("[ttt] [DataShareClientTest] <<Consumer>> connectDataShareExtension begin ");
    dsProxyHelper = await dataShare.createDataShareHelper(context, extUri);
    if (dsProxyHelper == null) {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> DSHelper1 is null");
    }
    console.log("[ttt] [DataShareClientTest] <<Consumer>> dsProxyHelper1 " + JSON.stringify(dsProxyHelper));
}

export async function disconnectDataShareExtAbility() {
    console.log("[ttt] [DataShareClientTest] <<Consumer>> disconnectDataShareExtAbility begin");
    if (dsProxyHelper == null) {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> on end, DSHelper is null");
        return;
    }
    dsProxyHelper.close();
    console.log("[ttt] [DataShareClientTest] <<Consumer>> disconnectDataShareExtAbility end");
}


export async function onRdb100() {
    console.log("[ttt] [DataShareClientTest] <<Consumer>> user100 onRdbDataChange begin");
    if (dsProxyHelper == null) {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> user100 onRdbDataChange end, DSHelper is null");
        return;
    }
    let template: dataShare.Template = {
        predicates: {
            "p1" : "select name0 as name from TBL00",
            "p2" : "select name1 as name from TBL00",
        },
        scheduler: "select remindTimer(strftime('%s', 'now', '+10 seconds'))"
    }
    try {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> addTemplate begin");
        dsProxyHelper.addTemplate(cardUri, "100", template);
        console.log("[ttt] [DataShareClientTest] <<Consumer>> addTemplate end");
    } catch (err) {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> addTemplate error " + JSON.stringify(err));
    }
    let templateId: dataShare.TemplateId = {
        subscriberId: "100", bundleNameOfOwner: "com.acts.ohos.data.datasharetestclient1"
    }
    let result: Array<dataShare.OperationResult> = dsProxyHelper.on("rdbDataChange", [cardUri], templateId, onRdb100Callback);
    console.log("[ttt] [DataShareClientTest] <<Consumer>> user100 onRdbDataChange end " + JSON.stringify(result));
}

export function onRdb100Callback(err:BusinessError, node:dataShare.RdbDataChangeNode) {
    console.info("[ttt] [DataShareClientTest] [callback] user100 receive rdbDataChange");
}

export async function onRdb101() {
    console.log("[ttt] [DataShareClientTest] <<Consumer>> user101 onRdbDataChange begin");
    if (dsProxyHelper == null) {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> user101 onRdbDataChange end, DSHelper is null");
        return;
    }
    let template: dataShare.Template = {
        predicates: {
            "p1" : "select name0 as name from TBL00",
            "p2" : "select name1 as name from TBL00",
        },
        scheduler: "select remindTimer(strftime('%s', 'now', '+10 seconds'))"
    }
    try {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> addTemplate begin");
        dsProxyHelper.addTemplate(cardUri, "101", template);
        console.log("[ttt] [DataShareClientTest] <<Consumer>> addTemplate end");
    } catch (err) {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> addTemplate error " + JSON.stringify(err));
    }
    let templateId: dataShare.TemplateId = {
        subscriberId: "101", bundleNameOfOwner: "com.acts.ohos.data.datasharetestclient1"
    }
    let result: Array<dataShare.OperationResult> = dsProxyHelper.on("rdbDataChange", [cardUri], templateId, onRdb101Callback);
    console.log("[ttt] [DataShareClientTest] <<Consumer>> user101 onRdbDataChange end " + JSON.stringify(result));
}

export function onRdb101Callback(err:BusinessError, node:dataShare.RdbDataChangeNode) {
    console.info("[ttt] [DataShareClientTest] [callback] user101 receive rdbDataChange");
}

export async function offRdb() {
    console.log("[ttt] [DataShareClientTest] <<Consumer>> off begin");
    if (dsProxyHelper == null) {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> off end, DSHelper is null");
        return;
    }

    let templateId:dataShare.TemplateId = {subscriberId:"100", bundleNameOfOwner:"com.acts.ohos.data.datasharetestclient1"}
    let result:Array<dataShare.OperationResult> = dsProxyHelper.off("rdbDataChange", [cardUri], templateId);
    console.log("[ttt] [DataShareClientTest] <<Consumer>> off end " + JSON.stringify(result));
    let templateId2:dataShare.TemplateId = {subscriberId:"101", bundleNameOfOwner:"com.acts.ohos.data.datasharetestclient1"}
    let result2:Array<dataShare.OperationResult> = dsProxyHelper.off("rdbDataChange", [cardUri], templateId2);
    console.log("[ttt] [DataShareClientTest] <<Consumer>> off end " + JSON.stringify(result2));
}

export async function insertTest() {
    console.log("[ttt] [DataShareClientTest] <<Consumer>> insert 100 begin");
    // dsProxyHelper = await dataShare.createDataShareHelper(context, cardUri, {isProxy : true});
    if (dsProxyHelper == null) {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> insert 100 end, DSHelper is null");
        return;
    }

    let i = 0;
    let ret = 0;
    for (i = 0; i < 1; i++) {
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
        ret = await dsProxyHelper.insert(cardUri, vb);
        console.log("[ttt] [DataShareClientTest] <<Consumer>> insert 100 end rowNum is " + ret);
    }
    return ret;
}

export async function offPublishData() {
    console.log("[ttt] [DataShareClientTest] <<Consumer>> offPublishData begin");
    if (dsProxyHelper == null) {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> on end, DSHelper is null");
        return;
    }

    try{
        let uris:Array<string> =  [cardUri];
        let result: Array<dataShare.OperationResult> = dsProxyHelper.off("publishedDataChange", uris, "11");
        console.log("[ttt] [DataShareClientTest] <<Consumer>> off end " + JSON.stringify(result));
    }catch(err){
        console.error(` [ttt] offPublishData error: code: ${err.code}, message: ${err.message} `);
    }
}

export function onPublish100Callback(err:BusinessError, node:dataShare.PublishedDataChangeNode) {
    console.info("[ttt] [DataShareClientTest] [callback]  user100 receive publishedDataChange");
}

export async function onPublishData100() {
    console.log("[ttt] [DataShareClientTest] <<Consumer>> user100 onPublishData begin");
    if (dsProxyHelper == null) {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> user100 on end, DSHelper is null");
        return;
    }
    let uris:Array<string> = [cardUri];
    try {
        let result: Array<dataShare.OperationResult> = dsProxyHelper.on("publishedDataChange", uris, "11", onPublish100Callback);
        console.log("[ttt] [DataShareClientTest] <<Consumer>> user100 on end " + JSON.stringify(result));
    } catch (e) {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> on err " + JSON.stringify(e));
    }
}

export function onPublish101Callback(err:BusinessError, node:dataShare.PublishedDataChangeNode) {
    console.info("[ttt] [DataShareClientTest] [callback]  user101 receive publishedDataChange");
}

export async function onPublishData101() {
    console.log("[ttt] [DataShareClientTest] <<Consumer>> user101 onPublishData begin");
    if (dsProxyHelper == null) {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> user101 on end, DSHelper is null");
        return;
    }
    let uris:Array<string> = [cardUri];
    try {
        let result: Array<dataShare.OperationResult> = dsProxyHelper.on("publishedDataChange", uris, "11", onPublish101Callback);
        console.log("[ttt] [DataShareClientTest] <<Consumer>> user101 on end " + JSON.stringify(result));
    } catch (e) {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> on err " + JSON.stringify(e));
    }
}

export async function onData100() {
    console.log("[ttt] [DataShareClientTest] <<Consumer>> user100 onDataChange begin");
    if (dsProxyHelper == null) {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> user100 onDataChange end, DSHelper is null");
        return;
    }

    try {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> user100 onDataChange begin");
        let result = dsProxyHelper.on("dataChange", cardUri, (err, changeInfo) => {
            console.info("[ttt] [DataShareClientTest] [callback] user100 onDataChange err" + JSON.stringify(err));
            console.info("[ttt] [DataShareClientTest] [callback] user100 receive dataChange");
        });
        console.log("[ttt] [DataShareClientTest] <<Consumer>> user100 onDataChange end " + JSON.stringify(result));
    } catch (e) {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> user100 onDataChange err " + JSON.stringify(e));
    }
}

export async function onData101() {
    console.log("[ttt] [DataShareClientTest] <<Consumer>> user101 onDataChange begin");
    if (dsProxyHelper == null) {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> user101 onDataChange end, DSHelper is null");
        return;
    }

    try {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> user101 onDataChange begin");
        let result = dsProxyHelper.on("dataChange", cardUri, (err, changeInfo) => {
            console.info("[ttt] [DataShareClientTest] [callback] user101 onDataChange err" + JSON.stringify(err));
            console.info("[ttt] [DataShareClientTest] [callback] user101 receive dataChange");
        });
        console.log("[ttt] [DataShareClientTest] <<Consumer>> user101 onDataChange end " + JSON.stringify(result));
    } catch (e) {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> user101 onDataChange err " + JSON.stringify(e));
    }
}

export async function onData100ext() {
    console.log("[ttt] [DataShareClientTest] <<Consumer>> user100 onDataChange ext begin");
    if (dsProxyHelper == null) {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> user100 onDataChange ext end, DSHelper is null");
        return;
    }

    try {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> user100 onDataChange ext begin");
        let result = dsProxyHelper.on("dataChange", extUri, (err) => {
            console.info("[ttt] [DataShareClientTest] [callback] user100 onDataChange ext err" + JSON.stringify(err));
            console.info("[ttt] [DataShareClientTest] [callback] user100 receive ext dataChange");
        });
        console.log("[ttt] [DataShareClientTest] <<Consumer>> user100 onDataChange ext end " + JSON.stringify(result));
    } catch (e) {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> user100 onDataChange ext err " + JSON.stringify(e));
    }
}

export async function onData101ext() {
    console.log("[ttt] [DataShareClientTest] <<Consumer>> user101 onDataChange ext begin");
    if (dsProxyHelper == null) {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> user101 onDataChange ext end, DSHelper is null");
        return;
    }

    try {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> user101 onDataChange ext begin");
        let result = dsProxyHelper.on("dataChange", extUri, (err) => {
            console.info("[ttt] [DataShareClientTest] [callback] user101 onDataChange ext err" + JSON.stringify(err));
            console.info("[ttt] [DataShareClientTest] [callback] user101 receive ext dataChange");
        });
        console.log("[ttt] [DataShareClientTest] <<Consumer>> user101 onDataChange ext end " + JSON.stringify(result));
    } catch (e) {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> user101 onDataChange ext err " + JSON.stringify(e));
    }
}

export async function offData() {
    console.log("[ttt] [DataShareClientTest] <<Consumer>> offDataChange begin");
    if (dsProxyHelper == null) {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> offDataChange end, DSHelper is null");
        return;
    }

    try {
        dsProxyHelper.off("dataChange", cardUri);
        console.log("[ttt] [DataShareClientTest] <<Consumer>> offDataChange end ");
    } catch (e) {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> offDataChange err " + JSON.stringify(e));
    }
}

export async function getPublishedData() {
    if (dsProxyHelper == null) {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> getPublishedData end, DSHelper is null");
        return;
    }
    console.log("[ttt] [DataShareClientTest] <<Consumer>> getPublishedData begin");
    let publishedData:Array<dataShare.PublishedItem> = await dsProxyHelper.getPublishedData("com.acts.ohos.data.datasharetest")
    let ret = publishedData.length;
    console.log("[ttt] [DataShareClientTest] <<Consumer>> getPublishedData end dataSize is " + ret);
    console.log("[ttt] [DataShareClientTest] <<Consumer>> getPublishedData end data is " + JSON.stringify(publishedData));
    return ret;

}

export async function publishTest() {
    console.log("[ttt] [DataShareClientTest] <<Consumer>> publish begin");
    if (dsProxyHelper == null) {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> publish end, DSHelper is null");
        return;
    }
    let data : Array<dataShare.PublishedItem> = [
        {key: "datashareproxy://com.acts.ohos.data.datasharetest/test", subscriberId:"11", data: "publish test" + Math.random()}
    ];

    try {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> data length is: " + data.length + " data is: " + JSON.stringify(data[0].data));
        let result: Array<dataShare.OperationResult> = await dsProxyHelper.publish(data, "com.acts.ohos.data.datasharetest");
        console.log("[ttt] [DataShareClientTest] <<Consumer>> publish end " + JSON.stringify(result));
    } catch (e) {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> publish error " + JSON.stringify(e));
    }
}

export async function getPublishDataTest() {
    console.log("[ttt] [DataShareClientTest] <<Consumer>> getPublishData begin");
    if (dsProxyHelper == null) {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> getPublishData end, DSHelper is null");
        return;
    }

    try {
        let publishedData: Promise<Array<dataShare.PublishedItem>> = dsProxyHelper.getPublishedData("com.acts.ohos.data.datasharetest");
        publishedData.then((data) => {
            console.log("[ttt] [DataShareClientTest] <<Consumer>> getPublishData end, size " + JSON.stringify(data.length) + " data " + JSON.stringify(data));
        })
    } catch (e) {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> getPublishData error " + JSON.stringify(e));
    }
}

export async function offAll() {
    offRdb();
    offPublishData();
    offData();
}