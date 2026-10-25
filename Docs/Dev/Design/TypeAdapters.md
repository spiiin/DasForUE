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


## Перенос soft references (2026-10-25)

Soft object/class references используют один адаптер семейства `FDasSoftReferenceAdapter`.
В отличие от math-структур их carrier зависит от native base class и создаётся по требованию.
Реестр хранит неизменный список семейств: точный поиск статических структур остаётся прежним,
а семейство проверяет путь через реестр реально созданных carriers. Само сходство имени не
означает принадлежность типу. Создание нового carrier не добавляет элементы в реестр адаптеров
и не инвалидирует указатели на его записи. Сброс carriers не оставляет в адаптере указателей на них.

Адаптер отвечает за native FSoftObjectProperty/FSoftClassProperty, ограничение класса,
CPF_UObjectWrapper, листовую политику авторских структур и их пустой default (`None`).
GetCarrierProperty подключает soft-значения к существующему owned-snapshot transport.
Пути и wire-формат carriers не изменены.

`FDasSoftReferenceScriptFacet` владеет идентификацией `ue_soft_ref`, получением carrier path,
pack/unpack, литеральными defaults полей и контейнеров, выбором generated native view и
преобразованием к родительскому ограничению. Общий DSL вызывает фасет вместо `DSLSoft*`.
Непустые defaults внутри `ue_struct` по-прежнему запрещены; defaults обычных классов и
контейнеров по-прежнему проходят проверку literal path и точного типа.

Предметные операции остаются в soft-модуле: проверка asset paths и resolved objects,
resolve/load, ограничения на преобразования object/class, отмена асинхронных запросов.
Они используют тот же carrier и существующие проверки времени жизни, без eager loading.
`DasRuntimeInputSessions.inl` сохраняет доступ к soft property для реализации async load.

Регрессии: foundation проверяет четыре комбинации native base × object/class, создание
свойства и обратное описание, отказ для незарегистрированных путей и неизменность реестра.
`TestTypeAdapterGate.py` запрещает возврат soft-предикатов в общий DSL и property builder.
Существующие soft-reference foundation/PIE tests покрывают defaults, GC, generated native
views, upcasts, загрузку и отмену запросов; общий automation-набор проверяет соседние типы.


## Перенос Text/Name (2026-10-25)

Name и Text остаются scalar-адаптерами, использующими FNameProperty/FTextProperty и
общие FDasKindName/FDasKindText accessor traits. К ним подключён
`FDasTextNameScriptFacet` из `DasTextNameTypesDSL.inl`:

- распознавание UENameValue/UETextValue и выбор view для native/authored свойств;
- разбор литеральных defaults обычных классов и контейнеров;
- FText defaults в ue_struct и параметрах методов, восстановление default AST;
- проверка runtime-сигнатуры через тот же фасет.

`DSLValueAdapter` ищет все представления; существующие `DSLAdapted` и
`DSLAdaptedHandle` ограничены Struct kind. Поэтому scalar handles Name/Text не
попадают в struct snapshot pack/unpack и не требуют UScriptStruct или TypePath.

Сайт defaults передаётся явно (`Property`, `StructField`, `Parameter`). Это сохраняет
существующие различия синтаксиса: ue_name_value принимается как default обычного поля
и элемента контейнера, но не становится новым default параметра или ue_struct-поля.
FText сохраняет wire с namespace/key/source либо признак culture-invariant; он не
превращается в обычную строку. Локализационные операции, владение runtime-снимками и
грамматика сериализации остаются в существующих runtime/scalar helpers.

Foundation дополнительно проверяет scalar ABI и точный локализационный default поля
и параметра, а также nominal Name default. Архитектурный gate запрещает отдельные
UETextValue/UENameValue predicates и DSLTextDefault вне фасета и объявления аннотаций.


## Перенос Enum (2026-10-25)

`FDasEnumAdapter` — адаптер вида Enum с обязательной номинальной идентичностью UEnum.
`UsesTypeIdentity` позволяет искать такой адаптер по виду, сохраняя TypePath в дескрипторе.
Наличие адаптера не означает валидность пути: `ValidateDeclared` проверяет загруженный UEnum
и прежний диапазон значений 0..254. Общие ограничения формы дескриптора остаются в validator.

Адаптер владеет созданием и описанием FByteProperty с Enum и FEnumProperty с byte underlying.
`DasResolvedEnumStorage` расположен рядом с ним и сохраняет существующую политику Automatic:
native enum class использует Enum8, UDasDefinedEnum — Byte; явное storage сохраняется.
Описание чужого свойства с широким underlying type отклоняется. Byte без Enum остаётся
обычным uint8 scalar и не становится enum. Идентичность не выводится из набора чисел.

`FDasEnumScriptFacet` владеет распознаванием ue_enum/ue_native_enum, разрешением авторского
пути и проверкой member IDs, native view, checked pack/unpack и именованными defaults полей
и контейнеров. Declaration annotations вынесены в `DasEnumAnnotations.inl`; формат IR,
пути assets и имена generated entry points сохранены. Общие DSL-участки спрашивают
`DSLTypedValue`/`DSLNativeValue` и фасет вместо собственных enum-предикатов.

Разделены source type и call type: daScript enumeration в пользовательском коде и
UEEnumValue token в экспортируемом ABI. `MatchesCallType` не позволяет принять source enum
с integer layout за token. Native и авторские значения продолжают проходить
ue_enum_from_number / __ue_checked_enum_number с проверкой конкретного UEnum.

Foundation расширен проверками native/authored × Automatic/Byte/Enum8, обратного описания,
неизвестного пути и недопустимого значения. Существующий property factory probe проверяет
оба enum storage в array/map/set, широкие underlying types и сохранение StorageIdentity.
Method-values и automation покрывают typed values, несовпадающие enum с одинаковыми числами,
параметры/результаты/ref/out, defaults, сохранение/открытие assets, delegates и reload.

Транспорт контейнеров и предметные операции над enum tokens используют прежние shared
runtime helpers. Новые формы enum, числовые диапазоны и defaults параметров не добавлены.


## Перенос Object/Class (2026-10-25)

`FDasObjectClassAdapter` регистрируется для двух видов с TypePath: Object и Class.
Создание FObjectProperty, FWeakObjectProperty, FClassProperty и совместимое существующее
представление FInterfaceProperty теперь принадлежат адаптеру. Сохранены ограничения:
weak применим только к конкретному object, class не принимает interface base, direct weak
containers и containers class references остаются закрыты. Container shape/depth/node
проверяет общий validator, существование класса и reference-specific ограничения — адаптер.
Обратное описание сохраняет PropertyClass/MetaClass и WeakReference, не объединяя storage.

`FDasObjectClassScriptFacet` выбирает source view и raw ABI для обоих видов. Strong/weak
object capability, native/interface casts, authored nullable/stable-ID check и class-base
check продолжают использовать прежние runtime codecs. Изменены выбор и размещение правил,
а не обработка GC, Destroy, proxy cancellation или lease lifetime. Source wrappers и raw
UObjectHandle/UEClassHandle различаются через MatchesScriptType/MatchesCallType.

Declaration/default policy вынесена в `DasReferenceDefaultsDSL.inl`: typed class constraint,
authored reference ID, legacy ue_object_value, ue_asset_default/ue_class_default и object
container asset defaults. Field initializer очищается и PendingReferences проверяется тем же
кодом, что и раньше. Private scope FDasScriptTypeScope хранит прежние ModuleId/Folder/Registry
и Group/Module; FDSLTypeScope — его alias, чтобы передать declaration facet полный scope.

Existing interface reference representation остаётся совместимой частью Object. Отдельные
контракты interface dispatch, ImplementsInterface и authored interface definitions остаются
следующим шагом; этот перенос не добавляет новые interface-возможности.

Foundation проверяет object/class × UObject/Actor/Interface × strong/weak: реальный класс
FProperty, base constraint, StorageIdentity после описания и отказы для weak class/interface
и неизвестного base. Существующие weak/GC/lifecycle и method-values/PIE tests проверяют
время жизни, неверные native/authored типы, параметры/результаты/ref/out и callbacks.
Native/computed-default suites проверяют сохранённую обработку reference defaults.
Архитектурный gate запрещает reference property construction вне адаптера и raw ABI names
в общих builder/conversion/signature участках DSL.


## Interface-контракты (2026-10-25)

Interface references сохраняют Object kind и FInterfaceProperty. Нового value kind или
второго реестра типов нет. `DasPropertyMatchesContract` использует адаптеры для nominal
matching параметров/результатов: точный UEnum, PropertyClass/MetaClass, scalar kind и
зарегистрированный math signature. Byte и Enum8 одного UEnum совместимы как контракт;
их физическое storage по-прежнему различает property descriptor. Native fixed arrays и
wide enum underlying отклоняются. Carrier SameType и точный UScriptStruct сохраняют
существующие правила struct/container ABI. Направления in/out/ref и purity/const остаются
в interface preflight: это правила метода, а не отдельного типа значения.

`DasInterfaceContracts` предоставляет общие проверки identity, ImplementsInterface и
разрешение UFunction реализации. Они используются typed interface annotations, preflight,
runtime cast/membership и dispatch. Эти helpers работают только с уже загруженным UClass.
Ограничения /Script/ и /Game/ сохраняются на соответствующих входах. Native/обычный
Blueprint interface не имеет authored ID; UDasAuthoredInterfaceClass требует непустой
StableId. Generated authored reference обязан совпадать с этим ID.

Interface call frame запоминает StableId отдельно от ключа сигнатуры. Перед ProcessEvent
проверяются исходная identity, сигнатура объявления, membership и текущая UFunction.
Изменившийся контракт требует нового frame; формат generated signature key остаётся прежним.
Время жизни frame ограничено активным вызовом VM, как и раньше: это не долгоживущий handle.

Проверки: foundation покрывает object/class constraints, оба enum storage, scalar contracts,
fixed/wide rejection и смену/утрату identity. Suite `interface-contracts` запускает существующий
AuthoredInterface Create/Reopen probe: реальные assets, generated wrappers и reflected calls,
дополненные проверками общего resolver. Reopen создаёт frame, меняет identity через тестовую native-функцию и проверяет отказ без изменения Score; после восстановления ID новый frame выполняет AddScore. Временный скрипт хранится в Saved, mutation helper — только в test probes. Method-values и automation остаются общей регрессией.
Физическое хранение/GC interface references и контейнерные правила не менялись. Container
combinators остаются следующим шагом переноса адаптеров.
