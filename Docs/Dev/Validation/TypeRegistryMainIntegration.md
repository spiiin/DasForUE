# Type registry integration with main

Integration base: `main` at `ebc9c31`. Incoming branch: `refactor/soft-type-adapters`
at `27ad5fc`, including the earlier runtime/editor probe separation and staging work.
Integration worktree: `C:/src/DasForUE2-integrate-types`.

## Reconciliation

- Keep production `ADasActor`, explicit test-only `ADasProbeActor`, WorldSubsystem
  and GameInstanceSubsystem hosts together.
- Retain main's script-path cache and write notifications, with runtime-only
  generated modules as the fallback for projects without test opt-in.
- Keep native binds, built-in operators, diagnostics and subsystem functionality.
  Move their new test commandlets/probes and test AOT code into `Tests/Editor`
  and `Tests/Runtime`; keep production native API and diagnostics in `Source`.
- Resolve relocated enum annotations through the enum facet while retaining
  source diagnostic scopes. Native FName/FText naming uses the registry facet.
- Use the main AOT call-site cache for erased interop too. Registration updates
  the shared generation, and role/context guards execute before cache lookup.
  Erased interop retains its scalar-only AOT boundary independently of broader
  native-bind handle support.
- Give merged AOT scenarios distinct mode numbers; retain all 33 linked checks
  and a fast regression against future collisions.
- Include `ue_subsystems.das` in the explicit runtime script manifest.

## Confirmed evidence

- `type-main-bootstrap`: Editor built; live export produced. Foundation's only
  initial rejection was the intentionally stale support baseline.
- Matrix refreshed from that export: 40 adapters, 43 representative
  descriptors and 22 contextual gates. Extern and kind manifests
  retain the combined production contracts.
- `type-main-core`: focused SaveGame compatibility, DSL diagnostics, complete
  built-ins, Quat/operators, subsystem and subsystem PIE checks passed.
- `type-main-core4`: linked AOT (33 checks), fresh export/drift, Python,
  Foundation, native binds/manifest, interfaces, native/merged/computed defaults,
  execution roles, delegates and event checks passed (exit 0).
- Final Python rerun: all 394 tests passed, including unique AOT scenario IDs and canonical transport fixture types.
- Source distribution audited successfully: 768 allowlisted files; test sources,
  test scripts and diagnostic artifacts excluded.
- Staging, Geometry, input events and EventReply passed. Paint exposed a stale
  pre-canonical LinearColor fixture; its correction and the final UI/boundary
  results are recorded below when completed.

No changes were applied to the main checkout during conflict resolution.
All required combined checks passed before the final local main fast-forward.

## Issues caught by combined verification

- Ordinary Game linking exposed `DasCallPhaseState` implemented inside the test-only
  benchmark include. Move the state and phase names to `DasCallPhases.cpp` in
  production runtime; the benchmark remains test-only. Editor and ordinary Game
  both build after this correction.
- Paint/collision fixtures still referenced a generated LinearColor struct after
  main introduced canonical `UELinearColor`. Use `ue_linear_color` and add a
  regression against the stale generated name. Both transport suites now pass.
- `type-main-final` finished with exit 0: Editor build, ordinary Game isolation,
  Paint, collision, 21 widget-event checks, component preparation, method-value
  Create/Reopen and all 59 Editor/PIE automation tests passed. The automation
  report contains zero failures, warnings or skipped tests.
- Clean source distribution: 768 allowlisted files. The extracted example built
  for Editor and Game; gameplay preparation and editor-game verification passed.
  Cook/stage/pak and the packaged executable passed with a rendered screenshot.
  Both staged manifests exclude tests and diagnostic artifacts.
- Ordinary Editor without test opt-in built and started successfully; UHT and
  compiler response audits exclude test Runtime/Editor sources. The final run
  validated all 18 available receipts and reports `DAS_SUITES=PASS`.
- Final formatting, whitespace, adapter gate, generated runtime bindings, matrix
  drift and guarded-extern audit (617 registrations) passed.

## Result

All four registry extensions (authored signatures, callbacks, defaults and generated
contextual support matrix) are integrated with the existing main functionality.
The clean runtime/editor boundaries and production-only staging were verified
against the merged code, including a standalone packaged game.

Evidence is local under `logs/` and the shared agent jobs `type-main-core4` and
`type-main-final`; artifacts/logs are ignored and are not included in the commit.
This validates the combined implementation on UE 5.8.3 Win64 Development. It does
not grant type support outside the documented contextual contracts.
