options gen2
module ue_native_engine_values public
require ue_dsl public

// A nominal view of the existing native UEnum; no authored enum asset.
[ue_native_enum(path="/Script/Engine.ESpawnActorCollisionHandlingMethod")]
enum SpawnCollision {
    Undefined=0
    AlwaysSpawn=1
    AdjustIfPossibleButAlwaysSpawn=2
    AdjustIfPossibleButDontSpawnIfColliding=3
    DontSpawnIfColliding=4
}
