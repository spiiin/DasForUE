# Repeatable Actor construction

Authored Actors can define `ConstructionScript():void`, optionally marked
`[ue_lifecycle]`. It runs from the native Actor's `OnConstruction`, after Unreal's
Blueprint construction and SCS component creation. It is separate from
`Initialize`: construction can repeat in the editor; initialization is delivered
once before the first daScript BeginPlay in a game world.

```das
class ChildCounter : BaseCounter {
    def override ConstructionScript():void {
        Super.ConstructionScript()
        self.ConstructionValue = self.Score * 2 + self.Bonus
    }
}
```

Use assignments derived from editable properties rather than cumulative updates:
moving an Actor or invoking RerunConstructionScripts can execute the hook again.
Per-instance edits are visible, CDOs/archetypes are excluded. Omitted overrides
forward to the authored parent; explicit overrides choose where to call Super.
Unlike Initialize, each explicit Super call executes the parent construction again.

Each construction invocation uses temporary script contexts. Gameplay VMs and
initializer delivery records are preserved. Temporary globals/handles do not
survive the invocation. Body changes are compiled on the next construction;
editing source alone does not automatically reconstruct all placed Actors.

The bridge rejects gameplay lifecycle calls, managed timers/input/latent/async
registrations, event subscriptions and dynamic Actor/component creation during
construction. Existing components may be looked up and configured. This is an API
policy, not a sandbox for arbitrary native reflected functions. Construction
cannot recurse or run after Actor BeginPlay. Hook errors are reported through
LastDasError; property writes are not rolled back, and the next reconstruction can
retry after the source is fixed.

Calls that enter another authored DAS VM through a reflected method are currently
rejected during construction. Same-context script helpers and native property/scene
access remain available. This restriction is separate from the ordinary gameplay
method-call support.

The first version is Actor-only. ActorComponent construction callbacks require a
separate policy for registration/re-registration and are not introduced here.
Loaded cooked map Actors retain saved construction results; they are not forced
through construction at load. Actors spawned at runtime execute construction
before their Initialize/BeginPlay.

Examples: ue_inheritance_base.das, dsl_inheritance_child.das and
dsl_inheritance_grandchild.das. The grandchild omits the hook and forwards to the
child. Tests exercise editor reconstruction, explicit Super, instance edits, CDO
exclusion, temporary VM cleanup, PIE and cooked persistence.


## Validation — 2026-10-15

Editor build and foundation passed (zero failures). Delegate commandlet Create and
fresh-process Reopen passed: reconstruction in an Editor world produced 23, then
43 after Score changed from 10 to 20, and preserved trace 12. CDO remained zero;
a pre-existing reflected-call VM survived construction with unchanged context count.
A forbidden next-tick timer failed on both attempts without creating registrations
or latching initialization failure. IR round-trip and component-hook rejection passed.

Real Gameplay.RuntimePIE passed: exactly one success, zero warnings/failures.
Full Win64 Development NullRHI cook/package pipeline passed, including packaged
foundation (zero failures), saved construction values and runtime-spawn construction
before BeginPlay. Test scripts: TestEditor.ps1, TestAuthoredDelegates.ps1,
TestGameplayRuntimePIE.ps1 and TestCookedGameplay.ps1. Shipping and rendered behavior
were not exercised in this slice.
