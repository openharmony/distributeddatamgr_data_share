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

//! IPC Interface factory for ISharedResultSet.
//!
//! Equivalent to C++ `ishared_result_set.cpp`.
//!
//! Provides factory functions for creating stubs and proxies via parcels.
//! In C++, `ISharedResultSet::WriteToParcel` creates a stub and writes
//! both the remote object and result set data (via Marshalling) to the parcel.
//! `ISharedResultSet::ReadFromParcel` reads the remote object, creates a proxy,
//! and unmarshals the result set data (via Unmarshalling).
//!
//! SharedBlock Marshalling/Unmarshalling is handled alongside the RemoteObj round-trip.

use ipc::parcel::MsgParcel;
use ipc::remote::RemoteObj;

use crate::result_set::DataShareResultSet;
use crate::shared_block::SharedBlock;

use super::proxy::ISharedResultSetProxy;
use super::stub::ISharedResultSetStub;

/// Factory function: create a proxy from a parcel (consumer side).
///
/// Equivalent to C++ `ISharedResultSet::ReadFromParcel` / `ISharedResultSetProxy::CreateProxy`.
///
/// Steps:
/// 1. Read the remote object from parcel → create proxy
/// 2. Consume SharedBlock data from parcel (name + ashmem) to keep cursor aligned.
///    Rust proxy delegates all data access via IPC, so the local SharedBlock is not retained.
pub fn read_from_parcel(parcel: &mut MsgParcel) -> Option<ISharedResultSetProxy> {
    let remote = parcel.read_remote().ok()?;
    let proxy = ISharedResultSetProxy::new(remote);

    // 消费 parcel 中的 SharedBlock 数据（name + ashmem），保持读取游标对齐。
    // 即使读取失败也不影响 proxy 的功能（proxy 通过 IPC 远程调用获取数据）。
    let _name = parcel.read_string16();
    let _ashmem = parcel.read_ashmem();

    Some(proxy)
}

/// Factory function: create a stub from a result set and write to parcel (provider side).
///
/// Equivalent to C++ `ISharedResultSet::WriteToParcel` / `ISharedResultSetStub::CreateStub`.
///
/// For bridge-based result sets, creates an empty SharedBlock and shares its ashmem fd
/// with the client. The client triggers OnGo IPC for lazy data loading (same physical memory).
/// For direct-block result sets (no bridge), sends the pre-filled block as-is.
pub fn write_to_parcel(mut result_set: DataShareResultSet, parcel: &mut MsgParcel) -> Option<()> {
    // For bridge-based result sets: ensure a SharedBlock exists for shared memory.
    // Don't pre-fill — client will trigger OnGo for lazy loading (matches C++ behavior).
    if !result_set.has_block() && result_set.get_bridge().is_some() {
        let mut block = SharedBlock::create("result_set_block", 2 * 1024 * 1024).ok()?;
        block.set_column_num(result_set.get_column_names().len() as u32);
        result_set.attach_shared_block(*block);
    }

    // Get block info for marshalling BEFORE moving result_set into stub
    let block_info = result_set
        .get_block()
        .map(|blk| (blk.name().to_string(), unsafe { blk.ashmem().c_ashmem() }.clone()));

    // Move result_set (with block inside) into stub
    let stub = ISharedResultSetStub::new(result_set);
    let remote = RemoteObj::from_stub(stub)?;
    parcel.write_remote(remote).ok()?;

    // Write SharedBlock data to parcel (name + ashmem fd) — matches C++ Marshalling format
    if let Some((name, c_ashmem)) = block_info {
        parcel.write_string16(&name).ok()?;
        parcel.write_ashmem(utils_rust::ashmem::Ashmem::new(c_ashmem)).ok()?;
    }

    Some(())
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_write_to_parcel_without_block() {
        // ResultSet 没有 SharedBlock 时也应该正常工作（只写入 RemoteObject）
        let rs = DataShareResultSet::new();
        let mut parcel = MsgParcel::new();
        assert!(write_to_parcel(rs, &mut parcel).is_some());
    }
}
