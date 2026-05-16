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
 ****************************************************************************/

/*============================ INCLUDES ======================================*/

#include "vsf.h"
#include "vsf_board.h"
#include "component/test/vsf_test/vsf_test.h"

#include <string.h>
#include <stdio.h>

#include "./scenario_gateway.h"

extern vsf_mem_stream_t VSF_DEBUG_STREAM_RX;

/*============================ MACROS ========================================*/

#ifndef VSF_SCENARIO_GATEWAY_HELLO_TIMEOUT_MS
#   define VSF_SCENARIO_GATEWAY_HELLO_TIMEOUT_MS    1500
#endif

#ifndef VSF_SCENARIO_GATEWAY_REPLY_TIMEOUT_MS
#   define VSF_SCENARIO_GATEWAY_REPLY_TIMEOUT_MS    500
#endif

#define POLL_INTERVAL_MS    1
#define LINE_BUF_SIZE       96

/*============================ LOCAL VARIABLES ===============================*/

static bool s_gateway_active = false;

/*============================ LOCAL FUNCTIONS ===============================*/

static void __drain_stream(void)
{
    uint8_t dummy[32];
    while (vsf_stream_read(&VSF_DEBUG_STREAM_RX.use_as__vsf_stream_t,
                           dummy, sizeof(dummy)) > 0);
}

// Read up to one line (terminated by \n) into buf. Returns true on success.
static bool __read_line(char *buf, size_t buf_size, uint32_t timeout_ms)
{
    size_t len = 0;
    buf[0] = '\0';
    uint32_t elapsed = 0;

    while (elapsed < timeout_ms) {
        uint8_t byte;
        while (vsf_stream_read(&VSF_DEBUG_STREAM_RX.use_as__vsf_stream_t,
                               &byte, 1) > 0) {
            if (byte == '\n') {
                if (len > 0 && buf[len - 1] == '\r') {
                    len--;
                }
                buf[len] = '\0';
                return true;
            }
            if (len < buf_size - 1) {
                buf[len++] = (char)byte;
            }
        }
        vsf_test_busy_wait_ms(POLL_INTERVAL_MS);
        elapsed += POLL_INTERVAL_MS;
    }
    return false;
}

/*============================ IMPLEMENTATION ================================*/

bool scenario_gateway_init(void)
{
    vsf_stream_connect_rx(&VSF_DEBUG_STREAM_RX.use_as__vsf_stream_t);
    __drain_stream();

    vsf_trace_info("GATEWAY:HELLO" VSF_TRACE_CFG_LINEEND);

    char line[LINE_BUF_SIZE];
    if (__read_line(line, sizeof(line), VSF_SCENARIO_GATEWAY_HELLO_TIMEOUT_MS)
     && (strcmp(line, "GATEWAY:HELLO") == 0)) {
        s_gateway_active = true;
        return true;
    }

    s_gateway_active = false;
    return false;
}

bool scenario_gateway(const char *name)
{
    if (!s_gateway_active) {
        return true;
    }

    vsf_trace_info("SCENARIO:%s:READY?" VSF_TRACE_CFG_LINEEND, name);

    char expected_go[LINE_BUF_SIZE];
    snprintf(expected_go, sizeof(expected_go), "SCENARIO:%s:GO", name);

    char line[LINE_BUF_SIZE];
    if (__read_line(line, sizeof(line), VSF_SCENARIO_GATEWAY_REPLY_TIMEOUT_MS)
     && (strcmp(line, expected_go) == 0)) {
        return true;
    }

    vsf_trace_info("SCENARIO:%s:SKIP" VSF_TRACE_CFG_LINEEND, name);
    return false;
}

void scenario_gateway_done(void)
{
    if (s_gateway_active) {
        vsf_trace_info("GATEWAY:DONE" VSF_TRACE_CFG_LINEEND);
    }
}

/* EOF */
