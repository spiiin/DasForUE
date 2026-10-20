options gen2
module ue_gas_authored_types public
require ue_dsl public
require ue_object_types public

[ue_native_object(path="/Script/DasRuntime.DasAuthoredAbilitySystemComponent")]
struct AuthoredAbilitySystemComponentRef { handle:UObjectHandle }
def AuthoredAbilitySystemComponentRef_from(value:UObjectHandle):AuthoredAbilitySystemComponentRef {
    return AuthoredAbilitySystemComponentRef(handle=ue_cast_nullable_object(value,"/Script/DasRuntime.DasAuthoredAbilitySystemComponent"))
}
[ue_native_object(path="/Script/DasRuntime.DasAuthoredAttributeSet")]
struct AttributeSetRef { handle:UObjectHandle }
def AttributeSetRef_from(value:UObjectHandle):AttributeSetRef {
    return AttributeSetRef(handle=ue_cast_nullable_object(value,"/Script/DasRuntime.DasAuthoredAttributeSet"))
}
[ue_native_object(path="/Script/DasRuntime.DasAuthoredGameplayEffect")]
struct AuthoredGameplayEffectRef { handle:UObjectHandle }
def AuthoredGameplayEffectRef_from(value:UObjectHandle):AuthoredGameplayEffectRef {
    return AuthoredGameplayEffectRef(handle=ue_cast_nullable_object(value,"/Script/DasRuntime.DasAuthoredGameplayEffect"))
}
typedef AttributeSetClass = $UEClass<AttributeSetRef>
typedef AuthoredGameplayEffectClass = $UEClass<AuthoredGameplayEffectRef>

[ue_native_object(path="/Script/DasRuntime.DasAuthoredGameplayAbility")]
struct AuthoredGameplayAbilityRef { handle:UObjectHandle }

def AuthoredGameplayAbilityRef_from(value:UObjectHandle):AuthoredGameplayAbilityRef {
    return AuthoredGameplayAbilityRef(handle=ue_cast_nullable_object(value,"/Script/DasRuntime.DasAuthoredGameplayAbility"))
}
typedef AuthoredGameplayAbilityClass = $UEClass<AuthoredGameplayAbilityRef>
