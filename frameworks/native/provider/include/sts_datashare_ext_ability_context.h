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

#ifndef STS_DATASHARE_EXT_ABILITY_CONTEXT_H
#define STS_DATASHARE_EXT_ABILITY_CONTEXT_H

#include <memory>

#include "ability_connect_callback.h"
#include "datashare_ext_ability_context.h"
#include "ohos_application.h"
#include "ani.h"

namespace OHOS {
namespace DataShare {
using namespace AbilityRuntime;
ani_object CreateStsDataShareExtAbilityContext(ani_env *env, std::shared_ptr<DataShareExtAbilityContext> context,
    const std::shared_ptr<AppExecFwk::OHOSApplication> &application);
} // namespace DataShare
} // namespace OHOS
#endif // STS_DATASHARE_EXT_ABILITY_CONTEXT_H