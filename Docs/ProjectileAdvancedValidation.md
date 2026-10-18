# PredictProjectilePath_Advanced: массивы ссылок внутри struct

Генератор принимает ObjectProperty и ClassProperty arrays в полях структур,
с проверкой storage marker и класса элемента, как для UFunction-параметров.
Используются существующие checked object/class bridges и nullable reads.
WeakObjectProperty arrays не заменяются strong storage.

Regression cases: Scripts/TestStructReferenceArraysGenerator.py. Проверяются
object/class input/output bridges, неизвестный класс, неправильные storage
markers и пути массива внутри вложенной структуры.

## Интеграционная проверка

Нужны runtime с native UEHitResult transport, Collision package generation
и smoke fixture с GameWorld/CollisionWorld. Эти компоненты развиваются отдельно.

После генерации полного Collision пакета:

```powershell
./Scripts/TestProjectileAdvanced.ps1 -Report <fresh-run/report.json>
```

Runner и Tests/ProjectileAdvanced/probe.template.das проверяют hit, nested
HitResult/PathData, ActorsToIgnore внутри PredictParams, clear, null element,
дубликаты и spatial miss. Сам runner не запускает сборку или полные наборы тестов.

Результат 2026-10-18: PASS, весь generated Collision module — 176 wrappers,
HitResult coverage 33/33. Пять regression cases, 96 существующих generator cases
и восемь HitResult cases прошли (109 суммарно). Runtime probe прошёл; хеши двух
DLL не менялись. Отчёт logs/projectile-advanced-validation.json. Native rebuild,
full Foundation, PIE и packaged для этого исправления не требовались.
