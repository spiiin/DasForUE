# Standalone authored struct arrays

ue_class and ue_interface functions can use ordinary `array<ue_struct>`
parameters, results and mutable references. Elements retain explicit registry/
folder resolution and source ID/GUID/layout/default checks.

```das
[ue_interface(module_id="example", type_registry="REGISTRY", BlueprintType)]
class DataContract {
    [ue_function(BlueprintCallable)]
    def EchoDetails(Values:array<InterfaceDetails>):array<InterfaceDetails> {}
    [ue_function(BlueprintCallable)]
    def NewDetails():array<InterfaceDetails> {}
    [ue_function(BlueprintCallable)]
    def CopyDetails(Values:array<InterfaceDetails>;
                    @ue_out var Copies:array<InterfaceDetails>&):void {}
    [ue_function(BlueprintCallable)]
    def BumpDetails(var Values:array<InterfaceDetails>&):void {}
}
```

Implementations use normal daScript array operations, for example
`return <- clone(Values)`, `Copies <- clone(Values)` or
`Values[0].Amount += 3`. The interface stubs remain empty annotated methods.
Blueprint pins are real arrays of the authored Unreal struct.

The internal transport is a reflection-built UScriptStruct containing a Values
array with the exact loaded authored element type. It has no opaque native state
and uses the existing bounded snapshot and Unreal GC reference collector.
Its internal path encodes the element path reversibly, so return-only calls and
out defaults can restore the adapter in a fresh process without prior input.
No extra public asset or DSL spelling is required.

Adapters are transient and bounded to 1,024 cached types per runtime. They root
their reflected type references until runtime shutdown, when their roots are
released. Cached element identity, size/alignment and recursive field layout
must stay unchanged; drift rejects rather than reusing an obsolete stride/layout.
Long internal paths beyond Unreal's existing name/path bound reject. General
structural migrations remain deferred.

Natural-source interface facades convert arrays element by element into fresh
native/source arrays. Nested maps/sets/arrays, Name and typed UObject fields use
the existing checked element converters. Result conversion completes before
copyback of mutable array arguments. Storage is independent; UObject identities
remain shared through their checked handles.

```das
var values : array<InterfaceDetails>
var detail = InterfaceDetails()
detail.Amount = 31
push_clone(values, detail)
var copied <- api.EchoDetails(values)
copied[0].Amount = 999
// values[0].Amount remains 31.
```

Source struct arrays use the existing 4,096-total-container-element snapshot
bound, including containers nested in elements. Existing generic low-level call
frame limits remain separate. Empty arrays preserve the same reflected element
identity. A mismatched carrier/element identity is rejected before invocation.

This stage supports authored ue_struct elements, not arbitrary opaque native
structs, standalone authored enum/object arrays or direct arrays-of-arrays. Native
generic binding export capabilities are unchanged. Class field syntax, reflected
interface storage and broader scalar object/enum/struct/math out/ref remain
separate work.

See DSLAuthoredStructArraysValidation.md and the existing ue_interface_values,
dsl_interface_registry_definition and dsl_interface_registry_implementation
fixtures for complete executable examples.
