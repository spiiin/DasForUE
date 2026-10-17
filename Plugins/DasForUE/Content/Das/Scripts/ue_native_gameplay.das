options gen2
module ue_native_gameplay public
require ue_character_control public
require ue_text public
require ue_native_hash_probe public

// Generated dependency views; methods/fields are synthesized from the compiled manifest.
[ue_native_object(path="/Script/Engine.ActorComponent")]
struct Native_ActorComponentRef_97441d45 { handle:UObjectHandle }
def Native_ActorComponentRef_97441d45_from(value:UObjectHandle):Native_ActorComponentRef_97441d45 { return Native_ActorComponentRef_97441d45(handle=ue_cast_nullable_object(value,"/Script/Engine.ActorComponent")) }

[ue_native_object(path="/Script/Engine.AnimMontage")]
struct Native_AnimMontageRef_e25467ee { handle:UObjectHandle }
def Native_AnimMontageRef_e25467ee_from(value:UObjectHandle):Native_AnimMontageRef_e25467ee { return Native_AnimMontageRef_e25467ee(handle=ue_cast_nullable_object(value,"/Script/Engine.AnimMontage")) }

[ue_native_object(path="/Script/Engine.CameraShakeSourceComponent")]
struct Native_CameraShakeSourceComponentRef_cf819c92 { handle:UObjectHandle }
def Native_CameraShakeSourceComponentRef_cf819c92_from(value:UObjectHandle):Native_CameraShakeSourceComponentRef_cf819c92 { return Native_CameraShakeSourceComponentRef_cf819c92(handle=ue_cast_nullable_object(value,"/Script/Engine.CameraShakeSourceComponent")) }

[ue_native_object(path="/Script/Engine.CheatManager")]
struct Native_CheatManagerRef_d4008399 { handle:UObjectHandle }
def Native_CheatManagerRef_d4008399_from(value:UObjectHandle):Native_CheatManagerRef_d4008399 { return Native_CheatManagerRef_d4008399(handle=ue_cast_nullable_object(value,"/Script/Engine.CheatManager")) }

[ue_native_object(path="/Script/Engine.ChildActorComponent")]
struct Native_ChildActorComponentRef_405d4379 { handle:UObjectHandle }
def Native_ChildActorComponentRef_405d4379_from(value:UObjectHandle):Native_ChildActorComponentRef_405d4379 { return Native_ChildActorComponentRef_405d4379(handle=ue_cast_nullable_object(value,"/Script/Engine.ChildActorComponent")) }

[ue_native_object(path="/Script/Engine.Controller")]
struct Native_ControllerRef_78a9db5e { handle:UObjectHandle }
def Native_ControllerRef_78a9db5e_from(value:UObjectHandle):Native_ControllerRef_78a9db5e { return Native_ControllerRef_78a9db5e(handle=ue_cast_nullable_object(value,"/Script/Engine.Controller")) }

[ue_native_object(path="/Script/Engine.Font")]
struct Native_FontRef_684a8ec8 { handle:UObjectHandle }
def Native_FontRef_684a8ec8_from(value:UObjectHandle):Native_FontRef_684a8ec8 { return Native_FontRef_684a8ec8(handle=ue_cast_nullable_object(value,"/Script/Engine.Font")) }

[ue_native_object(path="/Script/Engine.ForceFeedbackEffect")]
struct Native_ForceFeedbackEffectRef_551fc643 { handle:UObjectHandle }
def Native_ForceFeedbackEffectRef_551fc643_from(value:UObjectHandle):Native_ForceFeedbackEffectRef_551fc643 { return Native_ForceFeedbackEffectRef_551fc643(handle=ue_cast_nullable_object(value,"/Script/Engine.ForceFeedbackEffect")) }

[ue_native_object(path="/Script/Engine.HUD")]
struct Native_HUDRef_239f3d7c { handle:UObjectHandle }
def Native_HUDRef_239f3d7c_from(value:UObjectHandle):Native_HUDRef_239f3d7c { return Native_HUDRef_239f3d7c(handle=ue_cast_nullable_object(value,"/Script/Engine.HUD")) }

[ue_native_object(path="/Script/Engine.HapticFeedbackEffect_Base")]
struct Native_HapticFeedbackEffect_BaseRef_ab5298cf { handle:UObjectHandle }
def Native_HapticFeedbackEffect_BaseRef_ab5298cf_from(value:UObjectHandle):Native_HapticFeedbackEffect_BaseRef_ab5298cf { return Native_HapticFeedbackEffect_BaseRef_ab5298cf(handle=ue_cast_nullable_object(value,"/Script/Engine.HapticFeedbackEffect_Base")) }

[ue_native_object(path="/Script/Engine.Level")]
struct Native_LevelRef_d84e35d3 { handle:UObjectHandle }
def Native_LevelRef_d84e35d3_from(value:UObjectHandle):Native_LevelRef_d84e35d3 { return Native_LevelRef_d84e35d3(handle=ue_cast_nullable_object(value,"/Script/Engine.Level")) }

[ue_native_object(path="/Script/Engine.MaterialInstanceDynamic")]
struct Native_MaterialInstanceDynamicRef_65b20eca { handle:UObjectHandle }
def Native_MaterialInstanceDynamicRef_65b20eca_from(value:UObjectHandle):Native_MaterialInstanceDynamicRef_65b20eca { return Native_MaterialInstanceDynamicRef_65b20eca(handle=ue_cast_nullable_object(value,"/Script/Engine.MaterialInstanceDynamic")) }

[ue_native_object(path="/Script/Engine.MaterialInterface")]
struct Native_MaterialInterfaceRef_d64f3063 { handle:UObjectHandle }
def Native_MaterialInterfaceRef_d64f3063_from(value:UObjectHandle):Native_MaterialInterfaceRef_d64f3063 { return Native_MaterialInterfaceRef_d64f3063(handle=ue_cast_nullable_object(value,"/Script/Engine.MaterialInterface")) }

[ue_native_object(path="/Script/Engine.PawnMovementComponent")]
struct Native_PawnMovementComponentRef_653cf1d0 { handle:UObjectHandle }
def Native_PawnMovementComponentRef_653cf1d0_from(value:UObjectHandle):Native_PawnMovementComponentRef_653cf1d0 { return Native_PawnMovementComponentRef_653cf1d0(handle=ue_cast_nullable_object(value,"/Script/Engine.PawnMovementComponent")) }

[ue_native_object(path="/Script/Engine.PhysicsVolume")]
struct Native_PhysicsVolumeRef_d3600a0c { handle:UObjectHandle }
def Native_PhysicsVolumeRef_d3600a0c_from(value:UObjectHandle):Native_PhysicsVolumeRef_d3600a0c { return Native_PhysicsVolumeRef_d3600a0c(handle=ue_cast_nullable_object(value,"/Script/Engine.PhysicsVolume")) }

[ue_native_object(path="/Script/Engine.PlayerCameraManager")]
struct Native_PlayerCameraManagerRef_7d85a11d { handle:UObjectHandle }
def Native_PlayerCameraManagerRef_7d85a11d_from(value:UObjectHandle):Native_PlayerCameraManagerRef_7d85a11d { return Native_PlayerCameraManagerRef_7d85a11d(handle=ue_cast_nullable_object(value,"/Script/Engine.PlayerCameraManager")) }

[ue_native_object(path="/Script/Engine.PlayerState")]
struct Native_PlayerStateRef_bd340396 { handle:UObjectHandle }
def Native_PlayerStateRef_bd340396_from(value:UObjectHandle):Native_PlayerStateRef_bd340396 { return Native_PlayerStateRef_bd340396(handle=ue_cast_nullable_object(value,"/Script/Engine.PlayerState")) }

[ue_native_object(path="/Script/Engine.PrimitiveComponent")]
struct Native_PrimitiveComponentRef_5efdd477 { handle:UObjectHandle }
def Native_PrimitiveComponentRef_5efdd477_from(value:UObjectHandle):Native_PrimitiveComponentRef_5efdd477 { return Native_PrimitiveComponentRef_5efdd477(handle=ue_cast_nullable_object(value,"/Script/Engine.PrimitiveComponent")) }

[ue_native_object(path="/Script/Engine.RuntimeVirtualTexture")]
struct Native_RuntimeVirtualTextureRef_e53bf8a2 { handle:UObjectHandle }
def Native_RuntimeVirtualTextureRef_e53bf8a2_from(value:UObjectHandle):Native_RuntimeVirtualTextureRef_e53bf8a2 { return Native_RuntimeVirtualTextureRef_e53bf8a2(handle=ue_cast_nullable_object(value,"/Script/Engine.RuntimeVirtualTexture")) }

[ue_native_object(path="/Script/Engine.SpectatorPawn")]
struct Native_SpectatorPawnRef_c2962b49 { handle:UObjectHandle }
def Native_SpectatorPawnRef_c2962b49_from(value:UObjectHandle):Native_SpectatorPawnRef_c2962b49 { return Native_SpectatorPawnRef_c2962b49(handle=ue_cast_nullable_object(value,"/Script/Engine.SpectatorPawn")) }

[ue_native_object(path="/Script/Engine.TouchInterface")]
struct Native_TouchInterfaceRef_ae05e058 { handle:UObjectHandle }
def Native_TouchInterfaceRef_ae05e058_from(value:UObjectHandle):Native_TouchInterfaceRef_ae05e058 { return Native_TouchInterfaceRef_ae05e058(handle=ue_cast_nullable_object(value,"/Script/Engine.TouchInterface")) }

[ue_native_object(path="/Script/PhysicsCore.PhysicalMaterial")]
struct Native_PhysicalMaterialRef_b3257fa1 { handle:UObjectHandle }
def Native_PhysicalMaterialRef_b3257fa1_from(value:UObjectHandle):Native_PhysicalMaterialRef_b3257fa1 { return Native_PhysicalMaterialRef_b3257fa1(handle=ue_cast_nullable_object(value,"/Script/PhysicsCore.PhysicalMaterial")) }

[ue_native_object(path="/Script/UMG.UserWidget")]
struct Native_UserWidgetRef_a3d12281 { handle:UObjectHandle }
def Native_UserWidgetRef_a3d12281_from(value:UObjectHandle):Native_UserWidgetRef_a3d12281 { return Native_UserWidgetRef_a3d12281(handle=ue_cast_nullable_object(value,"/Script/UMG.UserWidget")) }

[ue_native_enum(path="/Script/CoreUObject.EMouseCursor")]
enum EMouseCursor {
    None=0
    Default=1
    TextEditBeam=2
    ResizeLeftRight=3
    ResizeUpDown=4
    ResizeSouthEast=5
    ResizeSouthWest=6
    CardinalCross=7
    Crosshairs=8
    Hand=9
    GrabHand=10
    GrabHandClosed=11
    SlashedCircle=12
    EyeDropper=13
    Custom=14
}

[ue_native_enum(path="/Script/DasRuntime.EDasNativeHashKey")]
enum EDasNativeHashKey {
    First=0
    Second=1
}

[ue_native_enum(path="/Script/Engine.EAspectRatioAxisConstraint")]
enum EAspectRatioAxisConstraint {
    AspectRatio_MaintainYFOV=0
    AspectRatio_MaintainXFOV=1
    AspectRatio_MajorAxisFOV=2
}

[ue_native_enum(path="/Script/Engine.EAttachLocation")]
enum EAttachLocation {
    KeepRelativeOffset=0
    KeepWorldPosition=1
    SnapToTarget=2
    SnapToTargetIncludingScale=3
}

[ue_native_enum(path="/Script/Engine.EAttachmentRule")]
enum EAttachmentRule {
    KeepRelative=0
    KeepWorld=1
    SnapToTarget=2
}

[ue_native_enum(path="/Script/Engine.ECameraProjectionMode")]
enum ECameraProjectionMode {
    Perspective=0
    Orthographic=1
}

[ue_native_enum(path="/Script/Engine.ECanBeCharacterBase")]
enum ECanBeCharacterBase {
    ECB_No=0
    ECB_Yes=1
    ECB_Owner=2
}

[ue_native_enum(path="/Script/Engine.ECollisionChannel")]
enum ECollisionChannel {
    ECC_WorldStatic=0
    ECC_WorldDynamic=1
    ECC_Pawn=2
    ECC_Visibility=3
    ECC_Camera=4
    ECC_PhysicsBody=5
    ECC_Vehicle=6
    ECC_Destructible=7
    ECC_EngineTraceChannel1=8
    ECC_EngineTraceChannel2=9
    ECC_EngineTraceChannel3=10
    ECC_EngineTraceChannel4=11
    ECC_EngineTraceChannel5=12
    ECC_EngineTraceChannel6=13
    ECC_GameTraceChannel1=14
    ECC_GameTraceChannel2=15
    ECC_GameTraceChannel3=16
    ECC_GameTraceChannel4=17
    ECC_GameTraceChannel5=18
    ECC_GameTraceChannel6=19
    ECC_GameTraceChannel7=20
    ECC_GameTraceChannel8=21
    ECC_GameTraceChannel9=22
    ECC_GameTraceChannel10=23
    ECC_GameTraceChannel11=24
    ECC_GameTraceChannel12=25
    ECC_GameTraceChannel13=26
    ECC_GameTraceChannel14=27
    ECC_GameTraceChannel15=28
    ECC_GameTraceChannel16=29
    ECC_GameTraceChannel17=30
    ECC_GameTraceChannel18=31
    ECC_GameTraceChannel19=32
    ECC_GameTraceChannel20=33
    ECC_GameTraceChannel21=34
    ECC_GameTraceChannel22=35
    ECC_GameTraceChannel23=36
    ECC_GameTraceChannel24=37
    ECC_GameTraceChannel25=38
    ECC_GameTraceChannel26=39
    ECC_GameTraceChannel27=40
    ECC_GameTraceChannel28=41
    ECC_GameTraceChannel29=42
    ECC_GameTraceChannel30=43
    ECC_GameTraceChannel31=44
    ECC_GameTraceChannel32=45
    ECC_GameTraceChannel33=46
    ECC_GameTraceChannel34=47
    ECC_GameTraceChannel35=48
    ECC_GameTraceChannel36=49
    ECC_GameTraceChannel37=50
    ECC_GameTraceChannel38=51
    ECC_GameTraceChannel39=52
    ECC_GameTraceChannel40=53
    ECC_GameTraceChannel41=54
    ECC_GameTraceChannel42=55
    ECC_GameTraceChannel43=56
    ECC_GameTraceChannel44=57
    ECC_GameTraceChannel45=58
    ECC_GameTraceChannel46=59
    ECC_GameTraceChannel47=60
    ECC_GameTraceChannel48=61
    ECC_GameTraceChannel49=62
    ECC_GameTraceChannel50=63
    ECC_OverlapAll_Deprecated=64
}

[ue_native_enum(path="/Script/Engine.ECollisionEnabled")]
enum ECollisionEnabled {
    NoCollision=0
    QueryOnly=1
    PhysicsOnly=2
    QueryAndPhysics=3
    ProbeOnly=4
    QueryAndProbe=5
}

[ue_native_enum(path="/Script/Engine.ECollisionResponse")]
enum ECollisionResponse {
    ECR_Ignore=0
    ECR_Overlap=1
    ECR_Block=2
}

[ue_native_enum(path="/Script/Engine.EComponentMobility")]
enum EComponentMobility {
    Static=0
    Stationary=1
    Movable=2
}

[ue_native_enum(path="/Script/Engine.EControllerAnalogStick")]
enum EControllerAnalogStick {
    CAS_LeftStick=0
    CAS_RightStick=1
}

[ue_native_enum(path="/Script/Engine.EDetachmentRule")]
enum EDetachmentRule {
    KeepRelative=0
    KeepWorld=1
}

[ue_native_enum(path="/Script/Engine.EDetailMode")]
enum EDetailMode {
    DM_Low=0
    DM_Medium=1
    DM_High=2
    DM_Epic=3
}

[ue_native_enum(path="/Script/Engine.EDOFMode")]
enum EDOFMode {
    Default=0
    SixDOF=1
    YZPlane=2
    XZPlane=3
    XYPlane=4
    CustomPlane=5
    None=6
}

[ue_native_enum(path="/Script/Engine.EFirstPersonPrimitiveType")]
enum EFirstPersonPrimitiveType {
    None=0
    FirstPerson=1
    WorldSpaceRepresentation=2
}

[ue_native_enum(path="/Script/Engine.EHLODBatchingPolicy")]
enum EHLODBatchingPolicy {
    None=0
    MeshSection=1
    Instancing=2
}

[ue_native_enum(path="/Script/Engine.EHLODLevelExclusion")]
enum EHLODLevelExclusion {
    HLOD0=1
    HLOD1=2
    HLOD2=4
    HLOD3=8
    HLOD4=16
    HLOD5=32
    HLOD6=64
    HLOD7=128
}

[ue_native_enum(path="/Script/Engine.EHorizTextAligment")]
enum EHorizTextAligment {
    EHTA_Left=0
    EHTA_Center=1
    EHTA_Right=2
}

[ue_native_enum(path="/Script/Engine.EIndirectLightingCacheQuality")]
enum EIndirectLightingCacheQuality {
    ILCQ_Off=0
    ILCQ_Point=1
    ILCQ_Volume=2
}

[ue_native_enum(path="/Script/Engine.EMovementMode")]
enum EMovementMode {
    MOVE_None=0
    MOVE_Walking=1
    MOVE_NavWalking=2
    MOVE_Falling=3
    MOVE_Swimming=4
    MOVE_Flying=5
    MOVE_Custom=6
}

[ue_native_enum(path="/Script/Engine.ENetDormancy")]
enum ENetDormancy {
    DORM_Never=0
    DORM_Awake=1
    DORM_DormantAll=2
    DORM_DormantPartial=3
    DORM_Initial=4
}

[ue_native_enum(path="/Script/Engine.ENetRole")]
enum ENetRole {
    ROLE_None=0
    ROLE_SimulatedProxy=1
    ROLE_AutonomousProxy=2
    ROLE_Authority=3
}

[ue_native_enum(path="/Script/Engine.ENetworkSmoothingMode")]
enum ENetworkSmoothingMode {
    Disabled=0
    Linear=1
    Exponential=2
}

[ue_native_enum(path="/Script/Engine.EPlaneConstraintAxisSetting")]
enum EPlaneConstraintAxisSetting {
    Custom=0
    X=1
    Y=2
    Z=3
    UseGlobalPhysicsSetting=4
}

[ue_native_enum(path="/Script/Engine.ERayTracingGroupCullingPriority")]
enum ERayTracingGroupCullingPriority {
    CP_0_NEVER_CULL=0
    CP_1=1
    CP_2=2
    CP_3=3
    CP_4_DEFAULT=4
    CP_5=5
    CP_6=6
    CP_7=7
    CP_8_QUICKLY_CULL=8
}

[ue_native_enum(path="/Script/Engine.ERelativeTransformSpace")]
enum ERelativeTransformSpace {
    RTS_World=0
    RTS_Actor=1
    RTS_Component=2
    RTS_ParentBoneSpace=3
}

[ue_native_enum(path="/Script/Engine.ERendererStencilMask")]
enum ERendererStencilMask {
    ERSM_Default=0
    ERSM_255=1
    ERSM_1=2
    ERSM_2=3
    ERSM_4=4
    ERSM_8=5
    ERSM_16=6
    ERSM_32=7
    ERSM_64=8
    ERSM_128=9
}

[ue_native_enum(path="/Script/Engine.ERuntimeVirtualTextureMainPassType")]
enum ERuntimeVirtualTextureMainPassType {
    Never=0
    Exclusive=1
    Always=2
}

[ue_native_enum(path="/Script/Engine.EShadowCacheInvalidationBehavior")]
enum EShadowCacheInvalidationBehavior {
    Auto=0
    Always=1
    Rigid=2
    Static=3
}

[ue_native_enum(path="/Script/Engine.ESpawnActorCollisionHandlingMethod")]
enum ESpawnActorCollisionHandlingMethod {
    Undefined=0
    AlwaysSpawn=1
    AdjustIfPossibleButAlwaysSpawn=2
    AdjustIfPossibleButDontSpawnIfColliding=3
    DontSpawnIfColliding=4
}

[ue_native_enum(path="/Script/Engine.ETickingGroup")]
enum ETickingGroup {
    TG_PrePhysics=0
    TG_StartPhysics=1
    TG_DuringPhysics=2
    TG_EndPhysics=3
    TG_PostPhysics=4
    TG_PostUpdateWork=5
    TG_LastDemotable=6
    TG_NewlySpawned=7
}

[ue_native_enum(path="/Script/Engine.EVerticalTextAligment")]
enum EVerticalTextAligment {
    EVRTA_TextTop=0
    EVRTA_TextCenter=1
    EVRTA_TextBottom=2
    EVRTA_QuadTop=3
}

[ue_native_enum(path="/Script/Engine.EViewTargetBlendFunction")]
enum EViewTargetBlendFunction {
    VTBlend_Linear=0
    VTBlend_Cubic=1
    VTBlend_EaseIn=2
    VTBlend_EaseOut=3
    VTBlend_EaseInOut=4
    VTBlend_PreBlended=5
}

[ue_native_enum(path="/Script/Engine.EWalkableSlopeBehavior")]
enum EWalkableSlopeBehavior {
    WalkableSlope_Default=0
    WalkableSlope_Increase=1
    WalkableSlope_Decrease=2
    WalkableSlope_Unwalkable=3
}

[ue_native_enum(path="/Script/InputCore.EControllerHand")]
enum EControllerHand {
    Left=0
    Right=1
    AnyHand=2
    Pad=3
    ExternalCamera=4
    Gun=5
    HMD=6
    Chest=7
    LeftShoulder=8
    RightShoulder=9
    LeftElbow=10
    RightElbow=11
    Waist=12
    LeftKnee=13
    RightKnee=14
    LeftFoot=15
    RightFoot=16
    Special=17
    ControllerHand_Count=18
}

[ue_native_enum(path="/Script/InputCore.ETouchIndex")]
enum ETouchIndex {
    Touch1=0
    Touch2=1
    Touch3=2
    Touch4=3
    Touch5=4
    Touch6=5
    Touch7=6
    Touch8=7
    Touch9=8
    Touch10=9
    CursorPointerIndex=10
    MAX_TOUCHES=11
}

[ue_native_struct(path="/Script/CoreUObject.PlatformUserId")]
struct PlatformUserId {
    InternalId:int
}

[ue_native_struct(path="/Script/CoreUObject.Quat")]
struct Quat {
    X:double
    Y:double
    Z:double
    W:double
}

[ue_native_struct(path="/Script/CoreUObject.Vector4")]
struct Vector4 {
    X:double
    Y:double
    Z:double
    W:double
}

[ue_native_struct(path="/Script/DasRuntime.DasNativeHashValue")]
struct DasNativeHashValue {
    Labels:table<int;string>
    Names:table<string>
}

[ue_native_struct(path="/Script/Engine.LightingChannels")]
struct LightingChannels {
    bChannel0:bool
    bChannel1:bool
    bChannel2:bool
}

[ue_native_struct(path="/Script/Engine.NavAvoidanceMask")]
struct NavAvoidanceMask {
    bGroup0:bool
    bGroup1:bool
    bGroup2:bool
    bGroup3:bool
    bGroup4:bool
    bGroup5:bool
    bGroup6:bool
    bGroup7:bool
    bGroup8:bool
    bGroup9:bool
    bGroup10:bool
    bGroup11:bool
    bGroup12:bool
    bGroup13:bool
    bGroup14:bool
    bGroup15:bool
    bGroup16:bool
    bGroup17:bool
    bGroup18:bool
    bGroup19:bool
    bGroup20:bool
    bGroup21:bool
    bGroup22:bool
    bGroup23:bool
    bGroup24:bool
    bGroup25:bool
    bGroup26:bool
    bGroup27:bool
    bGroup28:bool
    bGroup29:bool
    bGroup30:bool
    bGroup31:bool
}

[ue_native_struct(path="/Script/Engine.StreamingSourceShape")]
struct StreamingSourceShape {
    bUseGridLoadingRange:bool
    LoadingRangeScale:float
    Radius:float
    bIsSector:bool
    SectorAngle:float
    Location:UEVector
    Rotation:UERotator
}

[ue_native_struct(path="/Script/Engine.WalkableSlopeOverride")]
struct WalkableSlopeOverride {
    WalkableSlopeBehavior:EWalkableSlopeBehavior
    WalkableSlopeAngle:float
}

[ue_native_struct(path="/Script/InputCore.Key")]
struct Key {
    KeyName:UENameValue
}
