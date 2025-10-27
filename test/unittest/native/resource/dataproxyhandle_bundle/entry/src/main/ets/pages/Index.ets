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
  getSelfBundleInfo,
  createDataProxyHandleTest,
  deleteProxyDataTest001,
  publishProxyDataTest001,
  publishProxyDataTest002,
  getProxyDataTest001,
  onProxyDataTest001,
  offProxyDataTest001,
} from './proxyhandle.test';

import {
  publishProxyDataPerformTest001,
  getProxyDataPerformTest001,
  deleteProxyDataPerformTest001,
  onProxyDataPerformTest001,
  offProxyDataPerformTest001
} from './performance'

@Entry
@Component
struct Index {
  @State message: string = 'Hello World';

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
      Row() {
        Button() {
          Text("CreateHandle")
            .fontSize(25)
            .fontWeight(FontWeight.Bold)
        }
        .type(ButtonType.Capsule)
        .height('10%')
        .width('40%')
        .backgroundColor('#0ddffb')
        .onClick(() => {
          console.info('[ttt] [DataProxyHandleTest] <<UI>> CreateHandle onclick enter')
          this.message = 'CreateHandle onclick'
          createDataProxyHandleTest()
          console.info('[ttt] [DataProxyHandleTest] <<UI>> CreateHandle onclick leave')
        })

        Button() {
          Text("Get Self Info")
            .fontSize(25)
            .fontWeight(FontWeight.Bold)
        }
        .type(ButtonType.Capsule)
        .height('10%')
        .width('40%')
        .backgroundColor('#0ddffb')
        .onClick(() => {
          console.info('[ttt] [DataProxyHandleTest] <<UI>> Get AppId onclick enter')
          this.message = 'GetSelfBundleInfo onclick'
          getSelfBundleInfo()
          console.info('[ttt] [DataProxyHandleTest] <<UI>> Get AppId onclick leave')
        })
      }

      Row() {
        Button() {
          Text('Publish')
            .fontSize(25)
            .fontWeight(FontWeight.Bold)
        }
        .type(ButtonType.Capsule)
        .height('10%')
        .width('40%')
        .backgroundColor('#0ddffb')
        .onClick(() => {
          console.info('[ttt] [DataProxyHandleTest] <<UI>> publish onclick enter')
          this.message = 'Publish onclick'
          publishProxyDataTest001()
          // publishProxyDataPerformTest001()
          console.info('[ttt] [DataProxyHandleTest] <<UI>> publish onclick leave')
        })

        Button() {
          Text('Delete')
            .fontSize(25)
            .fontWeight(FontWeight.Bold)
        }
        .type(ButtonType.Capsule)
        .height('10%')
        .width('40%')
        .backgroundColor('#0ddffb')
        .onClick(() => {
          console.info('[ttt] [DataProxyHandleTest] <<UI>> delete onclick enter')
          this.message = 'Delete onclick'
          deleteProxyDataTest001()
          // deleteProxyDataPerformTest001()
          console.info('[ttt] [DataProxyHandleTest] <<UI>> delete onclick leave')
        })
      }

      Row() {
        Button() {
          Text('Get')
            .fontSize(25)
            .fontWeight(FontWeight.Bold)
        }
        .type(ButtonType.Capsule)
        .height('10%')
        .width('40%')
        .backgroundColor('#0ddffb')
        .onClick(() => {
          console.info('[ttt] [DataProxyHandleTest] <<UI>> get onclick enter')
          this.message = 'Get onclick'
          getProxyDataTest001()
          // getProxyDataPerformTest001()
          console.info('[ttt] [DataProxyHandleTest] <<UI>> get onclick leave')
        })

        Button() {
          Text('Publish2')
            .fontSize(25)
            .fontWeight(FontWeight.Bold)
        }
        .type(ButtonType.Capsule)
        .height('10%')
        .width('40%')
        .backgroundColor('#0ddffb')
        .onClick(() => {
          console.info('[ttt] [DataProxyHandleTest] <<UI>> publish onclick enter')
          this.message = 'Publish onclick'
          publishProxyDataTest002()
          console.info('[ttt] [DataProxyHandleTest] <<UI>> publish onclick leave')
        })
      }

      Row() {
        Button() {
          Text('On')
            .fontSize(25)
            .fontWeight(FontWeight.Bold)
        }
        .type(ButtonType.Capsule)
        .height('10%')
        .width('40%')
        .backgroundColor('#0ddffb')
        .onClick(() => {
          console.info('[ttt] [DataProxyHandleTest] <<UI>> on onclick enter')
          this.message = 'On onclick'
          onProxyDataTest001()
          // onProxyDataPerformTest001()
          console.info('[ttt] [DataProxyHandleTest] <<UI>> on onclick leave')
        })

        Button() {
          Text('Off')
            .fontSize(25)
            .fontWeight(FontWeight.Bold)
        }
        .type(ButtonType.Capsule)
        .height('10%')
        .width('40%')
        .backgroundColor('#0ddffb')
        .onClick(() => {
          console.info('[ttt] [DataProxyHandleTest] <<UI>> off onclick enter')
          this.message = 'Off onclick'
          offProxyDataTest001()
          // offProxyDataPerformTest001()
          console.info('[ttt] [DataProxyHandleTest] <<UI>> off onclick leave')
        })
      }
    }
    .height('100%')
  }
}