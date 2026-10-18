# Систематическое расширение API

Основание — [воспроизводимый аудит](ApiCoverage.md), UE 5.8.3, профиль из шести
модулей. Дата снимка: 2026-10-18. Это план расширения после измерения покрытия,
а не утверждение о поддержке всей подсистемы.

## Что показал аудит

- 1 938 native-классов, 16 401 уникальное объявление. Наследование даёт
  110 125 вхождений на receiver-классах; оно не означает столько разных API.
- Native-фильтр принимает 4 546 уникальных объявлений. Из них 1 231 уже есть
  хотя бы на одном receiver, 3 315 ещё нигде не зарегистрированы. Основной резерв
  — системное подключение классов и генерация пакетов, а не новые ручные методы.
- У Function Libraries 1 762 функции проходят существующий ExportBindings,
  но не имеют статической обёртки в проверенных generated-модулях. Найдены
  три обёртки в области аудита с совпавшими сигнатурами. Ещё 455 функций
  экспортёр отклоняет; причины сохранены отдельно.
- 401 свойство имеет единственную причину отказа `native_accessor`.
  Нужен общий путь getter/setter, сохраняющий семантику UE. Чтение/запись по
  смещению вместо нативного accessor здесь недопустимы. Многие такие свойства
  уже можно менять отдельным отражённым Set-методом; это не 401 полностью
  отсутствующая игровая возможность.
- 84 метода имеют единственную причину `UnsafeDuringActorConstruction`.
  Проверить перенос ограничения на контекст вызова: запрет construction
  не должен автоматически означать запрет обычного gameplay. Снять флаг
  отбора без проверки runtime guards и тестов нельзя.
- Общие типовые барьеры: weak UObject references, интерфейсные ссылки,
  FColor/byte-поля внутри структур, фиксированные массивы, более широкие
  числовые типы, отдельные enum и непрозрачные структуры. Их вклад измеряется
  по уникальным объявлениям; несколько weak-полей одного HitResult не нужно
  складывать как независимые методы.
- У library backend есть 14 отказов из-за структур с нативными объектными
  ссылками. Это преимущественно InstancedStruct/UniversalObjectLocator API,
  а не доказательство, что достаточно разрешить один тип HitResult.

## Следующий этап: пакеты генерации

Ввести декларативный профиль пакета: модули UE, receiver-классы/правила их
выбора, канонические DAS-типы, Function Libraries, исключения и managed adapters.
Источником пакета должна быть подсистема, а не список классов, случайно
достигнутых из очередного игрового примера. Текущий project-based discovery
сохранить для пользовательского C++ и небольших проектов.

Существующий native registry можно пока оставлять единым: пакеты — это единицы
отбора, генерации и проверки, а не повод заводить ещё один runtime dispatcher.
Использовать действующие type descriptors, FProperty factory, call frames и GC
handles. Для обычных reflected методов отдельный C++ helper не нужен.

До публикации пакета проверять всю его генерацию: замыкание зависимостей,
коллизии имён enum/struct, единственность canonical views, идентификаторы,
default arguments и компиляцию получившегося DAS-модуля. Reflection eligibility
не гарантирует прохождение этой стадии. Текущий генератор, например, отклоняет
совпадения коротких имён native enum/struct; это отдельная пакетная проблема,
не измеренная счётчиком eligible.

## Порядок пакетов

| Пакет | Объём | Зачем этот порядок |
| --- | --- | --- |
| Core types / Math | Общие value-типы и поддерживаемый API KismetMathLibrary | В аудите 628 ещё не созданных обёрток проходят экспорт; сначала проверить целый пакет, не писать их вручную |
| World / Actor / Components | World, Actor, ActorComponent, Scene/PrimitiveComponent, базовые Controller/Movement; GameplayStatics и соответствующий System API | Общий фундамент остальных систем; ActorComponent сейчас имеет 22 eligible-члена без собственного receiver |
| Collision / Physics | Запросы, HitResult, overlap, физические компоненты | Сначала общая поддержка weak references и нужных struct/container shapes, затем весь выбранный reflected API |
| Pawn / Character / AI / Navigation | Контроллеры, движение, навигация, perception | Автоматический API плюс явные managed-контракты MoveTo и асинхронных событий |
| Animation / Audio / Niagara | Компоненты, assets, параметры, библиотеки | Автоматические вызовы отделены от lifetime-sensitive playback/callback adapters |
| UMG / Input | Widget-классы, события, ввод, библиотеки | Использовать общий accessor/backend; callbacks и ownership сохраняют managed lifecycle |

В GameplayStatics сейчас 120 eligible-функций без обёрток и две имеющиеся;
в KismetSystemLibrary — 188 eligible без обёрток. Это кандидаты на пакетную
генерацию, не обещание добавить все сразу без анализа зависимостей и поведения.

Сначала для Core/World провести общий accessor/construction-policy этап; типовые
расширения добавлять один раз в общие механизмы, затем автоматически измерять
их эффект на всех пакетах. Не смешивать невключённые классы, ошибки генерации,
неподдержанные сигнатуры и намеренно закрытый API в один backlog.

## Критерий завершения пакета

Для каждого члена есть одно из состояний: generated, проверяемое исключение
политики, documented adapter или конкретный технический blocker. Выводится
diff покрытия относительно предыдущего снимка, отдельно по уникальным
объявлениям и inherited receiver surface. Причины могут пересекаться.

На итерации нужны проверки генератора и целевые runtime-сценарии новых форм
сигнатур. Полные Foundation/PIE/packaged — на границах крупного этапа либо при
изменении общей ABI, а не после подключения каждого класса. Cooked/Shipping,
сеть и thread affinity имеют самостоятельные критерии; generated не означает
проверено во всех режимах.

## За пределами текущего измерения

Неотражённые C++ методы, arbitrary virtual overrides, Slate, отключённые плагины
и модули вне профиля требуют отдельного источника описаний/адаптеров и аудита.
SaveGame/AssetManager можно оформить следующими пакетами существующих engine
классов и библиотек. GAS, StateTree, PCG и сеть нельзя объявлять поддержанными
по этому отчёту. Явные instance-обёртки старого ExportBindings и managed helpers
не включены в числитель native registry; они не исчезают при таком подсчёте.

## Выполнено: первый проход Math — 2026-10-18

[Пакетный конвейер](ApiPackages.md) реализован для Function Libraries:
профиль → экспорт → dependency closure → индивидуальная и общая генерация
→ компиляция всего DAS-модуля → целевой runtime probe → публикация.
Math: 619 wrappers проверены компиляцией; scalar, struct, ref/out и array
сценарии проверены выполнением. Поимённый список 122 блокеров сохранён.
Это ещё не закрывает весь Core/Math: canonical UEVector/UERotator/UETransform,
адаптеры непрозрачных spring-state типов и instance packages остаются в плане.
Текущий профиль явно использует generated transport и explicit arguments.

## Canonical Math — 2026-10-18

Пакет Math теперь использует foundation UEVector2D, UEVector, UERotator,
UETransform вместо копий этих типов. Общий frame resolver поддерживает
проверенные aggregate paths, включая array elements. Конкретные leaf assignments
не обходят проверку заполнения других элементов. Структурная сигнатура IR
сохраняется и проверяется до canonical mapping; дрейф native layout — ошибка
генерации. Исходный режим существующих модулей не изменён.
Покрытие Math осталось 619/741, блокеры 112 export + 10 generation.
Далее: пакет World/Actor/Components, общий accessor/construction-policy этап;
расширение canonical views для остальных типов — по измеренным потребностям.
Целевые проверки: [ApiPackages](ApiPackages.md), TestCanonicalMath.ps1.

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
Staged only, без full Foundation/PIE/packaged и коммита.


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


### Managed Drag & Drop — 2026-10-18

Открыт generated DetectDragIfPressed через scoped EventReply и provenance
текущего native pointer callback. Добавлен ue_drag_drop: одна новая native
UDasDragDropOperation в OnDragDetected, weak UObjectHandle, payload/visual/tag/
pivot/offset, synchronous enter/over/leave/drop/cancel на DasScriptUserWidget.
FDragDropEvent shared operation и EventPath не передаются в VM: Geometry и
pointer-часть — owned snapshots, operation передаётся отдельно.
Фабрика и чтение данных ограничены текущим drag callback; foreign операции,
неверные world/user/pointer, старый pointer snapshot и повторный factory вызов
отклоняются. Drop/cancel, source/participating target NativeDestruct и committed
hot reload отзывают операции, очищают payload/visual/delegates. Rejected reload
оставляет рабочую VM/операцию. Активный Slate drag отменяется на следующем
core ticker только при совпадении user и native operation. VM pointers/DAS
closures в native operation/ticker не сохраняются. Очередь обычных callbacks
не дренируется внутри synchronous drag scope.
Editor build, whole-module preflight, 47 SObjectWidget/FUMGDragDropOp runtime
checks, 8 новых generator checks — PASS. Проверены mouse/touch detection,
current-event provenance, GC, все шесть callbacks, native Slate cancellation,
защита чужого drag от устаревшей отмены, ошибки/parent fallback, source/target
Destroy/reconstruction и accepted/rejected reload. Engine Error/ensure нет.
Регрессии: EventReply 33, OnPaint 28; Python: shared 97, EventReply 8,
input events 9, Paint 8, audit 16 — PASS. Fresh coverage и guard audit — PASS;
Source/DLL/BuildId проверены после прогонов.
UMG/Input: 404 ready (+1), 113 export blocked, 0 generation blocked.
Остаются три live navigation queries; raw internal CreateDragDropOperation
не открыт, используется managed factory. Произвольные native/Blueprint operation
subclasses и авторские operation classes этим шагом не поддержаны.
Без physical input/rendering, full Foundation/PIE/packaged и коммита.
Docs/DragDrop.md; Scripts/TestDragDrop.ps1; Tests/DragDrop/probe.das;
logs/drag-validation.json; Bindings/DragDrop.contract.json.


### Live input navigation — 2026-10-18

Открыты Key_GetNavigationActionFromKey, Key_GetNavigationDirectionFromKey и
Key_GetNavigationDirectionFromAnalog через exact live_input_navigation_v1.
Нужны текущий native input token, callback epoch, живой widget/world и исходный
зарегистрированный Slate user (weak identity). Старые snapshots остаются
читаемыми, но не дают authority для live query. Frames освобождаются на выходе;
NativeDestruct отзывает input capability до последующих запросов.
Результат кэшируется один раз на функцию/callback. Analog query изменяет
native repeat state и потребляет input: unhandled/error/Destroy после запроса
возвращают bare handled, исключая повторную обработку в UE. Reply-команды
ошибочного callback не публикуются; native repeat state не откатывается.
Без query и при отказе до query сохраняется обычный parent fallback.
Queued callbacks не дренируются внутри synchronous input scope.
Editor build, whole-module preflight, 24 SObjectWidget native checks и 8 новых
generator tests — PASS. Проверены персональный custom NavigationConfig
виртуального Slate user, caching, repeat suppression/time/neutral reset,
stale snapshots/frames, неизвестный user, panic, Destroy/reconstruction, reload.
Регрессии: EventReply 33, Drag 47, InputEvents 38; shared generator 97,
input-event generator 9, drag generator 8, audit 16 — PASS. Fresh reflection/
coverage и execution-role guard audit — PASS. Source/DLL/BuildId стабильны,
engine Error/ensure нет. UMG/Input: 407 ready (+3), 110 export blocked,
0 generation blocked; отдельного navigation export backlog больше нет.
Остальные policy/event/delegate/soft-reference export отказы остаются явными.
Без physical input, visible focus traversal, full Foundation/PIE/packaged и
коммита. Docs/InputNavigation.md; Tests/InputNavigation/probe.das;
Scripts/TestInputNavigation.ps1; logs/navigation-validation.json;
Bindings/InputNavigation.contract.json.


## UMG: получение и потеря фокуса — 2026-10-18

Добавлены OnFocusReceived(UEGeometry, UEFocusEvent):UEEventReply и
OnFocusLost(UEFocusEvent):void через существующий ue_widget_bind_input.
Owned FocusEvent snapshot предоставляет uint user и имя EFocusCause, без
WidgetPath/SlateUser authority. Используется общая bounded/rooted snapshot table.
Received использует scoped reply; Lost не получает reply authority. NativeDestruct
освобождает VM, Destroy внутри callback отзывает результат, reentry отклоняется.
Hot reload проверяет использованные сигнатуры и сохраняет рабочую VM при отказе.

Editor build, 35 focus checks, 33 related EventReply checks, 8 generator checks
и execution-role guards — PASS; source/DLL/BuildId стабильны, Error/ensure нет.
Generated API coverage этим callback bridge не изменяется. Focus-path callbacks
и physical focus traversal остаются вне этого этапа. Только целевые проверки,
без полного Foundation/PIE/packaged и без коммита.
Docs/FocusEvents.md; Bindings/FocusEvents.contract.json;
Tests/FocusEvents/probe.das; Scripts/TestFocusEvents.ps1;
logs/focus-validation.json.


## Поимённая классификация 110 UMG export refusals — 2026-10-18

[Аудит оставшихся отказов](UMGExportBlockers.md): 60 policy exclusions,
32 декларации с существующими ограниченными адаптерами, 18 technical blockers.
Ни один отказ не скрыт: raw status/reason и published coverage сохраняются.
32 адаптированных объявления не добавлены к 407 generated wrappers.
25 delegate signatures классифицированы как определения типов, а не готовые
подписки. Из 60 исключений: 21 deprecated, 25 signatures, 8 protected legacy
input, 3 non-callable accessor entrypoints, 2 editor helpers, 1 engine lifecycle
helper. Общий accessor backend не доказывает проверенность каждого UMG свойства.

18 blockers: 5 widget lifecycle, 1 interactability query override, 2 focus-path
notifications, 3 pointer notifications, 1 motion event, 2 animation callbacks,
2 soft brush setters и 2 GetBoundActionValue entrypoints одной polling-возможности.
NativeDestruct cleanup не равен пользовательскому Destruct callback; animation
lease invalidation не равен пользовательскому OnAnimationFinished.
Следующий рекомендуемый этап — typed soft brush adapter; затем grouped pointer/
focus-path notifications и общий widget lifecycle/override bridge.

20 mutation/unit checks, свежий UE reflection export и реальный повтор package
generator — PASS. Проверены native/source anchors и соответствие adapters
конкретным callback/factory/conversion объявлениям. Source/native/DLL/BuildId
стабильны, engine Error/ensure нет. Coverage: 407 ready / 110 export blocked /
0 generation blocked. Это статический аудит существующей реализации; новые
runtime-сценарии, build, Foundation/PIE/packaged не запускались. Без коммита.
Bindings/UMGExportAudit.rules.json;
Bindings/Packages/ue_api_umg_input.export-audit.json;
Scripts/AuditUMGExports.ps1; logs/umg-export-audit-validation.json.
