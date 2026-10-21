options gen2
module ue_gas_types public
require ue_dsl public
require ue_object_types public

// The checked native adapter is an ASC subclass with the starter health set.
[ue_native_object(path="/Script/DasRuntime.DasHealthAbilitySystemComponent")]
struct HealthAbilitySystemComponentRef { handle:UObjectHandle }
def HealthAbilitySystemComponentRef_from(value:UObjectHandle):HealthAbilitySystemComponentRef {
    return HealthAbilitySystemComponentRef(handle=ue_cast_nullable_object(value,"/Script/DasRuntime.DasHealthAbilitySystemComponent"))
}

[ue_native_enum(path="/Script/DasRuntime.EDasHealthAttribute")]
enum HealthAttribute {
    Health=0
    MaxHealth=1
}
[ue_native_enum(path="/Script/DasRuntime.EDasHealthEffect")]
enum HealthEffect {
    Heal=0
    Damage=1
}
