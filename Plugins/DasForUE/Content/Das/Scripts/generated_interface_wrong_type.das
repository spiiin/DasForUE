options gen2
require ue_foundation
require ue_generated
[export]
def main(mode:int):int {
    let receiver = DasCollectionInterface_from(ue_self_object())
    return ue_probe_Query(receiver, 5)
}
