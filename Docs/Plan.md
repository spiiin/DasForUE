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
