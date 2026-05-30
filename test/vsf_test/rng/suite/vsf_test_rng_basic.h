#ifndef __VSF_TEST_RNG_BASIC_H__
#define __VSF_TEST_RNG_BASIC_H__

/*============================ INCLUDES ======================================*/

#include "../vsf_test_rng.h"
#ifndef VSF_TEST_RNG_BASIC_BUF_SIZE
#   define VSF_TEST_RNG_BASIC_BUF_SIZE        16
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_RNG_BASIC_CASE_COUNT
#   define VSF_TEST_RNG_BASIC_CASE_COUNT       1
#endif

/*============================ PROTOTYPES ====================================*/

void vsf_test_rng_basic_run(const vsf_test_suite_t *suite, const vsf_test_case_t *tc, const void *fixture);

#endif /* __VSF_TEST_RNG_BASIC_H__ */
/* EOF */
