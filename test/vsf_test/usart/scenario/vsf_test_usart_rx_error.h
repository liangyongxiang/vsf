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

#ifndef __TEST_USART_RX_ERROR_H__
#define __TEST_USART_RX_ERROR_H__

#include "../vsf_test_usart.h"

#if VSF_TEST_USART_RX_PARITY_ERROR_ENABLE == ENABLED || VSF_TEST_USART_RX_FRAME_ERROR_ENABLE == ENABLED

#ifdef __cplusplus
extern "C" {
#endif

/*============================ PROTOTYPES ====================================*/

#if VSF_TEST_USART_RX_PARITY_ERROR_ENABLE == ENABLED
void vsf_test_usart_rx_parity_error_add_cases(vsf_test_usart_rx_parity_error_suite_t *suite);
void vsf_test_usart_rx_parity_error_run(const vsf_test_usart_rx_parity_error_case_t *c);
#endif

#if VSF_TEST_USART_RX_FRAME_ERROR_ENABLE == ENABLED
void vsf_test_usart_rx_frame_error_add_cases(vsf_test_usart_rx_frame_error_suite_t *suite);
void vsf_test_usart_rx_frame_error_run(const vsf_test_usart_rx_frame_error_case_t *c);
#endif

#ifdef __cplusplus
}
#endif

#endif /* VSF_TEST_USART_RX_PARITY_ERROR_ENABLE == ENABLED || VSF_TEST_USART_RX_FRAME_ERROR_ENABLE == ENABLED */

#endif /* __TEST_USART_RX_ERROR_H__ */
/* EOF */
