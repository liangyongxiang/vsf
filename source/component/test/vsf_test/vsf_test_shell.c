#include "vsf.h"
#include "vsf_board.h"
#include "component/test/vsf_test/vsf_test.h"

#include <string.h>

#include "./vsf_test_shell.h"

extern vsf_mem_stream_t VSF_DEBUG_STREAM_RX;
extern void vsf_test_run_suite_case(vsf_test_suite_t *suite, uint16_t local_idx);

#define POLL_INTERVAL_MS    1
#define LINE_BUF_SIZE       128

/* ------------------------------------------------------------------------ */
/* Line reader                                                              */
/* ------------------------------------------------------------------------ */

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

/* ------------------------------------------------------------------------ */
/* Helpers                                                                  */
/* ------------------------------------------------------------------------ */

static int __find_suite_by_name(vsf_test_shell_t *shell, const char *name)
{
    for (uint8_t i = 0; i < shell->suite_count; i++) {
        if (shell->suites[i] != NULL && shell->suites[i]->name != NULL
            && strcmp(shell->suites[i]->name, name) == 0) {
            return (int)i;
        }
    }
    return -1;
}

static void __print_summary(vsf_test_suite_t *suite)
{
    uint32_t total = suite->case_count;
    uint32_t pass = 0, fail = 0, skip = 0;
    for (uint16_t i = 0; i < total; i++) {
        vsf_test_result_t r = (vsf_test_result_t)suite->cases[i].result;
        switch (r) {
        case VSF_TEST_RESULT_PASS: pass++; break;
        case VSF_TEST_RESULT_FAIL: fail++; break;
        case VSF_TEST_RESULT_SKIP: skip++; break;
        default: break;
        }
    }
    vsf_trace_info("[TEST] All test cases completed" VSF_TRACE_CFG_LINEEND);
    vsf_trace_info("[TEST] ========== Test Summary ==========" VSF_TRACE_CFG_LINEEND);
    vsf_trace_info("[TEST] Total test cases: %u" VSF_TRACE_CFG_LINEEND, total);
    vsf_trace_info("[TEST] Pass: %u, Fail: %u, Skip: %u" VSF_TRACE_CFG_LINEEND,
                   pass, fail, skip);
}

static void __run_suite(vsf_test_shell_t *shell, vsf_test_suite_t *suite)
{
    (void)shell;
    if (suite == NULL || suite->cases == NULL) return;
    for (uint16_t i = 0; i < suite->case_count; i++) {
        vsf_test_run_suite_case(suite, i);
    }
    __print_summary(suite);
}

/* ------------------------------------------------------------------------ */
/* Commands                                                                 */
/* ------------------------------------------------------------------------ */

static void __cmd_list_suites(vsf_test_shell_t *shell)
{
    vsf_trace_info("Scenes:" VSF_TRACE_CFG_LINEEND);
    for (uint8_t i = 0; i < shell->suite_count; i++) {
        vsf_trace_info("  %u %s" VSF_TRACE_CFG_LINEEND,
                       i, shell->suites[i]->name);
    }
}

static void __cmd_list_cases(vsf_test_shell_t *shell, const char *name)
{
    int idx = __find_suite_by_name(shell, name);
    if (idx < 0) {
        vsf_trace_info("Suite not found: %s" VSF_TRACE_CFG_LINEEND, name);
        return;
    }
    vsf_test_suite_t *suite = shell->suites[idx];
    uint16_t count = (suite != NULL && suite->cases != NULL) ? suite->case_count : 0;
    vsf_trace_info("Cases in '%s':" VSF_TRACE_CFG_LINEEND, suite->name);
    for (uint16_t i = 0; i < count; i++) {
        vsf_trace_info("  %u" VSF_TRACE_CFG_LINEEND, i);
    }
}

static void __cmd_run_all(vsf_test_shell_t *shell)
{
    for (uint8_t si = 0; si < shell->suite_count; si++) {
        vsf_test_suite_t *s = shell->suites[si];
        if (s == NULL || s->cases == NULL) continue;
        __run_suite(shell, s);
    }
}

static void __cmd_run_suite(vsf_test_shell_t *shell, const char *name)
{
    /* Exact name match first. */
    int idx = __find_suite_by_name(shell, name);
    if (idx >= 0) {
        vsf_trace_info("Suite ack: %s" VSF_TRACE_CFG_LINEEND, name);
        __run_suite(shell, shell->suites[idx]);
        return;
    }

    /* Purpose-based multi-instance fallback (transitional). */
    uint8_t matches[VSF_TEST_SHELL_MAX_MATCHES];
    uint8_t match_count = 0;
    for (uint8_t i = 0; i < shell->suite_count; i++) {
        vsf_test_suite_t *suite = shell->suites[i];
        if (suite != NULL && suite->purpose != NULL && strcmp(suite->purpose, name) == 0) {
            matches[match_count++] = i;
        }
    }

    if (match_count > 0) {
        vsf_trace_info("Suite ack: %s (%u instance(s))" VSF_TRACE_CFG_LINEEND,
                       name, match_count);
        uint32_t total = 0, pass = 0, fail = 0, skip = 0;
        for (uint8_t m = 0; m < match_count; m++) {
            vsf_test_suite_t *suite = shell->suites[matches[m]];
            __run_suite(shell, suite);
            total += suite->case_count;
            for (uint16_t i = 0; i < suite->case_count; i++) {
                vsf_test_result_t r = (vsf_test_result_t)suite->cases[i].result;
                switch (r) {
                case VSF_TEST_RESULT_PASS: pass++; break;
                case VSF_TEST_RESULT_FAIL: fail++; break;
                case VSF_TEST_RESULT_SKIP: skip++; break;
                default: break;
                }
            }
        }
        vsf_trace_info("[TEST] All test cases completed" VSF_TRACE_CFG_LINEEND);
        vsf_trace_info("[TEST] ========== Test Summary ==========" VSF_TRACE_CFG_LINEEND);
        vsf_trace_info("[TEST] Total test cases: %u" VSF_TRACE_CFG_LINEEND, total);
        vsf_trace_info("[TEST] Pass: %u, Fail: %u, Skip: %u" VSF_TRACE_CFG_LINEEND,
                       pass, fail, skip);
        return;
    }

    vsf_trace_info("Suite not found: %s" VSF_TRACE_CFG_LINEEND, name);
}

static void __cmd_run_case(vsf_test_shell_t *shell, const char *name, int case_idx)
{
    int idx = __find_suite_by_name(shell, name);
    if (idx < 0) {
        vsf_trace_info("Suite not found: %s" VSF_TRACE_CFG_LINEEND, name);
        return;
    }
    vsf_test_suite_t *suite = shell->suites[idx];
    if (case_idx < 0 || case_idx >= (int)suite->case_count) {
        vsf_trace_info("Case not found: %d" VSF_TRACE_CFG_LINEEND, case_idx);
        return;
    }
    vsf_trace_info("Suite ack: %s.%d" VSF_TRACE_CFG_LINEEND, name, case_idx);
    vsf_test_run_suite_case(suite, (uint16_t)case_idx);
    __print_summary(suite);
}

/* ------------------------------------------------------------------------ */
/* Dispatch                                                                 */
/* ------------------------------------------------------------------------ */

static void __dispatch(vsf_test_shell_t *shell, char *line)
{
    const char *prefix = "vsf-test ";
    size_t prefix_len = strlen(prefix);
    if (strncmp(line, prefix, prefix_len) != 0) {
        vsf_trace_info("Unknown command. Try 'vsf-test list-suites'" VSF_TRACE_CFG_LINEEND);
        return;
    }
    char *rest = line + prefix_len;

    if (strcmp(rest, "list-suites") == 0) {
        __cmd_list_suites(shell);
    } else if (strncmp(rest, "list-cases ", 11) == 0) {
        __cmd_list_cases(shell, rest + 11);
    } else if (strcmp(rest, "run-all") == 0) {
        __cmd_run_all(shell);
    } else if (strncmp(rest, "run-suite ", 10) == 0) {
        __cmd_run_suite(shell, rest + 10);
    } else if (strncmp(rest, "run-case ", 9) == 0) {
        char *name = rest + 9;
        char *space = strrchr(name, ' ');
        if (space == NULL) {
            vsf_trace_info("Usage: vsf-test run-case <name> <n>" VSF_TRACE_CFG_LINEEND);
            return;
        }
        *space = '\0';
        int n = atoi(space + 1);
        __cmd_run_case(shell, name, n);
    } else {
        vsf_trace_info("Unknown command. Try 'vsf-test list-suites'" VSF_TRACE_CFG_LINEEND);
    }
}

/* ------------------------------------------------------------------------ */
/* Public API                                                               */
/* ------------------------------------------------------------------------ */

void vsf_test_shell_init(vsf_test_shell_t *shell, vsf_test_suite_t **suites, uint8_t count)
{
    shell->suites      = suites;
    shell->suite_count = count;
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
