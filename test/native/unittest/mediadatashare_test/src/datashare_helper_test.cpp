/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

 #include <gtest/gtest.h>
 #include <unistd.h>
 
 #include "datashare_helper.h"
 #include "datashare_helper_impl.h"
 #include "datashare_log.h"
 #include "datashare_uri_utils.h"
 
 namespace OHOS {
 namespace DataShare {
 using namespace testing::ext;
 class DataShareHelperTest : public testing::Test {
 public:
     static void SetUpTestCase(void){};
     static void TearDownTestCase(void){};
     void SetUp(){};
     void TearDown(){};
 };
 
 /**
 * @tc.name: 
 * @tc.desc: test Creator function when options.isProxy_ = false and options.token_ = nullptr
 * @tc.type: FUNC
 * @tc.require:issueIC413F
 * @tc.precon: None
 * @tc.step:
     1.Creat a DataShareHelper object what options.isProxy_ = false and options.token_ = nullptr
     2.call Creator function and check the result
 * @tc.experct: Creator failed and reutrn nullptr
 */
 HWTEST_F(DataShareHelperTest, CreatorTest001, TestSize.Level0)
 {
     LOG_INFO("DataShareHelperTest CreatorTest001::Start");
     std::string strUri = "testUri";
     CreateOptions options;
     options.isProxy_ = false;
     options.token_ = nullptr;
     std::string bundleName = "testBundle";
     int waitTime = 1;
     bool isSystem = true;
     auto result = DataShareHelper::Creator(strUri, options, bundleName, waitTime, isSystem);
     EXPECT_EQ(result, nullptr);
     LOG_INFO("DataShareHelperTest CreatorTest001::End");
 }
 
 /**
 * @tc.name: 
 * @tc.desc: test Creator function when options.isProxy_ = true and options.token_ = nullptr
 * @tc.type: FUNC
 * @tc.require:issueIC413F
 * @tc.precon: None
 * @tc.step:
     1.Creat a DataShareHelper object what options.isProxy_ = true and options.token_ = nullptr
     2.call Creator function and check the result
 * @tc.experct: Creator failed and reutrn nullptr
 */
 HWTEST_F(DataShareHelperTest, CreatorTest002, TestSize.Level0)
 {
     LOG_INFO("DataShareHelperTest CreatorTest002::Start");
     std::string strUri = "testUri";
     CreateOptions options;
     options.isProxy_ = true;
     options.token_ = nullptr;
     std::string bundleName = "testBundle";
     int waitTime = 1;
     bool isSystem = true;
     auto result = DataShareHelper::Creator(strUri, options, bundleName, waitTime, isSystem);
     EXPECT_NE(result, nullptr);
     LOG_INFO("DataShareHelperTest CreatorTest002::End");
 }
 
 /**
 * @tc.name: 
 * @tc.desc: test CreateExtHelper function when Uri contains "appIndex="
 * @tc.type: FUNC
 * @tc.require:issueIC413F
 * @tc.precon: None
 * @tc.step:
     1.Creat a DataShareHelper object what Uri contains "appIndex="
     2.call CreateExtHelper function and check the result
 * @tc.experct: CreateExtHelper failed and reutrn nullptr
 */
 HWTEST_F(DataShareHelperTest, CreateExtHelper001, TestSize.Level0)
 {
     LOG_INFO("DataShareHelperTest CreateExtHelper001::Start");
     OHOS::Uri uri("datashareproxy://com.acts.ohos.data.datasharetest/test?appIndex=abcd");
     uri.query_ = ("appIndex=abcd");
     sptr<IRemoteObject> token = nullptr;
     int waitTime = 1000;
     bool isSystem = false;
     auto result = DataShareHelper::CreateExtHelper(uri, token, waitTime, isSystem);
     EXPECT_EQ(result, nullptr);
     LOG_INFO("DataShareHelperTest CreateExtHelper001::End");
 }
 } // namespace DataShare
 } // namespace OHOS
 