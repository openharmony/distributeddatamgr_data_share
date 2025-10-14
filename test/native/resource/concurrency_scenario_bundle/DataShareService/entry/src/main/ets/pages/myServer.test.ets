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

import dataShare from '@ohos.data.dataShare'
import dataSharePredicates from '@ohos.data.dataSharePredicates'
import { BusinessError } from '@ohos.base';
import { ValuesBucket } from '@ohos.data.ValuesBucket';
import rpc from '@ohos.rpc';
import common from "@ohos.app.ability.common"

let cardUri = ("datashareproxy://com.acts.ohos.data.datasharetest/test");
let dsProxyHelper: dataShare.DataShareHelper | undefined = undefined
let dsProxyHelper1:dataShare.DataShareHelper;
let context: common.UIAbilityContext
context = AppStorage.get<common.UIAbilityContext>("TestAbilityContext") as common.UIAbilityContext

export async function connectDataShareExtAbility001() {
    console.log("[ttt] [DataShareServerTest] <<Consumer>> connectDataShareExtAbility begin ");
    try{
        dsProxyHelper = await dataShare.createDataShareHelper(context, cardUri, {isProxy : true});
        // dsProxyHelper1 = await dataShare.createDataShareHelper(context, cardUri, {isProxy : true});
        console.log("[ttt] [DataShareServerTest] <<Consumer>> dsProxyHelper " + JSON.stringify(dsProxyHelper));
    } catch (e) {
        console.log("[ttt] [DataShareServerTest] createDataShareHelper failed :" + e);
    }
}

export async function disconnectDataShareExtAbility() {
    console.log("[ttt] [DataShareServerTest] <<Consumer>> disconnectDataShareExtAbility begin");
    dsProxyHelper = undefined;
    console.log("[ttt] [DataShareServerTest] <<Consumer>> disconnectDataShareExtAbility end");
}

export async function onRdb() {
    console.log("[ttt] [DataShareServerTest] <<Consumer>> on begin");
    if (dsProxyHelper == null) {
        console.log("[ttt] [DataShareServerTest] <<Consumer>> on end, DSHelper is null");
        return;
    }
    let template: dataShare.Template = {
        predicates : {
            "p1" : "select name0 as name from TBL00",
            "p2" : "select name1 as name from TBL00",
        },
        scheduler : "select remindTimer(strftime('%s', 'now', '+30 seconds')) from TBL00"
    }
    dsProxyHelper.addTemplate(cardUri, "111", template);
    let templateId:dataShare.TemplateId = {subscriberId:"111", bundleNameOfOwner:"com.acts.ohos.data.datasharetest"}
    let result:Array<dataShare.OperationResult> = dsProxyHelper.on("rdbDataChange", [cardUri], templateId, onCallback);
    console.log("[ttt] [DataShareServerTest] <<Consumer>> on end " + JSON.stringify(result));
}

export function onCallback(err:BusinessError, node:dataShare.RdbDataChangeNode) {
    console.info("[ttt] [callback] rdb change ININ");
    // console.info("[ttt][callback] " + JSON.stringify(node.uri));
    // console.info("[ttt][callback] " + JSON.stringify(node.templateId));
    // console.info("[ttt][callback] " + node.data.length);
    for (let i = 0; i < node.data.length; i++) {
        // console.info("[ttt][callback] " + typeof node.data[i]);
        console.info("[ttt][callback] " + node.data[i]);
    }
}

export async function offRdb() {
    console.log("[ttt] [DataShareServerTest] <<Consumer>> off begin");
    if (dsProxyHelper == null) {
        console.log("[ttt] [DataShareServerTest] <<Consumer>> off end, DSHelper is null");
        return;
    }
    let templateId:dataShare.TemplateId = {subscriberId:"111", bundleNameOfOwner:"com.acts.ohos.data.datasharetest"}
    let result:Array<dataShare.OperationResult> = dsProxyHelper.off("rdbDataChange", [cardUri], templateId);
    console.log("[ttt] [DataShareServerTest] <<Consumer>> off end " + JSON.stringify(result));
}

export async function queryTest() {
    console.log("[ttt] [DataShareServerTest] <<Consumer>> query begin");
    let da =  new dataSharePredicates.DataSharePredicates();
    if (da == null || da == undefined) {
        console.log("[ttt] [DataShareServerTest] <<Consumer>> da is null or undefined");
        return;
    }
    let count = 0;
    if (dsProxyHelper == null) {
        console.log("[ttt] [DataShareServerTest] <<Consumer>> query end, DSHelper is null");
        return;
    }
    let result = await dsProxyHelper.query(cardUri, da, ["*"]);
    if (result != undefined) {
        count = result.rowCount;
        console.log("[ttt] [DataShareServerTest] <<Consumer>> hanlu query end, result:" + JSON.stringify(result));
    }
    console.log("[ttt] [DataShareServerTest] <<Consumer>> query end, count222:" + count);
    return count;
}

export async function insertTest() {
    console.log("[ttt] [DataShareServerTest] <<Consumer>> insert begin");
    if (dsProxyHelper == null) {
        console.log("[ttt] [DataShareServerTest] <<Consumer>> insert end, DSHelper is null");
        return;
    }
    let i = 0;
    let ret = 0;
    for (i = 0; i < 20; i++) {
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
        await dsProxyHelper.insert(cardUri, vb).then((number)=>{
            ret =number;
        }).catch((err: BusinessError) => {
            console.error(`[ttt] insert error: code: ${err.code}, message: ${err.message} `);
        });
        console.log("[ttt] [DataShareServerTest] <<Consumer>> insert ret is " + ret);
        console.log("[ttt] [DataShareServerTest] <<Consumer>> insert end " + cardUri);
    }
    return ret;
}

export async function updateTest() {
    console.log("[ttt] [DataShareServerTest] <<Consumer>> update begin");
    if (dsProxyHelper == null) {
        console.log("[ttt] [DataShareServerTest] <<Consumer>> update end, DSHelper is null");
        return;
    }
    console.log("[ttt] [DataShareServerTest] <<Consumer>> update begin RPF666");
    let da = new dataSharePredicates.DataSharePredicates();
    console.log("[ttt] [DataShareServerTest] <<Consumer>> update begin RPF777");
    if (da == null || da == undefined) {
        console.log("[ttt] [DataShareServerTest] <<Consumer>> da is null or undefined");
        return;
    }
    let ret = dsProxyHelper.update(cardUri, da, { "name0": "ZhangSan" });
    console.log("[ttt] [DataShareServerTest] <<Consumer>> update end, result:" + ret);
    return ret;
}

export async function deleteTest() {
    console.log("[ttt] [DataShareServerTest] <<Consumer>> delete begin");
    if (dsProxyHelper == null) {
        console.log("[ttt] [DataShareServerTest] <<Consumer>> delete end, DSHelper is null");
        return;
    }
    let da =  new dataSharePredicates.DataSharePredicates();
    if (da == null || da == undefined) {
        console.log("[ttt] [DataShareServerTest] <<Consumer>> da is null or undefined");
        return;
    }
    let ret = await dsProxyHelper.delete(cardUri, da);
    console.log("[ttt] [DataShareServerTest] <<Consumer>> delete end, result:" + ret);
    return ret;
}

export async function offPublishData() {
    console.log("[ttt] [DataShareServerTest] <<Consumer>> offPublishData begin");
    if (dsProxyHelper == null) {
        console.log("[ttt] [DataShareServerTest] <<Consumer>> on end, DSHelper is null");
        return;
    }

    try{
        let uris:Array<string> = ["city", "datashareproxy://com.acts.ohos.data.datasharetest/test", "key2"];
        let result: Array<dataShare.OperationResult> = dsProxyHelper.off("publishedDataChange",
            uris,
            "11", onPublishCallback);
        console.log("[ttt] [DataShareServerTest] <<Consumer>> off end " + JSON.stringify(result));
    }catch(err){
        console.error(` [ttt] offPublishData error: code: ${err.code}, message: ${err.message} `);
    }
}

export function onPublishCallback(err:BusinessError, node:dataShare.PublishedDataChangeNode) {
    console.info("[ttt] [serverCallback] published change ININ");
    console.info("[ttt][serverCallback] bundleName " + JSON.stringify(node.bundleName));
    console.info("[ttt][serverCallback] data size" + node.data.length);
    for (let i = 0; i < node.data.length; i++) {
        console.info("[ttt][serverCallback] " + typeof node.data[i].data);
        // if (typeof node.data[i].data != 'string') {
        //     let ash:rpc.Ashmem = node.data[i].data;
        //     ash.mapReadonlyAshmem();
        //     console.info("[ttt][serverCallback]  " + JSON.stringify(ash.readAshmem(ash.getAshmemSize()/4, 0)));
        //
        // }
        console.info("[ttt][serverCallback] data " + i + " " + JSON.stringify(node.data[i]));
    }
}

export async function onPublishData() {
    console.log("[ttt] [DataShareServerTest] <<Consumer>> onPublishData begin");
    if (dsProxyHelper == null) {
        console.log("[ttt] [DataShareServerTest] <<Consumer>> on end, DSHelper is null");
        return;
    }
    let uris:Array<string> = ["city", "datashareproxy://com.acts.ohos.data.datasharetest/appInfo", "datashareproxy://com.acts.ohos.data.datasharetest/test"];
    try {
        console.log("[ttt] [DataShareServerTest] <<Consumer>> on begin");
        let result: Array<dataShare.OperationResult> = dsProxyHelper.on("publishedDataChange", uris, "11", onPublishCallback);
        console.log("[ttt] [DataShareServerTest] <<Consumer>> on end " + JSON.stringify(result));
        // return result.length;
    } catch (e) {
        console.log("[ttt] [DataShareServerTest] <<Consumer>> on err " + JSON.stringify(e));

    }
}

export async function getPublishedData() {
    if (dsProxyHelper == null) {
        console.log("[ttt] [DataShareServerTest] <<Consumer>> getPublishedData end, DSHelper is null");
        return;
    }
    console.log("[ttt] [DataShareServerTest] <<Consumer>> getPublishedData begin");
    let publishedData:Array<dataShare.PublishedItem> = await dsProxyHelper.getPublishedData("com.acts.ohos.data.datasharetest")
    let ret = publishedData.length;
    console.log("[ttt] [DataShareServerTest] <<Consumer>> getPublishedData end dataSize is " + ret);
    console.log("[ttt] [DataShareServerTest] <<Consumer>> getPublishedData end data is " + JSON.stringify(publishedData));
    return ret;
}

export async function onPublishData3() {
    console.log("[ttt] [DataShareServerTest] <<Consumer>> onPublishData3 begin");
    if (dsProxyHelper == null) {
        console.log("[ttt] [DataShareServerTest] <<Consumer>> on end, DSHelper is null");
        return;
    }
    let uris:Array<string> = ["city", "datashareproxy://com.acts.ohos.data.datasharetest/appInfo", "datashareproxy://com.acts.ohos.data.datasharetest/test"];
    try {
        console.log("[ttt] [DataShareServerTest] <<Consumer>> on begin");
        let result: Array<dataShare.OperationResult> = dsProxyHelper1.on("publishedDataChange", uris, "11", onPublishCallback3);
        console.log("[ttt] [DataShareServerTest] <<Consumer>> on end " + JSON.stringify(result));
        // return result.length;
    } catch (e) {
        console.log("[ttt] [DataShareServerTest] <<Consumer>> on err " + JSON.stringify(e));

    }
}

export function onPublishCallback3(err:BusinessError, node:dataShare.PublishedDataChangeNode) {
    console.info("[ttt] [callback]  server published change ININ 3333");
    console.info("[ttt][callback] bundleName 3333" + JSON.stringify(node.bundleName));
    console.info("[ttt][callback] data size" + node.data.length);
    for (let i = 0; i < node.data.length; i++) {
        console.info("[ttt][callback]  " + typeof node.data[i].data);
        // if (typeof node.data[i].data != 'string') {
        //     let ash:rpc.Ashmem = node.data[i].data;
        //     ash.mapReadonlyAshmem();
        //     console.info("[ttt][callback]  " + JSON.stringify(ash.readAshmem(ash.getAshmemSize()/4, 0)));
        //
        // }
        console.info("[ttt][callback] data " + i + " " + JSON.stringify(node.data[i]));
    }

}

export async function publishTest() {
    console.log("[ttt] [DataShareServerTest] <<Consumer>> publish begin");
    if (dsProxyHelper == null) {
        console.log("[ttt] [DataShareServerTest] <<Consumer>> publish end, DSHelper is null");
        return;
    }
    let data : Array<dataShare.PublishedItem> = [
        {key:"city", subscriberId:"11", data:"xianServer"+Math.random()},
        {key:"datashareproxy://com.acts.ohos.data.datasharetest/appInfo", subscriberId:"11", data:"appinfo is just a test app"},
        {key:"empty", subscriberId:"11", data:"nobody sub Server"+Math.random()},
        {key:"datashareproxy://com.acts.ohos.data.datasharetest/test", subscriberId:"11", data:"nobody sub Server"+Math.random()}];
    console.log("[ttt] [DataShareServerTest] <<Consumer>> publish begin");

    let nums:number[] = [1,2,3];
    try {
        let ashmem = rpc.Ashmem.create("ashmem", (nums.length) * 4);
        console.log("[ttt] [DataShareServerTest] <<Consumer>> hanlu222 " + JSON.stringify(ashmem));
        ashmem.mapReadWriteAshmem();
        console.log("[ttt] [DataShareServerTest] <<Consumer>> hanlu333");
        //nums.fill(6, 0 ,1024);
        console.log("[ttt] [DataShareServerTest] <<Consumer>> hanlu444 " + JSON.stringify(nums));
        ashmem.writeAshmem(nums, nums.length, 0);
        console.log("[ttt] [DataShareServerTest] <<Consumer>> hanlu555");
        //                data.push({
        //                    "key" : "key2",
        //                    "data" : ashmem,
        //                    "subscriberId" : "11",
        //                });
        console.log("[ttt] [DataShareServerTest] <<Consumer>> datasss length is:", data.length);
        let result: Array<dataShare.OperationResult> = await dsProxyHelper.publish(data, "com.acts.ohos.data.datasharetest");
        console.log("[ttt] [DataShareServerTest] <<Consumer>> publish end " + JSON.stringify(result));
        ashmem.closeAshmem();
        let publishedData:Array<dataShare.PublishedItem> = await dsProxyHelper.getPublishedData("com.acts.ohos.data.datasharetest")
        console.log("[ttt] [DataShareServerTest] <<Consumer>> getPublishedData end " + JSON.stringify(publishedData));
        //                for (let i = 0; i < publishedData.length; i++) {
        //                    console.info("[ttt][callback]  " + typeof publishedData[i].data);
        //                    if (typeof publishedData[i].data != 'string') {
        //                        let ash:rpc.Ashmem = publishedData[i].data;
        //                        ash.mapReadonlyAshmem();
        //                        console.info("[ttt][callback]  " + JSON.stringify(ash.readAshmem(ash.getAshmemSize()/4, 0)));
        //                        ash.closeAshmem();
        //                    }
        //                }
    } catch (e) {
        console.log("[ttt] [DataShareServerTest] <<Consumer>> publish error " + JSON.stringify(e));
    }
}

export async function publishTest2() {
    console.log("[ttt] [DataShareServerTest] <<Consumer>> publish begin");
    if (dsProxyHelper == null) {
        console.log("[ttt] [DataShareServerTest] <<Consumer>> publish end, DSHelper is null");
        return;
    }
    let data : Array<dataShare.PublishedItem> = [
        {key:"city", subscriberId:"11", data:"xianServer22222"},
        {key:"datashareproxy://com.acts.ohos.data.datasharetest/appInfo", subscriberId:"11", data:"appinfo is just a test app"},
        {key:"empty", subscriberId:"11", data:"nobody sub Server"+222222},
        {key:"datashareproxy://com.acts.ohos.data.datasharetest/test", subscriberId:"11", data:"nobody sub Server"+Math.random()}];
    console.log("[ttt] [DataShareServerTest] <<Consumer>> publish begin");

    let nums:number[] = [1,2,3];
    try {
        let ashmem = rpc.Ashmem.create("ashmem", (nums.length) * 4);
        console.log("[ttt] [DataShareServerTest] <<Consumer>> hanlu222 " + JSON.stringify(ashmem));
        ashmem.mapReadWriteAshmem();
        console.log("[ttt] [DataShareServerTest] <<Consumer>> hanlu333");
        //nums.fill(6, 0 ,1024);
        console.log("[ttt] [DataShareServerTest] <<Consumer>> hanlu444 " + JSON.stringify(nums));
        ashmem.writeAshmem(nums, nums.length, 0);
        console.log("[ttt] [DataShareServerTest] <<Consumer>> hanlu555");

        console.log("[ttt] [DataShareServerTest] <<Consumer>> datasss length is:", data.length);
        let result: Array<dataShare.OperationResult> = await dsProxyHelper.publish(data, "com.acts.ohos.data.datasharetest",2);
        console.log("[ttt] [DataShareServerTest] <<Consumer>> publish end " + JSON.stringify(result));
        ashmem.closeAshmem();
        let publishedData:Array<dataShare.PublishedItem> = await dsProxyHelper.getPublishedData("com.acts.ohos.data.datasharetest")
        console.log("[ttt] [DataShareServerTest] <<Consumer>> getPublishedData end " + JSON.stringify(publishedData));
    } catch (e) {
        console.log("[ttt] [DataShareServerTest] <<Consumer>> publish error " + JSON.stringify(e));
    }
}

export async function publishTest3() {
    console.log("[ttt] [DataShareServerTest] <<Consumer>> publish begin");
    if (dsProxyHelper == null) {
        console.log("[ttt] [DataShareServerTest] <<Consumer>> publish end, DSHelper is null");
        return;
    }
    let data : Array<dataShare.PublishedItem> = [
        {key:"city", subscriberId:"11", data:"xianServer3333333"},
        {key:"datashareproxy://com.acts.ohos.data.datasharetest/appInfo", subscriberId:"11", data:"appinfo is just a test app"},
        {key:"empty", subscriberId:"11", data:"nobody sub Server"+33333},
        {key:"datashareproxy://com.acts.ohos.data.datasharetest/test", subscriberId:"11", data:"nobody sub Server"+Math.random()}];
    console.log("[ttt] [DataShareServerTest] <<Consumer>> publish begin");

    let nums:number[] = [1,2,3];
    try {
        let ashmem = rpc.Ashmem.create("ashmem", (nums.length) * 4);
        console.log("[ttt] [DataShareServerTest] <<Consumer>> hanlu222 " + JSON.stringify(ashmem));
        ashmem.mapReadWriteAshmem();
        console.log("[ttt] [DataShareServerTest] <<Consumer>> hanlu333");
        //nums.fill(6, 0 ,1024);
        console.log("[ttt] [DataShareServerTest] <<Consumer>> hanlu444 " + JSON.stringify(nums));
        ashmem.writeAshmem(nums, nums.length, 0);
        console.log("[ttt] [DataShareServerTest] <<Consumer>> hanlu555");

        console.log("[ttt] [DataShareServerTest] <<Consumer>> datasss length is:", data.length);
        let result: Array<dataShare.OperationResult> = await dsProxyHelper.publish(data, "com.acts.ohos.data.datasharetest",3);
        console.log("[ttt] [DataShareServerTest] <<Consumer>> publish end " + JSON.stringify(result));
        ashmem.closeAshmem();
        let publishedData:Array<dataShare.PublishedItem> = await dsProxyHelper.getPublishedData("com.acts.ohos.data.datasharetest")
        console.log("[ttt] [DataShareServerTest] <<Consumer>> getPublishedData end " + JSON.stringify(publishedData));
    } catch (e) {
        console.log("[ttt] [DataShareServerTest] <<Consumer>> publish error " + JSON.stringify(e));
    }
}
