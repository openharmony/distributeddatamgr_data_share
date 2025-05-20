import { dataSharePredicates } from '@kit.ArkData';
import photoAccessHelper from '@ohos.file.photoAccessHelper';

let context = getContext(this)
let phAccessHelper = photoAccessHelper.getPhotoAccessHelper(context)

export class AssetUtil {
  async GetAssets(field: string, value: string) {
    let predicates = new dataSharePredicates.DataSharePredicates()
    if (field.length != 0 && value.length != 0) {
      if (field == photoAccessHelper.PhotoKeys.TITLE) {
        predicates.like(field, value)
      } else {
        predicates.equalTo(field, value)
      }
    }
    let fetchOptions: photoAccessHelper.FetchOptions = {
      fetchColumns: ['title'],
      predicates: predicates
    }
    let result: photoAccessHelper.PhotoAsset[] | undefined
    try {
      let fetchResult = await phAccessHelper.getAssets(fetchOptions)
      if (!fetchResult) {
        console.error('GetAssets FAILED')
        return result
      }
      result = await fetchResult.getAllObjects()
      console.info('GetAssets SUCCESS Count:' + fetchResult.getCount())
    } catch (err) {
      console.error(`GetAssets error: ${err.code}, ${err.message}`)
    }
    return result;
  }

  async CreateAsset(titleName: string) {
    console.info('CreateAsset start')
    let result: string | undefined
    let options: photoAccessHelper.CreateOptions = {
      title: titleName
    }
    try {
      result = await phAccessHelper.createAsset(photoAccessHelper.PhotoType.IMAGE, 'jpg', options)
    } catch (err) {
      console.error(`error: ${err.code}, ${err.message}`)
    }
    console.info('CreateAsset end')
    return result
  }

  async CreateAssetWithShortTermPermission(titleName: string) {
    console.info('CreateAssetWithShortTermPermission Start')
    let result: string | undefined
    let config: photoAccessHelper.PhotoCreationConfig = {
      title: titleName,
      fileNameExtension: 'jpg',
      photoType: photoAccessHelper.PhotoType.IMAGE,
      subtype: photoAccessHelper.PhotoSubtype.DEFAULT,
    }
    try {
      result = await phAccessHelper.createAssetWithShortTermPermission(config)
    } catch (err) {
      console.error(`error: ${err.code}, ${err.message}`)
    }
    console.info('CreateAssetWithShortTermPermission End')
    return result
  }

  async DeleteAssets(assets: photoAccessHelper.PhotoAsset[]) {
    console.info('DeleteAssets Start');
    for (let i = 0; assets && i < assets.length; i++) {
      try {
        let asset = assets[i]
        if (i == 0) {
          console.info('phAccessHelper.createDeleteRequest uri: ' + asset.uri)
          await phAccessHelper.createDeleteRequest([asset.uri])
        } else {
          console.info('MediaAssetChangeRequest.deleteAssets uri: ' + asset.uri)
          await photoAccessHelper.MediaAssetChangeRequest.deleteAssets(context, [asset.uri])
        }
      } catch (err) {
        console.error(`error: ${err.code}, ${err.message}`)
      }
    }
    console.info('DeleteAssets End')
  }

  async ModifyAsset(asset: photoAccessHelper.PhotoAsset) {
    console.info('ModifyAsset Start')
    try {
      let title = asset.get(photoAccessHelper.PhotoKeys.TITLE)
      asset.set(photoAccessHelper.PhotoKeys.TITLE, 'modify_' + title)
      asset.commitModify()
      console.info('asset title:' + asset.get(photoAccessHelper.PhotoKeys.TITLE))
    } catch (err) {
      console.error(`error: ${err.code}, ${err.message}`)
    }
    console.info('ModifyAsset end')
  }

  async CloneAsset(asset: photoAccessHelper.PhotoAsset) {
    console.info('CloneAsset Start')
    try {
      let title = asset.get(photoAccessHelper.PhotoKeys.TITLE)
      let clone = await asset.clone('clone_' + title)
      console.info('asset uri:' + clone.uri)
    } catch (err) {
      console.error(`error: ${err.code}, ${err.message}`)
    }
    console.info('CloneAsset end')
  }

  async GetAlbums(type: photoAccessHelper.AlbumType, subtype: photoAccessHelper.AlbumSubtype) {
    let result: photoAccessHelper.Album[] | undefined
    try {
      let fetchResult = await phAccessHelper.getAlbums(type, subtype)
      if (!fetchResult) {
        console.error('GetAlbums FAILED')
        return result
      }
      result = await fetchResult.getAllObjects()
      console.info('GetAlbums SUCCESS Count:' + fetchResult.getCount())
    } catch (err) {
      console.error(`error: ${err.code}, ${err.message}`)
    }
    return result
  }

  async GetAlbumAssets(album: photoAccessHelper.Album) {
    let predicates = new dataSharePredicates.DataSharePredicates()
    let fetchOption: photoAccessHelper.FetchOptions = {
      fetchColumns: ['title'],
      predicates: predicates
    }
    let result: photoAccessHelper.PhotoAsset[] | undefined
    try {
      let fetchResult = await album.getAssets(fetchOption)
      if (!fetchResult) {
        console.error('GetAlbumAssets FAILED')
        return result
      }
      result = await fetchResult.getAllObjects();
      console.info('GetAlbumAssets SUCCESS Count:' + fetchResult.getCount())
    } catch (err) {
      console.error(`error: ${err.code}, ${err.message}`)
    }
    return result
  }

  async RemoveAlbumAssets(album: photoAccessHelper.Album) {
    console.info('RemoveAlbumAssets Start albumName:' + album.albumName + ' albumUri:' + album.albumUri)
    let assets = await this.GetAlbumAssets(album)
    for (let i = 0; assets && i < assets.length; i++) {
      try {
        let asset = assets[i]
        if (i == 0) {
          console.info('Album.removeAssets uri:' + asset.uri)
          await album.removeAssets([asset]);
        } else {
          console.info('MediaAlbumChangeRequest.removeAssets uri:' + asset.uri)
          let request = new photoAccessHelper.MediaAlbumChangeRequest(album)
          request.removeAssets([asset]);
          await phAccessHelper.applyChanges(request)
        }
      } catch (err) {
        console.error(`error: ${err.code}, ${err.message}`)
      }
    }
    console.info('RemoveAlbumAssets End')
  }

  async SystemRecoverAlbumAssets(album: photoAccessHelper.Album) {
    console.info('SystemAlbumRecoverAssets Start albumName:' + album.albumName + ' albumUri:' + album.albumUri)
    let assets = await this.GetAlbumAssets(album)
    for (let i = 0; assets && i < assets.length; i++) {
      try {
        let asset = assets[i]
        if (i == 0) {
          console.info('Album.recoverAssets uri:' + asset.uri)
          await album.recoverAssets([asset])
        } else {
          console.info('MediaAlbumChangeRequest.recoverAssets uri:' + asset.uri)
          let request = new photoAccessHelper.MediaAlbumChangeRequest(album)
          request.recoverAssets([asset]);
          await phAccessHelper.applyChanges(request)
        }
      } catch (err) {
        console.error(`error: ${err.code}, ${err.message}`)
      }
    }
    console.info('SystemAlbumRecoverAssets End')
  }

  async SystemDeleteAlbumAssets(album: photoAccessHelper.Album) {
    console.info('SystemAlbumDeleteAsset Start albumName:' + album.albumName + ' albumUri:' + album.albumUri)
    let assets = await this.GetAlbumAssets(album)
    for (let i = 0; assets && i < assets.length; i++) {
      try {
        let asset = assets[i]
        if (i == 0) {
          console.info('Album.deleteAssets uri:' + asset.uri)
          await album.deleteAssets([asset])
        } else {
          console.info('MediaAlbumChangeRequest.deleteAssets uri:' + asset.uri)
          let request = new photoAccessHelper.MediaAlbumChangeRequest(album)
          request.deleteAssets([asset])
          await phAccessHelper.applyChanges(request);
        }
      } catch (err) {
        console.error(`error: ${err.code}, ${err.message}`);
      }
    }
    console.info('SystemAlbumDeleteAsset End');
  }

  async SystemDeleteAlbum(index: number, album: photoAccessHelper.Album) {
    console.info('SystemDeleteAlbum Start')
    try {
      if (index == 0) {
        console.info('photoAccessHelper.deleteAlbums albumName:' + album.albumName)
        await phAccessHelper.deleteAlbums([album])
      } else {
        console.info('MediaAlbumChangeRequest.deleteAlbums albumName:' + album.albumName)
        await photoAccessHelper.MediaAlbumChangeRequest.deleteAlbums(context, [album])
      }
    } catch (err) {
      console.error(`error: ${err.code}, ${err.message}`)
    }
    console.info('SystemDeleteAlbum end')
  }

  async SystemDeleteAsset(asset: photoAccessHelper.PhotoAsset) {
    console.info('SystemDeleteAsset Start')
    try {
      console.info('phAccessHelper.deleteAssets uri:' + asset.uri)
      await phAccessHelper.deleteAssets([asset.uri])
    } catch (err) {
      console.error(`error: ${err.code}, ${err.message}`)
    }
    console.info('SystemDeleteAsset End')
  }

  async SystemCreateAsset(titleName: string) {
    console.info('SystemCreateAsset Start')
    let result: string | undefined
    try {
      let asset = await phAccessHelper.createAsset(titleName + '.jpg')
      result = asset.uri
    } catch (err) {
      console.error(`error: ${err.code}, ${err.message}`)
    }
    console.info('SystemCreateAsset End')
    return result
  }

  async SystemCreateAssetSubType(titleName: string, subType: photoAccessHelper.PhotoSubtype) {
    console.info('SystemCreateAssetSubType Start')
    let result: string | undefined
    let createOption: photoAccessHelper.PhotoCreateOptions = {
      subtype: subType
    }
    try {
      let asset = await phAccessHelper.createAsset(titleName + '.jpg', createOption);
      result = asset.uri
    } catch (err) {
      console.error(`error: ${err.code}, ${err.message}`)
    }
    console.info('SystemCreateAssetSubType End')
    return result
  }

  async SystemCreateAssetForApp(titleName: string, appName: string, bundleName: string, appId: string) {
    console.info('SystemCreateAssetForApp Start')
    let result: string | undefined
    let configs: Array<photoAccessHelper.PhotoCreationConfig> = [
      {
        title: titleName,
        fileNameExtension: 'jpg',
        photoType: photoAccessHelper.PhotoType.IMAGE,
        subtype: photoAccessHelper.PhotoSubtype.DEFAULT,
      }
    ]
    try {
      let uris = await phAccessHelper.createAssetsForApp(bundleName, appName, appId, configs)
      result = uris[0]
    } catch (err) {
      console.error(`error: ${err.code}, ${err.message}`)
    }
    console.info('SystemCreateAssetForApp End')
    return result
  }

  async SystemCreateAssetForAppWithMode(titleName: string, appName: string, bundleName: string, appId: string, tokenId: number) {
    console.info('SystemCreateAssetForAppWithMode Start')
    let result: string | undefined
    let configs: Array<photoAccessHelper.PhotoCreationConfig> = [
      {
        title: titleName,
        fileNameExtension: 'jpg',
        photoType: photoAccessHelper.PhotoType.IMAGE,
        subtype: photoAccessHelper.PhotoSubtype.DEFAULT,
      }
    ]
    try {
      let mode = photoAccessHelper.AuthorizationMode.SHORT_TIME_AUTHORIZATION;
      let uris = await phAccessHelper.createAssetsForAppWithMode(bundleName, appName, appId, tokenId, mode, configs)
      result = uris[0]
    } catch (err) {
      console.error(`error: ${err.code}, ${err.message}`)
    }
    console.info('SystemCreateAssetForAppWithMode End')
    return result
  }
}