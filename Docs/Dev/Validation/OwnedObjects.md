# Owned script objects — 2026-11-02

Branch `codex/owned-uobjects`, based on main `8f1551f`. Validation ran in an isolated worktree. Transfer to main preserves unrelated
work; a build of the combined main tree is a separate check.

Build agent job: `owned-objects-20261102b`, exit code 0. Editor Development build
passed; the focused `DasForUE.Runtime.OwnedObjects` automation test passed with
one success, zero warnings and zero failures (test body 1.355 seconds). Existing
build warning C4305 in the unrelated `DasCompleteBuiltinValues.inl` remains.

The scenario imports the three `owned_object_*.das` examples and checks:

- Instanced property flags, descriptor JSON round-trip and rejection on a scalar.
- A configured inline object survives Blueprint recompile and automatic reimport.
- Two owners receive distinct template instances with the correct Outer.
- DAS factory creation from template and class defaults; explicit `Initialize(owner)`
  executes once and leaves the original template uninitialized.
- A reflected array retains the live action through GC. Clearing that array lets
  GC collect the action although Outer remains alive, releasing its DAS context.

The first run found implicit lifecycle-name classification incorrectly reserving
`Initialize` on ScriptObject. Plain script objects now have ordinary methods with
these names; Actor/component/subsystem lifecycle behavior is retained.

All previous Foundation extern contracts remain; one checked template-factory
extern was added. Source hashes are recorded in `logs/owned-objects-validation.json`
and the automation report is in `logs/owned-objects-report/index.json`.

Scope: concrete ScriptObject descendants in an active game world; direct strong
Instanced fields. Instanced containers remain explicitly unsupported. Full PIE,
save/reopen in another editor process, packaged Development and Shipping/AOT were
not run. No networking changes.
