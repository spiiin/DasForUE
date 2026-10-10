options gen2
options gc
require ue_dsl
require ue_struct_map_types
require ue_struct_map_actor_refs

// Import types and dsl_struct_maps.das first into /Game/DasStructMaps.
[ue_class(module_id="structmaps", type_folder="/Game/DasStructMaps", Blueprintable)]
class ReflectedMapActor {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    States:table<string;MapStatus>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    StatesById:table<int;MapStatus>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Actors:table<string;ActorRef>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    ActorsById:table<int;ActorRef>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Events:table<string;DasEventInterface>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    EventsById:table<int;DasEventInterface>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    AuthoredActors:table<string;StructMapActorRef>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    AlternativeActors:table<string;AlternativeMapActorRef>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Catalog:ReferenceMapCatalog=ReferenceMapCatalog()
    @ue_property=(EditAnywhere, BlueprintReadOnly)
    LockedStates:table<string;MapStatus>

    [ue_function(BlueprintCallable)]
    def Activate(name:string) {
        insert(self.States,name,MapStatus.Active)
        self.Catalog.States[name] = MapStatus.Waiting
    }
    [ue_function(BlueprintCallable)]
    def PutActor(name:string; actor:ActorRef) {
        insert(self.Actors,name,actor)
    }
    [ue_function(BlueprintCallable)]
    def PutEvent(name:string; events:DasEventInterface) {
        insert(self.Events,name,events)
    }
    [ue_function(BlueprintCallable)]
    def CopyStates(values:table<int;MapStatus>):table<int;MapStatus> {
        return <- clone(values)
    }
    [ue_function(BlueprintCallable)]
    def CopyActors(values:table<string;ActorRef>):table<string;ActorRef> {
        return <- clone(values)
    }
    [ue_function(BlueprintCallable)]
    def CopyEvents(values:table<int;DasEventInterface>):table<int;DasEventInterface> {
        return <- clone(values)
    }
    [ue_function(BlueprintCallable)]
    def CopyAuthored(values:table<string;StructMapActorRef>):table<string;StructMapActorRef> {
        return <- clone(values)
    }
    [ue_function(BlueprintCallable)]
    def ReplaceStates(var values:table<string;MapStatus>&;
                      @ue_out var previous:table<string;MapStatus>&) {
        previous := self.States
        self.States := values
        clear(values)
    }
    [ue_function(BlueprintCallable)]
    def ReplaceActors(var values:table<string;ActorRef>&;
                      @ue_out var previous:table<string;ActorRef>&) {
        previous := self.Actors
        self.Actors := values
        clear(values)
    }
    [ue_function(BlueprintCallable)]
    def ReplaceAuthored(var values:table<string;StructMapActorRef>&;
                        @ue_out var previous:table<string;StructMapActorRef>&) {
        previous := self.AuthoredActors
        self.AuthoredActors := values
        clear(values)
    }
}
