/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

export default interface DataShareResultSet {
  rowCount: int;
  goToFirstRow(): boolean;
  goToLastRow(): boolean;
  goToNextRow(): boolean;
  getString(columnIndex: int): string;
  getLong(columnIndex: int): long;
  close(): void;
  getColumnIndex(columnName: string): int;
}

class DataShareResultSetInner implements DataShareResultSet {

  rowCount: int;

  static { loadLibrary("datashare_ani") }

  private nativePtr: long = 0;

  private clearNativePtr(): void {
      this.nativePtr = 0;
  }

  constructor(nativePtr: long) {
      this.nativePtr = nativePtr;
  }

  native goToFirstRow(): boolean;

  native goToLastRow(): boolean;

  native goToNextRow(): boolean;

  native getString(columnIndex: int): string;

  native getLong(columnIndex: int): long;

  native close(): void;

  native getColumnIndex(columnName: string): int;
}
