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

#include "vsf_test_timer.h"

/*============================ IMPLEMENTATION ================================*/

// Suite-aware suites: each add_cases() calls vsf_test_register_suite()
// internally, which also opens the matching shell suite.
void vsf_test_timer_init(vsf_test_timer_suites_t *s, const vsf_test_timer_cfg_t *cfg)
{
    s->oneshot.timer = cfg->timer;
    s->periodic.timer = cfg->timer;
#if VSF_TEST_TIMER_ONESHOT_ENABLE == ENABLED
    vsf_test_timer_oneshot_add_cases(&s->oneshot);
#endif
#if VSF_TEST_TIMER_PERIODIC_ENABLE == ENABLED
    vsf_test_timer_periodic_add_cases(&s->periodic);
#endif
}

/* EOF */
