# Editor compilation reuse

`das.EditorModuleCache=1` enables validated disk module streams. UE-dependent
records retain the conservative reparse policy: restoring serialized AST alone
cannot replay host-side annotation registration against live Unreal Reflection.

`das.EditorSharedModules=1` reuses stable plugin helper AST in memory within one
synchronous editor source-read scope. Automatic editor preparation, explicit world
preparation and nested authoring requests establish nestable scopes. It does not
promote helpers to process-wide builtins. Eligible modules must have a `ue_` name, live under the plugin script
library, have no runtime globals, and depend only on eligible modules or builtins.
Authored UE declaration annotations are excluded. Each root still compiles its
own declarations and gets a separate simulated context.

Consumers pin the original compiler artifact, including its source locations,
and record borrowed source snapshots as dependencies. Ending a preparation pass,
clearing the program cache, or receiving a source-write notification clears the
shared lookup. Existing compiled contexts retain their AST owners. Computed
property defaults still use a fresh authored declaration group for the resolved
pass. The disk reparse policy is unchanged.

`DAS_EDITOR_COMPILE_ALL` includes `shared_modules`, the number of cached helper
modules inserted into compiler groups. It is a reuse counter, not a duration or
a count of root compilations avoided.

Projects can exclude unused test roots from automatic editor preparation:

```ini
[DasForUE.EditorPreparation]
+ExcludedDirectories=roguetest
```

Paths are relative to project `Content/Das/Scripts`; absolute paths and paths
outside that directory are ignored. Automatic scanning, stale Blueprint checks,
and changed-root preparation honor the exclusion. Explicit `PrepareEditorPrograms`
calls and `require` dependencies still compile those files. Scripts explicitly
listed in global or active-map `[DasForUE.Programs]` sections take precedence.
The declaration catalogue remains available for name resolution.

Regression coverage: `DasForUE.Runtime.ProgramCacheSharedModules` checks authored
computed defaults, dependency edits, independent state, owner lifetime after
batch completion/cache eviction/GC, and the disabled-sharing fallback.
`DasForUE.Runtime.ProgramCacheEditorExclusions` checks automatic exclusion,
explicit preparation, required dependencies, and configured-root precedence.

## Measured port runs

Both course-world runs used the same 63 configured scripts and enabled disk cache
and compiler timings. Before adding world/nested-authoring scopes,
`editor-shared-modules-v6-20261105` passed 11 course checks in 384 seconds; the
interval between the one-root and 63-root preparation markers was 259.362 seconds.
`editor-shared-world-final-20261105` passed the same 11 checks in 272 seconds;
that preparation interval was 162.554 seconds. These are local measurements,
not a guarantee of startup time on other machines or maps.

The automatic ChestTest pass fell from 234.136 seconds (148 roots, before these
changes) to 146.882 seconds (144 roots, 1774 helper insertions). This comparison
also includes directory exclusions; ChestTest explicitly opts 17 test roots back
in. The course map has no such test-root configuration. Of the project's 97
script roots, 21 reside in roguetest and are excluded from normal automatic
preparation. Blueprint asset preparation and authored dependency compilation
still account for substantial startup time.
