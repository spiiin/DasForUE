# Scalar signatures и bool properties в DSL

Actor adapter теперь поддерживает следующие сигнатуры:

| daScript | Unreal input / result | Runtime API |
|---|---|---|
| `(input:int):int` | int32 / int32 | RunNamedPersistent, result.Value |
| `(input:float):float` | float / float | RunNamedFloatPersistent, result.FloatValue |
| `(input:bool):bool` | bool / bool | RunNamedBoolPersistent, result.BoolValue |
| `():void` | без параметров и результата | RunNamedVoidPersistent |

Все это instance BlueprintCallable methods, с прежними stable IDs и metadata.
Имя отражённого входного pin пока фиксировано `Input`; имя аргумента в `.das`
может отличаться. Тип возвращаемого значения нужен явно, включая `:void`:
structure annotation срабатывает до полного type inference.

~~~das
options gen2
require ue_dsl
[ue_class(module_id="example",id="example.typed",Blueprintable)]
class TypedCounter {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Enabled : bool = true
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Gain : float = 1.5

    [ue_function(BlueprintCallable)]
    def Scale(amount:float):float {
        self.Gain += amount
        return self.Gain
    }
    [ue_function(BlueprintCallable)]
    def Toggle(value:bool):bool {
        self.Enabled = value
        return self.Enabled
    }
    [ue_function(BlueprintCallable)]
    def Reset():void {
        self.Enabled = false
    }
}
~~~

Полный исполняемый пример: dsl_scalar_methods.das. Команда импорта:

~~~text
Das.DSL.Apply dsl_scalar_methods.das /Game/DasExamples/BP_Scalar typed.fixture
~~~

Bool properties проходят весь путь: native AST literal → authoring JSON bool →
actor schema → Blueprint bool pin/default → FBoolProperty getter/setter.
Чтение/запись self использует ue_get_self_bool/ue_set_self_bool и штатный API
FBoolProperty, без предположений о bitfield layout. Readonly/hidden checks такие
же, как для int/float. Native DSL flags и metadata остаются прежними.

Генератор выбирает отдельный typed native adapter, соединяет правильные Input/
ReturnValue pins; void graph имеет только exec flow. Compile-only preflight
проверяет exported entry point, точные by-value типы и arity до mutation.
Изменение сигнатуры уже созданного метода требует migration и отклоняется.

Все сигнатуры используют общий owner/file persistent context. Runtime хранит
тип каждой ранее использованной entry point: reload, нарушающий контракт другого
метода, сохраняет последнюю рабочую версию. Source tracker, declaration gate,
body-only reimport и освобождение contexts остаются общими.

## Проверка и границы

TestDSLScalarsPIE.ps1 проверяет реальный ProcessEvent для float/bool/void/int,
bool default=true в CDO/Actor, bool write/reset, world isolation, отказ от signature
и bool-default migration без mutation, idempotent reimport и GC.
Foundation tests проверяют typed VM calls, общий globals counter между float/bool,
wrong-call signature, typed cross-entry reload fallback, void action и bool model.

Допустимые поля сейчас int/float/bool. Неподдержанные string/name/double/object/
struct/container поля и out/ref сигнатуры продолжают отклоняться. Mixed/multiple
scalar signatures реализованы; см. Docs/DSLMixedMethods.md.
Pure/static/RPC и parameter defaults/annotations ещё не реализованы. Наличие
таких типов в binding export/маршаллинге UFunction не означает authoring support.
В legacy временном FromDeclaration wire по-прежнему только int/float; новые
типы доступны через authoring model и native annotations.

Следующий этап — общий call-frame adapter для нескольких и смешанных параметров,
затем расширение типов с явным ownership и runtime/AOT capability checks.

Mixed/multiple и zero-input scalar signatures теперь реализованы: Docs/DSLMixedMethods.md.
