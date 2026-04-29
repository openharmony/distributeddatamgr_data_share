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

#ifndef FFI_KVSTORE_BRIDGE_H
#define FFI_KVSTORE_BRIDGE_H

#include "cxx.h"
#include <memory>
#include "grd_base/grd_db_api.h"
#include "grd_base/grd_type_export.h"
#include "grd_base/grd_resultset_api.h"
#include "grd_document/grd_document_api.h"

namespace OHOS::GRD {

// Wrapper class for GRD_DB opaque pointer.
// Destructor calls GRD_DBClose with flags=0 as a safety net.
// Use grd_db_close() for explicit close with custom flags.
class GrdDbCpp {
public:
    explicit GrdDbCpp(GRD_DB *db) : db_(db) {}
    ~GrdDbCpp()
    {
        if (db_ != nullptr) {
            GRD_DBClose(db_, 0);
            db_ = nullptr;
        }
    }

    GrdDbCpp(const GrdDbCpp &) = delete;
    GrdDbCpp &operator=(const GrdDbCpp &) = delete;

    GRD_DB *Get() const { return db_; }

    // Releases ownership of the raw pointer without closing.
    // Used by grd_db_close() to prevent the destructor from double-closing.
    GRD_DB *Release()
    {
        GRD_DB *tmp = db_;
        db_ = nullptr;
        return tmp;
    }

private:
    GRD_DB *db_;
};

// Wrapper class for GRD_ResultSet opaque pointer.
// Destructor calls GRD_FreeResultSet to release the result set.
class GrdResultSetCpp {
public:
    explicit GrdResultSetCpp(GRD_ResultSet *rs) : rs_(rs) {}
    ~GrdResultSetCpp()
    {
        if (rs_ != nullptr) {
            GRD_FreeResultSet(rs_);
            rs_ = nullptr;
        }
    }

    GrdResultSetCpp(const GrdResultSetCpp &) = delete;
    GrdResultSetCpp &operator=(const GrdResultSetCpp &) = delete;

    GRD_ResultSet *Get() const { return rs_; }

private:
    GRD_ResultSet *rs_;
};

// Database lifecycle
std::unique_ptr<GrdDbCpp> grd_db_open(rust::Str db_path, rust::Str config_str, uint32_t flags);
int32_t grd_db_close(GrdDbCpp &db, uint32_t flags);
int32_t grd_flush(const GrdDbCpp &db, uint32_t flags);

// Collection management
int32_t grd_create_collection(
    const GrdDbCpp &db, rust::Str collection_name, rust::Str option_str, uint32_t flags);
int32_t grd_drop_collection(const GrdDbCpp &db, rust::Str collection_name, uint32_t flags);

// Document operations
int32_t grd_upsert_doc(
    const GrdDbCpp &db, rust::Str collection_name,
    rust::Str filter, rust::Str document, uint32_t flags);
int32_t grd_delete_doc(
    const GrdDbCpp &db, rust::Str collection_name, rust::Str filter, uint32_t flags);
std::unique_ptr<GrdResultSetCpp> grd_find_doc(
    const GrdDbCpp &db, rust::Str collection_name,
    rust::Str filter, rust::Str projection, uint32_t flags);

// ResultSet navigation
int32_t grd_next(const GrdResultSetCpp &result_set);
rust::String grd_get_value(const GrdResultSetCpp &result_set);

} // namespace OHOS::GRD

#endif // FFI_KVSTORE_BRIDGE_H
