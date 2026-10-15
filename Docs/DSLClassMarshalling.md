# Class parameters in reflected calls

`FClassProperty` parameters (UClass* and TSubclassOf<T>) now use UEClassHandle in
generated wrappers. The export IR kind is class; type_path is the property's
MetaClass constraint. This identity is also included in the checked call signature.
Runtime validates IsChildOf(MetaClass) before assignment and validates outputs;
obsolete classes are rejected. Null class values round-trip via ue_class_null().

```das
require ue_library_generated
require ue_object_types
let klass = ue_class_loaded(type<ActorRef>, "/Script/Engine.Actor")
let actor = ue_lib_GetActorOfClass(klass)
```

GetActorOfClass is exported from GameplayStatics in FunctionLibraries.json; the
WorldContext argument is hidden and injected from the script owner as before.
The shared backend supports class inputs, return/out/inout values, array elements
and class leaves in supported reflected structs. Class map/set capabilities remain
outside this stage. The same parameter APIs are usable in native instance frames;
this does not add class parameters to user-authored ue_function/Blueprint adapters.

Low-level API: ue_arg_class(call,path,handle), ue_result_class(call,path),
ue_class_null(). Frames pin class arguments and outputs while live. Returned
handles use the existing weak interpreter-session capability registry.

General class handles now allow abstract classes as values. Construction adapters
continue to reject abstract classes; having a valid class reference is distinct
from being able to instantiate it. Native-interface class values can also be held
where the reflected MetaClass allows them. Exact authored IDs are enforced by
creation adapters; reflected TSubclassOf constrains Unreal inheritance.

TestFunctionLibraries.ps1 exports and generates GetActorOfClass and user C++
fixtures for UClass, TSubclassOf<Actor>, class outputs and arrays. Integration tests
cover null values, incompatible MetaClass rejection, abstract class values and
WorldContext-enabled engine calls. Existing unsupported-export regression now
uses an int64 fixture, since the old class-parameter example is supported.

Validation 2026-10-12: TestFunctionLibraries.ps1 PASS (class static+instance calls, out/arrays/null/abstract and negative MetaClass checks), 93 generator tests PASS, TestBindingsExport.ps1 negative export PASS, editor foundation smoke PASS failures=0.

Packaged validation: these features passed TestCookedCalls.ps1 in Win64 Development NullRHI on 2026-10-13. See PackagedCallsValidation.md for coverage and remaining limits.

Authored ue_function class inputs/results and ref/out use the same checked class
marshaller through generated method accessors. See [DSLClassMethods.md](DSLClassMethods.md).
