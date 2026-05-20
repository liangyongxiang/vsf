/*============================ INCLUDES ======================================*/

#include "vsf_test_adc_oneshot.h"

#if VSF_TEST_ADC_ONESHOT_ENABLE == ENABLED

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_MARKER_DELAY_MS
#   define VSF_TEST_MARKER_DELAY_MS            200
#endif

/*============================ LOCAL VARIABLES ===============================*/

static vsf_test_adc_oneshot_case_t __adc_oneshot_cases[] = {
    VSF_TEST_ADC_ONESHOT_CASES_INIT
};

/*============================ IMPLEMENTATION ================================*/

void vsf_test_adc_oneshot_add_cases(vsf_test_adc_oneshot_suite_t *suite)
{
    suite->name    = "adc_oneshot";
    suite->purpose = "adc_oneshot";
    suite->hw_req  = "none";
    vsf_test_register_suite(&suite->use_as__vsf_test_suite_t);
    for (uint8_t i = 0; i < VSF_TEST_ADC_ONESHOT_CASE_COUNT; i++) {
        __adc_oneshot_cases[i].suite = suite;
        vsf_test_suite_add_case(&suite->use_as__vsf_test_suite_t,
            (vsf_test_jmp_fn_t *)vsf_test_adc_oneshot_run,
            (void *)&__adc_oneshot_cases[i]);
    }
}

void vsf_test_adc_oneshot_run(void *arg)
{
    vsf_test_adc_oneshot_case_t *c = (vsf_test_adc_oneshot_case_t *)arg;
    vsf_adc_t *adc = c->suite->adc;

    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; suite-aware scenarios do not print them. */

    // Verify capability
    vsf_adc_capability_t cap = vsf_adc_capability(adc);
    VSF_TEST_ASSERT(cap.max_data_bits == 12);
    VSF_TEST_ASSERT(cap.channel_count >= 4);

    // Init ADC
    vsf_adc_cfg_t cfg = {
        .mode     = VSF_ADC_REF_VDD_1 | VSF_ADC_DATA_ALIGN_RIGHT | VSF_ADC_SCAN_CONV_SINGLE_MODE,
        .isr      = {NULL, NULL, 0},
        .clock_hz = 48000000,
    };
    vsf_err_t err = vsf_adc_init(adc, &cfg);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    // Enable ADC
    while (fsm_rt_cpl != vsf_adc_enable(adc));

    // Sample on channel 0 (GPIO26) — oneshot, verify 12-bit range
    vsf_adc_channel_cfg_t ch_cfg = {
        .channel       = 0,
        .mode          = VSF_ADC_CHANNEL_GAIN_1 | VSF_ADC_CHANNEL_REF_VDD_1,
        .sample_cycles = 0,
    };
    uint16_t sample = 0;
    err = vsf_adc_channel_request_once(adc, &ch_cfg, &sample);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    // Result must be within 12-bit range
    VSF_TEST_ASSERT(sample <= 0x0FFF);

    vsf_trace_info("ADC:ONESHOT:PASS sample=0x%03x" VSF_TRACE_CFG_LINEEND,
                   (unsigned)sample);

    // Disable ADC
    while (fsm_rt_cpl != vsf_adc_disable(adc));
}

#endif /* VSF_TEST_ADC_ONESHOT_ENABLE == ENABLED */

/* EOF */
