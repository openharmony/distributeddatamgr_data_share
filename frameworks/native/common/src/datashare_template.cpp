/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "datashare_template.h"

namespace OHOS {
namespace DataShare {
PublishedDataItem::~PublishedDataItem()
{
    AshmemNode *node = std::get_if<AshmemNode>(&value_);
    if (node != nullptr) {
        if (node->isManaged && node->ashmem != nullptr) {
            node->ashmem->UnmapAshmem();
            node->ashmem->CloseAshmem();
        }
    }
}

PublishedDataItem::PublishedDataItem(
    const std::string &key, int64_t subscriberId, std::variant<std::vector<uint32_t>, std::string> value)

    : key_(key), subscriberId_(subscriberId)
{
    if (value.index() == 0) {
        std::vector<uint32_t> &vecValue = std::get<std::vector<uint32_t>>(value);
        auto size = vecValue.size() * sizeof(uint32_t);
        sptr<Ashmem> mem = Ashmem::CreateAshmem((key_ + std::to_string(subscriberId_)).c_str(), size);
        if (mem == nullptr) {
            return;
        }
        if (!mem->MapReadAndWriteAshmem()) {
            mem->CloseAshmem();
            return;
        }
        if (!mem->WriteToAshmem(vecValue.data(), size, 0)) {
            mem->UnmapAshmem();
            mem->CloseAshmem();
            return;
        }
        AshmemNode node = { mem, true };
        value_ = std::move(node);
    } else {
        value_ = std::move(std::get<std::string>(value));
    }
}

PublishedDataItem::PublishedDataItem(PublishedDataItem &&item)
{
    key_ = std::move(item.key_);
    subscriberId_ = std::move(item.subscriberId_);
    value_ = std::move(item.value_);
    if (item.IsAshmem()) {
        item.MoveOutAshmem();
    }
}

PublishedDataItem &PublishedDataItem::operator=(PublishedDataItem &&item)
{
    key_ = std::move(item.key_);
    subscriberId_ = std::move(item.subscriberId_);
    value_ = std::move(item.value_);
    if (item.IsAshmem()) {
        item.MoveOutAshmem();
    }
    return *this;
}

sptr<Ashmem> PublishedDataItem::MoveOutAshmem()
{
    if (IsAshmem()) {
        AshmemNode &node = std::get<AshmemNode>(value_);
        if (!node.isManaged) {
            return nullptr;
        }
        node.isManaged = false;
        return std::move(node.ashmem);
    }
    return nullptr;
}

bool PublishedDataItem::IsAshmem() const
{
    return value_.index() == 0;
}

bool PublishedDataItem::IsString() const
{
    return value_.index() == 1;
}

void PublishedDataItem::Set(const std::string &value)
{
    value_ = value;
}

std::variant<std::vector<uint32_t>, std::string> PublishedDataItem::GetData() const
{
    if (IsAshmem()) {
        const AshmemNode &node = std::get<AshmemNode>(value_);
        if (node.ashmem != nullptr) {
            node.ashmem->MapReadOnlyAshmem();
            uint32_t *data = (uint32_t *)node.ashmem->ReadFromAshmem(node.ashmem->GetAshmemSize(), 0);
            if (data == nullptr) {
                return std::vector<uint32_t>();
            }
            return std::vector<uint32_t>(data, data + node.ashmem->GetAshmemSize()/sizeof(uint32_t));
        }
        return std::vector<uint32_t>();
    } else {
        return std::get<std::string>(value_);
    }
}

void PublishedDataItem::SetAshmem(sptr<Ashmem> ashmem, bool isManaged)
{
    AshmemNode node = { ashmem, isManaged };
    value_ = std::move(node);
}
} // namespace DataShare
} // namespace OHOS