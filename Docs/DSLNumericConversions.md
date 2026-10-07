# Numeric type migrations, Blueprint links and rollback

This completes the three numeric type-change stages together: graph pins/links,
additional conversions, and restoration of both data and graphs on failure.

```text
Das.DSL.Reimport /Game/Folder/BP_Name fields=convert_numeric_with_dependencies integers=nearest_away floats=round
```

Use `fields=convert_numeric` for an isolated Blueprint. Defaults for both rules
are `exact`; duplicate or unknown console options are refused. API:
`ReimportConvertedNumericFields(Blueprint, FDasNumericMigrationOptions, Error)`.
The older widening policies keep their original stricter behavior.

All directed conversions between int32, float32 and double are supported:
- `integers=exact`: refuse fractions; `truncate`: toward zero; `floor`: down;
  `ceil`: up; `nearest_away`: nearest integer, ties away from zero.
- `floats=exact`: refuse any float32 precision loss; `round`: explicitly allow
  floating-point rounding, including underflow to zero.
- NaN, Inf and values outside the destination range always fail. No clamping,
  wrapping or saturating conversion is implicit. Widening remains exact.

Source names and stable IDs remain unchanged. The new default must be exactly the
policy-converted old default; other default edits, rename/removal, flags/metadata
or reflected method-contract changes must be separate. Source method bodies can
adapt with explicit casts while retaining their reflected contracts.

Ordinary variable getter/setter value pins update in parent and dependent graphs.
Compatible links stay connected. For different scalar numeric types, the migration
inserts a typed checked UDasNumericLibrary adapter, using the selected rules. Setter
literals are parsed as the old pin type and converted by the same policy. Existing
adapter nodes/semantics are retained on later migrations; necessary new boundary
adapters are added. Variable GUIDs and method contracts remain stable.

For example, after changing Score from int to double, an existing integer-returning
graph gets a double->int adapter. With `exact`, a future fractional Score aborts the
Blueprint VM; with `nearest_away`, 2.5 becomes 3 and -2.5 becomes -3. An out-of-range
value always aborts. Failure occurs before a downstream setter can write a bogus
default. The rules are explicit graph semantics, not just one-time data conversion.

Preflight refuses unsupported wildcard/container/split/orphan/by-reference links,
uncompiled scoped Blueprints, replication and unsupported property/node types.
Scalar int/float/double is the supported domain; this does not adapt arbitrary
struct/object/string/enum changes. Ordinary Blueprint/map dependency scope retains
the previous limits: World Partition/streaming/external actors and unsupported
asset/source types remain separate work. No assets are automatically saved.

Graph snapshots retain original nodes, pin types/defaults and all connections.
Native snapshots cover parent/child CDOs and instances. After mutation, compilation,
restore and schema validation must all pass before metadata advances. On failure,
the service restores old variable descriptors, removes newly inserted nodes,
restores original pins/defaults/links, recompiles, restores native values and original
package dirty flags, and validates the old schema. A failed rollback is reported
explicitly. This extends numeric migration rollback; other migration policies and
engine-crash/external-side-effect recovery are not claimed as generally atomic.

Method-only VM contexts use the previously implemented reinstance preservation and
exact one-use declaration approval. Lifecycle/delegate contexts are still separate.
Unreal may warn about temporary truncation during its own reinstance copy; final
values are restored from the explicit converted snapshots, not that temporary copy.

Examples: dsl_widen_numeric.das; retain IDs/names and edit types/defaults in place.
Tests: TestDSLNumericConvertAsset.ps1 runs Create/Migrate/Reopen/NarrowFloat/ReopenFloat
in separate processes. TestDSLNumericConvertPIE.ps1 exercises connected graph execution,
rounding, VM abort and rollback. Foundation checks cover policy/range/precision rules.
