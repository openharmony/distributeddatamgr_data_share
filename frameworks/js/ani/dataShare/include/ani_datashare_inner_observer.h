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
    ANIInnerObserver(ani_vm *vm, ani_ref callback);
    virtual ~ANIInnerObserver();
    void OnChange(const DataShareObserver::ChangeInfo &changeInfo = {}, bool isNotifyDetails = false);
    ani_ref GetCallback();

private:
    ani_object GetNewChangeInfo(ani_env *env);
    ani_enum_item GetEnumItem(ani_env *env, int32_t type);
    ani_object Convert2TSValue(ani_env *env, const std::monostate &value = {});
    ani_object Convert2TSValue(ani_env *env, int64_t value);
    ani_object Convert2TSValue(ani_env *env, double value);
    ani_object Convert2TSValue(ani_env *env, bool value);
    ani_object Convert2TSValue(ani_env *env, const std::string &value);
    ani_object Convert2TSValue(ani_env *env, const std::vector<uint8_t> &values);
    template<class... Types>
    ani_object Convert2TSValue(ani_env *env, const std::variant<Types...> &value);
    ani_object Convert2TSValue(ani_env *env, const DataShareValuesBucket &valueBucket);
    template<typename T>
    ani_object Convert2TSValue(ani_env *env, const std::vector<T> &values);
    ani_object Convert2TSValue(ani_env *env, const DataShareObserver::ChangeInfo& changeInfo);
    template<typename _VTp>
    ani_object ReadVariant(ani_env *env, size_t step, size_t index, const _VTp &value);
    template<typename _VTp, typename _First, typename ..._Rest>
    ani_object ReadVariant(ani_env *env, size_t step, size_t index, const _VTp &value);

private:
    ani_vm *vm_;
    ani_ref callback_;
};
}  // namespace DataShare
}  // namespace OHOS
#endif //ANI_DATASHARE_INNER_OBSERVER_H