# Captured timer arguments

Added 2026-10-10. **Not built or tested**. No build, example import, timer dispatch,
ABI/overload check, reload, PIE or packaged execution was run for this extension.

```das
ue_timer_start("score",1.0,"AddScore",true,5)

[ue_function(BlueprintCallable)]
def AddScore(amount:int):void {
    self.Score += amount
}
```

The fifth argument captures one value at start. Supported: int32, float32, bool,
double64, string, UENameValue, UEVector, UERotator and UETransform. The existing
four-argument form keeps its no-argument callback contract. Class callbacks are
annotated void instance methods with exactly zero or one supported by-value input;
short literal method names still lower to their exported entries. Ordinary exported
DAS functions can use the same overloads with their exported callback name.

Values are copied into the native timer record. Strings/names retain native value
storage, not pointers/tokens from the starting VM. Math values are native copies.
Every repeating delivery gets a copy of the original value; callback-local writes
cannot change the saved capture or the caller's variable. Replacing the named timer
is the way to update its captured value. No closure or variable-reference capture
is implied.

Handler return/input contract is checked before replacing an existing named timer.
Strings are bounded to 4096 TCHARs and reject embedded NUL; math rejects NaN/Inf,
and Transform requires normalized rotation. These checks also precede replacement.
The existing interval/world/owner/count checks and timer lifecycle remain.

Captured delivery uses persistent frame dispatch with one owned argument, retaining
the same callback error handling and required-entry contract registration. Accepted
reload cancels timers/captures; rejected candidate reload retains the old timer.
Callbacks are not rescheduled automatically and BeginPlay is not replayed.

Example: dsl_timer_arguments.das. It covers int, bool, string and Vector callbacks
declared after the starting lifecycle function. Float/double/name/Rotator/Transform
overloads are implemented but have no executed validation yet.

Not supported here: object/interface/struct/container/enum captures, multiple
arguments, ref/out callbacks, arbitrary closures, async work or new scheduling
policies. Those need explicit ownership/signature rules in a later slice.

Pending checks: all DAS overloads and native ABI forms; exact callback type/arity;
forward method naming; once/repeat/rearm/replace; captured copy semantics; strings,
names and math limits; failure preserves previous timer; nested dispatch; accepted
and rejected reload; owner/world teardown; PIE and packaged Development.

Primitive array captures were added subsequently for annotated class methods,
without build/tests; see ManagedTimerArrays.md. Earlier container exclusions
above describe the first value-capture slice.
