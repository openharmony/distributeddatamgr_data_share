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

//! CXX bridge wrapper for CommonEvent.

/// FFI declarations for CommonEvent publish/subscribe operations.
#[cxx::bridge(namespace = "OHOS::EventFwk")]
pub mod ffi {
    extern "Rust" {
        /// Rust-side callback for receiving common events.
        type CommonEventCallback;

        /// Called when a subscribed common event is received.
        fn on_receive(self: &mut CommonEventCallback, event_name: &str);
    }

    unsafe extern "C++" {
        include!("ffi_commonevent_bridge.h");

        /// Opaque C++ common event subscriber handle.
        type CppCommonEventSubscriber;

        /// Publishes a common event with the given name and code.
        fn publish_common_event(event_name: &str, code: i32) -> bool;
        /// Subscribes to a common event and returns a subscriber handle.
        fn subscribe_common_event(
            event_name: &str,
            callback: Box<CommonEventCallback>,
        ) -> SharedPtr<CppCommonEventSubscriber>;

        /// Unsubscribes from a common event using the subscriber handle.
        fn unsubscribe_common_event(
            subscriber: SharedPtr<CppCommonEventSubscriber>,
        ) -> bool;
    }
}

/// Callback for common event notifications.
pub struct CommonEventCallback {
    inner: Box<dyn CommonEventCallbackTrait>,
}

impl CommonEventCallback {
    /// Called when a common event is received.
    pub fn on_receive(&mut self, event_name: &str) {
        self.inner.on_receive(event_name);
    }
}

/// Trait for receiving common event notifications.
pub trait CommonEventCallbackTrait: Send {
    /// Called when a subscribed common event is received.
    fn on_receive(&mut self, event_name: &str);
}

/// Subscriber for common event notifications.
pub struct CommonEventSubscriber {
    inner: cxx::SharedPtr<ffi::CppCommonEventSubscriber>,
}

impl CommonEventSubscriber {
    /// Subscribes to a common event with the given name and callback.
    pub fn subscribe(
        event_name: &str,
        callback: Box<dyn CommonEventCallbackTrait>,
    ) -> Option<Self> {
        let cb = Box::new(CommonEventCallback { inner: callback });
        let inner = ffi::subscribe_common_event(event_name, cb);
        if inner.is_null() {
            None
        } else {
            Some(Self { inner })
        }
    }

    /// Unsubscribes from the common event.
    pub fn unsubscribe(self) -> bool {
        ffi::unsubscribe_common_event(self.inner)
    }
}

/// Publisher for common event notifications.
pub struct CommonEventPublisher;

impl CommonEventPublisher {
    /// Publishes a common event with the given name and code.
    pub fn publish(event_name: &str, code: i32) -> bool {
        ffi::publish_common_event(event_name, code)
    }
}

#[cfg(test)]
mod ut_wrapper {
    include!("../tests/ut/ut_wrapper.rs");
}
