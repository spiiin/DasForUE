# Mixed frame DSL — проверка 2026-10-06

| Проверка | Результат |
|---|---|
| TestEditor.ps1 | PASS, 311 checkpoints |
| TestPackaged.ps1, Win64 Development | PASS, 311 checkpoints; BuildCookRun exit 0 |
| TestDSLFramePIE.ps1 | PASS, 12 logged checkpoints; Automation succeeded=1, failed=0, warnings=0 |
| TestDSLFrameAsset.ps1 | PASS, Create 7 + Reopen 6 checkpoints |
| TestDSLScalarsPIE.ps1 | PASS |
| TestDSLPIE.ps1, existing reload regression | PASS |
| TestDSL.ps1, existing Create/Reopen regression | PASS |

Проверены смешанные и ordered inputs, разные return types, zero-input result,
void с параметрами, invalid tags/arity/order/limit, восстановление после неверного
первого запроса, cross-entry reload contract с сохранением прежних VM globals.
В реальном PIE выполнены generated frame graphs через ProcessEvent, проверены
world isolation, reimport без reinstance, отказ от parameter reorder/ID change,
GC и EndPIE. Графы и parameter IDs сохранены в asset; новый процесс подтвердил
mixed ProcessEvent и idempotent reimport после LoadObject.

Пример без explicit IDs: dsl_mixed_methods.das. Границы: DasForUE-dsl-mixed-methods.md.
Source: C:/src/DasForUE2. Журналы: logs/smoke-editor.log, smoke-packaged.log,
dsl-frame-pie.log, frame-asset-Create.log, frame-asset-Reopen.log.
Следующий этап — string/name/double и затем object/struct signatures.
