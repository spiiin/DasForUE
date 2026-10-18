options gen2
require ue_native_gameplay

// Run with an Actor owner. UE accessors are used, not writes to backing storage.
[export]
def main(input:int):int {
    let actor=ue_ref(type<ActorRef>,ue_self_object())
    actor.NetUpdateFrequency=37.5
    if(actor.NetUpdateFrequency!=37.5) { return -1 }
    let root=actor.RootComponent
    if(!ue_is_valid(root)) { return -2 }
    return 42
}
