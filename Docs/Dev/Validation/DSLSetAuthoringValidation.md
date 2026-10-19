# Set DSL validation — 2026-10-08

The int/string key-only table milestone is complete. Structural set migrations
remain deferred and are rejected before native mutation.

## Verified outcomes

- `Scripts/TestDSLTypeRegistry.ps1 -AuthoredSet -Cooked`: PASS. The isolated
  set probe creates native reflected fields, saves/reopens the map and runs
  the cooked executable. The cooked probe completed 16 checks.
- Native contents and typed input/output preserve negative int keys, Unicode,
  empty strings, duplicate insertion, removed elements and an empty set.
  Populated ProcessEvent return is verified in the editor.
- Script keys iteration, key_exists and snapshot size/contains/add/remove pass.
  Owned snapshot changes leave the original snapshot and Actor unchanged.
- Invalid ordinal and wrong element type reject. Oversized inbound set snapshots
  reject at the shared 4096-element bound without changing Actor data.
- Native-equality collisions between distinct string spellings reject before
  root commit. Unsupported element types, explicit defaults and temporary self
  iteration/mutation reject. Migration refusal was tested without map fields.
- A combined set/string-map probe also passed save/reopen and cooked execution
  before isolation of the final test mode.
- `Scripts/TestEditor.ps1`: PASS, 424 checks and zero failures. Includes the
  compiled example, IR roundtrip and invalid element/container-shape checks.
- `Scripts/TestDSLDirectTypesPIE.ps1`: PASS, one successful test, no warnings
  or failures (shared typed DSL/rollback regression).

Evidence in the Codex workspace `work` directory: `sets-cooked.log`,
`sets-isolated-cooked.log`, `sets-foundation.log` and `sets-pie.log`.
Repo runtime evidence: `logs/type-registry-cooked.log` and
`logs/dsl-direct-types-pie-report/index.json`. Shared logs can be overwritten
by subsequent probes; the workspace transcripts preserve the recorded runs.

See `DSLSetAuthoring.md` for the supported syntax, APIs and limits.
No migration expansion or automatic commit was performed.
