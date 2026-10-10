options gen2
options gc
require ue_example_catalog_refs

[export]
def catalog_put_position(target:UObjectHandle; index:int; position:UEVector) {
    let actor = ue_ref(type<StructMapActorRef>, target)
    actor.PutPosition(index, position)
}

[export]
def catalog_activate(target:UObjectHandle; name:string) {
    let actor = ue_ref(type<ReflectedMapActorRef>, target)
    actor.Activate(name)
}

// MapEntry and MapStatus are re-exported by the catalog reference module.
[export]
def catalog_put_entry(target:UObjectHandle; name:string; entry:MapEntry) {
    let actor = ue_ref(type<StructMapActorRef>, target)
    actor.Put(name, entry)
}

[export]
def catalog_copy_states(target:UObjectHandle; values:table<int;MapStatus>):table<int;MapStatus> {
    let actor = ue_ref(type<ReflectedMapActorRef>, target)
    return <- actor.CopyStates(values)
}
