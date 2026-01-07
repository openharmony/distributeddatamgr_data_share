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

#define LOG_TAG "ANISubscriber"

#include "wrapper.rs.h"
#include "ani_subscriber.h"
#include "datashare_log.h"
#include <chrono>
#include <cinttypes>

namespace OHOS {
using namespace DataShare;
using namespace std::chrono;
namespace DataShareAni {
bool AniObserver::operator==(const AniObserver &rhs) const
{
    return callback_is_equal(*callback_, *(rhs.callback_));
}

bool AniObserver::operator!=(const AniObserver &rhs) const
{
    return !(rhs == *this);
}

void AniRdbObserver::OnChange(const RdbChangeNode &changeNode)
{
    LOG_DEBUG("AniRdbObserver onchange Start");

    rust::Box<DataShareAni::RdbDataChangeNode> node = rust_create_rdb_data_change_node(rust::String(changeNode.uri_),
        rust::String(std::to_string(changeNode.templateId_.subscriberId_)),
        rust::String(changeNode.templateId_.bundleName_));
    if (!changeNode.isSharedMemory_) {
        for (const auto &data : changeNode.data_) {
            rdb_data_change_node_push_data(*node, rust::String(data));
        }
    }
    callback_->execute_callback_rdb_data_change(*node);
}

void AniPublishedObserver::OnChange(DataShare::PublishedDataChangeNode &changeNode)
{
    LOG_DEBUG("AniPublishedObserver onchange Start");

    rust::Box<PublishedDataChangeNode> node = rust_create_published_data_change_node(
        rust::String(changeNode.ownerBundleName_));
    for (const auto &data : changeNode.datas_) {
        DataShare::PublishedDataItem::DataType dataItem = data.GetData();
        if (dataItem.index() == 0) {
            std::vector std_vec = std::get<std::vector<uint8_t>>(dataItem);
            rust::Vec<uint8_t> vec;
            for (auto data: std_vec) {
                vec.push_back(data);
            }
            published_data_change_node_push_item_arraybuffer(*node, rust::String(data.key_),
                vec, rust::String(std::to_string(data.subscriberId_)));
        } else {
            std::string strData = std::get<std::string>(dataItem);
            published_data_change_node_push_item_str(*node, rust::String(data.key_),
                rust::String(strData), rust::String(std::to_string(data.subscriberId_)));
        }
    }
    callback_->execute_callback_published_data_change(*node);
}

void AniProxyDataObserver::OnChange(const std::vector<DataShare::DataProxyChangeInfo> &changeNode)
{
    auto time =
        static_cast<uint64_t>(duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count());
    LOG_INFO("call proxyData obs, t %{public}" PRIu64 ", s %{public}zu", time, changeNode.size());
    rust::Vec<DataProxyChangeInfo> node = rust_create_proxy_data_change_info();
    for (const auto &result : changeNode) {
        if (std::holds_alternative<int64_t>(result.value_)) {
            data_proxy_change_info_push_i64(
                node, (int32_t)result.changeType_, result.uri_, std::get<int64_t>(result.value_));
        } else if (std::holds_alternative<double>(result.value_)) {
            data_proxy_change_info_push_f64(
                node, (int32_t)result.changeType_, result.uri_, std::get<double>(result.value_));
        } else if (std::holds_alternative<bool>(result.value_)) {
            data_proxy_change_info_push_bool(
                node, (int32_t)result.changeType_, result.uri_, std::get<bool>(result.value_));
        } else if (std::holds_alternative<std::string>(result.value_)) {
            data_proxy_change_info_push_string(
                node, (int32_t)result.changeType_, result.uri_, std::get<std::string>(result.value_));
        }
    }

    callback_->execute_callback_proxy_data_change_info(node);
    LOG_INFO("callback end");
}
} // namespace DataShareAni
} // namespace OHOS
