# Typed reflected interface references

Existing generated native/authored interface wrappers can be used as types in
ue_class/ue_struct fields and function input/result declarations. No extra
function type-path annotation is needed:

```das
require ue_dsl
require ue_generated

[ue_class(module_id="example", Blueprintable)]
class EventHolder {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Target : DasEventInterface
    [ue_function(BlueprintCallable)]
    def Store(value:DasEventInterface):void { self.Target = value }
    [ue_function(BlueprintCallable)]
    def Read():DasEventInterface { return self.Target }
    [ue_function(BlueprintCallable)]
    def Echo(value:DasEventInterface):DasEventInterface { return value }
}
```

Fields use implicit null defaults. Authored references use the equivalent
generated `ue_interface_ref` wrapper, such as CounterContract from its bindings
module. The wrapper checks the authored interface stable ID and the loaded class.
Native interfaces use `ue_native_interface`. UEObject remains restricted to native
object classes; it is not required for interface types.

Unreal fields and function pins are real FInterfaceProperty/TScriptInterface
values, including interface fields nested in authored structs. The VM uses its
existing context-owned UObject capability, checked against the requested
interface at conversion and assignment. Internal authoring/frame records retain
Object kind plus the exact interface class path, so existing enum numbering and
frame schemas remain compatible.

Reference checks use GetClass()->ImplementsInterface. The object part is authoritative
for Blueprint-only implementations: their native interface address can be null.
Native implementations retain their native pointer. Null values are valid; live
nonimplementers reject before replacing a field or invoking the method.

Blueprint wildcard pack/unpack adapters retain the exact interface identity across
the object-valued runtime frame. Generic generated native call marshalling also
reads/writes FInterfaceProperty and pins referenced inputs/outputs. Source value
converters reuse the existing checked interface factories for struct fields.

A reflected interface property owns a strong UObject reference and participates in
Unreal GC. A plain VM wrapper is weak. Clearing the reflected property permits GC
when no other strong reference remains. Destroyed actors read as null. Snapshot
references follow the existing GC ownership for FInstancedStruct values, including
nested structs and struct-array elements.

Native/authored interface input/results and fields are supported. Interface out/ref,
standalone interface arrays/hash values, editor picker refinements and VM reentry
remain separate stages. Existing runtime protection against dispatching back into
an active daScript VM is unchanged. Structural migrations remain deferred.

See DSLInterfaceReferencesValidation.md for results. Native field/method examples
are in dsl_interface_registry_implementation.das; ue_interface_values.das also has
a typed Events field. The authored interface-holder probe is generated from the
actual exported binding module by GenerateAuthoredInterfaceProbe.py.
