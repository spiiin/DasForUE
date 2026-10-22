# Typed identities and state — Phase 3.9

Worktree: `<repo>-ui`, branch `widget-layout`.

Implemented without reader macros or parser changes:

```das
ue_class_default(type<ActorRef>)
ue_class_default(type<MyAuthoredRef>, concreteClass)
ue_ref(type<ActorRef>, handle)
ue_loctext(Welcome, "Welcome")
var @ue_state Counter:int = 0
```

The actual global-variable grammar puts the annotation after `var`. Persistent values are limited to int, bool, float, double and string, with constant scalar initializers. Owned snapshots are restored before candidate VM initialization; a rejected candidate never mutates the live state. Named modules give state and localized text a stable namespace. Unnamed modules use the source basename.

Native CDO access uses the loaded class. Authored references describe a contract rather than one Blueprint asset, so they require a concrete checked class handle. Null/incompatible handles are rejected. Runtime CDO access is unavailable in global initialization. Declaration defaults retain their existing meaning: the selected UClass is assigned to a class-reference property, rather than a CDO object.

Generic reference construction checks native classes, interfaces and authored contracts using existing weak handles. Native wrappers need only their annotated handle field; no per-type `_from` helper is needed. Path lint reports real quoted source tokens with known wrappers, not generated transport strings; bindings and low-level fixtures/probes are excluded. Literal binding declarations and explicit asset identities remain valid.

## Targeted validation

```powershell
./Scripts/TestIdentityDSL.ps1 -SkipBuild
./Scripts/TestReflectionMacros.ps1 -SkipBuild
C:/Python312/python.exe Scripts/CheckFormat.py --staged
```

42 identity checks, 28 reflection regression checks, and staged formatting: PASS.

- Native CDO identity, concrete authored CDO identity, missing/null/incompatible concrete classes.
- Native reference identity, null conversion and a custom wrapper without a `_from` helper; authored null conversion.
- All five scalar state kinds, per-owner isolation and Unicode strings surviving GC and reload.
- Dependent global initialization sees restored values; changed initializers preserve state; explicit session release resets it.
- Invalid source, changed state type and forbidden global CDO access preserve the old working revision and values.
- Invalid state declarations, dynamic initializers, unsupported types and malformed identity/text macros fail compilation.
- Apply writes typed class defaults, localized text CDO defaults and localized function-entry pin defaults.
- Five migrated counter/lifecycle/AI/effects/montage examples compile. Real source path literals produce the lint warning.
- Reflection regression covers generated static/instance/interface calls, ref/out, containers, struct reads and unchanged low-level frame fixtures.
- Actual foundation extern signatures, return types and side effects match the updated manifest.

Receipts: `logs/identity-dsl-validation.json` and `logs/reflection-macro-validation.json`. Runtime logs: `logs/identity-dsl-runtime.log` and `logs/reflection-macro-runtime.log`. The runners freeze SHA-256 of sources, script modules, fixtures, DLLs and engine/project module manifests around execution; the identity runner also freezes the foundation extern baseline.

Builds target only this worktree's DasRuntime, DasEditor and DasForUESmoke modules with its own `-Project` and `-NoEngineChanges`. During development a concurrent engine build changed its BuildId. After a successful local module build/check, only this worktree's ignored project module manifest was aligned to the engine's current BuildId. No shared engine manifest or other worktree was edited.

PIE, packaging and full suites were not run. These checks establish headless runtime/reload and Apply behavior, not broader gameplay or packaged-build coverage.
