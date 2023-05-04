//
// Created by niudongyao on 2023/3/28.
//

#ifndef NAPI_RDB_OBSERVER_H
#define NAPI_RDB_OBSERVER_H

#include <uv.h>

#include "datashare_template.h"
#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace DataShare {
enum ObserverType {
    RDB = 0,
    PUBLISHED_DATA
};

class NapiObserver {
public:
    NapiObserver(napi_env env, napi_value callback);
    ~NapiObserver();
    virtual bool operator==(const NapiObserver &rhs) const;
    virtual bool operator!=(const NapiObserver &rhs) const;
    NapiObserver& operator=(NapiObserver &&rhs) = default;
protected:
    static void CallbackFunc(uv_work_t *work, int status);
    napi_env env_ = nullptr;
    napi_ref ref_ = nullptr;
    uv_loop_s *loop_ = nullptr;
};

class NapiRdbObserver final: public NapiObserver, public std::enable_shared_from_this<NapiRdbObserver> {
public:
    NapiRdbObserver(napi_env env, napi_value callback) : NapiObserver(env, callback) {};
    void OnChange(const RdbChangeNode &changeNode);

};

class NapiPublishedObserver final: public NapiObserver, public std::enable_shared_from_this<NapiPublishedObserver> {
public:
    NapiPublishedObserver(napi_env env, napi_value callback) : NapiObserver(env, callback) {};
    void OnChange(const PublishedDataChangeNode &changeNode);
};

struct ObserverWorker {
    std::weak_ptr<NapiRdbObserver> rdbObserver_;
    RdbChangeNode rdbChangeNode_;
    std::weak_ptr<NapiPublishedObserver> publisheObserver_;
    PublishedDataChangeNode publishedChangeNode_;
    ObserverType type_ = RDB;
    explicit ObserverWorker(std::shared_ptr<NapiRdbObserver> observerIn) : rdbObserver_(observerIn) {}
    explicit ObserverWorker(std::shared_ptr<NapiPublishedObserver> observerIn) : publisheObserver_(observerIn) {}
};
} // namespace DataShare
} // namespace OHOS
#endif //NAPI_RDB_OBSERVER_H
