/*
 * Copyright (C) 2026 Huawei Device Co., Ltd.
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
 
import { describe, it, expect, TestType, Size, Level, beforeAll, afterAll, beforeEach, afterEach} from "../../../hypium/index";
import Utils from './Util.test';
import dataShare from '@ohos.data.dataShare';
import dataSharePredicates from '@ohos.data.dataSharePredicates';
import DataShareResultSet from '@ohos.data.DataShareResultSet';
import { ValuesBucket } from '@ohos.data.ValuesBucket';
import common from '@ohos.app.ability.common';
import { BusinessError } from '@ohos.base';
import { AppStorage } from '@ohos.arkui.stateManagement';
 
let context: common.UIAbilityContext;
let uri = ("datashare:///com.acts.datasharetest.DataShare");
let dsProxyHelper: dataShare.DataShareHelper;
const TAG = "[ttt] "
export default function InsertMacAddressTest() {
  describe('InsertMacAddressTest', (): void => {
    beforeAll(async () => {
      context = AppStorage.get<common.UIAbilityContext>("context") as common.UIAbilityContext;
      try {
        dsProxyHelper = await dataShare.createDataShareHelper(context, uri, {
          isProxy: false
        });
        console.log("[ttt] [DataShareClientTest] <<Consumer>> dsProxyHelper " + JSON.stringify(dsProxyHelper));
      } catch (err: BusinessError) {
        console.error('[ttt] [DataShareClientTest] <<Consumer>> createDataShareHelper error' + err.code + "  " + err.message);
        expect().assertFail();
      }
      await Utils.msSleep(2000);
    })
    beforeEach(() => {
    })
    afterEach(async () => {
      let da = new dataSharePredicates.DataSharePredicates();
      await dsProxyHelper.delete(uri, da);
    })
    afterAll(async () => {
      await dsProxyHelper.close();
    })
 
    /**
     * @tc.name       : testMacAddressTest0010
     * @tc.number     : SUB_DistributedData_SDK_DataShare_ApiTest_0070
     * @tc.desc       : Insert Mac Address test
     * @tc.size       : MediumTest
     * @tc.type       : FUNCTION
     * @tc.level      : Level 3
     */
    it('testMacAddressTest0010', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL3, async (): Promise<void> => {
      try{
        let vb: ValuesBucket = {
          "name": "58:f2:fc:79:2a:e2",
        };
        let ret = await dsProxyHelper.insert(uri, vb);
        console.log(TAG + "[DataShareClientTest] <<Consumer>> insert end rowNum is " + ret);
        let da = new dataSharePredicates.DataSharePredicates();
        da.equalTo("name","58:f2:fc:79:2a:e2")
        let resultSet=await dsProxyHelper.query(uri,da,[]);
        expect((resultSet as DataShareResultSet).rowCount == 1).assertTrue();
      }catch (err) {
        console.error(TAG + "[DataShareClientTest] <<Consumer>> testMacAddressTest0010 error " + JSON.stringify(err));
        expect().assertFail()
      }
    });
 
  })
}
 