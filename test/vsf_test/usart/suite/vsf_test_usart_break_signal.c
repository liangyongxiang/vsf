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

#include "vsf_test_usart_break_signal.h"

#if VSF_TEST_USART_BREAK_SIGNAL_ENABLE == ENABLED


/*============================ IMPLEMENTATION ================================*/

/* TX break signaling — exercise SET_BREAK/CLEAR_BREAK and SEND_BREAK
 * and let the LA verify the line stays low.
 *
 * Sequence (observed on uart1_tx):
 *   idle high → SET_BREAK → low for hold_ms → CLEAR_BREAK → idle high
 *
 * Then SEND_BREAK fires the auto-sequence (low for >= 1 frame, then
 * release). The LA decode finds the low pulses in the window.
 *
 * Note: the LA timing decoder reports edge timestamps, not high/low
 * levels. The decode-side asserts on the inter-edge gap (= line-low
 * duration when the previous edge was a fall). */
void vsf_test_usart_break_signal_run(vsf_test_case_t *tc)
{
    vsf_test_usart_break_signal_params_t *p = tc->arg;
    vsf_test_suite_t *suite = tc->suite;
    /* Dispatcher (vsf_test_run_case) emits start / :DONE Capture Markers
     * and the settle delay; suite-aware suites do not print them. */

    vsf_err_t err = vsf_usart_init((vsf_usart_t *)suite->arg, &(vsf_usart_cfg_t){
        .mode     = VSF_USART_NO_PARITY | VSF_USART_1_STOPBIT
                  | VSF_USART_8_BIT_LENGTH | VSF_USART_TX_ENABLE,
        .baudrate = p->baudrate,
    });
    VSF_TEST_ASSERT(err == VSF_ERR_NONE);
    while (fsm_rt_cpl != vsf_usart_enable((vsf_usart_t *)suite->arg));

    /* Brief idle-high settle so the LA can latch the pre-break level. */
    vsf_test_busy_wait_ms(2);

    /* Manual: SET_BREAK → hold → CLEAR_BREAK. */
    VSF_TEST_ASSERT(vsf_usart_ctrl((vsf_usart_t *)suite->arg, VSF_USART_CTRL_SET_BREAK, NULL) == VSF_ERR_NONE);
    vsf_test_busy_wait_ms(p->hold_ms);
    VSF_TEST_ASSERT(vsf_usart_ctrl((vsf_usart_t *)suite->arg, VSF_USART_CTRL_CLEAR_BREAK, NULL) == VSF_ERR_NONE);

    /* Gap so the LA sees two distinct low pulses, not one merged pulse. */
    vsf_test_busy_wait_ms(3);

    /* Automated break: software-timed since PL011 has no hardware auto-break.
     * SET_BREAK -> hold low for >= 1 frame (10 bits) -> CLEAR_BREAK. */
    VSF_TEST_ASSERT(vsf_usart_ctrl((vsf_usart_t *)suite->arg, VSF_USART_CTRL_SET_BREAK, NULL) == VSF_ERR_NONE);
    /* 1 frame at baudrate = 10 bits.  1000ms / baudrate * 10 = ms per frame. */
    uint32_t frame_ms = (1000 * 10) / p->baudrate;
    if (frame_ms < 2) { frame_ms = 2; }
    vsf_test_busy_wait_ms(frame_ms);
    VSF_TEST_ASSERT(vsf_usart_ctrl((vsf_usart_t *)suite->arg, VSF_USART_CTRL_CLEAR_BREAK, NULL) == VSF_ERR_NONE);

    /* Trailing settle so the second pulse's rising edge is fully captured. */
    vsf_test_busy_wait_ms(5);

    while (fsm_rt_cpl != vsf_usart_disable((vsf_usart_t *)suite->arg));
    vsf_usart_fini((vsf_usart_t *)suite->arg);
}

#endif /* VSF_TEST_USART_BREAK_SIGNAL_ENABLE == ENABLED */

/* EOF */
