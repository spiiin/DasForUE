# Source type references inside DSL methods

Reflected ue_class methods and reflected field initializers can refer to types
without embedding canonical Unreal paths. The authoring compiler resolves these
references in the enclosing class's module, imports and explicit registry/folder.
The native ABI, IR and generated wrappers continue to use canonical paths.

```das
[ue_class(parent=Actor, module_id="game", Blueprintable)]
class Example {
    [ue_function(BlueprintCallable, arg_target_class=Object)]
    def Supports(target:UObjectHandle):bool {
        return ue_implements_interface(target, ue_type(DasProbeInterface))
    }
    [ue_function(BlueprintCallable, arg_target_class=Object)]
    def Query(target:UObjectHandle):int {
        let call = ue_begin_interface_call(target, ue_type(DasProbeInterface), "Query")
        ue_arg_int(call, "Input", 5)
        ue_invoke_call(call)
        let result = ue_result_int(call, "ReturnValue")
        ue_release_call(call)
        return result
    }
}
```

ue_type is a source-only marker in the type-identity argument of supported calls;
it does not expose an Unreal pointer, allocate a value or create a runtime string
lookup. It accepts one identifier/alias or literal identity; arbitrary expressions
reject. ue_dsl::ue_type is also accepted. Qualified identities can be written as
ue_type("Engine.Actor") or ue_type("game.CounterContract"). The receiving call
determines the required type kind, so an Actor token passed as an interface rejects.

Supported call positions:

| Calls | Type argument |
| --- | --- |
| ue_object_value | first: native class |
| ue_cast_object, ue_cast_nullable_object, ue_object_is_a | second: native class |
| ue_begin_interface_call, ue_implements_interface, ue_interface_call_int/string | second: interface |
| ue_enum_value, ue_enum_from_number | first: enum |
| ue_struct_value, ue_struct_default | first: struct |

Explicit ue_foundation:: qualification of these calls also works. A literal short
string in the same position is resolved identically: ue_struct_value("Settings")
and ue_enum_value("State", "Idle"). Full-path strings retain the existing runtime
validation behavior; dynamic string arguments retain the existing low-level ABI.
ue_type(...) is stricter and validates even a full path during authoring.

Natural authored enum/struct fields/signatures and UEObject aliases remain the
preferred typed value API. The marker is useful when accessing the existing
snapshot and interface-frame API. This is not a typed interface-reference value
or an automatically generated method wrapper; those remain separate plan work.

Named authored enum/struct references require the existing explicit type_registry
or type_folder. A matching registry identity wins; missing/ambiguous/wrong-kind
authored references do not fall back to global authored assets. If no registry
entry, selected-folder asset or authored declaration matches, a uniquely matching native /Script type
can resolve even in a class with a registry/folder. Native ambiguity still rejects.
Resource paths for registry/folder and canonical generated binding metadata stay
explicit. A type token by itself does not add a cook dependency; assets must retain
the existing registry/property/signature hard references.

Scope is deliberately the reflected class method/initializer. Standalone helper
functions, unreflected methods and other modules do not inherit a class registry;
ue_type is not a globally callable runtime function. Interface membership checks,
signature validation, weak handles, frame release and VM reentry limits are unchanged.

Examples: dsl_type_calls.das, dsl_type_syntax_registered.das,
dsl_interface_implementation.das. Validation: TestEditor.ps1 and
TestDSLTypeRegistry.ps1 -TypeSyntax/-AuthoredInterface/-TypedObjects -Cooked.

Initial declaration collection erases marker syntax without resolving assets or
executing bodies. Full scoped resolution occurs when authoring/runtime compilation
loads the selected registry; this supports the first creation of authored types.

The execution probe also exposed and fixed enum-token local copying: UEEnumValue
now declares its uint64 value representation and matching external return carrier.
The existing context-owned enum identity/value representation is unchanged.
