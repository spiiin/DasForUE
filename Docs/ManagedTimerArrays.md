# Captured primitive arrays in class timers

Current cross-surface support and shared callback policy:
[TypeSupportMatrix.md](TypeSupportMatrix.md). All eight primitive array families
are supported by authored class timers; the lists below describe earlier stages.

Current FText/uint8 extension and integration evidence:
[ManagedTextByteTimers.md](ManagedTextByteTimers.md). The initial validation notes
below describe the earlier implementation stage.

Added 2026-10-10. **Not built or tested**. Source import, callback ABI, copying,
timer execution, limits, reload, PIE and packaged behavior have not been run.

```das
var values:array<int>
push(values,5)
ue_timer_start("score",1.0,"Apply",true,values)

[ue_function(BlueprintCallable)]
def Apply(var values:array<int>):void {
    for value in values { self.Score += value }
    clear(values)
}
```

The fifth argument now supports array<int>, array<double> and array<string> for
annotated class callbacks. The callback has one by-value argument (var permits
local mutation; there is no &/ref/out). Source lowering packs the supplied array
through its exact existing snapshot converter and invokes an internal timer adapter.
The callback uses the class method's existing UEStructValue transport entry.

The timer stores an owned native FInstancedStruct copy, independent of the starting
script's array and context-local token. Each delivery creates an independent native
frame and unpacks an owned DAS array. Editing the caller's original array or clearing
the callback-local array does not update the saved capture. Replacement by name
updates the capture explicitly. This behavior is implemented but unverified.

Timer start checks the supported exact carrier, snapshot limits and callback class
contract before replacing the previous timer. String arrays require at most 4096
TCHARs per string, 1048576 TCHARs total and no NUL. Existing 4096-element / 16 MiB
counted snapshot storage bounds apply. Counted storage is not total heap overhead.

Only primitive array carriers are accepted. No object reference pinning rule is
needed for this slice; object/enum/struct/math arrays, maps/sets and nested captures
remain separate. Runtime call contract includes the exact array carrier identity.
Plain exported DAS array callbacks are not transparently lowered by this class DSL
feature; the ergonomic form currently requires an annotated ue_class method.

Existing timer owner/world scoping, dispatch queue, callback error cancellation,
EndPlay/release cleanup and accepted/rejected reload policies remain unchanged.
Accepting reload destroys captures with their old timers. Previous required-entry
contracts remain registered until session release, as in the existing timer API.

Example: dsl_timer_array_arguments.das. It includes caller mutation after start,
repeating int delivery with local mutation, and one-shot double/string arrays.

Pending checks: source lowering and parameter const/var behavior; exact ABI/carrier;
initial/callback/caller copy independence; repeated delivery; empty and limit-sized
arrays; string UTF-8/limits/NUL; wrong callback and failed replacement; errors,
reentry and snapshot collection; old timer regressions; reload/teardown; real PIE
and packaged Development. No tests or generation were run for this stage.
