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

#include "ffi_ds_bundlemgr_bridge.h"
#include "wrapper.rs.h"

#include "bundle_info.h"
#include "bundlemgr/bundle_mgr_interface.h"
#include "hilog/log.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "nlohmann/json.hpp"
#include "resource_manager.h"
#include "system_ability_definition.h"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <unistd.h>

namespace OHOS::DataShare {

namespace {
constexpr HiviewDFX::HiLogLabel LOG_LABEL = { LOG_CORE, 0xD001651, "FfiBundleMgr" };
#define FFI_LOGE(fmt, ...) HiviewDFX::HiLog::Error(LOG_LABEL, "%{public}s: " fmt, __FUNCTION__, ##__VA_ARGS__)

constexpr const char *PROFILE_FILE_PREFIX = "$profile:";
constexpr const char *DATA_SHARE_EXTENSION_META = "ohos.extension.dataShare";
constexpr const char *DATA_SHARE_PROPERTIES_META = "dataProperties";
constexpr int PROFILE_NOT_FOUND = 1;
constexpr int PROFILE_ERROR = 2;
constexpr size_t MAX_FILE_SIZE = 10 * 1024 * 1024;
const size_t PROFILE_PREFIX_LEN = strlen(PROFILE_FILE_PREFIX);
constexpr size_t MAX_ALLOWLIST_COUNT = 256;

std::shared_ptr<Global::Resource::ResourceManager> InitResMgr(const std::string &resourcePath)
{
    std::shared_ptr<Global::Resource::ResourceManager> resMgr(
        Global::Resource::CreateResourceManager(false));
    if (resMgr == nullptr) {
        return nullptr;
    }
    std::unique_ptr<Global::Resource::ResConfig> resConfig(Global::Resource::CreateResConfig());
    if (resConfig == nullptr) {
        return nullptr;
    }
    resMgr->UpdateResConfig(*resConfig);
    resMgr->AddResource(resourcePath.c_str());
    return resMgr;
}

std::string ReadProfile(const std::string &resPath)
{
    if (resPath.empty() || access(resPath.c_str(), F_OK) != 0) {
        return "";
    }
    std::fstream in(resPath, std::ios_base::in | std::ios_base::binary);
    if (!in.is_open()) {
        return "";
    }
    std::ostringstream tmp;
    tmp << in.rdbuf();
    std::string content = tmp.str();
    if (content.empty() || content.length() > MAX_FILE_SIZE) {
        return "";
    }
    return content;
}

std::string GetResFromResMgr(
    const std::string &resName, Global::Resource::ResourceManager &resMgr,
    const std::string &hapPath)
{
    if (resName.empty()) {
        return "";
    }
    size_t pos = resName.rfind(PROFILE_FILE_PREFIX);
    if (pos == std::string::npos || pos == resName.length() - PROFILE_PREFIX_LEN) {
        return "";
    }
    std::string profileName = resName.substr(pos + PROFILE_PREFIX_LEN);

    if (!hapPath.empty()) {
        std::unique_ptr<uint8_t[]> fileContent = nullptr;
        size_t len = 0;
        auto ret = resMgr.GetProfileDataByName(profileName.c_str(), len, fileContent);
        if (ret != Global::Resource::RState::SUCCESS || fileContent == nullptr || len == 0) {
            return "";
        }
        return std::string(fileContent.get(), fileContent.get() + len);
    }

    std::string resPath;
    auto ret = resMgr.GetProfileByName(profileName.c_str(), resPath);
    if (ret != Global::Resource::RState::SUCCESS) {
        return "";
    }
    return ReadProfile(resPath);
}

std::string GetProfileJsonByMetadata(
    const std::vector<AppExecFwk::Metadata> &metadata,
    const std::string &resourcePath, const std::string &hapPath, const std::string &name)
{
    if (metadata.empty() || resourcePath.empty()) {
        return "";
    }
    auto it = std::find_if(metadata.begin(), metadata.end(),
        [&name](const AppExecFwk::Metadata &meta) { return meta.name == name; });
    if (it == metadata.end()) {
        return "";
    }
    auto resMgr = InitResMgr(resourcePath);
    if (resMgr == nullptr) {
        return "";
    }
    return GetResFromResMgr(it->resource, *resMgr, hapPath);
}

struct AllowListFlat {
    std::string appIdentifier;
    bool onlyMain = false;
};

struct TableConfigFlat {
    std::string uri;
    int32_t crossUserMode = 0;
};

struct ProfileFields {
    int resultCode = PROFILE_NOT_FOUND;
    std::string storeName;
    std::string tableName;
    std::string type = "rdb";
    std::string scope = "module";
    bool isSilentProxyEnable = true;
    std::string backup;
    std::string extUri;
    bool storeMetaDataFromUri = false;
    std::vector<AllowListFlat> allowLists;
    std::vector<TableConfigFlat> tableConfig;
};

void ParseScalarFields(const nlohmann::json &j, ProfileFields &fields)
{
    if (j.contains("isSilentProxyEnable") && j["isSilentProxyEnable"].is_boolean()) {
        fields.isSilentProxyEnable = j["isSilentProxyEnable"].get<bool>();
    }
    if (j.contains("scope") && j["scope"].is_string()) {
        fields.scope = j["scope"].get<std::string>();
    }
    if (j.contains("type") && j["type"].is_string()) {
        fields.type = j["type"].get<std::string>();
    }
    if (j.contains("path") && j["path"].is_string()) {
        std::string path = j["path"].get<std::string>();
        size_t sepPos = path.find('/');
        if (sepPos != std::string::npos && sepPos > 0 && sepPos < path.length() - 1) {
            fields.storeName = path.substr(0, sepPos);
            fields.tableName = path.substr(sepPos + 1);
        }
    }
    if (j.contains("backup") && j["backup"].is_string()) {
        fields.backup = j["backup"].get<std::string>();
    }
    if (j.contains("extUri") && j["extUri"].is_string()) {
        fields.extUri = j["extUri"].get<std::string>();
    }
    if (j.contains("storeMetaDataFromUri") && j["storeMetaDataFromUri"].is_boolean()) {
        fields.storeMetaDataFromUri = j["storeMetaDataFromUri"].get<bool>();
    }
}

std::vector<AllowListFlat> ParseAllowLists(const nlohmann::json &j)
{
    std::vector<AllowListFlat> result;
    if (!j.contains("allowLists") || !j["allowLists"].is_array()) {
        return result;
    }
    for (const auto &item : j["allowLists"]) {
        if (!item.is_object()) {
            continue;
        }
        AllowListFlat al;
        if (item.contains("onlyMain") && item["onlyMain"].is_boolean()) {
            al.onlyMain = item["onlyMain"].get<bool>();
            if (item.contains("appIdentifier") && item["appIdentifier"].is_string()) {
                al.appIdentifier = item["appIdentifier"].get<std::string>();
            }
        }
        result.push_back(std::move(al));
    }
    if (result.size() > MAX_ALLOWLIST_COUNT) {
        result.resize(MAX_ALLOWLIST_COUNT);
    }
    return result;
}

std::vector<TableConfigFlat> ParseTableConfigArray(const nlohmann::json &j)
{
    std::vector<TableConfigFlat> result;
    if (!j.contains("tableConfig") || !j["tableConfig"].is_array()) {
        return result;
    }
    for (const auto &item : j["tableConfig"]) {
        if (!item.is_object()) {
            continue;
        }
        TableConfigFlat tc;
        if (item.contains("uri") && item["uri"].is_string()) {
            tc.uri = item["uri"].get<std::string>();
        }
        if (item.contains("crossUserMode") && item["crossUserMode"].is_number_integer()) {
            tc.crossUserMode = item["crossUserMode"].get<int32_t>();
        }
        result.push_back(std::move(tc));
    }
    return result;
}

ProfileFields ParseProfileJson(const std::string &jsonStr)
{
    ProfileFields fields;
    if (jsonStr.empty()) {
        fields.resultCode = PROFILE_NOT_FOUND;
        return fields;
    }
    auto j = nlohmann::json::parse(jsonStr, nullptr, false);
    if (j.is_discarded() || !j.is_object()) {
        fields.resultCode = PROFILE_ERROR;
        return fields;
    }
    fields.resultCode = 0;
    ParseScalarFields(j, fields);
    fields.allowLists = ParseAllowLists(j);
    fields.tableConfig = ParseTableConfigArray(j);
    return fields;
}

sptr<AppExecFwk::IBundleMgr> GetBmsMgr()
{
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgr == nullptr) {
        FFI_LOGE("Failed to get SAMGR");
        return nullptr;
    }
    auto remoteObj = samgr->CheckSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (remoteObj == nullptr) {
        FFI_LOGE("BMS service not ready");
        return nullptr;
    }
    return iface_cast<AppExecFwk::IBundleMgr>(remoteObj);
}
} // anonymous namespace

struct ProxyDataFlat {
    std::string uri;
    std::string readPerm;
    std::string writePerm;
    std::string moduleName;
    int32_t profileResultCode = PROFILE_NOT_FOUND;
    std::string storeName;
    std::string tableName;
    std::string type;
    std::string scope;
    std::string backup;
    std::string extUri;
    bool storeMetaDataFromUri = false;
    std::vector<AllowListFlat> allowLists;
    std::vector<TableConfigFlat> tableConfig;
};

struct ExtensionFlat {
    int32_t type = -1;
    std::string readPerm;
    std::string writePerm;
    std::string uri;
    bool isSilentEnabled = true;
    int32_t profileResultCode = PROFILE_NOT_FOUND;
    std::vector<TableConfigFlat> tableConfig;
};

struct BundleConfigCpp {
    int32_t resultCode = 0;
    std::string appIdentifier;
    std::string name;
    bool singleton = false;
    bool isSystemApp = false;
    std::vector<ProxyDataFlat> proxyDatas;
    std::vector<ExtensionFlat> extensions;
};

bool CollectProxyDatas(
    AppExecFwk::BundleInfo &bundleInfo, std::shared_ptr<BundleConfigCpp> &result)
{
    for (auto &hap : bundleInfo.hapModuleInfos) {
        std::string resourcePath = !hap.hapPath.empty() ? hap.hapPath : hap.resourcePath;
        for (auto &proxyData : hap.proxyDatas) {
            ProxyDataFlat flat;
            flat.uri = std::move(proxyData.uri);
            flat.readPerm = std::move(proxyData.requiredReadPermission);
            flat.writePerm = std::move(proxyData.requiredWritePermission);
            flat.moduleName = hap.moduleName;

            std::string jsonStr = GetProfileJsonByMetadata(
                std::vector<AppExecFwk::Metadata>{proxyData.metadata},
                resourcePath, hap.hapPath, DATA_SHARE_PROPERTIES_META);
            auto profile = ParseProfileJson(jsonStr);
            flat.profileResultCode = profile.resultCode;
            if (profile.resultCode == PROFILE_ERROR) {
                FFI_LOGE("Profile parse error for proxy uri:%{public}s", flat.uri.c_str());
                result->resultCode = -1;
                return false;
            }
            flat.storeName = std::move(profile.storeName);
            flat.tableName = std::move(profile.tableName);
            flat.type = std::move(profile.type);
            flat.scope = std::move(profile.scope);
            flat.backup = std::move(profile.backup);
            flat.extUri = std::move(profile.extUri);
            flat.storeMetaDataFromUri = profile.storeMetaDataFromUri;
            flat.allowLists = std::move(profile.allowLists);
            flat.tableConfig = std::move(profile.tableConfig);
            result->proxyDatas.push_back(std::move(flat));
        }
    }
    return true;
}

bool CollectExtensions(
    AppExecFwk::BundleInfo &bundleInfo, std::shared_ptr<BundleConfigCpp> &result)
{
    for (auto &ext : bundleInfo.extensionInfos) {
        if (ext.type != AppExecFwk::ExtensionAbilityType::DATASHARE) {
            continue;
        }
        ExtensionFlat flat;
        flat.type = static_cast<int32_t>(ext.type);
        flat.readPerm = std::move(ext.readPermission);
        flat.writePerm = std::move(ext.writePermission);
        flat.uri = std::move(ext.uri);

        std::string resourcePath = !ext.hapPath.empty() ? ext.hapPath : ext.resourcePath;
        std::string jsonStr = GetProfileJsonByMetadata(
            ext.metadata, resourcePath, ext.hapPath, DATA_SHARE_EXTENSION_META);
        auto profile = ParseProfileJson(jsonStr);
        if (profile.resultCode == PROFILE_ERROR) {
            FFI_LOGE("Profile parse error for extension uri:%{public}s", flat.uri.c_str());
            result->resultCode = -1;
            return false;
        }
        flat.isSilentEnabled = profile.isSilentProxyEnable;
        flat.profileResultCode = profile.resultCode;
        flat.tableConfig = std::move(profile.tableConfig);
        result->extensions.push_back(std::move(flat));
    }
    return true;
}

std::shared_ptr<BundleConfigCpp> get_bundle_info(
    rust::Str bundle_name, int32_t user_id, int32_t app_index)
{
    auto bmsClient = GetBmsMgr();
    if (bmsClient == nullptr) {
        return nullptr;
    }

    AppExecFwk::BundleInfo bundleInfo;
    bool success;
    if (app_index == 0) {
        success = bmsClient->GetBundleInfo(
            std::string(bundle_name),
            AppExecFwk::BundleFlag::GET_BUNDLE_WITH_EXTENSION_INFO,
            bundleInfo, user_id);
    } else {
        auto errCode = bmsClient->GetCloneBundleInfo(std::string(bundle_name),
            static_cast<int32_t>(
                AppExecFwk::GetBundleInfoFlag::GET_BUNDLE_INFO_WITH_EXTENSION_ABILITY) |
            static_cast<int32_t>(
                AppExecFwk::GetBundleInfoFlag::GET_BUNDLE_INFO_WITH_HAP_MODULE),
            app_index, bundleInfo, user_id);
        success = (errCode == 0);
        for (auto &hap : bundleInfo.hapModuleInfos) {
            for (auto &ext : hap.extensionInfos) {
                bundleInfo.extensionInfos.push_back(ext);
            }
        }
    }

    auto result = std::make_shared<BundleConfigCpp>();
    if (!success) {
        FFI_LOGE("GetBundleInfo failed, bundle:%{public}s, userId:%{public}d",
            std::string(bundle_name).c_str(), user_id);
        result->resultCode = -1;
        return result;
    }

    result->name = std::move(bundleInfo.name);
    result->singleton = bundleInfo.singleton;
    result->isSystemApp = bundleInfo.applicationInfo.isSystemApp;
    result->appIdentifier = std::move(bundleInfo.signatureInfo.appIdentifier);

    if (!CollectProxyDatas(bundleInfo, result)) {
        return result;
    }
    if (!CollectExtensions(bundleInfo, result)) {
        return result;
    }

    return result;
}

int32_t bundle_config_result_code(const BundleConfigCpp &config)
{
    return config.resultCode;
}

rust::String bundle_config_app_identifier(const BundleConfigCpp &config)
{
    return rust::String(config.appIdentifier);
}

rust::String bundle_config_name(const BundleConfigCpp &config)
{
    return rust::String(config.name);
}

bool bundle_config_singleton(const BundleConfigCpp &config)
{
    return config.singleton;
}

bool bundle_config_is_system_app(const BundleConfigCpp &config)
{
    return config.isSystemApp;
}

int32_t bundle_config_proxy_count(const BundleConfigCpp &config)
{
    return static_cast<int32_t>(config.proxyDatas.size());
}

rust::String bundle_config_proxy_uri(const BundleConfigCpp &config, int32_t index)
{
    if (index < 0 || static_cast<size_t>(index) >= config.proxyDatas.size()) {
        return rust::String();
    }
    return rust::String(config.proxyDatas[index].uri);
}

rust::String bundle_config_proxy_read_perm(const BundleConfigCpp &config, int32_t index)
{
    if (index < 0 || static_cast<size_t>(index) >= config.proxyDatas.size()) {
        return rust::String();
    }
    return rust::String(config.proxyDatas[index].readPerm);
}

rust::String bundle_config_proxy_write_perm(const BundleConfigCpp &config, int32_t index)
{
    if (index < 0 || static_cast<size_t>(index) >= config.proxyDatas.size()) {
        return rust::String();
    }
    return rust::String(config.proxyDatas[index].writePerm);
}

rust::String bundle_config_proxy_module_name(const BundleConfigCpp &config, int32_t index)
{
    if (index < 0 || static_cast<size_t>(index) >= config.proxyDatas.size()) {
        return rust::String();
    }
    return rust::String(config.proxyDatas[index].moduleName);
}

int32_t bundle_config_proxy_profile_result_code(const BundleConfigCpp &config, int32_t index)
{
    if (index < 0 || static_cast<size_t>(index) >= config.proxyDatas.size()) {
        return -1;
    }
    return config.proxyDatas[index].profileResultCode;
}

rust::String bundle_config_proxy_store_name(const BundleConfigCpp &config, int32_t index)
{
    if (index < 0 || static_cast<size_t>(index) >= config.proxyDatas.size()) {
        return rust::String();
    }
    return rust::String(config.proxyDatas[index].storeName);
}

rust::String bundle_config_proxy_table_name(const BundleConfigCpp &config, int32_t index)
{
    if (index < 0 || static_cast<size_t>(index) >= config.proxyDatas.size()) {
        return rust::String();
    }
    return rust::String(config.proxyDatas[index].tableName);
}

rust::String bundle_config_proxy_type(const BundleConfigCpp &config, int32_t index)
{
    if (index < 0 || static_cast<size_t>(index) >= config.proxyDatas.size()) {
        return rust::String();
    }
    return rust::String(config.proxyDatas[index].type);
}

rust::String bundle_config_proxy_scope(const BundleConfigCpp &config, int32_t index)
{
    if (index < 0 || static_cast<size_t>(index) >= config.proxyDatas.size()) {
        return rust::String();
    }
    return rust::String(config.proxyDatas[index].scope);
}

int32_t bundle_config_extension_count(const BundleConfigCpp &config)
{
    return static_cast<int32_t>(config.extensions.size());
}

int32_t bundle_config_extension_type(const BundleConfigCpp &config, int32_t index)
{
    if (index < 0 || static_cast<size_t>(index) >= config.extensions.size()) {
        return -1;
    }
    return config.extensions[index].type;
}

rust::String bundle_config_extension_read_perm(const BundleConfigCpp &config, int32_t index)
{
    if (index < 0 || static_cast<size_t>(index) >= config.extensions.size()) {
        return rust::String();
    }
    return rust::String(config.extensions[index].readPerm);
}

rust::String bundle_config_extension_write_perm(const BundleConfigCpp &config, int32_t index)
{
    if (index < 0 || static_cast<size_t>(index) >= config.extensions.size()) {
        return rust::String();
    }
    return rust::String(config.extensions[index].writePerm);
}

rust::String bundle_config_extension_uri(const BundleConfigCpp &config, int32_t index)
{
    if (index < 0 || static_cast<size_t>(index) >= config.extensions.size()) {
        return rust::String();
    }
    return rust::String(config.extensions[index].uri);
}

bool bundle_config_extension_is_silent_enabled(const BundleConfigCpp &config, int32_t index)
{
    if (index < 0 || static_cast<size_t>(index) >= config.extensions.size()) {
        return true;
    }
    return config.extensions[index].isSilentEnabled;
}

rust::String bundle_config_proxy_backup(const BundleConfigCpp &config, int32_t index)
{
    if (index < 0 || static_cast<size_t>(index) >= config.proxyDatas.size()) {
        return rust::String();
    }
    return rust::String(config.proxyDatas[index].backup);
}

rust::String bundle_config_proxy_ext_uri(const BundleConfigCpp &config, int32_t index)
{
    if (index < 0 || static_cast<size_t>(index) >= config.proxyDatas.size()) {
        return rust::String();
    }
    return rust::String(config.proxyDatas[index].extUri);
}

bool bundle_config_proxy_store_meta_data_from_uri(const BundleConfigCpp &config, int32_t index)
{
    if (index < 0 || static_cast<size_t>(index) >= config.proxyDatas.size()) {
        return false;
    }
    return config.proxyDatas[index].storeMetaDataFromUri;
}

int32_t bundle_config_proxy_allow_list_count(const BundleConfigCpp &config, int32_t proxy_idx)
{
    if (proxy_idx < 0 || static_cast<size_t>(proxy_idx) >= config.proxyDatas.size()) {
        return 0;
    }
    return static_cast<int32_t>(config.proxyDatas[proxy_idx].allowLists.size());
}

rust::String bundle_config_proxy_allow_list_app_id(
    const BundleConfigCpp &config, int32_t proxy_idx, int32_t list_idx)
{
    if (proxy_idx < 0 || static_cast<size_t>(proxy_idx) >= config.proxyDatas.size()) {
        return rust::String();
    }
    auto &lists = config.proxyDatas[proxy_idx].allowLists;
    if (list_idx < 0 || static_cast<size_t>(list_idx) >= lists.size()) {
        return rust::String();
    }
    return rust::String(lists[list_idx].appIdentifier);
}

bool bundle_config_proxy_allow_list_only_main(
    const BundleConfigCpp &config, int32_t proxy_idx, int32_t list_idx)
{
    if (proxy_idx < 0 || static_cast<size_t>(proxy_idx) >= config.proxyDatas.size()) {
        return false;
    }
    auto &lists = config.proxyDatas[proxy_idx].allowLists;
    if (list_idx < 0 || static_cast<size_t>(list_idx) >= lists.size()) {
        return false;
    }
    return lists[list_idx].onlyMain;
}

int32_t bundle_config_proxy_table_config_count(const BundleConfigCpp &config, int32_t proxy_idx)
{
    if (proxy_idx < 0 || static_cast<size_t>(proxy_idx) >= config.proxyDatas.size()) {
        return 0;
    }
    return static_cast<int32_t>(config.proxyDatas[proxy_idx].tableConfig.size());
}

rust::String bundle_config_proxy_table_config_uri(
    const BundleConfigCpp &config, int32_t proxy_idx, int32_t cfg_idx)
{
    if (proxy_idx < 0 || static_cast<size_t>(proxy_idx) >= config.proxyDatas.size()) {
        return rust::String();
    }
    auto &cfgs = config.proxyDatas[proxy_idx].tableConfig;
    if (cfg_idx < 0 || static_cast<size_t>(cfg_idx) >= cfgs.size()) {
        return rust::String();
    }
    return rust::String(cfgs[cfg_idx].uri);
}

int32_t bundle_config_proxy_table_config_cross_mode(
    const BundleConfigCpp &config, int32_t proxy_idx, int32_t cfg_idx)
{
    if (proxy_idx < 0 || static_cast<size_t>(proxy_idx) >= config.proxyDatas.size()) {
        return 0;
    }
    auto &cfgs = config.proxyDatas[proxy_idx].tableConfig;
    if (cfg_idx < 0 || static_cast<size_t>(cfg_idx) >= cfgs.size()) {
        return 0;
    }
    return cfgs[cfg_idx].crossUserMode;
}

int32_t bundle_config_extension_profile_result_code(const BundleConfigCpp &config, int32_t index)
{
    if (index < 0 || static_cast<size_t>(index) >= config.extensions.size()) {
        return -1;
    }
    return config.extensions[index].profileResultCode;
}

int32_t bundle_config_extension_table_config_count(const BundleConfigCpp &config, int32_t ext_idx)
{
    if (ext_idx < 0 || static_cast<size_t>(ext_idx) >= config.extensions.size()) {
        return 0;
    }
    return static_cast<int32_t>(config.extensions[ext_idx].tableConfig.size());
}

rust::String bundle_config_extension_table_config_uri(
    const BundleConfigCpp &config, int32_t ext_idx, int32_t cfg_idx)
{
    if (ext_idx < 0 || static_cast<size_t>(ext_idx) >= config.extensions.size()) {
        return rust::String();
    }
    auto &cfgs = config.extensions[ext_idx].tableConfig;
    if (cfg_idx < 0 || static_cast<size_t>(cfg_idx) >= cfgs.size()) {
        return rust::String();
    }
    return rust::String(cfgs[cfg_idx].uri);
}

int32_t bundle_config_extension_table_config_cross_mode(
    const BundleConfigCpp &config, int32_t ext_idx, int32_t cfg_idx)
{
    if (ext_idx < 0 || static_cast<size_t>(ext_idx) >= config.extensions.size()) {
        return 0;
    }
    auto &cfgs = config.extensions[ext_idx].tableConfig;
    if (cfg_idx < 0 || static_cast<size_t>(cfg_idx) >= cfgs.size()) {
        return 0;
    }
    return cfgs[cfg_idx].crossUserMode;
}

BmsProxyEntry bundle_config_get_proxy_entry(const BundleConfigCpp &config, int32_t index)
{
    BmsProxyEntry entry{};
    if (index < 0 || static_cast<size_t>(index) >= config.proxyDatas.size()) {
        return entry;
    }
    auto &proxy = config.proxyDatas[index];
    entry.uri = rust::String(proxy.uri);
    entry.read_permission = rust::String(proxy.readPerm);
    entry.write_permission = rust::String(proxy.writePerm);
    entry.module_name = rust::String(proxy.moduleName);
    entry.profile_result_code = proxy.profileResultCode;
    entry.store_name = rust::String(proxy.storeName);
    entry.table_name = rust::String(proxy.tableName);
    entry.data_type = rust::String(proxy.type);
    entry.scope = rust::String(proxy.scope);
    entry.backup = rust::String(proxy.backup);
    entry.ext_uri = rust::String(proxy.extUri);
    entry.store_meta_data_from_uri = proxy.storeMetaDataFromUri;
    for (auto &al : proxy.allowLists) {
        BmsAllowListEntry ale{};
        ale.app_identifier = rust::String(al.appIdentifier);
        ale.only_main = al.onlyMain;
        entry.allow_lists.push_back(std::move(ale));
    }
    for (auto &tc : proxy.tableConfig) {
        BmsTableConfigEntry tce{};
        tce.uri = rust::String(tc.uri);
        tce.cross_user_mode = tc.crossUserMode;
        entry.table_config.push_back(std::move(tce));
    }
    return entry;
}

} // namespace OHOS::DataShare
