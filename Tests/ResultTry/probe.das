options gen2
require ue_result
require daslib/defer

def counted(var calls : int&; fail : bool) : Result<int; string> {
    calls++
    return fail ? err("original error", type<int>) : ok(42, type<string>)
}

def propagate(var calls, cleanup : int&; fail : bool) : Result<string; string> {
    defer() { cleanup++ }
    let value = counted(calls, fail) |> ue_try
    ue_require(value == 42, "wrong value") |> ue_try
    return ok("done", type<string>)
}

def assign(var target, calls : int&; fail : bool) : Result<bool; string> {
    target = counted(calls, fail) |> ue_try
    return ok(true, type<string>)
}

def moves() : Result<bool; string> {
    var source <- array<int>(7, 11)
    var result <- move_ok(source, type<string>)
    let values <- result |> ue_try
    verify(empty(source) && empty(result._value))
    verify(length(values) == 2 && values[1] == 11)
    var target : array<int>
    var next <- array<int>(13, 17)
    target <- move_ok(next, type<string>) |> ue_try
    verify(empty(next) && target[0] == 13)
    let optional = ok(none(type<int>), type<string>) |> ue_try
    let falseValue = ok(false, type<string>) |> ue_try
    verify(is_none(optional) && !falseValue)
    return ok(true, type<string>)
}

def branch_loop(var calls : int&) : Result<bool; string> {
    if (false) { counted(calls, true) |> ue_try }
    for (i in range(3)) {
        let value = counted(calls, i == 1) |> ue_try
        verify(value == 42)
    }
    return ok(true, type<string>)
}

def run_inner(body : block<():Result<bool; string>>) : Result<bool; string> { return body() }

def nested() : Result<int; string> {
    let inner = run_inner() {
        ue_require(false, "inner") |> ue_try
        return ok(true, type<string>)
    }
    verify(is_err(inner) && unwrap_err(inner) == "inner")
    return ok(99, type<string>)
}

enum TestFailure {
    Denied
    Missing
}

def enum_guard() : Result<bool; TestFailure> {
    ue_require(false, TestFailure.Denied) |> ue_try
    return ok(true, type<TestFailure>)
}

def typed_error() : Result<bool; int> {
    err(23, type<float>) |> ue_try
    return ok(true, type<int>)
}

[export]
def main(input : int) : int {
    var calls = 0
    var cleanup = 0
    let failure = propagate(calls, cleanup, true)
    verify(is_err(failure) && unwrap_err(failure) == "original error")
    verify(calls == 1 && cleanup == 1)
    verify(unwrap(propagate(calls, cleanup, false)) == "done")
    verify(calls == 2 && cleanup == 2)
    var target = 9
    verify(is_err(assign(target, calls, true)) && target == 9 && calls == 3)
    verify(is_ok(assign(target, calls, false)) && target == 42 && calls == 4)
    verify(is_ok(moves()))
    calls = 0
    verify(is_err(branch_loop(calls)) && calls == 2)
    verify(unwrap(nested()) == 99)
    verify(unwrap_err(typed_error()) == 23)
    verify(unwrap_err(enum_guard()) == TestFailure.Denied)
    let owner = ue_ref(type<ActorRef>, ue_self_object())
    verify(is_ok(ue_require_valid(owner, TestFailure.Missing)))
    verify(is_err(ue_require_valid(default<ActorRef>, "missing")))
    return input + 73
}
