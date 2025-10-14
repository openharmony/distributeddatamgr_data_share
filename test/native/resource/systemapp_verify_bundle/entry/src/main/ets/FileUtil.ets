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

import fs from '@ohos.file.fs';

export function ReadPhotoData(uri: string) {
  console.info('ReadPhotoData:' + uri)
  try {
    let src = fs.openSync(uri, fs.OpenMode.READ_ONLY)
    let data: ArrayBuffer = new ArrayBuffer(100)
    fs.readSync(src.fd, data)
    fs.closeSync(src)
  } catch (err) {
    console.error(`error: ${err.code}, ${err.message}`)
  }
}

export function CopyPhotoData(srcUri: string, dstUri: string) {
  console.info('CopyPhotoData srcUri: ' + srcUri)
  console.info(' => dstUri: ' + dstUri)
  try {
    let dst = fs.openSync(dstUri, fs.OpenMode.WRITE_ONLY)
    let src = fs.openSync(srcUri, fs.OpenMode.READ_ONLY)
    fs.copyFileSync(src.fd, dst.fd, 0)
    fs.closeSync(src)
    fs.closeSync(dst)
  } catch (err) {
      console.error(`error: ${err.code}, ${err.message}`)
  }
}
