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

static vsf_test_t *__vsf_test = NULL;

/*============================ LOCAL FUNCTIONS ===============================*/

/*============================ IMPLEMENTATION ================================*/

void vsf_test_init(vsf_test_t *test, const vsf_test_cfg_t *cfg)
{
    VSF_ASSERT(test != NULL);
    __vsf_test = test;

    VSF_ASSERT(cfg != NULL);

    __vsf_test->wdt.internal = cfg->wdt.internal;
    __vsf_test->wdt.external = cfg->wdt.external;

    __vsf_test->reboot.internal = cfg->reboot.internal;
    __vsf_test->reboot.external = cfg->reboot.external;

    __vsf_test->restart_on_done = cfg->restart_on_done;

    __vsf_test->current_case = NULL;
    __vsf_test->test_case_count = 0;

    __VSF_TEST_TRACE_INFO("[TEST] Initialized with capacity %u\r\n", VSF_TEST_CFG_ARRAY_SIZE);
}

vsf_test_shell_t *vsf_test_get_shell(void)
{
    return &__vsf_test->shell;
}

bool vsf_test_register_suite(vsf_test_suite_t *suite)
{
    VSF_ASSERT(suite != NULL);
    VSF_ASSERT(suite->name != NULL);
    suite->first_case_idx = (uint16_t)__vsf_test->test_case_count;
    suite->case_count     = 0;
    vsf_test_shell_register_suite(&__vsf_test->shell, suite->name);
    __VSF_TEST_TRACE_DEBUG("[TEST] register suite '%s' at idx %u\r\n",
                          suite->name, (unsigned)suite->first_case_idx);
    return true;
}

bool vsf_test_suite_add_case_ex(vsf_test_suite_t *suite,
                                vsf_test_jmp_fn_t *jmp_fn,
                                void *arg,
                                bool needs_ready_handshake)
{
    VSF_ASSERT(suite != NULL);
    if (__vsf_test->test_case_count >= VSF_TEST_CFG_ARRAY_SIZE) {
        __VSF_TEST_TRACE_ERROR("suite_add_case: array full (count=%u, cap=%u)\r\n",
                               __vsf_test->test_case_count, VSF_TEST_CFG_ARRAY_SIZE);
        VSF_ASSERT(0);
        return true;
    }

    vsf_test_case_t *test_case = &__vsf_test->test_case_array[__vsf_test->test_case_count];
    test_case->jmp_fn                = jmp_fn;
    test_case->type                  = VSF_TEST_TYPE_LONGJMP_FN;
    test_case->expect_wdt            = 0;
    test_case->expect_assert         = 0;
    test_case->case_idx              = (uint8_t)suite->case_count;
    test_case->suite                 = suite;
    test_case->arg                   = arg;
    test_case->needs_ready_handshake = needs_ready_handshake;
    test_case->status                = VSF_TEST_STATUS_IDLE;
    test_case->result                = VSF_TEST_RESULT_PASS;

    __VSF_TEST_TRACE_DEBUG("suite_add_case: added case at global idx %u, suite='%s', local=%u\r\n",
                           __vsf_test->test_case_count, suite->name,
                           (unsigned)test_case->case_idx);
    vsf_test_shell_inc_case_count(&__vsf_test->shell);
    __vsf_test->test_case_count++;
    suite->case_count++;
    return false;
}

bool vsf_test_suite_add_case(vsf_test_suite_t *suite,
                             vsf_test_jmp_fn_t *jmp_fn,
                             void *arg)
{
    return vsf_test_suite_add_case_ex(suite, jmp_fn, arg, false);
}

void __vsf_test_longjmp(vsf_test_result_t result,
                        const char *file_name, uint32_t line,
                        const char *function_name, const char *condition)
{
    vsf_test_case_t *case_ptr = __vsf_test->current_case;
    if (case_ptr != NULL) {
        case_ptr->result              = result;
        case_ptr->error.function_name = function_name;
        case_ptr->error.file_name     = file_name;
        case_ptr->error.condition     = condition;
        case_ptr->error.line          = line;
    }

    __VSF_TEST_TRACE_ERROR("[TEST] Assertion failed: %s:%u in %s() - %s\r\n",
                          file_name, line, function_name, condition ? condition : "");

    longjmp(*__vsf_test->jmp_buf, 1);
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
    vsf_test_case_t *case_ptr = __vsf_test->current_case;
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

    if (__vsf_test->reboot.external != NULL) {
        __VSF_TEST_TRACE_INFO("[TEST] Calling external reboot\r\n");
        __vsf_test->reboot.external();
    }
    if (__vsf_test->reboot.internal != NULL) {
        __VSF_TEST_TRACE_INFO("[TEST] Calling internal reboot\r\n");
        __vsf_test->reboot.internal();
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

void vsf_test_run_case(uint32_t idx)
{
    if (idx >= __vsf_test->test_case_count) {
        return;
    }

    vsf_test_case_t *test_case = &__vsf_test->test_case_array[idx];

    // WDT recovery: if a prior run was interrupted by WDT, status is still RUNNING.
    if (test_case->status == VSF_TEST_STATUS_RUNNING) {
        __VSF_TEST_TRACE_INFO("[TEST] #%u: WDT timeout detected\r\n", idx);
        test_case->result = test_case->expect_wdt ? VSF_TEST_RESULT_WDT_PASS
                                                  : VSF_TEST_RESULT_WDT_FAIL;
        test_case->status = VSF_TEST_STATUS_IDLE;
        return;
    }

    // Skip cases explicitly marked (e.g., by setup returning false).
    if (test_case->result == VSF_TEST_RESULT_SKIP) {
        return;
    }

    if (__vsf_test->wdt.internal.feed != NULL) {
        __vsf_test->wdt.internal.feed(&__vsf_test->wdt.internal);
    }
    if (__vsf_test->wdt.external.feed != NULL) {
        __vsf_test->wdt.external.feed(&__vsf_test->wdt.external);
    }

    vsf_test_suite_t *suite = test_case->suite;
    if (suite != NULL && (uint32_t)idx == suite->first_case_idx) {
        if (suite->setup != NULL) {
            if (!suite->setup(suite)) {
                // Skip all cases in this suite
                for (uint16_t i = suite->first_case_idx;
                     i < suite->first_case_idx + suite->case_count; i++) {
                    __vsf_test->test_case_array[i].result = VSF_TEST_RESULT_SKIP;
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
    __VSF_TEST_TRACE_INFO("[TEST] #%u: Running '%s'\r\n", idx, test_name);

    /* Suite-aware dispatch: framework owns the start / DONE / END Capture
     * Markers and the setup / teardown lifecycle hooks. */
    if (suite != NULL) {
        __VSF_TEST_TRACE_INFO("%s:CASE:%u\r\n", suite->name, (unsigned)test_case->case_idx);
        if (test_case->needs_ready_handshake) {
            __VSF_TEST_TRACE_INFO("%s:CASE:%u:READY\r\n", suite->name, (unsigned)test_case->case_idx);
        }
        vsf_test_busy_wait_ms(VSF_TEST_MARKER_DELAY_MS);
    }

    __vsf_test->current_case = test_case;

    jmp_buf buf;
    test_case->result = VSF_TEST_RESULT_PASS;
    __vsf_test->jmp_buf = &buf;
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

    if (suite != NULL) {
        __VSF_TEST_TRACE_INFO("%s:CASE:%u:DONE\r\n", suite->name, (unsigned)test_case->case_idx);
        if ((uint32_t)idx == (uint32_t)suite->first_case_idx + (uint32_t)suite->case_count - 1) {
            if (suite->teardown != NULL) suite->teardown(suite);
            /* Scenario-level boundary marker. Host decoders use this to bound
             * the last case's payload window; without it, the last case has
             * no upper bound and decode would extend to end-of-capture,
             * picking up unrelated bytes from later suites. */
            __VSF_TEST_TRACE_INFO("%s:END\r\n", suite->name);
        }
    }

    test_case->status = VSF_TEST_STATUS_IDLE;
    __vsf_test->current_case = NULL;
}

void vsf_test_run_suite(vsf_test_suite_t *suite)
{
    if (suite == NULL) {
        return;
    }

    for (uint16_t i = 0; i < suite->case_count; i++) {
        uint32_t global_idx = (uint32_t)suite->first_case_idx + i;
        vsf_test_run_case(global_idx);
    }
}

void vsf_test_run_tests(void)
{
    __VSF_TEST_TRACE_INFO("[TEST] Starting test framework\r\n");

    if (__vsf_test->wdt.internal.init != NULL) {
        uint32_t timeout_ms = __vsf_test->wdt.internal.timeout_ms;
        if (timeout_ms == 0) {
            timeout_ms = VSF_TEST_CFG_INTERNAL_TIMEOUT_MS;
        }
        __VSF_TEST_TRACE_DEBUG("[TEST] Internal WDT: %u ms\r\n", timeout_ms);
        __vsf_test->wdt.internal.init(&__vsf_test->wdt.internal, timeout_ms);
    }
    if (__vsf_test->wdt.external.init != NULL) {
        uint32_t timeout_ms = __vsf_test->wdt.external.timeout_ms;
        if (timeout_ms == 0) {
            timeout_ms = VSF_TEST_CFG_EXTERNAL_TIMEOUT_MS;
        }
        __VSF_TEST_TRACE_DEBUG("[TEST] External WDT: %u ms\r\n", timeout_ms);
        __vsf_test->wdt.external.init(&__vsf_test->wdt.external, timeout_ms);
    }

    for (uint32_t i = 0; i < __vsf_test->test_case_count; i++) {
        vsf_test_run_case(i);
    }

    __VSF_TEST_TRACE_INFO("[TEST] All test cases completed\r\n");

    __VSF_TEST_TRACE_INFO("\r\n[TEST] ========== Test Summary ==========\r\n");
    __VSF_TEST_TRACE_INFO("[TEST] Total test cases: %u\r\n", __vsf_test->test_case_count);

    uint32_t pass_count = 0, fail_count = 0, skip_count = 0, wdt_pass_count = 0, wdt_fail_count = 0;

    for (uint32_t i = 0; i < __vsf_test->test_case_count; i++) {
        vsf_test_result_t result = __vsf_test->test_case_array[i].result;

        switch (result) {
        case VSF_TEST_RESULT_PASS:     pass_count++;      break;
        case VSF_TEST_RESULT_FAIL:     fail_count++;      break;
        case VSF_TEST_RESULT_SKIP:     skip_count++;      break;
        case VSF_TEST_RESULT_WDT_PASS: wdt_pass_count++;  break;
        case VSF_TEST_RESULT_WDT_FAIL: wdt_fail_count++;  break;
        default: break;
        }
    }

    __VSF_TEST_TRACE_INFO("[TEST] Pass: %u, Fail: %u, Skip: %u, WDT Pass: %u, WDT Fail: %u\r\n",
                          pass_count, fail_count, skip_count, wdt_pass_count, wdt_fail_count);
}

vsf_test_result_t vsf_test_get_case_result(uint32_t idx)
{
    if (__vsf_test == NULL || idx >= __vsf_test->test_case_count) {
        return VSF_TEST_RESULT_SKIP;
    }
    return (vsf_test_result_t)__vsf_test->test_case_array[idx].result;
}

uint32_t vsf_test_get_case_count(void)
{
    return __vsf_test ? __vsf_test->test_case_count : 0;
}

#endif
/* EOF */
