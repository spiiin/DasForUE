# Аудит макросов NimForUE для DSL DasForUE2

Дата: 2026-10-06. Прочитана локальная <NimForUE>, origin jmgomez/NimForUE,
commit `58140f4842a91a0187c17ce950e07181ba455862`. Это анализ исходников конкретной ревизии, не запуск NimForUE
и не подтверждение совместимости с нашим UE 5.8.3. Репозиторий NimForUE не изменялся.

## Что делает слой макросов

Главная цепочка: Nim AST → UEType/UEField → Nim declarations/accessors/call wrappers
и emitter registration → создание UE reflection и обновление runtime implementations.
Для native режима генерируются C++/Nim ABI adapters; для nuevm есть отдельный путь
через JSON типов, UECall/RuntimeField и borrow-dispatch.
См. [codegen/umacros.nim:311](https://github.com/jmgomez/NimForUE/blob/58140f4842a91a0187c17ce950e07181ba455862/src/nimforue/codegen/umacros.nim#L311), [codegen/ueemit.nim:539](https://github.com/jmgomez/NimForUE/blob/58140f4842a91a0187c17ce950e07181ba455862/src/nimforue/codegen/ueemit.nim#L539),
[vm/vmmacros.nim:404](https://github.com/jmgomez/NimForUE/blob/58140f4842a91a0187c17ce950e07181ba455862/src/nimforue/vm/vmmacros.nim#L404), [codegen/nuemacrocache.nim:55](https://github.com/jmgomez/NimForUE/blob/58140f4842a91a0187c17ce950e07181ba455862/src/nimforue/codegen/nuemacrocache.nim#L55).

| Возможность | Что найдено в реализации | Значение для нашей модели |
|---|---|---|
| uClass, explicit parent | Парсинг `of`, class flags, metadata, свойства, методы, конструктор, forward declarations | Несколько классов/баз, символы и capabilities backend; Actor — лишь первый адаптер |
| implements | Один или несколько интерфейсов; C++ interface offsets включают ExperimentalFields | Нужны interface contracts и explicit adapter capabilities; нельзя обещать универсальную поддержку |
| uStruct | Родитель struct, reflected и обычные Nim fields/methods; ufuncs в struct запрещены | Отдельные authoring struct declarations и ops/GC contracts, не только type_path на native struct |
| uEnum | Список идентификаторов/tuple; enum reflection и Nim type | Собственные enum declarations с явными int64 values/member IDs и member metadata |
| uprops/uprop | Общие specifiers на блок, несколько полей, tuple names, initializer AST | Общие annotation scopes, field-level overrides и отдельная модель initializer |
| property flags | BlueprintReadOnly/ReadWrite, EditAnywhere, VisibleAnywhere, ExposeOnSpawn, Transient, SaveGame, Config, Instanced, Interp, delegate/replication flags | Текущие editable/read_only недостаточны: нужна edit/visibility/access matrix и typed flags |
| metadata | Bare identifiers, key=value/key:value, shared type/function/property metadata, default Category | Разделить specifiers, metadata и macro-only directives; проверить известные правила и комбинации |
| ufuncs/uFunctions | Методы внутри класса и внешние blocks с self:type, общие annotations, индивидуальные pragmas; injected self/super | Collector частичных деклараций по class ID, несколько методов/entrypoints, удобный self/super |
| signatures | Return, out, var→out/ref, constp; CPP_Default_* из параметров | Param defaults/const semantics/metadata нужны в authoring модели, а не только binding exporter |
| Blueprint functions/events | Callable/Pure/Static, ImplementableEvent/NativeEvent, CallInEditor metadata, const для Nim func | Разные method kinds, UE→script dispatch, BP override и super/default implementation |
| defaults/constructors | Expr AST для property assignments, defaults block и uConstructor/FObjectInitializer; parent constructor chain | Литеральные CDO defaults и исполняемые initializer hooks должны быть разными сущностями |
| components | DefaultComponent создаёт default subobject; RootComponent, Attach/Socket и fallback scene root | Отдельный component graph со stable IDs, creation/root/attachment/migration policies |
| uDelegate | Macro создаёт dynamic multicast delegate type; delegate-valued properties распознаются отдельно | Authoring delegate declarations/signatures, kind/access flags; runtime subscriptions — отдельный слой |
| replication/RPC | Replicated/ReplicatedUsing, Cond/RepNotify; GetLifetimeReplicatedProps generator; Server/Client/NetMulticast/Reliable | Typed network schema с validation flags и server/client tests |
| virtual/override | C++ virtual generation и super; constructor/vtable update integration | Только явные native C++ adapters/generated glue для daScript |
| FieldNotify/GAS | Сеттеры и C++ boilerplate; gameplay attribute get/set/init и replication helper | Специализированные расширения модели и backend, не общие string metadata magic |
| source mapping | lineInfoObj, moduleRelativePath, setLineInfo, sourceHash | Source span, macro expansion trace и implementation dependency digest |
| ordinary Nim state | Non-reflected fields/procs допускаются внутри generated type | Script-private state отдельно от UE property layout, с собственной миграцией |

Основные источники:

- Classes/structs/enums/interfaces: [codegen/umacros.nim:19](https://github.com/jmgomez/NimForUE/blob/58140f4842a91a0187c17ce950e07181ba455862/src/nimforue/codegen/umacros.nim#L19), [codegen/umacros.nim:71](https://github.com/jmgomez/NimForUE/blob/58140f4842a91a0187c17ce950e07181ba455862/src/nimforue/codegen/umacros.nim#L71), [codegen/umacros.nim:453](https://github.com/jmgomez/NimForUE/blob/58140f4842a91a0187c17ce950e07181ba455862/src/nimforue/codegen/umacros.nim#L453).
- Flags, parameter directions/defaults, property parser: [codegen/uebindcore.nim:129](https://github.com/jmgomez/NimForUE/blob/58140f4842a91a0187c17ce950e07181ba455862/src/nimforue/codegen/uebindcore.nim#L129), [codegen/uebindcore.nim:173](https://github.com/jmgomez/NimForUE/blob/58140f4842a91a0187c17ce950e07181ba455862/src/nimforue/codegen/uebindcore.nim#L173), [codegen/uebindcore.nim:392](https://github.com/jmgomez/NimForUE/blob/58140f4842a91a0187c17ce950e07181ba455862/src/nimforue/codegen/uebindcore.nim#L392).
- Native thunks/return/out/super и events: [codegen/ueemit.nim:437](https://github.com/jmgomez/NimForUE/blob/58140f4842a91a0187c17ce950e07181ba455862/src/nimforue/codegen/ueemit.nim#L437).
- Constructors/default blocks: [codegen/ueemit.nim:377](https://github.com/jmgomez/NimForUE/blob/58140f4842a91a0187c17ce950e07181ba455862/src/nimforue/codegen/ueemit.nim#L377), [codegen/ueemit.nim:577](https://github.com/jmgomez/NimForUE/blob/58140f4842a91a0187c17ce950e07181ba455862/src/nimforue/codegen/ueemit.nim#L577), [codegen/ueemit.nim:626](https://github.com/jmgomez/NimForUE/blob/58140f4842a91a0187c17ce950e07181ba455862/src/nimforue/codegen/ueemit.nim#L626).
- Default subobjects/root/attachments: [codegen/uemeta.nim:812](https://github.com/jmgomez/NimForUE/blob/58140f4842a91a0187c17ce950e07181ba455862/src/nimforue/codegen/uemeta.nim#L812).
- Replication generator: [codegen/umacros.nim:239](https://github.com/jmgomez/NimForUE/blob/58140f4842a91a0187c17ce950e07181ba455862/src/nimforue/codegen/umacros.nim#L239); example [examples/netexample.nim:3](https://github.com/jmgomez/NimForUE/blob/58140f4842a91a0187c17ce950e07181ba455862/src/nimforue/examples/netexample.nim#L3).
- Delegates: [codegen/ueemit.nim:418](https://github.com/jmgomez/NimForUE/blob/58140f4842a91a0187c17ce950e07181ba455862/src/nimforue/codegen/ueemit.nim#L418), [codegen/modelconstructor.nim:1](https://github.com/jmgomez/NimForUE/blob/58140f4842a91a0187c17ce950e07181ba455862/src/nimforue/codegen/modelconstructor.nim#L1).
- FieldNotify/GAS: [codegen/umacros.nim:173](https://github.com/jmgomez/NimForUE/blob/58140f4842a91a0187c17ce950e07181ba455862/src/nimforue/codegen/umacros.nim#L173), [codegen/umacros.nim:275](https://github.com/jmgomez/NimForUE/blob/58140f4842a91a0187c17ce950e07181ba455862/src/nimforue/codegen/umacros.nim#L275), [codegen/uebindcore.nim:569](https://github.com/jmgomez/NimForUE/blob/58140f4842a91a0187c17ce950e07181ba455862/src/nimforue/codegen/uebindcore.nim#L569).

## Hot reload: механизм и пределы вывода

Emitter сравнивает прежний UEType с новым и при отличии готовит замену/reinstance
для classes/structs/enums/delegates. При неизменном классе обновляются constructor
и native function pointers; implementations имеют sourceHash.
См. [codegen/ueemit.nim:179](https://github.com/jmgomez/NimForUE/blob/58140f4842a91a0187c17ce950e07181ba455862/src/nimforue/codegen/ueemit.nim#L179), [codegen/ueemit.nim:220](https://github.com/jmgomez/NimForUE/blob/58140f4842a91a0187c17ce950e07181ba455862/src/nimforue/codegen/ueemit.nim#L220),
[codegen/ueemit.nim:296](https://github.com/jmgomez/NimForUE/blob/58140f4842a91a0187c17ce950e07181ba455862/src/nimforue/codegen/ueemit.nim#L296).

Это основание для hot reload без рестарта, но не свидетельство корректной миграции
любого изменения. В равенстве UEField/UEType сравнение metadata закомментировано;
class/struct flags тоже сравниваются не полностью. См. [codegen/models.nim:346](https://github.com/jmgomez/NimForUE/blob/58140f4842a91a0187c17ce950e07181ba455862/src/nimforue/codegen/models.nim#L346).
У нас metadata/default/implementation/structural diff должен оставаться явным.
Наш reload implementations должен заменять script call targets/contexts; менять
native pointer как Nim можно только там, где действительно существует C++ adapter.

## Важные ограничения конкретного DSL

1. Flags parser не является полной реализацией всех UHT specifiers: в коде есть
   TODO о взаимно исключающих flags. Не следует считать любую принятую metadata
   корректно применённым Unreal flag.
2. uStruct явно отказывает ufunc blocks; nuevm struct ветка содержит TODO по struct ops.
3. uDelegate напрямую вызывает makeUEMulDelegate: этот macro-path создаёт multicast,
   а не универсальную декларацию любого delegate kind.
4. uEnum macro собирает имена, а emitter использует ordinal index как int64 value.
   В этом пути нет общего парсинга explicit numeric enum assignments.
   См. [codegen/uemeta.nim:1070](https://github.com/jmgomez/NimForUE/blob/58140f4842a91a0187c17ce950e07181ba455862/src/nimforue/codegen/uemeta.nim#L1070).
5. Property initializer может быть expression с self; defaults **параметра функции**
   разбираются ограниченным набором AST forms. В parser есть TODO для function calls;
   пустая string default теряется при проверке defaultValue != empty string.
6. Constructor example говорит о приоритете пользовательского конструктора над
   property defaults, но constructorImpl вставляет fnBody перед assignments.
   Порядок инициализации нельзя переносить по комментарию: нужен отдельный тест.
7. Interfaces/virtuals имеют Experimental/TODO paths; есть emitter branch
   `uetInterface` с отказом. Из этого нельзя вывести одинаковые capabilities native/VM.
8. FieldNotify setter для UObjectPtr прямо помечен проблемным и пропускается
   в соответствующей генерации. Native pointers/layout/vtable tricks не подходят
   как общая схема безопасности daScript.

## Что добавить в DasForUE2

Текущая authoring v1 покрывает основу классов/сигнатур/IDs, literal defaults,
parent graph и ограниченную metadata. При этом backend умеет только Actor adapter,
int/float properties и один int→int main. Наличие IR type kinds не равно authoring
emission structs/enums/interfaces/delegates/components.

Ближайший контракт macro frontend:

1. Собрать declarations до mutation: общие annotation scopes + индивидуальные overrides,
   class/member IDs, несколько методов, bindings entrypoints, source spans.
2. Разделить Class/Property/Method/Parameter specifiers и metadata. Unknown/conflicting
   specifiers — ошибка либо capability refusal, с конкретной диагностикой.
3. Представить reflected properties и script-private state раздельно.
4. Сохранить signature directions, const, typed parameter defaults, event/override kind
   и super target. Начать со supported reflected adapters.
5. Добавить declarations UStruct/UEnum/delegate/interface как отдельные модели;
   enum values явные, IDs стабильные; migration policy отдельно.
6. Разделить literal defaults и initializer expressions/hooks. Явно определить
   CDO/instance/super/component/user constructor order и запрет side effects в preflight.
7. Component graph: type, stable subobject name, root, attach target, socket,
   instancing/ownership и правила reload/replacement.
8. Network descriptors и FieldNotify/GAS extensions добавлять отдельными слоями.
9. Runtime interpreted/AOT/native glue capabilities проверять независимо; один DSL
   не должен скрывать различия backends.

В существующем общем плане большинство направлений уже было, но текущая authoring
модель должна получить эти конкретные поля и ограничения. Синтаксис Nim не фиксирует
синтаксис daScript: нужны его возможности и контракт, а не буквальное копирование.
