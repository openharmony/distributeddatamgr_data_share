/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#include "ani_rs_bind.h"
#include "wrapper.rs.h"
#include <memory>
#include <thread>

uint32_t AniSendEvent(rust::box<RustClosure> closure, const rust::str name)
{
    std::shared_ptr<OHOS::AppExecFwk::EventRunner> runner = OHOS::AppExecFwk::EventRunner::GetMainEventRunner();
    if (!runner) {
        return ani_status::ANI_NOT_FOUND;
    }
    auto handler = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner);
    auto closureWrapper = std::make_shared<rust::Box<RustClosure>>(std::move(closure));

    auto callback = [closureWrapper = std::move(closureWrapper),
        main_handle = handler]() mutable { (*closureWrapper)->execute(); };
    handler->PostTask(callback, std::string(name));
    return ani_status::ANI_OK;
}