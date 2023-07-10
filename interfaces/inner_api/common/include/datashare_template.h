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
#include "iremote_object.h"

namespace OHOS {
namespace DataShare {
/**
 *  Specifies the predicates structure of the template.
 */
struct PredicateTemplateNode {
    PredicateTemplateNode() = default;
    PredicateTemplateNode(const std::string &key, const std::string &selectSql) : key_(key), selectSql_(selectSql) {}
    /** Specifies the key of the sql. */
    std::string key_;
    /** Specifies the sql of the template. */
    std::string selectSql_;
};

/**
 *  Specifies the template structure in subscribe.
 */
struct Template {
    Template() = default;
    Template(const std::vector<PredicateTemplateNode> &predicates,
        const std::string &scheduler) : predicates_(predicates), scheduler_(scheduler) {}
    /** Specifies the predicates of the template. {@link #PredicateTemplateNode} */
    std::vector<PredicateTemplateNode> predicates_;
    /** Specifies the scheduler sql of the template. */
    std::string scheduler_;
};

/**
 * Specifies the {@link Template} id structure, template is marked by the template id.
 */
struct TemplateId {
    /** Specifies the id of subscriber. */
    int64_t subscriberId_;
    /** Specifies the bundleName of template owner. */
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

/**
 * Manages create datashare helper options.
 */
struct CreateOptions {
    /** Specifies whether the {@link DataShareHelper} in proxy mode. */
    bool isProxy_ = true;
    /** Specifies the System token. */
    sptr<IRemoteObject> token_;
    /** Specifies whether use options to create DataShareHelper. */
    bool enabled_ = false;
};

struct AshmemNode {
    sptr<Ashmem> ashmem;
    bool isManaged;
};

/**
 * Specifies the published item structure.
 */
struct PublishedDataItem {
    using DataType = std::variant<std::vector<uint8_t>, std::string>;
    /** The key of the published data. */
    std::string key_;
    /** The subscriber id */
    int64_t subscriberId_;
    /** The published data. If the data is large, use Ashmem. Do not access, only for ipc */
    std::variant<AshmemNode, std::string> value_;
    PublishedDataItem(){};
    PublishedDataItem(const PublishedDataItem &) = delete;
    PublishedDataItem &operator=(const PublishedDataItem &) = delete;
    virtual ~PublishedDataItem();
    PublishedDataItem(const std::string &key, int64_t subscriberId, DataType value);
    PublishedDataItem(PublishedDataItem &&item);
    PublishedDataItem &operator=(PublishedDataItem &&item);
    bool IsAshmem() const;
    bool IsString() const;
    sptr<Ashmem> MoveOutAshmem();
    void SetAshmem(sptr<Ashmem> ashmem, bool isManaged = false);
    void Set(DataType &value);
    DataType GetData() const;

private:
    void Clear();
};

/** Specifies the published data structure. */
struct Data {
    /** Indicates the published data. {@link PublishedDataItem} */
    std::vector<PublishedDataItem> datas_;
    /** Indicates the version of data to publish, larger is newer. */
    int version_ = 0;
};

/**
 * Specifies the change node structure of published data in callback.
 */
struct PublishedDataChangeNode {
    /** Specifies the bundleName of the callback. */
    std::string ownerBundleName_;
    /** Specifies the datas of the callback. */
    std::vector<PublishedDataItem> datas_;
};

/**
 * Specifies the change node structure of rdb store data in callback.
 */
struct RdbChangeNode {
    /** Specifies the uri of the callback. */
    std::string uri_;
    /** Specifies the templateId of the callback. */
    TemplateId templateId_;
    /** Specifies the datas of the callback. */
    std::vector<std::string> data_;
};

/**
 * Specifies the operation result structure.
 */
struct OperationResult {
    OperationResult() = default;
    OperationResult(const std::string &key, int errCode) : key_(key), errCode_(errCode) {}
    /**  Specifies the key of the operation result. */
    std::string key_;
    /** Specifies the operation result error code. */
    int errCode_;
};
}  // namespace DataShare
}  // namespace OHOS
#endif //DATASHARE_TEMPLATE_H
