/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http: www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import dataShare from '@ohos.data.dataShare'
import dataSharePredicates from '@ohos.data.dataSharePredicates'

@Entry
@Component
struct Index {
  @State message: string = "Main thread message filed"

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
      Text('DataShare Extension Ability Test')
        .fontSize(25)
        .fontWeight(FontWeight.Bold)

      Text(`${this.message}`)
        .fontSize(30)
        .height('15%')
        .width('100%')
        .textAlign(TextAlign.Center)

      Row() {
        Button() {
          Text('Connect')
            .fontSize(25)
            .fontWeight(FontWeight.Bold)
        }.type(ButtonType.Capsule)
        .margin({
          top: 20
        })
        .width('45%')
        .height('8%')
        .backgroundColor('#0ddffb')
        .onClick(async () => {
          console.info('[ttt] [DataShareTest] <<UI>> Connect onclick enter')
          this.message = 'Connect onclick'
          await globalThis.connectDataShareExtAbility()
          console.info('[ttt] [DataShareTest] <<UI>> Connect onclick leave')
        })

        Button() {
          Text('Disconnect')
            .fontSize(25)
            .fontWeight(FontWeight.Bold)
        }.type(ButtonType.Capsule)
        .margin({
          top: 20
        })
        .width('45%')
        .height('8%')
        .backgroundColor('#0ddffb')
        .onClick(async () => {
          console.info('[ttt] [DataShareTest] <<UI>> Disconnect onclick enter')
          this.message = 'Disconnect onclick'
          await globalThis.disconnectDataShareExtAbility()
          console.info('[ttt] [DataShareTest] <<UI>> Disconnect onclick leave')
        })
      }

      Row() {
        Button() {
          Text('On')
            .fontSize(25)
              .fontWeight(FontWeight.Bold)
        }.type(ButtonType.Capsule)
        .margin({
          top: 20
        })
        .width('25%')
        .height('8%')
        .backgroundColor('#0ddffb')
        .onClick(async () => {
          console.info('[ttt] [DataShareTest] <<UI>> On onclick enter')
          this.message = '~ On onclick'
          try {
            await globalThis.on();
          } catch (err) {
            console.error('[ttt] [DataAbilityTest] Observer on catch(err)====>:' + err);
          }
          console.info('[ttt] [DataShareTest] <<UI>> On onclick leave')
        })
  
        Button() {
          Text('Off')
            .fontSize(25)
            .fontWeight(FontWeight.Bold)
        }.type(ButtonType.Capsule)
        .margin({
          top: 20
        })
        .width('25%')
        .height('8%')
        .backgroundColor('#0ddffb')
        .onClick(async () => {
          console.info('[ttt] [DataShareTest] <<UI>> Off onclick enter')
          this.message = '~ Off onclick'
          try {
            await globalThis.off();
          } catch (err) {
            console.error('[ttt] [DataAbilityTest] Observer Off catch(err)====>:' + err);
          }
          console.info('[ttt] [DataShareTest] <<UI>> Off onclick leave')
        })

        Button() {
          Text('OpenFile')
            .fontSize(25)
            .fontWeight(FontWeight.Bold)
        }.type(ButtonType.Capsule)
        .margin({
          top: 20
        })
        .width('40%')
        .height('8%')
        .backgroundColor('#0ddffb')
        .onClick(async () => {
          console.info('[ttt] [DataShareTest] <<UI>> OpenFile onclick enter')
          this.message = 'OpenFile onclick'
          let ret = await globalThis.openFile()
          this.message += "\n\n Test Result: " + ret;
          console.info('[ttt] [DataShareTest] <<UI>> OpenFile onclick leave')
        })
      }

      Row() {
        Button() {
          Text('Query')
            .fontSize(25)
            .fontWeight(FontWeight.Bold)
        }.type(ButtonType.Capsule)
        .margin({
          top: 20
        })
        .width('45%')
        .height('8%')
        .backgroundColor('#0ddffb')
        .onClick(async () => {
          console.info('[ttt] [DataShareTest] <<UI>> Query onclick enter')
          this.message = 'Query onclick'
          let ret = await globalThis.query()
          this.message += "\n\n Test Result: " + ret;
          console.info('[ttt] [DataShareTest] <<UI>> Query onclick leave')
        })

        Button() {
          Text('Insert')
            .fontSize(25)
            .fontWeight(FontWeight.Bold)
        }.type(ButtonType.Capsule)
        .margin({
          top: 20
        })
        .width('45%')
        .height('8%')
        .backgroundColor('#0ddffb')
        .onClick(async () => {
          console.info('[ttt] [DataShareTest] <<UI>> Insert onclick enter')
          this.message = 'Insert onclick'
          let ret = await globalThis.insert()
          this.message += "\n\n Test Result: " + ret;
          console.info('[ttt] [DataShareTest] <<UI>> Insert onclick leave')
        })
      }

      Row() {
        Button() {
          Text('Update')
            .fontSize(25)
            .fontWeight(FontWeight.Bold)
        }.type(ButtonType.Capsule)
        .margin({
          top: 20
        })
        .width('45%')
        .height('8%')
        .backgroundColor('#0ddffb')
        .onClick(async () => {
          console.info('[ttt] [DataShareTest] <<UI>> Update onclick enter')
          this.message = 'Update onclick'
          let ret = await globalThis.update()
          this.message += "\n\n Test Result: " + ret;
          console.info('[ttt] [DataShareTest] <<UI>> Update onclick leave')
        })

        Button() {
          Text('Delete')
            .fontSize(25)
            .fontWeight(FontWeight.Bold)
        }.type(ButtonType.Capsule)
        .margin({
          top: 20
        })
        .width('45%')
        .height('8%')
        .backgroundColor('#0ddffb')
        .onClick(async () => {
          console.info('[ttt] [DataShareTest] <<UI>> Delete onclick enter')
          this.message = 'Delete onclick'
          let ret = await globalThis.delete()
          this.message += "\n\n Test Result: " + ret;
          console.info('[ttt] [DataShareTest] <<UI>> Delete onclick leave')
        })
      }

      Row() {
        Button() {
          Text('BatchInsert')
            .fontSize(25)
            .fontWeight(FontWeight.Bold)
        }.type(ButtonType.Capsule)
        .margin({
          top: 20
        })
        .width('45%')
        .height('8%')
        .backgroundColor('#0ddffb')
        .onClick(async () => {
          console.info('[ttt] [DataShareTest] <<UI>> BatchInsert onclick enter')
          this.message = 'BatchInsert onclick'
          let ret = await globalThis.batchInsert()
          this.message += "\n\n Test Result: " + ret;
          console.info('[ttt] [DataShareTest] <<UI>> BatchInsert onclick leave')
        })

        Button() {
          Text('NotifyChange')
            .fontSize(25)
            .fontWeight(FontWeight.Bold)
        }.type(ButtonType.Capsule)
        .margin({
          top: 20
        })
        .width('45%')
        .height('8%')
        .backgroundColor('#0ddffb')
        .onClick(async () => {
          console.info('[ttt] [DataShareTest] <<UI>> NotifyChange onclick enter')
          this.message = 'NotifyChange onclick'
          await globalThis.notifyChange()
          console.info('[ttt] [DataShareTest] <<UI>> NotifyChange onclick leave')
        })
      }

      Row() {
        Button() {
          Text('GetType')
            .fontSize(25)
            .fontWeight(FontWeight.Bold)
        }.type(ButtonType.Capsule)
        .margin({
          top: 20
        })
        .width('45%')
        .height('8%')
        .backgroundColor('#0ddffb')
        .onClick(async () => {
          console.info('[ttt] [DataShareTest] <<UI>> GetType onclick enter')
          this.message = 'GetType onclick'
          let ret = await globalThis.getType()
          this.message += "\n\n Test Result: " + ret;
          console.info('[ttt] [DataShareTest] <<UI>> GetType onclick leave')
        })

        Button() {
          Text('GetFileTypes')
            .fontSize(25)
            .fontWeight(FontWeight.Bold)
        }.type(ButtonType.Capsule)
        .margin({
          top: 20
        })
        .width('45%')
        .height('8%')
        .backgroundColor('#0ddffb')
        .onClick(async () => {
          console.info('[ttt] [DataShareTest] <<UI>> GetFileTypes onclick enter')
          this.message = 'GetFileTypes onclick'
          let ret = await globalThis.getFileTypes()
          this.message += "\n\n Test Result: " + ret;
          console.info('[ttt] [DataShareTest] <<UI>> GetFileTypes onclick leave')
        })
      }

      Row() {
        Button() {
          Text('NormalizeUri')
            .fontSize(25)
            .fontWeight(FontWeight.Bold)
        }.type(ButtonType.Capsule)
        .margin({
          top: 20
        })
        .width('45%')
        .height('8%')
        .backgroundColor('#0ddffb')
        .onClick(async () => {
          console.info('[ttt] [DataShareTest] <<UI>> NormalizeUri onclick enter')
          this.message = 'NormalizeUri onclick'
          let ret = await globalThis.normalizeUri()
          this.message += "\n\n Test Result: " + ret;
          console.info('[ttt] [DataShareTest] <<UI>> NormalizeUri onclick leave')
        })

        Button() {
          Text('DenormalizeUri')
            .fontSize(25)
            .fontWeight(FontWeight.Bold)
        }.type(ButtonType.Capsule)
        .margin({
          top: 20
        })
        .width('45%')
        .height('8%')
        .backgroundColor('#0ddffb')
        .onClick(async () => {
          console.info('[ttt] [DataShareTest] <<UI>> DenormalizeUri onclick enter')
          this.message = 'DenormalizeUri onclick'
          let ret = await globalThis.denormalizeUri()
          this.message += "\n\n Test Result: " + ret;
          console.info('[ttt] [DataShareTest] <<UI>> DenormalizeUri onclick leave')
        })
      }
    }
    .width('100%')
    .height('100%')
  }
}