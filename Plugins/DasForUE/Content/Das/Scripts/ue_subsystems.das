options gen2
module ue_subsystems public
require ue_dsl public
[ue_native_object(path="/Script/Engine.WorldSubsystem")]
struct WorldSubsystemRef { handle:UObjectHandle }
[ue_native_object(path="/Script/Engine.GameInstanceSubsystem")]
struct GameInstanceSubsystemRef { handle:UObjectHandle }
