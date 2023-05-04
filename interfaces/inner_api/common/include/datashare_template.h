/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef DATASHARE_TEMPLATE_H
#define DATASHARE_TEMPLATE_H

#include <string>
#include <variant>
#include <vector>
#include "hilog/log.h"
#include "iremote_object.h"

namespace OHOS {
namespace DataShare {
struct PredicateTemplateNode {
    PredicateTemplateNode() = default;
    PredicateTemplateNode(const std::string &key, const std::string &selectSql) : key_(key), selectSql_(selectSql) {}
    std::string key_;
    std::string selectSql_;
};

struct Template {
    Template() = default;
    Template(const std::vector<PredicateTemplateNode> &predicates,
        const std::string &scheduler) : predicates_(predicates), scheduler_(scheduler) {}
    std::vector<PredicateTemplateNode> predicates_;
    std::string scheduler_;
};

struct TemplateId {
    int64_t subscriberId_;
    std::string bundleName_;
    bool operator==(const TemplateId &tplId) const
    {
        return subscriberId_ == tplId.subscriberId_ && bundleName_ == tplId.bundleName_;
    }
    bool operator!=(const TemplateId &tplId) const
    {
        return !(tplId == *this);
    }
    bool operator<(const TemplateId &tplId) const
    {
        if (subscriberId_ != tplId.subscriberId_) {
            return subscriberId_ < tplId.subscriberId_;
        }
        return bundleName_ < tplId.bundleName_;
    }
};

struct CreateOptions {
    bool isProxy_ = true;
    sptr<IRemoteObject> token_;
    bool enabled_ = false;
};

struct PublishedDataItem {
    std::string key_;
    int64_t subscriberId_;
    std::variant<sptr<Ashmem>, std::string> value_;
    PublishedDataItem() {}
    PublishedDataItem(const PublishedDataItem & item) : PublishedDataItem(item.key_, item.subscriberId_, item.value_)
    {
    }
    virtual ~PublishedDataItem()
    {
        Close();
    }
    PublishedDataItem(
        const std::string &key, int64_t subscriberId, const std::variant<sptr<Ashmem>, std::string> &value)
        : key_(key), subscriberId_(subscriberId), value_(value)
    {
    }
private:
    void Close() {
        if(value_.index() == 0) {
            sptr<Ashmem> ashmem = std::get<sptr<Ashmem>>(value_);
			static const OHOS::HiviewDFX::HiLogLabel DATASHARE_LABEL = { LOG_CORE, 0xD001651, "DataShare" };
            (void)OHOS::HiviewDFX::HiLog::Error(DATASHARE_LABEL,
                "try close %{public}s, %{public}d", key_.c_str(), ashmem->GetSptrRefCount());
            if (ashmem != nullptr && ashmem->GetSptrRefCount() == 2) {
                (void)OHOS::HiviewDFX::HiLog::Error(DATASHARE_LABEL,
                                                    "close %{public}s, %{public}p", key_.c_str(), ashmem.GetRefPtr());
                ashmem->UnmapAshmem();
                ashmem->CloseAshmem();
            }
        }
    }
};

struct Data {
    std::vector<PublishedDataItem> datas_;
    int version_ = 0;
};

struct PublishedDataChangeNode {
    std::string ownerBundleName_;
    std::vector<PublishedDataItem> datas_;
};

struct RdbChangeNode {
    std::string uri_;
    TemplateId templateId_;
    std::vector<std::string> data_;
};

struct OperationResult {
    OperationResult() = default;
    OperationResult(const std::string &key, int errCode) : key_(key), errCode_(errCode) {}
    std::string key_;
    int errCode_;
};
}  // namespace DataShare
}  // namespace OHOS
#endif //DATASHARE_TEMPLATE_H
