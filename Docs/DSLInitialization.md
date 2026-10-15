# Once-per-instance runtime initialization

Authored Actors and ActorComponents can declare a zero-argument, void lifecycle
method named `Initialize`. `[ue_lifecycle]` is optional, as for BeginPlay/Tick/EndPlay.
It is not a UFunction, native UObject constructor or repeatable editor construction
script. It runs on the game thread immediately before the first **daScript**
BeginPlay in a game world, including the implicit BeginPlay delivered by Tick.

```das
// In a separately imported parent module:
[ue_class(module_id="game", Blueprintable)]
class BaseActor {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Score:int=10
    def Initialize():void { self.Score+=1 }
}

// In the child script, after importing the parent module:
[ue_class(module_id="game", Blueprintable)]
class ChildActor : BaseActor {
    def override Initialize():void {
        Super.Initialize()
        self.Score+=2
    }
    def BeginPlay():void { /* Score is initialized here. */ }
}
```

An omitted initializer forwards through the authored parent hierarchy. An override
chooses whether and where to call Super.Initialize; it does not receive an automatic
parent call. Calling Super twice invokes that ancestor initializer only once.
Super.BeginPlay does not opt back into a parent initializer skipped by the child's
Initialize override. Super.Initialize outside an initializer is rejected.
Each ancestor callback runs in its own script session on the same UObject, following
the existing lifecycle inheritance backend. Script-only helper dispatch retains its
existing per-context rules.

The engine's component BeginPlay runs before the Actor's script lifecycle. Therefore
authored components perform Initialize -> script BeginPlay before the Actor performs
Initialize -> script BeginPlay. The Actor sees constructed/registered component
instances and their initialized fields. This hook does not move Unreal's native or
Blueprint ReceiveBeginPlay events; it orders the daScript lifecycle callbacks.
Do not assume ordering between different Actors or sibling components.

Reflected CDO values and per-instance edits are present when Initialize starts; the
bridge does not reset them to DAS defaults. Spawned and map-loaded instances use the
same runtime path. CDOs, archetypes and editor-world objects are excluded; ordinary
reflected method calls before BeginPlay do not trigger initialization.

Delivery records are keyed by weak UObject identity and canonical lifecycle source
path outside the VM/session. Successful body reload, explicit session release and
recreation do not replay Initialize. Records transfer through native object
replacement notifications and are pruned on GC/world cleanup. An entirely new
instance initializes again. Lifecycle source identity changes are not a mechanism
for migrating or replaying initialization.

An initializer is attempted once. If it throws or destroys its owner, BeginPlay/Tick
and subsequent script calls are blocked with the latched error. Managed bindings,
timers and input registrations for that object are cancelled. Existing side effects
and property writes are **not rolled back**; recreate the instance after fixing the
source. EndPlay/release remain available for cleanup. Failed initialization is not
retried on every frame, hot reload or session recreation.

The IR stores callback kind `Initialize`. Changes to the hook declaration follow the
normal import/preflight rules; editing the hook body uses normal body-only reload.
Inherited lifecycle declarations still require separate source modules. Repeatable
editor construction remains a separate future hook.

## Reproduction

`Scripts/TestActorSpawnPIE.ps1` covers real PIE initialization of a three-level
authored Actor hierarchy and a two-level component hierarchy, repeated Super calls,
pre-BeginPlay state, component availability and explicit repeat no-ops.

`Scripts/TestCookedCalls.ps1` adds omitted overrides, deliberate parent omission even
with Super.BeginPlay, failure latching, VM recreation, a temporary body-only reload
(restored before cooking), saved Blueprint reopening and Win64 Development NullRHI
packaged execution. The map contains placed fixtures with per-instance edits; the
Spawn fixture uses an edited Blueprint CDO. Logs contain `DAS_INITIALIZER` and
`DAS_INITIALIZER_BODY_RELOAD` markers.


Validation 2026-10-15: final SpawnPIE passed (one success, zero failures/warnings),
editor foundation passed with zero failures, fresh-process Blueprint reopen passed,
and full Win64 Development NullRHI packaged calls/foundation passed (zero failures).
Packaged checks include placed and spawned actors, CDO value 41, instance edit 49,
component readiness, explicit/omitted/skipped Super paths, failure latching and VM
recreation. Editor export verified body-only reload with a revision increase and no
initializer replay; the source was restored before cooking. Legacy manual lifecycle
calls without an Initialize hook retain their prior non-game-world behavior.
