/*============================ INCLUDES ======================================*/

#define __VSF_TEST_RTC_CLASS_IMPLEMENT
#include "vsf_test_rtc_alarm.h"

#if VSF_TEST_RTC_ALARM_ENABLE == ENABLED

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_MARKER_DELAY_MS
#   define VSF_TEST_MARKER_DELAY_MS            200
#endif

/*============================ LOCAL VARIABLES ===============================*/

static vsf_test_rtc_alarm_case_t __rtc_alarm_cases[] = {
    VSF_TEST_RTC_ALARM_CASES_INIT
};

/*============================ LOCAL FUNCTIONS ===============================*/

static void __rtc_alarm_isr(void *target_ptr, vsf_rtc_t *rtc_ptr,
                            vsf_rtc_irq_mask_t irq_mask)
{
    (void)rtc_ptr;
    vsf_test_rtc_alarm_scene_t *scene = (vsf_test_rtc_alarm_scene_t *)target_ptr;
    if (irq_mask & VSF_RTC_IRQ_MASK_ALARM) {
        scene->alarm_triggered = true;
    }
}

/*============================ IMPLEMENTATION ================================*/

void vsf_test_rtc_alarm_add_cases(vsf_test_rtc_alarm_scene_t *scene)
{
    scene->name    = "rtc_alarm";
    scene->purpose = "rtc_alarm";
    scene->hw_req  = "none";
    vsf_test_register_suite(&scene->use_as__vsf_test_suite_t);
    for (uint8_t i = 0; i < VSF_TEST_RTC_ALARM_CASE_COUNT; i++) {
        __rtc_alarm_cases[i].scene = scene;
        vsf_test_suite_add_case(&scene->use_as__vsf_test_suite_t,
            (vsf_test_jmp_fn_t *)vsf_test_rtc_alarm_run,
            (void *)&__rtc_alarm_cases[i]);
    }
}

void vsf_test_rtc_alarm_run(void *arg)
{
    vsf_test_rtc_alarm_case_t *c = (vsf_test_rtc_alarm_case_t *)arg;
    vsf_rtc_t *rtc = c->scene->rtc;

    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; suite-aware scenarios do not print them. */

    c->scene->alarm_triggered = false;

    // Set datetime to 2024-01-01 12:00:00 Monday
    vsf_rtc_tm_t set_tm = {
        .tm_year = 2024,
        .tm_mon  = 1,
        .tm_mday = 1,
        .tm_wday = 1,
        .tm_hour = 12,
        .tm_min  = 0,
        .tm_sec  = 0,
        .tm_ms   = 0,
    };

    vsf_err_t err = vsf_rtc_init(rtc, &(vsf_rtc_cfg_t){
        .isr = {
            .handler_fn = __rtc_alarm_isr,
            .target_ptr = c->scene,
            .prio       = vsf_arch_prio_0,
        },
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    err = vsf_rtc_set(rtc, &set_tm);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    while (fsm_rt_cpl != vsf_rtc_enable(rtc));

    // Set alarm for 2 seconds in the future: 12:00:02
    vsf_rtc_tm_t alarm_tm = {
        .tm_year = 2024,
        .tm_mon  = 1,
        .tm_mday = 1,
        .tm_wday = 1,
        .tm_hour = 12,
        .tm_min  = 0,
        .tm_sec  = 2,
        .tm_ms   = 0,
    };

    err = vsf_rtc_ctrl(rtc, VSF_RTC_CTRL_SET_ALARM, &alarm_tm);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    // Enable alarm IRQ
    err = vsf_rtc_ctrl(rtc, VSF_RTC_CTRL_IRQ_ENABLE, NULL);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    // Wait up to ~3.5 seconds for alarm to fire
    uint32_t timeout_ms = 3500;
    while (!c->scene->alarm_triggered && timeout_ms-- > 0) {
        vsf_test_busy_wait_ms(1);
    }

    VSF_TEST_ASSERT(c->scene->alarm_triggered);

    vsf_trace_info("RTC:ALARM:PASS" VSF_TRACE_CFG_LINEEND);

    // Disable alarm IRQ
    vsf_rtc_ctrl(rtc, VSF_RTC_CTRL_IRQ_DISABLE, NULL);
    while (fsm_rt_cpl != vsf_rtc_disable(rtc));
    vsf_rtc_fini(rtc);
}

#endif /* VSF_TEST_RTC_ALARM_ENABLE == ENABLED */

/* EOF */
