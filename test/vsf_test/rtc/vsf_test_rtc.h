/******************************************************************************
 *   Copyright(C)2009-2024 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 *****************************************************************************/

#ifndef __VSF_TEST_RTC_H__
#define __VSF_TEST_RTC_H__

/*============================ INCLUDES ======================================*/

#include "vsf.h"
#include "component/test/vsf_test/vsf_test.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================ MACROS ========================================*/

#ifndef VSF_TEST_RTC_SET_GET_ENABLE
#   define VSF_TEST_RTC_SET_GET_ENABLE         ENABLED
#endif

#ifndef VSF_TEST_RTC_ALARM_ENABLE
#   define VSF_TEST_RTC_ALARM_ENABLE           ENABLED
#endif

/*============================ TYPES =========================================*/

vsf_class(vsf_test_rtc_set_get_scene_t) {
    public_member(
        implement(vsf_test_suite_t)
        vsf_rtc_t *rtc;
    )
};

vsf_class(vsf_test_rtc_alarm_scene_t) {
    public_member(
        implement(vsf_test_suite_t)
        vsf_rtc_t *rtc;
    )
};

typedef struct vsf_test_rtc_scenes_t {
    vsf_test_rtc_set_get_scene_t set_get;
    vsf_test_rtc_alarm_scene_t   alarm;
} vsf_test_rtc_scenes_t;

/*============================ PROTOTYPES ====================================*/

void vsf_test_rtc_register_all(vsf_test_rtc_scenes_t *s);

#if VSF_TEST_RTC_SET_GET_ENABLE == ENABLED
void vsf_test_rtc_set_get_add_cases(vsf_test_rtc_set_get_scene_t *scene);
void vsf_test_rtc_set_get_run(void *arg);
#endif

#if VSF_TEST_RTC_ALARM_ENABLE == ENABLED
void vsf_test_rtc_alarm_add_cases(vsf_test_rtc_alarm_scene_t *scene);
void vsf_test_rtc_alarm_run(void *arg);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __VSF_TEST_RTC_H__ */
/* EOF */
