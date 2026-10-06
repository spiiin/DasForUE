# Exact numeric field widening

Explicit editor policies:

```text
Das.DSL.Reimport /Game/Folder/BP_Name fields=widen_numeric
Das.DSL.Reimport /Game/Folder/BP_Name fields=widen_numeric_with_dependencies
```

API: `FDasDSLService::ReimportWidenedNumericFields(Blueprint, Error,
IncludeDependencies)`.

This stage supports int32 -> double and **finite float32 -> double**. Both preserve
the original numeric value exactly. It refuses int -> float, narrowing, nonnumeric
types and NaN/Inf instance values before mutation. Names, stable IDs, flags,
metadata and reflected method contracts must stay unchanged. Bodies may be edited
to compile with the new field type. No field removals or renames are combined.

Edit the original DSL file in place, for example:

```dascript
// Before
Score : int = 5
Rate : float = 1.5
// After
Score : double = 5.0lf
Rate : double = 1.5lf
```

Keep @ue_property annotations. IDs remain automatic when names do not change.
The new default must equal the exact promotion of the old default. For an old
float literal 0.1, that value is 0.10000000149011612, not the double literal 0.1.
Changing a default is a separate migration. Runtime reads use the existing double
property adapter; method signatures can stay double before and after by using an
explicit `double(self.Score)` conversion in the body.

The isolated policy refuses dependent assets. The dependency policy uses the same
bounded ordinary Blueprint/map scope as dependency-aware rename: assets load
before capture, the parent compiles first, inherited CDO and actor overrides restore,
and affected packages stay dirty for review/save. World Partition/streaming and
unsupported saved asset/source types are still outside this scope.

Direct Blueprint property nodes referencing a converted field are refused, even
when disconnected. Their pin/link conversion needs another policy; existing DSL
method call graphs retain their reflected contracts. Scoped Blueprints must already
have a successful compiled status; compile pending Blueprint edits first.

Migration captures original native values and variable descriptors before changing
types. Successful conversion preserves variable GUID/IDs and restores promoted
CDO/instance values. If compilation, restoration or final validation fails, it
reinstates the old descriptors, recompiles the scope, restores original values,
validates the original schema and restores previous package dirty flags. Canonical
DSL metadata advances only on success. An incomplete rollback is reported explicitly;
this is not a guarantee of recovery from engine crashes or arbitrary external effects.

Idle method-only VM contexts without lifecycle delivery, subscriptions or cached
marshalled calls are temporarily held outside normal actor teardown, remapped to
replacements and resumed. Last-good callbacks survive a tested numeric rollback.
A successful migration grants one exact declaration approval to retained contexts; it is cleared on promotion. Removing the DSL declaration does not bypass the guard. Source validation retries after reinstance even when a previously rejected source
has identical bytes. Lifecycle/delegate/cached-call contexts are not preserved by
this mechanism. Successful source promotion uses the normal reload behavior.

Tests: TestDSLNumericWidenAsset.ps1 (Create/Migrate/Reopen in separate processes),
TestDSLNumericWidenPIE.ps1 (live actors, rollback, last-good and successful promotion).
Failure injection is development automation code and is not a user-facing policy.
The original source file is never rewritten by the migration service, including rollback.
