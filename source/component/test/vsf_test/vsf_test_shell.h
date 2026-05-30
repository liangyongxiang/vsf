#ifndef __VSF_TEST_SHELL_H__
#define __VSF_TEST_SHELL_H__

#include "vsf.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declaration — vsf_test_suite_t is defined later in vsf_test.h */
typedef struct vsf_test_suite_t vsf_test_suite_t;

#ifndef VSF_TEST_SHELL_MAX_MATCHES
#   define VSF_TEST_SHELL_MAX_MATCHES   96
#endif

#ifndef VSF_TEST_SHELL_MAX_CASES_PER_SUITE
#   define VSF_TEST_SHELL_MAX_CASES_PER_SUITE    32
#endif

typedef struct vsf_test_shell_t {
    vsf_test_suite_t **suites;       //!< pointer to external suites array (no copy)
    uint8_t            suite_count;
    int8_t cur_suite;
    int8_t cur_case;
    bool   auto_case;
    bool   auto_suite;
    /* When non-zero, __run_selection shuffles case order within the
     * selected suite via Fisher-Yates seeded with this value. Host sets
     * via `vsf-test config shuffle <N>` before each suite run; sequential
     * order is preserved at zero. */
    uint32_t shuffle_seed;
} vsf_test_shell_t;

void vsf_test_shell_init(vsf_test_shell_t *shell, vsf_test_suite_t **suites, uint8_t count);
void vsf_test_shell_run(vsf_test_shell_t *shell);

#ifdef __cplusplus
}
#endif

#endif
