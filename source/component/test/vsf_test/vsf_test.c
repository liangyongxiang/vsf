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
 *  See the License for the specific language governing permissions and       *
 *  limitations under the License.                                           *
 *                                                                           *
 *****************************************************************************/

/*============================ INCLUDES ======================================*/

#include "./vsf_test.h"
#include <string.h>

#if VSF_USE_TEST == ENABLED

#if VSF_TEST_CFG_USE_TRACE == ENABLED
#   if VSF_USE_TRACE != ENABLED
#       error "VSF_USE_TRACE must be ENABLED when VSF_TEST_CFG_USE_TRACE is ENABLED"
#   endif
#   include "service/trace/vsf_trace.h"
#endif

/*============================ MACROS ========================================*/

#if VSF_TEST_CFG_USE_TRACE == ENABLED
#   define __VSF_TEST_TRACE_INFO(...)    vsf_trace_info(__VA_ARGS__)
#   define __VSF_TEST_TRACE_DEBUG(...)   vsf_trace_debug(__VA_ARGS__)
#   define __VSF_TEST_TRACE_ERROR(...)   vsf_trace_error(__VA_ARGS__)
#else
#   define __VSF_TEST_TRACE_INFO(...)
#   define __VSF_TEST_TRACE_DEBUG(...)
#   define __VSF_TEST_TRACE_ERROR(...)
#endif

/*============================ MACROS ========================================*/
/*============================ TYPES =========================================*/
/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/

static vsf_test_t *__vsf_test_self;

#if VSF_TEST_CFG_EMIT_MARKERS == ENABLED
static void __emit_case_start(const char *suite_name, uint8_t case_idx, bool needs_ready)
{
    __VSF_TEST_TRACE_INFO("%s:CASE:%u\r\n", suite_name, (unsigned)case_idx);
    if (needs_ready) {
        __VSF_TEST_TRACE_INFO("%s:CASE:%u:READY\r\n", suite_name, (unsigned)case_idx);
    }
    vsf_test_busy_wait_ms(VSF_TEST_MARKER_DELAY_MS);
}

static void __emit_case_done(const char *suite_name, uint8_t case_idx)
{
    __VSF_TEST_TRACE_INFO("%s:CASE:%u:DONE\r\n", suite_name, (unsigned)case_idx);
}

static void __emit_suite_end(const char *suite_name)
{
    __VSF_TEST_TRACE_INFO("%s:END\r\n", suite_name);
}
#endif

/*============================ LOCAL FUNCTIONS ===============================*/

/*============================ IMPLEMENTATION ================================*/

void vsf_test_run(vsf_test_t *test)
{
    __vsf_test_self = test;

    __vsf_test_self->current_case = NULL;

    __VSF_TEST_TRACE_INFO("[TEST] Initialized (%u suites)\r\n", __vsf_test_self->suite_count);

    for (uint8_t i = 0; i < __vsf_test_self->wdt.count; i++) {
        if (__vsf_test_self->wdt.entries[i].init != NULL) {
            uint32_t timeout_ms = __vsf_test_self->wdt.entries[i].timeout_ms;
            if (timeout_ms == 0) {
                timeout_ms = (i == 0) ? VSF_TEST_CFG_INTERNAL_TIMEOUT_MS
                                      : VSF_TEST_CFG_EXTERNAL_TIMEOUT_MS;
            }
            __VSF_TEST_TRACE_DEBUG("[TEST] WDT[%u]: %u ms\r\n",
                                   (unsigned)i, (unsigned)timeout_ms);
            __vsf_test_self->wdt.entries[i].init(&__vsf_test_self->wdt.entries[i], timeout_ms);
        }
    }

    vsf_test_shell_init(&__vsf_test_self->shell,
                        __vsf_test_self->suites, __vsf_test_self->suite_count,
                        __vsf_test_self->instances, __vsf_test_self->instance_count);
    vsf_test_shell_run(&__vsf_test_self->shell);
}

void vsf_test_assert(vsf_test_result_t result,
                        const char *file_name, uint32_t line,
                        const char *function_name, const char *condition)
{
    vsf_test_case_t *case_ptr = __vsf_test_self->current_case;
    if (case_ptr != NULL) {
        case_ptr->result              = result;
        case_ptr->error.function_name = function_name;
        case_ptr->error.file_name     = file_name;
        case_ptr->error.condition     = condition;
        case_ptr->error.line          = line;
    }

    __VSF_TEST_TRACE_ERROR("[TEST] Assertion failed: %s:%u in %s() - %s\r\n",
                          file_name, line, function_name, condition ? condition : "");

    longjmp(*__vsf_test_self->jmp_buf, 1);
}

static const char *__vsf_test_get_name(vsf_test_case_t *test_case)
{
    if (test_case->suite != NULL && test_case->suite->name != NULL) {
        return test_case->suite->name;
    }
    return "unknown";
}

void vsf_test_reboot(vsf_test_result_t result,
                     const char *file_name, uint32_t line,
                     const char *function_name, const char *condition)
{
    vsf_test_case_t *case_ptr = __vsf_test_self->current_case;
    if (case_ptr != NULL) {
        case_ptr->result              = result;
        case_ptr->error.function_name = function_name;
        case_ptr->error.file_name     = file_name;
        case_ptr->error.condition     = condition;
        case_ptr->error.line          = line;
    }

    __VSF_TEST_TRACE_ERROR("[TEST] Reboot due to error: %s:%u in %s() - %s\r\n",
                          file_name, line, function_name, condition ? condition : "");

    for (uint8_t i = 0; i < __vsf_test_self->reboot.count; i++) {
        if (__vsf_test_self->reboot.entries[i] != NULL) {
            __VSF_TEST_TRACE_INFO("[TEST] Calling reboot[%u]\r\n", (unsigned)i);
            __vsf_test_self->reboot.entries[i]();
        }
    }
    while (1);
}

VSF_CAL_WEAK(vsf_test_busy_wait_ms)
void vsf_test_busy_wait_ms(uint32_t ms)
{
    for (volatile uint32_t i = 0; i < ms * VSF_TEST_CFG_BUSY_WAIT_CYCLES_PER_MS; i++);
}

VSF_CAL_WEAK(vsf_test_busy_wait_us)
void vsf_test_busy_wait_us(uint32_t us)
{
    for (volatile uint32_t i = 0; i < us * (VSF_TEST_CFG_BUSY_WAIT_CYCLES_PER_MS / 1000); i++);
}

void vsf_test_run_suite_case(vsf_test_suite_t *suite, uint16_t local_idx)
{
    if (suite == NULL || local_idx >= suite->case_count || suite->cases == NULL) {
        return;
    }
    vsf_test_case_t *test_case = &suite->cases[local_idx];
    test_case->suite = suite;

    // Skip cases explicitly marked (e.g., by setup returning false).
    if (test_case->result == VSF_TEST_RESULT_SKIP) {
        return;
    }

    for (uint8_t i = 0; i < __vsf_test_self->wdt.count; i++) {
        if (__vsf_test_self->wdt.entries[i].feed != NULL) {
            __vsf_test_self->wdt.entries[i].feed(&__vsf_test_self->wdt.entries[i]);
        }
    }

    test_case->error.function_name = NULL;
    test_case->error.file_name     = NULL;
    test_case->error.condition     = NULL;
    test_case->error.line          = 0;

    const char *test_name = __vsf_test_get_name(test_case);
    __VSF_TEST_TRACE_INFO("[TEST] Running '%s'\r\n", test_name);
#if VSF_TEST_CFG_EMIT_MARKERS == ENABLED
    __emit_case_start(suite->name, test_case->case_idx, test_case->needs_ready_handshake);
#endif

    __vsf_test_self->current_case = test_case;

    jmp_buf buf;
    test_case->result = VSF_TEST_RESULT_PASS;
    __vsf_test_self->jmp_buf = &buf;
    if (0 == setjmp(buf)) {
        test_case->jmp_fn(test_case->arg);
    }

#if VSF_TEST_CFG_EMIT_MARKERS == ENABLED
    __emit_case_done(suite->name, test_case->case_idx);
#endif

    __vsf_test_self->current_case = NULL;
}

static void __vsf_test_run_suite_all_cases(vsf_test_suite_t *suite)
{
    for (uint16_t i = 0; i < suite->case_count; i++) {
        vsf_test_run_suite_case(suite, i);
    }
}

void vsf_test_run_suite(vsf_test_suite_t *suite)
{
    if (suite == NULL || suite->cases == NULL) {
        return;
    }
    __vsf_test_run_suite_all_cases(suite);
#if VSF_TEST_CFG_EMIT_MARKERS == ENABLED
    __emit_suite_end(suite->name);
#endif
}

#endif
/* EOF */
