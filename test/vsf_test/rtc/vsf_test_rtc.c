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

/*============================ INCLUDES ======================================*/

#include "vsf_test_rtc.h"

/*============================ IMPLEMENTATION ================================*/

// Suite-aware suites: each add_cases() calls vsf_test_register_suite()
// internally, which also opens the matching shell suite.
void vsf_test_rtc_init(vsf_test_rtc_suites_t *s, const vsf_test_rtc_cfg_t *cfg)
{
    s->set_get.rtc = cfg->rtc;
    s->alarm.rtc = cfg->rtc;
#if VSF_TEST_RTC_SET_GET_ENABLE == ENABLED
    vsf_test_rtc_set_get_add_cases(&s->set_get);
#endif
#if VSF_TEST_RTC_ALARM_ENABLE == ENABLED
    vsf_test_rtc_alarm_add_cases(&s->alarm);
#endif
}

/* EOF */
