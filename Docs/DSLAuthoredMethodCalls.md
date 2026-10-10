# Typed authored method calls

```das
require ue_actor_refs
require ue_component_types

let actor = ue_owner(type<ComponentOwnerRef>)
let result = actor.Behavior.Evaluate(5)

var value = 10
let changed = actor.Behavior.Accumulate(value)

var total = -1
actor.Behavior.MakeScore(5, total)
```

The declaration supplies the method name, argument types, directions and result.
The caller does not specify a file, entry point, stable ID or marshalling routine.
The existing ue_object_ref declaration still identifies the target class source.
Methods must be annotated ue_function(BlueprintCallable).

```das
[ue_function(BlueprintCallable)]
def Accumulate(var value:int&):int {
    value += self.Bonus
    return value
}

[ue_function(BlueprintCallable)]
def MakeScore(input:int; @ue_out var total:int&):void {
    total = self.Bonus + input
}
```

Ordinary functions have exactly the same syntax and overload resolution:

```das
def evaluate_component(component:ScoreComponent; input:int):int {
    return component.Evaluate(input)
}
```

Component members also resolve through ordinary reference property operators
(e.g. actor.Behavior). Import the component view module; automatic view imports
remain subsequent work. Component-reference replacement is not supported by these
operators.

The inference pass emits public receiver-first functions. daScript's standard
method syntax resolves to those overloads; the receiver is evaluated once.
Unknown methods, incorrect argument count/type and immutable ref/out inputs are
compile errors. All declaration method signatures are collected before body
rewriting, which permits mutually referring Actor/Component sources.

Runtime dispatch uses the existing reflected UFunction call frame and ProcessEvent,
preserving Blueprint dispatch. Every call checks weak-handle liveness, authored
class identity and the current reflected parameter/return kinds, names, directions
and enum/struct/object identities. It does not retain a reflected property address
across calls. An incompatible reflected signature rejects before invocation.

Supported wrapper values are int, float, double, bool, string, UENameValue,
UEEnumValue, UEStructValue and UObjectHandle; void results are supported too.
Enum/struct/object identity constraints remain checked at runtime. Structs use
native snapshots, including nested struct fields and reflected object pins.
Authored enum/struct signatures now retain their natural source types; see
DSLMethodSourceValues.md. Container signatures and fields now use owned snapshots; see DSLMethodContainers.md.
Native object/interface and math signatures retain their source types; see
DSLMethodObjectMath.md. Unsupported
container shapes reject through the existing carrier policy.

ref/out arguments use copy-in/copy-out. out starts with the native default; its old
caller value is not sent to the callee. Return and all outputs are read before any
caller variables are changed. Failed calls preserve caller ref/out variables.
This is not a transaction over Unreal state: callee field writes already performed
are not rolled back. Aliased ref/out variables publish in declaration order.
Call-frame cleanup also runs during exception recovery.

Checked authored dispatch permits nested persistent execution, including a call
back into a running Actor or Component and same-context calls. It preserves the
active interpreter stack, context-local handles, pending call frames and output
frame. It never restarts or reloads a context while it is executing. Source changes
during such reentry reject until the outer execution ends. Nested errors return
to the caller DAS VM so try/recover can handle them without aborting the enclosing
Blueprint. A maximum of 32 nested authored dispatches bounds accidental recursion.
Events remain queued until the outer execution boundary; teardown still revokes
sessions and subscriptions. Other arbitrary nested native/interface call paths
remain guarded.

Working examples: dsl_actor_component_owner.das and dsl_component_behavior.das.
The latter also contains an ordinary function using the generated method overload.
General structural migration remains deferred.

Validation: [DSLAuthoredMethodCallsValidation](DSLAuthoredMethodCallsValidation.md).

Native object/interface and math method signatures:
[DSLMethodObjectMath](DSLMethodObjectMath.md).
