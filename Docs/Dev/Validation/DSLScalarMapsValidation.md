# Scalar/name map values validation — 2026-10-08

Authored map fields now support int/string keys with int, float, double, bool,
string and UENameValue values. Float/double native layouts remain distinct.
Map migrations remain deferred.

## Confirmed outcomes

- `Scripts/TestDSLTypeRegistry.ps1 -ScalarMap -AuthoredSet -Cooked`: PASS.
  Creation, map save/reopen and packaged execution pass. The cooked probe
  completed 29 checks, including existing string maps and sets.
- All eight new key/value combinations preserve native contents and populated
  typed input/output. Float/double values are compared exactly, true/false remain
  distinct and numbered FName / NAME_None identities survive serialization.
- Script lookup, values iteration and owned snapshot changes pass. Original
  snapshots and Actor data remain unchanged by low-level with operations.
  Populated ProcessEvent result is verified in the editor.
- Missing key, wrong value type and nested container value reject explicitly.
  Native-equivalent string key collisions reject before Actor commit.
- UENameValue now declares a uint64 value representation, a matching native
  extern return carrier and value-copy clone behavior. This permits safe table
  cloning and reference-to-value calls without treating tokens as pointers.
  The pinned daScript sources were not modified.
- `Scripts/TestDSLTypeRegistry.ps1 -PrimitiveArray`: PASS, including FName array
  regression after the shared UENameValue annotation change.
- `Scripts/TestEditor.ps1`: PASS, 428 checks, zero failures. Includes the source
  example and IR roundtrip checks using field identities rather than source order.
- `Scripts/TestDSLDirectTypesPIE.ps1`: PASS, one success, zero warnings/failures
  (shared typed DSL/rollback regression).

Evidence transcripts in the Codex workspace work directory:
scalar-maps-cooked.log, scalar-maps-array-regression.log,
scalar-maps-foundation.log and scalar-maps-pie.log. Shared registry Create/Reopen
logs were overwritten by the subsequent primitive array regression.

See DSLMapAuthoring.md and
Plugins/DasForUE/Content/Das/Scripts/dsl_scalar_maps.das. Replace the example's
REGISTRY placeholder before applying it. No automatic commit was performed.

Next planned step: standalone DSL container parameters/results, followed by
out/ref and interfaces. More keys and enum/struct/object map values remain
separate extensions; migration work remains deferred.
