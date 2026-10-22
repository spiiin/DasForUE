# Math value DSL — проверка 2026-10-06

| Проверка | Результат |
|---|---|
| TestEditor.ps1 | PASS, 343 checkpoints |
| TestPackaged.ps1, Win64 Development | PASS, 343 checkpoints; BuildCookRun exit 0 |
| TestDSLMathPIE.ps1 | PASS; Automation succeeded=1, failed=0, warnings=0 |
| TestDSLMathAsset.ps1 | PASS, Create 7 + Reopen 6 checkpoints |
| TestDSLObjectFramePIE.ps1 regression | PASS |

Проверены native FVector/FRotator/FTransform types и CMRES ABI, double precision,
rotation/translation/scale, native TransformPosition, independent VM/host copies,
owned results после context release, wrong struct types, non-finite vector и
unnormalized quaternion rejection. В real PIE выполнен ProcessEvent с exact UE
struct pins, mixed math types, reimport и GC. Новый процесс подтвердил сохранённый
FTransform graph, parameter identity и native ProcessEvent.

Staging испытал кратковременный отказ Zen Server после cook; автоматический
readiness/retry восстановил доступ. Итоговая упаковка и packaged smoke успешны.

Пример: dsl_math_methods.das. Контракт: DasForUE-dsl-math-values.md.
Source: <repo>. Журналы: logs/smoke-editor.log, smoke-packaged.log,
dsl-math-pie.log, math-asset-Create.log, math-asset-Reopen.log.
Следующий этап — общий reflected struct adapter; struct fields отдельно.
