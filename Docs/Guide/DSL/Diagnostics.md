# DSL diagnostics

Compiler, annotation/lowering, execution and editor-import errors share a source header:

```text
C:/project/Content/Das/Scripts/counter.das(12,9): [DAS:runtime] Counter.AddScore | <error message>
Details:
<original compiler/VM message and stack>
```

Lines and columns are one-based. `Counter.AddScore` and `Counter.Score` refer to
user declarations. Generated entry/implementation names are replaced in the
primary message where their compiler-owned source mapping is known. Original
messages remain under `Details` for investigation and existing log tooling.
Phases are `compile`, `DSL`, `runtime` and `import`.

## In the editor

Interactive import/reimport errors appear in the Blueprint Message Log; reported
PIE script failures use the PIE Message Log. The file/line action opens the `.das`
file with Unreal's configured Source Code Access provider. Configure an editor
provider that can open DAS files. The action validates the file and current line
before opening it; removed files or lines are refused instead of jumping to an
unrelated location. Commandlets and automation retain their existing logging
behavior and do not open a source editor.

Runtime errors from ordinary UE extern calls retain the original call-site even
when a helper does not accept a `LineInfoArg`. Lowered property/method operations
retain the source expression's coordinates. Errors in imported modules point to
the imported file. Handling a script exception with `try/recover` does not itself
emit a Message Log failure.

## Reload and precision

Source/name maps belong to the compiled program's ModuleGroup, not a global
cache keyed by filename. A rejected reload can report the candidate compiler
error in `ReloadError` while the retained VM's `Error` still points to the source
coordinates from its previous successful compilation. The file on disk may have
changed: stored coordinates do not restore the previous file contents.

When an import/preflight failure has no expression location, the diagnostic uses
an unambiguous declaration where available, including a single declaration whose
explicit stable ID differs from its public class name. Otherwise line/column are `0,0`;
the tooling does not invent an expression line. There is no universal rewriting
of arbitrary native C++ errors or unknown generated names.

These maps cover the interpreted DSL/glue path. Full authored gameplay AOT source
maps, debugger/LSP integration and native crash symbolication remain separate
work. This feature does not change reflection identities, generated function
names, persisted schemas, extern ABI or execution/lifetime guards.

## Focused validation

Queue through the shared build agent:

```text
test-suite diagnostics1 dsl-diagnostics repo=C:\src\DasForUE2-main
```

This builds Editor Development and runs one `DasDSLDiagnostics` commandlet.
Nine focused checks cover compiler/raw details, property annotation coordinates,
CMRES native errors, recover, imported modules, rejected reload with retained
coordinates, real authored methods and import preflight failures, lowering errors and the actual clickable
action token with an injected opener. No external editor, PIE or packaged run
is part of this suite. Receipt: `logs/dsl-diagnostics-validation.json`;
commandlet output: `logs/dsl-diagnostics-runtime.log`.

Verified on Win64 / UE 5.8.3 by build-agent job `dsl-diagnostics-20261023g`
(2026-10-23, exit 0): Editor Development build and nine grouped checks passed;
commandlet execution 0.26 seconds. The navigation test executes the real Message
Log action token with an injected opener, rather than launching an external editor.
