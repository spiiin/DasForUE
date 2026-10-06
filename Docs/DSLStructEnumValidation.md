# Enum leaves DSL — проверка 2026-10-06

| Проверка | Результат |
|---|---|
| TestEditor.ps1 | PASS, 367 checkpoints |
| TestPackaged.ps1, Win64 Development | PASS, 367 checkpoints; BuildCookRun exit 0 |
| TestDSLStructEnumPIE.ps1 | PASS; Automation succeeded=1, failed=0, warnings=0 |
| TestDSLStructEnumAsset.ps1 | PASS, Create/Reopen |

Проверены FEnumProperty, enum-backed byte, nested enum paths, values 7/42,
canonical/qualified/case-insensitive symbols, immutable input и сохранение FName.
Unknown/_MAX/type mismatch и invalid enum snapshot отклоняются. Real PIE и новый
процесс подтвердили typed native struct ProcessEvent и parameter identity.
Existing snapshot lifetime stress tests продолжают проходить в общем наборе.

Пример: dsl_struct_enum_methods.das. Контракт: DasForUE-dsl-struct-enums.md.
Следующий этап — struct fields/defaults/migration; собственные UEnums и direct
typed enum parameters пока отдельно.
