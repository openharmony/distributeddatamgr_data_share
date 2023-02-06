/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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
#define MLOG_TAG "DataShareUnitTest"

#include "mediadatashare_unit_test.h"

#include "datashare_helper.h"
#include "fetch_result.h"
#include "get_self_permissions.h"
#include "iservice_registry.h"
#include "datashare_log.h"
#include "media_file_utils.h"
#include "media_library_manager.h"
#include "medialibrary_errno.h"
#include "system_ability_definition.h"

using namespace std;
using namespace testing::ext;

namespace OHOS {
namespace Media {
constexpr int STORAGE_MANAGER_MANAGER_ID = 5003;
std::string MEDIALIBRARY_DATA_URI_ERROR = "test:///media";
std::shared_ptr<DataShare::DataShareHelper> g_mediaDataShareHelper;

std::shared_ptr<DataShare::DataShareHelper> CreateDataShareHelper(int32_t systemAbilityId)
{
    LOG_INFO("CreateDataShareHelper::CreateFileExtHelper ");
    auto saManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (saManager == nullptr) {
        LOG_ERROR("CreateDataShareHelper::CreateFileExtHelper Get system ability "
                 "mgr failed.");
        return nullptr;
    }
    auto remoteObj = saManager->GetSystemAbility(systemAbilityId);
    while (remoteObj == nullptr) {
        LOG_ERROR("CreateDataShareHelper::CreateFileExtHelper GetSystemAbility "
                 "Service Failed.");
        return nullptr;
    }
    return DataShare::DataShareHelper::Creator(remoteObj, MEDIALIBRARY_DATA_URI);
}

void MediaDataShareUnitTest::SetUpTestCase(void)
{
    vector<string> perms;
    perms.push_back("ohos.permission.READ_MEDIA");
    perms.push_back("ohos.permission.WRITE_MEDIA");
    perms.push_back("ohos.permission.FILE_ACCESS_MANAGER");
    uint64_t tokenId = 0;
    PermissionUtilsUnitTest::SetAccessTokenPermission("MediaDataShareUnitTest", perms, tokenId);
    ASSERT_TRUE(tokenId != 0);

    LOG_INFO("SetUpTestCase invoked");
    g_mediaDataShareHelper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID);
    ASSERT_TRUE(g_mediaDataShareHelper != nullptr);

    Uri uri(MEDIALIBRARY_DATA_URI);
    DataShare::DataSharePredicates predicates;
    string selections = MEDIA_DATA_DB_ID + " <> 0 ";
    predicates.SetWhereClause(selections);
    int retVal = g_mediaDataShareHelper->Delete(uri, predicates);
    LOG_INFO("SetUpTestCase Delete retVal: %{public}d", retVal);
    EXPECT_EQ((retVal >= 0), true);

    DataShare::DataShareValuesBucket valuesBucket;
    double valueD1 = 20.07;
    valuesBucket.Put(MEDIA_DATA_DB_LONGITUDE, valueD1);
    valuesBucket.Put(MEDIA_DATA_DB_TITLE, "dataShareTest003");
    int value1 = 1001;
    valuesBucket.Put(MEDIA_DATA_DB_PARENT_ID, value1);
    retVal = g_mediaDataShareHelper->Insert(uri, valuesBucket);
    EXPECT_EQ((retVal > 0), true);

    valuesBucket.Clear();
    double valueD2 = 20.08;
    valuesBucket.Put(MEDIA_DATA_DB_LONGITUDE, valueD2);
    valuesBucket.Put(MEDIA_DATA_DB_TITLE, "dataShareTest004");
    int value2 = 1000;
    valuesBucket.Put(MEDIA_DATA_DB_PARENT_ID, value2);
    retVal = g_mediaDataShareHelper->Insert(uri, valuesBucket);
    EXPECT_EQ((retVal > 0), true);

    valuesBucket.Clear();
    double valueD3 = 20.09;
    valuesBucket.Put(MEDIA_DATA_DB_LONGITUDE, valueD3);
    valuesBucket.Put(MEDIA_DATA_DB_TITLE, "dataShareTest005");
    int value3 = 999;
    valuesBucket.Put(MEDIA_DATA_DB_PARENT_ID, value3);
    retVal = g_mediaDataShareHelper->Insert(uri, valuesBucket);
    EXPECT_EQ((retVal > 0), true);
    LOG_INFO("SetUpTestCase end");
}

int32_t CreateFile(string displayName)
{
    LOG_INFO("CreateFile::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_mediaDataShareHelper;
    Uri createAssetUri(MEDIALIBRARY_DATA_URI + "/" + Media::MEDIA_FILEOPRN + "/" + Media::MEDIA_FILEOPRN_CREATEASSET);
    DataShare::DataShareValuesBucket valuesBucket;
    string relativePath = "Pictures/" + displayName + "/";
    displayName += ".jpg";
    MediaType mediaType = MEDIA_TYPE_IMAGE;
    valuesBucket.Put(MEDIA_DATA_DB_MEDIA_TYPE, mediaType);
    valuesBucket.Put(MEDIA_DATA_DB_NAME, displayName);
    valuesBucket.Put(MEDIA_DATA_DB_RELATIVE_PATH, relativePath);
    int32_t retVal = helper->Insert(createAssetUri, valuesBucket);
    LOG_INFO("CreateFile::File: %{public}s, retVal: %{public}d", (relativePath + displayName).c_str(), retVal);
    EXPECT_EQ((retVal > 0), true);
    if (retVal <= 0) {
        retVal = E_FAIL;
    }
    LOG_INFO("CreateFile::retVal = %{public}d. End", retVal);
    return retVal;
}

int32_t CreateAlbum(string displayName)
{
    LOG_INFO("CreateAlbum::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_mediaDataShareHelper;
    Uri createAlbumUri(MEDIALIBRARY_DATA_URI + "/" + MEDIA_ALBUMOPRN + "/" + MEDIA_ALBUMOPRN_CREATEALBUM);
    DataShare::DataShareValuesBucket valuesBucket;
    string dirPath = ROOT_MEDIA_DIR + "Pictures/" + displayName;
    valuesBucket.Put(MEDIA_DATA_DB_FILE_PATH, dirPath);
    valuesBucket.Put(MEDIA_DATA_DB_NAME, displayName);
    auto retVal = helper->Insert(createAlbumUri, valuesBucket);
    LOG_INFO("CreateAlbum::Album: %{public}s, retVal: %{public}d", dirPath.c_str(), retVal);
    EXPECT_EQ((retVal > 0), true);
    if (retVal <= 0) {
        retVal = E_FAIL;
    }
    LOG_INFO("CreateAlbum::retVal = %{public}d. End", retVal);
    return retVal;
}

bool GetFileAsset(unique_ptr<FileAsset> &fileAsset, bool isAlbum, string displayName)
{
    int32_t index = E_FAIL;
    if (isAlbum) {
        index = CreateAlbum(displayName);
    } else {
        index = CreateFile(displayName);
    }
    if (index == E_FAIL) {
        LOG_ERROR("GetFileAsset failed");
        return false;
    }
    std::shared_ptr<DataShare::DataShareHelper> helper = g_mediaDataShareHelper;
    vector<string> columns;
    DataShare::DataSharePredicates predicates;
    string selections = MEDIA_DATA_DB_ID + " = " + to_string(index);
    predicates.SetWhereClause(selections);
    Uri queryFileUri(MEDIALIBRARY_DATA_URI);
    auto resultSet = helper->Query(queryFileUri, predicates, columns);
    if (resultSet == nullptr) {
        LOG_ERROR("GetFileAsset::resultSet == nullptr");
        return false;
    }

    // Create FetchResult object using the contents of resultSet
    unique_ptr<FetchResult<FileAsset>> fetchFileResult = make_unique<FetchResult<FileAsset>>(move(resultSet));
    if (fetchFileResult->GetCount() <= 0) {
        LOG_ERROR("GetFileAsset::GetCount <= 0");
        return false;
    }

    fileAsset = fetchFileResult->GetFirstObject();
    if (fileAsset == nullptr) {
        LOG_ERROR("GetFileAsset::fileAsset = nullptr.");
        return false;
    }
    return true;
}

void MediaDataShareUnitTest::TearDownTestCase(void)
{
    LOG_INFO("TearDownTestCase invoked");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_mediaDataShareHelper;
    Uri deleteAssetUri(MEDIALIBRARY_DATA_URI);
    DataShare::DataSharePredicates predicates;
    string selections = MEDIA_DATA_DB_ID + " <> 0 ";
    predicates.SetWhereClause(selections);
    int retVal = helper->Delete(deleteAssetUri, predicates);
    LOG_INFO("TearDownTestCase Delete retVal: %{public}d", retVal);
    EXPECT_EQ((retVal >= 0), true);

    bool result = helper->Release();
    EXPECT_EQ(result, true);
    LOG_INFO("TearDownTestCase end");
}

void MediaDataShareUnitTest::SetUp(void) {}
void MediaDataShareUnitTest::TearDown(void) {}

HWTEST_F(MediaDataShareUnitTest, MediaDataShare_Predicates_Test_001, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_Predicates_Test_001::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_mediaDataShareHelper;
    Uri uri(MEDIALIBRARY_DATA_URI);
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(MEDIA_DATA_DB_TITLE, "dataShareTest003");
    predicates.Limit(1, 0);
    vector<string> columns;
    auto resultSet = helper->Query(uri, predicates, columns);
    int result = 0;
    if (resultSet != nullptr) {
        resultSet->GetRowCount(result);
    }
    EXPECT_EQ(result, 1);
    LOG_INFO("MediaDataShare_Predicates_Test_001, End");
}

HWTEST_F(MediaDataShareUnitTest, MediaDataShare_Predicates_Test_002, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_Predicates_Test_002::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_mediaDataShareHelper;
    DataShare::DataSharePredicates predicates;
    predicates.NotEqualTo(MEDIA_DATA_DB_TITLE, "dataShareTest003");
    vector<string> columns;
    Uri uri(MEDIALIBRARY_DATA_URI);
    auto resultSet = helper->Query(uri, predicates, columns);
    int result = 0;
    if (resultSet != nullptr) {
        resultSet->GetRowCount(result);
    }
    EXPECT_EQ(result, 2);
    LOG_INFO("MediaDataShare_Predicates_Test_002, End");
}

HWTEST_F(MediaDataShareUnitTest, MediaDataShare_Predicates_Test_003, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_Predicates_Test_003::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_mediaDataShareHelper;
    DataShare::DataSharePredicates predicates;
    predicates.Contains(MEDIA_DATA_DB_TITLE, "dataShareTest");
    vector<string> columns;
    Uri uri(MEDIALIBRARY_DATA_URI);
    auto resultSet = helper->Query(uri, predicates, columns);
    int result = 0;
    if (resultSet != nullptr) {
        resultSet->GetRowCount(result);
    }
    EXPECT_EQ(result, 3);
    LOG_INFO("MediaDataShare_Predicates_Test_003, End");
}

HWTEST_F(MediaDataShareUnitTest, MediaDataShare_Predicates_Test_004, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_Predicates_Test_004::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_mediaDataShareHelper;
    DataShare::DataSharePredicates predicates;
    predicates.BeginsWith(MEDIA_DATA_DB_TITLE, "dataShare");
    vector<string> columns;
    Uri uri(MEDIALIBRARY_DATA_URI);
    auto resultSet = helper->Query(uri, predicates, columns);
    int result = 0;
    if (resultSet != nullptr) {
        resultSet->GetRowCount(result);
    }
    EXPECT_EQ(result, 3);
    LOG_INFO("MediaDataShare_Predicates_Test_004, End");
}

HWTEST_F(MediaDataShareUnitTest, MediaDataShare_Predicates_Test_005, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_Predicates_Test_005::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_mediaDataShareHelper;
    DataShare::DataSharePredicates predicates;
    predicates.EndsWith(MEDIA_DATA_DB_TITLE, "003");
    vector<string> columns;
    Uri uri(MEDIALIBRARY_DATA_URI);
    auto resultSet = helper->Query(uri, predicates, columns);
    int result = 0;
    if (resultSet != nullptr) {
        resultSet->GetRowCount(result);
    }
    EXPECT_EQ(result, 1);
    LOG_INFO("MediaDataShare_Predicates_Test_005, End");
}

HWTEST_F(MediaDataShareUnitTest, MediaDataShare_Predicates_Test_006, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_Predicates_Test_006::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_mediaDataShareHelper;
    DataShare::DataSharePredicates predicates;
    predicates.IsNull(MEDIA_DATA_DB_TITLE);
    vector<string> columns;
    Uri uri(MEDIALIBRARY_DATA_URI);
    auto resultSet = helper->Query(uri, predicates, columns);
    int result = 0;
    if (resultSet != nullptr) {
        resultSet->GetRowCount(result);
    }
    EXPECT_EQ(result, 0);
    LOG_INFO("MediaDataShare_Predicates_Test_006, End");
}

HWTEST_F(MediaDataShareUnitTest, MediaDataShare_Predicates_Test_007, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_Predicates_Test_007::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_mediaDataShareHelper;
    DataShare::DataSharePredicates predicates;
    predicates.IsNotNull(MEDIA_DATA_DB_TITLE);
    vector<string> columns;
    Uri uri(MEDIALIBRARY_DATA_URI);
    auto resultSet = helper->Query(uri, predicates, columns);
    int result = 0;
    if (resultSet != nullptr) {
        resultSet->GetRowCount(result);
    }
    EXPECT_EQ(result, 3);
    LOG_INFO("MediaDataShare_Predicates_Test_007, End");
}

HWTEST_F(MediaDataShareUnitTest, MediaDataShare_Predicates_Test_008, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_Predicates_Test_008::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_mediaDataShareHelper;
    DataShare::DataSharePredicates predicates;
    predicates.Like(MEDIA_DATA_DB_TITLE, "%Test003");
    vector<string> columns;
    Uri uri(MEDIALIBRARY_DATA_URI);
    auto resultSet = helper->Query(uri, predicates, columns);
    int result = 0;
    if (resultSet != nullptr) {
        resultSet->GetRowCount(result);
    }
    EXPECT_EQ(result, 1);
    LOG_INFO("MediaDataShare_Predicates_Test_008, End");
}

HWTEST_F(MediaDataShareUnitTest, MediaDataShare_Predicates_Test_009, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_Predicates_Test_009::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_mediaDataShareHelper;
    DataShare::DataSharePredicates predicates;
    predicates.Glob(MEDIA_DATA_DB_TITLE, "dataShareTes?003");
    vector<string> columns;
    Uri uri(MEDIALIBRARY_DATA_URI);
    auto resultSet = helper->Query(uri, predicates, columns);
    int result = 0;
    if (resultSet != nullptr) {
        resultSet->GetRowCount(result);
    }
    EXPECT_EQ(result, 1);
    LOG_INFO("MediaDataShare_Predicates_Test_009, End");
}

HWTEST_F(MediaDataShareUnitTest, MediaDataShare_Predicates_Test_010, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_Predicates_Test_010::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_mediaDataShareHelper;
    DataShare::DataSharePredicates predicates;
    predicates.Between(MEDIA_DATA_DB_PARENT_ID, "0", "999");
    vector<string> columns;
    Uri uri(MEDIALIBRARY_DATA_URI);
    auto resultSet = helper->Query(uri, predicates, columns);
    int result = 0;
    if (resultSet != nullptr) {
        resultSet->GetRowCount(result);
    }
    EXPECT_EQ(result, 1);
    LOG_INFO("MediaDataShare_Predicates_Test_010, End");
}

HWTEST_F(MediaDataShareUnitTest, MediaDataShare_Predicates_Test_011, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_Predicates_Test_011::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_mediaDataShareHelper;
    DataShare::DataSharePredicates predicates;
    predicates.NotBetween(MEDIA_DATA_DB_PARENT_ID, "0", "999");
    vector<string> columns;
    Uri uri(MEDIALIBRARY_DATA_URI);
    auto resultSet = helper->Query(uri, predicates, columns);
    int result = 0;
    if (resultSet != nullptr) {
        resultSet->GetRowCount(result);
    }
    EXPECT_EQ(result, 2);
    LOG_INFO("MediaDataShare_Predicates_Test_011, End");
}

HWTEST_F(MediaDataShareUnitTest, MediaDataShare_Predicates_Test_012, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_Predicates_Test_012::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_mediaDataShareHelper;
    DataShare::DataSharePredicates predicates;
    predicates.GreaterThan(MEDIA_DATA_DB_PARENT_ID, 999);
    vector<string> columns;
    Uri uri(MEDIALIBRARY_DATA_URI);
    auto resultSet = helper->Query(uri, predicates, columns);
    int result = 0;
    if (resultSet != nullptr) {
        resultSet->GetRowCount(result);
    }
    EXPECT_EQ(result, 2);
    LOG_INFO("MediaDataShare_Predicates_Test_012, End");
}

HWTEST_F(MediaDataShareUnitTest, MediaDataShare_Predicates_Test_013, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_Predicates_Test_013::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_mediaDataShareHelper;
    DataShare::DataSharePredicates predicates;
    predicates.LessThan(MEDIA_DATA_DB_PARENT_ID, 1000);
    vector<string> columns;
    Uri uri(MEDIALIBRARY_DATA_URI);
    auto resultSet = helper->Query(uri, predicates, columns);
    int result = 0;
    if (resultSet != nullptr) {
        resultSet->GetRowCount(result);
    }
    EXPECT_EQ(result, 1);
    LOG_INFO("MediaDataShare_Predicates_Test_013, End");
}

HWTEST_F(MediaDataShareUnitTest, MediaDataShare_Predicates_Test_014, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_Predicates_Test_014::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_mediaDataShareHelper;
    DataShare::DataSharePredicates predicates;
    predicates.GreaterThanOrEqualTo(MEDIA_DATA_DB_PARENT_ID, 1000);
    vector<string> columns;
    Uri uri(MEDIALIBRARY_DATA_URI);
    auto resultSet = helper->Query(uri, predicates, columns);
    int result = 0;
    if (resultSet != nullptr) {
        resultSet->GetRowCount(result);
    }
    EXPECT_EQ(result, 2);
    LOG_INFO("MediaDataShare_Predicates_Test_014, End");
}

HWTEST_F(MediaDataShareUnitTest, MediaDataShare_Predicates_Test_015, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_Predicates_Test_015::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_mediaDataShareHelper;
    DataShare::DataSharePredicates predicates;
    predicates.LessThanOrEqualTo(MEDIA_DATA_DB_PARENT_ID, 1000);
    vector<string> columns;
    Uri uri(MEDIALIBRARY_DATA_URI);
    auto resultSet = helper->Query(uri, predicates, columns);
    int result = 0;
    if (resultSet != nullptr) {
        resultSet->GetRowCount(result);
    }
    EXPECT_EQ(result, 2);
    LOG_INFO("MediaDataShare_Predicates_Test_015, End");
}

HWTEST_F(MediaDataShareUnitTest, MediaDataShare_Predicates_Test_016, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_Predicates_Test_016::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_mediaDataShareHelper;
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(MEDIA_DATA_DB_LONGITUDE, 20.08)
        ->BeginWrap()
        ->EqualTo(MEDIA_DATA_DB_TITLE, "dataShareTest004")
        ->Or()
        ->EqualTo(MEDIA_DATA_DB_PARENT_ID, 1000)
        ->EndWrap();
    vector<string> columns;
    Uri uri(MEDIALIBRARY_DATA_URI);
    auto resultSet = helper->Query(uri, predicates, columns);
    int result = 0;
    if (resultSet != nullptr) {
        resultSet->GetRowCount(result);
    }
    EXPECT_EQ(result, 1);
    LOG_INFO("MediaDataShare_Predicates_Test_016, End");
}

HWTEST_F(MediaDataShareUnitTest, MediaDataShare_Predicates_Test_017, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_Predicates_Test_017::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_mediaDataShareHelper;
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(MEDIA_DATA_DB_LONGITUDE, 20.08)->And()->EqualTo(MEDIA_DATA_DB_TITLE, "dataShareTest004");
    vector<string> columns;
    Uri uri(MEDIALIBRARY_DATA_URI);
    auto resultSet = helper->Query(uri, predicates, columns);
    int result = 0;
    if (resultSet != nullptr) {
        resultSet->GetRowCount(result);
    }
    EXPECT_EQ(result, 1);
    LOG_INFO("MediaDataShare_Predicates_Test_017, End");
}

HWTEST_F(MediaDataShareUnitTest, MediaDataShare_Predicates_Test_018, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_Predicates_Test_018::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_mediaDataShareHelper;
    DataShare::DataSharePredicates predicates;
    predicates.OrderByAsc(MEDIA_DATA_DB_PARENT_ID);
    vector<string> columns;
    Uri uri(MEDIALIBRARY_DATA_URI);
    auto resultSet = helper->Query(uri, predicates, columns);
    int columnIndex = 0;
    std::string stringResult = "";
    if (resultSet != nullptr) {
        resultSet->GoToFirstRow();
        resultSet->GetColumnIndex(MEDIA_DATA_DB_TITLE, columnIndex);
        resultSet->GetString(columnIndex, stringResult);
    }
    EXPECT_EQ(stringResult, "dataShareTest005");
    LOG_INFO("MediaDataShare_Predicates_Test_018, End");
}

HWTEST_F(MediaDataShareUnitTest, MediaDataShare_Predicates_Test_019, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_Predicates_Test_019::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_mediaDataShareHelper;
    DataShare::DataSharePredicates predicates;
    predicates.OrderByDesc(MEDIA_DATA_DB_LONGITUDE);
    vector<string> columns;
    Uri uri(MEDIALIBRARY_DATA_URI);
    auto resultSet = helper->Query(uri, predicates, columns);
    int columnIndex = 0;
    std::string stringResult = "";
    if (resultSet != nullptr) {
        resultSet->GoToFirstRow();
        resultSet->GetColumnIndex(MEDIA_DATA_DB_TITLE, columnIndex);
        resultSet->GetString(columnIndex, stringResult);
    }
    EXPECT_EQ(stringResult, "dataShareTest005");
    LOG_INFO("MediaDataShare_Predicates_Test_019, End");
}

HWTEST_F(MediaDataShareUnitTest, MediaDataShare_Predicates_Test_020, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_Predicates_Test_020::Start");
    DataShare::DataSharePredicates predicates;
    predicates.SetSettingMode(DataShare::SettingMode::PREDICATES_METHOD);
    DataShare::SettingMode setting = predicates.GetSettingMode();
    EXPECT_EQ(setting, DataShare::SettingMode::PREDICATES_METHOD);
    LOG_INFO("MediaDataShare_Predicates_Test_020, End");
}

HWTEST_F(MediaDataShareUnitTest, MediaDataShare_Predicates_Test_021, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_Predicates_Test_021::Start");
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(MEDIA_DATA_DB_TITLE, "dataShareTest003");

    std::list<DataShare::OperationItem> operationItems = predicates.GetOperationList();
    DataShare::OperationItem operationItem = operationItems.front();
    EXPECT_EQ(operationItem.operation, DataShare::OperationType::EQUAL_TO);
    string param1 = operationItem.singleParams[0];
    string param2 = operationItem.singleParams[1];
    EXPECT_EQ(param1, MEDIA_DATA_DB_TITLE);
    EXPECT_EQ(param2, "dataShareTest003");
    LOG_INFO("MediaDataShare_Predicates_Test_021, End");
}

HWTEST_F(MediaDataShareUnitTest, MediaDataShare_Predicates_Test_022, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_Predicates_Test_022::Start");
    DataShare::DataSharePredicates predicates;
    string selections = MEDIA_DATA_DB_ID + " <> 0 ";
    predicates.SetWhereClause(selections);
    string clause = predicates.GetWhereClause();
    EXPECT_EQ(selections, clause);
    LOG_INFO("MediaDataShare_Predicates_Test_022, End");
}

HWTEST_F(MediaDataShareUnitTest, MediaDataShare_Predicates_Test_023, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_Predicates_Test_023::Start");
    DataShare::DataSharePredicates predicates;
    int res = predicates.SetWhereClause("`data2` > ?");
    EXPECT_EQ(res, 0);
    res = predicates.SetWhereArgs(std::vector<std::string> { "-5" });
    EXPECT_EQ(res, 0);
    res = predicates.SetOrder("data3");
    EXPECT_EQ(res, 0);

    string clause = predicates.GetWhereClause();
    EXPECT_EQ(clause, "`data2` > ?");
    vector<string> args = predicates.GetWhereArgs();
    EXPECT_EQ(args[0], "-5");
    string order = predicates.GetOrder();
    EXPECT_EQ(order, "data3");
    LOG_INFO("MediaDataShare_Predicates_Test_023, End");
}

HWTEST_F(MediaDataShareUnitTest, MediaDataShare_ValuesBucket_Test_001, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_ValuesBucket_Test_001::Start");
    DataShare::DataShareValuesBucket valuesBucket;
    EXPECT_EQ(valuesBucket.IsEmpty(), true);

    valuesBucket.Put(MEDIA_DATA_DB_TITLE, "dataShare_Test_001");
    EXPECT_EQ(valuesBucket.IsEmpty(), false);

    bool isValid;
    DataShare::DataShareValueObject object = valuesBucket.Get(MEDIA_DATA_DB_TITLE, isValid);
    string value = object;
    EXPECT_EQ(value, "dataShare_Test_001");

    valuesBucket.Clear();
    EXPECT_EQ(valuesBucket.IsEmpty(), true);
    LOG_INFO("MediaDataShare_ValuesBucket_Test_001 End");
}

HWTEST_F(MediaDataShareUnitTest, MediaDataShare_ValueObject_Test_001, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_ValueObject_Test_001::Start");

    int base = 100;
    DataShare::DataShareValueObject object(base);
    int value = object;
    EXPECT_EQ(value, base);

    DataShare::DataShareValueObjectType type = object.GetType();
    EXPECT_EQ(type, DataShare::DataShareValueObjectType::TYPE_INT);

    int64_t base64 = 100;
    DataShare::DataShareValueObject object64(base64);
    int64_t value64 = object64;
    EXPECT_EQ(value64, base64);

    double baseD = 10.0;
    DataShare::DataShareValueObject objectD(baseD);
    double valueD = objectD;
    EXPECT_EQ(valueD, baseD);

    bool baseB = true;
    DataShare::DataShareValueObject objectB(baseB);
    bool valueB = objectB;
    EXPECT_EQ(valueB, baseB);

    vector<uint8_t> baseV;
    DataShare::DataShareValueObject objectV(baseV);
    vector<uint8_t> valueV = objectV;
    EXPECT_EQ(valueV, baseV);

    DataShare::DataShareValueObject objectCopy(object);
    int valueCopy = objectCopy;
    EXPECT_EQ(valueCopy, value);

    DataShare::DataShareValueObject objectMove(std::move(object));
    int valueMove = objectMove;
    EXPECT_EQ(valueMove, value);

    LOG_INFO("MediaDataShare_ValueObject_Test_001 End");
}

HWTEST_F(MediaDataShareUnitTest, MediaDataShare_GetFileTypes_Test_001, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_GetFileTypes_Test_001::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_mediaDataShareHelper;
    Uri uri(MEDIALIBRARY_DATA_URI);
    std::string mimeTypeFilter("mimeTypeFiltertest");
    std::vector<std::string> result = helper->GetFileTypes(uri, mimeTypeFilter);
    EXPECT_EQ(result.size(), 0);
    LOG_INFO("MediaDataShare_GetFileTypes_Test_001 End");
}

HWTEST_F(MediaDataShareUnitTest, MediaDataShare_OpenRawFile_Test_001, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_OpenRawFile_Test_001::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_mediaDataShareHelper;
    Uri uri(MEDIALIBRARY_DATA_URI);
    std::string mode("modetest");
    int result = helper->OpenRawFile(uri, mode);
    EXPECT_EQ(result, 0);
    LOG_INFO("MediaDataShare_OpenRawFile_Test_001 End");
}

HWTEST_F(MediaDataShareUnitTest, MediaDataShare_NormalizeUri_Test_001, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_NormalizeUri_Test_001::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_mediaDataShareHelper;
    Uri uri(MEDIALIBRARY_DATA_URI);
    auto normalUri = helper->NormalizeUri(uri);
    EXPECT_EQ(normalUri, uri);
    LOG_INFO("MediaDataShare_NormalizeUri_Test_001 End");
}

HWTEST_F(MediaDataShareUnitTest, MediaDataShare_DenormalizeUri_Test_001, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_DenormalizeUri_Test_001::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_mediaDataShareHelper;
    Uri uri(MEDIALIBRARY_DATA_URI);
    auto denormalUri = helper->DenormalizeUri(uri);
    EXPECT_EQ(denormalUri, uri);
    LOG_INFO("MediaDataShare_DenormalizeUri_Test_001 End");
}

HWTEST_F(MediaDataShareUnitTest, MediaDataShare_GetType_Test_001, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_GetType_Test_001::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_mediaDataShareHelper;
    Uri uri(MEDIALIBRARY_DATA_URI);
    std::string result = helper->GetType(uri);
    EXPECT_NE(result.c_str(), "");
    LOG_INFO("MediaDataShare_GetType_Test_001 End");
}

HWTEST_F(MediaDataShareUnitTest, MediaDataShare_PredicatesObject_Test_001, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_PredicatesObject_Test_001::Start");

    int base = 100;
    DataShare::DataSharePredicatesObject po(base);
    int value = po;
    EXPECT_EQ(value, base);

    int64_t base64 = 100;
    DataShare::DataSharePredicatesObject po64(base64);
    int64_t value64 = po64;
    EXPECT_EQ(value64, base64);

    double baseD = 10.0;
    DataShare::DataSharePredicatesObject poD(baseD);
    double valueD = poD;
    EXPECT_EQ(valueD, baseD);

    bool baseB = true;
    DataShare::DataSharePredicatesObject poB(baseB);
    bool valueB = poB;
    EXPECT_EQ(valueB, baseB);

    string baseS = "dataShare_Test_001";
    DataShare::DataSharePredicatesObject poS(baseS);
    string valueS = poS;
    EXPECT_EQ(valueS, baseS);

    DataShare::DataSharePredicatesObject poCopy(po);
    int valueCopy = poCopy;
    EXPECT_EQ(valueCopy, value);

    DataShare::DataSharePredicatesObject poMove(std::move(po));
    int valueMove = poMove;
    EXPECT_EQ(valueMove, value);

    LOG_INFO("MediaDataShare_PredicatesObject_Test_001 End");
}

HWTEST_F(MediaDataShareUnitTest, MediaDataShare_ResultSet_Test_001, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_ResultSet_Test_001::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_mediaDataShareHelper;

    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(MEDIA_DATA_DB_TITLE, "dataShareTest003");
    vector<string> columns;
    Uri uri(MEDIALIBRARY_DATA_URI);
    auto resultSet = helper->Query(uri, predicates, columns);
    int columnIndex = 0;
    double result;
    bool isNull;
    if (resultSet != nullptr) {
        resultSet->GoToFirstRow();
        int err = resultSet->GoToRow(0);
        EXPECT_EQ(err, 0);
        resultSet->GetColumnIndex(MEDIA_DATA_DB_LONGITUDE, columnIndex);
        err = resultSet->IsColumnNull(columnIndex, isNull);
        EXPECT_EQ(err, 0);
        EXPECT_EQ(isNull, false);

        DataShare::DataType type;
        err = resultSet->GetDataType(columnIndex, type);
        EXPECT_EQ(err, 0);
        EXPECT_EQ(type, DataShare::DataType::TYPE_FLOAT);

        err = resultSet->GetDouble(columnIndex, result);
        EXPECT_EQ(err, 0);
    }
    double valueD1 = 20.07;
    EXPECT_EQ(result, valueD1);
    LOG_INFO("MediaDataShare_ResultSet_Test_001, End");
}

HWTEST_F(MediaDataShareUnitTest, MediaDataShare_ResultSet_Test_002, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_ResultSet_Test_002::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_mediaDataShareHelper;
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(MEDIA_DATA_DB_TITLE, "dataShareTest003");
    vector<string> columns;
    Uri uri(MEDIALIBRARY_DATA_URI);
    auto resultSet = helper->Query(uri, predicates, columns);
    int columnIndex = 0;
    vector<uint8_t> blob;
    vector<string> columnNames;
    if (resultSet != nullptr) {
        resultSet->GoToFirstRow();
        resultSet->GetColumnIndex(MEDIA_DATA_DB_TITLE, columnIndex);
        int err = resultSet->GetBlob(columnIndex, blob);
        EXPECT_EQ(err, 0);
        EXPECT_NE(blob.size(), 0);

        err = resultSet->GetAllColumnNames(columnNames);
        EXPECT_EQ(err, 0);
        EXPECT_NE(columnNames.size(), 0);
    }
    LOG_INFO("MediaDataShare_ResultSet_Test_002, End");
}

HWTEST_F(MediaDataShareUnitTest, MediaDataShare_ResultSet_Test_003, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_ResultSet_Test_003::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_mediaDataShareHelper;
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(MEDIA_DATA_DB_TITLE, "dataShareTest003");
    vector<string> columns;
    Uri uri(MEDIALIBRARY_DATA_URI);
    auto resultSet = helper->Query(uri, predicates, columns);
    AppDataFwk::SharedBlock *block = nullptr;
    if (resultSet != nullptr) {
        bool hasBlock = resultSet->HasBlock();
        EXPECT_EQ(hasBlock, true);
        block = resultSet->GetBlock();
        EXPECT_NE(block, nullptr);

        resultSet->SetBlock(block);
        EXPECT_EQ(block, resultSet->GetBlock());
        resultSet->FillBlock(0, block);
        EXPECT_EQ(block, resultSet->GetBlock());
    }
    LOG_INFO("MediaDataShare_ResultSet_Test_003, End");
}

HWTEST_F(MediaDataShareUnitTest, MediaDataShare_ResultSet_Test_004, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_ResultSet_Test_004::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_mediaDataShareHelper;
    DataShare::DataSharePredicates predicates;
    predicates.Contains(MEDIA_DATA_DB_TITLE, "dataShareTest");
    vector<string> columns;
    Uri uri(MEDIALIBRARY_DATA_URI);
    auto resultSet = helper->Query(uri, predicates, columns);
    if (resultSet != nullptr) {
        resultSet->GoToFirstRow();
        bool ok = resultSet->OnGo(0, 1);
        EXPECT_EQ(ok, true);
    }
    LOG_INFO("MediaDataShare_ResultSet_Test_004, End");
}

HWTEST_F(MediaDataShareUnitTest, MediaDataShare_ResultSet_Test_005, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_ResultSet_Test_005::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_mediaDataShareHelper;
    DataShare::DataSharePredicates predicates;
    predicates.Contains(MEDIA_DATA_DB_TITLE, "dataShareTest");
    std::vector<string> columns;
    DataShare::DataShareResultSet resultSet;
    std::vector<string> names;
    int err = resultSet.GetAllColumnNames(names);
    EXPECT_NE(err, 0);
    int count;
    err = resultSet.GetRowCount(count);
    EXPECT_NE(err, 0);
    err = resultSet.GoToRow(1);
    EXPECT_NE(err, 0);
    std::vector<uint8_t> blob;
    err = resultSet.GetBlob(0, blob);
    EXPECT_NE(err, 0);
    int64_t longValue;
    err = resultSet.GetLong(0, longValue);
    EXPECT_NE(err, 0);
    double doubleValue;
    err = resultSet.GetDouble(0, doubleValue);
    EXPECT_NE(err, 0);

    bool isNull;
    err = resultSet.IsColumnNull(0, isNull);
    EXPECT_NE(err, 0);
    bool flag = resultSet.OnGo(0, 1);
    EXPECT_EQ(flag, false);
    LOG_INFO("MediaDataShare_ResultSet_Test_005, End");
}

HWTEST_F(MediaDataShareUnitTest, MediaDataShare_CRUD_Test_001, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_CRUD_Test_001::Start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_mediaDataShareHelper;
    Uri uri(MEDIALIBRARY_DATA_URI);
    DataShare::DataShareValuesBucket valuesBucket;
    valuesBucket.Put(MEDIA_DATA_DB_TITLE, "Datashare_CRUD_Test001");
    int retVal = helper->Insert(uri, valuesBucket);
    EXPECT_EQ((retVal > 0), true);

    DataShare::DataSharePredicates predicates;
    string selections = MEDIA_DATA_DB_TITLE + " = 'Datashare_CRUD_Test001'";
    predicates.SetWhereClause(selections);

    valuesBucket.Clear();
    valuesBucket.Put(MEDIA_DATA_DB_TITLE, "Datashare_CRUD_Test002");
    retVal = helper->Update(uri, predicates, valuesBucket);
    EXPECT_EQ((retVal >= 0), true);

    predicates.EqualTo(MEDIA_DATA_DB_TITLE, "Datashare_CRUD_Test002");
    vector<string> columns;
    auto resultSet = helper->Query(uri, predicates, columns);
    int result = 0;
    if (resultSet != nullptr) {
        resultSet->GetRowCount(result);
    }
    EXPECT_EQ(result, 1);

    DataShare::DataSharePredicates deletePredicates;
    selections = MEDIA_DATA_DB_TITLE + " = 'Datashare_CRUD_Test002'";
    deletePredicates.SetWhereClause(selections);
    retVal = helper->Delete(uri, deletePredicates);
    EXPECT_EQ((retVal >= 0), true);
    LOG_INFO("MediaDataShare_CRUD_Test_001, End");
}

HWTEST_F(MediaDataShareUnitTest, MediaDataShare_NotImplPredicates_Test_001, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_NotImplPredicates_Test_001::Start");
    DataShare::DataSharePredicates predicates;
    vector<string> inColumn;
    inColumn.push_back("dataShare_Test_001");
    inColumn.push_back("dataShare_Test_002");
    predicates.In(MEDIA_DATA_DB_TITLE, inColumn);

    vector<string> notInColumn;
    notInColumn.push_back("dataShare_Test_003");
    notInColumn.push_back("dataShare_Test_004");
    predicates.NotIn(MEDIA_DATA_DB_TITLE, notInColumn);
    predicates.Unlike(MEDIA_DATA_DB_TITLE, "%Test003");

    vector<string> preV;
    preV.push_back(MEDIA_DATA_DB_TITLE);
    predicates.GroupBy(preV);
    predicates.Distinct();
    predicates.IndexedBy(MEDIA_DATA_DB_TITLE);
    predicates.KeyPrefix("%Test");
    predicates.InKeys(preV);

    std::list<DataShare::OperationItem> operationItems = predicates.GetOperationList();
    EXPECT_EQ(operationItems.size(), 8);
    LOG_INFO("MediaDataShare_NotImplPredicates_Test_001, End");
}

HWTEST_F(MediaDataShareUnitTest, MediaDataShare_Observer_001, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_Observer_001 start");
    std::shared_ptr<DataShare::DataShareHelper> helper = g_mediaDataShareHelper;
    Uri uri(MEDIALIBRARY_DATA_URI);
    sptr<IDataShareObserverTest> dataObserver;
    helper->RegisterObserver(uri, dataObserver);

    DataShare::DataShareValuesBucket valuesBucket;
    valuesBucket.Put(MEDIA_DATA_DB_TITLE, "Datashare_Observer_Test001");
    int retVal = helper->Insert(uri, valuesBucket);
    EXPECT_EQ((retVal > 0), true);
    helper->NotifyChange(uri);

    DataShare::DataSharePredicates deletePredicates;
    string selections = MEDIA_DATA_DB_TITLE + " = 'Datashare_Observer_Test001'";
    deletePredicates.SetWhereClause(selections);
    retVal = helper->Delete(uri, deletePredicates);
    EXPECT_EQ((retVal >= 0), true);
    helper->NotifyChange(uri);
    helper->UnregisterObserver(uri, dataObserver);
    LOG_INFO("MediaDataShare_Observer_001 end");
}

HWTEST_F(MediaDataShareUnitTest, Creator_ContextNull_Test_001, TestSize.Level0)
{
    LOG_INFO("Creator_ContextNull_Test_001::Start");
    std::shared_ptr<Context> remoteObjCon = nullptr;
    auto remoteNull = DataShare::DataShareHelper::Creator(remoteObjCon, MEDIALIBRARY_DATA_URI);
    EXPECT_EQ(remoteNull, nullptr);
    LOG_INFO("Creator_ContextNull_Test_001 End");
}

HWTEST_F(MediaDataShareUnitTest, Creator_IRemoteObjectNull_Test_001, TestSize.Level0)
{
    LOG_INFO("Creator_IRemoteObjectNull_Test_001::Start");
    sptr<IRemoteObject> remoteObjNull = nullptr;
    auto remoteNull = DataShare::DataShareHelper::Creator(remoteObjNull, MEDIALIBRARY_DATA_URI);
    EXPECT_EQ(remoteNull, nullptr);
    LOG_INFO("Creator_IRemoteObjectNull_Test_001 End");
}

HWTEST_F(MediaDataShareUnitTest, Creator_UriError_Test_001, TestSize.Level0)
{
    LOG_INFO("Creator_UriError_Test_001::Start");
    auto saManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    EXPECT_NE(saManager, nullptr);
    auto remoteObj = saManager->GetSystemAbility(STORAGE_MANAGER_MANAGER_ID);
    EXPECT_NE(remoteObj, nullptr);
    auto uriError = DataShare::DataShareHelper::Creator(remoteObj, MEDIALIBRARY_DATA_URI_ERROR);
    EXPECT_EQ(uriError, nullptr);
    LOG_INFO("Creator_UriError_Test_001 End");
}

HWTEST_F(MediaDataShareUnitTest, GetFileTypes_ConnectionNull_Test_001, TestSize.Level0)
{
    LOG_INFO("GetFileTypes_ConnectionNull_Test_001::Start");
    auto helper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID);
    ASSERT_TRUE(helper != nullptr);
    auto ret = helper->Release();
    EXPECT_TRUE(ret);
    Uri uri(MEDIALIBRARY_DATA_URI);
    std::string mimeTypeFilter("mimeTypeFiltertest");
    std::vector<std::string> result = helper->GetFileTypes(uri, mimeTypeFilter);
    EXPECT_EQ(result.size(), 0);
    LOG_INFO("GetFileTypes_ConnectionNull_Test_001 End");
}

HWTEST_F(MediaDataShareUnitTest, OpenRawFile_ConnectionNull_Test_001, TestSize.Level0)
{
    LOG_INFO("MediaDataShare_OpenRawFile_Test_001::Start");
    auto helper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID);
    ASSERT_TRUE(helper != nullptr);
    auto ret = helper->Release();
    EXPECT_TRUE(ret);
    Uri uri(MEDIALIBRARY_DATA_URI);
    std::string mode("modetest");
    int result = helper->OpenRawFile(uri, mode);
    EXPECT_EQ(result, -1);
    LOG_INFO("MediaDataShare_OpenRawFile_Test_001 End");
}

HWTEST_F(MediaDataShareUnitTest, OpenFile_ConnectionNull_Test_001, TestSize.Level0)
{
    LOG_INFO("OpenFile_ConnectionNull_Test_001::Start");
    auto helper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID);
    ASSERT_TRUE(helper != nullptr);
    auto ret = helper->Release();
    EXPECT_TRUE(ret);
    Uri uri(MEDIALIBRARY_DATA_URI);
    std::string mode("modetest");
    int result = helper->OpenFile(uri, mode);
    EXPECT_EQ(result, -1);
    LOG_INFO("OpenFile_ConnectionNull_Test_001 End");
}

HWTEST_F(MediaDataShareUnitTest, Insert_ConnectionNull_Test_001, TestSize.Level0)
{
    LOG_INFO("Insert_ConnectionNull_Test_001::Start");
    auto helper = CreateDataShareHelper(STORAGE_MANAGER_MANAGER_ID);
    ASSERT_TRUE(helper != nullptr);
    auto ret = helper->Release();
    EXPECT_TRUE(ret);
    Uri uri(MEDIALIBRARY_DATA_URI);
    DataShare::DataShareValuesBucket valuesBucket;

    valuesBucket.Clear();
    double valueD4 = 20.10;
    valuesBucket.Put(MEDIA_DATA_DB_LONGITUDE, valueD4);
    valuesBucket.Put(MEDIA_DATA_DB_TITLE, "dataShareTest006");
    int value4 = 998;
    valuesBucket.Put(MEDIA_DATA_DB_PARENT_ID, value4);
    auto resultInsert= helper->Insert(uri, valuesBucket);
    EXPECT_EQ(resultInsert, -1);

    auto resultGetType = helper->GetType(uri);
    EXPECT_EQ(resultGetType.size(), 0);
    valuesBucket.Clear();
    LOG_INFO("Insert_ConnectionNull_Test_001 End");
}
} // namespace Media
} // namespace OHOS