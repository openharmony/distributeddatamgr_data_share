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

use crate::block_writer::BlockWriter;
use datashare_common::error::{DataShareError, Result};

pub trait Writer {
    fn alloc_row(&mut self) -> Result<()>;
    fn write_null(&mut self, column: u32) -> Result<()>;
    fn write_long(&mut self, column: u32, value: i64) -> Result<()>;
    fn write_double(&mut self, column: u32, value: f64) -> Result<()>;
    fn write_blob(&mut self, column: u32, value: &[u8]) -> Result<()>;
    fn write_string(&mut self, column: u32, value: &str) -> Result<()>;
}

pub trait ResultSetBridge: Send + Sync {
    fn get_all_column_names(&self) -> Result<Vec<String>>;
    fn get_row_count(&self) -> Result<i32>;
    fn on_go(
        &mut self,
        start_row_index: i32,
        target_row_index: i32,
        writer: &mut dyn Writer,
    ) -> Result<()>;
}

pub struct ResultSetBridgeWrapper {
    _column_names: Vec<String>,
    _row_count: i32,
    get_all_column_names_fn: Box<dyn Fn() -> Result<Vec<String>> + Send + Sync>,
    get_row_count_fn: Box<dyn Fn() -> Result<i32> + Send + Sync>,
    on_go_fn: Box<dyn FnMut(i32, i32, &mut dyn Writer) -> Result<()> + Send + Sync>,
}

impl ResultSetBridgeWrapper {
    pub fn new(
        column_names: Vec<String>,
        row_count: i32,
        get_all_column_names_fn: impl Fn() -> Result<Vec<String>> + Send + Sync + 'static,
        get_row_count_fn: impl Fn() -> Result<i32> + Send + Sync + 'static,
        on_go_fn: impl FnMut(i32, i32, &mut dyn Writer) -> Result<()> + Send + Sync + 'static,
    ) -> Self {
        Self {
            _column_names: column_names,
            _row_count: row_count,
            get_all_column_names_fn: Box::new(get_all_column_names_fn),
            get_row_count_fn: Box::new(get_row_count_fn),
            on_go_fn: Box::new(on_go_fn),
        }
    }
}

impl ResultSetBridge for ResultSetBridgeWrapper {
    fn get_all_column_names(&self) -> Result<Vec<String>> {
        (self.get_all_column_names_fn)()
    }

    fn get_row_count(&self) -> Result<i32> {
        (self.get_row_count_fn)()
    }

    fn on_go(
        &mut self,
        start_row_index: i32,
        target_row_index: i32,
        writer: &mut dyn Writer,
    ) -> Result<()> {
        (self.on_go_fn)(start_row_index, target_row_index, writer)
    }
}

impl<T: Writer + ?Sized> Writer for Box<T> {
    fn alloc_row(&mut self) -> Result<()> {
        (**self).alloc_row()
    }

    fn write_null(&mut self, column: u32) -> Result<()> {
        (**self).write_null(column)
    }

    fn write_long(&mut self, column: u32, value: i64) -> Result<()> {
        (**self).write_long(column, value)
    }

    fn write_double(&mut self, column: u32, value: f64) -> Result<()> {
        (**self).write_double(column, value)
    }

    fn write_blob(&mut self, column: u32, value: &[u8]) -> Result<()> {
        (**self).write_blob(column, value)
    }

    fn write_string(&mut self, column: u32, value: &str) -> Result<()> {
        (**self).write_string(column, value)
    }
}

// NOTE: `impl Writer for BlockWriter` lives in `block_writer.rs` so it can
// adapt the new i32-returning API to this trait's `Result<()>` surface.

#[cfg(test)]
mod tests {
    use super::*;

    struct MockWriter {
        rows: Vec<Vec<CellValue>>,
        current_row: Vec<CellValue>,
    }

    #[derive(Debug, Clone)]
    enum CellValue {
        Null,
        Long(i64),
        Double(f64),
        Blob(Vec<u8>),
    }

    impl MockWriter {
        fn new() -> Self {
            Self {
                rows: Vec::new(),
                current_row: Vec::new(),
            }
        }
    }

    impl Writer for MockWriter {
        fn alloc_row(&mut self) -> Result<()> {
            self.current_row = Vec::new();
            Ok(())
        }

        fn write_null(&mut self, column: u32) -> Result<()> {
            while self.current_row.len() <= column as usize {
                self.current_row.push(CellValue::Null);
            }
            self.current_row[column as usize] = CellValue::Null;
            Ok(())
        }

        fn write_long(&mut self, column: u32, value: i64) -> Result<()> {
            while self.current_row.len() <= column as usize {
                self.current_row.push(CellValue::Null);
            }
            self.current_row[column as usize] = CellValue::Long(value);
            Ok(())
        }

        fn write_double(&mut self, column: u32, value: f64) -> Result<()> {
            while self.current_row.len() <= column as usize {
                self.current_row.push(CellValue::Null);
            }
            self.current_row[column as usize] = CellValue::Double(value);
            Ok(())
        }

        fn write_blob(&mut self, column: u32, value: &[u8]) -> Result<()> {
            while self.current_row.len() <= column as usize {
                self.current_row.push(CellValue::Null);
            }
            self.current_row[column as usize] = CellValue::Blob(value.to_vec());
            Ok(())
        }

        fn write_string(&mut self, column: u32, value: &str) -> Result<()> {
            self.write_blob(column, value.as_bytes())
        }
    }

    #[test]
    fn test_writer_trait() {
        let mut writer = MockWriter::new();
        writer.alloc_row().unwrap();
        writer.write_long(0, 42).unwrap();
        writer.write_double(1, 3.14).unwrap();
        writer.write_string(2, "hello").unwrap();
    }

    #[test]
    fn test_result_set_bridge_wrapper() {
        let mut bridge = ResultSetBridgeWrapper::new(
            vec!["id".to_string(), "value".to_string(), "name".to_string()],
            10,
            || {
                Ok(vec![
                    "id".to_string(),
                    "value".to_string(),
                    "name".to_string(),
                ])
            },
            || Ok(10),
            |start, end, writer| {
                for _ in start..end {
                    writer.alloc_row().unwrap();
                    writer.write_long(0, 1).unwrap();
                    writer.write_double(1, 1.0).unwrap();
                    writer.write_string(2, "test").unwrap();
                }
                Ok(())
            },
        );

        assert_eq!(bridge.get_all_column_names().unwrap().len(), 3);
        assert_eq!(bridge.get_row_count().unwrap(), 10);

        let mut mock_writer = MockWriter::new();
        bridge.on_go(0, 5, &mut mock_writer).unwrap();
    }
}
