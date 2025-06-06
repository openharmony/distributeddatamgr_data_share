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

#ifndef DATASHARE_PREDICATES_CLEANER_H
#define DATASHARE_PREDICATES_CLEANER_H

#include <ani.h>

#include <memory>
#include <string>
#include "datashare_predicates.h"

class DataSharePredicatesCleaner {
    public:
    static void Clean([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object)
    {
        ani_long ptr = 0;
        if (env == nullptr) {
            return;
        }
        if (ANI_OK != env->Object_GetFieldByName_Long(object, "targetPtr", &ptr)) {
            return;
        }
        delete reinterpret_cast<OHOS::DataShare::DataSharePredicates *>(ptr);
    }

    DataSharePredicatesCleaner(ani_env *env)
        : env_(env)
    {
    }

    ani_status Bind(ani_class cls)
    {
        std::array methods = {
            ani_native_function { "clean", nullptr, reinterpret_cast<void *>(DataSharePredicatesCleaner::Clean) },
        };

        if (env_ == nullptr) {
            return static_cast<ani_status>(ANI_ERROR);
        }

        if (ANI_OK != env_->Class_BindNativeMethods(cls, methods.data(), methods.size())) {
            return static_cast<ani_status>(ANI_ERROR);
        };

        return ANI_OK;
    }
    
    private:
        ani_env *env_ = nullptr;
    };

#endif // DATASHARE_PREDICATES_CLEANER_H