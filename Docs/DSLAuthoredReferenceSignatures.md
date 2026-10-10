# Authored references in reflected method signatures

Author-defined Actor/Component views may now be parameters, results and ref/out
arguments of BlueprintCallable methods:

```das
[ue_function(BlueprintCallable)]
def EchoAuthored(value:ValueTargetRef):ValueTargetRef { return value }

[ue_function(BlueprintCallable)]
def RefAuthored(var value:ValueTargetRef&;
                @ue_out var previous:ValueTargetRef&):void {
    previous = value
    value = default<ValueTargetRef>
}
```

The caller uses the ordinary typed method syntax:

```das
let returned = api.EchoAuthored(api)
var current = returned
var previous : ValueTargetRef
api.RefAuthored(current, previous)
let empty = api.EchoAuthored(default<ValueTargetRef>)
```

Declare/import the view with ue_object_ref(script=...,type=...) as before. A shared
view module lets both the class and caller use the same nominal type. Imports are
explicit. Class identity/host is published before signature collection, permitting
self-referential method declarations before the Blueprint exists.

Reflected storage currently uses the native DasProbeActor or DasScriptComponent
host class. The type IR adds optional authored_id alongside type_path, keeping the
logical author class identity separate from physical storage. It survives JSON
roundtrip and participates in type equality and declaration change detection.
Existing IR without authored_id is unchanged. Concrete Blueprint-class pins and
class registries/bootstrap are subsequent work; no global guess by short name or
selection of an arbitrary matching Blueprint is performed.

Source converters validate authored stable ID and weak-handle liveness when packing
and unpacking, including direct Blueprint-to-DAS entry and returned/out values.
Null signature values are allowed. ue_ref/ue_owner retain their strict authored
factory checks, while default<View> represents null. Merely putting another live
Actor's handle in the same view struct does not bypass identity validation.
Wrong nominal arguments reject during compilation; same-host wrong authored
instances reject at runtime. Failed publication preserves caller ref/out variables.
The reference method emitter and native object call frame remain the common path.

Tests/examples: ue_method_target_refs.das is a GUID test view-module template;
dsl_method_value_target.das declares self-view echo/ref/out and
dsl_method_value_driver.das exercises null and a forged view holding the wrong
author class. TestMethodValues.ps1 -Cooked prepares the actual reference module,
checks IR roundtrip and runs Create/Reopen/PIE/packaged tests. The Actor/Component
probe also passes ScoreComponent through a typed Component method and reads Bonus
from the returned view.

Authored-reference UPROPERTY ownership/struct fields, concrete Blueprint pin types,
automatic import/reference discovery and general migration remain separate work.

Validation: [DSLAuthoredReferenceSignaturesValidation](DSLAuthoredReferenceSignaturesValidation.md).
