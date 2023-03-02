/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
  @State message: string = "Main thread message filed"

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
      Text('DataShare Extension Ability ErrorCode Test')
        .fontSize(25)
        .fontWeight(FontWeight.Bold)

      Text(`${this.message}`)
        .fontSize(25)
        .height('20%')
        .width('100%')
        .textAlign(TextAlign.Center)

      Row() {
        Button() {
          Text('Connect')
            .fontSize(30)
            .fontWeight(FontWeight.Bold)
        }.type(ButtonType.Capsule)
        .margin({
          top: 25
        })
        .width('50%')
        .height('10%')
        .backgroundColor('#0ddffb')
        .onClick(async () => {
          console.info('[ttt] [DataShareTest] <<UI>> ErrorCode Connect onclick enter')
          this.message = 'Connect onclick'
          await globalThis.connectDataShareExtAbility()
          console.info('[ttt] [DataShareTest] <<UI>> ErrorCode Connect onclick leave')
        })
      }
    }
    .width('100%')
    .height('100%')
  }
}