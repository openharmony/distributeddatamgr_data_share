/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef DATASHARE_SHARED_RESULT_SET_H
#define DATASHARE_SHARED_RESULT_SET_H

#include <string>

namespace OHOS {
namespace AppDataFwk {
class SharedBlock;
}
namespace DataShare {
class DataShareSharedResultSet {
public:
    DataShareSharedResultSet() {}
    virtual ~DataShareSharedResultSet() {}
    /**
     * Obtains a block from the {@link SharedResultSet}
     */
    virtual std::shared_ptr<AppDataFwk::SharedBlock> GetBlock() = 0;
    /**
     * Adds the data of a {@code SharedResultSet} to a {@link SharedBlock}
     */
    virtual void FillBlock(int startRowIndex, AppDataFwk::SharedBlock *block) = 0;
    /**
     * Called when the position of the result set changes
     */
    virtual bool OnGo(int oldRowIndex, int newRowIndex, int *cachedIndex) = 0;
};
} // namespace DataShare
} // namespace OHOS

#endif