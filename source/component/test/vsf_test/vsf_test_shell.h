#ifndef __VSF_TEST_SHELL_H__
#define __VSF_TEST_SHELL_H__

#include "vsf.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef VSF_TEST_SHELL_MAX_SCENES
#   define VSF_TEST_SHELL_MAX_SCENES    32
#endif
#ifndef VSF_TEST_SHELL_MAX_CASES
#   define VSF_TEST_SHELL_MAX_CASES     256
#endif

typedef struct vsf_test_shell_scene_t {
    const char *name;
    uint16_t    first_case_idx;
    uint16_t    case_count;
} vsf_test_shell_scene_t;

typedef struct vsf_test_shell_case_t {
    const char *cfg_str;
    uint8_t     scene_idx;
} vsf_test_shell_case_t;

typedef struct vsf_test_shell_t {
    vsf_test_shell_scene_t scenes[VSF_TEST_SHELL_MAX_SCENES];
    uint8_t                scene_count;
    vsf_test_shell_case_t  cases[VSF_TEST_SHELL_MAX_CASES];
    uint16_t               case_count;
    int8_t cur_scene;
    int8_t cur_case;
    bool   auto_case;
    bool   auto_scene;
} vsf_test_shell_t;

uint8_t vsf_test_shell_register_scene(vsf_test_shell_t *shell, const char *name);
void    vsf_test_shell_register_case(vsf_test_shell_t *shell, const char *cfg_str);
void    vsf_test_shell_init(vsf_test_shell_t *shell);
void    vsf_test_shell_run(vsf_test_shell_t *shell);

vsf_test_shell_t *vsf_test_get_shell(void);

#define VSF_TEST_REGISTER_SCENE(s, field, add_fn)                  \
    do {                                                            \
        vsf_test_shell_register_scene(vsf_test_get_shell(), #field); \
        add_fn(&(s)->field);                                        \
    } while (0)

#ifdef __cplusplus
}
#endif

#endif
