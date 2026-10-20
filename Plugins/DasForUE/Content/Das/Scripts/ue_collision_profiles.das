options gen2
module ue_collision_profiles public
require ue_foundation public
// Configuration identifiers, not reflected UEnums. Convert before crossing a UE API boundary.
def ue_collision_profile_name(profile:CollisionProfile):UENameValue {
    return __ue_collision_profile_name(int64(profile))
}
