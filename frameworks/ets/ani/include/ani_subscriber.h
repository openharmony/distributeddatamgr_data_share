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

#ifndef ANI_SUBSCRIBER_H
#define ANI_SUBSCRIBER_H

#include "datashare_template.h"

namespace OHOS {
using namespace DataShare;
namespace DataShareAni {
class AniObserver {
public:
    AniObserver(long long envPtr, long long callbackPtr) : envPtr_(envPtr), callbackPtr_(callbackPtr) {};
    virtual bool operator==(const AniObserver &rhs) const;
    virtual bool operator!=(const AniObserver &rhs) const;
    AniObserver& operator=(AniObserver &&rhs) = default;
protected:
    long long envPtr_ = 0;
    long long callbackPtr_ = 0;
};

class AniRdbObserver final: public AniObserver, public std::enable_shared_from_this<AniRdbObserver> {
public:
    AniRdbObserver(long long envPtr, long long callbackPtr) : AniObserver(envPtr, callbackPtr) {};
    void OnChange(const RdbChangeNode &changeNode);
};

class AniPublishedObserver final: public AniObserver, public std::enable_shared_from_this<AniPublishedObserver> {
public:
    AniPublishedObserver(long long envPtr, long long callbackPtr) : AniObserver(envPtr, callbackPtr) {};
    void OnChange(DataShare::PublishedDataChangeNode &changeNode);
};
} // namespace DataShareAni
} // namespace OHOS
#endif // ANI_SUBSCRIBER_H