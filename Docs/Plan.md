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
