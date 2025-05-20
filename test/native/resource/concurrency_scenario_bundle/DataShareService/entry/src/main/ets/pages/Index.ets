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
  onPublishData3,
  publishTest,
  publishTest2,
  publishTest3
} from './myServer.test'

@Entry
@Component
struct Index {
  @State message: string = "Main thread message filed"

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
      Text('DataShareServer Ability Test')
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
          console.info('[ttt] [DataShareServerTest] <<UI>> Connect onclick enter')
          this.message = 'Connect onclick'
          await connectDataShareExtAbility001()
          console.info('[ttt] [DataShareServerTest] <<UI>> Connect onclick leave')
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
          console.info('[ttt] [DataShareServerTest] <<UI>> Disconnect onclick enter')
          this.message = 'Disconnect onclick'
          await disconnectDataShareExtAbility()
          console.info('[ttt] [DataShareServerTest] <<UI>> Disconnect onclick leave')
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
          console.info('[ttt] [DataShareServerTest] <<UI>> On onclick enter')
          this.message = '~ On onclick'
          try {
            await onRdb();
          } catch (err) {
            console.error('[ttt] [DataAbilityTest] Observer on catch(err)====>:' + err);
          }
          console.info('[ttt] [DataShareServerTest] <<UI>> On onclick leave')
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
          console.info('[ttt] [DataShareServerTest] <<UI>> Off onclick enter')
          this.message = '~ Off onclick'
          try {
            await offRdb();
          } catch (err) {
            console.error('[ttt] [DataAbilityTest] Observer Off catch(err)====>:' + err);
          }
          console.info('[ttt] [DataShareServerTest] <<UI>> Off onclick leave')
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
          console.info('[ttt] [DataShareServerTest] <<UI>> Query onclick enter')
          this.message = 'Query onclick'
          let ret = await queryTest()
          this.message += "\n\n Test Result: " + ret;
          console.info('[ttt] [DataShareServerTest] <<UI>> Query onclick leave')
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
          console.info('[ttt] [DataShareServerTest] <<UI>> Insert onclick enter')
          this.message = 'Insert onclick'
          let ret = await insertTest()
          this.message += "\n\n Test Result: " + ret;
          console.info('[ttt] [DataShareServerTest] <<UI>> Insert onclick leave')
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
          console.info('[ttt] [DataShareServerTest] <<UI>> Update onclick enter')
          this.message = 'Update onclick'
          let ret = await updateTest()
          this.message += "\n\n Test Result: " + ret;
          console.info('[ttt] [DataShareServerTest] <<UI>> Update onclick leave')
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
          console.info('[ttt] [DataShareServerTest] <<UI>> Delete onclick enter')
          this.message = 'Delete onclick'
          let ret = await deleteTest()
          this.message += "\n\n Test Result: " + ret;
          console.info('[ttt] [DataShareServerTest] <<UI>> Delete onclick leave')
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
          console.info('[ttt] [DataShareServerTest] <<UI>> OffPublishData onclick enter')
          this.message = 'OffPublishData onclick'
          await offPublishData()
          console.info('[ttt] [DataShareServerTest] <<UI>> OffPublishData onclick leave')
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
          console.info('[ttt] [DataShareServerTest] <<UI>> OnPublishData onclick enter')
          this.message = 'OnPublishData onclick'
          await onPublishData()
          console.info('[ttt] [DataShareServerTest] <<UI>> OnPublishData onclick leave')
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
          console.info('[ttt] [DataShareServerTest] <<UI>> getPublishedData onclick enter')
          this.message = 'getPublishedData onclick'
          let ret = await getPublishedData()
          this.message += "\n\n Test Result: " + ret;
          console.info('[ttt] [DataShareServerTest] <<UI>> getPublishedData onclick leave')
        })

        Button() {
          Text('OnPublishData3')
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
          console.info('[ttt] [DataShareServerTest] <<UI>> OnPublishData3 onclick enter')
          this.message = 'OnPublishData3 onclick'
          await onPublishData3()
          console.info('[ttt] [DataShareServerTest] <<UI>> OnPublishData3 onclick leave')
        })
      }

      Row() {
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
          console.info('[ttt] [DataShareServerTest] <<UI>> publish onclick enter')
          this.message = 'publish onclick'
          let ret = await publishTest()
          this.message += "\n\n Test Result: " + ret;
          console.info('[ttt] [DataShareServerTest] <<UI>> publish onclick leave')
        })


        Button() {
          Text('GetFileTypes')
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
          console.info('[ttt] [DataShareServerTest] <<UI>> GetFileTypes onclick enter')
          this.message = 'GetFileTypes onclick'
          // let ret = await globalThis.getFileTypes()
          // this.message += "\n\n Test Result: " + ret;
          console.info('[ttt] [DataShareServerTest] <<UI>> GetFileTypes onclick leave')
        })
      }

      Row() {
        Button() {
          Text('Publish2.0')
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
          console.info('[ttt] [DataShareServerTest] <<UI>> Publish2.0 onclick enter')
          this.message = 'Publish2.0 onclick'
          let ret = await publishTest2()
          this.message += "\n\n Test Result: " + ret;
          console.info('[ttt] [DataShareServerTest] <<UI>> Publish2.0 onclick leave')
        })


        Button() {
          Text('Publish3.0')
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
          console.info('[ttt] [DataShareServerTest] <<UI>> Publish3.0 onclick enter')
          this.message = 'Publish3.0 onclick'
          let ret = await publishTest3()
          this.message += "\n\n Test Result: " + ret;
          console.info('[ttt] [DataShareServerTest] <<UI>> Publish3.0 onclick leave')
        })
      }

    }
    .width('100%')
    .height('100%')
  }
}