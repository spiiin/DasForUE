#include "Hash/CityHash.h"
#include "UObject/FieldPath.h"
#include "DasGameplayTags.h"
#include "Engine/CurveTable.h"
#include "Engine/DataTable.h"
#include "DasDataAssets.h"
#include "DasPrimaryAssets.h"
#include "DasSaveGame.h"
#include "DasSaveGameProxy.h"
#include "DasAIMoveProxy.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "AISystem.h"
#include "GameFramework/Pawn.h"
#include "DasEffectProxy.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "DasMontageProxy.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Animation/Skeleton.h"
#include "Engine/SkeletalMesh.h"
#include "DasWidgetLifetime.h"
#include "DasImageSoftBrush.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/CollisionProfile.h"
#include "GameFramework/DamageType.h"
#include "DasCallbackTypes.h"
#include "DasDefaultValueProbe.h"
#include "DasObjectFixture.h"
#include "Misc/EngineVersion.h"
#include "Misc/App.h"
#include "Misc/CommandLine.h"
#include "ProfilingDebugging/CpuProfilerTrace.h"
#include "DasCallbackFrames.h"
#include "DasCallbackPayload.h"
#include "Misc/CoreDelegates.h"
#include "DasPropertyTypes.h"
#include "DasPropertyValues.h"
#include "DasPropertyAccess.h"
#include "DasSnapshotFields.h"
#include "DasSnapshotContainers.h"
#include "DasSoftReferences.h"
#include "UObject/ObjectKey.h"
#include "DasPawn.h"
#include "DasCharacter.h"
#include "DasNativeReferenceFields.h"
#include "DasNativeGameplayTypes.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "DasScriptHosts.h"
#include "EnhancedPlayerInput.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "DasInputEvents.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/Application/SlateUser.h"
#include "DasGeometry.h"
#include "Layout/Geometry.h"
#include "DasEventReplyTransport.h"
#include "DasPaintTransport.h"
#include "DasWidgetAnimationTransport.h"
#include "Animation/WidgetAnimationState.h"
#include "DasScriptUserWidget.h"
#include "DasDragDropOperation.h"
#include "Input/Reply.h"
#include "Input/Events.h"
#include "DasEnhancedInputProxy.h"
#include "Components/SceneComponent.h"
#include "Components/InputComponent.h"
#include "DasAsyncAssetProxy.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Misc/PackageName.h"
#include "DasLatentProxy.h"
#include "LatentActions.h"
#include "Engine/LatentActionManager.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Engine/OverlapResult.h"
#include "DasConstraintTransportRecord.h"
#include "PhysicsEngine/ConstraintInstanceBlueprintLibrary.h"
#include "Physics/PhysicsInterfaceCore.h"
#include "Chaos/PBDConstraintBaseData.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Engine/Engine.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EngineUtils.h"
#include "DasSourceCatalog.h"
#include "DasAuthoredInterfaceClass.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/AssetData.h"
#include "UObject/UObjectIterator.h"
#include "DasDefinedEnum.h"
#include "DasTypeRegistry.h"
#include "DasRuntime.h"
#include "DasExecutionRole.h"
#include "DasAsyncPhysicsBridge.h"
#include "DasExecutionGuardProbe.h"
#include "DasAsyncPhysicsVM.h"
#include "DasScopedPhysicsBody.h"
#include "DasSoftReferences.h"
#include "DasBlueprintLibrary.h"
#include "DasContainerTypes.h"
#include "DasFrameLibrary.h"
#include <type_traits>
#include "DasReflectedTypes.h"
#include "DasAuthoring.h"
#include "DasNativeDefaults.h"
#include "DasNativeDefaultsProbe.h"
#include "DasDelegateSignatures.h"
#include "Serialization/JsonSerializer.h"
#include "Policies/CondensedJsonPrintPolicy.h"
#include "Dom/JsonObject.h"
#include "Misc/EngineVersion.h"
#include "DasProbeActor.h"
#include "DasScriptComponent.h"
#include "DasIntDelegateProxy.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/GCObject.h"
#include "UObject/StructOnScope.h"
#include "GameFramework/Actor.h"
#include "Components/ActorComponent.h"
#include "Engine/World.h"
#include "Engine/OverlapResult.h"
#include "DasConstraintTransportRecord.h"
#include "PhysicsEngine/ConstraintInstanceBlueprintLibrary.h"
#include "Physics/PhysicsInterfaceCore.h"
#include "Chaos/PBDConstraintBaseData.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "UObject/UObjectGlobals.h"
#include "UObject/UnrealType.h"
#include "UObject/Script.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/FileHelper.h"
#include "Misc/ScopeExit.h"
#include "Misc/SecureHash.h"
#include "Misc/Paths.h"
#include "Misc/PackageName.h"
#include "HAL/FileManager.h"
#include "Modules/ModuleManager.h"

#pragma push_macro("verify")
#pragma push_macro("ensure")
#pragma push_macro("check")
#undef verify
#undef ensure
#undef check
THIRD_PARTY_INCLUDES_START
// arraytype.h resets packing: load simulation prerequisites first, then match
// the static daScript AST library's /Zp16 rather than Unreal's /Zp8.
#include "daScript/simulate/simulate.h"
#pragma pack(push, 16)
#include "daScript/daScript.h"
#include "daScript/ast/ast_handle.h"
#include "daScript/ast/ast_visitor.h"
#include "daScript/simulate/data_walker.h"
THIRD_PARTY_INCLUDES_END
#pragma pack(pop)
#pragma pop_macro("check")
#pragma pop_macro("ensure")
#pragma pop_macro("verify")

MAKE_TYPE_FACTORY(UEPhysicsBodyToken, FDasPhysicsBodyToken);
namespace das
{
template <> struct cast<FDasPhysicsBodyToken>
{
    static vec4f from(FDasPhysicsBodyToken V) { return cast<uint64_t>::from(V.Value); }
    static FDasPhysicsBodyToken to(vec4f V) { return {cast<uint64_t>::to(V)}; }
};
template <> struct ext_ret_carrier<FDasPhysicsBodyToken>
{
    using type = uint64_t;
};
template <> struct ext_ret_to_carrier<FDasPhysicsBodyToken, uint64_t>
{
    static uint64_t from(FDasPhysicsBodyToken V) { return V.Value; }
};
}
// No fields are exposed to scripts: this is a context-local capability, never a pointer.
struct FDasEnumValue
{
    uint64 Token = 0;
};
MAKE_TYPE_FACTORY(UEEnumValue, FDasEnumValue);
namespace das
{
template <> struct cast<FDasEnumValue>
{
    static vec4f from(FDasEnumValue V) { return cast<uint64_t>::from(V.Token); }
    static FDasEnumValue to(vec4f V) { return {cast<uint64_t>::to(V)}; }
};
}
struct FDasTextValue
{
    uint64 Token = 0;
};
MAKE_TYPE_FACTORY(UETextValue, FDasTextValue);
namespace das
{
template <> struct cast<FDasTextValue>
{
    static vec4f from(FDasTextValue V) { return cast<uint64_t>::from(V.Token); }
    static FDasTextValue to(vec4f V) { return {cast<uint64_t>::to(V)}; }
};
template <> struct ext_ret_carrier<FDasTextValue>
{
    using type = uint64_t;
};
template <> struct ext_ret_to_carrier<FDasTextValue, uint64_t>
{
    static uint64_t from(FDasTextValue V) { return V.Token; }
};
}
struct FDasNameValue
{
    uint64 Token = 0;
};
MAKE_TYPE_FACTORY(UENameValue, FDasNameValue);
namespace das
{
template <> struct cast<FDasNameValue>
{
    static vec4f from(FDasNameValue V) { return cast<uint64_t>::from(V.Token); }
    static FDasNameValue to(vec4f V) { return {cast<uint64_t>::to(V)}; }
};
}
// Keep native returns and the annotation's uint64 value representation in the same ABI slot.
namespace das
{
template <> struct ext_ret_carrier<FDasEnumValue>
{
    using type = uint64_t;
};
template <> struct ext_ret_to_carrier<FDasEnumValue, uint64_t>
{
    static uint64_t from(FDasEnumValue V) { return V.Token; }
};
template <> struct ext_ret_carrier<FDasNameValue>
{
    using type = uint64_t;
};
template <> struct ext_ret_to_carrier<FDasNameValue, uint64_t>
{
    static uint64_t from(FDasNameValue V) { return V.Token; }
};
}
struct FDasObjectHandle
{
    uint64 Token = 0;
};
MAKE_TYPE_FACTORY(UObjectHandle, FDasObjectHandle);
struct FDasClassHandle
{
    uint64 Token = 0;
};
MAKE_TYPE_FACTORY(UEClassHandle, FDasClassHandle);
namespace das
{
template <> struct cast<FDasClassHandle>
{
    static vec4f from(FDasClassHandle V) { return cast<uint64_t>::from(V.Token); }
    static FDasClassHandle to(vec4f V) { return {cast<uint64_t>::to(V)}; }
};
template <> struct ext_ret_carrier<FDasClassHandle>
{
    using type = uint64_t;
};
template <> struct ext_ret_to_carrier<FDasClassHandle, uint64_t>
{
    static uint64_t from(FDasClassHandle V) { return V.Token; }
};
}

struct FDasConstraintHandle
{
    uint64 Token = 0;
};
MAKE_TYPE_FACTORY(UEConstraintHandle, FDasConstraintHandle);
namespace das
{
template <> struct cast<FDasConstraintHandle>
{
    static vec4f from(FDasConstraintHandle V) { return cast<uint64_t>::from(V.Token); }
    static FDasConstraintHandle to(vec4f V) { return {cast<uint64_t>::to(V)}; }
};
}
struct FDasHitResultValue
{
    uint64 Token = 0;
};
MAKE_TYPE_FACTORY(UEHitResult, FDasHitResultValue);
namespace das
{
template <> struct cast<FDasHitResultValue>
{
    static vec4f from(FDasHitResultValue V) { return cast<uint64_t>::from(V.Token); }
    static FDasHitResultValue to(vec4f V) { return {cast<uint64_t>::to(V)}; }
};


}
struct FDasStructValue
{
    uint64 Token = 0;
};
MAKE_TYPE_FACTORY(UEStructValue, FDasStructValue);
namespace das
{
template <> struct cast<FDasStructValue>
{
    static vec4f from(FDasStructValue V) { return cast<uint64_t>::from(V.Token); }
    static FDasStructValue to(vec4f V) { return {cast<uint64_t>::to(V)}; }
};
}
// Owned numeric snapshot, not the unreflected native layout and not a context token.
struct FDasEventReplyValue
{
    uint64 Token = 0;
};
MAKE_TYPE_FACTORY(UEEventReply, FDasEventReplyValue);
struct FDasWidgetAnimationValue
{
    uint64 Token = 0;
};
MAKE_TYPE_FACTORY(UEWidgetAnimationHandle, FDasWidgetAnimationValue);
namespace das
{
template <> struct cast<FDasWidgetAnimationValue>
{
    static vec4f from(FDasWidgetAnimationValue V) { return cast<uint64_t>::from(V.Token); }
    static FDasWidgetAnimationValue to(vec4f V) { return {cast<uint64_t>::to(V)}; }
};
}
struct FDasPaintValue
{
    uint64 Token = 0;
};
MAKE_TYPE_FACTORY(UEPaintContext, FDasPaintValue);
namespace das
{
template <> struct cast<FDasPaintValue>
{
    static vec4f from(FDasPaintValue V) { return cast<uint64_t>::from(V.Token); }
    static FDasPaintValue to(vec4f V) { return {cast<uint64_t>::to(V)}; }
};
}
struct FDasGeometryValue
{
    uint64 Token = 0;
};
MAKE_TYPE_FACTORY(UEGeometry, FDasGeometryValue);
namespace das
{
template <> struct cast<FDasGeometryValue>
{
    static vec4f from(FDasGeometryValue V) { return cast<uint64_t>::from(V.Token); }
    static FDasGeometryValue to(vec4f V) { return {cast<uint64_t>::to(V)}; }
};
}
struct FDasInputEventValue
{
    uint64 Token = 0;
};
MAKE_TYPE_FACTORY(UEInputEvent, FDasInputEventValue);
namespace das
{
template <> struct cast<FDasInputEventValue>
{
    static vec4f from(FDasInputEventValue V) { return cast<uint64_t>::from(V.Token); }
    static FDasInputEventValue to(vec4f V) { return {cast<uint64_t>::to(V)}; }
};
}
struct FDasKeyEventValue
{
    uint64 Token = 0;
};
MAKE_TYPE_FACTORY(UEKeyEvent, FDasKeyEventValue);
namespace das
{
template <> struct cast<FDasKeyEventValue>
{
    static vec4f from(FDasKeyEventValue V) { return cast<uint64_t>::from(V.Token); }
    static FDasKeyEventValue to(vec4f V) { return {cast<uint64_t>::to(V)}; }
};
}
struct FDasAnalogInputEventValue
{
    uint64 Token = 0;
};
MAKE_TYPE_FACTORY(UEAnalogInputEvent, FDasAnalogInputEventValue);
namespace das
{
template <> struct cast<FDasAnalogInputEventValue>
{
    static vec4f from(FDasAnalogInputEventValue V) { return cast<uint64_t>::from(V.Token); }
    static FDasAnalogInputEventValue to(vec4f V) { return {cast<uint64_t>::to(V)}; }
};
}
struct FDasCharacterEventValue
{
    uint64 Token = 0;
};
MAKE_TYPE_FACTORY(UECharacterEvent, FDasCharacterEventValue);
namespace das
{
template <> struct cast<FDasCharacterEventValue>
{
    static vec4f from(FDasCharacterEventValue V) { return cast<uint64_t>::from(V.Token); }
    static FDasCharacterEventValue to(vec4f V) { return {cast<uint64_t>::to(V)}; }
};
}
struct FDasPointerEventValue
{
    uint64 Token = 0;
};
MAKE_TYPE_FACTORY(UEPointerEvent, FDasPointerEventValue);
namespace das
{
template <> struct cast<FDasPointerEventValue>
{
    static vec4f from(FDasPointerEventValue V) { return cast<uint64_t>::from(V.Token); }
    static FDasPointerEventValue to(vec4f V) { return {cast<uint64_t>::to(V)}; }
};
}
struct FDasFocusEventValue
{
    uint64 Token = 0;
};
MAKE_TYPE_FACTORY(UEFocusEvent, FDasFocusEventValue);
namespace das
{
template <> struct cast<FDasFocusEventValue>
{
    static vec4f from(FDasFocusEventValue V) { return cast<uint64_t>::from(V.Token); }
    static FDasFocusEventValue to(vec4f V) { return {cast<uint64_t>::to(V)}; }
};
}
struct FDasNavigationEventValue
{
    uint64 Token = 0;
};
MAKE_TYPE_FACTORY(UENavigationEvent, FDasNavigationEventValue);
namespace das
{
template <> struct cast<FDasNavigationEventValue>
{
    static vec4f from(FDasNavigationEventValue V) { return cast<uint64_t>::from(V.Token); }
    static FDasNavigationEventValue to(vec4f V) { return {cast<uint64_t>::to(V)}; }
};
}
struct FDasInputActionValue
{
    double X = 0, Y = 0, Z = 0;
    int32 Kind = 0;
};
MAKE_TYPE_FACTORY(InputActionValue, FDasInputActionValue);
MAKE_TYPE_FACTORY(UEVector2D, FVector2D);
MAKE_TYPE_FACTORY(UEVector, FVector);
MAKE_TYPE_FACTORY(UERotator, FRotator);
MAKE_TYPE_FACTORY(UETransform, FTransform);
// A source-side checked view of a native FHitResult. References are weak handles.
struct FDasTraceHit
{
    uint64 Scope = 0;
    bool BlockingHit = false, StartPenetrating = false;
    double Distance = 0, Time = 0;
    FVector Location = FVector::ZeroVector, ImpactPoint = FVector::ZeroVector, Normal = FVector::ZeroVector,
            ImpactNormal = FVector::ZeroVector;
    FVector TraceStart = FVector::ZeroVector, TraceEnd = FVector::ZeroVector;
    double PenetrationDepth = 0;
    int32 FaceIndex = -1, Item = -1, MyItem = -1, ElementIndex = 0;
    FName BoneName, MyBoneName;
    FDasObjectHandle Actor{}, Component{}, PhysMaterial{}, PhysicsObjectOwner{};
};
MAKE_TYPE_FACTORY(UETraceHit, FDasTraceHit);
struct FDasOverlapResult
{
    uint64 Scope = 0;
    bool BlockingHit = false;
    int32 ItemIndex = -1, MyItemIndex = -1;
    FDasObjectHandle Actor{}, Component{}, PhysicsObjectOwner{};
};
MAKE_TYPE_FACTORY(UEOverlapResult, FDasOverlapResult);
struct FDasCallHandle
{
    uint64 Token = 0;
};
MAKE_TYPE_FACTORY(UCallHandle, FDasCallHandle);

DEFINE_LOG_CATEGORY_STATIC(LogDasRuntime, Log, All);

namespace
{
#include "DasUEExternGuard.inl"
struct FSourceSnapshot
{
    bool bExists = false;
    TArray<uint8> Bytes;
    static FSourceSnapshot Read(const FString& Path)
    {
        FSourceSnapshot Result;
        Result.bExists = IFileManager::Get().FileExists(*Path) && FFileHelper::LoadFileToArray(Result.Bytes, *Path);
        return Result;
    }
    bool Matches(const FString& Path) const
    {
        const auto Current = Read(Path);
        return bExists == Current.bExists && Bytes == Current.Bytes;
    }
};
using FSourceSet = TMap<FString, FSourceSnapshot>;
struct FDSLAsyncPhysicsState;
TSharedPtr<IDasPreparedAsyncPhysicsVM> PreparePhysicsVMImpl(const FString&, UObject*, FString&, FSourceSet*);
void AsyncPhysicsSetInput(int32, double, das::Context*);
bool AsyncPhysicsHasOutput(das::Context*);
double AsyncPhysicsOutput(int32, das::Context*);
bool AsyncPhysicsRunning(das::Context*);
char* AsyncPhysicsError(das::Context*);
FSourceSet* AuthoredReferenceSources = nullptr;

#include "DasRuntimeState.inl"
#include "DasRuntimeSelfValues.inl"
#include "DasRuntimeStructValues.inl"
#include "DasRuntimeFramesWidgets.inl"
#include "DasRuntimeObjectAccess.inl"
#include "DasRuntimeMethodCalls.inl"
#include "DasRuntimeSceneComponents.inl"
#include "DasRuntimeSubscriptionApi.inl"
#include "DasRuntimeTypeAnnotations.inl"
#include "DasFoundationModule.inl"
#include "DasRuntimeTimers.inl"
#include "DasRuntimeInputSessions.inl"
}

void FDasRuntimeModule::StartupModule()
{
    FModuleManager::LoadModuleChecked<IModuleInterface>(TEXT("EnhancedInput"));
    Root = FPaths::ConvertRelativePathToFull(IPluginManager::Get().FindPlugin(TEXT("DasForUE"))->GetContentDir() /
                                                                              TEXT("Das"));
    const FTCHARToUTF8 Utf8Root(*Root);
    das::setDasRoot(das::string(Utf8Root.Get(), Utf8Root.Length()));
    NEED_ALL_DEFAULT_MODULES;
    das::Module::Initialize();
    new FUnrealFoundationModule();
    InputRestoreTickHandle = FWorldDelegates::OnWorldTickStart.AddLambda([](UWorld* World, ELevelTick, float)
                                                                         { ReconcileScriptInput(World); });
    WorldCleanupHandle = FWorldDelegates::OnWorldCleanup.AddLambda(
        [](UWorld* World, bool, bool)
        {
            UDasAsyncPhysicsBridge::RevokeWorld(World);
            for (auto It = InitializerDeliveries.CreateIterator(); It; ++It)
            {
                auto* O = It.Key().Get();
                if (!O || O->GetWorld() == World)
                    It.RemoveCurrent();
            }
            for (const auto& Session : Sessions)
                if (Session->World.Get() == World)
                    RevokeSession(*Session);
#if WITH_EDITOR
            for (const auto& Session : PreservedSessions)
                if (Session->World.Get() == World)
                    RevokeSession(*Session);
#endif
            FlushReleasedSessions();
        });
    PostGCHandle = FCoreUObjectDelegates::GetPostGarbageCollect().AddStatic(&PruneSessions);
#if WITH_EDITOR
    ObjectsReplacedHandle = FCoreUObjectDelegates::OnObjectsReplaced.AddLambda(
        [](const FCoreUObjectDelegates::FReplacementObjectMap& Map)
        {
            if (!IsInGameThread())
                return;
            for (const auto& Pair : Map)
                if (Pair.Key != Pair.Value && IsValid(Pair.Value))
                    if (auto* Old = InitializerDeliveries.Find(Pair.Key))
                    {
                        auto State = *Old;
                        InitializerDeliveries.Remove(Pair.Key);
                        InitializerDeliveries.Add(Pair.Value, MoveTemp(State));
                    }
            for (auto& Session : PreservedSessions)
            {
                if (UObject* const* New = Map.Find(Session->Owner.GetEvenIfUnreachable()))
                {
                    Session->Owner = *New;
                    Session->World = *New ? (*New)->GetWorld() : nullptr;
                    Session->bReinstanceOwnerChanged = true;
                }
                if (Session->Live)
                {
                    auto& C = *Session->Live->Context;
                    if (UObject* const* New = Map.Find(C.Self.GetEvenIfUnreachable()))
                        C.Self = *New;
                    for (auto& Declaration : C.Subscriptions)
                        if (UObject* const* New = Map.Find(Declaration.Source.GetEvenIfUnreachable()))
                            Declaration.Source = *New;
                    for (auto& Pair : C.DelegateValues)
                    {
                        auto& V = Pair.Value;
                        if (UObject* const* New = Map.Find(V.Source.GetEvenIfUnreachable()))
                            V.Source = *New;
                        auto Remap = [&](FScriptDelegate& D)
                        {
                            if (UObject* const* New = Map.Find(const_cast<UObject*>(D.GetUObjectEvenIfUnreachable())))
                            {
                                const FName N = D.GetFunctionName();
                                if (IsValid(*New))
                                    D.BindUFunction(*New, N);
                                else
                                    D.Unbind();
                            }
                        };
                        Remap(V.Single);
                        for (auto& D : V.Listeners)
                            Remap(D);
                    }
                    for (auto& Pair : C.ObjectHandles)
                    {
                        auto& Entry = Pair.Value;
                        if (UObject* const* New = Map.Find(Entry.ExpectedClass.GetEvenIfUnreachable()))
                            Entry.ExpectedClass = Cast<UClass>(*New);
                        if (UObject* const* New = Map.Find(Entry.Object.GetEvenIfUnreachable()))
                            Entry.Object = *New;
                        if (Entry.Object.IsValid() &&
                            (!Entry.ExpectedClass.IsValid() || !Entry.Object->IsA(Entry.ExpectedClass.Get())))
                            Entry.Object.Reset();
                    }
                }
            }
            for (const auto& Session : Sessions)
                if (Session->Live)
                    for (auto& Pair : Session->Live->Context->ObjectHandles)
                    {
                        auto& Entry = Pair.Value;
                        if (UObject* const* New = Map.Find(Entry.ExpectedClass.GetEvenIfUnreachable()))
                            Entry.ExpectedClass = Cast<UClass>(*New);
                        if (UObject* const* New = Map.Find(Entry.Object.GetEvenIfUnreachable()))
                            Entry.Object = *New;
                        if (Entry.Object.IsValid() &&
                            (!Entry.ExpectedClass.IsValid() || !Entry.Object->IsA(Entry.ExpectedClass.Get())))
                            Entry.Object.Reset();
                    }
        });
#endif
    RuntimePreExitHandle = FCoreDelegates::OnPreExit.AddRaw(this, &FDasRuntimeModule::ShutdownModule);
    bInitialized = true;
    UE_LOG(LogDasRuntime, Display, TEXT("daScript initialized; content: %s"), *Root);
}

void FDasRuntimeModule::ShutdownModule()
{
    // Release rooted callback signatures while UObject storage is still valid.
    FCoreDelegates::OnPreExit.Remove(RuntimePreExitHandle);
    RuntimePreExitHandle.Reset();
    UDasAsyncPhysicsBridge::RevokeAll();
    DSLPhysicsOwners.Empty();
    InitializerDeliveries.Empty();
    if (bInitialized)
    {
        FWorldDelegates::OnWorldTickStart.Remove(InputRestoreTickHandle);
        FWorldDelegates::OnWorldCleanup.Remove(WorldCleanupHandle);
        FCoreUObjectDelegates::GetPostGarbageCollect().Remove(PostGCHandle);
#if WITH_EDITOR
        FCoreUObjectDelegates::OnObjectsReplaced.Remove(ObjectsReplacedHandle);
#endif
#if WITH_EDITOR
        PreservedSessions.Empty();
        bPreservingSessions = false;
#endif
        ScriptTimers.Empty();
        PendingScriptTimers.Empty();
        Sessions.Empty();
        PendingEvents.Empty();
        CreatedScriptScenes.Empty();
        UDasIntDelegateProxy::ResetDynamicReceivers();
        ResetDasCallbackFrames();
        ResetDasContainerCarriers();
        das::Module::Shutdown();
        bInitialized = false;
    }
}

int32 FDasRuntimeModule::StructSnapshotCount(const UObject* Owner) const
{
    int32 Count = 0;
    for (const auto& S : Sessions)
        if (S->Live && (!Owner || S->Owner.Get() == Owner))
            Count += S->Live->Context->StructValues.Num();
    return Count;
}
uint64 FDasRuntimeModule::StructSnapshotsReclaimed(const UObject* Owner) const
{
    uint64 Count = 0;
    for (const auto& S : Sessions)
        if (S->Live && (!Owner || S->Owner.Get() == Owner))
            Count += S->Live->Context->StructReclaimed;
    return Count;
}

FString FDasRuntimeModule::ScriptPath(const FString& Name) const
{
    if (Name == FPaths::GetCleanFilename(Name))
    {
        const FString Project =
            FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir() / TEXT("Das/Scripts")/ Name);
        if (IFileManager::Get().FileExists(*Project))
            return Project;
    }
    return Root / TEXT("Scripts") / Name;
}

bool FDasRuntimeModule::ReadAuthoredInterfaces(const FString& Path, FDasAuthoringModule& Out, FString& Error)
{
    Error.Reset();
    if (!IsInGameThread() || !bInitialized)
    {
        Error = TEXT("Interface declaration compilation requires game-thread runtime");
        return false;
    }
    try
    {
        TGuardValue<bool> CollectOnly(CollectTypesOnly, true);
        das::TextWriter Text;
        das::ModuleGroup Group;
        auto Access = das::make_smart<FUnrealFileAccess>();
        Access->addFileSystem(new FUnrealFileSystem(), true, false);
        const FTCHARToUTF8 File(*Path);
        auto Program = DSLCompile(File.Get(), Access, Text, Group);
        if (!Program || Program->failed())
        {
            Error = Errors(Program, Text);
            return false;
        }
        FDasAuthoringModule Model;
        bool OK = true;
        Program->getThisModule()->structures.foreach (
            [&](const das::StructurePtr& St)
            {
                auto* A = DSLAnnotation(St->annotations, "ue_interface");
                if (!A || !OK)
                    return;
                auto* J = A->arguments.find(DSLInterfaceKey, das::Type::tString);
                FDasAuthoringModule Part;
                if (!J || !FDasAuthoringModule::FromJson(DSLText(J->sValue), Part, Error))
                {
                    OK = false;
                    return;
                }
                if (!Model.Id.IsEmpty() && Model.Id != Part.Id)
                {
                    Error = TEXT("Interface declarations must share module ID");
                    OK = false;
                    return;
                }
                Model.Id = Part.Id;
                Model.Classes.Append(Part.Classes);
            });
        if (!OK || !Model.Validate(Error))
            return false;
        Out = MoveTemp(Model);
        return true;
    }
    catch (const std::exception& X)
    {
        Error = UTF8_TO_TCHAR(X.what());
        return false;
    }
}
bool FDasRuntimeModule::ReadReflectedTypes(const FString& Path, FDasReflectedTypes& Out, FString& Error,
                                           FString* SourceModule)
{
    Error.Reset();
    if (!IsInGameThread() || !bInitialized)
    {
        Error = TEXT("Type declaration compilation requires initialized game-thread runtime");
        return false;
    }
    try
    {
        TGuardValue<bool> CollectOnly(CollectTypesOnly, true);
        das::TextWriter Text;
        das::ModuleGroup Group;
        auto Access = das::make_smart<FUnrealFileAccess>();
        Access->addFileSystem(new FUnrealFileSystem(), true, false);
        const FTCHARToUTF8 File(*Path);
        auto Program = DSLCompile(File.Get(), Access, Text, Group);
        if (!Program || Program->failed())
        {
            Error = Errors(Program, Text);
            return false;
        }
        FDasReflectedTypes Model;
        bool OK = true;
        auto Append = [&](const das::AnnotationList& List, const char* Name)
        {
            const auto* A = DSLAnnotation(List, Name);
            if (!A || !OK)
                return;
            const auto* J = A->arguments.find(DSLTypesKey, das::Type::tString);
            FDasReflectedTypes Part;
            if (!J || !FDasReflectedTypes::FromJson(DSLText(J->sValue), Part, Error, true))
            {
                OK = false;
                return;
            }
            if (!Model.Id.IsEmpty() && Model.Id != Part.Id)
            {
                Error = TEXT("Reflected declarations must share module ID");
                OK = false;
                return;
            }
            Model.Id = Part.Id;
            Model.Enums.Append(Part.Enums);
            Model.Structs.Append(Part.Structs);
        };
        Program->getThisModule()->enumerations.foreach ([&](const das::EnumerationPtr& E)
                                                        { Append(E->annotations, "ue_enum"); });
        Program->getThisModule()->structures.foreach ([&](const das::StructurePtr& S)
                                                      { Append(S->annotations, "ue_struct"); });
        if (!OK || !Model.Validate(Error))
            return false;
        if (SourceModule)
        {
            bool HasClasses = false;
            Program->getThisModule()->structures.foreach (
                [&](const das::StructurePtr& S)
                {
                    HasClasses |= DSLAnnotation(S->annotations, "ue_class") != nullptr ||
                                  DSLAnnotation(S->annotations, "ue_interface") != nullptr;
                });
            if (HasClasses)
            {
                Error = TEXT("Catalog value imports require dedicated enum/struct modules without classes/interfaces");
                return false;
            }
            *SourceModule = DSLText(Program->getThisModule()->name);
        }
        Out = MoveTemp(Model);
        return true;
    }
    catch (const std::exception& X)
    {
        Error = UTF8_TO_TCHAR(X.what());
        return false;
    }
}

// Sorted name, mangled signature, result type and side-effect flags of every ue_foundation function.
// Foundation tests compare it with Bindings/FoundationExterns.txt so that binding
// refactors keep the script-visible surface unchanged.
FString DasFoundationExternManifest()
{
    TArray<FString> Lines;
    if (das::Module* Module = das::Module::require("ue_foundation"))
        Module->functions.foreach (
            [&](const das::FunctionPtr& Fn)
            {
                const das::string Result = Fn->result ? Fn->result->describe() : das::string();
                Lines.Add(FString::Printf(TEXT("%s\t%s\t%s\t%u"), UTF8_TO_TCHAR(Fn->name.c_str()),
                                               UTF8_TO_TCHAR(Fn->getMangledName().c_str()),
                                               UTF8_TO_TCHAR(Result.c_str()), Fn->sideEffectFlags));
            });
    Lines.Sort();
    return FString::Join(Lines, TEXT("\n")) + TEXT("\n");
}

// Value kinds with their property class and daScript self/reference types, for the Python generators.
// Foundation tests compare it with Bindings/Kinds.json.
FString DasKindManifest()
{
    TMap<FString, FString> Results;
    if (das::Module* Module = das::Module::require("ue_foundation"))
        Module->functions.foreach (
            [&](const das::FunctionPtr& Fn)
            {
                if (!Fn->result)
                    return;
                FString Type = UTF8_TO_TCHAR(Fn->result->describe().c_str());
                Type.RemoveFromEnd(TEXT(" const"));
                Type.RemoveFromStart(TEXT("ue_foundation::"));
                Results.Add(UTF8_TO_TCHAR(Fn->name.c_str()), Type);
            });
    TArray<FString> Rows;
    auto Row = [&](const TCHAR* Name, const FString& Property, bool bStructLeaf)
    {
        Rows.Add(FString::Printf(
            TEXT("    {\"name\": \"%s\", \"property\": \"%s\", \"self\": \"%s\", \"reference\": \"%s\", ")
                TEXT("\"struct_leaf\": %s}"), Name, *Property,
                     *Results.FindRef(FString(TEXT("ue_get_self_")) + Name),
                                      *Results.FindRef(FString(TEXT("__ue_authored_get_")) + Name),
                                                       bStructLeaf ? TEXT("true") : TEXT("false")));
    };
#define DAS_KIND_MANIFEST_SCALAR(Suffix, SelfKind, RefKind) Row(TEXT(#Suffix), DasKindPropertyName<SelfKind>(), true);
#define DAS_KIND_MANIFEST_REFERENCE(Suffix, SelfKind, RefKind)                                                         \
    Row(TEXT(#Suffix), DasKindPropertyName<SelfKind>(), false);
    DAS_SCALAR_ACCESSOR_KINDS(DAS_KIND_MANIFEST_SCALAR)
    DAS_REFERENCE_ACCESSOR_KINDS(DAS_KIND_MANIFEST_REFERENCE)
#undef DAS_KIND_MANIFEST_SCALAR
#undef DAS_KIND_MANIFEST_REFERENCE
    return TEXT("{\n  \"kinds\": [\n") + FString::Join(Rows, TEXT(",\n")) + TEXT("\n  ]\n}\n");
}

#include "DasRuntimeModuleDispatch.inl"
#include "DasRuntimeModuleExecution.inl"
IMPLEMENT_MODULE(FDasRuntimeModule, DasRuntime)

#include "DasBindingExport.inl"
#include "DasNativeGameplayPolicy.inl"
#include "DasNativeGameplayExport.inl"
#include "DasNativeGameplayCoverage.inl"

#if WITH_EDITOR
bool FDasRuntimeModule::BeginDefaultUpdate()
{
    if (!bInitialized || !IsInGameThread() || ActiveContext || bDrainingEvents || bPreservingSessions ||
        !PendingEvents.IsEmpty())
        return false;
    for (const auto& S : Sessions)
        if (S->Live && !S->Live->Context->Calls.IsEmpty())
            return false;
    UDasAsyncPhysicsBridge::RevokeAll();
    bPreservingSessions = true;
    return true; // Do not detach sessions, timers or subscriptions.
}
void FDasRuntimeModule::EndDefaultUpdate(UClass* RootClass, bool Applied)
{
    if (!bPreservingSessions || !IsInGameThread())
        return;
    if (Applied)
        for (const auto& S : Sessions)
            if (S->Owner.IsValid() && S->Owner->IsA(RootClass))
            {
                S->bDefaultsUpdateApproved = true;
                S->AttemptedSources.Reset();
            }
    bPreservingSessions = false;
}
bool FDasRuntimeModule::BeginPreserveContextsForReinstance(UClass* RootClass)
{
    if (!bInitialized || !IsInGameThread() || ActiveContext || bDrainingEvents || bPreservingSessions || !RootClass ||
        !PendingEvents.IsEmpty())
        return false;
    PruneSessions();
    // Call frames are invocation-local and own reflected addresses. Require an
    // empty table at this idle boundary before any host layout can change.
    for (const auto& S : Sessions)
        if (S->Live && !S->Live->Context->Calls.IsEmpty())
            return false;
    UDasAsyncPhysicsBridge::RevokeAll();
    bPreservingSessions = true;
    // Suspend all contexts: an unaffected owner can subscribe to a replaced peer.
    PreservedSessions = MoveTemp(Sessions);
    for (auto& S : PreservedSessions)
    {
        S->bReinstanceOwnerInScope = S->Owner.IsValid() && S->Owner->IsA(RootClass);
        S->bReinstanceOwnerChanged = false;
        S->ReinstanceLifecyclePath.Empty();
        auto RememberLifecycle = [&](auto* A)
        {
            if (A)
            {
                if (A->bLifecycleActive)
                    S->ReinstanceLifecyclePath = A->ActiveLifecyclePath;
                S->bReinstanceTickEnabled = A->IsActorTickEnabled();
            }
        };
        RememberLifecycle(Cast<ADasProbeActor>(S->Owner.Get()));
        RememberLifecycle(Cast<ADasPawn>(S->Owner.Get()));
        RememberLifecycle(Cast<ADasCharacter>(S->Owner.Get()));
        ClearSessionTimers(*S);
        S->Bindings.Empty();
        S->InputContexts.Empty(); // Remove old proxies before Unreal copies delegate properties.
    }
    return true;
}
void FDasRuntimeModule::EndPreserveContextsForReinstance(const FString& ApprovedDeclaration)
{
    if (!IsInGameThread() || !bPreservingSessions)
        return;
    for (auto& Saved : PreservedSessions)
        if (Saved->Owner.IsValid() && !Saved->bReleaseRequested)
        {
            if (auto* Actor = Cast<AActor>(Saved->Owner.Get()); Actor && Actor->IsActorBeingDestroyed())
                continue;
            auto RestoreLifecycle = [&](auto* Actor)
            {
                if (Actor && Saved->bReinstanceOwnerChanged && !Saved->ReinstanceLifecyclePath.IsEmpty() &&
                    !Saved->bEndDelivered)
                {
                    Actor->ActiveLifecyclePath = Saved->ReinstanceLifecyclePath;
                    Actor->bLifecycleActive = true;
                    Actor->SetActorTickEnabled(Saved->bReinstanceTickEnabled);
                }
            };
            RestoreLifecycle(Cast<ADasProbeActor>(Saved->Owner.Get()));
            RestoreLifecycle(Cast<ADasPawn>(Saved->Owner.Get()));
            RestoreLifecycle(Cast<ADasCharacter>(Saved->Owner.Get()));
            if (Saved->bReinstanceOwnerInScope)
                Saved->ApprovedDSLDeclaration = ApprovedDeclaration;
            if (Saved->bReinstanceOwnerInScope || Saved->bReinstanceOwnerChanged)
                Saved->AttemptedSources.Reset();
            if (Saved->Live && !Saved->bEndDelivered)
                InstallSubscriptions(*Saved);
            Saved->ReinstanceLifecyclePath.Empty();
            Sessions.Add(MoveTemp(Saved));
        }
    PreservedSessions.Empty();
    bPreservingSessions = false;
}
#endif

FDasExecutionResult FDasRuntimeModule::RunLibraryFrame(const FString& Contract, const FString& Script,
                                                       const FString& Entry, const TArray<FDasScalarValue>& Arguments,
                                                       EDasMethodSignature Kind, const FString& ReturnClass)
{
    FDasExecutionResult Failed;
    FDasAuthoringModule Model;
    FDasClassSchema Schema;
    if (!IsInGameThread() || !bInitialized || Script != FPaths::GetCleanFilename(Script) ||
        FPaths::GetExtension(Script) !=
            TEXT("das") || Contract.Len() > 1048576 || !FDasAuthoringModule::FromJson(Contract, Model, Failed.Error) ||
                 Model.Classes.Num() != 1 || !Model.ToActorSchema(Model.Classes[0].Id, Schema, Failed.Error) ||
                 Schema.Parent != TEXT("FunctionLibrary"))
    {
        if (Failed.Error.IsEmpty())
            Failed.Error = TEXT("Invalid static library invocation contract");
        return Failed;
    }
    const auto* Method = Schema.Methods.FindByPredicate(
        [&](const auto& M)
        {
            return M.EntryPoint == Entry && M.ScriptFile == Script && M.ReturnKind == Kind &&
                   M.ReturnClass == ReturnClass;
        });
    if (!Method)
    {
        Failed.Error = TEXT("Static library entry/result differs from saved declaration");
        return Failed;
    }
    UObject* Owner = GetMutableDefault<UDasBlueprintLibrary>();
    if (!Method->WorldContext.IsEmpty())
    {
        const int32 Index =
            Method->Parameters.IndexOfByPredicate([&](const auto& P) { return P.Name == Method->WorldContext; });
        if (!Arguments.IsValidIndex(Index) || Arguments[Index].Kind != 8)
        {
            Failed.Error = TEXT("Missing library WorldContext object");
            return Failed;
        }
        Owner = Arguments[Index].ObjectValue.Get();
        auto* World = IsValid(Owner) && GEngine
                          ? GEngine->GetWorldFromContextObject(Owner, EGetWorldErrorMode::ReturnNull)
                          : nullptr;
        if (!World || !World->IsGameWorld() || World->bIsTearingDown || Owner->IsTemplate() ||
            Owner->GetWorld() != World ||
            (Cast<AActor>(Owner) && CastChecked<AActor>(Owner)->IsActorBeingDestroyed()) ||
            (Cast<UActorComponent>(Owner) && CastChecked<UActorComponent>(Owner)->IsBeingDestroyed()))
        {
            Failed.Error = TEXT("Library requires a live game-world WorldContext");
            return Failed;
        }
    }
    TGuardValue<FString> ScriptScope(LibraryInvocationScript, Script);
    TGuardValue<const FString*> Scope(LibraryInvocationContract, &Contract);
    return RunNamedFramePersistent(ScriptPath(Script), Entry, Arguments, Kind, Owner, ReturnClass);
}
#include "DasPerformanceProbe.inl"

#include "DasExecutionGuardProbe.inl"

#include "DasAsyncPhysicsVM.inl"
