# Объектные параметры и результаты DSL

Общий frame теперь принимает UObject references и возвращает объекты.
В daScript используется существующий nominal `UObjectHandle`, в Unreal —
object pins и FObjectProperty. Объектные поля также поддерживаются;
актуальное описание — Docs/DSLObjectFields.md.

~~~das
options gen2
require ue_dsl
[ue_class(module_id="objects",Blueprintable)]
class ObjectBridge {
    [ue_function(BlueprintCallable)]
    def Echo(value:UObjectHandle):UObjectHandle {
        return value
    }
    [ue_function(BlueprintCallable,
                 arg_actor_class="/Script/Engine.Actor",
                 return_class="/Script/Engine.Actor")]
    def ActorIdentity(actor:UObjectHandle):UObjectHandle {
        return actor
    }
}
~~~

IDs необязательны; здесь class ID получается `objects.ObjectBridge`.
Без class annotations constraint — `/Script/CoreUObject.Object`.
`arg_<argument-name>_class` задаёт native loaded class для object input;
`return_class` — для object result. Неиспользованные class annotations,
ненативные/unloaded paths и ref/out/default arguments отклоняются.
Указание класса на non-object parameter/result также является ошибкой.

Полный probe example: dsl_object_methods.das; он содержит дополнительные методы
проверки слабых globals и GC. Метод ReadAcrossGC — fixture-only: его owner должен
иметь ForceTestGC UFunction, он не предназначен для обычного Actor.

~~~text
Das.DSL.Apply dsl_object_methods.das /Game/DasExamples/BP_Objects objects.ObjectBridge
~~~

## Ownership и поведение

- Frames переносят ObjectValue и ObjectClass; Kind=8. ObjectClass должен совпадать
  с declared constraint, пустое значение означает UObject. Результату также
  передаётся ReturnClass (пустое — UObject). Contract включает ordered classes
  и проверяется вместе с exported AST signature при preflight/reload.
- VM получает context-local opaque handle, содержащий token, без native pointer.
  Таблица holds weak UObject/Class references. VM globals и host execution
  result (TWeakObjectPtr) не удерживают объекты от GC.
- На время eval каждый non-null object input удерживается TStrongObjectPtr;
  после возврата pin снимается. Это защищает вход, даже если body вызывает GC.
- Null input создаёт zero handle. Null result успешен. Nonzero expired/foreign
  handle в результате вызывает ошибку. Destroyed Actors не считаются valid,
  даже если их память ещё не собрана GC.
- Runtime проверяет actual input/result class. Неверный input отклоняется до
  body execution; неверный result не передаётся вызывающему коду.
- Editor-only Unreal replacement delegate перенаправляет weak handles на новые объекты
  и expected classes. Несовместимая замена инвалидирует ссылку. Проверено,
  что unaffected VM observer следует за target Actor при Blueprint reinstance.
  Контексты заменённого owner по-прежнему освобождаются штатным lifecycle;
  универсальной миграции его VM globals пока нет.
- Возвращаемый Unreal object pin участвует в обычных UE GC rules. Если Blueprint
  сохраняет его в собственном reflected property, это уже UE-owned reference,
  в отличие от слабого handle внутри daScript.

## Реализация и проверки

UObjectHandle — ref-type в ABI закреплённого daScript. Argument slots передают
адреса временных handle values; object result используется через CMRES storage.
Эти адреса существуют только на протяжении eval и не сохраняются в reflection.
Class-constrained Blueprint result использует проверенный UObject result и pure
DynamicCast к declared class. Параметры и metadata сохраняются в asset.

TestDSLObjectFramePIE.ps1: typed Actor ProcessEvent/null, observer globals,
structural reimport, weak-handle replacement, Destroy/GC/expired return и EndPIE.
TestDSLObjectAsset.ps1: Save/Reopen и typed Actor ProcessEvent в новом процессе.
Foundation: CMRES roundtrip, null, weak globals/results, input GC pin/release,
class mismatches и expired result; те же проверки входят в packaged runtime.

Ограничения: только loaded native class constraints, 0–16 input parameters,
scalar/object/void result; нет object authoring fields, interfaces/TSubclassOf,
containers, out/ref, constructors/components или production AOT authoring.
Следующий этап — reflected value structs, начиная с FVector/FRotator/FTransform;
object fields требуют отдельной сериализации, GC и reinstance policy.
