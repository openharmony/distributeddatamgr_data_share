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

#ifndef JS_DATASHARE_EXT_ABILITY_H
#define JS_DATASHARE_EXT_ABILITY_H

#include <memory>
#include "datashare_result_set.h"
#include "datashare_predicates.h"
#include "datashare_ext_ability.h"
#include "js_runtime.h"
#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"
#include "datashare_business_error.h"

namespace OHOS {
namespace DataShare {
using namespace AbilityRuntime;
class JsResult {
public:
    JsResult() = default;
    bool GetRecvReply() const
    {
        return isRecvReply_;
    }

    void GetResult(int &value)
    {
        value = callbackResultNumber_;
    }

    void GetResult(std::string &value)
    {
        std::lock_guard<std::mutex> lock(asyncLock_);
        value = callbackResultString_;
    }

    void GetResult(std::vector<std::string> &value)
    {
        std::lock_guard<std::mutex> lock(asyncLock_);
        value = callbackResultStringArr_;
    }

    void GetResult(std::vector<BatchUpdateResult> &results)
    {
        std::lock_guard<std::mutex> lock(asyncLock_);
        results = updateResults_;
    }

    void GetResultSet(std::shared_ptr<DataShareResultSet> &value)
    {
        std::lock_guard<std::mutex> lock(asyncLock_);
        value = callbackResultObject_;
    }

    void GetBusinessError(DatashareBusinessError &businessError)
    {
        std::lock_guard<std::mutex> lock(asyncLock_);
        businessError = businessError_;
    }

    void SetAsyncResult(napi_env env, DatashareBusinessError &businessError, napi_value result);
    void CheckAndSetAsyncResult(napi_env env);
private:
    bool UnwrapBatchUpdateResult(napi_env env, napi_value &info, std::vector<BatchUpdateResult> &results);
    bool isRecvReply_ = false;
    int callbackResultNumber_ = -1;
    std::string callbackResultString_ = "";
    std::vector<std::string> callbackResultStringArr_ = {};
    std::mutex asyncLock_;
    std::shared_ptr<DataShareResultSet> callbackResultObject_ = nullptr;
    DatashareBusinessError businessError_;
    std::vector<BatchUpdateResult> updateResults_ = {};
};
/**
 * @brief Basic datashare extension ability components.
 */
class JsDataShareExtAbility : public DataShareExtAbility {
public:
    explicit JsDataShareExtAbility(JsRuntime& jsRuntime);
    virtual ~JsDataShareExtAbility() override;

    /**
     * @brief Create JsDataShareExtAbility.
     *
     * @param runtime The runtime.
     * @return The JsDataShareExtAbility instance.
     */
    static JsDataShareExtAbility* Create(const std::unique_ptr<Runtime>& runtime);

    /**
     * @brief Init the extension.
     *
     * @param record the extension record.
     * @param application the application info.
     * @param handler the extension handler.
     * @param token the remote token.
     */
    void Init(const std::shared_ptr<AppExecFwk::AbilityLocalRecord> &record,
        const std::shared_ptr<AppExecFwk::OHOSApplication> &application,
        std::shared_ptr<AppExecFwk::AbilityHandler> &handler,
        const sptr<IRemoteObject> &token) override;

    /**
     * @brief Called when this datashare extension ability is started. You must override this function if you want to
     *        perform some initialization operations during extension startup.
     *
     * This function can be called only once in the entire lifecycle of an extension.
     * @param Want Indicates the {@link Want} structure containing startup information about the extension.
     */
    void OnStart(const AAFwk::Want &want) override;

    /**
     * @brief Called when this datashare extension ability is connected for the first time.
     *
     * You can override this function to implement your own processing logic.
     *
     * @param want Indicates the {@link Want} structure containing connection information about the datashare
     * extension.
     * @return Returns a pointer to the <b>sid</b> of the connected datashare extension ability.
     */
    sptr<IRemoteObject> OnConnect(const AAFwk::Want &want) override;

    /**
     * @brief Obtains the MIME types of files supported.
     *
     * @param uri Indicates the path of the files to obtain.
     * @param mimeTypeFilter Indicates the MIME types of the files to obtain. This parameter cannot be null.
     *
     * @return Returns the matched MIME types. If there is no match, null is returned.
     */
    std::vector<std::string> GetFileTypes(const Uri &uri, const std::string &mimeTypeFilter) override;

    /**
     * @brief Opens a file in a specified remote path.
     *
     * @param uri Indicates the path of the file to open.
     * @param mode Indicates the file open mode, which can be "r" for read-only access, "w" for write-only access
     * (erasing whatever data is currently in the file), "wt" for write access that truncates any existing file,
     * "wa" for write-only access to append to any existing data, "rw" for read and write access on any existing data,
     *  or "rwt" for read and write access that truncates any existing file.
     *
     * @return Returns the file descriptor.
     */
    int OpenFile(const Uri &uri, const std::string &mode) override;

    /**
     * @brief This is like openFile, open a file that need to be able to return sub-sections of files，often assets
     * inside of their .hap.
     *
     * @param uri Indicates the path of the file to open.
     * @param mode Indicates the file open mode, which can be "r" for read-only access, "w" for write-only access
     * (erasing whatever data is currently in the file), "wt" for write access that truncates any existing file,
     * "wa" for write-only access to append to any existing data, "rw" for read and write access on any existing
     * data, or "rwt" for read and write access that truncates any existing file.
     *
     * @return Returns the RawFileDescriptor object containing file descriptor.
     */
    int OpenRawFile(const Uri &uri, const std::string &mode) override;

    /**
     * @brief Inserts a single data record into the database.
     *
     * @param uri Indicates the path of the data to operate.
     * @param value  Indicates the data record to insert. If this parameter is null, a blank row will be inserted.
     *
     * @return Returns the index of the inserted data record.
     */
    int Insert(const Uri &uri, const DataShareValuesBucket &value) override;

    /**
     * @brief Updates data records in the database.
     *
     * @param uri Indicates the path of data to update.
     * @param predicates Indicates filter criteria. You should define the processing logic when this parameter is null.
     * @param value Indicates the data to update. This parameter can be null.
     *
     * @return Returns the number of data records updated.
     */
    int Update(const Uri &uri, const DataSharePredicates &predicates,
        const DataShareValuesBucket &value) override;

    /**
     * @brief Batch updates data records in the database.
     *
     * @param updateOperations Indicates the param of data to update.
     * @param results Indicates the number of data records updated.
     *
     * @return Return the execution results of batch updates.
     */
    virtual int BatchUpdate(const UpdateOperations &operations, std::vector<BatchUpdateResult> &results) override;

    /**
     * @brief Deletes one or more data records from the database.
     *
     * @param uri Indicates the path of the data to operate.
     * @param predicates Indicates filter criteria. You should define the processing logic when this parameter is null.
     *
     * @return Returns the number of data records deleted.
     */
    int Delete(const Uri &uri, const DataSharePredicates &predicates) override;

    /**
     * @brief Deletes one or more data records from the database.
     *
     * @param uri Indicates the path of data to query.
     * @param predicates Indicates filter criteria. You should define the processing logic when this parameter is null.
     * @param columns Indicates the columns to query. If this parameter is null, all columns are queried.
     *
     * @return Returns the query result.
     */
    std::shared_ptr<DataShareResultSet> Query(const Uri &uri, const DataSharePredicates &predicates,
        std::vector<std::string> &columns, DatashareBusinessError &businessError) override;

    /**
     * @brief Obtains the MIME type matching the data specified by the URI of the Data ability. This method should be
     * implemented by a Data ability. Data abilities supports general data types, including text, HTML, and JPEG.
     *
     * @param uri Indicates the URI of the data.
     *
     * @return Returns the MIME type that matches the data specified by uri.
     */
    std::string GetType(const Uri &uri) override;

    /**
     * @brief Inserts multiple data records into the database.
     *
     * @param uri Indicates the path of the data to operate.
     * @param values Indicates the data records to insert.
     *
     * @return Returns the number of data records inserted.
     */
    int BatchInsert(const Uri &uri, const std::vector<DataShareValuesBucket> &values) override;

    /**
     * @brief Registers an observer to DataObsMgr specified by the given Uri.
     *
     * @param uri, Indicates the path of the data to operate.
     * @param dataObserver, Indicates the IDataAbilityObserver object.
     */
    bool RegisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver) override;

    /**
     * @brief Deregisters an observer used for DataObsMgr specified by the given Uri.
     *
     * @param uri, Indicates the path of the data to operate.
     * @param dataObserver, Indicates the IDataAbilityObserver object.
     */
    bool UnregisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver) override;

    /**
     * @brief Notifies the registered observers of a change to the data resource specified by Uri.
     *
     * @param uri, Indicates the path of the data to operate.
     *
     * @return Return true if success. otherwise return false.
     */
    bool NotifyChange(const Uri &uri) override;

    /**
     * @brief Notifies the registered observers of a change to the data resource specified by Uri.
     *
     * @param uri, Indicates the path of the data to operate.
     *
     * @return Return true if success. otherwise return false.
     */
    bool NotifyChangeWithUser(const Uri &uri, int32_t userId);

    /**
     * @brief Converts the given uri that refer to the Data ability into a normalized URI. A normalized URI can be used
     * across devices, persisted, backed up, and restored. It can refer to the same item in the Data ability even if
     * the context has changed. If you implement URI normalization for a Data ability, you must also implement
     * denormalizeUri(ohos.utils.net.Uri) to enable URI denormalization. After this feature is enabled, URIs passed to
     * any method that is called on the Data ability must require normalization verification and denormalization. The
     * default implementation of this method returns null, indicating that this Data ability does not support URI
     * normalization.
     *
     * @param uri Indicates the Uri object to normalize.
     *
     * @return Returns the normalized Uri object if the Data ability supports URI normalization; returns null otherwise.
     */
    Uri NormalizeUri(const Uri &uri) override;

    /**
     * @brief Converts the given normalized uri generated by normalizeUri(ohos.utils.net.Uri) into a denormalized one.
     * The default implementation of this method returns the original URI passed to it.
     *
     * @param uri uri Indicates the Uri object to denormalize.
     *
     * @return Returns the denormalized Uri object if the denormalization is successful; returns the original Uri
     * passed to this method if there is nothing to do; returns null if the data identified by the original Uri cannot
     * be found in the current environment.
     */
    Uri DenormalizeUri(const Uri &uri) override;

    void InitResult(std::shared_ptr<JsResult> result);
    struct AsyncContext {
        bool isNeedNotify_ = false;
    };
private:
    struct AsyncPoint {
        std::shared_ptr<AsyncContext> context;
    };
    struct AsyncCallBackPoint {
        std::shared_ptr<JsResult> result;
    };
    napi_value CallObjectMethod(const char *name, napi_value const *argv = nullptr, size_t argc = 0,
        bool isAsync = true);
    napi_value CallObjectMethod(
        const char *name, napi_value const *argv, size_t argc, std::shared_ptr<AsyncContext> asyncContext);
    void SaveNewCallingInfo(napi_env &env);
    void GetSrcPath(std::string &srcPath);
    napi_value MakePredicates(napi_env env, const DataSharePredicates &predicates);
    napi_value MakeUpdateOperation(napi_env env, const UpdateOperation &updateOperation);
    static napi_value AsyncCallback(napi_env env, napi_callback_info info);
    static napi_value AsyncCallbackWithContext(napi_env env, napi_callback_info info);
    void CheckAndSetAsyncResult(napi_env env);
    static void NotifyToDataShareService();
    static void UnWrapBusinessError(napi_env env, napi_value info, DatashareBusinessError &businessError);
    static napi_valuetype UnWrapPropertyType(napi_env env, napi_value info,
        const std::string &key);

    static std::string UnWrapProperty(napi_env env, napi_value info, const std::string &key);
    int32_t InitAsyncCallParams(size_t argc, napi_env &env, napi_value *args);

    static constexpr int ACTIVE_INVOKER = 1;
    JsRuntime& jsRuntime_;
    std::unique_ptr<NativeReference> jsObj_;
    std::shared_ptr<JsResult> result_;
};
} // namespace DataShare
} // namespace OHOS
#endif // JS_DATASHARE_EXT_ABILITY_H