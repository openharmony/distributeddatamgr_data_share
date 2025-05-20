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
