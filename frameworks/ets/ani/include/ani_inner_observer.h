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

#ifndef ANI_DATASHARE_INNER_OBSERVER_H
#define ANI_DATASHARE_INNER_OBSERVER_H

#include <ani.h>
#include "cxx.h"
#include "datashare_log.h"
#include "datashare_helper.h"
#include "datashare_observer.h"
namespace OHOS {
using namespace DataShare;
namespace DataShareAni {
struct DataShareCallback;
class ANIInnerDataShareObserver : public std::enable_shared_from_this<ANIInnerDataShareObserver> {
public:
    explicit  ANIInnerDataShareObserver(rust::Box<DataShareCallback> &&callback);
    bool operator==(const ANIInnerDataShareObserver &rhs) const;
    void OnChange(const DataShareObserver::ChangeInfo &changeInfo, bool isNotifyDetails);
    void OnChange();
    rust::Box<DataShareCallback> &GetCallback();
private:
    rust::Box<DataShareCallback> callback_;

    ANIInnerDataShareObserver(const ANIInnerDataShareObserver&) = delete;
    ANIInnerDataShareObserver& operator=(const ANIInnerDataShareObserver&) = delete;
};
}  // namespace DataShareAni
}  // namespace OHOS
#endif //ANI_DATASHARE_INNER_OBSERVER_H
