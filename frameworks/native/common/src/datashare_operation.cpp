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

#include "datashare_operation.h"
#include "datashare_log.h"
#include "itypes_util.h"

namespace OHOS {
namespace DataShare {
DataShareOperation::DataShareOperation(
    const std::shared_ptr<DataShareOperation> &dataShareOperation, const std::shared_ptr<Uri> &withUri)
{
    uri_ = withUri;
    if (dataShareOperation != nullptr) {
        type_ = dataShareOperation->type_;
        valuesBucket_ = dataShareOperation->valuesBucket_;
        expectedCount_ = dataShareOperation->expectedCount_;
        dataSharePredicates_ = dataShareOperation->dataSharePredicates_;
        valuesBucketReferences_ = dataShareOperation->valuesBucketReferences_;
        dataSharePredicatesBackReferences_ = dataShareOperation->dataSharePredicatesBackReferences_;
        interrupted_ = dataShareOperation->interrupted_;
    } else {
        type_ = 0;
        expectedCount_ = 0;
        valuesBucket_ = std::make_shared<DataShareValuesBucket>();
        dataSharePredicates_ = std::make_shared<DataSharePredicates>();
        valuesBucketReferences_ = std::make_shared<DataShareValuesBucket>();
        dataSharePredicatesBackReferences_.clear();
        interrupted_ = false;
    }
}
DataShareOperation::DataShareOperation(Parcel &in)
{
    ReadFromParcel(in);
}
DataShareOperation::DataShareOperation(const std::shared_ptr<DataShareOperationBuilder> &builder)
{
    if (builder != nullptr) {
        type_ = builder->type_;
        uri_ = builder->uri_;
        valuesBucket_ = builder->valuesBucket_;
        expectedCount_ = builder->expectedCount_;
        dataSharePredicates_ = builder->dataSharePredicates_;
        valuesBucketReferences_ = builder->valuesBucketReferences_;
        dataSharePredicatesBackReferences_ = builder->dataSharePredicatesBackReferences_;
        interrupted_ = builder->interrupted_;
    }
}

DataShareOperation::DataShareOperation()
{
    type_ = 0;
    uri_ = nullptr;
    expectedCount_ = 0;
    valuesBucket_ = std::make_shared<DataShareValuesBucket>();
    dataSharePredicates_ = std::make_shared<DataSharePredicates>();
    valuesBucketReferences_ = std::make_shared<DataShareValuesBucket>();
    dataSharePredicatesBackReferences_.clear();
    interrupted_ = false;
}

DataShareOperation::~DataShareOperation()
{
    dataSharePredicatesBackReferences_.clear();
}

bool DataShareOperation::operator==(const DataShareOperation &other) const
{
    if (type_ != other.type_) {
        return false;
    }
    if ((uri_ != nullptr) && (other.uri_ != nullptr) && (uri_->ToString() != other.uri_->ToString())) {
        return false;
    }
    if (expectedCount_ != other.expectedCount_) {
        return false;
    }
    if (valuesBucket_ != other.valuesBucket_) {
        return false;
    }
    if (dataSharePredicates_ != other.dataSharePredicates_) {
        return false;
    }
    if (valuesBucketReferences_ != other.valuesBucketReferences_) {
        return false;
    }
    size_t backReferencesCount = dataSharePredicatesBackReferences_.size();
    size_t otherBackReferencesCount = other.dataSharePredicatesBackReferences_.size();
    if (backReferencesCount != otherBackReferencesCount) {
        return false;
    }

    std::map<int, int>::const_iterator it = dataSharePredicatesBackReferences_.begin();
    while (it != dataSharePredicatesBackReferences_.end()) {
        std::map<int, int>::const_iterator otherIt = other.dataSharePredicatesBackReferences_.find(it->first);
        if (otherIt != other.dataSharePredicatesBackReferences_.end()) {
            if (otherIt->second != it->second) {
                return false;
            }
        } else {
            return false;
        }
        ++it;
    }

    if (interrupted_ != other.interrupted_) {
        return false;
    }
    return true;
}

DataShareOperation &DataShareOperation::operator=(const DataShareOperation &other)
{
    if (this != &other) {
        type_ = other.type_;
        uri_ = other.uri_;
        expectedCount_ = other.expectedCount_;
        valuesBucket_ = other.valuesBucket_;
        dataSharePredicates_ = other.dataSharePredicates_;
        valuesBucketReferences_ = other.valuesBucketReferences_;
        dataSharePredicatesBackReferences_ = other.dataSharePredicatesBackReferences_;
        interrupted_ = other.interrupted_;
    }
    return *this;
}

std::shared_ptr<DataShareOperationBuilder> DataShareOperation::NewInsertBuilder(const std::shared_ptr<Uri> &uri)
{
    LOG_DEBUG("Start");
    if (uri == nullptr) {
        LOG_ERROR("uri is nullptr");
        return nullptr;
    }
    std::shared_ptr<DataShareOperationBuilder> builder =
        std::make_shared<DataShareOperationBuilder>(TYPE_INSERT, uri);
    return builder;
}

std::shared_ptr<DataShareOperationBuilder> DataShareOperation::NewUpdateBuilder(const std::shared_ptr<Uri> &uri)
{
    LOG_DEBUG("Start");
    if (uri == nullptr) {
        LOG_ERROR("uri is nullptr");
        return nullptr;
    }
    std::shared_ptr<DataShareOperationBuilder> builder =
        std::make_shared<DataShareOperationBuilder>(TYPE_UPDATE, uri);
    return builder;
}

std::shared_ptr<DataShareOperationBuilder> DataShareOperation::NewDeleteBuilder(const std::shared_ptr<Uri> &uri)
{
    LOG_DEBUG("Start");
    if (uri == nullptr) {
        LOG_ERROR("uri is nullptr");
        return nullptr;
    }
    std::shared_ptr<DataShareOperationBuilder> builder =
        std::make_shared<DataShareOperationBuilder>(TYPE_DELETE, uri);
    return builder;
}

std::shared_ptr<DataShareOperationBuilder> DataShareOperation::NewAssertBuilder(const std::shared_ptr<Uri> &uri)
{
    LOG_DEBUG("Start");
    if (uri == nullptr) {
        LOG_ERROR("uri is nullptr");
        return nullptr;
    }
    std::shared_ptr<DataShareOperationBuilder> builder =
        std::make_shared<DataShareOperationBuilder>(TYPE_ASSERT, uri);
    return builder;
}

int DataShareOperation::GetType() const
{
    return type_;
}

std::shared_ptr<Uri> DataShareOperation::GetUri() const
{
    return uri_;
}

std::shared_ptr<DataShareValuesBucket> DataShareOperation::GetValuesBucket() const
{
    return valuesBucket_;
}

int DataShareOperation::GetExpectedCount() const
{
    return expectedCount_;
}

std::shared_ptr<DataSharePredicates> DataShareOperation::GetDataSharePredicates() const
{
    return dataSharePredicates_;
}

std::shared_ptr<DataShareValuesBucket> DataShareOperation::GetValuesBucketReferences() const
{
    return valuesBucketReferences_;
}
std::map<int, int> DataShareOperation::GetDataSharePredicatesBackReferences() const
{
    return dataSharePredicatesBackReferences_;
}
bool DataShareOperation::IsInsertOperation() const
{
    LOG_DEBUG("type_ == TYPE_INSERT : %{public}d", type_ == TYPE_INSERT);
    return type_ == TYPE_INSERT;
}
bool DataShareOperation::IsUpdateOperation() const
{
    LOG_DEBUG("type_ == TYPE_INSERT : %{public}d", type_ == TYPE_UPDATE);
    return type_ == TYPE_UPDATE;
}
bool DataShareOperation::IsDeleteOperation() const
{
    LOG_DEBUG("type_ == TYPE_INSERT : %{public}d", type_ == TYPE_DELETE);
    return type_ == TYPE_DELETE;
}
bool DataShareOperation::IsAssertOperation() const
{
    LOG_DEBUG("type_ == TYPE_INSERT : %{public}d", type_ == TYPE_ASSERT);
    return type_ == TYPE_ASSERT;
}
bool DataShareOperation::IsInterruptionAllowed() const
{
    LOG_DEBUG("interrupted_ : %{public}d", interrupted_);
    return interrupted_;
}
bool DataShareOperation::Marshalling(Parcel &out) const
{
    LOG_DEBUG("Start");
    if (!out.WriteInt32(type_)) {
        LOG_ERROR("Write type_ error");
        return false;
    }
    if (!out.WriteInt32(expectedCount_)) {
        LOG_ERROR("Write expectedCount_ error");
        return false;
    }

    if (!out.WriteBool(interrupted_)) {
        LOG_ERROR("Write interrupted_ error");
        return false;
    }

    if (!Marshalling(out, uri_) || !Marshalling(out, valuesBucket_) || !Marshalling(out, dataSharePredicates_) ||
        !Marshalling(out, valuesBucketReferences_) || !Marshalling(out, dataSharePredicatesBackReferences_)) {
        return false;
    }

    LOG_DEBUG("End successfully");
    return true;
}
DataShareOperation *DataShareOperation::Unmarshalling(Parcel &in)
{
    LOG_DEBUG("Start");
    DataShareOperation *dataShareOperation = new (std::nothrow) DataShareOperation();
    if (dataShareOperation != nullptr && !dataShareOperation->ReadFromParcel(in)) {
        LOG_ERROR("Read dataShareOperation error");
        delete dataShareOperation;
        dataShareOperation = nullptr;
    }
    LOG_DEBUG("End");
    return dataShareOperation;
}
bool DataShareOperation::ReadFromParcel(Parcel &in)
{
    LOG_DEBUG("Start");
    if (!in.ReadInt32(type_)) {
        LOG_ERROR("Read type_ error");
        return false;
    }
    if (!in.ReadInt32(expectedCount_)) {
        LOG_ERROR("Read expectedCount_ error");
        return false;
    }
    interrupted_ = in.ReadBool();
    if (!ReadFromParcel(in, uri_) || !ReadFromParcel(in, valuesBucket_) || !ReadFromParcel(in, dataSharePredicates_) ||
        !ReadFromParcel(in, valuesBucketReferences_) || !ReadFromParcel(in, dataSharePredicatesBackReferences_)) {
        return false;
    }
    return true;
}
std::shared_ptr<DataShareOperation> DataShareOperation::CreateFromParcel(Parcel &in)
{
    LOG_DEBUG("Start");
    std::shared_ptr<DataShareOperation> operation = std::make_shared<DataShareOperation>(in);
    return operation;
}
void DataShareOperation::PutMap(Parcel &in)
{
    LOG_DEBUG("Start");
    int count = in.ReadInt32();
    if (count > 0 && count < REFERENCE_THRESHOLD) {
        for (int i = 0; i < count; ++i) {
            dataSharePredicatesBackReferences_.insert(std::make_pair(in.ReadInt32(), in.ReadInt32()));
        }
        return;
    }
    LOG_WARN("count <= 0 or count >= REFERENCE_THRESHOLD");
}

bool DataShareOperation::Marshalling(Parcel &out, const std::shared_ptr<Uri> &uri) const
{
    if (uri != nullptr) {
        if (!out.WriteInt32(VALUE_OBJECT)) {
            LOG_ERROR("Write VALUE_OBJECT error");
            return false;
        }

        if (!out.WriteParcelable(uri.get())) {
            LOG_ERROR("Write uri error");
            return false;
        }
    } else {
        if (!out.WriteInt32(VALUE_NULL)) {
            LOG_ERROR("Write VALUE_NULL error");
            return false;
        }
    }
    return true;
}

bool DataShareOperation::ReadFromParcel(Parcel &in, std::shared_ptr<Uri> &uri)
{
    int isEmpty = VALUE_NULL;
    if (!in.ReadInt32(isEmpty)) {
        LOG_ERROR("Read isEmpty error");
        return false;
    }
    if (isEmpty == VALUE_OBJECT) {
        uri.reset(in.ReadParcelable<Uri>());
    } else {
        uri.reset();
    }
    return true;
}

bool DataShareOperation::Marshalling(Parcel &out, const std::shared_ptr<DataShareValuesBucket> &valuesBucket) const
{
    if (valuesBucket != nullptr) {
        if (!out.WriteInt32(VALUE_OBJECT)) {
            LOG_ERROR("Write VALUE_OBJECT error");
            return false;
        }

        if (!ITypesUtil::Marshalling(*valuesBucket, out)) {
            LOG_ERROR("Write valuesBucket error");
            return false;
        }
    } else {
        if (!out.WriteInt32(VALUE_NULL)) {
            LOG_ERROR("Write VALUE_NULL error");
            return false;
        }
    }
    return true;
}

bool DataShareOperation::ReadFromParcel(Parcel &in, std::shared_ptr<DataShareValuesBucket> &valuesBucket)
{
    int isEmpty = VALUE_NULL;
    if (!in.ReadInt32(isEmpty)) {
        LOG_ERROR("Read isEmpty error");
        return false;
    }
    if (isEmpty == VALUE_OBJECT) {
        DataShareValuesBucket vb;
        if (!ITypesUtil::Unmarshalling(in, vb)) {
            return false;
        }
        valuesBucket.reset(&vb);
    } else {
        valuesBucket.reset();
    }
    return true;
}

bool DataShareOperation::Marshalling(Parcel &out, const std::shared_ptr<DataSharePredicates> &dataSharePredicates) const
{
    if (dataSharePredicates != nullptr) {
        if (!out.WriteInt32(VALUE_OBJECT)) {
            LOG_ERROR("Write VALUE_OBJECT error");
            return false;
        }
        if (!ITypesUtil::Marshalling(*dataSharePredicates, out)) {
            LOG_ERROR("Write dataSharePredicates error");
            return false;
        }
    } else {
        if (!out.WriteInt32(VALUE_NULL)) {
            LOG_ERROR("Write VALUE_NULL error");
            return false;
        }
    }
    return true;
}

bool DataShareOperation::ReadFromParcel(Parcel &in, std::shared_ptr<DataSharePredicates> &dataSharePredicates)
{
    int isEmpty = VALUE_NULL;
    if (!in.ReadInt32(isEmpty)) {
        LOG_ERROR("Read isEmpty error");
        return false;
    }
    if (isEmpty == VALUE_OBJECT) {
        DataSharePredicates tmpPredicates;
        if (!ITypesUtil::Unmarshalling(in, tmpPredicates)) {
            return false;
        }
        dataSharePredicates.reset(&tmpPredicates);
    } else {
        dataSharePredicates.reset();
    }
    return true;
}

bool DataShareOperation::Marshalling(Parcel &out, const std::map<int, int> &dataSharePredicatesBackReferences) const
{
    int referenceSize = 0;
    if (!dataSharePredicatesBackReferences.empty()) {
        referenceSize = (int)dataSharePredicatesBackReferences.size();
        if (!out.WriteInt32(referenceSize)) {
            LOG_ERROR("Write referenceSize error");
            return false;
        }
        if (referenceSize >= REFERENCE_THRESHOLD) {
            LOG_INFO("referenceSize >= REFERENCE_THRESHOLD");
            return true;
        }
        for (auto &it : dataSharePredicatesBackReferences) {
            if (!out.WriteInt32(it.first)) {
                LOG_ERROR("Write first error");
                return false;
            }
            if (!out.WriteInt32(it.second)) {
                LOG_ERROR("Write second error");
                return false;
            }
        }
    } else {
        LOG_DEBUG("dataSharePredicatesBackReferences is empty");
        if (!out.WriteInt32(referenceSize)) {
            LOG_ERROR("Write referenceSize error");
            return false;
        }
    }
    return true;
}

bool DataShareOperation::ReadFromParcel(Parcel &in, std::map<int, int> &dataSharePredicatesBackReferences)
{
    int referenceSize = 0;
    if (!in.ReadInt32(referenceSize)) {
        LOG_ERROR("Read referenceSize error");
        return false;
    }
    if (referenceSize >= REFERENCE_THRESHOLD) {
        LOG_INFO("referenceSize >= REFERENCE_THRESHOLD");
        return true;
    }
    for (int i = 0; i < REFERENCE_THRESHOLD && i < referenceSize; ++i) {
        int first = 0;
        int second = 0;
        if (!in.ReadInt32(first)) {
            LOG_ERROR("Read first error");
            return false;
        }
        if (!in.ReadInt32(second)) {
            LOG_ERROR("Read second error");
            return false;
        }
        dataSharePredicatesBackReferences.insert(std::make_pair(first, second));
    }
    return true;
}
}  // namespace DataShare
}  // namespace OHOS