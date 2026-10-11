options gen2
module ue_scene_components public
require ue_dsl public
require ue_object_types public

[ue_native_object(path="/Script/Engine.SceneComponent")]
struct SceneComponentRef { handle:UObjectHandle }
def SceneComponentRef_from(value:UObjectHandle):SceneComponentRef {
    return SceneComponentRef(handle=ue_cast_nullable_object(value,"/Script/Engine.SceneComponent"))
}
def SceneComponentRef_raw(value:SceneComponentRef):UObjectHandle {
    return ue_cast_nullable_object(value.handle,"/Script/Engine.SceneComponent")
}
def ue_scene_valid(value:SceneComponentRef):bool { return __ue_scene_valid(value.handle) }
def ue_scene_actor_root(actor:ActorRef):SceneComponentRef {
    return SceneComponentRef_from(__ue_scene_actor_root(ue_cast_nullable_object(actor.handle,"/Script/Engine.Actor")))
}
def ue_scene_parent(value:SceneComponentRef):SceneComponentRef {
    return SceneComponentRef_from(__ue_scene_parent(SceneComponentRef_raw(value)))
}
def ue_scene_relative_transform(value:SceneComponentRef):UETransform { return __ue_scene_relative_transform(SceneComponentRef_raw(value)) }
def ue_scene_world_transform(value:SceneComponentRef):UETransform { return __ue_scene_world_transform(SceneComponentRef_raw(value)) }
def ue_scene_set_relative_transform(value:SceneComponentRef; transform:UETransform; teleport:bool=false):void { __ue_scene_set_relative_transform(SceneComponentRef_raw(value),transform,teleport) }
def ue_scene_set_world_transform(value:SceneComponentRef; transform:UETransform; teleport:bool=false):void { __ue_scene_set_world_transform(SceneComponentRef_raw(value),transform,teleport) }
def ue_scene_attach(child:SceneComponentRef; parent:SceneComponentRef; socket:string=""; rule:string="KeepRelative"; weld:bool=false):bool { return __ue_scene_attach(SceneComponentRef_raw(child),SceneComponentRef_raw(parent),socket,rule,weld) }
def ue_scene_detach(value:SceneComponentRef; rule:string="KeepWorld"):void { __ue_scene_detach(SceneComponentRef_raw(value),rule) }
def ue_scene_create(actor:ActorRef; name:string):SceneComponentRef { return SceneComponentRef_from(__ue_scene_create(ue_cast_nullable_object(actor.handle,"/Script/Engine.Actor"),name)) }
def ue_scene_destroy(value:SceneComponentRef):bool { return __ue_scene_destroy(SceneComponentRef_raw(value)) }

def ue_scene_get(actor:ActorRef; name:string):SceneComponentRef { return SceneComponentRef_from(__ue_scene_get(ue_cast_nullable_object(actor.handle,"/Script/Engine.Actor"),name)) }

def ue_scene_get_or_create(actor:ActorRef; name:string):SceneComponentRef { return SceneComponentRef_from(__ue_scene_get_or_create(ue_cast_nullable_object(actor.handle,"/Script/Engine.Actor"),name)) }
