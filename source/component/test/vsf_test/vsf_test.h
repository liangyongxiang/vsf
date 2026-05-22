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

#ifndef __VSF_TEST_H__
#   define __VSF_TEST_H__

//! \note do not move this pre-processor statement to other places
#   include "component/vsf_component_cfg.h"
#   include "utilities/vsf_utilities.h"

/* example:

    // 0. Include vsf header file
    #include "vsf.h"

    // 1. Defining vsf_test variable
    static vsf_test_t __test = {
        .wdt =
            {
                .internal =
                    {
                        // 1.1 If you are using a device that already supports
                        // vsf_hal_wdt, then you can just use vsf_test_hal_wdt_*
                        .init = vsf_test_hal_wdt_init,
                        .feed = vsf_test_hal_wdt_feed,
                    },
                .external =
                    {
                        // 1.2 If you are using an assist device that can
                        // control the power pin or reset pin of the device,
                        // then you can use vsf_test_stdio_wdt_* to control it
                        .init = vsf_test_stdio_wdt_init,
                        .feed = vsf_test_stdio_wdt_feed,
                    },
            },
        .reboot =
            {
                // 1.3 Reset can be done using the functions provided by
                // vsf_arch or the chip's APIs
                .internal = vsf_arch_reset,

                // 1.4 To control the reset or power pins of the device, we can
                // use the stdio method to communicate.
                .external = vsf_test_stdio_reboot,
            },
        .data = {
            // We use stdio to communicate with assist devices for data
            // persistence. This way we only need to implement the stdio stub
            // function on the device to come.
            .init = vsf_test_stdio_data_init,
            .sync = vsf_test_stdio_data_sync,
        }};
    static vsf_test_t *test = &__test;

    // 3. Optionally, call vsf_test_reboot in the callback function for all
    // exceptions to abort the test and provide additional error messages if
    // possible information.
    // Here is an example of HardFault_Handler in Cortex-M:
    void HardFault_Handler(void)
    {
        vsf_test_reboot(VSF_TEST_RESULT_FAULT_HANDLER_FAIL, __FILE__,
                        __LINE__, __FUNCTION__, "More Info");
    }

    // 4. Here are some test examples. including:
    // - test succeeded
    // - test failed
    // - test with watchdog reset
    // - test with an exception

    // test succeeded
    static void __test_pass(void)
    {
        int a = 100;
        int b = 50 + 50;

        VSF_TEST_ASSERT(a == b);
    }

    // test failed
    static void __test_fail(void)
    {
        int a = 100;
        int b = 50 + 50;

        VSF_TEST_ASSERT(a != b);
    }

    // test with watchdog reset
    static void __test_wdt(void)
    {
        while (1);
    }

    // test with an exception, current only support cortex-m
    void __test_unalign(void)
    {
    #ifdef __CORTEX_M
        SCB->CCR |= (1 << 3);
        volatile uint32_t *p     = (volatile uint32_t *)0x03;
        uint32_t           value = *p;
    #else
    #    error "TODO"
    #endif
    }

    int main(void)
    {
        // 5. Configure and initialize the test framework
        vsf_test_cfg_t test_cfg = {
            .wdt = {
                .internal = {
                    .init = vsf_test_hal_wdt_init,
                    .feed = vsf_test_hal_wdt_feed,
                },
            },
            .reboot = {
                .internal = vsf_arch_reset,
            },
            .data = {
                .init = vsf_test_stdio_data_init,
                .sync = vsf_test_stdio_data_sync,
            },
            .restart_on_done = false,  // Set to true to restart when test completes or errors occur
        };
        vsf_test_init(&test_cfg);

        // 6. We support two styles of adding test cases.
        // - The first is the static way. We can use macros to initialize test
        //   cases. It is more RAM efficient.
        // - The second is the dynamic way of adding, which is closer to the
        //   traditional testing framework.
    #if 1
        vsf_test_add(__test_pass, "test_pass hw_req=none");
        vsf_test_add(__test_fail, "test_fail hw_req=none");
        vsf_test_add(__test_wdt, "test_wdt hw_req=none");
        vsf_test_add(__test_unalign, "test_invalid_address hw_req=none");
    #endif

        // 7. Here the test will start running
        vsf_test_run_tests();

        return 0;
    }
*/

/*
 * Protocol A — Shell REPL (host → device, real-time over UART):
 *   vsf-test suite --list          → enumerate registered suites
 *   vsf-test run <name>            → trigger execution
 *   vsf-test config shuffle <N>    → set random seed
 *   Suite ack: <name>              ← confirmation
 *   Pass: N, Fail: N, Skip: N      ← per-suite summary
 *
 * Protocol B — Capture Markers (device → host, offline via LA decode):
 *   <suite>:CASE:<N>               ← case start (framework)
 *   <suite>:CASE:<N>:READY         ← DUT ready for host input (framework, optional)
 *   <suite>:CASE:<N>:DONE          ← case end (framework)
 *   <suite>:END                    ← suite boundary (framework)
 *
 * Host-less standalone mode (data sync via assist device) is not supported
 * in the current configuration. Legacy port files are preserved under
 * component/test/vsf_test/port/legacy/ for reference.
 */

#    if VSF_USE_TEST == ENABLED

/*============================ MACROS ========================================*/

//!< Using longjmp/setjmp for assertions
#        ifndef VSF_TEST_CFG_LONGJMP
#            define VSF_TEST_CFG_LONGJMP ENABLED
#        endif

//!< Internal watchdog default timeout
#        ifndef VSF_TEST_CFG_INTERNAL_TIMEOUT_MS
#            define VSF_TEST_CFG_INTERNAL_TIMEOUT_MS 1000
#        endif

//!< External Watchdog Default Timeout
#        ifndef VSF_TEST_CFG_EXTERNAL_TIMEOUT_MS
#            define VSF_TEST_CFG_EXTERNAL_TIMEOUT_MS 1500
#        endif

//!< Using the hal wdt device
#        ifndef VSF_TEST_CFG_USE_HAL_WDT
#            define VSF_TEST_CFG_USE_HAL_WDT DISABLED
#        endif

//!< Enable trace output for test framework
#        ifndef VSF_TEST_CFG_USE_TRACE
#            define VSF_TEST_CFG_USE_TRACE ENABLED
#        endif

//!< Test case array size
#        ifndef VSF_TEST_CFG_ARRAY_SIZE
#            define VSF_TEST_CFG_ARRAY_SIZE 100
#        endif

//!< Marker settle delay in milliseconds. Framework emits CASE/READY then
//!< waits this long before invoking the test function, ensuring marker bytes
//!< are fully on the wire before any test-driven UART activity begins.
//!< Overridden by test_params_generated.h when the test-params generator runs.
#        ifndef VSF_TEST_MARKER_DELAY_MS
#            define VSF_TEST_MARKER_DELAY_MS 2
#        endif

//!< Loop iterations per millisecond for vsf_test_busy_wait_ms. CPU-frequency
//!< dependent; default tuned for RP2040 @ 133MHz. Override per-board if needed.
#        ifndef VSF_TEST_CFG_BUSY_WAIT_CYCLES_PER_MS
#            define VSF_TEST_CFG_BUSY_WAIT_CYCLES_PER_MS 22000
#        endif

/*!
    \def VSF_TEST_ASSERT(__v)
    \brief Add an assertion to the test case
    \param __v expressions for test conditions
*/
#        define VSF_TEST_ASSERT(__v)                                           \
            do {                                                               \
                if (!(__v)) {                                                  \
                    __vsf_test_longjmp(VSF_TEST_RESULT_FAIL, __FILE__,         \
                                       __LINE__, __FUNCTION__, #__v);          \
                }                                                              \
            } while (0)

/*============================ INCLUDES ======================================*/

#        include "./vsf_test_shell.h"

/*============================ TYPES =========================================*/

typedef enum vsf_test_status_t {
    VSF_TEST_STATUS_IDLE    = 0,
    VSF_TEST_STATUS_RUNNING = 1,
} vsf_test_status_t;

typedef enum vsf_test_req_t {
    VSF_TEST_REQ_NO_SUPPORT = 0,
    VSF_TEST_REQ_SUPPORT    = 1,
} vsf_test_req_t;

typedef enum vsf_test_result_t {
    VSF_TEST_RESULT_PASS               = 0x0u << 0,
    VSF_TEST_RESULT_SKIP               = 0x1u << 0,
    VSF_TEST_RESULT_WDT_PASS           = 0x2u << 0,
    VSF_TEST_RESULT_FAIL               = 0x3u << 0,
    VSF_TEST_RESULT_ASSERT_FAIL        = VSF_TEST_RESULT_FAIL,
    VSF_TEST_RESULT_WDT_FAIL           = 0x4u << 0,
    VSF_TEST_RESULT_ASSIST_FAIL        = 0x5u << 0,
    VSF_TEST_RESULT_FAULT_HANDLER_FAIL = 0x6u << 0,
} vsf_test_result_t;

//! Test the type of the function,
typedef enum vsf_test_type_t {
    //! Functions with no return value can use VSF_TEST_ASSERT, which depends on
    //! setjmp/longjmp.
    VSF_TEST_TYPE_LONGJMP_FN = 0,
    //! Functions with boolean return values do not depend on longjmp but cannot
    //! use VSF_TEST_ASSERT
    VSF_TEST_TYPE_BOOL_FN,
} vsf_test_type_t;

typedef void vsf_test_reboot_t(void);

typedef struct vsf_test_wdt_t vsf_test_wdt_t;
struct vsf_test_wdt_t {
    //! Watchdog driver. In hardware, the watchdog usually cannot be
    //! reconfigured after initialization, so here the initialization function
    //! is called just once after power-up
    void (*init)(vsf_test_wdt_t *wdt, uint32_t timeout_ms);
    //! The feed function will be called once after each test is completed.
    void (*feed)(vsf_test_wdt_t *wdt);
    //! Watchdog timeout time (in milliseconds), if not set, the default time
    //! (VSF_TEST_CFG_INTERNAL_TIMEOUT_MS or VSF_TEST_CFG_EXTERNAL_TIMEOUT_MS)
    //! will be used
    uint32_t timeout_ms;
};

typedef bool vsf_test_bool_fn_t(void *arg);
typedef void vsf_test_jmp_fn_t(void *arg);

//! \brief Test Suite — first-class grouping of related Test Cases.
//!
//! Each scenario extends vsf_test_suite_t via PLOOC `implement(vsf_test_suite_t)`
//! and adds its scenario-specific fields (typically a HAL handle).
//!
//! The dispatcher (vsf_test_run_case) emits a Capture Marker
//! "<suite.name>:CASE:<case.case_idx>" before each case and
//! "<suite.name>:CASE:<case.case_idx>:DONE" after each case, removing the
//! need for scenario _run functions to print them themselves.
//!
//! `setup(suite)` runs once before the first case of the suite; `teardown`
//! runs once after the last case. Both may be NULL.
dcl_simple_class(vsf_test_suite_t)
typedef bool vsf_test_suite_setup_fn_t(vsf_test_suite_t *suite);
typedef void vsf_test_suite_teardown_fn_t(vsf_test_suite_t *suite);

vsf_class(vsf_test_suite_t) {
    public_member(
        const char                     *name;          //!< also used as Capture Marker tag
        const char                     *purpose;       //!< short description, e.g. "rx-baud"
        const char                     *hw_req;        //!< hardware requirements, e.g. "uart1+la"
        vsf_test_suite_setup_fn_t      *setup;         //!< NULL = skip; return false to skip all cases
        vsf_test_suite_teardown_fn_t   *teardown;      //!< NULL = skip
    )
    private_member(
        uint16_t                   first_case_idx; //!< managed by framework
        uint16_t                   case_count;     //!< managed by framework
    )
};

typedef struct vsf_test_case_t {
    union {
        //! The test function uses Boolean return value of function that returns
        //! true for a successful test and false for a failed test. You cannot
        //! use VSF_TEST_ASSERT inside this function.
        vsf_test_bool_fn_t *b_fn;

        //! Test functions that use no return value use VSF_TEST_ASSERT to
        //! assert failure. Inside a function, if it is not asserted, the test
        //! succeeds
        vsf_test_jmp_fn_t *jmp_fn;
    };

    //! Use different test function prototypes depending on the type.
    //! @ref vsf_test_type_t
    //! VSF_TEST_TYPE_BOOL_FN: use b_fn
    //! VSF_TEST_TYPE_LONGJMP_FN : use jmp_fn
    uint8_t type;

    //! If the result of the test is expected to be a watchdog reset. Then set
    //! this variable to one
    uint8_t expect_wdt;

    //! If the test is expected to trigger an assertion (e.g., null pointer check),
    //! then set this variable to one. When an assertion is triggered, the test will
    //! be considered as PASS instead of FAIL.
    uint8_t expect_assert;

    //! Scene-local case index (0..suite->case_count-1). Used by the dispatcher
    //! to format the Capture Marker and the [TEST] # N: Running '<name>' line.
    uint8_t case_idx;

    //! Owning Test Suite. The dispatcher prints `<suite->name>:CASE:<case_idx>`
    //! before invoking the test function and `<...>:DONE` after.
    vsf_test_suite_t *suite;

    //! Argument pointer passed directly to the test function.
    void *arg;

    //! Runtime status: IDLE or RUNNING. Used by WDT recovery.
    uint8_t status;

    //! When true, the framework emits <suite>:CASE:<N>:READY before the
    //! settle delay. Set by scenarios that require host-side synchronization.
    bool needs_ready_handshake;

    //! Result of this test case (VSF_TEST_RESULT_*). Set by the dispatcher.
    uint8_t result;

    //! Error details when an assertion or exception occurs.
    struct {
        const char *function_name;
        const char *file_name;
        const char *condition;
        uint32_t    line;
    } error;
} vsf_test_case_t;

//! \brief Test framework configuration structure
typedef struct vsf_test_cfg_t {
    //! Watchdog configuration
    struct {
        //! Internal watchdog configuration
        vsf_test_wdt_t internal;
        //! External watchdog configuration
        vsf_test_wdt_t external;
    } wdt;

    //! Reboot configuration
    struct {
        //! Internal reboot function (chip's internal reset)
        vsf_test_reboot_t *internal;
        //! External reboot function (via reset pin or power pin)
        vsf_test_reboot_t *external;
    } reboot;

    //! Restart from the beginning when test completes or errors occur.
    //! Note: data-sync (assist-device) resume is not supported in the
    //! current configuration; this field is kept for API compatibility.
    bool restart_on_done;
} vsf_test_cfg_t;


typedef struct vsf_test_t {
    //! Without a watchdog, we can still can test.
    //! But the watchdog provides stronger guarantees for tests:
    //! if a test is abnormal, the next test continues to run after the watchdog
    //! times out.
    struct {
        //! internal watchdog means the watchdog
        //! inside the device
        vsf_test_wdt_t internal;

        //! Use an external method to implement a watchdog,
        //! possibly via the reset pin or the device's power pin.
        vsf_test_wdt_t external;
    } wdt;

    //! We perform a reset when the test program goes into exception.
    //! 1. First execute the external reset function.
    //! 2. If the external reset is not successful, then we continue to execute
    //! the internal reset function.
    //! 3. If the internal reset is not successful, then we enter a dead loop
    //! and wait for the watchdog to reset.
    struct {
        //! Use the chip's internal reset, possibly a hot reset
        vsf_test_reboot_t *internal;
        //! Use an external method to reset, possibly via the
        //! reset pin or the device's power pin.
        vsf_test_reboot_t *external;
    } reboot;

    //! Current test case pointer — set by vsf_test_run_case before invoking
    //! the test function, used by __vsf_test_longjmp and vsf_test_reboot.
    vsf_test_case_t *current_case;

#        if VSF_TEST_CFG_LONGJMP == ENABLED
    jmp_buf *jmp_buf;
#        endif

    //! Restart from the beginning when test completes or errors occur.
    //! Note: data-sync resume is not supported in the current configuration.
    bool restart_on_done;

    //! Test case count (number of test cases added)
    uint32_t test_case_count;
    //! Test case array
    vsf_test_case_t test_case_array[VSF_TEST_CFG_ARRAY_SIZE];


    //! Embedded shell instance — every vsf_test_add_* call also registers
    //! the case here. vsf_test_shell_init() activates the REPL.
    vsf_test_shell_t shell;
} vsf_test_t;

/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

/**
 @brief initialize vsf test
 @param[in] cfg: a pointer to configuration structure @ref vsf_test_cfg_t
 */
extern void vsf_test_init(vsf_test_t *test, const vsf_test_cfg_t *cfg);

/**
 @brief Add a populated test case to the framework. Used internally by
 `vsf_test_suite_add_case()`; scenarios should not call this directly.
 @param[in] test_case: a pointer to a `vsf_test_case_t` value
 @return bool: true if add was successfully, or false
 */
extern bool vsf_test_add_ex(vsf_test_case_t *test_case);

extern vsf_test_result_t vsf_test_get_case_result(uint32_t idx);
extern uint32_t vsf_test_get_case_count(void);

/**
 @brief Run all tests. Should be called after all use cases have been
 initialized.
 */
extern void vsf_test_run_tests(void);

/**
 @brief Run a single test case by index. Used by vsf-test-shell for selective
 execution. Does not advance data->idx; the caller manages iteration.
 @param[in] idx: global test case index to run
 */
extern void vsf_test_run_case(uint32_t idx);

/**
 @brief rong jump. the user does not need to directly call this API
 @param[in] result: test result,  @ref vsf_test_result_t
 @param[in] file_name: then name of the file where the assertion occurred
 @param[in] line: the line number of the code where the assertion occurred
 @param[in] function_name: the name of the function where the assertion occurred
 @param[in] condition: String of asserted code

 @note This function will be called when the test case asserts that the
 condition is not satisfied.
 */
extern void __vsf_test_longjmp(vsf_test_result_t result,
                               const char *file_name, uint32_t line,
                               const char *function_name,
                               const char *condition);

/**
 @brief reboot, usually called inside an exception.
 @param[in] result: test result,  @ref vsf_test_result_t
 @param[in] file_name: then name of the file where the assertion occurred
 @param[in] line: the line number of the code where the assertion occurred
 @param[in] function_name: the name of the function where the assertion occurred
 @param[in] additional_str: provide additional exception information

 */
extern void vsf_test_reboot(vsf_test_result_t result,
                            const char *file_name, uint32_t line,
                            const char *function_name,
                            const char *additional_str);

/**
 @brief Busy-wait for approximately the given milliseconds. Useful for
 simple inter-step delays in test scenarios where vsf_systimer is not
 initialized. Calibrated by VSF_TEST_CFG_BUSY_WAIT_CYCLES_PER_MS.
 @param[in] ms: milliseconds to wait
 */
extern void vsf_test_busy_wait_ms(uint32_t ms);
extern void vsf_test_busy_wait_us(uint32_t us);

/* ========================== Test Suite primitive ========================== */
/**
 @brief Register a Test Suite. Optional — only suites that need lifecycle
 hooks or dispatcher-owned Capture Marker emission need this call. The
 framework records the suite, then any subsequent cases added via
 `vsf_test_suite_add_case()` are linked to it (their `case->suite` is set,
 their `case->case_idx` is auto-numbered 0..N-1 in registration order).

 The suite's `setup` (if non-NULL) is called once before its first case;
 `teardown` (if non-NULL) is called once after its last case.

 @param[in] suite: pointer to a `vsf_test_suite_t` (typically the base of a
            PLOOC-extended scenario-specific suite struct)
 @return true on success; false if suite table is full
 */
extern bool vsf_test_register_suite(vsf_test_suite_t *suite);

/**
 @brief Add a case to the most recently registered suite. The case_idx
 field of the case is set to the suite-local index automatically.
 @param[in] suite: same pointer that was passed to `vsf_test_register_suite()`
 @param[in] jmp_fn: test function (longjmp-style)
 @param[in] arg: argument passed to the test function
 @return true on success
 */
extern bool vsf_test_suite_add_case(vsf_test_suite_t *suite,
                                    vsf_test_jmp_fn_t *jmp_fn,
                                    void *arg);

/**
 @brief Add a case to the most recently registered suite with optional
        ready-handshake flag. Same as vsf_test_suite_add_case when
        needs_ready_handshake is false.
 @param[in] suite: same pointer that was passed to `vsf_test_register_suite()`
 @param[in] jmp_fn: test function (longjmp-style)
 @param[in] arg: argument passed to the test function
 @param[in] needs_ready_handshake: when true, the framework emits
            `<suite>:CASE:<N>:READY` before the settle delay
 @return true on success
 */
extern bool vsf_test_suite_add_case_ex(vsf_test_suite_t *suite,
                                       vsf_test_jmp_fn_t *jmp_fn,
                                       void *arg,
                                       bool needs_ready_handshake);

/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/

/*============================ INCLUDES ======================================*/

#        include "./port/vsf_test_port_hal.h"

#    endif
#endif
/* EOF */