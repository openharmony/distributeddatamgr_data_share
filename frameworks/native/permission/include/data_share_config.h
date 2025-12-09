/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_DISTRIBUTED_DATA_SERVICES_CONFIG_CONFIG_FACTORY_H
#define OHOS_DISTRIBUTED_DATA_SERVICES_CONFIG_CONFIG_FACTORY_H
#include <cstdint>
#include <list>
#include <string>
#include "serializable.h"
#include "visibility.h"
namespace OHOS {
namespace DataShare {

class DataShareConfig final : public Serializable {
public:
    struct Bundle final : public Serializable {
        std::string name;
        std::string appIdentifier;
        bool Marshal(Serializable::json &node) const override;
        bool Unmarshal(const Serializable::json &node) override;
    };
    struct ConsumerProvider final : public Serializable {
        std::vector<Bundle> consumer;
        Bundle provider;
        bool Marshal(Serializable::json &node) const override;
        bool Unmarshal(const Serializable::json &node) override;
    };
    bool Marshal(Serializable::json &node) const override;
    bool Unmarshal(const Serializable::json &node) override;
    std::vector<std::string> dataShareExtNames;
    std::vector<std::string> uriTrusts;
    std::vector<ConsumerProvider> extensionObsTrusts;
    std::vector<std::string> singletonUriTrusts;
};

class GlobalConfig final : public Serializable {
public:
    DataShareConfig *dataShare = nullptr;
    ~GlobalConfig();
    bool Marshal(json &node) const override;
    bool Unmarshal(const json &node) override;
};

class ConfigFactory {
public:
    static ConfigFactory &GetInstance();
    int32_t Initialize();
    DataShareConfig *GetDataShareConfig();
private:
    static constexpr const char *CONF_PATH = "/system/etc/distributeddata/conf";
    ConfigFactory();
    ~ConfigFactory();

    std::string file_;
    GlobalConfig config_;
    bool isInited = false;
};
} // namespace DistributedData
} // namespace OHOS
#endif // OHOS_DISTRIBUTED_DATA_SERVICES_CONFIG_CONFIG_FACTORY_H
