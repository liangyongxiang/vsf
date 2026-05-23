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

static void __print_suite_list(vsf_test_shell_t *shell)
{
    vsf_trace_info("Scenes:" VSF_TRACE_CFG_LINEEND);
    for (uint8_t i = 0; i < shell->suite_count; i++) {
        vsf_trace_info("  %u %s" VSF_TRACE_CFG_LINEEND, i, shell->suites[i].name);
    }
}

static void __print_case_list(vsf_test_shell_t *shell)
{
    if (shell->cur_suite < 0) {
        vsf_trace_info("No suite selected. Use 'vsf-test suite <n>' first." VSF_TRACE_CFG_LINEEND);
        return;
    }
    vsf_test_shell_suite_t *sc = &shell->suites[shell->cur_suite];
    vsf_trace_info("Cases in '%s':" VSF_TRACE_CFG_LINEEND, sc->name);
    for (uint16_t i = 0; i < sc->case_count; i++) {
        vsf_trace_info("  %u" VSF_TRACE_CFG_LINEEND, i);
    }
}

static void __print_current_suite(vsf_test_shell_t *shell)
{
    if (shell->cur_suite < 0) {
        vsf_trace_info("Current suite: all" VSF_TRACE_CFG_LINEEND);
    } else {
        vsf_trace_info("Current suite: %s" VSF_TRACE_CFG_LINEEND,
                       shell->suites[shell->cur_suite].name);
    }
}

static void __print_current_case(vsf_test_shell_t *shell)
{
    if (shell->cur_suite < 0 && shell->cur_case < 0) {
        vsf_trace_info("Current case: all" VSF_TRACE_CFG_LINEEND);
    } else if (shell->cur_suite >= 0 && shell->cur_case < 0) {
        vsf_trace_info("Current case: all (in suite '%s')" VSF_TRACE_CFG_LINEEND,
                       shell->suites[shell->cur_suite].name);
    } else if (shell->cur_suite >= 0) {
        vsf_trace_info("Current case: %s.%d" VSF_TRACE_CFG_LINEEND,
                       shell->suites[shell->cur_suite].name,
                       (int)shell->cur_case);
    }
}

static void __print_config(vsf_test_shell_t *shell)
{
    vsf_trace_info("auto-case: %s" VSF_TRACE_CFG_LINEEND, shell->auto_case ? "on" : "off");
    vsf_trace_info("auto-suite: %s" VSF_TRACE_CFG_LINEEND, shell->auto_suite ? "on" : "off");
    __print_current_suite(shell);
    __print_current_case(shell);
}

static void __advance_case(vsf_test_shell_t *shell)
{
    vsf_test_shell_suite_t *sc = &shell->suites[shell->cur_suite];
    shell->cur_case++;
    if (shell->cur_case >= (int8_t)sc->case_count) {
        shell->cur_case = -1;
        if (shell->auto_suite) {
            shell->cur_suite++;
            if (shell->cur_suite >= (int8_t)shell->suite_count) shell->cur_suite = -1;
        }
    }
}

static void __run_selection(vsf_test_shell_t *shell)
{
    if (shell->cur_suite < 0) {
        vsf_test_run_tests();
        return;
    }
    vsf_test_shell_suite_t *sc = &shell->suites[shell->cur_suite];
    if (shell->cur_case < 0) {
        /* Default: sequential order. If shuffle_seed != 0 and the suite
         * fits in the stack buffer, shuffle via Fisher-Yates seeded with
         * the host-provided value. The seed is single-use: each
         * __run_selection consumes it and the host re-issues for the
         * next suite. */
        uint16_t order[VSF_TEST_SHELL_MAX_CASES_PER_SUITE];
        uint16_t n = sc->case_count;
        bool shuffled = false;
        if (shell->shuffle_seed != 0 && n <= VSF_TEST_SHELL_MAX_CASES_PER_SUITE) {
            for (uint16_t i = 0; i < n; i++) { order[i] = i; }
            /* glibc-equivalent LCG: state = state * 1103515245 + 12345.
             * Self-contained so the same seed produces the same order
             * across toolchains, in contrast to libc rand(). */
            uint32_t state = shell->shuffle_seed;
            for (uint16_t i = n - 1; i > 0; i--) {
                state = state * 1103515245u + 12345u;
                uint16_t j = (uint16_t)((state >> 16) % (uint32_t)(i + 1));
                uint16_t tmp = order[i];
                order[i] = order[j];
                order[j] = tmp;
            }
            shuffled = true;
            /* Echo the shuffled order so it lands in the serial audit log. */
            vsf_trace_info("[TEST] Shuffle seed: %lu, order:",
                           (unsigned long)shell->shuffle_seed);
            for (uint16_t i = 0; i < n; i++) {
                vsf_trace_info(" %u", (unsigned)order[i]);
            }
            vsf_trace_info(VSF_TRACE_CFG_LINEEND);
            /* Single-use: clear so the next suite reverts to sequential
             * unless the host re-seeds. */
            shell->shuffle_seed = 0;
        }
        for (uint16_t i = 0; i < n; i++) {
            uint16_t local = shuffled ? order[i] : i;
            uint16_t ci = sc->first_case_idx + local;
            vsf_test_run_case(ci);
            if (shell->auto_case) {
                shell->cur_case = (int8_t)(i + 1);
                if (shell->cur_case >= (int8_t)sc->case_count) {
                    shell->cur_case = -1;
                    if (shell->auto_suite) {
                        shell->cur_suite++;
                        if (shell->cur_suite >= (int8_t)shell->suite_count) shell->cur_suite = -1;
                    }
                }
            }
        }
    } else {
        uint16_t ci = sc->first_case_idx + shell->cur_case;
        vsf_test_run_case(ci);
        if (shell->auto_case) __advance_case(shell);
    }
    if (shell->cur_suite >= 0) {
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

static void __cmd_suite(vsf_test_shell_t *shell, char *args)
{
    if (args == NULL || args[0] == '\0') {
        __print_current_suite(shell);
    } else if (strcmp(args, "--list") == 0) {
        __print_suite_list(shell);
    } else {
        int n = atoi(args);
        if (n >= 0 && n < shell->suite_count) {
            shell->cur_suite = (int8_t)n;
            shell->cur_case  = -1;
            vsf_trace_info("Scene %d: %s" VSF_TRACE_CFG_LINEEND, n, shell->suites[n].name);
        } else {
            vsf_trace_info("Invalid suite index" VSF_TRACE_CFG_LINEEND);
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
        if (shell->cur_suite < 0) {
            vsf_trace_info("Select a suite first" VSF_TRACE_CFG_LINEEND);
            return;
        }
        int n = atoi(args);
        if (n >= 0 && n < (int)shell->suites[shell->cur_suite].case_count) {
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
        shell->cur_suite = -1;
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

    for (uint8_t i = 0; i < shell->suite_count; i++) {
        if (strcmp(shell->suites[i].name, args) == 0) {
            shell->cur_suite = (int8_t)i;
            shell->cur_case = -1;
            if (case_spec != NULL && case_spec[0] != '\0') {
                int numeric_idx = atoi(case_spec);
                bool is_numeric = true;
                for (char *p = case_spec; *p != '\0'; p++) {
                    if (*p < '0' || *p > '9') { is_numeric = false; break; }
                }
                if (is_numeric && numeric_idx >= 0 && numeric_idx < (int)shell->suites[i].case_count) {
                    shell->cur_case = (int8_t)numeric_idx;
                } else {
                    if (dot != NULL) *dot = '.';
                    vsf_trace_info("Case not found: %s" VSF_TRACE_CFG_LINEEND, case_spec);
                    return;
                }
            }
            if (dot != NULL) *dot = '.';
            vsf_trace_info("Suite ack: %s" VSF_TRACE_CFG_LINEEND, args);
            __run_selection(shell);
            return;
        }
    }

    if (dot != NULL) *dot = '.';
    vsf_trace_info("Suite not found: %s" VSF_TRACE_CFG_LINEEND, args);
}

static void __cmd_config(vsf_test_shell_t *shell, char *args)
{
    if (args == NULL || args[0] == '\0') { __print_config(shell); return; }
    char *space = strchr(args, ' ');
    char *sub = args, *val = NULL;
    if (space != NULL) { *space = '\0'; val = space + 1; }
    if (val == NULL) {
        vsf_trace_info("Usage: vsf-test config <key> <on|off|N>" VSF_TRACE_CFG_LINEEND);
        return;
    }
    /* `shuffle <N>` is numeric, not on|off — N=0 disables, N>0 seeds the
     * Fisher-Yates shuffle for the next `vsf-test run <suite>`. */
    if (strcmp(sub, "shuffle") == 0) {
        unsigned long seed = strtoul(val, NULL, 10);
        shell->shuffle_seed = (uint32_t)seed;
        vsf_trace_info("shuffle %s (seed=%lu)" VSF_TRACE_CFG_LINEEND,
                       seed ? "on" : "off", seed);
        return;
    }
    bool *target = NULL;
    const char *key_name = NULL;
    if (strcmp(sub, "auto-case") == 0)       { target = &shell->auto_case;  key_name = "auto-case"; }
    else if (strcmp(sub, "auto-suite") == 0) { target = &shell->auto_suite; key_name = "auto-suite"; }
    else {
        vsf_trace_info("Unknown config key. Valid: auto-case, auto-suite, shuffle" VSF_TRACE_CFG_LINEEND);
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
        vsf_trace_info("Unknown command. Try 'vsf-test suite --list'" VSF_TRACE_CFG_LINEEND);
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
    if (strcmp(cmd, "suite") == 0)       __cmd_suite(shell, args);
    else if (strcmp(cmd, "case") == 0)   __cmd_case(shell, args);
    else if (strcmp(cmd, "run") == 0)    __cmd_run(shell, args);
    else if (strcmp(cmd, "config") == 0) __cmd_config(shell, args);
    else vsf_trace_info("Unknown command. Try 'vsf-test suite --list'" VSF_TRACE_CFG_LINEEND);
}

uint8_t vsf_test_shell_register_suite(vsf_test_shell_t *shell, const char *name)
{
    if (shell == NULL || shell->suite_count >= VSF_TEST_SHELL_MAX_SUITES) return 0;
    uint8_t idx = shell->suite_count;
    shell->suites[idx].name           = name;
    // first_case_idx is the framework's current total case count (i.e. the
    // index that the next suite_add_case call will populate).
    shell->suites[idx].first_case_idx = (uint16_t)vsf_test_get_case_count();
    shell->suites[idx].case_count     = 0;
    shell->suite_count++;
    return idx;
}

void vsf_test_shell_inc_case_count(vsf_test_shell_t *shell)
{
    if (shell == NULL || shell->suite_count == 0) return;
    shell->suites[shell->suite_count - 1].case_count++;
}

void vsf_test_shell_init(vsf_test_shell_t *shell)
{
    memset(shell, 0, sizeof(*shell));
    shell->cur_suite = -1;
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
