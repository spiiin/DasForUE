# Async Physics Tick

A DSL class can run a separate, isolated daScript program inside Unreal's Async Physics Tick (the Chaos pre-simulation callback) and apply forces to its own bodies there. The gameplay script and the physics script never share memory: they talk through a fixed mailbox of 16 `double` values in each direction.

Ordinary game-thread forces and impulses (via the generated Collision/Physics API) do not need any of this; use async physics only when you need per-physics-step control.

## Declaring it

Add the `ue_async_physics` annotation next to `ue_class`:

```das
[ue_class(module_id="asyncphysicsdsl",Blueprintable),
 ue_async_physics(script="async_physics_thruster_tick.das",body_0="Body")]
class Thruster {
    @ue_component
    @RootComponent
    Body:AsyncBoxRef
    // Initialize/BeginPlay/Tick stay in the normal gameplay VM.
}
```

| Parameter | Meaning |
| --- | --- |
| `script` | File name of a separate `.das` file in the same directory (no path). |
| `body_0` ... `body_7` | Names of component properties (declared `ue_component` or native component property) to allow as bodies in that slot. Not UObject paths or C++ expressions. |

- You can declare several slots, or none for pure computation.
- In an ActorComponent class you may select a body on the owner: `body_0="owner.RootComponent"` (or another component property of the owner). An Actor class cannot use the `owner.` prefix.
- Duplicate parameters, unknown slots, the same target in two slots, and missing declared/native properties are rejected at compile/apply time. The real body type and generation are checked natively at runtime.

Example files: `Plugins/DasForUE/Content/Das/Scripts/async_physics_thruster.das` (Actor with a native Box component, simulation setup in `Initialize`, gameplay mailbox) and `async_physics_thruster_tick.das` (the physics script).

## The physics script

The physics file has one entry point, `[export] def physics_tick(dt:float; time:float)`:

```das
options gen2
require ue_foundation
var seconds:double=0.0lf
[export]
def physics_tick(dt:float; time:float) {
    let body=ue_physics_body(0)
    // World-space force; do not multiply by dt.
    ue_physics_add_force(body,ue_physics_input(0),0.0lf,0.0lf,false)
    seconds+=double(dt)
    ue_physics_output(0,seconds)
}
```

Only four UE functions exist in this program:

| Function | Purpose |
| --- | --- |
| `ue_physics_input(slot:int):double` | Read the latest input packet (slots 0..15). |
| `ue_physics_output(slot:int, value:double)` | Write the output packet (slots 0..15). |
| `ue_physics_body(slot:int):UEPhysicsBodyToken` | Token for the body bound to slot 0..7. |
| `ue_physics_add_force(body, x, y, z, accel_change:bool)` | Add a world-space force. With `true`, the value is an acceleration and is multiplied by mass natively. |

There is no gameplay `self`, and every other UE extern (including value helpers such as `ue_vector`) is refused. Plain daScript math and local values work normally. The physics globals belong to this VM only.

## Gameplay mailbox

From `Initialize`, `BeginPlay`, `Tick` and ordinary `ue_function` methods of the class:

```das
ue_async_physics_set_input(0,1000.0lf)
if(ue_async_physics_has_output()) {
    let seconds=ue_async_physics_output(0)
}
let running=ue_async_physics_running()
let failure=ue_async_physics_error()
```

- 16 finite `double` values each way, no strings, objects, tokens or containers. `double` cannot represent every large `int64` exactly.
- It is a mailbox, not a queue: the latest input packet replaces the previous one. Inputs set in `Initialize`/`BeginPlay` before activation are delivered at start.
- The output packet starts as zeros each step and is published only after `physics_tick` returns successfully. Before the first successful step there is no output: check `ue_async_physics_has_output()`.
- ConstructionScript, EndPlay and the physics callback itself cannot use the gameplay mailbox.

## Bodies and forces

Supported bodies: Box, Sphere, Capsule and StaticMesh components with a single own BodyInstance, not welded and without welded children, owned by the same Actor. Skeletal/ragdoll, InstancedStaticMesh/HISM, geometry collections/cluster unions, static/kinematic and not-yet-ready bodies are rejected.

`UEPhysicsBodyToken` is opaque: no fields, no numeric cast or constructor. A token is valid only in the callback that produced it; you can copy it into a local or container, but a previous step's token is rejected. It cannot be a property, a `ue_function` parameter/result, or appear in authoring JSON. Gameplay code and the globals initializer cannot obtain one.

Force rules:

- At most 64 `ue_physics_add_force` calls per VM per step. Components must be finite, mass/inverse mass positive and finite, and resulting force/acceleration/velocity must not overflow.
- Forces are accumulated and committed in the **same** pre-sim step after `physics_tick` succeeds, one summed AddForce per body. Chaos handles wake-up and integration.
- A script error, wrong/foreign/stale token, overflow or limit breach discards this VM's forces for the current step, discards the output packet and disables the VM. Catching the error with `try/recover` does not un-poison the step. Earlier steps are not rolled back.
- Body recreation invalidates the binding. If the script never touches a missing body, no error is raised for it.

## Lifecycle, inheritance and reload

- When the gameplay session is prepared, the physics program is compiled separately, `physics_tick` is verified, and its globals are initialized with UE calls disallowed.
- `Initialize` and the user `BeginPlay` run **before** the callback is registered, so bodies must be ready by then. If a component's owner Actor has not finished BeginPlay, activation waits until the component's first Tick after the Actor is ready. A BeginPlay error blocks registration. No force is applied before activation.
- The recipe is inherited from an authored parent; a child annotation replaces it entirely. There is one mailbox per instance, shared by inherited callbacks and parent methods.
- The physics file and its dependencies are part of the gameplay session's sources. Editing the implementation hot-reloads it on the game thread: on a syntax/signature/init error the old gameplay and physics versions keep running; on success the old callback is revoked and the new one takes over. `Initialize`/`BeginPlay` are not repeated, the input packet is kept, physics globals and output restart.
- Changing the recipe itself (script name, body slots, selectors) is a declaration change and needs the normal Apply/Reimport. The editor preflight compiles the physics script before the Blueprint is modified.
- A runtime error in `physics_tick` stops that VM; it is not restarted automatically. Read the message with `ue_async_physics_error()`.
- Destroy, unregister, EndPlay, world cleanup and runtime shutdown revoke the binding safely. Class migration/default updates conservatively revoke bindings without automatic rebind.

## Execution roles

The plugin distinguishes a **Gameplay** role and a **PhysicsPreSim** role. During the frozen pre-sim phase `IsInGameThread()` is still true, so the plugin additionally detects the frozen solver (using APIs available in Shipping). In PhysicsPreSim:

- every ordinary UE extern is refused before its native body runs, including low-level calls, static functions and calls through another WorldContext;
- entering the gameplay VM (RunFile/RunCallback, authored methods, static callbacks) is refused before compilation;
- only the four physics functions above are allowed, and only for the matching isolated program and the current engine-issued step.

The raw reflected `GetBodyInstanceAsyncPhysicsTickHandle` and `PhysicsThreadLibrary.AddForce` stay blocked in the generated API; the scoped token above is the supported replacement.

## Resource limits and trust model

The physics program disallows `unsafe` and the global heap; stack and static globals are limited to 64 KiB each, runtime and string heaps to 1 MiB each. This is not a hard deadline (an infinite loop can still stall the solver) and not a security sandbox for third-party modules or native extensions: the script is trusted code, isolated by Context and a checked FFI boundary.

## C++ API

C++ code can create the same VM without the DSL (`Public/DasAsyncPhysicsVM.h`), on the game thread after the owner's BeginPlay:

```cpp
auto VM = CreateDasAsyncPhysicsVM(Path, Actor, Error);
if (VM && !VM->BindBody(0, Mesh, Error)) { VM->Revoke(); }
FDasPhysicsPacket Input;
Input.Values[0] = 100.0;
VM->Publish(Input, Error);
FDasPhysicsOutput Output;
if (VM->Read(Output)) { /* use the copy in Output.Packet.Values */ }
```

`ClearBody(slot, Error)` removes a body permission while keeping the VM. To replace a VM, create and verify the new one, then `Revoke()` the old one. Dropping the handle does not unbind: use `Revoke` or the owner lifecycle. `Read` returns `false` after the VM is disabled.

## Limitations

- No network rewind/resimulation, no worker-thread VM, no hard execution deadline.
- Only the body kinds listed above; only AddForce/acceleration change.

See also: [Lifecycle](../DSL/Lifecycle.md), [Components](../DSL/Components.md), [Apply and hot reload](../Workflow/ApplyAndHotReload.md), [Collision and Physics API](../../Api/CollisionPhysics.md)
