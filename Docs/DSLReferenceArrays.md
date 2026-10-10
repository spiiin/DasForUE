# Arrays of native typed object/interface references

Arrays of generated native object wrappers (for example ActorRef/ObjectRef) and
native interface wrappers are supported in ue_class/ue_struct properties and
reflected method input/result/ref/out signatures.

```das
@ue_property=(EditAnywhere, BlueprintReadWrite)
Actors : array<ActorRef>
@ue_property=(EditAnywhere, BlueprintReadWrite)
Events : array<DasEventInterface>

push(self.Actors, actor)
push(self.Actors, default<ActorRef>)
self.Actors[0] = actor
self.Groups[0].Actors[0] = default<ActorRef>

var copy = api.CopyActors(api.Actors)
api.Actors := copy
clear(api.Events)
```

Element wrappers are nominal source types: ActorRef cannot be replaced by ObjectRef
even for null values, and object/interface arrays are not interchangeable. Native
pins retain the exact UObject class or interface. Dynamic carrier paths encode
the native class path and are restricted to loaded /Script native types.

The UE property stores ordinary strong object/interface references. Snapshot
FInstancedStruct values participate in the existing FGCObject root collectors;
they can temporarily retain referenced objects. Converted DAS wrappers contain
the existing weak checked handles and do not own the objects. Getters produce
owned arrays rather than a live native container view.

Null elements round-trip. A destroyed Actor is exposed as a null handle when read.
Writes through stale non-null handles still follow ResolveObject rejection. Wrong
live native class/interface values in snapshots are rejected by element validation.
Readonly/type/bounds checks and atomic publication/ref copyback remain in force.

Definitions default to empty. Arrays of authored object views and authored
interface wrappers are explicitly deferred because native host storage alone
does not enforce each element's logical authored ID. Soft/class references,
object-key maps/sets, additional ownership/default instancing and structural
migrations are separate work.

Fixtures: ValueTarget.ReferenceArrays/CopyActors/CopyEventRefs/RefActors/RefEventRefs,
FailActors and MethodGroup.Actors/EventRefs. Shared editor/PIE/game probe injects
and destroys an Actor before reading DirectActors.
Validation: DSLReferenceArraysValidation.md.

Update: authored class arrays in class fields/signatures are covered in DSLAuthoredReferenceArrays.md; authored struct members/interfaces remain deferred.
