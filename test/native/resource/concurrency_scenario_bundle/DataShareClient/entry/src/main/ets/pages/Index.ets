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

import {
  connectDataShareExtAbility001,
  connectDataShareExtAbility002,
  disconnectDataShareExtAbility,
  onRdb,
  offRdb,
  queryTest1,
  queryTest2,
  insertTest1,
  insertTest2,
  // queryTest,
  // insertTest,
  updateTest,
  deleteTest,
  offPublishData,
  onPublishData,
  getPublishedData,
  publishTest,
  publishTest2,
  publishTest3,
  offPublishDataErr1,
  offPublishDataErr2,
  GetCloneBundleInfoTest,
  GetSelfBundleInfoTest
} from './myClient.test'
let result = undefined;
@Entry
@Component
struct Index {
  @State message: string = "Main thread message filed"

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
      Text('DataShareClient1 Ability Test')
        .fontSize(20)
        .fontWeight(FontWeight.Bold)

      Text(`${this.message}`)
        .fontSize(25)
        .height('15%')
        .width('100%')
        .textAlign(TextAlign.Center)

      Row() {
        Button() {
          Text('Connect')
            .fontSize(20)
            .fontWeight(FontWeight.Bold)
        }.type(ButtonType.Capsule)
        .margin({
          top: 8
        })
        .width('45%')
        .height('8%')
        .backgroundColor('#0ddffb')
        .onClick(async () => {
          console.info('[ttt] [DataShareClientTest] <<UI>> Connect onclick enter')
          this.message = 'Connect onclick'
          await connectDataShareExtAbility001()
          console.info('[ttt] [DataShareClientTest] <<UI>> Connect onclick leave')
        })

        Button() {
          Text('Disconnect')
            .fontSize(20)
            .fontWeight(FontWeight.Bold)
        }.type(ButtonType.Capsule)
        .margin({
          top: 8
        })
        .width('45%')
        .height('8%')
        .backgroundColor('#0ddffb')
        .onClick(async () => {
          console.info('[ttt] [DataShareClientTest] <<UI>> Disconnect onclick enter')
          this.message = 'Disconnect onclick'
          await disconnectDataShareExtAbility()
          console.info('[ttt] [DataShareClientTest] <<UI>> Disconnect onclick leave')
        })
      }

      // Row() {
      //   Button() {
      //     Text('Connect1')
      //       .fontSize(20)
      //       .fontWeight(FontWeight.Bold)
      //   }.type(ButtonType.Capsule)
      //   .margin({
      //     top: 8
      //   })
      //   .width('45%')
      //   .height('8%')
      //   .backgroundColor('#0ddffb')
      //   .onClick(async () => {
      //     console.info('[ttt] [DataShareClientTest] <<UI>> Connect1 onclick enter')
      //     this.message = 'Connect1 onclick'
      //     await connectDataShareExtAbility001()
      //     console.info('[ttt] [DataShareClientTest] <<UI>> Connect1 onclick leave')
      //   })
      //
      //   Button() {
      //     Text('Connect2')
      //       .fontSize(20)
      //       .fontWeight(FontWeight.Bold)
      //   }.type(ButtonType.Capsule)
      //   .margin({
      //     top: 8
      //   })
      //   .width('45%')
      //   .height('8%')
      //   .backgroundColor('#0ddffb')
      //   .onClick(async () => {
      //     console.info('[ttt] [DataShareClientTest] <<UI>> Connect2 onclick enter')
      //     this.message = 'Connect2 onclick'
      //     await connectDataShareExtAbility002()
      //     console.info('[ttt] [DataShareClientTest] <<UI>> Connect2 onclick leave')
      //   })
      // }

      Row() {
        Button() {
          Text('On')
            .fontSize(20)
            .fontWeight(FontWeight.Bold)
        }.type(ButtonType.Capsule)
        .margin({
          top: 8
        })
        .width('45%')
        .height('8%')
        .backgroundColor('#0ddffb')
        .onClick(async () => {
          console.info('[ttt] [DataShareClientTest] <<UI>> addTemplate onclick enter')
          this.message = '~ addTemplate onclick'
          try {
            await onRdb();
          } catch (err) {
            console.error('[ttt] [DataAbilityTest] Observer on catch(err)====>:' + err);
          }
          console.info('[ttt] [DataShareClientTest] <<UI>> addTemplate onclick leave')
        })

        Button() {
          Text('Off')
            .fontSize(20)
            .fontWeight(FontWeight.Bold)
        }.type(ButtonType.Capsule)
        .margin({
          top: 8
        })
        .width('45%')
        .height('8%')
        .backgroundColor('#0ddffb')
        .onClick(async () => {
          console.info('[ttt] [DataShareClientTest] <<UI>> Off onclick enter')
          this.message = '~ Off onclick'
          try {
            await offRdb();
          } catch (err) {
            console.error('[ttt] [DataAbilityTest] Observer Off catch(err)====>:' + err);
          }
          console.info('[ttt] [DataShareClientTest] <<UI>> Off onclick leave')
        })
        // Button() {
        //   Text('Clone')
        //     .fontSize(20)
        //     .fontWeight(FontWeight.Bold)
        // }.type(ButtonType.Capsule)
        // .margin({
        //   top: 8
        // })
        // .width('45%')
        // .height('8%')
        // .backgroundColor('#0ddffb')
        // .onClick(async () => {
        //   console.info('[ttt] [DataShareClientTest] <<UI>> GetCloneBundleInfo onclick enter')
        //   this.message = '~ Off onclick'
        //   try {
        //     await GetCloneBundleInfoTest();
        //   } catch (err) {
        //     console.error('[ttt] [DataAbilityTest] Observer GetCloneBundleInfo catch(err)====>:' + err);
        //   }
        //   console.info('[ttt] [DataShareClientTest] <<UI>> GetCloneBundleInfo onclick leave')
        // })

      }

      Row() {
        Button() {
          Text('Query1')
            .fontSize(20)
            .fontWeight(FontWeight.Bold)
        }.type(ButtonType.Capsule)
        .margin({
          top: 8
        })
        .width('45%')
        .height('8%')
        .backgroundColor('#0ddffb')
        .onClick(async () => {
          console.info('[ttt] [DataShareClientTest] <<UI>> Query1 onclick enter')
          this.message = '~ Query1 onclick'
          try {
            await queryTest1();
          } catch (err) {
            console.error('[ttt] [DataAbilityTest] Observer Query1 catch(err)====>:' + err);
          }
          console.info('[ttt] [DataShareClientTest] <<UI>> Query1 onclick leave')
        })

        Button() {
          Text('Query2')
            .fontSize(20)
            .fontWeight(FontWeight.Bold)
        }.type(ButtonType.Capsule)
        .margin({
          top: 8
        })
        .width('45%')
        .height('8%')
        .backgroundColor('#0ddffb')
        .onClick(async () => {
          console.info('[ttt] [DataShareClientTest] <<UI>> Query2 onclick enter')
          this.message = '~ Query2 onclick'
          try {
            await queryTest2();
          } catch (err) {
            console.error('[ttt] [DataAbilityTest] Observer Query2 catch(err)====>:' + err);
          }
          console.info('[ttt] [DataShareClientTest] <<UI>> Query2 onclick leave')
        })
      }

      Row() {
        Button() {
          Text('Insert1')
            .fontSize(20)
            .fontWeight(FontWeight.Bold)
        }.type(ButtonType.Capsule)
        .margin({
          top: 8
        })
        .width('45%')
        .height('8%')
        .backgroundColor('#0ddffb')
        .onClick(async () => {
          console.info('[ttt] [DataShareClientTest] <<UI>> Insert1 onclick enter')
          this.message = 'insert1 onclick'
          let ret = await insertTest1()
          this.message += "\n\n Test Result: " + ret;
          console.info('[ttt] [DataShareClientTest] <<UI>> Insert1 onclick leave')
        })

        Button() {
          Text('Insert2')
            .fontSize(20)
            .fontWeight(FontWeight.Bold)
        }.type(ButtonType.Capsule)
        .margin({
          top: 8
        })
        .width('45%')
        .height('8%')
        .backgroundColor('#0ddffb')
        .onClick(async () => {
          console.info('[ttt] [DataShareClientTest] <<UI>> Insert2 onclick enter')
          this.message = 'Insert2 onclick'
          let ret = await insertTest2()
          this.message += "\n\n Test Result: " + ret;
          console.info('[ttt] [DataShareClientTest] <<UI>> Insert2 onclick leave')
        })
      }

      // Row() {
      //   Button() {
      //     Text('Query')
      //       .fontSize(20)
      //       .fontWeight(FontWeight.Bold)
      //   }.type(ButtonType.Capsule)
      //   .margin({
      //     top: 8
      //   })
      //   .width('45%')
      //   .height('8%')
      //   .backgroundColor('#0ddffb')
      //   .onClick(async () => {
      //     console.info('[ttt] [DataShareServerTest] <<UI>> Query onclick enter')
      //     this.message = 'Query onclick'
      //     let ret = await queryTest()
      //     this.message += "\n\n Test Result: " + ret;
      //     console.info('[ttt] [DataShareServerTest] <<UI>> Query onclick leave')
      //   })
      //
      //   Button() {
      //     Text('Insert')
      //       .fontSize(20)
      //       .fontWeight(FontWeight.Bold)
      //   }.type(ButtonType.Capsule)
      //   .margin({
      //     top: 8
      //   })
      //   .width('45%')
      //   .height('8%')
      //   .backgroundColor('#0ddffb')
      //   .onClick(async () => {
      //     console.info('[ttt] [DataShareServerTest] <<UI>> Insert onclick enter')
      //     this.message = 'Insert onclick'
      //     let ret = await insertTest()
      //     this.message += "\n\n Test Result: " + ret;
      //     console.info('[ttt] [DataShareServerTest] <<UI>> Insert onclick leave')
      //   })
      // }

      Row() {
        Button() {
          Text('Update')
            .fontSize(20)
            .fontWeight(FontWeight.Bold)
        }.type(ButtonType.Capsule)
        .margin({
          top: 8
        })
        .width('45%')
        .height('8%')
        .backgroundColor('#0ddffb')
        .onClick(async () => {
          console.info('[ttt] [DataShareClientTest] <<UI>> Update onclick enter')
          this.message = 'Update onclick'
          let ret = await updateTest()
          this.message += "\n\n Test Result: " + ret;
          console.info('[ttt] [DataShareClientTest] <<UI>> Update onclick leave')
        })

        Button() {
          Text('Delete')
            .fontSize(20)
            .fontWeight(FontWeight.Bold)
        }.type(ButtonType.Capsule)
        .margin({
          top: 8
        })
        .width('45%')
        .height('8%')
        .backgroundColor('#0ddffb')
        .onClick(async () => {
          console.info('[ttt] [DataShareClientTest] <<UI>> Delete onclick enter')
          this.message = 'Delete onclick'
          let ret = await deleteTest()
          this.message += "\n\n Test Result: " + ret;
          console.info('[ttt] [DataShareClientTest] <<UI>> Delete onclick leave')
        })
      }

      Row() {
        Button() {
          Text('OffPublishData')
            .fontSize(20)
            .fontWeight(FontWeight.Bold)
        }.type(ButtonType.Capsule)
        .margin({
          top: 8
        })
        .width('45%')
        .height('8%')
        .backgroundColor('#0ddffb')
        .onClick(async () => {
          console.info('[ttt] [DataShareClientTest] <<UI>> OffPublishData onclick enter')
          this.message = 'OffPublishData onclick'
          await offPublishData()
          console.info('[ttt] [DataShareClientTest] <<UI>> OffPublishData onclick leave')
        })


        Button() {
          Text('OnPublishData')
            .fontSize(20)
            .fontWeight(FontWeight.Bold)
        }.type(ButtonType.Capsule)
        .margin({
          top: 8
        })
        .width('45%')
        .height('8%')
        .backgroundColor('#0ddffb')
        .onClick(async () => {
          console.info('[ttt] [DataShareClientTest] <<UI>> OnPublishData onclick enter')
          this.message = 'OnPublishData onclick'
          onPublishData()
          offPublishData()
          console.info('[ttt] [DataShareClientTest] <<UI>> OnPublishData onclick leave')
        })
      }

      Row() {
        Button() {
          Text('getPublishedData')
            .fontSize(20)
            .fontWeight(FontWeight.Bold)
        }.type(ButtonType.Capsule)
        .margin({
          top: 8
        })
        .width('45%')
        .height('8%')
        .backgroundColor('#0ddffb')
        .onClick(async () => {
          console.info('[ttt] [DataShareClientTest] <<UI>> getPublishedData onclick enter')
          this.message = 'getPublishedData onclick'
          let ret = await getPublishedData()
          this.message += "\n\n Test Result: " + ret;
          console.info('[ttt] [DataShareClientTest] <<UI>> getPublishedData onclick leave')
        })

        Button() {
          Text('Publish')
            .fontSize(20)
            .fontWeight(FontWeight.Bold)
        }.type(ButtonType.Capsule)
        .margin({
          top: 8
        })
        .width('45%')
        .height('8%')
        .backgroundColor('#0ddffb')
        .onClick(async () => {
          console.info('[ttt] [DataShareClientTest] <<UI>> publish onclick enter')
          this.message = 'publish onclick'
          let ret = await publishTest()
          this.message += "\n\n Test Result: " + ret;
          console.info('[ttt] [DataShareClientTest] <<UI>> publish onclick leave')
        })
      }


      // Row() {
      //   Button() {
      //     Text('Publish2.0')
      //       .fontSize(20)
      //       .fontWeight(FontWeight.Bold)
      //   }.type(ButtonType.Capsule)
      //   .margin({
      //     top: 8
      //   })
      //   .width('45%')
      //   .height('8%')
      //   .backgroundColor('#0ddffb')
      //   .onClick(async () => {
      //     console.info('[ttt] [DataShareClientTest] <<UI>> Publish2.0 onclick enter')
      //     this.message = 'Publish2.0 onclick'
      //     let ret = await publishTest2()
      //     this.message += "\n\n Test Result: " + ret;
      //     console.info('[ttt] [DataShareClientTest] <<UI>> Publish2.0 onclick leave')
      //   })
      //
      //
      //   Button() {
      //     Text('Publish3.0')
      //       .fontSize(20)
      //       .fontWeight(FontWeight.Bold)
      //   }.type(ButtonType.Capsule)
      //   .margin({
      //     top: 8
      //   })
      //   .width('45%')
      //   .height('8%')
      //   .backgroundColor('#0ddffb')
      //   .onClick(async () => {
      //     console.info('[ttt] [DataShareClientTest] <<UI>> Publish3.0 onclick enter')
      //     this.message = 'Publish3.0 onclick'
      //     let ret = await publishTest3()
      //     this.message += "\n\n Test Result: " + ret;
      //     console.info('[ttt] [DataShareClientTest] <<UI>> Publish3.0 onclick leave')
      //   })
      // }

      Row() {
        Button() {
          Text('offPublishDataErr1')
            .fontSize(20)
            .fontWeight(FontWeight.Bold)
        }.type(ButtonType.Capsule)
        .margin({
          top: 8
        })
        .width('45%')
        .height('8%')
        .backgroundColor('#0ddffb')
        .onClick(async () => {
          console.info('[ttt] [DataShareClientTest] <<UI>> offPublishDataErr1 onclick enter')
          this.message = 'offPublishDataErr1 onclick'
          await offPublishDataErr1()
          console.info('[ttt] [DataShareClientTest] <<UI>> offPublishDataErr1 onclick leave')
        })


        Button() {
          Text('offPublishDataErr2')
            .fontSize(20)
            .fontWeight(FontWeight.Bold)
        }.type(ButtonType.Capsule)
        .margin({
          top: 8
        })
        .width('45%')
        .height('8%')
        .backgroundColor('#0ddffb')
        .onClick(async () => {
          console.info('[ttt] [DataShareClientTest] <<UI>> offPublishDataErr2 onclick enter')
          this.message = 'offPublishDataErr2 onclick'
          await offPublishDataErr2()
          console.info('[ttt] [DataShareClientTest] <<UI>> offPublishDataErr2 onclick leave')
        })
      }

    }
    .width('100%')
    .height('100%')
  }
}

