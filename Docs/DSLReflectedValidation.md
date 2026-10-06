# Generic reflected struct DSL — проверка 2026-10-06

| Проверка | Результат |
|---|---|
| TestEditor.ps1 | PASS, 352 checkpoints |
| TestPackaged.ps1, Win64 Development | PASS, 352 checkpoints; BuildCookRun exit 0 |
| TestDSLReflectedPIE.ps1 | PASS; Automation succeeded=1, failed=0, warnings=0 |
| TestDSLReflectedAsset.ps1 | PASS, Create/Reopen |
| TestDSLMathPIE.ps1 regression | PASS |

Проверены recursive reflection capabilities, immutable deep copies, nested float,
Unicode FString, host/VM independence, wrong result struct identity и results
после context release. Real PIE выполнил typed native struct ProcessEvent через
generic wildcard pack/unpack. Новый процесс подтвердил saved graphs и parameter
identity. ABI token использует managed ref/copy/CMRES pinned daScript, без pointer
на произвольную UE struct memory в script.

Ограничения: scalar/nested native reflected data types, без object/container/enum
fields; snapshot table — до 4096 distinct values на context, dedup одинаковых
values, без reclamation по VM roots. Production long-running value churn пока
требует следующего lifetime этапа. Struct fields/defaults не входят в этот slice.

Пример: dsl_reflected_methods.das. Документация: DasForUE-dsl-reflected-structs.md.
Source: C:/src/DasForUE2. Журналы: logs/smoke-editor.log, smoke-packaged.log,
dsl-reflected-pie.log, reflected-asset-Create.log, reflected-asset-Reopen.log.
