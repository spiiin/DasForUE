options gen2
module ue_soft_references public
require ue_object_types public
typedef SoftObject = $UESoftObject<ObjectRef>
typedef SoftActorClass = $UESoftClass<ActorRef>
// Reading a path and resolving an already loaded value never loads an asset.
def ue_soft_path(value:auto):string { return __ue_soft_path(ue_soft_value(value)) }
def ue_soft_is_null(value:auto):bool { return __ue_soft_null(ue_soft_value(value)) }
def ue_soft_is_loaded(value:auto):bool { return __ue_soft_loaded(ue_soft_value(value)) }
// Completion uses the owner's managed subscription lifetime and existing cancel API.
def ue_soft_load_async(name:string; value:auto; handler:string):void {
    __ue_soft_load_async(name,ue_soft_value(value),handler)
}
