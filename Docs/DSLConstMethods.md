# Const authored instance methods

Use the native DAS method qualifier, not an extra ue_function specifier:

```das
require ue_dsl
[ue_class(Blueprintable)]
class Counter : Actor {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Score:int=10

    def const LocalScore():int { return self.Score }

    [ue_function(BlueprintPure)]
    def const GetScore():int { return self.LocalScore() }

    [ue_function(BlueprintCallable, BlueprintPure=false)]
    def const ReadWithExec():int { return self.Score }

    [ue_function(BlueprintPure)]
    def const Snapshot(@ue_out var value:int&):void { value=self.Score }
}
```

The qualifier is recorded as optional const=true in authoring IR, propagated to
adapter schemas and emitted as FUNC_Const. Old IR without const remains non-const.
Blueprint entry metadata reapplies the flag on recompilation. Runtime owner checks
require the loaded UFunction to agree with source. Changing const is structural;
existing controlled refusal policies apply instead of treating it as body-only
reload. No new native object layout or separate method execution backend is added.

Generated const graphs call internal const frame adapters, kept impure for explicit
execution sequencing. These forward to the same dispatch/session/marshalling code
as ordinary calls; diagnostics and VM infrastructure may still update.

Const and BlueprintPure are independent contracts. BlueprintPure alone is still
non-const. Const does not force a pure node: an explicit BlueprintPure=false keeps
execution pins. Out/ref parameters may receive outputs; const applies to self,
not to mutable parameters. A void pure method still needs an output.

The collector checks original method bodies before reflected access is lowered.
Assignments, compound assignments, nested/indexed self paths, supported container
mutators and explicit ue_set_self_* calls are rejected in const methods. Calls to
known non-const self/Super methods are rejected, including script-only helpers.
Const helper bodies receive the same checks. Native DAS const typing remains
active on source methods. Nested paths rooted in self are checked conservatively.

This is a source-level const contract, not a side-effect sandbox or runtime lock.
It does not prove observational purity, transitive immutability of referenced
objects, or absence of changes through explicit low-level handles, external APIs,
global state or separately aliased references. Keep BlueprintPure bodies free of
observable side effects as described in DSLPureFunctions.md.

## Overrides and interfaces

An overridable const method combines def const with BlueprintEvent. Authored
overrides use def override const; the const flag must match the parent alongside
purity and the rest of the signature. Explicit Super dispatch retains the existing
parent-selection semantics. Ordinary callable methods remain final.

Annotated empty interface stubs support def const too. The implementation must
repeat const; const and non-const interface declarations cannot be interchanged.
Signature checks reject mismatches before Blueprint mutation. Interface declaration
bodies remain empty markers; direct DAS calls to those stubs still trap.

Examples: Query/ReadImpure and LocalRead in ue_inheritance_base.das, Query in
dsl_inheritance_child.das, Snapshot/ChildStatus in dsl_life_behavior_child.das, and
Query in the authored interface registry definition/implementation fixtures.
The standalone dsl_const_methods.das demonstrates the basic declarations.


## Validation — 2026-10-16

Editor foundation passes with zero failures. Tests cover const IR/schema/JSON,
structural const changes, return/out signatures, explicit impure const methods,
and rejection of self assignment, container mutation, non-const calls/helpers,
explicit setters and non-const overrides. Create/Reopen preserves reflected flags
and executes a const override with Super and a script-only const helper; the pure
non-const getter remains distinct from the impure const getter. Authored interface
Create/Reopen and Win64 Development cook/game validate const interface flags,
Blueprint data-node execution, generated wrappers and mismatch preflight. Strict
RuntimePIE reports one success with zero warnings/failures. The full packaged
gameplay/foundation pipeline passes, including a const void/out component method.
