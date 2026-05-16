/******************************************************************************
 *   Copyright(C)2009-2024 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/

#ifndef __VSF_TEST_SCENARIO_GATEWAY_H__
#define __VSF_TEST_SCENARIO_GATEWAY_H__

#include "vsf.h"

#ifdef __cplusplus
extern "C" {
#endif

// HELLO exchange. Returns true if host engaged (gating active),
// false on timeout (firmware runs everything).
bool scenario_gateway_init(void);

// Per-scenario gate. If gating is inactive, returns true.
// Otherwise sends READY?, waits for GO/SKIP, returns true on GO.
bool scenario_gateway(const char *name);

// Emit GATEWAY:DONE so the host can stop the dialog loop.
void scenario_gateway_done(void);

#ifdef __cplusplus
}
#endif

#endif
