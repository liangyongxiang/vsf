/*****************************************************************************
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

#ifdef __cplusplus
extern "C" {
#endif

/*============================ PROTOTYPES ====================================*/

void vsf_test_usart_rx_parity_error_scenario(void *arg);
void vsf_test_usart_rx_frame_error_scenario(void *arg);

#ifdef __cplusplus
}
#endif

#endif /* __TEST_USART_RX_ERROR_H__ */
/* EOF */
