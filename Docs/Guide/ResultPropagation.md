# Result propagation

Import `ue_result` to use standard `Result`/`Option` and the `ue_try` macro:

```das
require ue_result

def load_ready_system(target : ActorRef) : Result<RogueActionSystemComponentRef; string> {
    let system = ue_require_valid(ue_component_find(type<RogueActionSystemComponentRef>, target),
        "Missing action system") |> ue_try
    ue_require(system.Enabled, "Action system is disabled") |> ue_try
    return ok(system, type<string>)
}
```

The example also needs the module declaring `RogueActionSystemComponentRef`.
`ue_require(condition, error)` returns `Ok(true)` or `Err(error)`;
`ue_require_valid(value, error)` returns a valid object view or `Err(error)`.
The error may be a string, enum or another ordinary script value.
Each expression is evaluated once. `ue_try` extracts `Ok(value)` or returns the
existing error from the nearest Result-returning function/closure. That function
can have a different success type, but its error type must match. Ordinary compiler
checks enforce this; no error conversion, panic or native protected call is added.

`Ok(false)` and `Ok(None)` remain successful values. No configured effect and an
absent optional presentation profile should not be turned into errors implicitly.
Pointer/handle payloads keep their existing ownership rules: neither Result nor
the macro roots UObjects. Keep reflected strong references where required.

Supported uses are a standalone statement, the entire initializer of one local,
or the entire RHS of `=`/`<-` assignment to a local. Copyable results are copied;
move-only results are consumed. Use `<-` when extracting a move-only payload.
Unsupported nesting, field/index/compound assignments, taking the function address,
and use in `defer`/finally fail compilation. Use a separate local rather than
embedding `ue_try` inside arguments, arithmetic, conditions or returns. Normal
deferred cleanup runs on propagated returns; arbitrary panics retain their normal
language behavior. An explicit `return ok(...)` is still required for success.

The macro is adapted from dasSDL3's `sdl3_try.das`; its MIT copyright/license notice
is retained in `ue_result.das`. Expansion runs during typed inference and emits
ordinary temporaries, guards and returns. It does not modify reflected Blueprint
return signatures. For example, the sample's `CheckCanStart` returns a script-only
`Result<bool; RogueActionStartFailure>`, while Blueprint's `CanStart():bool` calls
`is_ok(self.CheckCanStart())`. Enum failures avoid allocating diagnostic strings
during frequent eligibility checks.

Use this for linear fallible helper workflows. Keep short boolean predicates,
normal absence and void lifecycle handlers simple; do not introduce a Result
adapter merely to replace every `if` statement.

Tests: `Scripts/RunDasTests.ps1 -Suite result-try` through the shared build agent.
The test runs against the actual embedded compiler, including negative cases.
