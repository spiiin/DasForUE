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

Before, every access resolved the receiver class by path (`FindObject`) and revalidated the generated
field (call shape string, declaring class path, access contract). Both are now cached per site
(`DasNativeGameplayRuntime.inl`): the class by path string, the validated field by (class path, field,
direction, kind, receiver class), with the stored strings compared, weak classes and field paths, and the
self site epoch. Authored references cache the identity check per (identity, class)
(`ResolveAuthoredObject`).
