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

#ifndef ANI_RS_BIND_H
#define ANI_RS_BIND_H

#include <memory>

#include "cxx.h"
#include "event_handler.h"
#include "event_runner.h"
#include "ani.h"

struct RustClosure;

uint32_t AniSendEvent(rust::box<RustClosure> closure, const rust::str name);

#endif