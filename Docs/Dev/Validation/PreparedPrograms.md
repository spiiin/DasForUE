# Gameplay program preparation — 2026-11-02

Worktree: `C:\src\DasForUE2-program-cache`, branch `codex/program-cache`.
Build-agent job `editor-all-20261102b` completed with exit 0.

- Win64 Development Editor build: PASS.
- `DasForUE.Runtime.ProgramCache`: PASS, no automation warnings.
- `DasForUE.Runtime.PreparedPrograms`: PASS, no automation warnings.
- `DasForUE.Runtime.EditorPreparation`: PASS, no automation warnings.
- Separate export and strict-load commandlet processes: PASS;
  `DAS_PREPARE_VERIFY PASS programs=1 compilations=0`.
- Both validation receipts match the final implementation hashes (0 mismatches).

The editor scenario checks the full pass over project scripts and selected plugin
runtime modules. Its cold pass checked 56 inputs with 0 failures and 56 source
compiler requests. Expected error fixtures then verify that an unused file with
no Blueprint is still diagnosed, unchanged failed attempts are retained across
LRU pressure, and repairs become available after preparation. The scenario checks
shared authoring/preflight compilation, dependency updates, no compilation from a
gameplay callback, retry after preparation, and PIE authorization rejection for
compile failures and unapplied declaration changes. Newly created files become
callable after the save queue prepares them. No lazy gameplay compiler switch is
retained.

The same full-pass method is hooked to interactive editor startup/map opening
and pre-PIE validation. The automation calls that method directly in a synthetic
game world; a separate visible editor startup/PIE session was not run.

The cache scenario uses real spawned Blueprint actors and checks independent
globals, containers and subscriptions, dependency reload, failed initialization,
LRU eviction and GC. Zero idle-cache retention still reuses programs held by
prepared worlds or live contexts. Recorded fixture timings: preparation 49.552 ms,
first prepared spawn 10.111 ms, subsequent spawn mean 1.418 ms. These measurements
are not an ActionRoguelike map benchmark or a performance guarantee.

The artifact scenario checks world-initialization preparation, no owner simulation
during preparation, independent VMs, authored AST export/load, a restored authored
method writing a real UE field, and strict refusal of changed source. A new
process loads the artifact without inheriting modules from its exporter.

Earlier failed checks exposed module-group/AST ownership during deserialization,
test-world teardown/context setup, and failed-attempt retention during a full
editor pass. The passing job includes the fixes. Commandlet startup retains four
existing ACL compression-asset warnings; all three automation reports have zero
warnings.

Receipts: `logs/program-cache-validation.json`,
`logs/editor-preparation-validation.json`. Feature logs use the corresponding
`program-cache`, `prepared-programs`, `prepared-export-fresh`,
`prepared-load-fresh` and `editor-preparation` names under `logs/`.

No rendered PIE, packaged Development, Shipping executable or whole-game native
AOT run was performed. The artifact backend loads typed AST for the VM; it does
not establish native gameplay AOT. Production-project manifests and cook/stage/load
acceptance remain project integration work.
