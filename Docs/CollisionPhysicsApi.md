# Collision / Physics API

Модуль `ue_api_collision` установлен в `Plugins/DasForUE/Content/Das/Scripts`.
Профиль UE 5.8.3: `Bindings/Packages/CollisionPhysics.profile.json`.

```dascript
options gen2
require ue_api_collision
```

Прямые зависимости: daScript `ue_foundation` (экспортируется public), UE `Engine`.
Для самостоятельного импорта World/Math не требуются: closure пакета содержит
нужные object views, enum и структуры, а canonical math берётся из foundation.
Зависимости и их компиляция проверяются генератором на всём модуле.

World/Math можно подключить дополнительно. Общие generated object views и
функции могут иметь совпадающие имена; выбирайте модуль явно:

```dascript
require ue_api_collision
require ue_api_world
require ue_api_math

// В gameplay-контексте с self:
let actor=ue_api_world::C_Actor_adf65d5b_from(ue_self_object())
let scale=ue_api_world::ue_actor_GetActorScale3D(actor)
```

Project `Content/Das/Scripts` имеет приоритет над plugin scripts. Копия
`ue_api_collision.das` в проекте намеренно переопределяет установленную версию;
для обновления используйте единый источник. В данном smoke-проекте такой копии нет.
Установка инвалидирует UBT rules cache: новый файл должен попасть в перечень
plugin DAS scripts при следующей сборке. Runner с `-Publish` без `-SkipBuild`
обновляет только target metadata после установки и проверяет наличие модуля
в UFS runtime dependencies. Это проверка build receipt; cooked/Shipping build
в этой итерации не запускается.

## Покрытие выбранного профиля

241 уникальное объявление: **234 generated, 0 export blockers, 0 generation
blockers, 7 documented policy exclusions**. Construction guards сохранены
у 44 функций. 89 готовых объявлений пересекаются с Math/World, 145 отсутствуют
в этих baseline-пакетах; это не число новых возможностей всей привязки.

Включены Line/Sphere/Box/Capsule Trace и sweep (24 объявления), overlap,
HitResult/projectile helpers, выбранный PrimitiveComponent physics/collision
API, ConstraintInstanceBlueprintLibrary, PhysicsConstraintComponent и
PhysicsHandleComponent. Native поля и унаследованные копии методов этим
профилем не добавляются; список не обещает всю физику Unreal.

Семь исключений заданы явно в профиле. Три deprecated-функции имеют современные
замены; три служебных/non-BlueprintCallable объявления остаются закрытыми.
Raw `GetBodyInstanceAsyncPhysicsTickHandle` не разрешён generic transport:
поддерживаемая альтернатива — `ue_async_physics`, `ue_physics_body` и
`ue_physics_add_force` в отдельной VM. Это реализованный
[Async Physics DSL](AsyncPhysicsDSL.md), а не ожидающий реализации codec.
Сложные/welded/skeletal тела, worker-thread VM и network resimulation остаются
отдельными расширениями вне этой области.

Поимённые причины, альтернативы, исходные export diagnostics и native-policy
evidence: [отчёт покрытия](CollisionPhysicsBlockers.md), машинный снимок
`Bindings/Packages/ue_api_collision.summary.json`. Технический backlog и
документированные исключения в отчёте разделены.

## Воспроизведение и границы проверки

```powershell
.\Scripts\TestCollisionApiPackage.ps1 -Publish
# При уже актуальной сборке:
.\Scripts\TestCollisionApiPackage.ps1 -SkipBuild -Publish
```

`-Publish` устанавливает сгенерированный модуль локально после успешного
staged preflight. Это не Git-коммит и не внешняя публикация. Без флага runner
проверяет staged-кандидат. Свежие успешные baseline reports World/Math нужны
только для отчёта пересечения покрытия.

Целевые проверки: 27 Python-тестов генератора/отчёта; fresh reflection и native
inventory; компиляция всего пакета; runtime damping/gravity/collision settings;
actor overlap hit/miss/ignore и component identity; три ожидаемых отказа
(null/wrong receiver и missing WorldContext); staged combined World/Math;
обычный installed `require ue_api_collision` отдельно и вместе с World/Math.
Runner сверяет SHA256 установленного модуля и стабильность Runtime/Editor DLL,
не принимает engine errors/ensure в положительных probes.

Результат: `logs/collision-api-package-validation.json`, исходные evidence в
`Build/ApiPackages/ue_api_collision/<run>/`. Generated означает проверенную
компиляцию, а не выполнение каждой функции. Полный Foundation, PIE, packaged,
повторные trace/constraint/async simulation suites здесь не запускаются.
Отдельные предыдущие сценарии: [HitResult](HitResultTransport.md),
[Constraint](ConstraintSimulation.md), [Async Physics DSL](AsyncPhysicsDSL.md).
