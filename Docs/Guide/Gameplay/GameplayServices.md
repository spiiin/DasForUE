# Gameplay Services

`ue_gameplay_services` is a small, curated layer for everyday gameplay: line and sphere traces, sphere overlaps, a simple HUD (create a Widget Blueprint, set named text, bind button clicks, toggle UI input) and native damage. The playable example uses all of them.

Example scripts (in `Plugins/DasForUE/Content/Das/Scripts/`): `dsl_playable_character.das`, `dsl_playable_pickup.das`, `dsl_health_component.das`, and the shared Blueprint interface in `ue_interaction_contract.das`.

## Traces

```das
require ue_gameplay_services

let hit=ue_line_trace(start,finish)
if(hit.blocking_hit) {
    let actor=ue_hit_actor(hit)
    let component=ue_hit_component(hit)
    // actor/component are checked weak references; they do not keep the target alive.
}
let sweep=ue_sphere_trace(start,finish,40.0lf)
var ignored:array<ActorRef>
push(ignored,actor)
let next=ue_line_trace(start,finish,ignored,TraceChannel.Visibility)
```

Signatures:

```das
ue_line_trace(start, finish [, var ignored:array<ActorRef>], channel=TraceChannel.Visibility, complex=false, ignore_self=true):UETraceHit
ue_sphere_trace(start, finish, radius:double [, var ignored:array<ActorRef>], channel=TraceChannel.Visibility, complex=false, ignore_self=true):UETraceHit
```

By default the query ignores the script's own Actor (or a component's owner). `TraceChannel` lists the engine channels (`WorldStatic`, `WorldDynamic`, `Pawn`, `Visibility`, `Camera`, `PhysicsBody`, `Vehicle`, `Destructible`) and `GameTrace1`..`GameTrace18` for custom project channels. Radius, coordinate finiteness, channel and world membership are checked.

### UETraceHit

| Fields / getters | Notes |
| --- | --- |
| `blocking_hit`, `start_penetrating`, `distance`, `time`, `penetration_depth` | Copies of FHitResult values. |
| `location`, `impact_point`, `normal`, `impact_normal`, `trace_start`, `trace_end` | `UEVector`. |
| `face_index`, `item`, `my_item`, `element_index` | Indices. |
| `ue_hit_actor(hit)`, `ue_hit_component(hit)` | Nullable `ActorRef` / `TraceComponentRef` (PrimitiveComponent). |
| `ue_hit_physical_material(hit)` | Nullable `PhysicalMaterialRef`; availability depends on collider and query type. |
| `ue_hit_physics_owner(hit)` | Nullable `Object`. |
| `ue_hit_bone_name(hit)`, `ue_hit_my_bone_name(hit)` | `UENameValue`. |

`UETraceHit` is an ordinary daScript value you can keep in a variable. Object getters are validated in the context that produced the result and return null after the target is destroyed; an uninitialized result is rejected. The Actor is read without materializing a dehydrated actor instance, so the Actor can be null while the component is live.

## Sphere overlap

```das
let results <- ue_sphere_overlap(center,75.0lf)
for(result in results) {
    let actor=ue_overlap_actor(result)
    let component=ue_overlap_component(result)
    let physics_owner=ue_overlap_physics_owner(result)
    print("blocking={result.blocking_hit}, item={result.item_index}\n")
}
```

`UEOverlapResult` has `blocking_hit`, `item_index`, `my_item_index` and checked getters for Actor, Component and physics-object owner. An overload takes `var ignored:array<ActorRef>`, plus `channel` and `ignore_self`. Input and output are limited to 4096 objects. This is a channel-based overlap; results are not deduplicated per Actor.

Multi-hit traces, box/capsule sweeps, object-type/profile queries and the full generated `FHitResult` transport are available in the [Collision and Physics API](../../Api/CollisionPhysics.md).

## Simple HUD

```das
// HUDClass is $UEClass<UserWidgetRef> (alias UserWidgetClass); it may reference a Widget Blueprint.
self.HUD=ue_widget_create(self.HUDClass,player)
verify(ue_widget_show(self.HUD),"HUD cannot be shown")
ue_widget_set_text(self.HUD,"Score",ue_text_invariant("Score: {self.Score}"))
ue_widget_bind_click(self.HUD,"Damage",self.HurtFromHUD)
ue_widget_input(self.HUD,true)
// Later:
ue_widget_remove(self.HUD)
```

| Function | Notes |
| --- | --- |
| `ue_widget_create(kind:UserWidgetClass, player:PlayerControllerRef):UserWidgetRef` | Needs a live game world and a local PlayerController; refused on a dedicated server. |
| `ue_widget_show(widget, z_order=0):bool`, `ue_widget_visible(widget):bool`, `ue_widget_remove(widget)` | Viewport management. |
| `ue_widget_set_text(widget, name, text)` | `name` must find a `TextBlock`; FText semantics are kept. |
| `ue_widget_bind_click(widget, name, handler)` | `name` must find a `Button`; handler (`self.Method` or its name; inside `ue_class` it is checked at compile time) is a `BlueprintCallable` method of the context owner with no parameters and no result. |
| `ue_widget_input(widget, enabled)` | `true` switches to GameAndUI with a cursor; `false`/cleanup restores GameOnly and the previous cursor visibility. |

- The context keeps created widgets alive against GC. Binding the same button again replaces the previous handler.
- Clicks are delivered on the next CoreTicker pass after rechecking owner, widget and method. Removing the widget, destroying the owner or replacing the context unbinds handlers and cancels undelivered clicks. Ending the context removes the HUD from the screen.
- `ue_widget_input` manages input mode for one owner only; it does not restore an arbitrary external UI input-mode stack.
- The example's `WBP_HUD` derives from the native `UDasGameplayHUD` with a ready widget tree; any Widget Blueprint works if the element names match.

For widget input handlers, painting, drag and drop and the generated UMG API, see [Widget events](../UMG/WidgetEvents.md) and [UMG and Input API](../../Api/UMGInput.md).

## Damage and health

`ue_apply_damage(target:ActorRef, amount:float, source=default<ActorRef>, instigator=default<PlayerControllerRef>):float` calls native `UGameplayStatics::ApplyDamage`. The amount must be finite and non-negative, objects must be live and in the same world, and the caller needs authority. No replication is added. (The `ue_ai` module has an overload taking an `AIControllerRef` instigator.)

`dsl_health_component.das` is a fully authored ActorComponent: it subscribes to `owner.OnTakeAnyDamage`, clamps health at zero, provides `Heal` clamped to `Maximum`, and broadcasts `Changed(current,maximum)`. `ue_event(source="Health",...)` lets the owning Character subscribe to its own component and update the HUD.

## Try it

`.\Scripts\CreatePlayableCharacter.ps1` creates a fresh asset folder with the playable map. Controls: WASD move, mouse look, Space jump, E interact with what the camera looks at (through the Blueprint interface), H take 25 damage, Tab toggle the cursor. Walking into the pickup also collects it.

## Limitations

- Traces here are single-hit Line/Sphere; `UETraceHit` cannot be a `ue_property` or `ue_function` parameter (use the generated `UEHitResult` from the Collision API for that).
- The HUD helpers cover creation, display, named text and clicks only.
- Point/radial damage and networking are not provided by this module.

See also: [Spawn and queries](SpawnAndQueries.md), [Input](Input.md), [AI and perception](AIAndPerception.md), [Animation and effects](AnimationAndEffects.md), [Examples](../GettingStarted/Examples.md)
