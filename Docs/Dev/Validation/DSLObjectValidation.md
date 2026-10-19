# Object DSL frames — проверка 2026-10-06

| Проверка | Результат |
|---|---|
| TestEditor.ps1 | PASS, 332 checkpoints |
| TestPackaged.ps1, Win64 Development | PASS, 332 checkpoints; BuildCookRun exit 0 |
| TestDSLObjectFramePIE.ps1 | PASS; Automation succeeded=1, failed=0, warnings=0 |
| TestDSLObjectAsset.ps1 | PASS, Create 7 + Reopen 6 checkpoints |
| TestDSLTextPIE.ps1 regression | PASS |
| TestDSLPIE.ps1 reload regression | PASS |

Проверены object CMRES roundtrip и null, class mismatch до выполнения/на результате,
слабые VM globals и host results, scoped input pin при GC и release после вызова,
expired return. Real PIE подтвердил Actor-class pins, typed ProcessEvent/null,
structural reimport и перенаправление existing weak handle на заменённый Actor,
Destroy/GC и EndPIE. Новый процесс подтвердил saved object graphs, parameter
identity и Actor ProcessEvent. Editor replacement delegate ограждён WITH_EDITOR;
в packaged runtime проходят общие object lifetime/class/ABI проверки.

Пример: dsl_object_methods.das. Контракт: DasForUE-dsl-object-methods.md.
Source: C:/src/DasForUE2. Журналы: logs/smoke-editor.log, smoke-packaged.log,
dsl-object-frame-pie.log, object-asset-Create.log, object-asset-Reopen.log.
Следующий этап — FVector/FRotator/FTransform value signatures; object fields
остаются отдельной задачей с сериализацией и GC policy.
