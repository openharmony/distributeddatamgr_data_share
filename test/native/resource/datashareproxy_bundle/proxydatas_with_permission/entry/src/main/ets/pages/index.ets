/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

import dataShare from '@ohos.data.dataShare'
import dataSharePredicates from '@ohos.data.dataSharePredicates'

@Entry
@Component
struct Index {
  @State message: string = "datashareproxyTest Main thread message filed"

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
      Text('datashareproxyTest Ability Test')
        .fontSize(25)
        .fontWeight(FontWeight.Bold)

      Text(`${this.message}`)
        .fontSize(35)
        .height('20%')
        .width('100%')
        .textAlign(TextAlign.Center)

      Row() {
        Button() {
          Text('Connect')
            .fontSize(20)
            .fontWeight(FontWeight.Bold)
        }.type(ButtonType.Capsule)
        .margin({
          top: 25
        })
        .width('40%')
        .height('8%')
        .backgroundColor('#0ddffb')
        .onClick(async () => {
          console.info('[ttt] [datashareproxyTest] <<UI>> Connect onclick enter')
          this.message = 'Connect onclick'
          await globalThis.connectDataShareExtAbility()
          console.info('[ttt] [datashareproxyTest] <<UI>> Connect onclick leave')
        })

        Button() {
          Text('Disconnect')
            .fontSize(20)
            .fontWeight(FontWeight.Bold)
        }.type(ButtonType.Capsule)
        .margin({
          top: 25
        })
        .width('40%')
        .height('8%')
        .backgroundColor('#0ddffb')
        .onClick(async () => {
          console.info('[ttt] [datashareproxyTest] <<UI>> Disconnect onclick enter')
          this.message = 'Disconnect onclick'
          await globalThis.disconnectDataShareExtAbility()
          console.info('[ttt] [datashareproxyTest] <<UI>> Disconnect onclick leave')
        })
      }

      Row() {
        Button() {
          Text('Insert')
            .fontSize(20)
            .fontWeight(FontWeight.Bold)
        }.type(ButtonType.Capsule)
        .margin({
          top: 25
        })
        .width('40%')
        .height('8%')
        .backgroundColor('#0ddffb')
        .onClick(async () => {
          console.info('[ttt] [datashareproxyTest] <<UI>> Insert onclick enter')
          this.message = 'Insert onclick'
          let ret = await globalThis.insert()
          this.message += "\n\n Test Result: " + ret;
          console.info('[ttt] [datashareproxyTest] <<UI>> Insert onclick leave')
        })
      }
    }
    .width('100%')
    .height('100%')
  }
}