# Lifecycle

Authored Actors, Pawns, Characters and ActorComponents react to Unreal's lifecycle
through methods with reserved names. They need no annotation, are not Blueprint
functions, and run against the real object's reflected fields through `self`.

## Hooks

| Hook | Signature | When it runs | Repeats? |
| --- | --- | --- | --- |
| `Initialize` | `():void` | Once per instance, right before the first daScript `BeginPlay` in a game world | No |
| `ConstructionScript` | `():void` | From `OnConstruction`, after Blueprint construction and SCS creation | Yes (editor reconstruction) |
| `BeginPlay` | `():void` | Game start or spawn | Once per session |
| `Tick` | `(delta:float):void` | Every frame | Yes |
| `EndPlay` | `(reason:int):void` | End of play/destroy | Once per session |

```das
require ue_dsl
[ue_class(Blueprintable)]
class Counter : Actor {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Score : int = 0

    def BeginPlay() { self.Score += 100 }
    def Tick(delta:float) { self.Score += 1 }
    def EndPlay(reason:int) { self.Score = 0 }
}
```

- Signatures are fixed: by-value parameters, no defaults, `void` result (the
  result type may be omitted).
- `[ue_lifecycle]` is optional; it makes intent explicit and improves signature
  diagnostics.
- Bad names or signatures, duplicate hooks, and combining a hook with
  `BlueprintCallable` are rejected.
- Hooks can be mixed freely with reflected methods, helpers and event handlers.
- One lifecycle class per source file. In a hierarchy, parent and child must be in
  separate files (see [Inheritance](Inheritance.md#inherited-lifecycle)).
- Older scripts using exported global `begin_play`/`tick`/`end_play` functions remain
  supported.
- For Pawn/Character without daScript lifecycle hooks, the native tick state chosen
  by the class defaults is preserved. `EndPlay` releases persistent script contexts.

Examples: `dsl_actor_component_owner.das`, `dsl_component_behavior.das`,
`dsl_class_lifecycle_only.das`.

## Initialize: once per instance

```das
// Parent module:
[ue_class(Blueprintable)]
class BaseActor {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Score:int=10
    def Initialize():void { self.Score+=1 }
}

// Child script, importing the parent module:
[ue_class(Blueprintable)]
class ChildActor : BaseActor {
    def override Initialize():void {
        Super.Initialize()
        self.Score+=2
    }
    def BeginPlay():void { /* Score is initialized here. */ }
}
```

- Runs on the game thread immediately before the first daScript `BeginPlay`,
  including the implicit BeginPlay delivered by Tick. It is not a UFunction and not
  a native UObject constructor.
- Reflected CDO values and per-instance edits are already present; nothing is reset
  to source defaults. Spawned and map-loaded instances behave the same. CDOs,
  archetypes and editor-world objects never initialize, and ordinary reflected
  method calls before BeginPlay do not trigger it.
- **Ordering:** Unreal begins components before their Actor, so authored components
  run `Initialize -> BeginPlay` before the Actor does; the Actor sees its components
  constructed, registered and initialized. Do not assume ordering between sibling
  components or different Actors. Native/Blueprint `ReceiveBeginPlay` is not moved.
- **Inheritance:** an omitted initializer forwards to the parent. An override decides
  whether and where to call `Super.Initialize()`; calling it twice still runs the
  ancestor once. `Super.BeginPlay()` does not run a parent initializer the child
  skipped. `Super.Initialize()` outside `Initialize` is rejected.
- **Reload:** delivery is recorded per object outside the VM, so body reloads,
  session release or recreation never replay `Initialize`. A new instance
  initializes again.
- **Failure:** the initializer is attempted once. If it throws or destroys its
  owner, BeginPlay/Tick and later script calls are blocked with the latched error,
  and managed bindings, timers and input for that object are cancelled. Writes
  already made are **not rolled back**; fix the source and recreate the instance.
  `EndPlay` and release still work.

Try it: `Scripts/TestActorSpawnPIE.ps1` (three-level Actor and two-level component
hierarchy in PIE).

## ConstructionScript: repeatable editor construction

```das
class ChildCounter : BaseCounter {
    def override ConstructionScript():void {
        Super.ConstructionScript()
        self.ConstructionValue = self.Score * 2 + self.Bonus
    }
}
```

- Runs from the native `OnConstruction`, after Unreal's Blueprint construction and
  SCS component creation. Moving an Actor or `RerunConstructionScripts` runs it
  again, so **assign values derived from editable properties** rather than
  accumulating.
- Per-instance edits are visible; CDOs/archetypes are excluded.
- Omitted overrides forward to the parent. Unlike `Initialize`, each explicit
  `Super.ConstructionScript()` call runs the parent again.
- Actors spawned at runtime run construction before `Initialize`/`BeginPlay`.
  Actors loaded from a cooked map keep their saved construction results.
- Each invocation uses temporary script contexts. Gameplay VMs, globals and
  initializer records are preserved; temporary globals do not survive. Body changes
  apply on the next construction, but editing the source does not reconstruct all
  placed Actors.

### Allowed and forbidden in construction

Allowed: reading and writing fields, looking up and configuring existing
components, and calling reflected `ue_function` methods on authored Actors and
components (including components created by the class's component declarations):

```das
def ConstructionScript():void {
    self.Result = self.Worker.Evaluate(7)
    if(ue_object_valid(self.CounterTarget.handle)) {
        self.Result += self.CounterTarget.ReadScore()
    }
}
```

Such calls run in the current construction's temporary VMs, do not deliver
`Initialize`/`BeginPlay`, and keep the usual typed marshalling and the 32-call
recursion limit. Native reflected functions and accessors are allowed unless they
are marked unsafe during Actor construction; that is checked at call time.

Forbidden (also inside nested methods): gameplay lifecycle calls, recursive
construction, managed timers, input, latent and async registrations, event
subscriptions and broadcasts, and dynamic Actor/component creation or destruction.
This is an API policy, not a sandbox for arbitrary native calls. Construction
cannot run after the Actor's BeginPlay.

Errors go to `LastDasError`. Property writes are not rolled back, and the next
reconstruction retries after you fix the source.

Examples: `ue_inheritance_base.das`, `dsl_inheritance_child.das`,
`dsl_inheritance_grandchild.das` (omitted hook), `dsl_construction_calls.das` with
`dsl_construction_call_worker.das`. Try it: `Scripts/TestConstructionCalls.ps1`.

## Component construction

Authored ActorComponents support the same `ConstructionScript():void` hook with
the same Super and forwarding rules:

```das
[ue_class(Blueprintable)]
class BaseBehaviour : ActorComponent {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Bonus:int=10
    @ue_property=(BlueprintReadWrite)
    Configured:int=0
    def ConstructionScript():void { self.Configured=self.Bonus*2 }
}

// Child in its own source file:
[ue_class(Blueprintable)]
class ChildBehaviour : BaseBehaviour {
    def override ConstructionScript():void {
        Super.ConstructionScript()
        self.Configured+=3
    }
}
```

Delivery depends on the owner:

- **On an authored Actor:** at the end of the Actor's `OnConstruction`, after SCS
  creation and instance-data restoration, every daScript component runs its hook
  before the Actor's own hook (even if the Actor has none). The Actor can read the
  configured values. Sibling order is unspecified. Reconstruction repeats the pass.
  A component error is also reported on the Actor, and the Actor hook is skipped.
- **On a native or Blueprint Actor:** the hook runs once, on first registration.
  Nothing guarantees that the owner's other SCS components or construction edits
  are ready.
- **Added dynamically after the owner began play:** same first-registration path,
  before the component's own `Initialize`/`BeginPlay`.

Unregister/Register does not repeat construction. Editing a live component
property in an editor world triggers a new pass, never twice for one
reconstruction. CDOs and archetypes are excluded; loaded components keep their
saved results; PIE duplication preserves delivery state. The same forbidden-API
list and error handling as Actor construction apply.

Examples: `ue_life_behavior_base.das`, `dsl_life_behavior_child.das`,
`dsl_construction_component.das`, `dsl_construction_component_leaf.das` (omitted
hook).

## Event handlers

Methods annotated with `ue_event` subscribe to multicast delegates for the
lifetime of the play session:

```das
[ue_class(Blueprintable)]
class Behavior : ActorComponent {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    SignalTotal : int = 0

    [ue_event(source="owner", name="OnDasSignal")]
    def OnOwnerSignal(amount:int) { self.SignalTotal += amount }
}
```

`source="self"` is the default; `source="owner"` targets the owning Actor of a
component. Subscriptions are created at BeginPlay and revoked on EndPlay,
destruction and world cleanup; you never call subscription helpers yourself.
Supported sources, signatures and parameter types are described in
[Delegates and events](DelegatesAndEvents.md).

See also: [Classes](Classes.md) · [Inheritance](Inheritance.md) ·
[Components](Components.md) · [Self access](SelfAccess.md) ·
[Timers](../Gameplay/Timers.md) · [Apply and hot reload](../Workflow/ApplyAndHotReload.md)
