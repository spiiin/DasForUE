# Native value signatures in authored interfaces

ue_interface declarations now accept the same native object and math value
identities as ue_class methods, plus constrained native enum/snapshot values.

```das
require ue_dsl
require ue_object_types

[ue_interface(module_id="contracts", BlueprintType)]
class ValueContract {
    [ue_function(BlueprintCallable)]
    def EchoActor(value:ActorRef):ActorRef {}
    [ue_function(BlueprintCallable)]
    def EchoVector(value:UEVector):UEVector {}
    [ue_function(BlueprintCallable, arg_value_enum=EDasCallMode,
                 return_enum=EDasCallMode)]
    def EchoMode(value:UEEnumValue):UEEnumValue {}
    [ue_function(BlueprintCallable, arg_value_struct=DasCallPayload,
                 return_struct=DasCallPayload)]
    def EchoPayload(value:UEStructValue):UEStructValue {}
}
```

ActorRef/ObjectRef infer native UObject identity from the imported generated
wrapper metadata. Raw UObjectHandle defaults to CoreUObject.Object and can use
arg_name_class/return_class constraints. UEVector/UERotator/UETransform infer
their exact CoreUObject struct identities. Redundant matching constraints are
accepted; mismatches reject. Raw UEEnumValue and UEStructValue need explicit
arg_name_enum/struct and return_enum/struct constraints, using existing native
short names, module-qualified names or full paths. Unknown/wrong-kind references,
unused constraint names and repeated singleton specifiers reject.

The implementing class declares matching signatures with actual bodies. Existing
weak UObject capabilities and owned struct snapshots remain the transport. Native
Blueprint interface graphs receive object/struct/enum pins with the actual loaded
type identity. Both native frame entry and Blueprint ProcessEvent reach the same
checked daScript implementation. Null object references are accepted; nonnull
references retain the ordinary class/liveness guards.

Native enum pins may use FEnumProperty; authored byte enums use FByteProperty.
The shared enum identity/read/write helpers and Blueprint pack/unpack thunks now
handle both representations. Type identity, existing value range (0..254), hidden/
reserved member rules and context ownership remain checked. This does not add
arbitrary integer-enum transport or silently reinterpret one enum as another.

Parameters of these newly added object/math/enum/struct kinds remain input-only,
with optional return values. Mutable references/output markers explicitly reject
because ue_class implementations still support out/ref only for primitives and
standalone supported containers. Interface-reference values themselves remain
outside reflected signatures. Natural authored enum/struct types, registry/folder
scope in interface declarations and generated authored BPI wrappers are subsequent
integrations; this native-value stage does not claim them.

Ordinary empty declaration stubs still trap direct daScript invocation. Metadata
reapply, stable method IDs, save/reopen and cook behavior are preserved. Structural
changes to an existing interface contract and VM reentry remain deferred.

Examples: dsl_interface_definition.das and dsl_interface_implementation.das now
include EchoActor/EchoObject, three math echoes, EchoMode and EchoPayload alongside
the earlier scalar/container/event examples. These are fixture method names, not
special Unreal or DSL keywords.
