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

vsf_test_t __vsf_test;

/*============================ LOCAL FUNCTIONS ===============================*/

/*============================ IMPLEMENTATION ================================*/

void vsf_test_init(vsf_test_t *test, const vsf_test_cfg_t *cfg)
{
    // backward compat: test parameter kept but ignored;
    // framework uses global __vsf_test for constructor auto-registration.
    (void)test;

    VSF_ASSERT(cfg != NULL);

    __vsf_test.wdt.entries   = cfg->wdt.entries;
    __vsf_test.wdt.count     = cfg->wdt.count;

    __vsf_test.reboot.entries = cfg->reboot.entries;
    __vsf_test.reboot.count   = cfg->reboot.count;

    __vsf_test.restart_on_done = cfg->restart_on_done;

    __vsf_test.current_case = NULL;
    __vsf_test.suite_count = 0;

    __VSF_TEST_TRACE_INFO("[TEST] Initialized\r\n");
}

vsf_test_shell_t *vsf_test_get_shell(void)
{
    return &__vsf_test.shell;
}

bool vsf_test_register_suite(vsf_test_suite_t *suite)
{
    VSF_ASSERT(suite != NULL);
    VSF_ASSERT(suite->name != NULL);

    if (__vsf_test.suite_count >= VSF_TEST_SHELL_MAX_SUITES) {
        __VSF_TEST_TRACE_ERROR("register_suite: suite table full (count=%u)\r\n",
                               __vsf_test.suite_count);
        return false;
    }
    __vsf_test.suites[__vsf_test.suite_count++] = suite;
    vsf_test_shell_register_suite(&__vsf_test.shell, suite);
    __VSF_TEST_TRACE_DEBUG("[TEST] register suite '%s' at idx %u\r\n",
                          suite->name, (unsigned)(__vsf_test.suite_count - 1));
    return true;
}

void __vsf_test_longjmp(vsf_test_result_t result,
                        const char *file_name, uint32_t line,
                        const char *function_name, const char *condition)
{
    vsf_test_case_t *case_ptr = __vsf_test.current_case;
    if (case_ptr != NULL) {
        case_ptr->result              = result;
        case_ptr->error.function_name = function_name;
        case_ptr->error.file_name     = file_name;
        case_ptr->error.condition     = condition;
        case_ptr->error.line          = line;
    }

    __VSF_TEST_TRACE_ERROR("[TEST] Assertion failed: %s:%u in %s() - %s\r\n",
                          file_name, line, function_name, condition ? condition : "");

    longjmp(*__vsf_test.jmp_buf, 1);
}

//! \brief Extract test name from test case
static const char *__vsf_test_get_name(vsf_test_case_t *test_case, char *name_buf, size_t name_buf_size)
{
    (void)name_buf;
    (void)name_buf_size;
    if (test_case->suite != NULL && test_case->suite->name != NULL) {
        return test_case->suite->name;
    }
    return "unknown";
}

void vsf_test_reboot(vsf_test_result_t result,
                     const char *file_name, uint32_t line,
                     const char *function_name, const char *condition)
{
    vsf_test_case_t *case_ptr = __vsf_test.current_case;
    if (case_ptr != NULL) {
        case_ptr->result              = result;
        case_ptr->error.function_name = function_name;
        case_ptr->error.file_name     = file_name;
        case_ptr->error.condition     = condition;
        case_ptr->error.line          = line;
        case_ptr->status              = VSF_TEST_STATUS_IDLE;
    }

    __VSF_TEST_TRACE_ERROR("[TEST] Reboot due to error: %s:%u in %s() - %s\r\n",
                          file_name, line, function_name, condition ? condition : "");

    for (uint8_t i = 0; i < __vsf_test.reboot.count; i++) {
        if (__vsf_test.reboot.entries[i] != NULL) {
            __VSF_TEST_TRACE_INFO("[TEST] Calling reboot[%u]\r\n", (unsigned)i);
            __vsf_test.reboot.entries[i]();
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

    // WDT recovery: if a prior run was interrupted by WDT, status is still RUNNING.
    if (test_case->status == VSF_TEST_STATUS_RUNNING) {
        __VSF_TEST_TRACE_INFO("[TEST] suite '%s' case %u: WDT timeout detected\r\n",
                              suite->name, (unsigned)local_idx);
        test_case->result = test_case->expect_wdt ? VSF_TEST_RESULT_WDT_PASS
                                                  : VSF_TEST_RESULT_WDT_FAIL;
        test_case->status = VSF_TEST_STATUS_IDLE;
        return;
    }

    // Skip cases explicitly marked (e.g., by setup returning false).
    if (test_case->result == VSF_TEST_RESULT_SKIP) {
        return;
    }

    for (uint8_t i = 0; i < __vsf_test.wdt.count; i++) {
        if (__vsf_test.wdt.entries[i].feed != NULL) {
            __vsf_test.wdt.entries[i].feed(&__vsf_test.wdt.entries[i]);
        }
    }

    // Setup runs once before the first case of the suite.
    if (local_idx == 0) {
        if (suite->setup != NULL) {
            if (!suite->setup(suite)) {
                // Skip all cases in this suite
                for (uint16_t i = 0; i < suite->case_count; i++) {
                    suite->cases[i].result = VSF_TEST_RESULT_SKIP;
                }
                return;
            }
        }
    }

    test_case->error.function_name = NULL;
    test_case->error.file_name     = NULL;
    test_case->error.condition     = NULL;
    test_case->error.line          = 0;

    test_case->status = VSF_TEST_STATUS_RUNNING;

    static char name_buf[64];
    const char *test_name = __vsf_test_get_name(test_case, name_buf, sizeof(name_buf));
    __VSF_TEST_TRACE_INFO("[TEST] Running '%s'\r\n", test_name);
    /* Suite-aware dispatch: framework owns the start / DONE / END Capture
     * Markers and the setup / teardown lifecycle hooks. */
    __VSF_TEST_TRACE_INFO("%s:CASE:%u\r\n", suite->name, (unsigned)test_case->case_idx);
    if (test_case->needs_ready_handshake) {
        __VSF_TEST_TRACE_INFO("%s:CASE:%u:READY\r\n", suite->name, (unsigned)test_case->case_idx);
    }
    vsf_test_busy_wait_ms(VSF_TEST_MARKER_DELAY_MS);

    __vsf_test.current_case = test_case;

    jmp_buf buf;
    test_case->result = VSF_TEST_RESULT_PASS;
    __vsf_test.jmp_buf = &buf;
    if (0 == setjmp(buf)) {
        test_case->jmp_fn(test_case->arg);
    } else {
        if (test_case->expect_assert) {
            test_case->result = VSF_TEST_RESULT_PASS;
            test_case->error.function_name = NULL;
            test_case->error.file_name     = NULL;
            test_case->error.condition     = NULL;
            test_case->error.line          = 0;
        }
    }

    __VSF_TEST_TRACE_INFO("%s:CASE:%u:DONE\r\n", suite->name, (unsigned)test_case->case_idx);
    // Teardown runs once after the last case.
    if (local_idx == suite->case_count - 1) {
        if (suite->teardown != NULL) suite->teardown(suite);
        /* Scenario-level boundary marker. Host decoders use this to bound
         * the last case's payload window; without it, the last case has
         * no upper bound and decode would extend to end-of-capture,
         * picking up unrelated bytes from later suites. */
        __VSF_TEST_TRACE_INFO("%s:END\r\n", suite->name);
    }

    test_case->status = VSF_TEST_STATUS_IDLE;
    __vsf_test.current_case = NULL;
}

void vsf_test_run_suite(vsf_test_suite_t *suite)
{
    if (suite == NULL || suite->cases == NULL) {
        return;
    }

    for (uint16_t i = 0; i < suite->case_count; i++) {
        vsf_test_run_suite_case(suite, i);
    }
}

void vsf_test_run_tests(void)
{
    __VSF_TEST_TRACE_INFO("[TEST] Starting test framework\r\n");

    for (uint8_t i = 0; i < __vsf_test.wdt.count; i++) {
        if (__vsf_test.wdt.entries[i].init != NULL) {
            uint32_t timeout_ms = __vsf_test.wdt.entries[i].timeout_ms;
            if (timeout_ms == 0) {
                timeout_ms = (i == 0) ? VSF_TEST_CFG_INTERNAL_TIMEOUT_MS
                                      : VSF_TEST_CFG_EXTERNAL_TIMEOUT_MS;
            }
            __VSF_TEST_TRACE_DEBUG("[TEST] WDT[%u]: %u ms\r\n",
                                   (unsigned)i, (unsigned)timeout_ms);
            __vsf_test.wdt.entries[i].init(&__vsf_test.wdt.entries[i], timeout_ms);
        }
    }

    for (uint8_t si = 0; si < __vsf_test.suite_count; si++) {
        vsf_test_run_suite(__vsf_test.suites[si]);
    }

    __VSF_TEST_TRACE_INFO("[TEST] All test cases completed\r\n");

    __VSF_TEST_TRACE_INFO("\r\n[TEST] ========== Test Summary ==========\r\n");

    uint32_t total = 0, pass_count = 0, fail_count = 0, skip_count = 0;
    uint32_t wdt_pass_count = 0, wdt_fail_count = 0;

    for (uint8_t si = 0; si < __vsf_test.suite_count; si++) {
        vsf_test_suite_t *suite = __vsf_test.suites[si];
        if (suite == NULL || suite->cases == NULL) continue;
        for (uint16_t ci = 0; ci < suite->case_count; ci++) {
            total++;
            switch (suite->cases[ci].result) {
            case VSF_TEST_RESULT_PASS:     pass_count++;      break;
            case VSF_TEST_RESULT_FAIL:     fail_count++;      break;
            case VSF_TEST_RESULT_SKIP:     skip_count++;      break;
            case VSF_TEST_RESULT_WDT_PASS: wdt_pass_count++;  break;
            case VSF_TEST_RESULT_WDT_FAIL: wdt_fail_count++;  break;
            default: break;
            }
        }
    }

    __VSF_TEST_TRACE_INFO("[TEST] Total test cases: %u\r\n", total);
    __VSF_TEST_TRACE_INFO("[TEST] Pass: %u, Fail: %u, Skip: %u, WDT Pass: %u, WDT Fail: %u\r\n",
                          pass_count, fail_count, skip_count, wdt_pass_count, wdt_fail_count);
}

#endif
/* EOF */
