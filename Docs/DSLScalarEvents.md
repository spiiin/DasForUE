# Scalar and no-argument dynamic multicast events

Added 2026-10-10. **Not built or tested**. UHT/build, importing the example,
Blueprint broadcasting, PIE, reload and packaged runs have not been executed.

```das
[ue_event(source="self",name="OnDasStringSignal")]
def OnLabel(value:string):void {
    self.Label = value
}
[ue_event(source="self",name="OnDasVoidSignal")]
def OnPulse():void {
    self.Pulses += 1
}
```

The source signature selects int, float, bool, string or void subscription adapters.
Methods return void; one argument is by value, with no out/ref/default. A void
event has no arguments. Existing self/owner source rules apply; owner is available
only to authored ActorComponents. Direct bind_events code can use the corresponding
ue_subscribe_object_int/float/bool/string/void entry with a checked source handle.

These are BlueprintAssignable reflected dynamic multicast properties. Numeric
types are exact: float32 is distinct from double. FString accepts value or const
input-reference forms; mutable out/ref and return values are refused. Native C++
delegates, single-cast, multiple arguments, UObject/struct payloads and delegate
declarations in the user DSL remain separate work.

The same subscription declaration, proxy, ID, installation, queue and teardown
path is used for all supported types. Runtime-only callback kind is stored with
the subscription. Candidate validation checks handler signatures before accepting
new code. Each proxy has a reflected Receive function for the matching signature;
unsubscription removes that exact bound function. Existing int entrypoints remain
as adapters into the shared implementation.

Queued strings own their FString storage until delivery, then are copied into
the active DAS context. No FString address crosses into scripts. Inputs longer
than 4096 TCHARs or containing NUL are dropped; non-game-thread proxy invocations
are dropped. Queue and recursive dispatch retain a 1024-event bound. bind_events
has a 256-declaration cap; duplicate declarations are rejected.

Revoked proxy IDs and mismatched payload kinds do not dispatch. EndPlay/session
release, source expiry, owner teardown, world cleanup, accepted reload and editor
reinstancing use existing subscription lifecycle handling. Failed candidate reload
retains old bindings. Callback failures use existing error reporting; they do not
automatically unsubscribe (unlike the initial timer adapter).

Callback model JSON gains optional parameter_kind for non-int events. Missing kind
means legacy int; int serialization stays unchanged. Kind participates in callback
structural comparison. This extends the callback model, not general migration:
unsupported structural transitions still follow existing refusal policies.

Example `dsl_scalar_events.das` uses new native probe-host signals and Blueprint
EmitDasFloatSignal/EmitDasBoolSignal/EmitDasStringSignal/EmitDasVoidSignal methods.
These host members provide a reproducible source for later validation; subscribing
to compatible existing native/Blueprint delegate properties uses the same adapter.

Pending checks: UHT/proxy signature compatibility (especially const FString&);
all payloads and exact type rejection; handler compile errors; source owner/self;
UTF-8 and queued lifetime; size/NUL/queue caps; recursion; duplicate registration;
legacy JSON parity; accepted/rejected reload; stale IDs; GC/EndPlay/world cleanup;
import/reopen, real PIE and packaged Development.

Double/Vector/native UObject payload transport was added subsequently, without
build/tests; see DSLValueEvents.md for its exact signature and lifetime boundaries.
