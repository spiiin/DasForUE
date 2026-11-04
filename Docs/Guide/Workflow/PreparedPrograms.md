# Preparing gameplay programs

Gameplay sessions share a compiled daScript program. Every object still receives
its own context, globals, heap, event subscriptions and lifecycle delivery state.
Preparing a program does not run its globals or gameplay callbacks.

List the gameplay roots in the project's `Config/DefaultGame.ini`. Include roots
used by dynamically spawned projectiles, abilities and widgets: placed actors
alone cannot describe all future spawns. Required modules are tracked automatically.

```ini
[DasForUE.Programs]
+Scripts=projectile.das
+Scripts=player.das

[DasForUE.Programs /Game/Maps/Arena]
+Scripts=arena_ability.das
```

The runtime prepares the global list and the current map's list during world
initialization, before actor initialization and BeginPlay. Programs remain pinned
until world cleanup, including across garbage collection and LRU eviction. This
does not preload soft-referenced UE assets or prepare unlisted dynamic scripts.
`FDasRuntimeModule::PreparePrograms` exposes the same operation to a custom loading
flow and refuses calls after BeginPlay. A preparation failure is logged; strict
release mode stops world loading rather than starting with a missing program.

## Editor and PIE

Every editor launch compiles all project `.das` files recursively under
`Content/Das/Scripts`, whether a Blueprint references them or not. It also checks
the plugin runtime modules listed in `Content/Das/RuntimeScripts.txt` and any
plugin scripts connected through the project's `Config/DasPluginScripts.txt`.
The plugin's intentional negative test fixtures and the entire vendor standard
library are not standalone project inputs; required library modules compile with
their importers. No per-class selection or lazy compilation mode is retained.

The full pass runs on the first safe editor tick after startup/map opening and
reports errors without needing Play. It checks every selected input even if an
earlier script failed. A slow-task indicator appears for longer passes. Startup
preparation is independent of `Das.DSL.AutoReimport`. Commandlets retain their
explicit preparation/export stages rather than launching the interactive editor
startup pass.

The editor shares full compilation results between class declaration reads,
catalog discovery, method preflight and persistent gameplay sessions. Dedicated
type-only collection remains a separate compile mode. Compiled programs, including
failed attempts, remain recorded for the session; repeating a validation pass
reuses unchanged results rather than repeatedly compiling the same broken source.

Saving a source schedules preparation after the file-watcher settle delay
(0.5 seconds). The queue updates that file and affected dependent programs,
including dependencies on modules/catalogs. Adding/removing files refreshes module
resolution. The project script directory is created/watched even when initially
empty. Compilation runs on the editor thread because UE-aware annotations use
Unreal state; it is not background compilation and does not run in a gameplay
callback.

Before PIE/SIE, another full pass checks for source edits and new files, and the
PIE authorizer rejects compilation failures or unapplied Blueprint declarations.
Silent toolbar permission queries do not compile or prevent a new attempt after
repairing the source. Turning automatic asset migrations off does not disable
source compilation. Structural changes retain their normal Apply/reimport policy.

Persistent gameplay entries never compile missing/stale sources. Existing owners
keep their last working VM while the save queue runs; new owners without prepared
code receive an actionable error. Publishing a prepared result lets a refused
callback retry without another file edit. Syntax errors keep working contexts,
subscriptions and owner data intact. Preparation does not replay lifecycle
callbacks or share mutable globals between owners.

For packaged games, prepare configured roots during loading or stage exported
artifacts. The interactive editor pass is not a packaged-game startup system.
One-shot editor tools and the isolated physics compiler remain separate execution
paths.

## Prepared release artifacts

After generating bindings/assets, export programs using the project's editor
commandlet before building the game and running cook/stage. The `prepare-programs`
build-agent suite runs `Scripts/PreparePrograms.ps1` for the checkout's smoke
project. For another project, run `-run=DasPreparePrograms` through the build agent's
project pipeline. With no arguments the commandlet reads all global/map lists;
`-Scripts=projectile.das,player.das` explicitly selects roots.

The commandlet produces `Content/Das/Prepared/*.dasp`. UBT stages these files as
UFS runtime dependencies alongside the existing staged sources. Run export before
the game build: a build receipt created earlier does not know about new artifacts.
Regenerate artifacts after changing the runtime bridge, compiler library,
generated modules or script dependencies. Every artifact is read back before the
export reports success. Export failures must stop packaging.

These artifacts contain the typed program AST, not native machine code. Loading
bypasses parsing, macros and type checking. The VM still generates an owner-local
execution context and initializes that owner's globals. This is separate from the
existing partial native AOT bridge; it does not establish full gameplay AOT.

At load time, the runtime checks the bridge/compiler fingerprint, compile
environment, payload digest and source/dependency digests. Content paths are
project/plugin relative, so artifacts can relocate with a packaged project. Keep
sources staged for dependency validation and diagnostics. Artifacts are trusted
build output, not a supported format for downloaded content.

`das.RequirePreparedPrograms=1` refuses source compilation on a persistent gameplay
cache miss. Shipping defaults to this mode; Development defaults to source fallback
at explicit preparation stages. Gameplay calls require ready programs. Set the
variable to 1 in a Development acceptance run to verify the release path. A missing, stale or incompatible artifact produces an explicit
error. Existing valid sessions keep their last working code on a rejected reload.
One-shot tools and the isolated physics compiler remain separate execution paths.

`das.CompiledProgramCacheEntries` bounds idle artifacts (default 32). Live sessions,
editor preparation records and prepared worlds retain their programs independently. Zero disables idle
retention while still allowing reuse of these owned programs.

## Focused validation

Queue `test-suite <id> program-cache repo=<worktree>` through the shared agent.
The tests cover independent mutable state/subscriptions, source and require edits,
failed initialization, LRU/GC lifetime, preparation before BeginPlay, authored-class
AST export/load, actual UE property writes through a restored authored method,
automatic configured world preparation, fresh-process loading and strict refusal
after a source change. Receipts are written to
`logs/program-cache-validation.json`. Editor automation is not packaged or full AOT
acceptance; record those runs separately.

Queue `test-suite <id> editor-preparation repo=<worktree>` for the focused editor
policy scenario: shared authoring/preflight compilation, preparation before play,
no compilation on gameplay calls, save/dependency updates, recovery of a refused
owner, last-working-code retention, compile/schema rejection by the PIE authorizer,
unused-script diagnostics and automatic compilation of newly saved files. This
uses a synthetic game world and the authorizer implementation, not a rendered PIE
session.

## Editor module cache and timings

The editor checks every project script and selected plugin module on startup and
before PIE. Saving files still prepares affected programs through the existing
watcher/dependency graph. The in-memory program cache retains its current rules.

`das.EditorModuleCache=1` (default) also caches standard daScript library modules
under `Saved/Das/ModuleCache/*.dasm`. The cache is private to the project/worktree
and disposable. The wrapper ABI (including the compiler libraries), compiler
mode, root path, computed-default pass and already-promoted shared modules
identify each stream. Saves in a running editor cannot overwrite the full library
prefix needed by the next editor process. daScript checks
module source bytes, policies, builtin signatures and declared macro dependencies.
Missing, stale or damaged streams recompile from source. A cache write failure
leaves the compiled program usable.

For this first integration, modules outside the configured `daslib` directory,
and modules directly exposing UE foundation, always reparse. They consult loaded
UE definitions and register compiler-only metadata; restoring their AST without
replaying those effects could accept stale Blueprint contracts. Source locations used by promoted shared libraries are retained until the
daScript environment shuts down; disk byte buffers are released after reading.
The standard library cache can accelerate the first compile in a new editor process while
preserving the full authoring checks. This is distinct from release `.dasp`
preparation. It does not persist or share owner state.

The default `DAS_EDITOR_COMPILE_ALL` log record includes wall-clock seconds,
source compile calls, memory hits, disk-served modules, reparsed modules and disk
writes. `DAS_EDITOR_COMPILE_SLOW` lists the five slowest roots. Set
`das.CompilerTimings=1` for `DAS_COMPILE_PASS` details (collect/resolved pass,
milliseconds and cache cutoff reason). Per-pass times include nested compilation;
use batch wall time for total startup cost. Set `das.EditorModuleCache=0` to
compare separate editor starts with source-only compilation; changing this flag
does not discard the ordinary memory cache.

The cache currently has a per-stream read limit of 256 MiB. Its project Saved
folder can be removed while the editor is closed. Total-size eviction is a
follow-up; old ABI-specific streams can accumulate.

## Shared source reads during editor preparation

`das.EditorSourceBatchCache=1` (default) shares source snapshots for one synchronous
editor preparation pass. Startup/map-open reimport, bulk compilation and declaration
checks nest into the same scope; saved-change processing and pre-PIE preparation
use the same rule. Each compiled program keeps its own dependency snapshot and AST.

A file is read and hashed on first use in the pass. Other roots reuse those bytes;
validation compares the content hash against each root's own previous revision,
so roots with different baselines are still invalidated correctly. File size and
mtime alone never establish equality during preparation. Cached file reads include
catalog JSON accessed through the source-snapshot layer.

The outermost scope releases all temporary snapshots. Every later pass reads fresh
bytes. Source-write notifications clear active snapshots too, including missing-file
entries, so a save or a newly created module can be observed within the same pass.
This is a short-lived view of source reads, not a persistent filesystem cache or an
atomic multi-file revision.

`DAS_EDITOR_SOURCE_READS` reports content-read requests, bytes, snapshot hits,
dependency checks and invalidations for the startup loop. Filesystem path/stat
lookups outside the source-snapshot layer are not included. Set
`das.EditorSourceBatchCache=0` to compare repeated preparation without this cache.
Compiled-program reuse and the disk module cache have their own independent settings.
