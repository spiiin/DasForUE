options gen2
module ue_struct_map_types public
require ue_dsl public
require ue_object_types public

[ue_struct(module_id="structmaps", BlueprintType)]
struct MapEntry {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Score:int=0
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Position:UEVector=ue_vector(0.0lf,0.0lf,0.0lf)
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Samples:array<double>
}

[ue_struct(module_id="structmaps", BlueprintType)]
struct MapCatalog {
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Entries:table<string;MapEntry>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Positions:table<int;UEVector>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Rotations:table<string;UERotator>
    @ue_property=(EditAnywhere, BlueprintReadWrite)
    Transforms:table<int;UETransform>
}

[ue_enum(module_id="structmaps", BlueprintType)]
enum MapStatus {
    Waiting=7
    Active=42
}

[ue_struct(module_id="structmaps", BlueprintType)]
struct ReferenceMapCatalog {
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
}
