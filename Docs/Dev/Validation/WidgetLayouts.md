# Typed widget layouts — Phase 3.7

Validated on 2026-10-20, Unreal Engine 5.8.3, Win64 Development Editor.
Worktree: `<repo>-ui`, branch `widget-layout`.

```powershell
./Scripts/TestWidgetLayouts.ps1
# Reuse an already built editor:
./Scripts/TestWidgetLayouts.ps1 -SkipBuild
```

The script derives its project path from its own worktree, uses `-WaitMutex` for builds,
and runs only `DasWidgetLayouts`. It freezes source/script/DLL hashes and the engine module
manifest across the probe. Local evidence is written to `logs/widget-layout-validation.json`
and `logs/widget-layout-runtime.log`.

Result: **PASS, 30 checks**. C++ formatting of the changed files and `git diff --check` also pass.

Covered:

- Layout declaration and `self.Method` in the typed button overload compile; invalid field types and unknown callbacks fail compilation.
- Layout contracts and HUD class origins survive authoring IR JSON round-trip.
- Apply accepts a matching compiled WBP tree and rejects missing `Scor` and a `Score:ButtonRef`
  where the WBP contains `TextBlock`, with field/expected/actual diagnostics.
- Rejection creates no target Blueprint; a rejected re-import preserves the existing generated
  class, Blueprint status and clean package flag.
- Both `self.HUD` from an authored HUDClass default and a local HUD created from a literal
  WBP class are checked at Apply. Dynamic origins fall back to runtime validation.
- A real Widget Blueprint is instantiated in a headless Game World with a valid local player
  context and NullSlate. The receiver expression runs once; text updates and button binding work.
- Runtime rejects an incompatible view with the expected/actual WBP types.
- Removing the HUD unbinds the button; view/cache references do not prevent HUD and child GC.
  A stale field access is rejected.

The headless fixture registers its PlayerController explicitly and supplies its World through
`FLocalPlayerContext`. `FEditorScriptExecutionGuard` permits the test's reflected native setup
call before actors are initialized. Runtime ownership checks remain enabled.

No full suites, visible UI interaction, PIE, packaged build or dedicated reload probe were run.
The existing managed button lifetime path is reused. Four startup warnings concern missing
ACL compression settings in the smoke project; there were no engine errors or ensures.
