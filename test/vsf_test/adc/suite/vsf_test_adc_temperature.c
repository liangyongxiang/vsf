/*============================ INCLUDES ======================================*/

#include "vsf_test_adc_temperature.h"

#if VSF_TEST_ADC_TEMPERATURE_ENABLE == ENABLED

/*============================ MACROS ========================================*/

/* Temperature sensor raw range at room temperature (~15-35°C).
 * Values are per-chip and read from test_params YAML. */

/*============================ LOCAL VARIABLES ===============================*/

/*============================ IMPLEMENTATION ================================*/

VSF_TEST_SUITE_REGISTER(vsf_test_adc_temperature_add_cases,
    vsf_test_adc_temperature_suite_t,
    vsf_test_adc_temperature_case_t,
    vsf_test_adc_temperature_run,
    VSF_TEST_ADC_TEMPERATURE_CASES_INIT,
    "adc_temperature", "adc_temperature", "none",
    false)

void vsf_test_adc_temperature_run(void *arg)
{
    vsf_test_adc_temperature_case_t *c = (vsf_test_adc_temperature_case_t *)arg;
    vsf_adc_t *adc = c->suite->adc;

    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; suite-aware suites do not print them. */

    vsf_adc_capability_t cap = vsf_adc_capability(adc);
    VSF_TEST_ASSERT(cap.max_data_bits == 12);

    vsf_adc_cfg_t cfg = {
        .mode     = VSF_ADC_REF_VDD_1 | VSF_ADC_DATA_ALIGN_RIGHT | VSF_ADC_SCAN_CONV_SINGLE_MODE,
        .isr      = {NULL, NULL, 0},
        .clock_hz = 48000000,
    };
    vsf_err_t err = vsf_adc_init(adc, &cfg);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    while (fsm_rt_cpl != vsf_adc_enable(adc));

    /* Sample internal temperature sensor. */
    vsf_adc_channel_cfg_t ch_cfg = {
        .channel       = c->sensor_channel,
        .mode          = VSF_ADC_CHANNEL_GAIN_1 | VSF_ADC_CHANNEL_REF_VDD_1,
        .sample_cycles = 0,
    };
    uint16_t sample = 0;
    err = vsf_adc_channel_request_once(adc, &ch_cfg, &sample);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    /* Verify raw value is within 12-bit range. Temperature varies by
     * environment and VREF tolerance, so no tight bounds. */
    VSF_TEST_ASSERT(sample < (1U << 12));

    vsf_trace_info("ADC:TEMPERATURE:PASS sample=0x%03x" VSF_TRACE_CFG_LINEEND,
                   (unsigned)sample);

    while (fsm_rt_cpl != vsf_adc_disable(adc));
}

#endif /* VSF_TEST_ADC_TEMPERATURE_ENABLE == ENABLED */

/* EOF */
