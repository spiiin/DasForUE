# Inline callbacks — Phase 3.6

Validated on 2026-10-20 with Unreal Engine 5.8.3, Win64 Development Editor.
Worktree: `<repo>-ui`, branch `widget-layout`.

```powershell
./Scripts/TestInlineCallbacks.ps1
# Reuse the built modules:
./Scripts/TestInlineCallbacks.ps1 -SkipBuild
```

The build selects only DasRuntime and DasEditor, with the project in this worktree.
The commandlet uses the checked-in `Tests/InlineCallbacks/probe.template.das`, writes unique
UTF-8 scratch scripts, and deletes them on exit. The runner freezes plugin source/script,
fixture, DLL and engine module-manifest hashes across the probe.
Evidence: `logs/inline-callback-runtime.log`, `logs/inline-callback-validation.json`.

**PASS: 26 targeted checks.** Also **PASS: 30 widget-layout regression checks** through
`Scripts/TestWidgetLayouts.ps1 -SkipBuild`. No full suites, PIE or packaged build were run.

The callback probe checks:

- Timer and Enhanced Input blocks compile and Apply generates the callback methods.
- Generated identities are distinct and independent of absolute filename and line shifts.
  Body changes change the identity; different Unicode string literals produce different hashes.
- Enclosing immutable/mutable locals, method parameters and loop iterators are rejected with
  the variable name and a `use timer payload` hint. Block-local variables remain valid.
- Non-void callbacks fail compilation.
- Real TimerManager delivery and EnhancedPlayerInput injection update the current Actor through self.
- Body-only reload accepts a new VM without changing callback identities. One inline timer remains
  active; one Enhanced Input recipe is restored through the existing OnWorldTickStart hook.
  Both callbacks deliver after the accepted reload.
- A forbidden capture in a reload candidate preserves the working revision and its callbacks.
  Repairing the source accepts another revision.
- Explicit scalar payload and a two-value callback frame deliver correctly. The frame timer
  also survives a subsequent compatible reload.
- An ordinary named timer is still cancelled on accepted reload, including a method whose
  name merely contains `__ue_cb_`.
- Context release removes timer records and native Enhanced Input delegate bindings.

Implementation notes:

- Normalization runs in the class annotation before type inference. It does not change the parser.
- The generated void method goes through the existing ue_function body/signature lowering.
  Its name hashes the class, containing method, managed-call ordinal and canonical block AST as UTF-8.
- AST generator headers must remain inside the runtime's existing daScript `/Zp16` include block.
- Only committed, signature-checked VM swaps may retarget inline timer records. Inline membership
  is taken from the authored method declaration, not a substring guess. Native timer handles and
  owned callback payloads remain intact; ordinary named timers and structural-reinstance cleanup
  retain their previous behavior.
- Changing a callback body or its positional identity requires Apply/reimport of the generated
  methods. This probe changes unrelated method bodies during compatible runtime reload.

The headless fixture declares its Tick lifecycle explicitly, permits reflected setup calls with
`FEditorScriptExecutionGuard`, and drives the real world-tick input reconciliation delegate.
No visible input/window interaction was tested. Startup ACL compression-setting warnings are
unchanged smoke-project warnings; the passing logs contain no engine errors or ensures.
