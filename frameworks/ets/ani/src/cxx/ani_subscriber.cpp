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

#include "wrapper.rs.h"
#include "ani_subscriber.h"
#include "datashare_log.h"

namespace OHOS {
using namespace DataShare;
namespace DataShareAni {
bool AniObserver::operator==(const AniObserver &rhs) const
{
    return callbackPtr_ == rhs.callbackPtr_;
}

bool AniObserver::operator!=(const AniObserver &rhs) const
{
    return !(rhs == *this);
}

void AniRdbObserver::OnChange(const RdbChangeNode &changeNode)
{
    LOG_DEBUG("AniRdbObserver onchange Start");
    if (callbackPtr_ == 0) {
        LOG_ERROR("callbackPtr_ is nullptr");
        return;
    }

    rust::Box<DataShareAni::RdbDataChangeNode> node = rust_create_rdb_data_change_node(rust::String(changeNode.uri_),
        rust::String(std::to_string(changeNode.templateId_.subscriberId_)),
        rust::String(changeNode.templateId_.bundleName_));
    if (!changeNode.isSharedMemory_) {
        for (const auto &data : changeNode.data_) {
            rdb_data_change_node_push_data(*node, rust::String(data));
        }
    }
    execute_callback_rdb_data_change(envPtr_, callbackPtr_, *node);
}

void AniPublishedObserver::OnChange(DataShare::PublishedDataChangeNode &changeNode)
{
    LOG_DEBUG("AniPublishedObserver onchange Start");
    if (callbackPtr_ == 0) {
        LOG_ERROR("callbackPtr_ is nullptr");
        return;
    }

    rust::Box<PublishedDataChangeNode> node = rust_create_published_data_change_node(
        rust::String(changeNode.ownerBundleName_));
    for (const auto &data : changeNode.datas_) {
        DataShare::PublishedDataItem::DataType dataItem = data.GetData();
        if (dataItem.index() == 0) {
            std::vector std_vec = std::get<std::vector<uint8_t>>(dataItem);
            rust::Slice<const uint8_t> slice(std_vec.data(), std_vec.size());
            published_data_change_node_push_item_arraybuffer(*node, rust::String(data.key_),
                slice, rust::String(std::to_string(data.subscriberId_)));
        } else {
            std::string strData = std::get<std::string>(dataItem);
            published_data_change_node_push_item_str(*node, rust::String(data.key_),
                rust::String(strData), rust::String(std::to_string(data.subscriberId_)));
        }
    }
    execute_callback_published_data_change(envPtr_, callbackPtr_, *node);
}
} // namespace DataShareAni
} // namespace OHOS