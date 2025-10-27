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
 * @tc.desc: Test the conversion functionality of the ValueProxy class, specifically converting a vector of
 *           DataShareValuesBucket objects to normal VBuckets (DataShareObserver::ChangeInfo::VBuckets), to verify
 *           consistency between the original vector size and the converted VBuckets size.
 * @tc.type: FUNC
 * @tc.require: NA
 * @tc.precon:
    1. The test environment supports instantiation of DataShare::DataShareValuesBucket objects and the use of
       their Put method to add key-value pairs (supports double and string types for values).
    2. The ValueProxy class provides a static Convert method that accepts a std::vector<DataShareValuesBucket>
       (moved via std::move) and returns a DataShareObserver::ChangeInfo::VBuckets object.
    3. The DataShareObserver::ChangeInfo::VBuckets type is a valid predefined type that can store the converted
       key-value pair data.
    4. The test environment allows the creation of a std::vector<DataShareValuesBucket> and the addition of
       DataShareValuesBucket instances to it.
 * @tc.step:
    1. Create the first DataShareValuesBucket object (valuesBucket1), call its Put method to add two key-value pairs:
       "phoneNumber" (double: 20.07) and "name" (string: "dataShareTest003").
    2. Create the second DataShareValuesBucket object (valuesBucket2), call its Put method to add one key-value pair:
       "age" (int: 1001).
    3. Create a std::vector<DataShareValuesBucket> (VBuckets) and initialize it with valuesBucket1 and valuesBucket2.
    4. Declare a DataShareObserver::ChangeInfo::VBuckets variable (extends), then assign it the result of
       ValueProxy::Convert(std::move(VBuckets)).
    5. Check the size of the converted extends (VBuckets) to verify it matches the original vector size.
 * @tc.expect:
    1. The size of the converted DataShareObserver::ChangeInfo::VBuckets (extends) is 2, which matches the size of
       the original std::vector<DataShareValuesBucket>.
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
 * @tc.desc: Test the conversion functionality of the ValueProxy class, specifically converting normal VBuckets
 *           (DataShareObserver::ChangeInfo::VBuckets) to a vector of DataShareValuesBucket objects, to verify
 *           consistency between the original VBuckets size and the converted vector size.
 * @tc.type: FUNC
 * @tc.require: NA
 * @tc.precon:
    1. The test environment supports instantiation of DataShareObserver::ChangeInfo::VBuckets objects and the
       initialization of their entries with key-value pairs (supports double, string, and int types for values).
    2. The ValueProxy class provides a static Convert method that accepts a DataShareObserver::ChangeInfo::VBuckets
       (moved via std::move) and returns a std::vector<DataShare::DataShareValuesBucket>.
    3. The DataShare::DataShareValuesBucket type is a valid predefined type that can store the converted key-value
       pair data from VBuckets.
    4. The test environment allows the creation of a std::vector<DataShareValuesBucket> to store the converted result.
 * @tc.step:
    1. Create a DataShareObserver::ChangeInfo::VBuckets object (extends) with two entries: the first entry contains
       key-value pairs "phoneNumber" (double: 20.07) and "name" (string: "dataShareTest003"); the second entry
       contains "age" (int: 1001).
    2. Declare a std::vector<DataShare::DataShareValuesBucket> variable (VBuckets), then assign it the result of
       ValueProxy::Convert(std::move(extends)).
    3. Check the size of the converted VBuckets (vector of DataShareValuesBucket) to verify it matches the original
       extends (VBuckets) size.
 * @tc.expect:
    1. The size of the converted std::vector<DataShare::DataShareValuesBucket> (VBuckets) is 2, which matches the
       size of the original DataShareObserver::ChangeInfo::VBuckets (extends).
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