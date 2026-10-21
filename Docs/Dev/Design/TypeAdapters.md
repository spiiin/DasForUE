# Адаптеры типов (фаза 2, шаг 2.1): эскиз

Статус: шаг 2.1 и перенос GameplayTag реализованы (2026-10-20). План: [ImprovementPlan](../Plans/ImprovementPlan.md), фаза 2.

Отличия реализации от эскиза ниже:
- скриптовый фасет передаётся при регистрации (`Register(Adapter, &Facet)`), а не через метод адаптера:
  реестр — публичный API, а типы AST daScript есть только внутри `DasRuntime.cpp`;
- `MatchesDeclared` и `ValidateType` не понадобились: реестр сопоставляет по `GetStruct()` / `GetKind()`;
- `ue_struct` тоже не принимал тег как значение map; исправлено вместе с `DasReflectedTypes.cpp`;
- гейт — `Scripts/TestTypeAdapterGate.py` (Python-набор), проверка реестра — foundation
  `type_adapters_reachable_by_identity`, map-значения — `authored_struct_tag_map_values` в `TestGameplayTags.ps1`.

## Зачем

Специальные типы значений (GameplayTag, math-структуры, soft-обёртки, геометрия, input events)
не являются отдельными видами: это `struct` с известным путём, которому разные подсистемы
добавляют свои ветки. Признак «это мой тип» существует в нескольких формах сразу:

- C++-идентичность структуры (`DasIsGameplayTagType`, `DasIsGeometryType`);
- строковый путь (`IsDasGameplayTagPath`, `IsDasMathStructPath`, `DasSoftProperty`, литералы);
- daScript-представление (`DSLGameplay`, `DSLSoft`, `DSLMathPath`, 81 вызов).

Адаптер собирает идентичность и решения одного типа в одном месте; подсистемы спрашивают
реестр вместо собственных предикатов.

## Что показала инвентаризация GameplayTag

Особые ветки тегов есть только в четырёх слоях; в остальных тег проходит общий путь `struct`:

| Слой | Места | Решение |
| --- | --- | --- |
| Валидация авторских типов | `DasReflectedTypes.cpp:15-19, 339-340, 361-366` | тег — листовое значение, не авторская дочерняя структура; дефолт только неявный пустой |
| Валидация значений | `DasStructValidation.cpp:231` → `DasGameplayTags.cpp:7-49` | тег зарегистрирован; контейнер ≤256, без пустых/дублей, кэш родителей канонический |
| DSL | `DasGameplayTagsDSL.inl`, `DasDSL.inl:364/486/518`, `DasDirectTypes.inl:30/104/417/452/507`, `DasDSLTypeAnnotations.inl:245/403/422`, `DasNativeGameplayDSL.inl:83-93` | вид `struct`, путь по имени view, pack/unpack через `__ue_tag_check`, без помощников полей, поле без инициализатора, нативное свойство → view из `ue_gameplay_tags` |
| Редактор, дефолты | `DasEditor/.../DasBlueprintDefaults.cpp:34` | дефолт экспортируется пустой строкой |

Нет особых веток в: создании свойств, пинах, миграции, call frames/маршалинге, генераторах.
Литералы путей: `DasGameplayTagsDSL.inl:13-14`, `DasNativeGameplayDSL.inl:83-84`,
`DasReflectedTypes.cpp:17-18`, `ue_gameplay_tags.das:15`.

Найдена несогласованность: `DasReflectedTypes.cpp:263` освобождает от поиска авторской структуры
map-значения soft и math, но не теги — авторский `table<K; GameplayTag>` даёт
«Unresolved authored map struct value», хотя DSL (`DasDSL.inl:518`) такое значение строит.

## Интерфейс

Один объект на тип, с независимыми фасетами. Фасет есть только у тех слоёв, где у типа реально
есть решения; остальное идёт общим путём. Хуки из плана, которых сейчас никому не нужно
(`MakePin`, `Migrate`, `Marshal`), не добавляются заранее: появятся вместе с первым типом,
которому нужны (soft — `MakeProperty`, math — пины).

```cpp
// DasRuntime/Public/DasTypeAdapters.h
enum class EDasAdapterDefault : uint8 { Explicit, ImplicitEmpty };

class DASRUNTIME_API IDasTypeAdapter
{
public:
    virtual ~IDasTypeAdapter() = default;
    virtual FName GetName() const = 0;                       // "gameplay_tag", "scalar.int"

    // Идентичность во всех формах. Путь берётся из StaticStruct(), не из литерала.
    virtual bool MatchesDeclared(const FDasDeclaredType& Type) const = 0;
    virtual const UScriptStruct* GetStruct() const { return nullptr; } // для struct-адаптеров
    virtual bool MatchesProperty(const FProperty* Property) const { return false; }

    // Тип: политика авторских объявлений.
    virtual bool IsLeafValue() const { return false; }        // не разворачивается как авторская структура
    virtual EDasAdapterDefault GetDefaultPolicy() const { return EDasAdapterDefault::Explicit; }
    virtual bool ValidateType(const FDasDeclaredType&, EDasPropertyUsage, FString& Error) const { return true; }
    virtual FProperty* MakeProperty(FFieldVariant Owner, FName Name, const FDasDeclaredType&,
                                    EDasPropertyUsage) const { return nullptr; } // nullptr → общий Build

    // Значения: снапшоты, вложенные значения, аргументы.
    virtual bool ValidateValue(const UScriptStruct* Type, const void* Data, FString& Error) const { return true; }

    // DSL (время компиляции скрипта); nullptr — тип не имеет своего view.
    virtual const class IDasScriptTypeFacet* GetScriptFacet() const { return nullptr; }
};

// DasRuntime/Private/DasScriptTypeFacet.h — зависит от das AST, поэтому не в Public.
class IDasScriptTypeFacet
{
public:
    virtual bool MatchesScriptType(const das::TypeDeclPtr& Type) const = 0; // было DSLGameplay
    virtual FString GetTypePath(const das::TypeDeclPtr& Type) const = 0;    // было DSLGameplayPath
    virtual das::ExpressionPtr Pack(const das::LineInfo&, const das::TypeDeclPtr&, das::ExpressionPtr) const = 0;
    virtual das::ExpressionPtr Unpack(const das::LineInfo&, const das::TypeDeclPtr&, das::ExpressionPtr) const = 0;
    virtual bool NeedsFieldHelpers() const { return false; }
    // Тип view для нативного свойства этого пути и модуль, который нужно импортировать.
    virtual bool NativeView(const FString& Path, das::string& Type, FString& Module) const = 0;
};
```

Реестр:

```cpp
class DASRUNTIME_API FDasTypeAdapters
{
public:
    static FDasTypeAdapters& Get();
    void Register(TSharedRef<IDasTypeAdapter> Adapter);       // StartupModule, game thread
    const IDasTypeAdapter* Find(const FDasDeclaredType& Type) const;
    const IDasTypeAdapter* FindStruct(const UScriptStruct* Type) const;   // TMap по указателю
    const IDasTypeAdapter* FindProperty(const FProperty* Property) const;
    const IDasTypeAdapter* FindScript(const das::TypeDeclPtr& Type) const; // Private overload
    TConstArrayView<TSharedRef<IDasTypeAdapter>> All() const; // для шага 2.4
};
```

- Struct-адаптеры ищутся по `UScriptStruct*` (O(1)); адаптеры видов (`Scalar<K>`) — по `EDasTypeKind`.
- Регистрация только при старте модуля; после — только чтение, без блокировок.
- Встроенные адаптеры регистрирует `FDasRuntimeModule::StartupModule`; проверка foundation:
  у каждого struct-адаптера `GetStruct()` не null и `MatchesDeclared` согласован с `GetStruct()->GetPathName()`.

## Первый адаптер (гейт 2.1): `Scalar<K>`

По строке на каждый вид из `DAS_SCALAR_ACCESSOR_KINDS` (int, float, double, bool, string, name, byte,
text). Забирает ровно два решения, у которых сейчас есть `switch`:

- `MakeProperty`: ветки `Build` в `DasPropertyTypes.cpp:191-210` (FByteProperty … FNameProperty, bool с
  `SetBoolSize`);
- `MatchesProperty`: соответствующие ветки `DasPropertyKind`.

`Build` и `DasPropertyKind` сначала спрашивают реестр, затем идут по оставшимся видам. Поведение и
манифест externs не меняются; гейт — foundation и полный пресет.

## Перенос GameplayTag (первый вид шага 2.2)

1. `DasGameplayTags.cpp`: `FDasGameplayTagAdapter` — два экземпляра (тег и контейнер), идентичность из
   `FGameplayTag::StaticStruct()` / `FGameplayTagContainer::StaticStruct()`; `IsLeafValue = true`;
   `ImplicitEmpty`; `ValidateValue` = нынешнее тело `ValidateDasGameplayTagValue`.
   Script-фасет = нынешние `DSLGameplay*` из `DasGameplayTagsDSL.inl` (переезжают в адаптер).
2. Значения: `DasStructValidation.cpp:231` → `FindStruct(Type)->ValidateValue`. Этот же вызов потом
   заберёт геометрию и input events без новых веток.
3. Авторские типы: `DasReflectedTypes.cpp` 339-340, 361-366 → `IsLeafValue` / `GetDefaultPolicy`;
   `IsDasGameplayTagPath` удаляется. Строка 263 начинает спрашивать `IsLeafValue` — это исправляет
   отказ для `table<K; GameplayTag>` (поведение меняется, добавляется тест).
4. DSL: `DSLTypeName`, `DSLContainerType`, `DSLDirectStruct/Path/Pack/Unpack`, `DSLEnsureStructHelpers`,
   аннотация `ue_struct` и `DSLNativeValueType` спрашивают `FindScript` / `NativeView` вместо
   `DSLGameplay*` и литералов. `DSLSoft` в тех же местах остаётся до переноса soft.
5. Редактор: `DasBlueprintDefaults.cpp:34` → `GetDefaultPolicy() == ImplicitEmpty`.
6. Удаляются `DasIsGameplayTagType`, `IsDasGameplayTagPath`, литералы путей и лишний
   `#include "DasGameplayTags.h"` в `DasRuntime.cpp`.

Гейт переноса: вне файлов адаптера нет `/Script/GameplayTags.` и `DSLGameplay`/`IsDasGameplayTagPath`
(проверка grep в тесте); `TestGameplayTags.ps1`, foundation и полный пресет проходят.

Не переносится: `ue_gameplay_tags.das:15` — путь контейнера в daScript-модуле (`ue_tags()` создаёт пустой
контейнер через `ue_struct_value(path)`). Можно заменить отдельным extern `__ue_tags_empty()`, но это
расширяет манифест ради одной строки; предлагаю оставить.

## Следующие виды

Порядок плана: math-структуры (`DSLMathPath` 81 вызов, `IsDasMathStructPath`), soft-обёртки
(`DSLSoft`, `DasSoftProperty`; им нужен `MakeProperty`), затем Text/Name, enum, object/class.
Геометрия и input events уже имеют свои `Validate…Data` и встают в тот же `ValidateValue`.

## Вопросы на согласование

1. Фасеты на одном объекте (как выше) или отдельные интерфейсы с общим ключом?
2. Хуки только по факту использования (`MakePin`, `Migrate`, `Marshal` — позже) — отклонение от
   списка в плане.
3. Чинить map-значения тегов (`DasReflectedTypes.cpp:263`) в том же шаге?
4. Оставить литерал пути в `ue_gameplay_tags.das`?

## Перенос math-структур (2026-10-21)

Vector, Vector2D, Rotator, Transform и InputActionValue перенесены в четыре этапа (2.2a–2.2d). Их view в daScript —
handle-типы `ue_foundation`, поэтому фасет узнаёт и handle (`DSLAdaptedHandle`), а `DSLAdapted` по-прежнему ищет
только struct-view: иначе поменялось бы поведение math там, где `DSLAdapted` уже стоит.

Хуки, добавленные для math:

| Хук | Где | Данные |
| --- | --- | --- |
| `IsMath()` | адаптер | V, V2, R, T: лист, текстовые дефолты, конечные значения (`ValidateValue`; у T нормализованный поворот) |
| `GetDefaultKind()` | адаптер | `vector`/`vector2d`/`rotator`/`transform`: дефолты конструкторов и типы payload колбэков |
| `GetSignature()` | адаптер | V/R/T → `EDasMethodSignature` 9/10/11 (номера записаны в данных) |
| `GetViewUses()`, `MakeView()` | фасет | где используется view: нативное свойство, self, авторская ссылка; у V2 нет авторской ссылки, у IAV ни одного |
| `HasMember()` | фасет | x/y/z, pitch/yaw/roll; V2 — только через авторские ссылки, не как self-лист |
| `GetCallBridge()` | фасет | свой слот кадра (V, R, T) и мост метода (V, R, T, IAV); иначе снимок struct |

Найденная ошибка: дефолт авторского поля Vector2D форматировался как Transform (`DasTypeService.cpp`), тест
`DasForUE.Types.MathFieldDefaults`. Поведение изменено в одном месте: поле InputActionValue без инициализатора
теперь явная ошибка, а не молчаливый дефолт Transform.

Остались как предметная логика (не решения по типу): разбор конструктора `ue_transform`, свойство `ActorTransform`,
контракты Enhanced Input, ObjectClass payload Vector2D, текстовые грамматики дефолтов в редакторе. Python-генераторы
(`CANONICAL_MATH`) — фаза 6.
