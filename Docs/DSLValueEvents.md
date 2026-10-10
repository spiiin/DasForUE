# Double, Vector and native object event payloads

Added 2026-10-10. **Not built or tested**; UHT, proxy compatibility, typed callback
lowering, GC, example import, PIE and packaged execution have not been verified.

```das
[ue_event(source="self",name="OnDasObjectSignal")]
def OnTarget(value:ObjectRef):void {
    self.Target = value
}
[ue_event(source="self",name="OnDasVectorSignal")]
def OnPosition(value:UEVector):void {
    self.Position = value
}
```

The existing managed dynamic multicast bridge now additionally selects:
- double64, without reducing it to float32;
- by-value FVector, copied into the owned queued payload;
- native UObject* values, converted to typed native object wrappers in source DSL.

Object callback signatures must use a typed native object wrapper, e.g. ObjectRef.
Authored class views, native interface wrappers and raw UObjectHandle callback
parameters are refused. The source wrapper lowers to UObjectHandle transport plus
checked unpacking inside the generated callback. A more specific native source
wrapper may narrow an incoming UObject and will reject incompatible live values.

The **native delegate property** must declare exactly UObject*, not AActor* or
another derived class. The reflected proxy's UObject parameter is not assumed
signature-compatible with more specific object properties. Those signatures need
dedicated proxy adapters. Vector currently accepts a by-value parameter; const
reference Vector events remain outside this slice. String retains its prior
const-reference support. Multiple parameters, Rotator/Transform, enum/struct and
container payloads remain separate work.

An object queued for delivery is held by TStrongObjectPtr until its event record
is discarded/delivered. The DAS wrapper retains the existing weak-handle semantics;
native reflected properties retain ordinary UE ownership. Null or expired/destroyed
objects use the existing MakeObjectHandle behavior. A wrong source-wrapper class
is an error in checked unpacking, rather than an unchecked pointer cast.

Non-finite FVector inputs are dropped before queuing. The queue's existing event
count limit, string limit and game-thread rule apply. IDs, callback validation and
owner/source lifetime cleanup use the same implementation as scalar events.

Model parameter_kind records double/object/vector; object wrapper details remain
in source and its generated converter. Changing the parameter kind participates
in callback structural comparison and does not imply a new migration policy.

Example `dsl_value_events.das` uses the probe host's BlueprintAssignable
OnDasDoubleSignal/OnDasObjectSignal/OnDasVectorSignal and Blueprint-callable emit
methods. No new DSL event declaration syntax is introduced.

Pending checks: build/UHT; all native and DAS ABI signatures; null/expired objects;
GC between queue and delivery; narrowing refusal; more-specific native delegate
refusal; vector lifetime/invalid input; double precision; reentry; callback failure;
old scalar/int regression; reload/EndPlay/source cleanup; PIE and packaged behavior.

Subsequent extension: Rotator/Transform, math const-reference proxies and
interface/authored source views of UObject payloads (all validation pending);
see DSLMathReferenceEvents.md. Earlier exclusions above describe the initial slice.
