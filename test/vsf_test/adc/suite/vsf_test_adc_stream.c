/*============================ INCLUDES ======================================*/

#define __VSF_TEST_ADC_CLASS_IMPLEMENT
#include "vsf_test_adc_stream.h"
#include "vsf_test_suites.h"
/*============================ LOCAL VARIABLES ===============================*/


#if VSF_TEST_ADC_STREAM_ENABLE == ENABLED

static void __adc_isr(void *target_ptr, vsf_adc_t *adc_ptr,

                      vsf_adc_irq_mask_t irq_mask)
{
    (void)adc_ptr;
    vsf_test_suite_t *suite = target_ptr;
    if (irq_mask & VSF_ADC_IRQ_MASK_CPL) {
        vsf_test_suite_data.adc.adc_stream.completed = true;
    }
}



/*============================ MACROS ========================================*/

#define ADC_STREAM_SAMPLE_COUNT     100
#define ADC_STREAM_RAPID_COUNT      10

/*============================ LOCAL VARIABLES ===============================*/


/*============================ IMPLEMENTATION ================================*/

void vsf_test_adc_stream_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture)
{
    vsf_test_adc_stream_params_t *p = tc->arg;
    vsf_adc_t *adc = (vsf_adc_t *)fixture;

    vsf_adc_capability_t cap = vsf_adc_capability(adc);
    VSF_TEST_ASSERT(cap.max_data_bits == 12);
    VSF_TEST_ASSERT(cap.channel_count >= 4);

    vsf_adc_cfg_t cfg = {
        .mode     = VSF_ADC_REF_VDD_1 | VSF_ADC_DATA_ALIGN_RIGHT | VSF_ADC_SCAN_CONV_SINGLE_MODE,
        .isr      = {
            .handler_fn = __adc_isr,
            .target_ptr = NULL,
            .prio       = vsf_arch_prio_0,
        },
        .clock_hz = 48000000,
    };
    vsf_err_t err = vsf_adc_init(adc, &cfg);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    while (fsm_rt_cpl != vsf_adc_enable(adc));

    /* --- Test 1: Stream 100 samples from temp sensor --- */
    vsf_test_suite_data.adc.adc_stream.completed = false;

    vsf_adc_channel_cfg_t ch_cfg = {
        .channel       = 4,
        .mode          = VSF_ADC_CHANNEL_GAIN_1 | VSF_ADC_CHANNEL_REF_VDD_1,
        .sample_cycles = 0,
    };
    err = vsf_adc_channel_config(adc, &ch_cfg, 1);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    uint16_t samples[ADC_STREAM_SAMPLE_COUNT] = {0};
    err = vsf_adc_channel_request(adc, samples, ADC_STREAM_SAMPLE_COUNT);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    uint32_t timeout_ms = 500;
    while (!vsf_test_suite_data.adc.adc_stream.completed && timeout_ms-- > 0) {
        vsf_test_busy_wait_ms(1);
    }
    VSF_TEST_ASSERT(vsf_test_suite_data.adc.adc_stream.completed);

    for (uint32_t i = 0; i < ADC_STREAM_SAMPLE_COUNT; i++) {
        VSF_TEST_ASSERT(samples[i] <= 0x0FFF);
    }

    /* --- Test 2: Rapid fire 1-sample requests --- */
    for (uint32_t i = 0; i < ADC_STREAM_RAPID_COUNT; i++) {
        vsf_test_suite_data.adc.adc_stream.completed = false;
        uint16_t single = 0;
        err = vsf_adc_channel_request(adc, &single, 1);
        VSF_TEST_ASSERT(err == VSF_ERR_NONE);

        timeout_ms = 100;
        while (!vsf_test_suite_data.adc.adc_stream.completed && timeout_ms-- > 0) {
            vsf_test_busy_wait_ms(1);
        }
        VSF_TEST_ASSERT(vsf_test_suite_data.adc.adc_stream.completed);
        VSF_TEST_ASSERT(single <= 0x0FFF);
    }

    vsf_trace_info("ADC:STREAM:PASS" VSF_TRACE_CFG_LINEEND);

    while (fsm_rt_cpl != vsf_adc_disable(adc));
}

#endif /* VSF_TEST_ADC_STREAM_ENABLE == ENABLED */

/* EOF */