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
    static vsf_test_wdt_t __wdt_entries[] = {
        // 1.1 Internal WDT (chip's own watchdog)
        {
            .init = vsf_test_hal_wdt_init,
            .feed = vsf_test_hal_wdt_feed,
        },
        // 1.2 External WDT (assist device controlling power/reset pin)
        {
            .init = vsf_test_stdio_wdt_init,
            .feed = vsf_test_stdio_wdt_feed,
        },
    };
    static vsf_test_reboot_t *__reboot_entries[] = {
        // 1.3 External reboot first (assist device)
        vsf_test_stdio_reboot,
        // 1.4 Internal reboot fallback (chip reset)
        vsf_arch_reset,
    };
    static vsf_test_t __test = {
        .wdt = {
            .entries = __wdt_entries,
            .count   = dimof(__wdt_entries),
        },
        .reboot = {
            .entries = __reboot_entries,
            .count   = dimof(__reboot_entries),
        },
    };
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
        static vsf_test_wdt_t __test_wdt_entries[] = {
            {
                .init = vsf_test_hal_wdt_init,
                .feed = vsf_test_hal_wdt_feed,
            },
        };
        static vsf_test_reboot_t *__test_reboot_entries[] = {
            vsf_arch_reset,
        };
        __vsf_test.wdt.entries = __test_wdt_entries;
        __vsf_test.wdt.count   = dimof(__test_wdt_entries);
        __vsf_test.reboot.entries = __test_reboot_entries;
        __vsf_test.reboot.count   = dimof(__test_reboot_entries);
        __vsf_test.restart_on_done = false;  // Set to true to restart when test completes or errors occur
        vsf_test_run(NULL);

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

        // 7. vsf_test_run runs all tests and optionally starts the shell REPL.
        // No explicit vsf_test_run_tests call is needed.

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

//!< Emit Capture Markers (CASE:N / READY / DONE / :END) for host-side LA decode.
//!< Disabled by default — only needed when vsf-bench decodes LA captures.
#        ifndef VSF_TEST_CFG_EMIT_MARKERS
#            define VSF_TEST_CFG_EMIT_MARKERS DISABLED
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
                    vsf_test_assert(VSF_TEST_RESULT_FAIL, __FILE__,         \
                                       __LINE__, __FUNCTION__, #__v);          \
                }                                                              \
            } while (0)

/*============================ INCLUDES ======================================*/

#        include "./vsf_test_shell.h"

/*============================ TYPES =========================================*/

typedef enum vsf_test_result_t {
    VSF_TEST_RESULT_PASS = 0,
    VSF_TEST_RESULT_SKIP = 1,
    VSF_TEST_RESULT_FAIL = 2,
} vsf_test_result_t;

typedef enum vsf_peripheral_type_t {
    VSF_PERIPHERAL_TYPE_NONE   = 0,
    VSF_PERIPHERAL_TYPE_GPIO,
    VSF_PERIPHERAL_TYPE_USART,
    VSF_PERIPHERAL_TYPE_SPI,
    VSF_PERIPHERAL_TYPE_I2C,
    VSF_PERIPHERAL_TYPE_ADC,
    VSF_PERIPHERAL_TYPE_PWM,
    VSF_PERIPHERAL_TYPE_TIMER,
    VSF_PERIPHERAL_TYPE_RTC,
    VSF_PERIPHERAL_TYPE_WDT,
    VSF_PERIPHERAL_TYPE_RNG,
    VSF_PERIPHERAL_TYPE_DMA,
    VSF_PERIPHERAL_TYPE_FLASH,
    VSF_PERIPHERAL_TYPE_I2C_SLAVE,
} vsf_peripheral_type_t;

typedef void vsf_test_reboot_t(void);

vsf_class(vsf_test_wdt_t) {
    public_member(
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
    )
};

typedef struct vsf_test_case_t vsf_test_case_t;
typedef void vsf_test_jmp_fn_t(vsf_test_case_t *tc);

dcl_simple_class(vsf_test_suite_t)
typedef bool vsf_test_suite_setup_fn_t(vsf_test_suite_t *suite);
typedef void vsf_test_suite_teardown_fn_t(vsf_test_suite_t *suite);

//! \brief Test Suite — pure logic, no HAL binding.
//!
//! Each scenario extends vsf_test_suite_t via PLOOC and adds its
//! scenario-specific fields (typically a HAL handle slot that the framework
//! fills at runtime from the matching instance).

vsf_class(vsf_test_case_t) {
    public_member(
        vsf_test_jmp_fn_t *jmp_fn;
        uint8_t            case_idx;
        vsf_test_suite_t  *suite;
        void              *arg;
        bool               needs_ready_handshake;
        uint8_t            result;

        struct {
            const char *function_name;
            const char *file_name;
            const char *condition;
            uint32_t    line;
        } error;
    )
};

vsf_class(vsf_test_suite_t) {
    public_member(
        const char                     *name;          //!< also used as Capture Marker tag
        uint16_t                       case_count;     //!< per-suite case array length
        vsf_test_case_t               *cases;          //!< per-suite case array
        vsf_peripheral_type_t          peripheral_type;//!<< which HAL type this suite needs (0 = none)
        void                          *arg;            //!< HAL handle(s), injected by instance matcher
    )
};

typedef struct vsf_test_inst_t {
    vsf_peripheral_type_t       peripheral_type; //!< which HAL interface this instance satisfies
    void                       *hal_handle;      //!< pointer to HAL hardware struct (e.g. &vsf_hw_usart1)
    vsf_test_suite_setup_fn_t  *setup;           //!< called once before first case for this instance
    vsf_test_suite_teardown_fn_t *teardown;      //!< called once after last case for this instance
} vsf_test_inst_t;

typedef struct vsf_test_t {
    //! Without a watchdog, we can still can test.
    //! But the watchdog provides stronger guarantees for tests:
    //! if a test is abnormal, the next test continues to run after the watchdog
    //! times out.
    struct {
        vsf_test_wdt_t *entries;
        uint8_t         count;
    } wdt;

    //! We perform a reset when the test program goes into exception.
    //! Reboot functions are called in array order; typically external first,
    //! then internal fallback. If none succeed, the framework enters a dead
    //! loop and waits for the watchdog to reset.
    struct {
        vsf_test_reboot_t **entries;
        uint8_t             count;
    } reboot;

    //! Current test case pointer — set by vsf_test_run_case before invoking
    //! the test function, used by vsf_test_assert and vsf_test_reboot.
    vsf_test_case_t *current_case;

#        if VSF_TEST_CFG_LONGJMP == ENABLED
    jmp_buf *jmp_buf;
#        endif

    //! Registered suites array and count — populated at compile time.
    vsf_test_suite_t  *suites;
    uint8_t            suite_count;

    //! Peripheral instances array and count — populated at compile time.
    vsf_test_inst_t  **instances;
    uint8_t            instance_count;

    //! Embedded shell REPL — started by vsf_test_run() after init.
    vsf_test_shell_t shell;
} vsf_test_t;

/*============================ INCLUDES ======================================*/
/*============================ PROTOTYPES ====================================*/

/**
 @brief initialize vsf test
 @param[in] test: test instance pointer (must not be NULL)
 */
extern void vsf_test_run(vsf_test_t *test);

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
extern void vsf_test_assert(vsf_test_result_t result,
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
 @brief Run a single test case within a suite. No setup/teardown is performed
 — the caller is responsible for calling setup before the first case and
 teardown after the last case.
 @param[in] suite: pointer to the suite containing the case
 @param[in] local_idx: index of the case within the suite's cases array
 */
extern void vsf_test_run_suite_case(vsf_test_suite_t *suite, uint16_t local_idx);

/**
 @brief Run all cases in a suite. Calls setup before the first case and
 teardown after the last case. If setup returns false, all cases are skipped.
 @param[in] suite: pointer to the suite to run
 */
extern void vsf_test_run_suite(vsf_test_suite_t *suite);

/*============================ LOCAL VARIABLES ===============================*/
/*============================ GLOBAL VARIABLES ==============================*/

#    endif
#endif
/* EOF */