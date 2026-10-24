# Binding performance probe

`Scripts/TestPerformance.ps1` runs an opt-in benchmark in a dedicated process.
Normal runtime operations gain no profiling counters or allocator hooks. The probe
reads existing VM heap, snapshot path and key-order counters and uses the native
copy-counted snapshot fixture. It refuses to run alongside persistent sessions.

```powershell
./Scripts/TestPerformance.ps1
# After building the current Editor:
./Scripts/TestPerformance.ps1 -SkipBuild -Iterations 64 -Samples 9
# Inspect native allocations with Unreal Insights, in a separate run:
./Scripts/TestPerformance.ps1 -SkipBuild -TraceMemory -LogPrefix performance-trace
# Use an already cooked/staged current smoke game:
./Scripts/TestPerformance.ps1 -PackagedExe C:/path/to/DasForUESmoke.exe
# The existing packaging runner can prepare a new smoke game first:
./Scripts/TestPackaged.ps1 -SkipZenStore
./Scripts/TestPerformance.ps1 -PackagedExe ./Build/Packaged/Windows/DasForUESmoke/Binaries/Win64/DasForUESmoke.exe
```

The packaged executable must come from the current smoke project and contain
`performance_probe.das`. The runner does not silently reuse or package an old game.
Reports and logs use `logs/<prefix>-editor.*` or `logs/<prefix>-packaged.*`.
The commandlet is `-run=DasPerformance`; the game flag is `-DasPerformance`.
Both accept `-PerfIterations=64 -PerfSamples=9 -PerfOutput=<absolute-json-path>`.

## Workloads and interpretation

Each container size (16, 256, 1024) has eleven cases:

- Native nested field access, preparing a fresh path each time versus a warmed
  path cache. Both execute the same resolver and array bounds checks.
- A script loop baseline with one native snapshot capture.
- Script size/get/contains/nested-field queries.
- Sorted integer map key iteration through the prepared order.
- One detached map edit per iteration and four sequential detached edits.
- The same four map edits inside one scoped edit block with one root clone.
- Script-to-UE checked reflected `AddHealth` calls.
- UE-to-script repeated public persistent scalar dispatch.
- A VM array allocation positive control.

Every script entry is warmed twice before timing. Each measured batch checks its
result checksum and expected real root-copy count, and verifies that its compiled
context was not replaced. The native path comparison checks the read value, zero
root copies and warmed lookup reuse. Aliasing and invalidation have separate
regression coverage in SnapshotFieldPaths.md. A separate real
native clone proves that its copy counter works. The VM array case must register
an allocation or reallocation. Persistent sessions and temporary property flags
are cleaned up at the end.

Default results contain nine samples, median, nearest-rank p95 and normalized
nanoseconds per **work iteration**. A read iteration has several accessor calls;
the four-edit iteration has four edits. Native path batches have 64 times more
iterations to reduce timer-resolution noise. Public UE-to-script dispatch includes
source checking and boundary cleanup; it is not a raw VM trampoline measurement.
There are no timing pass/fail thresholds. This is a current-build comparison of
specific operations, not an old-versus-new binding benchmark or an AOT benchmark.

## Allocation and memory scope

`vm_allocations`, `vm_reallocations` and `vm_allocated_bytes` are deltas of
daScript heap plus string-heap cumulative counters. They do **not** count native
`FString`, property-frame, map/set backing-store or snapshot buffer allocations.
`native_root_copies` counts actual `FDasSnapshotReadProbe` copy assignments only.
`vm_live_bytes_after` and `snapshot_table_bytes_after` describe after-call live VM
storage and native snapshot-table capacity, not peak or total process memory.
Zero VM allocations must never be interpreted as zero native allocations.

`-TraceMemory` writes a `.utrace` using Unreal's memory trace. Its CPU scopes are
named `DasPerformance/<case>/size_<n>/sample_<n>` and surround measured operations;
JSON construction occurs outside those scopes. Open the trace in Unreal Insights
and restrict native allocation analysis to a case's interval. Startup/warmup
allocations are present elsewhere in the trace. Tracing has overhead, so use a
separate run and do not mix its timings with the untraced baseline.

## Measured baseline — 2026-10-17

Win64 Development, UE 5.8.3, NullRHI, Intel Core i9-14900HX (24 cores / 32 threads).
Both Editor and packaged runs passed all 30 cases with nine samples each. The
separate memory-traced Editor run passed with three samples. Both Foundation
regressions returned zero failures. Game/Editor builds and cook/stage/archive
passed; all seven recorded benchmark/integration source hashes remained stable.

Packaged medians below are milliseconds per batch of 64 work iterations, except
the native-path comparison, which uses 4096 iterations. Root copy counts include
the initial native-property capture.

| Case | 16 elements | 256 elements | 1024 elements | Root copies / batch |
| --- | ---: | ---: | ---: | ---: |
| Read query groups (four accessors each) | 0.243 | 0.270 | 0.346 | 1 |
| One immutable edit per iteration | 1.395 | 13.055 | 50.524 | 65 |
| Four sequential immutable edits per iteration | 15.225 | 187.975 | 749.548 | 257 |
| Public UE-to-VM scalar dispatch | 4.831 | 4.774 | 4.954 | 0 |

For the 1024-element fixture, native cached paths took 0.888 ms versus 3.111 ms
without reuse (about 3.50 times faster for this specific resolver workload).
Prepared key iteration took 0.352 ms per 64 lookups and built one key order per
captured snapshot; read queries performed zero further field lookups after warmup.
The VM array positive control recorded one allocation and 256 allocated bytes.
Zero VM allocations in native snapshot edits reflect the documented counter scope,
not allocation-free edits.

The next optimization target is snapshot modification. Four edits have four times
as many mutations but approximately 15 times the batch cost at 1024 elements.
Do not attribute this whole difference to copying: validation and snapshot reuse
are also inside the timer. `StoreStructValueImpl` validates each candidate and
linearly checks existing snapshots for equality. Separate timing/CPU analysis of
these steps is still needed. Batch edits with one detached copy and one publication
are a strong next candidate, together with reducing full-value identity searches.
The public dispatch path also rereads source files through `FSourceSnapshot::Matches`;
its isolated contribution has not been measured, so source-check scheduling is a
further profiling target rather than an established optimization result.

These are local measurements on a shared workstation, not portable performance
guarantees or a historical old/new comparison. Nine samples provide descriptive
p95 values, not confidence bounds. Interpreter only; Shipping/AOT and rendered
gameplay were not measured.

Evidence: `logs/performance-validation.json`, `logs/performance-editor.json`,
`logs/performance-packaged.json` and `logs/performance-trace-editor.json` retain
raw samples and scoped counters. Native trace: `logs/performance-trace-editor.utrace`
(about 150 MB). It was captured and checked for successful output; native allocation
totals have not been automatically aggregated from the trace. The validation report
also records the packaged executable hash. Archive: `Build/PerformancePackaged`.
No commit was created.

## Scoped batch edits — 2026-10-17

The current probe adds `batch_four_edits`, bringing the total to 33 cases.
The same four map writes now run inside one `ue_struct_edit` block; final
validation and identity lookup happen once per block. The source snapshot and
checksums match the sequential workload. Root copy counts per 64 groups fall
from 257 to 65, including one initial native capture.

Editor and packaged Win64 Development runs passed all 33 cases, with nine
samples each. Both Foundation runs passed with zero failures, including mixed
draft edits, key-order changes, escaped handles, nested scopes, GC and rollback
after a handled partial map insertion failure. Editor/Game builds and the
Entry-map cook/stage/archive passed. The seven recorded core/test/benchmark
source hashes stayed stable throughout final validation. No commit.

Packaged medians in milliseconds per 64 groups of four writes:

| Elements | Four sequential edits, ms | Four edits in one block, ms | Ratio |
| --- | ---: | ---: | ---: |
| 16 | 15.308 | 1.614 | 9.48 |
| 256 | 187.354 | 13.463 | 13.92 |
| 1024 | 739.183 | 51.294 | 14.41 |

This speedup combines fewer root copies, validations and identity searches;
it does not isolate the cost of copying alone. Timing thresholds are not used.
The earlier memory trace belongs to the original baseline; this change did not
rerun native memory tracing. VM allocation counters retain their previous scope.
See [SnapshotBatchEdits.md](SnapshotBatchEdits.md), raw reports
`logs/batch-edit-editor.json`, `logs/batch-edit-packaged.json` and
`logs/batch-edit-validation.json` for current evidence and limitations.

## Self property access — 2026-10-20

The foundation tests print an informational breakdown (`self_access_benchmark_breakdown`, ns per loop
iteration, 200000 iterations, Development editor, figures vary ±20% between runs):

| Path | read | read + write |
| --- | --- | --- |
| checked path (cached class/name resolution, validated contract) | 251 | 529 |
| self property site cache | 110 | 248 |
| site cache + guard cached per scope | 70 | 168 |

The site cache (`DasScalarAccessors.inl`) is a direct-mapped table per kind and direction keyed by the
property-name string. An entry is used while the self class, the cache epoch (bumped when runtime contexts
are created or destroyed and after garbage collection) and the field path to the same `FProperty` hold;
the name content is compared, so reused heap strings cannot alias. Anything else takes the checked path,
which reports every error.

Every UE extern runs `GuardUEExtern`. Its frozen-physics check (self world and every engine world
context, 30–45 ns, `extern_guard` in the breakdown, measured outside a script) is now done once per
`FActiveContextScope` and owner: Chaos sets the frozen flag only around whole PreSim callback dispatches
(`FPhysicsSolverFrozenGTPreSimCallbacks`), and every script entry opens its own scope. The execution-role
and isolated-VM checks still run on every call. This speeds up all extern calls, not only self access.

## Generated native reference fields — 2026-10-20

`DasForUE.Gameplay.RuntimePIE` prints informational timings from `native_gameplay_reference_probe.das`
(`INFO native_*_ns`, 20000 iterations on the playable character's `CharacterMovement.MaxWalkSpeed`):

| Loop body | before | after |
| --- | --- | --- |
| field write + read | 19 800 ns | 1 100 ns |
| field read | — | 500–620 ns |
| field write | — | 520–640 ns |
| `ue_object_valid` (extern baseline) | — | 240–310 ns |
| object field read (`character.CharacterMovement`) | 12 400 ns | 1 400 ns |

Before, every access resolved the receiver class by path (`FindObject`) and revalidated the generated
field (call shape string, declaring class path, access contract). Both are now cached per site
(`DasNativeGameplayRuntime.inl`): the class by path string, the validated field by (class path, field,
direction, kind, receiver class), with the stored strings compared, weak classes and field paths, and the
self site epoch. Authored references cache the identity check per (identity, class)
(`ResolveAuthoredObject`).

Object fields were dominated by the generic value codec, which builds a full type descriptor (with path
strings) on every read; reference properties now return the object they already read after the same type
check. Class paths are resolved through one shared cache (`ResolveClass`, generated receivers, the
`ue_cast_nullable_object` behind typed views), and `MakeObjectHandle` finds existing handles through an
(object, class) index verified against the handle table instead of scanning every handle. Self object and
component properties (`self.Camera`) use the cached (class, name) lookup.

## Value codec — 2026-10-20

`ReadDasPropertyValue`/`WriteDasPropertyValue`/`WriteDasPropertySnapshot` (accessor properties, call
frames, snapshots) described and validated the property on every call. The validated descriptor is now
cached per property (field path checked, failures not cached). The foundation test prints
`value_codec_benchmark`: describing an int property costs about 440 ns per call (object and struct
properties more, since their descriptors carry path strings); a full cached int read is about 400 ns, most
of which is constructing and moving the `FDasScalarValue` carrier. `DasPerformance` (`performance`
suite) shows no measurable change for `vm_to_ue`: reflected UFunction calls spend their time elsewhere.

## Self container operations — 2026-10-20

Operations on self containers with scalar elements used to run a generated helper that read the whole
container as a snapshot (`ue_get_self_struct`), converted it to a VM array or table, changed it there and
converted and wrote it back (`ue_set_self_struct`). The DSL now lowers them to in-place externs
(`DasScalarAccessors.inl`): `__ue_self_array_get/set/push/insert_<kind>`, `__ue_self_array_pop/erase`,
`__ue_self_container_clear`, `__ue_self_map_get/set_<key>_<value>`, `__ue_self_map_erase_<key>` and
`__ue_self_set_insert/erase_<key>`. They work on the property memory through `FScriptArrayHelper`,
`FScriptMapHelper` and `FScriptSetHelper`, check bounds, element kinds, readonly and `bind_events` like the
other self writes, and keep the snapshot limit of 4096 elements on growth. Nested paths and struct, enum,
object and math elements still use the snapshot helpers.

The foundation test prints `self_container_benchmark` for push+pop on a 1024-element int array: about
220–270 ns per in-place operation, against 190–230 us for one snapshot read and write of the same array.
The snapshot figure is a lower bound for the former path, which also converted the container to and from
a VM value. After `erase` a map keeps the native element order; before, it was rebuilt from the VM table.

## Call cost between C++, Blueprint and daScript — 2026-10-23

`Scripts/TestInteropPerformance.ps1` (suite `interop-performance`) measures what one call costs in each
direction: C++ -> script (`RunNamedPersistent`), the Blueprint bridge (`ExecuteDasFrame`, `ProcessEvent` of a
generated function), script -> C++ (direct externs, reflected calls, generated engine bindings), method
calls through typed references, property access and AOT against the interpreter. It is informational:
medians and p95 over 9 samples plus a checksum per case, no thresholds. The editor commandlet is
`-run=DasInteropPerformance`, the game flag `-DasInteropPerformance` (runtime cases only); the report is
`logs/interop-performance-editor.json`. Cases with an opt-in phase profiler (`DasCallPhases.h`, one branch per
mark when off) also list the nanoseconds of each step of the call (`phases_ns_per_call`).

Medians in ns per call, Development editor, RTX laptop; figures drift up to 3x between runs with the machine
state, so compare cases of one run (the call shape cache was measured A/B in one run).
"Before" is the first measurement of the benchmark; for generated bindings and the typed reference it is
the same run with the call shape cache switched off.

| Call | Before | After |
| --- | ---: | ---: |
| C++ -> script, `RunNamedPersistent` (int -> int) | 1 202 525 | 1 430 |
| Blueprint bridge `ExecuteDasFrame` (int) | 6 086 854 | 3 127 |
| `ProcessEvent` of a generated Blueprint function (int) | 6 217 783 | 3 246 |
| Script -> authored method through a typed reference | 3 559 914 | 6 150 |
| Generated engine binding, object (`fixture_AddHealth`) | 9 900 | 2 183 |
| Generated engine binding, static (`Add_IntInt`) | 16 300 | 2 312 |
| Reflected `ue_call_int` (no call frame, unchanged) | 1 000 | 890 |
| Script -> direct extern (`ue_add`), unchanged | 12 | 14 |
| Script -> script function call, unchanged | 3 | 3 |

What made the difference:

- Hot reload checked every call by reading all script sources (with the engine API modules about 1 MB).
  Calls now check sources only after a reported change: writes in this process are reported by
  `DasScriptWriteTracker` (a pass-through `IPlatformFile` layer), saves in the editor by `DasAutoReimport`,
  edits by other programs by a shared poll of file size and time (`das.ScriptChangePollSeconds`).
  `das.ScriptChangeCheck 0` restores the per-call check (630 us per call in the same run).
- Normalized script paths, verified entry functions, `ScriptPath` (it probed the project folder with
  `FileExists`, 25 us) and parsed authored method contracts (a JSON parse per call, about 230 us) are cached.
- Reflected call frames reuse an `FCallShape` per `UFunction`: parameter descriptions, the signature key,
  parameters by name and which adapters and validators apply (some of these built class path names on every
  call). A/B in one run: 9.9 -> 2.2 us (object), 16.3 -> 2.3 us (static), 12.1 -> 6.1 us (typed reference).

What a call still costs (phase profile): `RunCallback` spends its 1.4 us on a dozen small steps — entry and
lifecycle checks, frame and contract building, session lookup, VM restart and scopes, entry bookkeeping,
event drain — the VM itself about 0.1 us. A generated binding spends its 2.2 us on the frame (`MakeShared`
of the frame state and `FStructOnScope`), argument and result paths and the release. AOT makes pure script
code 7-9x faster (loop 2.6 -> 0.4 ns) and does not change calls across the boundary (guarded externs,
40-170 ns either way).

## Native binds — 2026-10-23

Script -> C++ through native binds (`DasNativeBinds.h`: `ue_engine`, `ue_math`, `ue_time`, game modules), each
case a loop inside the script, `interop-performance` group E. All rows come from one run: this machine's speed
varied about 2x between runs (the native function call measured 36 ns and 71 ns on different days), so compare
rows of one run only.

| Case | ns/call |
| --- | ---: |
| `script_self_method_call` (script -> script, for scale) | 17 |
| `script_native_bind_call` (free function, two ints) | 71 |
| `script_native_method_call` (method through a typed reference) | 153 |
| `script_native_actor_location` (`actor.get_actor_location()`, `ue_engine`) | 136 |
| `script_reflected_actor_location` (the generated binding of `K2_GetActorLocation`) | 22115 |
| `script_native_interp` (`f_interp_to`, `ue_math`) | 160 |
| `script_daslang_interp` (the same formula written in daScript) | 57 |

- The same engine function costs about 160x less through `ue_engine` than through the reflected generated
  binding.
- A method costs about twice a free function: the receiver goes through the context's handle table (lookup,
  `IsA`).
- Crossing into C++ costs more than a few arithmetic operations: `f_interp_to` through `ue_math` is about three
  times slower than the same formula in daScript. Native math pays off for work that is not a one-liner.
- AOT: native binds whose types all have an ABI in the guarded bridge (scalars, strings, engine math values,
  dates and time spans, objects as their handles) are called from AOT code through `dasue::aot::Call`
  (`DasUEAot.h`). `FName`, `FText`, operators and routed gameplay calls stay closed; a script function that calls one stays in the interpreter (lint
  `ue_closed_builtin_calls`), the rest of the file is compiled. `DasNativeBindsManifest` checks this on
  `native_binds_aot_check.das`. Measured in `interop_aot.das` (C3, one run, a faster machine state than the
  table above):

  | Case | Interpreter ns | AOT ns |
  | --- | ---: | ---: |
  | `native_bind` (`days_in_year`, one call) | 34.8 | 25.1 |
  | `native_values` (six calls: `FVector` by value and by const reference, a date) | 226.9 | 174.2 |
  | `scalar_extern` (a guarded `ue_foundation` extern, for comparison) | 155.4 | 171.8 |

  Objects (2026-10-24, another run: `native_values` 240.4 / 212.0 there), `native_objects`, five calls an
  iteration: `set_owner`, `get_owner`, `valid`, `handle`, `as_AActor` — interpreter 536.3 ns, AOT 545.2 ns. A
  compiled object call is not faster: its cost is the handle table (lookup, `IsA`, a new handle for each result)
  and the bridge's guards, the same on both paths. What AOT gains is the script around the calls, which is no
  longer kept in the interpreter because it touches an object. `ue_foundation`'s externs with object handles
  (`ue_self_object()`, casts, validity, object properties: 121 of 186) are compiled too; reflected `UFUNCTION`
  calls through a call frame are not.

  Reference parameters (2026-10-24, `native_refs`: `get_clamped_to_size` and `to_direction_and_length` with two
  output parameters, an iteration): interpreter 359.3 ns, AOT 184.8 ns. A reference is copied in and written back
  on both paths; the compiled caller saves the interpreter's argument nodes.

  Math externs (2026-10-24, `vector_math`: `ue_vector`, `*`, `+`, `ue_length` and one more `ue_vector`, five
  guarded `ue_foundation` calls an iteration; same run: `scalar_extern` 263.3 / 296.5, `native_refs` 356.6 /
  184.9): interpreter 309.4 ns, AOT 350.0 ns. Like `scalar_extern`, a guarded extern is not faster compiled: the
  bridge resolves the entry and checks the guards on every call (`Resolve` / `Finish`), the interpreter's node
  does the same work. Operators go through `dasue::aot::CallOp`, which adds the active context the generated
  code does not pass. The gain is again the script around the calls, which math no longer keeps in the
  interpreter; for speed, `ue_math` binds (unguarded, `native_values`) are the faster path.

Coverage of the AngelScript binds by the native modules: `Docs/Dev/Architecture/NativeBindsCoverage.md`.

### Gameplay methods answered by binds (`Reflects`) — 2026-10-24

A native bind that declares `.Reflects(UFunctionName)` answers the gameplay call of that UFUNCTION on a native
reference (`ref.K2_GetActorLocation()`): the same syntax and receiver checks, without the reflected call frame.
`DasForUE.NativeRoutes` runs the same loop with the bind and with `options ue_native_routes = false`
(reflection), in a temporary game world; the line `native_routes_bench` (one call of `K2_GetActorLocation`,
informational):

| Run | Bind ns/call | Reflection ns/call | Ratio |
| --- | ---: | ---: | ---: |
| Editor automation | 326 | 38925 | ~120x |
| Packaged game (`-DasNativeBinds`) | 187 | 10087 | ~54x |

- The routed call costs a little more than `actor.get_actor_location()` through `ue_engine` (136 ns in the table
  above): it also runs the receiver checks of the reflected call (`NativeGameplayObject`: a live receiver in
  the owner's world, no calls in `bind_events` / `EndPlay`, its class) and passes through a script wrapper that
  keeps the method's name and default arguments.
- Routed calls stay in the interpreter (object receivers have no AOT ABI); scripts with reflected calls did too.

An object result (`native_routes_bench_object`: `GetOwner`, the route returns the handle and the wrapper makes the
reference type), measured with `K2_GetActorLocation` in the same run:

| Run | `GetOwner` bind / reflection, ns | `K2_GetActorLocation` bind / reflection, ns |
| --- | ---: | ---: |
| Editor automation | 502 / 18689 | 357 / 30483 |
| Packaged game (`-DasNativeBinds`) | 254 / 5177 | 172 / 9245 |

The object result adds ~80-150 ns over a vector result: the reference conversion (`ue_cast_nullable_object`)
in the wrapper. The reflected `GetOwner` is cheaper than the reflected `K2_GetActorLocation` (no struct result to
marshal), so the ratio is lower (~20x).

## Typed calls from C++ (`TDasFunction`) — 2026-10-24

`TDasFunction<R(A...)>` (`DasScript.h`) calls an exported script function with typed arguments in the owner's
persistent session. Its first call, and any call after something changed, takes the full persistent call path
(`RunCallback` with the typed contract, no call frame); it then remembers the session and the function, and later
calls with the same owner go straight to the VM (`das.ScriptFunctionFastPath`, see `DasScriptFunctionCalls.inl` for
when it applies).

| Call (int -> int, editor, one run of `interop-performance`) | ns/call, median |
| --- | ---: |
| `frame_bridge_int_check_watch` (`ExecuteDasFrame`) | 3165 |
| `frame_direct_check_watch` (`RunNamedFramePersistent`) | 2604 |
| `cpp_typed_function_call` (`TDasFunction`) | 426 |

`DasForUE.ScriptFunction` measures `add(int, float)` with the fast path on and off in one run: 453 vs 3698 ns
in the editor, 445 vs 5048 ns in the packaged game. The VM itself takes about 100 ns; the rest of the fast call
is the same scopes as the full path (restart, active context, struct collection, event drain).
