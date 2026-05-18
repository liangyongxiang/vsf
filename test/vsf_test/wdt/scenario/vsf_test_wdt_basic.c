/*============================ INCLUDES ======================================*/

#include "vsf_test_wdt_basic.h"

#if VSF_TEST_WDT_BASIC_ENABLE == ENABLED

#include "hal/vsf_hal.h"

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_MARKER_DELAY_MS
#   define VSF_TEST_MARKER_DELAY_MS            200
#endif

/*============================ LOCAL VARIABLES ===============================*/

static vsf_test_wdt_basic_case_t __wdt_basic_cases[] = {
    VSF_TEST_WDT_BASIC_CASES_INIT
};

/*============================ IMPLEMENTATION ================================*/

void vsf_test_wdt_basic_add_cases(vsf_test_wdt_basic_scene_t *scene)
{
    for (uint8_t i = 0; i < VSF_TEST_WDT_BASIC_CASE_COUNT; i++) {
        static char __cfg_str_pool[VSF_TEST_WDT_BASIC_CASE_COUNT][64];
        snprintf(__cfg_str_pool[i], sizeof(__cfg_str_pool[i]),
            "wdt_basic_%u purpose=wdt_basic hw_req=none",
            (unsigned)__wdt_basic_cases[i].idx);
        vsf_test_add_simple_case((vsf_test_jmp_fn_t *)vsf_test_wdt_basic_run,
            __cfg_str_pool[i], (void *)&__wdt_basic_cases[i]);
        __wdt_basic_cases[i].scene = scene;
    }
}

void vsf_test_wdt_basic_run(void *arg)
{
    vsf_test_wdt_basic_case_t *c = (vsf_test_wdt_basic_case_t *)arg;
    vsf_wdt_t *wdt = c->scene->wdt;

    vsf_trace_info("WDT:CASE:%d" VSF_TRACE_CFG_LINEEND, (int)c->idx);
    vsf_test_busy_wait_ms(VSF_TEST_MARKER_DELAY_MS);

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
