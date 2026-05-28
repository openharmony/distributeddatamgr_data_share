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

//! ConfigFactory — singleton for loading DataShare config.
//!
//! Corresponds to C++ `ConfigFactory` in
//! `frameworks/native/permission/src/data_share_config.cpp` (132 lines).
//!
//! Reads config.json and unmarshals it into DataShareConfig.
//! Shared between datashare_provider and datashare_permission targets.

use std::sync::OnceLock;

/// Configuration path (placeholder).
const CONF_PATH: &str = "/system/etc/distributeddatamgr";

/// Consumer-Provider trust pair.
///
/// Corresponds to C++ `DataShareConfig::ConsumerProvider`.
#[derive(Debug, Clone, Default)]
pub struct ConsumerProvider {
    /// Consumer bundle name.
    pub consumer: String,
    /// Provider bundle name.
    pub provider: String,
}

/// Bundle info.
///
/// Corresponds to C++ `DataShareConfig::Bundle`.
#[derive(Debug, Clone, Default)]
pub struct Bundle {
    /// Bundle name.
    pub name: String,
    /// App identifier.
    pub app_identifier: String,
}

/// DataShare configuration.
///
/// Corresponds to C++ `DataShareConfig`.
#[derive(Debug, Clone, Default)]
pub struct DataShareConfig {
    /// DataShare extension names.
    pub data_share_ext_names: Vec<String>,
    /// URI trust pairs.
    pub uri_trusts: Vec<ConsumerProvider>,
    /// Extension observer trust bundles.
    pub extension_obs_trusts: Vec<Bundle>,
}

impl DataShareConfig {
    /// Marshal the config to JSON.
    ///
    /// Corresponds to C++ `DataShareConfig::Marshal()`.
    pub fn marshal(&self) -> String {
        // TODO: Implement JSON serialization
        String::from("{}")
    }

    /// Unmarshal the config from JSON.
    ///
    /// Corresponds to C++ `DataShareConfig::Unmarshal()`.
    pub fn unmarshal(&mut self, _json: &str) -> bool {
        // TODO: Implement JSON deserialization
        true
    }
}

/// Global configuration wrapper.
///
/// Corresponds to C++ `GlobalConfig`.
#[derive(Debug, Default)]
struct GlobalConfig {
    /// DataShare configuration.
    data_share: Option<DataShareConfig>,
}

impl GlobalConfig {
    fn unmarshal(&mut self, _json: &str) {
        // TODO: Parse JSON and extract DataShareConfig
        self.data_share = Some(DataShareConfig::default());
    }
}

/// ConfigFactory — singleton for loading config.
///
/// Corresponds to C++ `ConfigFactory`.
pub struct ConfigFactory {
    /// Config file path.
    _file: String,
    /// Initialization flag.
    is_inited: bool,
    /// Global config.
    config: GlobalConfig,
}

/// Singleton instance.
static INSTANCE: OnceLock<ConfigFactory> = OnceLock::new();

impl ConfigFactory {
    /// Get the singleton instance.
    ///
    /// Corresponds to C++ `ConfigFactory::GetInstance()`.
    pub fn get_instance() -> &'static ConfigFactory {
        INSTANCE.get_or_init(|| {
            let mut factory = ConfigFactory {
                _file: format!("{}/config.json", CONF_PATH),
                is_inited: false,
                config: GlobalConfig::default(),
            };
            factory.initialize();
            factory
        })
    }

    /// Initialize the factory by reading config.json.
    ///
    /// Corresponds to C++ `ConfigFactory::Initialize()`.
    fn initialize(&mut self) -> i32 {
        // TODO: Read config.json from file system
        // For now, create default config
        self.config.unmarshal("{}");
        self.is_inited = true;
        0
    }

    /// Get the DataShareConfig.
    ///
    /// Corresponds to C++ `ConfigFactory::GetDataShareConfig()`.
    pub fn get_data_share_config(&self) -> Option<&DataShareConfig> {
        self.config.data_share.as_ref()
    }

    /// Check if the factory is initialized.
    pub fn is_initialized(&self) -> bool {
        self.is_inited
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_consumer_provider_default() {
        let cp = ConsumerProvider::default();
        assert!(cp.consumer.is_empty());
        assert!(cp.provider.is_empty());
    }

    #[test]
    fn test_bundle_default() {
        let bundle = Bundle::default();
        assert!(bundle.name.is_empty());
        assert!(bundle.app_identifier.is_empty());
    }

    #[test]
    fn test_datashare_config_default() {
        let config = DataShareConfig::default();
        assert!(config.data_share_ext_names.is_empty());
        assert!(config.uri_trusts.is_empty());
        assert!(config.extension_obs_trusts.is_empty());
    }

    #[test]
    fn test_datashare_config_marshal() {
        let config = DataShareConfig::default();
        let json = config.marshal();
        assert!(!json.is_empty());
    }

    #[test]
    fn test_datashare_config_unmarshal() {
        let mut config = DataShareConfig::default();
        assert!(config.unmarshal("{}"));
    }

    #[test]
    fn test_config_factory_singleton() {
        let instance1 = ConfigFactory::get_instance();
        let instance2 = ConfigFactory::get_instance();
        assert!(std::ptr::eq(instance1, instance2));
    }

    #[test]
    fn test_config_factory_initialized() {
        let factory = ConfigFactory::get_instance();
        assert!(factory.is_initialized());
    }

    #[test]
    fn test_config_factory_get_config() {
        let factory = ConfigFactory::get_instance();
        let config = factory.get_data_share_config();
        assert!(config.is_some());
    }
}
