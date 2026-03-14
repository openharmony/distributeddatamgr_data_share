/*
* Copyright (c) 2026 Huawei Device Co., Ltd.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

let OrderByDesc = new dataSharePredicates.DataSharePredicates();
let Wrap = new dataSharePredicates.DataSharePredicates();
let Contains = new dataSharePredicates.DataSharePredicates();
let BeginsWith = new dataSharePredicates.DataSharePredicates();
BeginsWith.beginsWith("name", "Li");
let EndsWith = new dataSharePredicates.DataSharePredicates();
EndsWith.endsWith("name", "i");
let Glob = new dataSharePredicates.DataSharePredicates();
Glob.glob("name", "*i");
let Between = new dataSharePredicates.DataSharePredicates();
let NotBetween = new dataSharePredicates.DataSharePredicates();
NotBetween.notBetween("age", 19 as Long, 20 as Long);
let Distinct = new dataSharePredicates.DataSharePredicates();
Distinct.equalTo("age", 18 as Long).distinct();
let GroupBy = new dataSharePredicates.DataSharePredicates();
let IndexedBy = new dataSharePredicates.DataSharePredicates();
let PredicatesAll = new dataSharePredicates.DataSharePredicates();
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    it('SUB_DDM_DataSharePredicatesRdb1800', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
      try {
        await pubfun.publicupdate(globalThis.helper!, uri, BeginsWith, UpdateLiSi).then((data) => {
          console.info("TestDataShare going update = " + data);
          expect(DataProcessResultOne).assertEqual(data);
        }).catch((err) => {
          console.info("SUB_DDM_DataSharePredicatesRdb1800 Update err" + JSON.stringify(err));
          expect(err).assertFail();
        })
        await pubfun.publicquery(globalThis.helper!, uri, BeginsWith, ["*"]).then((data) => {
          console.info("TestDataShare going query = " + data);
          globalThis.ResultSet!.goToFirstRow();
          Column = globalThis.ResultSet!.getColumnIndex(Name);
          expect(RetLiSii).assertEqual(globalThis.ResultSet!.getString(Column));
          globalThis.ResultSet!.close();
        }).catch((err) => {
          console.info("SUB_DDM_DataSharePredicatesRdb1800 query err" + JSON.stringify(err));
          expect(err).assertFail();
        })
        await pubfun.publicdelete(globalThis.helper!, uri, BeginsWith).then((data) => {
          console.info("TestDataShare going delete = " + data);
          expect(DataProcessResultOne).assertEqual(data);
          done();
        }).catch((err) => {
          console.info("SUB_DDM_DataSharePredicatesRdb1800 delete err" + JSON.stringify(err));
          expect(err).assertFail();
          done();
        })
      } catch (err) {
        console.info("SUB_DDM_DataSharePredicatesRdb1800 fail" + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })
 
 
 
 
 
 
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_7900
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    it('SUB_DDM_DataSharePredicatesRdb1900', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
      try {
        await pubfun.publicupdate(globalThis.helper!, uri, EndsWith, UpdateLiSi).then((data) => {
          console.info("TestDataShare going update = " + data);
          expect(DataProcessResultOne).assertEqual(data);
        }).catch((err) => {
          console.info("SUB_DDM_DataSharePredicatesRdb1900 Update err" + JSON.stringify(err));
          expect(err).assertFail();
        })
        await pubfun.publicquery(globalThis.helper!, uri, EndsWith, ["*"]).then((data) => {
          console.info("TestDataShare going query = " + data);
          globalThis.ResultSet!.goToFirstRow();
          Column = globalThis.ResultSet!.getColumnIndex(Name);
          expect(RetLiSii).assertEqual(globalThis.ResultSet!.getString(Column));
          globalThis.ResultSet!.close();
        }).catch((err) => {
          console.info("SUB_DDM_DataSharePredicatesRdb1900 query err" + JSON.stringify(err));
          expect(err).assertFail();
        })
        await pubfun.publicdelete(globalThis.helper!, uri, EndsWith).then((data) => {
          console.info("TestDataShare going delete = " + data);
          expect(DataProcessResultOne).assertEqual(data);
          done();
        }).catch((err) => {
          console.info("SUB_DDM_DataSharePredicatesRdb1900 delete err" + JSON.stringify(err));
          expect(err).assertFail();
          done();
        })
      } catch (err) {
        console.info("SUB_DDM_DataSharePredicatesRdb1900 fail" + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })
 
 
 
 
 
 
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_8000
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    it('SUB_DDM_DataSharePredicatesRdb2000', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
      try {
        await pubfun.publicupdate(globalThis.helper!, uri, Glob, UpdateLiSi).then((data) => {
          console.info("TestDataShare going update = " + data);
          expect(DataProcessResultOne).assertEqual(data);
        }).catch((err) => {
          console.info("SUB_DDM_DataSharePredicatesRdb2000 Update err" + JSON.stringify(err));
          expect(err).assertFail();
        })
        await pubfun.publicquery(globalThis.helper!, uri, Glob, ["*"]).then((data) => {
          console.info("TestDataShare going query = " + data);
          globalThis.ResultSet!.goToFirstRow();
          Column = globalThis.ResultSet!.getColumnIndex(Name);
          expect(RetLiSii).assertEqual(globalThis.ResultSet!.getString(Column));
          globalThis.ResultSet!.close();
        }).catch((err) => {
          console.info("SUB_DDM_DataSharePredicatesRdb2000 query err" + JSON.stringify(err));
          expect(err).assertFail();
        })
        await pubfun.publicdelete(globalThis.helper!, uri, Glob).then((data) => {
          console.info("TestDataShare going delete = " + data);
          expect(DataProcessResultOne).assertEqual(data);
          done();
        }).catch((err) => {
          console.info("SUB_DDM_DataSharePredicatesRdb2000 delete err" + JSON.stringify(err));
          expect(err).assertFail();
          done();
        })
      } catch (err) {
        console.info("SUB_DDM_DataSharePredicatesRdb2000 fail" + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })
 
 
 
 
 
 
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_8100
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    it('SUB_DDM_DataSharePredicatesRdb2200', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
      try {
        await pubfun.publicupdate(globalThis.helper!, uri, NotBetween, UpdateLiSi).then((data) => {
          console.info("TestDataShare going update = " + data);
          expect(DataProcessResultOne).assertEqual(data);
        }).catch((err) => {
          console.info("SUB_DDM_DataSharePredicatesRdb2200 Update err" + JSON.stringify(err));
          expect(err).assertFail();
        })
        await pubfun.publicquery(globalThis.helper!, uri, NotBetween, ["*"]).then((data) => {
          console.info("TestDataShare going query = " + data);
          globalThis.ResultSet!.goToFirstRow();
          Column = globalThis.ResultSet!.getColumnIndex(Name);
          expect(RetLiSii).assertEqual(globalThis.ResultSet!.getString(Column));
          globalThis.ResultSet!.close();
        }).catch((err) => {
          console.info("SUB_DDM_DataSharePredicatesRdb2200 query err" + JSON.stringify(err));
          expect(err).assertFail();
        })
        await pubfun.publicdelete(globalThis.helper!, uri, NotBetween).then((data) => {
          console.info("TestDataShare going delete = " + data);
          expect(DataProcessResultOne).assertEqual(data);
          done();
        }).catch((err) => {
          console.info("SUB_DDM_DataSharePredicatesRdb2200 delete err" + JSON.stringify(err));
          expect(err).assertFail();
          done();
        })
      } catch (err) {
        console.info("SUB_DDM_DataSharePredicatesRdb2200 fail" + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })
 
 
 
 
 
 
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_8300
     * @tc.type      : Function
     * @tc.level     : Level 1
     */
    it('SUB_DDM_DataSharePredicatesRdb2300', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL1, async (done: () => void): Promise<void> => {
      try {
        await pubfun.publicquery(globalThis.helper!, uri, Distinct, ["*"]).then((data) => {
          console.info("TestDataShare going query = " + data);
          globalThis.ResultSet!.goToFirstRow();
          Column = globalThis.ResultSet!.getColumnIndex(Name);
          expect(RetLiSi).assertEqual(globalThis.ResultSet!.getString(Column));
          globalThis.ResultSet!.close();
          done();
        }).catch((err) => {
          console.info("SUB_DDM_DataSharePredicatesRdb2300 query err" + JSON.stringify(err));
          expect(err).assertFail();
          done();
        })
      } catch (err) {
        console.info("SUB_DDM_DataSharePredicatesRdb2300 fail" + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })
 
 
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_8400
    //   try {
    //     await pubfun.publicinsert(globalThis.helper!, uri, Insert).then((data) => {
    //       console.info("TestDataShare going batchInsert = " + data);
    //       IndexedBy.indexedBy(globalThis.insertData!.toString());
    //     })
    //     await pubfun.publicquery(globalThis.helper!, uri, IndexedBy, ["*"]).then((data) => {
    //       console.info("TestDataShare going query = " + data);
    //       expect(RetLiSi).assertEqual(globalThis.ResultSet!.getString(Column));
    //       globalThis.ResultSet!.close();
    //       done();
    //     }).catch((err) => {
    //       console.info("SUB_DDM_DataSharePredicatesRdb2500 query err" + JSON.stringify(err));
 
 
    //       expect(err).assertFail();
    //       done();
    //     })

    //normalizeUri公共方法
function publicnormalizeUri(helper: dataShare.DataShareHelper, uri: string): Promise<string> {
  return new Promise<string>((resolve, reject) => {
    helper.normalizeUri(uri, (err: BusinessError | null, data: string | undefined) => {
      console.info("[ttt] [TestDataShare]  publicnormalizeUri Callback=" + JSON.stringify(data));
      if (err) {
        console.info("test normalizeUri err information: " + JSON.stringify(err));
        reject(err as BusinessError);
      } else {
        resolve(data as string);
      }
    })
  })
}
 
//denormalizeUri公共方法
function publicdenormalizeUri(helper: dataShare.DataShareHelper, uri: string): Promise<string> {
  return new Promise<string>((resolve, reject) => {
    helper.denormalizeUri(uri, (err: BusinessError | null, data: string | undefined) => {
      console.info("[ttt] [TestDataShare]  publicdenormalizeUri Callback=" + JSON.stringify(data));
      if (err) {
        console.info("test denormalizeUri err information: " + JSON.stringify(err));
        reject(err as BusinessError);
      } else {
        resolve(data as string);
      }
    })
  })
}
//
// //denormalizeUri公共方法
// function publicopenFile(helper, uri, mode) {
//   })
// }
 
export { publicinsert, publicdelete, publicquery, publicupdate, publicbatchInsert, publicnormalizeUri, publicdenormalizeUri }

// normalizeUri公共方法
function publicnormalizeUri(helper: dataShare.DataShareHelper, uri: string): Promise<string> {
  return new Promise<string>((resolve, reject) => {
    helper.normalizeUri(uri).then((data: string) => {
      console.info("[ttt] [TestDataShare]  publicnormalizeUri Callback=" + JSON.stringify(data));
      resolve(data);
    }).catch((err) => {
      console.info("test normalizeUri err information: " + JSON.stringify(err));
      reject(err);
    })
  })
}
 
 
 
// denormalizeUri公共方法
function publicdenormalizeUri(helper: dataShare.DataShareHelper, uri: string): Promise<string> {
  return new Promise<string>((resolve, reject) => {
    helper.denormalizeUri(uri).then((data: string) => {
      console.info("[ttt] [TestDataShare]  publicdenormalizeUri Callback=" + JSON.stringify(data));
      resolve(data);
    }).catch((err) => {
      console.info("test denormalizeUri err information: " + JSON.stringify(err));
      reject(err);
    })
  })
}
 
 
 
//denormalizeUri公共方法
// function publicopenFile(helper, uri, mode) {
//   })
// }
 
export { publicinsert, publicdelete, publicquery, publicupdate, publicbatchInsert, publicnormalizeUri, publicdenormalizeUri }

    it('SUB_DDM_DataShareUpdateRdb0400', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL0, async (done: () => void): Promise<void> => {
      try {
        await pubfun.publicinsert(globalThis.helper!, uri, InsertUint8Array).then((data) => {
          console.info("TestDataShare going insert = " + data);
          let count = globalThis.Count as int;
          globalThis.Count = count + 1;
          expect(globalThis.Count!).assertEqual(data);
        })
        await pubfun.publicupdate(globalThis.helper!, uri, PredicatesUint8Array, UpdateUint8Array).then((data) => {
          console.info("TestDataShare going update = " + data);
          expect(DataProcessResultOne).assertEqual(data);
        }).catch((err) => {
          console.info("SUB_DDM_DataShareUpdateRdb0400 err " + JSON.stringify(err));
          expect(err).assertFail();
        })
        await pubfun.publicdelete(globalThis.helper!, uri, PredicatesAll).then((data) => {
          console.info("TestDataShare going delete = " + data);
          expect(DataProcessResultOne).assertEqual(data);
          done();
        })
      } catch (err) {
        console.info("SUB_DDM_DataShareUpdateRdb0400 fail " + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })
 
 
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_1000
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_DDM_DataShareQueryRdb0400', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL0, async (done: () => void): Promise<void> => {
      try {
        await pubfun.publicinsert(globalThis.helper!, uri, InsertUint8Array).then((data) => {
          console.info("TestDataShare going insert = " + data);
          let count = globalThis.Count as int;
          globalThis.Count = count + 1;
          expect(globalThis.Count!).assertEqual(data);
        })
        await pubfun.publicquery(globalThis.helper!, uri, PredicatesUint8Array, ["*"]).then((data) => {
          console.info("TestDataShare going query = " + data);
          globalThis.ResultSet!.goToFirstRow();
          expect(JSON.stringify(ResultSetUint8Array))
            .assertEqual(
              JSON.stringify(globalThis.ResultSet!.getBlob(globalThis.ResultSet!.getColumnIndex(IndexUint8Array))));
          globalThis.ResultSet!.close();
        }).catch((err) => {
          console.info("SUB_DDM_DataShareQueryRdb0400 err " + JSON.stringify(err));
          expect(err).assertFail();
        })
        await pubfun.publicdelete(globalThis.helper!, uri, PredicatesAll).then((data) => {
          console.info("TestDataShare going delete = " + data);
          expect(DataProcessResultOne).assertEqual(data);
          done();
        })
      } catch (err) {
        console.info("SUB_DDM_DataShareQueryRdb0400 fail " + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })
 
 
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_1500
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_DDM_DataShareDeleteRdb0400', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL0, async (done: () => void): Promise<void> => {
      try {
        await pubfun.publicinsert(globalThis.helper!, uri, InsertUint8Array).then((data) => {
          console.info("TestDataShare going insert = " + data);
          let count = globalThis.Count as int;
          globalThis.Count = count + 1;
          expect(globalThis.Count!).assertEqual(data);
        })
        await pubfun.publicdelete(globalThis.helper!, uri, PredicatesUint8Array).then((data) => {
          console.info("TestDataShare going delete = " + data);
          expect(DataProcessResultOne).assertEqual(data);
          done();
        }).catch((err) => {
          console.info("SUB_DDM_DataShareDeleteRdb0400 err " + JSON.stringify(err));
          expect(err).assertFail();
          done();
        })
      } catch (err) {
        console.info("SUB_DDM_DataShareDeleteRdb0400 fail " + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })
 
 
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_2000
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_DDM_DataShareBatchRdb0300', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL0, async (done: () => void): Promise<void> => {
      try {
        await pubfun.publicbatchInsert(globalThis.helper!, uri, BatchInsert).then((data) => {
          console.info("TestDataShare going batchInsert = " + data);
          globalThis.Count = globalThis.Count! + DataProcessResultFour;
          expect(DataProcessResultFour).assertEqual(data);
        })
        await pubfun.publicquery(globalThis.helper!, uri, PredicatesAll, ["*"]).then((data) => {
          console.info("TestDataShare going query = " + data);
          globalThis.ResultSet!.goToLastRow();
          expect(JSON.stringify(ResultSetUint8Array))
            .assertEqual(
              JSON.stringify(globalThis.ResultSet!.getBlob(globalThis.ResultSet!.getColumnIndex(IndexUint8Array))));
          globalThis.ResultSet!.close();
        }).catch((err) => {
          console.info("SUB_DDM_DataShareBatchRdb0300 err " + JSON.stringify(err));
          expect(err).assertFail();
        })
        await pubfun.publicdelete(globalThis.helper!, uri, PredicatesAll).then((data) => {
          console.info("TestDataShare going delete = " + data);
          expect(DataProcessResultFour).assertEqual(data);
          done();
        })
      } catch (err) {
        console.info("SUB_DDM_DataShareBatchRdb0300 fail " + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })
 
 
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_2400
    //       console.info("TestDataShare going NormalizeUri = " + data);
    //       expect(Normalize).assertEqual(data);
    //       done();
    //     }).catch((err) => {
    //       console.info("SUB_DDM_DataShareNormalizeUri0100 err " + JSON.stringify(err));
 
 
    //       expect(err).assertFail();
    //       done();
    //     })
    //       console.info("TestDataShare going denormalizeUri = " + data);
    //       expect(Denormalize).assertEqual(data);
    //       done();
    //     }).catch((err) => {
    //       console.info("SUB_DDM_DataShareDenormalizeUri0100 err " + JSON.stringify(err));
 
 
    //       expect(err).assertFail();
    //       done();
    //     })

        it('SUB_DDM_DataShareQueryRdb0900', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL0, async (done: () => void): Promise<void> => {
      try {
        await pubfun.publicinsert(globalThis.helper!, uri, InsertUint8Array).then((data) => {
          console.info("TestDataShare going insert = " + data);
          globalThis.Count = globalThis.Count! + 1;
          expect(globalThis.Count!).assertEqual(data);
        })
        await pubfun.publicquery(globalThis.helper!, uri, PredicatesUint8Array, ["*"]).then((data) => {
          console.info("TestDataShare going query = " + data);
          globalThis.ResultSet!.goToFirstRow();
          expect(JSON.stringify(ResultSetUint8Array))
            .assertEqual(
              JSON.stringify(globalThis.ResultSet!.getBlob(globalThis.ResultSet!.getColumnIndex(IndexUint8Array))));
          globalThis.ResultSet!.close();
        }).catch((err) => {
          console.info("SUB_DDM_DataShareQueryRdb0900 err " + JSON.stringify(err));
          expect(err).assertFail();
        })
        await pubfun.publicdelete(globalThis.helper!, uri, PredicatesAll).then((data) => {
          console.info("TestDataShare going delete = " + data);
          expect(DataProcessResultOne).assertEqual(data);
          done();
        })
      } catch (err) {
        console.info("SUB_DDM_DataShareQueryRdb0900 fail " + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })
 
 
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_4500
     * @tc.type      : Function
     * @tc.level     : Level 0
     */
    it('SUB_DDM_DataShareBatchRdb0700', TestType.FUNCTION | Size.MEDIUMTEST | Level.LEVEL0, async (done: () => void): Promise<void> => {
      try {
        await pubfun.publicbatchInsert(globalThis.helper!, uri, BatchInsert).then((data) => {
          console.info("TestDataShare going batchInsert = " + data);
          globalThis.Count = globalThis.Count! + DataProcessResultFour;
          expect(DataProcessResultFour).assertEqual(data);
        })
        await pubfun.publicquery(globalThis.helper!, uri, PredicatesAll, ["*"]).then((data) => {
          console.info("TestDataShare going query = " + data);
          globalThis.ResultSet!.goToLastRow();
          expect(JSON.stringify(ResultSetUint8Array))
            .assertEqual(
              JSON.stringify(globalThis.ResultSet!.getBlob(globalThis.ResultSet!.getColumnIndex(IndexUint8Array))));
          globalThis.ResultSet!.close();
        }).catch((err) => {
          console.info("SUB_DDM_DataShareBatchRdb0700 err " + JSON.stringify(err));
          expect(err).assertFail();
        })
        await pubfun.publicdelete(globalThis.helper!, uri, PredicatesAll).then((data) => {
          console.info("TestDataShare going delete = " + data);
          expect(DataProcessResultFour).assertEqual(data);
          done();
        })
      } catch (err) {
        console.info("SUB_DDM_DataShareBatchRdb0700 fail " + JSON.stringify(err));
        expect(err).assertFail();
        done();
      }
    })
 
 
 
    /**
     * @tc.number    : SUB_DistributedData_DataShare_SDK_RdbJsApiTest_5400
    //       console.info("TestDataShare going NormalizeUri = " + data);
    //       expect(Normalize).assertEqual(data);
    //       done();
    //     }).catch((err) => {
    //       console.info("SUB_DDM_DataShareNormalizeUri0200 err " + JSON.stringify(err));
 
 
    //       expect(err).assertFail();
    //       done();
    //     })
    //       console.info("TestDataShare going denormalizeUri = " + data);
    //       expect(Denormalize).assertEqual(data);
    //       done();
    //     }).catch((err) => {
    //       console.info("SUB_DDM_DataShareDenormalizeUri0200 err " + JSON.stringify(err));
 
 
    //       expect(err).assertFail();
    //       done();
    //     })

    