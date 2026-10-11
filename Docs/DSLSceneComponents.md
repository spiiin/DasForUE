# Native SceneComponent reference and gameplay adapters

Added 2026-10-10. Basic creation, transforms, attachment, detachment and destruction
are covered by `DasForUE.Components.Native` in an Editor-hosted game UWorld.
This is not a PIE or packaging test; advanced socket/physics cases remain pending.

```das
require ue_scene_components
let actor = ue_ref(type<ActorRef>,ue_self_object())
let pivot = ue_scene_create(actor,"ScriptPivot")
ue_scene_set_relative_transform(pivot,ue_transform_identity())
```

SceneComponentRef is a checked native weak wrapper with actual native property
class /Script/Engine.SceneComponent. Actor root and attachment-parent getters return
nullable references. ue_scene_valid checks the handle/component and non-destroying
owner. Getters return copied UETransform values.

API:
- ue_scene_actor_root(actor), ue_scene_parent(component), ue_scene_valid(component).
- ue_scene_relative_transform / ue_scene_world_transform.
- ue_scene_set_relative_transform / ue_scene_set_world_transform (teleport defaults false).
- ue_scene_attach(child,parent,socket="",rule="KeepRelative",weld=false):bool.
- ue_scene_detach(component,rule="KeepWorld").
- ue_scene_create(actor,name):SceneComponentRef; ue_scene_destroy(component):bool.

Attach rules: KeepRelative, KeepWorld, SnapIncludingScale, SnapNotIncludingScale.
Detach supports KeepWorld/KeepRelative. Unknown sockets, self/ancestor cycles and
cross-owner-world mutation are rejected before native attach. UE mobility/body rules
can still refuse attachment and return false. Weld forwards the native body-welding
flag. Transform writes require finite normalized values; no sweep/hit-result adapter
is added, and teleport explicitly selects TeleportPhysics vs None.

Creation is native USceneComponent only, in a live game world matching the script
owner's world. Actor/class templates and bind_events/EndPlay creation are refused.
Names allow letters/digits/underscore, require a unique object name and avoid None.
The Actor owns the registered transient instance component. It attaches to an
existing root; if there is none, it becomes root. Registration failure cleans up.
No Blueprint asset/SCS declaration or editor project file is created.

Created components survive ordinary logic VM replacement and session release;
Actor ownership controls lifetime. Do not recreate the same name on every reload.
Destroy is limited to components recorded as created by the same script-owner
UObject; it does not destroy existing native/SCS/foreign-created components. Detach
children first. This is owner identity, not per-script-file ownership. Tracking is
bounded to 1024 live entries and pruned on later creation; weak records do not retain
owners/components. Structural reinstancing/ownership transfer is not implemented
by this tracking and remains outside the current promise.

Examples: ue_scene_components.das and dsl_scene_components.das. The actor creates
a pivot on BeginPlay and exposes move/attach/detach/remove methods. Import and runtime
behavior were not executed. Dynamic scene creation is distinct from the existing
authored SCS component DSL; authoring SceneComponent-derived DAS classes is not added.

Still separate: arbitrary native component classes/mesh setup, authored scene hosts,
root replacement, child migration, sweeps/hit results, replication, construction
script/editor creation and broad structural reload.

Pending checks: build/UHT/DAS default arguments; nullable references; duplicate names;
game-world/template/context restrictions; registration/root behavior; relative/world
math/teleport; socket/mobility/cycle/refusal rules; exact owned destruction/children;
Actor/GC/logic reload and reinstancing boundaries; real PIE and packaged Development.

## Named lookup and reuse

`ue_scene_get(actor,name)` returns a nullable `SceneComponentRef` for an owned
component with that exact UObject name (Unreal FName comparison). Missing names
return an invalid reference. A matching non-scene/destroying component is an error.
Names follow the same validation as creation. Actor must be live, non-template,
and in the script owner's game world.

`ue_scene_get_or_create(actor,name)` returns the existing scene component or calls
the normal creation path. Existing components retain their transform, attachment
and ownership; lookup never grants destruction rights. An unrelated UObject name
collision still fails creation. This searches instance names, not DSL field names
or component tags. No arbitrary component class parameter is provided yet.

```das
let pivot = ue_scene_get_or_create(actor,"ScriptPivot")
let existing = ue_scene_get(actor,"ScriptPivot")
```

The native-components fixture checks missing lookup, creation, repeat reuse,
lookup after creation and invalidation of both references after destruction.
