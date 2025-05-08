/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License"),
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

/**
 * @file
 * @kit ArkData
 */

import { BusinessError } from '@ohos.base'
import type { AsyncCallback, Callback } from '@ohos.base';
import ExtensionContext from 'application.ExtensionContext';
import Want from '@ohos.app.ability.Want';
import dataSharePredicates from './@ohos.data.dataSharePredicates';
import { ValuesBucket } from '@ohos.data.ValuesBucket';

export default class DataShareExtensionAbility {

  context: ExtensionContext;

    onCreate(want: Want): void {
        console.info("basic onCreate");
    }

    insert(uri: string, valueBucket: ValuesBucket): void {
        console.info("basic insert");
    }

    update(
        uri: string,
        predicates: dataSharePredicates.DataSharePredicates,
        valueBucket: ValuesBucket,
    ): void {
        console.info("basic update");
    }

    delete(uri: string, predicates: dataSharePredicates.DataSharePredicates): void {
        console.info("basic delete");
    }

    query(
        uri: string,
        predicates: dataSharePredicates.DataSharePredicates,
        columns: Array<string>
    ): void {
        console.info("basic query");
    }

    batchInsert(uri: string, valueBuckets: Array<ValuesBucket>): void {
        console.info("basic batchInsert");
    }
}

