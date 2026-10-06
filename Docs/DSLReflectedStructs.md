# Общий адаптер reflected value structs

DSL получил `UEStructValue` для supported loaded native UScriptStructs.
Один reflection adapter обслуживает разные types, без копирования C++ layout
в daScript и отдельного C++ thunk на каждую структуру. Пример использует
DasCallPayload, уже присутствующий в exported binding IR.

~~~das
options gen2
require ue_dsl
[ue_class(module_id="reflected",Blueprintable)]
class StructBridge {
    [ue_function(BlueprintCallable,
                 arg_value_struct="/Script/DasRuntime.DasCallPayload",
                 return_struct="/Script/DasRuntime.DasCallPayload")]
    def Edit(value:UEStructValue; amount:int):UEStructValue {
        var result = ue_struct_with_int(value,"Count",
                                       ue_struct_get_int(value,"Count") + amount)
        result = ue_struct_with_string(result,"Label",
                                       ue_struct_get_string(value,"Label") + " !")
        return result
    }
}
~~~

Class/method/parameter IDs автоматические. Для каждого UEStructValue input
нужен `arg_<name>_struct`, для результата — `return_struct`. Signature проверяет
точный path и reflected type. Это native loaded `/Script/...` types; Blueprint
user-defined structs пока не включены. Actor Blueprint имеет настоящий typed
struct pin, а не UEStructValue pin.

~~~text
Das.DSL.Apply dsl_reflected_methods.das /Game/DasExamples/BP_Struct reflected.StructBridge
~~~

## Values и helpers

`ue_struct_value(path)` создаёт default-initialized native value. Чтение:
`ue_struct_get_int/float/double/bool/string/name(value,path)`.
Обновление: `ue_struct_with_int/float/double/bool/string/name(value,path,newValue)`.
Path вида `Metrics.Ratio` проходит nested reflected structs.

Values неизменяемые. Каждый with-helper делает native deep copy и обновляет
копию; исходное значение остаётся прежним. Поэтому обычное присваивание token
не создаёт mutable alias. String/FName helpers используют established VM-owned
strings/UENameValue и Unreal property API, а не raw offsets.

Context хранит FInstancedStruct snapshots, а token закрыт от скрипта и другого
context. Equal snapshots deduplicate через UScriptStruct::CompareScriptStruct.
Adapter допускает до 4096 одновременно retained snapshots. Mark/sweep по
VM roots реализован: между calls автоматически, внутри interpreter loops с
options gc. Подробности: Docs/DSLStructLifetime.md.

Frame kind=12, StructValue=FInstancedStruct, ObjectClass содержит struct path
(поле исторически общее с object transport). Input/result копируются через
UScriptStruct operations и принадлежат host независимо от VM. daScript ABI
использует managed ref/copy/CMRES для самого token value.

## Reflection и Blueprint

ValidateDasValueStruct рекурсивно проверяет поддержанные fields, fixed arrays,
depth/leaf/size bounds и native AddStructReferencedObjects hook. Пока допустимы
int32/float/double/bool/FString/FName/enum leaves и nested supported structs.
Object/container/other fields, fixed arrays, native reference hooks и
recursive types отклоняются. Reflection не означает поддержку произвольного
неописанного C++ state: пользоваться следует проверенными value data types.

Generated graphs используют общие CustomStructureParam PackValue/UnpackValue
thunks. Они копируют typed UE struct в FInstancedStruct и обратно с проверкой
точного типа. Existing math values и generated S_* wrappers не заменены;
UEStructValue — отдельный generic интерфейс.

TestDSLReflectedPIE.ps1: typed native struct ProcessEvent, nested/string/int
updates, immutable input, reimport и GC. TestDSLReflectedAsset.ps1: сохранение
wildcard graphs, parameter identity и ProcessEvent в новом процессе. Foundation
tests проверяют capabilities, independent host/VM values, wrong result type и
context release; тот же ABI проверяется в packaged runtime.

Следующие этапы: enums и
другие leaf types, затем struct fields/defaults и migration. Object fields,
containers, out/ref, authored structs и production AOT остаются отдельными
capabilities. Смена имени/порядка/типа существующего параметра требует migration.

Enum leaves implemented: Docs/DSLStructEnums.md.
