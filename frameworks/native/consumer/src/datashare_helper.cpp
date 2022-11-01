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

#include "datashare_helper.h"

#include "datashare_result_set.h"
#include "data_share_manager.h"
#include "data_ability_observer_interface.h"
#include "dataobs_mgr_client.h"
#include "datashare_log.h"
#include "idatashare.h"

namespace OHOS {
namespace DataShare {
using namespace AppExecFwk;
namespace {
const std::string SCHEME_DATASHARE = "datashare";
constexpr int INVALID_VALUE = -1;
}  // namespace

DataShareHelper::DataShareHelper(const sptr<IRemoteObject> &token, const Uri &uri,
    sptr<DataShareConnection> dataShareConnection)
{
    LOG_INFO("DataShareHelper::DataShareHelper start");
    token_ = token;
    uri_ = uri;
    dataShareConnection_ = dataShareConnection;
}

DataShareHelper::DataShareHelper(const sptr<IRemoteObject> &token, const Uri &uri)
{
    LOG_INFO("DataShareHelper::DataShareHelper start");
    token_ = token;
    uri_ = uri;
    isDataShareService_ = (uri_.GetQuery().find("Proxy=true") != std::string::npos);
    LOG_INFO("DataShareHelper::DataShareHelper end");
}

DataShareHelper::~DataShareHelper()
{
}

/**
 * @brief You can use this method to specify the Uri of the data to operate and set the binding relationship
 * between the ability using the Data template (data share for short) and the associated client process in
 * a DataShareHelper instance.
 *
 * @param context Indicates the Context object on OHOS.
 * @param strUri Indicates the database table or disk file to operate.
 *
 * @return Returns the created DataShareHelper instance.
 */
std::shared_ptr<DataShareHelper> DataShareHelper::Creator(
    const std::shared_ptr<Context> &context, const std::string &strUri)
{
    LOG_DEBUG("Creator with context and uri called start");
    if (context == nullptr) {
        LOG_ERROR("DataShareHelper::Creator failed, context == nullptr");
        return nullptr;
    }
    sptr<IRemoteObject> token = context->GetToken();
    return Creator(token, strUri);
}

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
std::shared_ptr<DataShareHelper> DataShareHelper::Creator(const sptr<IRemoteObject> &token, const std::string &strUri)
{
    LOG_DEBUG("Creator with runtime token and uri called start");
    if (token == nullptr) {
        LOG_ERROR("token == nullptr");
        return nullptr;
    }

    Uri uri(strUri);
    if (uri.GetScheme() != SCHEME_DATASHARE) {
        LOG_ERROR("the Scheme is not datashare, Scheme: %{public}s", uri.GetScheme().c_str());
        return nullptr;
    }
    if ((uri.GetQuery().find("Proxy=true") != std::string::npos) &&
        DataShareManager::GetDataShareService() != nullptr) {
        LOG_DEBUG("Creator with dataShareService successfully.");
        DataShareHelper *dataShareHelper = new (std::nothrow) DataShareHelper(token, uri);
        if (dataShareHelper) {
            return std::shared_ptr<DataShareHelper>(dataShareHelper);
        }
        LOG_ERROR("create DataShareHelper failed");
    }
    sptr<DataShareConnection> dataShareConnection = new (std::nothrow) DataShareConnection(uri);
    if (dataShareConnection == nullptr) {
        LOG_ERROR("create dataShareConnection failed");
        return nullptr;
    }
    if (!dataShareConnection->ConnectDataShareExtAbility(uri, token)) {
        LOG_ERROR("connect failed");
        return nullptr;
    }

    DataShareHelper *ptrDataShareHelper =
        new (std::nothrow) DataShareHelper(token, uri, dataShareConnection);
    if (ptrDataShareHelper == nullptr) {
        LOG_ERROR("create DataShareHelper failed");
        if (dataShareConnection->IsExtAbilityConnected()) {
            dataShareConnection->DisconnectDataShareExtAbility();
        }
        return nullptr;
    }

    return std::shared_ptr<DataShareHelper>(ptrDataShareHelper);
}

/**
 * @brief Releases the client resource of the data share.
 * You should call this method to releases client resource after the data operations are complete.
 *
 * @return Returns true if the resource is successfully released; returns false otherwise.
 */
bool DataShareHelper::Release()
{
    LOG_INFO("Release Start");
    if (dataShareConnection_ != nullptr) {
        dataShareConnection_->DisconnectDataShareExtAbility();
    }
    uri_ = Uri("");
    return true;
}

/**
 * @brief Obtains the MIME types of files supported.
 *
 * @param uri Indicates the path of the files to obtain.
 * @param mimeTypeFilter Indicates the MIME types of the files to obtain. This parameter cannot be null.
 *
 * @return Returns the matched MIME types. If there is no match, null is returned.
 */
std::vector<std::string> DataShareHelper::GetFileTypes(Uri &uri, const std::string &mimeTypeFilter)
{
    LOG_DEBUG("Start");
    std::vector<std::string> matchedMIMEs;
    if (dataShareConnection_ == nullptr) {
        LOG_ERROR("dataShareConnection_ is nullptr");
        return matchedMIMEs;
    }

    if (!dataShareConnection_->ConnectDataShareExtAbility(uri_, token_)) {
        LOG_ERROR("dataShareProxy is nullptr");
        return matchedMIMEs;
    }

    auto proxy = dataShareConnection_->GetDataShareProxy();
    if (proxy != nullptr) {
        matchedMIMEs = proxy->GetFileTypes(uri, mimeTypeFilter);
    }
    return matchedMIMEs;
}

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
int DataShareHelper::OpenFile(Uri &uri, const std::string &mode)
{
    LOG_DEBUG("Start");
    int fd = INVALID_VALUE;
    if (dataShareConnection_ == nullptr) {
        LOG_ERROR("dataShareConnection_ is nullptr");
        return fd;
    }

    if (!dataShareConnection_->ConnectDataShareExtAbility(uri_, token_)) {
        LOG_ERROR("dataShareProxy is nullptr");
        return fd;
    }

    auto proxy = dataShareConnection_->GetDataShareProxy();
    if (proxy != nullptr) {
        fd = proxy->OpenFile(uri, mode);
    }
    return fd;
}

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
int DataShareHelper::OpenRawFile(Uri &uri, const std::string &mode)
{
    LOG_DEBUG("Start");
    int fd = INVALID_VALUE;
    if (dataShareConnection_ == nullptr) {
        LOG_ERROR("dataShareConnection_ is nullptr");
        return fd;
    }

    if (!dataShareConnection_->ConnectDataShareExtAbility(uri_, token_)) {
        LOG_ERROR("dataShareProxy is nullptr");
        return fd;
    }

    auto proxy = dataShareConnection_->GetDataShareProxy();
    if (proxy != nullptr) {
        fd = proxy->OpenRawFile(uri, mode);
    }
    return fd;
}

/**
 * @brief Inserts a single data record into the database.
 *
 * @param uri Indicates the path of the data to operate.
 * @param value Indicates the data record to insert. If this parameter is null, a blank row will be inserted.
 *
 * @return Returns the index of the inserted data record.
 */
int DataShareHelper::Insert(Uri &uri, const DataShareValuesBucket &value)
{
    LOG_DEBUG("Start");
    int index = INVALID_VALUE;
    if (isDataShareService_) {
        LOG_DEBUG("DataShareService mode.");
        auto service = DataShareManager::GetDataShareService();
        if (!service) {
            LOG_DEBUG("DataShareService mode, but fail to get dataShareService.");
            return index;
        }
        return service->Insert(uri.ToString(), value);
    }

    if (dataShareConnection_ == nullptr) {
        LOG_ERROR("dataShareConnection_ is nullptr");
        return index;
    }

    if (!dataShareConnection_->ConnectDataShareExtAbility(uri_, token_)) {
        LOG_ERROR("dataShareProxy is nullptr");
        return index;
    }

    auto proxy = dataShareConnection_->GetDataShareProxy();
    if (proxy != nullptr) {
        index = proxy->Insert(uri, value);
    }
    return index;
}

/**
 * @brief Updates data records in the database.
 *
 * @param uri Indicates the path of data to update.
 * @param predicates Indicates filter criteria. You should define the processing logic when this parameter is null.
 * @param value Indicates the data to update. This parameter can be null.
 *
 * @return Returns the number of data records updated.
 */
int DataShareHelper::Update(Uri &uri, const DataSharePredicates &predicates, const DataShareValuesBucket &value)
{
    LOG_DEBUG("Start");
    int index = INVALID_VALUE;
    if (isDataShareService_) {
        LOG_DEBUG("DataShareService mode.");
        auto service = DataShareManager::GetDataShareService();
        if (!service) {
            LOG_DEBUG("DataShareService mode, but fail to get dataShareService.");
            return index;
        }
        return service->Update(uri.ToString(), predicates, value);
    }

    if (dataShareConnection_ == nullptr) {
        LOG_ERROR("dataShareConnection_ is nullptr");
        return index;
    }

    if (!dataShareConnection_->ConnectDataShareExtAbility(uri_, token_)) {
        LOG_ERROR("dataShareProxy is nullptr");
        return index;
    }

    auto proxy = dataShareConnection_->GetDataShareProxy();
    if (proxy != nullptr) {
        index = proxy->Update(uri, predicates, value);
    }
    return index;
}

/**
 * @brief Deletes one or more data records from the database.
 *
 * @param uri Indicates the path of the data to operate.
 * @param predicates Indicates filter criteria. You should define the processing logic when this parameter is null.
 *
 * @return Returns the number of data records deleted.
 */
int DataShareHelper::Delete(Uri &uri, const DataSharePredicates &predicates)
{
    LOG_DEBUG("Start");
    int index = INVALID_VALUE;
    if (isDataShareService_) {
        LOG_DEBUG("DataShareService mode.");
        auto service = DataShareManager::GetDataShareService();
        if (!service) {
            LOG_DEBUG("DataShareService mode, but fail to get dataShareService.");
            return index;
        }
        return service->Delete(uri.ToString(), predicates);
    }

    if (dataShareConnection_ == nullptr) {
        LOG_ERROR("dataShareConnection_ is nullptr");
        return index;
    }

    if (!dataShareConnection_->ConnectDataShareExtAbility(uri_, token_)) {
        LOG_ERROR("dataShareProxy is nullptr");
        return index;
    }

    auto proxy = dataShareConnection_->GetDataShareProxy();
    if (proxy != nullptr) {
        index = proxy->Delete(uri, predicates);
    }
    return index;
}

/**
 * @brief Deletes one or more data records from the database.
 *
 * @param uri Indicates the path of data to query.
 * @param predicates Indicates filter criteria. You should define the processing logic when this parameter is null.
 * @param columns Indicates the columns to query. If this parameter is null, all columns are queried.
 *
 * @return Returns the query result.
 */
std::shared_ptr<DataShareResultSet> DataShareHelper::Query(
    Uri &uri, const DataSharePredicates &predicates, std::vector<std::string> &columns)
{
    LOG_DEBUG("Start");
    std::shared_ptr<DataShareResultSet> resultset = nullptr;
    if (isDataShareService_) {
        LOG_DEBUG("DataShareService mode.");
        auto service = DataShareManager::GetDataShareService();
        if (!service) {
            LOG_DEBUG("DataShareService mode, but fail to get dataShareService.");
            return nullptr;
        }
        return service->Query(uri.ToString(), predicates, columns);
    }

    if (dataShareConnection_ == nullptr) {
        LOG_ERROR("dataShareConnection_ is nullptr");
        return resultset;
    }

    if (!dataShareConnection_->ConnectDataShareExtAbility(uri_, token_)) {
        LOG_ERROR("dataShareProxy is nullptr");
        return resultset;
    }

    auto proxy = dataShareConnection_->GetDataShareProxy();
    if (proxy != nullptr) {
        resultset = proxy->Query(uri, predicates, columns);
    }
    return resultset;
}

/**
 * @brief Obtains the MIME type matching the data specified by the URI of the data share. This method should be
 * implemented by a data share. Data abilities supports general data types, including text, HTML, and JPEG.
 *
 * @param uri Indicates the URI of the data.
 *
 * @return Returns the MIME type that matches the data specified by uri.
 */
std::string DataShareHelper::GetType(Uri &uri)
{
    LOG_DEBUG("Start");
    std::string type;

    if (dataShareConnection_ == nullptr) {
        LOG_ERROR("dataShareConnection_ is nullptr");
        return type;
    }

    if (!dataShareConnection_->ConnectDataShareExtAbility(uri_, token_)) {
        LOG_ERROR("dataShareProxy is nullptr");
        return type;
    }

    auto proxy = dataShareConnection_->GetDataShareProxy();
    if (proxy != nullptr) {
        type = proxy->GetType(uri);
    }
    return type;
}

/**
 * @brief Inserts multiple data records into the database.
 *
 * @param uri Indicates the path of the data to operate.
 * @param values Indicates the data records to insert.
 *
 * @return Returns the number of data records inserted.
 */
int DataShareHelper::BatchInsert(Uri &uri, const std::vector<DataShareValuesBucket> &values)
{
    LOG_DEBUG("Start");
    int ret = INVALID_VALUE;
    if (dataShareConnection_ == nullptr) {
        LOG_ERROR("dataShareConnection_ is nullptr");
        return ret;
    }

    if (!dataShareConnection_->ConnectDataShareExtAbility(uri_, token_)) {
        LOG_ERROR("dataShareProxy is nullptr");
        return ret;
    }

    auto proxy = dataShareConnection_->GetDataShareProxy();
    if (proxy != nullptr) {
        ret = proxy->BatchInsert(uri, values);
    }
    return ret;
}

/**
 * @brief Registers an observer to DataObsMgr specified by the given Uri.
 *
 * @param uri, Indicates the path of the data to operate.
 * @param dataObserver, Indicates the IDataAbilityObserver object.
 */
void DataShareHelper::RegisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver)
{
    LOG_INFO("Start");
    if (dataObserver == nullptr) {
        LOG_ERROR("dataObserver is nullptr");
        return;
    }
    if (isDataShareService_) {
        LOG_DEBUG("DataShareService mode.");
        if (!RegObserver(uri, dataObserver)) {
            LOG_ERROR("RegisterObserver failed");
        }
        return;
    }

    if (!dataShareConnection_->ConnectDataShareExtAbility(uri, token_)) {
        LOG_ERROR("connect failed");
        return;
    }

    auto proxy = dataShareConnection_->GetDataShareProxy();
    if (proxy == nullptr) {
        LOG_ERROR("proxy has disconnected");
        return;
    }
    proxy->RegisterObserver(uri, dataObserver);
}

/**
 * @brief Deregisters an observer used for DataObsMgr specified by the given Uri.
 *
 * @param uri, Indicates the path of the data to operate.
 * @param dataObserver, Indicates the IDataAbilityObserver object.
 */
void DataShareHelper::UnregisterObserver(const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver)
{
    LOG_INFO("Start");
    if (dataObserver == nullptr) {
        LOG_ERROR("dataObserver is nullptr");
        return;
    }

    if (isDataShareService_) {
        LOG_DEBUG("DataShareService mode.");
        if (!UnregObserver(uri, dataObserver)) {
            LOG_ERROR("UnregisterObserver failed");
        }
        return;
    }

    auto proxy = dataShareConnection_->GetDataShareProxy();
    if (proxy == nullptr) {
        LOG_ERROR("dataShareConnection_->GetDataShareProxy() is nullptr");
        return;
    }
    proxy->UnregisterObserver(uri, dataObserver);
}

/**
 * @brief Notifies the registered observers of a change to the data resource specified by Uri.
 *
 * @param uri, Indicates the path of the data to operate.
 */
void DataShareHelper::NotifyChange(const Uri &uri)
{
    LOG_DEBUG("Start");

    if (dataShareConnection_ == nullptr) {
        LOG_ERROR("dataShareConnection_ is nullptr");
        return;
    }

    if (!dataShareConnection_->ConnectDataShareExtAbility(uri_, token_)) {
        LOG_ERROR("dataShareProxy is nullptr");
        return;
    }

    auto proxy = dataShareConnection_->GetDataShareProxy();
    if (proxy != nullptr) {
        proxy->NotifyChange(uri);
    }
}

/**
 * @brief Converts the given uri that refer to the data share into a normalized URI. A normalized URI can be used
 * across devices, persisted, backed up, and restored. It can refer to the same item in the data share even if the
 * context has changed. If you implement URI normalization for a data share, you must also implement
 * denormalizeUri(ohos.utils.net.Uri) to enable URI denormalization. After this feature is enabled, URIs passed to any
 * method that is called on the data share must require normalization verification and denormalization. The default
 * implementation of this method returns null, indicating that this data share does not support URI normalization.
 *
 * @param uri Indicates the Uri object to normalize.
 *
 * @return Returns the normalized Uri object if the data share supports URI normalization; returns null otherwise.
 */
Uri DataShareHelper::NormalizeUri(Uri &uri)
{
    LOG_DEBUG("Start");
    Uri urivalue("");
    if (dataShareConnection_ == nullptr) {
        LOG_ERROR("dataShareConnection_ is nullptr");
        return urivalue;
    }

    if (!dataShareConnection_->ConnectDataShareExtAbility(uri_, token_)) {
        LOG_ERROR("dataShareProxy is nullptr");
        return urivalue;
    }

    auto proxy = dataShareConnection_->GetDataShareProxy();
    if (proxy != nullptr) {
        urivalue = proxy->NormalizeUri(uri);
    }
    return urivalue;
}

/**
 * @brief Converts the given normalized uri generated by normalizeUri(ohos.utils.net.Uri) into a denormalized one.
 * The default implementation of this method returns the original URI passed to it.
 *
 * @param uri uri Indicates the Uri object to denormalize.
 *
 * @return Returns the denormalized Uri object if the denormalization is successful; returns the original Uri passed to
 * this method if there is nothing to do; returns null if the data identified by the original Uri cannot be found in
 * the current environment.
 */
Uri DataShareHelper::DenormalizeUri(Uri &uri)
{
    LOG_DEBUG("Start");
    Uri urivalue("");
    if (dataShareConnection_ == nullptr) {
        LOG_ERROR("dataShareConnection_ is nullptr");
        return urivalue;
    }

    if (!dataShareConnection_->ConnectDataShareExtAbility(uri_, token_)) {
        LOG_ERROR("dataShareProxy is nullptr");
        return urivalue;
    }

    auto proxy = dataShareConnection_->GetDataShareProxy();
    if (proxy != nullptr) {
        urivalue = proxy->DenormalizeUri(uri);
    }
    return urivalue;
}

bool DataShareHelper::RegObserver (const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver)
{
    auto obsMgrClient = OHOS::AAFwk::DataObsMgrClient::GetInstance();
    if (obsMgrClient == nullptr) {
        LOG_ERROR("get DataObsMgrClient failed");
        return false;
    }
    ErrCode ret = obsMgrClient->RegisterObserver(uri, dataObserver);
    if (ret != ERR_OK) {
        return false;
    }
    return true;
}
bool DataShareHelper::UnregObserver (const Uri &uri, const sptr<AAFwk::IDataAbilityObserver> &dataObserver)
{
    auto obsMgrClient = OHOS::AAFwk::DataObsMgrClient::GetInstance();
    if (obsMgrClient == nullptr) {
        LOG_ERROR("get DataObsMgrClient failed");
        return false;
    }
    ErrCode ret = obsMgrClient->UnregisterObserver(uri, dataObserver);
    if (ret != ERR_OK) {
        LOG_ERROR("UnregisterObserver failed");
        return false;
    }
    return true;
}
}  // namespace DataShare
}  // namespace OHOS