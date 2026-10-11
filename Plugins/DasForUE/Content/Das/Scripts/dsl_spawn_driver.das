options gen2
require ue_dsl
require ue_object_types
require ue_scene_components
[ue_object_ref(script="dsl_spawn_target.das",type="SpawnTarget")]
struct SpawnTargetRef { handle:UObjectHandle }
[ue_object_ref(script="dsl_dynamic_lifecycle.das",type="DynamicLife")]
struct SpawnLifeRef { handle:UObjectHandle }
[ue_class(module_id="spawndriver",Blueprintable)]
class SpawnDriver {
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    @ue_class_base="/Script/DasRuntime.DasProbeActor"
    SpawnClass:UEClassHandle
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    Spawned:SpawnTargetRef
    @ue_property=(EditAnywhere,BlueprintReadWrite)
    ForeignActor:ActorRef
    [ue_function(BlueprintCallable)]
    def Spawn(input:bool):bool {
        let owner=ue_ref(type<ActorRef>,ue_self_object())
        let pose=ue_transform(ue_vector(125.0lf,250.0lf,375.0lf),ue_rotator(0.0lf,30.0lf,0.0lf),ue_vector(1.0lf,1.0lf,1.0lf))
        self.Spawned=ue_actor_spawn(type<SpawnTargetRef>,self.SpawnClass,pose,owner,"AlwaysSpawn")
        return ue_object_valid(self.Spawned.handle) && self.Spawned.Begins==1
    }
    [ue_function(BlueprintCallable)]
    def Native(input:bool):bool {
        let klass=ue_class_loaded(type<ActorRef>,"/Script/Engine.Actor")
        let actor=ue_actor_spawn(type<ActorRef>,klass)
        let valid=ue_object_valid(actor.handle)
        return valid && ue_actor_destroy_handle(actor.handle) && !ue_object_valid(actor.handle)
    }
    [ue_function(BlueprintCallable)]
    def WrongClass(input:bool):bool {
        let klass=ue_class_loaded(type<ObjectRef>,"/Script/Engine.SceneComponent")
        let actor=ue_actor_spawn(type<ActorRef>,klass)
        return ue_object_valid(actor.handle)
    }
    [ue_function(BlueprintCallable)]
    def WrongCollision(input:bool):bool {
        let owner=ue_ref(type<ActorRef>,ue_self_object())
        let actor=ue_actor_spawn(type<SpawnTargetRef>,self.SpawnClass,ue_transform_identity(),owner,"Invalid")
        return ue_object_valid(actor.handle)
    }
    [ue_function(BlueprintCallable)]
    def ForeignDestroy(input:bool):bool { return ue_actor_destroy_handle(ue_self_object()) }
    [ue_function(BlueprintCallable)]
    def QueryEmpty(input:bool):bool {
        let actor=ue_ref(type<ActorRef>,ue_self_object())
        return !ue_object_valid(ue_actor_find(type<SpawnTargetRef>).handle) && length(ue_actor_find_all(type<SpawnTargetRef>))==0 && !ue_object_valid(ue_component_find(type<SpawnLifeRef>,actor).handle) && length(ue_component_find_all(type<SpawnLifeRef>,actor))==0
    }
    [ue_function(BlueprintCallable)]
    def QueryInterrupted(input:bool):bool {
        let snapshot=__ue_query_begin(ue_self_object(),"/Script/Engine.Actor","",false)
        panic("query unwind fixture")
        return false
    }
    [ue_function(BlueprintCallable)]
    def QueryForeign(input:bool):bool {
        let values=ue_component_find_all(type<SceneComponentRef>,self.ForeignActor)
        return length(values)>0
    }
    [ue_function(BlueprintCallable)]
    def SpawnExtra(input:bool):bool {
        let extra=ue_actor_spawn(type<SpawnTargetRef>,self.SpawnClass)
        return extra.Begins==1
    }
    [ue_function(BlueprintCallable)]
    def QueryMatches(input:bool):bool {
        let first=ue_actor_find(type<SpawnTargetRef>)
        let actors=ue_actor_find_all(type<SpawnTargetRef>)
        let raw=ue_ref(type<ActorRef>,self.Spawned.handle)
        let component=ue_component_find(type<SpawnLifeRef>,raw)
        let components=ue_component_find_all(type<SpawnLifeRef>,raw)
        let scenes=ue_component_find_all(type<SceneComponentRef>,raw)
        let allActors=ue_actor_find_all(type<ActorRef>)
        return ue_object_valid(first.handle) && first.Begins==1 && length(actors)==2 && actors[0].Begins==1 && actors[1].Begins==1 && component.Begins==1 && length(components)==1 && components[0].Begins==1 && length(scenes)>=1 && ue_scene_valid(scenes[0]) && length(allActors)>=3
    }
    [ue_function(BlueprintCallable)]
    def Remove(input:bool):bool {
        let actor=self.Spawned
        let raw=ue_ref(type<ActorRef>,actor.handle)
        let actors=ue_actor_find_all(type<SpawnTargetRef>)
        let components=ue_component_find_all(type<SpawnLifeRef>,raw)
        for (entry in actors) {
            ue_actor_destroy_handle(entry.handle)
        }
        return length(actors)==2 && !ue_object_valid(actor.handle) && !ue_object_valid(actors[0].handle) && !ue_object_valid(actors[1].handle) && !ue_object_valid(components[0].handle) && length(ue_actor_find_all(type<SpawnTargetRef>))==0
    }
}
