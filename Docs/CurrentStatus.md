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
