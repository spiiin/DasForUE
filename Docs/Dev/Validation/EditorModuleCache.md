# Editor module cache — 2026-11-04

Implemented in isolated `codex/editor-module-cache`, based on main `0bbbd39`.
Validation ran in the isolated worktree. Transfer to main preserves unrelated
work; a build of the combined main tree is a separate check.

Build agent job `editor-module-cache-20261104g`: build PASS, focused
`editor-module-cache` suite PASS, exit 0. One scenario runs three commandlet
processes: cold, warm and reopen after dependency saves.

| Phase | Preparation seconds | Disk modules served | Checks |
|---|---:|---:|---:|
| cold | 0.227114 | 0 | 4 |
| warm | 0.071700 | 9 | 18 |
| reopen | 0.086319 | 9 | 5 |

These are single observations on the synthetic script, including authored DSL
and computed defaults. Cold includes writing new cache streams; it is not an
uncached-source baseline. They are not total editor/project startup timings.
The regular startup log now records batch wall time and the five slowest roots.

The warm phase checks equivalent authored metadata, actual execution, no
recompilation on repeated preparation, saved dependency invalidation, an unused
invalid script, last-working-code retention, repair, and damaged-stream fallback.
The reopen phase proves a save did not replace the full library-prefix stream
with a shortened stream from an already-warm compiler process.

Safety policy: only standard-library modules without UE foundation visibility
are eligible for disk restoration. All other modules reparse, preserving UE
reflection validation and host-only macro metadata. Source hashes, require sets,
policies and builtin identities use native daScript module-cache validation;
the wrapper ABI, compile mode, root, pass and promoted-module set partition streams.
Nested compiles restore the parent reader/writer. Shared module source locations
outlive program-cache eviction; bulk byte buffers are released after decoding.

Evidence: `logs/editor-module-cache-validation.json` contains the verified source
hashes and measurements; `logs/editor-cache-{cold,warm,reopen}.log` contain phase
markers and per-pass timings. Four pre-existing ACLPlugin compression-settings
warnings appear at engine startup; there are no commandlet errors. The unrelated
C4305 warning in DasCompleteBuiltinValues.inl is unchanged.

Full interactive-project startup, PIE, packaged Development and Shipping/AOT were
not run. No C++ bindings or network changes. Cache streams have a 256 MiB read
limit; total-size eviction is deferred. Project Saved cache files are disposable.
