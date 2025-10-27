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
 * @tc.desc: Verify the conversion from a vector of DataShareValuesBucket to DataShareObserver::ChangeInfo::VBuckets
 *           using ValueProxy::Convert, ensuring the converted size matches the original.
 * @tc.type: FUNC
 * @tc.precon:
    1. The DataShare::DataShareValuesBucket class supports the Put method to add key-value pairs (e.g., double, string,
       int).
    2. DataShareObserver::ChangeInfo::VBuckets is a valid type that can store key-value pairs.
    3. The ValueProxy::Convert method accepts a std::vector<DataShareValuesBucket> and returns VBuckets.
 * @tc.step:
    1. Create two DataShareValuesBucket instances:
        a. valuesBucket1 with key-value pairs ("phoneNumber" = 20.07, "name" = "dataShareTest003").
        b. valuesBucket2 with key-value pair ("age" = 1001).
    2. Create a std::vector<DataShareValuesBucket> (VBuckets) containing the two instances.
    3. Call ValueProxy::Convert with std::move(VBuckets) to convert to DataShareObserver::ChangeInfo::VBuckets
       (extends).
    4. Check the size of the converted extends.
 * @tc.expect:
    1. The size of the converted extends is 2, matching the size of the original VBuckets vector.
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
 * @tc.desc: Verify the conversion from DataShareObserver::ChangeInfo::VBuckets to a vector of DataShareValuesBucket
 *           using ValueProxy::Convert, ensuring the converted size matches the original.
 * @tc.type: FUNC
 * @tc.precon:
    1. DataShareObserver::ChangeInfo::VBuckets can be initialized with key-value pairs (e.g., double, string, int).
    2. The DataShare::DataShareValuesBucket class is a valid type that can store key-value pairs.
    3. The ValueProxy::Convert method accepts VBuckets and returns a std::vector<DataShareValuesBucket>.
 * @tc.step:
    1. Create a DataShareObserver::ChangeInfo::VBuckets (extends) with two entries:
        a. First entry: {"phoneNumber" = 20.07, "name" = "dataShareTest003"}.
        b. Second entry: {"age" = 1001}.
    2. Call ValueProxy::Convert with std::move(extends) to convert to a std::vector<DataShareValuesBucket> (VBuckets).
    3. Check the size of the converted VBuckets.
 * @tc.expect:
    1. The size of the converted VBuckets vector is 2, matching the size of the original extends.
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