# Typed native object/interface fields

Native object and interface wrappers retain their source types in authored
reference property access. Declare fields with the imported wrapper types:

```das
@ue_property=(EditAnywhere, BlueprintReadWrite)
NativeActor:ActorRef
@ue_property=(EditAnywhere, BlueprintReadWrite)
NativeEvents:DasEventInterface
@ue_property=(EditAnywhere, BlueprintReadOnly)
LockedNativeActor:ActorRef
```

Omit initializers: reflected object/interface defaults are null. In an ordinary
function or DSL method with a ValueTargetRef named api:

```das
let actor = api.NativeActor                 // ActorRef
api.NativeActor = anotherActor
api.NativeEvents = DasEventInterface_from(api.handle)
api.NativeActor = default<ActorRef>
api.NativeEvents = default<DasEventInterface>
```

Import ue_object_types for these sample native wrappers and the matching authored
reference module for api. Other generated native wrappers use the same generic
source descriptor/type resolver and converter path; no per-field backend is added.

Getters and setters verify the actual reflected field's native class/interface
path before transferring the value. This also checks assignments of null, for
which value inspection alone cannot reveal a changed property declaration.
Setters retain runtime class/interface/liveness checks; readonly fields expose
no setter. Nominal ObjectRef does not implicitly become ActorRef or an interface
wrapper even if its live object happens to satisfy the native type.

Wrappers remain weak checked handles. The ordinary Unreal property's ownership
semantics are unchanged. A field referring to a destroyed Actor reads as an
empty wrapper; explicitly clearing it is allowed. Null validity checks return
false. Component field access keeps its existing authored-identity validation;
component fields expose the generated Blueprint component class, so the generic
native field path check is not substituted for that identity check.

This stage covers native wrappers. Ordinary authored class-reference properties,
soft/class references, object containers and automatic import discovery remain
separate. See dsl_method_value_target.das and dsl_method_value_driver.das.

Validation: [DSLNativeReferenceFieldsValidation.md](DSLNativeReferenceFieldsValidation.md).
