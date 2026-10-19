# String map validation — 2026-10-08

Supported fields inside `[ue_struct]`: `table<int;int>`, `table<int;string>`,
`table<string;int>` and `table<string;string>`. Structural map migration remains
deferred; unsupported migration requests reject before native changes.

## Confirmed checks

- `Scripts/TestDSLTypeRegistry.ps1 -StringMap -Cooked`: PASS. Editor creation,
  save/reopen and packaged execution passed; the cooked probe completed 19 checks.
  Native contents and typed returns preserve Unicode, empty keys/values and quoted
  text. Exact duplicate keys update values. Missing keys reject.
- Distinct string spellings considered equal by native reflected map equality
  reject during snapshot packing. The failed operation preserves the Actor's
  previous native map contents.
- `Scripts/TestDSLTypeRegistry.ps1 -PrimitiveArray`: PASS (regression).
- `Scripts/TestEditor.ps1`: PASS, 420 checks and zero failures.
- `Scripts/TestDSLDirectTypesPIE.ps1`: PASS, one successful test, zero warnings
  and zero failures.
- `git diff --check`: PASS. Temporary GUID probe sources were removed.

Evidence transcripts in the Codex workspace `work` directory:
`string-map-cooked.log`, `string-map-array-regression.log`,
`string-map-foundation.log` and `string-map-pie.log`. The shared registry
Create/Reopen logs are overwritten by later regression runs.

See `DSLMapAuthoring.md` for API, ownership, iterator rules and limits.
The example is `Plugins/DasForUE/Content/Das/Scripts/dsl_string_maps.das`;
replace its `REGISTRY` placeholder with the actual registry object path.

Next planned functionality: set support, more map value types and standalone
container signatures, followed by out/ref and interfaces. Migration development
remains a separate deferred task.
