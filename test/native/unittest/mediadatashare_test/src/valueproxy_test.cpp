/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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
#include "datashare_valuebucket_convert.h"

namespace OHOS {
namespace DataShare {
using namespace testing::ext;
class ValueProxyTest : public testing::Test {
};
HWTEST_F(ValueProxyTest, VBucketsDataShare2Normal, TestSize.Level0)
{
    using DataShareBucket = OHOS::DataShare::DataShareValuesBucket;
    DataShareBucket valuesBucket1;
    DataShareBucket valuesBucket2;
    valuesBucket1.Put("phoneNumber", 20.07);
    valuesBucket1.Put("name", "dataShareTest003");
    valuesBucket2.Put("age", 1001);
    std::vector<DataShareBucket> VBuckets = {valuesBucket1, valuesBucket2};
    DataShareObserver::ChangeInfo::VBuckets extends;
    extends = ValueProxy::Convert(std::move(VBuckets));
    ASSERT_EQ(extends.size(), 2);
}
HWTEST_F(ValueProxyTest, VBucketsNormal2DataShare, TestSize.Level0)
{
    using DataShareBucket = OHOS::DataShare::DataShareValuesBucket;
    std::vector<DataShareBucket> VBuckets;
    DataShareObserver::ChangeInfo::VBuckets extends = {
        {{"phoneNumber", 20.07}, {"name", "dataShareTest003"}},
        {{"age", 1001}}
    };
    VBuckets = ValueProxy::Convert(std::move(extends));
    ASSERT_EQ(VBuckets.size(), 2);
}
} // namespace DataShare
}