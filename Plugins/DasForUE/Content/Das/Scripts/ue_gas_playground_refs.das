options gen2
module ue_gas_playground_refs public
require ue_gas public

// Declaration-only link: typed access to the actor's editable effect classes and state.
[ue_object_ref(script="dsl_gas_ability_playground.das",type="AbilityPlayground")]
struct AbilityPlaygroundRef { handle:UObjectHandle }
