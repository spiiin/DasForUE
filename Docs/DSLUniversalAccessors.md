# Universal authored-reference properties

```das
require ue_scalar_refs

// Ordinary function: no ue_class or annotation required.
def add_score(actor:ScalarTargetRef; amount:int):int {
    actor.Number += amount
    return actor.Number
}
```

Named property operators are generated from the referenced class declaration.
They serve reads, assignment and supported compound assignment through standard
daScript property resolution. They work in ordinary functions, imported modules,
DSL methods/callbacks, globals and function parameters/returns of script references.
Let-bound references remain fixed while their writable Unreal fields can change.

The compiler resolves receiver types. The old FDSLAuthoredRefVisitor, lexical-local
type guesses, special assignment tracking and manual compound receiver expansion
have been removed. Each generated operator receives its receiver once and uses
the existing checked runtime field adapter. Readonly properties have a getter and
no setter. Enum/struct/object value ownership and native field checks remain.

ue_ref and ue_owner are shared call macros, usable in ordinary functions too.
ue_owner uses the current script execution object's Component context; using it
while executing on an Actor rejects through the existing runtime component check.
Its target type must still describe an authored Actor. Short native type identities
in value helpers normalize through the common compiler pass.

self remains a context bridge inside ue_class bodies: the DSL establishes its
Unreal execution owner and converts component members to typed references. Once
obtained, self.Behavior and ordinary reference values use the same operators.
The existing own-self struct/value conversion bridge is retained because source
classes are not directly laid out in Unreal memory; it is not an alternative
implementation for typed reference receiver access. Plain functions receive a
typed reference explicitly or call ue_ref; they do not gain an implicit self.

Declaration snapshots live only for the active compile chain. This permits own-type
and cyclic source declaration references without caching stale declarations between
reloads. Imported accessors reuse matching receiver/property definitions rather
than generating an ambiguous second overload.

Supported view fields include primitive/Name, whole enum/struct/object values
and typed component references.
Container/nested mutation, natural source mappings for opaque values, reflected
signature reference types remain separate work. Authored method dispatch is now
implemented; see DSLAuthoredMethodCalls.md.
General migrations remain deferred. Low-level ue_get_self/object APIs remain
native interop primitives, not a parallel user property-rewriting strategy.

Executable ordinary-function module: Content/Das/Scripts/ue_reference_functions.das.
dsl_ref_scalar_driver.das calls it from Blueprint-bound methods; the same tests
exercise live fields, identities, values, readonly and destroyed references.

Validation: [DSLUniversalAccessorsValidation](DSLUniversalAccessorsValidation.md).
