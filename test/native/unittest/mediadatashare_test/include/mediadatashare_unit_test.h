/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#ifndef MEDIA_DATASHARE_UNIT_TEST_H
#define MEDIA_DATASHARE_UNIT_TEST_H

#include <gtest/gtest.h>
#include <condition_variable>
#include "data_ability_observer_interface.h"
#include "datashare_helper.h"
#include "data_ability_observer_stub.h"

namespace OHOS {
namespace DataShare {
using ChangeInfo = DataShareObserver::ChangeInfo;
using Value = std::variant<std::monostate, int64_t, double, std::string, bool, std::vector<uint8_t>>;
using VBucket = std::map<std::string, Value>;
using VBuckets = std::vector<VBucket>;
class MediaDataShareUnitTest : public testing::Test {
public:
    /* SetUpTestCase:The preset action of the test suite is executed before the first TestCase */
    static void SetUpTestCase(void);

    /* TearDownTestCase:The test suite cleanup action is executed after the last TestCase */
    static void TearDownTestCase(void);

    /* SetUp:Execute before each test case */
    void SetUp();

    /* TearDown:Execute after each test case */
    void TearDown();
    bool UrisEqual(std::list<Uri> uri1, std::list<Uri> uri2);
    bool ValueBucketEqual(const VBuckets& vb1, const VBuckets& vb2);
    bool ChangeInfoEqual(const ChangeInfo &changeInfo, const ChangeInfo &expectChangeInfo);
};

class IDataAbilityObserverTest : public AAFwk::DataAbilityObserverStub {
public:
    IDataAbilityObserverTest();
    ~IDataAbilityObserverTest()
    {}

    void OnChange()
    {
        GTEST_LOG_(INFO) << "OnChange enter";
    }
};

} // namespace Media
} // namespace OHOS

#endif  // MEDIA_DATASHARE_UNIT_TEST_H
