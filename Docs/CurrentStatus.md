# Актуальное состояние и функциональные приоритеты

Обновлено 2026-10-18. Это сводка по текущему коду и отчётам, а не процент
готовности к production. Ранние разделы README и записи Plan.md описывают
исторические этапы; ограничения нужно читать вместе с последующими расширениями.

## С чего начать

Готовый пример — [PlayableCharacter](PlayableCharacter.md), исходник
`Plugins/DasForUE/Content/Das/Scripts/dsl_playable_character.das`.
В нём уже есть настоящий Character, камера и текстовый компонент, defaults,
редактируемые input-ссылки, WASD, мышь, прыжок, lifecycle и освобождение input.
Нативные поля и отражённые методы вызываются обычным синтаксисом `self`.

```powershell
.\Scripts\CreatePlayableCharacter.ps1
# Если Editor уже собран:
.\Scripts\CreatePlayableCharacter.ps1 -SkipBuild
```

Команда создаёт свежую папку `/Game/DasExamples/Playable/<id>` с Character,
pickup, HealthComponent, Widget Blueprint HUD, шестью InputAction,
MappingContext, отдельным mesh, двумя NiagaraSystem и картой (15 ассетов). Интерфейс взаимодействия
хранится один раз в `/Game/DasExamples/Shared/Interactable`.
Путь записан в `logs/playable-example.json`.
Набор регрессионных Blueprint-ов при этом не создаётся. Повторный запуск создаёт
новую папку; изменение структуры существующего Blueprint требует его reimport.

## Что уже реализовано

| Область | Возможности и текущая граница |
| --- | --- |
| DSL | Class/struct/enum/interface, свойства и функции, аннотации и групповые объявления, defaults, компоненты и наследование. Это отдельная authoring-модель, а не прямое размещение объектов daScript в памяти UE. |
| Вызовы | UE ↔ daScript, return/ref/out, Blueprint Function Libraries и WorldContext, типизированные объектные и классовые ссылки. Произвольный C++ и любые virtual override автоматически не открываются. |
| Значения | Native enum/struct, массивы, карты и множества, объектные ссылки, FName/FText, soft object/class и поддержанные soft-контейнеры. Точные сочетания типов имеют явные ограничения. |
| Игровая логика | Actor/Component/Pawn/Character, Initialize/Construction/BeginPlay/Tick/EndPlay, default components и attachments, поиск/Spawn, input, timers, delegates, async/latent adapters. Отдельные примеры уже существуют. |
| Нативный API | Генерация по reflection для выбранных классов и поддержанных сигнатур. Сейчас семь игровых Engine receivers плюс отдельный ABI fixture; тип возвращаемой ссылки сам по себе не открывает все методы её класса. |
| Обновление | Logic reload с сохранением последней рабочей версии; часть structural/defaults migrations с предварительной проверкой и ограниченным откатом. Универсальная миграция любых типов и объектов не обещается. |

Подробности: [генерация нативного API](NativeGameplayGeneration.md),
[контейнеры](NativeGameplayHashContainers.md), [soft-ссылки](SoftReferences.md),
[Function Libraries](DSLFunctionLibraries.md), [композиция компонентов](DSLComponentComposition.md).

## Следующие функциональные шаги

1. **Сквозное взаимодействие с миром — реализовано.** В сцене есть
   [подбираемый предмет](PlayablePickup.md): mesh, сфера столкновений, реальное
   событие пересечения, счёт и восстановление по таймеру. Для генератора добавлены
   StaticMeshComponent/SphereComponent, для ссылок — публичные проверки
   валидности и идентичности. Дальнейшие API добавляются под следующую игровую
   механику, а не ради расширения списка поддержанных классов.
2. **Убирать низкоуровневый код по результатам этого примера.** Пользовательские
   методы должны использовать типизированные ссылки, обычные поля/методы и
   публичные managed helpers. Ручные call frames и внутренние `__ue_*` нужны
   только там, где ещё нет подходящего публичного API. Переименование helpers
   без улучшения возможностей само по себе не является задачей.
3. **Отдельно закрывать production-возможности.** Shipping, полноценный AOT
   всего игрового пути, чистая машина, сеть/RPC/replication и расширенные
   миграции — самостоятельные этапы. GAS, StateTree и прочие пакеты
   добавляются под конкретную потребность; их наличие в плане не означает реализацию.

Оптимизации хранилища snapshot-ов и проверки исходников пока отложены.
Возвращаться к ним стоит при измеримой проблеме в игровом сценарии.

## Проверки при разработке

На итерации — необходимая сборка и целевые проверки изменённого пути.
Полный Foundation, PIE-набор и packaged build выполняются перед завершением
крупного этапа, по запросу либо при обоснованном риске для нескольких систем.
В отчёте отдельно перечисляются выполненные и отложенные проверки.

Для текущей версии: Editor Development, семь ассетов и семь сценариев
подбора в отдельном game-процессе — PASS; также прошла 31 проверка генератора.
Полные Foundation/PIE/packaged не запускались. NullRHI и программно поданный input
не подтверждают качество рендера или работу физических устройств.
Отчёт: `logs/pickup-validation.json`. Коммит не создавался.

Дополнение: целевая проверка pickup прошла, включая реальное пересечение,
восстановление в занятой области и отмену при Destroy. Семь сохранённых ассетов;
31 проверка генератора. Полные Foundation/PIE/packaged не запускались.
См. PlayablePickup.md и logs/pickup-validation.json.

## Автоматическое обнаружение native API — 2026-10-18

C++ список receiver-классов удалён. Генерация обнаруживает native-объявления
из входных DAS-модулей и их импортов; весь поддерживаемый API обнаруженных классов
экспортируется без выбора отдельных вызовов. Текущий реестр: 18 классов, 879 полей,
1873 метода. Команда генерации также собирает обновлённый реестр, если не указан
-SkipBuild/-Check. См. [процесс и ограничения](NativeGameplayDiscovery.md).
Точечные discovery/generator/reflection/runtime/pickup проверки прошли; полный
Foundation/PIE/packaged прогон не выполнялся.

## Async mesh pickup — 2026-10-18

MeshAsset в PlayablePickup — типизированная soft-ссылка. До completion предмет
скрыт и не собирается; ошибки оставляют его недоступным. LoadMesh заменяет старую
заявку, managed teardown отменяет её при Destroy/EndPlay. Сохранён отдельный mesh
пакет для проверки настоящей холодной загрузки. Все 12 точечных сценариев прошли
в чистом smoke-проекте с текущими DLL/DAS, без полного Foundation/PIE/package.
Неоднозначность копий Interactable при повторном экспорте затем устранена общим контрактом;
повторный экспорт и все 12 сценариев прошли в основном smoke-проекте.
Актуальные пути и границы проверки — в logs/pickup-validation.json.


## Трассировки, UMG и damage — 2026-10-18

Добавлен [игровой API и пример](GameplayServices.md): LineTrace и SphereTrace
с типизированным результатом и слабыми ссылками; создание Widget Blueprint HUD,
FText, кнопки и управление локальным вводом с очисткой по времени жизни контекста;
нативный ApplyDamage и авторский HealthComponent с событием Changed.
`ue_event(source="Health",...)` теперь может подписываться на свой компонент.
E взаимодействует с предметом по лучу камеры через Blueprint-интерфейс;
H наносит урон; Tab включает курсор. Обычный подбор при пересечении сохранён.

Целевые проверки и их границы: `logs/gameplay-features-validation.json` и
`logs/pickup-validation.json`. Это начальный срез соответствующих систем:
multi-hit/object queries, полный UMG API, point/radial damage, networking и
передача UETraceHit через отражённые свойства/методы пока не реализованы.
Полные Foundation/PIE/packaged проверки в этом изменении не выполнялись.
Коммит не создавался.

## Повторный экспорт Interactable — 2026-10-18

Новый игровой пример использует один shared Interactable по каноническому пути;
повторный экспорт больше не создаёт одноимённый интерфейс в папке каждой карты.
Старые копии сохраняются. Ранее описанная блокировка основного smoke-проекта
устранена для этого примера; см. SharedInteractionInterface.md и целевой отчёт
logs/shared-interface-validation.json.


## Montage и animation events — 2026-10-18

Добавлен [managed Montage API](Montage.md), модуль `ue_animation.das` и пример
`dsl_montage_actor.das`: Play со скоростью, позицией и секцией; Stop, Cancel,
проверка активности; типизированный MontageEvent (Completed/Interrupted/BlendOut/
NotifyBegin/NotifyEnd). Поддержаны Montage Notify и Montage Notify Window.
ID подписки относится к конкретному native instance ID. Завершение, отмена,
Destroy/EndPlay и успешный hot reload снимают собственные обработчики; отклонённый
reload сохраняет рабочую версию. Успешный reload не перезапускает анимацию.

`Scripts/TestMontage.ps1 -SkipBuild`: восемь групп целевых сценариев — PASS,
включая настоящий Notify Window, отрицательные входы, повторный запуск,
ожидающие события, сохранение стороннего listener, принятый/отклонённый reload
и Destroy. Отчёт: `logs/montage-validation.json`.
Это проверка Editor Development в game-процессе без рендера; полноценный AnimBP,
видимые позы/root motion, сеть, полный Foundation/PIE/packaged не проверялись.
Коммит не создавался.

## Звук и Niagara — 2026-10-18

Добавлен [управляемый API эффектов](Effects.md), `ue_effects.das`.
Playable pickup и Character воспроизводят звук и Niagara при подборе и уроне.
Завершение пары освобождает компоненты и вызывает необязательный callback один
раз; stop, Destroy/EndPlay и успешный reload отзывают pending callbacks.
Отклонённый reload сохраняет эффект. Чужие компоненты не затрагиваются.
Семь целевых групп `TestEffects.ps1 -SkipBuild` — PASS, реальный RHI offscreen,
AudioComponent и CPU Niagara. Визуальное/слуховое качество, networking, полные
Foundation/PIE и packaged не проверялись. Без коммита.

## Завершение точечной проверки montage — 2026-10-18

Editor Development и TestMontage в основном smoke-проекте — PASS: десять игровых
групп и одна отрицательная authoring-проверка. Дополнительно подтверждены повторный
запуск с тем же именем из Completed и сохранение native playback другой группы
после play/stop/replace/cancel. Неверный тип аргумента callback отклоняется до
создания Blueprint. Базовые notify/stop/cancel/reload/Destroy проверки сохранены.
Отчёт: logs/montage-validation.json; ограничения и пример: Montage.md.
Полный Foundation/PIE/packaged и видимые позы/root motion не проверялись.

## AIController и MoveTo — 2026-10-18

Добавлен [управляемый AI API](AI.md): AIControllerRef, движение к Actor/позиции,
MoveResult (Success/Blocked/OffPath/Aborted/Invalid), необязательный completion,
проверка активности и отмена по имени. Cleanup учитывает UE request ID, сохраняет
более новое внешнее движение и чужие listeners. Destroy/EndPlay и успешный reload
отзывают движение и pending callback; отклонённый reload сохраняет рабочий запрос.
Отдельный `CreateAIEnemy.ps1` экспортирует playable demo с противником, который
обходит препятствие по сохранённому NavMesh и наносит урон игроку.
`TestAI.ps1 -SkipBuild`: восемь целевых групп — PASS, включая настоящее движение
и damage, queued completion/Destroy и принятый/отклонённый reload. Отчёт:
`logs/ai-validation.json`. Rendering/audio, сеть, полный Foundation/PIE и packaged
не проверялись. Без коммита. Behavior Tree/Perception/StateTree пока не добавлены.

## Аудит покрытия API — 2026-10-18

Добавлен воспроизводимый [отчёт](ApiCoverage.md) и
[план системного расширения](ApiExpansionPlan.md). Шесть модулей, 1 938 классов,
16 401 уникальное объявление; 4 546 проходят native-фильтр, из них 3 315 пока
нигде не зарегистрированы. У library exporter 1 762 eligible-функции без
статических обёрток в проверенных generated-модулях. Числа не являются общим
процентом готовности UE binding и не учитывают всю доступность через adapters.
Генератор и аудит используют общую политику; native artifacts -Check — PASS.
14 тестов отчёта и 34 теста генератора — PASS. Полные runtime/PIE/packaged в
аудите не запускались; существующий набор generated API не расширялся.

## Самостоятельный проект — 2026-10-18

Добавлен [DasStarter](../Examples/Standalone/README.md): шаблон нового проекта,
NewDasProject.ps1 и project-aware native generation. Скрипты размещаются в
Content/Das/Scripts проекта; flat require и authored references используют
project-first lookup с fallback в плагин. Generated header/IR/DAS остаются
в проекте; внешние plugin DLL UBT размещает в его Binaries.

На C:/src/DasStarter/DasStarter.uproject прошли Editor Development build,
экспорт трёх Blueprint и карты, четыре игровых проверки NullRHI: possession/
компонент, настоящий collision и авторский вызов, respawn timer и импорт,
hot reload импорта с сохранением Score/состояния компонента и восстановлением
исходника. Повторная project-local генерация — PASS (17 native-классов),
глобальные generated-артефакты плагина не изменились. Исправлена генерация
пустых C++ registry arrays; неизменившиеся outputs больше не переписываются.
Отчёт: logs/standalone-validation.json; проектный отчёт и логи —
C:/src/DasStarter/Saved/DasValidation. Полные Foundation/PIE/packaged не запускались.
Визуальный вид, управление и подбор подтверждены пользователем вручную.
Освещение переведено в Movable; точечный offscreen D3D12/SM5 запуск подтвердил
ноль незапечённых объектов. LightingOnly обновляет свет существующей карты
без пересоздания сцены; отчёт — Saved/DasValidation/lighting-validation.json.

## Первый пакет API Math — 2026-10-18

Добавлены декларативный профиль, замыкание зависимостей, поимённые причины
пропусков и UE preflight всего модуля. Из 741 функции KismetMathLibrary
619 сгенерированы и скомпилированы вместе, 112 блокирует экспортёр, 10 —
непрозрачные структуры в генераторе. Целевой NullRHI probe проверил scalar,
Vector, Transform, LinearColor, массивы и ref/out. Модуль ue_api_math установлен;
Vector2D, Vector, Rotator и Transform используют canonical foundation-типы,
включая вложенные значения и массивы. Остальные structs пока используют
generated transport. Все аргументы явные по политике профиля. Подробнее: [ApiPackages](ApiPackages.md).
Полные Foundation/PIE/packaged на этом этапе не запускались.

## Пакет World / Actor / Components — 2026-10-18

Реализованы receiver profiles и отдельные counts для library/instance surfaces;
унаследованные occurrences не смешиваются с уникальными declarations.
WorldActorComponents: 311 instance wrappers и 253 library wrappers (564 вместе).
339 экспортных blocker'ов и один opaque-struct generation blocker сохранены
поимённо. Экспортёр явно отклоняет soft-reference transport без адаптера и
UnsafeDuringActorConstruction до общего context guard. Ни native registry,
ни native fields/accessors этим пакетом не расширяются.
Подробности, примеры и целевые проверки: [WorldApiPackage](WorldApiPackage.md).
Далее — общий accessor/construction-policy этап, затем Collision/Physics.

## Native accessors и construction guard — 2026-10-18

[Общий object-property слой](NativeAccessAndConstruction.md) поддерживает native
FProperty Getter/Setter и BlueprintGetter/Setter, сохраняет readonly и проверяет
значение до вызова setter. Используются существующие value descriptors/carriers
и GC roots; bitfield bool ABI нормализуется отдельно. Контракты сериализованы
в native manifest/header; FProperty/UFunction pointers не кэшируются между reload.
UnsafeDuringActorConstruction больше не исключает методы целиком: native registry
и generic wrappers сохраняют guard, runtime проверяет фазу до dispatch.

160 Python-тестов, native artifacts check, Game/Editor ConstructionScript,
metadata-removed guards, getter/setter semantics, readonly/stale/type/infinity,
GC в setter и настоящий Engine accessor example — PASS. World пакет повторно
проверен: 652 wrappers, 88 guarded, blockers 246 export + 6 generation; Math 619.
Без полного Foundation/PIE/packaged прогона и без собственного коммита.
Далее — Collision/Physics/type blockers; унификация object/enum/UEName views
между пакетами остаётся отдельной задачей.


## Collision / Physics: профиль и backlog — 2026-10-18

Добавлен [профиль Collision / Physics](CollisionPhysicsApi.md) и свежий
[поимённый отчёт блокеров](CollisionPhysicsBlockers.md). 185 кандидатов:
143 готовых wrapper, 41 export blocker, один generation blocker. Из готовых
89 уже есть в Math/World, 54 отсутствуют в этих двух пакетах (constraint/handle).
Это не подсчёт новых возможностей всей привязки. 41 функция сохраняет construction guard.

Главный общий blocker — transport HitResult (33 объявления, включая 24 traces):
weak references и ActorInstanceHandle требуют сохранения collision/lifetime
семантики, одного разрешения weak-полей недостаточно. GetConstraint дополнительно
требует uint32/weak-owner handle; opaque async physics handle и CustomThunk
остаются адаптерами. Шесть отказов — deprecated/служебная policy.

Целевой runner TestCollisionApiPackage.ps1 проверяет весь staged-модуль,
overlap hit/miss/ignore, Actor/Component identity, collision/damping/gravity,
WorldContext и receiver rejections, совместный импорт с World/Math.
Модуль не публикуется; итог выполнения — logs/collision-api-package-validation.json.
Дальше — общий collision-result transport, затем повторный экспорт выбранного API.

## Collision / Physics foundation — 2026-10-18

Добавлены FWeakObjectProperty в общий reference/value backend, физический weak
descriptor/factory/storage identity, nullable expired/destroying reads и отказ
от strong pins для weak input/output/snapshot leaves. Reflection IR сохраняет
weak_reference marker; оба генератора проверяют его тип/применимость.
Прямые weak-container carriers пока явно запрещены, без подмены на strong.

UETraceHit расширен trace endpoints, penetration depth, item/my_item/element
indices, bone names, physical material и physics owner. Добавлены UEOverlapResult
и sphere overlap с native item indices и checked Actor/Component/Owner getters.
Actor instances читаются через GetCachedActor без неявной материализации.
Private ActorInstanceHandle и raw Chaos state не flatten'ятся: полная интеграция
native collision structs в пакетные reflected signatures остаётся отдельным
adapter этапом. Это safe views, не обещание полного Physics API.

Editor Development, шесть generator cases и семь transient Game-world групп —
PASS: descriptor/factory, weak GC, класс/shape отказы, generated weak struct
frames, GC при удерживаемом weak output frame, настоящие trace/penetration/
overlap, invalid radius/scope и Destroy. Native marker и стабильность DLL
проверены. Полные Foundation/PIE/packaged не запускались.
Описание: [CollisionFoundation](CollisionFoundation.md); воспроизведение:
Scripts/TestCollisionFoundation.ps1; отчёт logs/collision-foundation-validation.json.


## Generated HitResult transport — 2026-10-18

Добавлен [общий native HitResult transport](HitResultTransport.md): opaque
UEHitResult в generic frames/IR/generator, native copy с private ActorInstanceHandle,
checked UETraceHit view и настоящими weak-ссылками. Используется существующая
bounded snapshot table/root collection. Scalar/ref/out, array и nested forms
проверены; native private state не flatten'ится и не строится из JSON.

Collision: 175 wrappers вместо 143, 8 export + 2 generation blockers вместо
41 + 1. Все 24 generated trace/sweep declarations компилируются. Дополнительно
открыты шесть GameplayStatics HitResult consumers/prediction helpers и два
component overlap метода. Advanced projectile params по-прежнему блокирует
struct array object shape, GetConstraint — отдельный weak/uint32 handle,
async physics handle — opaque adapter, ещё семь отказов — policy/CustomThunk.

127 Python tests и transient Game-world probes — PASS: generated line/sphere/
box/capsule, miss/penetration, multi sweep, native consumer и ActorInstanceHandle
roundtrip, scalar/array/nested/ref, GC с snapshot/output/pending input frame,
Destroy, type/direction/missing/private-path rejections, snapshot root collection.
Collision overlap/WorldContext/receiver/combined World-Math checks — PASS.
Отчёт logs/hit-transport-validation.json. Пакет остаётся staged; полные
Foundation/PIE/packaged не запускались, коммит не создавался.
Далее — struct arrays с object elements для Advanced projectile params либо
отдельный проверенный constraint-handle contract; это независимые задачи.

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


## Общие reference arrays в struct — 2026-10-18

[Object/class arrays](StructReferenceArrays.md) поддержаны в generic struct members.
Объединён storage validator для struct arrays и параметров UFunction; неверный
marker/класс/MetaClass и подмена weak storage отклоняются. Проба охватывает input/
output, struct arrays с вложенными object/class arrays, nullable/duplicate/empty
значения, resize и detached output. Native ABI общего runtime не менялся;
собрана расширенная тестовая fixture.

113 Python tests, весь Collision module, Advanced projectile prediction и
reflected nested array roundtrip/negative checks — PASS. Collision 176 wrappers,
8 export + 1 generation blockers, HitResult declarations 33/33 generation ready.
Полные Foundation/PIE/packaged не запускались, staged only, без коммита.
Отчёт logs/struct-reference-arrays-validation.json.
Далее — отдельный checked constraint-handle contract либо следующий пакет
Pawn/Character/AI/Navigation с документированными исключениями Physics.


## Симуляция тела и PhysicsHandle — 2026-10-18

Добавлен DasPhysicsSimulation commandlet и phased DAS probe, использующие
generated ue_body_*/ue_handle_* API в transient Chaos Game world с фиксированным
шагом 1/60 с. Без GameMode fixture явно маршрутизирует NotifyBeginPlay,
чтобы PhysicsHandle участвовал в обычном component tick.

Editor build, whole-module preflight, восемь независимых native measurements
и null receiver rejection — PASS: масса/linear/angular velocity, impulse,
force, damping, gravity, grab/translate, target rotation и release/free motion.
450 шагов соответствуют 7,5 с симуляции. Фактические DLL и fixture/module
не менялись во время проверки; engine errors/ensure отсутствуют.

Runner: Scripts/TestPhysicsSimulation.ps1; DAS template:
Tests/PhysicsSimulation/probe.template.das; описание: Docs/PhysicsSimulation.md.
Отчёт logs/physics-simulation-validation.json. Contact/friction, ragdoll,
отдельные constraints, async/network physics и полные Foundation/PIE/packaged
этот этап не проверяет. Новых ручных wrappers не добавлено.


## Checked GetConstraint transport — 2026-10-18

Добавлен [безопасный UEConstraintHandle](ConstraintTransport.md): component-owned
getter и ref/out transport ConstraintInstanceBlueprintLibrary. Weak owner/bodies,
Game world/lifecycle и поколение Chaos proxy проверяются перед dispatch;
пересоздание constraint, Destroy и смена VM-контекста инвалидируют токен.
Native private index/editor callback не становятся частью DAS snapshot.

CollisionPhysics расширен 56 методами библиотеки: 233/241 ready, 7 export +
1 generation blockers. GetConstraint закрыт; остаются CustomThunk, opaque async
physics tick handle и шесть deprecated/служебных объявлений. Все функции пакета
скомпилированы; runtime проверяет выбранный getter/setter и lifecycle guards.

Smoke Editor build, 112 Python tests, whole module и целевые runtime generation/
pending/delayed result/weak GC/foreign context probes — PASS. Отчёт:
logs/constraint-transport-validation.json. Пакет staged; полный Foundation,
PIE и packaged не запускались, без коммита.


## Симуляция двух тел с PhysicsConstraint — 2026-10-18

Добавлен DasConstraintSimulation commandlet и phased DAS probe для двух dynamic
Chaos bodies. Generated ue_joint_*/ue_constraint_* управляют linear Locked/Free/
Limited и angular Locked/Free/Limited twist; native assertions измеряют рамки
constraint после реальных World ticks. Лимиты также меняются на движущейся связи.

Editor build, whole Collision module preflight и 12 runtime этапов — PASS.
Break освобождает тела; recreation инвалидирует старые snapshot и pending setter
без побочного эффекта, fresh handle и новая физическая связь работают.
516 шагов (8,6 с). Нет engine errors/ensure; DLL/fixture/module hashes стабильны.
Fixture без gravity/contacts; swings, drives/motors, force-threshold breaking,
network/async physics и долгий stress не входят в эти проверки.

Runner Scripts/TestConstraintSimulation.ps1; template
Tests/ConstraintSimulation/probe.template.das; Docs/ConstraintSimulation.md.
Отчёт logs/constraint-simulation-validation.json. Полные Foundation/PIE/packaged
не запускались. Новых ручных production wrappers не добавлено.


## Checked collision-profile property adapter — 2026-10-18

[SetCollisionProfileNameProperty](CollisionProfilePropertyAdapter.md) открыт
узким native dispatch adapter: точная native function/parameter signature,
scalar FCollisionProfileName, writable BlueprintVisible property, живой
non-template target в том же Game world. Запись использует общий property
access layer и сохраняет setter semantics. Это присваивание struct-поля,
а не применение collision profile к PrimitiveComponent.

CustomThunk stub не вызывается; исключение internal/thunk policy действует
только для этого известного адаптера. В runtime встроен construction guard;
bind_events/EndPlay и editor/чужие миры запрещены. Generated IR содержит
call_adapter=collision_profile_property_v1 и construction-unsafe contract.

Collision: 234/241 ready, 6 export + 1 generation blockers. Остались opaque
async physics tick handle и шесть deprecated/служебных объявлений. Smoke Editor
build, 113 Python tests, whole staged module и 22 целевых native/runtime checks
— PASS, включая inherited field, setter, wrong type/source и incomplete input,
missing/readonly/hidden/array/name errors, CDO/null/foreign world, pending Destroy
и lifecycle guards без CustomThunk metadata. Отчёт:
logs/collision-profile-adapter-validation.json. Пакет staged; Foundation/PIE/
packaged не запускались, коммит не создавался. Далее — исследовать consumers
async physics tick handle и определить безопасный контракт; остальные шесть
отказов могут остаться документированными исключениями.


## Pawn / Character / AI / Navigation: первый пакет — 2026-10-18

Добавлен декларативный PawnCharacterAINavigation.profile.json: одна library,
12 receivers, полные local reflected declarations. 410 кандидатов: 241 ready,
168 export blockers, 0 generation blockers, один явный managed-ownership exclusion.
Все 241 wrappers скомпилированы; construction guards сохранены для шести функций.
Baseline World/Math/Collision разделяет 21 ready declaration, ещё 220 отсутствуют
в этих трёх пакетах (не счётчик новых возможностей всей привязки).

Smoke Editor build, whole module, пять native runtime groups и combined World/Math
imports — PASS: Pawn input, Character movement mode/pending launch, possession/focus,
empty/constructed NavigationPath, empty perception arrays и receiver/MetaClass guards.
Реальный NavMesh, movement ticks, perception stimuli и managed MoveTo suite не
запускались; существующий managed AI adapter сохранён. DLL/source hashes стабильны.

Следующий общий blocker: static BlueprintCallable вне BlueprintFunctionLibrary
(11 NavigationSystemV1 queries и два Pawn helpers). Шесть Client* RPC, latent
force feedback и raw MoveTo proxy имеют отдельные policy/adapter решения.
Visibility/deprecation даёт ещё 148 отказов. Collision / Physics продолжает
свой отдельный план и не блокирует этот пакет.

Описание Docs/PawnAINavigation.md; backlog Docs/PawnAINavigationBlockers.md;
runner Scripts/TestPawnAIPackage.ps1; logs/pawn-ai-package-validation.json.
Пакет не опубликован; полные Foundation/PIE/packaged не запускались.


## Async physics body handle: контракт и явный execution gate — 2026-10-18

Исследован [GetBodyInstanceAsyncPhysicsTickHandle](AsyncPhysicsHandleContract.md).
Это raw Chaos proxy, не owning/weak snapshot. В доступных C++ исходниках
Engine/Source + Engine/Plugins найден один consumer — PhysicsThreadLibrary.AddForce;
loaded reflection подтверждает один getter и один consumer. Engine callback
идёт на frozen GameThread в internal physics context; обычного IsInGameThread
недостаточно. Debug thread flags не заменяют shipping capability.

Выбран будущий контракт: precompiled isolated PhysicsPreSim VM, whitelist UE
operations, weak owner/body selector + generation marker, session/revision +
callback epoch, запрет токена вне текущего callback. Первый body scope — unwelded
single-body primitives; первая операция — finite AddForce/accel-change. До
поддержки execution roles нельзя открывать raw getter/consumer общей VM.
Design record: Bindings/AsyncPhysics.contract.json; это не runtime-конфигурация.

В generic call marshalling добавлен явный отказ до создания frame. Collision
остаётся 234/241 ready; теперь 7 export + 0 generation blockers, один в группе
execution_context и шесть deprecated/служебных. Это уточнение причины, а не
реализация async handle transport или новый рабочий DAS API.

Build, 7 Python report tests, whole staged Collision preflight и 24 целевых
native/runtime checks (sync + fixed async 1/60) — PASS: native consumer force/
accel-change, presim thread context, recreate/generation, unregister/Destroy/GC,
два ordinary DAS rejection paths в каждом режиме. Стабильность DLL подтверждена
финальным прогоном. Отчёт logs/async-physics-audit-validation.json; source hashes
и anchors — logs/async-physics-source-audit.json. Foundation/PIE/packaged и
skeletal/welding/network/resim не запускались. Пакет staged, без коммита.

Далее по этой теме: execution-role guards всех UE externs и authored callbacks,
безопасный native lifecycle bridge, затем отдельный PhysicsPreSim context и
scoped body/AddForce adapter. Синтаксис DSL не фиксировался. Обычные GT force/
impulse уже доступны и не требуют открытия async handle.


### Native static NavigationSystemV1 — 2026-10-18

Общий backend теперь поддерживает native static BlueprintCallable вне BFL,
без ручных wrappers. Повторно сгенерирован Pawn/AI пакет: 254 ready (+13),
155 export blocked, 0 generation blocked, 1 policy excluded. Все 11 NavigationSystemV1
static APIs и два Pawn helpers выполнены в transient мире без NavigationSystem.
Сохранены WorldContext, signature, construction и RPC policy. Сборка Editor,
115 Python tests, whole-module, семь runtime groups и совместные World/Math imports — PASS.
Следующее: успешные query/path результаты на настоящем NavMesh. Полный PIE/packaged
не запускались, пакет остаётся staged. Docs/PawnAINavigation.md и
logs/pawn-ai-package-validation.json содержат границы проверки.


### Navigation queries на настоящем Recast NavMesh — 2026-10-18

Сборка Editor, свежий Pawn/AI package preflight и шесть точечных групп — PASS.
Проекция и random/reachable, blocked/clear raycast, полные location/Actor paths,
Success cost/length, partial path к изолированной площадке и отказ проекции вне
NavMesh проверены через сгенерированный static backend. Transient сцена не
сохраняется; маршрут 1410.962 см обходит препятствие при прямой 1200 см.
Engine errors/ensure нет, DLL/source/module hashes стабильны. Методика:
Docs/NavigationQueries.md; runner Scripts/TestNavigationQueries.ps1;
отчёт logs/navigation-validation.json. Полный PIE/packaged и movement ticks
не запускались. Дальше по AI — sight/hearing perception stimuli и callbacks.


### Настоящие sight/hearing stimuli и callbacks — 2026-10-18

Добавлены DasPerception commandlet, Tests/Perception/probe.template.das и
Scripts/TestPerception.ps1. Editor build, свежая генерация/preflight Pawn/AI
пакета и 11 целевых проверок — PASS: sight acquire/loss, реальный ReportNoiseEvent,
Actor+FAIStimulus callback в DAS, current/known arrays, MaxAge, далёкий шум,
ReleaseObject и свежая session без stale counters. Native итог: sight_on=2,
sight_off=1, hearing_on=1. DLL/source/module hashes стабильны, Error/ensure нет.
Используется существующий snapshot callback backend; отдельного perception
dispatcher и новых externs нет. User-facing typed stimulus DSL не проверялся.
Методика и границы: Docs/Perception.md; отчёт logs/perception-validation.json.
Карты не сохранялись, полный PIE/packaged и сеть не запускались.
Далее: удобный typed stimulus/sense view, затем occlusion/team-policy и
дополнительные perception delegates/reload lifecycle по отдельным сценариям.


## Execution-role guards и native physics bridge — 2026-10-18

[ExecutionRolesAndAsyncBridge](ExecutionRolesAndAsyncBridge.md): все 579 UE
extern registration sites используют общую checked factory, включая scalar,
CMRES, low-level и функции без self/context. PhysicsPreSim запрещает все UE
externs до появления явного physics allowlist; builtin-математика DAS работает.
Frozen solver detection присутствует без debug-флагов и закрывает обход через
другой WorldContext/static no-self callback. Gameplay VM entry из этой фазы
отклоняется до компиляции/сессии; AOT raw cppName bypass закрыт noAot для UE externs.

Native-only UDasAsyncPhysicsBridge использует обычный component lifecycle,
weak owner/world, callback epoch и binding revision. Revoke/Deactivate немедленно
отзывают scope; штатный DeferRemoveAsyncPhysicsTick и последующая GT finalization
снимают регистрацию, не изменяя callback set во время обхода. Callback error,
nested scope, script/native EndPlay, unregister, Destroy, ReleaseObject, accepted
reload и world cleanup безопасно завершают binding; rejected reload сохраняет его.
Class migration/default-update консервативно отзывает bindings без auto-rebind.

Build, 102 целевых native/runtime checks (51 sync + 51 fixed async 1/60), source
coverage audit и ordinary whole Collision preflight — PASS. Engine errors/ensure
отсутствуют, обе DLL стабильны. Отчёт logs/execution-role-validation.json.
Collision остаётся 234/241 ready. Foundation/PIE/packaged не запускались;
коммит не создавался.

Публичный DAS AsyncPhysicsTick/DSL, isolated production physics VM и scoped
body/AddForce transport ещё не реализованы. Далее — dedicated precompiled VM
с bounded value IO, затем body token и первый explicitly allowed physics adapter.


### Dedicated Async Physics VM

Добавлена отдельная заранее подготовленная interpreter VM, native API
`CreateDasAsyncPhysicsVM`, value-only mailbox 16 finite double в каждую сторону,
scoped physics IO и транзакционная публикация выхода. Gameplay Context не
переиспользуется; stop/reload/Destroy проходят через native bridge. Проверки и
точные ограничения: [AsyncPhysicsVM](AsyncPhysicsVM.md),
`logs/physics-vm-validation.json`. Следующий слой: scoped body token/AddForce;
generic async handle transport остаётся запрещён. Без коммита.


### Типизированный AIStimulus и ue_event — 2026-10-18

Добавлен модуль ue_perception: AIStimulus с Sense, SuccessfullySensed, Expired,
Valid, Age, ExpirationAge, Strength, позициями и Tag; AISense=Invalid/Sight/Hearing/Other.
FAISenseID классифицируется native-кодом при capture, не по Tag и не по raw ID.
Точный callback adapter FAIStimulus → FDasAIStimulus создаёт owned value snapshot;
канонический модуль и layout проверяются существующим adapter value registry.
Пример переведён на Actor + PerceptionComponentRef и обычный
ue_event(source="Perception",name="OnTargetPerceptionUpdated"). Ручного binder,
@FRAME@ и строкового чтения stimulus в пользовательском обработчике больше нет.
Build и 15 целевых проверок — PASS: реальные sight/hearing и lifecycle, Invalid/Other,
отказ несовместимого struct и совместимость raw callback contract. Error/ensure нет,
DLL/backend/module/fixture hashes стабильны. Docs/Perception.md,
Tests/Perception/probe.template.das и logs/perception-validation.json.
Прямое наследование от AIPerceptionComponent и single-parameter projection не
расширялись. Blueprint/maps не создавались, полный PIE/packaged не запускались.
Далее — sight occlusion, team filters, дополнительные delegates и reload lifecycle.


### Первый пакет Animation / Audio / Niagara — 2026-10-18

Профиль AnimationAudioVFX: 4 libraries, 9 receivers, modules Engine/AnimGraphRuntime/
AudioMixer/Niagara. Из 580 кандидатов 514 generation_ready, 57 export_blocked,
9 generation_blocked; 30 construction guards. Весь staged DAS module компилируется.
Принят сокращённый режим: Editor build + один CalculateDirection runtime probe
(90 градусов для движения вправо) — PASS, без full suites и новых commandlets.
Error/ensure нет, DLL/profile/generator hashes стабильны. Playback/audio output/
Niagara simulation отдельно не запускались. Async callbacks и modulation object
sets остаются явными blockers. Документация Docs/AnimationAudioVFXPackage.md,
backlog Bindings/Packages/ue_api_animation_audio_vfx.summary.json, результат
logs/animation-audio-vfx-validation.json. Пакет staged, без публикации/коммита.
Следующий основной пакет — UMG / Input; сеть и структурные миграции остаются отложены.


### Scoped Async Physics body / AddForce

Реализованы native BindBody/ClearBody, opaque UEPhysicsBodyToken и
physics-only ue_physics_body/ue_physics_add_force. Разрешения ограничены восемью
собственными single unwelded телами Actor; лимит 64 AddForce за callback.
Силы commit в том же preSim после успешного скрипта, с проверкой weak lifecycle,
World/solver и generation. Контракт/ограничения и целевые проверки:
[ScopedPhysicsBody](ScopedPhysicsBody.md), `logs/scoped-body-validation.json`.
Обычный reflected raw async handle по-прежнему закрыт. Без коммита.


### Первый пакет UMG / Input — 2026-10-18

Профиль UMGInput: 5 libraries и 20 receivers, modules Engine/InputCore/SlateCore/
Slate/UMG/EnhancedInput. Из 517 кандидатов 320 generation_ready, 110 export_blocked,
87 generation_blocked. Весь staged модуль скомпилирован. В принятом сокращённом
режиме Editor build + один FKey(SpaceBar) → Key_IsKeyboardKey probe — PASS.
Error/ensure нет, DLL/profile/generator hashes стабильны. Widget rendering,
physical input, managed input suite, полный PIE/packaged не запускались.
77 opaque-struct и 10 delegate-parameter generation failures оставлены в backlog;
non-callable события и delegate signatures не объявляются ordinary call API.
Документация Docs/UMGInputPackage.md; отчёт logs/umg-input-validation.json;
backlog Bindings/Packages/ue_api_umg_input.summary.json. Новых C++ dispatchers нет.
Пакет staged, без публикации и коммита. Следующий функциональный шаг выбирается
из общих value/callback barriers под нужный сценарий; сеть и миграции отложены.

### Типизированный InputActionValue — 2026-10-18

Добавлен ue_input_value: owned InputActionValue, InputValueType и перегруженный
ue_input_value(bool/float/double/UEVector2D/UEVector), ue_input_type и getters
ue_input_bool/axis1d/axis2d/axis3d. Native layout не копируется в DAS; отдельный
numeric carrier преобразуется через UE constructors/Get. Exact-path transport
input_action_value_v1 подключён к общим exporter/generator/call frames.
Сгенерированы MakeInputActionValueOfType, BreakInputActionValue и ToString;
UMG/Input: 323 ready (+3), 110 export blocked, 84 generation blocked.
Editor build + whole-module compile + один короткий runtime main — PASS:
четыре типа, native Make/Break/ToString, нулевые лишние оси, array copy.
Исправлен порядок регистрации FFI: UCallHandle регистрируется до transport externs.
DLL/profile/generator/adapter source hashes стабильны, Error/ensure в принятом
прогоне нет. Полный PIE/packaged и physical input не запускались.
Existing bool/float/vector Enhanced Input callbacks сохранены; typed callback
payload и авторские поля этим шагом не расширялись. Документация
Docs/InputActionValue.md; пример input_action_values.das; отчёт
logs/umg-input-validation.json. Пакет staged; изменения без коммита.
Следующее: managed callback/delegate-параметры и UI-сценарий по потребности;
сеть и миграции остаются отложены.


### Async Physics registration DSL

Добавлена class annotation ue_async_physics(script, body_0..body_7), typed
recipe в authoring IR/JSON, автоматическая подготовка/регистрация отдельной VM
после lifecycle и gameplay mailbox. Physics dependencies участвуют в безопасном
runtime reload; Blueprint import выполняет compile-only physics preflight.
Пример и точные ограничения: [AsyncPhysicsDSL](AsyncPhysicsDSL.md).
Целевой отчёт: `logs/async-dsl-validation.json`. Без коммита.

Итоговый целевой прогон: **298 PASS** — 50 DSL, 80 scoped body, 66 VM и 102
execution-role/lifecycle checks, в sync и fixed async (1/60). Runtime/Editor DLL
SHA256 стабильны, engine errors/ensure отсутствуют. Проверены наследование,
ActorComponent owner binding, отказ до модификации Blueprint, сохранение старой
VM при ошибке reload, отсутствие повторного BeginPlay, ошибка BeginPlay и GC.
Foundation, полный PIE и packaged не запускались. Коммит не создавался.


### Collision package: обычный require и завершённый coverage report

Установлен `Content/Das/Scripts/ue_api_collision.das`: 234 ready из 241,
0 export/generation blockers и 7 явных policy exclusions. Сохранены исходные
export diagnostics, native-policy evidence и альтернативы; raw async handle
закрыт с указанием реализованного safe DSL. Прямые зависимости — ue_foundation
(public) и UE Engine; standalone импорт не требует World/Math. Совместный импорт
World/Math проверен с namespaces. Свежий report/inventory, 27 Python tests,
whole-module и installed standalone/combined runtime probes, overlap/settings,
три ожидаемых отказа — PASS. DLL hashes стабильны; engine errors/ensure нет.
Отчёт logs/collision-api-package-validation.json. Полные Foundation/PIE/packaged
и повторные simulation suites не запускались; без коммита.

### Typed Enhanced Input callbacks и UMG delegate-параметры — 2026-10-18

ue_enhanced_bind принимает InputActionValue в plain exported handlers и в
авторских ue_function методах. Добавлен общий native struct pack/unpack bridge;
direct numeric ABI ограничен Enhanced Input и соответствующими reload requirements.
Native proxy проверяет SourceAction и finite values, передаёт owned snapshot.
Старые bool/float/Vector2D/Vector handlers и managed recipes сохранены.
Native scalar input-only single-cast delegate параметры генерируются как
UEFunctionDelegate: checked UFunction endpoint, exact native signature identity,
общий call frame/DelegateHandler и weak semantics. Unbind остаётся явным native
контрактом; managed HUD/input cleanup сохраняется. Все 10 delegate blockers сняты.
UMG/Input опубликован: 333 ready, 110 export blocked, 74 generation blocked.
PlayableCharacter OnMove/OnLook переведены на InputActionValue; существующий HUD
использует managed кнопку и Tab → GameOnly/GameAndUI без второй реализации UI.
Editor build, generation/preflight, обновление example assets и один короткий
совместный -game/NullRHI сценарий — PASS: четыре native payload shapes и копии,
native navigation delegate/отказ неверной сигнатуры, real LocalPlayer injection
в авторский OnMove, HUD click, UI/game cursor, ReleaseObject снимает input handles.
DLL/source hashes стабильны; Error/ensure в принятом прогоне нет.
Полные PIE/packaged, физические устройства/rendering и reload suites не запускались.
Docs/InputUICallbacks.md; Scripts/TestInputUI.ps1; logs/input-ui-validation.json.
Изменения без коммита. Сеть и структурные миграции остаются отложены.


### Аудит 74 UMG failures и контракт input events

Свежая reflection + реальный generator + 9 native source files подтверждают
36 input events, 17 Geometry, 12 EventReply, 5 PaintContext, 4 animation handles.
InputEvent содержит borrowed EventPath; PointerEvent pressed keys — owned TSet.
Запланированы detached owned snapshots шести видов и 33 ordinary functions;
три navigation calls требуют отдельного live contract (analog меняет repeat state).
Это design-only этап: coverage 333/110/74 не менялся. 16 audit/contract unit tests,
fresh export и anchors/hash checks — PASS; build/PIE/packaged не запускались.
Docs/InputEventTransportContract.md, Docs/UMGInputBlockers.md; runner
Scripts/AuditUMGInput.ps1, logs/umg-input-audit-validation.json. Без коммита.


### Реализованы owned input events и 33 функции

Шесть opaque типов UEInputEvent/KeyEvent/AnalogInputEvent/CharacterEvent/
PointerEvent/NavigationEvent подключены к existing rooted snapshot table и
checked scalar call frames. Capture обнуляет собственный EventPath и сохраняет
native данные/owned pressed keys; generic reflected storage/defaults закрыты.
Exact allowlist и exporter/generator attestation открывают 33 ordinary функции.
Три navigation calls и DetectDragIfPressed теперь explicit execution-contract
отказы. UMG: 366 ready (+33), 114 export blocked, 37 generation blocked.
Пакет установлен и проверен обычным require. 38 native/runtime checks,
33 exercised functions, 3 compile-time token refusals и 122 Python checks — PASS.
Подтверждены реальные path widget release, globals/arrays/clone/GC, reload и
context cleanup; DLL hashes стабильны, engine errors/ensure нет. Без полного
Foundation/PIE/packaged и коммита. Docs/InputEvents.md; runner
Scripts/TestInputEvents.ps1; logs/input-events-validation.json.
Следующее: Geometry/EventReply и отдельный synchronous UserWidget bridge.

### Синхронный UserWidget input bridge — 2026-10-18

Добавлен Blueprintable UDasScriptUserWidget: 17 NativeOn-событий клавиатуры,
мыши и touch вызывают exported DAS handlers с точным типом input snapshot и
возвратом UEEventReply. ScriptFile/InputHandlers задаются в Widget Blueprint
или через ue_widget_bind_input для owned widget. Снимки сохраняются после
callback; reply commands действуют только в текущем синхронном вызове.
Handled/Unhandled, focus, capture/release, lock/unlock, cursor position и
detect/end drag формируют native FReply без раннего изменения Slate.
Default/stale reply, неверная сигнатура и panic дают native parent fallback;
NativeDestruct освобождает контекст. Общий reflected EventReply transport
не открыт, Geometry не передаётся в handler. Generated API coverage не менялся.
Editor build и один короткий SObjectWidget/NullApplication runtime-сценарий —
PASS: 17 проверок, чистый exit 0, без Error/ensure, source/DLL/BuildId стабильны.
Полные PIE/packaged и physical input не запускались. Изменения без коммита.
Docs/WidgetInputHandlers.md; Scripts/TestWidgetEvents.ps1;
logs/widget-events-validation.json; Tests/WidgetEvents/probe.das.

### Owned Geometry transport — 2026-10-18

`UEGeometry` хранит точную native-копию FGeometry в общей VM snapshot-таблице,
без widget/window/path pointers. Открыты 17 ordinary UMG functions: координаты,
векторы, scalar conversion, размеры, hit-test, Widget/viewport/player getters.
Finite/overflow/inverse, scalar allowlist, call-frame direction и gameplay role
проверяются до dispatch; outputs проверяются перед публикацией.
UMG: 383 ready (+17), 114 export blocked, 20 generation blocked. Остались
EventReply 11, PaintContext 5, WidgetAnimationHandle 4 и прежние scoped отказы.
Editor build, 25 native/runtime проверок, 3 token compile-time отказа,
8 Geometry generator tests — PASS. Связанные проверки: 38 input runtime,
9 input generator, 97 shared generator и 16 audit tests — PASS.
Подтверждены globals/arrays/clone/GC, widget release и hot reload; native
viewport branches проверены в headless fallback, видимое окно не проверялось.
Целевой окончательный прогон имеет стабильные DLL hashes/BuildId, без
Error/ensure; предыдущий прогон отклонён из-за параллельного изменения DLL.
Полные Foundation/PIE/packaged не запускались. Без коммита.
Docs/Geometry.md; Scripts/TestGeometry.ps1; logs/geometry-validation.json.

### Geometry в синхронных UserWidget handlers — 2026-10-18

Все 17 NativeOn-входов передают owned Geometry первым параметром и typed input
event вторым. Exact verifier требует `(UEGeometry, event):UEEventReply`;
прежняя одноаргументная форма отклоняется. Используются готовые CaptureDasGeometry
и общий rooted snapshot transport, без повторного чтения cached geometry.
Обновлены WidgetEvents/EventReply fixtures и примеры. Общая Editor-сборка — PASS;
один короткий SObjectWidget runtime-сценарий — PASS, 21 проверка, exit 0.
Проверены local/absolute, pointer coordinates, clone/array/GC и сохранность после
замены native Geometry, независимость callback revisions и неверные сигнатуры.
Source/DLL/BuildId стабильны; без Error/ensure. PIE/packaged не запускались.
Без коммита. Docs/WidgetInputHandlers.md; logs/widget-events-validation.json.

### AssetManager / Primary Assets — 2026-10-18

Добавлен managed API `ue_primary_assets`: value IDs, поиск зарегистрированных ID
по типу без загрузки, exists/path/object, async load с nullable object callback,
bundles, pending/cancel и unload. Primary-запросы отделены от обычных streamable
request names. Владение load state разделяется между VM; последняя lease вызывает
UnloadPrimaryAsset. VM release/reload/world cleanup снимают владение и callbacks.
Разные наборы bundles у одновременных владельцев и захват уже занятого native
load state отклоняются. Смена внешнего native state не вызывает его выгрузку.

Сборка Editor Development и один короткий DasPrimaryAssets-сценарий — PASS,
15 проверок: компиляция примера, настоящие временные .uasset и UI bundle,
no-load discovery, callbacks/data, GC, два владельца, last-owner unload,
null failure, cancel delivery и session release. Финальный прогон имеет
стабильные source/DLL hashes и engine BuildId, без engine Error/ensure.
Временные файлы и уникальный test type/mount удалены. Первые прогоны выявили
ошибки тестового pump/fixture и были повторены после исправления; прогоны с
параллельными изменениями исходников не приняты как финальная верификация.
Полные Foundation/PIE/packaged и cook/scan-настройки пользовательского проекта
не проверялись. Изменения без коммита.
Docs/PrimaryAssets.md; Scripts/TestPrimaryAssets.ps1;
logs/primary-assets-validation.json; Tests/PrimaryAssets/probe.das.

### Авторские DataAsset / PrimaryDataAsset — 2026-10-18

Добавлены DSL roots DataAsset и PrimaryDataAsset, native frame-hosts и data-only
классы. primary_asset_type хранится в сериализованном native свойстве и даёт ID
Type:InstanceName; по умолчанию тип равен имени авторского класса. AssetBundles
поддерживается в метаданных поля и общей группы свойств; native UE builder/PreSave
собирает soft dependencies и сохраняет registry bundle data. Новый редакторский
сервис и Das.DSL.Asset.Create создают dirty asset instances без перезаписи пакетов
и без автоматического изменения AssetManager/cook settings. Изменения деклараций
для новых asset roots отклоняются до defaults migration; body-only reimport доступен.

Editor Development build и один короткий DasAuthoredAssets-сценарий — PASS,
16 проверок: public DSL, два класса/экземпляра, Primary ID, отказ перезаписи и
изменения декларации, native bundle metadata, сохранение, AssetRegistry scan,
реальная async disk load с bundle, типизированные поля/метод, GC и unload.
Source/DLL hashes и engine BuildId стабильны, engine Error/ensure нет.
Удалены только собственные временные scripts и точные уникальные asset files.
Без полного Foundation/PIE/packaged и cross-process/cooked проверки, без коммита.
Docs/AuthoredDataAssets.md; Scripts/TestAuthoredAssets.ps1;
logs/authored-assets-validation.json; Tests/AuthoredDataAssets/probe.das.


### Диагностика authored assets перед cook/staging — 2026-10-18

Добавлены редакторская команда `Das.DSL.Asset.Check` и CI-commandlet
`DasAssetCheck` с JSON-отчётом. Проверяются сохранность класса и экземпляра,
DAS-источники и соответствие декларации, регистрация Primary Asset, bundles
и эффективное правило cook. Диагностика не сохраняет ассеты и не меняет настройки.
Предупреждения о недоказанном включении в cook не являются подтверждением доставки;
`packaged_verified` всегда false. См. Docs/AssetPreflight.md.

Editor Development build и один объединённый короткий runtime-сценарий — PASS,
21 проверка. Source/DLL hashes и engine BuildId стабильны. Контрольные случаи:
несохранённые изменения, отсутствующий источник, устаревшие bundles и NeverCook.
Полный Foundation/PIE/packaged не запускался. Изменения не закоммичены.
