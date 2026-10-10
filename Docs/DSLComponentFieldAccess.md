# Typed component fields in Actor method bodies

A field declared with @ue_component now references the actual SCS instance from
the current Unreal Actor:

```das
@ue_component
@ue_property=(VisibleAnywhere, BlueprintReadOnly)
Behavior : ScoreComponent

[ue_function(BlueprintCallable)]
def ConfigureBehavior(bonus:int):void {
    self.Behavior.Bonus = bonus
}

[ue_function(BlueprintCallable)]
def AddBehaviorBonus(amount:int):int {
    self.Behavior.Bonus += amount
    let behavior = self.Behavior
    return behavior.Bonus
}
```

The getter creates a nominal typed weak reference through checked self property
access. It verifies the imported component's authored identity, then existing field
adapters check liveness/kind/permissions on each access. The source reference has
no separate Bonus storage. A local alias denotes the same real component.

BlueprintReadOnly protects the component reference from replacement; it does not
make the component's writable fields immutable. Unknown component fields and
replacement of a readonly component reference fail compilation. Writable-reference
replacement remains explicitly refused pending an ownership policy. Destroyed/null
component access rejects instead of reading a stale instance.

Compound access evaluates the receiver once into a generated local, reads its
field and commits to that same receiver. It does not evaluate a getter again after
the right-hand side. Existing global/local authored reference access is retained.

Example: dsl_actor_component_owner.das. Component methods such as
self.Behavior.Evaluate(5) now use typed overloads and checked VM reentry;
see DSLAuthoredMethodCalls.md. Richer nested value mutation and reflected
authored-reference signatures remain subsequent steps.

The manually constructed runtime probe worlds now use InitializeActorsForPlay so
AActor::ProcessEvent is enabled. PostInitializeComponents alone was insufficient:
Unreal otherwise skipped Actor Blueprint method execution. Actual PIE initializes
its worlds through the normal editor flow.

Validation: [DSLComponentFieldAccessValidation](DSLComponentFieldAccessValidation.md).

The manual compound receiver expansion was replaced by native property operators;
see DSLUniversalAccessors.md for the current common mechanism.

Typed method calls now use the common overload mechanism as well; see
[DSLAuthoredMethodCalls](DSLAuthoredMethodCalls.md).
