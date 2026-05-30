/*============================ INCLUDES ======================================*/

#include "vsf_test_rtc_epoch.h"

#if VSF_TEST_RTC_EPOCH_ENABLE == ENABLED

/*============================ MACROS ========================================*/

/*============================ LOCAL VARIABLES ===============================*/

/*============================ IMPLEMENTATION ================================*/

void vsf_test_rtc_epoch_run(void *arg)
{
    vsf_test_rtc_epoch_case_t *c = (vsf_test_rtc_epoch_case_t *)arg;
    vsf_rtc_t *rtc = c->suite->rtc;

    /* Case 1: Set and get epoch time */
    vsf_rtc_time_t set_seconds = 1700000000;
    vsf_err_t err = vsf_rtc_set_time(rtc, set_seconds, 0);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    while (fsm_rt_cpl != vsf_rtc_enable(rtc));

    vsf_rtc_time_t get_seconds = 0;
    vsf_rtc_time_t get_ms = 0xFF;
    err = vsf_rtc_get_time(rtc, &get_seconds, &get_ms);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    VSF_TEST_ASSERT(get_seconds == set_seconds);
    VSF_TEST_ASSERT(get_ms == 0);

    vsf_trace_info("RTC:EPOCH:SET_GET seconds=%llu ms=%u" VSF_TRACE_CFG_LINEEND,
                   (unsigned long long)get_seconds, (unsigned)get_ms);

    /* Case 2: Increment after ~1.1 seconds */
    vsf_test_busy_wait_ms(1100);

    vsf_rtc_time_t later_seconds;
    err = vsf_rtc_get_time(rtc, &later_seconds, NULL);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    VSF_TEST_ASSERT(later_seconds >= set_seconds + 1);

    vsf_trace_info("RTC:EPOCH:INCREMENT after 1.1s: %llu (expected >= %llu)"
                   VSF_TRACE_CFG_LINEEND,
                   (unsigned long long)later_seconds,
                   (unsigned long long)(set_seconds + 1));

    /* Case 3: Millisecond pointer NULL is OK */
    err = vsf_rtc_get_time(rtc, &get_seconds, NULL);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    vsf_trace_info("RTC:EPOCH:NULL_MS_OK" VSF_TRACE_CFG_LINEEND);

    while (fsm_rt_cpl != vsf_rtc_disable(rtc));
}

#endif /* VSF_TEST_RTC_EPOCH_ENABLE == ENABLED */

/* EOF */
