/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */



export default interface DataShareResultSet {
    close(): void
    getColumnIndex(columnName: string): number;
    getString(columnIndex: number): string;
    goToFirstRow(): boolean;
    rowCount:number;
}

class DataShareResultSetImpl implements DataShareResultSet {

    static {loadLibrary("data_share_result_set")}
    private nativePtr: long = 0;

    private clearNativePtr(): void {
        this.nativePtr = 0;
    }

    constructor(nativePtr: long) {
        this.nativePtr = nativePtr;
    }
    native close(): void;
    native getColumnIndex(columnName: string): number;
    native getString(columnIndex: number): string;
    native goToFirstRow(): boolean;
    native getRowCount(): number;
    get rowCount(): number {
        return this.getRowCount();
    }

    set rowCount(value: number) {
        this.rowCount = value;
    }
}
