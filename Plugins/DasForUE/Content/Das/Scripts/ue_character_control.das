options gen2
module ue_character_control public
require ue_native_self_refs public
require ue_enhanced_input public

[ue_native_object(path="/Script/Engine.Character")]
struct CharacterRef { handle:UObjectHandle }
def CharacterRef_from(value:UObjectHandle):CharacterRef { return CharacterRef(handle=ue_cast_nullable_object(value,"/Script/Engine.Character")) }

[ue_native_object(path="/Script/Engine.CharacterMovementComponent")]
struct CharacterMovementRef { handle:UObjectHandle }
def CharacterMovementRef_from(value:UObjectHandle):CharacterMovementRef { return CharacterMovementRef(handle=ue_cast_nullable_object(value,"/Script/Engine.CharacterMovementComponent")) }

[ue_native_object(path="/Script/Engine.CapsuleComponent")]
struct CapsuleComponentRef { handle:UObjectHandle }
def CapsuleComponentRef_from(value:UObjectHandle):CapsuleComponentRef { return CapsuleComponentRef(handle=ue_cast_nullable_object(value,"/Script/Engine.CapsuleComponent")) }

[ue_native_object(path="/Script/Engine.SkeletalMeshComponent")]
struct SkeletalMeshComponentRef { handle:UObjectHandle }
def SkeletalMeshComponentRef_from(value:UObjectHandle):SkeletalMeshComponentRef { return SkeletalMeshComponentRef(handle=ue_cast_nullable_object(value,"/Script/Engine.SkeletalMeshComponent")) }

[ue_native_object(path="/Script/Engine.CameraComponent")]
struct CameraComponentRef { handle:UObjectHandle }
def CameraComponentRef_from(value:UObjectHandle):CameraComponentRef { return CameraComponentRef(handle=ue_cast_nullable_object(value,"/Script/Engine.CameraComponent")) }

def ue_character_self():CharacterRef { return CharacterRef_from(ue_self_object()) }
def ue_pawn_controller(pawn:NativePawnRef):PlayerControllerRef { return PlayerControllerRef_from(__ue_pawn_controller(pawn.handle)) }
def ue_pawn_controller(pawn:CharacterRef):PlayerControllerRef { return PlayerControllerRef_from(__ue_pawn_controller(pawn.handle)) }
def ue_pawn_possess(player:PlayerControllerRef; pawn:NativePawnRef):bool { return __ue_pawn_possess(player.handle,pawn.handle) }
def ue_pawn_possess(player:PlayerControllerRef; pawn:CharacterRef):bool { return __ue_pawn_possess(player.handle,pawn.handle) }
def ue_character_controls_ready(pawn:CharacterRef):bool { return __ue_character_controls_ready(pawn.handle) }
// Forward/right are relative to controller yaw. Unreal applies movement scaling.
def ue_character_move(pawn:CharacterRef; forward:float; right:float):void { __ue_character_move(pawn.handle,forward,right) }
def ue_character_look(pawn:CharacterRef; yaw:float; pitch:float):void { __ue_character_look(pawn.handle,yaw,pitch) }
def ue_character_jump(pawn:CharacterRef; held:bool):void { __ue_character_jump(pawn.handle,held) }
