# Проверка native DSL frontend — 2026-10-06

Репозиторий: C:/src/DasForUE2. Unreal Engine 5.8.3, daScript 0.6.4.

| Проверка | Результат |
|---|---|
| TestEditor.ps1 | PASS, 287 checkpoints |
| TestPackaged.ps1 (Win64 Development) | PASS, 287 checkpoints; BuildCookRun exit 0 |
| TestDSL.ps1 Create/Reopen | PASS, 20 + 13 checkpoints |
| TestAuthoring.ps1 (JSON regression) | PASS |
| TestLifecyclePIE.ps1 | PASS |
| TestObjectsPIE.ps1 | PASS |

DSL проверен на создании Actor Blueprint, вызове generated methods через
ProcessEvent, добавлении полей/методов, сохранении override Score=77, metadata,
stable IDs, сохранении asset и повторном открытии в новом процессе. Negative
probes отклоняют неизвестные/конфликтующие флаги, неподдержанные типы/signatures,
write в BlueprintReadOnly, внутренний argument и необъявленные private fields;
предыдущая authoring-модель сохраняется. Runtime probe проверяет `self` reads,
int/float assignment и compound writes на настоящем UObject.

Editor/packaged smoke также сохраняют существующие проверки marshalling,
containers, delegates, GC, contexts и reload fallback. PIE probes проверяют
существующий lifecycle/object path. Автоматический DSL editor watcher и новый
DSL structural hot reload во время PIE пока не реализованы; это следующий этап.

Подробности и границы: DasForUE-dsl-frontend.md. Примеры: dsl_counter_v1.das,
dsl_counter_v2.das. Полные журналы — C:/src/DasForUE2/logs.
