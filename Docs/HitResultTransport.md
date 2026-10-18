# Native HitResult transport для generated API

`/Script/Engine.HitResult` — проверенный непрозрачный leaf в generic call frames.
В DAS он представлен `UEHitResult`, а не автоматически восстановленной записью
из reflected полей. Экспортёр и генератор используют явный marker
`transport="hit_result_v1"` у параметра/leaf и opaque struct definition с `fields=[]`.
Произвольные структуры не могут использовать этот marker; flatten private
ActorInstanceHandle по-прежнему отклоняется.

## Представление и вызовы

```das
// generated LineTraceSingle возвращает bool и UEHitResult в одном result record
let result <- ue_collision_LineTraceSingle(/* explicit arguments */)
let hit=ue_hit_result_view(result.out_outhit)
if(hit.blocking_hit) {
    let actor=ue_hit_actor(hit)
    let component=ue_hit_component(hit)
    print("distance={hit.distance}, penetration={hit.penetration_depth}\n")
}

// Тот же opaque native value можно передать в generated consumer.
let details <- ue_collision_gameplay_BreakHitResult(result.out_outhit)
let empty=ue_hit_result_empty()
```

Полный выполняемый пример: `Tests/HitResultTransport/probe.template.das`.
Runner подставляет имя проверенного staged collision module; fixture генерируется
в той же Build-папке. Публичные аргументы всех пакетных функций остаются явными.

`UEHitResult` содержит только закрытый token. Контекст хранит `FInstancedStruct`
с настоящим FHitResult; native struct copy сохраняет и reflected поля, и private
native state. Ни адрес UE-памяти, ни поля ActorInstanceHandle наружу не выдаются.
Результат отделён от call frame и переживает его освобождение. Вход — native copy,
поэтому изменение ref/out не меняет исходный immutable snapshot.

Механизм работает для scalar/ref/out, массивов FHitResult и вложенных структур
с такими полями/массивами. Он не включает UEHitResult в авторские `ue_property`,
`ue_function`, JSON defaults или arbitrary native handle construction.

## Weak references и ActorInstanceHandle

Снимок и входные/выходные frames сохраняют настоящую weak storage FHitResult.
PhysMaterial, Component и ActorInstanceHandle не превращаются в strong pins.
`ue_hit_result_view()` строит существующий UETraceHit с checked weak handles.
Actor берётся через `HitObjectHandle.GetCachedActor()`: просмотр не создаёт
dehydrated actor. После Destroy/GC getters возвращают null; числовая копия
попадания остаётся доступной.

Вызванная native функция сохраняет собственную семантику UE: например, её код
может использовать GetActor(). Гарантия отсутствия материализации относится
к нашему view/инспекции, а не к произвольному коду вызываемой функции.

При передаче проверяются pending/executed frame, направление, точный native
struct identity, и принадлежность token активному контексту. Незаданный input,
нулевой/чужой token, неправильная структура и попытка доступа к private path
отклоняются. На выходе отвергаются nonfinite координаты/time/distance/depth.

## Хранение и очистка

Используется существующая ограниченная таблица StructValues, без второго
неограниченного registry. Root walker распознаёт UEHitResult, включая элементы
массивов и вложенные records. При `options gc` unreachable snapshots собираются
по тем же правилам, что UEStructValue; общий предел — 4096 живых записей.
На границе invocation работает существующая сборка snapshots. Destroy контекста
и hot reload уничтожают его таблицу; старые tokens не переносятся в новый контекст.
Автоматический перенос HitResult как persisted hot-reload state не добавлен.

## Целевая проверка

```powershell
.\Scripts\TestHitResultTransport.ps1
# Если editor target уже собран:
.\Scripts\TestHitResultTransport.ps1 -SkipBuild
```

Runner выполняет generator cases, обновляет/компилирует Collision package,
проверяет его основной overlap probe, receiver/world rejections и совместный
импорт World/Math. Затем в transient Game world с двумя BoxComponent проверяет
generated line trace, sphere/box/capsule sweep, miss/penetration, multi sweep,
BreakHitResult, native scalar/array/nested/ref roundtrip и ActorInstanceHandle.

Реальный UE GC выполняется с удерживаемыми opaque snapshots, output frame и
pending input frame. Отдельно проверяются Destroy, неправильные type/direction,
отсутствующий input, private path и сборка временных opaque snapshots с сохранением
живого массива. Fixture USTRUCT/UCLASS не являются новым пользовательским API.

Итог: `logs/hit-transport-validation.json`; native/runtime logs и generated
fixture находятся в соответствующей `Build/ApiPackages/ue_api_collision/<id>`.
Runner проверяет, что DLL не изменилась во время валидации. Полные Foundation,
PIE, packaged/Shipping и физическая симуляция здесь не запускаются.

## PredictProjectilePath_Advanced: blocker закрыт — 2026-10-18

Генератор разрешает ObjectProperty/ClassProperty arrays в полях struct так же,
как в параметрах UFunction. Сохраняются проверка native storage marker и класса,
nullable object reads, контроль направления и существующий frame marshalling.
WeakObjectProperty arrays по-прежнему не подменяются сильными ссылками.
Новый native ABI или ручной wrapper для Advanced не понадобился.

Повторная генерация и весь Collision module preflight — PASS: 176 wrappers,
все 33/33 объявления HitResult. Пять новых regression tests, 96 существующих
generator tests и восемь HitResult generator tests — PASS (109 суммарно).

Целевой runtime probe: hit, nested HitResult/PathData, ignored Actor внутри
PredictParams, clear массива, null element, дубликаты и spatial miss — PASS.
Две загруженные DLL стабильны. Native rebuild, full Foundation/PIE/packaged
не запускались. Collision package остаётся staged; новый коммит не создавался.

Воспроизведение: Scripts/TestStructReferenceArraysGenerator.py;
Scripts/GenerateApiPackage.ps1 -Profile Bindings/Packages/CollisionPhysics.profile.json -SkipBuild;
Scripts/TestProjectileAdvanced.ps1 -Report <fresh-run/report.json>.
Probe: Tests/ProjectileAdvanced/probe.template.das.
Отчёт: logs/projectile-advanced-validation.json;
coverage: logs/collision-hitresult-coverage-after.json.


ActorsToIgnore в Advanced projectile params теперь поддержан общим [reference-array generator](StructReferenceArrays.md); этот blocker закрыт отдельным целевым этапом.
