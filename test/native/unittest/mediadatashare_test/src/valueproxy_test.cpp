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

/**
* @tc.name: VBucketsDataShare2Normal
* @tc.desc: Test conversion from DataShareValuesBucket vector to normal VBuckets
* @tc.type: FUNC
* @tc.require: NA
* @tc.precon: NA
* @tc.step:
* 1. Create two DataShareValuesBucket objects and add key-value pairs
* 2. Put the buckets into a vector
* 3. Convert the vector using ValueProxy::Convert()
* 4. Verify the converted VBuckets size is 2
* @tc.expect: The converted VBuckets has size 2, matching original vector size
*/
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

/**
* @tc.name: VBucketsNormal2DataShare
* @tc.desc: Test conversion from normal VBuckets to DataShareValuesBucket vector
* @tc.type: FUNC
* @tc.require: NA
* @tc.precon: NA
* @tc.step:
* 1. Create a VBuckets object with two entries containing key-value pairs
* 2. Convert the VBuckets using ValueProxy::Convert()
* 3. Verify the converted DataShareValuesBucket vector size is 2
* @tc.expect: The converted vector has size 2, matching original VBuckets size
*/
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