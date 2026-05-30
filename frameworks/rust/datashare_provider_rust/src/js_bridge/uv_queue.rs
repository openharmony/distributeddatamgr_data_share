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

//! DataShareUvQueue — UV event queue for async JS callback execution.
//!
//! Corresponds to C++ `DataShareUvQueue` in
//! `frameworks/native/provider/src/datashare_uv_queue.cpp` (100 lines).
//!
//! Provides `SyncCall()` to execute a task closure in the NAPI event loop
//! and optionally poll for the result using a retry mechanism.
//!
//! Key pattern:
//! 1. Create TaskEntry with atomic count, mutex, condition variable
//! 2. Send task via napi_send_event (immediate priority)
//! 3. Wait on condition variable with 3s timeout
//! 4. Poll result via CheckFuncAndExec (up to 2000 * 1ms)

use std::sync::atomic::{AtomicBool, AtomicI32, Ordering};
use std::sync::{Condvar, Mutex};
use std::time::Duration;

/// Wait timeout for condition variable (seconds).
const WAIT_TIME_SECS: u64 = 3;

/// Sleep time between retry polls (milliseconds).
const SLEEP_TIME_MS: u64 = 1;

/// Maximum number of retry polls.
const TRY_TIMES: i32 = 2000;

/// Task entry for the UV event queue.
///
/// Corresponds to C++ `TaskEntry` struct.
pub struct TaskEntry {
    /// Task function to execute.
    pub func: Option<Box<dyn FnOnce() + Send>>,
    /// Whether the task has completed.
    pub done: AtomicBool,
    /// Mutex for synchronization.
    pub mutex: Mutex<()>,
    /// Condition variable for waiting.
    pub condvar: Condvar,
    /// Reference count for shared ownership.
    pub count: AtomicI32,
}

impl TaskEntry {
    /// Create a new TaskEntry.
    pub fn new(func: Box<dyn FnOnce() + Send>) -> Self {
        Self {
            func: Some(func),
            done: AtomicBool::new(false),
            mutex: Mutex::new(()),
            condvar: Condvar::new(),
            count: AtomicI32::new(1),
        }
    }
}

/// DataShareUvQueue — UV event queue for async JS callbacks.
///
/// Corresponds to C++ `DataShareUvQueue`.
///
/// Uses `napi_send_event` (placeholder) to execute tasks in the NAPI
/// event loop, with synchronous waiting via condition variables.
pub struct DataShareUvQueue {
    /// NAPI environment handle (placeholder — opaque u64).
    _env: u64,
}

impl DataShareUvQueue {
    /// Create a new DataShareUvQueue.
    ///
    /// Corresponds to C++ `DataShareUvQueue::DataShareUvQueue(napi_env)`.
    pub fn new(env: u64) -> Self {
        // TODO: Call napi_get_uv_event_loop(env, &loop_)
        Self { _env: env }
    }

    /// Execute task function and mark as done.
    ///
    /// Corresponds to C++ `DataShareUvQueue::LambdaForWork(TaskEntry*)`.
    pub fn lambda_for_work(entry: &mut TaskEntry) {
        if let Some(func) = entry.func.take() {
            func();
        }
        entry.done.store(true, Ordering::Release);
        entry.condvar.notify_all();
    }

    /// Synchronously call a function in the event loop.
    ///
    /// Corresponds to C++ `DataShareUvQueue::SyncCall()`.
    ///
    /// 1. Creates a TaskEntry with the given function
    /// 2. Sends the task via napi_send_event (placeholder)
    /// 3. Waits on condition variable with timeout
    /// 4. Polls ret_func for result
    pub fn sync_call(
        &self,
        func: Box<dyn FnOnce() + Send>,
        ret_func: Option<Box<dyn Fn() -> bool>>,
    ) {
        let mut entry = TaskEntry::new(func);

        // TODO: napi_send_event(env_, task, napi_eprio_immediate)
        // For now, execute directly (placeholder)
        Self::lambda_for_work(&mut entry);

        // Wait for completion
        {
            let guard = entry.mutex.lock().unwrap();
            let _result = entry.condvar.wait_timeout_while(
                guard,
                Duration::from_secs(WAIT_TIME_SECS),
                |_| !entry.done.load(Ordering::Acquire),
            );
        }

        // Check and execute return function
        Self::check_func_and_exec(ret_func);
    }

    /// Poll the return function until it returns true or timeout.
    ///
    /// Corresponds to C++ `DataShareUvQueue::CheckFuncAndExec()`.
    ///
    /// Polls `ret_func()` up to 2000 times with 1ms sleep between each attempt.
    pub fn check_func_and_exec(ret_func: Option<Box<dyn Fn() -> bool>>) {
        if let Some(func) = ret_func {
            let mut try_times = TRY_TIMES;
            while !func() && try_times > 0 {
                std::thread::sleep(Duration::from_millis(SLEEP_TIME_MS));
                try_times -= 1;
            }
            if try_times <= 0 {
                // TODO: LOG_ERROR("function execute timeout.")
            }
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::sync::atomic::AtomicI32;
    use std::sync::Arc;

    #[test]
    fn test_uv_queue_creation() {
        let _queue = DataShareUvQueue::new(0);
    }

    #[test]
    fn test_task_entry_creation() {
        let entry = TaskEntry::new(Box::new(|| {}));
        assert!(!entry.done.load(Ordering::Acquire));
        assert_eq!(entry.count.load(Ordering::Acquire), 1);
    }

    #[test]
    fn test_lambda_for_work() {
        let executed = Arc::new(AtomicBool::new(false));
        let executed_clone = executed.clone();
        let mut entry = TaskEntry::new(Box::new(move || {
            executed_clone.store(true, Ordering::Release);
        }));
        DataShareUvQueue::lambda_for_work(&mut entry);
        assert!(entry.done.load(Ordering::Acquire));
        assert!(executed.load(Ordering::Acquire));
    }

    #[test]
    fn test_sync_call_without_ret_func() {
        let queue = DataShareUvQueue::new(0);
        let executed = Arc::new(AtomicBool::new(false));
        let executed_clone = executed.clone();
        queue.sync_call(
            Box::new(move || {
                executed_clone.store(true, Ordering::Release);
            }),
            None,
        );
        assert!(executed.load(Ordering::Acquire));
    }

    #[test]
    fn test_sync_call_with_ret_func() {
        let queue = DataShareUvQueue::new(0);
        let counter = Arc::new(AtomicI32::new(0));
        let counter_clone = counter.clone();
        queue.sync_call(
            Box::new(move || {
                counter_clone.store(42, Ordering::Release);
            }),
            Some(Box::new(move || {
                // Always returns true immediately
                true
            })),
        );
    }

    #[test]
    fn test_check_func_and_exec_none() {
        DataShareUvQueue::check_func_and_exec(None);
    }

    #[test]
    fn test_check_func_and_exec_immediate_true() {
        DataShareUvQueue::check_func_and_exec(Some(Box::new(|| true)));
    }

    #[test]
    fn test_check_func_and_exec_eventually_true() {
        let counter = Arc::new(AtomicI32::new(0));
        let counter_clone = counter.clone();
        DataShareUvQueue::check_func_and_exec(Some(Box::new(move || {
            let val = counter_clone.fetch_add(1, Ordering::SeqCst);
            val >= 3 // Returns true after 4 calls
        })));
        assert!(counter.load(Ordering::Acquire) >= 4);
    }

    #[test]
    fn test_task_entry_condvar() {
        let entry = TaskEntry::new(Box::new(|| {}));
        let guard = entry.mutex.lock().unwrap();
        let (_guard, timeout_result) = entry
            .condvar
            .wait_timeout(guard, Duration::from_millis(1))
            .unwrap();
        assert!(timeout_result.timed_out());
    }
}
