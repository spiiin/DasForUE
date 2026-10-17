# Prepared snapshot field paths

Snapshot accessors reuse a context-owned path cache. For example, repeated reads
of `Rows.0.Priority` parse the string once and resolve the `Rows` and `Priority`
properties once while the reflected layout remains valid. Read and write access
use the same resolver and preserve the pointer's constness.

The cache contains parsed segments, validated numeric indices, weak struct
identities and Unreal `FFieldPath` handles. It contains no snapshot storage
addresses, array element pointers, copied values or strong UObject references.
Each access traverses the current storage and checks each array index against
its current length. A prepared path can therefore be reused for an original
snapshot and a detached copy, including a copy with a shorter array.

Each property handle is checked for staleness before use. In Editor builds,
Unreal's field serial invalidates the handle when properties are destroyed
during a struct recompile. The resolver then repeats `FindDasStructField`,
including its authored-name lookup, rather than resolving an obsolete native
field path. Nested segments also verify their current struct identity. Cooked
builds follow Unreal's assumption that a live struct's field layout is stable.

Resolution is lazy: an out-of-range array index still fails before a missing
later field. Failed access leaves the output untouched. Missing fields are
looked up again on subsequent access; negative reflection results are not cached.
Public accessors and index syntax remain unchanged. Empty paths are checked
explicitly and report `empty struct field path` before tokenization.
An overlong field segment is rejected as a missing field before constructing
an Unreal `FName`, avoiding the engine assertion for names beyond its limit.

The cache retains at most 128 paths per context. Paths longer than 1024 UTF-16
code units use temporary storage. Dead root types are pruned on insertion; the
cache resets when full and is released with its context. The cache is local to
the existing game-thread context execution model and has no global mutable state.

Implementation: `DasSnapshotFields.h`, integrated through `StructLeafMemory` in
`DasRuntime.cpp`. Native regression checks in `DasSnapshotFieldsTests.inl` cover
repeated resolution counters, const and mutable storage, dynamic bounds, failure
output, error ordering, terminal array elements, cache limits, and destruction /
replacement of properties on the same live struct (Editor).

Validation passed on UE 5.8.3, Win64 Development with NullRHI:

- Game and Editor builds; Foundation in Editor and packaged game, zero failures.
- All six portable path assertions and the Editor field-destruction assertion.
- PropertyValues and RuntimePIE: one success each, zero warnings/failures/skips.
- Fresh gameplay export and native defaults Create / Reopen / Recompile.
- Cook, stage, archive and all four packaged probes: native defaults,
  Foundation, gameplay and playable map.

After a parallel change altered the header's raw hash, the current sources were
frozen and rebuilt; cook, all four packaged probes and Editor Foundation were
repeated successfully. The earlier automation and native defaults phases are
identified separately in the report. Final hashes of the implementation, tests
and runtime integration match their frozen copies. No throughput benchmark was
performed; the reuse claim is checked by parse/build and reflection-lookup counters.

Evidence: `logs/snapshot-paths-validation.json`, with logs under the
`snapshot-paths-` prefix. Archive: `Build/SnapshotPathsPackaged`. No commit.
