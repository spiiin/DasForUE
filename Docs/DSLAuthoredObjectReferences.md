# Typed access to another authored object's fields

Declare a nominal reference once in a reusable module:

```das
module ue_actor_refs public
require ue_dsl public
[ue_object_ref(script="dsl_actor_component_owner.das", type="ComponentOwner")]
struct ComponentOwnerRef { handle : UObjectHandle }
```

Use its known fields in a ue_class method or lifecycle/event callback:

```das
require ue_actor_refs
// Inside an ActorComponent Tick method:
let actor = ue_ref(type<ComponentOwnerRef>, ue_component_owner(ue_self_object()))
actor.Score += 1
let score = actor.Score
```

There are no user-written property-name strings or get/set calls in those accesses.
The reference contains only a context-local weak handle. It does not store copies
of Score or other Unreal fields. Compiler rewriting emits checked runtime reads and
writes; the live target receives the assignment. Const local bindings such as let
actor keep the reference fixed while allowing writes to writable Unreal fields.

Fields and read-only rules come from the named source class. The reference annotation
uses a local plugin script filename and a short class name; full Unreal paths and
explicit stable IDs are unnecessary. Definitions resolve when used, including
references to another instance of the current class. Current class fields are made
available during compilation so an own-type reference does not cause recursive
source compilation. Dependencies read through other source declarations participate
in runtime source snapshots/reload detection.

Supported direct fields: int, float, double, bool, string and UENameValue/FName.
Reads, whole-field assignments and +=/-=/*=/ /= are lowered through shared adapters.
Local aliases, explicit reference local types, lexical scopes and typed globals in
the same source module are supported. A typedef can alias an existing reference
type. Different authored classes need their own nominal reference declarations;
adding a class does not require adding a C++ field adapter or repeating its fields.

Each operation checks handle ownership/liveness and the cooked authored class ID,
then resolves the reflected field again. No FProperty pointer is retained across
reinstancing. Wrong class IDs, forged handles, null/destroyed references and runtime
field-kind/permission mismatches reject. Unknown fields and BlueprintReadOnly
assignments reject at compilation. The reference does not keep its target alive;
existing native input frames temporarily pin their object inputs during a call.

New imports store the hidden identity on their generated CDO; it survives save/cook.
Previously imported Blueprints can receive the identity on validated body-only
reimport without compilation. They should be reimported and saved before packaging
with this feature. An existing nonempty conflicting identity is refused.

Scope: authored Actor/ActorComponent views in DSL bodies and ordinary functions.
Primitive/Name and whole enum/struct/object fields use common property operators.
Component member views and BlueprintCallable method calls are supported; see
DSLAuthoredMethodCalls.md. Container/nested mutation, native class property views
and reflected signatures containing authored reference types remain separate work.
No per-function annotation or fake in-memory fields are required.
The existing UObjectHandle and native ActorRef APIs remain available.

Examples: dsl_component_behavior.das, ue_actor_refs.das, dsl_ref_scalar_driver.das,
ue_scalar_refs.das and dsl_ref_same_class.das under Content/Das/Scripts.

Validation: [DSLAuthoredObjectReferencesValidation](DSLAuthoredObjectReferencesValidation.md).

Whole enum/struct/object fields have shared adapters now; see
[DSLAuthoredReferenceValues](DSLAuthoredReferenceValues.md) for representation and limits.

## Component owner shorthand

Inside ActorComponent DSL methods and lifecycle/event callbacks, use:

```das
let actor = ue_owner(type<ComponentOwnerRef>)
actor.Score += 1
```

This lowers to ue_ref(type<ComponentOwnerRef>,
ue_component_owner(ue_self_object())). It retains the same weak handle,
authored class identity and field/liveness checks. It needs no new runtime ABI
or exported Blueprint function. The argument must be an authored reference type
whose declaration is Actor-derived. Bad argument counts/types reject at compilation; a different live owner identity rejects at runtime.
The explicit ue_ref form remains available for arbitrary object handles.

Shorthand validation — 2026-10-10: Create/Reopen/cooked PASS with the shortened
owner expression. Component-context/type/argument errors reject during compilation;
live owner access and wrong authored identity checks PASS. Actual PIE: one success,
zero warnings/failures/skips. Smoke: 524 PASS, failures=0. Native binding
regeneration -Check PASS; generator 84 tests PASS; git diff --check PASS;
ThirdParty unchanged. Changes are not committed. Evidence: actor-component logs
and PIE report, smoke-editor.log and bindings-export.log under logs/.

The former class-only access rewrite has been replaced; see [DSLUniversalAccessors](DSLUniversalAccessors.md).
