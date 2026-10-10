# Math const-reference events and checked reference views

Added 2026-10-10. **Not built or tested**. No UHT, proxy binding, example import,
callback dispatch, reload, GC, PIE or cook was run for this extension.

```das
[ue_event(source="self",name="OnDasTransformRefSignal")]
def OnPose(value:UETransform):void { self.Pose = value }

[ue_event(source="self",name="OnDasObjectSignal")]
def OnTarget(value:StructMapActorRef):void { self.Target = value }
```

Added by-value and const-reference FVector/FRotator/FTransform native delegate
payloads. The source DAS callback remains by value: values are copied into the
queue and transport temporaries, not borrowed from the broadcasting stack. Mutable
native out/ref parameters remain refused. Reflection selects a matching Receive
proxy function; the managed subscription stores that exact function for teardown.

Queued Rotator values reject NaN/Inf. Transform values also require normalized
rotation. These checks happen before queuing; invalid input is dropped. Existing
game-thread, queue bounds and owner/source lifecycle rules remain.

Object callbacks now accept all existing typed reference views: native object,
native/authored interface, and authored class wrappers. The native delegate signature
is still **exactly UObject***. An interface-typed native parameter or AActor* parameter
is not added by this change. UObject is converted through existing source adapters:
interface membership or authored stable ID is checked for the chosen source view.
Wrong non-null values cause a callback conversion error before its body runs.
Null/destroyed values follow the existing converter's nullable semantics.

No new raw-pointer access or identity fallback is introduced. Object queue records
retain the previous temporary strong GC pin, while source wrappers keep checked
weak handles. Authored interface views still require their generated bindings and
loaded reflected interface identity; this extension does not generate those assets.

Examples:
- dsl_math_events.das: Vector const ref, Rotator/Transform value and const ref.
- dsl_interface_object_events.das: native interface view of a UObject payload.
- dsl_authored_object_events.das: catalog-backed authored class view.

The authored example depends on the earlier StructMapActor assets/source/catalog.
Examples have not been imported. Probe-host math signals have Blueprint-callable
emit methods so the same paths can later be checked through real broadcasts.

Runtime parameter_kind adds rotator/transform. Object events retain object kind;
their more specific source constraints are checked by the generated converter.
Changing a source view does not change the physical native UObject ABI. General
structural migration remains deferred.

Still separate: multiple payload arguments; native derived-object/interface delegate
signatures; arbitrary enum/struct/container payloads; user-authored delegates;
single-cast/native C++ delegates; async thread handoff.

Pending verification: UHT and signature compatibility for value/const-ref variants;
actual copy/ABI behavior; exact subscription removal; math identity/normalization;
null/expired/incorrect interface and logical class IDs; source-wrapper conversion
before body execution; queue GC, reentry, reload, teardown, PIE and packaged runs.
