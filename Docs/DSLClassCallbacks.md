# Lifecycle and event methods inside DSL classes

Standard lifecycle names are recognized automatically on ue_class methods:

```das
require ue_dsl
[ue_class(Blueprintable)]
class Counter {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Score : int = 0

    def BeginPlay() { self.Score += 100 }
    def Tick(delta:float) { self.Score += 1 }
    def EndPlay(reason:int) { self.Score = 0 }
}
```

No ue_function, ue_lifecycle or export annotation is required for these methods.
They are script lifecycle hooks, not extra Blueprint callable functions. The optional
ue_lifecycle marker is retained for explicitness and signature diagnostics. Hooks
return void; omitted result types are accepted. The fixed signatures are BeginPlay(),
Tick(float), EndPlay(int), with by-value parameters and no defaults. The class may
contain only reflected fields and callbacks, with no Blueprint methods.

The importer assigns the lifecycle source on newly created Actor/Component CDOs.
The macro emits the existing runtime entry points begin_play/tick/end_play and
rewrites self field reads/assignments through checked reflected access. The script
does not construct a second, independent daScript object to hold those fields.

Delegate handlers use an annotation to specify the source and delegate mapping:

```das
[ue_class(parent="ActorComponent", Blueprintable)]
class Behavior {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    SignalTotal : int = 0

    [ue_event(source="owner", name="OnDasSignal")]
    def OnOwnerSignal(amount:int) { self.SignalTotal += amount }
}
```

source="self" is the default; source="owner" is for ActorComponent declarations.
The event name identifies the existing reflected multicast delegate. The first
version supports void handlers with one by-value int argument, matching the current
runtime subscription backend. Several handlers are allowed. The macro generates
bind_events and exported handler functions; users do not call subscription helpers
or name exported entry points manually. Runtime validates the delegate on the live
source before admitting a candidate. Removal/destruction/world cleanup retain the
existing managed revocation behavior.

Repeated/conflicting callback annotations, BlueprintCallable combined with a
callback, bad lifecycle names/signatures and invalid event sources are rejected.
Callbacks have a separate optional authoring-model callbacks array. They participate
in declaration comparison and owner validation without becoming Blueprint methods.
Old files using exported global callbacks remain supported. Generated callback name
collisions are rejected; one lifecycle class per source is the current contract.

The Actor/Component example now uses normal class methods and self assignments;
native bridge example fields remain available for the old raw script example.
ActorComponent native fields are not implicitly injected into a daScript class.
Access to another UObject's fields through an untyped UObjectHandle still uses
the checked object API. Typed authored-object access (such as actor.Score) is a
separate follow-up; self access is implemented in this stage.

Examples: Content/Das/Scripts/dsl_actor_component_owner.das,
dsl_component_behavior.das and dsl_class_lifecycle_only.das.

Validation: [DSLClassCallbacksValidation](DSLClassCallbacksValidation.md).

Direct scalar fields on other authored objects are now supported in DSL methods;
see [DSLAuthoredObjectReferences](DSLAuthoredObjectReferences.md).
