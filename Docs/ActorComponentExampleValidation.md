# Actor/Component validation — 2026-10-09

UE 5.8.3 source build, Win64 Development, pinned daScript 0.6.4.

- `Scripts\TestActorComponent.ps1 -Cooked`: PASS. Actor Blueprint created from
  ue_class DSL, native ScriptComponent added to SCS, defaults saved and reopened.
  Game build, cook/archive and actual packaged execution succeeded.
- Nine runtime checks PASS in Create, Reopen and packaged run: per-instance
  BeginPlay, two-world session/binding isolation, owner multicast delivery,
  Tick DeltaSeconds/state isolation, checked writes to DSL Actor fields, component
  Destroy cleanup, Actor Destroy cleanup, fresh sessions and world revocation.
  Cleanup preserves unrelated subscriptions in the already-running packaged map.
- `Scripts\TestActorComponentPIE.ps1`: PASS. One Unreal Automation test succeeded
  with zero failures/not-run/warnings. Real PIE callbacks/delta, editor-world
  exclusion, owner signal isolation, DestroyComponent/Actor and EndPIE cleanup
  succeeded. EndPlay is observed without rooting the PIE world and occurs once.
- `Scripts\TestEditor.ps1`: 524 PASS, zero failures.
- `Scripts\GenerateBindings.ps1 -Check`: PASS; current native IR/module unchanged.
- `C:\Python312\python.exe Scripts\TestBindingsGenerator.py`: 84 tests PASS.
- `git diff --check`: PASS; ThirdParty unchanged. No commit was made.

Scope: a DSL Actor plus an explicit native ScriptComponent lifecycle bridge.
Component ue_class emission, constructor/default-subobject DSL and active structural
component migration are not implemented by this stage. Existing Actor lifecycle
and last-good reload rules remain; this probe does not claim a new component
structural reload policy. Native OnScriptEnded permits observing completion after
the component script callback without keeping the play world alive.

Saved test assets are under Tests/DasForUESmoke/Content/DasActorComponent/<GUID>.
The packaged project is Build/ActorComponentPackaged/Windows. No unrelated projects
or user files were removed or changed.

Evidence: logs/actor-component-Create.log, logs/actor-component-Reopen.log,
logs/actor-component-cooked.log, logs/actor-component-pie.log and its report,
logs/smoke-editor.log and logs/bindings-export.log.
