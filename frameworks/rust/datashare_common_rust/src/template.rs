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

/// Data size limits for IPC transfers
pub const DATA_SIZE_IPC_TRANSFER_LIMIT: usize = 200 * 1024; // 200KB
pub const DATA_SIZE_ASHMEM_TRANSFER_LIMIT: usize = 10 * 1024 * 1024; // 10MB
pub const ASHMEM_NAME: &str = "DataShareRdbChangeNode";
pub const DEFAULT_WAITTIME: i32 = 2;

/// Predicate template node - represents a single predicate in a template
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct PredicateTemplateNode {
    /// Key of the predicate
    pub key: String,
    /// SQL select statement for this predicate
    pub select_sql: String,
}

impl PredicateTemplateNode {
    /// Create a new predicate template node
    pub fn new<S: Into<String>>(key: S, select_sql: S) -> Self {
        Self {
            key: key.into(),
            select_sql: select_sql.into(),
        }
    }
}

/// Query template for subscription
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct Template {
    /// Update SQL for the template
    pub update: String,
    /// Predicates of the template
    pub predicates: Vec<PredicateTemplateNode>,
    /// Scheduler SQL for the template
    pub scheduler: String,
}

impl Template {
    /// Create a new empty template
    pub fn new() -> Self {
        Self {
            update: String::new(),
            predicates: Vec::new(),
            scheduler: String::new(),
        }
    }

    /// Create a template with predicates and scheduler
    pub fn with_predicates<S: Into<String>>(
        predicates: Vec<PredicateTemplateNode>,
        scheduler: S,
    ) -> Self {
        Self {
            update: String::new(),
            predicates,
            scheduler: scheduler.into(),
        }
    }

    /// Create a full template
    pub fn full<S: Into<String>>(
        update: S,
        predicates: Vec<PredicateTemplateNode>,
        scheduler: S,
    ) -> Self {
        Self {
            update: update.into(),
            predicates,
            scheduler: scheduler.into(),
        }
    }

    /// Add a predicate to the template
    pub fn add_predicate(&mut self, predicate: PredicateTemplateNode) {
        self.predicates.push(predicate);
    }

    /// Set the update SQL
    pub fn set_update<S: Into<String>>(&mut self, update: S) {
        self.update = update.into();
    }

    /// Set the scheduler SQL
    pub fn set_scheduler<S: Into<String>>(&mut self, scheduler: S) {
        self.scheduler = scheduler.into();
    }
}

impl Default for Template {
    fn default() -> Self {
        Self::new()
    }
}

/// Template identifier - uniquely identifies a template
#[derive(Debug, Clone, PartialEq, Eq, Hash)]
pub struct TemplateId {
    /// Subscriber ID
    pub subscriber_id: i64,
    /// Bundle name of template owner
    pub bundle_name: String,
}

impl TemplateId {
    /// Create a new template ID
    pub fn new(subscriber_id: i64, bundle_name: String) -> Self {
        Self {
            subscriber_id,
            bundle_name,
        }
    }
}

/// Options for creating a DataShare connection
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct CreateOptions {
    /// Whether this is a proxy connection
    pub is_proxy: bool,
    /// Optional security token
    pub token: Option<Vec<u8>>,
    /// Whether the connection is enabled
    pub enabled: bool,
    /// Wait time for connection in seconds
    pub wait_time: i32,
}

impl CreateOptions {
    /// Create a new CreateOptions with default settings
    pub fn new() -> Self {
        Self {
            is_proxy: false,
            token: None,
            enabled: true,
            wait_time: DEFAULT_WAITTIME,
        }
    }

    /// Create a proxy connection option
    pub fn proxy() -> Self {
        Self {
            is_proxy: true,
            token: None,
            enabled: true,
            wait_time: DEFAULT_WAITTIME,
        }
    }

    /// Set whether this is a proxy connection
    pub fn set_proxy(&mut self, is_proxy: bool) {
        self.is_proxy = is_proxy;
    }

    /// Set the security token
    pub fn set_token(&mut self, token: Vec<u8>) {
        self.token = Some(token);
    }

    /// Set whether the connection is enabled
    pub fn set_enabled(&mut self, enabled: bool) {
        self.enabled = enabled;
    }

    /// Set the wait time
    pub fn set_wait_time(&mut self, wait_time: i32) {
        self.wait_time = wait_time;
    }
}

impl Default for CreateOptions {
    fn default() -> Self {
        Self::new()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_predicate_template_node() {
        let node = PredicateTemplateNode::new("key1", "SELECT * FROM table");
        assert_eq!(node.key, "key1");
        assert_eq!(node.select_sql, "SELECT * FROM table");
    }

    #[test]
    fn test_template() {
        let mut tpl = Template::new();
        tpl.add_predicate(PredicateTemplateNode::new("p1", "SELECT * FROM t1"));
        assert_eq!(tpl.predicates.len(), 1);
    }

    #[test]
    fn test_template_id() {
        let tid = TemplateId::new(123, "com.example.app".to_string());
        assert_eq!(tid.subscriber_id, 123);
        assert_eq!(tid.bundle_name, "com.example.app");
    }

    #[test]
    fn test_create_options() {
        let opts = CreateOptions::new();
        assert!(!opts.is_proxy);
        assert!(opts.enabled);
        assert_eq!(opts.wait_time, DEFAULT_WAITTIME);

        let proxy_opts = CreateOptions::proxy();
        assert!(proxy_opts.is_proxy);
    }
}
