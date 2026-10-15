# Runtime class references

```das
let klass = ue_class_loaded(type<DynamicLifeRef>, classPath)
let component = ue_component_create(type<DynamicLifeRef>, actor, "DynamicLife", klass)
let same = ue_component_get_or_create(type<DynamicLifeRef>, actor, "DynamicLife", klass)
```

`ue_class_loaded` resolves an already-loaded UClass and returns an opaque
`UEClassHandle`. The type argument constrains resolution: native references check
IsChildOf; authored references also check the generated class CDO's AuthoredTypeId.
Missing, deprecated and superseded classes are rejected. Abstract classes can be held as references; construction rejects them.
`ue_class_valid(handle)` checks validity; `ue_class_path(handle)` returns its path.
The handle supports local values, copying and containers. It is not a compile-time
TSubclassOf<T>: component calls re-check their own requested type and authored ID.

Handles use the existing interpreter-context object token registry. They are weak,
scoped to the owner session, invalid after session release, and do not load or pin
assets permanently. Class-based creation holds the UClass during the native call.
Native component calls accept an optional fourth class handle and permit compatible
Blueprint subclasses. Authored create/get_or_create accept either the existing
path string or a class handle. Existing three-argument native calls are unchanged.

The runtime stage below is extended by reflected FClassProperty and class parameters in generated native UFunction wrappers (DSLClassMarshalling.md). Soft loading remains separate. Standalone runtime values
must not be treated as serialized class references. Those features require the
reflected field form described below.

Tests: DynamicPIE exercises authored class resolution/creation, bad base, bad
identity, missing class, mismatched create type, empty handle and lifecycle. Native
component integration exercises SceneComponent creation through a class handle.

## Reflected class properties

```das
@ue_property=(EditAnywhere,BlueprintReadWrite)
@ue_class_base="/Script/DasRuntime.DasScriptComponent"
ComponentClass:UEClassHandle

// inside a ue_function
let component = ue_component_create(type<DynamicLifeRef>,actor,"DynamicLife",self.ComponentClass)
```

The field is a real FClassProperty with MetaClass equal to the loaded native base
specified by ue_class_base. Blueprint/Details uses the standard class picker for
that base. The IR uses kind="class" and type_path as the base constraint; the value
is a UClass reference serialized by Unreal, not an interpreter token or path string.
The VM creates a context-local handle when the property is read.

Initializers are omitted (null). A class can be selected on the Blueprint CDO or an
instance. Unchanged reimport preserves editor-selected CDO values. Runtime assignment
supports null and compatible class values, checks before writing and obeys
BlueprintReadOnly. Assignment and creation reject incompatible or obsolete classes.
The Details picker filters by native inheritance; exact daScript identity is checked
by the typed create operation. Callable ue_function parameters/results and ref/out
are documented in [DSLClassMethods.md](DSLClassMethods.md). Class containers,
authored structs and retired-field/default migrations remain deferred.

A hard class reference participates in Unreal serialization and dependency traversal.
TestDynamicComponentsPIE.ps1 saves test-owned component and owner Blueprint assets,
starts a fresh editor, reloads the selected CDO class, and repeats creation/lifecycle
in PIE. Packaged-build/cooker coverage passed; see the validation note below.


Validation: DynamicPIE and ClassReload each report one success, zero failures/warnings. The saved owner package dependency list contains the selected component's package. Generated test assets are deleted after both processes exit.

Packaged validation: these features passed TestCookedCalls.ps1 in Win64 Development NullRHI on 2026-10-13. See PackagedCallsValidation.md for coverage and remaining limits.
