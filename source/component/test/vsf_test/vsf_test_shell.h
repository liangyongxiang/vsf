#ifndef __VSF_TEST_SHELL_H__
#define __VSF_TEST_SHELL_H__

#include "vsf.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations — defined later in vsf_test.h */
typedef struct vsf_test_suite_t  vsf_test_suite_t;
typedef struct vsf_test_inst_t   vsf_test_inst_t;

#ifndef VSF_TEST_SHELL_MAX_MATCHES
#   define VSF_TEST_SHELL_MAX_MATCHES   96
#endif

typedef struct vsf_test_shell_t {
    vsf_test_suite_t **suites;
    uint8_t            suite_count;
    vsf_test_inst_t  **instances;
    uint8_t            instance_count;
} vsf_test_shell_t;

void vsf_test_shell_init(vsf_test_shell_t *shell,
                         vsf_test_suite_t **suites, uint8_t suite_count,
                         vsf_test_inst_t **instances, uint8_t instance_count);
void vsf_test_shell_run(vsf_test_shell_t *shell);

#ifdef __cplusplus
}
#endif

#endif
