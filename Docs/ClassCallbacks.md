# Class references in interfaces and callbacks

Use the same `UEClass<T>` type as in ordinary reflected methods. The
`ue_object_types` module already exports `ActorClass = $UEClass<Actor>`.
The type records the permitted parent once; individual methods do not need
`arg_*_class` or `return_class` annotations.

```das
require ue_object_types

[ue_delegate]
struct ClassQuery {
    signature:function<(value:ActorClass):ActorClass>
}

[ue_multicast]
struct ClassChanged {
    signature:function<(value:ActorClass):void>
}

[ue_interface(BlueprintType)]
class ClassContract {
    [ue_function(BlueprintCallable)]
    def EchoClass(value:ActorClass):ActorClass {}

    [ue_function(BlueprintCallable)]
    def ChangeClass(var value:ActorClass&;
                    @ue_out var previous:ActorClass&):ActorClass {}
}
```

The interface is imported before its implementation, as with other authored
interfaces. An implementation uses the same parameter and result types.
Single delegates support input/result/ref/out. Multicast delegates retain their
existing void-result and ref rules; queued `ue_event` handlers are by value.

Inside a reflected class, timer captures use ordinary typed parameters:

```das
let kind=ue_class_cast(type<ActorClass>,
    ue_class_loaded(type<ActorRef>,"/Script/Engine.StaticMeshActor"))
ue_timer_next_tick("spawn","SpawnLater",kind)
ue_timer_next_tick("spawn-count","SpawnManyLater",kind,3)
ue_timer_start("repeat",0.1f,"SpawnLater",true,kind)

[ue_function(BlueprintCallable)]
def SpawnLater(kind:ActorClass):void { /* use kind */ }

[ue_function(BlueprintCallable)]
def SpawnManyLater(kind:ActorClass; count:int):void { /* use kind and count */ }
```

Null (`default<ActorClass>`) is valid. A non-null class must derive from the
declared parent. Wrong-base, foreign/stale capabilities and obsolete reinstanced
classes are rejected. A native event may widen its class constraint to an
ancestor in the script handler; narrowing is rejected before subscription.
Native const-ref event inputs are copied; mutable native ref/out inputs are
rejected for queued delivery.

Timers and queued events hold captured UClasses through the existing reflected
GC owner, including class members of compiler-generated mixed argument frames.
Cancellation releases the capture. A rejected timer replacement leaves the
previous timer intact. A capture of a class marked as superseded is not delivered.
This uses the existing callback lifecycle and queue limits.

This addition covers standalone class arguments. Arrays/maps/sets of class
references remain outside the container policy. `UEClass<T>` continues to use a
loaded native non-interface parent type; class values themselves may also be
derived Blueprint classes.

Examples: `ue_class_callback_signals.das`, `dsl_class_callback_interface.das`,
`dsl_class_callbacks.das`. Verification: Foundation callback/factory/frame probes
and `DasForUE.Callbacks.ClassReferencesPIE`.

## Verification — 2026-10-17

Editor Development build, all 116 Python checks and Foundation (zero failures)
passed in the isolated `cccheck` source snapshot. The dedicated real PIE test
`DasForUE.Callbacks.ClassReferencesPIE` and the existing
`DasForUE.Gameplay.RuntimePIE` both passed with zero warnings/failures/skips.
Feature source files were compared with the main working tree after validation.
The dedicated scenario checks typed delegate returns/ref/out, interface class
pins and Blueprint return/ref/out dispatch, scalar/null/mixed events, scalar/null/
mixed timer captures, repeat cancellation and rejected replacement preservation.
Native probes cover class constraints, const-ref receivers, safe widening and
rejected narrowing, scalar and mixed capture GC, obsolete classes and release.

`Scripts/TestClassCallbacks.ps1` reproduces build + Foundation + dedicated PIE.
Evidence and exact snapshot hashes are in `logs/class-callbacks-validation.json`
and `logs/class-callbacks-source-hashes.json`. Packaged execution was subsequently verified; see below.

## Packaged Development — 2026-10-17

`Scripts/TestClassCallbacksPackaged.ps1` builds the exporter, saves an interface,
actor Blueprint and GUID-scoped map, then builds/cooks/stages/archives Win64
Development and launches the packaged executable. The existing packaged
Foundation smoke runs first. Both finished with PASS; Foundation had zero failures.

The game-side probe repeats scalar/null/mixed events and timer captures, typed
delegate return/ref/out, repeat cancellation and rejected replacement semantics.
It also checks the cooked interface membership after GC, retained class pin
constraints, and interface return/ref/out with native, Blueprint and null classes.
Owner cleanup leaves zero persistent contexts and timers.

The Game build exposed an editor-only FText::ChangeKey in the concurrent snapshot
regression fixture. The existing main-tree NSLOCTEXT correction was synchronized
into the checked snapshot. No runtime validation was weakened.

See `logs/class-callbacks-packaged-validation.json` for the exact map, executable
hash and source manifest, and `logs/class-callbacks-packaged.log` for individual
checks.
