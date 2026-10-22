# Hidden callbacks — Phase 3.2

Worktree: `<repo>-ui`, branch `widget-layout`.

```das
[ue_function(BlueprintCallable)]
def Start():void { ue_timer_start("pulse", 1.0f, self.Pulse, true) }
def Pulse():void { self.Score += 1 }
```

Managed `self.Method` callback arguments now promote an ordinary same-class void instance method into the existing checked callback transport. An annotation-only `[ue_function]` is supported as well. No extra source annotation is required. The compiler adds an internal marker and emits `BlueprintInternalUseOnly=true` into method metadata. These functions retain the reflected Blueprint transport needed by widget binding and runtime dispatch, while Unreal's `UEdGraphSchema_K2::CanUserKismetCallFunction` excludes them from its public palette. Explicit `BlueprintCallable` visibility is preserved. Inline blocks use the same hidden-entry helper.

The common callback-argument table covers timers, input, Enhanced Input, latent actions, async asset/soft loads, widget clicks, effects, montage and AI move handlers. Per-API signature and payload validation is unchanged. String literals are not automatically promoted. Inherited method promotion is not introduced here; methods must be declared on the same class. Lifecycle/event methods keep their own callback contracts and cannot also be managed callback targets. Unused ordinary helpers stay script-only.

The IR format is unchanged: the existing method metadata carries the internal flag. Both authoring and adapter schema validation accept only the service value `true`. Apply clears old internal metadata on graph entries and generated UFunctions before writing the new metadata, allowing explicit exposure and subsequent hiding on reimport. Structural migration retains its existing requirement that the prior Blueprint be compiled.

## Targeted validation

```powershell
./Scripts/TestHiddenCallbacks.ps1 -SkipBuild
./Scripts/TestInlineCallbacks.ps1 -SkipBuild
C:/Python312/python.exe Scripts/CheckFormat.py --staged
```

25 hidden-callback checks and 26 inline-callback regression checks: PASS.

- Plain/annotation-only callback compilation, internal metadata and IR round trip; unrelated helpers remain absent from reflected methods.
- Unknown references, non-void handlers, lifecycle conflicts, bad widget signatures, ref payloads and source-supplied compiler markers are rejected.
- Apply creates working hidden entry points; the actual Unreal palette predicate hides named and inline handlers and keeps explicitly exposed functions visible.
- Real world timers and EnhancedPlayerInput dispatch, single/multiple scalar payloads, direct script calls, and reflected dispatch of the widget handler.
- Compatible logic reload rebinds hidden Enhanced Input handlers; releasing the owner removes subscriptions and timers.
- Reapplying after adding/removing explicit `BlueprintCallable` makes the function public/hidden respectively.
- The migrated `dsl_managed_timers.das` example compiles without a handler annotation.
- Existing inline regression covers stable generated names, capture rejection, timer/input execution across accepted and rejected reloads, payloads and cleanup.

The UI-handler check exercises reflected dispatch and signature compilation; it does not simulate a Slate mouse click. Receipts: `logs/hidden-callback-validation.json` and `logs/inline-callback-validation.json`; runtime logs: `logs/hidden-callback-runtime.log` and `logs/inline-callback-runtime.log`. The hidden runner freezes plugin sources/scripts, its fixture, runner, extern baseline, local DLLs and engine/project module manifests during execution.

Only DasRuntime, DasEditor and DasForUESmoke modules were built, with `-Project` pointing to this worktree and `-NoEngineChanges`. PIE, packaging and full suites were not run. Formatting also normalizes existing widget-layout serialization blocks in the touched authoring source; their behavior is unchanged.
