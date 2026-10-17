options gen2
require ue_native_gameplay

// CapsuleComponent was never a receiver in the former C++ class list.
// Its declaration is discovered through ue_character_control automatically.
[export]
def main(input:int):int {
    let character=ue_character_self()
    let capsule=character.CapsuleComponent
    verify(ue_is_valid(capsule),"Discovered capsule reference")
    capsule.SetCapsuleRadius(41.0f,true)
    verify(capsule.GetUnscaledCapsuleRadius()==41.0f,"Discovered capsule methods")
    verify(capsule.GetScaledCapsuleRadius()==41.0f,"Discovered capsule result")
    return input+1
}
