# Typed object/interface and math method signatures

Reference method overloads now preserve native object/interface wrapper and
UEVector/UERotator/UETransform argument/result types, including ref/out:

```das
require ue_object_types

let actor = ue_ref(type<ActorRef>, ue_self_object())
let returned = api.EchoActor(actor)

var current = actor
var previous : ActorRef
api.RefActor(current, previous)

var position = api.EchoVector(ue_vector(1.0lf,2.0lf,3.0lf))
var oldPosition = ue_vector(0.0lf,0.0lf,0.0lf)
api.RefVector(position, oldPosition)
```

Target declarations use ordinary types and annotations:

```das
[ue_function(BlueprintCallable)]
def EchoActor(value:ActorRef):ActorRef { return value }

[ue_function(BlueprintCallable)]
def RefVector(var value:UEVector&; @ue_out var previous:UEVector&):void {
    previous = value
    value = ue_vector(5.0lf,6.0lf,7.0lf)
}
```

ue_ref(type<T>, handle) now accepts existing native object/interface wrappers as
well as authored reference views. It delegates to the existing checked nullable
object/interface conversions. It stores a weak context-local capability rather
than a raw UObject pointer. ue_owner retains its existing authored Actor-view
contract. Imports remain explicit; qualified source type names live only for the
active compile chain, with no retained nested compiler AST pointers.

Object/interface method overloads reuse existing _raw/_from converters and the
general reflected object frame adapter. Nominal wrapper types reject incompatible
arguments during compilation. Runtime class/interface checks, liveness and object
pins remain. Null object values and null interface ref/out values are supported;
an empty wrapper is not a live object. Previously explicit UObjectHandle signatures
remain explicit; no implicit narrowing from ObjectRef to ActorRef is introduced.

Math methods use typed native copied values in the same call frame, without a
temporary opaque UEStructValue at the call site. The adapter checks reflected
struct identity and rejects non-finite values and non-normalized transforms.
Results and ref/out values keep their source types. Output validation completes
before caller variables change; recover preserves a failed math ref argument.
Existing scalar, authored value and container method paths are unchanged.

Examples: dsl_method_value_target.das and dsl_method_value_driver.das include
object/interface echo and ref/out, nullable object echo, Vector/Rotator/Transform
echo and ref/out, and a recoverable math failure. The target implements the native
DasEventInterface with its required methods for the interface value checks.
TestMethodValues.ps1 -Cooked prepares GUID-scoped sources/assets and runs Create,
Reopen, actual PIE and packaged tests.

This stage maps existing generated native object/interface wrapper types. Authored ue_object_ref signature support now uses host storage plus authored_id;
see DSLAuthoredReferenceSignatures.md. Concrete Blueprint-class pins remain
a separate resolution/bootstrap step. Automatic reference/import discovery and
general migrations remain separate work.

Validation: [DSLMethodObjectMathValidation](DSLMethodObjectMathValidation.md).
