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

import { BusinessError } from '@kit.BasicServicesKit';
import dataShare from '@ohos.data.dataShare';
import { dataSharePredicates, ValuesBucket } from '@kit.ArkData';
import photoAccessHelper from '@ohos.file.photoAccessHelper';

let context = getContext(this);

function DataShareDelete(operation: string, field: string, value: string) {
  console.info('DataShareDelete Demo Start:' + operation + ',' + field + '=' + value);
  let uri: string = 'datashare:///media/' + operation + '/delete?api_version=10';
  dataShare.createDataShareHelper(context, uri).then((helper: dataShare.DataShareHelper) => {
    let predicates = new dataSharePredicates.DataSharePredicates();
    predicates.equalTo(field, value);
    helper.delete(uri, predicates);
    console.info('DataShareHelper.delete successfully');
  }).catch ((err: BusinessError) => {
    console.error(`error: ${err.code}, ${err.message}`);
  })
  console.info('DataShareDelete Demo End:');
}

function DataShareMove(assetUri: string, oldAlbumId: string, newAlbumId: number) {
  console.info('DataShareMove Demo Start: ' + assetUri);
  let uri: string = 'datashare:///media/phaccess_photo_operation/batch_update_owner_album_id?api_version=10';
  dataShare.createDataShareHelper(context, uri).then((helper: dataShare.DataShareHelper) => {
    let values: ValuesBucket = {};
    let predicates = new dataSharePredicates.DataSharePredicates();
    predicates.equalTo('owner_album_id', oldAlbumId);
    predicates.and().in('file_id', [assetUri]);
    values['owner_album_id'] = newAlbumId;
    helper.update(uri, predicates, values);
    console.info('DataShareHelper.update successfully');
  }).catch ((err: BusinessError) => {
    console.error(`error: ${err.code}, ${err.message}`);
  })
  console.info('DataShareMove Demo End:');
}

@Entry
@Component
struct DataShare {
  build() {
    Row() {
      Column() {
        Text('DataShare').margin(6).fontWeight(FontWeight.Bold)

        Button("DataShareMoveAsset").margin(6).onClick(()=>{
          DataShareMove('', '11', 13);
        })

        Button('DataShareDeleteAlbum').margin(6).onClick(()=> {
          DataShareDelete('phaccess_album_operation', photoAccessHelper.AlbumKeys.ALBUM_NAME, 'test1');
          DataShareDelete('photo_album_v10_operation', photoAccessHelper.AlbumKeys.ALBUM_NAME, 'test2');
        })
      }
      .width('100%')
    }
    .height('100%')
  }
}