#include <gtest/gtest.h>
#include <unistd.h>

#include "accesstoken_kit.h"
#include "datashare_helper.h"
#include "datashare_log.h"
#include "datashare_template.h"
#include "hap_token_info.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "token_setproc.h"
#include "datashare_errno.h"

#include "published_data_subscriber_manager.h"
#include "rdb_subscriber_manager.h"
#include "ishared_result_set_stub.h"
#include "message_parcel.h"
#include "ikvstore_data_service.h"
#include "shared_block.h"
#include "uri.h"
#include "datashare_connection.h"

namespace OHOS {
namespace DataShare {
using namespace testing::ext;
using namespace OHOS::Security::AccessToken;
std::string DATA_SHARE_PROXY_URI = "datashareproxy://com.acts.ohos.data.datasharetest/test";
std::shared_ptr<DataShare::DataShareHelper> dataShareHelper;
std::string TBL_NAME0 = "name0";

class AbnormalBranchTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

using namespace OHOS::DataShare;
using Uri = OHOS::Uri;
void AbnormalBranchTest::SetUpTestCase(void)
{
    LOG_INFO("SetUpTestCase invoked");
    int sleepTime = 1;
    sleep(sleepTime);

    HapInfoParams info = { .userID = 100,
        .bundleName = "ohos.datashareproxyclienttest.demo",
        .instIndex = 0,
        .appIDDesc = "ohos.datashareproxyclienttest.demo" };
    HapPolicyParams policy = { .apl = APL_SYSTEM_BASIC,
        .domain = "test.domain",
        .permList = { { .permissionName = "ohos.permission.GET_BUNDLE_INFO",
            .bundleName = "ohos.datashareproxyclienttest.demo",
            .grantMode = 1,
            .availableLevel = APL_SYSTEM_BASIC,
            .label = "label",
            .labelId = 1,
            .description = "ohos.datashareproxyclienttest.demo",
            .descriptionId = 1 } },
        .permStateList = { { .permissionName = "ohos.permission.GET_BUNDLE_INFO",
            .isGeneral = true,
            .resDeviceID = { "local" },
            .grantStatus = { PermissionState::PERMISSION_GRANTED },
            .grantFlags = { 1 } } } };
    AccessTokenKit::AllocHapToken(info, policy);
    auto testTokenId =
        Security::AccessToken::AccessTokenKit::GetHapTokenID(info.userID, info.bundleName, info.instIndex);
    SetSelfTokenID(testTokenId);

    CreateOptions options;
    options.enabled_ = true;
    dataShareHelper = DataShare::DataShareHelper::Creator(DATA_SHARE_PROXY_URI, options);
    ASSERT_TRUE(dataShareHelper != nullptr);
    LOG_INFO("SetUpTestCase end");
}

void AbnormalBranchTest::TearDownTestCase(void)
{
    auto tokenId = AccessTokenKit::GetHapTokenID(100, "ohos.datashareclienttest.demo", 0);
    AccessTokenKit::DeleteToken(tokenId);
    dataShareHelper = nullptr;
}

void AbnormalBranchTest::SetUp(void)
{
}
void AbnormalBranchTest::TearDown(void)
{
}

HWTEST_F(AbnormalBranchTest, AbnormalBranchTest_ResultSet_Test_001, TestSize.Level0)
{
    LOG_INFO("AbnormalBranchTest_ResultSet_Test_001::Start");
    auto helper = dataShareHelper;
    Uri uri(DATA_SHARE_PROXY_URI);
    DataShare::DataSharePredicates predicates;
    predicates.EqualTo(TBL_NAME0, "wang");
    std::vector<string> columns;
    auto resultSet = helper->Query(uri, predicates, columns);
    EXPECT_NE(resultSet, nullptr);

    int result = 0;
    resultSet->GetRowCount(result);
    EXPECT_EQ(result, 1);

    AppDataFwk::SharedBlock *block = nullptr;
    ASSERT_TRUE(resultSet != nullptr);
    bool hasBlock = resultSet->HasBlock();
    EXPECT_EQ(hasBlock, true);
    block = resultSet->GetBlock();
    EXPECT_NE(block, nullptr);
    
    std::vector<uint8_t> blob;
    int err = resultSet->GetBlob(-1, blob);
    EXPECT_EQ(err, E_INVALID_COLUMN_INDEX);
    resultSet->SetBlock(nullptr);
    EXPECT_EQ(nullptr, resultSet->GetBlock());
    std::string stringValue;
    result = resultSet->GetString(0, stringValue);
    EXPECT_EQ(result, E_ERROR);
    int intValue;
    result = resultSet->GetInt(0, intValue);
    EXPECT_EQ(result, E_ERROR);
    LOG_INFO("AbnormalBranchTest_ResultSet_Test_001::End");
}

HWTEST_F(AbnormalBranchTest, AbnormalBranchTest_shareBlock_Null_Test_001, TestSize.Level0)
{
    LOG_INFO("AbnormalBranchTest_shareBlock_Null_Test_001::Start");
    DataShareBlockWriterImpl impl;
    int result = impl.AllocRow();
    EXPECT_EQ(result, E_ERROR);
    result = impl.Write(1);
    EXPECT_EQ(result, E_ERROR);
    int64_t intValue = 0;
    result = impl.Write(1, intValue);
    EXPECT_EQ(result, E_ERROR);
    double doubleValue = 0.0;
    result = impl.Write(1, doubleValue);
    EXPECT_EQ(result, E_ERROR);
    uint8_t *unitValue = nullptr;
    result = impl.Write(1, unitValue, 0);
    EXPECT_EQ(result, E_ERROR);
    char *charValue = nullptr;
    result = impl.Write(1, charValue, 0);
    EXPECT_EQ(result, E_ERROR);
    LOG_INFO("AbnormalBranchTest_shareBlock_Null_Test_001::End");
}

HWTEST_F(AbnormalBranchTest, AbnormalBranchTest_ResultSetStubNull_Test_001, TestSize.Level0)
{
    LOG_INFO("AbnormalBranchTest_ResultSetStubNull_Test_001::Start");
    ISharedResultSetStub stub(nullptr);
    std::shared_ptr<DataShareResultSet> result = nullptr;
    OHOS::MessageParcel parcel;
    sptr<ISharedResultSet> resultSet = stub.CreateStub(result, parcel);
    EXPECT_EQ(resultSet, nullptr);
    LOG_INFO("AbnormalBranchTest_ResultSetStubNull_Test_001::End");
}

HWTEST_F(AbnormalBranchTest, AbnormalBranchTest_RegisterClientDeathObserverNull_Test_001, TestSize.Level0)
{
    LOG_INFO("AbnormalBranchTest_RegisterClientDeathObserverNull_Test_001::Start");
    DataShareKvServiceProxy proxy(nullptr);
    std::string appId;
    uint32_t result = proxy.RegisterClientDeathObserver(appId, nullptr);
    EXPECT_EQ(result, -1);
    LOG_INFO("AbnormalBranchTest_RegisterClientDeathObserverNull_Test_001::End");
}

HWTEST_F(AbnormalBranchTest, AbnormalBranchTest_mReadOnlyInvalid_Test_001, TestSize.Level0)
{
    LOG_INFO("AbnormalBranchTest_mReadOnlyInvalid_Test_001::Start");
    std::string name;
    size_t size = 0;
    bool readOnly = true;
    AppDataFwk::SharedBlock temp(name, nullptr, size, readOnly);
    int result = temp.Clear();
    EXPECT_EQ(result, AppDataFwk::SharedBlock::SHARED_BLOCK_INVALID_OPERATION);
    result = temp.SetColumnNum(1);
    EXPECT_EQ(result, AppDataFwk::SharedBlock::SHARED_BLOCK_INVALID_OPERATION);
    result = temp.AllocRow();
    EXPECT_EQ(result, AppDataFwk::SharedBlock::SHARED_BLOCK_INVALID_OPERATION);
    result = temp.FreeLastRow();
    EXPECT_EQ(result, AppDataFwk::SharedBlock::SHARED_BLOCK_INVALID_OPERATION);
    int64_t intValue = 0;
    result = temp.PutLong(1, 1, intValue);
    EXPECT_EQ(result, AppDataFwk::SharedBlock::SHARED_BLOCK_INVALID_OPERATION);
    double doubleValue = 0.0;
    result = temp.PutDouble(1, 1, doubleValue);
    EXPECT_EQ(result, AppDataFwk::SharedBlock::SHARED_BLOCK_INVALID_OPERATION);
    result = temp.PutNull(1, 1);
    EXPECT_EQ(result, AppDataFwk::SharedBlock::SHARED_BLOCK_INVALID_OPERATION);
    result = temp.SetRawData(nullptr, size);
    EXPECT_EQ(result, AppDataFwk::SharedBlock::SHARED_BLOCK_INVALID_OPERATION);
    LOG_INFO("AbnormalBranchTest_mReadOnlyInvalid_Test_001::End");
}

HWTEST_F(AbnormalBranchTest, AbnormalBranchTest_CreatorPossibleNull_Test_001, TestSize.Level0)
{
    LOG_INFO("AbnormalBranchTest_CreatorPossibleNull_Test_001::Start");
    std::string strUri;
    CreateOptions options;
    options.token_ = nullptr;
    std::string bundleName;
    std::shared_ptr<DataShareHelper> dataHelper = DataShare::DataShareHelper::Creator(strUri, options, bundleName);
    EXPECT_EQ(dataHelper, nullptr);
    LOG_INFO("AbnormalBranchTest_CreatorPossibleNull_Test_001::End");
}

HWTEST_F(AbnormalBranchTest, AbnormalBranchTest_CreatorPossibleNull_Test_002, TestSize.Level0)
{
    LOG_INFO("AbnormalBranchTest_CreatorPossibleNull_Test_002::Start");
    std::string strUri;
    CreateOptions options;
    options.token_ = nullptr;
    options.isProxy_ = false;
    std::string bundleName;
    std::shared_ptr<DataShareHelper> dataHelper = DataShare::DataShareHelper::Creator(strUri, options, bundleName);
    EXPECT_EQ(dataHelper, nullptr);
    LOG_INFO("AbnormalBranchTest_CreatorPossibleNull_Test_002::End");
}

HWTEST_F(AbnormalBranchTest, AbnormalBranchTest_extSpCtl_Null_Test_001, TestSize.Level0)
{
    LOG_INFO("AbnormalBranchTest_extSpCtl_Null_Test_001::Start");
    auto helper = dataShareHelper;
    bool ret = helper->Release();
    EXPECT_EQ(ret, true);
    Uri uri("");
    std::string str;
    std::vector<std::string> result = helper->GetFileTypes(uri, str);
    EXPECT_EQ(result.size(), 0);
    int err = helper->OpenFile(uri, str);
    EXPECT_EQ(err, -1);
    err = helper->OpenRawFile(uri, str);
    EXPECT_EQ(err, -1);
    LOG_INFO("AbnormalBranchTest_extSpCtl_Null_Test_001::End");
}

HWTEST_F(AbnormalBranchTest, AbnormalBranchTest_extSpCtl_Null_Test_002, TestSize.Level0)
{
    LOG_INFO("AbnormalBranchTest_extSpCtl_Null_Test_002::Start");
    auto helper = dataShareHelper;
    bool ret = helper->Release();
    EXPECT_EQ(ret, true);
    Uri uri("");
    Uri uriResult = helper->NormalizeUri(uri);
    EXPECT_EQ(uriResult, uri);
    uriResult = helper->DenormalizeUri(uri);
    EXPECT_EQ(uriResult, uri);
    LOG_INFO("AbnormalBranchTest_extSpCtl_Null_Test_002::End");
}

HWTEST_F(AbnormalBranchTest, AbnormalBranchTest_AddObserversProxyNull_Test_001, TestSize.Level0)
{
    LOG_INFO("AbnormalBranchTest_AddObserversProxyNull_Test_001::Start");
    void *subscriber = nullptr;
    std::shared_ptr<DataShareServiceProxy> proxy = nullptr;
    const std::vector<std::string> uris = {};
    int64_t subscriberId = 0;
    const PublishedDataCallback callback = [](const PublishedDataChangeNode &changeNode){};
    std::vector<OperationResult> results = PublishedDataSubscriberManager::GetInstance().AddObservers(subscriber,
        proxy, uris, subscriberId, callback);
    EXPECT_EQ(results.size(), uris.size());
    LOG_INFO("AbnormalBranchTest_AddObserversProxyNull_Test_001::End");
}

HWTEST_F(AbnormalBranchTest, AbnormalBranchTest_AddObserversProxyNull_Test_002, TestSize.Level0)
{
    LOG_INFO("AbnormalBranchTest_AddObserversProxyNull_Test_002::Start");
    void *subscriber = nullptr;
    std::shared_ptr<DataShareServiceProxy> proxy = nullptr;
    const std::vector<std::string> uris = {};
    TemplateId templateId;
    const RdbCallback callback = [](const RdbChangeNode &changeNode){};
    std::vector<OperationResult> results = RdbSubscriberManager::GetInstance().AddObservers(subscriber, proxy, uris,
        templateId, callback);
    EXPECT_EQ(results.size(), uris.size());
    LOG_INFO("AbnormalBranchTest_AddObserversProxyNull_Test_002::End");
}

HWTEST_F(AbnormalBranchTest, AbnormalBranchTest_DelObserversProxyNull_Test_001, TestSize.Level0)
{
    LOG_INFO("AbnormalBranchTest_DelObserversProxyNull_Test_001::Start");
    void *subscriber = nullptr;
    std::shared_ptr<DataShareServiceProxy> proxy = nullptr;
    const std::vector<std::string> uris = {};
    int64_t subscriberId = 0;
    std::vector<OperationResult> results = PublishedDataSubscriberManager::GetInstance().DelObservers(subscriber,
        proxy);
    EXPECT_EQ(results.size(), uris.size());
    results = PublishedDataSubscriberManager::GetInstance().DelObservers(subscriber, proxy, uris, subscriberId);
    EXPECT_EQ(results.size(), uris.size());
    LOG_INFO("AbnormalBranchTest_DelObserversProxyNull_Test_001::End");
}

HWTEST_F(AbnormalBranchTest, AbnormalBranchTest_DelObserversProxyNull_Test_002, TestSize.Level0)
{
    LOG_INFO("AbnormalBranchTest_DelObserversProxyNull_Test_002::Start");
    void *subscriber = nullptr;
    std::shared_ptr<DataShareServiceProxy> proxy = nullptr;
    const std::vector<std::string> uris = {};
    TemplateId templateId;
    std::vector<OperationResult> results = RdbSubscriberManager::GetInstance().DelObservers(subscriber, proxy, uris,
        templateId);
    EXPECT_EQ(results.size(), uris.size());
    results = RdbSubscriberManager::GetInstance().DelObservers(subscriber, proxy);
    EXPECT_EQ(results.size(), uris.size());
    LOG_INFO("AbnormalBranchTest_DelObserversProxyNull_Test_002::End");
}

HWTEST_F(AbnormalBranchTest, AbnormalBranchTest_EnableObserversProxyNull_Test_001, TestSize.Level0)
{
    LOG_INFO("AbnormalBranchTest_EnableObserversProxyNull_Test_001::Start");
    void *subscriber = nullptr;
    std::shared_ptr<DataShareServiceProxy> proxy = nullptr;
    const std::vector<std::string> uris = {};
    int64_t subscriberId = 0;
    std::vector<OperationResult> results = PublishedDataSubscriberManager::GetInstance().EnableObservers(subscriber,
        proxy, uris, subscriberId);
    EXPECT_EQ(results.size(), uris.size());
    LOG_INFO("AbnormalBranchTest_EnableObserversProxyNull_Test_001::End");
}

HWTEST_F(AbnormalBranchTest, AbnormalBranchTest_EnableObserversProxyNull_Test_002, TestSize.Level0)
{
    LOG_INFO("AbnormalBranchTest_EnableObserversProxyNull_Test_002::Start");
    void *subscriber = nullptr;
    std::shared_ptr<DataShareServiceProxy> proxy = nullptr;
    const std::vector<std::string> uris = {};
    TemplateId templateId;
    std::vector<OperationResult> results = RdbSubscriberManager::GetInstance().EnableObservers(subscriber, proxy,
        uris, templateId);
    EXPECT_EQ(results.size(), uris.size());
    LOG_INFO("AbnormalBranchTest_EnableObserversProxyNull_Test_002::End");
}

HWTEST_F(AbnormalBranchTest, AbnormalBranchTest_DisableObserversProxyNull_Test_001, TestSize.Level0)
{
    LOG_INFO("AbnormalBranchTest_DisableObserversProxyNull_Test_001::Start");
    void *subscriber = nullptr;
    std::shared_ptr<DataShareServiceProxy> proxy = nullptr;
    const std::vector<std::string> uris = {};
    int64_t subscriberId = 0;
    std::vector<OperationResult> results = PublishedDataSubscriberManager::GetInstance().DisableObservers(subscriber,
        proxy, uris, subscriberId);
    EXPECT_EQ(results.size(), uris.size());
    LOG_INFO("AbnormalBranchTest_DisableObserversProxyNull_Test_001::End");
}

HWTEST_F(AbnormalBranchTest, AbnormalBranchTest_DisableObserversProxyNull_Test_002, TestSize.Level0)
{
    LOG_INFO("AbnormalBranchTest_DisableObserversProxyNull_Test_002::Start");
    void *subscriber = nullptr;
    std::shared_ptr<DataShareServiceProxy> proxy = nullptr;
    const std::vector<std::string> uris = {};
    TemplateId templateId;
    std::vector<OperationResult> results = RdbSubscriberManager::GetInstance().DisableObservers(subscriber, proxy,
        uris, templateId);
    EXPECT_EQ(results.size(), uris.size());
    LOG_INFO("AbnormalBranchTest_DisableObserversProxyNull_Test_002::End");
}
} // namespace DataShare
} // namespace OHOS