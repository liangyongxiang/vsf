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

#include "./vsf_test_shell.h"

extern vsf_mem_stream_t VSF_DEBUG_STREAM_RX;

/*============================ MACROS ========================================*/

#define POLL_INTERVAL_MS    1
#define LINE_BUF_SIZE       128
#define MAX_SCENES          16
#define MAX_CASES           256

/*============================ TYPES =========================================*/

typedef struct {
    const char *name;
    uint16_t    first_case_idx;
    uint16_t    case_count;
} scene_entry_t;

typedef struct {
    const char *cfg_str;
    uint8_t     scene_idx;
} case_entry_t;

/*============================ LOCAL VARIABLES ===============================*/

static scene_entry_t s_scenes[MAX_SCENES];
static uint8_t       s_scene_count;

static case_entry_t  s_cases[MAX_CASES];
static uint16_t      s_case_count;

static int8_t s_cur_scene  = -1;  // -1 = all
static int8_t s_cur_case   = -1;  // -1 = all (in current scene or all scenes)
static bool   s_auto_case   = false;
static bool   s_auto_scene  = false;

/*============================ LOCAL FUNCTIONS ===============================*/

// Read one line from VSF_DEBUG_STREAM_RX. Blocks forever.
static void __read_line(char *buf, size_t buf_size)
{
    size_t len = 0;
    buf[0] = '\0';

    while (1) {
        uint8_t byte;
        while (vsf_stream_read(&VSF_DEBUG_STREAM_RX.use_as__vsf_stream_t,
                               &byte, 1) > 0) {
            if (byte == '\n') {
                if (len > 0 && buf[len - 1] == '\r') {
                    len--;
                }
                buf[len] = '\0';
                return;
            }
            if (len < buf_size - 1) {
                buf[len++] = (char)byte;
            }
        }
        vsf_test_busy_wait_ms(POLL_INTERVAL_MS);
    }
}

// ---- Output helpers ----

static void __print_scene_list(void)
{
    vsf_trace_info("Scenes:" VSF_TRACE_CFG_LINEEND);
    for (uint8_t i = 0; i < s_scene_count; i++) {
        vsf_trace_info("  %u %s" VSF_TRACE_CFG_LINEEND, i, s_scenes[i].name);
    }
}

static void __print_case_list(void)
{
    if (s_cur_scene < 0) {
        vsf_trace_info("No scene selected. Use 'vsf-test scene <n>' first."
                       VSF_TRACE_CFG_LINEEND);
        return;
    }
    scene_entry_t *sc = &s_scenes[s_cur_scene];
    vsf_trace_info("Cases in '%s':" VSF_TRACE_CFG_LINEEND, sc->name);
    for (uint16_t i = 0; i < sc->case_count; i++) {
        uint16_t ci = sc->first_case_idx + i;
        vsf_trace_info("  %u %s" VSF_TRACE_CFG_LINEEND, i,
                       s_cases[ci].cfg_str ? s_cases[ci].cfg_str : "(null)");
    }
}

static void __print_current_scene(void)
{
    if (s_cur_scene < 0) {
        vsf_trace_info("Current scene: all" VSF_TRACE_CFG_LINEEND);
    } else {
        vsf_trace_info("Current scene: %s" VSF_TRACE_CFG_LINEEND,
                       s_scenes[s_cur_scene].name);
    }
}

static void __print_current_case(void)
{
    if (s_cur_scene < 0 && s_cur_case < 0) {
        vsf_trace_info("Current case: all" VSF_TRACE_CFG_LINEEND);
    } else if (s_cur_scene >= 0 && s_cur_case < 0) {
        vsf_trace_info("Current case: all (in scene '%s')"
                       VSF_TRACE_CFG_LINEEND, s_scenes[s_cur_scene].name);
    } else if (s_cur_scene >= 0) {
        uint16_t ci = s_scenes[s_cur_scene].first_case_idx + s_cur_case;
        vsf_trace_info("Current case: %s" VSF_TRACE_CFG_LINEEND,
                       s_cases[ci].cfg_str ? s_cases[ci].cfg_str : "(null)");
    }
}

static void __print_config(void)
{
    vsf_trace_info("auto-case: %s" VSF_TRACE_CFG_LINEEND,
                   s_auto_case ? "on" : "off");
    vsf_trace_info("auto-scene: %s" VSF_TRACE_CFG_LINEEND,
                   s_auto_scene ? "on" : "off");
    __print_current_scene();
    __print_current_case();
}

// ---- Auto-advance ----

static void __advance_case(void)
{
    scene_entry_t *sc = &s_scenes[s_cur_scene];
    s_cur_case++;
    if (s_cur_case >= (int8_t)sc->case_count) {
        s_cur_case = -1;
        if (s_auto_scene) {
            s_cur_scene++;
            if (s_cur_scene >= (int8_t)s_scene_count) {
                s_cur_scene = -1;
            }
        }
    }
}

// ---- Run ----

static void __run_selection(void)
{
    if (s_cur_scene < 0) {
        // Run all cases across all scenes
        vsf_test_run_tests();
        return;
    }

    scene_entry_t *sc = &s_scenes[s_cur_scene];
    if (s_cur_case < 0) {
        // Run all cases in current scene
        for (uint16_t i = 0; i < sc->case_count; i++) {
            uint16_t ci = sc->first_case_idx + i;
            vsf_test_run_case(ci);
            if (s_auto_case) {
                s_cur_case = (int8_t)(i + 1);
                if (s_cur_case >= (int8_t)sc->case_count) {
                    s_cur_case = -1;
                    if (s_auto_scene) {
                        s_cur_scene++;
                        if (s_cur_scene >= (int8_t)s_scene_count) {
                            s_cur_scene = -1;
                        }
                    }
                }
            }
        }
    } else {
        // Run single case
        uint16_t ci = sc->first_case_idx + s_cur_case;
        vsf_test_run_case(ci);
        if (s_auto_case) {
            __advance_case();
        }
    }
}

// ---- Command handlers ----

static void __cmd_scene(char *args)
{
    if (args == NULL || args[0] == '\0') {
        __print_current_scene();
    } else if (strcmp(args, "--list") == 0) {
        __print_scene_list();
    } else {
        int n = atoi(args);
        if (n >= 0 && n < s_scene_count) {
            s_cur_scene = (int8_t)n;
            s_cur_case  = -1;
            vsf_trace_info("Scene %d: %s" VSF_TRACE_CFG_LINEEND, n,
                           s_scenes[n].name);
        } else {
            vsf_trace_info("Invalid scene index" VSF_TRACE_CFG_LINEEND);
        }
    }
}

static void __cmd_case(char *args)
{
    if (args == NULL || args[0] == '\0') {
        __print_current_case();
    } else if (strcmp(args, "--list") == 0) {
        __print_case_list();
    } else {
        if (s_cur_scene < 0) {
            vsf_trace_info("Select a scene first" VSF_TRACE_CFG_LINEEND);
            return;
        }
        int n = atoi(args);
        if (n >= 0 && n < (int)s_scenes[s_cur_scene].case_count) {
            s_cur_case = (int8_t)n;
            uint16_t ci = s_scenes[s_cur_scene].first_case_idx + n;
            vsf_trace_info("Case %d: %s" VSF_TRACE_CFG_LINEEND, n,
                           s_cases[ci].cfg_str ? s_cases[ci].cfg_str : "(null)");
        } else {
            vsf_trace_info("Invalid case index" VSF_TRACE_CFG_LINEEND);
        }
    }
}

static void __cmd_run(char *args)
{
    if (args != NULL && strcmp(args, "all") == 0) {
        s_cur_scene = -1;
        s_cur_case  = -1;
    }
    __run_selection();
}

static void __cmd_config(char *args)
{
    if (args == NULL || args[0] == '\0') {
        __print_config();
        return;
    }

    // Split "auto-case on" into sub and val
    char *space = strchr(args, ' ');
    char *sub = args;
    char *val = NULL;
    if (space != NULL) {
        *space = '\0';
        val = space + 1;
    }

    if (val == NULL) {
        vsf_trace_info("Usage: vsf-test config <key> <on|off>"
                       VSF_TRACE_CFG_LINEEND);
        return;
    }

    bool *target = NULL;
    const char *key_name = NULL;

    if (strcmp(sub, "auto-case") == 0) {
        target = &s_auto_case;
        key_name = "auto-case";
    } else if (strcmp(sub, "auto-scene") == 0) {
        target = &s_auto_scene;
        key_name = "auto-scene";
    } else {
        vsf_trace_info("Unknown config key. Valid: auto-case, auto-scene"
                       VSF_TRACE_CFG_LINEEND);
        return;
    }

    if (strcmp(val, "on") == 0) {
        *target = true;
    } else if (strcmp(val, "off") == 0) {
        *target = false;
    } else {
        vsf_trace_info("Usage: vsf-test config %s on|off"
                       VSF_TRACE_CFG_LINEEND, key_name);
        return;
    }

    vsf_trace_info("%s %s" VSF_TRACE_CFG_LINEEND, key_name,
                   *target ? "on" : "off");
}

// ---- Main dispatch ----

static void __dispatch(char *line)
{
    const char *prefix = "vsf-test ";
    size_t prefix_len = strlen(prefix);

    if (strncmp(line, prefix, prefix_len) != 0) {
        vsf_trace_info("Unknown command. Try 'vsf-test scene --list'"
                       VSF_TRACE_CFG_LINEEND);
        return;
    }

    char *rest = line + prefix_len;

    // Split "cmd [args]" at first space
    char *space = strchr(rest, ' ');
    char *cmd = rest;
    char *args = NULL;
    if (space != NULL) {
        *space = '\0';
        args = space + 1;
        // Trim leading spaces in args
        while (*args == ' ') args++;
        if (*args == '\0') args = NULL;
    }

    if (strcmp(cmd, "scene") == 0) {
        __cmd_scene(args);
    } else if (strcmp(cmd, "case") == 0) {
        __cmd_case(args);
    } else if (strcmp(cmd, "run") == 0) {
        __cmd_run(args);
    } else if (strcmp(cmd, "config") == 0) {
        __cmd_config(args);
    } else {
        vsf_trace_info("Unknown command. Try 'vsf-test scene --list'"
                       VSF_TRACE_CFG_LINEEND);
    }
}

/*============================ IMPLEMENTATION ================================*/

uint8_t vsf_test_shell_register_scene(const char *name)
{
    if (s_scene_count >= MAX_SCENES) {
        return 0;
    }

    uint8_t idx = s_scene_count;
    s_scenes[idx].name           = name;
    s_scenes[idx].first_case_idx = s_case_count;
    s_scenes[idx].case_count     = 0;
    s_scene_count++;
    return idx;
}

void vsf_test_shell_register_case(const char *cfg_str)
{
    if (s_case_count >= MAX_CASES || s_scene_count == 0) {
        return;
    }

    uint16_t case_idx = s_case_count;
    s_cases[case_idx].cfg_str   = cfg_str;
    s_cases[case_idx].scene_idx = s_scene_count - 1;

    s_scenes[s_scene_count - 1].case_count++;
    s_case_count++;
}

void vsf_test_shell_init(void)
{
    vsf_stream_connect_rx(&VSF_DEBUG_STREAM_RX.use_as__vsf_stream_t);

    vsf_trace_info("VSF Test Ready" VSF_TRACE_CFG_LINEEND);
    vsf_trace_info("> " VSF_TRACE_CFG_LINEEND);
}

void vsf_test_shell_run(void)
{
    char line[LINE_BUF_SIZE];

    while (1) {
        __read_line(line, sizeof(line));

        if (line[0] != '\0') {
            __dispatch(line);
        }

        vsf_trace_info("> " VSF_TRACE_CFG_LINEEND);
    }
}

/* EOF */
