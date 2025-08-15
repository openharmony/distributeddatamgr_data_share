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
#include <fstream>
#include <mutex>
#include "data_share_config.h"
#include "datashare_log.h"
namespace OHOS {
namespace DataShare {
ConfigFactory::ConfigFactory() : file_(std::string(CONF_PATH) + "/config.json")
{
}

ConfigFactory::~ConfigFactory()
{
}

ConfigFactory &ConfigFactory::GetInstance()
{
    static std::mutex mutex;
    static ConfigFactory factory;
    if (factory.isInited) {
        return factory;
    }
    std::lock_guard<std::mutex> lock(mutex);
    factory.Initialize();
    return factory;
}

int32_t ConfigFactory::Initialize()
{
    std::string jsonStr;
    std::ifstream fin(file_);
    if (!fin.is_open()) {
        LOG_ERROR("ConfigFactory open file failed");
        return -1;
    }
    while (fin.good()) {
        std::string line;
        std::getline(fin, line);
        jsonStr += line;
    }
    config_.Unmarshall(jsonStr);
    isInited = true;
    return 0;
}

DataShareConfig *ConfigFactory::GetDataShareConfig()
{
    return config_.dataShare;
}

bool DataShareConfig::Marshal(Serializable::json &node) const
{
    SetValue(node[GET_NAME(dataShareExtNames)], dataShareExtNames);
    SetValue(node[GET_NAME(uriTrusts)], uriTrusts);
    SetValue(node[GET_NAME(extensionObsTrusts)], extensionObsTrusts);
    return true;
}

bool DataShareConfig::Unmarshal(const Serializable::json &node)
{
    GetValue(node, GET_NAME(dataShareExtNames), dataShareExtNames);
    GetValue(node, GET_NAME(uriTrusts), uriTrusts);
    GetValue(node, GET_NAME(extensionObsTrusts), extensionObsTrusts);
    return true;
}

bool DataShareConfig::ConsumerProvider::Marshal(Serializable::json &node) const
{
    SetValue(node[GET_NAME(consumer)], consumer);
    SetValue(node[GET_NAME(provider)], provider);
    return true;
}

bool DataShareConfig::ConsumerProvider::Unmarshal(const Serializable::json &node)
{
    GetValue(node, GET_NAME(consumer), consumer);
    GetValue(node, GET_NAME(provider), provider);
    return true;
}

bool DataShareConfig::Bundle::Marshal(Serializable::json &node) const
{
    SetValue(node[GET_NAME(name)], name);
    SetValue(node[GET_NAME(appIdentifier)], appIdentifier);
    return true;
}

bool DataShareConfig::Bundle::Unmarshal(const Serializable::json &node)
{
    GetValue(node, GET_NAME(name), name);
    GetValue(node, GET_NAME(appIdentifier), appIdentifier);
    return true;
}

bool GlobalConfig::Marshal(json &node) const
{
    SetValue(node[GET_NAME(dataShare)], dataShare);
    return true;
}

bool GlobalConfig::Unmarshal(const json &node)
{
    GetValue(node, GET_NAME(dataShare), dataShare);
    return true;
}

GlobalConfig::~GlobalConfig()
{
    if (dataShare != nullptr) {
        delete dataShare;
    }
}
} // namespace DistributedData
} // namespace OHOS