# Authored ActorComponent classes

`ue_class` now supports a direct native `ActorComponent` parent:

```das
require ue_dsl
[ue_class(Blueprintable)]
class ScoreBehavior : ActorComponent {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Bonus : int = 10
    [ue_function(BlueprintCallable)]
    def Evaluate(input:int):int { return self.Bonus + input }
    [ue_function(BlueprintCallable)]
    def Accumulate(var value:int&):int { value += self.Bonus; return value }
}
```

The importer produces an ordinary Component Blueprint inheriting the fixed
UDasScriptComponent runtime host. Declare an @ue_component field on the Actor; import adds its generated class to SCS.
Blueprint fields, defaults, callable methods and out/ref parameters use the existing
typed frame backend. The component CDO receives the source path automatically;
BeginPlay/Tick/EndPlay use the same exported callback contract as native components.
The `self` in a class method refers to component fields, and
`ue_component_owner(ue_self_object())` gives the checked owning Actor capability.

Executable example: Content/Das/Scripts/dsl_component_behavior.das. The original
actor_component_behavior.das remains a raw native-component lifecycle example.
TestActorComponent.ps1 creates BP_Behavior and BP_Owner, saves/reopens them and their
map, then optionally cooks and runs the map. TestActorComponentPIE.ps1 uses the
authored component in real PIE. No generated C++ is required for each declaration.

Body-only reimport validates the existing declaration without recompiling the
Blueprint. Component declaration/default migration is deferred: the normal importer
rejects changed declarations before mutation. Inheritance from authored components,
SceneComponent-specific support, reentrant script interface dispatch and broader
structural migration are separate future steps. Actor remains the default parent.
The compatibility method ToActorSchema now produces both supported host schemas.

Validation: [DSLActorComponentsValidation](DSLActorComponentsValidation.md).

Actor SCS composition is now declared through repeatable ue_component annotations;
see [DSLComponentComposition](DSLComponentComposition.md).

The simple Actor/Component examples omit module_id. IDs default to the source
filename plus class name: dsl_actor_component_owner.ComponentOwner and
dsl_component_behavior.ScoreBehavior. The source reference declarations resolve
these automatically. Previously generated example assets with actorcomponent.*
identities require importing into a new example folder; this edit does not migrate
or delete existing assets.
