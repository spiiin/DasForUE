# String/name/double DSL — проверка 2026-10-06

| Проверка | Результат |
|---|---|
| TestEditor.ps1 | PASS, 322 checkpoints |
| TestPackaged.ps1, Win64 Development | PASS, 322 checkpoints; BuildCookRun exit 0 |
| TestDSLTextPIE.ps1 | PASS; Automation succeeded=1, failed=0, warnings=0 |
| TestDSLTextAsset.ps1 | PASS, Create 8 + Reopen 7 checkpoints |
| TestDSLFramePIE.ps1 regression | PASS |
| TestDSLPIE.ps1 reload regression | PASS |

Проверены Unicode FString input/result, VM-owned input после изменения host
buffer, результаты после valid reload и context release, broken-reload fallback,
64-битная double precision, FName case/number/None, nominal name/string mismatch
и отказ от embedded NUL на входе. В real PIE проверены новые CDO defaults,
ProcessEvent, world isolation, сохранённые globals, reimport и UE GC. Новый
процесс подтвердил saved text graphs, double/name defaults и parameter identity.

Пример: dsl_text_methods.das. API и ограничения: DasForUE-dsl-text-types.md.
Source: <repo>. Полные журналы: logs/smoke-editor.log,
smoke-packaged.log, dsl-text-pie.log, text-asset-Create.log, text-asset-Reopen.log.
Следующий этап — object references и reflected value structs.
