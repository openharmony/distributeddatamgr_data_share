/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

let dseUri = ("datashare:///com.acts.datasharetest");
let uri = ("datashare://com.acts.datasharetest/entry/DB00/TBL00?Proxy=true");

export function onCallback() {
    console.info("[ttt] [DataShareTest] <<Consumer>> **** Observer on callback ****");
}

export function offCallback() {
    console.info("[ttt] [DataShareTest] <<Consumer>> **** Observer off callback ****");
}

export default class MainAbility extends Ability {
    onCreate(want, launchParam) {
        // Ability is creating, initialize resources for this ability
        console.log("[ttt] [DataShareTest] <<Consumer>> MainAbility onCreate")
        globalThis.abilityWant = want;
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
        
        this.connectDataShareExtAbility();    
        this.disconnectDataShareExtAbility();
        this.on();
        this.off();
        this.openFile();
        this.query();
        this.insert();
        this.update();
        this.delete();
        this.batchInsert();
        this.notifyChange();
        this.getType();
        this.getFileTypes();
        this.normalizeUri();
        this.denormalizeUri();

        windowStage.setUIContent(this.context, "pages/index", null)
    }

    connectDataShareExtAbility() {
        globalThis.connectDataShareExtAbility = (async () => {
            console.log("[ttt] [DataShareTest] <<Consumer>> connectDataShareExtAbility begin");
            globalThis.dsHelper = await dataShare.createDataShareHelper(this.context, dseUri);
            console.log("[ttt] [DataShareTest] <<Consumer>> connectDataShareExtAbility end");
        })
    }

    disconnectDataShareExtAbility() {
        globalThis.disconnectDataShareExtAbility = (async () => {
            console.log("[ttt] [DataShareTest] <<Consumer>> disconnectDataShareExtAbility begin");
            globalThis.dsHelper = null;
            console.log("[ttt] [DataShareTest] <<Consumer>> disconnectDataShareExtAbility end");
        })

    }

    on() {
        globalThis.on = (async () => {
            console.log("[ttt] [DataShareTest] <<Consumer>> on begin");
            if (globalThis.dsHelper == null) {
                console.log("[ttt] [DataShareTest] <<Consumer>> on end, DSHelper is null");
                return;
            }
            await globalThis.dsHelper.on("dataChange", uri, onCallback);
            console.log("[ttt] [DataShareTest] <<Consumer>> on end");
        }) 
    }
    
    off() {
        globalThis.off = (async () => {
            console.log("[ttt] [DataShareTest] <<Consumer>> off begin");
            if (globalThis.dsHelper == null) {
                console.log("[ttt] [DataShareTest] <<Consumer>> off end, DSHelper is null");
                return;
            }
            await globalThis.dsHelper.off("dataChange", uri);
            await globalThis.dsHelper.off("dataChange", uri, offCallback);
            console.log("[ttt] [DataShareTest] <<Consumer>> off end");
        })
    }

    openFile() {
        globalThis.openFile = (async () => {
            console.log("[ttt] [DataShareTest] <<Consumer>> openFile begin");
            if (globalThis.dsHelper == null) {
                console.log("[ttt] [DataShareTest] <<Consumer>> openFile end, DSHelper is null");
                return;
            }
            let result = await globalThis.dsHelper.openFile(dseUri, "rw");
            console.log("[ttt] [DataShareTest] <<Consumer>> openFile end, result:" + result);
            return result;
        })
    }

    query() {
        globalThis.query = (async () => {
            console.log("[ttt] [DataShareTest] <<Consumer>> query begin");
            let da =  new dataSharePredicates.DataSharePredicates();
            if (da == null || da == undefined) {
                console.log("[ttt] [DataShareTest] <<Consumer>> da is null or undefined");
                return;
            }
            let count = 0;
            if (globalThis.dsHelper == null) {
                console.log("[ttt] [DataShareTest] <<Consumer>> query end, DSHelper is null");
                return;
            }
            da.equalTo("name", "ZhangSan");
            let result = await globalThis.dsHelper.query(dseUri, da, ["*"]);
            if (result != undefined) {
                count = result.rowCount;
            }
            console.log("[ttt] [DataShareTest] <<Consumer>> query end, count:" + count);
            return count;
        })
    }

    insert() {
        globalThis.insert = (async () => {
            console.log("[ttt] [DataShareTest] <<Consumer>> insert begin");
            if (globalThis.dsHelper == null) {
                console.log("[ttt] [DataShareTest] <<Consumer>> insert end, DSHelper is null");
                return;
            }
            let vb = {"name":"ZhangSan", "age": 21}

            console.log("[ttt] [DataShareTest] <<Consumer>> insert vb:" + JSON.stringify(vb));
            let ret = await globalThis.dsHelper.insert(dseUri, vb);
            await console.log("[ttt] [DataShareTest] <<Consumer>> insert end, ret:" + ret);
            return ret;
        })
    }

    update() {
        globalThis.update = (async () => {
            console.log("[ttt] [DataShareTest] <<Consumer>> update begin");
            if (globalThis.dsHelper == null) {
                console.log("[ttt] [DataShareTest] <<Consumer>> update end, DSHelper is null");
                return;
            }
            console.log("[ttt] [DataShareTest] <<Consumer>> update begin RPF666");
            let da =  new dataSharePredicates.DataSharePredicates();
            console.log("[ttt] [DataShareTest] <<Consumer>> update begin RPF777");
            if (da == null || da == undefined) {
                console.log("[ttt] [DataShareTest] <<Consumer>> da is null or undefined");
                return;
            }
            da.equalTo("name", "ZhangSan");

            let ret = await globalThis.dsHelper.update(dseUri, da, {"name":"ZhangSan", "age":31});
            console.log("[ttt] [DataShareTest] <<Consumer>> update end, result:" + ret);
            return ret;
        })
    }

    delete() {
        globalThis.delete = (async () => {
            console.log("[ttt] [DataShareTest] <<Consumer>> delete begin");
            if (globalThis.dsHelper == null) {
                console.log("[ttt] [DataShareTest] <<Consumer>> delete end, DSHelper is null");
                return;
            }
            let da =  new dataSharePredicates.DataSharePredicates();
            if (da == null || da == undefined) {
                console.log("[ttt] [DataShareTest] <<Consumer>> da is null or undefined");
                return;
            }
            da.equalTo("name", "ZhangSan");
            let ret = await globalThis.dsHelper.delete(dseUri, da);
            console.log("[ttt] [DataShareTest] <<Consumer>> delete end, result:" + ret);
            return ret;
        })
    }

    batchInsert() {
        globalThis.batchInsert = (async () => {
            console.log("[ttt] [DataShareTest] <<Consumer>> batchInsert begin");
            if (globalThis.dsHelper == null) {
                console.log("[ttt] [DataShareTest] <<Consumer>> batchInsert end, DSHelper is null");
                return;
            }
            let ar = new Uint8Array([1,2,3,4,5,6]);
            let arr = new Uint8Array([4,5,6,7]);
            let people = new Array(
                {"name": "LiSi", "age": 41, "Binary": ar},
                {"name": "WangWu", "age": 21, "Binary": arr},
                {"name": "ZhaoLiu", "age": 61, "Binary": arr});
            let ret = await globalThis.dsHelper.batchInsert(dseUri, people);
            console.log("[ttt] [DataShareTest] <<Consumer>> batchInsert end, result:" + ret);
            return ret;
        })
    }

    notifyChange() {
        globalThis.notifyChange = (async () => {
            console.log("[ttt] [DataShareTest] <<Consumer>> notifyChange begin");
            if (globalThis.dsHelper == null) {
                console.log("[ttt] [DataShareTest] <<Consumer>> notifyChange end, DSHelper is null");
                return;
            }
            await globalThis.dsHelper.notifyChange(dseUri);
            console.log("[ttt] [DataShareTest] <<Consumer>> notifyChange end");
        })
    }
    
    getType() {
        globalThis.getType = (async () => {
            console.log("[ttt] [DataShareTest] <<Consumer>> getType begin");
            if (globalThis.dsHelper == null) {
                console.log("[ttt] [DataShareTest] <<Consumer>> getType end, DSHelper is null");
                return;
            }
            let result = await globalThis.dsHelper.getType(dseUri);
            console.log("[ttt] [DataShareTest] <<Consumer>> getType end, result:" + result);
            return result;
        })
    }

    getFileTypes() {
        globalThis.getFileTypes = (async () => {
            console.log("[ttt] [DataShareTest] <<Consumer>> getFileTypes begin");
            if (globalThis.dsHelper == null) {
                console.log("[ttt] [DataShareTest] <<Consumer>> getFileTypes end, DSHelper is null");
                return;
            }
            let result = await globalThis.dsHelper.getFileTypes(dseUri, "image/*");
            console.log("[ttt] [DataShareTest] <<Consumer>> getFileTypes end, result:" + result);
            return result;
        })
    }

    normalizeUri() {
        globalThis.normalizeUri = (async () => {
            console.log("[ttt] [DataShareTest] <<Consumer>> normalizeUri begin");
            if (globalThis.dsHelper == null) {
                console.log("[ttt] [DataShareTest] <<Consumer>> normalizeUri end, DSHelper is null");
                return;
            }
            let result = await globalThis.dsHelper.normalizeUri(dseUri);
            console.log("[ttt] [DataShareTest] <<Consumer>> normalizeUri end, result:" + result);
            return result;
        })
    }

    denormalizeUri() {
        globalThis.denormalizeUri = (async () => {
            console.log("[ttt] [DataShareTest] <<Consumer>> denormalizeUri begin");
            if (globalThis.dsHelper == null) {
                console.log("[ttt] [DataShareTest] <<Consumer>> denormalizeUri end, DSHelper is null");
                return;
            }
            let result = await globalThis.dsHelper.denormalizeUri(dseUri);
            console.log("[ttt] [DataShareTest] <<Consumer>> denormalizeUri end, result:" + result);
            return result;
        })
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