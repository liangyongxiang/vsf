# Legacy Data-Sync Port Files

These files implement the `vsf_test_data_t` assist-device protocol for
host-less (standalone) test execution. They are **not compiled or maintained**
in the current configuration, which uses the shell REPL as the sole host
interaction path.

Preserved for reference: ports that genuinely need standalone operation
(without a PC host driving tests via `vsf-test` shell commands) can use
these as a starting point.

Files:
- `vsf_test_port_stdio.c/.h` — stdio-based data sync
- `vsf_test_port_file.c/.h` — file-based data sync
- `vsf_test_port_appcfg.c/.h` — appcfg-based data sync
