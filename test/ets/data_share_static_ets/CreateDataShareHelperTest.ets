// /*
//  * Copyright (C) 2025 Huawei Device Co., Ltd.
//  * Licensed under the Apache License, Version 2.0 (the "License");
//  * you may not use this file except in compliance with the License.
//  * You may obtain a copy of the License at
//  *
//  *     http://www.apache.org/licenses/LICENSE-2.0
//  *
//  * Unless required by applicable law or agreed to in writing, software
//  * distributed under the License is distributed on an "AS IS" BASIS,
//  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  * See the License for the specific language governing permissions and
//  * limitations under the License.
//  */
//
// import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect, TestType, Size, Level } from '@ohos/hypium';
// import dataShare from '@ohos.data.dataShare'
// import common from "@ohos.app.ability.common"
//
// let uri = ("datashareproxy://com.acts.ohos.data.datasharetest/test");
// let dsProxyHelper: dataShare.DataShareHelper | undefined = undefined
// let context: common.UIAbilityContext
//
// export default function createDataShareHelperTest() {
//   describe('createDataShareHelperTest', () => {
//     beforeAll(async () => {
//       context = AppStorage.get<common.UIAbilityContext>("TestAbilityContext") as common.UIAbilityContext
//       try {
//         dsProxyHelper = await dataShare.createDataShareHelper(context, uri, { isProxy: false, waitTime: 2 });
//         expect(dsProxyHelper != null).assertTrue();
//         console.log("[ttt] [DataShareClientTest] <<Consumer>> dsProxyHelper " + JSON.stringify(dsProxyHelper));
//       } catch (err) {
//         console.error('[ttt] [DataShareClientTest] <<Consumer>> createDataShareHelper error' + err.code + "  " +
//         err.message);
//         expect().assertFail();
//       }
//       await sleep(2000);
//     })
//     beforeEach(() => {
//     })
//     afterEach(() => {
//     })
//     afterAll(async () => {
//       await dsProxyHelper?.close();
//     })
//
//     /**
//      * @tc.name testCreateDataShareHelper001
//      * @tc.number SUB_DistributedData_DataShare_DFX_CreateDataShareHelper_0100
//      * @tc.desc createDataShareHelper
//      * @tc.size MediumTest
//      * @tc.type RELIABILITY
//      * @tc.level Level 3
//      */
//     it('testCreateDataShareHelper001', TestType.RELIABILITY | Size.MEDIUMTEST | Level.LEVEL3, async (done: Function) => {
//       try{
//         const GetRdbStoreCallBackPerfTest = async (index: number) => {
//           let startTime = new Date().getTime()
//           await dataShare.createDataShareHelper(context, uri, { isProxy: false })
//             .then((result: dataShare.DataShareHelper) => {
//               if (index < 2000) {
//                 let endTime = new Date().getTime()
//                 expect((endTime - startTime) < 2000).assertTrue();
//                 expect(result != null).assertTrue();
//                 console.log("[ttt] [DataShareClientTest] <<Consumer>> dsProxyHelper time： " + (endTime - startTime));
//                 GetRdbStoreCallBackPerfTest(index + 1);
//               } else {
//                 done();
//               }
//             })
//         }
//         await GetRdbStoreCallBackPerfTest(0);
//       }catch (err) {
//         console.info("[ttt] testCreateDataShareHelper002 fail err.code is " + err.code + "err.message is " +err.message)
//         expect().assertFail()
//       }
//     });
//
//     /**
//      * @tc.name testCreateDataShareHelper002
//      * @tc.number SUB_DistributedData_DataShare_DFX_CreateDataShareHelper_0200
//      * @tc.desc createDataShareHelper
//      * @tc.size MediumTest
//      * @tc.type RELIABILITY
//      * @tc.level Level 3
//      */
//     it('testCreateDataShareHelper002', TestType.RELIABILITY | Size.MEDIUMTEST | Level.LEVEL3, async () => {
//       try{
//         for(let i=0;i<2000;i++){
//           let startTime = new Date().getTime()
//           let result = await dataShare.createDataShareHelper(context, uri, { isProxy: false });
//           let endTime = new Date().getTime()
//           expect((endTime - startTime) < 2000).assertTrue();
//           expect(result != null).assertTrue();
//           console.log("[ttt] [DataShareClientTest] <<Consumer>> dsProxyHelper time： " + (endTime - startTime));
//         }
//       }catch (err) {
//         console.info("[ttt] testCreateDataShareHelper002 fail err.code is " + err.code + "err.message is " +err.message)
//         expect().assertFail()
//       }
//     });
//   })
// }
//
// function sleep(ms: number): Promise<void> {
//   return new Promise(resolve => setTimeout(resolve, ms));
// }
//