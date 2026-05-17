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

#ifndef __VSF_TEST_SHELL_H__
#define __VSF_TEST_SHELL_H__

#include "vsf.h"

#ifdef __cplusplus
extern "C" {
#endif

// Register a scene name. Must be called before its cases are added.
// Returns the scene index (0-based).
uint8_t vsf_test_shell_register_scene(const char *name);

// Register a case (cfg_str) in the most recently registered scene.
// Must be called after vsf_test_shell_register_scene and after
// vsf_test_add_simple_case (or similar) for the same case.
void vsf_test_shell_register_case(const char *cfg_str);

// Print welcome banner and initial prompt.
void vsf_test_shell_init(void);

// Main read-eval-print loop. Blocks forever.
void vsf_test_shell_run(void);

// Register a scene AND add its cases unconditionally (no gating).
// Usage in register_all() functions:
//   VSF_TEST_REGISTER_SCENE(s, baud, vsf_test_usart_baud_add_cases);
#define VSF_TEST_REGISTER_SCENE(s, field, add_fn)    \
    do {                                              \
        vsf_test_shell_register_scene(#field);         \
        add_fn(&(s)->field);                          \
    } while (0)

#ifdef __cplusplus
}
#endif

#endif // __VSF_TEST_SHELL_H__
