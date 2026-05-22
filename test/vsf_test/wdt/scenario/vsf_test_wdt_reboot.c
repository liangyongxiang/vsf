/*============================ INCLUDES ======================================*/

#include "vsf_test_wdt_reboot.h"

#if VSF_TEST_WDT_REBOOT_ENABLE == ENABLED

#include "hal/vsf_hal.h"

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_WDT_REBOOT_TIMEOUT_MS
#   define VSF_TEST_WDT_REBOOT_TIMEOUT_MS      200
#endif

/*============================ LOCAL VARIABLES ===============================*/

static vsf_test_wdt_reboot_case_t __wdt_reboot_cases[] = {
    VSF_TEST_WDT_REBOOT_CASES_INIT
};

/*============================ IMPLEMENTATION ================================*/

void vsf_test_wdt_reboot_add_cases(vsf_test_wdt_reboot_suite_t *suite)
{
    suite->name    = "wdt_reboot";
    suite->purpose = "wdt_reboot";
    suite->hw_req  = "none";
    vsf_test_register_suite(&suite->use_as__vsf_test_suite_t);
    for (uint8_t i = 0; i < VSF_TEST_WDT_REBOOT_CASE_COUNT; i++) {
        __wdt_reboot_cases[i].suite = suite;
        vsf_test_suite_add_case(&suite->use_as__vsf_test_suite_t,
            (vsf_test_jmp_fn_t *)vsf_test_wdt_reboot_run,
            (void *)&__wdt_reboot_cases[i]);
    }
}

void vsf_test_wdt_reboot_run(void *arg)
{
    vsf_test_wdt_reboot_case_t *c = (vsf_test_wdt_reboot_case_t *)arg;
    vsf_wdt_t *wdt = c->suite->wdt;

    vsf_wdt_capability_t cap = vsf_wdt_capability(wdt);
    VSF_TEST_ASSERT(cap.support_reset_soc == 1);

    vsf_err_t err = vsf_wdt_init(wdt, &(vsf_wdt_cfg_t){
        .mode   = VSF_WDT_MODE_NO_EARLY_WAKEUP | VSF_WDT_MODE_RESET_SOC,
        .max_ms = VSF_TEST_WDT_REBOOT_TIMEOUT_MS,
        .min_ms = 0,
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    while (fsm_rt_cpl != vsf_wdt_enable(wdt));

    vsf_wdt_feed(wdt);
    vsf_trace_info("WDT:ARMED timeout=%dms" VSF_TRACE_CFG_LINEEND,
                   VSF_TEST_WDT_REBOOT_TIMEOUT_MS);

    /* Stop feeding — WDT will expire and reset the chip.
     * This function never returns normally on this path.
     * Host detects reset via serial disconnect + reconnect + "VSF Test Ready". */
    vsf_test_busy_wait_ms(VSF_TEST_WDT_REBOOT_TIMEOUT_MS + 100);

    /* Should never reach here — if we do, WDT failed to reset. */
    VSF_TEST_ASSERT(false);
}

#endif /* VSF_TEST_WDT_REBOOT_ENABLE == ENABLED */

/* EOF */
