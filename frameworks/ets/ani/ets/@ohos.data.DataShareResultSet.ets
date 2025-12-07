/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

 export enum DataType {
  TYPE_NULL = 0,
  TYPE_LONG = 1,
  TYPE_DOUBLE = 2,
  TYPE_STRING = 3,
  TYPE_BLOB = 4
}

export default interface DataShareResultSet {
  rowCount: int;
  goToFirstRow(): boolean;
  goToLastRow(): boolean;
  goToNextRow(): boolean;
  goToPreviousRow(): boolean;
  goTo(offset: int): boolean;
  goToRow(position: int): boolean;
  getBlob(columnIndex: int): Uint8Array;
  getString(columnIndex: int): string;
  getLong(columnIndex: int): long;
  getDouble(columnIndex: int): double;
  close(): void;
  getColumnIndex(columnName: string): int;
  getColumnName(columnIndex: int): string;
  getDataType(columnIndex: int): DataType;
}

class DataShareResultSetInner implements DataShareResultSet {
  static { loadLibrary("datashare_ani_rs") }

  private nativePtr: long = 0;

  private clearNativePtr(): void {
      this.nativePtr = 0;
  }

  constructor(nativePtr: long) {
      this.nativePtr = nativePtr;
  }

  native getRowCount(): int;

  get rowCount(): int {
    return this.getRowCount();
  }

  set rowCount(value: int) {
    this.rowCount = value;
  }

  native goToFirstRow(): boolean;

  native goToLastRow(): boolean;

  native goToNextRow(): boolean;

  native goToPreviousRow(): boolean;

  native goTo(offset: int): boolean;

  native goToRow(position: int): boolean;

  native getBlob(columnIndex: int): Uint8Array;

  native getString(columnIndex: int): string;

  native getLong(columnIndex: int): long;

  native getDouble(columnIndex: int): double;

  native close(): void;

  native getColumnIndex(columnName: string): int;

  native getColumnName(columnIndex: int): string;

  native getDataType(columnIndex: int): DataType;
}
