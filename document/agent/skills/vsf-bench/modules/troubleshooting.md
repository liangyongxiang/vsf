# Troubleshooting

| Symptom | Likely cause | Action |
|---------|-------------|--------|
| Build fails | cmake or SDK not installed, wrong `build.source_dir` in hardware-map.yml | verify build.source_dir path; check cmake/SDK installation |
| Flash fails | board not connected, wrong runner selected | check USB cable; verify `flash.runner` in hardware-map.yml (swd vs uf2) |
| Test timeout | firmware not outputting expected pattern, baud rate mismatch | verify serial.port and baudrate in hardware-map.yml match firmware config |
| `Suite not found` | suite disabled in firmware config (`vsf_usr_cfg.h`), or CLI `--suite` name doesn't match firmware registration | check `VSF_USE_TEST_<SUITE>` is ENABLED; verify suite name spelling against scene list in firmware (`vsf-test scene --list`) |
| No scenes listed (`vsf-test scene --list` empty) | test framework not enabled, or no suites compiled in | verify `VSF_USE_TEST = ENABLED` in `vsf_usr_cfg.h`; check suite config flags |
| LA (logic analyzer) errors | channel names in hardware-map.yml don't match LA wiring | verify `logic_analyzer.channels` matches physical DSLogic connections |
| Script fails with `suite not found` | script registered under wrong scene name | check script filename matches YAML `script:` field in test params |
