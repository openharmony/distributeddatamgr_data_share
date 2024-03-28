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

#ifndef DATASHARE_HELPER_H
#define DATASHARE_HELPER_H

#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <string>

#include "data_ability_observer_interface.h"
#include "datashare_business_error.h"
#include "datashare_observer.h"
#include "datashare_operation_statement.h"
#include "datashare_predicates.h"
#include "datashare_result_set.h"
#include "datashare_template.h"
#include "datashare_values_bucket.h"
#include "uri.h"

using Uri = OHOS::Uri;

namespace OHOS {
namespace AppExecFwk {
class PacMap;
class IDataAbilityObserver;
} // namespace AppExecFwk

namespace DataShare {
using string = std::string;
class DataShareHelper : public std::enable_shared_from_this<DataShareHelper> {
public:
    /**
     * @brief Destructor.
     */
    virtual ~DataShareHelper() = default;

    /**
     * @brief You can use this method to specify the Uri of the data to operate and set the binding relationship
     * between the ability using the Data template (data share for short) and the associated client process in
     * a DataShareHelper instance.
     *
     * @param token Indicates the System token.
     * @param strUri Indicates the database table or disk file to operate.
     *
     * @return Returns the created DataShareHelper instance.
     */
    static std::shared_ptr<DataShareHelper> Creator(
        const sptr<IRemoteObject> &token, const std::string &strUri, const std::string &extUri = "");

    /**
     * @brief Creates a DataShareHelper instance with the Uri and {@link #CreateOptions} .
     *
     * @param strUri Indicates the database table or disk file to operate.
     * @param options Indicates the optional config.
     *
     * @return Returns the created DataShareHelper instance with a specified Uri.
     */
    static std::shared_ptr<DataShareHelper> Creator(const std::string &strUri, const CreateOptions &options,
        const std::string &bundleName = "");

    /**
     * @brief Releases the client resource of the Data share.
     * You should call this method to releases client resource after the data operations are complete.
     *
     * @return Returns true if the resource is successfully released; returns false otherwise.
     */
    virtual bool Release() = 0;

    /**
     * @brief Obtains the MIME types of files supported.
     *
     * @param uri Indicates the path of the files to obtain.
     * @param mimeTypeFilter Indicates the MIME types of the files to obtain. This parameter cannot be null.
     *
     * @return Returns the matched MIME types. If there is no match, null is returned.
     */
    virtual std::vector<std::string> GetFileTypes(Uri &uri, const std::string &mimeTypeFilter) = 0;

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
    virtual int OpenFile(Uri &uri, const std::string &mode) = 0;

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
    virtual int OpenRawFile(Uri &uri, const std::string &mode) = 0;

    /**
     * @brief Inserts a single data record into the database.
     *
     * @param uri Indicates the path of the data to operate.
     * @param value  Indicates the data record to insert. If this parameter is null, a blank row will be inserted.
     *
     * @return Returns the index of the inserted data record.
     */
    virtual int Insert(Uri &uri, const DataShareValuesBucket &value) = 0;

    /**
     * @brief Inserts a single data record into the database.
     *
     * @param uri Indicates the path of the data to operate.
     * @param value  Indicates the data record to insert. If this parameter is null, a blank row will be inserted.
     * @param result Indicates the result string of the insert operation.
     *
     * @return Returns the index of the inserted data record.
     */
    virtual int InsertExt(Uri &uri, const DataShareValuesBucket &value, std::string &result) = 0;

    /**
     * @brief Updates data records in the database.
     *
     * @param uri Indicates the path of data to update.
     * @param predicates Indicates filter criteria. You should define the processing logic when this parameter is null.
     * @param value Indicates the data to update. This parameter can be null.
     *
     * @return Returns the number of data records updated.
     */
    virtual int Update(Uri &uri, const DataSharePredicates &predicates, const DataShareValuesBucket &value) = 0;

    /**
     * @brief Batch updates data records in the database.
     *
     * @param updateOperations Indicates the param of data to update.
     * @param results Indicates the number of data records updated.
     *
     * @return Return the execution results of batch updates.
     */
    virtual int BatchUpdate(const UpdateOperations &operations, std::vector<BatchUpdateResult> &results) = 0;

    /**
     * @brief Deletes one or more data records from the database.
     *
     * @param uri Indicates the path of the data to operate.
     * @param predicates Indicates filter criteria. You should define the processing logic when this parameter is null.
     *
     * @return Returns the number of data records deleted.
     */
    virtual int Delete(Uri &uri, const DataSharePredicates &predicates) = 0;

    /**
     * @brief Query records from the database.
     *
     * @param uri Indicates the path of data to query.
     * @param predicates Indicates filter criteria. You should define the processing logic when this parameter is null.
     * @param columns Indicates the columns to query. If this parameter is null, all columns are queried.
     * @param businessError Indicates the error by query.
     *
     * @return Returns the query result.
     */
    virtual std::shared_ptr<DataShareResultSet> Query(Uri &uri, const DataSharePredicates &predicates,
        std::vector<std::string> &columns, DatashareBusinessError *businessError = nullptr) = 0;

    /**
     * @brief Obtains the MIME type matching the data specified by the URI of the Data share. This method should be
     * implemented by a Data share. Data abilities supports general data types, including text, HTML, and JPEG.
     *
     * @param uri Indicates the URI of the data.
     *
     * @return Returns the MIME type that matches the data specified by uri.
     */
    virtual std::string GetType(Uri &uri) = 0;

    /**
     * @brief Inserts multiple data records into the database.
     *
     * @param uri Indicates the path of the data to operate.
     * @param values Indicates the data records to insert.
     *
     * @return Returns the number of data records inserted.
     */
    virtual int BatchInsert(Uri &uri, const std::vector<DataShareValuesBucket> &values) = 0;

    /**
     * @brief Performs batch operations on the database.
     *
     * @param statements Indicates a list of database operation statement on the database.
     * @param result Indicates the result of the operation.
     *
     * @return Returns the ipc result.
     */
    virtual int ExecuteBatch(const std::vector<OperationStatement> &statements, ExecResultSet &result) = 0;

    /**
     * @brief Registers an observer to DataObsMgr specified by the given Uri.
     *
     * @param uri, Indicates the path of the data to operate.
     * @param dataObserver, Indicates the IDataAbilityObserver object.
     */
    virtual void RegisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver) = 0;

    /**
     * @brief Deregisters an observer used for DataObsMgr specified by the given Uri.
     *
     * @param uri, Indicates the path of the data to operate.
     * @param dataObserver, Indicates the IDataAbilityObserver object.
     */
    virtual void UnregisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver) = 0;

    /**
     * @brief Notifies the registered observers of a change to the data resource specified by Uri.
     *
     * @param uri, Indicates the path of the data to operate.
     */
    virtual void NotifyChange(const Uri &uri) = 0;

    /**
     * Registers an observer to DataObsMgr specified by the given Uri.
     *
     * @param uri, Indicates the path of the data to operate.
     * @param dataObserver, Indicates the IDataAbilityObserver object.
     * @param isDescendants, Indicates the Whether to note the change of descendants.
     */
    void RegisterObserverExt(const Uri &uri, std::shared_ptr<DataShareObserver> dataObserver, bool isDescendants);

    /**
     * Deregisters an observer used for DataObsMgr specified by the given Uri.
     *
     * @param uri, Indicates the path of the data to operate.
     * @param dataObserver, Indicates the IDataAbilityObserver object
     */
    void UnregisterObserverExt(const Uri &uri, std::shared_ptr<DataShareObserver> dataObserver);

    /**
     * Notifies the registered observers of a change to the data resource specified by Uris.
     *
     * @param changeInfo Indicates the info of the data to operate.
     */
    void NotifyChangeExt(const DataShareObserver::ChangeInfo &changeInfo);

    /**
     * @brief Converts the given uri that refer to the Data share into a normalized URI. A normalized URI can be used
     * across devices, persisted, backed up, and restored. It can refer to the same item in the Data share even if the
     * context has changed. If you implement URI normalization for a Data share, you must also implement
     * denormalizeUri(ohos.utils.net.Uri) to enable URI denormalization. After this feature is enabled, URIs passed to
     * any method that is called on the Data share must require normalization verification and denormalization. The
     * default implementation of this method returns null, indicating that this Data share does not support URI
     * normalization.
     *
     * @param uri Indicates the Uri object to normalize.
     *
     * @return Returns the normalized Uri object if the Data share supports URI normalization; returns null otherwise.
     */
    virtual Uri NormalizeUri(Uri &uri) = 0;

    /**
     * @brief Converts the given normalized uri generated by normalizeUri(ohos.utils.net.Uri) into a denormalized one.
     * The default implementation of this method returns the original URI passed to it.
     *
     * @param uri uri Indicates the Uri object to denormalize.
     *
     * @return Returns the denormalized Uri object if the denormalization is successful; returns the original Uri passed
     * to this method if there is nothing to do; returns null if the data identified by the original Uri cannot be found
     * in the current environment.
     */
    virtual Uri DenormalizeUri(Uri &uri) = 0;

    /**
     * @brief Adds a template of {@link #SubscribeRdbData}.
     * @param uri, the uri to add.
     * @param subscriberId, the subscribe id to add.
     * @param tpl, the template to add.
     * @return Returns the error code.
     */
    virtual int AddQueryTemplate(const std::string &uri, int64_t subscriberId, Template &tpl) = 0;

    /**
     * @brief Deletes a template of {@link #SubscribeRdbData}
     * @param uri, the uri to delete.
     * @param subscriberId, the subscribe id to delete.
     * @return Returns the error code.
     */
    virtual int DelQueryTemplate(const std::string &uri, int64_t subscriberId) = 0;

    /**
     * @brief Update a single data into host data area.
     * @param data, the data to publish.
     * @param bundleName the bundleName of data to publish.
     * @return Returns the error code.
     */
    virtual std::vector<OperationResult> Publish(const Data &data, const std::string &bundleName) = 0;

    /**
     * @brief Get published data by bundleName.
     * @param bundleName, the bundleName of data.
     * @param resultCode, the errcode returned by function
     * @return Data {@link #Data}
     */
    virtual Data GetPublishedData(const std::string &bundleName, int &resultCode) = 0;

    /**
     * @brief Registers observers to observe rdb data specified by the given uris and template.
     * @param uris, the paths of the data to operate.
     * @param templateId, the template of observers.
     * @param callback, the callback function of observers.
     * @return Returns the error code.
     */
    virtual std::vector<OperationResult> SubscribeRdbData(const std::vector<std::string> &uris,
        const TemplateId &templateId, const std::function<void(const RdbChangeNode &changeNode)> &callback) = 0;

    /**
     * @brief Unregisters observers used for monitoring data specified by the given uris and template.
     * @param uris, the paths of the data to operate, if uris is empty, Unregisters all observers.
     * @param templateId, the template of observers.
     * @return Returns the error code.
     */
    virtual std::vector<OperationResult> UnsubscribeRdbData(const std::vector<std::string> &uris,
        const TemplateId &templateId) = 0;

    /**
     * @brief Enable observers by the given uris and template.
     * @param uris, the paths of the data to operate.
     * @param templateId, the template of observers.
     * @return Returns the error code.
     */
    virtual std::vector<OperationResult> EnableRdbSubs(const std::vector<std::string> &uris,
        const TemplateId &templateId) = 0;

    /**
     * @brief Disable observers by the given uris and template.
     * @param uris, the paths of the data to operate.
     * @param templateId, the template of observers.
     * @return Returns the error code.
     */
    virtual std::vector<OperationResult> DisableRdbSubs(const std::vector<std::string> &uris,
        const TemplateId &templateId) = 0;

    /**
     * @brief Registers observers to observe published data specified by the given uris and subscriberId.
     * @param uris, the uris of the data to operate.
     * @param subscriberId, the subscriberId of observers.
     * @param callback, the callback function of observers.
     * @return Returns the error code.
     */
    virtual std::vector<OperationResult> SubscribePublishedData(const std::vector<std::string> &uris,
        int64_t subscriberId, const std::function<void(const PublishedDataChangeNode &changeNode)> &callback) = 0;

    /**
     * @brief Unregisters observers used for monitoring data specified by the given uris and subscriberId.
     * @param uris, the uris of the data to operate, if uris is empty, Unregisters all observers.
     * @param subscriberId, the subscriberId of observers.
     * @return Returns the error code.
     */
    virtual std::vector<OperationResult> UnsubscribePublishedData(const std::vector<std::string> &uris,
        int64_t subscriberId) = 0;

    /**
     * @brief Enable observers by the given uris and subscriberId.
     * @param uris, the paths of the data to operate.
     * @param subscriberId, the subscriberId of observers.
     * @return Returns the error code.
     */
    virtual std::vector<OperationResult> EnablePubSubs(const std::vector<std::string> &uris, int64_t subscriberId) = 0;

    /**
     * @brief Disable observers by the given uris and template.
     * @param uris, the paths of the data to operate.
     * @param subscriberId, the subscriberId of observers.
     * @return Returns the error code.
     */
    virtual std::vector<OperationResult> DisablePubSubs(const std::vector<std::string> &uris, int64_t subscriberId) = 0;

    /**
     * @brief Set default switch for silent access.
     * @param uri, the uri to disable/enable.
     * @param enable, the enable of silent switch.
     * @return Returns the error code.
     */
    static int SetSilentSwitch(Uri &uri, bool enable);

private:
    static std::shared_ptr<DataShareHelper> CreateServiceHelper(const std::string &bundleName = "");

    static bool IsSilentProxyEnable(const std::string &uri);

    static std::shared_ptr<DataShareHelper> CreateExtHelper(Uri &uri, const sptr<IRemoteObject> &token);

    static std::string TransferUriPrefix(const std::string &originPrefix, const std::string &replacedPrefix,
        const std::string &originUriStr);
};
} // namespace DataShare
} // namespace OHOS
#endif // DATASHARE_HELPER_H