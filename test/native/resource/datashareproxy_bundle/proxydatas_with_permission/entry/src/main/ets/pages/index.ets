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
    }
    .width('100%')
    .height('100%')
  }
}