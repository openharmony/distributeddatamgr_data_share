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
#include "ani_inner_observer.h"

#include <chrono>
#include <cinttypes>
namespace OHOS {
using namespace DataShare;
namespace DataShareAni {
using namespace std::chrono;
ANIInnerDataShareObserver::ANIInnerDataShareObserver(
    rust::Box<DataShareCallback> &&callback) : callback_(std::move(callback))
{
}

bool ANIInnerDataShareObserver::operator==(const ANIInnerDataShareObserver &rhs) const
{
    return callback_is_equal(*callback_, *(rhs.callback_));
}

using Value = std::variant<std::monostate, int64_t, double, std::string, bool, std::vector<uint8_t>>;
void PushBucket(const std::map<std::string, Value> &bucket, rust::Box<DataShareAni::ChangeInfo> &change_info)
{
    for (const auto &[key, value] : bucket) {
        switch (value.index()) {
            case DataShareValueObjectType::TYPE_NULL: {
                change_info_push_kv_null(*change_info, rust::String(key), false);
                break;
            }
            case DataShareValueObjectType::TYPE_INT: {
                int64_t data = std::get<int64_t>(value);
                change_info_push_kv_f64(*change_info, rust::String(key), data, false);
                break;
            }
            case DataShareValueObjectType::TYPE_DOUBLE: {
                double data = std::get<double>(value);
                change_info_push_kv_f64(*change_info, rust::String(key), data, false);
                break;
            }
            case DataShareValueObjectType::TYPE_STRING: {
                std::string data = std::get<std::string>(value);
                change_info_push_kv_str(*change_info, rust::String(key), rust::String(data), false);
                break;
            }
            case DataShareValueObjectType::TYPE_BOOL: {
                bool data = std::get<bool>(value);
                change_info_push_kv_boolean(*change_info, rust::String(key), data, true);
                break;
            }
            case DataShareValueObjectType::TYPE_BLOB: {
                std::vector<uint8_t> data = std::get<std::vector<uint8_t>>(value);
                rust::Vec<uint8_t> vec;
                for (auto val: data) {
                    vec.push_back(val);
                }
                change_info_push_kv_uint8array(*change_info, rust::String(key), vec, false);
                break;
            }
            default: {
                LOG_ERROR("ChangeInfo's valueType is wrong");
            }
        }
    }
}

rust::Box<DataShareAni::ChangeInfo> ConvertChangeInfo(const std::string uri,
    const DataShareObserver::ChangeInfo changeInfo)
{
    rust::Box<DataShareAni::ChangeInfo> change_info = rust_create_change_info(changeInfo.changeType_,
        rust::String(uri));
    for (const auto &bucket : changeInfo.valueBuckets_) {
        PushBucket(bucket, change_info);
    }
    return change_info;
}

void ANIInnerDataShareObserver::OnChange(const DataShareObserver::ChangeInfo& changeInfo, bool isNotifyDetails)
{
    auto time =
        static_cast<uint64_t>(duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count());
    LOG_INFO("ANIInnerDataShareObserver datashare changeInfo callback start, times %{public}" PRIu64 ".", time);

    std::string uri = changeInfo.uris_.front().ToString();
    rust::Box<DataShareAni::ChangeInfo> change_info = ConvertChangeInfo(uri, changeInfo);
    callback_->execute_callback_changeinfo(*change_info);

    LOG_INFO("ANIInnerDataShareObserver datashare changeInfo callback end, times %{public}" PRIu64 ".", time);
}

void ANIInnerDataShareObserver::OnChange()
{
    auto time =
        static_cast<uint64_t>(duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count());
    LOG_INFO("ANIInnerDataShareObserver datashare dataChange callback start, times %{public}" PRIu64 ".", time);

    callback_->execute_callback();

    LOG_INFO("ANIInnerDataShareObserver datashare dataChange callback end, times %{public}" PRIu64 ".", time);
}

rust::Box<DataShareCallback> &ANIInnerDataShareObserver::GetCallback()
{
    return callback_;
}
}  // namespace DataShareAni
}  // namespace OHOS