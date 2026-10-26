#include "Hash/CityHash.h"
#include "UObject/FieldPath.h"
#include "DasGameplayTags.h"
#include "DasTypeAdapters.h"
#include "DasUEAot.h"
#include "DasUEAotProbe.h"
#include "Async/Async.h"
#include "DasObjectFixture.h"
#include "DasExecutionRolePrivate.h"
#include "DasGASAuthoring.h"
#include "DasGASCalculations.h"
#include "DasMathAdapters.h"
#include "DasDiagnostics.h"
#include "Engine/CurveTable.h"
#include "Engine/DataTable.h"
#include "DasDataAssets.h"
#include "DasPrimaryAssets.h"
#include "DasSaveGame.h"
#include "DasSubsystems.h"
#include "Engine/GameInstance.h"
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
#include "DasWidgetLayouts.h"
#include "Blueprint/WidgetTree.h"
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
#include "DasInteropProbe.h"
#include "DasCallPhases.h"
#include "DasNativeBindsRegistry.h"
#include "DasScriptWriteTracker.h"
#include "HAL/PlatformFileManager.h"
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
#include "DasScript.h"
#include "DasFoundationManifest.h"
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
#include "DasScriptDelegateProxy.h"
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
#include "HAL/IConsoleManager.h"
#include "Misc/Guid.h"
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
#include "daScript/ast/ast_generate.h"
#include "daScript/simulate/aot_builtin_ast.h"
#include "daScript/simulate/data_walker.h"
THIRD_PARTY_INCLUDES_END
#pragma pack(pop)
#pragma pop_macro("check")
#pragma pop_macro("ensure")
#pragma pop_macro("verify")
#include "DasScriptTypeFacet.h"

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
MAKE_TYPE_FACTORY(UEEnumValue, FDasEnumValue); // DasScriptValues.h
namespace das
{
template <> struct cast<FDasEnumValue>
{
    static vec4f from(FDasEnumValue V) { return cast<uint64_t>::from(V.Token); }
    static FDasEnumValue to(vec4f V) { return {cast<uint64_t>::to(V)}; }
};
}
MAKE_TYPE_FACTORY(UETextValue, FDasTextValue); // DasScriptValues.h
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
MAKE_TYPE_FACTORY(UENameValue, FDasNameValue); // DasScriptValues.h
namespace das
{
template <> struct cast<FDasNameValue>
{
    static vec4f from(FDasNameValue V) { return cast<uint64_t>::from(V.Token); }
    static FDasNameValue to(vec4f V) { return {cast<uint64_t>::to(V)}; }
};
}
// FDateTime and FTimespan as script values (ticks, DasScriptValues.h), for native binds (module ue_time).
MAKE_TYPE_FACTORY(UEDateTime, FDasDateTimeValue);
MAKE_TYPE_FACTORY(UETimespan, FDasTimespanValue);
namespace das
{
template <> struct cast<FDasDateTimeValue>
{
    static vec4f from(FDasDateTimeValue V) { return cast<int64_t>::from(V.Ticks); }
    static FDasDateTimeValue to(vec4f V) { return {cast<int64_t>::to(V)}; }
};
template <> struct cast<FDasTimespanValue>
{
    static vec4f from(FDasTimespanValue V) { return cast<int64_t>::from(V.Ticks); }
    static FDasTimespanValue to(vec4f V) { return {cast<int64_t>::to(V)}; }
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
MAKE_TYPE_FACTORY(UObjectHandle, FDasObjectHandle); // DasScriptValues.h
MAKE_TYPE_FACTORY(UEClassHandle, FDasClassHandle); // DasScriptValues.h
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

MAKE_TYPE_FACTORY(UEConstraintHandle, FDasConstraintHandle); // DasScriptValues.h
namespace das
{
template <> struct cast<FDasConstraintHandle>
{
    static vec4f from(FDasConstraintHandle V) { return cast<uint64_t>::from(V.Token); }
    static FDasConstraintHandle to(vec4f V) { return {cast<uint64_t>::to(V)}; }
};
}
MAKE_TYPE_FACTORY(UEHitResult, FDasHitResultValue); // DasScriptValues.h
namespace das
{
template <> struct cast<FDasHitResultValue>
{
    static vec4f from(FDasHitResultValue V) { return cast<uint64_t>::from(V.Token); }
    static FDasHitResultValue to(vec4f V) { return {cast<uint64_t>::to(V)}; }
};


}
MAKE_TYPE_FACTORY(UEStructValue, FDasStructValue); // DasScriptValues.h
namespace das
{
template <> struct cast<FDasStructValue>
{
    static vec4f from(FDasStructValue V) { return cast<uint64_t>::from(V.Token); }
    static FDasStructValue to(vec4f V) { return {cast<uint64_t>::to(V)}; }
};
}
// Owned numeric snapshot, not the unreflected native layout and not a context token.
MAKE_TYPE_FACTORY(UEEventReply, FDasEventReplyValue); // DasScriptValues.h
MAKE_TYPE_FACTORY(UEWidgetAnimationHandle, FDasWidgetAnimationValue); // DasScriptValues.h
namespace das
{
template <> struct cast<FDasWidgetAnimationValue>
{
    static vec4f from(FDasWidgetAnimationValue V) { return cast<uint64_t>::from(V.Token); }
    static FDasWidgetAnimationValue to(vec4f V) { return {cast<uint64_t>::to(V)}; }
};
}
MAKE_TYPE_FACTORY(UEPaintContext, FDasPaintValue); // DasScriptValues.h
namespace das
{
template <> struct cast<FDasPaintValue>
{
    static vec4f from(FDasPaintValue V) { return cast<uint64_t>::from(V.Token); }
    static FDasPaintValue to(vec4f V) { return {cast<uint64_t>::to(V)}; }
};
}
MAKE_TYPE_FACTORY(UEGeometry, FDasGeometryValue); // DasScriptValues.h
namespace das
{
template <> struct cast<FDasGeometryValue>
{
    static vec4f from(FDasGeometryValue V) { return cast<uint64_t>::from(V.Token); }
    static FDasGeometryValue to(vec4f V) { return {cast<uint64_t>::to(V)}; }
};
}
MAKE_TYPE_FACTORY(UEInputEvent, FDasInputEventValue); // DasScriptValues.h
namespace das
{
template <> struct cast<FDasInputEventValue>
{
    static vec4f from(FDasInputEventValue V) { return cast<uint64_t>::from(V.Token); }
    static FDasInputEventValue to(vec4f V) { return {cast<uint64_t>::to(V)}; }
};
}
MAKE_TYPE_FACTORY(UEKeyEvent, FDasKeyEventValue); // DasScriptValues.h
namespace das
{
template <> struct cast<FDasKeyEventValue>
{
    static vec4f from(FDasKeyEventValue V) { return cast<uint64_t>::from(V.Token); }
    static FDasKeyEventValue to(vec4f V) { return {cast<uint64_t>::to(V)}; }
};
}
MAKE_TYPE_FACTORY(UEAnalogInputEvent, FDasAnalogInputEventValue); // DasScriptValues.h
namespace das
{
template <> struct cast<FDasAnalogInputEventValue>
{
    static vec4f from(FDasAnalogInputEventValue V) { return cast<uint64_t>::from(V.Token); }
    static FDasAnalogInputEventValue to(vec4f V) { return {cast<uint64_t>::to(V)}; }
};
}
MAKE_TYPE_FACTORY(UECharacterEvent, FDasCharacterEventValue); // DasScriptValues.h
namespace das
{
template <> struct cast<FDasCharacterEventValue>
{
    static vec4f from(FDasCharacterEventValue V) { return cast<uint64_t>::from(V.Token); }
    static FDasCharacterEventValue to(vec4f V) { return {cast<uint64_t>::to(V)}; }
};
}
MAKE_TYPE_FACTORY(UEPointerEvent, FDasPointerEventValue); // DasScriptValues.h
namespace das
{
template <> struct cast<FDasPointerEventValue>
{
    static vec4f from(FDasPointerEventValue V) { return cast<uint64_t>::from(V.Token); }
    static FDasPointerEventValue to(vec4f V) { return {cast<uint64_t>::to(V)}; }
};
}
MAKE_TYPE_FACTORY(UEFocusEvent, FDasFocusEventValue); // DasScriptValues.h
namespace das
{
template <> struct cast<FDasFocusEventValue>
{
    static vec4f from(FDasFocusEventValue V) { return cast<uint64_t>::from(V.Token); }
    static FDasFocusEventValue to(vec4f V) { return {cast<uint64_t>::to(V)}; }
};
}
MAKE_TYPE_FACTORY(UENavigationEvent, FDasNavigationEventValue); // DasScriptValues.h
namespace das
{
template <> struct cast<FDasNavigationEventValue>
{
    static vec4f from(FDasNavigationEventValue V) { return cast<uint64_t>::from(V.Token); }
    static FDasNavigationEventValue to(vec4f V) { return {cast<uint64_t>::to(V)}; }
};
}
MAKE_TYPE_FACTORY(InputActionValue, FDasInputActionValue); // DasScriptValues.h
MAKE_TYPE_FACTORY(UEVector2D, FVector2D);
MAKE_TYPE_FACTORY(UEVector, FVector);
MAKE_TYPE_FACTORY(UERotator, FRotator);
MAKE_TYPE_FACTORY(UEQuat, FQuat);
MAKE_TYPE_FACTORY(UELinearColor, FLinearColor);
MAKE_TYPE_FACTORY(UEColor, FColor);
MAKE_TYPE_FACTORY(UETransform, FTransform);
MAKE_TYPE_FACTORY(UETraceHit, FDasTraceHit); // DasScriptValues.h
MAKE_TYPE_FACTORY(UEOverlapResult, FDasOverlapResult); // DasScriptValues.h
MAKE_TYPE_FACTORY(UCallHandle, FDasCallHandle); // DasScriptValues.h

DEFINE_LOG_CATEGORY_STATIC(LogDasRuntime, Log, All);

namespace
{
#include "DasUEExternGuard.inl"
struct FSourceSnapshot
{
    bool bExists = false;
    TArray<uint8> Bytes;
    // Size and modification time the bytes were read with: the watch-mode poll compares these first.
    int64 Size = -1;
    FDateTime Stamp, ReadAt;
    static FSourceSnapshot Read(const FString& Path)
    {
        FSourceSnapshot Result;
        Result.bExists = IFileManager::Get().FileExists(*Path) && FFileHelper::LoadFileToArray(Result.Bytes, *Path);
        if (Result.bExists)
        {
            const FFileStatData Stat = IFileManager::Get().GetStatData(*Path);
            Result.Size = Stat.FileSize;
            Result.Stamp = Stat.ModificationTime;
            Result.ReadAt = FDateTime::UtcNow();
        }
        return Result;
    }
    // File times advance in coarse steps (about 15 ms on Windows): a same-size write soon after the snapshot can
    // keep size and time. Like git's racy entries, such a snapshot is compared by bytes until it settles.
    bool IsRacy() const { return bExists && ReadAt - Stamp < FTimespan::FromSeconds(2); }
    bool StatMatches(const FFileStatData& Stat) const
    {
        return bExists ? Stat.bIsValid && Stat.FileSize == Size && Stat.ModificationTime == Stamp : !Stat.bIsValid;
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
#include "DasBuiltinValueOps.inl"
#include "DasRuntimeStructValues.inl"
#include "DasRuntimeFramesWidgets.inl"
#include "DasRuntimeObjectAccess.inl"
#include "DasRuntimeMethodCalls.inl"
#include "DasRuntimeSceneComponents.inl"
#include "DasRuntimeSubscriptionApi.inl"
#include "DasRuntimeTypeAnnotations.inl"
#include "DasTypeAdaptersBuiltin.inl"
#include "DasPersistentGlobals.inl"
#include "DasCollisionNames.inl"
#include "DasFoundationModule.inl"
#include "DasNativeBinds.inl"
#include "DasRuntimeTimers.inl"
#include "DasDynamicEvents.inl"
#include "DasScriptDelegateBindings.inl"
#include "DasRuntimeGASCueSessions.inl"
#include "DasRuntimeInputSessions.inl"
}

TArray<FDasNativeModuleInfo> DescribeDasNativeModules()
{
    EnsureDasNativeModules();
    TArray<FDasNativeModuleInfo> Result;
    for (const FDasNativeModuleDesc* Desc : DasNativeModuleDescs())
    {
        if (!Desc->bLoaded)
            continue;
        FDasNativeModuleInfo& Module = Result.AddDefaulted_GetRef();
        Module.Name = Desc->Name;
        Module.Error = Desc->Error;
        for (const FDasNativeClassDecl& Decl : Desc->Classes)
            Module.Classes.Add(Decl.ScriptName);
        if (!Desc->bBuilt)
            continue;
        auto Describe = [&](const FDasNativeFunctionDesc& F, const TCHAR* Origin)
        {
            FDasNativeFunctionInfo& Info = Module.Functions.AddDefaulted_GetRef();
            Info.Name = F.Name;
            for (int32 I = 0; I < F.Arguments.Num(); ++I)
                Info.Arguments.Add(DasNativeScriptTypeName(F.Arguments[I], F.Resolved[I]));
            Info.Receiver = F.IsMethod() ? Info.Arguments[0] : FString();
            Info.Result = DasNativeScriptTypeName(F.Result, F.Resolved.Last());
            Info.Origin = Origin;
            Info.bPure = EnumHasAnyFlags(F.Flags, EDasBind::Pure);
            Info.bNonNullArgs = EnumHasAnyFlags(F.Flags, EDasBind::NonNullArgs);
            if (F.ReflectedPath.IsEmpty() || FCString::Strcmp(Origin, TEXT("declared")) != 0)
                return;
            Info.Reflects = F.ReflectedPath;
            const UFunction* U = FindObject<UFunction>(nullptr, *F.ReflectedPath);
            // Gameplay scripts pass objects as native reference types (the route takes their handles): each object
            // class of the bind needs one.
            FString MissingView;
            auto CheckView = [&](const FDasNativeParam& P, const FDasNativeResolvedParam& R)
            {
                if (P.Type != EDasNativeType::Object || !MissingView.IsEmpty())
                    return;
                const FString Path = R.Class ? R.Class->GetPathName() : FString();
                bool bFound = false;
                for (const auto& View : DasNativeGameplayViews)
                    bFound |= Path == View.Class;
                if (!bFound)
                    MissingView = TEXT("no native reference type for ") + Path;
            };
            CheckView(F.Result, F.Resolved.Last());
            for (int32 I = 1; I < F.Arguments.Num(); ++I)
                CheckView(F.Arguments[I], F.Resolved[I]);
            // Gameplay references of descendant classes also call the bind: without a row for the receiver class
            // (UActorComponent.GetOwner), the policy of a descendant's row applies.
            const UClass* PolicyClass = F.Receiver();
            if (U && !DasNativeGameplayMethod(PolicyClass->GetPathName(), U->GetName()))
                for (const auto& Row : DasNativeGameplayMethods)
                    if (U->GetName() == Row.Name)
                        if (const UClass* Class = FindObject<UClass>(nullptr, Row.Class);
                            Class && Class->IsChildOf(PolicyClass))
                        {
                            PolicyClass = Class;
                            break;
                        }
            Info.RouteNote = !U                      ? FString(TEXT("UFUNCTION not found"))
                             : !MissingView.IsEmpty() ? MissingView
                                                      : DasNativeRoutePolicy(U, PolicyClass);
            Info.bRouted = Info.RouteNote.IsEmpty();
        };
        for (const FDasNativeFunctionDesc& F : Desc->Functions)
            Describe(F, TEXT("declared"));
        for (const TUniquePtr<FDasNativeFunctionDesc>& F : Desc->Generated)
            Describe(*F, F->Helper == EDasNativeHelper::Call ? TEXT("inherited") : TEXT("helper"));
    }
    return Result;
}

char* DasNativeAotString(const FString& Value, das::Context* Context)
{
    return CopyDSLString(Value, Context);
}
FName DasNativeAotName(FDasNameValue Value, das::Context* Context)
{
    FName Out;
    if (const TCHAR* Error = DasNativeNameArgument(Value, Context, Out))
        DasNativeError(Error);
    return Out;
}
FText DasNativeAotText(FDasTextValue Value, das::Context* Context)
{
    FText Out;
    if (const TCHAR* Error = DasNativeTextArgument(Value, Context, Out))
        DasNativeError(Error);
    return Out;
}
FDasNameValue DasNativeAotNameValue(FName Value, das::Context* Context)
{
    return MakeDSLName(Value, Context);
}
FDasTextValue DasNativeAotTextValue(const FText& Value, das::Context* Context)
{
    return MakeDSLText(Value, Context);
}
static_assert(sizeof(FDasArrayValue) == sizeof(das::Array) && alignof(FDasArrayValue) == alignof(das::Array),
              "FDasArrayValue must have the layout of das::Array");
// An array result of a compiled call: made in the context's heap like SimNode_DasNative::ReturnObjects (an object
// that is not a Class reads as null).
// The checks of SimNode_DasNativeRoute for a compiled routed call, with the interpreter's errors.
void DasNativeAotRoute(const FDasObjectHandle& Self, const char* ClassPath, das::Context* Context)
{
    NativeGameplayObject(Self, ClassPath, true, Context);
}
void DasNativeAotRouteArgument(const FDasObjectHandle& Handle, das::Context* Context)
{
    if (Handle.Token && !TryObject(Handle, Context))
        Context->throw_error("object handle is null, expired, or belongs to another context");
}
void DasNativeAotRouted()
{
    ++DasNativeRoutedCallCount();
}
FDasArrayValue DasNativeAotObjects(UObject* const* Objects, int32 Count, UClass* Class, das::Context* Context)
{
    FDasArrayValue Result;
    auto& Array = *reinterpret_cast<das::Array*>(&Result);
    FMemory::Memzero(&Array, sizeof(das::Array));
    das::builtin_array_resize(Array, Count, sizeof(FDasObjectHandle), Context, nullptr);
    auto* Handles = reinterpret_cast<FDasObjectHandle*>(Array.data);
    for (int32 I = 0; I < Count; ++I)
        Handles[I] = DasNativeAotHandle(Objects[I], Class, Context);
    return Result;
}
// The checks of SimNode_DasNative::ResolveArgument and ReturnObject for compiled calls; errors go to the AOT call's
// slot with the interpreter's text (Finish adds the function's label).
UObject* DasNativeAotObject(const FDasObjectHandle& Handle, UClass* Class, das::Context* Context, int32 Number,
                            EDasNativeAotObject Kind)
{
    UObject* Object = Handle.Token ? TryObject(Handle, Context) : nullptr;
    if (Object && !Object->IsA(Class))
    {
        DasNativeError(FString::Printf(TEXT("argument %d must be %s, got %s"), Number, *Class->GetName(),
                                       *Object->GetClass()->GetName()));
        return nullptr;
    }
    if (!Object && Kind == EDasNativeAotObject::Receiver)
        DasNativeError(TEXT("receiver is null or expired"));
    if (!Object && Kind == EDasNativeAotObject::NonNullArgument)
        DasNativeError(FString::Printf(TEXT("argument %d is null or expired"), Number));
    return Object;
}
UClass* DasNativeAotClass(FDasClassHandle Handle, UClass* Base, das::Context* Context, int32 Number)
{
    if (!Handle.Token)
        return nullptr;
    UClass* Class = Cast<UClass>(TryObject(FDasObjectHandle{Handle.Token}, Context));
    if (!Class)
        DasNativeError(TEXT("object handle is null, expired, or belongs to another context"));
    else if (!Class->IsChildOf(Base))
    {
        DasNativeError(FString::Printf(TEXT("argument %d must be a subclass of %s, got %s"), Number, *Base->GetName(),
                                       *Class->GetName()));
        return nullptr;
    }
    return Class;
}
FDasObjectHandle DasNativeAotHandle(const UObject* Object, UClass* Class, das::Context* Context)
{
    return Object && Object->IsA(Class) ? MakeObjectHandle(const_cast<UObject*>(Object), Class, Context)
                                        : FDasObjectHandle();
}
void dasue::aot::Finish(uint64_t Key, das::Context* Context)
{
    ErrorPending = false;
    FString& Error = DasNativeAotPendingError();
    if (Error.IsEmpty())
        return;
    const auto* Entry = UEAotEntries.Find(Key);
    const FString Message = (Entry ? Entry->Label : FString(TEXT("native bind"))) + TEXT(": ") + Error;
    Error.Empty();
    Context->throw_error(TCHAR_TO_UTF8(*Message));
}

das::Context* dasue::aot::CurrentContext()
{
    return ActiveContext;
}
void* dasue::aot::Resolve(uint64_t Key, das::Context* Context, FCallSite& Site)
{
    if (!Context)
        throw std::runtime_error("UE AOT call requires a context");
    // Context != ActiveContext also covers a missing active context; the thread check runs after the cheap one.
    if (Context != ActiveContext || !IsInGameThread())
        Context->throw_error("UE AOT requires the active game-thread context");
    GuardUEExtern();
    // Game thread only (checked above), so the call site's cache needs no synchronization.
    if (Site.Generation != UEAotGeneration)
    {
        const auto* Found = UEAotEntries.Find(Key);
        if (!Found)
            Context->throw_error("UE AOT extern ABI is absent or stale; regenerate and rebuild");
        Site = {Found, UEAotGeneration};
    }
    const auto* Entry = static_cast<const FUEAotEntry*>(Site.Entry);
    if (Entry->Loaded && !*Entry->Loaded)
        Context->throw_error(TCHAR_TO_UTF8(*(Entry->Label + TEXT(": its native module was unloaded"))));
    return Entry->Function;
}

// Declared by DasTypeAdapters.cpp; the adapters themselves live in the anonymous namespace.
void DasRegisterBuiltinTypeAdapters(FDasTypeAdapters& Registry)
{
    RegisterBuiltinTypeAdapters(Registry);
}
void FDasRuntimeModule::StartupModule()
{
    DasInstallScriptWriteTracker();
    FModuleManager::LoadModuleChecked<IModuleInterface>(TEXT("EnhancedInput"));
    Root = FPaths::ConvertRelativePathToFull(IPluginManager::Get().FindPlugin(TEXT("DasForUE"))->GetContentDir() /
                                                                              TEXT("Das"));
    const FTCHARToUTF8 Utf8Root(*Root);
    das::setDasRoot(das::string(Utf8Root.Get(), Utf8Root.Length()));
    NEED_ALL_DEFAULT_MODULES;
    UEAotEntries.Reset();
    ++UEAotGeneration;
    das::Module::Initialize();
    new FUnrealFoundationModule();
    InputRestoreTickHandle = FWorldDelegates::OnWorldTickStart.AddLambda(
        [](UWorld* World, ELevelTick, float)
        {
            for (const auto& Session : Sessions)
                if (Session->Owner.IsValid() && Session->Owner->IsA<UDasGameInstanceSubsystem>())
                    Session->World = Session->Owner->GetWorld();
            ReconcileScriptInput(World);
        });
    WorldCleanupHandle = FWorldDelegates::OnWorldCleanup.AddLambda(
        [](UWorld* World, bool, bool)
        {
            UDasAsyncPhysicsBridge::RevokeWorld(World);
            for (auto It = InitializerDeliveries.CreateIterator(); It; ++It)
            {
                auto* O = It.Key().Get();
                if (!O ||
                    (O->GetWorld() == World && !O->IsA<UDasGameInstanceSubsystem>() && !O->IsA<UDasWorldSubsystem>()))
                    It.RemoveCurrent();
            }
            for (const auto& Session : Sessions)
                if (Session->World.Get() == World && Session->Owner.IsValid() &&
                    Session->Owner->IsA<UDasGameInstanceSubsystem>())
                {
                    ClearSessionTimers(*Session);
                    Session->Bindings.Empty();
                    Session->InputContexts.Empty();
                    Session->InputRecipes.Empty();
                    Session->ContextRecipes.Empty();
                }
            for (const auto& Session : Sessions)
                if (Session->World.Get() == World &&
                    (!Session->Owner.IsValid() ||
                     (!Session->Owner->IsA<UDasGameInstanceSubsystem>() && !Session->Owner->IsA<UDasWorldSubsystem>())))
                    RevokeSession(*Session);
#if WITH_EDITOR
            for (const auto& Session : PreservedSessions)
                if (Session->World.Get() == World &&
                    (!Session->Owner.IsValid() ||
                     (!Session->Owner->IsA<UDasGameInstanceSubsystem>() && !Session->Owner->IsA<UDasWorldSubsystem>())))
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
                    for (auto& Pair : C.ScriptDelegateProxies)
                        if (UObject* const* New = Map.Find(Pair.Value->Owner.GetEvenIfUnreachable()))
                            Pair.Value->Owner = *New;
                    for (auto& Binding : C.OwnedScriptDelegateEvents)
                        if (UObject* const* New = Map.Find(Binding->Source.GetEvenIfUnreachable()))
                            Binding->Source = *New;
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

// das.StructStats: the snapshot diagnostics so far (DasStructStatsLine).
static FAutoConsoleCommand GDasStructStatsCommand(TEXT("das.StructStats"),
                                                  TEXT("Logs struct snapshot counts per top-level execution"),
                                                  FConsoleCommandDelegate::CreateLambda(
                                                      []() { UE_LOG(LogDasRuntime, Display, TEXT("%s"),
                                                                    *DasStructStatsLine()); }));
void FDasRuntimeModule::ShutdownModule()
{
    UE_LOG(LogDasRuntime, Display, TEXT("%s"), *DasStructStatsLine());
    DasRemoveScriptWriteTracker();
    // Release rooted callback signatures while UObject storage is still valid.
    FCoreDelegates::OnPreExit.Remove(RuntimePreExitHandle);
    ResetDasSubsystemClasses();
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
        ++SessionSetEpoch;
        PendingEvents.Empty();
        DropPendingGASCueHooks();
        CreatedScriptScenes.Empty();
        UDasIntDelegateProxy::ResetDynamicReceivers();
        UDasScriptDelegateProxy::ResetReceivers();
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
    int32 Slash;
    if (Name.FindChar(TEXT('/'), Slash) || Name.FindChar(TEXT('\\'), Slash))
        return Root / TEXT("Scripts") / Name;
    // A project script shadows the plugin script of the same name. The file check costs tens of microseconds,
    // so the answer is cached until a reported folder change or the poll interval (NotifyScriptsChanged).
    const bool bCache = IsInGameThread();
    if (bCache)
        DrainScriptWrites(); // a script just written in this process may shadow the plugin one
    const double Now = FPlatformTime::Seconds();
    if (const auto* Cached = bCache ? ScriptPathCache.Find(Name) : nullptr;
        Cached && Now - Cached->CheckedAt < CVarScriptChangePollSeconds.GetValueOnGameThread())
        return Cached->Path;
    const FString Project = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir() / TEXT("Das/Scripts") / Name);
    FString Path = IFileManager::Get().FileExists(*Project) ? Project : Root / TEXT("Scripts") / Name;
    if (bCache)
        ScriptPathCache.Add(Name, {Path, Now});
    return Path;
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
                // Routes of reflected UFUNCTIONs (AddRoute) appear when a native module is built and are numbered
                // per build: internal, not part of the script-visible surface.
                if (Fn->name.rfind("__ue_native_route_", 0) == 0)
                    return;
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

// Informational: cost of the guard every UE extern call runs (foundation benchmark breakdown).
void DasSetSelfSitesEnabled(bool bEnabled)
{
    GDasSelfSitesEnabled = bEnabled;
}
double DasExternGuardBenchmarkNs(int32 Iterations)
{
    const double Start = FPlatformTime::Seconds();
    for (int32 I = 0; I < Iterations; ++I)
        GuardUEExtern();
    return (FPlatformTime::Seconds() - Start) * 1e9 / FMath::Max(1, Iterations);
}

#include "DasRuntimeModuleDispatch.inl"
#include "DasRuntimeModuleExecution.inl"
#include "DasScriptFunctionCalls.inl"
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
    ++SessionSetEpoch; // sessions that were not restored are gone
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
#include "DasInteropProbe.inl"

#include "DasExecutionGuardProbe.inl"
#include "DasUEAotProbe.inl"

#include "DasAsyncPhysicsVM.inl"

TMap<FString, FName> DasCollisionProfileConstants()
{
    return CollisionProfileSymbols;
}

FDasExecutionResult FDasRuntimeModule::RunScriptDelegateFrame(const TSharedPtr<FDasScriptDelegateLease>& Generation,
                                                              const FString& Path, const FString& Entry,
                                                              const TArray<FDasScalarValue>& Arguments,
                                                              EDasMethodSignature ReturnKind,
                                                              TWeakObjectPtr<UObject> Owner, const FString& ReturnClass)
{
    if (!IsInGameThread())
    {
        FDasExecutionResult R;
        R.Error = TEXT("Script delegates require the game thread");
        return R;
    }
    if (!Generation.IsValid() || !Generation->Active)
    {
        FDasExecutionResult R;
        R.Error = TEXT("Script delegate VM generation was revoked");
        return R;
    }
    TGuardValue<const FDasScriptDelegateLease*> Guard(ExpectedDelegateGeneration, Generation.Get());
    return RunNamedFramePersistent(Path, Entry, Arguments, ReturnKind, Owner, ReturnClass);
}
