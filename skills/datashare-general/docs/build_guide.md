# Datashare 构建指南

## 代码目录

| 模块 | 代码路径 |
|------|---------|
| data_share（客户端 + Provider） | `code/foundation/distributeddatamgr/data_share` |
| datamgr_service（服务端） | `code/foundation/distributeddatamgr/datamgr_service` |
| ability_runtime（DataObs） | `code/foundation/ability/ability_runtime` |

## 编译命令

### 完整编译（推荐）

完整编译会编译整个模块及其所有依赖，适合首次编译或完整验证。

```bash
# 编译 data_share 模块（客户端 + Provider）
./build.sh --product-name <product> --gn-flags='--export-compile-commands' --build-target data_share

# 编译 datamgr_service 模块（服务端）
./build.sh --product-name <product> --gn-flags='--export-compile-commands' --build-target datamgr_service

# 编译 ability_runtime 模块（DataObs）
./build.sh --product-name <product> --gn-flags='--export-compile-commands' --build-target ability_runtime
```

### 独立编译（快速编译）

独立编译只编译指定的子目录，适合快速验证代码修改。

```bash
# 编译 data_share 客户端部分
./build.sh --product-name <product> --gn-flags='--export-compile-commands' --build-target foundation/distributeddatamgr/data_share/interfaces/inner_api:datashare_consumer

# 编译 data_share 服务端部分
./build.sh --product-name <product> --gn-flags='--export-compile-commands' --build-target foundation/distributeddatamgr/datamgr_service/services/distributeddataservice/service/data_share:data_share_service

# 编译 DataObs 服务
./build.sh --product-name <product> --gn-flags='--export-compile-commands' --build-target foundation/ability/ability_runtime/services/dataobsmgr:dataobsms
```

### 编译 UT 测试

```bash
# 编译 data_share UT 测试
./build.sh --product-name <product> --gn-flags='--export-compile-commands' --build-target foundation/distributeddatamgr/data_share/test/unittest/native:unittest
```

## 注意事项

1. **`<product>`** 替换为实际产品名，如 `rk3568`、`hi3516` 等
2. **`--export-compile-commands`** 生成 compile_commands.json，方便 IDE 代码补全和跳转
3. **独立编译失败时**，尝试使用完整编译命令
4. **第二次编译起**，可添加 `--fast-rebuild` 参数提高编译效率：
   ```bash
   ./build.sh --product-name <product> --gn-flags='--export-compile-commands' --fast-rebuild --build-target data_share
   ```

## 编译输出

编译产物位于：
- 可执行文件/库：`out/<product>/` 目录下
- compile_commands.json：`out/<product>/compile_commands.json`