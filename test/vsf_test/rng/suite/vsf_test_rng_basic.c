/*============================ INCLUDES ======================================*/

#include "vsf_test_rng_basic.h"

#if VSF_TEST_RNG_BASIC_ENABLE == ENABLED

/*============================ MACROS ========================================*/

/*============================ IMPLEMENTATION ================================*/

void vsf_test_rng_basic_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture)
{
    vsf_test_rng_basic_params_t *p = tc->arg;
    vsf_rng_t *rng = (vsf_rng_t *)fixture;

    vsf_err_t err = vsf_rng_init(rng);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    uint32_t buffer[16];
    uint8_t word_count = p->word_count > 16 ? 16 : p->word_count;

    err = vsf_rng_generate_request(rng, buffer, word_count, NULL, NULL);
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);

    /* Basic entropy sanity check: not all zeros and not all identical */
    bool all_zero = true;
    bool all_same = true;
    for (uint8_t i = 0; i < word_count; i++) {
        if (buffer[i] != 0) {
            all_zero = false;
        }
        if (i > 0 && buffer[i] != buffer[0]) {
            all_same = false;
        }
    }
    VSF_TEST_ASSERT(!all_zero);
    VSF_TEST_ASSERT(!all_same);

    vsf_trace_info("RNG:BASIC:PASS" VSF_TRACE_CFG_LINEEND);
}

#endif /* VSF_TEST_RNG_BASIC_ENABLE == ENABLED */

/* EOF */
