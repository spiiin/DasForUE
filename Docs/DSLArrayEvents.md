# Name and primitive array event payloads

Added 2026-10-10. **Not built or tested**. No UHT/build, proxy binding, source
import, runtime dispatch, PIE or packaged execution was run for this stage.

```das
[ue_event(source="self",name="OnDasArrayDoubleSignal")]
def OnWeights(values:array<double>):void {
    self.Weights := values
}
```

Added native FName by-value events (source UENameValue), plus native const-input
reference TArray<int32>, TArray<double> and TArray<FString> events (source by-value
array<int/double/string>). Native mutable out/ref and by-value array signatures
remain refused in this slice. Multiple arguments and additional container/element
types remain separate work.

Array adapters check exact element/property identity while declaring subscriptions.
Each subscription records its expected snapshot carrier path; mismatched queued
array shapes cannot reach its handler. The native proxy copies an incoming array
into the existing owned carrier. Raw UEStructValue callback transport is unpacked
into an owned DAS array at the start of the generated source callback. Whole field
replacement uses ordinary clone assignment; no array address escapes Unreal.

Existing aggregate snapshot bounds apply (4096 elements / 16 MiB counted storage).
String arrays additionally allow at most 4096 TCHARs per string, 1048576 TCHARs
total, and no embedded NUL. These checks occur before the native copy and at queue
entry. Out-of-bounds payloads are dropped. Snapshot counted storage does not claim
to measure all allocator overhead or arbitrary string heap memory.

FName values use the existing context-local name token adapter, preserving Unreal
name semantics. No conversion to a user-facing string is required.

Event callbacks now enter an execution-depth scope and collect temporary struct
snapshots on exit, like ordinary persistent dispatch. This is intended to support
checked nested authored method calls and avoid retaining temporary snapshots over
successive event deliveries; both behaviors remain pending runtime verification.

Example: dsl_array_events.das. The probe host provides OnDasNameSignal and
OnDasArrayInt/Double/StringSignal plus Blueprint-callable emit functions. Queue,
weak owner/source, reload and teardown behavior uses the shared subscription path.
Callback metadata records name/array_int/array_double/array_string. Event signature
changes do not introduce broader structural migration support.

Pending checks: UHT and const-array signature compatibility; name ABI/case/number;
exact element rejection; empty/multi-element arrays; owned copy and string UTF-8;
limits and invalid data; repeated changed payloads and snapshot collection; nested
authored method calls; readonly/clone writes; old callback regressions; queued
lifetime, reload, EndPlay/world cleanup, PIE and packaged Development.
