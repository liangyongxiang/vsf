#!/usr/bin/env python3
"""Generate vsf_test_suite_registry.h from YAML test parameters.

Usage:
    python gen_suite_registry.py <input.yml> <output.h>

Reads test_params.yml (and its includes) and generates the
vsf_test_suite_registry.h file.

Generated regions:
  1. Aggregated data types (all_cases_t, all_params_t)
  2. Aggregated static data (__all_suites[], __all_cases, __all_params)
  3. Peripheral instances
  4. Flat vsf_test_t __vsf_test (no wrapper struct, no entries array)

Hand-written sections (pinmux callbacks, peripheral instances)
are preserved as template constants in this script.
"""

import argparse
import sys
from pathlib import Path

try:
    import yaml
except ImportError as e:
    print("Error: PyYAML is required.", file=sys.stderr)
    raise SystemExit(1) from e

# Add vsf_bench to path for test_params import
_SCRIPT_DIR = Path(__file__).parent
sys.path.insert(0, str(_SCRIPT_DIR))
from vsf_bench.test_params import load_yaml_with_includes


# ---------------------------------------------------------------------------
# Configuration
# ---------------------------------------------------------------------------

PERIPHERAL_ORDER = [
    "gpio", "usart", "i2c", "spi", "rng",
    "adc", "pwm", "dma", "timer", "rtc", "flash", "wdt",
]

# Map peripheral -> (type macro, arg value for single-handle suites)
PERIPHERAL_MAP = {
    "gpio":  ("VSF_PERIPHERAL_TYPE_GPIO",  "VSF_BOARD_GPIO_INSTANCE"),
    "usart": ("VSF_PERIPHERAL_TYPE_USART", "VSF_BOARD_USART_INSTANCE"),
    "i2c":   ("VSF_PERIPHERAL_TYPE_I2C",   "VSF_BOARD_I2C0_INSTANCE"),
    "spi":   ("VSF_PERIPHERAL_TYPE_SPI",   "VSF_BOARD_SPI_INSTANCE"),
    "rng":   ("VSF_PERIPHERAL_TYPE_RNG",   "VSF_BOARD_RNG_INSTANCE"),
    "adc":   ("VSF_PERIPHERAL_TYPE_ADC",   "VSF_BOARD_ADC_INSTANCE"),
    "pwm":   ("VSF_PERIPHERAL_TYPE_PWM",   "VSF_BOARD_PWM_INSTANCE"),
    "dma":   ("VSF_PERIPHERAL_TYPE_DMA",   "VSF_BOARD_DMA_INSTANCE"),
    "timer": ("VSF_PERIPHERAL_TYPE_TIMER", "VSF_BOARD_TIMER_INSTANCE"),
    "rtc":   ("VSF_PERIPHERAL_TYPE_RTC",   "VSF_BOARD_RTC_INSTANCE"),
    "flash": ("VSF_PERIPHERAL_TYPE_FLASH", "VSF_BOARD_FLASH_INSTANCE"),
    "wdt":   ("VSF_PERIPHERAL_TYPE_WDT",   "VSF_BOARD_WDT_INSTANCE"),
}

# Multi-handle scenarios: arg points to a handles array
MULTI_HANDLE_ARGS = {
    "gpio_pinmux":     "__gpio_pinmux_handles",
    "i2c_slave":       "__i2c_slave_handles",
    "i2c_slave_fifo":  "__i2c_slave_handles",
}

I2C_SLAVE_TYPE = "VSF_PERIPHERAL_TYPE_I2C_SLAVE"

# Peripherals whose variable/suite names include an instance suffix
PERI_WITH_INSTANCE_SUFFIX = {"adc", "pwm", "dma", "timer", "rtc", "flash", "wdt", "rng", "spi"}


# ---------------------------------------------------------------------------
# Hand-written template sections
# ---------------------------------------------------------------------------

_HEADER = """\
/******************************************************************************
 *   Copyright(C)2009-2024 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.*
 *  See the License for the specific language governing permissions and       *
 *  limitations under the License.                                           *
 *                                                                           *
 *****************************************************************************/

#pragma once

#include "vsf.h"
#include "vsf_board.h"
#include "component/test/vsf_test/vsf_test.h"
#include "vsf_test_suites.h"

/*============================ LOCAL FUNCTIONS ===============================*/

#define DEFINE_PINMUX_CB(name) \\
    static bool __setup_##name(vsf_test_suite_t *s) { \\
        (void)s; vsf_board_pinmux_##name##_init(); return true; \\
    } \\
    static void __teardown_##name(vsf_test_suite_t *s) { \\
        (void)s; vsf_board_pinmux_##name##_fini(); \\
    }

DEFINE_PINMUX_CB(usart1)
DEFINE_PINMUX_CB(i2c0)
DEFINE_PINMUX_CB(i2c1)

static bool __setup_i2c_slave(vsf_test_suite_t *s)
{
    (void)s;
    vsf_board_pinmux_i2c0_init();
    vsf_board_pinmux_i2c1_init();
    return true;
}
static void __teardown_i2c_slave(vsf_test_suite_t *s)
{
    (void)s;
    vsf_board_pinmux_i2c0_fini();
    vsf_board_pinmux_i2c1_fini();
}

/*============================ MULTI-HANDLE ARRAYS ===========================*/

static void *__gpio_pinmux_handles[] = {
    VSF_BOARD_GPIO_INSTANCE, VSF_BOARD_PINMUX_USART_INSTANCE,
};

static void *__i2c_slave_handles[] = {
    VSF_BOARD_I2C0_INSTANCE, VSF_BOARD_I2C1_INSTANCE,
};
"""

_INSTANCES = """\
/*============================ PERIPHERAL INSTANCES ============================*/

#define VSF_TEST_INST(name, ptype, handle, setup_fn, teardown_fn) \\
    static vsf_test_inst_t __inst_##name = {                        \\
        .peripheral_type = ptype,                                   \\
        .hal_handle      = handle,                                  \\
        .setup           = setup_fn,                                \\
        .teardown        = teardown_fn,                             \\
    }

#define VSF_TEST_INST_ENTRIES \\
    VSF_TEST_INST_ENTRY(i2c0,      VSF_PERIPHERAL_TYPE_I2C,       VSF_BOARD_I2C0_INSTANCE,     __setup_i2c0,      __teardown_i2c0)      \\
    VSF_TEST_INST_ENTRY(i2c1,      VSF_PERIPHERAL_TYPE_I2C,       VSF_BOARD_I2C1_INSTANCE,     __setup_i2c1,      __teardown_i2c1)      \\
    VSF_TEST_INST_ENTRY(i2c_slave, VSF_PERIPHERAL_TYPE_I2C_SLAVE, NULL,                        __setup_i2c_slave, __teardown_i2c_slave) \\
    VSF_TEST_INST_ENTRY(usart1,    VSF_PERIPHERAL_TYPE_USART,     VSF_BOARD_USART_INSTANCE,    __setup_usart1,    __teardown_usart1)    \\
    VSF_TEST_INST_ENTRY(spi0,      VSF_PERIPHERAL_TYPE_SPI,       VSF_BOARD_SPI_INSTANCE,      NULL,              NULL)                 \\
    VSF_TEST_INST_ENTRY(adc0,      VSF_PERIPHERAL_TYPE_ADC,       VSF_BOARD_ADC_INSTANCE,      NULL,              NULL)                 \\
    VSF_TEST_INST_ENTRY(pwm0,      VSF_PERIPHERAL_TYPE_PWM,       VSF_BOARD_PWM_INSTANCE,      NULL,              NULL)                 \\
    VSF_TEST_INST_ENTRY(dma0,      VSF_PERIPHERAL_TYPE_DMA,       VSF_BOARD_DMA_INSTANCE,      NULL,              NULL)                 \\
    VSF_TEST_INST_ENTRY(timer0,    VSF_PERIPHERAL_TYPE_TIMER,     VSF_BOARD_TIMER_INSTANCE,    NULL,              NULL)                 \\
    VSF_TEST_INST_ENTRY(rtc0,      VSF_PERIPHERAL_TYPE_RTC,       VSF_BOARD_RTC_INSTANCE,      NULL,              NULL)                 \\
    VSF_TEST_INST_ENTRY(flash0,    VSF_PERIPHERAL_TYPE_FLASH,     VSF_BOARD_FLASH_INSTANCE,    NULL,              NULL)                 \\
    VSF_TEST_INST_ENTRY(wdt0,      VSF_PERIPHERAL_TYPE_WDT,       VSF_BOARD_WDT_INSTANCE,      NULL,              NULL)                 \\
    VSF_TEST_INST_ENTRY(rng0,      VSF_PERIPHERAL_TYPE_RNG,       VSF_BOARD_RNG_INSTANCE,      NULL,              NULL)                 \\
    VSF_TEST_INST_ENTRY(gpio0,     VSF_PERIPHERAL_TYPE_GPIO,      VSF_BOARD_GPIO_INSTANCE,     NULL,              NULL)

#define VSF_TEST_INST_ENTRY(name, ptype, handle, setup_fn, teardown_fn) \\
    VSF_TEST_INST(name, ptype, handle, setup_fn, teardown_fn);
VSF_TEST_INST_ENTRIES
#undef VSF_TEST_INST_ENTRY

static vsf_test_inst_t *__vsf_test_instances[] = {
#define VSF_TEST_INST_ENTRY(name, ptype, handle, setup_fn, teardown_fn) \\
    &__inst_##name,
VSF_TEST_INST_ENTRIES
#undef VSF_TEST_INST_ENTRY
};
"""

_REGISTRY = """\
/*============================ REGISTRY ========================================*/

static vsf_test_reboot_t *__vsf_test_reboot_entries[] = {
    vsf_arch_reset,
};

static vsf_test_t __vsf_test = {
    .wdt = { .entries = NULL, .count = 0 },
    .reboot = { .entries = __vsf_test_reboot_entries,
                .count   = dimof(__vsf_test_reboot_entries) },
    .suites      = __all_suites,
    .suite_count = dimof(__all_suites),
    .instances   = __vsf_test_instances,
    .instance_count = dimof(__vsf_test_instances),
};
"""


# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------

def get_peripheral(name: str) -> str:
    """Extract peripheral prefix: 'gpio_output_input' -> 'gpio'."""
    return name.split("_")[0]


def get_type_suffix(name: str) -> str:
    """Extract type-name suffix from scenario name."""
    if name == "usart_tx_baud":
        return "baud"
    if name == "usart_tx_mode":
        return "mode"
    peripheral = get_peripheral(name)
    return name[len(peripheral) + 1:]  # after "peripheral_"


def _instance_suffix(peripheral: str) -> str:
    """Return '0' for peripherals that use instance suffix, empty otherwise."""
    return "0" if peripheral in PERI_WITH_INSTANCE_SUFFIX else ""


def get_suite_name_str(name: str) -> str:
    """Generate suite name string."""
    peripheral = get_peripheral(name)
    suffix = get_type_suffix(name)
    inst = _instance_suffix(peripheral)
    return f"{peripheral}{inst}_{suffix}"


def is_rx_scenario(name: str) -> bool:
    """RX scenarios need ready handshake (true), TX scenarios don't (false)."""
    return "_rx_" in name


def get_enable_macro(name: str) -> str:
    return f"VSF_TEST_{name.upper()}_ENABLE"


def get_count_macro(name: str) -> str:
    return f"VSF_TEST_{name.upper()}_CASE_COUNT"


def get_case_data_macro(name: str) -> str:
    return f"VSF_TEST_{name.upper()}_PARAMS_INIT"


def get_cases_macro(name: str) -> str:
    return f"VSF_TEST_{name.upper()}_CASES"


def get_peripheral_type(name: str) -> str:
    if name.startswith("i2c_slave"):
        return I2C_SLAVE_TYPE
    peripheral = get_peripheral(name)
    return PERIPHERAL_MAP[peripheral][0]


def get_arg_value(name: str) -> str:
    """Generate .arg value for a scenario."""
    if name in MULTI_HANDLE_ARGS:
        return MULTI_HANDLE_ARGS[name]
    peripheral = get_peripheral(name)
    return PERIPHERAL_MAP[peripheral][1]


def _get_field_name(name: str) -> str:
    """Field name in the aggregated struct: e.g. 'gpio_analog_mode'."""
    return get_suite_name_str(name)


# ---------------------------------------------------------------------------
# Generators
# ---------------------------------------------------------------------------

def generate_all_types(scenarios: list[tuple[str, dict]]) -> str:
    """Generate aggregated case and param struct types."""
    cases_lines = ["typedef struct {"]
    params_lines = ["typedef struct {"]

    for _scenario_key, sc in scenarios:
        name = sc["name"]
        enable = get_enable_macro(name)
        peripheral = get_peripheral(name)
        suffix = get_type_suffix(name)
        count = get_count_macro(name)
        field = _get_field_name(name)

        cases_lines.append(f"#if {enable} == ENABLED")
        cases_lines.append(f"    vsf_test_case_t {field}[{count}];")
        cases_lines.append(f"#endif")

        params_lines.append(f"#if {enable} == ENABLED")
        params_lines.append(f"    vsf_test_{peripheral}_{suffix}_params_t {field}[{count}];")
        params_lines.append(f"#endif")

    cases_lines.append("} vsf_test_all_cases_t;")
    params_lines.append("} vsf_test_all_params_t;")

    lines = [
        "/*============================ AGGREGATED DATA TYPES ========================*/",
        "",
        *cases_lines,
        "",
        *params_lines,
    ]
    return "\n".join(lines)


def generate_all_data(scenarios: list[tuple[str, dict]]) -> str:
    """Generate __all_suites[], __all_cases, __all_params."""
    suites_lines = ["static vsf_test_suite_t __all_suites[] = {"]
    cases_lines = ["static vsf_test_all_cases_t __all_cases = {"]
    params_lines = ["static const vsf_test_all_params_t __all_params = {"]

    for _scenario_key, sc in scenarios:
        name = sc["name"]
        enable = get_enable_macro(name)
        count = get_count_macro(name)
        ptype = get_peripheral_type(name)
        arg = get_arg_value(name)
        case_data = get_case_data_macro(name)
        cases = get_cases_macro(name)
        run_fn = f"vsf_test_{get_peripheral(name)}_{get_type_suffix(name)}_run"
        ready = "true" if is_rx_scenario(name) else "false"
        suite_str = get_suite_name_str(name)
        field = _get_field_name(name)

        suites_lines.append(f"#if {enable} == ENABLED")
        suites_lines.append(f"    {{")
        suites_lines.append(f"        .name       = \"{suite_str}\",")
        suites_lines.append(f"        .cases      = __all_cases.{field},")
        suites_lines.append(f"        .case_count = {count},")
        suites_lines.append(f"        .peripheral_type = {ptype},")
        suites_lines.append(f"        .arg        = {arg},")
        suites_lines.append(f"    }},")
        suites_lines.append(f"#endif")

        cases_lines.append(f"#if {enable} == ENABLED")
        cases_lines.append(f"    .{field} = {{ {cases}(__all_params.{field}, {run_fn}, {ready}) }},")
        cases_lines.append(f"#endif")

        params_lines.append(f"#if {enable} == ENABLED")
        params_lines.append(f"    .{field} = {{ {case_data} }},")
        params_lines.append(f"#endif")

    suites_lines.append("};")
    cases_lines.append("};")
    params_lines.append("};")

    lines = [
        "/*============================ STATIC DATA ==================================*/",
        "",
        "static vsf_test_all_cases_t __all_cases;",
        "static const vsf_test_all_params_t __all_params;",
        "",
        *suites_lines,
        "",
        *cases_lines,
        "",
        *params_lines,
    ]
    return "\n".join(lines)


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def load_scenarios(yml_path: Path, project_root: Path) -> list[tuple[str, dict]]:
    global_base = project_root / "vsf.demo" / "vsf" / "test" / "vsf_test" / "params"
    params = load_yaml_with_includes(yml_path, global_base=global_base)

    scenarios = []
    for key, value in params.items():
        if key == "marker" or not isinstance(value, dict):
            continue
        name = value.get("name")
        if not name:
            continue
        scenarios.append((key, value))

    def sort_key(item):
        key, sc = item
        name = sc.get("name", key)
        peripheral = get_peripheral(name)
        try:
            peri_idx = PERIPHERAL_ORDER.index(peripheral)
        except ValueError:
            peri_idx = len(PERIPHERAL_ORDER)
        return (peri_idx, key)

    scenarios.sort(key=sort_key)
    return scenarios


def generate_registry(yml_path: Path, project_root: Path) -> str:
    scenarios = load_scenarios(yml_path, project_root)

    lines = []

    # Header (hand-written)
    lines.append(_HEADER)
    lines.append("")

    # Region 1: Aggregated data types
    lines.append(generate_all_types(scenarios))
    lines.append("")

    # Region 2: Aggregated static data
    lines.append(generate_all_data(scenarios))
    lines.append("")

    # Hand-written: peripheral instances
    lines.append(_INSTANCES)
    lines.append("")

    # Hand-written: flat registry (no wrapper struct, no entries array)
    lines.append(_REGISTRY)
    lines.append("")

    return "\n".join(lines)


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Generate vsf_test_suite_registry.h from test params YAML"
    )
    parser.add_argument("yml", type=Path, help="Input YAML file")
    parser.add_argument("out", type=Path, help="Output C header file")
    parser.add_argument("--project-root", type=Path, default=Path.cwd(),
                        help="Project root directory (default: cwd)")
    args = parser.parse_args()

    if not args.yml.exists():
        print(f"Error: {args.yml} not found", file=sys.stderr)
        return 1

    content = generate_registry(args.yml, project_root=args.project_root)
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(content)
    print(f"Generated: {args.out}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
