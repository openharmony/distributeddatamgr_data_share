// Copyright (c) 2026 Huawei Device Co., Ltd.
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// C++ bridge implementation for BundleMgr FFI.

#include "ffi_bundlemgr_bridge.h"
#include "wrapper.rs.h"
#include "bundle_mgr_interface.h"
#include "bundle_mgr_proxy.h"
#include "bundle_info.h"
#include "bundle_constants.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

namespace OHOS::AppExecFwk {

static sptr<IBundleMgr> GetBundleMgrProxy()
{
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!systemAbilityManager) {
        return nullptr;
    }
    sptr<IRemoteObject> remoteObject =
        systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (!remoteObject) {
        return nullptr;
    }
    return iface_cast<IBundleMgr>(remoteObject);
}

int32_t get_uid_for_bundle(rust::Str bundle_name)
{
    std::string name(bundle_name);
    auto bundleMgr = GetBundleMgrProxy();
    if (!bundleMgr) {
        return -1;
    }
    return bundleMgr->GetUidByBundleName(name, Constants::DEFAULT_USERID);
}

rust::String get_bundle_name_for_uid(int32_t uid)
{
    auto bundleMgr = GetBundleMgrProxy();
    if (!bundleMgr) {
        return rust::String("");
    }
    std::string bundleName;
    auto ret = bundleMgr->GetNameForUid(uid, bundleName);
    if (ret != 0) {
        return rust::String("");
    }
    return rust::String(bundleName);
}

bool is_system_app(rust::Str bundle_name)
{
    std::string name(bundle_name);
    auto bundleMgr = GetBundleMgrProxy();
    if (!bundleMgr) {
        return false;
    }
    int32_t uid = bundleMgr->GetUidByBundleName(name, Constants::DEFAULT_USERID);
    if (uid < 0) {
        return false;
    }
    return bundleMgr->CheckIsSystemAppByUid(uid);
}

FfiBundleInfo get_bundle_info(rust::Str bundle_name)
{
    FfiBundleInfo info;
    std::string name(bundle_name);
    info.name = rust::String(name);
    info.uid = get_uid_for_bundle(bundle_name);
    info.is_system_app = is_system_app(bundle_name);
    info.app_id = rust::String("");
    info.signature_info = rust::String("");

    auto bundleMgr = GetBundleMgrProxy();
    if (bundleMgr) {
        BundleInfo bundleInfo;
        bool ret = bundleMgr->GetBundleInfo(name,
            static_cast<int32_t>(GetBundleInfoFlag::GET_BUNDLE_INFO_WITH_SIGNATURE_INFO),
            bundleInfo, Constants::DEFAULT_USERID);
        if (ret) {
            info.app_id = rust::String(bundleInfo.appId);
            if (!bundleInfo.signatureInfo.fingerprint.empty()) {
                info.signature_info = rust::String(bundleInfo.signatureInfo.fingerprint);
            }
        }
    }
    return info;
}

FfiBundleInfo get_bundle_info_with_flags(rust::Str bundle_name, int32_t flags, int32_t user_id)
{
    FfiBundleInfo info;
    std::string name(bundle_name);
    info.name = rust::String(name);
    info.uid = -1;
    info.is_system_app = false;
    info.app_id = rust::String("");
    info.signature_info = rust::String("");

    auto bundleMgr = GetBundleMgrProxy();
    if (!bundleMgr) {
        return info;
    }
    BundleInfo bundleInfo;
    bool ret = bundleMgr->GetBundleInfo(name, flags, bundleInfo, user_id);
    if (!ret) {
        return info;
    }
    info.uid = bundleInfo.uid;
    info.is_system_app = bundleInfo.applicationInfo.isSystemApp;
    info.app_id = rust::String(bundleInfo.appId);
    if (!bundleInfo.signatureInfo.fingerprint.empty()) {
        info.signature_info = rust::String(bundleInfo.signatureInfo.fingerprint);
    }
    return info;
}

FfiBundleInfo get_bundle_info_for_self(int32_t flags)
{
    FfiBundleInfo info;
    info.name = rust::String("");
    info.uid = -1;
    info.is_system_app = false;
    info.app_id = rust::String("");
    info.signature_info = rust::String("");

    auto bundleMgr = GetBundleMgrProxy();
    if (!bundleMgr) {
        return info;
    }
    BundleInfo bundleInfo;
    auto ret = bundleMgr->GetBundleInfoForSelf(flags, bundleInfo);
    if (ret != 0) {
        return info;
    }
    info.name = rust::String(bundleInfo.name);
    info.uid = bundleInfo.uid;
    info.is_system_app = bundleInfo.applicationInfo.isSystemApp;
    info.app_id = rust::String(bundleInfo.appId);
    if (!bundleInfo.signatureInfo.fingerprint.empty()) {
        info.signature_info = rust::String(bundleInfo.signatureInfo.fingerprint);
    }
    return info;
}

} // namespace OHOS::AppExecFwk
