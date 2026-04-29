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

#include "ffi_kvstore_bridge.h"
#include "wrapper.rs.h"
#include "grd_base/grd_error.h"

namespace OHOS::GRD {

std::unique_ptr<GrdDbCpp> grd_db_open(rust::Str db_path, rust::Str config_str, uint32_t flags)
{
    std::string pathStr(db_path);
    const char *config = nullptr;
    std::string configStr;
    if (!config_str.empty()) {
        configStr = std::string(config_str);
        config = configStr.c_str();
    }

    GRD_DB *db = nullptr;
    int32_t status = GRD_DBOpen(pathStr.c_str(), config, flags, &db);
    if (status != GRD_OK || db == nullptr) {
        return nullptr;
    }
    return std::make_unique<GrdDbCpp>(db);
}

int32_t grd_db_close(GrdDbCpp &db, uint32_t flags)
{
    GRD_DB *raw = db.Release();
    if (raw == nullptr) {
        return GRD_OK;
    }
    return GRD_DBClose(raw, flags);
}

int32_t grd_flush(const GrdDbCpp &db, uint32_t flags)
{
    return GRD_Flush(db.Get(), flags);
}

int32_t grd_create_collection(
    const GrdDbCpp &db, rust::Str collection_name, rust::Str option_str, uint32_t flags)
{
    std::string nameStr(collection_name);
    const char *option = nullptr;
    std::string optionStr;
    if (!option_str.empty()) {
        optionStr = std::string(option_str);
        option = optionStr.c_str();
    }
    return GRD_CreateCollection(db.Get(), nameStr.c_str(), option, flags);
}

int32_t grd_drop_collection(const GrdDbCpp &db, rust::Str collection_name, uint32_t flags)
{
    std::string nameStr(collection_name);
    return GRD_DropCollection(db.Get(), nameStr.c_str(), flags);
}

int32_t grd_upsert_doc(
    const GrdDbCpp &db, rust::Str collection_name,
    rust::Str filter, rust::Str document, uint32_t flags)
{
    std::string nameStr(collection_name);
    std::string filterStr(filter);
    std::string docStr(document);
    return GRD_UpsertDoc(db.Get(), nameStr.c_str(), filterStr.c_str(), docStr.c_str(), flags);
}

int32_t grd_delete_doc(
    const GrdDbCpp &db, rust::Str collection_name, rust::Str filter, uint32_t flags)
{
    std::string nameStr(collection_name);
    std::string filterStr(filter);
    return GRD_DeleteDoc(db.Get(), nameStr.c_str(), filterStr.c_str(), flags);
}

std::unique_ptr<GrdResultSetCpp> grd_find_doc(
    const GrdDbCpp &db, rust::Str collection_name,
    rust::Str filter, rust::Str projection, uint32_t flags)
{
    std::string nameStr(collection_name);
    std::string filterStr(filter);
    std::string projStr(projection);

    Query query;
    query.filter = filterStr.c_str();
    query.projection = projStr.c_str();

    GRD_ResultSet *resultSet = nullptr;
    int32_t status = GRD_FindDoc(db.Get(), nameStr.c_str(), query, flags, &resultSet);
    if (status != GRD_OK || resultSet == nullptr) {
        return nullptr;
    }
    return std::make_unique<GrdResultSetCpp>(resultSet);
}

int32_t grd_next(const GrdResultSetCpp &result_set)
{
    return GRD_Next(result_set.Get());
}

rust::String grd_get_value(const GrdResultSetCpp &result_set)
{
    char *value = nullptr;
    int32_t status = GRD_GetValue(result_set.Get(), &value);
    if (status != GRD_OK || value == nullptr) {
        return rust::String("");
    }
    rust::String result(value);
    GRD_FreeValue(value);
    return result;
}

} // namespace OHOS::GRD
