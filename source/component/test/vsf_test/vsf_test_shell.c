#include "vsf.h"
#include "vsf_board.h"
#include "component/test/vsf_test/vsf_test.h"

#include <string.h>
#include <stdio.h>

#include "./vsf_test_shell.h"

extern vsf_mem_stream_t VSF_DEBUG_STREAM_RX;

#define POLL_INTERVAL_MS    1
#define LINE_BUF_SIZE       128

static void __read_line(char *buf, size_t buf_size)
{
    size_t len = 0;
    buf[0] = '\0';
    while (1) {
        uint8_t byte;
        while (vsf_stream_read(&VSF_DEBUG_STREAM_RX.use_as__vsf_stream_t,
                               &byte, 1) > 0) {
            if (byte == '\n') {
                if (len > 0 && buf[len - 1] == '\r') len--;
                buf[len] = '\0';
                return;
            }
            if (len < buf_size - 1) buf[len++] = (char)byte;
        }
        vsf_test_busy_wait_ms(POLL_INTERVAL_MS);
    }
}

static void __print_scene_list(vsf_test_shell_t *shell)
{
    vsf_trace_info("Scenes:" VSF_TRACE_CFG_LINEEND);
    for (uint8_t i = 0; i < shell->scene_count; i++) {
        vsf_trace_info("  %u %s" VSF_TRACE_CFG_LINEEND, i, shell->scenes[i].name);
    }
}

static void __print_case_list(vsf_test_shell_t *shell)
{
    if (shell->cur_scene < 0) {
        vsf_trace_info("No scene selected. Use 'vsf-test scene <n>' first." VSF_TRACE_CFG_LINEEND);
        return;
    }
    vsf_test_shell_scene_t *sc = &shell->scenes[shell->cur_scene];
    vsf_trace_info("Cases in '%s':" VSF_TRACE_CFG_LINEEND, sc->name);
    for (uint16_t i = 0; i < sc->case_count; i++) {
        vsf_trace_info("  %u" VSF_TRACE_CFG_LINEEND, i);
    }
}

static void __print_current_scene(vsf_test_shell_t *shell)
{
    if (shell->cur_scene < 0) {
        vsf_trace_info("Current scene: all" VSF_TRACE_CFG_LINEEND);
    } else {
        vsf_trace_info("Current scene: %s" VSF_TRACE_CFG_LINEEND,
                       shell->scenes[shell->cur_scene].name);
    }
}

static void __print_current_case(vsf_test_shell_t *shell)
{
    if (shell->cur_scene < 0 && shell->cur_case < 0) {
        vsf_trace_info("Current case: all" VSF_TRACE_CFG_LINEEND);
    } else if (shell->cur_scene >= 0 && shell->cur_case < 0) {
        vsf_trace_info("Current case: all (in scene '%s')" VSF_TRACE_CFG_LINEEND,
                       shell->scenes[shell->cur_scene].name);
    } else if (shell->cur_scene >= 0) {
        vsf_trace_info("Current case: %s.%d" VSF_TRACE_CFG_LINEEND,
                       shell->scenes[shell->cur_scene].name,
                       (int)shell->cur_case);
    }
}

static void __print_config(vsf_test_shell_t *shell)
{
    vsf_trace_info("auto-case: %s" VSF_TRACE_CFG_LINEEND, shell->auto_case ? "on" : "off");
    vsf_trace_info("auto-scene: %s" VSF_TRACE_CFG_LINEEND, shell->auto_scene ? "on" : "off");
    __print_current_scene(shell);
    __print_current_case(shell);
}

static void __advance_case(vsf_test_shell_t *shell)
{
    vsf_test_shell_scene_t *sc = &shell->scenes[shell->cur_scene];
    shell->cur_case++;
    if (shell->cur_case >= (int8_t)sc->case_count) {
        shell->cur_case = -1;
        if (shell->auto_scene) {
            shell->cur_scene++;
            if (shell->cur_scene >= (int8_t)shell->scene_count) shell->cur_scene = -1;
        }
    }
}

static void __run_selection(vsf_test_shell_t *shell)
{
    if (shell->cur_scene < 0) {
        vsf_test_run_tests();
        return;
    }
    vsf_test_shell_scene_t *sc = &shell->scenes[shell->cur_scene];
    if (shell->cur_case < 0) {
        for (uint16_t i = 0; i < sc->case_count; i++) {
            uint16_t ci = sc->first_case_idx + i;
            vsf_test_run_case(ci);
            if (shell->auto_case) {
                shell->cur_case = (int8_t)(i + 1);
                if (shell->cur_case >= (int8_t)sc->case_count) {
                    shell->cur_case = -1;
                    if (shell->auto_scene) {
                        shell->cur_scene++;
                        if (shell->cur_scene >= (int8_t)shell->scene_count) shell->cur_scene = -1;
                    }
                }
            }
        }
    } else {
        uint16_t ci = sc->first_case_idx + shell->cur_case;
        vsf_test_run_case(ci);
        if (shell->auto_case) __advance_case(shell);
    }
    if (shell->cur_scene >= 0) {
        uint32_t total = sc->case_count;
        uint32_t pass = 0, fail = 0, skip = 0, wdt_pass = 0, wdt_fail = 0;
        for (uint16_t i = 0; i < total; i++) {
            uint16_t ci = sc->first_case_idx + i;
            vsf_test_result_t r = vsf_test_get_case_result(ci);
            switch (r) {
            case VSF_TEST_RESULT_PASS:     pass++;      break;
            case VSF_TEST_RESULT_FAIL:     fail++;      break;
            case VSF_TEST_RESULT_SKIP:     skip++;      break;
            case VSF_TEST_RESULT_WDT_PASS: wdt_pass++;  break;
            case VSF_TEST_RESULT_WDT_FAIL: wdt_fail++;  break;
            default: break;
            }
        }
        vsf_trace_info("[TEST] All test cases completed" VSF_TRACE_CFG_LINEEND);
        vsf_trace_info("[TEST] ========== Test Summary ==========" VSF_TRACE_CFG_LINEEND);
        vsf_trace_info("[TEST] Total test cases: %u" VSF_TRACE_CFG_LINEEND, total);
        vsf_trace_info("[TEST] Pass: %u, Fail: %u, Skip: %u, WDT Pass: %u, WDT Fail: %u" VSF_TRACE_CFG_LINEEND,
                       pass, fail, skip, wdt_pass, wdt_fail);
    }
}

static void __cmd_scene(vsf_test_shell_t *shell, char *args)
{
    if (args == NULL || args[0] == '\0') {
        __print_current_scene(shell);
    } else if (strcmp(args, "--list") == 0) {
        __print_scene_list(shell);
    } else {
        int n = atoi(args);
        if (n >= 0 && n < shell->scene_count) {
            shell->cur_scene = (int8_t)n;
            shell->cur_case  = -1;
            vsf_trace_info("Scene %d: %s" VSF_TRACE_CFG_LINEEND, n, shell->scenes[n].name);
        } else {
            vsf_trace_info("Invalid scene index" VSF_TRACE_CFG_LINEEND);
        }
    }
}

static void __cmd_case(vsf_test_shell_t *shell, char *args)
{
    if (args == NULL || args[0] == '\0') {
        __print_current_case(shell);
    } else if (strcmp(args, "--list") == 0) {
        __print_case_list(shell);
    } else {
        if (shell->cur_scene < 0) {
            vsf_trace_info("Select a scene first" VSF_TRACE_CFG_LINEEND);
            return;
        }
        int n = atoi(args);
        if (n >= 0 && n < (int)shell->scenes[shell->cur_scene].case_count) {
            shell->cur_case = (int8_t)n;
            vsf_trace_info("Case %d" VSF_TRACE_CFG_LINEEND, n);
        } else {
            vsf_trace_info("Invalid case index" VSF_TRACE_CFG_LINEEND);
        }
    }
}

static void __cmd_run(vsf_test_shell_t *shell, char *args)
{
    if (args == NULL || args[0] == '\0') {
        __run_selection(shell);
        return;
    }

    if (strcmp(args, "all") == 0) {
        shell->cur_scene = -1;
        shell->cur_case  = -1;
        __run_selection(shell);
        return;
    }

    char *dot = strchr(args, '.');
    char *case_spec = NULL;
    if (dot != NULL) {
        case_spec = dot + 1;
        *dot = '\0';
    }

    for (uint8_t i = 0; i < shell->scene_count; i++) {
        if (strcmp(shell->scenes[i].name, args) == 0) {
            shell->cur_scene = (int8_t)i;
            shell->cur_case = -1;
            if (case_spec != NULL && case_spec[0] != '\0') {
                int numeric_idx = atoi(case_spec);
                bool is_numeric = true;
                for (char *p = case_spec; *p != '\0'; p++) {
                    if (*p < '0' || *p > '9') { is_numeric = false; break; }
                }
                if (is_numeric && numeric_idx >= 0 && numeric_idx < (int)shell->scenes[i].case_count) {
                    shell->cur_case = (int8_t)numeric_idx;
                } else {
                    if (dot != NULL) *dot = '.';
                    vsf_trace_info("Case not found: %s" VSF_TRACE_CFG_LINEEND, case_spec);
                    return;
                }
            }
            if (dot != NULL) *dot = '.';
            vsf_trace_info("Scene ack: %s" VSF_TRACE_CFG_LINEEND, args);
            __run_selection(shell);
            return;
        }
    }

    if (dot != NULL) *dot = '.';
    vsf_trace_info("Scene not found: %s" VSF_TRACE_CFG_LINEEND, args);
}

static void __cmd_config(vsf_test_shell_t *shell, char *args)
{
    if (args == NULL || args[0] == '\0') { __print_config(shell); return; }
    char *space = strchr(args, ' ');
    char *sub = args, *val = NULL;
    if (space != NULL) { *space = '\0'; val = space + 1; }
    if (val == NULL) {
        vsf_trace_info("Usage: vsf-test config <key> <on|off>" VSF_TRACE_CFG_LINEEND);
        return;
    }
    bool *target = NULL;
    const char *key_name = NULL;
    if (strcmp(sub, "auto-case") == 0)       { target = &shell->auto_case;  key_name = "auto-case"; }
    else if (strcmp(sub, "auto-scene") == 0) { target = &shell->auto_scene; key_name = "auto-scene"; }
    else {
        vsf_trace_info("Unknown config key. Valid: auto-case, auto-scene" VSF_TRACE_CFG_LINEEND);
        return;
    }
    if (strcmp(val, "on") == 0)       *target = true;
    else if (strcmp(val, "off") == 0) *target = false;
    else {
        vsf_trace_info("Usage: vsf-test config %s on|off" VSF_TRACE_CFG_LINEEND, key_name);
        return;
    }
    vsf_trace_info("%s %s" VSF_TRACE_CFG_LINEEND, key_name, *target ? "on" : "off");
}

static void __dispatch(vsf_test_shell_t *shell, char *line)
{
    const char *prefix = "vsf-test ";
    size_t prefix_len = strlen(prefix);
    if (strncmp(line, prefix, prefix_len) != 0) {
        vsf_trace_info("Unknown command. Try 'vsf-test scene --list'" VSF_TRACE_CFG_LINEEND);
        return;
    }
    char *rest = line + prefix_len;
    char *space = strchr(rest, ' ');
    char *cmd = rest, *args = NULL;
    if (space != NULL) {
        *space = '\0';
        args = space + 1;
        while (*args == ' ') args++;
        if (*args == '\0') args = NULL;
    }
    if (strcmp(cmd, "scene") == 0)       __cmd_scene(shell, args);
    else if (strcmp(cmd, "case") == 0)   __cmd_case(shell, args);
    else if (strcmp(cmd, "run") == 0)    __cmd_run(shell, args);
    else if (strcmp(cmd, "config") == 0) __cmd_config(shell, args);
    else vsf_trace_info("Unknown command. Try 'vsf-test scene --list'" VSF_TRACE_CFG_LINEEND);
}

uint8_t vsf_test_shell_register_scene(vsf_test_shell_t *shell, const char *name)
{
    if (shell == NULL || shell->scene_count >= VSF_TEST_SHELL_MAX_SCENES) return 0;
    uint8_t idx = shell->scene_count;
    shell->scenes[idx].name           = name;
    // first_case_idx is the framework's current total case count (i.e. the
    // index that the next vsf_test_add_ex() call will populate).
    shell->scenes[idx].first_case_idx = (uint16_t)vsf_test_get_case_count();
    shell->scenes[idx].case_count     = 0;
    shell->scene_count++;
    return idx;
}

void vsf_test_shell_inc_case_count(vsf_test_shell_t *shell)
{
    if (shell == NULL || shell->scene_count == 0) return;
    shell->scenes[shell->scene_count - 1].case_count++;
}

void vsf_test_shell_init(vsf_test_shell_t *shell)
{
    memset(shell, 0, sizeof(*shell));
    shell->cur_scene = -1;
    shell->cur_case  = -1;
    vsf_stream_connect_rx(&VSF_DEBUG_STREAM_RX.use_as__vsf_stream_t);
    vsf_trace_info("VSF Test Ready" VSF_TRACE_CFG_LINEEND);
    vsf_trace_info("> " VSF_TRACE_CFG_LINEEND);
}

void vsf_test_shell_run(vsf_test_shell_t *shell)
{
    char line[LINE_BUF_SIZE];
    while (1) {
        __read_line(line, sizeof(line));
        if (line[0] != '\0') __dispatch(shell, line);
        vsf_trace_info("> " VSF_TRACE_CFG_LINEEND);
    }
}
