# План DasForUE2 после аудита NimForUE

Дата: 2026-10-05. Проверен исходный код NimForUE devel, commit
`58140f4842a91a0187c17ce950e07181ba455862`. Это аудит исходников и документации,
не подтверждение работоспособности NimForUE на UE 5.8.3. В этой ревизии заявлена
поддержка UE 5.7. Старый DasForUE не используется как основа.


## Current priority — 2026-10-08

User decision: defer broader structural/data migration logic and continue binding
functionality. Existing supported migration policies remain; unsupported transitions
must reject explicitly. Deferred: outer nested/container definition changes,
primitive/set/map state/key migration, nonempty defaults, split/custom graph repair,
DataTables, multiple roots, base/components and active-PIE structural reload.

Active sequence: container DSL and typed interop (primitive arrays, set/map and
standalone signatures), then out/ref and interfaces; lifecycle/production/network
work follows the relevant original stages. Universal migration is not a prerequisite.
See DSLPrimitiveArrays.md for the current feature boundary.

## Что добавлено по результатам аудита

Первоначальная архитектура охватывала главные направления, но не весь функционал.
Ниже требования к новому проекту; наличие строки не означает готовую реализацию.
Синтаксис пользовательского DSL выбирает пользователь позже.

| Возможность NimForUE / свидетельство | Требование DasForUE2 | Этап |
| --- | --- | --- |
| Декларации UClass, UStruct, UEnum, UFunction, UProperty; макросы и спецификаторы | Единая модель типов/метаданных, DSL через синтаксические макросы; раздельные editor/runtime backends | 2, 6 |
| UInterface; документация отмечает незавершённую статическую проверку implements | Проверка сигнатур при компиляции DSL, корректный вызов через UE interface dispatch | 3, 6 |
| Blueprint-события и функции | Вызовы в обе стороны; NativeEvent/ImplementableEvent, pure/static functions, out/ref/return, world context | 3, 5, 6 |
| Actor/Component, конструкторы, default subobjects и native overrides | Родительский lifecycle, компоненты и attachment, CDO/defaults; native virtual только через явные адаптеры/генерацию C++ | 5, 6 |
| Контейнеры, FText, soft object references, native/shared pointers | Владение, GC handles, weak/soft/class references, TSubclassOf, TArray/TMap/TSet, локализованный текст; запрет опасного копирования layout | 2, 3 |
| Native и script delegates, single/multicast, handles | Отдельные адаптеры, подписки с владельцем, снятие при EndPlay, GC и reload | 3, 5 |
| Timers; async example использует Nim asyncdispatch | UE timers/ticker, отмена задач, возобновление на game thread; latent actions как самостоятельная интеграция | 5 |
| netexample: replicated fields, RepNotify, conditions, Server/Client/Multicast RPC | Сетевая схема в IR, проверка флагов, authority, сериализация; dedicated server + два клиента | 6, 7 |
| ueemit, FNimReload, EditorUtils: reflection emission и reinstancing | Hot reload тел функций и структур отдельно; BP descendants, CDO, instances, ссылки, компоненты, pins/nodes, открытые assets | 1b, 6 |
| В hot reload metadata-only изменения не учитываются; часть миграции в EditorUtils закомментирована | Явный diff метаданных, изменения enums/structs/DataTables, диагностика несовместимости; проверяемая миграция с сохранением предыдущей рабочей версии | 6 |
| Reflection codegen, module/dependency rules, limited header parsing | Генератор из UE reflection + UHT exporter + ручные описания, фильтры модулей, dependency graph/cycles, воспроизводимый manifest | 4 |
| CLI: build, codegen, project/plugin generation, tests; editor notifications | CLI и commandlets, запуск тестов без UI, compile/reload status, переход к строке ошибки | 1, 4, 7 |
| Plugin generation / native packaging code | Cook/staging/Pak, Shipping, проверка сборки без редактора и исходников; AOT как отдельный проверяемый режим | 1, 7 |
| GAS/GameplayTags: ручные helpers | Дополнительный пакет GAS: attributes, effects, abilities, delegates и replication | 8 |
| UMG/Slate helpers; MVVM/FieldNotify | UMG, биндинги viewmodel и FieldNotify; Slate/editor UI — отдельный более сложный пакет | 8 |
| PCG example реализует native element/settings | PCG adapters и выполнение daScript kernel с явными правилами потоков | 8 |
| GameplayMessageRouter helpers | Typed messages, listener lifetime и unregister при reload | 8 |
| StateTree файл содержит преимущественно imports/небольшие helpers | Исследовать полноценные task/evaluator/condition adapters; не считать готовой возможностью NimForUE | 8 |
| VM interop / ue-borrow примеры, raw C++ virtual | Экспериментальные ориентиры; подмена чужих функций и произвольный vtable patching не входят в обязательное ядро | исследование |

## Последовательность реализации и критерии готовности

### 1. Основание

Новый независимый plugin, закреплённая версия daScript, изолированный test project,
проверка interpreter/native call, AOT без fallback, ошибок и владения строками.
Одни и те же тесты в editor commandlet и packaged Development executable.
Граница C++ exceptions локальна runtime-модулю; ошибки возвращаются UE как результат.
Минимальный syntax macro выдаёт описание типа, не фиксируя будущий DSL.

### 1b. Ранний эксперимент structural hot reload

До массового генератора доказать возможность: Actor-derived reflected class,
Blueprint child, размещённый instance; изменение поля/метода без перезапуска;
сохранение overrides/defaults и ссылок; обновление Blueprint; save/reopen.
Сначала вне PIE, затем PIE. Наличие macro IR само по себе не считается hot reload.
При неудаче эксперимента пересмотреть механизм backend, сохранив независимую IR.

### 2. Безопасная модель типов и объектов

Нормализованная IR: стабильные type/member IDs, версии схем, flags/metadata,
ownership, nullability, thread constraints, native/reflected вызовы. GC-safe handles,
weak/soft refs, generation checks против устаревших context/object references.
Инициализация/уничтожение UE property values через UE API. Тесты GC и EndPlay.

### 3. Двусторонний вызов UE ↔ daScript

ProcessEvent/property access и curated native adapters; arrays/maps/sets, structs,
out/ref/return, interfaces, delegates, UObject/class references, FText.
Диагностика неподдерживаемых сигнатур до исполнения. Ошибки не выходят C++ exception
через обычный код движка; UE check/fatal/access violation не обещаем восстановить.

### 4. Генератор и инструменты

UE reflection/UHT export/ручные adapters → IR → daScript declarations + native glue.
Стабильный порядок вывода, инкрементальность, manifest версий и зависимостей,
понятные причины пропуска API; отчёт о покрытии. CLI/commandlets и CI smoke tests.
Разбирать весь Unreal C++ обычным генератором daScript не требуется.

### 5. Сквозной игровой пример и logic reload

Actor + Component, asset reference, Blueprint event, timer, delegate, input;
editor и packaged game. Per-world contexts, несколько PIE worlds, teardown,
отмена async/timers, reload функций с сохранением согласованного состояния.
Ошибка компиляции оставляет последнюю рабочую версию активной.

### 6. Полный DSL и structural reload

Синтаксис выбирается отдельно. Macro AST → проверенная IR; classes/structs/enums,
interfaces, properties/functions/delegates, defaults/components, Blueprint metadata,
RPC/replication. Dynamic editor backend и generated C++/UHT backend для cook.
Проверить одинаковую семантику обоих backends, не полагаться на layout daScript.

Матрица reload: add/remove/rename member, metadata-only change, type/default change,
base class change, enum values, struct/DataTable rows, components, Blueprint pins,
references, save/reopen. Для каждой операции: live migration, controlled refusal
или необходимый restart. Не обещать универсальную атомарность UE reinstancing:
сначала валидация candidate, затем безопасная точка переключения, rollback только
для явно поддержанных переходов. Multi-user/network schema mismatch обнаруживается.

### 7. Production и сеть

Shipping/cook/AOT, сохранение type identity и metadata нужных runtime API,
dedicated server + clients, RepNotify/RPC, совместимость схем сохранений.
Профилировать reflection calls против native adapters, compilation/reload time,
память и GC. Проверить чистую машину без дерева исходников; лицензии зависимости.
Платформы за пределами Win64 — отдельные проверяемые задачи.

### 8. Дополнительные интеграции

GAS/GameplayTags, UMG/MVVM/FieldNotify, GameplayMessageRouter; затем PCG,
StateTree, Slate/editor extensions по потребности реального проекта.
Эти пакеты не должны блокировать работающее ядро.

## Полезные дополнения сверх наблюдаемого функционала NimForUE

- Отчёт покрытия API: bound / adapter needed / unsupported и причина.
- Source mapping ошибок сгенерированного DSL/glue обратно к исходному daScript.
- Статическая проверка lifetime/thread/replication contracts насколько позволяет IR.
- Тесты нескольких PIE worlds, GC, reload подписок/задач и последовательных отказов.
- Лимиты времени/памяти для скриптов и compiler macros как отдельное исследование;
  это не обещание безопасного исполнения недоверенного кода.
- Версионируемые миграции сериализованных типов и обнаружение несовместимых схем.
- LSP integration и profiler markers; debugger integration исследовать отдельно.

## Источники и ограничения аудита

- https://github.com/jmgomez/NimForUE/tree/58140f4842a91a0187c17ce950e07181ba455862
- `src/nimforue/codegen/{ueemit,models,genreflectiondata,genreflectiondatav2,headerparser}.nim`
- `Source/NimForUEEditor/Public/FNimReload.cpp`, `Private/EditorUtils.cpp`
- `src/nimforue/game/nue/{gameplayabilities,modelviewviewmodel,statetree,gameplaymessageruntime,ui}.nim`
- `src/nimforue/unreal/core/delegates.nim` и соответствующие container/reference bindings
- `src/nimforue/examples/{netexample,pcgexample,asyncexample,ueborrowexample,vminteroppocexample}.nim`
- `src/nue.nim`, `src/buildscripts/plugingenerator.nim`
- https://nimforue.pages.dev/concepts/hot-reload/
- https://nimforue.pages.dev/concepts/uinterface/

Некоторые пути выше сгруппированы для краткости; источником истины служит закреплённое
дерево репозитория. Пример/ручной adapter не равен полному покрытию подсистемы.

## Статус первого этапа — 2026-10-05

Реализован в C:\src\DasForUE2. По 12 проверок успешно выполнены в editor commandlet
и packaged Win64 Development game; оба процесса завершились с кодом 0.
Проверены native call, отдельные контексты, диагностика ошибок, AOT без fallback,
макрос описания типа, отклонение неподдерживаемого поля, UTF-8 и восстановление
после ошибки. Packaged game загружает скрипты из своего RemappedPlugins/Pak.
Этап 1b теперь проверен через Blueprint backend; см. актуальный результат ниже.

## Результат этапа 1b — 2026-10-05

Реализован экспериментальный Blueprint backend: macro declaration → UBlueprint →
UBlueprintGeneratedClass, native Actor adapter и script-backed метод фиксированной
сигнатуры. Пройдены 26 проверок обновления/сохранения и 12 проверок загрузки assets
в новом процессе. Также пройден настоящий PIE automation test: 15 контрольных
точек, разные значения editor/play worlds, reinstancing, ссылки, GC и EndPIE.
Unreal Automation: 1 successful test, 0 errors/warnings.

Это доказательство выбранного механизма, не готовый DSL и не прямой native UClass
emitter. Добавления int/float поддержаны; удаления/смена типов отклоняются.
Self/UObject binding, полная функция/metadata IR, lifecycle callbacks, подписки,
несколько клиентов и cook parity остаются следующими задачами. Подробности и
воспроизводимые команды: Docs/ReloadProbe.md.

## Следующий срез этапов 2–3 — 2026-10-05

Выделены FDasClassSchema (типизированные scalar-поля/defaults и один int→int метод)
и FDasBlueprintService. Runtime получил слабый self, проверяемый доступ к int32/float32
свойствам и отказ для истёкшего объекта, неверного типа или read-only записи.
Проверены GC, Actor::Destroy, reload/reopen и раздельное состояние editor/PIE.
Foundation: 21 PASS в editor и packaged game; PIE: 1 success без ошибок/предупреждений.
Полная модель типов/метаданных, persistent contexts, произвольные object handles,
UFunction-вызовы и callbacks/подписки остаются в плане. Подробности: Docs/Interop.md.


## Постоянные контексты и hot reload логики — 2026-10-05

Реализованы отдельные сессии для owner/script, повторное использование Program/Context,
проверка изменений исходника и импортов перед вызовом, подготовка и проверка кандидата,
сохранение старой версии при ошибке, перенос именованного int32-state и очистка через
Destroyed/EndPlay, world cleanup и GC. RunFile сохраняет прежнюю разовую семантику.
Проверки: 44 PASS в editor и packaged Win64 Development; настоящий PIE с двумя
счётчиками и fallback; structural reload/reopen. Подробности: Docs/PersistentContexts.md.

Следующий функциональный срез: BeginPlay/Tick/EndPlay callbacks, затем delegates и
подписки с очисткой при reload. Ещё нужны общий кеш Program, очередь обновлений,
расширение схемы переносимого состояния и его перенос между reinstanced UObject.
Полный пользовательский DSL остаётся отдельным этапом; его синтаксис не фиксируем.


## Lifecycle callbacks — 2026-10-05

ADasProbeActor подключает LifecycleScript и доставляет необязательные экспортированные
begin_play(), tick(float), end_play(int). Сигнатуры проверяются до принятия новой версии,
BeginPlay не повторяется при logic reload, EndPlay выполняется на рабочей версии до
освобождения контекста. Доступ к self разрешён во время EndPlay уничтожаемого Actor.
Неверные кандидаты оставляют предыдущие callbacks; LastDasError хранит диагностику.

Проверки: 66 PASS в editor и packaged Win64 Development; отдельный настоящий PIE
с двумя Actor, автоматическими Tick, reload, Destroy и EndPIE — 17 контрольных точек,
1 success без предупреждений. Прежние structural reload/reopen/PIE тоже прошли.
Подробности и примеры: Docs/Lifecycle.md. DSL-синтаксис не выбран и не фиксируется.

Следующий функциональный срез: delegates/подписки с владением, снятием при reload
и EndPlay и защитой от вызова старого контекста. Lifecycle при замене UObject через
structural reinstancing, общий Program cache и очередь изменений остаются открытыми.


## Управляемые delegate-подписки — 2026-10-05

Первый срез: BlueprintAssignable dynamic multicast с одним int32, источники self/peer,
экспортированный void(int) обработчик. bind_events() собирает и валидирует набор
подписок до принятия кандидата; успешный reload отзывает старые IDs и заменяет
привязки, неуспешный сохраняет их. UObject-прокси хранит ID вместо VM-указателей.
Владение слабое; EndPlay/Destroy/GC/world cleanup/shutdown снимают только свои
привязки. Вложенные события ставятся в ограниченную очередь; уничтожение owner
во время VM-вызова откладывает освобождение контекста до безопасного выхода.

Проверки: 85 PASS в editor и packaged Win64 Development, отдельный PIE delegates
с 16 контрольными точками и без warnings; lifecycle/structural reload/reopen регрессии
пройдены. Пример delegate_actor.das; подробности и ограничения: Docs/Delegates.md.

Следующий срез: типизированные UObject handles и UFunction-вызовы для выбора
произвольных источников, затем расширение сигнатур delegates. Остаются single-cast,
worker-thread delivery, лямбды, runtime unsubscribe, structural rebind и безопасная
отложенная доставка EndPlay при уничтожении Actor из вложенного события внутри VM.
Синтаксис пользовательского DSL по-прежнему не фиксируется.


## UObject handles и UFunction adapters — 2026-10-05

Добавлен отдельный непрозрачный тип UObjectHandle в daScript, слабый реестр на
контекст, null/valid/is-a/cast, чтение сильных объектных UPROPERTY и int32-свойств.
Класс проверяется в runtime; сгенерированных daScript-типов для каждого UCLASS пока нет.
Подписки могут получать произвольный доступный объектный источник через handle.

ProcessEvent adapters поддерживают int32→int32, float32→float32, int32→void,
()→object и object→void. Проверяются BlueprintCallable, signature, out/ref и класс
аргумента; параметры используют FStructOnScope, вызов временно удерживает объекты.
RPC/static и UFunction-вызовы из bind_events запрещены. После Destroy/GC старые
handles отклоняются. При reload они заново получаются в новом контексте.

Проверки: 103 PASS editor и packaged Win64; настоящий PIE с Blueprint Echo,
native EmitDasSignal, engine SetOwner/GetOwner и Destroy/GC — 8 контрольных точек,
1 success без warnings. Делегаты/lifecycle/structural reload/reopen регрессии прошли.
Подробности: Docs/Objects.md. Пользовательский DSL-синтаксис по-прежнему не фиксируем.

Следующий срез — типизированная function/signature IR и общий marshalling для
нескольких аргументов, bool, строк и структур. AOT новых bindings, weak/soft/interface
references, полноценная class-specific генерация и structural handle migration
остаются отдельными задачами; текущие пять adapters не заменяют весь UE API.


## Общий UFunction marshalling — 2026-10-05

Выполнен первый общий срез: непрозрачный UCallHandle, IR скалярных путей/типов/
направлений, несколько аргументов, int32/float/double/bool/FString, вложенные
UScriptStruct, const-reference inputs и чтение struct return. Полная поддерживаемая
сигнатура проверяется до подготовки вызова, заполненность inputs — до ProcessEvent.
Параметры живут в FStructOnScope; строка результата копируется в daScript.
Frames одноразовые, с ограничением количества и очисткой после callback/ошибки.
Подробности, ограничения и результаты: Docs/Marshalling.md.

Следующий срез: объединить object arguments с этим механизмом, добавить out/inout
и enum/FName, затем контейнеры и генератор типизированных daScript-обёрток поверх
IR. Необходимы полноценный IR с class/struct identity и метаданными, политика
экспорта UE API и AOT. Текущий IR — внутреннее описание скалярных листьев конкретной
UFunction, не окончательный формат схемы. DSL для авторских классов и его hot reload
остаются в плане; выбор синтаксиса остаётся за пользователем.


## Объектные параметры и out/inout — 2026-10-05

Общий marshalling расширен до UObject input/return/out/inout и изменяемых
скалярных/строковых/структурных ссылок. IR различает вход и выход независимо;
все inout inputs обязательны, чистые out задавать нельзя. Frame удерживает
объектные параметры/результаты, проверяет класс и истёкшие handles и освобождает
pins явно либо при выходе из VM. Возвращаемые UObjectHandle остаются слабыми.
Детали и верификация — Docs/InOut.md. Старые call adapters остаются совместимыми.

Следующий срез: enum/FName, затем генерация типизированных daScript-обёрток по IR.
Рекурсивное владение объектными полями структур, контейнеры, полный IR с метаданными
и AOT остаются отдельными задачами. DSL авторских классов и его hot reload остаются
в плане; выбор синтаксиса за пользователем.


## Enum и FName — 2026-10-05

Общий marshalling поддерживает FEnumProperty, enum-backed FByteProperty и FName,
включая поля структур, return и out/inout. Enum задаётся путём UEnum и символом;
IR хранит TypePath. Чужие типы, неизвестные элементы и зарезервированные _MAX
отклоняются. Name использует правила UE для None, регистра и числовых суффиксов.
Строки результатов принадлежат daScript, а ввод проверяется до создания FName.
Подробности, границы и проверки — Docs/EnumsNames.md.

Следующий срез: экспорт IR и генерация типизированных daScript-обёрток для явно
выбранного UE API. Полная политика метаданных, enum declarations, контейнеры,
GC-aware объектные поля структур и AOT ещё впереди. Синтаксис DSL классов не выбран.


## Экспорт IR и типизированные обёртки — 2026-10-06

Добавлен DasExport commandlet, JSON IR v1 и Python-генератор для 11 явно выбранных
функций. Экспорт использует общую валидацию reflection с runtime. Обёртки получают
типизированные параметры, отдельные enum/UEName и структуры нескольких результатов;
они самостоятельно выполняют маршаллинг и проверку актуальности сигнатуры.
GenerateBindings.ps1 -Check проверяет свежесть IR и кода. Подробности и результаты:
Docs/GeneratedBindings.md. DSL авторских классов этим этапом не определяется.

Следующий срез: именованные struct-типы вместо развёрнутых скалярных листьев,
class-specific handles и политика экспорта метаданных. Затем расширение охвата API
и AOT. Контейнеры, объектные поля структур и authoring DSL/hot reload остаются в плане.


## Именованные структуры — 2026-10-06

IR v2 сохраняет definitions структур и параметры верхнего уровня, а также листья
для сверки с runtime. Генератор выпускает уникальные daScript struct-типы с
вложенностью, enum/FName и строками. Входы/выходы передаются целиком; inout возвращает
копию в result. FVector проверяется через реальные Set/GetActorRelativeScale3D.
Список API расширен до 14 функций. Подробности и миграция: Docs/StructBindings.md.

Следующий срез: class-specific UObject-типы, типизированные ссылки и их преобразования.
Default values, policy метаданных, контейнеры, GC-aware объектные поля структур,
AOT и DSL пользовательских классов с hot reload остаются отдельными этапами.


## Типизированные ссылки UCLASS — 2026-10-06

IR v3 хранит классы и их предков. Обёртки принимают конкретные nominal class-типы
в receiver, input/return/out/inout; явные casts сохраняют проверку класса и слабое
владение. Null допускается для параметров/результатов, истёкшие ссылки отклоняются.
Выбор расширен до 18 функций; подробности: Docs/ClassBindings.md.

Следующий срез: политика экспорта reflected API и метаданных, с явными правилами
включения/исключения и диагностикой неподдерживаемых сигнатур. Затем defaults,
контейнеры, GC-aware объектные поля структур и AOT. DSL пользовательских классов
через синтаксические макросы и его hot reload остаются отдельными этапами;
синтаксис выбирает пользователь.


## Политика API и metadata — 2026-10-06

Реализованы class_rules с include/exclude и явным include_inherited, обязательные
и необязательные функции, фильтры deprecated/internal и диагностика пропусков.
IR v3 дополнен declaring_class, pure/const, allowlist metadata и default_text.
Экспорт требует Editor; metadata не меняет исполнение обёрток. Существующий выбор
сохраняет 18 функций. Подробности: Docs/BindingPolicy.md.

Следующий срез: типизированные значения по умолчанию. Затем расширение типов,
контейнеры, объектные поля структур с GC, AOT и пользовательский DSL с hot reload.
Синтаксис DSL по-прежнему не выбран и остаётся решением пользователя.


## Типизированные defaults — 2026-10-06

Генератор преобразует default_text в проверенные значения для int/float/double,
bool, FString/FName, enum и null UObject. Поддержаны пропуск и переопределение
аргументов; недопустимые представления и направления дают явную ошибку без
перезаписи старого модуля. Источник defaults — metadata UHT с её точностью,
а не исходный C++ литерал. Подробности и ограничения: Docs/Defaults.md.

Следующий срез: TArray поддерживаемых значений и управление временем жизни
элементов при маршаллинге. Затем дальнейшие контейнеры/объектные поля структур,
AOT и DSL пользовательских классов с hot reload; синтаксис выбирает пользователь.


## TArray скаляров — 2026-10-06

Реализованы верхнеуровневые TArray<int32/float/double/bool/FString/FName>:
input, return, out/inout, типизированные array<T> и move результатов в daScript.
Добавлены проверки полноты, индексов, направлений, размеров и времени жизни frame.
IR v3 дополнен element, runtime signature включает inner property. См. Docs/Arrays.md.

Следующие шаги: массивы структур и UObject (GC), TMap/TSet; authoring DSL и
синтаксические макросы; hot reload методов, затем структуры классов; AOT.
Модель DSL может быть приоритетнее остальных контейнеров по выбору пользователя.


## TArray структур и UObject — 2026-10-06

IR v4 содержит wildcard-листья элементов, identity enum/struct/class и их
зависимости. Добавлены рекурсивные value-структуры и enum в массивах, typed UObject
input/return/out/inout с GC-pins на срок frame и слабыми возвращёнными handles.
В PIE проверяется настоящий Actor.GetAttachedActors с прикреплённым ребёнком.
Подробности: Docs/ComplexArrays.md. Object-поля внутри struct и nested containers
этим этапом не добавлялись.

Следующий этап: TMap/TSet. Далее authoring DSL (модель, синтаксические макросы),
hot reload тел методов и структурных изменений классов, AOT. Синтаксис DSL
выбирает пользователь; DSL может быть отдельным ближайшим приоритетом.


## TMap/TSet скаляров — 2026-10-06

IR v5 и nominal daScript wrappers поддерживают map key/value и set element
скалярных типов. Входы заполняются в staging-values, затем проверяются полнота,
UE hash/equality и уникальность. Native контейнеры строятся перед ProcessEvent;
выходы читаются с учётом sparse/compact layout. Поддержаны return/out/inout,
finite float keys и signed zero; failed post-call frames не раскрывают результаты.
Подробности: Docs/HashContainers.md.

Следующий основной этап: модель authoring DSL для классов/наследования/свойств/
функций/metadata, затем синтаксические макросы; синтаксис выбирает пользователь.
Далее hot reload методов, структурные изменения с переносом состояния, AOT.
Map/Set структур и UObject и вложенные контейнеры остаются отдельным расширением.


## Синтаксически независимая authoring model — 2026-10-06

Добавлена модель module/class/property/method/parameter со стабильными IDs,
типами, defaults, metadata, parent graph и implementation bindings. JSON authoring
v1 является транспортом; reflection binding IR остаётся v5. Проверяются inheritance
cycles/collisions, ID/name/type/default constraints и capabilities existing Actor backend.
Diff различает structure/defaults/metadata/implementation; миграцию он не выполняет.
Модель подключена к Blueprint-сервису с compile/signature preflight методов и
персистентными IDs. Сквозной сценарий: class generation, instance reinstancing,
override preservation, сохранение и reopen. Подробности: Docs/AuthoringModel.md.

Следующий этап: macro frontend для модели; синтаксис выбирает пользователь.
Далее расширение method/metadata emission, reload по diff и контролируемая
структурная миграция, generated C++/UHT/AOT backend для cook/production.


## Уточнение DSL после аудита макросов NimForUE — 2026-10-06

Повторно проверены umacros/ueemit/uebindcore/vmmacros локального commit
58140f4842a91a0187c17ce950e07181ba455862. Подробности и исходные ссылки:
Docs/NimMacroAudit.md. Это аудит реализации, не runtime-валидация NimForUE.

Перед фиксацией DSL syntax добавить в контракт фронтенда/модели:

- annotation scopes для групп properties/functions и индивидуальные overrides;
- отдельные typed specifiers class/property/method/parameter, edit/visibility/access
  matrix, const/directions/parameter defaults и method/event/override kinds;
- source spans + expansion trace, collector внешних/частичных method declarations;
- reflected state отдельно от script-private state и их migration policies;
- authoring declarations UStruct/UEnum/delegate/interface, explicit enum values;
- literal defaults отдельно от runtime initializer hooks, явный порядок CDO/instance/
  super/components/user constructor и запрет исполнения body/global init в preflight;
- component graph со stable subobject IDs/names, root/attachment/socket/instancing;
- network/RPC/RepNotify/conditions и FieldNotify/GAS как отдельные extensions;
- независимые capabilities interpreted/AOT/native adapters, без произвольного
  копирования Nim C++ layout/vtable обновлений в daScript.

Metadata-only changes должны участвовать в diff, в отличие от проверенного
UEType equality NimForUE. Синтаксис выбирает пользователь. Ближайший шаг остаётся
macro frontend на нашей модели, но расширение emission/signatures должно идти
с явными capability checks; ничего из аудита автоматически не считается готовым.


## Authoring backend: несколько методов и metadata — 2026-10-06

Добавлены named persistent entry points с общим owner/file context и reload
fallback; проверка ранее использованных signatures действует на весь session.
Actor schema поддерживает до 64 int→int методов, compile/signature preflight
всех candidate bindings, metadata class/property/method и BlueprintReadOnly новых
полей. Metadata сохраняется в Blueprint source для recompile/reopen.
Подробности и ограничения: Docs/AuthoringMethods.md.

Следующий этап: macro frontend и annotation scopes на существующей модели.
Синтаксис DSL выбирает пользователь; pure/static/general signatures, initializer
hooks, component graph, events/delegates и C++ adapters остаются дальнейшим расширением.


## Проверка выбранной формы DSL — 2026-10-06

Форма ue_class/ue_properties/ue_functions одобрена пользователем как направление.
Проверка daScript 0.6.4 показала: bare ue_class не входит в grammar, но reader macro
позволяет ту же форму с prefix `%ue_class~` и завершением по внешней скобке.
Оба reader варианта проверены реальными compilation/execution capability probes.
См. Docs/DSLSyntaxCheck.md. Полный frontend/tokenizer/emitter ещё не реализован;
этот минимальный native prefix следует явно учесть в окончательном DSL.


## DSL без reader macro — 2026-10-06

Пользователь отказался от reader-macro формы. Новое направление: native
`[ue_class(parent=...)] class`, field metadata `@ue_property=(flags)` и
`[ue_function(flags)] def`. Grammar и извлечение flags из field AST проверены
на daScript 0.6.4, exit 0. См. Docs/DSLNativeSyntax.md. Это syntax capability
proof, не готовый Unreal frontend; reader вариант не используется как план.
Ближайший шаг: collector annotations → validated model → существующий backend.


## Уточнение annotations и optional blocks — 2026-10-06

ue_function_group исключён по просьбе пользователя. Основной вариант —
[ue_class(...)] class, @ue_property=(flags), [ue_function(...)] def.
Точные class-body ue_properties/ue_functions blocks проверены и отклоняются
grammar daScript 0.6.4 до macro expansion; дополнение не включается при заданном
условии отсутствия reader/parser modifications. См. Docs/DSLNativeSyntax.md.


## Native annotation frontend — 2026-10-06

Реализованы native ue_class/ue_function annotations и @ue_property metadata,
collector → validated authoring model → Actor Blueprint backend. Generated
entry points читают/пишут self properties через runtime bridges. Это первая
рабочая ограниченная версия DSL; reader/groups/blocks не используются.
Примеры: dsl_counter_v1.das/dsl_counter_v2.das. Подробности: Docs/DSLFrontend.md.
Следующий шаг — editor apply/reimport + проверка body/structural reload в PIE;
далее general signatures/types, inheritance, constructors/components/events.


## Editor DSL Apply/Reimport и PIE hot reload — 2026-10-06

Добавлен FDasDSLService и console commands Das.DSL.Apply/Das.DSL.Reimport.
Source/class ID/model сохраняются в metadata asset; reimport после reopen
проверен. Body-only apply не запускает Blueprint recompile/reinstance.
Runtime отказывает несовместимой/изменённой декларации до её применения к UE
и продолжает last-good context. Проверены body/structural изменения в настоящем
PIE, override/reference/world isolation, syntax/default-change fallback и recovery.
Подробности: Docs/DSLReimport.md. Полный compiler rollback и automatic watcher/UI
не входят в этот slice. Следующий этап: general signatures/types и capabilities.


## Scalar signatures и bool properties — 2026-10-06

Добавлены float→float, bool→bool и zero-argument void() methods; int→int
совместим с прежним backend. Bool fields поддерживают literal defaults и self
bridges. Typed native adapters и Blueprint graphs проходят compile-only preflight;
existing signature/default migrations отклоняются до mutation. Общий owner/file
context хранит типы ранее использованных entry points для reload validation.
Подробности и текущие границы: Docs/DSLScalarSignatures.md. Следующий шаг —
общий call frame для mixed/multiple parameters; затем дальнейшие field/method types.


## Mixed scalar call frames — 2026-10-06

Реализованы 0–16 ordered by-value int/float/bool parameters и scalar/void result.
Новые graphs используют один ExecuteDasFrame и стандартные UE MakeStruct/
MakeArray/BreakStruct nodes. Runtime проверяет AST contracts всех использованных
entry points и сохраняет last-good context. Parameter names/order/IDs проверяются
при reimport; IDs сохраняются в asset metadata. Пример без explicit IDs:
dsl_mixed_methods.das. Проверены real PIE и Save/Reopen в новом процессе.
Подробности: Docs/DSLMixedMethods.md. Следующий шаг — string/name/double,
затем object/struct signatures с ownership и capability checks.


## String/name/double authoring — 2026-10-06

Добавлены FString/FName/double fields и frame inputs/results. UENameValue —
nominal context-local value token с UE FName semantics; generated UEName wrapper
не изменён. Strings копируются в VM heap на входе и в FString на выходе;
double не сужается до float. Literal defaults, CDO validation и editor pins
расширены. Проверены ownership, real PIE, Save/Reopen, reload fallback.
Подробности: Docs/DSLTextTypes.md. Следующий шаг — object/struct signatures,
затем containers/out-ref и дополнительные authoring capabilities.


## Object input/result frames — 2026-10-06

Добавлены UObjectHandle inputs/results, native class constraints через
arg_<name>_class/return_class и class-typed Blueprint pins. Runtime ABI использует
temporary handle addresses/CMRES; inputs GC-pinned только на время eval, VM и
host results слабые. Destroy/expired/wrong-class return отклоняются. Core
replacement delegate перенаправляет existing weak handles при UE reinstance.
Проверены real PIE и saved typed Actor graphs в новом процессе.
Подробности: Docs/DSLObjectMethods.md. Object fields по-прежнему отдельно;
следующий шаг — FVector/FRotator/FTransform value signatures.


## FVector/FRotator/FTransform method values — 2026-10-06

Добавлены UEVector/UERotator/UETransform handled values, native constructors/math
helpers и exact UE struct Blueprint pins. Aligned input copies и CMRES outputs
не сохраняют временных pointers. Проверяются finite values и quaternion validity.
Проверены double precision, independent copies, real PIE и Save/Reopen.
Подробности: Docs/DSLMathValues.md. Struct fields и arbitrary reflected structs
ещё не реализованы; следующий этап — общий reflected struct adapter.


## Generic reflected struct values — 2026-10-06

Добавлен UEStructValue с arg_<name>_struct/return_struct constraints, recursive
scalar/nested reflection capability validation и immutable native snapshots.
Blueprint pack/unpack — два generic wildcard thunks, без per-type C++ adapters.
Проверены DasCallPayload (binding IR type), deep copies/string/nested update,
real PIE и Save/Reopen. Snapshot table bounded до 4096 distinct values; reclamation
по VM roots пока не реализован, production long-lived loops требуют этого шага.
Подробности: Docs/DSLReflectedStructs.md. Дальше — snapshot lifetime, enum leaves,
затем struct fields/defaults/migration и дополнительные authoring capabilities.


## Snapshot lifetime/reclamation — 2026-10-06

Добавлен typed VM-root mark/sweep для UEStructValue snapshots. Persistent call
boundaries очищают transient values; options gc + needCallerStackFrame/native
line carriers позволяют pressure collection внутри long interpreter loops.
Недостоверные frames не приводят к unsafe sweep. Проверены 20k + 10k updates,
global array anchors, active locals/args, 5k коротких calls без gc option и release.
Подробности: Docs/DSLStructLifetime.md. Дальше — enum leaves, struct fields/defaults
и migration; production AOT/JIT root attribution отдельно.


## Enum leaves reflected structs — 2026-10-06

Добавлены FEnumProperty и enum-backed byte leaves, strict enum path + registered
symbol helpers, immutable update и snapshot-wide enum value validation.
Unknown/_MAX values не доходят до method body. Editor/cooked rules используют
registered names, не display labels/editor-only Hidden metadata. Проверены
DasNamedMode (IR type), native values 7/42, nested/legacy enum и real PIE/SaveReopen.
Подробности: Docs/DSLStructEnums.md. Далее — struct fields/defaults/migration.


## Reflected struct fields/defaults — 2026-10-06

UEStructValue properties now create typed Blueprint variables with native CDO
defaults and immutable self read/write. Literal ue_struct_value/default initializers
provide native struct identity; strict Unreal export-text default validation rejects
unknown/duplicate fields, parse errors, trailing text and invalid enum values.
Existing defaults/types cannot change without migration policy. Tests cover actor
override preservation through reinstance, persistence and real PIE.
Details: Docs/DSLStructFields.md. Next: migration policy design/implementation,
object properties and further authoring capabilities; native math field sugar and
script-defined reflected structs remain explicit follow-up stages.


## Explicit default migration — 2026-10-06

Added opt-in defaults=preserve_instances to Das.DSL.Reimport. Only defaults may
change in this step; the current Blueprint must match its stored model. Selected
CDO and variable descriptors update without reinstance; all existing instance
values are preserved, including old-default values. New spawns use new defaults.
Scalar/struct defaults, saved asset reopen, later compile and PIE are validated.
Details: Docs/DSLDefaultMigration.md. Next migration capability: explicit field
rename mapping by stable identity, followed by constrained type conversion/removal
policies; old-default propagation is a separate optional policy, not inferred.


## Field rename by stable ID — 2026-10-07

Added explicit fields=rename_by_id reimport for isolated Actor Blueprints. Only
property names may change; retained ue_id determines the mapping. Vacant target
names, unchanged type/default/flags/methods and matching stored model are required.
Unreal variable GUID/local references persist; pre-reinstance native snapshots
restore renamed CDO/instance values on replacement objects. Save/Reopen and real
PIE validate source promotion and scalar/struct overrides. Dependent Blueprint or
saved referencing asset migrations are refused pending a separate scope.
Details: Docs/DSLFieldRename.md. Next: dependency-aware migration (children and
saved maps/assets), then constrained type conversion/removal policies and rollback.


## Dependency-aware field rename — 2026-10-07

Added fields=rename_with_dependencies. Loaded dependencies and saved package
referencers form a bounded scope; ordinary Blueprint and map assets load before
snapshot capture. Root/derived CDOs and instances restore through parent-first
recompile/reinstance; dependent graph references update via Unreal's rename API.
Packages stay dirty for review/save. Separate-process Create/Migrate/Reopen tests
cover child/grandchild overrides, external getter and initially unloaded saved map.
Inherited DSL owner/default validation uses the declaring property's class CDO;
child overrides work without bypassing unapplied parent-default checks.
Details: Docs/DSLRenameDependencies.md. Next: explicit constrained type conversion
and removal policies with data preservation/rollback. World Partition/streaming,
external actor packages and other asset types remain separate scope extensions.


## Exact numeric widening and bounded rollback — 2026-10-07

Added explicit widening policies for int32/finite float32 -> double, isolated or
ordinary Blueprint/map dependency scope. Defaults must be exact promotions; IDs,
names and other declarations stay unchanged. Direct Blueprint property references,
uncompiled scoped Blueprints and nonfinite/lossy conversions are refused.
Original descriptors/values restore after a post-compile failure; metadata commits
only on success. Method-only no-binding/no-cached-call VM contexts survive tested
reinstance rollback; reflected host changes force retry of rejected source validation.
Details: Docs/DSLNumericWiden.md. Next: explicit Blueprint pin/link adaptation and
additional conversion policies, field removal with retained data, then lifecycle/
delegate context preservation and general rollback beyond numeric migration.


## Combined numeric conversion/graph/rollback stages — 2026-10-07

Implemented all scalar numeric directions with explicit integer rounding and float
precision rules. Standard getter/setter pins/literals update, compatible links
persist, typed checked adapters handle other numeric boundaries. Runtime failures
abort Blueprint execution. Numeric rollback restores original nodes, pin defaults/
types/connections and native CDO/instance data before leaving prior metadata intact.
Details: Docs/DSLNumericConversions.md. Remaining separate tasks: field removal with
retained data, general rollback for other migrations, lifecycle/delegate preservation,
and World Partition/streaming/other asset scopes. Scalar numeric type-change work no
longer requires another basic pin/link or rounding stage.


## Field retirement with retained data — 2026-10-07

Added explicit archive_remove/archive_restore policies, isolated or with ordinary
Blueprint/map dependencies. Hidden native instance/CDO maps retain values by class
and stable field ID; editor metadata retains declarations and original variable
GUIDs. Exact restoration consumes retained values only on success. Fresh actors
created during retirement inherit archived child defaults. Missing/incompatible
values, altered restoration declarations, replicated fields and direct graph
references are refused before mutation. Post-compile faults roll back descriptors,
graphs, values and archive maps; eligible method-only contexts survive in real PIE.
Details: Docs/DSLFieldArchive.md. Next: lifecycle/delegate/cached-call preservation
during reinstancing, general rollback for other structural migrations, and separate
World Partition/streaming/other asset scopes. Archive purge and combined restore
plus rename/type conversion remain explicit future policies.


## Lifecycle/delegate preservation during reinstancing — 2026-10-07

Expanded the editor preservation scope to lifecycle and managed delegate contexts,
including unmodified owners subscribed to replaced peers. Managed proxies detach
before compilation and reinstall once with remapped sources/new IDs; BeginPlay and
EndPlay delivery flags persist. Native adapter lifecycle path/Tick state restore
on replacement actors. Archive/numeric rollback keeps last-good lifecycle and
handler state; source promotion remains exact-declaration guarded. DSL structural
reimport and rename now use the same scope. Real Tick/Destroy/EndPIE, independent
listener state, self/peer subscriptions, external owner source replacement and GC
are checked in PIE. Call frames were already invocation-local: no cross-call cache
preservation stage is needed for the current API; future caching would need its
own contract. Details: Docs/DSLLifecycleMigration.md. Next: general rollback for
ordinary structural reimport/rename, broader asset scopes, and further authoring
capabilities from the main plan.


## Bounded structural reimport/rename rollback — 2026-10-07

Added reusable Blueprint definition/graph/metadata/value snapshots for existing
DSL declaration reimport and stable-ID rename. Failures after mutation, compilation
or metadata writes restore previous variable GUIDs, graph references/pins/links,
class/function metadata, CDO/instance values, retirement maps and package dirty
flags. Added method graphs are removed; a corrected retry can recreate their exact
names. Runtime lifecycle/delegate contexts survive forward and rollback reinstance.
Ordinary declaration reimport now loads bounded saved ordinary Blueprint/map
dependencies before snapshot and compiles them before success. Separate-process
save/reopen tests and real PIE fault injection cover these paths. Details:
Docs/DSLStructuralRollback.md. This closes rollback for currently supported
declaration reimport and rename operations; arbitrary engine transactions/native
reload side effects remain outside scope. Next migration extensions: explicit
World Partition/streaming/other asset scopes and coordinated dependent DSL source
changes. Further authoring capabilities remain in the main plan.


## Typed UObject authoring properties — 2026-10-07

Added UObjectHandle fields with a literal ue_object_value(native_class_path)
initializer and null default. Native annotations/model/backend produce typed
FObjectProperty fields; self reads/writes use checked native accessors. Strong UE
property ownership and weak VM handles remain distinct. Null, wrong-class and
read-only access, GC retention/release and explicit Actor destruction are covered.
Object-valued migration snapshots pin and remap actor/component references during
rename, retirement/restoration and structural rollback. Saved maps preserve cycles,
component references and ordinary UObject asset references through separate-process
reopen and migration. Details: Docs/DSLObjectFields.md. Next authoring stages:
script-defined reflected UEnum and UStruct declarations, components and event/
override declarations. Soft/weak/class/interface properties, component construction,
non-null asset defaults and script-defined property class identities remain explicit
extensions. Broader migration asset scopes remain separate work.


## Reflected enum/struct declaration backend — 2026-10-07

Added native ue_enum/ue_struct annotations and a separate compile-only reflected
types model. Das.DSL.Types.Apply creates Blueprint-usable reflected enum/struct
assets in a selected /Game folder, with stable identities, metadata, explicit enum
values and scalar struct defaults. Enum assets use runtime UDasDefinedEnum to avoid
UUserDefinedEnum's editor-load renumbering. Structs use UUserDefinedStruct/compiler
and persist Unreal variable GUIDs plus DasMemberId. Existing assets are immutable
in this slice: exact reimport reuses identities; changed declarations/native drift
require explicit migration. Save/reopen validates values 7/42, Unicode defaults and
ordinary Blueprint fields/defaults referencing the generated types. Details:
Docs/DSLTypeDeclarations.md. Next: resolve authored type identities in ue_class
properties/method frames, checked native enum/struct value adapters, then bounded
type changes/data migration. Nested/complex struct fields, component declarations
and event/override authoring remain further stages.


## Authored enum/struct class fields and typed frames — 2026-10-07

Loaded authored UEnum/UUserDefinedStruct identities now connect to ue_class fields
and method frames through checked UEEnumValue/UEStructValue wrappers. Enum kind 13
and native Blueprint packing thunks preserve exact UEnum identity and explicit
values; wrong/unknown/reserved values refuse before mutation. Struct snapshots
support authored friendly field names and translate strict default text to native
GUID property names. No native/daScript binary layout equality is assumed. Typed
Blueprint ProcessEvent, mixed frames, save/reopen, GC and structural rollback with
last-good callback are exercised. Details: Docs/DSLAuthoredValues.md. Next: direct
plain daScript enum/struct value adapters and stable-ID type resolution, then bounded
type changes/data migration and nested/complex declared fields. Type assets remain
immutable; components, event/override authoring and broader asset scopes remain
separate capabilities.


## Direct authored daScript values — 2026-10-07

Annotated plain enums/structs now work in ue_class properties and method frames.
Native AST adapters preserve the existing checked token/snapshot ABI internally.
The same file may declare types and a class; collection-only compilation allows
the first type import before assets exist. type_folder resolves loaded assets by
declared name, with enum value/layout/default checks. Reflected self reads, whole
struct writes and scalar leaf writes/compound updates are supported without
native/daScript layout assumptions. Details: Docs/DSLDirectTypes.md.

Next: stable-ID type resolution and a cooked end-to-end direct-types fixture,
then explicit type-definition migration policies (enum members and struct fields,
dependent Blueprint/map repair and rollback), followed by nested/complex types.
Components and event/override authoring remain later capabilities.


## Stable-ID catalogs and cooked direct types — 2026-10-07

UDasTypeRegistry catalogs resolve plain authored enums/structs by stable IDs through
the type_registry class annotation. Binary catalog bindings carry hard references,
kind and struct GUID; enums serialize their ID. The catalog works without editor
metadata after cook. IDs remain automatic unless intentionally preserved across
source declaration renames. Folder/name resolution remains a compatible option.
Types.Apply accepts an optional catalog package; exact publication is idempotent,
while catalog rebinding requires a later explicit migration policy.

The native Actor adapter exposes a catalog reference for cook/GC dependencies.
An isolated saved Blueprint/map uses renamed source declarations to reference the
original assets. Its packaged game exercises direct scalar leaf writes, mixed
enum/struct frames, ProcessEvent, persisted overrides, Unicode, identity failures
and GC. Details and validation: Docs/DSLTypeRegistry.md.

Next: explicit authored type-definition migrations, starting with enum additions
and stable member identities, then struct field changes with dependent Blueprint/
map repair and transactional rollback. Nested/complex types, components and event/
override authoring remain later capabilities.


## Append-only enum definition migration — 2026-10-07

Das.DSL.Types.ExtendEnums adds explicit enum members through a runtime catalog,
preserving existing names/order/numbers and object/stable-ID identity. Existing
ordinary Blueprint variables/method wrappers and map values need no data rewrite.
Preflight bounds dependency scope, verifies old values and unchanged class/struct
declarations, and rejects enum nodes requiring reconstruction. Mutation/model
publication rollback restores entries, metadata and dirty flags. Source edits and
asset saves remain explicit. The first policy requires stopped PIE and no active
script contexts. Details: Docs/DSLEnumAppend.md.

Next: stable enum-member IDs and graph reconstruction/transactional repair,
followed by rename/removal policies and struct field migrations with dependent
Blueprint/map repair. Full type-definition hot reload and broader scopes remain
later stages.


## Stable enum-member IDs and Switch reconstruction — 2026-10-07

Enum entries now carry automatic/optional explicit IDs, serialized in v2 type
models and binary UDasDefinedEnum maps; v1 models remain readable. Append migration
and direct adapters check identity drift. `graphs=repair` supports Switch on Enum
within one imported DSL Actor root and ordinary dependent Blueprint/map scope.
It reconstructs new outputs, compiles root/dependents, verifies old links and uses
graph/class/value rollback after reconstruction or compilation failures. Details:
Docs/DSLEnumMembersAndSwitch.md.

Next: expand enum-node support (literal/select/count/iteration with verified sparse
value semantics), then member rename by ID and explicit removal/value policies.
Struct field migration and broader type hot reload remain later stages.


## Standard enum-node family reconstruction — 2026-10-07

graphs=repair now supports literal, select, count, safe byte cast and iteration,
in addition to Switch. Native enum reload/reconstruction and Blueprint compilation
preserve old links/defaults with transactional graph/class/value rollback. Tests
exercise actual sparse values 7/42/200, count 2->3, select/literal results and
iteration sums 49->249 before/after append, rollback, reopen and cook. Safe casts
retain Unreal's MAX-on-invalid behavior; the binding still rejects MAX. Unsafe
casts and broader independent-root coordination remain outside this scope.
Details: Docs/DSLEnumNodeRepair.md.

Next: member rename by stable ID with graph-default/branch repair and explicit
class default rules, then removal/value policies and struct field migrations.


## Enum member rename by stable ID — 2026-10-07

Das.DSL.Types.RenameEnumMembers changes member names with unchanged ID/order/value.
Native enum, Blueprint variable/pin defaults, Literal and Switch/Select names are
updated in the existing rollback scope. Class enum default symbols may change only
through the same value-preserving rename map; other declaration changes reject.
Graph links/defaults, executable node results, CDO/map values, four failure stages,
reopen and cooked invocation are verified. Details: Docs/DSLEnumRename.md.

Next: explicit removal/value-change policies, then struct field add/rename and
remove/type-change with Blueprint/map repair. Multi-root coordination and active
PIE type reload remain broader later stages.


## Explicit enum removal/value rules — 2026-10-07

MigrateEnumValues reads a bounded, explicit JSON transition map. Surviving IDs and
names retain identity while numbers change; removed member data uses a declared
existing target. The first storage scope covers declared scalar enum Actor fields
and current CDOs with snapshot/remap through reinstance. Linked/overridden removed
case/options reject; empty cases/options and typed defaults are repaired. Four
failure stages, saved map/CDO, updated node results and cook are validated.
Containers/nested/archive state remain explicitly outside this storage policy.
Details: Docs/DSLEnumValuePolicies.md.

Next: authored struct field add/rename with stable IDs, copied native values and
dependent Make/Break Struct repair, then remove/type-change policies. Wider enum
storage, multiple roots and active-PIE type reload remain later expansions.


## Authored struct field add/rename — 2026-10-07

MigrateStructFields supports one scalar authored struct and one DSL Actor root.
Stable IDs/GUIDs identify old fields; friendly/native GUID-suffixed names change
together for cook. Independent per-field snapshots preserve overrides/Unicode/CDO
data across native structure/Blueprint recompilation, new fields use defaults,
and Make/Break Struct pins/graphs are repaired. Old layouts are not retained in
FInstancedStruct snapshots for the changing type. Four-stage rollback, ID/type
refusal, idempotency, save/reopen and cook are verified. Bounds:
Docs/DSLStructFieldMigration.md.

Next: explicit struct removal/type-change policies and supported value conversion,
then nested/container state, split/custom struct nodes and multi-root coordination.
Active-PIE type-definition reload remains a broader later stage.


## Explicit authored struct field removal — 2026-10-08

MigrateStructFields optionally accepts bounded per-stable-ID drop rules. Explicit
rules discard direct Actor/CDO field data; Blueprint member links and changed
Make Struct defaults reject before mutation. Surviving IDs/GUIDs and values use
the existing independent scalar snapshots and Make/Break repair. Old removed
values remain available for rollback. Verification covers missing/stale rules,
linked/default refusal, four rollback stages, save/reopen and cooked execution.
Details: Docs/DSLStructFieldMigration.md.

Next: explicit numeric struct field type-conversion policies, native value/default
conversion and checked graph adapters. Nested/container state, split/custom nodes,
multiple roots and active-PIE type reload remain broader later stages.


## Authored struct numeric field conversions — 2026-10-08

MigrateStructFields accepts an explicit numeric convert rule using existing
integer/float policies. One field changes type per transaction; identity/name
remain stable. New defaults must be converted old defaults. Actor/CDO snapshots
retain round-trip numeric precision and separate original/forward values. The
shared numeric graph migration also accepts explicit Make/Break pins, preserves
user/default distinctions and inserts checked conversion nodes. Four-stage
rollback removes adapters and restores layout/data/graphs. Bounds and syntax:
Docs/DSLStructFieldMigration.md.

Next: nested/container value state and dependent graph repair, including split
pins/custom struct operations. Multiple roots and active-PIE type definition reload
remain broader stages. Numeric conversion currently cannot combine add/drop/rename
and does not simplify existing adapter chains.


## Nested struct storage migration — 2026-10-08

Existing user-struct wrapper trees on the DSL Actor/CDO family now use recursive
independent scalar snapshots during authored scalar struct add/rename/drop/numeric
conversion. Repeated inner occurrences have separate path/ID keys. Unrelated
sibling values, native wrapper defaults and GUIDs survive; wrappers are explicitly
included in loaded/asset dependency scope. Root/default and editor numeric text
round-trip guards reject inconsistent defaults. Old-layout FInstancedStruct values
are excluded throughout the affected trees. The bridge's Blueprint default exporter
preserves numeric precision recursively. Details: DSLStructFieldMigration.md.

This milestone covers nested storage, not nested daScript type authoring. Next:
allow nested `[ue_struct]` field declarations and recursive pack/unpack helpers;
then container storage/state migration, split/custom graph repair, multiple roots
and active-PIE type reload. Wrapper operation nodes and explicit nested defaults
remain outside the current migration policy.


## Nested authored type declarations — 2026-10-08

ue_struct supports by-value fields of other authored structs with empty child
constructors. Private stable-ID references form a closed validated DAG; native
assets are created child-first. Recursive scalar-path AST helpers pack/unpack
owned values. Deep self scalar writes commit to the Actor, while whole nested
member writes reject explicitly and whole root-property assignment is supported.
Typed frames, ProcessEvent, aliases, idempotency, save/reopen and cook are verified.
Details: Docs/DSLNestedTypeAuthoring.md; example: dsl_nested_authoring.das.

Next: container declarations/storage migration, then broader nested writes and
split/custom graph repair. Nested child-member definition changes, multiple roots
and active-PIE type reload remain separate policies.


## Arrays of struct: storage and migration — 2026-10-08

Owned struct snapshots support bounded arrays of supported structs and scalar
indexed paths. Independent migration images capture lengths, ordered element paths
and stable-ID values; new layouts allocate/initialize elements before restoration.
Add/rename/drop/numeric rules apply to each element with precise rollback. Empty
native/CDO array defaults are the initial policy. Details: DSLStructArrayStorage.md.

This milestone extends native wrapper storage and UEStructValue access, not DSL
container declarations. Next: array declarations/recursive dynamic pack-unpack,
then set/map key/state migration, nonempty defaults and container graph repair.
Direct Actor container fields, multiple roots and active-PIE type reload remain
separate work.


## DSL array fields and dynamic owned helpers — 2026-10-08

ue_struct supports implicit-empty array<authored struct> fields. Closed model
references include element types and the native factory creates typed FArrayProperty
fields child-first. Statement/loop AST helpers compose owned nested values and
array elements, with checked size/type/index APIs. Whole self move assignment and
daScript clone/move typed values work; direct indexed self writes reject explicitly.
Details: DSLArrayTypeAuthoring.md; example: dsl_array_authoring.das.

Next: primitive/standalone array coverage and set/map key/identity declarations,
then direct mutations and container-definition/graph migration. Declared defaults
remain empty; active-PIE definition reload and multiple roots remain later policies.


## Primitive array DSL functionality — 2026-10-08

ue_struct supports array<int/float/double/bool/string/UENameValue>. Native fields,
bounded owned snapshots and loop helpers preserve values, Unicode and name identity.
Managed names pass to setters by const-reference; no native element alias escapes.
Typed enclosing-struct interop, native state, reopen and cook are verified.
Migration expansion is explicitly deferred. Details: DSLPrimitiveArrays.md.

Next functionality: set/map and standalone container signatures, then out/ref and
interfaces. Deferred migration policies should be revisited as separate tasks.


## Initial map DSL and owned interop — 2026-10-08

table<int;int> fields in ue_struct create typed native TMap storage. Readonly
pack loops and owned unpacked tables preserve keys/values through typed frames,
root-property commits and save/reopen/cook. Duplicate keys update values; missing
keys reject. Iterators over temporary self reads reject and local clone usage is
documented. Structural map migrations remain deferred. Details: DSLMapAuthoring.md.

Next functionality: broaden map types, set and standalone container signatures,
then out/ref and interfaces. Do not expand deferred migration logic as a prerequisite.


## String map keys and values — 2026-10-08

All int/string key/value map combinations are supported inside ue_struct. Owned
FString/daScript storage preserves Unicode and empty text. Native-equality string
key collisions reject explicitly before root commit; exact duplicate keys update.
Typed values, save/reopen and cook are verified. Details: DSLMapAuthoring.md;
example: dsl_string_maps.das. Map migrations remain deferred.

Next functionality: set, more map value types and standalone container signatures,
then out/ref and interfaces. Do not make universal migrations a prerequisite.


## Set DSL and owned interop — 2026-10-08

Key-only daScript tables `table<int>` / `table<string>` inside ue_struct map
to native TSet storage. Generated keys/insert helpers preserve owned sets in
typed input/output and whole Actor property commits. Native equality collisions
reject explicitly. Snapshot APIs provide size, sorted ordinal enumeration,
contains and owned add/remove. Empty defaults only; set migration is deferred.
Details: DSLSetAuthoring.md; example: dsl_set_authoring.das.

Next functionality: more map value types and standalone container signatures,
followed by out/ref and interfaces. Keep universal migrations deferred.


## Scalar/name map values — 2026-10-08

Authored map fields accept int/string keys and int/float/double/bool/string/name
values. Native float/double terminal types remain distinct and UENameValue uses
owned FName identity. The bridge provides typed helpers and explicit owned
snapshot get/with APIs. Empty defaults and earlier collision/budget rules remain.
Details: DSLMapAuthoring.md; example: dsl_scalar_maps.das.

Next: standalone DSL container parameters/results, followed by out/ref and
interfaces. Further map key types and enum/struct/object values remain separate
extensions. Migration development remains deferred.


## Standalone container calls — 2026-10-08

BlueprintCallable DSL instance methods accept/return primitive arrays, int/string
sets and all current scalar/name maps directly. Twenty native container shapes
use ordinary Blueprint pins and internal compiled owned carriers through the
existing frame ABI. Mixed scalar/container input order is supported. K2 array
ref/out flags are native implementation details; script input values remain owned
copies. Details: DSLContainerCalls.md; example: dsl_container_calls.das.

Next: explicit DSL out/ref semantics, then interfaces. Further container element/
key types, class container properties and migration expansion remain separate.
Migration development remains deferred.


## Explicit out/ref method parameters — 2026-10-08

ue_function(ref="counter", out="total") defines parameter directions; named
arguments require mutable daScript references. Primitive and current standalone
container values use owned locals and deferred publication of all outputs.
Blueprint gets native output/reference pins and copy-back graphs. Failed calls,
including late output packing errors, preserve ref input values and return out
defaults. Per-slot reference aliasing is not preserved. Details: DSLOutRef.md;
example: dsl_out_ref.das.

Next: interfaces. Object/enum/struct/math out/ref extensions remain separate
follow-up work, along with further container/class property support. Structural
migration development remains deferred.


## Existing native interface implementations — 2026-10-08

ue_class(implements="/Script/Module.Interface") records native interface identities.
Exact method contracts validate before mutation. Real Blueprint interface graphs
dispatch through C++ Execute_* / ProcessEvent into DSL methods, including owned
array returns and out/ref copy-back. Checked daScript membership and initial
int/string dispatch to ordinary Blueprint receivers are supported.
Details: DSLInterfaces.md; example: dsl_interfaces.das.

Next interface work: broader typed binding dispatch, authored interface definitions
and event-only methods. Reentrant VM callbacks, native fallback wrappers,
interface-value properties and structural membership changes remain explicit
follow-up capabilities. Migration development remains deferred.


## Typed interface frame dispatch — 2026-10-08

ue_begin_interface_call constrains an existing UCallHandle to a declared native
interface method, checking identity/signatures at creation and invocation. All
existing supported marshalling shapes and directions reuse the owned native frame.
Scalar, array, map/set and multiple out/ref interface calls to ordinary Blueprint
receivers are verified in editor/reopen/cooked. Missing inputs, type mismatch and
released handles reject. Details: DSLInterfaces.md; example: dsl_interfaces.das.

Next: DSL-authored interface definitions. Automatic generated interface wrappers,
event-only implementation, native fallback wrappers and reentrant VM entry remain
separate capabilities. Migration development remains deferred.


## DSL-authored interface definitions — 2026-10-08

ue_interface class declarations use native daScript def abstract signatures.
Methods default to BlueprintCallable; mutable references are inout and the
interface-level out list marks output-only slots. Abstract methods cannot carry
function annotations in the pinned parser. Collection emits validated authoring
IR; Das.DSL.Interfaces.Apply creates real BPI assets with a custom compiler and
persistent runtime contract identity. DSL classes implement generated /Game
interface paths with existing owned calls. Details: DSLAuthoredInterfaces.md.

Next interface capabilities: event-only methods, additional authored type
references and automatic typed wrappers. Structural interface/contract migration,
interface-value properties, native fallback and reentrant VM entry remain deferred
or separate capabilities.


### Repeatable interface annotations — 2026-10-08

- Added ue_implements(interface=CounterContract), repeatable on ue_class.
- Short names resolve within the implementing module ID; qualified stable IDs
  and full Unreal interface paths are supported. Existing implements path lists
  remain compatible. Unknown, ambiguous and duplicate identities reject.
- Authored Blueprint StableId is searchable in editor asset registry; generated
  class identity handles loaded/cooked execution. Resolution occurs after all
  annotations are attached and persists concrete Unreal paths in authoring IR.
- No reader macro, custom parser or type expression in annotation arguments.
- Generated typed interface references/calls using typemacro_boost are a later
  stage, alongside event-only methods and additional contract types.


### DSL syntax audit — 2026-10-08

See DSLSyntaxAudit.md. Fixed source parameter names when repeatable ue_implements
is attached after legacy scalar lowering. Recommended core follow-ups: connect
generated typed native wrappers to authoring, share scoped identity rules, add
native tuple direction lists and reject repeated singleton/conflicting redundant
annotations. Keep annotation-only declarations and current compatibility syntax;
typed macro sketches are not implemented capabilities. Migration work stays deferred.


### Typed UObject authoring — 2026-10-08

- UEObject type macro reuses existing generated native object wrappers. Native
  metadata and unambiguous short aliases connect them to class fields/signatures.
- Concrete ActorRef/ObjectRef aliases live in a separately compiled module to
  respect the pinned parser's generic member-method restriction. No reader or
  parser modification is used.
- Source signatures infer native class constraints; conflicts reject. Bridge ABI,
  weak VM handles, strong UPROPERTY fields and checked native conversions remain.
- Later work: broader native exports, convenient checked source conversions,
  authored classes/interfaces, generic matching, object containers and out/ref.
  Migration work remains deferred. See DSLTypedObjects.md.


### Annotated interface method stubs — 2026-10-08

- Ordinary empty methods replace abstract signatures in current interface examples.
  ue_function carries stable IDs, native method metadata and explicit out/ref.
- The compiler supplies non-void placeholder returns behind a direct-call trap;
  source implementations/nonempty bodies reject. Legacy abstract sources remain
  readable. Reader/preprocessor/parser changes are not used.
- Metadata is applied to native UFunctions and signature graphs. Metadata-only
  reapplication preserves the class; structural contract changes remain deferred.
- Event-only methods, broader interface types and typed interface references/calls
  remain subsequent core stages. General migration work stays deferred.


### Shared DSL identity and annotation rules — 2026-10-08

- Added canonical, kind-checked resolution for parent/classes, interfaces, enums
  and structs. Native names/aliases and source stable IDs are supported; named
  authored enum/struct resolution keeps explicit registry/folder scope and hard
  cook dependencies. Missing/ambiguous/wrong-kind references reject.
- Added native tuple direction lists to class/interface methods, preserving
  single names, legacy CSV, parameter IDs and copy-back ABI.
- Singleton method/scalar options and conflicting natural-type constraints now
  reject explicitly; ue_implements remains repeatable.
- Actor-only parent capability and unsupported structural migrations are unchanged.
  See DSLTypeResolution.md. General migration work remains deferred.


### Interface event backend — 2026-10-09

- Added authored/native void/no-output interface event declarations and actual
  overriding UK2Node_Event nodes in Actor Event Graphs. Function graphs remain for
  return/out/inout methods. Shared owned input frame marshalling is reused.
- Stable IDs, reimport shape/ownership checks and native interface signature
  validation cover event adapters. Body reload keeps the class; unsupported event
  removal/rename rejects under the existing deferred structural policy.
- Core follow-ups: broader authored interface type references and generated typed
  interface wrappers/calls, then relevant lifecycle/production/network stages.
  General migrations and VM reentry remain deferred. See DSLInterfaceEvents.md.


### Parameter-local directions — 2026-10-09

- Preferred syntax: mutable var references infer inout, @ue_out marks output-only,
  and @ue_ref is optional explicit inout. Classes and ordinary interface methods
  share validation and preserve source names/automatic IDs.
- Legacy tuple/CSV function lists remain compatible; contradictory/repeated/invalid
  parameter directions reject. Native frame initialization/copy-back semantics
  and primitive/container capability limits are unchanged.
- Examples and runtime/editor/cook probes use the parameter-local form. General
  migrations and remaining core binding stages stay on the existing plan.


### Source type references in DSL bodies — 2026-10-09

- Added compiler lowering of ue_type(TypeName) and short literal names in object,
  interface, enum and struct runtime calls within reflected class bodies/defaults.
- Reuses scoped identity/kind/ambiguity validation and the existing checked runtime
  ABI; canonical paths stay internal. Registry identities retain precedence, with
  native-only fallback when no authored declaration/entry matches.
- Generated typed interface-reference values/method wrappers remain subsequent work.
  General migrations and VM reentry remain deferred. See DSLTypeCalls.md.


### Generated native interface bindings — 2026-10-09

- Native interface receiver metadata now reaches the reflection IR/generator.
  Nominal weak references, collision-safe friendly helpers and typed methods
  reuse checked membership, declared signature guards and owned marshalling.
- Includes scalar/string, arrays, map/set, multiple out/ref results and void calls.
  Blueprint receivers exercise real event state changes as well as functions.
- Scoped authored BPI wrapper generation, broader authored signature types and
  reflected interface storage remain next integrations. Native Execute fallback,
  VM reentry and general migration work remain deferred. See DSLGeneratedInterfaces.md.


### Authored interface native value signatures — 2026-10-09

- Added typed/native object identities, math types and constrained native enum/struct
  snapshots to ordinary interface stubs and actual Blueprint interface pins.
- Shared type resolution and inferred/explicit constraint conflict checks match
  class declarations. Unsupported object/math/enum/struct out/ref rejects explicitly.
- Enum contract/owner checks and wildcard frame packing now support both native
  FEnumProperty and Blueprint FByteProperty without relaxing identity/value rules.
- Next: scoped natural authored enum/struct signatures and authored interface wrapper
  generation. Reflected interface storage, VM reentry and structural migrations
  remain separate/deferred. See DSLAuthoredInterfaceNativeTypes.md.


### Authored interface registry values and bindings — 2026-10-09

- Interface signatures infer concrete authored enum/struct identities through an
  explicit registry/folder, preserving IDs, GUID/layout/default checks and conflict
  rules. Empty declaration stubs support enums without a zero member and initialized
  structs behind the existing direct-call trap.
- Reflection export now prepares selected authored BPI signatures and retains stable
  identity. The generator supports isolated module names, authored nominal receivers,
  short aliases and enum symbol helpers; real Blueprint calls cover enum/struct too.
- Reflected interface storage, broader out/ref, unification of generated vs authored
  source value types, VM reentry and general migrations remain separate stages.
  See DSLAuthoredInterfaceRegistryBindings.md.


### Interface method calls and ref/out copyback — 2026-10-09

- Added typed method-name facades resolved by ordinary daScript dot calls over the
  existing native/authored interface wrappers. No reader/parser changes are required.
- ReturnValue becomes a return; non-return outputs become mutable references, with
  copyback/move after successful result extraction. Arrays/containers retain owned
  transport and receiver/signature/lifetime guards remain on the same call boundary.
- Natural enum/struct representation unification and a generic reference factory
  remain next ergonomics integrations; reflected interface storage and broader
  authoring out/ref, VM reentry and migrations remain separate. See DSLInterfaceMethodCalls.md.


### Typed interface factory and source value facade — 2026-10-09

- Added exact `ue_interface(type<Interface>, target)` overloads using the existing
  checked weak capability factory, following verified daScript grammar.
- Optional explicit source-type manifests connect facade signatures to public
  ue_enum/ue_struct declarations. Symbol/member converters retain low-level ABI.
- Compile-time annotations validate source IDs against the selected registry and
  canonical path/layout/default/GUID checks; bad identity fails before invocation.
- First struct mapping covers primitive/string fields. Nested/container/name/object
  shapes and broader reflection/storage remain subsequent extensions; VM reentry
  and general migrations remain deferred. See DSLInterfaceSourceValues.md.


### Nested source structs and arrays — 2026-10-09

- Source mapping now validates nested enum/struct identities and array element
  mappings; recursive converters own and move container-containing values.
- Generic reflected call frames support arrays inside structs with bounded concrete
  wildcard traversal and required-leaf validation; generated owned readers recurse.
- Source/interface probes cover deep copies, distinct array values, Unicode, empty
  arrays/defaults, save/reopen/game. Source name/object/map/set shapes and standalone
  authored struct-array signatures remain later extensions. See DSLNestedSourceValues.md.


### Source struct Name and native UObject references — 2026-10-09

- Added natural UENameValue and typed native object fields with implicit null
  defaults, exact reflected class identity checks and source facade conversion.
- Context snapshots use Unreal FGCObject/FInstancedStruct reference collection;
  reflected call-frame object leaves retain checked input/output pinning.
- Nested fields and struct-array elements cover Name/null values, independent
  copies, GC retention/reclamation and destroyed Actor reads after collection.
- Map/set source conversion, object arrays, standalone authored struct-array
  signatures and interface storage remain subsequent stages. General migrations
  stay deferred. See DSLSourceObjectValues.md.


### Source struct maps and sets — 2026-10-09

- Added natural table<K; V>/table<T> conversion in mapped authored source structs,
  including nested structs and struct-array elements, with owned output tables.
- Map keys/set elements retain int/string authoring rules; map values retain the
  existing scalar/Name rules. Native type markers and source identities are checked.
- Generalized call-frame wildcard validation for arrays and scalar hash fields;
  required slots, UE duplicate detection, resize invalidation and sparse outputs
  are covered. Container-valued hashes remain rejected.
- Standalone authored struct-array signatures, reflected interface storage and
  broader authoring out/ref remain subsequent stages. General migrations stay
  deferred. See DSLSourceHashValues.md.


### Standalone authored struct arrays — 2026-10-09

- Added ordinary array<ue_struct> function input/result and out/ref signatures
  with explicit registry/folder identity and real authored struct-array BP pins.
- Internal reflection-built carriers preserve exact element/layout identity,
  bounded owned snapshots and GC; reversible paths support return-only/out calls.
- Natural interface facades convert array elements and independent output arrays
  before mutable copyback; Actor/ProcessEvent and direct runtime tests cover it.
- Reflected interface fields/parameters and broader authoring out/ref remain next.
  General migrations remain deferred. See DSLAuthoredStructArrays.md.


### Reflected interface references — 2026-10-09

- Existing native/authored nominal interface wrappers infer real interface fields
  and input/result BP pins; Class vs Interface type resolution stays explicit.
- Shared checked object/interface property access preserves Blueprint-only object
  semantics, native interface pointers, null/defaults and GC lifetime.
- Blueprint wildcard frame conversion and generic reflected call marshalling retain
  exact interface identity. Source structs can contain interface reference fields.
- Native/authored holder probes, ProcessEvent/runtime roundtrips, generated calls,
  GC/weak lifetime and actor destruction are covered. Interface out/ref and wider
  collections/VM reentry stay separate; migrations remain deferred.
  See DSLInterfaceReferences.md.


### Extended authoring out/ref — 2026-10-09

- Added object/interface, authored/raw enum/struct and math mutable parameters
  using existing var T& and @ue_out annotations, with valid type-specific defaults.
- Outputs use strict kind/identity/value/bounds validation and GC ownership;
  Blueprint abort on failure prevents partial caller copyback.
- Source facades convert mapped outputs before any argument updates. Native
  ProcessEvent and runtime tests cover every new type plus negative failures.
- Broader lifecycle/world integration and an end-to-end Actor/Component example
  remain next; structural migrations and VM reentry remain deferred.
  See DSLExtendedOutRef.md.


### Actor + native script Component example — 2026-10-09

- Added a Blueprint-spawnable ActorComponent lifecycle adapter with independent
  VM ownership, owner-handle access and managed owner-delegate subscription.
- Example Actor is created from ue_class DSL with an SCS ScriptComponent; defaults,
  saved Blueprint/map, editor reopen and packaged script staging are checked.
- Two-world isolation, component/Actor destruction and real PIE Tick/EndPIE cleanup
  are covered. Component class DSL/default-subobject syntax and structural migration
  remain separate work. See ActorComponentExample.md.

### Authored ActorComponent classes — 2026-10-09

- ue_class accepts the native ActorComponent parent via the shared resolver and
  produces a Component Blueprint on the fixed UDasScriptComponent host.
- Reflected fields/defaults and methods use the existing typed frame backend,
  including ref/out. Component source/lifecycle defaults are installed by import.
- The Actor/Component example now uses an authored Component Blueprint. Body-only
  reimport avoids Blueprint recompilation; declaration migration stays deferred.
- Component composition/default-subobject DSL and SceneComponent-specific behavior
  remain subsequent steps. See DSLActorComponents.md.

### Actor component composition — 2026-10-09

- Added repeatable ue_component(name, script, type) annotations on Actor classes,
  explicit authored source/type references and derived component stable IDs.
- Import creates/reuses dependent Component Blueprints and builds Actor SCS nodes;
  repeated instances of one type have distinct templates. Asset ownership/path
  conflicts are refused before Actor creation.
- Existing Actor/Component probes now rely on DSL composition instead of manual
  SCS setup. Body-only reimport remains; migrations of composed Actor declarations
  are guarded until component-aware preservation exists.
- Typed component access, instance defaults, native/SceneComponent declarations and
  attachments/transforms remain next. See DSLComponentComposition.md.

### Lifecycle and event class methods — 2026-10-09

- Standard BeginPlay/Tick/EndPlay methods are inferred without annotations and
  rewritten to the existing persistent runtime callback ABI with direct self access.
- Added ue_event source/delegate mapping and automatic bind_events generation;
  the current event contract is one by-value int and void result.
- Callback-only classes are supported; newly created Actor/Component Blueprints
  receive lifecycle source defaults automatically. Callback declaration identity
  participates in owner/reload validation.
- Typed access to fields of other authored objects and wider delegate signatures
  remain follow-ups. See DSLClassCallbacks.md.

### Typed authored object field references — 2026-10-10

- Added nominal ue_object_ref declarations resolved from explicit source/class,
  ue_ref factory syntax and direct scalar field reads/assignments in class bodies.
- References retain weak context-local handles and validate cooked class identity,
  current field kind and permissions on each operation. Existing imports receive
  identity on body-only reimport; no new adapter is needed per authored class.
- Owner example now uses actor.Score += 1. Tests cover six scalar kinds, aliases,
  typed globals, another instance of the same class, forged/wrong/null/destroyed
  handles and readonly/unknown fields.
- Authored method dispatch, component members, richer field kinds and native views
  remain next. Structural migration remains deferred.

### Additional authored reference value fields — 2026-10-10

- Added whole enum, struct snapshot and object/interface field reads/assignments
  through typed authored reference member syntax, using existing value ownership.
- Writes validate exact enum/struct identity and object property membership before
  mutation; struct reads own independent GC-aware snapshots.
- Natural value mappings, nested access/containers and authored method dispatch
  remain next. See DSLAuthoredReferenceValues.md. Migrations remain deferred.

### Component owner shorthand — 2026-10-10

- Added ue_owner(type<Reference>) in ActorComponent DSL bodies, lowering to the
  existing checked owner/reference APIs. The main example now uses this form.
- Compile checks enforce component context and Actor reference target; runtime
  keeps authored identity and weak liveness validation. No runtime ABI changed.

### Component field syntax and metadata — 2026-10-10

- Current examples use imported reference types and @ue_component fields instead
  of class-level name/script/type declarations. The collector supplies reference
  initialization while Unreal creates the actual SCS component instance.
- Added VisibleAnywhere/EditAnywhere and BlueprintReadOnly/ReadWrite component
  flags, separate Category/DisplayName/ToolTip metadata and persistent compiler
  replay on generated properties. Legacy class annotations remain compatibility.
- Metadata is part of declaration identity; existing migration guards remain.
  Component access/defaults, attachments and richer component types stay separate.

### Typed self component access — 2026-10-10

- self.Component returns the real component's typed weak view. Direct field reads,
  assignments, local aliases and compound writes reuse checked authored adapters.
- Compound receivers are evaluated once. Readonly reference replacement and unknown
  fields reject; destroyed component references fail safely. Method dispatch is next.
- Synthetic probe worlds now initialize Actor gameplay to enable ProcessEvent;
  Actor post-initialization alone did not enable Blueprint calls.

### Universal authored-reference properties — 2026-10-10

- Replaced FDSLAuthoredRefVisitor and lexical type/assignment tracking with generated
  native daScript property operators and compiler-resolved receiver types.
- Ordinary imported functions now perform the same primitive and value-field
  operations as DSL methods. Factories/type-name normalization are shared too.
- self remains an execution-context bridge; component references use common
  property operators. Low-level interop APIs are not removed.

### Typed authored method calls — 2026-10-10

- Compiler-resolved method syntax now generates receiver-first overloads for
  authored reference types, shared by DSL methods and ordinary functions.
- Reflected call frames validate current identities/signatures. Scalars, Name,
  whole enum/struct/object values, void and ref/out copyback are implemented.
- Checked nested calls preserve executing VM stacks and pending output/call
  frames, defer reload of active contexts, propagate recoverable failures and
  bound recursive dispatch. Blueprint dispatch still uses ProcessEvent.
- Automatic source type mappings, container method adapters and automatic
  reference types/import discovery remain next work. General migration stays
  deferred. See DSLAuthoredMethodCalls.md for supported syntax and boundaries.

### Natural authored method values — 2026-10-10

- Authored enum/struct argument, return and ref/out types are preserved by the
  reference method emitter. Qualified source descriptors live only within the
  current compile chain; nested compiler AST pointers are not retained.
- Existing source-value converters and reflected frames perform the transfer;
  the caller uses ordinary enum/struct values, including nested fields.
- Explicit registry/folder and imports remain. Wrong enum/struct types produce
  compile errors. Failure recovery preserves caller values.
- Native-object/math wrapper signatures, container method adapters and automatic
  reference/import discovery remain next; general migration stays deferred.

### Typed authored container method calls — 2026-10-10

- Reference overloads preserve supported array/map/set source types for inputs,
  results and ref/out. Arrays of authored structs and container fields inside
  nested authored structs use existing source converters and owned snapshots.
- Whole snapshot inputs share reflected call frames, bounded validation and object
  pinning. Blueprint physical by-reference container inputs normalize to logical
  DAS in semantics; only explicit ref/out values are published back to callers.
- Output conversion completes before move publication. Recovered failures preserve
  caller containers. Wrong element types reject during compilation.
- Existing carrier limits remain. Natural native-object/math method signatures and
  automatic references/import discovery are next; general migration stays deferred.

### Native object/interface and math method signatures — 2026-10-10

- Reference overloads retain generated native object/interface wrappers and
  UEVector/UERotator/UETransform for inputs/results/ref/out. Source descriptors
  remain compile-chain names; native wrappers reuse existing checked converters.
- ue_ref accepts native object/interface wrappers as well as authored views.
  Math adapters copy native values into the shared reflected frame and validate
  identity, finite values and normalized transforms before publishing outputs.
- Null/ref/out and recovery behavior retain existing checked capability semantics.
- Authored views in reflected signatures and automatic reference/import discovery
  remain next; general migration stays deferred.

### Authored reference method signatures — 2026-10-10

- Author Actor/Component views support parameters/results/ref/out through the
  common reference/object frame path, including null and same-class references.
- Native DAS-host reflected storage avoids Blueprint bootstrap for self types;
  optional authored_id in type IR preserves logical identity and change detection.
- Packing/unpacking checks stable identity and liveness, including Blueprint entry.
- Concrete Blueprint-class pins/class catalogs, authored field ownership and
  automatic references/import discovery remain next; migration remains deferred.

### Native math ue_class properties — 2026-10-10

- UEVector/UERotator/UETransform are native struct properties with zero/identity
  defaults or finite literal constructors. Save/reopen/cook use existing schema.
- self reads/whole writes and Vector/Rotator scalar member writes share checked
  snapshots; authored view property operators return/accept native math values.
- readonly/type checks and math validity remain enforced. Generic struct members,
  direct class container fields and automatic discovery remain next work.

### Native math authored struct members — 2026-10-10

- ue_struct supports native UEVector/UERotator/UETransform members, with the same
  literal default rules as class properties. Native types need no registry rows.
- Type validation and editor asset construction distinguish builtin native
  structs from authored dependencies; exact native identity and defaults are
  checked on reuse. Existing nested/array-of-authored-struct snapshot adapters
  reuse checked native math converters.
- Direct math containers, nested self member writes, ordinary authored-reference
  field ownership and automatic discovery remain separate work.

### Nested self math scalar writes — 2026-10-10

- self authored struct fields preserve the full path when writing nested
  Vector x/y/z or Rotator pitch/yaw/roll; assignment/compound operations reuse
  existing scalar snapshot commits. Readonly/type validation remains.
- Snapshot validation now checks native math values recursively, including
  nested/array children, before publishing updates. Recovered invalid updates
  preserve the field's previous value.
- Indexed container writes, direct math containers, typed-view in-place writes,
  authored-reference field ownership and automatic discovery remain separate.

### Natural authored property values — 2026-10-10

- Common authored-reference property operators retain source enum/struct types,
  using compile-chain qualified-name/scope descriptors and the existing method
  source resolver. No nested AST pointers or new runtime storage format.
- Getter copies and whole setters reuse checked pack/unpack helpers for nested
  structs, math values and supported containers. Nominal type, readonly and asset
  identity/layout/default validation remain enforced; explicit imports remain.
- Nested writes through foreign views, direct class containers, authored-reference
  field ownership and automatic reference/import discovery remain separate.

### Typed native object/interface properties — 2026-10-10

- Common source property descriptors preserve native object/interface wrapper
  types. Getters/setters now compare the reflected property's native class path
  to its declared path before transfer, including null assignments.
- Native null/liveness/type checks and readonly operators remain; destroyed Actor
  properties normalize to empty wrappers. Component views retain their separate
  authored identity validation within the same accessor implementation.
- Ordinary authored-reference field ownership, object containers, soft/class
  references and automatic discovery remain separate work.

### Authored enum members in structs — 2026-10-10

- ue_struct supports same-module authored enum members with explicit named
  defaults, including enums without zero. Model validation checks references
  and members; editor creation seeds the enum default before compiling the field.
- Source snapshot helpers validate native enum identity/defaults and transfer
  enum members in nested/array-of-authored structs, properties and method ref/out.
- Direct self enum leaf writes, standalone enum containers, generated native enum
  source types and cross-module reflected enum dependencies remain separate.

### Direct self enum member assignment — 2026-10-10

- self nested authored enum assignments use typed source commit helpers and the
  existing latest-snapshot member update. Source nominal type checks precede
  enum packing; other enums and integers cannot silently supply their numbers.
- Registry/folder identity, runtime enum checks and readonly roots remain.
  Compound enum arithmetic, indexed self container writes and foreign-view
  nested writes remain separate/unsupported as documented.

### Direct self native object/interface member writes — 2026-10-10

- Object/interface and enum leaves share a source-typed commit helper generator.
  Nested self native reference assignments reuse existing snapshot member writes,
  including null; nominal RHS, readonly and runtime reference checks remain.
- Destroyed Actor members normalize to empty views and can be cleared; other
  struct members remain intact. Ordinary Unreal ownership is unchanged.
- Authored-reference member ownership, soft/class refs, indexed self mutations
  and foreign-view nested writes remain separate work.

### Whole nested self struct/math values — 2026-10-10

- The common typed leaf commit supports authored structs and native math values,
  reusing nested snapshot replacement and whole-root commit. Nominal source type,
  native identity/math validity and readonly checks remain.
- ExprClone is handled for self properties/members. Container-bearing structs
  use native DAS := semantics; sibling data and independent containers persist.
- Indexed self mutations and foreign-view nested writes remain separate work.

### Whole self container member replacement — 2026-10-10

- The common typed leaf helper supports source array/map/set members with :=,
  generating missing carrier converters within the owning compile scope.
- Nested snapshot replacement accepts exact matching native carrier Values,
  with owned CopyCompleteValue, aggregate validation and whole-root commit.
- Source type/element and readonly checks remain; empty replacement clears.
  Direct class containers, new container shapes, indexed/live self mutation
  and foreign-view nested writes remain separate work.

### Authored class reference fields — 2026-10-10

- Nullable ue_class object properties retain authored_id and use common native
  Actor/Component host storage, as for signatures. Source typed self/property
  setters and nullable stable-ID converters guard values before transfer.
- Self references avoid concrete Blueprint bootstrap. Ordinary Unreal ownership
  remains; concrete class pins/pickers are separate. Ordinary Apply blocks
  changes to existing logical authored field identities.
- Authored struct members, richer ownership/default instancing and automatic
  imports/reference catalogs remain separate work.

### Self container insert/erase/clear — 2026-10-10

- Typed helpers perform supported set/map insert, array/table erase and clear
  using standard DAS operations on owned local containers, then snapshot commit.
  Table erase returns bool; array erase/clear/insert preserve void semantics.
- Exact RHS types, readonly, identity and aggregate validation remain. Operands
  are evaluated before native read; no live UE container view is introduced.
- Indexed assignments, push/pop, int64 array overloads, iterators and foreign-view
  container mutations remain separate work.


### Indexed containers and typed-reference mutations — 2026-10-10

- Indexed self array/map assignment, compound arithmetic, authored element fields
  and math leaves use owned root snapshots with publication after validation.
- Added element push/push_clone (optional insertion index), pop and map insert_clone.
  Array index, insertion and erase adapters accept int/int64 overloads.
- Compiler-resolved authored references support equivalent container operations
  and nested field assignments in ordinary functions, preserving receiver identity,
  source types, readonly annotations and bool erase results.
- Existing supported container shapes remain unchanged. Direct class containers,
  multiple indexed self levels, consuming moves/live iterators and mutable argument
  projections are still separate work. See DSLIndexedContainers.md.


### Direct ue_class container fields — 2026-10-10

- Direct authored array/map/set properties use existing supported carrier shapes
  and implicit empty defaults. Authoring JSON retains recursive container types;
  the Actor adapter derives native Blueprint container pins from the carrier.
- Common property snapshot adapters handle both structs and real native containers.
  Self/reference getters, replacements, indexed writes and operations preserve
  source types, native identity, readonly and atomic publication.
- Runtime owner validation checks container identity and empty class defaults.
  Body-only reimport is supported; declaration/default migrations of classes
  containing direct containers are explicitly deferred before rollback capture.
- New element/value families, nonempty defaults, mutable projections, live
  iterators and multiple indexed self levels remain separate work.
  See DSLClassContainers.md.


### Multi-level indexed self mutations — 2026-10-10

- Replaced the single-index self helper with full field/index path reconstruction.
  Assignments, compound writes, container replacement and operations below indices
  commit one validated owned root snapshot.
- Generic int|int64 arguments support mixed index widths independently; map keys
  and leaf values retain exact source types. No exponential overload generation.
- Existing array-of-authored-struct shapes can contain other supported containers.
  New native shapes, mutable projections, live iterators, consuming moves and
  declaration/default migrations remain separate work. See DSLMultiIndexSelf.md.


### Native math arrays — 2026-10-10

- Supported Vector/Rotator/Transform arrays in authored class/struct properties
  and reflected input/result/ref/out signatures with native Blueprint element types.
- Existing dynamic struct-array carriers now also accept an explicit native math
  allowlist; layout identity/cache and path validation remain in place.
- Natural pack/unpack uses existing math value adapters for every element, with
  owned copies, readonly/type validation and atomic native publication.
- Enum/object arrays, other native struct arrays, new map/set families and
  declaration/default migrations remain separate work. See DSLMathArrays.md.


### Authored enum arrays — 2026-10-10

- Added exact authored-enum array carriers for class/struct fields and reflected
  input/result/ref/out signatures, retaining native enum Blueprint identity.
- Source converters use checked typed enum values per element. Snapshot validation
  now rejects unknown/reserved enum elements; resizing seeds only new elements
  with a valid first member, supporting sparse enums without zero.
- Carrier cache validation includes enum members and stable IDs. Source nominal
  typing, readonly, snapshot bounds and atomic publication/copyback remain.
- Native enum arrays, enum maps/sets, object arrays, nonempty defaults and general
  type/declaration migration remain separate work. See DSLEnumArrays.md.


### Native object/interface arrays — 2026-10-10

- Added native-class/interface array carriers, exact Blueprint pins and natural
  typed element conversion for class/struct fields and input/result/ref/out.
- Snapshot validation checks live element class/interface constraints. Existing
  FGCObject snapshot/frame collectors retain references while DAS wrappers use
  weak handles; null and destroyed-Actor reads follow existing checked semantics.
- Existing indexed/multi-level mutations, owned replacement and readonly/source
  nominal checks apply to the new arrays.
- Authored object/interface view arrays, soft/class arrays, new map/set families,
  ownership/default extensions and structural migration remain separate work.
  See DSLReferenceArrays.md.


### Authored class reference arrays — 2026-10-10

- Class properties and input/result/ref/out arrays preserve authored element ID
  and validate every source element while sharing physical Actor/Component hosts.
- Source-aware converter/helper keys prevent nominal wrapper collisions on one
  native carrier; null, readonly and atomic publication/copyback remain.
- Parameter contract verification now matches native properties by name, allowing
  Blueprint's physical input/out ordering to differ from source argument order.
- Authored struct member bootstrap, authored interface arrays, concrete class pins,
  nonempty defaults, ownership extensions and structural migration remain deferred.
  See DSLAuthoredReferenceArrays.md.


### Struct and math map values — 2026-10-10 (validation pending)

- Added int/string-key maps with Vector/Rotator/Transform or authored ue_struct
  values across class/struct properties and input/result/ref/out declarations.
- Dynamic carriers preserve key/value identity and layout fingerprints; source
  converters use per-value owned snapshots. Nested snapshot bounds/math checks,
  Blueprint struct value pins and reflected dependency ordering were extended.
- Added complete type/class examples; no build, tests, PIE or cook were run for
  this batch at the user's request. See DSLStructMaps.md for pending validation.
- Object/enum map values, new key families, live projections and migrations remain
  separate work.


### Enum/native-reference/authored-class map values — 2026-10-10 (validation pending)

- Added int/string-key maps with authored enums, native object/interface wrappers,
  and authored class views for class fields and input/result/ref/out. Struct
  members accept enums and native references; authored views there remain deferred.
- Native carriers, exact Blueprint pins, enum/reference snapshot validation and
  source converters extended together. Nominal reference helper keys now cover
  both arrays and map values, including alternate native wrappers.
- Added examples and pending validation checklist. No build, tests, PIE or cook
  were run. See DSLReflectedMaps.md.
- Native enum maps, authored interface views, new keys, sets, live projections
  and structural migration remain separate work.


### Authored source catalog and reference import generation — 2026-10-10 (validation pending)

- Added an explicit class source catalog; ue_object_ref can resolve a simple name
  or stable ID without script. Canonical source keys preserve existing accessors,
  components and container behavior; expected stable IDs are checked on resolution.
- Catalog bytes join persistent compile dependency tracking. Existing runtime
  staging includes the JSON files. Unknown/ambiguous identities fail explicitly.
- Added DasSourceCatalog commandlet for compiler-model-based manifest and reference
  module generation from explicit source lists, plus non-writing -Check.
- One generated require imports class wrappers; value-type imports and broader
  enum/struct/interface catalog bootstrap remain separate work. No build/tests run.
  See DSLSourceCatalog.md and dsl_catalog_access.das.


### Catalog value modules and consolidated imports — 2026-10-10 (validation pending)

- Catalog v2 adds compiler-inventoried enum/struct modules and their type identities;
  v1 manifests remain readable. Dedicated value modules are selected by TypeScripts.
- Generated reference modules publicly import these modules, making one require
  sufficient for selected class wrappers and value types. Type-only catalogs allowed.
- Added module/source/ID/name-collision and self-import/input-overwrite guards;
  examples now use consolidated imports. Automatic transitive source discovery,
  authored interface catalogs and asset creation remain separate. No build/tests run.
  See DSLSourceCatalog.md.


### Compiler-based catalog value dependency discovery — 2026-10-10 (validation pending)

- Selected class roots now inventory imported enum/struct modules from the compiler
  library, including transitive imports. No directory or textual source scanning.
- Generator merges shared/manual dependencies only when source/module/type inventory
  agrees; automatic imports are sorted. ExplicitTypeImports retains manual selection.
- Local source, dedicated value module and output-overwrite boundaries remain.
  Class roots, interface catalogs and separate reference-source bootstrap are still
  explicit. No build, commandlet or tests run. See DSLSourceCatalog.md.


### Catalog interface declarations and existing bindings — 2026-10-10 (validation pending)

- Catalog v3 inventories ue_interface definitions and ue_interface_ref view modules;
  v1/v2 remain readable. Automatic class discovery and InterfaceScripts selection
  collect them plus value dependencies through one shared compiler collector.
- Interface definitions/views share logical IDs; duplicate definitions and public
  name collisions fail. Binding views replace redundant declaration-only imports
  in the aggregate while the manifest keeps the full inventory.
- Existing reflection generator remains responsible for interface call adapters;
  source catalogs do not create assets or bypass native interface identity checks.
  Added declaration example/documentation; no build, generation or tests run.


### Managed timers and class callback names — 2026-10-10 (validation pending)

- Added named UE TimerManager schedules scoped to owner/script sessions: replace,
  cancel, pause/resume, active/remaining queries. Exported void callbacks, weak
  ownership, bounded idle dispatch and callback failure cancellation implemented.
- ue_class short literal callback names lower to existing annotated method exports.
  EndPlay/release/world teardown/reinstancing/accepted reload cancel schedules;
  rejected reload retains the old VM. Restart after reload remains explicit.
- Added DSL/plain-function examples and limitations. No build/tests/PIE/cook run.
  Wider delegate callbacks, captured timer args, input and async/latent are next.
  See ManagedTimers.md.


### Scalar/void dynamic delegate callbacks — 2026-10-10 (validation pending)

- Extended shared managed subscription/proxy/queue/lifecycle path from int32 to
  float32, bool, owned FString and zero-argument BlueprintAssignable events.
- ue_event source signatures select adapters automatically. Callback metadata
  records non-int parameter_kind while preserving legacy int JSON output.
- Added native probe-host signals and DSL example, exact signature/queue checks;
  no build/UHT/tests run. Multiple arguments, object/struct payloads, native/single
  delegates and authored delegate declarations remain separate. See DSLScalarEvents.md.


### Double, Vector and native object event payloads — 2026-10-10 (validation pending)

- Extended the shared dynamic multicast bridge with double64, by-value FVector and
  exact native UObject* signals. Typed native object callbacks lower to checked
  handle transport/unpack; queued object records temporarily pin against GC.
- Added probe-host signals/emitters and DSL example. Derived-class native delegate
  signatures, authored/interface views, const Vector refs and multi-argument events
  remain separate. No UHT/build/tests run. See DSLValueEvents.md.


### Math const-reference and checked object event views — 2026-10-10 (validation pending)

- Added Rotator/Transform values and Vector/Rotator/Transform const-ref proxies,
  copied owned transport and math validity checks. Subscriptions retain the exact
  proxy function used for registration/removal.
- UObject native payloads can use interface/authored source wrappers, lowering
  through existing checked converters and stable IDs. Native property signature
  remains exactly UObject*. Added three source examples and support boundaries.
- No UHT/build/tests run. Multiple arguments, derived/interface native signatures,
  enum/struct payloads and authored delegates remain separate. See DSLMathReferenceEvents.md.


### Name and primitive array event payloads — 2026-10-10 (validation pending)

- Added FName and const-input TArray<int32/double/FString> proxy payloads, exact
  carrier/element checks, owned native/DAS copies and bounded string array transport.
- Event dispatch now tracks execution depth and collects temporary struct snapshots
  on exit. Added probe signals/emitters and DSL example. No UHT/build/tests run.
- Other array families, map/set/struct/enum and multi-argument delegate payloads
  remain separate. See DSLArrayEvents.md.


### Captured scalar/name/math timer arguments — 2026-10-10 (validation pending)

- ue_timer_start adds one immutable captured int/float/bool/double/string/name/math
  value with persistent frame delivery and exact void callback contract checking.
- Class short callback names support zero/one supported by-value argument, including
  forward declarations. Captured strings/math validation precedes replacement.
- Added example and ownership/reload boundaries; no build/tests run. Object/struct/
  container/enum captures, multiple args and closures remain separate.
  See ManagedTimerArguments.md.


### Captured primitive array timer arguments — 2026-10-10 (validation pending)

- Class timers accept one saved array<int/double/string> through existing exact
  owned snapshot converters and persistent struct frame delivery. Timer records
  own native copies independently of originating context tokens/caller arrays.
- Carrier/string/snapshot/contract checks precede replacement. Example includes
  caller mutation, repeating local mutation and one-shot replacements.
- No build/tests run; other arrays, object/struct/container capture families and
  plain exported array callback lowering remain separate. See ManagedTimerArrays.md.


### Named next-tick scheduling and timer queries — 2026-10-10 (validation pending)

- Added TimerManager next-tick requests with the same named owner/session scope,
  replacement/capture/contract checks and lifetime cleanup as interval timers.
- Shared source helper handles both callback argument layouts; all existing capture
  families retained. Added paused/elapsed queries and current-session cancel-all.
- Example/documentation added; no build/tests run. UE pending/paused timing,
  overload ABI, rearm, cancellation, reload and packaged checks remain pending.
  See ManagedNextTickTimers.md.


### Managed legacy Action/Axis input — 2026-10-10 (validation pending)

- Added owner/script-scoped bindings on existing Actor InputComponents (Component
  owners use their Actor): action phases, float axes, consume/paused flags, ready
  query and selective/session-wide unbind. Existing proxy/idle dispatch reused.
- Exact action handles and proxy-specific axis removal preserve unrelated bindings.
  Lifecycle cleanup/accepted reload removes bindings; explicit rebind remains needed.
- Added DSL example and setup boundaries; no mappings/assets/build/tests changed
  or run. Enhanced Input, possession hooks and input configuration remain separate.
  See ManagedLegacyInput.md.


### Managed Enhanced Input bindings — 2026-10-10 (validation pending)

- Added typed InputActionRef source module and owned asset/trigger binding records
  on existing EnhancedInputComponents. Boolean/Axis1D/Axis3D select exact bool/float/
  Vector void handlers, using the reflected dynamic proxy and shared queued dispatch.
- Asset GC pin, uint32 binding-handle removal, callback lowering and lifecycle cleanup
  implemented; shared input cancel-all/count limit retained. Plugin/runtime dependency
  declared and module explicitly loaded. No project mappings/contexts/assets changed.
- Added example/documentation; no UHT/build/tests run. Axis2D, context APIs and
  automatic possession/component hooks remain separate. See ManagedEnhancedInput.md.


### Explicit LocalPlayer mapping context ownership — 2026-10-10 (validation pending)

- Added typed PlayerController/MappingContext views and explicit add/update/query/
  release/clear APIs, validating local controller/world/subsystem readiness.
- Session context records pin assets, reject acquiring externally installed contexts
  and preserve externally changed priorities on cleanup. Lifecycle/reload teardown
  releases owned records after handlers. Cooperative ownership boundaries documented.
- No assets/project settings/build/tests run or changed. Added source example;
  real input, priority/rebuild/ownership and multi-player validation remain pending.
  See ManagedMappingContexts.md.


### Vector2D source/runtime and Enhanced Input Axis2D — 2026-10-10 (validation pending)

- Added copied UEVector2D, x/y and constructor/default support; actual native struct
  identity with existing kind-12 snapshot transport for properties/calls/ref/out.
  Existing math array/map carriers recognize native Vector2D without ABI renumbering.
- Added value/const-ref event proxies and timer/next-tick captures; Enhanced Input
  Axis2D uses exact Vector2D snapshots and typed callback lowering.
- Added source example/documentation. No build/UHT/tests run. Generator/AOT coverage,
  frame/ABI parity and all gameplay execution remain pending. See DSLVector2D.md.


### Desired input/context state restoration — 2026-10-10 (validation pending)

- Retained successful legacy/enhanced binding and owned context recipes. Idle world
  tick reconciliation replaces stale component handlers and restores accepted-reload
  state through existing signature/ownership checks, without replaying BeginPlay.
- Missing components/subsystems defer; errors retain recipes with bounded retries.
  Explicit cancellation and EndPlay/release/teardown discard desired state/assets.
- Restores last installed configuration, not edited setup-call execution. No build/
  reload/input tests run; lifecycle, delegate ABI and conflict/GC/performance checks
  remain pending. See ManagedInputRestore.md.


### Native SceneComponent gameplay adapters — 2026-10-10 (validation pending)

- Added checked SceneComponentRef, Actor root/parent, relative/world transforms,
  attach/detach with socket/cycle/world checks and explicit native rules.
- Native game-world USceneComponent creation registers Actor-owned transient instances;
  tracked same-owner destruction refuses existing/foreign components and children.
- Source module/example added; no build/tests run. Authored SceneComponent hosts,
  arbitrary native classes, sweeps/replication and structural remapping remain separate.
  See DSLSceneComponents.md.


### Managed world latent delay — 2026-10-10 (validation pending)

- Added named delay/replace/pending/cancel APIs through LatentActionManager, private
  UObject completion targets and shared idle void dispatch. Completion record is
  consumed before user rearm; exact-target removal follows session lifecycle.
- Export contract validated before replacement; game-world/duration/count guards and
  native proxy lifetime pin implemented. No coroutine/await or worker execution.
- Added DSL example and pending signature/timing/cleanup checks; no build/tests run.
  Async asset/task adapters, payloads and coroutine syntax remain separate.
  See ManagedLatentDelay.md.


### Managed Streamable Manager async assets — 2026-10-10 (validation pending)

- Added named load/replace/pending/cancel API, stalled registration, private weak
  completion targets and game-thread object dispatch. Typed native callback class
  contracts, nullable failures and handle/result lifetime ownership implemented.
- Session/reload/teardown cancels requests without touching shared engine loads;
  queued results pin assets through delivery. Cook inclusion remains project policy.
- Added source example/documentation; no build/loading/tests run. Futures/await,
  structured results, multiple-asset progress and soft-reference DSL remain separate.
  See ManagedAsyncAssets.md.


### Integrated gameplay source example — 2026-10-10 (scene/validation pending)

- Added coherent Actor/Component sources with catalog-backed refs, component timer/
  owner signal, Axis2D controls, pivot SceneComponent motion, latent Ready and async
  asset result. Natural fields/method calls and public native adapters used together.
- Added isolated setup manifest and scene/reload/cook instructions under
  Examples/GameplayDemo. This custom manifest is not an Unreal importer format.
- No Blueprint/input assets/map, build, import, PIE or cook created/run. Executable
  scene and coupled behavior verification remain required; no completion claim.
  See Examples/GameplayDemo/README.md.

### Gameplay runtime validation — 2026-10-10

- Editor build and new DasForUE.Gameplay.RuntimePIE automation passed with no
  automation warnings/errors. Timers/captures, typed delegates, scene operations,
  latent delay, async success and Destroy/EndPIE cleanup are exercised together.
- Scene validity/access rejects components already being destroyed; cyclic
  attachment and destroyed-component reads are tested. Tests use unsaved packages.
- Input dispatch/context restoration, managed-operation reload and async negative
  paths remain the next validation batch. Packaged gameplay and a playable demo
  map remain separate. See GameplayRuntimePIEValidation.md.
### Native declaration groups — 2026-10-11

- Added properties = ue_properties(flags, metadata) { var Field:Type=Default }
  in ue_class and ue_struct, plus components = ue_components(...) for Actor SCS
  fields. AST expansion occurs before inference; labels are erased. Existing
  property/component collectors remain the only IR/runtime implementation.
- Supports multiple groups, individual fields, per-member metadata overrides and
  existing stable IDs. Rejects duplicate fields, unknown/conflicting flags,
  executable statements, inferred/reference fields and empty/nested blocks.
- Converted both main Actor/Component and GameplayDemo sources. Foundation IR
  equivalence/negative checks and main Actor/Component create/reopen/PIE passed.
- Ordinary def and enum/class/interface declarations are forbidden by the pinned
  expression-block grammar. Those entities keep their annotations until another
  valid group syntax is chosen; no reader macro or parser modification was added.
  See DSLDeclarationGroups.md.
### Functions/events parity audit — 2026-10-11

- Compared the specified Hazelight functions/events page with current code and
  four temporary compiler probes. Callable methods and fixed lifecycle hooks
  work; parity is incomplete. See AngelScriptFunctionsEventsAudit.md.
- Prioritize usable script-only self method calls, then child Blueprint override/
  parent-call tests and explicit overridable-event dispatch policies. Existing
  ue_event multicast subscriptions are not BlueprintOverride/BlueprintEvent.
- Authored class inheritance/Super, global reflected function libraries, optional
  callable defaults, comment tooltips and parent-event aliases remain extensions.
- Generated BP graph functions already receive BlueprintEvent through UE. Audit
  the current behavior before replacing it; full child-BP dispatch is not verified.
## Actor/Component expansion checkpoint — 2026-10-11
Native component fields and root/attachment declarations implemented; Editor integration test passes. See [DSLNativeComponents](DSLNativeComponents.md) for the exact validation boundary and remaining authorized stages. Component defaults, general creation/query/spawn APIs, user construction and inheritance overrides are still open.

## Component template defaults — 2026-10-11
Implemented literal component defaults through ue_defaults, native scalar/vector/rotator template properties and authored scalar overrides. See [DSLComponentDefaults](DSLComponentDefaults.md). Defaults migration and arbitrary method calls remain deferred; component lookup/create/spawn and construction/inheritance remain open.

### SceneComponent named lookup (2026-10-11)
Added `ue_scene_get` and `ue_scene_get_or_create` with nullable lookup and existing creation/lifetime rules. Integration coverage lives in `DasForUE.Components.Native`. This is the SceneComponent slice; generalized typed ActorComponent creation, authored component resolution, enumeration and Actor spawning remain separate steps.

### Own methods and explicit Blueprint events — 2026-10-11

- Lowered script-only helpers and ordinary own reflected calls into typed
  same-context DAS bodies. Helpers do not enter reflection or authoring IR.
- Added ue_function(BlueprintEvent), optional true-only blueprint_event JSON,
  structural role comparison, and self event calls through the existing authored
  reference/ProcessEvent transport. Callable functions are Final; events and
  interface implementations remain overridable through an editor precompile policy.
- Real child-BP tests cover inherited default execution, override+Call to Parent,
  replacement without parent, and runtime/context cleanup. Core foundation,
  Actor/Component and MethodValues editor/PIE regressions passed.
- Role changes on existing BP are refused before mutation pending dependency
  migration. DAS class inheritance/Super and general native BlueprintOverride
  remain separate; this completes the child-Blueprint parent-call stage only.
  See DSLMethodDispatch.md and DSLMethodDispatchValidation.md.
### Typed native component operations (2026-10-11)
Added type-token create/get/get_or_create for concrete native ActorComponent classes and owned destruction. Authored/Blueprint component resolution, enumeration and Actor spawn remain pending. See DSLTypedComponents.md.

### Authored component lookup (2026-10-11)
ue_component_get now supports authored ActorComponent references with stable type-ID checking. Dynamic authored creation and Blueprint-class resolution remain pending.

### Authored dynamic component creation (2026-10-11)
Added explicit loaded generated-class path to authored create/get_or_create, guarded by CDO stable type identity. Basic game-UWorld integration coverage added; active PIE lifecycle and cooked class availability remain pending.

### Dynamic component lifecycle in PIE (2026-10-11)
DasForUE.Components.DynamicPIE passed: nested BeginPlay before create returns, ticks, one EndPlay on script-owned destruction, unregistration and session cleanup after EndPIE. Fixed runtime guard to allow lifecycle only for the component under adapter registration/destruction. Packaged builds and replication remain unverified.

### Documentation comments → ToolTip

Added the native daScript CommentReader integration: adjacent declaration comments provide default ToolTip metadata for reflected types, fields/components, methods and interface stubs. Explicit ToolTip remains an override. Examples use comments; enum-entry tooltips remain outside this step. See [DSLCommentTooltips.md](DSLCommentTooltips.md).

### Runtime class handles (2026-10-11)
Added ue_class_loaded(type<T>, path), UEClassHandle, validity/path APIs and component creation through checked class handles. Resolution and use validate native base and authored identity. DynamicPIE passed with wrong-base/identity/missing-class/wrong-create rejection checks. This is runtime scope: reflected FClassProperty, Details picker, serialized defaults, soft loading and cook dependency collection remain next. See DSLClassReferences.md.

### Reflected class properties (2026-10-11)
UEClassHandle fields with ue_class_base now become FClassProperty/PC_Class constrained by native base. Runtime reads/writes are checked; unchanged reimport preserves editor-selected class defaults. DynamicPIE and ClassReload passed, including saved package dependency recorded by AssetRegistry, fresh-process CDO reload and creation/lifecycle. Syntax defaults, class method parameters, containers, exact authored-ID picker filtering, retired-field/default migrations and packaged-build validation remain deferred.

### Actor spawning and PIE (2026-10-11)
Added ue_actor_spawn(type<T>, classHandle[, transform, owner, collision]) and owned actor destruction. Checked native/authored identity, game world, transform and collision rule; deferred construction installs scoped lifecycle permission for spawned Actor and its authored components. SpawnPIE passed: native Actor, Blueprint child of authored Actor, owner/transform, actor+SCS component BeginPlay/Tick/EndPlay, invalidation and rejection before mutation. Physical collision outcomes, replication and packaged runtime remain unverified.

### Authored dynamic delegates and events — 2026-10-11

Added ue_delegate/ue_multicast signature declarations using normal DAS function
types; typed BindUFunction, Execute/ExecuteIfBound, IsBound, Unbind and multicast
AddUFunction/RemoveUFunction/Broadcast/Clear. Authored multicast fields create saved
Blueprint Event Dispatchers and use universal self/object-reference accessors.
Bindings are weak, single delegates copy their binding value, and VM root collection
reclaims unused values. Structural migration of classes with events is deferred.
The typed ue_bind factory provides direct bound construction. Single delegate class fields, reflected delegate arguments,
ref/out signatures and VS Code handler generation remain separate. See
[DSLAuthoredDelegates.md](DSLAuthoredDelegates.md).

### Actor/component queries by type (2026-10-11)
Added ue_actor_find/find_all and ue_component_find/find_all for native subclasses and authored stable IDs. First is nullable; all materializes an ordinary typed DAS array using temporary weak snapshots, reclaimed on success and error. Queries stay in the owner's game world; component owner must be in that world. SpawnPIE passed missing/multiple matches, native+authored types, same-type Actor in foreign world, rejected foreign component owner, stale array entries and 70 interrupted snapshots. Ordering is unspecified; tags/radius/interface queries, class-handle filters and reflected result arrays remain deferred.

### Blueprint Function Libraries (2026-10-11)
Native static BlueprintCallable library functions now export and generate free DAS wrappers with signature checks. WorldContext is recorded in IR, omitted from wrapper inputs and automatically filled/pinned from script owner; override/mismatched/stale contexts are rejected. FunctionLibraries.PIE passed engine+user C++ functions, two distinct worlds, pure worldless calls, world-required rejection and cleanup. Generator: 90 tests PASS; export policy: 9 checks PASS. Latent/CustomThunk, unsupported parameter shapes, Blueprint-authored libraries and packaged validation remain separate. See DSLFunctionLibraries.md.

### Accumulated validation and cooked gameplay — 2026-10-11

Editor/foundation, 19 gameplay DSL fixtures, 90 generator tests, real PIE suites and packaged Development passed. Fixed class-reference Blueprint pins and cooked SCS root restoration; added reproducible cooked gameplay/map export coverage. See [AccumulatedValidation.md](AccumulatedValidation.md).


### Single-cast delegate fields — 2026-10-11

Added `ue_delegate` fields on authored Actor/Component classes, represented by UE
`FDelegateProperty`. Assignment copies a binding, reads return a value snapshot;
`ue_bind` assignment and empty-value assignment bind/clear the property. Own-object
and typed foreign accessors share this behavior. Readonly properties reject writes;
unsupported editor defaults/EditAnywhere fail explicitly. Generated signature
Blueprints use separate sibling packages and ownership checks; reflected references
retain the cook dependency without loading the owning class recursively.

Validation covers creation/reopening in fresh processes, reflected dispatch, copied
binding independence, foreign accessors, actual PIE including Destroy/EndPIE, and
packaged Development gameplay. Details: [DSLAuthoredDelegates.md](DSLAuthoredDelegates.md).

Next delegate work: reflected method parameters/results, then ref/out in signatures.
Structural/default migration of classes containing delegate/event fields remains
deferred and is rejected by the normal import and explicit field migration commands.


### Delegate-valued reflected methods — 2026-10-11

Added single-cast delegate input parameters/results to callable `ue_function`
Actor/Component methods. The authoring type contains a signature contract and its
identity; saved Blueprint function pins refer to the generated signature asset.
Internal frame transport copies native weak bindings, with receiving-context tokens
created at VM entry. Typed foreign method accessors pack/unpack the value using the
same checked call marshaller. Existing scalar/struct/object frame kinds retain their
numeric values. Native roundtrip, empty binding, cross-owner/context invocation,
fresh-process asset reopen, foundation and actual PIE scenarios are covered.

Next: ref/out payloads in delegate signatures. Delegate-valued ref/out method
parameters, multicast parameters/results, delegate containers and authored interface
method declarations require their own adapters. Structural migration stays deferred.

Final packaged Development method parameter/result scenario passed as well
(TestCookedGameplay, 2026-10-11); rendering disabled with NullRHI.


### Ref/out payloads in delegate signatures — 2026-10-11

Added an empty `def signature(...)` declaration inside ue_delegate/ue_multicast
structs to retain variable-level @ue_out/@ue_ref metadata. Mutable T& infers ref;
@ue_out selects out-only. Existing by-value function-type syntax remains supported;
reference function-type syntax is rejected because daScript discards annotations.
No parser change or reader macro was introduced.

Single-cast calls stage return/output values before publishing caller updates;
unbound ExecuteIfBound preserves variables. Multicast supports ref, rejects out-only
and returns. Native signatures carry proper property flags. Binding additionally
checks frame offsets, and newly generated Blueprint methods put out-only pins before
the return to align their frame layout. Delegate-local converters are private to
avoid duplicate container helper overloads across modules. Old mismatched graph
layouts are rejected; structural migration remains deferred.

Native scalar/string and authored scalar/string/array<double> cases, saved asset
reopening, optional execution and multicast ref are covered. See
[DSLAuthoredDelegates.md](DSLAuthoredDelegates.md).

Final validation passed: foundation with zero failures, method-dispatch regression PIE, gameplay PIE and packaged Development ref/out scenarios (NullRHI).


### Authored class inheritance / Super: first slice — 2026-10-11

Added class Child : Parent for ue_class declarations, parent_id plus owned IR
members, effective-view resolution with conflict/override checks, canonical parent
Blueprint assets and owned-only child emission. Super.Method explicitly dispatches
to the parent's UFunction on the same object. Typed base accessors accept descendant
CDO identities. Imported generated functions now clear isClassMethod when detached
from classParent, fixing a null-parent crash during module symbol-use analysis.

Actor Create/Reopen and foundation scenarios passed, including inherited fields,
event override, parent invocation once, inherited callable and a typed base reference.
Details and limits: [DSLInheritance.md](DSLInheritance.md). Remaining inheritance
work: inherited-component scenarios, lifecycle/constructor chaining, script-only
helpers, richer parent asset selection and ancestor-aware query/class-handle filters.
Structural hierarchy migration remains deferred.

Final first-slice validation passed: foundation (zero failures), fresh-process Create/Reopen, method-dispatch regression PIE, gameplay PIE and packaged Development inheritance/Super scenario (NullRHI).

### Inherited components and parent lifecycle — 2026-10-11

Added inherited SCS/root composition and authored ActorComponent descendants as
component field types. Parent source is retained in IR instead of inferred from an
owned method. Lifecycle callbacks can override parent hooks, call Super explicitly,
or inherit omitted hooks through generated forwarding entries. Parent calls use
separate sessions on the same UObject; destruction and repeated BeginPlay retain
delivery guards. Inherited editable component defaults resolve through ancestors.

Remaining inheritance work: adding/overriding child component nodes, managed event
subscription policy, script-only helpers/constructors, arbitrary parent asset
selection and ancestor-aware queries/class handles. Hierarchy migration stays
deferred. Details: [DSLInheritance.md](DSLInheritance.md).

Validation passed: foundation, Create/Reopen, PIE without warnings, native-component regression and packaged Development, including Tick-only inheritance. Fixed a PIE duplication cycle involving the native fallback root.

### New components in authored child classes — 2026-10-11

Added owned-only child SCS emission with implicit inherited-root attachment and
explicit attachment to inherited/owned scene nodes. Cross-Blueprint attachments
use USCS_Node::SetParent; inherited graphs are not modified. Root/count/cycle and
member-conflict checks operate on the effective class. Updated lifecycle example
adds two native scene nodes and another derived authored ActorComponent with an
independent template default. Replacing inherited nodes and composition migration
remain deferred.

Validation of new child components passed: foundation (zero failures), fresh-process Create/Reopen, PIE (one success, zero warnings/errors) and packaged Development. Both inherited/owned scene attachments, separate authored-component template defaults and BeginPlay/EndPlay delivery are covered.

### Defaults for inherited component templates — 2026-10-11

The existing ue_defaults syntax now distinguishes owned templates from inherited
component overrides. IR stores component_overrides by inherited stable ID and
merges effective defaults without changing parent declarations. Editor uses
InheritableComponentHandler/FComponentKey templates; parent SCS nodes are untouched.
Native values and authored editable scalars share existing preflight checks.
Changing component type/topology, inherited field metadata and migration of existing
composition/override declarations remain deferred.

Inherited-default override validation passed: foundation with zero failures, PIE with one successful test and zero warnings/errors, fresh-process Create/Reopen and packaged Development. Parent templates retain Bonus=42 and native visibility; the child gets Bonus=55 and bHiddenInGame=true while its owned ExtraBehavior retains Bonus=77. The PIE launcher now waits for the editor process itself instead of persistent helper descendants.

### Inherited events; component-class replacement audit — 2026-10-11

Committed the previously validated inheritance/lifecycle/component/default work as
5fa213d. Engine source audit found that SCS override templates must retain the exact
component class; native class overrides only apply to native default subobjects.
Authored SCS type replacement therefore needs a separate construction-backend proof
and is deferred, rather than forcing a record UE will invalidate.

Added additive parent/child managed event subscriptions for Actors and components.
Missing lifecycle hooks synthesize parent BeginPlay/EndPlay forwarding for event
parents too. Explicit overridden BeginPlay retains the Super policy. Derived owner
callbacks validate against the effective ActorComponent root. Same-name handler
replacement remains rejected; unrelated handler names coexist. Fixtures cover
self/owner sources, once-per-handler delivery and Destroy/EndPlay cleanup.

Inherited-event validation passed: foundation (zero failures), Create/Reopen, PIE (one success, zero warnings/errors) and packaged Development. Covered additive Actor self events, component owner events, one delivery per handler and cleanup. An imported-parent binder was initially resolved in a child without its own handlers, causing duplicate subscriptions; an explicit empty child binder now prevents this, and the no-BeginPlay/EndPlay parent regression passes.

### Class parameters in reflected calls (2026-10-12)
Added class marshalling kind, MetaClass in export IR and signature fingerprints, checked UEClassHandle arguments/results and null defaults. Return/out/inout, array elements and supported struct leaves share the general frame backend; class map/set support remains deferred. General class handles can represent abstract classes; construction still rejects them. Generator tests: 93 PASS. Function-library integration covers UClass/TSubclassOf, out, arrays, null/abstract values, bad MetaClass and GameplayStatics.GetActorOfClass; instance Actor.GetComponentsByClass added. Authored ue_function class signatures and packaged validation remain separate.

Class-marshalling validation completed: instance GetComponentsByClass, static GetActorOfClass, generator 93 tests, negative export preserving previous output, and editor foundation smoke all PASS.

### Script-only helper hierarchy — 2026-10-13

Committed inherited-event work as 270a70f. Added effective same-context helper
inheritance, child helper selection for self calls, explicit Super variants and
remapping of lowered ancestor calls through three levels. Helpers retain normal DAS
parameters/results and remain absent from reflected IR/Blueprint function graphs.
Original source calls are module-qualified so imported class methods infer without
an unbound receiver. Cross-context helper virtual dispatch remains a separate step:
an inherited reflected UFunction still runs its declaring script's helper table.

Construction audit separates a once-per-instance runtime initializer before
BeginPlay from repeatable editor construction. Neither hook is added in this slice;
parent ordering, body reload, instance edits and editor side-effect policy are the
next construction work. See DSLInheritance.md for the contract and limits.

Helper hierarchy validation passed: foundation with zero failures, three-level Create/Reopen, PIE and packaged Development. LocalProbe(2) returned 21 and Score became 12; Offset/Template remain absent from UFunctions and reflected IR. Temporary AST diagnostics were removed.

### Packaged classes/spawn/query/libraries (2026-10-13)
TestCookedCalls.ps1 full pipeline PASS on Win64 Development NullRHI: class-property cook dependency, native/authored Spawn, Actor/component queries, BeginPlay/Tick/EndPlay, stale handles, static engine/user libraries, WorldContext without editor metadata, UClass/TSubclassOf input/return/out/arrays and instance GetComponentsByClass. Packaged foundation smoke PASS failures=0. Shipping, graphics/physics collision outcomes and replication remain unverified.


### Class references in authored ue_function — 2026-10-15

Added UEClassHandle input/result and ref/out support for authored Actor and
ActorComponent callable methods. Native base constraints use arg_<name>_class and
return_class; omitted constraints default to UObject. Generated Blueprint signatures
use FClassProperty.MetaClass, including class casts for result/output pins. Internal
transport carries GC-visible UClass values and creates fresh receiving-context
capabilities, rather than sharing VM tokens. Input/result/output base checks reject
incompatible and obsolete classes; null is accepted and outputs stage atomically.
Saved-owner checks and reference-call contracts compare MetaClass as well as kind.

Validation passed: real SpawnPIE (one success, zero warnings/errors), native
ProcessEvent on Actors/components, saved Blueprint reopening in another process,
editor foundation (zero failures), and complete Win64 Development NullRHI packaged
calls plus packaged foundation (zero failures). Includes Blueprint subclasses, null,
ref/out, wrong input/result/output bases, and cross-context method calls. The export
commandlet uses a scoped editor script guard for deliberate Actor graph execution.
Class containers, class-valued delegates and authored-interface class signatures
remain separate adapters. See [DSLClassMethods.md](DSLClassMethods.md).


### UEClass type macro / inferred class constraints — 2026-10-15

Added $UEClass<NativeWrapper> and public ActorClass/ObjectClass aliases in
ue_object_types. A nominal struct view carries one checked UEClassHandle; native
base identity resides on ue_class_ref and supplies function/property constraints.
Callable ue_function inputs/results/ref/out no longer need string hints when using
these types. Raw UEClassHandle remains available and contradictory hints fail.
Added checked ue_class_cast/ue_class_handle conversion, typed class_loaded results,
Spawn/component argument lowering, typed properties and authored accessors. Aliases
must resolve in an imported module before class methods are parsed; no grammar
change or reader macro. Multiple aliases of a base reuse one nominal declaration.

Validation: source-negative checks, actual SpawnPIE (one success, zero warnings/errors),
editor foundation (zero failures), saved Blueprint reopen in a fresh process, complete
packaged Win64 Development NullRHI class/call pipeline and packaged foundation (zero
failures) all passed. Includes typed properties, ref/out, null, Blueprint subclasses,
cross-context calls and forged typed-class values. See DSLClassMethods.md.


### Once-per-instance runtime Initialize — 2026-10-15

Added Initialize():void to authored Actor/ActorComponent lifecycle IR and lowering.
Omitted hooks forward through parents; overrides use explicit Super.Initialize.
Repeated parent calls are no-ops and Super.Initialize outside Initialize is rejected.
Parent BeginPlay does not implicitly run an initializer deliberately skipped by a
child override. Delivery state uses weak UObject/source-path records outside VMs,
survives body reload/session recreation, transfers on object replacement and is
pruned on GC/world cleanup. Errors latch once, cancel owned managed registrations
and block subsequent gameplay callbacks; previous writes are not rolled back.

Ordering is before daScript BeginPlay, following engine component-before-Actor
lifecycle order; native/Blueprint ReceiveBeginPlay ordering is unchanged. Editor
construction/CDOs are excluded. Existing manual lifecycle calls without an
initializer preserve their old behavior.

Validation passed: final real SpawnPIE (one success, zero warnings/errors), editor
foundation zero failures, saved assets reopened in another process, and full packaged
Win64 Development NullRHI calls plus foundation zero failures. Tests cover three
Actor/two component levels, repeated/omitted/skipped Super, instance/CDO edits,
component availability, failure once, session recreation and body-only reload with
an increased revision and unchanged initialization count. Repeatable editor
ConstructionScript remains a separate next stage. See DSLInitialization.md.


### Repeatable Actor ConstructionScript — 2026-10-15

Added ConstructionScript():void lifecycle collection/IR, implicit authored-parent
forwarding and explicit Super.ConstructionScript. Native OnConstruction runs it
after Blueprint/SCS construction, excluding templates and Actors after BeginPlay.
Temporary script contexts preserve gameplay sessions and initializer delivery;
queued gameplay events are not drained while those sessions are isolated.
Existing component lookup works in the editor construction world. Managed callbacks,
event emission and dynamic Actor/component creation/destruction through bridge APIs
are blocked. Errors are retryable and writes are not rolled back. Cooked placed
Actors preserve saved construction results. ActorComponent construction callbacks
and native UObject constructors remain deferred. See DSLConstruction.md.


Construction validation passed: editor foundation zero failures; Create/Reopen with
editor reconstruction, instance edits, CDO exclusion, preservation of an existing
gameplay VM and recoverable forbidden-timer errors; real Gameplay.RuntimePIE with
one success and zero warnings/failures; full Win64 Development NullRHI packaged
gameplay plus foundation zero failures. Packaged probes cover saved map values and
runtime-spawn construction. Cross-context authored method entry during construction is now supported through
temporary VMs; see the 2026-10-16 construction-call follow-up below.


## 2026-10-16 — reflected authored calls during construction

ConstructionScript can call ue_function methods on authored Actors and components
using ordinary typed references. Nested dispatch uses the construction invocation's
temporary session set, preserves existing persistent VMs/subscriptions and does not
deliver Initialize or BeginPlay. Editor construction scopes enable Actor Blueprint
method graphs. Existing lifecycle, recursive construction, managed-callback,
event-emission and dynamic object creation/destruction restrictions remain active
inside nested methods; errors propagate and the next construction may retry.

Dedicated dsl_construction_calls / dsl_construction_call_worker fixtures and
TestConstructionCalls.ps1 cover Editor and Game worlds, saved Blueprint reopening,
Actor/component dispatch, temporary globals, preservation of a persistent component
VM and subscription, no initializer delivery, reconstruction, forbidden timer calls,
panic propagation and retry. See DSLConstructionCalls.md.


### ActorComponent ConstructionScript — 2026-10-16

Enabled lifecycle collection/IR for authored components, explicit Super and omitted
overrides. The authored Actor bridge configures all components after SCS/instance
data restoration and before its own hook. Standalone/new dynamic components use
first registration before their own Initialize/BeginPlay. Unregister/Register does
not repeat the hook; editor component property changes repeat it, avoiding a double
delivery when the owning Actor already reconstructed. PostLoad/PIE duplication keep
saved construction results. Native owner fallback cannot promise a complete
after-SCS owner pass; that ordering requires the authored Actor bridge.
Temporary contexts and construction side-effect policy are shared with Actors.
Errors are retryable and propagate from the component pass to the authored Actor.
See DSLComponentConstruction.md for the complete delivery contract and examples.


ActorComponent construction validation passed: editor foundation zero failures,
Create/Reopen with explicit/omitted Super, property-edit/reregistration delivery,
CDO exclusion, timer rejection/recovery and VM preservation; real RuntimePIE with
one success and zero warnings/failures; full Win64 Development NullRHI package plus
foundation zero failures. PIE required explicit duplication-state handling. Cooked
tests confirm that saved Score=8/value=21 is retained and newly registered dynamic
components run construction before Initialize/BeginPlay. See DSLComponentConstruction.md.


### Authored BlueprintPure functions — 2026-10-16

Enabled BlueprintPure instance methods for authored Actors and ActorComponents.
The DSL normalizes callable flags and validates an output contract; IR/schema,
Blueprint generation, precompile policy and runtime contract checks preserve
purity. Ordinary pure methods remain final; BlueprintEvent permits inherited
overrides and explicit Super while preserving purity. BlueprintPure=false retains
execution pins. Pure interfaces, static and const-method authoring remain deferred.
See DSLPureFunctions.md for syntax and semantics.

Validation passed: editor foundation zero failures, saved Create/Reopen with a
real pure data-node consumer, strict RuntimePIE one success and zero warnings or
failures, and Win64 Development packaged NullRHI gameplay plus foundation. Probes
exercise inherited pure overrides/Super, saved flags, impure opt-out and component
void/out results. The consumer fixture removes an automatic unused return node so
Blueprint compilation remains warning-free.


### Pure interface contracts — 2026-10-16

Added BlueprintPure to annotated authored interface stubs and allowed matching
pure native/authored UInterface implementations. Callable normalization and output
validation match authored class methods. Interface signature entry flags persist
through compilation; implementing methods remain overridable. A purity mismatch
rejects before Blueprint mutation. Structural interface changes retain controlled
refusal; const/static methods are separate work. See DSLPureFunctions.md.

Validation: editor foundation zero failures; authored interface Create/Reopen,
independent interface recompilation, negative implementation mismatch, Blueprint
pure data-node execution through the interface UFunction, generated DAS wrappers,
and the full Win64 Development NullRHI packaged interface probe all pass.


### Static class/component defaults extension — 2026-10-16

Added self.Property assignments in ue_defaults, lowering into the existing owned
property default contract. Block values override inline initializers. Expanded
math constructors to nonidentity Transform; native/authored component templates
accept math and supported reflected struct values, class references and assets.
Declaration-only ue_class_default/ue_asset_default and ue_default_text markers
are validated before Blueprint mutation. Reflected CDO/template hard references
provide saved/cooked dependencies; non-null authored object identity remains deferred.
Existing Vector/Rotator component wire spelling is preserved. Repeated/unknown
assignments, executable expressions and invalid references are rejected. Defaults
migration is implemented below; inherited property override declarations are implemented below; computed scalar defaults are implemented below.
See DSLExtendedDefaults.md and TestExtendedDefaults.ps1.

Static defaults validation passed: Create/Reopen, JSON roundtrip and six negative
preflight cases, real Defaults.RuntimePIE (one success, zero warnings/failures),
editor/packaged foundation (zero failures), existing native-component automation
(one success, zero warnings/failures), and final Win64 Development NullRHI packaged
runtime without engine errors/ensures. Unreal-specific Transform pin formatting,
exact Euler/quaternion default matching and native template edit notifications are
covered. See DSLExtendedDefaults.md for the full contract and test commands.

### Safe defaults reimport with manual overrides — 2026-10-16

Implemented Automatic / PreserveOverrides reimport for default-only declarations.
Three-way source/CDO comparison preserves manual values; archetype comparison
propagates new defaults through ordinary Blueprint children, existing inheritable
component templates and editor instances. Struct members merge independently;
quaternions remain atomic. Static asset/class defaults share the same policy.
CDO variable defaults survive later Blueprint compilation, and cooked source
fingerprints preserve runtime owner validation with manual CDO edits.

No Blueprint compilation, actor reconstruction or VM detachment occurs during
the merge. Staging, bounded dependency discovery and snapshot rollback guard
property/metadata updates. Affected PIE/game worlds, active VM calls/events,
uninitialized component maps and unsupported dependency scopes are rejected.
Packages are marked dirty for user review/save. See DSLDefaultsMigration.md and
TestDefaultsMerge.ps1 for semantics, limitations and regression commands.

Safe defaults validation passed: Create/Reopen/Recompile/Unopened; automatic and explicit console commands; two rollback stages; Editor/packaged foundation with zero failures; final NullRHI packaged runtime; legacy PreserveInstances asset and PIE regressions (one PIE success, zero warnings/failures).


### Native base inheritance syntax — 2026-10-16

ue_dsl exports Actor and ActorComponent classes marked with ue_native_base native
identity metadata. ue_class accepts these as DAS parents and emits the existing
native_parent IR without an authored parent_id or extra asset. Authored parent
resolution/Super remain separate; helper ancestor traversal stops at host markers.
Type resolution recognizes marker identity alongside existing native reference
aliases. Parent annotations remain compatible; mixed declarations and unsupported,
stateful or unmarked bases reject. Principal Actor/component, gameplay, inherited
lifecycle/initialization and construction examples now use class inheritance.
See DSLNativeBases.md.

Validation: foundation zero failures, Create/Reopen, strict RuntimePIE one success
and zero warnings/failures, Win64 Development packaged NullRHI gameplay and
packaged foundation all pass with the updated examples.


### Defaults on inherited authored properties — 2026-10-16

Added `self.Property` defaults on authored ancestors without redeclaration. Optional
property_overrides records retain ancestor stable ID/name and a typed literal;
effective resolution preserves the original property's type, flags and owner.
Initial import changes the child's CDO only. Runtime validation selects the nearest
authored default owner, preserving base entry calls and cooked manual overrides.

Safe reimport supports changing, adding and removing inherited defaults. Absent
overrides use the actual parent CDO baseline; explicit child values block parent
propagation even when equal to the previous parent default. Manual values and
member-wise struct edits survive; no Blueprint reinstance occurs. Parent import
uses Automatic policy to retain manual parent defaults. Layout/access checks run
before native writes. Record comparisons are order-independent; low-level backend
authoring refuses models needing DSL provenance. See DSLInheritedPropertyDefaults.md
and TestInheritedDefaults.ps1. Computed scalar defaults are implemented below;
arbitrary native-parent field projection remains separate work.

Inherited-property defaults validation passed: actor/component Create/Reopen/Recompile, manual parent/child/instance values, three-level overrides, add/remove, rollback, unchanged reimport and unapplied-parent refusal; Win64 Development NullRHI packaged probe; editor/packaged foundation zero failures. Existing safe-default regression also passed; its startup Zen assert was bypassed for the remaining phase using filesystem DDC without modifying the engine/service.


### Const authored methods — 2026-10-16

Native DAS def const now emits optional const IR/schema state and FUNC_Const for
Actor/ActorComponent methods and annotated interface stubs. Purity remains
independent; BlueprintPure=false preserves exec pins. Overrides and interfaces
must preserve const; source/runtime signature checks and precompile metadata
preserve it through reload/recompile/save/cook. Const transitions are structural.
An original-source visitor rejects ordinary self writes/container mutations and
known non-const self/Super calls before property lowering, including helper bodies.
Thin const frame adapters share the existing dispatcher without duplicate VM or
marshalling implementations. Const is not a side-effect sandbox. Static authored
functions remain separate work. See DSLConstMethods.md.

Validation: foundation zero failures, negative const bodies/overrides and IR
roundtrip, saved Create/Reopen const flags and Super/helper execution, authored
interface Create/Reopen/packaged dispatch plus mismatch refusal, strict RuntimePIE
one success and zero warnings/failures, and full Win64 Development NullRHI packaged
gameplay/foundation all pass. Const graph compilation requires const native adapter
UFunctions; ordinary non-const dispatch functions cannot be called on readonly self.


### Computed class defaults — 2026-10-16

Added real daScript scalar calculations and pure helper calls in ue_class field
initializers and ue_defaults. Defaults assignments see source fields and earlier
assignments, including authored ancestor values. Source snapshots are isolated
per class so sibling overrides cannot affect each other. Explicit defaults take
precedence over inline initializers. Actor/ActorComponent and component-template
defaults share the existing typed lowering; math coordinates and asset/class/text
marker arguments accept computed scalars/strings.

A collection compilation retains helper bodies without RunFolding execution,
audits their call graph, and evaluates them in an isolated Context with global
init/shutdown disabled. A second normal compilation substitutes canonical
literals. Temporary AST clones have an explicit GC root across imported modules
and compiler passes. Global state, side effects, World/UObject calls, unsafe code,
loops, recursion, indirect calls and non-scalar helpers reject; graph/depth/memory
limits bound evaluation. Errors occur before Blueprint mutation.

IR/saved metadata remain resolved data. The existing transactional merge handles
changes to computed results while preserving manual values. Cook stores normal
CDO/template values and reflected asset/class dependencies. Pure calculations may
repeat during source compilation for runtime contract validation; objects inherit
baked defaults rather than executing a defaults callback on Spawn.

See DSLComputedDefaults.md, dsl_computed_defaults.das and TestComputedDefaults.ps1.
Returning arbitrary structs/objects from helpers, reading native-parent fields,
instance-method calls in defaults and unrestricted CDO constructors remain future
work. Instance side effects belong in Initialize/ConstructionScript.


Validation completed: Editor Create/Reopen/Recompile; computed int/double/bool/
string/math/asset values; unchanged reimport and helper-result migration with
manual CDO/instance overrides; inherited properties, isolated siblings and
ActorComponent; source cycles, type mismatch, global state, side effects, loops,
recursion, World calls and division-by-zero refusal. Failed reimport preserves
the working class and metadata. Declaration-only interface collection skips
computed component defaults. First-launch global initializers do not execute
inside the defaults evaluator.

Existing inherited-default Create/Reopen/Recompile and safe-merge
Create/Reopen/Recompile/Unopened regressions passed. Editor and packaged foundation
report zero failures. Final Win64 Development NullRHI cook/package and runtime
probe passed: saved CDO/template values, strong asset dependency, child/sibling
isolation, ActorComponent, runtime source contracts and fresh Spawn. Shipping and
rendered gameplay were not exercised. Logs: computed-defaults-final-package-suite.log,
computed-defaults-packaged.log, computed-defaults-foundation-editor.log and
smoke-packaged.log under logs/.


### Static library first slice

Added `ue_library` + native `def static`: real Blueprint Function Library assets,
worldless callable/pure scalar functions, out/ref, direct DAS static calls,
metadata/comment tooltips and saved source contracts. See [DSLStaticLibraries.md](DSLStaticLibraries.md).
WorldContext, aggregate/object signatures and structural library migrations remain
follow-up work.


### Native host and built-in component defaults — 2026-10-16

ue_defaults now accepts editable Engine.Actor/ActorComponent properties and
fixed native default-subobject properties without declaring Blueprint variables.
Optional native_defaults IR groups are separate from authored properties/SCS
nodes. Lookup excludes bridge bookkeeping and requires a native reflected owner,
editable template property and stable native component class/name/outer identity.
The shared strict editable-value parser now resides in Runtime and is reused by
both default-import paths.

Pure calculations can read native scalar source defaults, including authored
ancestor source overrides and earlier assignments. Native-only calculations are
omitted from the collection model until evaluated. The second compiler pass now
rebuilds its module group and restores declaration caches so imported parents
cannot retain placeholder models from the collection pass.

Initial import changes only the new authored CDO/native templates. Reimport uses
the existing transactional three-way merge, preserving manual values and struct
members, explicit child defaults, class identity and runtime sessions. Removing a
source key falls back to the actual immediate parent CDO. Cooked native source
fingerprints are scoped by authored class, target and property; runtime validation
allows manual CDO overrides but refuses changed/removed source contracts until
reimport. Low-level authoring refuses models needing DSL native provenance.

Actor/ActorComponent Create/Reopen/Recompile passed, including computed native
ancestor reads, rollback at both stages, native subobject struct members, key
removal/re-addition and pre-asset rejection of unknown/private/wrong-type/duplicate
targets. Existing computed defaults and Editor foundation (zero failures) passed.
Existing inherited-default and safe-merge regressions passed as well.

See DSLNativeDefaults.md and TestNativeDefaults.ps1. Gameplay-method native field
projection, Character/Pawn host adapters, component replacement, native enums/
containers, deep destinations and mixing native subobject defaults with SCS
composition remain separate work.

Verified: Create/Reopen + reload/refusal; strict RuntimePIE; Editor and packaged foundation failures=0; Win64 Development cook/package/gameplay with a separate Blueprint library caller. Shipping/rendered gameplay not tested.


Final validation: TestNativeDefaults.ps1 -SkipBuild -Package passed all
Create/Reopen/Recompile phases and the Win64 Development NullRHI runtime probe.
Cooked actor/component CDO values, manual instance and struct-member overrides,
authored descendants, source contracts and fresh Spawn all passed. No engine
errors/ensures were reported by the final packaged probe. Editor and packaged
foundation each report zero failures. Existing computed defaults, inherited
defaults and safe-merge Create/Reopen/Recompile/Unopened regressions also passed.
Case aliases are rejected before asset creation. Shipping and rendered gameplay
were not exercised.

Logs: logs/native-defaults-final-suite.log, native-defaults-packaged.log,
native-defaults-foundation-editor.log and smoke-packaged.log. Packaging now uses
AutomationTool's -WaitForUATMutex so concurrent UAT runs serialize instead of
failing or stopping another run.

## Native self fields in gameplay methods

Implemented checked native field reads/writes for Actor and ActorComponent,
including script-only helpers, inherited implementations and lifecycle lowering.
Scalar/name and strong object/class ABI values reuse the existing runtime checks.
No authored IR fields or Blueprint variables are generated. Readonly, hidden,
accessor-backed and unsupported types are rejected; native field shadowing is
rejected. See DSLNativeSelfFields.md for syntax, scope and test coverage.
Next: typed native reference projection, native enum/container/struct fields,
and additional host adapters (Pawn/Character).

Validation (2026-10-16): Editor build and TestNativeDefaults.ps1 -Package passed
Create/Reopen/Recompile and the Win64 Development NullRHI game probe. Nine invalid
native-method variants, the positive control and the const-write check passed.
Editor and packaged foundation each report DAS_FOUNDATION_RESULT=PASS failures=0.
Logs: logs/native-self-verified-suite.log,
logs/native-self-foundation-editor-final.log, logs/native-defaults-packaged.log,
and logs/smoke-packaged.log. Shipping and rendered gameplay were not exercised.

### Static libraries: WorldContext and complex values

Added named input-object WorldContext with Blueprint automatic pin defaulting,
explicit direct DAS calls and exception-safe nested world scopes. No implicit
self or instance lifecycle is granted. Game-world validation rejects null,
destroyed/template/non-game contexts; context-bound VM ownership follows the
supplied object and its world. Existing native-library automatic WorldContext,
actor/component operations and queries resolve the selected context.

Library signatures now use existing typed object/interface/class, authored
enum/struct, math and supported array/map/set snapshot ABIs, including complex
out/ref. Delegate values use the shared ABI; delegate out/ref remains deferred.
Registry and value assets are retained on the native library CDO for cook.
Structural library migration and parameter defaults remain follow-up work.
See DSLStaticLibraries.md and dsl_library_values.das.

## Typed native self references

Native object/class fields resolve exact, visible canonical wrapper declarations.
Reads and assignment lower through their checked typed converters. Missing and
ambiguous wrappers fail explicitly; no fake fields or new authored property IR.
Actor.Owner is a readonly GetOwner facade. RootComponent projects to
SceneComponentRef, and typed references can feed existing component APIs directly.
The native enum/container/struct field and Pawn/Character host work remains next.
Validation is tracked in DSLNativeSelfFields.md.

Validated final extension: Create/Reopen/recompile; cross-world and recovered DAS calls; nested Actor ConstructionScript; strict RuntimePIE with automatic WorldContext; Editor and packaged foundation failures=0; Win64 Development cook/package/gameplay with authored enum/struct and map/set snapshots. Logs: library-values-*-final.log, cooked-gameplay-runtime.log, smoke-packaged.log. Shipping/rendered gameplay not tested.

Typed-reference validation (2026-10-16): Editor Create/Reopen/Recompile and
Win64 Development NullRHI packaged probe passed. Covered typed SceneComponent
returns and direct scene API calls, inherited methods, null/live Actor owners,
Pawn assignment/return/null/recovery, wrong nominal types, readonly Owner,
missing/ambiguous wrappers and forged wrapper rejection before UE storage changed.
Editor and packaged foundation each report PASS failures=0.
Logs: logs/typed-native-refs-editor-2.log, typed-native-refs-foundation.log,
typed-native-refs-package.log, typed-native-refs-packaged.log and smoke-packaged.log.
Native UClass wrapper projection shares the implementation but the current
Actor/ActorComponent host surface has no native UClass property fixture; it has
not been separately exercised. Shipping and rendered gameplay remain untested.
## Native self primitive arrays

Native primitive arrays use the existing container snapshot/carrier pipeline.
Actor.Tags and ActorComponent.ComponentTags project to array<UENameValue>, with
indexed read/write, whole-array clone assignment, push/push_clone, erase, pop and
clear. Source templates and declaration-only passes generate the necessary helpers
without adding synthetic fields or authored property IR. Tests cover snapshots,
inherited methods, component fields, bounds failure/recovery and RHS side effects.
Next: native enum/struct fields and additional container element kinds/maps/sets.
See DSLNativeSelfFields.md for scope and validation.

Native-array validation (2026-10-16): Editor build, Create/Reopen/Recompile and
Win64 Development NullRHI packaged probe passed. Verified live Actor.Tags and
ActorComponent.ComponentTags mutation, inherited methods, whole-array assignment,
snapshot return/isolation, indexed reads/writes, clear/push/erase/pop, preservation
of RHS side effects, bounds failure without partial commit and recovery. Wrong
array element/assignment types and const writes were rejected. Declaration-only
compilation with the native array methods also passed. Editor and packaged
foundation each report DAS_FOUNDATION_RESULT=PASS failures=0.
Logs: logs/native-arrays-editor-verified.log, native-arrays-foundation-editor.log,
native-arrays-package.log, native-arrays-packaged.log and smoke-packaged.log.
Only the FName native arrays have live host fixtures; other primitive arrays share
the carrier implementation. Shipping and rendered gameplay were not exercised.

### Static library parameter defaults

Added ordinary trailing int/float/double/bool/string literal defaults, including
negative numbers, exact double values, Unicode and empty strings. Authoring IR
retains optional defaults separately from the full native call ABI. Source DAS
omitted arguments and new Blueprint call pins use the same declarations.
Default changes are metadata reimports: explicit caller pin values survive;
removing defaults clears stale metadata. Out/ref, WorldContext, computed and
complex defaults are rejected. Instance-method defaults are implemented below;
migration of unloaded dependent caller assets remains follow-up work. See DSLStaticLibraries.md.

Parameter defaults validated: Create/Reopen/recompile, DAS omission/partial/explicit calls, scalar pin metadata, six rejection cases, default-only reimport preserving explicit pins, changed/removed defaults, strict RuntimePIE, Editor/packaged foundation failures=0, Win64 Development cook/package/gameplay. Logs: library-defaults-*.log. Shipping/rendered gameplay not tested.


### Native enum and struct values through self

Implemented canonical ue_native_enum declarations for loaded native UEnums, checked
nominal packing, native self field inference and typed ue_function parameters/results.
Native FStructProperty projection uses existing validated snapshot/math ABI. Added
readonly RootComponent relative math reads and explicit whole-value ActorTransform
world-transform facade. Preserve native visibility/readonly, const and lifetime/world
checks without additional authored fields. Native enum nesting in authored structs,
all-engine wrapper generation and native enum/struct containers remain separate work.
See DSLNativeSelfFields.md and ue_native_engine_values.das.


### Ordinary method parameter defaults

Implemented trailing scalar literal defaults for Actor and ActorComponent
ue_function declarations using the same collector helper and IR validation as
static libraries. Defaults work for self calls, typed authored references from
free functions, Blueprint calls, inherited BlueprintEvents and Super calls.
Typed dispatch wrappers reconstruct source-level defaults before entering the
complete native parameter ABI. Existing legacy scalar ABI/pin names are retained.

Defaults belong to the call-site declaration: a base view/call uses base defaults
while dispatch still invokes the derived override; a derived view uses derived
defaults. Super uses parent defaults. Overrides retain signature validation.

Blueprint entry pins and generated metadata carry the defaults. Default-only
reimport changes new pin values, clears removed defaults and preserves explicit
values on an existing caller. No automatic migration of unloaded callers.
Supported: int/float/double/bool/string literals, negatives, Unicode and empty
strings. Ref/out, computed and complex defaults remain unsupported.

Validated Editor build, Create/Reopen/recompile, Actor and ActorComponent self/
reference/Blueprint calls, virtual/Super dispatch, five invalid declarations,
changed/removed default metadata and preserved explicit caller arguments.
Foundation smoke: PASS failures=0. Logs: instance-defaults-editor.log,
static-library-Create.log, static-library-Reopen.log,
instance-defaults-foundation.log. See DSLParameterDefaults.md.


Native enum/struct validation (2026-10-16): Editor build, Create/Reopen/Recompile,
Editor foundation and Win64 Development NullRHI package passed. Native UEnum
parameters/results and all five values, inherited methods, forged enum rejection
without partial write and recovery, missing/ambiguous/wrong nominal declarations,
declaration member mismatch, readonly/const/type rejection, nested math leaf reads,
exact nonzero rotation getters and whole-value world-transform writes were verified.
Packaged native probe reports DAS_NATIVE_DEFAULTS_PACKAGED=PASS; Editor and packaged
foundation report DAS_FOUNDATION_RESULT=PASS failures=0. Logs:
logs/native-enum-struct-editor-verified.log, native-enum-struct-foundation.log,
native-enum-struct-package.log, native-enum-struct-packaged.log and smoke-packaged.log.
Probe map: /Game/DasNativeDefaults/281e6c57275c4c8ea8e3b1609a0d38e8/Map.
Shipping and rendered gameplay were not exercised.

Validation: strict DasForUE.Gameplay.RuntimePIE regression PASS (1 success,
0 failures, 0 warning tests; instance-defaults-pie.log and
instance-defaults-pie-report/index.json). The original stage covered saved/reopened commandlet consumers. The game-world
and packaged checks below extend that coverage.


### Native enum and struct arrays through self

Added native array descriptor/type inference, exact enum-class/TEnumAsByte carriers,
safe native struct carriers, math element arrays and generic UEStructValue snapshots.
Reuse checked container reads/mutations and preserve readonly/const/snapshot isolation.
Wider enum storage is rejected explicitly. Live regression fixtures use scoped native
C++ test adapter properties; fixed Engine hosts have no eligible fields of these kinds.
See DSLNativeSelfFields.md and dsl_native_array_self.das. Object arrays, native maps/sets
and arbitrary native C++ parent adapters are separate work.


### Instance-default game-world and cook integration — 2026-10-17

Closed the missing direct PIE/packaged assertions for ordinary method defaults.
The shared Runtime probe now runs on live Actor, dynamically registered
ActorComponent and inherited Actor instances after BeginPlay. The same editor
fixture helper creates source classes and Blueprint consumers for both paths;
reflected carrier references retain their cooked class/parent dependencies.

Verified self/reference calls, omitted/partial/explicit arguments, scalar literal
precision and strings, Blueprint disconnected-pin values, virtual/base-view
defaults, Super, component registration/BeginPlay and context cleanup.
Editor build, strict RuntimePIE, Win64 Development cook/package/gameplay and
packaged foundation (failures=0) passed with NullRHI. Logs:
instance-defaults-game-pie.log, instance-defaults-game-pie-report.json,
instance-defaults-game-cooked.log, cooked-gameplay-runtime.log,
smoke-packaged.log. Shipping/rendered gameplay remain untested. See
DSLParameterDefaults.md; broader migration and complex defaults remain deferred.


Native value-array validation (2026-10-17 Europe/Budapest): Editor build and
Create/Reopen/Recompile passed. Verified real FEnumProperty and FByteProperty
UPROPERTY array storage; FVector, FRotator, FTransform and generic FIntPoint arrays;
indexed and vector/rotator leaf writes, clone assignment, clear/push/pop/erase,
detached snapshots, invalid enum and bounds recovery, wrong generic struct identity
and whole-array failure after a valid element without partial commit. Wrong nominal
enum/math types and readonly mutation are rejected at compilation. No authored
Blueprint variables were generated for the fixture. Win64 Development NullRHI
cook/package and packaged native probe passed with exit 0; Editor and packaged
foundation each report DAS_FOUNDATION_RESULT=PASS failures=0. Logs:
logs/native-value-arrays-editor-verified.log, native-value-arrays-foundation.log,
native-value-arrays-package.log, native-value-arrays-packaged.log and
native-value-arrays-foundation-packaged.log. Probe map:
/Game/DasNativeDefaults/19ec0214580b4e8a9c45e72bf24aaf87/Map.
Vector2D shares the math array path but has no additional live fixture here.
Shipping and rendered gameplay were not exercised.


### Native maps, sets and object arrays through self

Implemented strong native object arrays, int/string-keyed native maps with scalar,
name, native enum, math/safe struct and object-reference values, and int/string
native sets. Reuse canonical wrappers, checked snapshot mutation, readonly/const
policies and exact enum storage carriers. Other key/set element types, weak/soft
references, class containers and nested containers remain separate work. See
DSLNativeSelfFields.md and dsl_native_array_self.das.


Native map/set/object-array validation (2026-10-17 Europe/Budapest): Editor build,
Create/Reopen/Recompile and Win64 Development NullRHI cook/package/runtime passed.
Live C++ fixtures verify Actor/Pawn TObjectPtr arrays with nulls, detached clones,
whole assignment and forged wrong-class rejection; int/string sets with duplicate
insertion, erase, membership and clone assignment; int/int, string/double and int/name
maps; string/vector, int/IntPoint, int/native-enum, string/byte-enum and int/Pawn maps.
Verified indexed/compound writes, map insert/erase, clear, clone assignment,
wrong enum/object/struct rejection with recovery and no partial native snapshot
commit. Wrong key/value/wrapper/element types and readonly mutation were rejected
during compilation. Actual native fields were compared against expected values.
Other supported primitive map value combinations and interface references reuse
existing carriers; they do not have additional native live fixtures in this stage.
Editor and packaged foundation report DAS_FOUNDATION_RESULT=PASS failures=0;
packaged native probe reports DAS_NATIVE_DEFAULTS_PACKAGED=PASS and exit 0.
Logs: logs/native-containers-editor-verified.log, native-containers-foundation.log,
native-containers-package.log, native-containers-packaged.log and
native-containers-foundation-packaged.log. Map:
/Game/DasNativeDefaults/877cccf7e8a74a9b873d717e293608cb/Map.
Shipping and rendered gameplay were not exercised.


### Pawn and Character native DSL roots — 2026-10-17

Implemented `class Player : Pawn/Character` and equivalent explicit `ue_class`
native parents. Blueprints use real APawn/ACharacter-derived native adapters.
Actor/Pawn/Character share lifecycle/frame dispatch; trusted common-host access
handles authored identity, default contracts, parent dispatch, error reporting
and lifecycle preservation. New hosts use the frame ABI, including out/ref.

Generalized native-self/default CDO selection, inherited lifecycle/Super,
authored references and Blueprint validation. Character retains capsule/mesh/
movement and rejects capsule-root replacement. Short parent names normalize
before computed-default evaluation. Native Pawn/Character tick remains intact
when there are no DAS lifecycle callbacks. Unsupported/mixed parents are rejected;
broader structural/data migration remains deferred.

Examples: dsl_pawn_player.das, dsl_character_player.das, their children/reference
callers and dsl_pawn_explicit_parent.das. Shared runtime coverage validates them
in strict RuntimePIE and Win64 Development NullRHI packaged gameplay, with
packaged foundation failures=0. See DSLPawnCharacter.md and pawn-character-*.log.
Possession/input/movement gameplay, arbitrary native roots/virtual overrides,
networking, Shipping and rendered gameplay are not part of this step.


### FName keys in native containers through self

Added native TMap<FName, supported value> and TSet<FName> projection using UENameValue
keys. Reuse canonical per-context FName tokens and daScript's uint64 handle-key
representation, preserving Unreal name equality and suffix semantics. Added exact
FName snapshot carriers and checked native key accessors. Authored property/method
signatures using name-key containers, enum/struct/object keys and weak/soft references
remain separate work. See DSLNativeSelfFields.md.


### Playable Character integration — 2026-10-17

Added explicit checked Pawn possession and Character move/look/jump APIs, native
typed component views, a first-person DSL Character with a composed camera,
readiness-based input setup/release, saved WASD/mouse/Space actions/context and a
generated PlayableMap. Shared PIE/packaged probe exercises real EnhancedPlayerInput
including keyboard mapping, native movement/jump/camera and owned-input teardown.
The component-field collector now accepts Pawn/Character actor roots. See
PlayableCharacter.md.

Validated 2026-10-17: strict RuntimePIE (1 success, 0 failed, 0 warnings), Win64
Development build/cook/stage/Pak/archive, packaged foundation failures=0, packaged
gameplay and a separate run of the saved PlayableMap. Input injection evaluates
real Enhanced Input bindings; a synthetic W key also verifies the Mapping Context.
Coverage includes authority possession, move/look/jump, native camera, free-function
movement accessors, rejected negative speed, unpossess/repossess and EndPlay cleanup.
The saved map verifies auto-possession, readiness and movement on its cooked floor.
Logs: playable-character-pie.log, playable-character-pie-report.json,
playable-character-cooked.log, cooked-gameplay-runtime.log, playable-map-runtime.log.
Rendered output, physical devices, multiplayer and Shipping were not tested.


FName container validation (2026-10-17 Europe/Budapest): Editor build,
Create/Reopen/Recompile and Win64 Development NullRHI package/runtime passed.
Live native fixtures confirm CPP-origin name keys, case-insensitive aliases,
Unit/Unit_0/Unit_1/Unit_01 distinctions, None/empty alias, Unicode, set duplicate
handling, key membership, erase, compound writes, local snapshot isolation and
whole-container clone assignment. FName-key map values tested: int, FName, FVector,
FIntPoint, enum class, TEnumAsByte and Pawn references including null. Wrong enum,
struct and object values are rejected without partial native commit; wrong key
types and readonly mutations fail compilation. Other scalar map values reuse the
same scalar accessor templates and do not have additional native fixtures here.
Corrected new name/enum getter registration to the existing uint64 return ABI;
managed object/struct getters retain their copy/move return ABI. No SDK changes.
Editor and packaged foundation report DAS_FOUNDATION_RESULT=PASS failures=0;
native packaged probe reports DAS_NATIVE_DEFAULTS_PACKAGED=PASS and exit 0.
Logs: logs/native-name-containers-editor-final.log,
native-name-containers-foundation.log, native-name-containers-package.log,
native-name-containers-packaged.log, native-name-containers-foundation-packaged.log.
Probe map: /Game/DasNativeDefaults/edfcc276c80349b6a66c559081148910/Map.
Shipping and rendered gameplay were not exercised.


### 2026-10-17 — authored FName maps and sets

Completed FName-key TMap and TSet<FName> in direct ue_class properties and
ue_function inputs/results/out/inout. Maps retain both key and value source
descriptors when reconstructing typed accessors/calls; this fixes math-valued
map signatures. Native pins/properties, name equality, copies, empty containers,
readonly/type errors and body-only reimport are covered by method-value fixtures.
Editor Create/Reopen, natural PIE (1 pass, no warnings) and packaged Win64
Development passed. See [DSLNameContainers.md](DSLNameContainers.md).
Nonempty defaults, general container declaration migration and richer Set element
families remain separate work.


### 2026-10-17 — TSet of native and authored enum

Completed table<Enum> in native self fields, direct ue_class properties and
ue_function input/result/out/inout. Native enum-class and TEnumAsByte fields retain
their actual reflected storage; authored Kismet declarations select the canonical
form from native enum CppForm. Transient set carriers resolve enum identity after
loading and share enum member fingerprint checks with enum-array carriers.
Generated pack/unpack helpers and runtime adapters validate type, member values,
readonly constraints and snapshot limits before publication. Native Create/Reopen/
Recompile, authored Create/Reopen, PIE (1 pass, no warnings), game build/cook/stage
and both packaged probes passed. See [DSLEnumSets.md](DSLEnumSets.md).
Remaining work includes object/struct Set elements, richer map keys, nonempty
defaults and container declaration migration.


### Native gameplay field/method generation — 2026-10-17

Network work remains deferred by user decision. Added reflection-driven generation
for Character, CharacterMovementComponent, PlayerController and CameraComponent,
including supported inherited members: 216 fields and 406 methods in the first
manifest. Unsupported API is listed with reasons. The generated registry preserves
editor metadata policy in cooked builds; property/method operators use the shared
compiler emitters, checked weak refs, frame ABI, shape checks and ProcessEvent.
Canonical imported reference types are reused; dependency views are generated.
Playable Character input uses inherited native self calls. See
NativeGameplayGeneration.md.

Validated 2026-10-17: GenerateNativeGameplay.ps1 -Check, six Python generator
tests, strict RuntimePIE (1 success, 0 failed, 0 warnings), Win64 Development
build/cook/stage/Pak/archive, packaged foundation failures=0, packaged gameplay
and separate saved-map auto-possession/movement/jump/camera checks. Six invalid
source fixtures and runtime wrong-receiver checks passed. Math fields are whole
snapshots; nested writes reject explicitly. Fixed raw annotation-pointer usage
and an early dependency declaration in accumulated object-key helpers so the
combined source tree builds. No claim of full object-key feature validation here.
Logs: native-gameplay-check.log, native-gameplay-pie.log,
native-gameplay-pie-report.json, native-gameplay-cooked.log,
cooked-gameplay-runtime.log and playable-map-runtime.log. NullRHI/synthetic input
only; physical devices, rendered output, Shipping and network remain untested.


### 2026-10-17 — UObject TSet and UObject-key TMap

Completed native self fields, direct ue_class properties and ue_function input/
result/out/inout for UObject sets and object-key maps. Source uses nominal
by-value $UEObjectKey<Reference> keys (ActorKey/ObjectKey aliases), ue_key,
ue_key_ref and ue_key_valid because ordinary reference-wrapper structs cannot
serve as daScript table keys. Authored key constraints retain their stable class
ID; native storage remains real class-constrained FObjectProperty containers.
Object identity is interned by FObjectKey independently of handle ExpectedClass.
Native object hashing, nulls, snapshot ownership/GC, destroyed/forged keys,
readonly/type checks, complete validation before publication and recovery through
empty assignment are covered. General call marshalling now accepts object keys
and set elements while excluding interface/weak/soft/class keys.
Native Create/Reopen/Recompile, fresh authored Create/Reopen, PIE (1 pass, no
warnings), cook/stage and both packaged Win64 Development probes passed.
See [DSLObjectHashContainers.md](DSLObjectHashContainers.md).
Remaining container work: struct hash elements/keys, enum keys in authored maps,
nonempty/computed defaults and declaration migration of container-bearing classes.


### 2026-10-17 — Generated native enums and object-reference upcasts

NativeGameplay reflection manifest schema 2 records nominal native enum layouts;
source generation reuses imported enum views or emits checked ue_native_enum
declarations. The selected four receiver classes now expose 232 fields and 442
methods, using 21 enums. Property, input and return adapters share the existing
typed enum carrier; int/different-enum source arguments are rejected. Existing
0..254 unique-value ABI remains; raw byte, signed/wide/aliased enum layouts and
enum default expressions remain explicit limitations.

FUEPropertyPass adds contextual checked reference upcasts using loaded UClass
ancestry for concrete function/method value arguments, local initialization,
copy/move assignment and function return. Authored source views retain stable-ID
validation before conversion to native ancestors. Native packing no longer
requires an optional source `_raw` helper. Exact overloads win before any
upcast; a unique shortest-distance parent overload is used otherwise, and ties
remain errors. daScript's implicit method receiver slot is handled explicitly.
Downcasts/unrelated/interface conversions, mutable out/ref covariance and
container covariance are not introduced.

Validation: 11 generator tests, reproducible GenerateNativeGameplay -Check,
strict RuntimePIE (1 success, 0 warnings/failures/notRun), Win64 Development
build/cook/stage, packaged foundation (0 failures), packaged gameplay and
separate playable-map launch pass. The shared runtime probe covers native enum
fields/input/results, native Possess(CharacterRef), free function/assignment/
return upcasts, null, single evaluation and overload precedence. Thirteen bad
source fixtures cover old native policy plus enum typing, invalid upcasts,
mutable ref invariance and ambiguity. Logs: native-enum-upcast-check.log,
native-enum-upcast-pie.log, native-enum-upcast-pie-report.json,
native-enum-upcast-cooked.log, cooked-gameplay-runtime.log,
smoke-packaged.log and playable-map-runtime.log.

See NativeGameplayGeneration.md. Rendering/physical devices/Shipping/network
are not validated. Network work remains deferred by user; native text, custom
structs and container generation remain separate follow-ups.
