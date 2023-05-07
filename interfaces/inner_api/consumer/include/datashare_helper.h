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

#include <map>
#include <memory>
#include <mutex>
#include <published_data_subscriber_manager.h>
#include <string>

#include "base_connection.h"
#include "context.h"
#include "datashare_business_error.h"
#include "datashare_template.h"
#include "rdb_subscriber_manager.h"
#include "uri.h"

using Uri = OHOS::Uri;

namespace OHOS {
namespace AppExecFwk {
class PacMap;
class IDataAbilityObserver;
} // namespace AppExecFwk

namespace DataShare {
using string = std::string;
class DataShareObserver {
public:
    DataShareObserver() = default;
    virtual ~DataShareObserver() = default;
    enum ChangeType : uint32_t {
        INSERT = 0,
        DELETE,
        UPDATE,
        OTHER,
        INVAILD,
    };

    struct ChangeInfo {
        ChangeType changeType_ = INVAILD;
        std::list<Uri> uris_ = {};
        const void *data_ = nullptr;
        uint32_t size_ = 0;
    };

    virtual void OnChange(const ChangeInfo &changeInfo) = 0;
};

class DataShareHelper final : public std::enable_shared_from_this<DataShareHelper> {
public:
    /**
     * @brief Destructor.
     */
    ~DataShareHelper();

    /**
     * @brief Creates a DataShareHelper instance with the Uri specified based on the given Context.
     *
     * @param context Indicates the Context object on OHOS.
     * @param strUri Indicates the database table or disk file to operate.
     *
     * @return Returns the created DataShareHelper instance with a specified Uri.
     */
    static std::shared_ptr<DataShareHelper> Creator(const std::shared_ptr<AppExecFwk::Context> &context,
        const std::string &strUri);

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
    static std::shared_ptr<DataShareHelper> Creator(const sptr<IRemoteObject> &token, const std::string &strUri);

    /**
     * @brief Creates a DataShareHelper instance with the Uri and {@link #CreateOptions} .
     *
     * @param strUri Indicates the database table or disk file to operate.
     * @param options Indicates the optional config.
     *
     * @return Returns the created DataShareHelper instance with a specified Uri.
     */
    static std::shared_ptr<DataShareHelper> Creator(const std::string &strUri, const CreateOptions &options);

    /**
     * @brief Releases the client resource of the Data share.
     * You should call this method to releases client resource after the data operations are complete.
     *
     * @return Returns true if the resource is successfully released; returns false otherwise.
     */
    bool Release();

    /**
     * @brief Obtains the MIME types of files supported.
     *
     * @param uri Indicates the path of the files to obtain.
     * @param mimeTypeFilter Indicates the MIME types of the files to obtain. This parameter cannot be null.
     *
     * @return Returns the matched MIME types. If there is no match, null is returned.
     */
    std::vector<std::string> GetFileTypes(Uri &uri, const std::string &mimeTypeFilter);

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
    int OpenFile(Uri &uri, const std::string &mode);

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
    int OpenRawFile(Uri &uri, const std::string &mode);

    /**
     * @brief Inserts a single data record into the database.
     *
     * @param uri Indicates the path of the data to operate.
     * @param value  Indicates the data record to insert. If this parameter is null, a blank row will be inserted.
     *
     * @return Returns the index of the inserted data record.
     */
    int Insert(Uri &uri, const DataShareValuesBucket &value);

    /**
     * @brief Updates data records in the database.
     *
     * @param uri Indicates the path of data to update.
     * @param predicates Indicates filter criteria. You should define the processing logic when this parameter is null.
     * @param value Indicates the data to update. This parameter can be null.
     *
     * @return Returns the number of data records updated.
     */
    int Update(Uri &uri, const DataSharePredicates &predicates, const DataShareValuesBucket &value);

    /**
     * @brief Deletes one or more data records from the database.
     *
     * @param uri Indicates the path of the data to operate.
     * @param predicates Indicates filter criteria. You should define the processing logic when this parameter is null.
     *
     * @return Returns the number of data records deleted.
     */
    int Delete(Uri &uri, const DataSharePredicates &predicates);

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
    std::shared_ptr<DataShareResultSet> Query(Uri &uri, const DataSharePredicates &predicates,
        std::vector<std::string> &columns, DatashareBusinessError *businessError = nullptr);

    /**
     * @brief Obtains the MIME type matching the data specified by the URI of the Data share. This method should be
     * implemented by a Data share. Data abilities supports general data types, including text, HTML, and JPEG.
     *
     * @param uri Indicates the URI of the data.
     *
     * @return Returns the MIME type that matches the data specified by uri.
     */
    std::string GetType(Uri &uri);

    /**
     * @brief Inserts multiple data records into the database.
     *
     * @param uri Indicates the path of the data to operate.
     * @param values Indicates the data records to insert.
     *
     * @return Returns the number of data records inserted.
     */
    int BatchInsert(Uri &uri, const std::vector<DataShareValuesBucket> &values);

    /**
     * @brief Registers an observer to DataObsMgr specified by the given Uri.
     *
     * @param uri, Indicates the path of the data to operate.
     * @param dataObserver, Indicates the IDataAbilityObserver object.
     */
    void RegisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver);

    /**
     * @brief Deregisters an observer used for DataObsMgr specified by the given Uri.
     *
     * @param uri, Indicates the path of the data to operate.
     * @param dataObserver, Indicates the IDataAbilityObserver object.
     */
    void UnregisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver);

    /**
     * @brief Notifies the registered observers of a change to the data resource specified by Uri.
     *
     * @param uri, Indicates the path of the data to operate.
     */
    void NotifyChange(const Uri &uri);

    /**
     * Registers an observer to DataObsMgr specified by the given Uri.
     *
     * @param uri, Indicates the path of the data to operate.
     * @param dataObserver, Indicates the IDataAbilityObserver object.
     * @param isDescendants, Indicates the Whether to note the change of descendants.
     */
    void RegisterObserverExt(const Uri &uri, std::shared_ptr<DataShareObserver> dataObserver,
        bool isDescendants);

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
    Uri NormalizeUri(Uri &uri);

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
    Uri DenormalizeUri(Uri &uri);

    /**
     * @brief Adds a template of {@link #SubscribeRdbData}.
     * @param uri, the uri to add.
     * @param subscriberId, the subscribe id to add.
     * @param tpl, the template to add.
     * @return Returns the error code.
     */
    int AddQueryTemplate(const std::string &uri, int64_t subscriberId, Template &tpl);

    /**
     * @brief Deletes a template of {@link #SubscribeRdbData}
     * @param uri, the uri to delete.
     * @param subscriberId, the subscribe id to delete.
     * @return Returns the error code.
     */
    int DelQueryTemplate(const std::string &uri, int64_t subscriberId);

    /**
     * @brief Update a single data into host data area.
     * @param data, the data to publish.
     * @param bundleName the bundleName of data to publish.
     * @return Returns the error code.
     */
    std::vector<OperationResult> Publish(const Data &data, const std::string &bundleName);

    /**
     * @brief Get published data by bundleName.
     * @param bundleName, the bundleName of data.
     * @return Data {@link #Data}
     */
    Data GetPublishedData(const std::string &bundleName);

    /**
     * @brief Registers observers to observe rdb data specified by the given uris and template.
     * @param uris, the paths of the data to operate.
     * @param templateId, the template of observers.
     * @param callback, the callback function of observers.
     * @return Returns the error code.
     */
    std::vector<OperationResult> SubscribeRdbData(const std::vector<std::string> &uris, const TemplateId &templateId,
        const std::function<void(const RdbChangeNode &changeNode)> &callback);

    /**
     * @brief Unregisters observers used for monitoring data specified by the given uris and template.
     * @param uris, the paths of the data to operate, if uris is empty, Unregisters all observers.
     * @param templateId, the template of observers.
     * @return Returns the error code.
     */
    std::vector<OperationResult> UnsubscribeRdbData(const std::vector<std::string> &uris = std::vector<std::string>(),
        const TemplateId &templateId = TemplateId());

    /**
     * @brief Enable observers by the given uris and template.
     * @param uris, the paths of the data to operate.
     * @param templateId, the template of observers.
     * @return Returns the error code.
     */
    std::vector<OperationResult> EnableRdbSubs(const std::vector<std::string> &uris, const TemplateId &templateId);

    /**
     * @brief Disable observers by the given uris and template.
     * @param uris, the paths of the data to operate.
     * @param templateId, the template of observers.
     * @return Returns the error code.
     */
    std::vector<OperationResult> DisableRdbSubs(const std::vector<std::string> &uris, const TemplateId &templateId);

    /**
     * @brief Registers observers to observe published data specified by the given uris and subscriberId.
     * @param uris, the uris of the data to operate.
     * @param subscriberId, the subscriberId of observers.
     * @param callback, the callback function of observers.
     * @return Returns the error code.
     */
    std::vector<OperationResult> SubscribePublishedData(const std::vector<std::string> &uris, int64_t subscriberId,
        const std::function<void(const PublishedDataChangeNode &changeNode)> &callback);

    /**
     * @brief Unregisters observers used for monitoring data specified by the given uris and subscriberId.
     * @param uris, the uris of the data to operate, if uris is empty, Unregisters all observers.
     * @param subscriberId, the subscriberId of observers.
     * @return Returns the error code.
     */
    std::vector<OperationResult> UnsubscribePublishedData(
        const std::vector<std::string> &uris = std::vector<std::string>(), int64_t subscriberId = 0);

    /**
     * @brief Enable observers by the given uris and subscriberId.
     * @param uris, the paths of the data to operate.
     * @param subscriberId, the subscriberId of observers.
     * @return Returns the error code.
     */
    std::vector<OperationResult> EnablePubSubs(const std::vector<std::string> &uris, int64_t subscriberId);

    /**
     * @brief Disable observers by the given uris and template.
     * @param uris, the paths of the data to operate.
     * @param subscriberId, the subscriberId of observers.
     * @return Returns the error code.
     */
    std::vector<OperationResult> DisablePubSubs(const std::vector<std::string> &uris, int64_t subscriberId);

private:
    DataShareHelper(const sptr<IRemoteObject> &token, const Uri &uri,
        std::shared_ptr<BaseConnection> dataShareConnection);
    DataShareHelper(const sptr<IRemoteObject> &token, const Uri &uri);
    DataShareHelper(const CreateOptions &options, const Uri &uri, std::shared_ptr<BaseConnection> dataShareConnection);
    bool isDataShareService_ = false;
    sptr<IRemoteObject> token_ = {};
    Uri uri_ = Uri("");
    std::shared_ptr<BaseConnection> connection_ = nullptr;
    static bool RegObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver);
    static bool UnregObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver);
    std::shared_ptr<RdbSubscriberManager> rdbSubscriberManager_ = nullptr;
    std::shared_ptr<PublishedDataSubscriberManager> publishedDataSubscriberManager_ = nullptr;
};
} // namespace DataShare
} // namespace OHOS
#endif // DATASHARE_HELPER_H