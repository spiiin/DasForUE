# Owned script objects

Use `class Action : ScriptObject` for a plain authored UObject. It has no Actor
BeginPlay/Tick lifecycle. `Outer` supplies the containment/name chain and world
context, but does not by itself keep a child alive.

```das
require owned_object_refs

[ue_class(Blueprintable)]
class Owner : Actor {
    @ue_property=(EditAnywhere,BlueprintReadWrite,Instanced)
    ActionTemplate:OwnedActionRef
    @ue_property=(BlueprintReadWrite)
    Actions:array<OwnedActionRef>

    [ue_function(BlueprintCallable)]
    def Create(kind:OwnedActionClass):bool {
        let owner=ue_ref(type<ActorRef>,ue_self_object())
        let action=ue_object_new(type<OwnedActionRef>,kind,owner,self.ActionTemplate)
        push(self.Actions,action)
        action.Initialize(owner)
        return action.Ready()
    }
}
```

Apply `owned_object_action.das` first, then `owned_object_owner.das`. The imported
`owned_object_refs.das` resolves the authored base to its applied Blueprint class.
The `owned_object_*.das` samples are in `Plugins/DasForUE/Content/Das/Scripts/`.

The three-argument factory uses class defaults; the optional fourth argument uses
an object template (a null reference also means class defaults). A non-null
template must have exactly the requested class, to prevent accidentally slicing
or copying a sibling. It may be an archetype or a live object in the same world.
Actors/components use their own factories. This factory remains restricted to
concrete `ScriptObject` descendants and a live, same-world Outer.

`Initialize` is an ordinary reflected DAS method, called explicitly after storing
the object and assigning prerequisite fields. It is not automatically invoked by
construction, duplication, loading or template editing; repeated calls follow the
method's own policy. The example records initialization in reflected state.
Standard lifecycle names are not implicitly reserved on ScriptObject; an annotated `Initialize(owner)`
is an ordinary method. Actor/component/subsystem lifecycle rules are unchanged.

`Instanced` on a direct strong ScriptObject property emits Unreal's
`InstancedReference`, `ExportObject` and `PersistentInstance` flags, with EditInline
metadata. Select/create a configured inline instance in Blueprint defaults; native
Unreal instancing gives each owning object a distinct copy. Ordinary object fields
and arrays still hold strong references, without implicit cloning. Instanced
containers, weak references, class references and scalar values are rejected by
this first instanced-property implementation. Do not use transient runtime objects
as persistent asset defaults.

DAS handles are checked weak references. A reflected field or array retains the
live object; clearing the last strong reference permits GC even while Outer is
alive. GC releases the collected ScriptObject's runtime context. Holding references
to the created object in snapshots or other owners can extend its lifetime.
