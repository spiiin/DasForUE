# Component composition validation — 2026-10-09

UE 5.8.3 source build, Win64 Development, pinned daScript 0.6.4.

- Automatic ue_component composition: Create/Reopen PASS, with 12 runtime checks
  covering defaults/methods/ref outputs, lifecycle, owner signals, isolation,
  destruction and world cleanup. No manual SCS construction remains in the probes.
- Seven additional Create checks PASS: body-only import preserves generated class;
  repeated annotations share a Component class but create distinct templates;
  components survive authoring JSON transport; duplicate names are refused;
  noncomponent types are refused before Actor creation; Actor/Component package
  collision is refused; unrelated Blueprint assets remain untouched; composition
  changes are refused without recompilation. The wire roundtrip shares the repeated
  annotation check.
- TestActorComponent.ps1 -Cooked: PASS on the final runtime/macro model, using
  automatically composed, saved Actor/Component assets in the packaged executable.
  Subsequent editor-only ownership/migration guards were verified by final
  TestActorComponent.ps1 Create/Reopen checks.
- TestActorComponentPIE.ps1: one actual PIE automation test PASS, no warnings,
  failures or skipped tests, using automatically imported components.
- TestEditor.ps1: 524 PASS, failures=0.
- GenerateBindings.ps1 -Check: PASS; native IR/module unchanged.
- TestBindingsGenerator.py: 84 PASS.
- git diff --check: PASS; ThirdParty unchanged. No commit was made.

Scope: authored ActorComponent composition, standard Blueprint SCS and body-only
reload. Declaration migration for composed Actors remains blocked; SceneComponent,
attachments, typed component member access and instance-default DSL remain separate.

Evidence: logs/actor-component-Create.log, actor-component-Reopen.log,
actor-component-cooked.log, actor-component-pie.log and report, smoke-editor.log,
bindings-export.log. GUID assets remain in the ignored test project's
Content/DasActorComponent; packaged output is Build/ActorComponentPackaged/Windows.
