options gen2
require ue_foundation
[export]
def main(Input:int):int {
    let value = ue_enum_value("/Script/DasRuntime.EDasCallMode", "Active")
    var copied = value
    copied = ue_enum_from_number("/Script/DasRuntime.EDasCallMode", ue_enum_number(copied))
    return ue_enum_number(copied)
}
