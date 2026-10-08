# DSL-authored Blueprint Interfaces

Use ordinary methods with empty declaration bodies and ue_function annotations:

```das
[ue_interface(module_id="sample", BlueprintType)]
class CounterContract {
    [ue_function(BlueprintCallable, Category="Counter", DisplayName="Get Score")]
    def Query(Input:int):int {}

    [ue_function(BlueprintCallable)]
    def Label():string {}

    [ue_function(BlueprintCallable)]
    def Weights(Values:array<double>):array<double> {}

    [ue_function(BlueprintCallable)]
    def Stats(var Counter:int&; @ue_out var Total:int&):void {}
}
```

These are signature declarations, not implementations. Empty bodies work even
for non-void/container results: the macro supplies a compilation-only default
return and a runtime trap. A direct daScript call fails with
`ue_interface declaration stub cannot be called directly`; it never returns a
fictional default result. Unreal implementations are supplied by implementing
Blueprint/DSL classes through the existing interface dispatch bridge.

Only empty source bodies are accepted; statements, returns, local state, fields,
static methods, constructors and finalizers reject. Ordinary declaration methods
require exactly one ue_function annotation and BlueprintCallable. Supported method
options are id, Category, DisplayName, ToolTip and explicit out/ref directions.
Metadata reaches the generated Unreal UFunction and signature graph. Unknown
options and parameter defaults reject. BlueprintPure remains separate capability work. Void/no-output events are now
supported through Event Graph adapters; see DSLInterfaceEvents.md.

For ordinary methods, mutable `var ... &` parameters default to inout;
`@ue_out var ... &` is output-only and `@ue_ref` is an optional explicit inout
marker. This matches implementing class methods. Legacy function-level out/ref
lists remain compatible; contradictory parameter/list directions reject.
Unknown/repeated/conflicting markers, non-reference slots and const refs reject.
Stable type/method/parameter IDs still default to module, class and member names;
optional explicit interface and ue_function IDs remain available.

Legacy `def abstract` declarations with interface-level out="Method.Parameter"
remain readable for compatibility. New examples use annotated empty methods.
Legacy and new forms derive the same native signatures and automatic IDs; changing
source declaration form alone does not require a native signature migration.
Do not assign both class-level and method-level directions to the same slot.

Initial type coverage remains primitive values and the supported standalone
array/map/set shapes. Object/enum/struct/math signatures, interface inheritance
remain later work. Void/no-output events are supported; see DSLInterfaceEvents.md. Interfaces have
1..64 methods with at most 16 parameters. Collection compiles/validates declarations
without executing the source program. No reader or parser changes are used.

Create the interface assets in the Unreal console:

```
Das.DSL.Interfaces.Apply dsl_interface_definition.das /Game/DasInterfaces
```

The command creates real Blueprint Interface assets and marks them dirty; save
the assets through the editor. Identical reapplication reuses the same objects.
Conflicting packages or changed structural contracts reject before modification.
Method/class metadata-only reapplication updates graph/UFunction metadata and the
persistent contract description without replacing the generated class. Save the
dirty asset to persist those changes. A custom
Blueprint/compiler pair preserves UDasAuthoredInterfaceClass during compilation;
StableId and ContractJson are runtime properties that survive cook, rather than
editor-only package metadata. ContractJson describes signatures, not value payloads.

An implementing DSL class uses repeatable annotations:

```das
[ue_class(module_id="sample", Blueprintable),
 ue_implements(interface=CounterContract)]
class CounterActor {
    [ue_function(BlueprintCallable)]
    def Query(Input:int):int { return Input + 7 }
    // Implement every remaining interface method with its exact signature.
}
```

Multiple `ue_implements` annotations are supported. Interface method names must
be distinct across interfaces in this backend; overlaps reject before Blueprint
mutation, even when their signatures match. An unquoted identifier is a
string annotation argument, not a daScript type expression. The bridge resolves
`CounterContract` to stable ID `sample.CounterContract`, using the implementing
class's module_id. Cross-module references and explicit interface IDs use the
complete stable ID, e.g. `ue_implements(interface="contracts.CounterContract")`.
Full native or generated Unreal class paths also work:
`ue_implements(interface="/Script/DasRuntime.DasProbeInterface")` or
`ue_implements(interface="/Game/DasInterfaces/CounterContract.CounterContract_C")`.
The legacy ue_class(implements="...") path list remains supported.

Create the interface assets before compiling the implementation. For assets made
by an older plugin version, reapply the identical definition and save the asset
to populate its searchable StableId tag. Editor lookup
uses persistent searchable StableId tags on UDasAuthoredInterfaceBlueprint, plus
loaded UDasAuthoredInterfaceClass identities. Thus a saved interface can resolve
before loading its Blueprint. No scan of arbitrary source files or implicit
creation of assets is performed. Multiple assets sharing a stable ID reject as
ambiguous; use an explicit Unreal path to select one. Missing, non-interface and
duplicate references reject before Blueprint mutation. Annotation order does not
matter. ue_implements requires ue_class and exactly one string `interface` argument.

Cooked execution resolves authored IDs from loaded generated interface classes;
implementing Blueprint hard references load those classes as cook dependencies.
It does not depend on editor Blueprint assets or scan the editor asset registry.
Calling ReadAuthoringScript for an unrelated source in a cooked process therefore
requires its interface classes to have been loaded, or explicit class paths.

The complete sources are dsl_interface_definition.das and
dsl_interface_implementation.das. Implementations use existing interface graphs
and owned frame transport. /Game authored interface paths also work with checked
membership and typed interface call frames within their existing limitations.

Validation: Scripts/TestDSLTypeRegistry.ps1 -AuthoredInterface -Cooked.
Structural contract/membership changes remain deferred. Next interface stages:
broader authored type references and generated typed wrappers. Interface event
methods are supported; see DSLInterfaceEvents.md.


Type references, tuple direction lists and singleton options use the shared rules
in DSLTypeResolution.md.
