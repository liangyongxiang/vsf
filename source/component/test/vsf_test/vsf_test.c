/*****************************************************************************
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

/*============================ INCLUDES ======================================*/

#include "./vsf_test.h"
#include <string.h>
#if VSF_TEST_CFG_USE_FILE_DATA_SYNC == ENABLED
#   include "./port/vsf_test_port_file.h"
#endif
#if VSF_TEST_CFG_USE_APPCFG_DATA_SYNC == ENABLED
#   include "./port/vsf_test_port_appcfg.h"
#endif

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

static void __vsf_test_data_sync(vsf_test_data_t *data, vsf_test_data_cmd_t cmd)
{
    if (data == NULL) {
        __VSF_TEST_TRACE_ERROR("[TEST] Warning: data is NULL in __vsf_test_data_sync\r\n");
        return;
    }
    if (data->sync == NULL) {
        __VSF_TEST_TRACE_ERROR("[TEST] Warning: data->sync is NULL in __vsf_test_data_sync (cmd=%u)\r\n", cmd);
        return;
    }
    data->sync(data, cmd);
}

/*============================ IMPLEMENTATION ================================*/

void vsf_test_init(vsf_test_t *test, const vsf_test_cfg_t *cfg)
{
    VSF_ASSERT(test != NULL);
    __vsf_test = test;

    VSF_ASSERT(cfg != NULL);

    // 配置看门狗
    __vsf_test->wdt.internal = cfg->wdt.internal;
    __vsf_test->wdt.external = cfg->wdt.external;

    // 配置复位函数
    __vsf_test->reboot.internal = cfg->reboot.internal;
    __vsf_test->reboot.external = cfg->reboot.external;

    // 配置数据持久化
    __vsf_test->data.init = cfg->data.init;
    __vsf_test->data.sync = cfg->data.sync;

    // 配置完成时重启选项
    __vsf_test->restart_on_done = cfg->restart_on_done;

    // 初始化测试用例计数
    __vsf_test->test_case_count = 0;

    // 初始化数据同步
    if (__vsf_test->data.init != NULL) {
        __vsf_test->data.init(&__vsf_test->data);
    }

    __VSF_TEST_TRACE_INFO("[TEST] Initialized with capacity %u\r\n", VSF_TEST_CFG_ARRAY_SIZE);
}

vsf_test_shell_t *vsf_test_get_shell(void)
{
    return &__vsf_test->shell;
}

bool vsf_test_add_ex(vsf_test_case_t *test_case)
{
    if (__vsf_test->test_case_count < VSF_TEST_CFG_ARRAY_SIZE) {
        __vsf_test->test_case_array[__vsf_test->test_case_count] = *test_case;
        __VSF_TEST_TRACE_DEBUG("vsf_test_add_ex: added test case at index %u, type=%u\r\n",
                              __vsf_test->test_case_count, test_case->type);
        vsf_test_shell_register_case(&__vsf_test->shell, test_case->cfg_str);
        __vsf_test->test_case_count++;
        return false;
    } else {
        __VSF_TEST_TRACE_ERROR("vsf_test_add_ex: test case array is full (count=%u, capacity=%u)\r\n",
                              __vsf_test->test_case_count, VSF_TEST_CFG_ARRAY_SIZE);
        VSF_ASSERT(0);
        return true;
    }
}

bool vsf_test_add_simple_case(vsf_test_jmp_fn_t *jmp_fn, char *cfg_str, void *arg)
{
    return vsf_test_add_case(jmp_fn, cfg_str, 0, arg);
}

bool vsf_test_add_bool_fn(vsf_test_bool_fn_t *b_fn, char *cfg_str, void *arg)
{
    vsf_test_case_t test_case = {
        .b_fn       = b_fn,
        .type       = VSF_TEST_TYPE_BOOL_FN,
        .expect_wdt = 0,
        .cfg_str    = cfg_str,
        .arg        = arg,
    };

    __VSF_TEST_TRACE_DEBUG("vsf_test_add_bool_fn: adding BOOL_FN test case, cfg_str=%s\r\n",
                          cfg_str ? cfg_str : "NULL");

    return vsf_test_add_ex(&test_case);
}

bool vsf_test_add_case(vsf_test_jmp_fn_t *fn, char *cfg, uint8_t expect_wdt, void *arg)
{
    vsf_test_case_t test_case = {
        .jmp_fn      = fn,
        .cfg_str     = cfg,
        .type        = VSF_TEST_TYPE_LONGJMP_FN,
        .expect_wdt  = expect_wdt,
        .expect_assert = 0,
        .arg         = arg,
    };
    return vsf_test_add_ex(&test_case);
}

bool vsf_test_add_bool_fn_case(vsf_test_bool_fn_t *fn, char *cfg, uint8_t expect_wdt, void *arg)
{
    vsf_test_case_t test_case = {
        .b_fn       = fn,
        .cfg_str    = cfg,
        .type       = VSF_TEST_TYPE_BOOL_FN,
        .expect_wdt = expect_wdt,
        .expect_assert = 0,
        .arg        = arg,
    };
    return vsf_test_add_ex(&test_case);
}

bool vsf_test_add_ex_case(vsf_test_jmp_fn_t *fn, char *cfg,
                          vsf_test_type_t type,
                          uint8_t expect_wdt,
                          uint8_t expect_assert,
                          void *arg)
{
    vsf_test_case_t test_case = {
        .jmp_fn      = fn,
        .cfg_str     = cfg,
        .type        = type,
        .expect_wdt  = expect_wdt,
        .expect_assert = expect_assert,
        .arg         = arg,
    };
    return vsf_test_add_ex(&test_case);
}

bool vsf_test_add_expect_assert_case(vsf_test_jmp_fn_t *fn,
                                     char *cfg,
                                     uint8_t expect_wdt,
                                     void *arg)
{
    vsf_test_case_t test_case = {
        .jmp_fn      = fn,
        .cfg_str     = cfg,
        .type        = VSF_TEST_TYPE_LONGJMP_FN,
        .expect_wdt  = expect_wdt,
        .expect_assert = 1,
        .arg         = arg,
    };
    return vsf_test_add_ex(&test_case);
}

bool vsf_test_register_suite(vsf_test_suite_t *suite)
{
    VSF_ASSERT(suite != NULL);
    VSF_ASSERT(suite->name != NULL);
    suite->first_case_idx = (uint16_t)__vsf_test->test_case_count;
    suite->case_count     = 0;
    // Open a shell-side scene under the same name so `vsf-test scene --list`
    // / `vsf-test run <name>` see the suite without a second registration
    // call. The next vsf_test_add_ex() (via vsf_test_suite_add_case) will
    // attribute its case to this scene.
    vsf_test_shell_register_scene(&__vsf_test->shell, suite->name);
    __VSF_TEST_TRACE_DEBUG("[TEST] register suite '%s' at idx %u\r\n",
                          suite->name, (unsigned)suite->first_case_idx);
    return true;
}

bool vsf_test_suite_add_case(vsf_test_suite_t *suite,
                             vsf_test_jmp_fn_t *jmp_fn,
                             void *arg)
{
    VSF_ASSERT(suite != NULL);
    vsf_test_case_t test_case = {
        .jmp_fn        = jmp_fn,
        .cfg_str       = (char *)suite->name,    // legacy: shown in [TEST] # N: Running '...'
        .type          = VSF_TEST_TYPE_LONGJMP_FN,
        .expect_wdt    = 0,
        .expect_assert = 0,
        .case_idx      = (uint8_t)suite->case_count,
        .suite         = suite,
        .arg           = arg,
    };
    bool err = vsf_test_add_ex(&test_case);
    if (!err) {
        suite->case_count++;
    }
    return err;
}

void __vsf_test_longjmp(vsf_test_result_t result,
                        const char *file_name, uint32_t line,
                        const char *function_name, const char *condition)
{
    vsf_test_data_t *data     = &__vsf_test->data;
    data->result              = result;
    data->error.function_name = function_name;
    data->error.file_name     = file_name;
    data->error.condition     = condition;
    data->error.line          = line;

    __VSF_TEST_TRACE_ERROR("[TEST] Assertion failed: %s:%u in %s() - %s\r\n",
                          file_name, line, function_name, condition ? condition : "");

    longjmp(*__vsf_test->jmp_buf, 1);
}

//! \brief 从 test case 中提取测试名字
static const char *__vsf_test_get_name(vsf_test_case_t *test_case, char *name_buf, size_t name_buf_size)
{
    if (test_case->cfg_str != NULL) {
        // cfg_str 格式通常是 "test_name purpose=... hw_req=..."
        const char *space = strchr(test_case->cfg_str, ' ');
        if (space != NULL) {
            size_t len = space - test_case->cfg_str;
            if (len < name_buf_size) {
                strncpy(name_buf, test_case->cfg_str, len);
                name_buf[len] = '\0';
                return name_buf;
            } else {
                strncpy(name_buf, test_case->cfg_str, name_buf_size - 1);
                name_buf[name_buf_size - 1] = '\0';
                return name_buf;
            }
        } else {
            strncpy(name_buf, test_case->cfg_str, name_buf_size - 1);
            name_buf[name_buf_size - 1] = '\0';
            return name_buf;
        }
    } else {
        strncpy(name_buf, "unknown", name_buf_size - 1);
        name_buf[name_buf_size - 1] = '\0';
        return name_buf;
    }
}

void vsf_test_reboot(vsf_test_result_t result,
                     const char *file_name, uint32_t line,
                     const char *function_name, const char *condition)
{
    vsf_test_data_t *data     = &__vsf_test->data;
    data->result              = result;
    data->error.function_name = function_name;
    data->error.file_name     = file_name;
    data->error.condition     = condition;
    data->error.line          = line;

    __VSF_TEST_TRACE_ERROR("[TEST] Reboot due to error: %s:%u in %s() - %s\r\n",
                          file_name, line, function_name, condition ? condition : "");

    __vsf_test_data_sync(data, VSF_TEST_TESTCASE_RESULT_WRITE);

    data->status = VSF_TEST_STATUS_IDLE;
    __vsf_test_data_sync(data, VSF_TEST_STATUS_WRITE);

    data->idx++;
    __vsf_test_data_sync(data, VSF_TEST_TESTCASE_INDEX_WRITE);

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

void vsf_test_run_case(uint32_t idx)
{
    if (idx >= __vsf_test->test_case_count) {
        return;
    }

    vsf_test_data_t *data = &__vsf_test->data;
    vsf_test_case_t *test_case = &__vsf_test->test_case_array[idx];

    data->idx = idx;
    __vsf_test_data_sync(data, VSF_TEST_TESTCASE_INDEX_WRITE);

    __vsf_test_data_sync(data, VSF_TEST_STATUS_READ);
    if (data->status != VSF_TEST_STATUS_IDLE) {
        __VSF_TEST_TRACE_INFO("[TEST] #%u: WDT timeout detected\r\n", idx);
        data->result = test_case->expect_wdt ? VSF_TEST_RESULT_WDT_PASS
                                             : VSF_TEST_RESULT_WDT_FAIL;
        __vsf_test_data_sync(data, VSF_TEST_TESTCASE_RESULT_WRITE);
        data->status = VSF_TEST_STATUS_IDLE;
        __vsf_test_data_sync(data, VSF_TEST_STATUS_WRITE);
        return;
    }

    if (__vsf_test->wdt.internal.feed != NULL) {
        __vsf_test->wdt.internal.feed(&__vsf_test->wdt.internal);
    }
    if (__vsf_test->wdt.external.feed != NULL) {
        __vsf_test->wdt.external.feed(&__vsf_test->wdt.external);
    }

    if (test_case->cfg_str != NULL) {
        data->request_str = test_case->cfg_str;
        __vsf_test_data_sync(data, VSF_TEST_TESECASE_REQUEST_WRITE);
        if (data->req_continue == VSF_TEST_REQ_NO_SUPPORT) {
            __VSF_TEST_TRACE_INFO("[TEST] #%u: Not supported, skipping\r\n", idx);
            data->result = VSF_TEST_RESULT_SKIP;
            __vsf_test_data_sync(data, VSF_TEST_TESTCASE_RESULT_WRITE);
            return;
        }
    }

    data->error.function_name = NULL;
    data->error.file_name     = NULL;
    data->error.condition     = NULL;
    data->error.line          = 0;

    data->status = VSF_TEST_STATUS_RUNNING;
    __vsf_test_data_sync(data, VSF_TEST_STATUS_WRITE);

    static char name_buf[64];
    const char *test_name = __vsf_test_get_name(test_case, name_buf, sizeof(name_buf));
    __VSF_TEST_TRACE_INFO("[TEST] #%u: Running '%s'\r\n", idx, test_name);

    /* Suite-aware dispatch: when a case has been registered through
     * vsf_test_suite_add_case(), the framework owns the start / DONE Capture
     * Markers (no per-scenario vsf_trace_info needed) and the setup /
     * teardown lifecycle hooks. */
    vsf_test_suite_t *suite = test_case->suite;
    if (suite != NULL) {
        if ((uint32_t)idx == suite->first_case_idx) {
            if (suite->setup != NULL) suite->setup(suite);
        }
        __VSF_TEST_TRACE_INFO("%s:CASE:%u\r\n", suite->name, (unsigned)test_case->case_idx);
        /* Brief settle delay so the marker bytes are fully on the UART line
         * before any test transmission begins. 2 ms covers a ~17-char marker
         * at 115200 baud with margin. */
        vsf_test_busy_wait_ms(2);
    }

    vsf_test_type_t type = test_case->type;
    switch (type) {
    case VSF_TEST_TYPE_BOOL_FN:
        data->result = test_case->b_fn(test_case->arg);
        break;
    case VSF_TEST_TYPE_LONGJMP_FN: {
        jmp_buf buf;
        data->result  = VSF_TEST_RESULT_PASS;
        __vsf_test->jmp_buf = &buf;
        if (0 == setjmp(buf)) {
            test_case->jmp_fn(test_case->arg);
        } else {
            if (test_case->expect_assert) {
                data->result = VSF_TEST_RESULT_PASS;
                data->error.function_name = NULL;
                data->error.file_name     = NULL;
                data->error.condition     = NULL;
                data->error.line          = 0;
            }
        }
    } break;
    default:
        VSF_ASSERT(0);
        break;
    }

    if (suite != NULL) {
        __VSF_TEST_TRACE_INFO("%s:CASE:%u:DONE\r\n", suite->name, (unsigned)test_case->case_idx);
        if ((uint32_t)idx == (uint32_t)suite->first_case_idx + (uint32_t)suite->case_count - 1) {
            if (suite->teardown != NULL) suite->teardown(suite);
        }
    }

    __vsf_test_data_sync(data, VSF_TEST_TESTCASE_RESULT_WRITE);
    data->status = VSF_TEST_STATUS_IDLE;
    __vsf_test_data_sync(data, VSF_TEST_STATUS_WRITE);
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

    vsf_test_data_t *data = &__vsf_test->data;
    if (data->init != NULL) {
        data->init(data);
    }

    if (__vsf_test->restart_on_done) {
        data->idx = 0;
        __vsf_test_data_sync(data, VSF_TEST_TESTCASE_INDEX_WRITE);
        __VSF_TEST_TRACE_INFO("[TEST] Restart on done: starting from test case #0\r\n");
    } else {
        __vsf_test_data_sync(data, VSF_TEST_TESTCASE_INDEX_READ);
        if (data->idx > 0) {
            __VSF_TEST_TRACE_INFO("[TEST] Resuming from test case #%u\r\n", data->idx);
        }
    }

    while (data->idx < __vsf_test->test_case_count) {
        vsf_test_run_case(data->idx);
        data->idx++;
    }

    __VSF_TEST_TRACE_INFO("[TEST] All test cases completed\r\n");

    __VSF_TEST_TRACE_INFO("\r\n[TEST] ========== Test Summary ==========\r\n");
    __VSF_TEST_TRACE_INFO("[TEST] Total test cases: %u\r\n", __vsf_test->test_case_count);

    uint32_t pass_count = 0, fail_count = 0, skip_count = 0, wdt_pass_count = 0, wdt_fail_count = 0;

    for (uint32_t i = 0; i < __vsf_test->test_case_count; i++) {
        data->idx = i;
        __vsf_test_data_sync(data, VSF_TEST_TESTCASE_INDEX_READ);

        vsf_test_result_t result = (vsf_test_result_t)data->result;

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

    __vsf_test_data_sync(data, VSF_TEST_DONE);
}

vsf_test_result_t vsf_test_get_case_result(uint32_t idx)
{
    if (__vsf_test == NULL || idx >= __vsf_test->test_case_count) {
        return VSF_TEST_RESULT_SKIP;
    }
    vsf_test_data_t *data = &__vsf_test->data;
    data->idx = idx;
    __vsf_test_data_sync(data, VSF_TEST_TESTCASE_INDEX_READ);
    return (vsf_test_result_t)data->result;
}

uint32_t vsf_test_get_case_count(void)
{
    return __vsf_test ? __vsf_test->test_case_count : 0;
}

#endif
/* EOF */
