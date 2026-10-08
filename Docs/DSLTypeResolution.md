# Shared DSL identity and annotation rules

All accepted source type references are resolved and kind-checked before the
authoring IR is published. The IR retains canonical Unreal object paths, not
unresolved names. This adds source spelling conveniences, not new inheritance,
container or migration capabilities.

## Type reference spellings

| Reference | Resolution |
| --- | --- |
| `/Script/Engine.Actor` or `/Game/Folder/Settings.Settings` | Exact object path, validated against the required kind |
| `Actor`, `DasCallPayload`, `EDasCallMode` | Native object of the expected kind, or imported native class alias |
| `Engine.Actor`, `DasRuntime.DasCallPayload` | Loaded native `/Script/Module.Type` when no authored scope is required |
| `GameSettings`, `PlayState` | Source declaration identity, including its optional explicit stable ID |
| `direct.Settings`, `direct.State` | Complete stable ID resolved through the explicitly selected enum/struct registry |
| `CounterContract` | Source interface identity or module_id.CounterContract |

An unquoted annotation identifier is an ordinary string argument. The bridge,
not the annotation parser, performs existence/kind/ambiguity checks. Source
declarations in the current module and imported modules are both considered.
No speculative first-match resolution is used; ambiguous identities reject.

```das
[ue_class(parent=Actor, module_id="game", Blueprintable),
 ue_implements(interface=CounterContract)]
class CounterActor { /* reflected fields and methods */ }
```

Parent accepts native class names/aliases; the current adapter still requires
direct native Actor inheritance. Authored class inheritance is not added.
Interfaces accept exact paths, native names and authored stable IDs. With an
explicit type_folder, authored short names are restricted to that folder. With
no folder, existing authored interface identity lookup remains supported via
saved searchable tags in editor and loaded generated classes in cooked runtime.

## Enum/struct registries remain explicit

`type_registry="/Game/.../TypesRegistry.TypesRegistry"` remains an authoritative
catalog for named authored enum/struct references. Source declarations supply
their stable ID, so renaming GameSettings while preserving its ID still resolves
the original asset. A qualified ID is exact. For a short name without a source
declaration, a matching module-local ID has precedence; otherwise a unique
registered ID leaf/asset name may match. Missing, ambiguous or wrong-kind entries
reject without falling back to unrelated loaded/global assets.

An explicit type_folder remains the alternative exact folder/name convention;
it does not infer arbitrary stable-ID bindings. type_registry and type_folder
remain mutually exclusive. Registry hard references and struct GUID checks are
unchanged. The current registry holds enums/structs, not native classes/interfaces.

Fully specified object paths are explicit references independent of named lookup
and are still validated by kind/capability. A path string alone is not a cook
dependency; keep the existing registry/property/signature hard references.

```das
[ue_class(parent=Actor, module_id="game", Blueprintable,
          type_registry="/Game/Types/TypesRegistry.TypesRegistry")]
class Example {
    [ue_function(BlueprintCallable,
                 arg_value_struct=GameSettings,
                 return_struct="direct.Settings")]
    def Echo(value:UEStructValue):UEStructValue { return value }
}
```

Natural direct enum/struct/object/math signatures already infer native identity.
A redundant explicit arg_name_class/enum/struct or return_class/enum/struct is
accepted only if it resolves to the same path. A conflicting annotation rejects;
it is never silently ignored. Constraints on unrelated parameter/result kinds
remain unused-option errors. Type kind is enforced at resolution.

## Parameter directions (preferred) and legacy lists

Class and ordinary interface methods share one direction resolver:

```das
[ue_function(BlueprintCallable)]
def Adjust(var counter:int&; var weights:array<double>&;
           @ue_out var total:int&; @ue_out var message:string&):void {
    counter += 1
    total = counter
    message = "Updated"
}
```

Mutable references default to inout; @ue_out denotes output-only. Optional @ue_ref
makes inout explicit. Use the same parameter declarations with an empty body in
an interface definition. Current primitive/container capability limits still apply.

Native tuple lists and legacy CSV strings remain accepted on ue_function. They
produce the same IDs/ABI directions as parameter metadata. Matching redundant
markers/lists are permitted; disagreement, duplicates, overlap, unknown names,
false/non-boolean markers, direction/reference mismatches and const refs reject.
Unmarked value parameters remain input-only. See DSLOutRef.md.

## Singleton and conflict validation

Repeated ue_function annotations on one method explicitly reject. ue_implements
remains intentionally repeatable. Class/type annotation lookups reject repeated
singleton annotations and contradictory declaration kinds. Scalar annotation
options (id, class/type constraints, BlueprintCallable, metadata, etc.) cannot be
repeated, even with the same value. out/ref entries are repeatable only as lists;
their parameter names must remain unique. Per-field ue_property flag lists retain
their existing metadata syntax and supported flag rules.

Examples: dsl_type_syntax.das, dsl_type_syntax_registered.das, dsl_out_ref.das and
dsl_interface_definition.das. Validation includes TestEditor.ps1 and
TestDSLTypeRegistry.ps1 -TypeSyntax/-OutRef/-AuthoredInterface.
