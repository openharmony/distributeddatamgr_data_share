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

#ifndef DATASHARE_ERRNO_H
#define DATASHARE_ERRNO_H

namespace OHOS {
namespace DataShare {

/**
* @brief The error code in the correct case.
*/
constexpr int E_OK = 0;

/**
* @brief The base code of the exception error code.
*/
constexpr int E_BASE = 1000;

/**
* @brief The error code for common exceptions.
*/
constexpr int E_ERROR = (E_BASE + 1);

/**
* @brief The error code for readonly.
*/
constexpr int E_CANNOT_UPDATE_READONLY = (E_BASE + 2);

/**
* @brief The error code for remove file.
*/
constexpr int E_REMOVE_FILE = (E_BASE + 3);

/**
* @brief The error code for empty file name.
*/
constexpr int E_EMPTY_FILE_NAME = (E_BASE + 4);

/**
* @brief The error code for empty table name.
*/
constexpr int E_EMPTY_TABLE_NAME = (E_BASE + 5);

/**
* @brief The error code for empty valuesbucket.
*/
constexpr int E_EMPTY_VALUES_BUCKET = (E_BASE + 6);

/**
* @brief The error code for invalid statement.
*/
constexpr int E_INVALID_STATEMENT = (E_BASE + 7);

/**
* @brief The error code for invalid column index.
*/
constexpr int E_INVALID_COLUMN_INDEX = (E_BASE + 8);

/**
* @brief The error code for invalid column type.
*/
constexpr int E_INVALID_COLUMN_TYPE = (E_BASE + 9);

/**
* @brief The error code for invalid column name.
*/
constexpr int E_INVALID_COLUMN_NAME = (E_BASE + 10);

/**
* @brief The error code for query execute.
*/
constexpr int E_QUERY_IN_EXECUTE = (E_BASE + 11);

/**
* @brief The error code for transaction execute.
*/
constexpr int E_TRANSACTION_IN_EXECUTE = (E_BASE + 12);

/**
* @brief The error code for execute step query.
*/
constexpr int E_EXECUTE_IN_STEP_QUERY = (E_BASE + 13);

/**
* @brief The error code for execute write read connection.
*/
constexpr int E_EXECUTE_WRITE_IN_READ_CONNECTION = (E_BASE + 14);

/**
* @brief The error code for begin transaction read connection.
*/
constexpr int E_BEGIN_TRANSACTION_IN_READ_CONNECTION = (E_BASE + 15);

/**
* @brief The error code for no transaction read session.
*/
constexpr int E_NO_TRANSACTION_IN_SESSION = (E_BASE + 16);

/**
* @brief The error code for more step query one session.
*/
constexpr int E_MORE_STEP_QUERY_IN_ONE_SESSION = (E_BASE + 17);

/**
* @brief The error code for no row query.
*/
constexpr int E_NO_ROW_IN_QUERY = (E_BASE + 18);

/**
* @brief The error code for invalid bind args count.
*/
constexpr int E_INVALID_BIND_ARGS_COUNT = (E_BASE + 19);

/**
* @brief The error code for invalid object type.
*/
constexpr int E_INVALID_OBJECT_TYPE = (E_BASE + 20);

/**
* @brief The error code for invalid conflict flag.
*/
constexpr int E_INVALID_CONFLICT_FLAG = (E_BASE + 21);

/**
* @brief The error code for having clause not in group.
*/
constexpr int E_HAVING_CLAUSE_NOT_IN_GROUP_BY = (E_BASE + 22);

/**
* @brief The error code for not supported step resultset.
*/
constexpr int E_NOT_SUPPORTED_BY_STEP_RESULT_SET = (E_BASE + 23);

/**
* @brief The error code for step resultset cross threads.
*/
constexpr int E_STEP_RESULT_SET_CROSS_THREADS = (E_BASE + 24);

/**
* @brief The error code for step result query not executed.
*/
constexpr int E_STEP_RESULT_QUERY_NOT_EXECUTED = (E_BASE + 25);

/**
* @brief The error code for step result is after last.
*/
constexpr int E_STEP_RESULT_IS_AFTER_LAST = (E_BASE + 26);

/**
* @brief The error code for step result query exceeded.
*/
constexpr int E_STEP_RESULT_QUERY_EXCEEDED = (E_BASE + 27);

/**
* @brief The error code for statement not prepared.
*/
constexpr int E_STATEMENT_NOT_PREPARED = (E_BASE + 28);

/**
* @brief The error code for execute result incorrect.
*/
constexpr int E_EXECUTE_RESULT_INCORRECT = (E_BASE + 29);

/**
* @brief The error code for step result closed.
*/
constexpr int E_STEP_RESULT_CLOSED = (E_BASE + 30);

/**
* @brief The error code for relative path.
*/
constexpr int E_RELATIVE_PATH = (E_BASE + 31);

/**
* @brief The error code for empty new encrypt key.
*/
constexpr int E_EMPTY_NEW_ENCRYPT_KEY = (E_BASE + 32);

/**
* @brief The error code for change unencrypted to encrypted.
*/
constexpr int E_CHANGE_UNENCRYPTED_TO_ENCRYPTED = (E_BASE + 33);

/**
* @brief The error code for change encrypt key in busy.
*/
constexpr int E_CHANGE_ENCRYPT_KEY_IN_BUSY = (E_BASE + 34);

/**
* @brief The error code for step statement not init.
*/
constexpr int E_STEP_STATEMENT_NOT_INIT = (E_BASE + 35);

/**
* @brief The error code for not supported attach in wal mode.
*/
constexpr int E_NOT_SUPPORTED_ATTACH_IN_WAL_MODE = (E_BASE + 36);

/**
* @brief The error code for create folder fail.
*/
constexpr int E_CREATE_FOLDER_FAIL = (E_BASE + 37);

/**
* @brief The error code for sqlite builder normalize fail.
*/
constexpr int E_SQLITE_SQL_BUILDER_NORMALIZE_FAIL = (E_BASE + 38);

/**
* @brief The error code for store session not give connection temporarily.
*/
constexpr int E_STORE_SESSION_NOT_GIVE_CONNECTION_TEMPORARILY = (E_BASE + 39);

/**
* @brief The error code for store session no current transaction.
*/
constexpr int E_STORE_SESSION_NO_CURRENT_TRANSACTION = (E_BASE + 40);

/**
* @brief The error code for not support.
*/
constexpr int E_NOT_SUPPORT = (E_BASE + 41);

/**
* @brief The error code for invalid parcel.
*/
constexpr int E_INVALID_PARCEL = (E_BASE + 42);

/**
* @brief The error code for invalid file path.
*/
constexpr int E_INVALID_FILE_PATH = (E_BASE + 43);

/**
* @brief The error code for set persist wal.
*/
constexpr int E_SET_PERSIST_WAL = (E_BASE + 44);
} // namespace DataShare
} // namespace OHOS

#endif