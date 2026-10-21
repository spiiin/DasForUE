# Improvement plan

Engineering plan derived from the October 2026 code review. It targets three things:
less repeated work when adding a feature, user DSL code without string identifiers,
and hot-path performance. Feature work (new APIs, networking, Shipping) stays in the
[roadmap](Roadmap.md); this plan is about the foundation that feature work runs on.

Phases are ordered by dependency. Each step lists the acceptance gate that closes it.
Effort is in focused working days.

## Status

| Step | State | Notes |
| --- | --- | --- |
| 0.1 formatting | Done | `.clang-format`, `.editorconfig`, `Scripts/CheckFormat.py` (`--fix`, `--staged`). |
| 0.2 split large files | Done (textual) | `DasRuntime.cpp` and `DasDSL.inl` split into contiguous `DasRuntime*.inl`, `DasFoundationModule.inl` and `DasDSL*.inl` parts included in place; the preprocessed unit is unchanged. Turning parts into separate `.cpp` units is still open. |
| 0.3 generated registry | Done | `Private/Generated/DasNativeGameplay.generated.h`; `GenerateNativeGameplay.ps1 -Check` covers freshness. |
| 0.4 line-length gate | Done | `CheckFormat.py --staged` rejects new lines over 400 characters. |
| Extern manifest gate | Done | Foundation tests compare name, mangled signature, result type and side effects of every `ue_foundation` function with `Bindings/FoundationExterns.txt`. |
| 1.1 value kind table | Done | `DasValueKinds.h/.cpp`: one row per kind with name, actor field kind (`EDasScalarKind`) and method signature (`EDasMethodSignature`). `EDasTypeKind` is the canonical kind; the other two enums keep their persisted numbering and are mapped through the table (actor adapter, `FrameKind`, pin schema types, `DasTypeKind`/`DasTypeKindName`). Foundation test `value_kind_table_round_trips`. |
| 1.2 template accessors | Done (scalars) | `DasScalarAccessors.inl`: kind traits for int, float, double, bool, string, name, byte, text drive self, authored, native-reference, struct-leaf, call-frame, scalar struct-map, timer payload and in-place self container accessors; enum and class share the self/authored/native templates. The eight object and struct reference accessors stay hand-written on purpose: each has its own shape (expected class, line info, getter/setter snapshot path). |
| 1.3 DSL lowering | Done | Lowering compares `GetKind()` with `EDasTypeKind` instead of kind strings (`DasDSL*.inl`, `DasDelegateDSL`, `DasCallbackFramesDSL`, `DasNativeGameplayDSL`, `DasStaticLibraryDSL`). Remaining string comparisons are other vocabularies: lifecycle callback names (`BeginPlay`, `Tick`) and default literal kinds (`number`). |
| 1.4 kinds.json | Done | `Bindings/Kinds.json` is exported from the kind table and checked by foundation tests; `Scripts/DasKinds.py` feeds `GenerateBindings.py` and `GenerateNativeGameplay.py`. `GenerateApiPackage.py` has no scalar type table (it emits through `GenerateBindings.py`), so byte maps identically everywhere. |
| 1.5 daScript generics | Not planned | `ue_dsl.das` keeps three `ue_dsl_commit_struct_member` overloads (enum, object, struct; previously 11): daScript overloading already is the generic form. A generic `ue_arg`/`ue_result` would be unused until lowering emits it. |
| 2.1 adapter registry | Done | `DasTypeAdapters.h`: `IDasTypeAdapter` and `FDasTypeAdapters` (struct adapters by `UScriptStruct*`/path, kind adapters by `EDasTypeKind`); DSL views through `IDasScriptTypeFacet`. `Scalar<K>` adapters create and recognise scalar properties (`Build`, `DasPropertyKind`). Design: [TypeAdapters](../Design/TypeAdapters.md). |
| 2.2 per-kind ports | Partly done | GameplayTag and the math structs (Vector, Vector2D, Rotator, Transform, InputActionValue) are ported: identity, leaf and default policy, value validation, default kinds, handle views, member access, packing, call signatures and transports live in their adapters and script facets; per-type asymmetries (Vector2D as a struct snapshot, InputActionValue with its own bridge) are adapter data. `Scripts/TestTypeAdapterGate.py` keeps their predicates, paths and handle names inside the adapter files. Next: soft references, Text/Name, enum, object/class. |
| 3.1 method references | Done | `self.Method` covers managed callbacks, widget clicks and `ue_bind`/`BindUFunction`/`AddUFunction`/`RemoveUFunction`; delegate contracts include results and ref/out. Outside classes, `@@function` uses stable ABI wrappers and VM-generation leases (managed callbacks keep their existing ABI and current-module restriction). See [handler references](../../Guide/DSL/HandlerReferences.md) and [validation](../Validation/HandlerReferences.md). |
| 3.2 hidden callbacks | Done | `self.Method` searches authored ancestors across required modules, dispatches the most-derived override and supports `Super`. Callback targets are reflected automatically with internal Blueprint visibility; unannotated and annotation-only methods work, explicit `BlueprintCallable` stays visible. Inline callbacks share this path. `TestHiddenCallbacks.ps1`: 25 targeted checks; inline regression: 26. See [validation](../Validation/HiddenCallbacks.md) and [inherited handlers](../Validation/InheritedHandlers.md). |
| 3.3 typed input / collision names | Done | `TriggerEvent` plus literal-string warnings; project-configured `CollisionProfile` constants with checked FName conversion. See [validation](../Validation/EnumNames.md). |
| 3.4 callback operation keys | Done | Keyless named starts and `self.Method` cancel/query for timer, latent, effect and montage families; UTF-8 class/method identities; unknown literal-key lint. Explicit/dynamic keys remain available. `TestOperationKeys.ps1`: 26 targeted checks. |
| 3.6 inline callbacks | Done | `$` blocks lifted to stable UTF-8 AST/position hashes; implicit self, lexical capture rejection. Inline timer schedules/payloads transfer to compatible VMs; input recipes restore on the next world tick. `TestInlineCallbacks.ps1`: 26 focused checks. |
| 3.7 typed widget layouts | Done | `[ue_widget_layout]`, `ue_widget_view`, typed text/click overloads; Apply checks known WBP trees before mutation. `TestWidgetLayouts.ps1`: 30 focused Apply/runtime checks. |
| 3.8 reflection call macros | Done | `ue_static(Library.Function, args...)`, `ue_call(type<Reference>, Method, target, args...)`, `ue_struct_get(value, Field)`; static binding descriptors, compile-time diagnostics, existing frame transport. `TestReflectionMacros.ps1`: 28 targeted checks, plus 4 generator checks. Curried `macro(...)(...)` is not accepted by the daScript grammar. |
| 3.9 typed identities and state | Done | `ue_class_default(type<T>[, concreteClass])`, generic checked `ue_ref`, module-scoped `ue_loctext`, `var @ue_state` scalar globals restored before candidate initialization, and source-only path-literal lint. `TestIdentityDSL.ps1`: 42 targeted checks; reflection regression: 28. See [validation](../Validation/IdentityDSL.md). |
| 3.5 event checks | Done | Full native/authored source signatures use queued-property compatibility before Apply; typed owner contracts checked again at binding. Identifier annotations and declarative `BindEvents` use queued delivery; dynamic `+=` / `-=` use exact-signature, session-owned synchronous bindings with reload/Destroy cleanup. `TestEventChecks.ps1`: 51 checks. See [event validation](../Validation/OperationKeysAndEvents.md) and [dynamic operators](../Validation/DynamicEvents.md). |
| 4.1 benchmark | Done | `self_int_read_write_benchmark` in foundation tests. |
| 4.2 property access | Done | Gate measured against the 4.1 baseline (self int read+write 14.9 us): now 70–170 ns per read+write (90x or more). Registry index, cached (class, name) resolution with field paths, cached validated access contracts, in-place plain scalar access, a per-name self site cache and the extern guard's frozen-world scan cached per scope. Against the current checked path (which already includes the cached resolution) the site cache is 3–3.6x (Architecture/Performance.md). DSL-emitted slots were not implemented: they would only remove the name hash and compare. |
| 4.3 in-place writes | Done | Scalar members of authored self structs are read and written in place (no snapshot copies). Operations on self containers with scalar elements (array elements; map and set keys int/string/name/byte; map values) run in place: `push`, `push(v, at)`, `pop`, `erase`, `insert`, `clear`, `self.Items[i] = v`, `self.Scores[k] += v` (`__ue_self_array_*`, `__ue_self_map_*`, `__ue_self_set_*`). Push+pop on 1024 ints: about 0.25 us versus about 200 us for the snapshot read and write the former lowering did around every operation. Nested paths (`self.Groups[0].Scores`) and struct, enum, object and math elements keep the snapshot helpers. |
| 4.4 components and native fields | Done | Self components and object fields use the cached (class, name) lookup: object field reads 12.4 us -> 1.4 us (no value codec for reference properties, shared class path cache, handle index). Generated native field write+read 19.8 us -> 1.1 us (class lookup and field validation cached per site); authored references cache their identity check. |
| 5.1 test helpers | Done | 136 runners pass syntax/audit gates: no direct `Build.bat` calls or result parsing. Ten receipt runners are 11–17 lines; targets, arguments and negative checks are preserved. All 15 manifest script suites passed via the agent. See [Testing](../Testing.md#shared-script-helpers-and-validation-index). |
| 5.3 suite manifest | Done | `Tests/suites.json` + `Scripts/RunDasTests.ps1` (`-Preset quick|editor|full`, `-Suite a,b`, dependencies via `requires`); `TestCombined*.ps1` are wrappers; build agent jobs `test-quick`, `test-suite`. |
| 5.4 validation index | Done | `RenderValidation.py` generates [the index](../Validation/index.md) and has a non-writing `--check` gate. Suite runs regenerate/check it. Eleven renderer tests are in the passing 347-test Python suite. Manual reports remain; [removal candidates](../Testing.md#shared-script-helpers-and-validation-index) need review. |

## Phase 0 — make the code reviewable (3–4 days)

Everything later touches `DasRuntime.cpp` and `DasDSL.inl`. Today `DasRuntime.cpp` is
477 KB and changed in 44 of 59 commits; `DasDSL.inl` has lines up to 8 000 characters;
189 lines exceed 1 000 characters.

| Step | Work | Gate |
| --- | --- | --- |
| 0.1 | Add `.clang-format` (ColumnLimit 120, Unreal style) and reformat `Plugins/DasForUE/Source` in one mechanical commit. Exclude generated files. | Build + `TestCombined.ps1` unchanged; diff is whitespace-only (`git diff -w` empty). |
| 0.2 | Split `DasRuntime.cpp` by subsystem: extern registration, self access, objects/handles, structs/maps, timers/callbacks, text/names, gameplay services. Turn the included `.inl` files into `.cpp` + private headers. | No file > 100 KB; no behavior change; build time not worse. |
| 0.3 | Move `DasNativeGameplay.generated.h` (891 KB) out of hand-written sources into a `Generated/` folder; keep it checked by a `-Check` freshness run as `ue_generated.das` already is. | Freshness check fails on a stale header. |
| 0.4 | Add a `.editorconfig`, and a pre-commit check for line length in new code. | CI/local hook rejects new 1000+ character lines. |

## Phase 1 — one source of truth for value kinds (1.5–2 weeks)

About 250 extern functions are hand-written points of one matrix (kind × operation ×
container): `ue_arg_*`, `ue_result_*`, `ue_get/set_self_*`, `ue_struct_with/get_*`,
`__ue_native_get/set_*`, `__ue_authored_get/set_*`, 72 `__ue_struct_map_<key>_<value>`,
11 `ue_dsl_commit_struct_member_*`. Kinds are listed in three C++ enums
(`EDasScalarKind`, `EDasMethodSignature`, `EDasTypeKind`), 444 `Kind==TEXT("...")`
comparisons and separate tables in each Python generator; copies already diverge
(byte is `uint8` in `GenerateBindings.py`, `int` in `GenerateNativeGameplay.py`).

| Step | Work | Gate |
| --- | --- | --- |
| 1.1 | `DasValueKinds.h`: X-macro table `DAS_VALUE_KINDS(X)` with C++ type, FProperty class, kind name, daScript type; `DasKindTraits<K>` with `Read/Write/ToDas/FromDas/Validate`. One `enum class EDasKind`; `KindByName()` replaces string comparisons. | All three old enums map to `EDasKind`; unit test round-trips every kind. |
| 1.2 | Template wrappers (`das_get_self<K>`, `das_set_self<K>`, `das_arg<K>`, `das_result<K>`, `das_struct_with<K>`, `das_map_get<KK,VK>`) registered by loops over the table in `DasValueExterns.cpp`. Keep the existing public names as generated aliases. | Extern name set before/after is identical (test compares the registered list); `TestCombined.ps1` passes. |
| 1.3 | Replace the ternary chains in DSL lowering (`DasDSL.inl` around lines 280/288, `DasDelegateDSL.inl:60`) with `Traits(kind)` lookups. | No remaining `Kind==TEXT(` in lowering code. |
| 1.4 | Export the table as `kinds.json` via `DasExport`; `GenerateBindings.py`, `GenerateApiPackage.py`, `GenerateNativeGameplay.py` read bridge names and types from it. | Generator tests pass; byte maps identically in all generators. |
| 1.5 | daScript side: generic `ue_arg(frame, name, value:auto)` / `ue_result(...)`, one generic `ue_dsl_commit_struct_member`, generated from the table. | `ue_dsl.das` has no per-type commit helpers. |

Payoff: adding a scalar kind becomes one table row plus a traits specialization
instead of ~25 externs and edits in every generator.

## Phase 2 — value adapters instead of an N-subsystem matrix (2–4 weeks, incremental)

Adding FText/byte (commit b82a254) touched 11 subsystems; GameplayTag (6038cec)
touched 34 files with the struct path hard-coded in three places. Special struct
predicates are scattered (`DSLMathPath` 76 calls, `DasSoftProperty` 25, `DSLSoft` 28,
`DasIsGeometryType` 13).

| Step | Work | Gate |
| --- | --- | --- |
| 2.1 | Define `IDasTypeAdapter`: `Match`, `MakeProperty`, `MakePin`, `ParseDefault/WriteDefault`, `Migrate`, `Marshal/Unmarshal`, `SnapshotKind`, `LowerDSL`, `Validate`, `GeneratorInfo`. Registry keyed by type descriptor. | Interface + registry merged with one adapter (Scalar<K> from Phase 1). |
| 2.2 | Port one value kind at a time: GameplayTag first (newest, smallest), then Math structs, Soft refs, Text/Name, Enum, Object/Class, Interface. Each port deletes the scattered branches for that kind. | Per kind: no remaining hard-coded path literal or predicate outside its adapter; full regression passes. |
| 2.3 | Containers as combinators over the element adapter (array/map/set), replacing per-shape branches and the 72 key×value map externs. | New element kind works in all containers without container-specific code. |
| 2.4 | Generate the support matrix in `Docs/Reference/TypeSupport.md` from the registry. | Doc regenerated by a script; drift check in tests. |

## Phase 3 — DSL without string identifiers (2–3 weeks)

User scripts still name methods, triggers, widgets, events and paths by string.
The `[ue_class]` visitor already validates many literals at compile time
(`RewriteTimer`, `RewriteEnhancedInput`, `RewriteLatent`, `RewriteEffect`,
`RewriteMontage`, `RewriteAssetLoad`, `RewriteAIMove`, `RewriteInput`), so most of
this is accepting typed expressions in the same places. Old string forms stay
accepted for compatibility and get a deprecation warning.

| Step | Work | Gate |
| --- | --- | --- |
| 3.1 | **Method references.** Accept `self.Method` (`ExprField` to a class method) wherever a handler-name literal is accepted today (~160 sites: timers, input, latent, effects, montage, async load, AI move). Add `ue_widget_bind_click`, `BindUFunction/AddUFunction/RemoveUFunction` and `ue_bind` to the visitor (today runtime-only). Outside classes accept `@@function`. | `dsl_playable_character.das` contains no handler strings; a misspelled method is a compile error. |
| 3.2 | **Hidden exports for callbacks.** A method used only as a callback no longer needs `[ue_function(BlueprintCallable)]`; the macro emits a private exported entry point. | Handlers disappear from the Blueprint palette unless marked. |
| 3.3 | **Enums instead of strings:** `TriggerEvent` for Enhanced Input; collision profile names as generated constants. | `ue_enhanced_bind(a, TriggerEvent.Started, self.OnJump)` compiles; string form warns. |
| 3.4 | **Handles/keys:** timer/effect/montage/latent APIs default the key to the callback (`ue_timer_cancel(self.Pulse)`) or return a nominal handle stored in a field. Visitor warns on cancel/query of an unknown literal key. | No silent `false` from a misspelled key in examples. |
| 3.5 | **Events checked at compile time.** In class `finalize`, resolve `[ue_event(source=..., name=...)]` against the component's type (authored model or UClass) and check the multicast signature. Allow identifiers (`source=Health, name=Changed`) and `self.Health.Changed += self.OnHealthChanged`. | Wrong event name or signature fails Apply, not PIE. |
| 3.6 | **Lambdas as callbacks:** the macro lifts a block into a generated method `__ue_cb_<hash>`, `self` captured implicitly, other captures forbidden (use timer payload). | Timer and input examples work with inline blocks across hot reload. |
| 3.7 | **Typed widget layouts:** `[ue_widget_layout] struct HUDView { Score:TextBlockRef; Damage:ButtonRef }` resolved once by `ue_widget_view(type<HUDView>, hud)`; validated against the WBP when the class is known at Apply time. | HUD example has no widget-name strings. |
| 3.8 | **Call macros over reflection:** `ue_static(GameplayStatics.GetTimeSeconds)`, `ue_call(type<IFace>, Query, target, 5)`, `ue_struct_get(s, Count)`; low-level frames stay for tests. | Fixture scripts for frames keep working; user docs show only the macro form. |
| 3.9 | **Paths and the rest:** `ue_class_default(type<T>)`, generic `ue_ref(type<T>, h)` instead of per-type `*_from` with duplicated paths; `ue_loctext(Key, "text")` with namespace from the module; `var @ue_state Name:Type` instead of `ue_state_set_int("key")`; lint warning for a `"/Script/..."` literal that has a wrapper. | Typed forms in user examples; source-only warning when a literal has a loaded wrapper. Binding declarations, asset identities and low-level fixtures retain literal paths. |

## Phase 4 — hot-path performance (1–1.5 weeks; can start after Phase 0)

`self.Score += a` lowers to `ue_set_self_int("Score", ue_get_self_int("Score") + a)`;
each call converts UTF-8, builds an `FName` and walks the property chain
(`ResolveProperty`, DasRuntime.cpp ~572: "Resolve again for every call"). A struct
member write copies the whole struct twice.

| Step | Work | Gate |
| --- | --- | --- |
| 4.1 | Benchmark first: extend `DasPerformanceProbe` with Tick-like scenarios (N actors × field reads/writes, struct member writes, container push). Record baselines. | Numbers in `Docs/Dev/Architecture/Performance.md`. |
| 4.2 | Slot-based access: the macro emits `__ue_self_int(slot)`; per-class slot table from the authoring model; runtime cache `(UClass*, slot) → FProperty*/offset` invalidated on reinstancing/reload epoch. | ≥5× faster field access in the benchmark; reload/reinstancing tests pass. |
| 4.3 | In-place struct member and container writes via `ContainerPtrToValuePtr`, without snapshots. | No snapshot copy for scalar member writes; read-only and validation rules unchanged. |
| 4.4 | Same cache for component access (`self.Camera`) and native self fields. | Benchmark shows no name lookup on the hot path. |

## Phase 5 — tests and validation as data (1 week + 2 days)

There are 52 `*Commandlet.cpp`, 18 `*Probe.cpp`, 10 `*PIE.cpp`, 126 `Test*.ps1`; the
Build.bat block is repeated in 115 scripts, `_RESULT=PASS` parsing in 103, and 20
features exist as near-identical `*Asset.ps1`/`*PIE.ps1` pairs. 69 files redefine the
same `Check` lambda.

| Step | Work | Gate |
| --- | --- | --- |
| 5.1 | `Scripts/DasTest.psm1`: `Invoke-DasBuild`, `Invoke-DasCommandlet`, `Assert-DasResult`, `Write-DasValidation`, `Lock-SourceHashes`. Migrate scripts gradually. | New scripts are ≤ 20 lines; old ones migrated without behavior change. |
| 5.2 | One `UDasTestCommandlet -Suite=<name>` with `DAS_TEST_SUITE(Name)` and a shared `FDasCheck`; or Automation Spec where PIE is involved. | New feature test = one suite file. |
| 5.3 | `Tests/suites.json` (suite → fixture `.das`, modes `editor|asset|pie|packaged`) and one `RunDasTests.ps1 -Suite X`; `TestCombined.ps1` reads the same manifest. | Full regression from the manifest. |
| 5.4 | `Scripts/RenderValidation.py` builds `Docs/Dev/Validation/index.md` from `logs/*.json`; stop hand-writing `*Validation.md`. | Index regenerates and its freshness check passes; manual reports remain until review of the proposed removal candidates. |

## Phase 6 — generator consolidation (3–5 days, after 1.4)

Three generator chains (`GenerateBindings.py`, `GenerateApiPackage.py`,
`GenerateNativeGameplay.py` + `DiscoverNativeGameplay.py`) with their own type tables
and PowerShell wrappers; probe code embedded as JSON strings in `*.profile.json`;
~1 MB of generated summaries committed.

| Step | Work | Gate |
| --- | --- | --- |
| 6.1 | Shared `Scripts/dasgen/` package: kinds from `kinds.json`, one IR loader, one writer. | Single type table in Python. |
| 6.2 | NativeGameplay becomes a package profile instead of a separate generator. | Output identical to today's `ue_native_gameplay.das`. |
| 6.3 | Probe code moves to `.das` files next to profiles; summaries generated in test runs, not committed. | No `"probe"` strings in JSON. |

## Documentation upkeep

- Fix stale internal links in moved `Docs/Dev/*` pages.
- Verify the facts flagged during the docs merge: automatic input restore after
  reload, Source catalog status, maps with math/struct/reference values and Vector2D,
  `int64` as an authored type, unchecked examples for the staged Pawn/AI and
  Animation/Audio/Niagara packages.
- Compile every code block in `Docs/Guide` as part of the test suite (extract to
  temporary `.das` files and run the syntax check).

## Suggested order

1. Phase 0 (prerequisite for clean diffs).
2. Phase 3.1–3.3 and 4.1 in parallel: biggest user-visible win at low cost, plus baselines.
3. Phase 1, then 4.2–4.4 (slots reuse the kind table).
4. Phase 5.1 early as scripts are touched; 5.2–5.4 after Phase 1.
5. Phase 2 kind by kind, starting with GameplayTag; Phase 6 after 1.4.
6. Remaining Phase 3 items as the adapters land.
