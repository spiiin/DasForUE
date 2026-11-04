# Editor module-cache record isolation

## Root cause and evidence

The original warm-start failure requested 1,881,019,372,055 bytes while importing
an authored attribute hierarchy. The isolated reproduction confirmed that a
cached Function reference was actually a Structure/Variable reference. The old
reader copied its incompatible name layout as a string. The earlier Function
kind guard prevents that allocation, but did not repair the producer.

Byte-offset traces now identify the producer error. UE modules such as
`ue_object_keys` and `ue_native_self_refs` contain host-only annotations that
cannot be serialized. Their records are intentionally reparsed. A failed write,
however, left the shared `AstSerializer::failed` flag set. Later standard-library
modules inherited it: `Module::serialize` returned after the functions section,
omitting generics, lookup tables, FileInfo/use sets and macro state. Record
lengths were backpatched, so the file headers looked consistent, but the records
were still advertised as readable. The reader consumed following bytes as the
missing fields, eventually treating an unrelated node as Function.

The first `rtti` write/read traces agree through offset 686594; the writer then
ends the module, while the reader attempts a missing section at 686603. This
rules out file size and explains why a 256 MiB file limit did not prevent a
terabyte string allocation. Traces: `logs/cache-traces/trace-*.tsv`.

## Fix

- Reset the writer error state before each record.
- Write a reparse-only flag first. Clear it by backpatch only after successful
  serialization of an eligible module. Failed payloads remain opaque; a storage
  without backpatch support also stays conservative.
- A serialization write failure no longer marks a successfully compiled source
  program as failed. Read failures still invalidate the deserialized candidate.
- Retain the Function-kind check for stale/damaged streams.

The existing flag format is unchanged. Compiler-library hashes in the prepared
ABI key invalidate old module-cache filenames after the dependency rebuild.
Original project streams are retained; no broad cache deletion is required.

`Scripts/BuildDependencies.ps1` applies the reviewed patch to both dependency
sources after verifying the pinned revision. `ApplyDaScriptPatches.ps1` validates
all contexts before writing, preserves unrelated edits and is idempotent. Its
replacement specification is `Scripts/Dependencies/daScript-module-cache-patches.json`;
the equivalent unified patch is `daScript-module-cache-safety.patch` beside it.
Both `libDaScript.lib` and `libDaScript_runtime.lib` must be rebuilt: wrapper
source alone cannot fix an old compiler binary. The checked worktree uses
standalone library copies; shared dependency sources/libraries are untouched.

## Observable acceptance

`FDasProgramCacheStats::DiskModuleRejections` counts failed payload reads,
including policy/environment drift, separately from intentional source reparses.
`DAS_COMPILE_PASS` reports `rejected`; startup reports `disk_rejected`.

The maintained `editor-module-cache` probe imports UE adapters before stdlib,
which reproduces the poisoning order without depending on another project. It
requires positive disk hits, zero payload rejections, unchanged DSL metadata and
actual typed Option execution. An unchanged warm stream must not be rewritten.
Save, invalid-save retention, source repair and post-save process restart remain
covered. Compiler libraries and dependency patch files join the source freeze.

`DasForUE.Runtime.ModuleCacheSafety` also writes an intentionally unserializable
module followed by a valid module. It checks that the failed record is marked
opaque, the source program remains valid, and the next record is cacheable and
fully deserializes. The existing wrong-node and valid-Function checks remain.

## Validation

Diagnostic job `cache-record-boundaries-20261104b` passed the real five-class
attribute import in two fresh processes. Its first process encountered a retained
old bad stream and rewrote it. The next process served 15 stdlib modules from disk,
reparsed 24 UE/authored records by design, and reported no deserializer diagnostics.
The measured root preparation changed from 1.671369 s to 0.669991 s. These are
single script observations, not a whole-editor startup benchmark.

Final reviewed-source job `cache-module-format-fixed-20261104d` exited 0:
Editor Development build, 1 automation test with zero warnings/failures, and
5/20/6 cold/warm/reopen checks passed. The synthetic-root initial preparations took
2.205753 / 1.166904 / 1.235033 seconds; warm and reopen served 16 stdlib modules with
zero payload rejections. The unchanged warm run performed zero disk rewrites.
Its 46 intentional reparses span collect and resolved-default passes, not failed
AST reads. The native regression test took 0.016553 seconds. Source, compiler
library and patch hashes stayed frozen throughout verification. The preceding
build caught a test-only constructor signature mismatch, corrected before this run.

Receipts: `logs/editor-module-cache-validation.json`,
`logs/editor-module-cache-format-validation.json`; automation:
`logs/module-cache-safety-report/index.json`. Whole-editor startup was not benchmarked.

No full PIE, packaged run, main integration or original-project cache
re-enablement is included. The original project's `MODULE_CACHE_REPRO.md` still
requires its own two successful combat PIE runs before removing its local
workaround; that is a separate project acceptance gate.
