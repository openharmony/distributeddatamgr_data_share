/*
* Copyright (c) 2025 Huawei Device Co., Ltd.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
 
import hilog from '@ohos.hilog';
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect, Level, Size, TestType } from "../../../hypium/index";
import dataShare from '@ohos.data.dataShare';
import { BusinessError } from '@ohos.base';
import common from "@ohos.app.ability.common";
import { AppStorage } from '@ohos.arkui.stateManagement';
 
let domain: int = 0x0000; //日志标识,0x0000作为测试框架的业务标识
let tag: string = 'testTag'; //日志标识字符串,作为tag标识当前runner类下的测试行为
 
let cardUri = "datashare:///com.acts.datasharetest.DataShare/entry/DB00/TBL00?Proxy=false";//非静默访问
let dsProxyHelper: dataShare.DataShareHelper;
let context: common.UIAbilityContext;
 
export default function DataShareWaitTimeTest() {
  describe('DataShareWaitTimeTest', (): void => {
    // Defines a test suite. Two parameters are supported: test suite name and test suite function.
    beforeAll(() => {
      context = AppStorage.get<common.UIAbilityContext>("context") as common.UIAbilityContext;
    })
    beforeEach(() => {
      // Presets an action, which is performed before each unit test case starts.
      // The number of execution times is the same as the number of test cases defined by **it**.
      // This API supports only one parameter: preset action function.
    })
    afterEach(() => {
      // Presets a clear action, which is performed after each unit test case ends.
      // The number of execution times is the same as the number of test cases defined by **it**.
      // This API supports only one parameter: clear action function.
    })
    afterAll(() => {
      // Presets a clear action, which is performed after all test cases of the test suite end.
      // This API supports only one parameter: clear action function.
    })
 
    /**
     * @tc.name DataShareHelperOptions params waitTime test
     * @tc.number SUB_DistributedData_DataShare_SDK_WaitTime_0010
     * @tc.desc isProxy is false and aitTime is -4
     * @tc.size MediumTest
     * @tc.type Function
     * @tc.level Level 1
     */
    it('SUB_DistributedData_DataShare_SDK_WaitTime_0010', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (): Promise<void> => {
      try {
        dsProxyHelper = await dataShare.createDataShareHelper(context, cardUri, {isProxy: false, waitTime: -4});
        console.log("[ttt] [DataShareClientTest] createDataShareHelper is success ");
        dsProxyHelper.close();
      } catch(err: BusinessError) {
        hilog.error(domain, tag, '%{public}s', `[ttt] [DataShareClientTest] createDataShareHelper is failed, err: code: ${err.code}, message: ${err.message}`);
        expect(err.code).assertEqual(15700010);
      }
 
      // try{
      //   await dataShare.createDataShareHelper(context, cardUri, {isProxy: false, waitTime: -4}).then((data:dataShare.DataShareHelper) => {
      //     dsProxyHelper = data;
      //     // if(dsProxyHelper == null){
      //     //   console.log("[ttt] [DataShareClientTest] dsProxyHelper is null ");
      //     //   expect().assertFail()
      //     // }
      //     console.log("[ttt] [DataShareClientTest] createDataShareHelper is success ");
      //     dsProxyHelper.close();
      //   }).catch((e) => {
      //     console.log("[ttt] [DataShareClientTest] createDataShareHelper1111 is failed .code is "+ e.code + " message is : " + e.message);
      //     expect(e.code).assertEqual(15700010);
      //   });
      // } catch (e) {
      //   console.log("[ttt] [DataShareClientTest] createDataShareHelper22222 is failed .code is "+ e.code + " message is : " + e.message);
      //   expect().assertFail();
      // }
    })
 
    /**
     * @tc.name DataShareHelperOptions params waitTime test
     * @tc.number SUB_DistributedData_DataShare_SDK_WaitTime_0020
     * @tc.desc isProxy is false and aitTime is 0
     * @tc.size MediumTest
     * @tc.type Function
     * @tc.level Level 1
     */
    it('SUB_DistributedData_DataShare_SDK_WaitTime_0020', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (): Promise<void> => {
      try {
        dsProxyHelper = await dataShare.createDataShareHelper(context, cardUri, {isProxy: false, waitTime: 0});
        console.log("[ttt] [DataShareClientTest] createDataShareHelper is success ");
        dsProxyHelper.close();
      } catch(err: BusinessError) {
        hilog.error(domain, tag, '%{public}s', `[ttt] [DataShareClientTest] createDataShareHelper is failed, err: code: ${err.code}, message: ${err.message}`);
        expect(err.code).assertEqual(15700010);
      }
    })
 
    /**
     * @tc.name DataShareHelperOptions params waitTime test
     * @tc.number SUB_DistributedData_DataShare_SDK_WaitTime_0030
     * @tc.desc isProxy is false and aitTime is 1
     * @tc.size MediumTest
     * @tc.type Function
     * @tc.level Level 1
     */
    it('SUB_DistributedData_DataShare_SDK_WaitTime_0030', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (): Promise<void> => {
      try {
        dsProxyHelper = await dataShare.createDataShareHelper(context, cardUri, {isProxy: false, waitTime: 1});
        console.log("[ttt] [DataShareClientTest] createDataShareHelper is success ");
        dsProxyHelper.close();
      } catch(err: BusinessError) {
        hilog.error(domain, tag, '%{public}s', `[ttt] [DataShareClientTest] createDataShareHelper is failed, err: code: ${err.code}, message: ${err.message}`);
        expect().assertFail();
      }
    })
 
    /**
     * @tc.name DataShareHelperOptions params waitTime test
     * @tc.number SUB_DistributedData_DataShare_SDK_WaitTime_0040
     * @tc.desc isProxy is false and aitTime is 2
     * @tc.size MediumTest
     * @tc.type Function
     * @tc.level Level 1
     */
    it('SUB_DistributedData_DataShare_SDK_WaitTime_0040', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (): Promise<void> => {
      try {
        dsProxyHelper = await dataShare.createDataShareHelper(context, cardUri, {isProxy: false, waitTime: 2});
        console.log("[ttt] [DataShareClientTest] createDataShareHelper is success ");
        dsProxyHelper.close();
      } catch(err: BusinessError) {
        hilog.error(domain, tag, '%{public}s', `[ttt] [DataShareClientTest] createDataShareHelper is failed, err: code: ${err.code}, message: ${err.message}`);
        expect().assertFail();
      }
    })
 
    /**
     * @tc.name DataShareHelperOptions params waitTime test
     * @tc.number SUB_DistributedData_DataShare_SDK_WaitTime_0050
     * @tc.desc isProxy is false and aitTime is 3
     * @tc.size MediumTest
     * @tc.type Function
     * @tc.level Level 1
     */
    it('SUB_DistributedData_DataShare_SDK_WaitTime_0050', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (): Promise<void> => {
      try {
        dsProxyHelper = await dataShare.createDataShareHelper(context, cardUri, {isProxy: false, waitTime: 3});
        console.log("[ttt] [DataShareClientTest] createDataShareHelper is success ");
        dsProxyHelper.close();
      } catch(err: BusinessError) {
        hilog.error(domain, tag, '%{public}s', `[ttt] [DataShareClientTest] createDataShareHelper is failed, err: code: ${err.code}, message: ${err.message}`);
        expect().assertFail();
      }
    })
 
    /**
     * @tc.name DataShareHelperOptions params waitTime test
     * @tc.number SUB_DistributedData_DataShare_SDK_WaitTime_0060
     * @tc.desc isProxy is false and aitTime is 60
     * @tc.size MediumTest
     * @tc.type Function
     * @tc.level Level 1
     */
    it('SUB_DistributedData_DataShare_SDK_WaitTime_0060', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (): Promise<void> => {
      try {
        dsProxyHelper = await dataShare.createDataShareHelper(context, cardUri, {isProxy: false, waitTime: 60});
        console.log("[ttt] [DataShareClientTest] createDataShareHelper is success ");
        dsProxyHelper.close();
      } catch(err: BusinessError) {
        hilog.error(domain, tag, '%{public}s', `[ttt] [DataShareClientTest] createDataShareHelper is failed, err: code: ${err.code}, message: ${err.message}`);
        expect().assertFail();
      }
    })
 
    /**
     * @tc.name DataShareHelperOptions params waitTime test
     * @tc.number SUB_DistributedData_DataShare_SDK_WaitTime_0070
     * @tc.desc isProxy is false and aitTime is default
     * @tc.size MediumTest
     * @tc.type Function
     * @tc.level Level 1
     */
    it('SUB_DistributedData_DataShare_SDK_WaitTime_0070', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (): Promise<void> => {
      try {
        dsProxyHelper = await dataShare.createDataShareHelper(context, cardUri, {isProxy: false});
        console.log("[ttt] [DataShareClientTest] createDataShareHelper is success ");
        dsProxyHelper.close();
      } catch(err: BusinessError) {
        hilog.error(domain, tag, '%{public}s', `[ttt] [DataShareClientTest] createDataShareHelper is failed, err: code: ${err.code}, message: ${err.message}`);
        expect().assertFail();
      }
    })
 
    /**
     * @tc.name DataShareHelperOptions params waitTime test
     * @tc.number SUB_DistributedData_DataShare_SDK_WaitTime_0080
     * @tc.desc isProxy is false and aitTime is default
     * @tc.size MediumTest
     * @tc.type Function
     * @tc.level Level 1
     */
    it('SUB_DistributedData_DataShare_SDK_WaitTime_0080', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (): Promise<void> => {
      try {
        dsProxyHelper = await dataShare.createDataShareHelper(context, cardUri, {isProxy: true});
        console.log("[ttt] [DataShareClientTest] createDataShareHelper is success ");
        dsProxyHelper.close();
      } catch(err: BusinessError) {
        hilog.error(domain, tag, '%{public}s', `[ttt] [DataShareClientTest] createDataShareHelper is failed, err: code: ${err.code}, message: ${err.message}`);
        expect().assertFail();
      }
    })
 
    /**
     * @tc.name DataShareHelperOptions params waitTime test
     * @tc.number SUB_DistributedData_DataShare_SDK_WaitTime_0090
     * @tc.desc isProxy is false and aitTime is undefined
     * @tc.size MediumTest
     * @tc.type Function
     * @tc.level Level 1
     */
    it('SUB_DistributedData_DataShare_SDK_WaitTime_0090', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (): Promise<void> => {
      try {
        dsProxyHelper = await dataShare.createDataShareHelper(context, cardUri, {isProxy: false, waitTime: undefined});
        console.log("[ttt] [DataShareClientTest] createDataShareHelper is success ");
        dsProxyHelper.close();
      } catch(err: BusinessError) {
        hilog.error(domain, tag, '%{public}s', `[ttt] [DataShareClientTest] createDataShareHelper is failed, err: code: ${err.code}, message: ${err.message}`);
        expect().assertFail();
      }
    })
 
    /**
     * @tc.name DataShareHelperOptions params waitTime test
     * @tc.number SUB_DistributedData_DataShare_SDK_WaitTime_0100
     * @tc.desc isProxy is false and aitTime is 1.5
     * @tc.size MediumTest
     * @tc.type Function
     * @tc.level Level 1
     */
    // it('SUB_DistributedData_DataShare_SDK_WaitTime_0100', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (): Promise<void> => {
    //   await dataShare.createDataShareHelper(context, cardUri, {isProxy : false, waitTime: 1.5}).then((data:dataShare.DataShareHelper)=>{
    //     dsProxyHelper = data;
    //     // if(dsProxyHelper == null){
    //     //   console.log("[ttt] [DataShareClientTest] dsProxyHelper is null ");
    //     //   expect().assertFail();
    //     // }
    //     console.log("[ttt] [DataShareClientTest] createDataShareHelper is succeed ");
    //     dsProxyHelper.close();
    //   }).catch((e)=>{
    //     console.log("[ttt] [DataShareClientTest] createDataShareHelper is failed .code is "+ e.code + " message is : " + e.message);
    //     expect().assertFail();
    //   });
    // }) // waitTime现在为int类型
 
    /**
     * @tc.name DataShareHelperOptions params waitTime test
     * @tc.number SUB_DistributedData_DataShare_SDK_WaitTime_0110
     * @tc.desc isProxy is false and aitTime is 2147483647
     * @tc.size MediumTest
     * @tc.type Function
     * @tc.level Level 1
     */
    it('SUB_DistributedData_DataShare_SDK_WaitTime_0110', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (): Promise<void> => {
      try {
        dsProxyHelper = await dataShare.createDataShareHelper(context, cardUri, {isProxy: false, waitTime: 2147483647 });
        console.log("[ttt] [DataShareClientTest] createDataShareHelper is success ");
        dsProxyHelper.close();
      } catch(err: BusinessError) {
        hilog.error(domain, tag, '%{public}s', `[ttt] [DataShareClientTest] createDataShareHelper is failed, err: code: ${err.code}, message: ${err.message}`);
        expect().assertFail();
      }
    })
 
    /**
     * @tc.name DataShareHelperOptions params waitTime test
     * @tc.number SUB_DistributedData_DataShare_SDK_WaitTime_0120
     * @tc.desc isProxy is false and aitTime is 2147483647+1
     * @tc.size MediumTest
     * @tc.type Function
     * @tc.level Level 1
     */
    // it('SUB_DistributedData_DataShare_SDK_WaitTime_0120', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (): Promise<void> => {
    //   await dataShare.createDataShareHelper(context, cardUri, {isProxy: false , waitTime: 2147483648}).then((data:dataShare.DataShareHelper)=>{
    //     dsProxyHelper = data;
    //     // if(dsProxyHelper == null){
    //     //   console.log("[ttt] [DataShareClientTest] dsProxyHelper is null ");
    //     //   expect().assertFail();
    //     // }
    //     console.log("[ttt] [DataShareClientTest] createDataShareHelper is succeed ");
    //     dsProxyHelper.close();
    //   }).catch((e)=>{
    //     console.log("[ttt] [DataShareClientTest] createDataShareHelper is failed .code is "+ e.code + " message is : " + e.message);
    //     expect(e.code).assertEqual(15700010);
    //   });
    // })
 
    /**
     * @tc.name DataShareHelperOptions params waitTime test
     * @tc.number SUB_DistributedData_DataShare_SDK_WaitTime_0130
     * @tc.desc options is null
     * @tc.size MediumTest
     * @tc.type Function
     * @tc.level Level 1
     */
    // it('SUB_DistributedData_DataShare_SDK_WaitTime_0130', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (): Promise<void> => {
    //   let options : dataShare.DataShareHelperOptions | null = null;
    //   await dataShare.createDataShareHelper(context, cardUri, options).then((data:dataShare.DataShareHelper)=>{
    //     dsProxyHelper = data;
    //     // if(dsProxyHelper == null){
    //     //   console.log("[ttt] [DataShareClientTest] dsProxyHelper is null ");
    //     //   expect().assertFail();
    //     // }
    //     console.log("[ttt] [DataShareClientTest] createDataShareHelper is succeed ");
    //     dsProxyHelper.close();
    //   }).catch((e)=>{
    //     console.log("[ttt] [DataShareClientTest] createDataShareHelper is failed .code is "+ e.code + " message is : " + e.message);
    //     expect().assertFail();
    //   });
    // })
 
    /**
     * @tc.name DataShareHelperOptions params waitTime test
     * @tc.number SUB_DistributedData_DataShare_SDK_WaitTime_0140
     * @tc.desc options is undefined
     * @tc.size MediumTest
     * @tc.type Function
     * @tc.level Level 1
     */
    it('SUB_DistributedData_DataShare_SDK_WaitTime_0140', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (): Promise<void> => {
      let options : dataShare.DataShareHelperOptions | undefined = undefined;
      try {
        dsProxyHelper = await dataShare.createDataShareHelper(context, cardUri, options);
        console.log("[ttt] [DataShareClientTest] createDataShareHelper is success ");
        dsProxyHelper.close();
      } catch(err: BusinessError) {
        hilog.error(domain, tag, '%{public}s', `[ttt] [DataShareClientTest] createDataShareHelper is failed, err: code: ${err.code}, message: ${err.message}`);
        expect().assertFail();
      }
    })
 
    /**
     * @tc.name DataShareHelperOptions params waitTime test
     * @tc.number SUB_DistributedData_DataShare_SDK_WaitTime_0010
     * @tc.desc isProxy is true and aitTime is -4
     * @tc.size MediumTest
     * @tc.type Function
     * @tc.level Level 1
     */
    it('SUB_DistributedData_DataShare_SDK_WaitTime_0150', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (): Promise<void> => {
      try {
        dsProxyHelper = await dataShare.createDataShareHelper(context, cardUri, {isProxy: true, waitTime: -4});
        console.log("[ttt] [DataShareClientTest] createDataShareHelper is success ");
        dsProxyHelper.close();
      } catch(err: BusinessError) {
        hilog.error(domain, tag, '%{public}s', `[ttt] [DataShareClientTest] createDataShareHelper is failed, err: code: ${err.code}, message: ${err.message}`);
        expect(err.code).assertEqual(15700010);
      }
    })
 
    /**
     * @tc.name DataShareHelperOptions params waitTime test
     * @tc.number SUB_DistributedData_DataShare_SDK_WaitTime_0020
     * @tc.desc isProxy is false and aitTime is 0
     * @tc.size MediumTest
     * @tc.type Function
     * @tc.level Level 1
     */
    it('SUB_DistributedData_DataShare_SDK_WaitTime_0160', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (): Promise<void> => {
      try {
        dsProxyHelper = await dataShare.createDataShareHelper(context, cardUri, {isProxy: true, waitTime: 0});
        console.log("[ttt] [DataShareClientTest] createDataShareHelper is success ");
        dsProxyHelper.close();
      } catch(err: BusinessError) {
        hilog.error(domain, tag, '%{public}s', `[ttt] [DataShareClientTest] createDataShareHelper is failed, err: code: ${err.code}, message: ${err.message}`);
        expect(err.code).assertEqual(15700010);
      }
    })
 
    /**
     * @tc.name DataShareHelperOptions params waitTime test
     * @tc.number SUB_DistributedData_DataShare_SDK_WaitTime_0030
     * @tc.desc isProxy is false and aitTime is 1
     * @tc.size MediumTest
     * @tc.type Function
     * @tc.level Level 1
     */
    it('SUB_DistributedData_DataShare_SDK_WaitTime_0170', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (): Promise<void> => {
      try {
        dsProxyHelper = await dataShare.createDataShareHelper(context, cardUri, {isProxy: true, waitTime: 1});
        console.log("[ttt] [DataShareClientTest] createDataShareHelper is success ");
        dsProxyHelper.close();
      } catch(err: BusinessError) {
        hilog.error(domain, tag, '%{public}s', `[ttt] [DataShareClientTest] createDataShareHelper is failed, err: code: ${err.code}, message: ${err.message}`);
        expect().assertFail();
      }
    })
 
    /**
     * @tc.name DataShareHelperOptions params waitTime test
     * @tc.number SUB_DistributedData_DataShare_SDK_WaitTime_0040
     * @tc.desc isProxy is false and aitTime is 2
     * @tc.size MediumTest
     * @tc.type Function
     * @tc.level Level 1
     */
    it('SUB_DistributedData_DataShare_SDK_WaitTime_0180', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (): Promise<void> => {
      try {
        dsProxyHelper = await dataShare.createDataShareHelper(context, cardUri, {isProxy: true, waitTime: 2});
        console.log("[ttt] [DataShareClientTest] createDataShareHelper is success ");
        dsProxyHelper.close();
      } catch(err: BusinessError) {
        hilog.error(domain, tag, '%{public}s', `[ttt] [DataShareClientTest] createDataShareHelper is failed, err: code: ${err.code}, message: ${err.message}`);
        expect().assertFail();
      }
    })
 
    /**
     * @tc.name DataShareHelperOptions params waitTime test
     * @tc.number SUB_DistributedData_DataShare_SDK_WaitTime_0050
     * @tc.desc isProxy is false and aitTime is 3
     * @tc.size MediumTest
     * @tc.type Function
     * @tc.level Level 1
     */
    it('SUB_DistributedData_DataShare_SDK_WaitTime_0190', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (): Promise<void> => {
      try {
        dsProxyHelper = await dataShare.createDataShareHelper(context, cardUri, {isProxy: true, waitTime: 3});
        console.log("[ttt] [DataShareClientTest] createDataShareHelper is success ");
        dsProxyHelper.close();
      } catch(err: BusinessError) {
        hilog.error(domain, tag, '%{public}s', `[ttt] [DataShareClientTest] createDataShareHelper is failed, err: code: ${err.code}, message: ${err.message}`);
        expect().assertFail();
      }
    })
 
    /**
     * @tc.name DataShareHelperOptions params waitTime test
     * @tc.number SUB_DistributedData_DataShare_SDK_WaitTime_0060
     * @tc.desc isProxy is false and aitTime is 60
     * @tc.size MediumTest
     * @tc.type Function
     * @tc.level Level 1
     */
    it('SUB_DistributedData_DataShare_SDK_WaitTime_0200', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (): Promise<void> => {
      try {
        dsProxyHelper = await dataShare.createDataShareHelper(context, cardUri, {isProxy: true, waitTime: 60});
        console.log("[ttt] [DataShareClientTest] createDataShareHelper is success ");
        dsProxyHelper.close();
      } catch(err: BusinessError) {
        hilog.error(domain, tag, '%{public}s', `[ttt] [DataShareClientTest] createDataShareHelper is failed, err: code: ${err.code}, message: ${err.message}`);
        expect().assertFail();
      }
    })
 
    /**
     * @tc.name DataShareHelperOptions params waitTime test
     * @tc.number SUB_DistributedData_DataShare_SDK_WaitTime_0090
     * @tc.desc isProxy is false and aitTime is undefined
     * @tc.size MediumTest
     * @tc.type Function
     * @tc.level Level 1
     */
    it('SUB_DistributedData_DataShare_SDK_WaitTime_0230', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (): Promise<void> => {
      try {
        dsProxyHelper = await dataShare.createDataShareHelper(context, cardUri, {isProxy: true, waitTime: undefined});
        console.log("[ttt] [DataShareClientTest] createDataShareHelper is success ");
        dsProxyHelper.close();
      } catch(err: BusinessError) {
        hilog.error(domain, tag, '%{public}s', `[ttt] [DataShareClientTest] createDataShareHelper is failed, err: code: ${err.code}, message: ${err.message}`);
        expect().assertFail();
      }
    })
 
    /**
     * @tc.name DataShareHelperOptions params waitTime test
     * @tc.number SUB_DistributedData_DataShare_SDK_WaitTime_0100
     * @tc.desc isProxy is false and aitTime is 1.5
     * @tc.size MediumTest
     * @tc.type Function
     * @tc.level Level 1
     */
    // it('SUB_DistributedData_DataShare_SDK_WaitTime_0240', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (): Promise<void> => {
    //   await dataShare.createDataShareHelper(context, cardUri, {isProxy: true, waitTime: 1.5}).then((data:dataShare.DataShareHelper)=>{
    //     dsProxyHelper = data;
    //     // if(dsProxyHelper == null){
    //     //   console.log("[ttt] [DataShareClientTest] dsProxyHelper is null ");
    //     //   expect().assertFail();
    //     // }
    //     console.log("[ttt] [DataShareClientTest] createDataShareHelper is succeed ");
    //     dsProxyHelper.close();
    //   }).catch((e)=>{
    //     console.log("[ttt] [DataShareClientTest] createDataShareHelper is failed .code is "+ e.code + " message is : " + e.message);
    //     expect().assertFail();
    //   });
    // })
 
    /**
     * @tc.name DataShareHelperOptions params waitTime test
     * @tc.number SUB_DistributedData_DataShare_SDK_WaitTime_0110
     * @tc.desc isProxy is false and aitTime is 2147483647
     * @tc.size MediumTest
     * @tc.type Function
     * @tc.level Level 1
     */
    it('SUB_DistributedData_DataShare_SDK_WaitTime_0250', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (): Promise<void> => {
      try {
        dsProxyHelper = await dataShare.createDataShareHelper(context, cardUri, {isProxy: true, waitTime: 2147483647});
        console.log("[ttt] [DataShareClientTest] createDataShareHelper is success ");
        dsProxyHelper.close();
      } catch(err: BusinessError) {
        hilog.error(domain, tag, '%{public}s', `[ttt] [DataShareClientTest] createDataShareHelper is failed, err: code: ${err.code}, message: ${err.message}`);
        expect().assertFail();
      }
    })
 
    /**
     * @tc.name DataShareHelperOptions params waitTime test
     * @tc.number SUB_DistributedData_DataShare_SDK_WaitTime_0120
     * @tc.desc isProxy is false and aitTime is 2147483647+1
     * @tc.size MediumTest
     * @tc.type Function
     * @tc.level Level 1
     */
    // it('SUB_DistributedData_DataShare_SDK_WaitTime_0260', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (): Promise<void> => {
    //   await dataShare.createDataShareHelper(context, cardUri, {isProxy: true, waitTime: 2147483648}).then((data:dataShare.DataShareHelper)=>{
    //     dsProxyHelper = data;
    //     // if(dsProxyHelper == null){
    //     //   console.log("[ttt] [DataShareClientTest] dsProxyHelper is null ");
    //     //   expect().assertFail();
    //     // }
    //     console.log("[ttt] [DataShareClientTest] createDataShareHelper is succeed ");
    //     dsProxyHelper.close();
    //   }).catch((e)=>{
    //     console.log("[ttt] [DataShareClientTest] createDataShareHelper is failed .code is "+ e.code + " message is : " + e.message);
    //     expect(e.code).assertEqual(15700010);
    //   });
    // })
  })
}