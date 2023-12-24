/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'
import ability_featureAbility from '@ohos.ability.featureAbility'
import dataShare from '@ohos.data.dataShare'
var context = ability_featureAbility.getContext()

const TAG = "[DATA_SHARE_JSKITS_TEST]"
let uri = ("datashare://com.acts.datasharetest/entry/DB00/TBL00?Proxy=true");

describe('dataShareTest', function () {
    beforeAll(async function () {
        console.info(TAG + 'beforeAll')
    })

    beforeEach(function () {
        console.info(TAG + 'beforeEach')
    })

    afterEach(async function () {
        console.info(TAG + 'afterEach')
    })

    afterAll(async function () {
        console.info(TAG + 'afterAll')
    })

    console.log(TAG + "*************Unit Test Begin*************");

    /**
     * @tc.number testSilentAccess0001
     * @tc.name Normal test case of enableSilentProxy, test enable silent proxy;
     * @tc.desc Execute enableSilentProxy
     * @tc.size MediumTest
     * @tc.type Function
     * @tc.level Level 2
     */
    it('testSilentAccess0001', 0, async function () {
        console.log(TAG + "************* testSilentAccess0001 start *************");
        try {
            await dataShare.enableSilentProxy(context, uri);
            console.log(TAG + "enableSilentProxy done");
        } catch (e) {
            expect().assertFail();
        }
        console.log(TAG + "************* testSilentAccess0001 end   *************");
    })

    /**
     * @tc.number testSilentAccess0002
     * @tc.name Normal test case of enableSilentProxy, test enable silent proxy, uri is null;
     * @tc.desc Execute enableSilentProxy
     * @tc.size MediumTest
     * @tc.type Function
     * @tc.level Level 2
     */
    it('testSilentAccess0002', 0, async function () {
        console.log(TAG + "************* testSilentAccess0002 start *************");
        try {
            await dataShare.enableSilentProxy(context, "");
            console.log(TAG + "enableSilentProxy done");
        } catch (e) {
            expect().assertFail();
        }
        console.log(TAG + "************* testSilentAccess0002 end   *************");
    })

    /**
     * @tc.number testSilentAccess0003
     * @tc.name Normal test case of disableSilentProxy, test disable silent proxy;
     * @tc.desc Execute disableSilentProxy
     * @tc.size MediumTest
     * @tc.type Function
     * @tc.level Level 2
     */
    it('testSilentAccess0003', 0, async function () {
        console.log(TAG + "************* testSilentAccess0003 start *************");
        try {
            await dataShare.disableSilentProxy(context, uri);
            console.log(TAG + "disableSilentProxy done");
        } catch (e) {
            expect().assertFail();
        }
        console.log(TAG + "************* testSilentAccess0003 end   *************");
    })

    /**
     * @tc.number testSilentAccess0004
     * @tc.name Normal test case of disableSilentProxy, test disable silent proxy, uri is null;
     * @tc.desc Execute disableSilentProxy
     * @tc.size MediumTest
     * @tc.type Function
     * @tc.level Level 2
     */
    it('testSilentAccess0004', 0, async function () {
        console.log(TAG + "************* testSilentAccess0004 start *************");
        try {
            await dataShare.disableSilentProxy(context, "");
            console.log(TAG + "disableSilentProxy done");
        } catch (e) {
            expect().assertFail();
        }
        console.log(TAG + "************* testSilentAccess0004 end   *************");
    })

    console.log(TAG + "*************Unit Test End*************");
})