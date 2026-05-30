/*============================ INCLUDES ======================================*/

#include "vsf_test_wdt_basic.h"

#if VSF_TEST_WDT_BASIC_ENABLE == ENABLED

#include "hal/vsf_hal.h"

/*============================ MACROS ========================================*/


/*============================ LOCAL VARIABLES ===============================*/

/*============================ IMPLEMENTATION ================================*/

void vsf_test_wdt_basic_run(void *arg)
{
    vsf_test_wdt_basic_case_t *c = (vsf_test_wdt_basic_case_t *)arg;
    vsf_wdt_t *wdt = c->suite->wdt;

    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; suite-aware suites do not print them. */

    vsf_wdt_capability_t cap = vsf_wdt_capability(wdt);
    VSF_TEST_ASSERT(cap.support_reset_soc == 1);
    VSF_TEST_ASSERT(cap.support_disable == 0);

    vsf_err_t err = vsf_wdt_init(wdt, &(vsf_wdt_cfg_t){
        .mode   = VSF_WDT_MODE_NO_EARLY_WAKEUP | VSF_WDT_MODE_RESET_SOC,
        .max_ms = 500,
        .min_ms = 0,
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    while (fsm_rt_cpl != vsf_wdt_enable(wdt));

    for (int i = 0; i < 2; i++) {
        vsf_wdt_feed(wdt);
        vsf_trace_info("WDT:FEED:%d" VSF_TRACE_CFG_LINEEND, i);
        vsf_test_busy_wait_ms(10);
    }

    vsf_trace_info("WDT:BASIC:PASS" VSF_TRACE_CFG_LINEEND);
}

#endif /* VSF_TEST_WDT_BASIC_ENABLE == ENABLED */

/* EOF */
