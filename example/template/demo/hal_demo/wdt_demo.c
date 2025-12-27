/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
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

#if APP_USE_HAL_DEMO == ENABLED && APP_USE_HAL_WDT_DEMO == ENABLED && VSF_HAL_USE_WDT == ENABLED

#if APP_USE_LINUX_DEMO == ENABLED
#   include <string.h>
#endif
#include <inttypes.h>

/*============================ MACROS ========================================*/

#ifndef APP_WDT_DEMO_CFG_DEFAULT_INSTANCE
#   define APP_WDT_DEMO_CFG_DEFAULT_INSTANCE        vsf_hw_wwdt0
#endif

#ifndef APP_WDT_DEMO_CFG_IRQ_PRIO
#   define APP_WDT_DEMO_CFG_IRQ_PRIO                vsf_arch_prio_2
#endif

// WDT 超时时间配置
#define WDT_TEST_TIMEOUT_MS_MAX                    2000    // 最大超时时间 2 秒（作为上限）
#define WDT_TEST_TIMEOUT_RATIO                      80      // 使用硬件最大超时时间的百分比
#define WDT_TEST_WINDOW_RATIO                       40      // 窗口时间使用硬件最大超时时间的百分比
// WWDT 窗口时间说明：
// - max_ms: 最大超时时间，超过此时间未喂狗会导致系统复位
// - min_ms: 最小超时时间（窗口起始时间），在此时间之前喂狗也会导致系统复位（过早喂狗保护）
// - 有效喂狗窗口：[min_ms, max_ms]，必须在此时间窗口内喂狗
//   时间轴：0ms -------- min_ms -------- max_ms
//           禁止喂狗     允许喂狗窗口    超时复位

// 测试时间配置（统一使用百分比，便于理解和调整）
#define WDT_TEST_FEED_INTERVAL_RATIO                50      // 喂狗间隔比率（相对于超时时间的百分比，默认 50%）
#define WDT_TEST_SAFETY_MARGIN_RATIO                10      // 安全余量比率（用于复位测试和窗口等待，默认 10%）
#define WDT_TEST_INTERRUPT_TIMEOUT_RATIO             150     // 中断测试超时比率（相对于 max_ms 的百分比，默认 150%）
#define WDT_TEST_FEED_DURATION_MULTIPLIER           3       // 喂狗测试持续时间倍数（相对于 max_ms）
#define WDT_TEST_FEED_DURATION_MIN_MS               100     // 喂狗测试最小持续时间（毫秒）

// 固定时间配置（这些值通常不需要调整）
#define WDT_TEST_MIN_TIME_MS                         1       // 最小时间值（毫秒）
#define WDT_TEST_POLL_INTERVAL_MS                    10      // 轮询间隔（毫秒）
#define WDT_TEST_WAIT_REPORT_INTERVAL                10      // 等待进度报告间隔（相对于等待时间的百分比）

/*============================ MACROFIED FUNCTIONS ===========================*/

// Helper macro for trace with timestamp
// Note: Use PRIu64 for vsf_systimer_tick_t to avoid format mismatch
#define __WDT_TRACE_INFO(fmt, ...)                  do { \
                                                        uint64_t __wdt_time_ms = (uint64_t)vsf_systimer_get_ms(); \
                                                        vsf_trace_info("[%" PRIu64 " ms] " fmt, __wdt_time_ms, ##__VA_ARGS__); \
                                                    } while(0)
#define __WDT_TRACE_ERROR(fmt, ...)                 do { \
                                                        uint64_t __wdt_time_ms = (uint64_t)vsf_systimer_get_ms(); \
                                                        vsf_trace_error("[%" PRIu64 " ms] " fmt, __wdt_time_ms, ##__VA_ARGS__); \
                                                    } while(0)
#define __WDT_TRACE_WARNING(fmt, ...)               do { \
                                                        uint64_t __wdt_time_ms = (uint64_t)vsf_systimer_get_ms(); \
                                                        vsf_trace_warning("[%" PRIu64 " ms] " fmt, __wdt_time_ms, ##__VA_ARGS__); \
                                                    } while(0)
/*============================ TYPES =========================================*/

typedef struct wdt_test_t {
    vsf_wdt_t *wdt;
    volatile bool irq_triggered;
    volatile uint32_t irq_count;
    bool test_reset;
} wdt_test_t;

/*============================ GLOBAL VARIABLES ==============================*/
/*============================ PROTOTYPES ====================================*/

static void __wdt_irq_handler(void *target_ptr, vsf_wdt_t *wdt_ptr);
static void __wdt_cleanup(vsf_wdt_t *wdt, vsf_wdt_capability_t *capability);
static bool __wdt_init_and_enable(wdt_test_t *ctx, vsf_wdt_cfg_t *cfg);
static void __wdt_print_capability(const vsf_wdt_capability_t *capability);
static void __wdt_print_config(const vsf_wdt_cfg_t *cfg, const vsf_wdt_capability_t *capability, const char *mode_str);
static uint32_t __wdt_calc_feed_interval(const vsf_wdt_cfg_t *cfg, const vsf_wdt_capability_t *capability);
static void __wdt_wait_for_window(uint32_t wait_ms, uint32_t min_ms, uint32_t max_ms);
static void __wdt_prepare_mode_cfg(const vsf_wdt_capability_t *capability, vsf_wdt_cfg_t *cfg, vsf_wdt_mode_t mode, wdt_test_t *ctx, bool use_irq);
static bool __wdt_prepare_test(wdt_test_t *ctx, vsf_wdt_capability_t *capability, vsf_wdt_cfg_t *cfg, vsf_wdt_mode_t mode, bool use_irq, bool test_reset, const char *mode_str, bool log_capability);
static bool __wdt_test_interrupt(wdt_test_t *ctx);
static bool __wdt_test_reset(wdt_test_t *ctx);
static bool __wdt_test_feed(wdt_test_t *ctx);
static uint32_t __wdt_clamp_ms(uint32_t value);
static bool __wdt_pick_reset_mode(const vsf_wdt_capability_t *capability, vsf_wdt_mode_t *mode);

/*============================ IMPLEMENTATION ================================*/

static void __wdt_irq_handler(void *target_ptr, vsf_wdt_t *wdt_ptr)
{
    wdt_test_t *ctx = (wdt_test_t *)target_ptr;

    if (ctx != NULL) {
        ctx->irq_triggered = true;
        ctx->irq_count++;

        // 在中断中喂狗，防止复位
        if (!ctx->test_reset) {
            vsf_wdt_feed(wdt_ptr);
        }
    }
}

// Helper function to calculate timeout and window based on hardware capability
static void __wdt_calc_timeout_config(const vsf_wdt_capability_t *capability, vsf_wdt_cfg_t *cfg)
{
    // 根据硬件能力设置超时时间：使用最大超时时间的指定比率，但不超过 WDT_TEST_TIMEOUT_MS_MAX
    uint32_t calculated_timeout = (capability->max_timeout_ms * WDT_TEST_TIMEOUT_RATIO) / 100;
    cfg->max_ms = __wdt_clamp_ms(calculated_timeout);
    // 如果支持窗口模式（WWDT），设置窗口时间（使用最大超时时间的指定比率）
    if (capability->support_min_timeout) {
        uint32_t calculated_window = (capability->max_timeout_ms * WDT_TEST_WINDOW_RATIO) / 100;
        cfg->min_ms = __wdt_clamp_ms(calculated_window);
        if (cfg->min_ms >= cfg->max_ms) {
            cfg->min_ms = cfg->max_ms / 2;  // 确保 min < max
        }
    } else {
        cfg->min_ms = 0;
    }
}

// Helper function to print WDT capability
static void __wdt_print_capability(const vsf_wdt_capability_t *capability)
{
    __WDT_TRACE_INFO("WDT Capability: early_wakeup=%d, reset_none=%d, reset_cpu=%d, reset_soc=%d, disable=%d, min_timeout=%d" VSF_TRACE_CFG_LINEEND,
                   capability->support_early_wakeup,
                   capability->support_reset_none,
                   capability->support_reset_cpu,
                   capability->support_reset_soc,
                   capability->support_disable,
                   capability->support_min_timeout);
}

// Helper function to print WDT configuration
static void __wdt_print_config(const vsf_wdt_cfg_t *cfg, const vsf_wdt_capability_t *capability, const char *mode_str)
{
    if (capability->support_min_timeout && cfg->min_ms > 0) {
        __WDT_TRACE_INFO("WDT initialized with %s mode, timeout: max=%d ms, min=%d ms (window)" VSF_TRACE_CFG_LINEEND,
                        mode_str, cfg->max_ms, cfg->min_ms);
    } else {
        __WDT_TRACE_INFO("WDT initialized with %s mode, timeout: %d ms" VSF_TRACE_CFG_LINEEND,
                        mode_str, cfg->max_ms);
    }
}

// Helper function to calculate feed interval based on configuration
static uint32_t __wdt_calc_feed_interval(const vsf_wdt_cfg_t *cfg, const vsf_wdt_capability_t *capability)
{
    uint32_t feed_interval_ms;

    // 对于 WWDT，使用窗口时间的指定比率作为喂狗间隔
    if (capability->support_min_timeout && cfg->min_ms > 0) {
        feed_interval_ms = (cfg->min_ms * WDT_TEST_FEED_INTERVAL_RATIO) / 100;
        // 对于 WWDT，喂狗间隔必须在窗口 [min_ms, max_ms] 内
        // 如果计算出的间隔小于 min_ms，调整为 min_ms
        feed_interval_ms = feed_interval_ms < cfg->min_ms ? cfg->min_ms : feed_interval_ms;
        // 如果计算出的间隔大于 max_ms，调整为 max_ms（避免超时）
        feed_interval_ms = feed_interval_ms > cfg->max_ms ? cfg->max_ms : feed_interval_ms;
    } else {
        // 对于普通 WDT，使用最大超时时间的指定比率作为喂狗间隔
        feed_interval_ms = (cfg->max_ms * WDT_TEST_FEED_INTERVAL_RATIO) / 100;
    }

    // 确保至少 1ms
    if (feed_interval_ms < WDT_TEST_MIN_TIME_MS) {
        feed_interval_ms = WDT_TEST_MIN_TIME_MS;
    }

    return feed_interval_ms;
}

// Helper function to wait for WWDT window with progress reporting
static void __wdt_wait_for_window(uint32_t wait_ms, uint32_t min_ms, uint32_t max_ms)
{
    if (wait_ms < WDT_TEST_MIN_TIME_MS) {
        wait_ms = WDT_TEST_MIN_TIME_MS;
    }

    __WDT_TRACE_INFO("Waiting %d ms before first feed (WWDT window: [%d, %d] ms)..." VSF_TRACE_CFG_LINEEND,
                    wait_ms, min_ms, max_ms);

    vsf_systimer_tick_t wait_start_tick = vsf_systimer_get_tick();
    vsf_systimer_tick_t wait_duration_tick = vsf_systimer_ms_to_tick(wait_ms);
    uint32_t elapsed_ms = 0;
    uint32_t report_interval_ms = wait_ms / WDT_TEST_WAIT_REPORT_INTERVAL;

    if (report_interval_ms < WDT_TEST_MIN_TIME_MS) {
        report_interval_ms = WDT_TEST_MIN_TIME_MS;
    }

    while (vsf_systimer_get_tick() - wait_start_tick < wait_duration_tick) {
        vsf_systimer_tick_t current_tick = vsf_systimer_get_tick();
        uint32_t current_elapsed_ms = vsf_systimer_tick_to_ms(current_tick - wait_start_tick);

        // 定期输出进度
        if (current_elapsed_ms >= elapsed_ms + report_interval_ms) {
            elapsed_ms = current_elapsed_ms;
            __WDT_TRACE_INFO("Waiting... %d/%d ms" VSF_TRACE_CFG_LINEEND, elapsed_ms, wait_ms);
        }
        vsf_thread_delay_ms(WDT_TEST_MIN_TIME_MS);
    }
}

// Helper function to cleanup WDT (disable and fini if supported)
static void __wdt_cleanup(vsf_wdt_t *wdt, vsf_wdt_capability_t *capability)
{
    if (wdt == NULL) {
        return;
    }

    if (capability != NULL && capability->support_disable) {
        fsm_rt_t fsm_rt = vsf_wdt_disable(wdt);
        if (fsm_rt != fsm_rt_cpl) {
            __WDT_TRACE_ERROR("WDT disable failed: %d" VSF_TRACE_CFG_LINEEND, fsm_rt);
        }
        vsf_wdt_fini(wdt);
    } else {
        __WDT_TRACE_WARNING("WDT does not support disable, it will continue running!" VSF_TRACE_CFG_LINEEND);
        // Feed WDT to prevent reset before returning
        vsf_wdt_feed(wdt);
        // Do not call fini if disable is not supported
    }
}

// Helper function to initialize and enable WDT
static bool __wdt_init_and_enable(wdt_test_t *ctx, vsf_wdt_cfg_t *cfg)
{
    if (ctx == NULL || ctx->wdt == NULL || cfg == NULL) {
        __WDT_TRACE_ERROR("WDT test context, device or config is NULL!" VSF_TRACE_CFG_LINEEND);
        return false;
    }

    vsf_wdt_t *wdt = ctx->wdt;
    vsf_err_t err;
    fsm_rt_t fsm_rt;

    // 初始化 WDT
    err = vsf_wdt_init(wdt, cfg);
    if (err != VSF_ERR_NONE) {
        __WDT_TRACE_ERROR("WDT init failed: %d" VSF_TRACE_CFG_LINEEND, err);
        return false;
    }

    // 启用 WDT
    fsm_rt = vsf_wdt_enable(wdt);
    if (fsm_rt != fsm_rt_cpl) {
        __WDT_TRACE_ERROR("WDT enable failed: %d" VSF_TRACE_CFG_LINEEND, fsm_rt);
        vsf_wdt_fini(wdt);
        return false;
    }

    return true;
}

static void __wdt_prepare_mode_cfg(const vsf_wdt_capability_t *capability, vsf_wdt_cfg_t *cfg, vsf_wdt_mode_t mode, wdt_test_t *ctx, bool use_irq)
{
    cfg->mode = mode;
    __wdt_calc_timeout_config(capability, cfg);
    cfg->isr.handler_fn = use_irq ? __wdt_irq_handler : NULL;
    cfg->isr.target_ptr = use_irq ? ctx : NULL;
    cfg->isr.prio = APP_WDT_DEMO_CFG_IRQ_PRIO;
}

static bool __wdt_prepare_test(wdt_test_t *ctx, vsf_wdt_capability_t *capability, vsf_wdt_cfg_t *cfg, vsf_wdt_mode_t mode, bool use_irq, bool test_reset, const char *mode_str, bool log_capability)
{
    if ((ctx == NULL) || (ctx->wdt == NULL) || (capability == NULL) || (cfg == NULL)) {
        __WDT_TRACE_ERROR("WDT test context or config invalid!" VSF_TRACE_CFG_LINEEND);
        return false;
    }

    if (log_capability) {
        __wdt_print_capability(capability);
    }

    __wdt_prepare_mode_cfg(capability, cfg, mode, ctx, use_irq);
    ctx->irq_triggered = false;
    ctx->irq_count = 0;
    ctx->test_reset = test_reset;

    if (!__wdt_init_and_enable(ctx, cfg)) {
        return false;
    }

    __wdt_print_config(cfg, capability, mode_str);
    return true;
}

static bool __wdt_test_interrupt(wdt_test_t *ctx)
{
    if (ctx == NULL || ctx->wdt == NULL) {
        __WDT_TRACE_ERROR("WDT test context or device is NULL!" VSF_TRACE_CFG_LINEEND);
        return false;
    }
    vsf_wdt_t *wdt = ctx->wdt;
    vsf_wdt_cfg_t cfg;
    vsf_wdt_capability_t capability;

    __WDT_TRACE_INFO("=== WDT Interrupt Test ===" VSF_TRACE_CFG_LINEEND);

    // 获取 WDT 能力
    capability = vsf_wdt_capability(wdt);

    // 检查是否支持中断模式
    if (!capability.support_reset_none) {
        __WDT_TRACE_ERROR("WDT does not support interrupt mode (RESET_NONE)!" VSF_TRACE_CFG_LINEEND);
        return false;
    }

    // 配置 WDT 为中断模式（不复位）
    if (!__wdt_prepare_test(ctx, &capability, &cfg, VSF_WDT_MODE_RESET_NONE, true, false, "interrupt", true)) {
        return false;
    }
    __WDT_TRACE_INFO("WDT enabled, waiting for interrupt..." VSF_TRACE_CFG_LINEEND);

    // 等待中断触发（最多等待超时时间的指定比率）
    // 注意：如果硬件不支持中断，WDT 会在超时后复位系统
    vsf_systimer_tick_t start_tick = vsf_systimer_get_tick();
    vsf_systimer_tick_t timeout_tick = vsf_systimer_ms_to_tick((cfg.max_ms * WDT_TEST_INTERRUPT_TIMEOUT_RATIO) / 100);

    // 在等待期间定期喂狗，防止系统复位（如果中断没有触发）
    uint32_t feed_interval_ms = __wdt_calc_feed_interval(&cfg, &capability);
    vsf_systimer_tick_t last_feed_tick = start_tick;
    vsf_systimer_tick_t feed_interval_tick = vsf_systimer_ms_to_tick(feed_interval_ms);

    while (!ctx->irq_triggered) {
        vsf_systimer_tick_t current_tick = vsf_systimer_get_tick();

        // 定期喂狗，防止系统复位（如果中断没有触发）
        if (current_tick - last_feed_tick >= feed_interval_tick) {
            vsf_wdt_feed(wdt);
            last_feed_tick = current_tick;
            __WDT_TRACE_INFO("WDT fed to prevent reset..." VSF_TRACE_CFG_LINEEND);
        }

        if (current_tick - start_tick > timeout_tick) {
            __WDT_TRACE_ERROR("WDT interrupt test timeout! Interrupt may not be supported by hardware." VSF_TRACE_CFG_LINEEND);
            __wdt_cleanup(wdt, &capability);
            return false;
        }
        vsf_thread_delay_ms(WDT_TEST_POLL_INTERVAL_MS);
    }

    __WDT_TRACE_INFO("WDT interrupt triggered! Count: %d" VSF_TRACE_CFG_LINEEND, ctx->irq_count);

    // 清理 WDT
    __wdt_cleanup(wdt, &capability);

    __WDT_TRACE_INFO("WDT interrupt test passed!" VSF_TRACE_CFG_LINEEND);
    return true;
}

static bool __wdt_test_reset(wdt_test_t *ctx)
{
    if (ctx == NULL || ctx->wdt == NULL) {
        __WDT_TRACE_ERROR("WDT test context or device is NULL!" VSF_TRACE_CFG_LINEEND);
        return false;
    }
    vsf_wdt_t *wdt = ctx->wdt;
    vsf_wdt_cfg_t cfg;
    vsf_wdt_capability_t capability;

    __WDT_TRACE_INFO("=== WDT Reset Test ===" VSF_TRACE_CFG_LINEEND);
    __WDT_TRACE_INFO("WARNING: This test will cause system reset!" VSF_TRACE_CFG_LINEEND);

    // 获取 WDT 能力
    capability = vsf_wdt_capability(wdt);

    // 配置 WDT 为复位模式
    if (!__wdt_pick_reset_mode(&capability, &cfg.mode)) {
        __WDT_TRACE_ERROR("WDT does not support reset mode!" VSF_TRACE_CFG_LINEEND);
        return false;
    }

    if (!__wdt_prepare_test(ctx, &capability, &cfg, cfg.mode, false, true, "reset", false)) {
        return false;
    }
    if (capability.support_min_timeout && cfg.min_ms > 0) {
        __WDT_TRACE_INFO("System will reset in %d ms if not fed within window [%d, %d] ms..." VSF_TRACE_CFG_LINEEND,
                        cfg.max_ms, cfg.min_ms, cfg.max_ms);
    } else {
        __WDT_TRACE_INFO("System will reset in %d ms if not fed..." VSF_TRACE_CFG_LINEEND, cfg.max_ms);
    }
    __WDT_TRACE_INFO("WDT enabled, system will reset if not fed..." VSF_TRACE_CFG_LINEEND);

    // 不喂狗，等待复位
    // 注意：这里不喂狗，系统会在超时后复位
    // 在实际应用中，应该定期调用 vsf_wdt_feed() 来防止复位

    // 等待一段时间（应该小于超时时间），然后系统会复位
    uint32_t wait_ms = cfg.max_ms + (cfg.max_ms * WDT_TEST_SAFETY_MARGIN_RATIO / 100);
    vsf_thread_delay_ms(wait_ms);

    // 如果执行到这里，说明复位测试失败（可能是硬件不支持或配置错误）
    __WDT_TRACE_ERROR("WDT reset test failed - system did not reset!" VSF_TRACE_CFG_LINEEND);
    __wdt_cleanup(wdt, &capability);
    return false;
}

static bool __wdt_test_feed(wdt_test_t *ctx)
{
    if (ctx == NULL || ctx->wdt == NULL) {
        __WDT_TRACE_ERROR("WDT test context or device is NULL!" VSF_TRACE_CFG_LINEEND);
        return false;
    }
    vsf_wdt_t *wdt = ctx->wdt;
    vsf_wdt_cfg_t cfg;
    vsf_wdt_capability_t capability;
    uint32_t feed_count = 0;

    __WDT_TRACE_INFO("=== WDT Feed Test ===" VSF_TRACE_CFG_LINEEND);

    // 获取 WDT 能力
    capability = vsf_wdt_capability(wdt);

    // 配置 WDT 为复位模式
    if (!__wdt_pick_reset_mode(&capability, &cfg.mode)) {
        __WDT_TRACE_ERROR("WDT does not support reset mode!" VSF_TRACE_CFG_LINEEND);
        return false;
    }
    if (!__wdt_prepare_test(ctx, &capability, &cfg, cfg.mode, false, false, "feed", false)) {
        return false;
    }

    // 计算合适的喂狗间隔
    uint32_t feed_interval_ms = __wdt_calc_feed_interval(&cfg, &capability);

    __wdt_print_config(&cfg, &capability, "feed");
    if (capability.support_min_timeout && cfg.min_ms > 0) {
        __WDT_TRACE_INFO("WDT enabled, feeding every %d ms (must be within window [%d, %d] ms)..." VSF_TRACE_CFG_LINEEND,
                        feed_interval_ms, cfg.min_ms, cfg.max_ms);
    } else {
        __WDT_TRACE_INFO("WDT enabled, feeding every %d ms..." VSF_TRACE_CFG_LINEEND, feed_interval_ms);
    }

    // 定期喂狗，持续一段时间（应该不会复位）
    vsf_systimer_tick_t start_tick = vsf_systimer_get_tick();
    // 测试持续时间：最大超时时间的指定倍数，但至少指定最小值
    uint32_t test_duration_ms = cfg.max_ms * WDT_TEST_FEED_DURATION_MULTIPLIER;
    if (test_duration_ms < WDT_TEST_FEED_DURATION_MIN_MS) {
        test_duration_ms = WDT_TEST_FEED_DURATION_MIN_MS;
    }
    vsf_systimer_tick_t test_duration = vsf_systimer_ms_to_tick(test_duration_ms);

    // 对于 WWDT，需要等待窗口时间（min_ms）后才能第一次喂狗
    // 否则过早喂狗会导致立即复位
    // 每次喂狗后，计数器重置，下次喂狗必须在 [min_ms, max_ms] 窗口内
    if (capability.support_min_timeout && cfg.min_ms > 0) {
        uint32_t wait_time_ms = cfg.min_ms + (cfg.min_ms * WDT_TEST_SAFETY_MARGIN_RATIO / 100);
        __wdt_wait_for_window(wait_time_ms, cfg.min_ms, cfg.max_ms);
        __WDT_TRACE_INFO("Wait completed, starting feed (interval: %d ms)..." VSF_TRACE_CFG_LINEEND, feed_interval_ms);
    }

    while (vsf_systimer_get_tick() - start_tick < test_duration) {
        vsf_wdt_feed(wdt);
        feed_count++;
        __WDT_TRACE_INFO("WDT fed, count: %d" VSF_TRACE_CFG_LINEEND, feed_count);

        // 对于 WWDT，每次喂狗后计数器重置，下次喂狗必须在窗口 [min_ms, max_ms] 内
        // 注意：feed_interval_ms 已经通过 __wdt_calc_feed_interval 确保在窗口内
        if (capability.support_min_timeout && cfg.min_ms > 0) {
            // 直接使用喂狗间隔（已经在窗口 [min_ms, max_ms] 内）
            vsf_thread_delay_ms(feed_interval_ms);
        } else {
            // 普通 WDT，直接使用喂狗间隔
            vsf_thread_delay_ms(feed_interval_ms);
        }
    }

    __WDT_TRACE_INFO("WDT feed test completed, total feeds: %d" VSF_TRACE_CFG_LINEEND, feed_count);

    // 清理 WDT
    __wdt_cleanup(wdt, &capability);

    __WDT_TRACE_INFO("WDT feed test passed!" VSF_TRACE_CFG_LINEEND);
    return true;
}

#if APP_USE_LINUX_DEMO == ENABLED
int wdt_main(int argc, char *argv[])
{
    wdt_test_t ctx = {
        .wdt = (vsf_wdt_t *)&APP_WDT_DEMO_CFG_DEFAULT_INSTANCE,
    };

    if (argc < 2) {
        __WDT_TRACE_INFO("Usage: wdt-test [interrupt|reset|feed]" VSF_TRACE_CFG_LINEEND);
        __WDT_TRACE_INFO("  interrupt: Test WDT interrupt functionality" VSF_TRACE_CFG_LINEEND);
        __WDT_TRACE_INFO("  reset:     Test WDT reset functionality (WARNING: will reset system)" VSF_TRACE_CFG_LINEEND);
        __WDT_TRACE_INFO("  feed:      Test WDT feed functionality" VSF_TRACE_CFG_LINEEND);
        return -1;
    }

    if (strcmp(argv[1], "interrupt") == 0) {
        return __wdt_test_interrupt(&ctx) ? 0 : -1;
    } else if (strcmp(argv[1], "reset") == 0) {
        return __wdt_test_reset(&ctx) ? 0 : -1;
    } else if (strcmp(argv[1], "feed") == 0) {
        return __wdt_test_feed(&ctx) ? 0 : -1;
    } else {
        __WDT_TRACE_ERROR("Unknown test: %s" VSF_TRACE_CFG_LINEEND, argv[1]);
        return -1;
    }

    return 0;
}
#else
int VSF_USER_ENTRY(void)
{
    // WDT demo is only available in Linux demo mode
    // Use wdt_main() function in Linux demo mode instead
    return 0;
}
#endif

#endif

static uint32_t __wdt_clamp_ms(uint32_t value)
{
    if (value > WDT_TEST_TIMEOUT_MS_MAX) {
        value = WDT_TEST_TIMEOUT_MS_MAX;
    }
    if (value < WDT_TEST_MIN_TIME_MS) {
        value = WDT_TEST_MIN_TIME_MS;
    }
    return value;
}

static bool __wdt_pick_reset_mode(const vsf_wdt_capability_t *capability, vsf_wdt_mode_t *mode)
{
    if (capability->support_reset_cpu) {
        *mode = VSF_WDT_MODE_RESET_CPU;
        return true;
    }
    if (capability->support_reset_soc) {
        *mode = VSF_WDT_MODE_RESET_SOC;
        return true;
    }
    return false;
}
