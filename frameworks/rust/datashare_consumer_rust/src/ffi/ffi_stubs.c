/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

/**
 * Weak stub definitions for C++ wrapper functions.
 *
 * The Connection_ConnectExt/Disconnect stubs forward at runtime to the real
 * implementations in libdatashare_consumer.z.so via dlopen+dlsym. This is
 * required because the dynamic linker with BIND_NOW resolves the JUMP_SLOT
 * for these symbols to the local weak definition before libdatashare_consumer.z.so
 * is fully loaded — RTLD_NEXT also fails to find the strong def because the
 * consumer lib is not on the "next" search path from this cdylib.
 *
 * The other stubs (AbilityMgr_*, DataObsMgr_*) return error codes; those callers
 * do not reach this file in working tests but are kept to satisfy the linker.
 */

#include <dlfcn.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

static void *LoadConsumerSym(const char *name)
{
    static void *handle = NULL;
    if (!handle) {
#ifdef __LP64__
        handle = dlopen("/system/lib64/platformsdk/libdatashare_consumer.z.so", RTLD_NOW);
#else
        handle = dlopen("/system/lib/platformsdk/libdatashare_consumer.z.so", RTLD_NOW);
#endif
    }
    if (!handle) {
        return NULL;
    }
    return dlsym(handle, name);
}

#pragma clang attribute push(__attribute__((weak)), apply_to = function)

int32_t DataShareAbilityMgrConnect(const char *uriPtr, uint32_t uriLen, void *connectRemote, void *callerToken)
{
    return -1;
}

int32_t DataShareAbilityMgrDisconnect(void *connectRemote)
{
    return -1;
}

typedef struct {
    void (*on_connect)(void *, void *, int32_t);
    void (*on_disconnect)(void *, int32_t);
} DataShareConnectionCallbacks;

void *DataShareConnectionConnectExt(const char *uriPtr, uint32_t uriLen,
    void *callerToken, void *context, const DataShareConnectionCallbacks *callbacks)
{
    typedef void *(*FnType)(const char *, uint32_t, void *, void *, const DataShareConnectionCallbacks *);
    static FnType realFn = NULL;
    if (!realFn) {
        realFn = (FnType)LoadConsumerSym("DataShareConnectionConnectExt");
        if ((void *)realFn == (void *)&DataShareConnectionConnectExt) {
            realFn = NULL;
        }
    }
    if (realFn) {
        return realFn(uriPtr, uriLen, callerToken, context, callbacks);
    }
    return NULL;
}

void DataShareConnectionDisconnect(void *connectionHandle)
{
    typedef void (*FnType)(void *);
    static FnType realFn = NULL;
    if (!realFn) {
        realFn = (FnType)LoadConsumerSym("DataShareConnectionDisconnect");
        if ((void *)realFn == (void *)&DataShareConnectionDisconnect) {
            realFn = NULL;
        }
    }
    if (realFn) {
        realFn(connectionHandle);
    }
}

int32_t DataShareRemoteObjSendRequest(void *remote, uint32_t code, void *data, void *reply)
{
    typedef int32_t (*FnType)(void *, uint32_t, void *, void *);
    static FnType realFn = NULL;
    if (!realFn) {
        realFn = (FnType)LoadConsumerSym("DataShareRemoteObjSendRequest");
        if ((void *)realFn == (void *)&DataShareRemoteObjSendRequest) {
            realFn = NULL;
        }
    }
    if (realFn) {
        return realFn(remote, code, data, reply);
    }
    return -1;
}

int32_t DataShareDataObsMgrRegisterObserver(const char *uriPtr, uint32_t uriLen, void *observerRemote)
{
    return -1;
}

int32_t DataShareDataObsMgrUnregisterObserver(const char *uriPtr, uint32_t uriLen, void *observerRemote)
{
    return -1;
}

int32_t DataShareDataObsMgrRegisterObserverSilent(const char *uriPtr, uint32_t uriLen, void *observerPtr)
{
    typedef int32_t (*FnType)(const char *, uint32_t, void *);
    static FnType realFn = NULL;
    if (!realFn) {
        realFn = (FnType)LoadConsumerSym("DataShareDataObsMgrRegisterObserverSilent");
        if ((void *)realFn == (void *)&DataShareDataObsMgrRegisterObserverSilent) {
            realFn = NULL;
        }
    }
    if (realFn) {
        return realFn(uriPtr, uriLen, observerPtr);
    }
    return -1;
}

int32_t DataShareDataObsMgrUnregisterObserverSilent(const char *uriPtr, uint32_t uriLen, void *observerPtr)
{
    typedef int32_t (*FnType)(const char *, uint32_t, void *);
    static FnType realFn = NULL;
    if (!realFn) {
        realFn = (FnType)LoadConsumerSym("DataShareDataObsMgrUnregisterObserverSilent");
        if ((void *)realFn == (void *)&DataShareDataObsMgrUnregisterObserverSilent) {
            realFn = NULL;
        }
    }
    if (realFn) {
        return realFn(uriPtr, uriLen, observerPtr);
    }
    return -1;
}

int32_t DataShareDataObsMgrNotifyChange(const char *uriPtr, uint32_t uriLen)
{
    return -1;
}

int32_t DataShareDataObsMgrRegisterObserverExt(
    const char *uriPtr, uint32_t uriLen, void *observerRemote, bool isDescendants)
{
    return -1;
}

int32_t DataShareDataObsMgrUnregisterObserverExt(const char *uriPtr, uint32_t uriLen, void *observerRemote)
{
    return -1;
}

int32_t DataShareDataObsMgrNotifyChangeExt(const uint8_t *changeInfoData, uint32_t changeInfoLen)
{
    return -1;
}

int32_t DataShareDataObsMgrRegisterObserverExtWithOption(
    const char *uriPtr, uint32_t uriLen, void *observerRemote, bool isDescendants, bool isSystem)
{
    return -1;
}

int32_t DataShareDataObsMgrUnregisterObserverExtWithOption(
    const char *uriPtr, uint32_t uriLen, void *observerRemote, bool isSystem)
{
    return -1;
}

int32_t DataShareDataObsMgrNotifyChangeExtWithOption(
    const uint8_t *changeInfoData, uint32_t changeInfoLen, bool isSystem)
{
    return -1;
}

int32_t DataShareProxyRegisterObserver(
    void *proxyRemote, const char *uriPtr, uint32_t uriLen, void *observerRemote)
{
    typedef int32_t (*FnType)(void *, const char *, uint32_t, void *);
    static FnType realFn = NULL;
    if (!realFn) {
        realFn = (FnType)LoadConsumerSym("DataShareProxyRegisterObserver");
        if ((void *)realFn == (void *)&DataShareProxyRegisterObserver) {
            realFn = NULL;
        }
    }
    if (realFn) {
        return realFn(proxyRemote, uriPtr, uriLen, observerRemote);
    }
    return 0;
}

int32_t DataShareProxyUnregisterObserver(
    void *proxyRemote, const char *uriPtr, uint32_t uriLen, void *observerRemote)
{
    typedef int32_t (*FnType)(void *, const char *, uint32_t, void *);
    static FnType realFn = NULL;
    if (!realFn) {
        realFn = (FnType)LoadConsumerSym("DataShareProxyUnregisterObserver");
        if ((void *)realFn == (void *)&DataShareProxyUnregisterObserver) {
            realFn = NULL;
        }
    }
    if (realFn) {
        return realFn(proxyRemote, uriPtr, uriLen, observerRemote);
    }
    return 0;
}

int32_t DataShareProxyRegisterObserverExtProvider(void *proxyRemote, const char *uriPtr, uint32_t uriLen,
    void *observerRemote, uint32_t flags)
{
    typedef int32_t (*FnType)(void *, const char *, uint32_t, void *, uint32_t);
    static FnType realFn = NULL;
    if (!realFn) {
        realFn = (FnType)LoadConsumerSym("DataShareProxyRegisterObserverExtProvider");
        if ((void *)realFn == (void *)&DataShareProxyRegisterObserverExtProvider) {
            realFn = NULL;
        }
    }
    if (realFn) {
        return realFn(proxyRemote, uriPtr, uriLen, observerRemote, flags);
    }
    return -1;
}

int32_t DataShareProxyUnregisterObserverExtProvider(
    void *proxyRemote, const char *uriPtr, uint32_t uriLen, void *observerRemote)
{
    typedef int32_t (*FnType)(void *, const char *, uint32_t, void *);
    static FnType realFn = NULL;
    if (!realFn) {
        realFn = (FnType)LoadConsumerSym("DataShareProxyUnregisterObserverExtProvider");
        if ((void *)realFn == (void *)&DataShareProxyUnregisterObserverExtProvider) {
            realFn = NULL;
        }
    }
    if (realFn) {
        return realFn(proxyRemote, uriPtr, uriLen, observerRemote);
    }
    return -1;
}

#pragma clang attribute pop
