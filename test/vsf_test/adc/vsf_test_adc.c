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

#include "vsf_test_adc.h"

/*============================ IMPLEMENTATION ================================*/

#define REG_IF(gate, s, field, add_fn)            \
    VSF_TEST_REGISTER_SCENE(s, field, add_fn)

void vsf_test_adc_register_all(vsf_test_adc_scenes_t *s)
{
#if VSF_TEST_ADC_ONESHOT_ENABLE == ENABLED
    REG_IF("adc_oneshot", s, oneshot, vsf_test_adc_oneshot_add_cases);
#endif
}

/* EOF */
