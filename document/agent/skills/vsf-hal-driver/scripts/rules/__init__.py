#!/usr/bin/env python3
"""Registry for all VSF HAL driver quality rule modules."""

from pathlib import Path
from typing import Callable

from checker_base import Finding

from rules.pattern_rules import (
    rule_hardcoded_address,
    rule_macro_backslash_align,
    rule_spin_wait_comment,
    rule_chip_prefixed_define,
)
from rules.nvic_priority import check_nvic_priority_order
from rules.init_clock_reset import check_init_has_clock, check_init_has_reset
from rules.fini_nvic import check_fini_nvic_order
from rules.irq_disable import check_irq_disable_nvic_leak
from rules.null_isr import check_init_null_isr
from rules.freq_default import check_silent_freq_default
from rules.mode_bits import check_mode_bits_translation

ALL_PATTERN_RULES: list[Callable[[list, Path], list[Finding]]] = [
    rule_hardcoded_address,
    rule_macro_backslash_align,
    rule_spin_wait_comment,
    rule_chip_prefixed_define,
]

ALL_FUNC_RULES: list[Callable[[list[dict], Path], tuple[list[Finding], list[Finding]]]] = [
    check_nvic_priority_order,
    check_init_has_reset,
    check_init_has_clock,
    check_fini_nvic_order,
    check_irq_disable_nvic_leak,
    check_init_null_isr,
    check_silent_freq_default,
    check_mode_bits_translation,
]
