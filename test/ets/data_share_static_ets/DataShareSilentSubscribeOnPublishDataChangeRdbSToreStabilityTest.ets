/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect, TestType, Size, Level } from '../../../hypium/index';
import dataShare from '@ohos.data.dataShare';
import { BusinessError } from '@ohos.base';
import Utils from './Util.test';
import common from '@ohos.app.ability.common';
import { AppStorage } from '@ohos.arkui.stateManagement';
import UIAbility from '@ohos.app.ability.UIAbility';
 
let context: common.UIAbilityContext;
let uri = ("datashareproxy://com.acts.ohos.data.datasharetest.static/test");
let uris: Array<string> = [uri];
let dsProxyHelper: dataShare.DataShareHelper | undefined = undefined
const TAG = "[ttt] "
const COUNT_NUM  = 1000;
 
export default function DataShareSilentSubscribeOnPublishDataChangeRdbStoreMTBFTest() {
  describe('DataShareSilentSubscribeOnPublishDataChangeRdbStoreMTBFTest', () => {
    beforeAll(() => {
      context = AppStorage.get<common.UIAbilityContext>("context") as common.UIAbilityContext;
    })
 
    beforeEach(async () => {
        console.info(TAG + "===========beforeAll Begin===========");
        try{
          dsProxyHelper = await dataShare.createDataShareHelper(context, uri, {isProxy: true}); // 默访问
          await Utils.msSleep(2000);
          expect(dsProxyHelper != undefined).assertTrue();
          console.info(TAG + "[DataShareClientTest] <<Consumer>> dsProxyHelper " + JSON.stringify(dsProxyHelper));
        } catch (err: BusinessError) {
          console.error(TAG + '[DataShareClientTest] <<Consumer>> createDataShareHelper error' + err.code + "  " + err.message);
          expect().assertFail();
        }
        console.info(TAG + "===========beforeAll End===========");
    })
 
    afterEach(async () => {
      console.info(TAG + "===========afterAll Begin===========");
      await dsProxyHelper!.close();
      console.info(TAG + "===========afterAll End===========");
    })
 
    afterAll(() => {});
 
    /**
     * @tc.name       : testSubscribePublishDataChangeTest0010
     * @tc.number     : SUB_DistributedData_DFX_MTBF_DataShare_Subscribe_Test_016
     * @tc.desc       : onRdbDataChange test
     * @tc.size       : MediumTest
     * @tc.type       : RELIABILITY
     * @tc.level      : Level 3
     */
    it('testSubscribePublishDataChangeTest0010', TestType.RELIABILITY | Size.MEDIUMTEST | Level.LEVEL3, async (done: () => void) => {
      // 构造发布的数据string|ArrayBuffer
      let data_buff = new ArrayBuffer(20 * 1024); // 20KB
      let data_string= "中文English<>|''\"&or123";
      try{
          for(let i = 0; i < COUNT_NUM; i++) {
              let result_on: Array<dataShare.OperationResult> = dsProxyHelper!.onPublishedDataChange(uris, "100", (node: dataShare.PublishedDataChangeNode) => {
                console.info(TAG + "testSubscribePublishDataChangeTest0010 publishedData onCallback.");
                try{
                  console.info(TAG + "testSubscribePublishDataChangeTest0010 publishedData onCallback node.data size = " + node.data.length);
                  console.info(TAG + "testSubscribePublishDataChangeTest0010 publishedData onCallback node.data = " + JSON.stringify(node.data[0]));
                  // 断言回调的data是否与发布的data一致，data_buffer为object对象无法获取具体的值故断言类型
                  expect(typeof node.data[0].data === typeof data_buff || node.data[0].data === data_string).assertTrue();
                } catch (err) {
                  console.error(TAG + "testSubscribePublishDataChangeTest0010 publishedData on failed! err = " + JSON.stringify(err));
                }
              });
 
            // 发布数据触发回调
            let data1 : Array<dataShare.PublishedItem> = [
              {key: uri, subscriberId:"100", data:data_string}];
            try {
              let result_pub1: Array<dataShare.OperationResult> = await dsProxyHelper!.publish(data1, "com.acts.ohos.data.datasharetest");
              console.log(TAG + "[DataShareClientTest] <<Consumer>> testSubscribePublishDataChangeTest0010 publish end. result_pub1 = " + JSON.stringify(result_pub1));
              // 发布成功result=0
              expect(result_pub1[0].result === 0).assertTrue();
            } catch (e: BusinessError) {
              console.error(TAG + "[DataShareClientTest] <<Consumer>> testSubscribePublishDataChangeTest0010 publish error " + e.code + "  " + e.message);
            }
 
            // 回调完成后断言
            try {
              console.log(TAG + "[DataShareClientTest] <<Consumer>> testSubscribePublishDataChangeTest0010 user onRdbDataChange end. result_on = " + JSON.stringify(result_on));
              // 回调成功result=0
              expect(result_on[0].result === 0).assertTrue();
            } catch (err: BusinessError) {
              console.error(TAG + "testSubscribePublishDataChangeTest0010 ret_inset notifyChange failed! err = " + err);
              // 失败只能因为是时序问题导致句柄释放后回调慢继续使用
              expect(err.code === 15700013).assertTrue();
            }
 
            // 再次发布数据，即就是修改数据，触发回调
            let data2 : Array<dataShare.PublishedItem> = [
              {key: uri, subscriberId:"100", data:data_buff}];
            try {
              let result_pub2: Array<dataShare.OperationResult> = await dsProxyHelper!.publish(data2, "com.acts.ohos.data.datasharetest");
              console.log(TAG + "[DataShareClientTest] <<Consumer>> testSubscribePublishDataChangeTest0010 publish end. result_pub2 = " + JSON.stringify(result_pub2));
              // 取消订阅成功result=0
              expect(result_pub2[0].result === 0).assertTrue();
            } catch (e: BusinessError) {
              console.error(TAG + "[DataShareClientTest] <<Consumer>> testSubscribePublishDataChangeTest0010 publish error " + e.code + "  " + e.message);
            }
 
            // 最后取消订阅,PublishDataChange属于过程数据，无人订阅10后自动删除，无接口调用主动清理
            try {
              let result_off: Array<dataShare.OperationResult> = dsProxyHelper!.offPublishedDataChange(uris, "100");
              console.log(TAG + "[DataShareClientTest] <<Consumer>> testSubscribePublishDataChangeTest0010 off end " + JSON.stringify(result_off));
              // 取消订阅成功result=0
              expect(result_off[0].result === 0).assertTrue();
            } catch(err: BusinessError) {
              console.error(TAG + `[DataShareClientTest] off error: code: ${err.code}, message: ${err.message} `);
            }
          }
          done();
        } catch (err: BusinessError) {
          console.error(TAG +'[DataShareClientTest] <<Consumer>> testSubscribeRdbDataChangeInsertTest0010 error ' + err.code + "  "+err.message);
          done();
          // 失败只能因为是时序问题导致句柄释放后回调慢继续使用
          expect(err.code === 15700013).assertTrue();
        }
    });
  })
}