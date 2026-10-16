# Computed class defaults

`ue_class` accepts scalar daScript expressions in property initializers and in
`ue_defaults` assignments. Ordinary scalar helper functions can participate:

```dascript
def starting_score(seed:int):int { return seed * 4 }

[ue_class(module_id="gameplay",Blueprintable)]
class Counter : Actor {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Seed:int=2+3
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Score:int=0
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    NextScore:int=0

    defaults=ue_defaults() {
        self.Score=starting_score(self.Seed*2)
        self.NextScore=self.Score+2
    }
}
```

This produces CDO values `Seed=5`, `Score=40`, `NextScore=42`. It works for
Actor and ActorComponent, including overrides of editable authored ancestor
properties. Existing property annotations and declaration groups share the same
path. An explicit defaults assignment overrides its inline initializer.

Supported results are `int`, `float`, `double`, `bool`, and `string`.
Mathematical constructors accept computed scalar coordinates; numeric coordinates
are explicitly converted to double. Reference markers accept computed string
paths, for example `ue_asset_default(asset_path())` and
`ue_class_default(actor_class_path())`. The resulting objects/classes still pass
the existing constraint and strong-reference validation. `ue_default_text` and
the text argument of `ue_struct_default` can use computed strings. Type identities
remain declaration metadata, not dynamic choices. A helper returning an entire
UEVector/UETransform/arbitrary reflected struct is not supported in this stage;
compose supported constructors from scalar helper results instead.

`self.Field` reads scalar source defaults, not a live UObject/CDO. Earlier assignments
in the defaults block are visible to subsequent assignments; other fields use
their source initializers or implicit scalar zero. Inherited source values are
available. Cycles and excessive expansion reject. Manual editor changes do not
become inputs to the calculation. Editable native scalar source reads are now supported by
[DSLNativeDefaults.md](DSLNativeDefaults.md). Reading component state or calling
instance methods through self is not supported.

## Execution and publication

Compilation first generates temporary evaluator functions with typed returns.
The collector disables optimizer execution of pure calls in that pass so it can
audit the actual helper graph. An isolated native daScript Context then executes
the approved calculations. Simulation skips global init/shutdown scripts; the
context has no Unreal owner. A second compilation substitutes scalar literals
and runs the existing metadata/type/default validators.

IR and saved Blueprint metadata contain only canonical resolved defaults. They
do not contain evaluator closures or VM addresses. Evaluators are absent from
the final compiled program. Pure calculations can run again when source is
compiled for validation/runtime; this does not reapply defaults to objects.
Spawned objects inherit Unreal's saved CDO/template values. Asset/class results
are saved as ordinary reflected references, so cook discovers their dependencies.

Reimport compares old and new resolved values and uses the existing transactional
defaults merge. Changes in helper code therefore update unchanged CDO/template/
instance values while preserving manual overrides. Parent-before-child import
rules, compatibility checks and rollback remain in force. A calculation/type/
reference error rejects before Blueprint publication.

## Limits

Calculations must be pure scalar code. Global state, UObject/World calls,
indirect calls, unsafe code, loops, recursion, allocation of user objects and
generators are rejected. Scalar locals and conditionals are allowed. Native
calls must be pure scalar functions from the builtin, math or strings modules.
The graph has a depth/expression budget; each context has bounded heap/string
storage. Nonfinite numbers and oversized results reject. This is a constrained
defaults facility, not a general sandbox for untrusted daScript source or macros.
Runtime instance work belongs in Initialize/ConstructionScript/BeginPlay.

Example: `Plugins/DasForUE/Content/Das/Scripts/dsl_computed_defaults.das`.
Integration test: `Scripts/TestComputedDefaults.ps1`, optionally `-Package`.


Validation completed: Editor Create/Reopen/Recompile; computed int/double/bool/
string/math/asset values; unchanged reimport and helper-result migration with
manual CDO/instance overrides; inherited properties, isolated siblings and
ActorComponent; source cycles, type mismatch, global state, side effects, loops,
recursion, World calls and division-by-zero refusal. Failed reimport preserves
the working class and metadata. Declaration-only interface collection skips
computed component defaults. First-launch global initializers do not execute
inside the defaults evaluator.

Existing inherited-default Create/Reopen/Recompile and safe-merge
Create/Reopen/Recompile/Unopened regressions passed. Editor and packaged foundation
report zero failures. Final Win64 Development NullRHI cook/package and runtime
probe passed: saved CDO/template values, strong asset dependency, child/sibling
isolation, ActorComponent, runtime source contracts and fresh Spawn. Shipping and
rendered gameplay were not exercised. Logs: computed-defaults-final-package-suite.log,
computed-defaults-packaged.log, computed-defaults-foundation-editor.log and
smoke-packaged.log under logs/.
