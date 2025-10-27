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
import bundleManager from '@ohos.bundle.bundleManager'
import { BusinessError } from '@kit.BasicServicesKit'
import dataShare from '@ohos.data.dataShare'

import common from "@ohos.app.ability.common"
let context: common.UIAbilityContext
context = AppStorage.get<common.UIAbilityContext>("TestAbilityContext") as common.UIAbilityContext
let g_handle : dataShare.DataProxyHandle | undefined = undefined
let selfBundleName : 'com.ohos.datashareclient'
let visitorAppIdentifier = '6918703363676659027'
// let g_type : dataShare.DataProxyType
let g_type = 0 as dataShare.DataProxyType.SHARED_CONFIG
let g_config : dataShare.DataProxyConfig = {
  type : 0 as dataShare.DataProxyType.SHARED_CONFIG
}
let config : dataShare.DataProxyConfig = {
  type : 0 as dataShare.DataProxyType.SHARED_CONFIG
}
let allowAllUri = "datashareproxy://com.test.dataproxyhandle/allowall"
let allowNoneUri = "datashareproxy://com.test.dataproxyhandle/allownone"
// let testUri1 = "datashareproxy://com.test.dataproxyhandle/test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1"
let testUri1 = "datashareproxy://com.test.dataproxyhandle/test01"
let testUri2 = "datashareproxy://com.test.dataproxyhandle/test02"
let testUri3 = "datashareproxy://com.test.dataproxyhandle/test03"
let testUri4 = "datashareproxy://com.test.dataproxyhandle/test04"

let g_proxyDataAllowAll : dataShare.ProxyData = {
  uri : allowAllUri,
  value : 'proxyHandleTest',
  allowList : [
    'all'
  ]
}

let g_proxyDataAllowNone : dataShare.ProxyData = {
  uri : allowNoneUri,
  value : 'proxyHandleTest',
  allowList : []
}

let g_proxyDataExceedLimit : dataShare.ProxyData = {
  uri : testUri1,
  value : "datashareproxy://com.test.dataproxyhandle/test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1testdatashareproxy://com.test.dataproxyhandle/test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1testdatashareproxy://com.test.dataproxyhandle/test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1testdatashareproxy://com.test.dataproxyhandle/test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1testdatashareproxy://com.test.dataproxyhandle/test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1testdatashareproxy://com.test.dataproxyhandle/test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1testdatashareproxy://com.test.dataproxyhandle/test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1testdatashareproxy://com.test.dataproxyhandle/test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1testdatashareproxy://com.test.dataproxyhandle/test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1testdatashareproxy://com.test.dataproxyhandle/test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1testdatashareproxy://com.test.dataproxyhandle/test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1testdatashareproxy://com.test.dataproxyhandle/test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1testdatashareproxy://com.test.dataproxyhandle/test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1testdatashareproxy://com.test.dataproxyhandle/test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1testdatashareproxy://com.test.dataproxyhandle/test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1testdatashareproxy://com.test.dataproxyhandle/test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test1test",
  allowList : [
    "12345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678123456781234567812345678",
  ]
}

let execeedUris = [
  testUri1,
  "testUri2",
  "testUri3",
  "testUri4",
]

let g_proxyDataAllowTest1 : dataShare.ProxyData = {
  uri : testUri1,
  value : '123456777',
  allowList : [
    visitorAppIdentifier,
    "111111",
    "222222",
    "11111"
  ]
}

let g_updateProxyDataTest1 : dataShare.ProxyData = {
  uri : testUri1,
  value : '123456654321',
  allowList : [
    visitorAppIdentifier,
    "111111",
    "222222"
  ]
}

let g_proxyDataAllowTest2 : dataShare.ProxyData = {
  uri : testUri2,
  value : 123456,
  allowList : [

  ]
}

let g_proxyDataAllowTest3 : dataShare.ProxyData = {
  uri : testUri3,
  value : true,
  allowList : [

  ]
}

let g_proxyDataAllowTest4 : dataShare.ProxyData = {
  uri : testUri4,
  value : 654321.123456,
  allowList : [
    visitorAppIdentifier
  ]
}

let g_proxyDataAllowTest5 : dataShare.ProxyData = {
  uri : "datashareproxy://com.test.dataproxyhandle/test5",
  value : 654321.123456,
  allowList : [
    visitorAppIdentifier
  ]
}

let g_proxyDataAllowTest6 : dataShare.ProxyData = {
  uri : "datashareproxy://com.test.dataproxyhandle/test6",
  value : 654321.123456,
  allowList : [
    visitorAppIdentifier
  ]
}

let g_proxyDataAllowTest7 : dataShare.ProxyData = {
  uri : "datashareproxy://com.test.dataproxyhandle/test7",
  value : 654321.123456,
  allowList : [
    visitorAppIdentifier
  ]
}

let g_proxyDataAllowTest8 : dataShare.ProxyData = {
  uri : "datashareproxy://com.test.dataproxyhandle/test8",
  value : 654321.123456,
  allowList : [
    visitorAppIdentifier
  ]
}
let g_proxyDataAllowTest9 : dataShare.ProxyData = {
  uri : "datashareproxy://com.test.dataproxyhandle/test9",
  value : 654321.123456,
  allowList : [
    visitorAppIdentifier
  ]
}
let g_proxyDataAllowTest10 : dataShare.ProxyData = {
  uri : "datashareproxy://com.test.dataproxyhandle/test10",
  value : 654321.123456,
  allowList : [
    visitorAppIdentifier
  ]
}
let g_proxyDataAllowTest11 : dataShare.ProxyData = {
  uri : "datashareproxy://com.test.dataproxyhandle/test11",
  value : 654321.123456,
  allowList : [
    visitorAppIdentifier
  ]
}
let g_proxyDataAllowTest12 : dataShare.ProxyData = {
  uri : "datashareproxy://com.test.dataproxyhandle/test12",
  value : 654321.123456,
  allowList : [
    visitorAppIdentifier
  ]
}
let g_proxyDataAllowTest13 : dataShare.ProxyData = {
  uri : "datashareproxy://com.test.dataproxyhandle/test13",
  value : 654321.123456,
  allowList : [
    visitorAppIdentifier
  ]
}
let g_proxyDataAllowTest14 : dataShare.ProxyData = {
  uri : "datashareproxy://com.test.dataproxyhandle/test14",
  value : 654321.123456,
  allowList : [
    visitorAppIdentifier
  ]
}
let g_proxyDataAllowTest15 : dataShare.ProxyData = {
  uri : "datashareproxy://com.test.dataproxyhandle/test15",
  value : 654321.123456,
  allowList : [
    visitorAppIdentifier
  ]
}
let g_proxyDataAllowTest16 : dataShare.ProxyData = {
  uri : "datashareproxy://com.test.dataproxyhandle/test16",
  value : 654321.123456,
  allowList : [
    visitorAppIdentifier
  ]
}
let g_proxyDataAllowTest17 : dataShare.ProxyData = {
  uri : "datashareproxy://com.test.dataproxyhandle/test17",
  value : 654321.123456,
  allowList : [
    visitorAppIdentifier
  ]
}
let g_proxyDataAllowTest18 : dataShare.ProxyData = {
  uri : "datashareproxy://com.test.dataproxyhandle/test18",
  value : 654321.123456,
  allowList : [
    visitorAppIdentifier
  ]
}
let g_proxyDataAllowTest19 : dataShare.ProxyData = {
  uri : "datashareproxy://com.test.dataproxyhandle/test19",
  value : 654321.123456,
  allowList : [
    visitorAppIdentifier
  ]
}
let g_proxyDataAllowTest20 : dataShare.ProxyData = {
  uri : "datashareproxy://com.test.dataproxyhandle/test20",
  value : 654321.123456,
  allowList : [
    visitorAppIdentifier
  ]
}
let g_proxyDataAllowTest21 : dataShare.ProxyData = {
  uri : "datashareproxy://com.test.dataproxyhandle/test21",
  value : 654321.123456,
  allowList : [
    visitorAppIdentifier
  ]
}
let g_proxyDataAllowTest22 : dataShare.ProxyData = {
  uri : "datashareproxy://com.test.dataproxyhandle/test22",
  value : 654321.123456,
  allowList : [
    visitorAppIdentifier
  ]
}
export function getSelfBundleInfo() {
  console.log("[ttt] [DataProxyClientTest] getSelfBundleInfo begin");
  // let appIdentifier = "";
  let bundleFlags = bundleManager.BundleFlag.GET_BUNDLE_INFO_WITH_APPLICATION |
                    bundleManager.BundleFlag.GET_BUNDLE_INFO_WITH_METADATA;
  try {
    bundleManager.getBundleInfoForSelf(bundleFlags).then((data) => {
      // console.log("[ttt] [DataProxyClientTest] getSelfBundleInfo bundleName : " + data.name + ", appIdentifier : " + data.signatureInfo.appIdentifier);
      console.log("[ttt] [DataProxyClientTest] getSelfBundleInfo bundleName : " + JSON.stringify(data));
      // appIdentifier = data.signatureInfo.appIdentifier;
    }).catch((err : BusinessError) => {
      console.log("[ttt] [DataProxyClientTest] getSelfBundleInfo failed, err : " + JSON.stringify(err));
    })
  } catch(e) {
    console.log("[ttt] [DataProxyClientTest] getSelfBundleInfo failed, e : " + JSON.stringify(e));
  }
  // return appIdentifier;
}

export function createDataProxyHandleTest() {
  // let errcode = dataShare.DataProxyErrorCode.SUCCESS;
  // console.log(`[ttt] [DataProxyClientTest] createDataProxyHandleTest errcode ${errcode}`);
  console.log("[ttt] [DataProxyClientTest] createDataProxyHandleTest begin");
  try {
    dataShare.createDataProxyHandle().then((handle : dataShare.DataProxyHandle) => {
      if (handle == null || handle == undefined) {
        console.log("[ttt] [DataProxyClientTest] createDataProxyHandle failed, handle is null");
        return;
      }
      g_handle = handle;
      console.log("[ttt] [DataProxyClientTest] createDataProxyHandle success");
    }).catch((err : BusinessError) => {
      console.log("[ttt] [DataProxyClientTest] createDataProxyHandle failed, err : " + JSON.stringify(err));
    })
  } catch (e) {
    console.log("[ttt] [DataProxyClientTest] createDataProxyHandle failed, err : " + JSON.stringify(e));
  }

}

export function publishProxyDataTest001() {
  console.log("[ttt] [DataProxyClientTest] publishProxyDataTest001 begin");
  try {
    if (g_handle == null || g_handle == undefined) {
      console.log("[ttt] [DataProxyClientTest] publish proxyData failed, g_handle is null");
      return;
    }
  } catch(e) {
    console.log("[ttt] [DataProxyClientTest] publish proxyData failed, err : " + JSON.stringify(e));
    return;
  }

  let data : dataShare.ProxyData = {
    uri : "datashareproxy://com.test.dataproxyhandle/test111111111111",
    value : 654321.123456,
    allowList : [
      visitorAppIdentifier
    ]
  }

  let proxyData : dataShare.ProxyData[] = [
    data,
    g_proxyDataAllowTest1,
    g_proxyDataAllowTest2,
    g_proxyDataAllowTest3,
  ]

  let count : number = 0;
  proxyData.forEach(() => {
    count += 1;
  })
  console.log(`[ttt] [DataProxyClientTest] proxyData.size is ${count}`)
  try {
    g_handle.publish(proxyData, config).then((results: dataShare.DataProxyResult[]) => {
      console.log("[ttt] [DataProxyClientTest] publishProxyDataTest001 didn't catch error, results: " + JSON.stringify(results));
    }).catch((err: BusinessError) => {
      console.log("[ttt] [DataProxyClientTest] publishProxyDataTest001 failed, err : " + JSON.stringify(err.code) + "message" + JSON.stringify((err.message)));
    })
  } catch(e) {
    console.log("[ttt] [DataProxyClientTest] publishProxyDataTest001 failed, err : " + JSON.stringify(e));
  }
}

export function publishProxyDataTest002() {
  console.log("[ttt] [DataProxyClientTest] publishProxyDataTest002 begin");
  try {
    if (g_handle == null || g_handle == undefined) {
      console.log("[ttt] [DataProxyClientTest] publish proxyData failed, g_handle is null");
      return;
    }
  } catch(e) {
    console.log("[ttt] [DataProxyClientTest] publish proxyData failed, err : " + JSON.stringify(e));
    return;
  }


  let proxyData : dataShare.ProxyData[] = [
    g_updateProxyDataTest1
  ]

  let count : number = 0;
  proxyData.forEach(() => {
    count += 1;
  })
  console.log(`[ttt] [DataProxyClientTest] proxyData.size is ${count}`)
  try {
    g_handle.publish(proxyData, config).then((results: dataShare.DataProxyResult[]) => {
      console.log("[ttt] [DataProxyClientTest] publishProxyDataTest001 didn't catch error, results: " + JSON.stringify(results));
    }).catch((err: BusinessError) => {
      console.log("[ttt] [DataProxyClientTest] publishProxyDataTest001 failed, err : " + JSON.stringify(err.code) + "message" + JSON.stringify((err.message)));
    })
  } catch(e) {
    console.log("[ttt] [DataProxyClientTest] publishProxyDataTest001 failed, err : " + JSON.stringify(e));
  }
}

export function deleteProxyDataTest001() {
  console.log("[ttt] [DataProxyClientTest] deleteProxyDataTest001 begin");
  if (g_handle == null || g_handle == undefined) {
    console.log("[ttt] [DataProxyClientTest] delete proxyData failed, g_handle is null");
    return;
  }

  // let uris = [
  //   // testUri1,
  //   // testUri2,
  //   testUri3,
  //   testUri4,
  // ]

  let uris = [
    "datashareproxy://com.test.dataproxyhandle/test111111111111",
    // "datashareproxy://com.test.dataproxyhandle/test02",
    // "datashareproxy://com.test.dataproxyhandle/test03",
    // "datashareproxy://com.test.dataproxyhandle/test04",
    // "datashareproxy://com.test.dataproxyhandle/test05",
    // "datashareproxy://com.test.dataproxyhandle/test06",
    // "datashareproxy://com.test.dataproxyhandle/test07",
    // "datashareproxy://com.test.dataproxyhandle/test08",
    // "datashareproxy://com.test.dataproxyhandle/test09",
    // "datashareproxy://com.test.dataproxyhandle/test010",
    // "datashareproxy://com.test.dataproxyhandle/test011",
    // "datashareproxy://com.test.dataproxyhandle/test012"
  ]

  // let uris = execeedUris;
  try{
    g_handle.delete(uris, config).then((results : dataShare.DataProxyResult[]) => {
      console.log("[ttt] [DataProxyClientTest] deleteProxyDataTest001 didn't catch error, results: " + JSON.stringify(results));
    }).catch((err: BusinessError) => {
      console.log("[ttt] [DataProxyClientTest] deleteProxyDataTest001 failed, err : " + JSON.stringify(err));
    })
  } catch(e) {
    console.log("[ttt] [DataProxyClientTest] deleteProxyDataTest001 failed, err : " + JSON.stringify(e));
  }
}

export function getProxyDataTest001() {
  console.log("[ttt] [DataProxyClientTest] getProxyDataTest001 begin");
  if (g_handle == null || g_handle == undefined) {
    console.log("[ttt] [DataProxyClientTest] get proxyData failed, g_handle is null");
    return;
  }

  let uris = [
    "datashareproxy://com.test.dataproxyhandle/test111111111111",
    testUri1,
    testUri2,
    testUri3,
    testUri4,
    // testUri1,
    // testUri2,
    // testUri3,
    // testUri4,
    // testUri1,
    // testUri2,
    // testUri3,
    // testUri4,
    // testUri1,
    // testUri2,
    // testUri3,
    // testUri4,
    // testUri1,
    // testUri2,
    // testUri3,
    // testUri4,
    // testUri1,
    // testUri2,
    // testUri3,
    // testUri4,
    // testUri1,
    // testUri2,
    // testUri3,
    // testUri4,
    // testUri1,
    // testUri2,
    // testUri3,
    // testUri4,
    // testUri4,
  ]
  // let uris = execeedUris;
  try{
    g_handle.get(uris, config).then((results : dataShare.DataProxyGetResult[]) => {
      // console.log("[ttt] [DataProxyClientTest] getProxyDataTest001 didn't catch error, results: " + JSON.stringify(results));
      results.forEach((res) => {
        console.log("[ttt] [DataProxyClientTest] getProxyDataTest001 success, results: " + JSON.stringify(res));
        if (res.value == undefined) {
          console.log("[ttt] [DataProxyClientTest] getProxyDataTest001 success, value is undefined")
        }
        if (res.allowList == undefined) {
          console.log("[ttt] [DataProxyClientTest] getProxyDataTest001 success, allowlist is undefined")
        }
      })
    }).catch((err : BusinessError) => {
      console.log("[ttt] [DataProxyClientTest] getProxyDataTest001 failed, err : " + JSON.stringify(err));
    })
  } catch(e) {
    console.log("[ttt] [DataProxyClientTest] getProxyDataTest001 failed, err : " + JSON.stringify(e));
  }
}

function onProxyDataCallback(err : BusinessError, changeInfo : dataShare.DataProxyChangeInfo[]) {
  if (err) {
    console.log("[ttt] [DataProxyClientTest] onProxyDataCallback error : " + JSON.stringify(err));
  } else {
    console.log("[ttt] [DataProxyClientTest] onProxyData receive change : " + JSON.stringify(changeInfo));
  }
}

export function onProxyDataTest001() {
  console.log("[ttt] [DataProxyClientTest] onProxyDataTest001 begin");
  if (g_handle == null || g_handle == undefined) {
    console.log("[ttt] [DataProxyClientTest] on proxyData failed, g_handle is null");
    return;
  }

  let uris = [
    testUri1
  ]
// let uris = execeedUris;

  try {
    let results = g_handle.on('dataChange', uris, config, onProxyDataCallback);
    console.log("[ttt] [DataProxyClientTest] onProxyDataTest001 success : " + JSON.stringify(results));
  } catch(err) {
    console.log("[ttt] [DataProxyClientTest] onProxyDataTest001 failed, err : " + JSON.stringify(err));
  }
}

export function offProxyDataTest001() {
  console.log("[ttt] [DataProxyClientTest] offProxyDataTest001 begin");
  if (g_handle == null || g_handle == undefined) {
    console.log("[ttt] [DataProxyClientTest] off proxyData failed, g_handle is null");
    return;
  }

  let uris = [
    testUri1,
    testUri2,
    testUri3
  ]
  // let uris = execeedUris;

  try {
    let results = g_handle.off('dataChange', uris, config, onProxyDataCallback);
    console.log("[ttt] [DataProxyClientTest] offProxyDataCallback success : " + JSON.stringify(results));
  } catch(err) {
    console.log("[ttt] [DataProxyClientTest] offProxyDataTest001 failed, err : " + JSON.stringify(err));
  }
}