# Mixed scalar methods: общий кадр вызова

DSL поддерживает 0–16 by-value входных параметров int/float/bool в любом
порядке; результат — int, float, bool или void. Доступны несколько аргументов,
один аргумент с другим типом результата, scalar result без аргументов и
void action с аргументами. Return type задаётся явно.

~~~das
options gen2
require ue_dsl
[ue_class(module_id="mixed",Blueprintable)]
class MixedCounter {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    health : int = 10
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    speed : float = 1.5

    [ue_function(BlueprintCallable)]
    def Compute(count:int; factor:float; apply:bool):float {
        if (apply) { self.health += count; self.speed += factor }
        return float(self.health) * self.speed
    }
    [ue_function(BlueprintCallable)]
    def ReadScore():int { return self.health }
    [ue_function(BlueprintCallable)]
    def ResetWith(amount:int; resetGain:bool):void {
        self.health = amount
        if (resetGain) { self.speed = 1.5 }
    }
}
~~~

Явные IDs не нужны. Здесь автоматически получаются class ID `mixed.MixedCounter`,
method ID `mixed.MixedCounter.Compute` и parameter ID
`mixed.MixedCounter.Compute.count`. Module ID тоже можно опустить: тогда используется
basename исходника. Явные IDs остаются необязательным средством будущей миграции.

Полный пример: dsl_mixed_methods.das. Импорт:

~~~text
Das.DSL.Apply dsl_mixed_methods.das /Game/DasExamples/BP_Mixed mixed.MixedCounter
~~~

Новые frame-сигнатуры сохраняют имена и порядок параметров из `.das` в reflection
и Blueprint pins. Для совместимости прежние single same-type scalar adapters
продолжают использовать pin `Input` и parameter ID `.input`.

## Реализация

`FDasCallContract` содержит ordered input kinds и result kind. AST preflight
проверяет exported function, число и типы параметров, return type и отсутствие
ref. Clone methods получают обычные exported entry points; self bridges прежние.

Generated Blueprint graph имеет стандартные typed user pins, упаковывает scalar
значения в array `FDasScalarValue` и вызывает один `ExecuteDasFrame`. Scalar
result извлекается через BreakStruct. Для void остаётся exec flow. Структуры
и коды типов — внутренний транспорт; автор DSL описывает обычные typed methods.
Отдельный C++ adapter для каждой комбинации параметров не создаётся.

Runtime API: `RunNamedFramePersistent(Path,Entry,Arguments,ReturnKind,Self)`.
Arguments — ordered tagged scalar values: Kind 0=int, 1=float, 2=bool;
ReturnKind использует EDasMethodSignature Int/Float/Bool/Void. Runtime проверяет
tags/arity и AST contract до eval, преобразует значения в temporary vec4f slots
и вызывает ту же owner/file session. Результат — Value/FloatValue/BoolValue,
успех/ошибка/ReloadError, как у прежних API. Нативные pointers в frame отсутствуют.

Reload проверяет ordered contracts всех ранее вызванных entry points. Ошибочный
source сохраняет последнюю рабочую версию и globals прежнего context. Неверный
первый call contract не блокирует последующий корректный вызов того же файла.
Unchanged broken request не приводит к повторной компиляции на каждом вызове.

Editor preflight проверяет существующую signature, names и order; перестановка,
переименование, смена типа или stable parameter ID требуют migration и
отклоняются до mutation. Все parameter IDs сохраняются в package metadata;
reflection не содержит сохранённых VM/function pointers.

## Проверка и ограничения

TestDSLFramePIE.ps1 проверяет ProcessEvent с int/float/bool inputs, float/bool/int
results, zero-input result, mixed void action, world isolation, reimport без
reinstance, отказ от parameter reorder/ID change, GC и EndPIE.
TestDSLFrameAsset.ps1 сохраняет графы и проверяет mixed ProcessEvent/parameter
identity/reimport в новом процессе. Foundation tests дополнительно проверяют
bad arity/order/tags/limit, cold recovery и cross-entry mixed reload contracts.

Legacy int/float/bool/void adapters и lifecycle продолжают работать. Fields
по-прежнему int/float/bool. String/name/double/object/struct/container signatures,
out/ref, parameter defaults/metadata, pure/static/RPC и arbitrary VM-state
migration ещё не реализованы. Наличие generic marshalling для существующих
UFunctions не означает authoring support для этих типов.

Следующий этап — string/name/double, затем object/struct arguments и results
с явным ownership, reflection и runtime/AOT capability checks.

Расширение frame types и fields до string/double/name: Docs/DSLTextTypes.md.
