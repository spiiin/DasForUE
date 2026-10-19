# Functions

Methods of an authored class become Unreal functions when annotated with
`[ue_function(...)]`. Unannotated methods stay script-only helpers. This page
covers function kinds (callable, pure, event), `const`, out/ref parameters,
dispatch and overriding.

## Function kinds

```das
[ue_class(Blueprintable)]
class Counter : Actor {
    properties=ue_properties(EditAnywhere,BlueprintReadWrite) { var Score:int=0 }

    // Script-only helper: no Blueprint node or UFunction.
    def Add(amount:int):int { self.Score+=amount; return self.Score }

    [ue_function(BlueprintCallable)]
    def Increase(amount:int):int { return self.Add(amount) }

    // Default daScript body that child Blueprints may override.
    [ue_function(BlueprintEvent)]
    def Evaluate(input:int):int { return self.Score+input }

    [ue_function(BlueprintPure, Category="Queries")]
    def const GetScore():int { return self.Score }
}
```

| `ue_function` option | Effect |
| --- | --- |
| `BlueprintCallable` | Node with execution pins. Final: cannot be overridden |
| `BlueprintPure` | Data node without execution pins; implies callable |
| `BlueprintPure=false` | Keep execution pins explicitly |
| `BlueprintEvent` | Overridable function with a default daScript body; implies callable |
| `Category`, `DisplayName`, `ToolTip` | Metadata (a comment above the method also gives a tooltip) |
| `id="module.Class.Method"` | Explicit stable ID (normally automatic) |
| `ref=(...)`, `out=(...)` | Legacy direction lists, see below |
| `arg_<name>_class/enum/struct`, `return_class/enum/struct` | Type constraints for raw handle parameters, see [Modules and type identity](ModulesAndTypeIdentity.md) |

- `BlueprintEvent` is unrelated to `ue_event`, which subscribes a method to a
  delegate ([Delegates and events](DelegatesAndEvents.md)).
- An explicit `BlueprintCallable=false` together with `BlueprintPure` or
  `BlueprintEvent` is rejected, regardless of argument order. Repeating
  `ue_function` on one method, or repeating an option, is an error.
- Changing a method's role (callable/event/pure/const) is a declaration change,
  not a body-only reload. Reimport refuses such changes on existing Blueprints;
  use a new Blueprint.

Supported parameter and return types are listed in [Built-in types](BuiltinTypes.md),
[Structs](Structs.md), [Enums](Enums.md), [Containers](Containers.md) and
[Object and class references](ObjectAndClassReferences.md). A method frame holds
at most 16 parameters.

## Pure functions

```das
[ue_function(BlueprintPure)]
def Snapshot(@ue_out var value:int&):void { value=self.Score }

[ue_function(BlueprintCallable, BlueprintPure=false)]
def ReadWithExec():int { return self.Score }
```

- A pure function needs a return value or at least one out/ref parameter; a `void`
  function with only inputs is rejected.
- `BlueprintPure` does not imply `const` and does not prove the body is free of side
  effects. Blueprint may re-evaluate data nodes any number of times; there is no
  memoization. Do not start timers, emit events or change gameplay state in a pure
  getter.
- An overridable pure function combines `BlueprintPure` and `BlueprintEvent`.
  Overrides must keep purity; pure versus impure overrides are rejected.
- Interface stubs accept `BlueprintPure` too; implementations must repeat it
  (see [Interfaces](Interfaces.md)).

## Const methods

Use the daScript method qualifier `def const`:

```das
def const LocalScore():int { return self.Score }

[ue_function(BlueprintPure)]
def const GetScore():int { return self.LocalScore() }

[ue_function(BlueprintCallable, BlueprintPure=false)]
def const ReadWithExec():int { return self.Score }

[ue_function(BlueprintPure)]
def const Snapshot(@ue_out var value:int&):void { value=self.Score }
```

- Reflected const methods get `FUNC_Const`. Const and pure are independent: a const
  method keeps execution pins unless marked pure.
- Checked at compile time, on the original body: assignments and compound
  assignments to `self` paths, container mutators on `self`, explicit
  `ue_set_self_*` calls and calls to non-const `self`/`Super` methods or helpers are
  rejected. Const helpers get the same checks.
- Out/ref parameters can still be written; const applies to `self`.
- Overridable const methods combine `def const` with `BlueprintEvent`; overrides use
  `def override const`. Interface stubs can be `def const`; implementations must
  match.
- This is a source-level contract, not a runtime lock: it does not prevent changes
  through other references, low-level handles, external APIs or globals.

Example: `dsl_const_methods.das`.

## Out and ref parameters

```das
[ue_function(BlueprintCallable)]
def Adjust(var counter:int&; @ue_out var total:int&; amount:int):int {
    counter += amount
    total += counter * 2
    return counter
}
```

| Declaration | Direction | Blueprint pin |
| --- | --- | --- |
| `amount:int` | input | input |
| `var counter:int&` (or `@ue_ref var counter:int&`) | in/out: value copied in, updated value copied back | reference input + output |
| `@ue_out var total:int&` | output only: starts at the type's default, caller data ignored | output |

- Output-only defaults: zero/false/empty/`NAME_None`, null object/interface, the
  first valid enum member, the struct's loaded defaults, zero vector/rotator,
  identity transform.
- Supported types: primitives, `UENameValue`, standalone containers, typed object
  and interface references, authored enums and structs, constrained raw
  `UEEnumValue`/`UEStructValue`, and `UEVector`/`UERotator`/`UETransform`. Delegate
  out/ref is not supported. Container shapes follow the usual container rules.
- **Commit semantics:** outputs are published together only after the body returns
  normally and all outputs validate. On failure no outputs are published; Blueprint
  callers keep original ref values and get defaults for outs. Earlier writes to
  object fields are not rolled back.
- Each slot is an independent local copy; passing the same variable to two slots
  copies back in declaration order (no pointer aliasing). UObject identity is
  shared; struct/container storage is not.
- Const references, direction markers on value parameters, both `@ue_ref` and
  `@ue_out` on one parameter, repeated or false markers and unknown `ue_*`
  parameter annotations are rejected.
- The older form `ue_function(ref=(counter,weights), out=(total,message))` (or CSV
  strings) is still accepted. A list must agree with explicit markers; overlaps,
  unknown names and disagreements are rejected.
- C++ callers using the typed API (`FDasScalarValue.Direction`: 0 in, 1 out,
  2 inout) must check the success flag before reading `OutputValues`.

Examples: `dsl_out_ref.das`, `dsl_parameter_directions.das`. Try it:
`Scripts/TestDSLTypeRegistry.ps1 -OutRef`.

## Parameter defaults

Trailing input parameters may have literal defaults (`int`, `float`, `double`,
`bool`, `string`); omitted arguments work in daScript calls, and disconnected
Blueprint pins start with the declared value:

```das
[ue_function(BlueprintPure)]
def Add(left:int=7; right:int=-2):int { return left+right }
```

See [Defaults](Defaults.md#parameter-defaults) for the full rules.

## Calls and dispatch

- **Own helpers and reflected methods** called as `self.Name(...)` from methods,
  helpers or lifecycle hooks are direct same-context daScript calls. Recursion,
  mutable ref arguments and arrays work.
- **Script-only helpers** are not exported and not part of the authoring IR. They
  must be instance methods with value or void results; static helpers and
  reference results are rejected. Named-argument calls are not supported.
- **Events:** `self.Evaluate(...)` on a `BlueprintEvent` routes through Unreal's
  `ProcessEvent`, so a child Blueprint override receives the call. Without an
  override the default daScript body runs; it may be empty for a `void` function.
- **Blueprint overrides:** in a child Blueprint, override the function and use
  **Add Call to Parent Function** to run the inherited daScript body. Omitting that
  node replaces the body completely. Names and signatures must match.
- **Authored daScript children** override with `def override` and call
  `Super.Method(...)`; see [Inheritance](Inheritance.md#overriding-methods).
- Calls on other objects use typed references:
  [Authored references](AuthoredReferences.md).
- Native inherited methods (for example `self.Jump()` on a Character) are
  available through `self`; authored methods take precedence. See
  [Self access](SelfAccess.md).

Example: `dsl_method_dispatch_probe.das`. Try it: `Scripts/TestMethodDispatchPIE.ps1`.

Static functions belong in [Function libraries](FunctionLibraries.md).

See also: [Classes](Classes.md) · [Inheritance](Inheritance.md) ·
[Defaults](Defaults.md) · [Self access](SelfAccess.md) · [Interfaces](Interfaces.md) ·
[Authored references](AuthoredReferences.md)
