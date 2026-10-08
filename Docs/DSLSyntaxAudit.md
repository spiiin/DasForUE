# DSL syntax audit — 2026-10-08

Scope: runtime authoring annotations, direct-type adapters, generated native
wrappers, public examples and current syntax documentation. This is a source
audit, not a claim to have exercised every historical migration fixture.

## Immediate regression fixed

The legacy scalar adapter renames a single int/float/bool parameter to `Input`.
The old ue_class(implements=...) form disables that behavior during parsing.
Repeatable ue_implements is resolved later, after annotations are attached, so
the parser previously still renamed the input. A contract such as
`Query(input:int):int` would then fail because `input` and `Input` are distinct.

The final annotation pass now restores source parameter names and automatic
parameter IDs for the repeatable-interface form before validating the interface.
Legacy non-interface adapters retain their existing identities. The authored
interface probe now deliberately uses lowercase `input` to catch this regression.

## Recommended next changes

1. **Typed UObject references and native structs.** The low-level UObjectHandle
   form spreads native identity between ue_object_value(path), arg_name_class and
   return_class. UEStructValue/UEEnumValue similarly use path annotations. Direct
   authored enums/structs already derive identity from actual declared types;
   use those in new high-level examples. For native objects/structs, connect the
   generated wrapper types to the authoring frontend and expose convenient type
   aliases or typemacro adapters. Reuse generated wrappers and weak-handle/value
   ownership rather than inventing an independent representation. A form such as
   `$UEObject<Actor>` is a design sketch; it is not implemented or a verified
   complete declaration today. Keep existing raw-handle annotations as a low-level
   compatibility API.

2. **Typed identity resolution with explicit scope.** Native parent paths,
   authored enum/struct registry IDs, interface IDs and wrapper constraints
   currently have separate resolvers. Share identity/ambiguity/error rules, but
   preserve the explicit enum/struct catalog and its cook hard references. Do not
   replace scoped type_registry resolution with an unrestricted global search.
   Short `parent=Actor` could use native binding metadata; parent support must
   still respect the current Actor-only backend and deferred inheritance work.

3. **Lists of parameter directions.** Single `ref=counter`/`out=total` values
   can use ordinary identifier arguments, which the parser represents as strings.
   Multiple directions currently use CSV strings. Native tuple-valued annotation
   syntax could allow `ref=(counter,weights)`; it flattens to repeated arguments
   and requires a new collector. The current collector calls find() by name and
   repeatedly processes its first value, so tuple support must not be advertised
   yet. Retain explicit directions for compatibility; automatic inout inference
   would change existing contracts and needs a separate decision.

4. **Singleton annotations and redundant constraints.** ue_implements is
   deliberately repeatable. Several other collectors use DSLAnnotation(), which
   returns the first match, and do not explicitly reject repeated ue_function or
   ue_class. Define and test singleton rules. Direct enum/struct parameters take
   their type path from the declaration while removing arg_name_enum/struct
   entries; an explicit redundant constraint can therefore be ignored. Reject
   conflicting constraints instead of accepting an apparently effective option.

5. **Signature-only interfaces — implemented.** ue_interface now accepts ordinary
   empty methods with ue_function metadata/direction annotations. The macro makes
   them compile for non-void results and installs a direct-call trap. Nonempty
   bodies reject; legacy abstract declarations remain readable. See
   DSLAuthoredInterfaces.md. No reader or parser modification is needed.

## Forms to keep

- ue_class, ue_function and per-field @ue_property metadata use normal daScript
  grammar and already match the chosen annotation-only DSL.
- Authored enum/struct declarations, native array/table types, normal method
  signatures and source-level member access do not need wholesale replacement.
- Optional explicit stable IDs remain useful for renames and migration fixtures;
  they should not be removed from compatibility tests.
- Metadata strings (Category/DisplayName/ToolTip), asset paths and registry paths
  remain strings. Type macros do not permit arbitrary type expressions inside
  ordinary annotation arguments.
- Old native wrapper examples are low-level compatibility coverage; keep them
  alongside direct-type examples rather than mechanically rewriting all fixtures.

Generated typed interface references/calls remain a future plan item. Universal
migration work stays deferred, as requested by the user.


## Validation of the parameter-name fix

- TestEditor.ps1: 439 checks, zero failures.
- TestDSLTypeRegistry.ps1 -AuthoredInterface -Cooked: PASS, including lowercase
  source parameter identity, native interface graph creation, reopen/reimport and
  six packaged runtime checks. Multiple-interface and direction tests still pass.
- The syntax recommendations above are proposals, not implemented new syntax.
- Evidence: workspace work/dsl-audit-foundation.log and work/dsl-audit-interfaces.log.


## Implemented follow-up: typed native UObject declarations

The first recommendation now has a native-object milestone: UEObject returns the
existing generated wrapper, with separate-module concrete aliases for member
methods. See DSLTypedObjects.md for actual supported syntax and parser limitations.
Broader native structs, shared registry resolution, direction lists and singleton
annotation policies remain follow-up work; this does not implement all proposals.


## Implemented follow-up: shared identity and annotation rules

Recommendations 2–4 now have implemented core support. Source type references
share canonical path/kind validation; parent/native/interface names and explicit
enum/struct registry IDs are supported without removing catalog scope. Tuple
out/ref lists share one collector with legacy CSV compatibility. Repeated
ue_function/scalar options and conflicting inferred/explicit native type
constraints reject. See DSLTypeResolution.md for current syntax and scope rules.
