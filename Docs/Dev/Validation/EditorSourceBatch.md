# Editor source batch — 2026-11-04

Implemented in isolated `codex/editor-source-batch`, based on `b6998ab`.
Build-agent job `editor-source-batch-20261104a`: Editor Development build PASS,
focused `DasForUE.Runtime.EditorSourceBatch` PASS (1 success, 0 test warnings,
0 failures; body 0.232466 seconds), exit code 0.

Repeated content validation of three roots with a common module:

| Measurement | Without batch cache | With batch cache |
|---|---:|---:|
| Content-read requests | 9 | 5 |
| Bytes read | 242295 | 81033 |
| Dependency checks | 9 | 9 |

These are content-read counters in the snapshot layer, not OS disk-cache misses
or total editor startup timings. The disk module cache was disabled to isolate
the effect of shared snapshots. Compiled programs were reused in both cases.

The scenario checks nested scopes matching startup's per-root calls, disposal
of temporary snapshots, all three roots recompiling after a same-size/same-mtime
edit, write invalidation inside an active scope, missing-to-created files,
last-working-code retention on a bad save and successful repair.

Scopes span map-open/startup reimport plus bulk preparation, save processing and
pre-PIE declaration checks. Content hashes are captured when bytes are read;
each root compares its own previous hash, avoiding a single stale yes/no per
path when roots have different source revisions. Writes clear active snapshots;
new passes always read fresh bytes. This does not share mutable AST/ModuleGroup
ownership or retain a process-wide filesystem snapshot cache.

Evidence: `logs/editor-source-batch-validation.json` contains source SHA-256s and
measurements; `logs/editor-source-batch-report/index.json` contains automation
results. The existing C4305 build warning is unchanged. Full PIE, packaged and
an interactive project startup benchmark were not run. Transfer to main preserves
unrelated work; a build of the combined main tree is a separate check.
