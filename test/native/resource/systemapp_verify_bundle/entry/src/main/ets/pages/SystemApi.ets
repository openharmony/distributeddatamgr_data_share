/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

import { AssetUtil } from '../AssetUtil'
import { CopyPhotoData, ReadPhotoData } from '../FileUtil'
import photoAccessHelper from '@ohos.file.photoAccessHelper'
let assetUtil: AssetUtil = new AssetUtil()

@Entry
@Component
struct SystemApi {
  build() {
    Row() {
      Column() {
        Text('System').margin(6).fontWeight(FontWeight.Bold)
        Button('SystemAssetTest').margin(6).onClick(async () => {
          console.info('=======GetAssets All=======')
          let result = await assetUtil.GetAssets(photoAccessHelper.PhotoKeys.TITLE, '')
          if (!result || result.length <= 0) {
            console.error('=======SystemAssetTest======= End')
            return
          }

          let uris: Array<string> = new Array<string>()
          console.info('=======SystemCreateAsset sys_test1=======')
          let uri = await assetUtil.SystemCreateAsset('sys_test1')
          if (uri) {
            uris.push(uri)
          }

          console.info('=======SystemCreateAsset sys_test2=======')
          uri = await assetUtil.SystemCreateAssetSubType('sys_test2', photoAccessHelper.PhotoSubtype.DEFAULT)
          if (uri) {
            uris.push(uri)
          }

          console.info('=======SystemCreateAsset sys_test3=======')
          uri = await assetUtil.SystemCreateAssetForApp('sys_test3', 'testAppName', 'testBundleName','testAppId')
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

          console.info('=======GetAssets sys_%=======')
          result = await assetUtil.GetAssets(photoAccessHelper.PhotoKeys.TITLE, 'sys_%')
          for (let i = 0; result && i < result.length; i++) {
            await assetUtil.SystemDeleteAsset(result[i])
          }
          console.info('=======SystemAssetTest======= End')
        })

        Button('SystemAlbumTest').margin(6).onClick(async ()=> {
          console.info('=======SystemRecoverAlbumAssets======= GetAlbums TRASH')
          let result = await assetUtil.GetAlbums(photoAccessHelper.AlbumType.SYSTEM,
            photoAccessHelper.AlbumSubtype.TRASH)
          for (let i = 0; result && i < result.length; i++) {
            await assetUtil.SystemRecoverAlbumAssets(result[i])
          }
          console.info('=======SystemRecoverAlbumAssets======= END')

          console.info('=======SystemDeleteAlbum======= GetAlbums USER_GENERIC')
          result = await assetUtil.GetAlbums(photoAccessHelper.AlbumType.USER,
            photoAccessHelper.AlbumSubtype.USER_GENERIC)
          for (let i = 0; result && i < result.length; i++) {
            await assetUtil.SystemDeleteAlbum(i, result[i])
          }
          console.info('=======SystemDeleteAlbum======= END')

          console.info('=======SystemDeleteAlbumAssets======= GetAlbums TRASH')
          result = await assetUtil.GetAlbums(photoAccessHelper.AlbumType.SYSTEM,
            photoAccessHelper.AlbumSubtype.TRASH)
          for (let i = 0; result && i < result.length; i++) {
            await assetUtil.SystemDeleteAlbumAssets(result[i])
          }
          console.info('=======SystemDeleteAlbumAssets======= END')
        })
      }
      .width('100%')
    }
    .height('100%')
  }
}