# BlueprintPure functions in the authoring DSL

Authored Actors and ActorComponents accept `BlueprintPure` on `ue_function`.
Blueprint presents these calls as data nodes without execution pins. The same
reflected dispatch, typed references and input/output marshalling are used as for
other authored functions; pure calls do not introduce a second implementation.

```das
[ue_function(BlueprintPure, Category="Queries")]
def GetScore():int { return self.Score }

[ue_function(BlueprintPure)]
def Snapshot(@ue_out var value:int&):void { value=self.Score }

[ue_function(BlueprintCallable, BlueprintPure=false)]
def ReadWithExec():int { return self.Score }
```

`BlueprintPure` implies BlueprintCallable. Explicit `BlueprintCallable=false`
conflicts with `BlueprintPure=true` and is rejected independently of annotation
argument order. `BlueprintPure=false` requires an ordinary callable/event declaration;
it retains execution pins. A pure function must have a return value or at least one
out/ref parameter. Void functions with inputs only are rejected in both DSL/IR and
adapter schema validation.

`BlueprintPure` does not imply `const` and is not a proof that the DAS body is free
from side effects. Authors should keep pure bodies observational: Blueprint may
reevaluate data nodes, and no memoization or once-per-frame guarantee is added.
Do not use a pure getter to start timers, emit events or mutate gameplay state.

## Inheritance and persistence

Ordinary pure methods remain final under the existing callable policy. Combine
`BlueprintPure` and `BlueprintEvent` when an authored/Blueprint child should be able
to override a pure method. The override must preserve purity, result and parameter
contracts. Explicit `Super.Method(...)` uses the parent body through normal reflected
dispatch. Pure versus impure overrides are rejected, rather than changing the
parent's Blueprint calling convention.

IR already contains the pure flag; the adapter schema now carries it into generated
functions. Entry metadata preserves the flag through skeleton/full Blueprint
recompilation, saved-asset reopening and cook. Runtime owner checks reject a source
contract whose purity disagrees with the loaded UFunction. Purity changes are
structural changes, not a body-only reload; existing controlled refusal rules apply.

This slice covers instance methods on Actors/ActorComponents, including overridable
pure functions and supported out/ref values. Static functions remain separate work. Const methods use def const independently
of purity; see DSLConstMethods.md. Pure UInterface contracts are supported as described below.

Examples: ReadScore/ReadImpure/Query in ue_inheritance_base.das, the Query override in
dsl_inheritance_child.das, and Snapshot/ChildStatus in dsl_life_behavior_child.das.
The generated test Blueprint EvaluatePure consumes the pure Query node's output
without connecting execution pins to that node.


## Validation — 2026-10-16

Editor foundation checks pass with zero failures, including DSL/IR/schema purity,
JSON roundtrip and rejection of void-only, non-callable and impure override cases.
Create/Reopen checks preserve flags and execute a saved child Blueprint data-node
consumer through an authored override and Super. Real RuntimePIE passes with one
success and zero warnings/failures. Win64 Development packaged NullRHI gameplay
and packaged foundation pass, including the saved pure Blueprint consumer and a
void pure component method with an out parameter.


## Interface contracts

Authored interface declaration stubs accept BlueprintPure, including void methods
with an out/ref result. Implementations must explicitly repeat the purity contract:

```das
[ue_interface(BlueprintType)]
class ScoreContract {
    [ue_function(BlueprintPure)]
    def Query(input:int):int {}
}

[ue_class(Blueprintable), ue_implements(interface=ScoreContract)]
class ScoreActor {
    [ue_function(BlueprintPure)]
    def Query(input:int):int { return input+7 }
}
```

Create/register the interface asset before importing the implementation, following
DSLAuthoredInterfaces.md. Loaded native Blueprint interfaces with pure functions
use the same matching rule. Pure/impure mismatches reject before Blueprint changes.
Interface functions stay overridable, preserve their flag during independent
recompilation/save/cook and use the existing interface dispatch and typed wrappers.
Empty declaration bodies still trap on direct DAS stub calls. Changing an existing
interface between pure and impure is structural and remains controlled refusal.
BlueprintPure does not imply const or enforce absence of side effects.


Interface validation — 2026-10-16: editor foundation passes with zero failures,
including pure return/out signatures, explicit impure opt-out, JSON roundtrip,
void-without-output and callable=false rejection. Authored interface Create/Reopen
and Win64 Development NullRHI cook/stage/game pass. Checks cover independent
interface recompilation, purity mismatch rejection before asset mutation, a real
Blueprint pure data node targeting the interface UFunction, preserved cooked flags
and generated DAS interface wrappers against a Blueprint receiver.
