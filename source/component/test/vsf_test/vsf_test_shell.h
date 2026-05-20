#ifndef __VSF_TEST_SHELL_H__
#define __VSF_TEST_SHELL_H__

#include "vsf.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef VSF_TEST_SHELL_MAX_SUITES
#   define VSF_TEST_SHELL_MAX_SUITES    48
#endif

#ifndef VSF_TEST_SHELL_MAX_CASES_PER_SUITE
#   define VSF_TEST_SHELL_MAX_CASES_PER_SUITE    32
#endif

typedef struct vsf_test_shell_suite_t {
    const char *name;
    uint16_t    first_case_idx;
    uint16_t    case_count;
} vsf_test_shell_suite_t;

typedef struct vsf_test_shell_t {
    vsf_test_shell_suite_t suites[VSF_TEST_SHELL_MAX_SUITES];
    uint8_t                suite_count;
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

uint8_t vsf_test_shell_register_suite(vsf_test_shell_t *shell, const char *name);
void    vsf_test_shell_inc_case_count(vsf_test_shell_t *shell);
void    vsf_test_shell_init(vsf_test_shell_t *shell);
void    vsf_test_shell_run(vsf_test_shell_t *shell);

vsf_test_shell_t *vsf_test_get_shell(void);

#define VSF_TEST_REGISTER_SUITE(s, field, add_fn)                  \
    do {                                                            \
        vsf_test_shell_register_suite(vsf_test_get_shell(), #field); \
        add_fn(&(s)->field);                                        \
    } while (0)

#ifdef __cplusplus
}
#endif

#endif
