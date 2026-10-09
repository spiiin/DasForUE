# Registered values and generated authored interface bindings

Authored enum/struct method signatures now infer their identity through the explicit
ue_interface type_registry (or the existing exact type_folder convention), using the
same declaration ID, layout/default and GUID checks as ue_class direct values.

```das
require ue_dsl
require ue_interface_values
[ue_interface(module_id="contracts", BlueprintType,
              type_registry="/Game/Types/Registry.Registry")]
class ValueContract {
    [ue_function(BlueprintCallable)]
    def EchoState(value:InterfaceState):InterfaceState {}
    [ue_function(BlueprintCallable)]
    def EchoSettings(value:InterfaceSettings):InterfaceSettings {}
}
```

The implementation declares those concrete types too, with actual method bodies.
No arg_name_enum/struct or return_enum/struct annotation is required. Matching explicit
constraints remain accepted; conflicts and unused constraints reject. Registry and
folder remain mutually exclusive. Renames preserve the declaration's explicit stable
ID and registry binding; there is no global first-match authored type lookup.

Create value assets/registry first from a dedicated declaration module, then apply the
interface and implementing class. ue_interface_values.das demonstrates that module;
the registry definition/implementation examples are dsl_interface_registry_definition.das
and dsl_interface_registry_implementation.das. Their REGISTRY placeholder is replaced
by the test's actual isolated asset path. The interface stub compiler uses the first
declared enum member and normal struct initialization for its unreachable placeholder,
so enum value zero is not required. Direct calls still trap.

## Generating authored interface calls

The reflection exporter now accepts explicitly selected bridge-authored BPI generated
classes under /Game, carrying their stable interface ID into the IR. The editor
commandlet prepares selected BPI signature functions through the Blueprint compiler
in memory, without saving or changing their source contract. Native selection behavior
and required/optional signature policy remain supported.

Use Scripts/GenerateBindings.py with --module ue_contract_bindings to generate an
isolated public module. Existing invocations default to ue_generated. A minimal
selection is a functions array with class=/Game/.../ValueContract.ValueContract_C,
function=EchoState and alias=ue_contract_EchoState. Module names and authored stable
identity metadata are validated before publishing source.

```das
require ue_foundation
require ue_contract_bindings
let api = CounterContract_from(target)
let answer = ue_contract_Query(api, 5)
let changed = ue_contract_Stats(api, 10)
let state = ue_contract_EchoState(api, InterfaceState_from_symbol("Active"))
```

Actual names come from the selected interface and aliases; Query/Stats/etc are fixture
names. Friendly authored interface names omit the generated _C suffix. Nominal weak
receiver references check the exact authored class path and stable ID at module
compilation, then membership and declared/actual signatures at invocation. Moving or
changing a contract requires regeneration; an ID mismatch is not silently accepted.

Authored exported value types receive collision-safe short aliases and enum symbol
conversion helpers. Exported value structs retain the existing generated f_<field>
representation, including Unreal's internal member names; these are separate from
the natural source declaration module, not a promise of implicit conversion between
the two representations. The generator's owned marshalling handles the conversion to
native memory. A later convenience layer can unify these source value representations.

These references remain local values, not reflected TScriptInterface properties.
Object/math/enum/struct out/ref, structural interface changes, VM reentry and arbitrary
native Execute fallback remain deferred. Registry assets must retain real cook hard
references, e.g. the implementing Actor's TypeRegistry; strings/ue_type alone do not
create dependencies. Generated binding metadata does not substitute for those refs.

TestDSLTypeRegistry.ps1 -AuthoredInterface -Cooked now creates values/registry, a
16-method interface and its DSL implementation, exports five selected methods, creates
a separate module and calls it on a plain Blueprint receiver after reopening and in
the packaged game. GenerateAuthoredInterfaceProbe.py is test orchestration; production
generation uses the regular exporter and GenerateBindings.py --module.


The higher call layer now supports api.Query(...) and api.Stats(counter,total)
with automatic copyback. See DSLInterfaceMethodCalls.md. Value type representation
unification remains separate.
