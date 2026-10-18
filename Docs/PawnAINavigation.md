# Пакет Pawn / Character / AI / Navigation

Профиль: Bindings/Packages/PawnCharacterAINavigation.profile.json.
Модуль: ue_api_pawn_ai. UE modules: Engine, AIModule, NavigationSystem.
Выбраны AIBlueprintHelperLibrary и 12 receiver-классов с полным локальным
reflected API. Унаследованные копии не размножаются. Project-based discovery
и существующие native accessors остаются прежними: пакет генерирует функции,
а не ещё один список C++ классов или отдельный dispatcher.

## Первый проход — 2026-10-18

410 кандидатов: 241 generation_ready, 168 export_blocked, 0 generation_blocked,
1 policy_excluded. Весь staged DAS-модуль успешно скомпилирован в UE.
Шесть обёрток сохраняют construction guard.

| Класс | Ready | Export blocked | Policy excluded |
| --- | ---: | ---: | ---: |
| AIBlueprintHelperLibrary | 15 | 0 | 1 |
| AIController | 19 | 2 | 0 |
| AIPerceptionComponent | 9 | 1 | 0 |
| PathFollowingComponent | 0 | 4 | 0 |
| Character | 18 | 31 | 0 |
| CharacterMovementComponent | 39 | 5 | 0 |
| Controller | 21 | 7 | 0 |
| NavMovementComponent | 6 | 0 | 0 |
| Pawn | 26 | 8 | 0 |
| PawnMovementComponent | 6 | 2 | 0 |
| PlayerController | 66 | 96 | 0 |
| NavigationPath | 9 | 0 | 0 |
| NavigationSystemV1 | 7 | 12 | 0 |

Из 241 уникального ready-объявления 21 уже есть в baseline World/Math/Collision,
220 отсутствуют в этих пакетах. Это не подсчёт новых возможностей всей привязки:
часть уже доступна через native registry или managed API. Runtime-проверки
выборочны и не означают исполнения каждого из 241 методов.

Машинный отчёт: Bindings/Packages/ue_api_pawn_ai.summary.json;
поимённый backlog с native evidence: [PawnAINavigationBlockers](PawnAINavigationBlockers.md).

## Целевые проверки

```powershell
./Scripts/TestPawnAIPackage.ps1
# На уже собранном smoke Editor:
./Scripts/TestPawnAIPackage.ps1 -SkipBuild
# Повторная проверка конкретного свежего staged run:
./Scripts/TestPawnAIPackage.ps1 -SkipBuild -Report <run/report.json>
```

Runner без Report экспортирует и генерирует новый пакет, выполняет whole-module
preflight, затем DasPawnAI commandlet в transient Game world. Проверены:

- Pawn AddMovementInput, pending/consume/last input и пустой результат после consume;
- identity CharacterMovement owner, SetMovementMode(Flying) и PendingLaunchVelocity;
- AIController focus/clear, nullable outputs, UnPossess/Possess и связь Pawn/Controller;
- пустой NavigationPath: IsValid=false, native length/cost sentinel -1;
- native path из трёх точек: длина 200 см, valid/partial flags;
- пустые массивы currently/known perceived Actors, отсутствие perception record;
- отказы null receiver, неправильного receiver-класса и неправильной Sense MetaClass.

Пять независимых native groups — PASS. Совместный импорт с установленными
World/Math — PASS. Свежий native inventory дополняет generic export diagnostics.
DLL, fixture/template/profile и staged module не менялись во время проверки.
Engine errors/ensure отсутствуют. Итог: logs/pawn-ai-package-validation.json.
DAS source: Tests/PawnAI/probe.template.das.

Мир не содержит NavMesh и не выполняет movement ticks: LaunchCharacter здесь
проверяет постановку pending velocity, а не физический полёт. Native NavigationPath
собран из заданных точек и не является результатом поиска маршрута. Perception
query-проверки не создают стимулы и не доказывают sight/hearing detection.

## Оставшиеся категории

148 отказов относятся к visibility/deprecation policy. Из остальных:

- 13 static Blueprint APIs (11 NavigationSystemV1, два Pawn movement-base helpers)
  теперь поддержаны общим native static-call backend; текущий итог ниже.
- Шесть Client* объявлений PlayerController требуют отдельного разбора RPC policy.
  Networking остаётся отложенным; ослабления правил RPC не было.
- PlayDynamicForceFeedback требует special-call/latent adapter.
- CreateMoveToProxyObject намеренно исключён: используется existing lifecycle-managed
  ue_ai adapter, а не raw async proxy без контракта callbacks и ownership.

Managed MoveTo и реальный AI/NavMesh пример уже описаны в [AI](AI.md).
Этот этап не заменяет их и не повторяет их runtime suite. BehaviorTree/EQS,
perception stimuli/events, pathfinding queries, network и async ownership имеют
отдельные следующие проверки; это не объявление завершения всей AI-подсистемы.

## Следующий этап

Сгенерированные navigation queries на настоящем Recast NavMesh проверены —
см. [NavigationQueries](NavigationQueries.md). Следующий этап — perception
с настоящими sight/hearing stimuli и lifecycle callbacks.

Пакет остаётся staged и не публикуется в Content/Das/Scripts. Полные Foundation,
PIE, packaged и сеть не запускались.

## Native static вне BlueprintFunctionLibrary — 2026-10-18

Экспортёр, генератор и runtime допускают native static BlueprintCallable методы
любого загруженного `/Script/` native-класса. Вызов идёт через его CDO и обычный
ProcessEvent/call frame; ручных NavigationSystem wrappers нет. Сохраняются
FUNC_Native/Static/BlueprintCallable, запреты RPC/delegate/latent, специальные
адаптеры CustomThunk, проверка сигнатуры и construction policy. WorldContext
скрыт из параметров DAS и подставляется из live game world владельца скрипта;
заменить его вручную нельзя. Секция receivers пакета включает instance и static
методы; libraries по-прежнему допускает только static.

Свежий пакет: **254 ready, 155 export_blocked, 0 generation_blocked,
1 policy_excluded** из 410. NavigationSystemV1: 18 ready вместо 7; Pawn: 28
вместо 26. Дополнительно доступны 13 функций, все прошли generation/preflight
и runtime. 21 декларация пересекается с baseline World/Math/Collision,
233 отсутствуют в этих baseline. Эти числа не означают покрытие всего UE.

Точечная проверка использует transient мир **без NavigationSystem/NavMesh**:

- GetNavigationSystem, IsNavigationBeingBuilt/OrLocked;
- project, random navigable/reachable, raycast с native failure-семантикой;
- GetPathCost/Length: Error enum и zero-initialized out double;
- оба synchronous path вызова: nullable NavigationPath;
- Pawn.GetMovementBaseActor/Owner: nullable результаты;
- отказы instance-as-static, non-native owner, неверного WorldContext и ручной
  подмены automatic WorldContext.

Сборка Editor, 97 generator + 18 package tests, whole-module preflight,
семь runtime groups (включая исходные пять) и combined World/Math imports — PASS.
Engine errors/ensure отсутствуют, DLL/fixture hashes стабильны. Отчёт:
`logs/pawn-ai-package-validation.json`. Пакет остаётся staged. Успешные
NavMesh-запросы, полный PIE и packaged на этом шаге не запускались.

## Запросы на настоящем NavMesh — 2026-10-18

Добавлены transient Recast fixture и `Scripts/TestNavigationQueries.ps1`.
Сборка Editor, свежая генерация/preflight пакета и шесть групп native/DAS
navigation checks — PASS. Подтверждены project/random/reachable, blocked/clear
raycast, полные пути к location/Actor, Success cost/length и partial path к
изолированной площадке. Минимум восемь успешных случайных выборок каждого вида
с проверкой радиуса и NavMesh; максимум 64 попытки. Путь обходит препятствие:
1410.962 см при прямом расстоянии 1200 см. Native GetPathLength даёт отдельную
corridor-оценку 1714.689 см; native cost — 1878.951.

Engine errors/ensure отсутствуют, SHA256 DLL/fixture/module стабильны.
Отчёт: `logs/navigation-validation.json`; методика и ограничения:
[NavigationQueries](NavigationQueries.md). Карты не сохранялись, движение AI,
PIE и packaged не запускались. Runtime backend расширять не потребовалось.
