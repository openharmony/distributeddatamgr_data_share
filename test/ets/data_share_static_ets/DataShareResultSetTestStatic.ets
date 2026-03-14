/*
 * Copyright (C) 2026 Huawei Device Co., Ltd.
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
 
import { describe, it, expect, TestType, Size, Level, beforeAll, beforeEach, afterEach, afterAll} from "../../../hypium/index";
import hilog from '@ohos.hilog'
import Utils from './Util.test';
import common from "@ohos.app.ability.common"
import { BusinessError } from '@ohos.base';
import { ValuesBucket } from '@ohos.data.ValuesBucket';
import abilityDelegatorRegistry from '@ohos.app.ability.abilityDelegatorRegistry';
import UIAbility from '@ohos.app.ability.UIAbility';
import performance from './Performance.util'
import dataShare from '@ohos.data.dataShare';
import dataSharePredicates from '@ohos.data.dataSharePredicates';
import DataShareResultSet from '@ohos.data.DataShareResultSet';
import contextConstant from '@ohos.app.ability.contextConstant';
import { systemDateTime } from '@kit.BasicServicesKit';
 
let domain: int = 0x0000; //日志标识,0x0000作为测试框架的业务标识
let tag: string = 'testTag'; //日志标识字符串,作为tag标识当前runner类下的测试行为
 
// 基线值单位us，根据性能专项规格小于100us直接加10us,大于100us上浮20%
let BASELINE_GOTONEXTROW = 11;
let BASELINE_GETSTRING = 11;
let BASELINE_GETLONG = 11;
let BASELINE_GETCOLUMNINDEX = 11;
export default function dataShareResultSetTest() {
 
    describe('dataShareResultSetTest', () => {
        let uri = ("datashareproxy://com.acts.ohos.data.datasharetest.static/test");
        let context: common.UIAbilityContext;
        let dataShareHelper: dataShare.DataShareHelper | undefined = undefined;
        let dsHelper: dataShare.DataShareHelper;
        const BASE_COUNT = 2000 // 循环次数:  测试普通接口性能
        beforeAll(async () => {
            hilog.info(domain, tag, '%{public}s', 'beforeAll start');
            let abilityDelegator: abilityDelegatorRegistry.AbilityDelegator = abilityDelegatorRegistry.getAbilityDelegator()
            abilityDelegator.addAbilityMonitor({
                abilityName: "EntryAbility",
                moduleName: "entry",
                onAbilityCreate: (ability: UIAbility) : void => {
                    context = ability.context;
                    hilog.info(domain, tag, '%{public}s', '[ttt] ability.context.databaseDir:' + context.databaseDir);
                    hilog.info(domain, tag, '%{public}s', '[ttt] ability.context.area:' + context.area);
                    hilog.info(domain, tag, '%{public}s', 'onAbilityCreate end');
                },
            }, (err : BusinessError | null) : void => {
                if (err && err.code != 0 ) {
                    hilog.info(domain, tag, '%{public}s', '-----' + err.code);
                }
                hilog.info(domain, tag, '%{public}s', 'BusinessError  end');
            });
            await Utils.msSleep(2000);
            abilityDelegator.executeShellCommand("aa start -a EntryAbility -b com.acts.datashareperformancetest.static");
            await Utils.msSleep(2000);
 
            try {
                hilog.info(domain, tag, '%{public}s', '[ttt] [DataShareClientTest] <<Consumer>> connectDataShareExtAbility begin');
                hilog.info(domain, tag, '%{public}s', '[ttt] <<Consumer>> context.databaseDir:' + context.databaseDir);
                hilog.info(domain, tag, '%{public}s', '[ttt] <<Consumer>> context.area:' + context.area);
                context.area = contextConstant.AreaMode.EL2;
                hilog.info(domain, tag, '%{public}s', '[ttt] [DataShareTest] <<Consumer>> after set context.area:' + context.area);
                dataShareHelper = await dataShare.createDataShareHelper(context, uri, {isProxy : true});
                expect(dataShareHelper != undefined).assertTrue();
                dsHelper = dataShareHelper as dataShare.DataShareHelper;
            } catch (err: BusinessError) {
                hilog.error(domain, tag, '%{public}s', `[ttt] beforeAll failed, error code is ${err.code}, message is ${err.message}`);
                expect(false).assertTrue();
            }
            hilog.info(domain, tag, '%{public}s', 'beforeAll end');
        });
 
        beforeEach(async () => {
            hilog.info(domain, tag, '%{public}s', '[ttt] beforeEach');
            if (dataShareHelper != undefined) {
                try {
                    for (let i = 0; i < BASE_COUNT + 1; i++) {
                        let vb: ValuesBucket = {
                            "name": "rose",
                            "age": i as long,
                            "phoneNumber": 2147483647 as long,
                        };
                        await dsHelper.insert(uri, vb);
                    }
                } catch (err: BusinessError) {
                    hilog.error(domain, tag, '%{public}s', `[ttt] beforeEach failed, error code is ${err.code}, message is ${err.message}`);
                }
            } else {
                hilog.info(domain, tag, '%{public}s', '[ttt] beforeEach failed, dataShareHelper is nullptr');
            }
        });
 
        afterEach(async () => {
            hilog.info(domain, tag, '%{public}s', '[ttt] afterEach');
            if (dataShareHelper != undefined) {
                try {
                    let predicates = new dataSharePredicates.DataSharePredicates();
                    let res = await dsHelper.delete(uri, predicates);
                    hilog.info(domain, tag, '%{public}s', '[ttt] afterEach delete success: ' + res);
                } catch (err: BusinessError) {
                    hilog.error(domain, tag, '%{public}s', `[ttt] afterEach failed, error code is ${err.code},message is ${err.message}`);
                }
            } else {
                hilog.info(domain, tag, '%{public}s', '[ttt] afterEach failed, dataShareHelper is nullptr');
            }
        });
 
        afterAll(async () => {
            hilog.info(domain, tag, '%{public}s', '[ttt] afterAll');
            performance.save('apiPerformance_DataShare.txt');
            if (dataShareHelper != undefined) {
                try {
                    await dsHelper.close();
                } catch (err: BusinessError) {
                    hilog.error(domain, tag, '%{public}s', `[ttt] afterAll failed, error code is ${err.code},message is ${err.message}`);
                }
            } else {
                hilog.info(domain, tag, '%{public}s', '[ttt] afterAll failed, dataShareHelper is nullptr');
            }
        });
 
        /**
         * @tc.name testDataShareResultGoToNextRow
         * @tc.number SUB_DistributedData_DFX_Performance_SDK_HighFrequency_DatashareJsApiTest_1000
         * @tc.desc RdbStoreResultSet GetAssert API Test
         * @tc.size MediumTest
         * @tc.type PERFORMANCE
         * @tc.level Level 3
         */
        it('testDataShareResultGoToNextRow', TestType.PERFORMANCE | Size.MEDIUMTEST | Level.LEVEL3, async (done: () => void): Promise<void> => {
            try{
                let predicates = new dataSharePredicates.DataSharePredicates();
                predicates.equalTo("name", "rose");
                let columns = ["*"];
                if (dataShareHelper != undefined) {
                    let resultSet:DataShareResultSet = await dsHelper.query(uri, predicates, columns);
                    resultSet.goToFirstRow();
                    let totalTime = 0.0;
                    let startTime = systemDateTime.getTime(true); // 纳秒值
                    for (let index = 0; index < BASE_COUNT; index++) {
                        resultSet.goToNextRow();
                    }
                    let endTime = systemDateTime.getTime(true)
                    totalTime += (endTime - startTime) / 1000 // 总时长转换为微秒值
                    let averageTime: double = totalTime / BASE_COUNT; // 计算接口平均时长
                    hilog.info(domain, tag, '%{public}s', 'testDataShareResultGoToNextRow averageTime: ' + averageTime.toFixed(1) + 'μs');
                    hilog.info(domain, tag, '%{public}s', 'testDataShareResultGetColumnIndex totalTime: ' + totalTime + 'μs');
                    performance.apiWithClassAndKeyword('ohos.data.DataShareResultSet', 'DataShareResultSet', 'goToNextRow',
                        'goToNextRow(): boolean', averageTime);
                    expect(averageTime <= BASELINE_GOTONEXTROW).assertTrue()
                    resultSet.close();
                } else {
                    hilog.error(domain, tag, '%{public}s', '[ttt] testDataShareResultGoToNextRow failed, dataShareHelper is nullptr');
                    expect(false).assertTrue();
                }
                done();
            } catch (err: BusinessError) {
                hilog.error(domain, tag, '%{public}s', `[ttt] testDataShareResultGoToNextRow failed, error code is ${err.code}, message is ${err.message}`);
                expect(false).assertTrue();
                done();
            }
        });
 
        /**
         * @tc.name testDataShareResultGetString
         * @tc.number SUB_DistributedData_DFX_Performance_SDK_HighFrequency_DatashareJsApiTest_0900
         * @tc.desc RdbStoreResultSet GetAssert API Test
         * @tc.size MediumTest
         * @tc.type PERFORMANCE
         * @tc.level Level 3
         */
        it('testDataShareResultGetString', TestType.PERFORMANCE | Size.MEDIUMTEST | Level.LEVEL3, async (done: () => void): Promise<void> => {
            try{
                let predicates = new dataSharePredicates.DataSharePredicates();
                predicates.equalTo("name", "rose");
                let columns = ["*"];
                if (dataShareHelper != undefined) {
                    let resultSet:DataShareResultSet = await dsHelper.query(uri, predicates, columns);
                    resultSet.goToFirstRow();
 
                    let totalTime = 0.0;
                    for (let index = 0; index < BASE_COUNT; index++) {
                        let ColumnIndex:int = resultSet.getColumnIndex("name");
 
                        let startTime = systemDateTime.getTime(true); // 纳秒值
                        resultSet.getString(ColumnIndex);
                        let endTime = systemDateTime.getTime(true)
                        totalTime += (endTime - startTime) / 1000; // 总时长转换为微秒值
                    }
                    let averageTime: double = totalTime / BASE_COUNT // 计算接口平均时长
                    hilog.info(domain, tag, '%{public}s', 'testDataShareResultGetString averageTime: ' + averageTime.toFixed(1) + 'μs');
                    hilog.info(domain, tag, '%{public}s', 'testDataShareResultGetString totalTime: ' + totalTime + 'μs');
                    performance.apiWithClassAndKeyword('ohos.data.DataShareResultSet', 'DataShareResultSet', 'getString',
                        'getString(columnIndex: number): string', averageTime);
                    expect(averageTime <= BASELINE_GETSTRING).assertTrue()
                    resultSet.close();
                } else {
                    hilog.error(domain, tag, '%{public}s', '[ttt] testDataShareResultGetString failed, dataShareHelper is nullptr');
                    expect(false).assertTrue();
                }
                done();
            } catch (err: BusinessError) {
                hilog.error(domain, tag, '%{public}s', `testDataShareResultGetString failed, error code is ${err.code},message is ${err.message}`);
                expect(false).assertTrue();
                done();
            }
        });
 
        /**
         * @tc.name testDataShareResultGetLong
         * @tc.number SUB_DistributedData_DFX_Performance_SDK_HighFrequency_DatashareJsApiTest_0800
         * @tc.desc RdbStoreResultSet GetAssert API Test
         * @tc.size MediumTest
         * @tc.type PERFORMANCE
         * @tc.level Level 3
         */
        it('testDataShareResultGetLong', TestType.PERFORMANCE | Size.MEDIUMTEST | Level.LEVEL3, async (done: () => void): Promise<void> => {
            try{
                let predicates = new dataSharePredicates.DataSharePredicates();
                predicates.equalTo("name", "rose");
                let columns = ["*"];
                if (dataShareHelper != undefined) {
                    let resultSet:DataShareResultSet = await dsHelper.query(uri, predicates, columns);
                    resultSet.goToFirstRow();
                    let totalTime = 0.0;
                    for (let index = 0; index < BASE_COUNT; index++) {
                        let ColumnIndex: int = resultSet.getColumnIndex("age");
 
                        let startTime = systemDateTime.getTime(true); // 纳秒值
                        resultSet.getLong(ColumnIndex);
                        let endTime = systemDateTime.getTime(true);
                        totalTime += (endTime - startTime) / 1000; // 总时长转换为微秒值
                    }
                    let averageTime: double = totalTime / BASE_COUNT; // 计算接口平均时长
                    hilog.info(domain, tag, '%{public}s', 'testDataShareResultGetLong averageTime: ' + averageTime.toFixed(1) + 'μs');
                    hilog.info(domain, tag, '%{public}s', 'testDataShareResultGetLong totalTime: ' + totalTime + 'μs');
                    performance.apiWithClassAndKeyword('ohos.data.DataShareResultSet', 'DataShareResultSet', 'getLong',
                        'getLong(columnIndex: number): number', averageTime);
                    expect(averageTime <= BASELINE_GETLONG).assertTrue()
                    resultSet.close();
                } else {
                    hilog.error(domain, tag, '%{public}s', '[ttt] testDataShareResultGetLong failed, dataShareHelper is nullptr');
                    expect(false).assertTrue();
                }
                done();
            } catch (err: BusinessError) {
                hilog.error(domain, tag, '%{public}s', `testDataShareResultGetLong failed, error code is ${err.code},message is ${err.message}`);
                expect(false).assertTrue();
                done();
            }
        });
 
        /**
         * @tc.name testDataShareResultGetColumnIndex
         * @tc.number SUB_DistributedData_DFX_Performance_SDK_HighFrequency_DatashareJsApiTest_0600
         * @tc.desc RdbStoreResultSet GetAssert API Test
         * @tc.size MediumTest
         * @tc.type PERFORMANCE
         * @tc.level Level 3
         */
        it('testDataShareResultGetColumnIndex', TestType.PERFORMANCE | Size.MEDIUMTEST | Level.LEVEL3, async (done: () => void): Promise<void> => {
            try{
                let predicates = new dataSharePredicates.DataSharePredicates();
                predicates.equalTo("name", "rose");
                let columns = ["*"];
                if (dataShareHelper != undefined) {
                    let resultSet:DataShareResultSet = await dsHelper.query(uri, predicates, columns);
                    resultSet.goToFirstRow();
                    let totalTime = 0.0;
 
                    let startTime = systemDateTime.getTime(true); // 纳秒值
                    for (let index = 0; index < BASE_COUNT; index++) {
                        resultSet.getColumnIndex("age");
                    }
                    let endTime = systemDateTime.getTime(true);
                    totalTime += (endTime - startTime) / 1000; // 总时长转换为微秒值
                    let averageTime: double = totalTime / BASE_COUNT; // 计算接口平均时长
                    hilog.info(domain, tag, '%{public}s', 'testDataShareResultGetColumnIndex averageTime: ' + averageTime.toFixed(1) + 'μs');
                    hilog.info(domain, tag, '%{public}s', 'testDataShareResultGetColumnIndex totalTime: ' + totalTime + 'μs');
                    performance.apiWithClassAndKeyword('ohos.data.DataShareResultSet', 'DataShareResultSet', 'getColumnIndex',
                        'getColumnIndex(columnName: string): number', averageTime);
                    expect(averageTime <= BASELINE_GETCOLUMNINDEX).assertTrue()
                    resultSet.close();
                } else {
                    hilog.error(domain, tag, '%{public}s', '[ttt] testDataShareResultGetColumnIndex failed, dataShareHelper is nullptr');
                    expect(false).assertTrue();
                }
                done();
            } catch (err: BusinessError) {
                hilog.error(domain, tag, '%{public}s', `[ttt] testDataShareResultGetColumnIndex failed, error code is ${err.code},message is ${err.message}`);
                expect(false).assertTrue();
                done();
            }
        });
    })
}