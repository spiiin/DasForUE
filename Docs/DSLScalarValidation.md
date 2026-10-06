# Scalar DSL — проверка 2026-10-06

| Проверка | Результат |
|---|---|
| TestEditor.ps1 | PASS, 301 checkpoints |
| TestPackaged.ps1, Win64 Development | PASS, 301 checkpoints; BuildCookRun exit 0 |
| TestDSLScalarsPIE.ps1 | PASS, 13 logged checkpoints; Automation succeeded=1, failed=0, warnings=0 |
| TestDSLPIE.ps1, existing reload regression | PASS |
| TestDSL.ps1, Create/Reopen regression | PASS |

Runtime: float/bool/void entry points, scalar self reads/writes, bool literal true,
shared owner/file context and VM globals, refusal of wrong call signature,
cross-entry typed reload contracts and last-good fallback.

Настоящий PIE: Blueprint float/bool Input/ReturnValue через ProcessEvent, void
exec graph, bool=true default в созданном Actor/CDO, bool setters, общий Actor,
world isolation, reimport без reinstance, отказ от несовпадающего method signature
и bool default migration до mutation, GC и EndPIE.

Образец: dsl_scalar_methods.das. Границы API: DasForUE-dsl-scalars.md.
Следующий этап — mixed/multiple parameters через общий call-frame adapter,
затем strings/names/objects/structs с capability/ownership checks.
