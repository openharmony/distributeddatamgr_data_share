/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef FFI_DS_BUNDLEMGR_BRIDGE_H
#define FFI_DS_BUNDLEMGR_BRIDGE_H

#include "cxx.h"
#include <cstdint>
#include <memory>

namespace OHOS::DataShare {

struct BundleConfigCpp;

std::shared_ptr<BundleConfigCpp> get_bundle_info(
    rust::Str bundle_name, int32_t user_id, int32_t app_index);

int32_t bundle_config_result_code(const BundleConfigCpp &config);
rust::String bundle_config_app_identifier(const BundleConfigCpp &config);
rust::String bundle_config_name(const BundleConfigCpp &config);
bool bundle_config_singleton(const BundleConfigCpp &config);
bool bundle_config_is_system_app(const BundleConfigCpp &config);

int32_t bundle_config_proxy_count(const BundleConfigCpp &config);
rust::String bundle_config_proxy_uri(const BundleConfigCpp &config, int32_t index);
rust::String bundle_config_proxy_read_perm(const BundleConfigCpp &config, int32_t index);
rust::String bundle_config_proxy_write_perm(const BundleConfigCpp &config, int32_t index);
rust::String bundle_config_proxy_module_name(const BundleConfigCpp &config, int32_t index);
int32_t bundle_config_proxy_profile_result_code(const BundleConfigCpp &config, int32_t index);
rust::String bundle_config_proxy_store_name(const BundleConfigCpp &config, int32_t index);
rust::String bundle_config_proxy_table_name(const BundleConfigCpp &config, int32_t index);
rust::String bundle_config_proxy_type(const BundleConfigCpp &config, int32_t index);
rust::String bundle_config_proxy_scope(const BundleConfigCpp &config, int32_t index);

int32_t bundle_config_extension_count(const BundleConfigCpp &config);
int32_t bundle_config_extension_type(const BundleConfigCpp &config, int32_t index);
rust::String bundle_config_extension_read_perm(const BundleConfigCpp &config, int32_t index);
rust::String bundle_config_extension_write_perm(const BundleConfigCpp &config, int32_t index);
rust::String bundle_config_extension_uri(const BundleConfigCpp &config, int32_t index);
bool bundle_config_extension_is_silent_enabled(const BundleConfigCpp &config, int32_t index);

rust::String bundle_config_proxy_backup(const BundleConfigCpp &config, int32_t index);
rust::String bundle_config_proxy_ext_uri(const BundleConfigCpp &config, int32_t index);
bool bundle_config_proxy_store_meta_data_from_uri(const BundleConfigCpp &config, int32_t index);

int32_t bundle_config_proxy_allow_list_count(const BundleConfigCpp &config, int32_t proxy_idx);
rust::String bundle_config_proxy_allow_list_app_id(
    const BundleConfigCpp &config, int32_t proxy_idx, int32_t list_idx);
bool bundle_config_proxy_allow_list_only_main(
    const BundleConfigCpp &config, int32_t proxy_idx, int32_t list_idx);

int32_t bundle_config_proxy_table_config_count(const BundleConfigCpp &config, int32_t proxy_idx);
rust::String bundle_config_proxy_table_config_uri(
    const BundleConfigCpp &config, int32_t proxy_idx, int32_t cfg_idx);
int32_t bundle_config_proxy_table_config_cross_mode(
    const BundleConfigCpp &config, int32_t proxy_idx, int32_t cfg_idx);

int32_t bundle_config_extension_profile_result_code(const BundleConfigCpp &config, int32_t index);
int32_t bundle_config_extension_table_config_count(const BundleConfigCpp &config, int32_t ext_idx);
rust::String bundle_config_extension_table_config_uri(
    const BundleConfigCpp &config, int32_t ext_idx, int32_t cfg_idx);
int32_t bundle_config_extension_table_config_cross_mode(
    const BundleConfigCpp &config, int32_t ext_idx, int32_t cfg_idx);

struct BmsAllowListEntry;
struct BmsTableConfigEntry;
struct BmsProxyEntry;
BmsProxyEntry bundle_config_get_proxy_entry(const BundleConfigCpp &config, int32_t index);

} // namespace OHOS::DataShare

#endif // FFI_DS_BUNDLEMGR_BRIDGE_H
