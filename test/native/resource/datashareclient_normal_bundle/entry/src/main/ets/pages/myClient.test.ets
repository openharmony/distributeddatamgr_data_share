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

import dataShare from '@ohos.data.dataShare'
import dataSharePredicates from '@ohos.data.dataSharePredicates'
import { BusinessError } from '@ohos.base';
import { ValuesBucket } from '@ohos.data.ValuesBucket';
import rpc from '@ohos.rpc';
import common from "@ohos.app.ability.common"

let cardUri = ("datashareproxy://com.acts.ohos.data.datasharetest/test");
let dsProxyHelper: dataShare.DataShareHelper | undefined = undefined


let context: common.UIAbilityContext
context = AppStorage.get<common.UIAbilityContext>("TestAbilityContext") as common.UIAbilityContext

export async function connectDataShareExtAbility001() {
    try {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> connectDataShareExtAbility begin ");
        dsProxyHelper = await dataShare.createDataShareHelper(context, cardUri, { isProxy: true });
        console.log("[ttt] [DataShareClientTest] <<Consumer>> dsProxyHelper " + JSON.stringify(dsProxyHelper));
    }
    catch(err){
        console.error(` [ttt] createdatashare error: code: ${err.code}, message: ${err.message} `);
    }
}

export async function disconnectDataShareExtAbility() {
    console.log("[ttt] [DataShareClientTest] <<Consumer>> disconnectDataShareExtAbility begin");
    dsProxyHelper = undefined;
    console.log("[ttt] [DataShareClientTest] <<Consumer>> disconnectDataShareExtAbility end");
}

export async function enable() {
    try {

        console.log("[ttt] [DataShareClientTest] <<Consumer>> enable begin");
        await dataShare.enableSilentProxy(context, cardUri);
        console.log("[ttt] [DataShareClientTest] <<Consumer>> enable end");
    }
    catch(err){
        console.error(` [ttt] enable error: code: ${err.code}, message: ${err.message} `);
    }
}

export async function disable() {
    try {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> disable begin");
        await dataShare.disableSilentProxy(context, cardUri);
        console.log("[ttt] [DataShareClientTest] <<Consumer>> disable end");
    }
    catch(err){
        console.error(` [ttt] disable error: code: ${err.code}, message: ${err.message} `);
    }
}


export async function onRdb() {
    console.log("[ttt] [DataShareClientTest] <<Consumer>> on begin");
    if (dsProxyHelper == null) {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> on end, DSHelper is null");
        return;
    }
    let template: dataShare.Template = {
        predicates: {
            "p1": "select name0 as name from TBL00",
            "p2": "select name1 as name from TBL00",
        },
        scheduler: "select remindTimer(strftime('%s', 'now', '+10 seconds'))"
    }
    dsProxyHelper.addTemplate(cardUri, "111", template);
    let templateId: dataShare.TemplateId = {
        subscriberId: "111", bundleNameOfOwner: "com.acts.ohos.data.datasharetestclient"
    }
    let result: Array<dataShare.OperationResult> = dsProxyHelper.on("rdbDataChange", [cardUri], templateId, onCallback);
    console.log("[ttt] [DataShareClientTest] <<Consumer>> on end " + JSON.stringify(result));
}

export function onCallback(err:BusinessError, node:dataShare.RdbDataChangeNode) {
    console.info("[ttt] [DataShareClientTest] [callback] rdb change ININ");
    for (let i = 0; i < node.data.length; i++) {
        // console.info("[ttt] [DataShareClientTest] [callback] " + typeof node.data[i]);
        console.info("[ttt] [DataShareClientTest] [callback] " + node.data[i]);
    }

}

export async function offRdb() {
    console.log("[ttt] [DataShareClientTest] <<Consumer>> off begin");
    if (dsProxyHelper == null) {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> off end, DSHelper is null");
        return;
    }
    let templateId:dataShare.TemplateId = {subscriberId:"111", bundleNameOfOwner:"com.acts.ohos.data.datasharetestclient"}
    let result:Array<dataShare.OperationResult> = dsProxyHelper.off("rdbDataChange", [cardUri], templateId);
    console.log("[ttt] [DataShareClientTest] <<Consumer>> off end " + JSON.stringify(result));
}

export async function queryTest() {
    console.log("[ttt] [DataShareClientTest] <<Consumer>> query begin");
    let da =  new dataSharePredicates.DataSharePredicates();
    if (da == null || da == undefined) {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> da is null or undefined");
        return;
    }
    let count = 0;
    if (dsProxyHelper == null) {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> query end, DSHelper is null");
        return;
    }
    let result = await dsProxyHelper.query(cardUri, da, ["*"]);
    if (result != undefined) {
        count = result.rowCount;
        console.log("[ttt] [DataShareClientTest] <<Consumer>>  query end, result:" + JSON.stringify(result));
        result.close();
    }
    console.log("[ttt] [DataShareClientTest] <<Consumer>> query end, count222:" + count);
    return count;
}

export async function queryTest2() {
    console.log("[ttt] [DataShareClientTest] <<Consumer>> query begin");
    let da =  new dataSharePredicates.DataSharePredicates();
    if (da == null || da == undefined) {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> da is null or undefined");
        return;
    }
    let count = 0;
    dsProxyHelper = await dataShare.createDataShareHelper(context, cardUri, {isProxy : true});
    if (dsProxyHelper == null) {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> query end, DSHelper is null");
        return;
    }
    let result = await dsProxyHelper.query(cardUri, da, ["*"]);
    if (result != undefined) {
        count = result.rowCount;
        console.log("[ttt] [DataShareClientTest] <<Consumer>>  query end, result:" + JSON.stringify(result));
        result.close();
    }
    console.log("[ttt] [DataShareClientTest] <<Consumer>> query end, count222:" + count);
    return count;
}

export async function insertTest() {
    console.log("[ttt] [DataShareClientTest] <<Consumer>> insert begin");
    if (dsProxyHelper == null) {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> insert end, DSHelper is null");
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
        ret = await dsProxyHelper.insert(cardUri, vb);
        console.log("[ttt] [DataShareClientTest] <<Consumer>> insert end rowNum is " + ret);
    }
    return ret;
}

export async function updateTest() {
    console.log("[ttt] [DataShareClientTest] <<Consumer>> update begin");
    if (dsProxyHelper == null) {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> update end, DSHelper is null");
        return;
    }
    console.log("[ttt] [DataShareClientTest] <<Consumer>> update begin RPF666");
    let da = new dataSharePredicates.DataSharePredicates();
    console.log("[ttt] [DataShareClientTest] <<Consumer>> update begin RPF777");
    if (da == null || da == undefined) {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> da is null or undefined");
        return;
    }
    let ret = dsProxyHelper.update(cardUri, da, { "name0": "ZhangSan" });
    console.log("[ttt] [DataShareClientTest] <<Consumer>> update end, result:" + ret);
    return ret;
}

export async function batchUpdate() {
    console.log("[ttt] [DataShareClientTest] <<Consumer>> update begin");
    if (dsProxyHelper == null) {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> update end, DSHelper is null");
        return;
    }

    console.log("[ttt] [DataShareClientTest] <<Consumer>> update begin RPF666");
    let da = new dataSharePredicates.DataSharePredicates();
    console.log("[ttt] [DataShareClientTest] <<Consumer>> update begin RPF777");
    if (da == null || da == undefined) {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> da is null or undefined");
        return;
    }
}

export async function deleteTest() {
    console.log("[ttt] [DataShareClientTest] <<Consumer>> delete begin");
    if (dsProxyHelper == null) {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> delete end, DSHelper is null");
        return;
    }
    let da =  new dataSharePredicates.DataSharePredicates();
    if (da == null || da == undefined) {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> da is null or undefined");
        return;
    }
    let ret = await dsProxyHelper.delete(cardUri, da);
    console.log("[ttt] [DataShareClientTest] <<Consumer>> delete end, result:" + ret);
    return ret;
}

export async function offPublishData() {
    console.log("[ttt] [DataShareClientTest] <<Consumer>> offPublishData begin");
    if (dsProxyHelper == null) {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> on end, DSHelper is null");
        return;
    }
    try{
        let uris:Array<string> =  ["city", "datashareproxy://com.acts.ohos.data.datasharetest/appInfo", "datashareproxy://com.acts.ohos.data.datasharetest/test"];
        let result: Array<dataShare.OperationResult> = dsProxyHelper.off("publishedDataChange",
            uris,
            "11", onPublishCallback);
        console.log("[ttt] [DataShareClientTest] <<Consumer>> off end " + JSON.stringify(result));
    }catch(err){
        console.error(` [ttt] offPublishData error: code: ${err.code}, message: ${err.message} `);
    }
}

export async function offPublishDataErr1() {
    console.log("[ttt] [DataShareClientTest] <<Consumer>> offPublishDataErr1 begin");
    if (dsProxyHelper == null) {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> on end, DSHelper is null");
        return;
    }

    try{
        let uris:Array<string> =  ["city", "datashareproxy://com.acts.ohos.data.datasharetest/appInfo", "datashareproxy://com.acts.ohos.data.datasharetest/test"];
        let result: Array<dataShare.OperationResult> = dsProxyHelper.off("publishedDataChange",
            uris,
            "11", undefined);
        console.log("[ttt] [DataShareClientTest] <<Consumer>> off end " + JSON.stringify(result));
    }catch(err){
        console.error(` [ttt] offPublishDataErr1 error: code: ${err.code}, message: ${err.message} `);
    }
}


export async function offPublishDataErr2() {
    console.log("[ttt] [DataShareClientTest] <<Consumer>> off begin");
    if (dsProxyHelper == null) {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> off end, DSHelper is null");
        return;
    }
    let templateId:dataShare.TemplateId = {subscriberId:"111", bundleNameOfOwner:"com.acts.ohos.data.datasharetestclient"}
    try{
        let result:Array<dataShare.OperationResult> = dsProxyHelper.off("rdbDataChange", [cardUri], templateId,undefined);
        console.log("[ttt] [DataShareClientTest] <<Consumer>> off end " + JSON.stringify(result));
    }catch(err){
        console.error(` [ttt] offPublishDataErr2 error: code: ${err.code}, message: ${err.message} `);
    }
}

export function onPublishCallback(err:BusinessError, node:dataShare.PublishedDataChangeNode) {
    console.info("[ttt] [DataShareClientTest] [callback] publishedData change ININ");
    // console.info("[ttt] [DataShareClientTest] [callback] bundleName " + JSON.stringify(node.bundleName));
    console.info("[ttt] [DataShareClientTest] [callback] data size" + node.data.length);
    for (let i = 0; i < node.data.length; i++) {
        // console.info("[ttt] [DataShareClientTest] [callback]" + typeof node.data[i].data);
        console.info("[ttt] [DataShareClientTest] [callback] data " + i + " " + JSON.stringify(node.data[i]));
    }
}

export async function onPublishData() {
    console.log("[ttt] [DataShareClientTest] <<Consumer>> onPublishData begin");
    if (dsProxyHelper == null) {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> on end, DSHelper is null");
        return;
    }
    let uris:Array<string> = ["city", "datashareproxy://com.acts.ohos.data.datasharetest/appInfo", "datashareproxy://com.acts.ohos.data.datasharetest/test"];
    try {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> on begin");
        let result: Array<dataShare.OperationResult> = dsProxyHelper.on("publishedDataChange", uris, "11", onPublishCallback);
        console.log("[ttt] [DataShareClientTest] <<Consumer>> on end " + JSON.stringify(result));
    } catch (e) {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> on err " + JSON.stringify(e));

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
        {key: "city", subscriberId: "11", data: "xian" + Math.random()},
        {key: "datashareproxy://com.acts.ohos.data.datasharetest/appInfo", subscriberId:"11", data:"appinfo is just a test app"},
        {key: "empty", subscriberId: "11", data: "nobody sub" + Math.random()},
        {key: "datashareproxy://com.acts.ohos.data.datasharetest/test", subscriberId:"11", data:"nobody sub " + Math.random()}];

    let nums:number[] = [1,2,3];
    try {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> data length is:", data.length);
        let result: Array<dataShare.OperationResult> = await dsProxyHelper.publish(data, "com.acts.ohos.data.datasharetest");
        console.log("[ttt] [DataShareClientTest] <<Consumer>> publish end " + JSON.stringify(result));
        let publishedData:Array<dataShare.PublishedItem> = await dsProxyHelper.getPublishedData("com.acts.ohos.data.datasharetest")
        console.log("[ttt] [DataShareClientTest] <<Consumer>> getPublishedData end " + JSON.stringify(publishedData));
    } catch (e) {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> publish error " + JSON.stringify(e));
    }
}

export async function publishTest2() {
    console.log("[ttt] [DataShareClientTest] <<Consumer>> publish begin");
    if (dsProxyHelper == null) {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> publish end, DSHelper is null");
        return;
    }
    let data : Array<dataShare.PublishedItem> = [
        {key:"city", subscriberId:"11", data:"xianServer22222"},
        {key:"datashareproxy://com.acts.ohos.data.datasharetest/appInfo", subscriberId:"11", data:"appinfo is just a test app"},
        {key:"empty", subscriberId:"11", data:"nobody sub Server"+222222},
        {key:"datashareproxy://com.acts.ohos.data.datasharetest/test", subscriberId:"11", data:"nobody sub Server"+Math.random()}];

    let nums:number[] = [1,2,3];
    try {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> datasss length is:", data.length);
        let result: Array<dataShare.OperationResult> = await dsProxyHelper.publish(data, "com.acts.ohos.data.datasharetest",2);
        console.log("[ttt] [DataShareClientTest] <<Consumer>> publish end " + JSON.stringify(result));
        let publishedData:Array<dataShare.PublishedItem> = await dsProxyHelper.getPublishedData("com.acts.ohos.data.datasharetest")
        console.log("[ttt] [DataShareClientTest] <<Consumer>> getPublishedData end " + JSON.stringify(publishedData));
    } catch (e) {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> publish error " + JSON.stringify(e));
    }
}

export async function publishTest3() {
    console.log("[ttt] [DataShareClientTest] <<Consumer>> publish begin");
    if (dsProxyHelper == null) {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> publish end, DSHelper is null");
        return;
    }
    let data : Array<dataShare.PublishedItem> = [
        {key:"city", subscriberId:"11", data:"xianClient3333333"},
        {key:"datashareproxy://com.acts.ohos.data.datasharetest/appInfo", subscriberId:"11", data:"appinfo is just a test app"},
        {key:"empty", subscriberId:"11", data:"nobody sub Client"+33333},
        {key:"datashareproxy://com.acts.ohos.data.datasharetest/test", subscriberId:"11", data:"nobody sub Server"+Math.random()}];

    try {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> datasss length is:", data.length);
        let result: Array<dataShare.OperationResult> = await dsProxyHelper.publish(data, "com.acts.ohos.data.datasharetest",3);
        console.log("[ttt] [DataShareClientTest] <<Consumer>> publish end " + JSON.stringify(result));
        let publishedData:Array<dataShare.PublishedItem> = await dsProxyHelper.getPublishedData("com.acts.ohos.data.datasharetest")
        console.log("[ttt] [DataShareClientTest] <<Consumer>> getPublishedData end " + JSON.stringify(publishedData));
    } catch (e) {
        console.log("[ttt] [DataShareClientTest] <<Consumer>> publish error " + JSON.stringify(e));
    }
}