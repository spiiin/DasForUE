# UEStructValue snapshot GC — проверка 2026-10-06

| Проверка | Результат |
|---|---|
| TestEditor.ps1 | PASS, 357 checkpoints |
| TestPackaged.ps1, Win64 Development | PASS, 357 checkpoints; BuildCookRun exit 0 |
| TestDSLReflectedPIE.ps1 regression | PASS |
| TestDSLReflectedAsset.ps1 Save/Reopen regression | PASS |

20,000 immutable updates в первом call и ещё 10,000 во втором прошли pressure
collection с сохранением active locals/args, global anchor и array globals.
После call осталось не более четырёх global values, reclaimed >10,000 snapshots.
5,000 коротких calls без options gc удерживают ровно один anchor; release
освобождает все snapshots. Те же stress tests прошли в packaged runtime.

Документация: DasForUE-dsl-struct-lifetime.md. Пример: struct_snapshot_gc.das.
Root attribution ограничен supported interpreter frames; options gc нужен для
collection внутри active call. Hard cap 4096 действительно живых snapshots
сохранён. Foreign compiled/AOT/JIT attribution остаётся отдельной capability.
