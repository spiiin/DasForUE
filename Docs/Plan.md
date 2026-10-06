# План DasForUE2 после аудита NimForUE

Дата: 2026-10-05. Проверен исходный код NimForUE devel, commit
`58140f4842a91a0187c17ce950e07181ba455862`. Это аудит исходников и документации,
не подтверждение работоспособности NimForUE на UE 5.8.3. В этой ревизии заявлена
поддержка UE 5.7. Старый DasForUE не используется как основа.

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
