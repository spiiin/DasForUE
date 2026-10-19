# DSL editor Apply/Reimport — проверка 2026-10-06

| Проверка | Результат |
|---|---|
| TestEditor.ps1 | PASS, 292 checkpoints |
| TestPackaged.ps1, Win64 Development | PASS, 292 checkpoints; BuildCookRun exit 0 |
| TestDSLPIE.ps1 | PASS, 24 logged checkpoints; Unreal Automation succeeded=1, failed=0 |
| TestDSL.ps1 | PASS, Create 20 + Reopen 15 checkpoints |
| TestAuthoring.ps1 (JSON regression) | PASS |

Console Apply/Reimport проверены на настоящем PIE. Body-only reload сохраняет
Actor identity; structural reimport сохраняет Score override, references и
BeginPlay state, добавляет Bonus default и обновляет method body. Editor/PIE
миры изолированы. Broken DSL и неподдержанная смена class default сохраняют
last-good runtime context. Исправление source восстанавливает выполнение.
Import source/class ID переживают asset save/reopen в новом процессе.
Runtime property/declaration guards и fallback дополнительно проверены в packaged.

Команды, ограничения и rollback caveat: DasForUE-dsl-reimport.md.
Source: C:/src/DasForUE2; журналы: logs/dsl-pie.log, dsl-Create.log,
dsl-Reopen.log, smoke-editor.log, smoke-packaged.log и authoring-*.log.
Automatic watcher/UI и general signatures/types остаются дальнейшими этапами.
