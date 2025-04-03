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

#include "datashare_log.h"
#include "datashare_helper.h"

namespace OHOS {
namespace DataShare {
class ANIInnerObserver : public std::enable_shared_from_this<ANIInnerObserver> {
public:
    ANIInnerObserver(ani_env *env, ani_class cls, ani_method callback);
    void OnChange(const DataShareObserver::ChangeInfo &changeInfo = {}, bool isNotifyDetails = false);
    ani_method GetCallback();

private:
    ani_env *env_;
    ani_class cls_;
    ani_method callback_;
};
}  // namespace DataShare
}  // namespace OHOS
#endif //ANI_DATASHARE_INNER_OBSERVER_H