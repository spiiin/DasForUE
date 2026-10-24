# Прямая привязка C++-типов: эскиз

Статус: реализовано (2026-10-24) как нативные привязки `DAS_NATIVE_BINDS` (`DasNativeBinds.h`): гайд —
[Native C++ binds](../../Guide/Native/NativeBinds.md), модули плагина — [справочник](../../Reference/NativeModules.md),
покрытие — [NativeBindsCoverage](../Architecture/NativeBindsCoverage.md). Ниже — исходный эскиз (2026-10-21). Связано: [TypeAdapters](TypeAdapters.md),
[аудит функций и событий AngelScript](../Audits/AngelScriptFunctionsEventsAudit.md) (авторская сторона, не привязка C++),
[Limitations](../../Reference/Limitations.md) («Only reflected API is exposed»), фаза 2 и 6 в
[ImprovementPlan](../Plans/ImprovementPlan.md).

## Зачем

Сейчас весь нативный API доступен скриптам только через рефлексию:

- вызов — `UFUNCTION(BlueprintCallable)` через `ProcessEvent` с кадром вызова (`DasMarshalling.inl:1128`);
  на каждый вызов — заполнение параметрического struct, проверки, разбор результата;
- значения — снимки `FInstancedStruct` (структуры, контейнеры), строки и имена — копии;
- недоступно: не-UFUNCTION методы (`AActor::IsHidden`, `FMath`, `FPaths`, …), операторы math-типов, не-BlueprintCallable
  функции, C++-типы без рефлексии.

Цель — дать скриптам прямой вызов C++ (одна косвенная функция с типизированными аргументами, без `FFrame` и
`ProcessEvent`) и привязку C++-типов с методами и операторами, **без правок движка**, оставив рефлексию запасным путём.

Не цель этого документа: классы скриптов как `UClass` без Blueprint-ассетов. Авторские классы остаются на
Blueprint-бэкенде; см. «Вне рамок».

## Как это сделано в Hazelight UnrealEngine-Angelscript

Исследован `<UnrealEngine-Angelscript>` (частичный клон: плагины `Engine/Plugins/Angelscript*` на диске, движок —
через `git show`; список правок движка неполный).

| Что | Как | Нужны правки движка |
| --- | --- | --- |
| Value-типы с методами и операторами (FVector, FRotator, FTransform, FQuat, FColor, …) | 113 ручных `Bind_*.cpp`: `ValueClass<FVector>("FVector")`, `.Method("FVector opAdd(...)", METHODPR_TRIVIAL(...))`, `.Property("float64 X", &FVector::X)` | нет |
| FString / FName / FText | ручные привязки | нет |
| TArray / TMap / TSet / TOptional | шаблонные типы VM поверх `FScriptArray/Map/Set`, раскладка как у UE — массивы передаются в UFUNCTION без копий | нет |
| USTRUCT и UPROPERTY | автоматически из рефлексии при старте: размер и ops из `UScriptStruct`, свойства по смещению | нет |
| UFUNCTION (BlueprintCallable, `ScriptCallable`) | **прямой указатель** на C++-функцию: UHT пишет `ERASE_METHOD_PTR(Class, Func, (точные C++-типы))` + шаблонный thunk `void(void** args, void* ret)` | **да** (UHT, CoreUObject) |
| BlueprintImplementable/NativeEvent | через `ProcessEvent`, чтобы работали переопределения в Blueprint | нет |
| Не-UFUNCTION методы и глобальные функции | ручные `ExistingClass("AActor").Method(...)`, `BindGlobalFunction(...)` в пространствах имён | нет |
| Mixin (методы-расширения) | `UCLASS(meta=ScriptMixin=…)`: статическая UFUNCTION с объектом первым параметром становится методом типа | `ScriptCallable` — да; через BlueprintCallable — нет |
| Вызов | ассемблерный ABI выключен; каждая привязка — шаблонный thunk | правка VM AngelScript, не движка |
| StaticJIT | байткод → C++, вызывающий `Obj->Func(args)` по имени с нужным `#include`; кэш привязок для cooked-сборок | нет |
| Классы скриптов как `UASClass` без ассетов | `RuntimeCallFunction`, маршрутизация Blueprint VM и `ProcessEvent` | **да** |

Вывод: всё, кроме указателей из UHT и `UASClass`, делается плагином. Указатели из UHT заменяются собственным
генератором C++ (ниже).

## Что у нас уже есть

- **Регистрация C++ в daScript**: `addExtern` с шаблонными обёртками (`DAS_BIND_FUN`, методы через
  `das_call_member`), `ManagedStructureAnnotation` / `ManagedValueAnnotation` для C++-структур, `MAKE_TYPE_FACTORY`.
  Это аналог API привязки AngelScript, ABI-код не нужен. Используется в `ue_foundation` и для `UEVector` и др.
  (`FMathValueAnnotation`, `DasRuntimeTypeAnnotations.inl`).
- **AOT daScript в C++** — аналог StaticJIT. Сейчас у всех UE-externs стоит `noAot`, чтобы сгенерированный C++ не обходил
  `GuardUEExtern` (`DasUEExternGuard.inl`).
- **Безопасные handle-объекты**: `FDasObjectHandle` (токен, `ResolveObject` проверяет жизнь, контекст, уничтожение
  актора — `DasRuntimeObjectAccess.inl:65`), типизированные view для классов (`ue_generated::…`).
- **Экспорт рефлексии в IR** (`DasBindingExport.inl` → `Bindings/*.json`) и генераторы (`GenerateBindings.py`,
  `GenerateApiPackage.py`, `GenerateNativeGameplay.py`): по IR строятся daScript-обёртки над `__ue_*`-вызовами.
  В IR функции уже есть `const`, `static`, `pure`, направления параметров, `declaring_class`.
- **Адаптеры типов** (фаза 2): один объект на value-тип — идентичность, проверка значения, view в daScript, упаковка.
  Math-структуры, GameplayTag и скаляры уже на адаптерах.

## Слои

```
L4  daScript API-пакеты (сгенерированные .das)       ue_api_world.das: actor.SetActorLocation(v) …
L3  C++-привязки модулей (сгенерированные + ручные)  DasNativeBinds_Engine.cpp: thunk'и на &AActor::Func
L2  Слой привязки (API плагина)                      DAS_NATIVE_CLASS / METHOD / STATIC / PROPERTY / OPERATOR
L1  Транспорт значений                               адаптеры типов: C++-тип <-> значение daScript
L0  Регистрация daScript                             addExtern, аннотации, модули, манифест externs
```

### L1. Транспорт значений

Каждый тип параметра и результата прямого вызова должен иметь C++-форму и правило передачи. Правила — данные адаптера
(новая часть «нативный транспорт» в `IDasTypeAdapter`):

| Вид | daScript | C++ | Передача |
| --- | --- | --- | --- |
| скаляры, bool, byte | `int`, `float`, … | `int32`, `float`, … | по значению, без копий |
| enum | `UEEnumValue` / типизированный enum | `TEnumAsByte<E>` / `E` | по значению с проверкой диапазона |
| Vector, Rotator, Transform, Vector2D | `UEVector` … (handle с полями) | `FVector` … | по значению или `const&` — та же память, проверка конечности на входе |
| FString / FName / FText | `string` / `UENameValue` / `UETextValue` | `FString` / `FName` / `FText` | копия на границе (`TCHAR`↔UTF-8); `FName` — по токену |
| UObject | типизированный handle | `T*` | `ResolveObject` + проверка класса; результат — в handle |
| USTRUCT (общий) | `UEStructValue` / авторский view | `const T&` / `T&` | этап 1: снимок (как сейчас), этап 3: указатель на память снимка без копии |
| TArray / TMap / TSet | `array` / `table` | `const TArray<T>&` / `TArray<T>&` | этап 1: копия; этап 4: handle поверх `FScriptArray` |
| out / ref | `var x : T&` | `T&` | временный C++-объект, после вызова — запись обратно (как сейчас для кадров) |

Функция, у которой хоть один тип без нативного транспорта, привязывается старым путём (`ProcessEvent`). Это решает
генератор, а не пользователь.

### L2. Слой привязки (API плагина)

Тонкий C++-API поверх `addExtern`, который скрывает безопасность и транспорт:

- `DAS_NATIVE_CLASS(Module, AActor)` — тип handle и наследование (для приведения и проверки класса);
- `DAS_NATIVE_METHOD(AActor, GetActorLocation)`, `…_STATIC`, `…_PROPERTY(FVector, X)`, `…_OPERATOR(FVector, +)`,
  `…_MIXIN(UKismetMathLibrary, Add_VectorVector, FVector)`;
- модификаторы: `WorldContext`, `NoDiscard`, «только game thread» (по умолчанию), «можно в PhysicsPreSim» (по умолчанию
  нет), «безопасно в ConstructionScript».

Каждая привязка разворачивается в шаблонный thunk, который:

1. вызывает guard (роль исполнения, изолированная VM, game thread — то же, что `GuardUEExtern`);
2. превращает аргументы daScript в C++ через L1 (handle → `T*` с проверкой жизни и класса);
3. вызывает функцию напрямую;
4. превращает результат и out-параметры обратно.

Регистрация попадает в манифест externs (как `Bindings/FoundationExterns.txt`, отдельный файл на модуль привязок), чтобы
изменения видимой скриптам поверхности были осознанными.

### L3. C++-привязки модулей

Два источника, один формат:

- **сгенерированные** — для UFUNCTION (генератор, ниже);
- **ручные** — для всего, что не в рефлексии: методы math-типов и операторы, `FMath`, `FPaths`, не-UFUNCTION методы
  AActor/UWorld/USceneComponent, строки. Один файл на тип или область (`Bind_FVector.cpp`, `Bind_FMath.cpp`), как у
  AngelScript. Парсера заголовков нет и не планируется: ручные файлы растут по потребности.

### L4. daScript API-пакеты

Сгенерированные пакеты (`Docs/Api/*`) остаются тем, что видит пользователь. Меняется только то, что внутри обёртки:
для функций с прямой привязкой — вызов extern из L3, для остальных — нынешний путь через кадр вызова. Сигнатуры в
daScript не меняются, поэтому скрипты пользователей не переписываются.

## Генератор (замена указателей из UHT)

Вход — тот же IR из `DasBindingExport`, расширенный полями (собираются в редакторе, где есть метаданные):

| Поле | Откуда | Зачем |
| --- | --- | --- |
| C++-форма параметра: const, `&`, `TEnumAsByte`, указатель | `CPF_ConstParm`, `CPF_ReferenceParm`, `CPF_OutParm`, тип свойства | точный тип в `&Class::Func` и в thunk |
| заголовок | метаданные `ModuleRelativePath` / `IncludePath` класса | `#include` в сгенерированном .cpp |
| модуль UE | пакет класса (`/Script/Engine`) | зависимости модуля сборки |
| доступ, CustomThunk | `FUNC_Public/Protected/Private`, метаданные `CustomThunk` | такие функции — только `ProcessEvent` |
| BlueprintNativeEvent | `FUNC_Event` + `_Implementation` | всегда `ProcessEvent` (переопределения в Blueprint) |

Выход:

- `Generated/DasNativeBinds_<Package>.cpp` — список привязок L2 для подходящих функций;
- `Bindings/<Package>.native.json` — какие функции получили прямой путь и почему остальные нет (для отчёта покрытия,
  как `ApiCoverage`);
- изменения в `.das` пакета (L4).

Где собирается: в модуле плагина `DasNativeBinds` с зависимостями от модулей UE, которые реально используются
(`Engine`, `UMG`, `AIModule`, …). Генерация и сборка — та же модель, что у сегодняшних сгенерированных привязок:
результат принадлежит одной версии движка и API, проверка свежести — `-Check`, как у `GenerateNativeGameplay.ps1`.

Проверка сигнатуры: в сгенерированном .cpp — `static_assert` на тип указателя (`decltype(&AActor::Func)` против
ожидаемого). Неверная C++-форма из рефлексии ломает сборку, а не вызов. Это заменяет флаги, которые Hazelight добавлял
в UHT.

## Безопасность

Прямой вызов убирает слой, который сейчас ловит ошибки (`ProcessEvent` с проверкой кадра). Обязательные правила thunk'а:

- **жизнь объекта**: только через handle и `ResolveObject` (сборка мусора, уничтоженный актор, чужой контекст); никаких
  сырых `UObject*` в daScript;
- **класс**: проверка `IsA` при разыменовании handle и при приведении результата;
- **поток и роль**: guard как у `GuardUEExtern` (game thread, `PhysicsPreSim`, замороженный мир, изолированная VM);
- **ConstructionScript**: функции, небезопасные при конструировании, помечаются в привязке (как `ConstructionUnsafe`
  в реестре native gameplay);
- **значения**: конечность math-аргументов (адаптер `ValidateValue`), диапазоны enum, длина строк;
- **повторный вход в VM**: C++-функция, вызывающая обратно в daScript через делегат, должна идти по уже существующему
  правилу запрета повторного входа;
- **ссылки на память снимков** (этап 3): живут только на время вызова; результат-ссылка копируется.

Правила проверяются тестами: на каждую категорию — негативный тест (уничтоженный объект, неверный класс, NaN, вызов из
PhysicsPreSim), как сейчас для `ue_foundation`.

## AOT и cooked-сборки

- Guard переезжает внутрь thunk'а привязки. Тогда `noAot` не нужен: AOT daScript вызывает thunk напрямую, и guard
  обойти нельзя. Это отдельный шаг, после которого возможен AOT пути игры (сейчас его нет, см. Limitations).
- Метаданные UFUNCTION в cooked-сборке вырезаны, но генератору они нужны только в редакторе: в игру попадает уже
  скомпилированный C++ и таблица привязок. Отдельный кэш привязок, как `Binds.Cache` у AngelScript, не нужен.

## Этапы

| Этап | Что | Готово, когда |
| --- | --- | --- |
| N0. Пилот и замер | Вручную 5–10 частых функций (`GetActorLocation`, `SetActorLocation`, `GetComponentLocation`, `K2_SetWorldLocation`, `GetWorld`…) через `addExtern` + guard + handle; бенчмарк против `ProcessEvent` в foundation | есть цифры «вызов через кадр» против «прямой вызов»; решение, идти ли дальше |
| N1. Слой L2 | API привязки, транспорт L1 для скаляров, enum, math, строк, UObject; манифест модуля привязок; негативные тесты безопасности | пилот переписан на L2, все правила безопасности покрыты тестами |
| N2. Генератор | Поля IR (C++-форма, заголовок, доступ, CustomThunk), генерация .cpp и отчёта, переключение L4 для подходящих функций | API-пакет `WorldActorComponents` вызывает подходящие функции напрямую; отчёт показывает причины отказов; полный пресет зелёный |
| N3. Структуры без копий | USTRUCT-параметры `const&` по памяти снимка | нет копий снимка в замере на структурном параметре |
| N4. Ручные value-типы | методы и операторы math-типов, `FMath` (закрывает «нет операторов у `UEVector`») | пакет Math использует операторы; Limitations обновлён |
| N5. Контейнеры | handle-типы поверх `FScriptArray/Map/Set` вместо копий | `TArray`-параметр без копии; лимиты снимков не применяются к прямым вызовам |
| N6. AOT | guard внутри thunk'ов, снять `noAot` с прямых привязок | AOT-тест вызывает прямую привязку; guard срабатывает в AOT |

N0 обязателен: если выигрыш против `ProcessEvent` небольшой на типичных вызовах, дальше имеет смысл только N4
(операторы и `FMath`, которых сейчас нет вообще), а генератор — нет.

## Риски

- **Объём ручных привязок.** У AngelScript 113 файлов. Без парсера заголовков покрытие растёт медленно; приоритет —
  по реальному использованию в примерах.
- **Хрупкость к версии движка.** Сгенерированный .cpp ломается при смене сигнатур; это ловит сборка (`static_assert`),
  а не рантайм — приемлемо, но требует перегенерации на каждую версию.
- **Время сборки** модуля привязок со многими заголовками движка.
- **Две семантики вызова.** Прямой путь не вызывает переопределения Blueprint у не-event функций (так и в C++), а
  путь через `ProcessEvent` для BlueprintNativeEvent — вызывает. Генератор обязан держать события на `ProcessEvent`.
- **Повторный вход в VM** из C++, который сейчас поймал бы кадр вызова, при прямом вызове должен ловиться guard'ом.

## Вне рамок

- Классы скриптов как `UClass` без Blueprint-ассетов (у AngelScript — `UASClass` с правками `ScriptCore.cpp`). Без правок
  движка возможно только частично (нативные exec-thunk'и `FNativeFuncPtr` на функцию); на привязку C++-типов не влияет.
- Парсер C++-заголовков (libclang) для автоматической привязки не-рефлектированного API.
- Сеть, репликация, RPC.

## Вопросы на согласование

1. Начинать с N0 (пилот и замер) и решать о генераторе по цифрам?
2. Модуль привязок — отдельный модуль плагина `DasNativeBinds` или часть `DasRuntime`?
3. Генерировать прямые привязки для всех подходящих функций API-пакетов или только по списку (профиль пакета)?
4. Нужен ли прямой вызов в Shipping/AOT уже на N1, или сначала только Development-интерпретатор?
