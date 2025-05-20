import { router } from '@kit.ArkUI'
import { AssetUtil } from '../AssetUtil'
import { CopyPhotoData, ReadPhotoData } from '../FileUtil'
import photoAccessHelper from '@ohos.file.photoAccessHelper'

let assetUtil: AssetUtil = new AssetUtil()

@Entry
@Component
struct Index {
  build() {
    Row() {
      Column() {
        Text('Public').margin(6).fontWeight(FontWeight.Bold)

        Button('System').margin(6).onClick(() => {
          try {
            router.pushUrl({ url: 'pages/SystemApi' })
          } catch (err) {
            console.error(`failed to ${err.code} ${err.message}`);
          }
        })

        Button('DataShare').margin(6).onClick(() => {
          try {
            router.pushUrl({ url: 'pages/DataShare' })
          } catch (err) {
            console.error(`failed to ${err.code} ${err.message}`);
          }
        })

        Button('ReadPhotoData').margin(6).onClick(async ()=> {
          let uri = 'file://media/Photo/35/IMG_1742786685_034/sys_test3.jpg'
          let result = await assetUtil.GetAssets(photoAccessHelper.PhotoKeys.URI, uri)
          for (let i = 0; result && i < result.length; i++) {
            console.info('GetAssets uri:' + result[i].uri + ' displayName:' + result[i].displayName)
          }
          ReadPhotoData(uri)
        })

        Button('PublicAssetTest').margin(6).onClick(async () => {
          console.info('=======GetAssets All=======')
          let result = await assetUtil.GetAssets(photoAccessHelper.PhotoKeys.TITLE, '')
          if (!result || result.length <= 0) {
            console.error('=======PublicAssetTest======= End')
            return
          }

          let uris: Array<string> = new Array<string>()
          console.info('=======CreateAsset pub_test1=======')
          let uri = await assetUtil.CreateAsset('pub_test1')
          if (uri) {
            uris.push(uri)
          }

          console.info('=======CreateAsset pub_test2=======')
          uri = await assetUtil.CreateAssetWithShortTermPermission('pub_test2')
          if (uri) {
            uris.push(uri)
          }

          console.info('=======CopyPhotoData=======')
          for (let i = 0; i < uris.length; i++) {
            CopyPhotoData(result[0].uri, uris[i])
          }

          console.info('=======ReadPhotoData=======')
          for (let i = 0; i < uris.length; i++) {
            ReadPhotoData(uris[i])
          }

          console.info('=======GetAssets pub_%=======')
          result = await assetUtil.GetAssets(photoAccessHelper.PhotoKeys.TITLE, 'pub_%')
          for (let i = 0; result && i < result.length; i++) {
            await assetUtil.CloneAsset(result[i])
            await assetUtil.ModifyAsset(result[i])
          }

          console.info('=======GetAssets clone_%=======')
          result = await assetUtil.GetAssets(photoAccessHelper.PhotoKeys.TITLE, 'clone_%')
          if (result) {
            await assetUtil.DeleteAssets(result)
          }

          console.info('=======GetAssets modify_%=======')
          result = await assetUtil.GetAssets(photoAccessHelper.PhotoKeys.TITLE, 'modify_%')
          if (result) {
            await assetUtil.DeleteAssets(result)
          }

          console.info('=======PublicAssetTest======= End')
        })

        Button('PublicAlbumTest').margin(6).onClick(async ()=> {
          console.info('=======PublicAlbumTest======= GetAlbums USER_GENERIC')
          let result = await assetUtil.GetAlbums(photoAccessHelper.AlbumType.USER,
            photoAccessHelper.AlbumSubtype.USER_GENERIC)
          for (let i = 0; result && i < result.length; i++) {
            await assetUtil.RemoveAlbumAssets(result[i])
          }

          console.info('=======PublicAlbumTest======= GetAlbums TRASH')
          result = await assetUtil.GetAlbums(photoAccessHelper.AlbumType.SYSTEM,
            photoAccessHelper.AlbumSubtype.TRASH)
          for (let i = 0; result && i < result.length; i++) {
            console.info('albumName:' + result[i].albumName + ' albumUri:' + result[i].albumUri)
          }

          console.info('=======PublicAssetTest======= End')
        })
      }
      .width('100%')
    }
    .height('100%')
  }
}