# Typed UObject properties in DSL classes

```text
[ue_class(module_id="example", Blueprintable)]
class Tracker {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Target : UObjectHandle = ue_object_value("/Script/Engine.Actor")

    [ue_function(BlueprintCallable)]
    def SetTarget(value:UObjectHandle):void { self.Target = value }

    [ue_function(BlueprintCallable, return_class="/Script/Engine.Actor")]
    def GetTarget():UObjectHandle { return self.Target }
}
```

The initializer selects the reflected native class and produces null. Its one
argument must be a literal path to an already loaded `/Script/` UClass. Actor,
ActorComponent, SceneComponent and general UObject bases work; instances of
compatible Blueprint subclasses are allowed. Script-defined property classes,
asset defaults, soft/weak/class/interface properties and instanced component
construction require additional capabilities.

The Blueprint field is a typed FObjectProperty/UPROPERTY. It is a strong Unreal
reference: a referenced UObject stays alive while the owner is reachable. The
daScript UObjectHandle remains weak and bound to its VM context. Reading a field
creates a typed handle; no FProperty address is cached across replacements.
Assigning null clears the property. A wrong class, expired/foreign handle, or
read-only property refuses the write. Destroyed Actor references read as null and
are never resurrected by a getter. Explicit Destroy still destroys an Actor even
when another object's UPROPERTY refers to it.

`@ue_property=(EditAnywhere,BlueprintReadOnly)` permits reading but refuses source
assignments during macro validation; native accessor checks enforce it too. The
current field default contract is null on the declaring class CDO. Derived CDO
and actor instance references are preserved through existing supported migrations.

Authoring JSON uses `{ "kind": "object", "type_path": "/Script/Engine.Actor" }`
with `default: null`. The adapter schema uses Object kind 8 plus ObjectClass.
Runtime performs native property access and does not interpret authoring JSON.

Stable-ID rename, structural rollback and retirement/restoration now support
object values. Temporary native snapshots pin referenced objects for GC and remap
references through every forward/rollback replacement, including referenced actors
and components. Retirement storage itself contains a GC-traced strong reference.
Stored class identity is checked before restoration. Save changed Blueprint/map
packages to persist the operation; production services do not rewrite source or
autosave assets. Numeric migrations continue to use their existing policy.

Example: Plugins/DasForUE/Content/Das/Scripts/dsl_object_fields.das. It includes
Actor/Component/general-object/read-only fields, getter/setter, validity and clear.
Tests: Scripts/TestDSLObjectFieldsPIE.ps1 and TestDSLObjectFieldsAsset.ps1.
Confirmed results: DSLObjectFieldsValidation.md.
