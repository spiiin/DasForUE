# ActorComponent ConstructionScript

Authored ActorComponents support the same zero-argument `ConstructionScript():void`
hook as Actors. `[ue_lifecycle]` is optional. An override explicitly calls
`Super.ConstructionScript()`; an omitted override forwards to the authored parent.
Each explicit Super call runs the parent again. This is separate from the
once-per-instance `Initialize` hook and from native UObject constructors.

```das
// Base class in an imported public module.
[ue_class(Blueprintable)]
class BaseBehaviour : ActorComponent {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Bonus:int=10
    @ue_property=(BlueprintReadWrite)
    Configured:int=0
    def ConstructionScript():void { self.Configured=self.Bonus*2 }
}

// Child in its own source file, importing the base module.
[ue_class(Blueprintable)]
class ChildBehaviour : BaseBehaviour {
    def override ConstructionScript():void {
        Super.ConstructionScript()
        self.Configured+=3
    }
}
```

## Delivery

The native authored Actor bridge enumerates its live daScript components at the end
of OnConstruction, after SCS creation and Unreal's instance-data restoration. Each
component hook runs before the Actor hook, including when the Actor itself has no
script construction hook. The Actor can then read the configured component fields.
Sibling order is unspecified. Reconstructing the Actor repeats the component pass;
recreated SCS components receive the edited/template values restored by Unreal.

For a component on another native/Blueprint Actor, the fallback runs once on its
first registration. A component dynamically registered after its owner's BeginPlay
also uses this path, before the component's own Initialize/BeginPlay. This fallback
does not promise that the owner's remaining SCS or Blueprint construction is done,
or that sibling components and final owner construction edits are available. The
full after-SCS ordering requires the authored Actor bridge. A native/instance
component preserved across reconstruction of an unrelated Actor does not receive a
repeat merely because that Actor reconstructed.

Unregister/Register does not repeat construction. Editing a live component property
in an editor world triggers a new pass after the ordinary UE property-change path.
If UE already reconstructed the authored owner, the component does not run twice;
if the original component was replaced, it is not accessed again. CDOs/archetypes
are excluded. Loaded components retain their saved results rather than running the
fallback during map load/cook. PIE duplication preserves prior delivery state.

Construction must finish before the receiving component's BeginPlay; its owner may
already have begun play when a new component is added dynamically. Ordinary
game-world re-registration and property edits do not reconstruct running components.
Source edits alone do not reconstruct every placed Actor/component automatically.

## State and failures

Construction uses temporary VMs, preserving gameplay sessions and initializer
delivery state. Managed timers, subscriptions, input/latent/async registrations and
dynamic Actor/component creation/destruction through bridge APIs remain prohibited.
This is an API policy rather than a sandbox for arbitrary native calls. Reflected
call policy is shared with Actor construction.

Errors are recorded in the component's LastDasError and may be retried by an
explicit construction pass or the next editor reconstruction. Existing writes are
not rolled back. During an authored Actor pass, a component error is also reported
on the Actor and its own construction hook is skipped. This does not introduce a
permanent Initialize failure latch or transactional rollback.

Examples: ue_life_behavior_base.das, dsl_life_behavior_child.das,
dsl_construction_component.das and dsl_construction_component_leaf.das. The leaf
omits the hook and exercises implicit forwarding. ConstructionCalls in the test
fixture is instrumentation; production hooks should normally compute values from
editable inputs so repeated construction is safe.


## Validation — 2026-10-16

Editor build and foundation passed with zero failures. Create and fresh-process
Reopen passed for explicit component Super and an omitted leaf override. The editor
probe covers property edits, registration/re-registration, CDO exclusion, preservation
of an existing gameplay VM, forbidden timers and successful retry after failure.
The authored owner sees its inherited component's configured value 113 before its
own hook, and reconstruction updates preserved instance components.

Real Gameplay.RuntimePIE passed: one success, zero warnings/failures. An initial PIE
failure exposed the registration-state duplication case; explicit PostDuplicate/PIE
and PostLoad handling resolved it. Final tests retain the deliberately saved state
Score=8, ConstructionValue=21 and ConstructionCalls=1 instead of recalculating 24.
Dynamic native-owner components initialize and begin play only after construction;
plain re-registration does not repeat either construction or initialization.

The full Win64 Development NullRHI pipeline passed: cook/package, packaged foundation
(zero failures), loaded native-component saved values, inherited component-before-Actor
values and runtime component creation. Temporary PIE diagnostic logging was removed.
Reproduction: TestEditor.ps1, TestAuthoredDelegates.ps1,
TestGameplayRuntimePIE.ps1 and TestCookedGameplay.ps1. Shipping was not tested.
