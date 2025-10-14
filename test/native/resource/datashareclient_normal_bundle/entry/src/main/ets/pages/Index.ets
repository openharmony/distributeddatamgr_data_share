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
  disconnectDataShareExtAbility,
  onRdb,
  offRdb,
  queryTest,
  insertTest,
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
  enable,
  disable
} from './myClient.test'

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
          console.info('[ttt] [DataShareClientTest] <<UI>> On onclick enter')
          this.message = '~ On onclick'
          try {
            await onRdb();
          } catch (err) {
            console.error('[ttt] [DataAbilityTest] Observer on catch(err)====>:' + err);
          }
          console.info('[ttt] [DataShareClientTest] <<UI>> On onclick leave')
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

      }

      Row() {
        Button() {
          Text('Query')
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
          console.info('[ttt] [DataShareClientTest] <<UI>> Query onclick enter')
          this.message = 'Query onclick'
          let ret = await queryTest()
          this.message += "\n\n Test Result: " + ret;
          console.info('[ttt] [DataShareClientTest] <<UI>> Query onclick leave')
        })

        Button() {
          Text('Insert')
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
          console.info('[ttt] [DataShareClientTest] <<UI>> Insert onclick enter')
          this.message = 'Insert onclick'
          let ret = await insertTest()
          this.message += "\n\n Test Result: " + ret;
          console.info('[ttt] [DataShareClientTest] <<UI>> Insert onclick leave')
        })
      }

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
          await onPublishData()
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

      Row() {
        Button() {
          Text('enable')
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
          console.info('[ttt] [DataShareClientTest] <<UI>> enable onclick enter')
          this.message = 'offPublishDataErr1 onclick'
          await enable()
          console.info('[ttt] [DataShareClientTest] <<UI>> enable onclick leave')
        })

        Button() {
          Text('disable')
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
          console.info('[ttt] [DataShareClientTest] <<UI>> disable onclick enter')
          this.message = 'offPublishDataErr2 onclick'
          await disable()
          console.info('[ttt] [DataShareClientTest] <<UI>> disable onclick leave')
        })
      }

    }
    .width('100%')
    .height('100%')
  }
}