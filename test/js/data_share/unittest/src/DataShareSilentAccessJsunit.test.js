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
     * @tc.name Normal test case of enableSilentProxy: test enabling silent proxy with valid parameters
     * @tc.desc Verify that the enableSilentProxy interface works correctly when provided with a valid context and uri
     * @tc.size MediumTest
     * @tc.type Function
     * @tc.level Level 2
     * @tc.require None
     * @tc.precon
     *   1. A valid context object is initialized
     *   2. A valid uri (target for silent proxy) is predefined
     *   3. The dataShare module is imported and available
     * @tc.step
     *   1. Call dataShare.enableSilentProxy with the valid context and uri
     *   2. Wait for the asynchronous operation to complete
     *   3. Check if the operation executes without throwing an exception
     * @tc.expect
     *   1. The enableSilentProxy method completes successfully with no errors
     *   2. No exception is thrown during the execution
     *   3. The console logs "enableSilentProxy done" indicating success
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
     * @tc.name Normal test case of enableSilentProxy: test enabling silent proxy with empty uri
     * @tc.desc Verify that the enableSilentProxy interface handles an empty uri correctly
     * @tc.size MediumTest
     * @tc.type Function
     * @tc.level Level 2
     * @tc.require None
     * @tc.precon
     *   1. A valid context object is initialized
     *   2. The dataShare module is imported and available
     * @tc.step
     *   1. Call dataShare.enableSilentProxy with the valid context and an empty string as uri
     *   2. Wait for the asynchronous operation to complete
     *   3. Check if the operation executes without throwing an exception
     * @tc.expect
     *   1. The enableSilentProxy method completes successfully with no errors
     *   2. No exception is thrown despite the empty uri
     *   3. The console logs "enableSilentProxy done" indicating handling of empty uri
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
     * @tc.name Normal test case of disableSilentProxy: test disabling silent proxy with valid parameters
     * @tc.desc Verify that the disableSilentProxy interface works correctly when provided with a valid context and uri
     * @tc.size MediumTest
     * @tc.type Function
     * @tc.level Level 2
     * @tc.require None
     * @tc.precon
     *   1. A valid context object is initialized
     *   2. A valid uri (target for silent proxy) is predefined (matching the one used in enable)
     *   3. The dataShare module is imported and available
     * @tc.step
     *   1. Call dataShare.disableSilentProxy with the valid context and uri
     *   2. Wait for the asynchronous operation to complete
     *   3. Check if the operation executes without throwing an exception
     * @tc.expect
     *   1. The disableSilentProxy method completes successfully with no errors
     *   2. No exception is thrown during the execution
     *   3. The console logs "disableSilentProxy done" indicating success
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
     * @tc.name Normal test case of disableSilentProxy: test disabling silent proxy with empty uri
     * @tc.desc Verify that the disableSilentProxy interface handles an empty uri correctly
     * @tc.size MediumTest
     * @tc.type Function
     * @tc.level Level 2
     * @tc.require None
     * @tc.precon
     *   1. A valid context object is initialized
     *   2. The dataShare module is imported and available
     * @tc.step
     *   1. Call dataShare.disableSilentProxy with the valid context and an empty string as uri
     *   2. Wait for the asynchronous operation to complete
     *   3. Check if the operation executes without throwing an exception
     * @tc.expect
     *   1. The disableSilentProxy method completes successfully with no errors
     *   2. No exception is thrown despite the empty uri
     *   3. The console logs "disableSilentProxy done" indicating handling of empty uri
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