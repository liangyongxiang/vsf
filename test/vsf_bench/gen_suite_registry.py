#!/usr/bin/env python3
"""Generate vsf_test_suite_registry.h from YAML test parameters.

Usage:
    python gen_suite_registry.py <input.yml> <output.h>

Reads test_params.yml (and its includes) and generates the
vsf_test_suite_registry.h file with zero local macros.

Generated regions:
  1. Table type declarations (typedef struct { ... } table_t)
  2. Static suite definitions
  3. Suite entries array

Hand-written sections (pinmux callbacks, peripheral instances, registry struct)
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

PERIPHERAL_MAP = {
    "gpio":  {"type": "VSF_PERIPHERAL_TYPE_GPIO",  "field": ".gpio = VSF_BOARD_GPIO_INSTANCE"},
    "usart": {"type": "VSF_PERIPHERAL_TYPE_USART", "field": ".usart = VSF_BOARD_USART_INSTANCE"},
    "i2c":   {"type": "VSF_PERIPHERAL_TYPE_I2C",   "field": ".i2c = VSF_BOARD_I2C0_INSTANCE"},
    "spi":   {"type": "VSF_PERIPHERAL_TYPE_SPI",   "field": ".spi = VSF_BOARD_SPI_INSTANCE"},
    "rng":   {"type": "VSF_PERIPHERAL_TYPE_RNG",   "field": ".rng = VSF_BOARD_RNG_INSTANCE"},
    "adc":   {"type": "VSF_PERIPHERAL_TYPE_ADC",   "field": ".adc = VSF_BOARD_ADC_INSTANCE"},
    "pwm":   {"type": "VSF_PERIPHERAL_TYPE_PWM",   "field": ".pwm = VSF_BOARD_PWM_INSTANCE"},
    "dma":   {"type": "VSF_PERIPHERAL_TYPE_DMA",   "field": ".dma = VSF_BOARD_DMA_INSTANCE"},
    "timer": {"type": "VSF_PERIPHERAL_TYPE_TIMER", "field": ".timer = VSF_BOARD_TIMER_INSTANCE"},
    "rtc":   {"type": "VSF_PERIPHERAL_TYPE_RTC",   "field": ".rtc = VSF_BOARD_RTC_INSTANCE"},
    "flash": {"type": "VSF_PERIPHERAL_TYPE_FLASH", "field": ".flash = VSF_BOARD_FLASH_INSTANCE"},
    "wdt":   {"type": "VSF_PERIPHERAL_TYPE_WDT",   "field": ".wdt = VSF_BOARD_WDT_INSTANCE"},
}

# Scenarios that need special instance field binding
OVERRIDE_FIELDS = {
    "gpio_pinmux": ".gpio = VSF_BOARD_GPIO_INSTANCE, .usart = VSF_BOARD_PINMUX_USART_INSTANCE",
}

# i2c_slave: special peripheral type and instance fields
I2C_SLAVE_TYPE = "VSF_PERIPHERAL_TYPE_I2C_SLAVE"
I2C_SLAVE_FIELD = ".master_i2c = VSF_BOARD_I2C0_INSTANCE, .slave_i2c = VSF_BOARD_I2C1_INSTANCE"

# Peripherals whose variable/suite names include an instance suffix (e.g., __adc0_oneshot, "adc0_oneshot")
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
 *  You may obtain a copy of the License at                                  *
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

typedef struct vsf_test_registry_t {
    vsf_test_reboot_t  **reboot_entries;
    uint8_t              reboot_count;
    vsf_test_suite_t   **suite_entries;
    uint8_t              suite_count;
    vsf_test_inst_t    **inst_entries;
    uint8_t              inst_count;
    vsf_test_t           test;
} vsf_test_registry_t;

static vsf_test_reboot_t *__vsf_test_reboot_entries[] = {
    vsf_arch_reset,
};

static vsf_test_registry_t __vsf_test_registry = {
    .reboot_entries = __vsf_test_reboot_entries,
    .reboot_count   = dimof(__vsf_test_reboot_entries),
    .suite_entries  = __vsf_test_suite_entries,
    .suite_count    = dimof(__vsf_test_suite_entries),
    .inst_entries   = __vsf_test_instances,
    .inst_count     = dimof(__vsf_test_instances),
    .test = {
        .suites         = __vsf_test_suite_entries,
        .suite_count    = dimof(__vsf_test_suite_entries),
        .instances      = __vsf_test_instances,
        .instance_count = dimof(__vsf_test_instances),
        .wdt            = { .entries = NULL, .count = 0 },
        .reboot         = { .entries = __vsf_test_reboot_entries,
                            .count   = dimof(__vsf_test_reboot_entries) },
    },
};
"""


# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------

def get_peripheral(name: str) -> str:
    """Extract peripheral prefix: 'gpio_output_input' -> 'gpio'."""
    return name.split("_")[0]


def get_type_suffix(name: str) -> str:
    """Extract type-name suffix from scenario name.

    Only usart_tx_baud and usart_tx_mode strip the 'tx_' prefix to match
    the existing type names in vsf_test_usart.h
    (e.g., usart_tx_baud -> baud, matching vsf_test_usart_baud_suite_t).
    All other scenarios keep their full suffix.
    """
    if name == "usart_tx_baud":
        return "baud"
    if name == "usart_tx_mode":
        return "mode"
    peripheral = get_peripheral(name)
    return name[len(peripheral) + 1:]  # after "peripheral_"


def _instance_suffix(peripheral: str) -> str:
    """Return '0' for peripherals that use instance suffix, empty otherwise."""
    return "0" if peripheral in PERI_WITH_INSTANCE_SUFFIX else ""


def get_var_name(name: str) -> str:
    """Generate variable name for the suite table.

    Examples: __gpio_output_input, __usart_baud, __adc0_oneshot, __spi0_loopback
    """
    peripheral = get_peripheral(name)
    suffix = get_type_suffix(name)
    inst = _instance_suffix(peripheral)
    return f"__{peripheral}{inst}_{suffix}"


def get_suite_name_str(name: str) -> str:
    """Generate suite name string.

    Examples: "gpio_output_input", "usart_baud", "adc0_oneshot", "spi0_loopback"
    """
    peripheral = get_peripheral(name)
    suffix = get_type_suffix(name)
    inst = _instance_suffix(peripheral)
    return f"{peripheral}{inst}_{suffix}"


def is_rx_scenario(name: str) -> bool:
    """RX scenarios need ready handshake (true), TX scenarios don't (false)."""
    # usart_rx_data, usart_rx_baud, usart_request_rx_irq, etc.
    return "_rx_" in name


def get_enable_macro(name: str) -> str:
    """VSF_TEST_{NAME_UPPER}_ENABLE."""
    return f"VSF_TEST_{name.upper()}_ENABLE"


def get_count_macro(name: str) -> str:
    """VSF_TEST_{NAME_UPPER}_CASE_COUNT."""
    return f"VSF_TEST_{name.upper()}_CASE_COUNT"


def get_case_data_macro(name: str) -> str:
    """VSF_TEST_{NAME_UPPER}_CASE_DATA."""
    return f"VSF_TEST_{name.upper()}_CASE_DATA"


def get_cases_macro(name: str) -> str:
    """VSF_TEST_{NAME_UPPER}_CASES."""
    return f"VSF_TEST_{name.upper()}_CASES"


def get_peripheral_type(name: str) -> str:
    """Get VSF_PERIPHERAL_TYPE_* for a scenario."""
    if name.startswith("i2c_slave"):
        return I2C_SLAVE_TYPE
    peripheral = get_peripheral(name)
    return PERIPHERAL_MAP[peripheral]["type"]


def get_instance_field(name: str) -> str:
    """Get instance field binding for a scenario."""
    if name in OVERRIDE_FIELDS:
        return OVERRIDE_FIELDS[name]
    if name.startswith("i2c_slave"):
        return I2C_SLAVE_FIELD
    peripheral = get_peripheral(name)
    return PERIPHERAL_MAP[peripheral]["field"]


# ---------------------------------------------------------------------------
# Generators
# ---------------------------------------------------------------------------

def generate_table_type_decl(name: str) -> str:
    """Generate typedef struct { ... } table_t; for one scenario.

    Wrapped in #if ENABLE == ENABLED because the underlying suite_t/case_t
    types in vsf_test_*.h may be conditionally compiled.
    """
    enable = get_enable_macro(name)
    peripheral = get_peripheral(name)
    suffix = get_type_suffix(name)
    count = get_count_macro(name)

    lines = [
        f"#if {enable} == ENABLED",
        f"typedef struct {{",
        f"    vsf_test_{peripheral}_{suffix}_suite_t suite;",
        f"    vsf_test_{peripheral}_{suffix}_case_t data[{count}];",
        f"    vsf_test_case_t cases[{count}];",
        f"}} vsf_test_{peripheral}_{suffix}_table_t;",
        f"#endif",
    ]
    return "\n".join(lines)


def generate_static_def(name: str) -> str:
    """Generate static suite definition for one scenario."""
    enable = get_enable_macro(name)
    peripheral = get_peripheral(name)
    suffix = get_type_suffix(name)
    var = get_var_name(name)
    suite_str = get_suite_name_str(name)
    table_t = f"vsf_test_{peripheral}_{suffix}_table_t"
    count = get_count_macro(name)
    ptype = get_peripheral_type(name)
    field = get_instance_field(name)
    case_data = get_case_data_macro(name)
    cases = get_cases_macro(name)
    run_fn = f"vsf_test_{peripheral}_{suffix}_run"
    ready = "true" if is_rx_scenario(name) else "false"

    lines = [
        f"#if {enable} == ENABLED",
        f"static {table_t} {var} = {{",
        f"    .suite = {{",
        f"        .name       = \"{suite_str}\",",
        f"        .cases      = {var}.cases,",
        f"        .case_count = {count},",
        f"        .peripheral_type = {ptype},",
        f"        {field},",
        f"    }},",
        f"    .data  = {{ {case_data}(&{var}.suite) }},",
        f"    .cases = {{ {cases}({var}.data, {run_fn}, {ready}) }},",
        f"}};",
        f"#endif",
    ]
    return "\n".join(lines)


def generate_entry(name: str) -> str:
    """Generate one entry in __vsf_test_suite_entries[]."""
    enable = get_enable_macro(name)
    var = get_var_name(name)

    lines = [
        f"#if {enable} == ENABLED",
        f"    &{var}.suite.use_as__vsf_test_suite_t,",
        f"#endif",
    ]
    return "\n".join(lines)


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def load_scenarios(yml_path: Path, project_root: Path) -> list[tuple[str, dict]]:
    """Load YAML and return list of (scenario_key, scenario_dict) tuples.

    Scenarios are sorted by peripheral order, then alphabetically within
    each peripheral group.
    """
    global_base = project_root / "vsf.demo" / "vsf" / "test" / "vsf_test" / "params"
    params = load_yaml_with_includes(yml_path, global_base=global_base)

    # Extract scenario entries
    scenarios = []
    for key, value in params.items():
        if key == "marker" or not isinstance(value, dict):
            continue
        name = value.get("name")
        if not name:
            continue
        scenarios.append((key, value))

    # Sort by peripheral order, then by key
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
    """Generate the full vsf_test_suite_registry.h content."""
    scenarios = load_scenarios(yml_path, project_root)

    lines = []

    # Header (hand-written)
    lines.append(_HEADER)
    lines.append("")

    # Region 1: Table type declarations
    lines.append("/*============================ TABLE TYPE DECLARATIONS =======================*/")
    lines.append("")
    for _scenario_key, sc in scenarios:
        name = sc["name"]
        lines.append(generate_table_type_decl(name))
        lines.append("")

    # Region 2: Static suite definitions
    lines.append("/*============================ STATIC SUITE DEFINITIONS =====================*/")
    lines.append("")
    for _scenario_key, sc in scenarios:
        name = sc["name"]
        lines.append(generate_static_def(name))
        lines.append("")

    # Hand-written: peripheral instances
    lines.append(_INSTANCES)
    lines.append("")

    # Region 3: Suite entries array
    lines.append("static vsf_test_suite_t *__vsf_test_suite_entries[] = {")
    for _scenario_key, sc in scenarios:
        name = sc["name"]
        lines.append(generate_entry(name))
    lines.append("};")
    lines.append("")

    # Hand-written: registry struct
    lines.append(_REGISTRY)
    lines.append("")

    return "\n".join(lines)


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Generate vsf_test_suite_registry.h from test params YAML"
    )
    parser.add_argument("yml", type=Path, help="Input YAML file (e.g., application/component/vsf-test/test_params.yml)")
    parser.add_argument("out", type=Path, help="Output C header file")
    parser.add_argument("--project-root", type=Path, default=Path.cwd(),
                        help="Project root directory (default: cwd)")
    args = parser.parse_args()

    if not args.yml.exists():
        print(f"Error: {args.yml} not found", file=sys.stderr)
        return 1

    content = generate_registry(args.yml, args.project_root)
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(content)
    print(f"Generated: {args.out}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
