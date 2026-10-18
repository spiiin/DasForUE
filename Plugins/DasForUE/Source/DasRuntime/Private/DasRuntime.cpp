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

MAKE_TYPE_FACTORY(UEPhysicsBodyToken,FDasPhysicsBodyToken);
namespace das {
template<>struct cast<FDasPhysicsBodyToken>{static vec4f from(FDasPhysicsBodyToken V){return cast<uint64_t>::from(V.Value);}static FDasPhysicsBodyToken to(vec4f V){return {cast<uint64_t>::to(V)};}};
template<>struct ext_ret_carrier<FDasPhysicsBodyToken>{using type=uint64_t;};
template<>struct ext_ret_to_carrier<FDasPhysicsBodyToken,uint64_t>{static uint64_t from(FDasPhysicsBodyToken V){return V.Value;}};
}
// No fields are exposed to scripts: this is a context-local capability, never a pointer.
struct FDasEnumValue {uint64 Token=0;};
MAKE_TYPE_FACTORY(UEEnumValue,FDasEnumValue);
namespace das {template<> struct cast<FDasEnumValue>{static vec4f from(FDasEnumValue V){return cast<uint64_t>::from(V.Token);}static FDasEnumValue to(vec4f V){return {cast<uint64_t>::to(V)};}};}
struct FDasTextValue { uint64 Token=0; };
MAKE_TYPE_FACTORY(UETextValue,FDasTextValue);
namespace das {
template<> struct cast<FDasTextValue>{static vec4f from(FDasTextValue V){return cast<uint64_t>::from(V.Token);}static FDasTextValue to(vec4f V){return {cast<uint64_t>::to(V)};}};
template<> struct ext_ret_carrier<FDasTextValue>{using type=uint64_t;};
template<> struct ext_ret_to_carrier<FDasTextValue,uint64_t>{static uint64_t from(FDasTextValue V){return V.Token;}};
}
struct FDasNameValue { uint64 Token=0; };
MAKE_TYPE_FACTORY(UENameValue,FDasNameValue);
namespace das {template<> struct cast<FDasNameValue>{static vec4f from(FDasNameValue V){return cast<uint64_t>::from(V.Token);}static FDasNameValue to(vec4f V){return {cast<uint64_t>::to(V)};}};}
// Keep native returns and the annotation's uint64 value representation in the same ABI slot.
namespace das {
template<> struct ext_ret_carrier<FDasEnumValue> {using type=uint64_t;};
template<> struct ext_ret_to_carrier<FDasEnumValue,uint64_t> {static uint64_t from(FDasEnumValue V){return V.Token;}};
template<> struct ext_ret_carrier<FDasNameValue> {using type=uint64_t;};
template<> struct ext_ret_to_carrier<FDasNameValue,uint64_t> {static uint64_t from(FDasNameValue V){return V.Token;}};
}
struct FDasObjectHandle { uint64 Token = 0; };
MAKE_TYPE_FACTORY(UObjectHandle, FDasObjectHandle);
struct FDasClassHandle { uint64 Token = 0; };
MAKE_TYPE_FACTORY(UEClassHandle, FDasClassHandle);
namespace das {
template<> struct cast<FDasClassHandle>{static vec4f from(FDasClassHandle V){return cast<uint64_t>::from(V.Token);}static FDasClassHandle to(vec4f V){return {cast<uint64_t>::to(V)};}};
template<> struct ext_ret_carrier<FDasClassHandle>{using type=uint64_t;};
template<> struct ext_ret_to_carrier<FDasClassHandle,uint64_t>{static uint64_t from(FDasClassHandle V){return V.Token;}};
}

struct FDasConstraintHandle {uint64 Token=0;};
MAKE_TYPE_FACTORY(UEConstraintHandle,FDasConstraintHandle);
namespace das{template<>struct cast<FDasConstraintHandle>{static vec4f from(FDasConstraintHandle V){return cast<uint64_t>::from(V.Token);}static FDasConstraintHandle to(vec4f V){return {cast<uint64_t>::to(V)};}};}
struct FDasHitResultValue {uint64 Token=0;};
MAKE_TYPE_FACTORY(UEHitResult,FDasHitResultValue);
namespace das {
template<>struct cast<FDasHitResultValue>{static vec4f from(FDasHitResultValue V){return cast<uint64_t>::from(V.Token);}static FDasHitResultValue to(vec4f V){return {cast<uint64_t>::to(V)};}};


}
struct FDasStructValue {uint64 Token=0;};
MAKE_TYPE_FACTORY(UEStructValue,FDasStructValue);
namespace das{template<>struct cast<FDasStructValue>{static vec4f from(FDasStructValue V){return cast<uint64_t>::from(V.Token);}static FDasStructValue to(vec4f V){return {cast<uint64_t>::to(V)};}};}
// Owned numeric snapshot, not the unreflected native layout and not a context token.
struct FDasInputEventValue { uint64 Token=0; };
MAKE_TYPE_FACTORY(UEInputEvent,FDasInputEventValue);
namespace das{template<>struct cast<FDasInputEventValue>{static vec4f from(FDasInputEventValue V){return cast<uint64_t>::from(V.Token);}static FDasInputEventValue to(vec4f V){return {cast<uint64_t>::to(V)};}};}
struct FDasKeyEventValue { uint64 Token=0; };
MAKE_TYPE_FACTORY(UEKeyEvent,FDasKeyEventValue);
namespace das{template<>struct cast<FDasKeyEventValue>{static vec4f from(FDasKeyEventValue V){return cast<uint64_t>::from(V.Token);}static FDasKeyEventValue to(vec4f V){return {cast<uint64_t>::to(V)};}};}
struct FDasAnalogInputEventValue { uint64 Token=0; };
MAKE_TYPE_FACTORY(UEAnalogInputEvent,FDasAnalogInputEventValue);
namespace das{template<>struct cast<FDasAnalogInputEventValue>{static vec4f from(FDasAnalogInputEventValue V){return cast<uint64_t>::from(V.Token);}static FDasAnalogInputEventValue to(vec4f V){return {cast<uint64_t>::to(V)};}};}
struct FDasCharacterEventValue { uint64 Token=0; };
MAKE_TYPE_FACTORY(UECharacterEvent,FDasCharacterEventValue);
namespace das{template<>struct cast<FDasCharacterEventValue>{static vec4f from(FDasCharacterEventValue V){return cast<uint64_t>::from(V.Token);}static FDasCharacterEventValue to(vec4f V){return {cast<uint64_t>::to(V)};}};}
struct FDasPointerEventValue { uint64 Token=0; };
MAKE_TYPE_FACTORY(UEPointerEvent,FDasPointerEventValue);
namespace das{template<>struct cast<FDasPointerEventValue>{static vec4f from(FDasPointerEventValue V){return cast<uint64_t>::from(V.Token);}static FDasPointerEventValue to(vec4f V){return {cast<uint64_t>::to(V)};}};}
struct FDasNavigationEventValue { uint64 Token=0; };
MAKE_TYPE_FACTORY(UENavigationEvent,FDasNavigationEventValue);
namespace das{template<>struct cast<FDasNavigationEventValue>{static vec4f from(FDasNavigationEventValue V){return cast<uint64_t>::from(V.Token);}static FDasNavigationEventValue to(vec4f V){return {cast<uint64_t>::to(V)};}};}
struct FDasInputActionValue { double X=0,Y=0,Z=0; int32 Kind=0; };
MAKE_TYPE_FACTORY(InputActionValue,FDasInputActionValue);
MAKE_TYPE_FACTORY(UEVector2D,FVector2D);
MAKE_TYPE_FACTORY(UEVector,FVector);
MAKE_TYPE_FACTORY(UERotator,FRotator);
MAKE_TYPE_FACTORY(UETransform,FTransform);
// A source-side checked view of a native FHitResult. References are weak handles.
struct FDasTraceHit {
    uint64 Scope=0;
    bool BlockingHit=false,StartPenetrating=false;
    double Distance=0,Time=0;
    FVector Location=FVector::ZeroVector,ImpactPoint=FVector::ZeroVector,Normal=FVector::ZeroVector,ImpactNormal=FVector::ZeroVector;
    FVector TraceStart=FVector::ZeroVector,TraceEnd=FVector::ZeroVector;
    double PenetrationDepth=0;
    int32 FaceIndex=-1,Item=-1,MyItem=-1,ElementIndex=0;
    FName BoneName,MyBoneName;
    FDasObjectHandle Actor{},Component{},PhysMaterial{},PhysicsObjectOwner{};
};
MAKE_TYPE_FACTORY(UETraceHit,FDasTraceHit);
struct FDasOverlapResult {
 uint64 Scope=0;bool BlockingHit=false;int32 ItemIndex=-1,MyItemIndex=-1;
 FDasObjectHandle Actor{},Component{},PhysicsObjectOwner{};
};
MAKE_TYPE_FACTORY(UEOverlapResult,FDasOverlapResult);
struct FDasCallHandle { uint64 Token = 0; };
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
    TSharedPtr<IDasPreparedAsyncPhysicsVM> PreparePhysicsVMImpl(const FString&,UObject*,FString&,FSourceSet*);
    void AsyncPhysicsSetInput(int32,double,das::Context*);
    bool AsyncPhysicsHasOutput(das::Context*);
    double AsyncPhysicsOutput(int32,das::Context*);
    bool AsyncPhysicsRunning(das::Context*);
    char* AsyncPhysicsError(das::Context*);
    FSourceSet* AuthoredReferenceSources=nullptr;

    class FUnrealTextFile final : public das::FileInfo
    {
    public:
        TArray<uint8> Bytes;
        virtual void getSourceAndLength(const char*& Source, uint32_t& Length) override
        {
            Source = reinterpret_cast<const char*>(Bytes.GetData());
            Length = static_cast<uint32_t>(Bytes.Num() - 1);
        }
    };

    // Reads through UE's platform file chain, including staged Pak/UFS files.
    class FUnrealFileSystem final : public das::AnyFileSystem
    {
    public:
        FSourceSet* Sources = nullptr;
        explicit FUnrealFileSystem(FSourceSet* InSources = nullptr) : Sources(InSources?InSources:AuthoredReferenceSources) {}
        virtual das::FileInfo* tryOpenFile(const das::string& Name) override
        {
            const FString Path(UTF8_TO_TCHAR(Name.c_str()));
            auto Snapshot = FSourceSnapshot::Read(Path);
            if (Sources) Sources->Add(Path, Snapshot);
            if (!Snapshot.bExists) return nullptr;
            auto File = MakeUnique<FUnrealTextFile>();
            File->Bytes = MoveTemp(Snapshot.Bytes);
            File->Bytes.Add(0);
            File->name = Name;
            return File.Release();
        }
    };

    // Resolve flat project modules before plugin modules; daslib keeps its own resolver.
    class FUnrealFileAccess final : public das::FsFileAccess {
    public:
        virtual das::ModuleInfo getModuleInfo(const das::string& Request,const das::string& From)const override {
            const FString Name=UTF8_TO_TCHAR(Request.c_str());
            bool Identifier=!Name.IsEmpty();for(int32 I=0;I<Name.Len();++I)Identifier &= (I==0?!FChar::IsDigit(Name[I]):true) && (FChar::IsAlnum(Name[I]) || Name[I]==TCHAR('_'));
            if(Identifier){
                const auto* Runtime=FModuleManager::GetModulePtr<FDasRuntimeModule>(TEXT("DasRuntime"));
                const FString Path=Runtime?Runtime->ScriptPath(Name+TEXT(".das")):FString();
                if(!Path.IsEmpty() && IFileManager::Get().FileExists(*Path)){
                    das::ModuleInfo Info;Info.moduleName=Request;Info.fileName=TCHAR_TO_UTF8(*Path);return Info;
                }
            }
            return das::FsFileAccess::getModuleInfo(Request,From);
        }
    };

    struct FSubscriptionDeclaration
    {
        TWeakObjectPtr<UObject> Source;
        FName Event;
        FString Handler;
        EDasMethodSignature Kind=EDasMethodSignature::Int;
        FString PayloadPath;
    };

    struct FObjectHandleEntry
    {
        TWeakObjectPtr<UObject> Object;
        TWeakObjectPtr<UClass> ExpectedClass;
    };
    uint64 NextObjectHandle = 1;

    enum class EMarshallingKind { Int, Float, Double, Bool, String, Struct, Object, Name, Enum, Array, Map, Set, Delegate, Class, Byte, Text };
    // Flattened scalar signature IR: names/types/directions, independent of property addresses.
    struct FMarshalledParameter { FString Path; EMarshallingKind Kind; bool bInput; bool bOutput; FString TypePath; };
    struct FHashValue
    {
        FProperty* Property;
        void* Memory;
        explicit FHashValue(FProperty* InProperty) : Property(InProperty)
        { Memory = FMemory::Malloc(Property->GetSize(), Property->GetMinAlignment()); Property->InitializeValue(Memory); }
        ~FHashValue() { Property->DestroyValue(Memory); FMemory::Free(Memory); }
        FHashValue(const FHashValue&) = delete;
        FHashValue& operator=(const FHashValue&) = delete;
    };
    struct FPendingHash
    {
        TArray<TUniquePtr<FHashValue>> Keys;
        TArray<TUniquePtr<FHashValue>> Values;
    };
    struct FMarshalledCall
    {
        TWeakObjectPtr<UObject> Target;
        TStrongObjectPtr<UFunction> Function;
        TUniquePtr<FStructOnScope> Frame;
        TMap<FString,FInstancedStruct> ConstraintRecords;
        TSet<FString> Assigned;
        TSet<FString> WholeInputs;
        TMap<FString, TStrongObjectPtr<UObject>> ObjectPins;
        TArray<FMarshalledParameter> Signature;
        FString SignatureKey;
        TWeakObjectPtr<UClass> RequiredInterface;
        TWeakObjectPtr<UClass> FunctionScopeClass;
        FString InterfaceSignatureKey;
        TMap<FString, TSharedPtr<FPendingHash>> PendingHashes;
        TMap<FString, TArray<int32>> HashIndices;
        TFunction<void(void*)> DelegateInvoke;
        FString AutomaticWorldContext;
        TWeakObjectPtr<UWorld> StaticWorld;
        FString CallbackFramePath;
        bool bConstructionUnsafe = false;
        bool bConstructionPolicyKnown = false;
        bool bAuthored = false;
        bool bExecuted = false;
        bool bFailed = false;
    };
    struct FDelegateValue {
        FString Wire;TStrongObjectPtr<UFunction> Signature;bool Multicast=false;
        FScriptDelegate Single;TArray<FScriptDelegate> Listeners;
        TWeakObjectPtr<UObject> Source;FName Property;
    };
    uint64 NextCallHandle = 1;

    uint64 NextTraceScope=1;
    class FUnrealDasContext final : public das::Context, public FGCObject
    {
    public:
        explicit FUnrealDasContext(uint32 StackSize) : das::Context(StackSize) {}
        virtual void AddReferencedObjects(FReferenceCollector& Collector) override {
            for(auto& Value:StructValues)Value.Value.AddStructReferencedObjects(Collector);
            for(auto& Edit:StructEdits)Edit.Value->Value.AddStructReferencedObjects(Collector);
        }
        virtual FString GetReferencerName() const override {return TEXT("FUnrealDasContext snapshots");}
        uint64 TraceScope=NextTraceScope++;
        FDasWidgetLifetime WidgetLifetime;
        TArray<FDasTypeDeclaration> Types;
        DasSnapshotContainers::FKeyOrderCache SnapshotKeyOrders;
        DasSnapshotFields::FPathCache SnapshotFieldPaths;
        struct FStructEdit { FInstancedStruct Value; bool Poisoned=false; };
        TMap<uint64,TUniquePtr<FStructEdit>> StructEdits;
        TMap<uint64,FInstancedStruct> StructValues;uint64 StructCollections=0,StructReclaimed=0;int32 StructNextCollection=256;
        TMap<uint64,FText> TextValues;
        struct FEnumValue {TWeakObjectPtr<UEnum> Type;int32 Value;};TMap<uint64,FEnumValue> EnumValues;
        TMap<uint64,FName> DSLNames;TMap<FName,uint64> DSLNameTokens;
        TMap<FString, int32> PersistentInts;
        int32 ExecutionDepth = 0;
        bool bEnding = false;
        bool bCollectingSubscriptions = false;
        bool bInitializingObject = false;
        bool bLibraryScope=false;TWeakObjectPtr<UObject> LibraryWorld;
        TArray<FSubscriptionDeclaration> Subscriptions;
        TWeakObjectPtr<UObject> Self;
        struct FObjectQuery {TWeakObjectPtr<UClass> Base;TArray<TWeakObjectPtr<UObject>> Objects;};
        TMap<uint64,FObjectQuery> ObjectQueries;
        TMap<uint64, FObjectHandleEntry> ObjectHandles;
        TMap<FObjectKey,uint64> ObjectKeyTokens;TMap<uint64,TWeakObjectPtr<UObject>> ObjectKeys;
        TMap<uint64, TSharedPtr<FMarshalledCall>> Calls;
        TMap<uint64,FDelegateValue> DelegateValues;
        TMap<FString,TStrongObjectPtr<UFunction>> DelegateSignatures;
    };

    struct FStructFrameRoots final : FGCObject {
        TArray<FDasScalarValue>& Values;
        explicit FStructFrameRoots(TArray<FDasScalarValue>& InValues):Values(InValues){}
        virtual void AddReferencedObjects(FReferenceCollector& Collector) override {
            for(auto& Value:Values)if(Value.Kind==12)Value.StructValue.AddStructReferencedObjects(Collector);
        }
        virtual FString GetReferencerName() const override {return TEXT("daScript native input frame snapshots");}
    };
    thread_local FUnrealDasContext* ActiveContext = nullptr;
    thread_local das::Context* RestrictedPhysicsContext=nullptr;
    void GuardUEExtern(){
        if(RestrictedPhysicsContext)RestrictedPhysicsContext->throw_error("UE extern is forbidden in isolated physics VM");
        if(GetDasExecutionRole()==EDasExecutionRole::PhysicsPreSim || IsDasFrozenPhysicsWorld(ActiveContext?(ActiveContext->bLibraryScope?ActiveContext->LibraryWorld.Get():ActiveContext->Self.Get()):nullptr)){
            if(ActiveContext)ActiveContext->throw_error("UE extern is forbidden in PhysicsPreSim execution role");
            throw std::runtime_error("UE extern is forbidden in PhysicsPreSim execution role");
        }
    }
    thread_local const FString* LibraryInvocationContract=nullptr;
    thread_local FString LibraryInvocationScript;
    struct FOutputFrame:FGCObject {const TArray<FDasScalarValue>* Arguments=nullptr;TMap<int32,FDasScalarValue> Values;TArray<TStrongObjectPtr<UObject>> ObjectPins;
        virtual void AddReferencedObjects(FReferenceCollector& Collector)override{for(auto& V:Values)if(V.Value.Kind==12)V.Value.StructValue.AddStructReferencedObjects(Collector);}
        virtual FString GetReferencerName()const override{return TEXT("daScript pending frame outputs");}
    };
    thread_local FOutputFrame* ActiveOutputFrame=nullptr;


    thread_local AActor* ActorLifecycleTarget=nullptr;
    thread_local int32 ActorSpawnDepth=0;
    thread_local UDasScriptComponent* ComponentLifecycleTarget=nullptr;
    thread_local int32 ComponentLifecycleDepth=0;
    thread_local int32 AuthoredDispatchDepth=0;
    thread_local FString* AuthoredDispatchFailure=nullptr;
    struct FActiveContextScope
    {
        FUnrealDasContext* Previous;
        TSet<uint64> ExistingCalls;
        explicit FActiveContextScope(FUnrealDasContext& Context) : Previous(ActiveContext)
        {
            for(const auto& Call:Context.Calls)ExistingCalls.Add(Call.Key);
            for(const auto& Query:Context.ObjectQueries)ExistingQueries.Add(Query.Key);
            ActiveContext = &Context;
        }
        TSet<uint64> ExistingQueries;
        ~FActiveContextScope() { for(auto It=ActiveContext->ObjectQueries.CreateIterator();It;++It)if(!ExistingQueries.Contains(It.Key()))It.RemoveCurrent(); for(auto It=ActiveContext->Calls.CreateIterator();It;++It)if(!ExistingCalls.Contains(It.Key()))It.RemoveCurrent(); ActiveContext = Previous; }
    };

    int32 GetStateInt(const char* Name, int32 Default, das::Context* Context)
    {
        if (Context != ActiveContext || !Name || !*Name)
            Context->throw_error("state access requires an active context and a nonempty key");
        return ActiveContext->PersistentInts.FindOrAdd(UTF8_TO_TCHAR(Name), Default);
    }
    void SetStateInt(const char* Name, int32 Value, das::Context* Context)
    {
        if (Context != ActiveContext || !Name || !*Name)
            Context->throw_error("state access requires an active context and a nonempty key");
        ActiveContext->PersistentInts.Add(UTF8_TO_TCHAR(Name), Value);
    }

    int32 NativeAdd(int32 A, int32 B) { return A + B; }

    int32 EmitType(const char* Name, const char* Parent, const char* Fields, das::Context* Context)
    {
        // Copies strings while their owning context is alive. No UE objects or
        // compiled function pointers escape this execution in the foundation.
        if (Context != ActiveContext)
            Context->throw_error("UE schema emission is only allowed in the active runtime context");
        if (!Name || !Parent || !Fields)
            Context->throw_error("UE schema declaration strings must not be null");
        auto& UEContext = *ActiveContext;
        UEContext.Types.Add({ UTF8_TO_TCHAR(Name), UTF8_TO_TCHAR(Parent), UTF8_TO_TCHAR(Fields) });
        return UEContext.Types.Num();
    }

    UObject* ResolveSelf(das::Context* Context)
    {
        if (!IsInGameThread() || Context != ActiveContext || !ActiveContext)
        { Context->throw_error("self access requires the active game-thread runtime context"); }
        UObject* Self = ActiveContext->Self.Get();
        if(LibraryInvocationContract || ActiveContext->bLibraryScope || (Self && Self->IsA<UDasBlueprintLibrary>()))Context->throw_error("Static library has no implicit self or WorldContext");
        if (!IsValid(Self) || (Self->IsA<AActor>() && CastChecked<AActor>(Self)->IsActorBeingDestroyed() && !ActiveContext->bEnding))
        { Context->throw_error("self is missing or expired"); }
        return Self;
    }

    UObject* ResolveWorldContextObject(das::Context* C){
        if(C!=ActiveContext || !IsInGameThread())C->throw_error("WorldContext requires active game-thread context");
        if(!ActiveContext->bLibraryScope){if(LibraryInvocationContract)C->throw_error("Library WorldContext is only available inside a method");return ResolveSelf(C);}
        auto* Object=ActiveContext->LibraryWorld.Get();auto* World=IsValid(Object) && GEngine?GEngine->GetWorldFromContextObject(Object,EGetWorldErrorMode::ReturnNull):nullptr;
        if(!World || !World->IsGameWorld() || World->bIsTearingDown || Object->IsTemplate() || (Cast<AActor>(Object) && CastChecked<AActor>(Object)->IsActorBeingDestroyed()) || (Cast<UActorComponent>(Object) && CastChecked<UActorComponent>(Object)->IsBeingDestroyed()))C->throw_error("Library requires a live game-world WorldContext");return Object;
    }

    template<class P> P* ResolveProperty(UObject* Self, const char* Name, bool bWrite, das::Context* Context)
    {
        if (!Name || !*Name) { Context->throw_error("property name is empty"); }
        // Resolve again for every call; never retain FProperty pointers across reinstancing.
        const FString FieldName(UTF8_TO_TCHAR(Name));
        if (FieldName.Len() >= NAME_SIZE) { Context->throw_error("property name is too long"); }
        FProperty* Property = Self->GetClass()->FindPropertyByName(FName(*FieldName));
        if (!Property) { Context->throw_error("self property does not exist"); }
        P* Typed = CastField<P>(Property);
        if (!Typed || Property->ArrayDim != 1)
        { Context->throw_error("self property has a different scalar type"); }
        if (!Property->HasAnyPropertyFlags(CPF_BlueprintVisible) ||
            (bWrite && Property->HasAnyPropertyFlags(CPF_BlueprintReadOnly)))
        { Context->throw_error("self property access is not permitted (hidden or read-only)"); }
        return Typed;
    }

    thread_local bool ConstructingObject=false;
    void CheckConstructionCall(bool Unsafe,das::Context* C){if(Unsafe && ConstructingObject)C->throw_error("UFunction is unsafe during ConstructionScript");}
    #include "DasNativePropertyValues.inl"

    bool GetSelfBool(const char* Name,das::Context* Context)
    {auto* Self=ResolveSelf(Context);return ReadPropertyScalar(ResolveProperty<FBoolProperty>(Self,Name,false,Context),Self,Context);}
    void SetSelfBool(const char* Name,bool Value,das::Context* Context)
    {if(ActiveContext && ActiveContext->bCollectingSubscriptions) Context->throw_error("self writes are forbidden in bind_events");UObject* Self=ResolveSelf(Context);WritePropertyScalar(ResolveProperty<FBoolProperty>(Self,Name,true,Context),Self,Value,Context);}
    FDasNameValue MakeDSLName(FName Name,das::Context* C)
    {
        if(C!=ActiveContext || !ActiveContext || !IsInGameThread()) C->throw_error("name requires active context");
        if(Name.IsNone())return {};if(auto* T=ActiveContext->DSLNameTokens.Find(Name))return {*T};
        if(ActiveContext->DSLNames.Num()>=65536)C->throw_error("name table limit exceeded");
        static uint64 NextToken=1;const uint64 Token=NextToken++;ActiveContext->DSLNames.Add(Token,Name);ActiveContext->DSLNameTokens.Add(Name,Token);return {Token};
    }
    FName ResolveDSLName(FDasNameValue V,das::Context* C)
    {if(C!=ActiveContext || !ActiveContext)C->throw_error("name requires active context");if(!V.Token)return NAME_None;auto* N=ActiveContext->DSLNames.Find(V.Token);if(!N)C->throw_error("name belongs to another context");return *N;}
    FDasNameValue DSLNameFromString(const char* S,das::Context* C)
    {const FString T=S?UTF8_TO_TCHAR(S):TEXT("");if(T.Len()>=NAME_SIZE)C->throw_error("FName value is too long");return MakeDSLName(FName(*T),C);}
    char* CopyDSLString(const FString& S,das::Context* C){for(int32 I=0;I<S.Len();++I)if(S[I]==0)C->throw_error("embedded NUL is not supported in strings");const FTCHARToUTF8 U(*S);return C->allocateString(U.Get(),U.Length(),nullptr);}
    char* DSLNameToString(FDasNameValue V,das::Context* C){return CopyDSLString(ResolveDSLName(V,C).ToString(),C);}
    bool DSLNamesEqual(FDasNameValue A,FDasNameValue B,das::Context* C){return ResolveDSLName(A,C)==ResolveDSLName(B,C);}
    double GetSelfDouble(const char* N,das::Context* C){auto* S=ResolveSelf(C);return ReadPropertyScalar(ResolveProperty<FDoubleProperty>(S,N,false,C),S,C);}
    void SetSelfDouble(const char* N,double V,das::Context* C){auto* S=ResolveSelf(C);if(ActiveContext->bCollectingSubscriptions)C->throw_error("self writes are forbidden in bind_events");WritePropertyScalar(ResolveProperty<FDoubleProperty>(S,N,true,C),S,V,C);}
    char* GetSelfString(const char* N,das::Context* C){auto* S=ResolveSelf(C);return CopyDSLString(ReadPropertyScalar(ResolveProperty<FStrProperty>(S,N,false,C),S,C),C);}
    void SetSelfString(const char* N,const char* V,das::Context* C){auto* S=ResolveSelf(C);if(ActiveContext->bCollectingSubscriptions)C->throw_error("self writes are forbidden in bind_events");WritePropertyScalar(ResolveProperty<FStrProperty>(S,N,true,C),S,V?UTF8_TO_TCHAR(V):TEXT(""),C);}
    FDasNameValue GetSelfName(const char* N,das::Context* C){auto* S=ResolveSelf(C);return MakeDSLName(ReadPropertyScalar(ResolveProperty<FNameProperty>(S,N,false,C),S,C),C);}
    void SetSelfName(const char* N,FDasNameValue V,das::Context* C){auto* S=ResolveSelf(C);if(ActiveContext->bCollectingSubscriptions)C->throw_error("self writes are forbidden in bind_events");WritePropertyScalar(ResolveProperty<FNameProperty>(S,N,true,C),S,ResolveDSLName(V,C),C);}
    FVector DSLVector(double X,double Y,double Z,das::Context* C){if(!FMath::IsFinite(X) || !FMath::IsFinite(Y) || !FMath::IsFinite(Z))C->throw_error("vector components must be finite");return FVector(X,Y,Z);}
    FRotator DSLRotator(double Pitch,double Yaw,double Roll,das::Context* C){if(!FMath::IsFinite(Pitch) || !FMath::IsFinite(Yaw) || !FMath::IsFinite(Roll))C->throw_error("rotator components must be finite");return FRotator(Pitch,Yaw,Roll);}
    FTransform DSLTransform(const FVector& Translation,const FRotator& Rotation,const FVector& Scale,das::Context* C){if(Translation.ContainsNaN() || Rotation.ContainsNaN() || Scale.ContainsNaN())C->throw_error("transform components must be finite");return FTransform(Rotation,Translation,Scale);}
    FTransform DSLIdentityTransform(){return FTransform::Identity;}
    FVector DSLTransformPosition(const FTransform& T,const FVector& V,das::Context* C){if(T.ContainsNaN() || !T.IsRotationNormalized() || V.ContainsNaN())C->throw_error("invalid transform or position");const FVector Result=T.TransformPosition(V);if(Result.ContainsNaN())C->throw_error("non-finite transformed position");return Result;}
    FVector DSLTranslation(const FTransform& T){return T.GetTranslation();}
    FVector DSLScale(const FTransform& T){return T.GetScale3D();}
    FRotator DSLRotation(const FTransform& T){return T.Rotator();}
    UEnum* RequireCallEnum(FProperty*,const char*,das::Context*);
    FNumericProperty* CallEnumStorage(FProperty*);
    bool IsCallEnumMember(UEnum*,int32);
    struct FStructRootWalker:das::DataWalker {
        TSet<uint64> Tokens,DelegateTokens;bool Complete=true;
        virtual void beforeStructureField(char*,das::StructInfo*,char* Data,das::VarInfo* Info,bool)override{if(Info && Info->name && das::string(Info->name)=="__delegate_token" && Info->type==das::Type::tUInt64)DelegateTokens.Add(*reinterpret_cast<uint64*>(Data));}
        virtual bool canVisitHandle(char* Data,das::TypeInfo* Info) override {auto* A=Info->getAnnotation();if(A && (A->name=="UEStructValue" || A->name=="UEHitResult" || A->name=="UEConstraintHandle" || A->name=="UEInputEvent" || A->name=="UEKeyEvent" || A->name=="UEAnalogInputEvent" || A->name=="UECharacterEvent" || A->name=="UEPointerEvent" || A->name=="UENavigationEvent"))Tokens.Add(*reinterpret_cast<const uint64*>(Data));return false;}
    };
    bool CollectStructValues(FUnrealDasContext& C,const das::LineInfo* At,bool Boundary=false)
    {
        if(C.StructValues.IsEmpty() && C.DelegateValues.IsEmpty())return true;
        // Active compiled/position-less frames cannot prove reachability.
        if(!Boundary && !C.gcEnabled)return false;
        FStructRootWalker W;W.context=&C;
        C.foreach_gc_root([&](void* P,das::TypeInfo* T){if(T)W.walk(static_cast<char*>(P),T);else {W.Complete=false;}});
        for(int I=0;I<C.getTotalVariables();++I){const auto V=C.getGlobalVariable(I);if(!V.debugInfo){W.Complete=false;break;}char* Base=V.shared?C.shared:C.globals;if(Base)W.walk(Base+V.offset,V.debugInfo);}
        if(!Boundary){char* SP=C.stack.ap();const das::LineInfo* Handoff=At;while(SP<C.stack.top()){
            auto* P=reinterpret_cast<das::Prologue*>(SP);das::FuncInfo* Info=nullptr;char* Locals=SP;
            if(P->info){intptr_t B=reinterpret_cast<intptr_t>(P->block);if(B&1){auto* Block=reinterpret_cast<das::Block*>(B&~intptr_t(1));Info=Block->info;Locals=C.stack.bottom()+Block->stackOffset;}else Info=P->info;}
            if(Info){const uint32 Pos=das::lineFramePos(Handoff,Info->spaceId);if(Info->localCount && !Pos)return false;
                for(uint32 I=0;I<Info->count;++I)W.walk(P->arguments[I],Info->fields[I]);
                for(uint32 I=0;I<Info->localCount;++I){auto* L=Info->locals[I];if(das::isLiveAt(L,Pos)){char* Addr=L->cmres?static_cast<char*>(P->cmres):Locals+L->stackTop;if(Addr)W.walk(Addr,L);}}
            }
            const uint32 Step=Info?Info->stackSize:P->stackSize;if(!Step)return false;Handoff=Info?P->line:nullptr;SP+=Step;
        }}
        if(!W.Complete)return false;
        const int32 Before=C.StructValues.Num();for(auto It=C.StructValues.CreateIterator();It;++It)if(!W.Tokens.Contains(It.Key()))It.RemoveCurrent();
        C.SnapshotKeyOrders.Prune([&](uint64 Token){return C.StructValues.Contains(Token);});
        for(auto It=C.DelegateValues.CreateIterator();It;++It)if(!W.DelegateTokens.Contains(It.Key()))It.RemoveCurrent();
        ++C.StructCollections;C.StructReclaimed+=Before-C.StructValues.Num();C.StructNextCollection=FMath::Min(4096,FMath::Max(256,C.StructValues.Num()+256));return true;
    }
    void ExplicitStructCollect(das::Context* C,das::LineInfoArg* At){if(C!=ActiveContext || !ActiveContext || !CollectStructValues(*ActiveContext,At))C->throw_error("struct collection requires options gc and attributable interpreter frames");}
    int32 StructValueCount(das::Context* C){if(C!=ActiveContext || !ActiveContext)C->throw_error("struct stats require active context");return ActiveContext->StructValues.Num();}
    struct FStructCollectOnExit {FUnrealDasContext& C;~FStructCollectOnExit(){if(C.ExecutionDepth>1)return;try{CollectStructValues(C,nullptr,true);}catch(...){/* Failed root traversal must not turn a result into an exception. */}}};
    // The context reports every stored snapshot through FInstancedStruct's GC collector.
    // This roots its reflected objects and lets UE clear destroyed references in native memory.
    bool MathSnapshotValid(const FInstancedStruct& Snapshot){const auto* Type=Snapshot.GetScriptStruct();if(Type==TBaseStructure<FVector2D>::Get())return !Snapshot.Get<FVector2D>().ContainsNaN();if(Type==TBaseStructure<FVector>::Get())return !Snapshot.Get<FVector>().ContainsNaN();if(Type==TBaseStructure<FRotator>::Get())return !Snapshot.Get<FRotator>().ContainsNaN();if(Type==TBaseStructure<FTransform>::Get())return !Snapshot.Get<FTransform>().ContainsNaN() && Snapshot.Get<FTransform>().IsRotationNormalized();return true;}
    // Borrowed snapshots copy only on insertion. Owned temporaries transfer their
    // buffer after validation/deduplication; all routes share one token counter.
    uint64 NextStructValueToken=1;
    FDasStructValue StoreInputEvent(const UScriptStruct*,const void*,das::Context*,das::LineInfoArg*);
    FDasStructValue StoreStructValueImpl(const FInstancedStruct& V,FInstancedStruct* Owned,das::Context* C,das::LineInfoArg* At){if(C!=ActiveContext || !ActiveContext)C->throw_error("struct requires active context");if(DasIsInputEventType(V.GetScriptStruct()))return StoreInputEvent(V.GetScriptStruct(),V.GetMemory(),C,At);FString Error;if(!ValidateDasStructSnapshot(V,Error))C->throw_error(TCHAR_TO_UTF8(*Error));if(!MathSnapshotValid(V))C->throw_error("invalid math snapshot value");for(const auto& P:ActiveContext->StructValues)if(IdenticalDasStructSnapshot(P.Value,V))return {P.Key};if(At && ActiveContext->StructValues.Num()>=ActiveContext->StructNextCollection)CollectStructValues(*ActiveContext,At);if(ActiveContext->StructValues.Num()>=4096)C->throw_error("struct value table limit exceeded");uint64 Token=NextStructValueToken++;if(Owned)ActiveContext->StructValues.Add(Token,MoveTemp(*Owned));else ActiveContext->StructValues.Add(Token,V);return {Token};}
    FDasStructValue StoreStructValue(const FInstancedStruct& V,das::Context* C,das::LineInfoArg* At=nullptr){return StoreStructValueImpl(V,nullptr,C,At);}
    FDasStructValue StoreStructValue(FInstancedStruct&& V,das::Context* C,das::LineInfoArg* At=nullptr){return StoreStructValueImpl(V,&V,C,At);}
    const FInstancedStruct& ResolveStructValue(FDasStructValue V,das::Context* C){if(C!=ActiveContext || !ActiveContext)C->throw_error("struct requires active context");if(const auto* Edit=ActiveContext->StructEdits.Find(V.Token)){if((*Edit)->Poisoned)C->throw_error("snapshot edit was invalidated by a failed write");return (*Edit)->Value;}const auto* S=ActiveContext->StructValues.Find(V.Token);if(!S)C->throw_error("struct is null or belongs to another context");return *S;}
    #include "DasSnapshotEdits.inl"
    FDasStructValue NewStructDefault(const char* Path,const char* Text,das::Context* C,das::LineInfoArg* At){FInstancedStruct Value;FString Error;if(!ParseDasStructDefault(Path?UTF8_TO_TCHAR(Path):TEXT(""),Text?UTF8_TO_TCHAR(Text):TEXT(""),Value,Error))C->throw_error(TCHAR_TO_UTF8(*Error));return StoreStructValue(MoveTemp(Value),C,At);}
    FDasStructValue ReadPropertySnapshot(FProperty* P,const void* Memory,das::Context* C,das::LineInfoArg* At){
        auto V=ReadNativePropertyValue(P,Memory,C);
        if(V.Kind!=int32(EDasMethodSignature::Struct))C->throw_error("Expected struct/container property");
        return StoreStructValue(MoveTemp(V.StructValue),C,At);
    }
    void WritePropertySnapshot(FProperty* P,void* Memory,const FDasStructValue& Handle,das::Context* C){
        const auto& Snapshot=ResolveStructValue(Handle,C);
        if(!MathSnapshotValid(Snapshot))C->throw_error("Invalid math property value");
        FString Error;if(!WriteDasPropertySnapshot(P,Memory,Snapshot,Error))C->throw_error(TCHAR_TO_UTF8(*Error));
    }
    FDasStructValue GetSelfStruct(const char* N,das::Context* C,das::LineInfoArg* At){auto* Self=ResolveSelf(C);auto* P=ResolveProperty<FProperty>(Self,N,false,C);return ReadPropertySnapshot(P,P->ContainerPtrToValuePtr<void>(Self),C,At);}
    void SetSelfStruct(const char* N,const FDasStructValue& V,das::Context* C){auto* Self=ResolveSelf(C);if(ActiveContext->bCollectingSubscriptions)C->throw_error("self writes are forbidden in bind_events");auto* P=ResolveProperty<FProperty>(Self,N,true,C);WritePropertySnapshot(P,P->ContainerPtrToValuePtr<void>(Self),V,C);}

    FDasStructValue NewStructValue(const char* Path,das::Context* C,das::LineInfoArg* At){FString P=Path?UTF8_TO_TCHAR(Path):TEXT("");FString E;if(!ValidateDasValueStruct(P,E))C->throw_error(TCHAR_TO_UTF8(*E));FInstancedStruct S;S.InitializeAs(FindObject<UScriptStruct>(nullptr,*P));return StoreStructValue(MoveTemp(S),C,At);}
    template<class Byte> using TStructLeaf=DasSnapshotFields::TField<Byte>;
    using FStructLeaf=TStructLeaf<uint8>;
    using FConstStructLeaf=TStructLeaf<const uint8>;
    template<class Byte> TStructLeaf<Byte> StructLeafMemory(const UScriptStruct* Type,Byte* Memory,const char* Path,das::Context* C){
        TStructLeaf<Byte> Field{};FString Error;
        if(C!=ActiveContext || !ActiveContext)C->throw_error("struct field access requires active context");
        if(!ActiveContext->SnapshotFieldPaths.Find(Type,Memory,FString(Path?UTF8_TO_TCHAR(Path):TEXT("")),Field,Error))C->throw_error(TCHAR_TO_UTF8(*Error));
        return Field;
    }
    FStructLeaf StructLeaf(FInstancedStruct& V,const char* Path,das::Context* C){return StructLeafMemory(V.GetScriptStruct(),V.GetMutableMemory(),Path,C);}
    FConstStructLeaf StructLeaf(const FInstancedStruct& V,const char* Path,das::Context* C){return StructLeafMemory(V.GetScriptStruct(),V.GetMemory(),Path,C);}
    using ESnapshotKind=EDasTypeKind;
    template<class Byte> auto SnapshotMap(TStructLeaf<Byte> F,ESnapshotKind Key,ESnapshotKind Value,das::Context* C){
        DasSnapshotContainers::TMapView<Byte> View;FString Error;
        if(!DasSnapshotContainers::Map(F,Key,Value,View,Error))C->throw_error(TCHAR_TO_UTF8(*Error));return View;
    }
    template<class Byte> auto SnapshotSet(TStructLeaf<Byte> F,ESnapshotKind Element,das::Context* C){
        DasSnapshotContainers::TSetView<Byte> View;FString Error;
        if(!DasSnapshotContainers::Set(F,Element,View,Error))C->throw_error(TCHAR_TO_UTF8(*Error));return View;
    }
    template<class Byte> auto SnapshotArray(TStructLeaf<Byte> F,das::Context* C){
        DasSnapshotContainers::TArrayView<Byte> View;FString Error;
        if(!DasSnapshotContainers::Array(F,View,Error))C->throw_error(TCHAR_TO_UTF8(*Error));return View;
    }
    template<class Property> constexpr ESnapshotKind SnapshotValueKind(){
        if constexpr(std::is_same_v<Property,FIntProperty>)return ESnapshotKind::Int;
        else if constexpr(std::is_same_v<Property,FByteProperty>)return ESnapshotKind::Byte;
        else if constexpr(std::is_same_v<Property,FTextProperty>)return ESnapshotKind::Text;
        else if constexpr(std::is_same_v<Property,FFloatProperty>)return ESnapshotKind::Float;
        else if constexpr(std::is_same_v<Property,FDoubleProperty>)return ESnapshotKind::Double;
        else if constexpr(std::is_same_v<Property,FBoolProperty>)return ESnapshotKind::Bool;
        else if constexpr(std::is_same_v<Property,FStrProperty>)return ESnapshotKind::String;
        else if constexpr(std::is_same_v<Property,FNameProperty>)return ESnapshotKind::Name;
        else {static_assert(std::is_same_v<Property,FStructProperty>);return ESnapshotKind::Struct;}
    }
    template<class KeyType,class Read> auto ReadSnapshotMap(const FDasStructValue& H,const char* Path,const KeyType& Key,ESnapshotKind KeyKind,ESnapshotKind ValueKind,das::Context* C,Read Reader){
        const auto& V=ResolveStructValue(H,C);auto View=SnapshotMap(StructLeaf(V,Path,C),KeyKind,ValueKind,C);
        FConstStructLeaf Value{};FString Error;if(!View.Find(&Key,Value,Error))C->throw_error(TCHAR_TO_UTF8(*Error));return Reader(Value.Property,Value.Memory);
    }
    template<class KeyType,class Write> FDasStructValue WriteSnapshotMap(const FDasStructValue& H,const char* Path,const KeyType& Key,ESnapshotKind KeyKind,ESnapshotKind ValueKind,das::Context* C,das::LineInfoArg* At,Write Writer){
        FStructMutation Mutation(H,C);auto& V=Mutation.Value;auto View=SnapshotMap(StructLeaf(V,Path,C),KeyKind,ValueKind,C);FString Error;
        if(!View.Upsert(&Key,[&](FProperty* P,void* Memory,FString&){Writer(P,Memory);return true;},Error))C->throw_error(TCHAR_TO_UTF8(*Error));
        return Mutation.Finish(C,At);
    }
    template<class ValueProperty,class KeyType> auto StructScalarMapGet(const FDasStructValue& H,const char* Path,const KeyType& Key,ESnapshotKind KeyKind,das::Context* C){
        return ReadSnapshotMap(H,Path,Key,KeyKind,SnapshotValueKind<ValueProperty>(),C,[&](FProperty* P,const void* Memory){return ReadPropertyScalarValue(CastFieldChecked<ValueProperty>(P),Memory,C);});
    }
    template<class ValueProperty,class KeyType,class ValueType> FDasStructValue StructScalarMapWith(const FDasStructValue& H,const char* Path,const KeyType& Key,const ValueType& Value,ESnapshotKind KeyKind,das::Context* C,das::LineInfoArg* At){
        return WriteSnapshotMap(H,Path,Key,KeyKind,SnapshotValueKind<ValueProperty>(),C,At,[&](FProperty* P,void* Memory){WriteNativePropertyValue(P,Memory,PropertyScalarValue<ValueProperty>(Value),C);});
    }
    template<class View> FConstStructLeaf SnapshotKeyAt(const FDasStructValue& H,const char* Path,const View& Values,int32 Index,das::Context* C){
        if(Index<0 || Index>=Values.Num())C->throw_error(View::IsMap?"map key index out of range":"set element index out of range");
        // Draft tokens retain identity while their contents change. Immutable key-order caches cannot apply.
        TArray<int32> DraftOrder;
        if(ActiveContext->StructEdits.Contains(H.Token))Values.PrepareKeyOrder(DraftOrder);
        const auto& Order=ActiveContext->StructEdits.Contains(H.Token)?DraftOrder:ActiveContext->SnapshotKeyOrders.Get(H.Token,Path?UTF8_TO_TCHAR(Path):TEXT(""),Values);
        FConstStructLeaf Field{};FString Error;if(!Values.KeyAtSlot(Order[Index],Field,Error))C->throw_error(TCHAR_TO_UTF8(*Error));return Field;
    }
    FDasStructValue SetSnapshotValue(const FDasStructValue& H,const char* Path,const void* Key,ESnapshotKind Kind,bool Present,das::Context* C,das::LineInfoArg* At){
        FStructMutation Mutation(H,C);auto& V=Mutation.Value;auto View=SnapshotSet(StructLeaf(V,Path,C),Kind,C);FString Error;
        if(!View.Set(Key,Present,Error))C->throw_error(TCHAR_TO_UTF8(*Error));return Mutation.Finish(C,At);
    }
    FDasScalarValue SnapshotObjectValue(FProperty* P,UObject* Object,das::Context* C){
        auto* Type=DasReferencePropertyClass(P);if(!Type)C->throw_error("expected snapshot object/interface");
        FDasScalarValue Value;Value.Kind=int32(EDasMethodSignature::Object);Value.ObjectClass=Type->GetPathName();Value.ObjectValue=Object;return Value;
    }



    int32 StructSetSize(const FDasStructValue& H,const char* Path,das::Context* C){return SnapshotSet(StructLeaf(ResolveStructValue(H,C),Path,C),ESnapshotKind::Invalid,C).Num();}
    int32 StructSetElementInt(const FDasStructValue& H,const char* Path,int32 Index,das::Context* C){auto F=SnapshotKeyAt(H,Path,SnapshotSet(StructLeaf(ResolveStructValue(H,C),Path,C),ESnapshotKind::Int,C),Index,C);return ReadPropertyScalarValue(CastFieldChecked<FIntProperty>(F.Property),F.Memory,C);}
    char* StructSetElementString(const FDasStructValue& H,const char* Path,int32 Index,das::Context* C){auto F=SnapshotKeyAt(H,Path,SnapshotSet(StructLeaf(ResolveStructValue(H,C),Path,C),ESnapshotKind::String,C),Index,C);return CopyDSLString(ReadPropertyScalarValue(CastFieldChecked<FStrProperty>(F.Property),F.Memory,C),C);}
    bool StructSetContainsInt(const FDasStructValue& H,const char* Path,int32 Element,das::Context* C){return SnapshotSet(StructLeaf(ResolveStructValue(H,C),Path,C),ESnapshotKind::Int,C).Contains(&Element);}
    bool StructSetContainsString(const FDasStructValue& H,const char* Path,const char* Element,das::Context* C){FString Key=Element?UTF8_TO_TCHAR(Element):TEXT("");return SnapshotSet(StructLeaf(ResolveStructValue(H,C),Path,C),ESnapshotKind::String,C).Contains(&Key);}
    FDasStructValue StructSetWithInt(const FDasStructValue& H,const char* Path,int32 Element,bool Present,das::Context* C,das::LineInfoArg* At){return SetSnapshotValue(H,Path,&Element,ESnapshotKind::Int,Present,C,At);}
    FDasStructValue StructSetWithString(const FDasStructValue& H,const char* Path,const char* Element,bool Present,das::Context* C,das::LineInfoArg* At){FString Key=Element?UTF8_TO_TCHAR(Element):TEXT("");return SetSnapshotValue(H,Path,&Key,ESnapshotKind::String,Present,C,At);}
    int32 StructMapSize(const FDasStructValue& H,const char* Path,das::Context* C){return SnapshotMap(StructLeaf(ResolveStructValue(H,C),Path,C),ESnapshotKind::Invalid,ESnapshotKind::Invalid,C).Num();}
    int32 StructMapKey(const FDasStructValue& H,const char* Path,int32 Index,das::Context* C){auto F=SnapshotKeyAt(H,Path,SnapshotMap(StructLeaf(ResolveStructValue(H,C),Path,C),ESnapshotKind::Int,ESnapshotKind::Invalid,C),Index,C);return ReadPropertyScalarValue(CastFieldChecked<FIntProperty>(F.Property),F.Memory,C);}
    int32 StructMapGetInt(const FDasStructValue& H,const char* Path,int32 Key,das::Context* C){return StructScalarMapGet<FIntProperty>(H,Path,Key,ESnapshotKind::Int,C);}
    FDasStructValue StructMapWithInt(const FDasStructValue& H,const char* Path,int32 Key,int32 Value,das::Context* C,das::LineInfoArg* At){return StructScalarMapWith<FIntProperty>(H,Path,Key,Value,ESnapshotKind::Int,C,At);}

    char* StructMapStringKey(const FDasStructValue& H,const char* Path,int32 Index,das::Context* C){auto F=SnapshotKeyAt(H,Path,SnapshotMap(StructLeaf(ResolveStructValue(H,C),Path,C),ESnapshotKind::String,ESnapshotKind::Invalid,C),Index,C);return CopyDSLString(ReadPropertyScalarValue(CastFieldChecked<FStrProperty>(F.Property),F.Memory,C),C);}
    char* StructMapGetIntString(const FDasStructValue& H,const char* Path,int32 Key,das::Context* C){return CopyDSLString(StructScalarMapGet<FStrProperty>(H,Path,Key,ESnapshotKind::Int,C),C);}
    FDasStructValue StructMapWithIntString(const FDasStructValue& H,const char* Path,int32 Key,const char* Value,das::Context* C,das::LineInfoArg* At){return StructScalarMapWith<FStrProperty>(H,Path,Key,FString(Value?UTF8_TO_TCHAR(Value):TEXT("")),ESnapshotKind::Int,C,At);}
    int32 StructMapGetStringInt(const FDasStructValue& H,const char* Path,const char* Key,das::Context* C){return StructScalarMapGet<FIntProperty>(H,Path,FString(Key?UTF8_TO_TCHAR(Key):TEXT("")),ESnapshotKind::String,C);}
    FDasStructValue StructMapWithStringInt(const FDasStructValue& H,const char* Path,const char* Key,int32 Value,das::Context* C,das::LineInfoArg* At){return StructScalarMapWith<FIntProperty>(H,Path,FString(Key?UTF8_TO_TCHAR(Key):TEXT("")),Value,ESnapshotKind::String,C,At);}
    char* StructMapGetStringString(const FDasStructValue& H,const char* Path,const char* Key,das::Context* C){return CopyDSLString(StructScalarMapGet<FStrProperty>(H,Path,FString(Key?UTF8_TO_TCHAR(Key):TEXT("")),ESnapshotKind::String,C),C);}
    FDasStructValue StructMapWithStringString(const FDasStructValue& H,const char* Path,const char* Key,const char* Value,das::Context* C,das::LineInfoArg* At){return StructScalarMapWith<FStrProperty>(H,Path,FString(Key?UTF8_TO_TCHAR(Key):TEXT("")),FString(Value?UTF8_TO_TCHAR(Value):TEXT("")),ESnapshotKind::String,C,At);}
    int32 StructArraySize(const FDasStructValue& H,const char* Path,das::Context* C){return SnapshotArray(StructLeaf(ResolveStructValue(H,C),Path,C),C).Num();}
    FDasStructValue StructArrayResize(const FDasStructValue& H,const char* Path,int32 N,das::Context* C,das::LineInfoArg* At){FStructMutation Mutation(H,C);auto& V=Mutation.Value;auto View=SnapshotArray(StructLeaf(V,Path,C),C);FString Error;if(!View.Resize(N,Error))C->throw_error(TCHAR_TO_UTF8(*Error));return Mutation.Finish(C,At);}




    template<typename KeyType> FDasStructValue StructMapGetStruct(const FDasStructValue& H,const char* Path,const KeyType& Key,ESnapshotKind KeyKind,das::Context* C,das::LineInfoArg* At){return ReadSnapshotMap(H,Path,Key,KeyKind,ESnapshotKind::Struct,C,[&](FProperty* P,const void* Memory){return ReadPropertySnapshot(P,Memory,C,At);});}
    template<typename KeyType> FDasStructValue StructMapWithStruct(const FDasStructValue& H,const char* Path,const KeyType& Key,const FDasStructValue& Value,ESnapshotKind KeyKind,das::Context* C,das::LineInfoArg* At){return WriteSnapshotMap(H,Path,Key,KeyKind,ESnapshotKind::Struct,C,At,[&](FProperty* P,void* Memory){WritePropertySnapshot(P,Memory,Value,C);});}
    FDasStructValue StructMapGetIntStruct(const FDasStructValue& H,const char* Path,int32 Key,das::Context* C,das::LineInfoArg* At){return StructMapGetStruct(H,Path,Key,ESnapshotKind::Int,C,At);}
    FDasStructValue StructMapWithIntStruct(const FDasStructValue& H,const char* Path,int32 Key,const FDasStructValue& V,das::Context* C,das::LineInfoArg* At){return StructMapWithStruct(H,Path,Key,V,ESnapshotKind::Int,C,At);}
    FDasStructValue StructMapGetStringStruct(const FDasStructValue& H,const char* Path,const char* Key,das::Context* C,das::LineInfoArg* At){return StructMapGetStruct(H,Path,FString(Key?UTF8_TO_TCHAR(Key):TEXT("")),ESnapshotKind::String,C,At);}
    FDasStructValue StructMapWithStringStruct(const FDasStructValue& H,const char* Path,const char* Key,const FDasStructValue& V,das::Context* C,das::LineInfoArg* At){return StructMapWithStruct(H,Path,FString(Key?UTF8_TO_TCHAR(Key):TEXT("")),V,ESnapshotKind::String,C,At);}

    float StructMapGetIntFloat(const FDasStructValue& H,const char* Path,int32 Key,das::Context* C){return StructScalarMapGet<FFloatProperty>(H,Path,Key,ESnapshotKind::Int,C);}
    FDasStructValue StructMapWithIntFloat(const FDasStructValue& H,const char* Path,int32 Key,float Value,das::Context* C,das::LineInfoArg* At){return StructScalarMapWith<FFloatProperty>(H,Path,Key,Value,ESnapshotKind::Int,C,At);}
    double StructMapGetIntDouble(const FDasStructValue& H,const char* Path,int32 Key,das::Context* C){return StructScalarMapGet<FDoubleProperty>(H,Path,Key,ESnapshotKind::Int,C);}
    FDasStructValue StructMapWithIntDouble(const FDasStructValue& H,const char* Path,int32 Key,double Value,das::Context* C,das::LineInfoArg* At){return StructScalarMapWith<FDoubleProperty>(H,Path,Key,Value,ESnapshotKind::Int,C,At);}
    bool StructMapGetIntBool(const FDasStructValue& H,const char* Path,int32 Key,das::Context* C){return StructScalarMapGet<FBoolProperty>(H,Path,Key,ESnapshotKind::Int,C);}
    FDasStructValue StructMapWithIntBool(const FDasStructValue& H,const char* Path,int32 Key,bool Value,das::Context* C,das::LineInfoArg* At){return StructScalarMapWith<FBoolProperty>(H,Path,Key,Value,ESnapshotKind::Int,C,At);}
    FDasNameValue StructMapGetIntName(const FDasStructValue& H,const char* Path,int32 Key,das::Context* C){return MakeDSLName(StructScalarMapGet<FNameProperty>(H,Path,Key,ESnapshotKind::Int,C),C);}
    FDasStructValue StructMapWithIntName(const FDasStructValue& H,const char* Path,int32 Key,FDasNameValue Value,das::Context* C,das::LineInfoArg* At){return StructScalarMapWith<FNameProperty>(H,Path,Key,ResolveDSLName(Value,C),ESnapshotKind::Int,C,At);}
    float StructMapGetStringFloat(const FDasStructValue& H,const char* Path,const char* Key,das::Context* C){return StructScalarMapGet<FFloatProperty>(H,Path,FString(Key?UTF8_TO_TCHAR(Key):TEXT("")),ESnapshotKind::String,C);}
    FDasStructValue StructMapWithStringFloat(const FDasStructValue& H,const char* Path,const char* Key,float Value,das::Context* C,das::LineInfoArg* At){return StructScalarMapWith<FFloatProperty>(H,Path,FString(Key?UTF8_TO_TCHAR(Key):TEXT("")),Value,ESnapshotKind::String,C,At);}
    double StructMapGetStringDouble(const FDasStructValue& H,const char* Path,const char* Key,das::Context* C){return StructScalarMapGet<FDoubleProperty>(H,Path,FString(Key?UTF8_TO_TCHAR(Key):TEXT("")),ESnapshotKind::String,C);}
    FDasStructValue StructMapWithStringDouble(const FDasStructValue& H,const char* Path,const char* Key,double Value,das::Context* C,das::LineInfoArg* At){return StructScalarMapWith<FDoubleProperty>(H,Path,FString(Key?UTF8_TO_TCHAR(Key):TEXT("")),Value,ESnapshotKind::String,C,At);}
    bool StructMapGetStringBool(const FDasStructValue& H,const char* Path,const char* Key,das::Context* C){return StructScalarMapGet<FBoolProperty>(H,Path,FString(Key?UTF8_TO_TCHAR(Key):TEXT("")),ESnapshotKind::String,C);}
    FDasStructValue StructMapWithStringBool(const FDasStructValue& H,const char* Path,const char* Key,bool Value,das::Context* C,das::LineInfoArg* At){return StructScalarMapWith<FBoolProperty>(H,Path,FString(Key?UTF8_TO_TCHAR(Key):TEXT("")),Value,ESnapshotKind::String,C,At);}
    FDasNameValue StructMapGetStringName(const FDasStructValue& H,const char* Path,const char* Key,das::Context* C){return MakeDSLName(StructScalarMapGet<FNameProperty>(H,Path,FString(Key?UTF8_TO_TCHAR(Key):TEXT("")),ESnapshotKind::String,C),C);}
    FDasStructValue StructMapWithStringName(const FDasStructValue& H,const char* Path,const char* Key,FDasNameValue Value,das::Context* C,das::LineInfoArg* At){return StructScalarMapWith<FNameProperty>(H,Path,FString(Key?UTF8_TO_TCHAR(Key):TEXT("")),ResolveDSLName(Value,C),ESnapshotKind::String,C,At);}
    template<class Snapshot> auto StructNestedLeaf(Snapshot& V,const char* Path,int32 Index,das::Context* C){auto F=StructLeaf(V,Path,C);if(Index==-1)return F;auto View=SnapshotArray(F,C);FString Error;if(!View.ElementAt(Index,F,Error))C->throw_error(TCHAR_TO_UTF8(*Error));if(!F.Property->template IsA<FStructProperty>() && !F.Property->template IsA<FSoftObjectProperty>())C->throw_error("expected snapshot value");return F;}
    FDasStructValue StructNestedGet(const FDasStructValue& H,const char* Path,int32 Index,das::Context* C,das::LineInfoArg* At){
        const auto& V=ResolveStructValue(H,C);auto F=StructNestedLeaf(V,Path,Index,C);return ReadPropertySnapshot(F.Property,F.Memory,C,At);
    }
    FDasStructValue StructNestedWith(const FDasStructValue& H,const char* Path,int32 Index,const FDasStructValue& Value,das::Context* C,das::LineInfoArg* At){
        FStructMutation Mutation(H,C);auto& V=Mutation.Value;auto F=StructNestedLeaf(V,Path,Index,C);WritePropertySnapshot(F.Property,F.Memory,Value,C);return Mutation.Finish(C,At);
    }
    int32 StructGetInt(const FDasStructValue& H,const char* P,das::Context* C){const auto& V=ResolveStructValue(H,C);auto F=StructLeaf(V,P,C);auto* T=CastField<FIntProperty>(F.Property);if(!T)C->throw_error("struct field scalar type mismatch");return ReadPropertyScalarValue(T,F.Memory,C);}
    FDasStructValue StructWithInt(const FDasStructValue& H,const char* P,int32 X,das::Context* C,das::LineInfoArg* At){FStructMutation Mutation(H,C);auto& V=Mutation.Value;auto F=StructLeaf(V,P,C);auto* T=CastField<FIntProperty>(F.Property);if(!T)C->throw_error("struct field scalar type mismatch");WritePropertyScalarValue(T,F.Memory,X,C);return Mutation.Finish(C,At);}
    float StructGetFloat(const FDasStructValue& H,const char* P,das::Context* C){const auto& V=ResolveStructValue(H,C);auto F=StructLeaf(V,P,C);auto* T=CastField<FFloatProperty>(F.Property);if(!T)C->throw_error("struct field scalar type mismatch");return ReadPropertyScalarValue(T,F.Memory,C);}
    FDasStructValue StructWithFloat(const FDasStructValue& H,const char* P,float X,das::Context* C,das::LineInfoArg* At){FStructMutation Mutation(H,C);auto& V=Mutation.Value;auto F=StructLeaf(V,P,C);auto* T=CastField<FFloatProperty>(F.Property);if(!T)C->throw_error("struct field scalar type mismatch");WritePropertyScalarValue(T,F.Memory,X,C);return Mutation.Finish(C,At);}
    double StructGetDouble(const FDasStructValue& H,const char* P,das::Context* C){const auto& V=ResolveStructValue(H,C);auto F=StructLeaf(V,P,C);auto* T=CastField<FDoubleProperty>(F.Property);if(!T)C->throw_error("struct field scalar type mismatch");return ReadPropertyScalarValue(T,F.Memory,C);}
    FDasStructValue StructWithDouble(const FDasStructValue& H,const char* P,double X,das::Context* C,das::LineInfoArg* At){FStructMutation Mutation(H,C);auto& V=Mutation.Value;auto F=StructLeaf(V,P,C);auto* T=CastField<FDoubleProperty>(F.Property);if(!T)C->throw_error("struct field scalar type mismatch");WritePropertyScalarValue(T,F.Memory,X,C);return Mutation.Finish(C,At);}
    bool StructGetBool(const FDasStructValue& H,const char* P,das::Context* C){const auto& V=ResolveStructValue(H,C);auto F=StructLeaf(V,P,C);auto* T=CastField<FBoolProperty>(F.Property);if(!T)C->throw_error("struct field scalar type mismatch");return ReadPropertyScalarValue(T,F.Memory,C);}
    FDasStructValue StructWithBool(const FDasStructValue& H,const char* P,bool X,das::Context* C,das::LineInfoArg* At){FStructMutation Mutation(H,C);auto& V=Mutation.Value;auto F=StructLeaf(V,P,C);auto* T=CastField<FBoolProperty>(F.Property);if(!T)C->throw_error("struct field scalar type mismatch");WritePropertyScalarValue(T,F.Memory,X,C);return Mutation.Finish(C,At);}
    char* StructGetString(const FDasStructValue& H,const char* P,das::Context* C){const auto& V=ResolveStructValue(H,C);auto F=StructLeaf(V,P,C);auto* T=CastField<FStrProperty>(F.Property);if(!T)C->throw_error("struct field scalar type mismatch");return CopyDSLString(ReadPropertyScalarValue(T,F.Memory,C),C);}
    FDasStructValue StructWithString(const FDasStructValue& H,const char* P,const char* X,das::Context* C,das::LineInfoArg* At){FStructMutation Mutation(H,C);auto& V=Mutation.Value;auto F=StructLeaf(V,P,C);auto* T=CastField<FStrProperty>(F.Property);if(!T)C->throw_error("struct field scalar type mismatch");WritePropertyScalarValue(T,F.Memory,X?UTF8_TO_TCHAR(X):TEXT(""),C);return Mutation.Finish(C,At);}
    FDasObjectHandle MakeObjectHandle(UObject*,UClass*,das::Context*);
    FDasObjectHandle ReadNullablePropertyObject(FProperty*,const void*,das::Context*);
    uint64 InternObjectKey(UObject*,das::Context*);
    UObject* ResolveObjectKey(uint64,das::Context*);
    UObject* ResolveObject(const FDasObjectHandle&,das::Context*);
    FDasClassHandle StructGetClass(const FDasStructValue& H,const char* P,das::Context* C){const auto& V=ResolveStructValue(H,C);auto F=StructLeaf(V,P,C);auto* T=CastField<FClassProperty>(F.Property);if(!T)C->throw_error("struct field class type mismatch");const auto Value=ReadNativePropertyValue(T,F.Memory,C);return {MakeObjectHandle(Value.ClassValue.Get(),UClass::StaticClass(),C).Token};}
    FDasObjectHandle StructGetObject(const FDasStructValue& H,const char* P,das::Context* C){const auto& V=ResolveStructValue(H,C);auto F=StructLeaf(V,P,C);auto* T=DasReferencePropertyClass(F.Property);if(!T)C->throw_error("struct field object type mismatch");return ReadNullablePropertyObject(F.Property,F.Memory,C);}
    FDasStructValue StructWithObject(const FDasStructValue& H,const char* P,const FDasObjectHandle& X,das::Context* C,das::LineInfoArg* At){FStructMutation Mutation(H,C);auto& V=Mutation.Value;auto F=StructLeaf(V,P,C);auto* T=DasReferencePropertyClass(F.Property);if(!T)C->throw_error("struct field object type mismatch");UObject* Object=X.Token?ResolveObject(X,C):nullptr;if(Object && !DasReferenceMatches(Object,T))C->throw_error("struct object field has an incompatible class");WriteNativePropertyValue(F.Property,F.Memory,SnapshotObjectValue(F.Property,Object,C),C);return Mutation.Finish(C,At);}
    FDasNameValue StructGetName(const FDasStructValue& H,const char* P,das::Context* C){const auto& V=ResolveStructValue(H,C);auto F=StructLeaf(V,P,C);auto* T=CastField<FNameProperty>(F.Property);if(!T)C->throw_error("struct field scalar type mismatch");return MakeDSLName(ReadPropertyScalarValue(T,F.Memory,C),C);}
    FDasStructValue StructWithName(const FDasStructValue& H,const char* P,FDasNameValue X,das::Context* C,das::LineInfoArg* At){FStructMutation Mutation(H,C);auto& V=Mutation.Value;auto F=StructLeaf(V,P,C);auto* T=CastField<FNameProperty>(F.Property);if(!T)C->throw_error("struct field scalar type mismatch");WritePropertyScalarValue(T,F.Memory,ResolveDSLName(X,C),C);return Mutation.Finish(C,At);}
    char* StructGetEnum(const FDasStructValue& H,const char* Path,const char* EnumPath,das::Context* C){const auto& V=ResolveStructValue(H,C);auto F=StructLeaf(V,Path,C);UEnum* E=RequireCallEnum(F.Property,EnumPath,C);int32 Index=E->GetIndexByValue(ReadNativeEnumNumber(F.Property,F.Memory,C));if(!IsCallEnumMember(E,Index))C->throw_error("unknown or reserved enum value in struct");return CopyDSLString(E->GetNameStringByIndex(Index),C);}
    FDasStructValue StructWithEnum(const FDasStructValue& H,const char* Path,const char* EnumPath,const char* Member,das::Context* C,das::LineInfoArg* At){FStructMutation Mutation(H,C);auto& V=Mutation.Value;auto F=StructLeaf(V,Path,C);UEnum* E=RequireCallEnum(F.Property,EnumPath,C);const FString Symbol=Member?UTF8_TO_TCHAR(Member):TEXT("");if(Symbol.Len()>=NAME_SIZE)C->throw_error("enum member is too long");int32 Index=INDEX_NONE;for(int32 I=0;I<E->NumEnums();++I)if(Symbol.Equals(E->GetNameStringByIndex(I),ESearchCase::IgnoreCase) || Symbol.Equals(E->GetNameByIndex(I).ToString(),ESearchCase::IgnoreCase)){Index=I;break;}if(!IsCallEnumMember(E,Index))C->throw_error("unknown or reserved enum member");WriteNativeEnumNumber(F.Property,F.Memory,E,E->GetValueByIndex(Index),C);return Mutation.Finish(C,At);}
    FDasEnumValue StructEnumGetValue(const FDasStructValue& H,const char* Path,const char* EnumPath,das::Context* C);
    FDasStructValue StructEnumWithValue(const FDasStructValue& H,const char* Path,FDasEnumValue Value,das::Context* C,das::LineInfoArg* At);
    FDasEnumValue MakeEnumValue(const FString& Path,int32 V,das::Context* C){if(!ActiveContext || ActiveContext!=C)C->throw_error("Enum values require active context");FString Error;auto* E=ResolveDasEnum(Path,Error);if(!E || !ValidateDasEnumNumber(Path,V,Error))C->throw_error(TCHAR_TO_UTF8(*Error));for(const auto& P:ActiveContext->EnumValues)if(P.Value.Type.Get()==E && P.Value.Value==V)return {P.Key};if(ActiveContext->EnumValues.Num()>=4096)C->throw_error("Enum value limit reached");static uint64 Next=1;const uint64 Token=Next++;ActiveContext->EnumValues.Add(Token,{E,V});return {Token};}
    const FUnrealDasContext::FEnumValue& ResolveEnumValue(FDasEnumValue V,das::Context* C){if(!ActiveContext || ActiveContext!=C)C->throw_error("Enum values require active context");auto* Value=ActiveContext->EnumValues.Find(V.Token);FString Error;if(!Value || !Value->Type.IsValid() || !ValidateDasEnumNumber(Value->Type->GetPathName(),Value->Value,Error))C->throw_error("Invalid or foreign enum value");return *Value;}
    FDasEnumValue EnumValue(const char* Path,const char* Member,das::Context* C){FString Error;auto* E=ResolveDasEnum(Path?UTF8_TO_TCHAR(Path):TEXT(""),Error);if(!E)C->throw_error(TCHAR_TO_UTF8(*Error));return MakeEnumValue(E->GetPathName(),int32(E->GetValueByNameString(Member?UTF8_TO_TCHAR(Member):TEXT(""))),C);}
    FDasEnumValue EnumFromNumber(const char* Path,int32 V,das::Context* C){return MakeEnumValue(Path?UTF8_TO_TCHAR(Path):TEXT(""),V,C);}
    FDasEnumValue StructEnumGetValue(const FDasStructValue& H,const char* Path,const char* EnumPath,das::Context* C){const auto& V=ResolveStructValue(H,C);auto F=StructLeaf(V,Path,C);auto* E=RequireCallEnum(F.Property,EnumPath,C);return MakeEnumValue(E->GetPathName(),ReadNativeEnumNumber(F.Property,F.Memory,C),C);}
    FDasStructValue StructEnumWithValue(const FDasStructValue& H,const char* Path,FDasEnumValue Value,das::Context* C,das::LineInfoArg* At){FStructMutation Mutation(H,C);auto& V=Mutation.Value;auto F=StructLeaf(V,Path,C);const auto& E=ResolveEnumValue(Value,C);WriteNativeEnumNumber(F.Property,F.Memory,E.Type.Get(),E.Value,C);return Mutation.Finish(C,At);}




    template<typename KeyType> FDasEnumValue StructMapGetEnum(const FDasStructValue& H,const char* Path,const KeyType& Key,ESnapshotKind KeyKind,das::Context* C){return ReadSnapshotMap(H,Path,Key,KeyKind,ESnapshotKind::Enum,C,[&](FProperty* P,const void* Memory){auto* E=DasEnumPropertyType(P);return MakeEnumValue(E->GetPathName(),ReadNativeEnumNumber(P,Memory,C),C);});}
    template<typename KeyType> FDasStructValue StructMapWithEnum(const FDasStructValue& H,const char* Path,const KeyType& Key,FDasEnumValue Value,ESnapshotKind KeyKind,das::Context* C,das::LineInfoArg* At){const auto& E=ResolveEnumValue(Value,C);return WriteSnapshotMap(H,Path,Key,KeyKind,ESnapshotKind::Enum,C,At,[&](FProperty* P,void* Memory){WriteNativeEnumNumber(P,Memory,E.Type.Get(),E.Value,C);});}
    template<typename KeyType> FDasObjectHandle StructMapGetObject(const FDasStructValue& H,const char* Path,const KeyType& Key,ESnapshotKind KeyKind,das::Context* C){return ReadSnapshotMap(H,Path,Key,KeyKind,ESnapshotKind::Object,C,[&](FProperty* P,const void* Memory){return ReadNullablePropertyObject(P,Memory,C);});}
    template<typename KeyType> FDasStructValue StructMapWithObject(const FDasStructValue& H,const char* Path,const KeyType& Key,const FDasObjectHandle& Value,ESnapshotKind KeyKind,das::Context* C,das::LineInfoArg* At){UObject* Object=Value.Token?ResolveObject(Value,C):nullptr;return WriteSnapshotMap(H,Path,Key,KeyKind,ESnapshotKind::Object,C,At,[&](FProperty* P,void* Memory){WriteNativePropertyValue(P,Memory,SnapshotObjectValue(P,Object,C),C);});}
    FDasEnumValue StructMapGetIntEnum(const FDasStructValue& H,const char* Path,int32 Key,das::Context* C){return StructMapGetEnum(H,Path,Key,ESnapshotKind::Int,C);}
    FDasStructValue StructMapWithIntEnum(const FDasStructValue& H,const char* Path,int32 Key,FDasEnumValue V,das::Context* C,das::LineInfoArg* At){return StructMapWithEnum(H,Path,Key,V,ESnapshotKind::Int,C,At);}
    FDasObjectHandle StructMapGetIntObject(const FDasStructValue& H,const char* Path,int32 Key,das::Context* C){return StructMapGetObject(H,Path,Key,ESnapshotKind::Int,C);}
    FDasStructValue StructMapWithIntObject(const FDasStructValue& H,const char* Path,int32 Key,const FDasObjectHandle& V,das::Context* C,das::LineInfoArg* At){return StructMapWithObject(H,Path,Key,V,ESnapshotKind::Int,C,At);}
    FDasEnumValue StructMapGetStringEnum(const FDasStructValue& H,const char* Path,const char* Key,das::Context* C){return StructMapGetEnum(H,Path,FString(Key?UTF8_TO_TCHAR(Key):TEXT("")),ESnapshotKind::String,C);}
    FDasStructValue StructMapWithStringEnum(const FDasStructValue& H,const char* Path,const char* Key,FDasEnumValue V,das::Context* C,das::LineInfoArg* At){return StructMapWithEnum(H,Path,FString(Key?UTF8_TO_TCHAR(Key):TEXT("")),V,ESnapshotKind::String,C,At);}
    FDasObjectHandle StructMapGetStringObject(const FDasStructValue& H,const char* Path,const char* Key,das::Context* C){return StructMapGetObject(H,Path,FString(Key?UTF8_TO_TCHAR(Key):TEXT("")),ESnapshotKind::String,C);}
    FDasStructValue StructMapWithStringObject(const FDasStructValue& H,const char* Path,const char* Key,const FDasObjectHandle& V,das::Context* C,das::LineInfoArg* At){return StructMapWithObject(H,Path,FString(Key?UTF8_TO_TCHAR(Key):TEXT("")),V,ESnapshotKind::String,C,At);}

    int32 EnumNumber(FDasEnumValue V,das::Context* C){return ResolveEnumValue(V,C).Value;}
    int32 CheckedEnumNumber(FDasEnumValue V,const char* Path,das::Context* C){const auto& Value=ResolveEnumValue(V,C);if(!Path || Value.Type->GetPathName()!=UTF8_TO_TCHAR(Path))C->throw_error("Source enum snapshot identity mismatch");return Value.Value;}
    void RequireStructType(const FDasStructValue& V,const char* Path,das::Context* C){const auto& Value=ResolveStructValue(V,C);if(!Path || Value.GetScriptStruct()->GetPathName()!=UTF8_TO_TCHAR(Path))C->throw_error("Source struct snapshot identity mismatch");}
    FDasEnumValue ReadPropertyEnum(FProperty* P,UObject* O,das::Context* C){
        const auto V=ReadOwnerProperty(P,O,C);
        if(V.Kind!=int32(EDasMethodSignature::Enum))C->throw_error("Expected enum property");
        return MakeEnumValue(V.ObjectClass,V.IntValue,C);
    }
    void WritePropertyEnum(FProperty* P,UObject* O,FDasEnumValue Input,das::Context* C){
        const auto& E=ResolveEnumValue(Input,C);FDasScalarValue V;V.Kind=int32(EDasMethodSignature::Enum);V.ObjectClass=E.Type->GetPathName();V.IntValue=E.Value;
        WriteOwnerProperty(P,O,V,C);
    }
    FDasEnumValue GetSelfEnum(const char* Name,das::Context* C){auto* Self=ResolveSelf(C);auto* P=ResolveProperty<FProperty>(Self,Name,false,C);return ReadPropertyEnum(P,Self,C);}
    void SetSelfEnum(const char* Name,FDasEnumValue V,das::Context* C){auto* Self=ResolveSelf(C);if(ActiveContext->bCollectingSubscriptions)C->throw_error("self writes are forbidden in bind_events");auto* P=ResolveProperty<FProperty>(Self,Name,true,C);WritePropertyEnum(P,Self,V,C);}
    bool FrameClassMatches(UClass* Value,const FString& BasePath){auto* Base=FindObject<UClass>(nullptr,*BasePath);return BasePath.StartsWith(TEXT("/Script/")) && IsValid(Base) && !Base->HasAnyClassFlags(CLASS_Interface|CLASS_Deprecated|CLASS_NewerVersionExists) && (!Value || (IsValid(Value) && Value->IsChildOf(Base) && !Value->HasAnyClassFlags(CLASS_Deprecated|CLASS_NewerVersionExists)));}
    void PublishFrameValue(int32 Index,FDasScalarValue Value,das::Context* C){
        if(C!=ActiveContext || !ActiveOutputFrame || !ActiveOutputFrame->Arguments || !ActiveOutputFrame->Arguments->IsValidIndex(Index))C->throw_error("output requires active declared frame slot");
        const auto& Expected=(*ActiveOutputFrame->Arguments)[Index];
        if(Expected.Direction==0 || Value.Kind!=Expected.Kind || ActiveOutputFrame->Values.Contains(Index))C->throw_error("output slot direction/type/duplicate mismatch");
        if(Value.Kind==13){FString Error;if(Value.ObjectClass!=Expected.ObjectClass || !ValidateDasEnumNumber(Expected.ObjectClass,Value.IntValue,Error))C->throw_error("output enum identity/value mismatch");}
        if(Value.Kind==15){if(!FrameClassMatches(Value.ClassValue.Get(),Expected.ObjectClass))C->throw_error("output class base mismatch");if(Value.ClassValue)ActiveOutputFrame->ObjectPins.Emplace(Value.ClassValue.Get());}
        if(Value.Kind==8){auto* Object=Value.ObjectValue.Get();auto* Class=FindObject<UClass>(nullptr,*Expected.ObjectClass);if(!Class || !DasReferenceMatches(Object,Class) || (Object && (!IsValid(Object) || (Cast<AActor>(Object) && CastChecked<AActor>(Object)->IsActorBeingDestroyed()))))C->throw_error("output object/interface identity mismatch");if(Object)ActiveOutputFrame->ObjectPins.Emplace(Object);}
        if((Value.Kind==9 && Value.VectorValue.ContainsNaN()) || (Value.Kind==10 && Value.RotatorValue.ContainsNaN()) || (Value.Kind==11 && (Value.TransformValue.ContainsNaN() || !Value.TransformValue.IsRotationNormalized())))C->throw_error("invalid math frame output");
        Value.Direction=Expected.Direction;Value.ObjectClass=Expected.ObjectClass;
        if(Value.Kind==12){FString Error;if(!Value.StructValue.IsValid() || Value.StructValue.GetScriptStruct()->GetPathName()!=Expected.ObjectClass || !ValidateDasStructSnapshot(Value.StructValue,Error))C->throw_error("output snapshot type/limit mismatch");}
        if((Value.Kind==1 && !FMath::IsFinite(Value.FloatValue)) || (Value.Kind==6 && !FMath::IsFinite(Value.DoubleValue)))C->throw_error("non-finite frame output");
        ActiveOutputFrame->Values.Add(Index,MoveTemp(Value));
    }
    void PublishFrameInt(int32 I,int32 X,das::Context* C){FDasScalarValue V;V.Kind=0;V.IntValue=X;PublishFrameValue(I,MoveTemp(V),C);}
    void PublishFrameFloat(int32 I,float X,das::Context* C){FDasScalarValue V;V.Kind=1;V.FloatValue=X;PublishFrameValue(I,MoveTemp(V),C);}
    void PublishFrameBool(int32 I,bool X,das::Context* C){FDasScalarValue V;V.Kind=2;V.BoolValue=X;PublishFrameValue(I,MoveTemp(V),C);}
    void PublishFrameString(int32 I,const char* X,das::Context* C){FDasScalarValue V;V.Kind=5;V.StringValue=X?UTF8_TO_TCHAR(X):TEXT("");PublishFrameValue(I,MoveTemp(V),C);}
    void PublishFrameDouble(int32 I,double X,das::Context* C){FDasScalarValue V;V.Kind=6;V.DoubleValue=X;PublishFrameValue(I,MoveTemp(V),C);}
    // Name table keys are canonical context-owned tokens; never stringify FName keys.
    FDasEnumValue StructSetElementEnum(const FDasStructValue& H,const char* Path,int32 Index,das::Context* C){auto F=SnapshotKeyAt(H,Path,SnapshotSet(StructLeaf(ResolveStructValue(H,C),Path,C),ESnapshotKind::Enum,C),Index,C);auto* E=DasEnumPropertyType(F.Property);return MakeEnumValue(E->GetPathName(),ReadNativeEnumNumber(F.Property,F.Memory,C),C);}
    FDasStructValue StructSetWithEnum(const FDasStructValue& H,const char* Path,FDasEnumValue Key,bool Present,das::Context* C,das::LineInfoArg* At){const auto& E=ResolveEnumValue(Key,C);const auto& Source=ResolveStructValue(H,C);auto View=SnapshotSet(StructLeaf(Source,Path,C),ESnapshotKind::Enum,C);if(DasEnumPropertyType(View.Property->ElementProp)!=E.Type.Get())C->throw_error("snapshot set enum identity/storage mismatch");uint8 Value=0;WriteNativeEnumNumber(View.Property->ElementProp,&Value,E.Type.Get(),E.Value,C);return SetSnapshotValue(H,Path,&Value,ESnapshotKind::Enum,Present,C,At);}
    FDasNameValue StructSetElementName(const FDasStructValue& H,const char* Path,int32 Index,das::Context* C){auto F=SnapshotKeyAt(H,Path,SnapshotSet(StructLeaf(ResolveStructValue(H,C),Path,C),ESnapshotKind::Name,C),Index,C);return MakeDSLName(ReadPropertyScalarValue(CastFieldChecked<FNameProperty>(F.Property),F.Memory,C),C);}
    FDasStructValue StructSetWithName(const FDasStructValue& H,const char* Path,FDasNameValue Key,bool Present,das::Context* C,das::LineInfoArg* At){const FName Name=ResolveDSLName(Key,C);return SetSnapshotValue(H,Path,&Name,ESnapshotKind::Name,Present,C,At);}
    FDasNameValue StructMapNameKey(const FDasStructValue& H,const char* Path,int32 Index,das::Context* C){auto F=SnapshotKeyAt(H,Path,SnapshotMap(StructLeaf(ResolveStructValue(H,C),Path,C),ESnapshotKind::Name,ESnapshotKind::Invalid,C),Index,C);return MakeDSLName(ReadPropertyScalarValue(CastFieldChecked<FNameProperty>(F.Property),F.Memory,C),C);}
    int32 StructMapGetNameInt(const FDasStructValue& H,const char* Path,FDasNameValue Key,das::Context* C){return StructScalarMapGet<FIntProperty>(H,Path,ResolveDSLName(Key,C),ESnapshotKind::Name,C);}
    FDasStructValue StructMapWithNameInt(const FDasStructValue& H,const char* Path,FDasNameValue Key,int32 Value,das::Context* C,das::LineInfoArg* At){return StructScalarMapWith<FIntProperty>(H,Path,ResolveDSLName(Key,C),Value,ESnapshotKind::Name,C,At);}
    float StructMapGetNameFloat(const FDasStructValue& H,const char* Path,FDasNameValue Key,das::Context* C){return StructScalarMapGet<FFloatProperty>(H,Path,ResolveDSLName(Key,C),ESnapshotKind::Name,C);}
    FDasStructValue StructMapWithNameFloat(const FDasStructValue& H,const char* Path,FDasNameValue Key,float Value,das::Context* C,das::LineInfoArg* At){return StructScalarMapWith<FFloatProperty>(H,Path,ResolveDSLName(Key,C),Value,ESnapshotKind::Name,C,At);}
    double StructMapGetNameDouble(const FDasStructValue& H,const char* Path,FDasNameValue Key,das::Context* C){return StructScalarMapGet<FDoubleProperty>(H,Path,ResolveDSLName(Key,C),ESnapshotKind::Name,C);}
    FDasStructValue StructMapWithNameDouble(const FDasStructValue& H,const char* Path,FDasNameValue Key,double Value,das::Context* C,das::LineInfoArg* At){return StructScalarMapWith<FDoubleProperty>(H,Path,ResolveDSLName(Key,C),Value,ESnapshotKind::Name,C,At);}
    bool StructMapGetNameBool(const FDasStructValue& H,const char* Path,FDasNameValue Key,das::Context* C){return StructScalarMapGet<FBoolProperty>(H,Path,ResolveDSLName(Key,C),ESnapshotKind::Name,C);}
    FDasStructValue StructMapWithNameBool(const FDasStructValue& H,const char* Path,FDasNameValue Key,bool Value,das::Context* C,das::LineInfoArg* At){return StructScalarMapWith<FBoolProperty>(H,Path,ResolveDSLName(Key,C),Value,ESnapshotKind::Name,C,At);}
    char* StructMapGetNameString(const FDasStructValue& H,const char* Path,FDasNameValue Key,das::Context* C){return CopyDSLString(StructScalarMapGet<FStrProperty>(H,Path,ResolveDSLName(Key,C),ESnapshotKind::Name,C),C);}
    FDasStructValue StructMapWithNameString(const FDasStructValue& H,const char* Path,FDasNameValue Key,const char* Value,das::Context* C,das::LineInfoArg* At){return StructScalarMapWith<FStrProperty>(H,Path,ResolveDSLName(Key,C),FString(Value?UTF8_TO_TCHAR(Value):TEXT("")),ESnapshotKind::Name,C,At);}
    FDasNameValue StructMapGetNameName(const FDasStructValue& H,const char* Path,FDasNameValue Key,das::Context* C){return MakeDSLName(StructScalarMapGet<FNameProperty>(H,Path,ResolveDSLName(Key,C),ESnapshotKind::Name,C),C);}
    FDasStructValue StructMapWithNameName(const FDasStructValue& H,const char* Path,FDasNameValue Key,FDasNameValue Value,das::Context* C,das::LineInfoArg* At){return StructScalarMapWith<FNameProperty>(H,Path,ResolveDSLName(Key,C),ResolveDSLName(Value,C),ESnapshotKind::Name,C,At);}
    FDasStructValue StructMapGetNameStruct(const FDasStructValue& H,const char* Path,FDasNameValue Key,das::Context* C,das::LineInfoArg* At){return StructMapGetStruct(H,Path,ResolveDSLName(Key,C),ESnapshotKind::Name,C,At);}
    FDasStructValue StructMapWithNameStruct(const FDasStructValue& H,const char* Path,FDasNameValue Key,const FDasStructValue& Value,das::Context* C,das::LineInfoArg* At){return StructMapWithStruct(H,Path,ResolveDSLName(Key,C),Value,ESnapshotKind::Name,C,At);}
    FDasEnumValue StructMapGetNameEnum(const FDasStructValue& H,const char* Path,FDasNameValue Key,das::Context* C){return StructMapGetEnum(H,Path,ResolveDSLName(Key,C),ESnapshotKind::Name,C);}
    FDasStructValue StructMapWithNameEnum(const FDasStructValue& H,const char* Path,FDasNameValue Key,FDasEnumValue Value,das::Context* C,das::LineInfoArg* At){return StructMapWithEnum(H,Path,ResolveDSLName(Key,C),Value,ESnapshotKind::Name,C,At);}
    FDasObjectHandle StructMapGetNameObject(const FDasStructValue& H,const char* Path,FDasNameValue Key,das::Context* C){return StructMapGetObject(H,Path,ResolveDSLName(Key,C),ESnapshotKind::Name,C);}
    FDasStructValue StructMapWithNameObject(const FDasStructValue& H,const char* Path,FDasNameValue Key,const FDasObjectHandle& Value,das::Context* C,das::LineInfoArg* At){return StructMapWithObject(H,Path,ResolveDSLName(Key,C),Value,ESnapshotKind::Name,C,At);}

    void PublishFrameName(int32 I,FDasNameValue X,das::Context* C){FDasScalarValue V;V.Kind=7;V.NameValue=ResolveDSLName(X,C);PublishFrameValue(I,MoveTemp(V),C);}
    void PublishFrameStruct(int32 I,const FDasStructValue& X,das::Context* C){FDasScalarValue V;V.Kind=12;V.StructValue=ResolveStructValue(X,C);PublishFrameValue(I,MoveTemp(V),C);}
    void PublishFrameObject(int32 I,const FDasObjectHandle& X,das::Context* C){FDasScalarValue V;V.Kind=8;V.ObjectValue=X.Token?ResolveObject(X,C):nullptr;PublishFrameValue(I,MoveTemp(V),C);}
    void PublishFrameClass(int32 I,FDasClassHandle X,das::Context* C){FDasScalarValue V;V.Kind=15;if(X.Token){auto* O=ResolveObject(FDasObjectHandle{X.Token},C);V.ClassValue=Cast<UClass>(O);if(!V.ClassValue)C->throw_error("expected class capability");}PublishFrameValue(I,MoveTemp(V),C);}
    void PublishFrameEnum(int32 I,FDasEnumValue X,das::Context* C){const auto& Value=ResolveEnumValue(X,C);FDasScalarValue V;V.Kind=13;V.ObjectClass=Value.Type->GetPathName();V.IntValue=Value.Value;PublishFrameValue(I,MoveTemp(V),C);}
    void PublishFrameVector(int32 I,const FVector& X,das::Context* C){FDasScalarValue V;V.Kind=9;V.VectorValue=X;PublishFrameValue(I,MoveTemp(V),C);}
    void PublishFrameRotator(int32 I,const FRotator& X,das::Context* C){FDasScalarValue V;V.Kind=10;V.RotatorValue=X;PublishFrameValue(I,MoveTemp(V),C);}
    void PublishFrameTransform(int32 I,const FTransform& X,das::Context* C){FDasScalarValue V;V.Kind=11;V.TransformValue=X;PublishFrameValue(I,MoveTemp(V),C);}
    FDelegateHandle RuntimePreExitHandle;
    thread_local bool InputSnapshotDispatch=false;
    bool VerifyContract(const das::ProgramPtr& Program,const FString& Entry,const FDasCallContract& C,bool AllowNativeInputValue=false)
    {
        if(C.Inputs.Num()>16 || ((static_cast<uint8>(C.ReturnKind)>13 && C.ReturnKind!=EDasMethodSignature::Class && C.ReturnKind!=EDasMethodSignature::Delegate && C.ReturnKind!=EDasMethodSignature::Byte && C.ReturnKind!=EDasMethodSignature::Text) || C.ReturnKind==EDasMethodSignature::Frame)) return false;
        const FTCHARToUTF8 N(*Entry);auto* F=Program->getThisModule()->findUniqueFunction(N.Get());
        if(AllowNativeInputValue && F && F->exports && C.ReturnKind==EDasMethodSignature::Void && C.Inputs.Num()==1 && C.Inputs[0]==EDasMethodSignature::Struct && C.InputClasses.Num()==1 && C.InputClasses[0]==TEXT("/Script/EnhancedInput.InputActionValue") && C.InputDirections.IsEmpty() && F->arguments.size()==1 && F->result && !F->result->ref && F->result->baseType==das::Type::tVoid){
            const auto& T=F->arguments[0]->type;if(T && !T->ref && T->baseType==das::Type::tHandle && T->annotation && T->annotation->name=="InputActionValue")return true;
        }
        auto Matches=[](const das::TypeDeclPtr& T,EDasMethodSignature K){if(!T || T->ref || ((static_cast<uint8>(K)>13 && K!=EDasMethodSignature::Class && K!=EDasMethodSignature::Delegate && K!=EDasMethodSignature::Byte && K!=EDasMethodSignature::Text) || K==EDasMethodSignature::Frame))return false;if(K==EDasMethodSignature::Byte)return T->isSimpleType(das::Type::tUInt8);if(K==EDasMethodSignature::Text)return T->baseType==das::Type::tHandle && T->annotation && T->annotation->name=="UETextValue";if(K==EDasMethodSignature::Class)return T->baseType==das::Type::tHandle && T->annotation && T->annotation->name=="UEClassHandle";if(K==EDasMethodSignature::Delegate)return T->isSimpleType(das::Type::tUInt64);if(K==EDasMethodSignature::Enum)return T->baseType==das::Type::tHandle && T->annotation && T->annotation->name=="UEEnumValue";if(K==EDasMethodSignature::Struct)return T->baseType==das::Type::tHandle && T->annotation && T->annotation->name=="UEStructValue";if(K==EDasMethodSignature::Vector || K==EDasMethodSignature::Rotator || K==EDasMethodSignature::Transform)return T->baseType==das::Type::tHandle && T->annotation && T->annotation->name==(K==EDasMethodSignature::Vector?"UEVector":K==EDasMethodSignature::Rotator?"UERotator":"UETransform");if(K==EDasMethodSignature::Object)return T->baseType==das::Type::tHandle && T->annotation && T->annotation->name=="UObjectHandle";if(K==EDasMethodSignature::Name)return T->baseType==das::Type::tHandle && T->annotation && T->annotation->name=="UENameValue";const auto D=K==EDasMethodSignature::String?das::Type::tString:K==EDasMethodSignature::Double?das::Type::tDouble:K==EDasMethodSignature::Int?das::Type::tInt:K==EDasMethodSignature::Float?das::Type::tFloat:K==EDasMethodSignature::Bool?das::Type::tBool:das::Type::tVoid;return K==EDasMethodSignature::Void?T->baseType==D:T->isSimpleType(D);};
        if(!F || !F->exports || F->arguments.size()!=static_cast<size_t>(C.Inputs.Num()) || !Matches(F->result,C.ReturnKind)) return false;
        for(int32 I=0;I<C.Inputs.Num();++I) if(C.Inputs[I]==EDasMethodSignature::Void || !Matches(F->arguments[I]->type,C.Inputs[I])) return false;
        for(const auto* A:F->annotations)if(F->generated && A->annotation->name=="__ue_callback_frame"){
            auto* P=A->arguments.find("path",das::Type::tString);return P && C.ReturnKind==EDasMethodSignature::Void && C.Inputs.Num()==1 && C.Inputs[0]==EDasMethodSignature::Struct && C.InputClasses.Num()==1 && C.InputClasses[0]==UTF8_TO_TCHAR(P->sValue.c_str()) && FindDasCallbackFrame(C.InputClasses[0]);
        }
        bool Objects=C.ReturnKind==EDasMethodSignature::Delegate || C.ReturnKind==EDasMethodSignature::Object || C.ReturnKind==EDasMethodSignature::Struct || C.ReturnKind==EDasMethodSignature::Enum;for(auto K:C.Inputs)Objects |= K==EDasMethodSignature::Delegate || K==EDasMethodSignature::Object || K==EDasMethodSignature::Struct || K==EDasMethodSignature::Enum;
        {bool Found=false,Valid=true;Program->getThisModule()->structures.foreach([&](const das::StructurePtr& St){for(auto* A:St->annotations)if(A->annotation->name=="ue_class" || A->annotation->name=="ue_library"){auto* Wire=A->arguments.find("_ue_model_json",das::Type::tString);FDasAuthoringModule M;FString Error;if(!Wire || !FDasAuthoringModule::FromJson(UTF8_TO_TCHAR(Wire->sValue.c_str()),M,Error)){Valid=false;continue;}for(const auto& Cl:M.Classes)for(const auto& Method:Cl.Methods)if(Method.EntryPoint==Entry){FDasClassSchema Schema;if(!M.ToActorSchema(Cl.Id,Schema,Error)){Valid=false;continue;}for(const auto& S:Schema.Methods)if(S.EntryPoint==Entry){Found=true;Valid &= S.Contract()==C;}}}});if(!Valid)return false;if(!Found){if(InputSnapshotDispatch && (C.ReturnKind==EDasMethodSignature::Int || C.ReturnKind==EDasMethodSignature::Void) && C.InputDirections.IsEmpty() && !C.Inputs.IsEmpty() && C.Inputs.Num()==C.InputClasses.Num()){
                bool Safe=true;for(int32 I=0;I<C.Inputs.Num();++I)Safe &= C.Inputs[I]==EDasMethodSignature::Struct && DasIsInputEventType(FindObject<UScriptStruct>(nullptr,*C.InputClasses[I]));if(Safe)return true;
            }if(!C.InputDirections.IsEmpty())return false;if(C.ReturnKind==EDasMethodSignature::Class || C.ReturnKind==EDasMethodSignature::Delegate || C.ReturnKind==EDasMethodSignature::Struct || C.ReturnKind==EDasMethodSignature::Enum)return false;for(auto K:C.Inputs)if(K==EDasMethodSignature::Class || K==EDasMethodSignature::Delegate || K==EDasMethodSignature::Struct || K==EDasMethodSignature::Enum)return false;if(C.ReturnKind==EDasMethodSignature::Object && C.ReturnClass!=TEXT("/Script/CoreUObject.Object"))return false;for(int32 I=0;I<C.Inputs.Num();++I)if(C.Inputs[I]==EDasMethodSignature::Object && (!C.InputClasses.IsValidIndex(I) || C.InputClasses[I]!=TEXT("/Script/CoreUObject.Object")))return false;}}
        return true;
    }
    int32 GetSelfInt(const char* Name, das::Context* Context)
    {
        UObject* Self = ResolveSelf(Context);
        return ReadPropertyScalar(ResolveProperty<FIntProperty>(Self, Name, false, Context),Self,Context);
    }
    float GetSelfFloat(const char* Name, das::Context* Context)
    {
        UObject* Self = ResolveSelf(Context);
        return ReadPropertyScalar(ResolveProperty<FFloatProperty>(Self, Name, false, Context),Self,Context);
    }
    void SetSelfInt(const char* Name, int32 Value, das::Context* Context)
    {
        if (ActiveContext && ActiveContext->bCollectingSubscriptions) Context->throw_error("self writes are forbidden in bind_events");
        UObject* Self = ResolveSelf(Context);
        WritePropertyScalar(ResolveProperty<FIntProperty>(Self, Name, true, Context),Self,Value,Context);
    }
    void SetSelfFloat(const char* Name, float Value, das::Context* Context)
    {
        if (ActiveContext && ActiveContext->bCollectingSubscriptions) Context->throw_error("self writes are forbidden in bind_events");
        UObject* Self = ResolveSelf(Context);
        WritePropertyScalar(ResolveProperty<FFloatProperty>(Self, Name, true, Context),Self,Value,Context);
    }

    void RequireObjectContext(das::Context* Context)
    {
        if (!IsInGameThread() || !ActiveContext || Context != ActiveContext)
            Context->throw_error("object handles require the active game-thread context");
    }

    FDasObjectHandle MakeObjectHandle(UObject* Object, UClass* Expected, das::Context* Context)
    {
        RequireObjectContext(Context);
        if(Expected && Expected->HasAnyClassFlags(CLASS_Interface))Expected=UObject::StaticClass();
        if (!IsValid(Object) || (Object->IsA<AActor>() && CastChecked<AActor>(Object)->IsActorBeingDestroyed())) return {};
        for (auto It = ActiveContext->ObjectHandles.CreateIterator(); It; ++It)
        {
            if (!It.Value().Object.IsValid() || !It.Value().ExpectedClass.IsValid()) { It.RemoveCurrent(); continue; }
            if (It.Value().Object.Get() == Object && It.Value().ExpectedClass.Get() == Expected) return {It.Key()};
        }
        const uint64 Token = NextObjectHandle++;
        ActiveContext->ObjectHandles.Add(Token, {Object, Expected});
        return {Token};
    }

    UObject* TryObject(const FDasObjectHandle& Handle, das::Context* Context)
    {
        RequireObjectContext(Context);
        const auto* Entry = ActiveContext->ObjectHandles.Find(Handle.Token);
        if (!Entry) return nullptr;
        UObject* Object = Entry->Object.Get();
        UClass* Expected = Entry->ExpectedClass.Get();
        if (!IsValid(Object) || !Expected || !Object->IsA(Expected)) return nullptr;
        if (auto* Actor = Cast<AActor>(Object); Actor && Actor->IsActorBeingDestroyed()) return nullptr;
        return Object;
    }

    UObject* ResolveObject(const FDasObjectHandle& Handle, das::Context* Context)
    {
        UObject* Object = TryObject(Handle, Context);
        if (!Object) Context->throw_error("object handle is null, expired, or belongs to another context");
        return Object;
    }

    void LibraryScope(const FDasObjectHandle& H,bool RequiresWorld,const das::TBlock<void>& Block,das::Context* C){
        if(C!=ActiveContext || !IsInGameThread())C->throw_error("Library scope requires active game-thread context");
        auto* Object=H.Token?ResolveObject(H,C):nullptr;auto* ResolvedWorld=IsValid(Object) && GEngine?GEngine->GetWorldFromContextObject(Object,EGetWorldErrorMode::ReturnNull):nullptr;
        if(RequiresWorld && (!ResolvedWorld || !ResolvedWorld->IsGameWorld() || ResolvedWorld->bIsTearingDown || Object->IsTemplate() || (Cast<AActor>(Object) && CastChecked<AActor>(Object)->IsActorBeingDestroyed()) || (Cast<UActorComponent>(Object) && CastChecked<UActorComponent>(Object)->IsBeingDestroyed())))C->throw_error("Library requires a live game-world WorldContext");
        bool Good=false;{
            TStrongObjectPtr<UObject> Root(Object);TGuardValue<bool> Library(ActiveContext->bLibraryScope,true);TGuardValue<TWeakObjectPtr<UObject>> World(ActiveContext->LibraryWorld,Object);
            Good=C->runWithCatch([&](){C->invoke(Block,nullptr,nullptr,nullptr);});
        }if(!Good)C->rethrow();
    }
    FDasObjectHandle NullObject() { return {}; }
    FDasObjectHandle ComponentOwner(const FDasObjectHandle& H,das::Context* C){auto* Component=Cast<UActorComponent>(ResolveObject(H,C));if(!Component)C->throw_error("expected ActorComponent handle");return MakeObjectHandle(Component->GetOwner(),AActor::StaticClass(),C);}
    FDasObjectHandle ObjectSelf(das::Context* Context) { return MakeObjectHandle(ResolveSelf(Context), UObject::StaticClass(), Context); }
    bool ObjectValid(const FDasObjectHandle& Handle, das::Context* Context) { return TryObject(Handle, Context) != nullptr; }
    UClass* ResolveClass(const char* Path, das::Context* Context)
    {
        if (!Path || !*Path) Context->throw_error("a loaded full class path is required");
        const FString Name(UTF8_TO_TCHAR(Path));
        if (!Name.StartsWith(TEXT("/")) || !Name.Contains(TEXT("."))) Context->throw_error("use a full loaded class path");
        UClass* Class = FindObject<UClass>(nullptr, *Name);
        if (!Class) Context->throw_error("class is not loaded");
        return Class;
    }
    FDasObjectHandle ObjectValue(const char* Path,das::Context* C){const FString P=Path?UTF8_TO_TCHAR(Path):TEXT("");if(!P.StartsWith(TEXT("/Script/")))C->throw_error("Object field requires a native class path");ResolveClass(Path,C);return {};}
    FProperty* SelfObjectProperty(const char* N,bool Write,das::Context* C){auto* S=ResolveSelf(C);auto* P=S->GetClass()->FindPropertyByName(FName(UTF8_TO_TCHAR(N)));if(!P || P->ArrayDim!=1 || !DasReferencePropertyClass(P))C->throw_error("self reference property type mismatch");if(!P->HasAnyPropertyFlags(CPF_BlueprintVisible) || (Write && P->HasAnyPropertyFlags(CPF_BlueprintReadOnly)))C->throw_error("self property access is not permitted");return P;}
    FDasObjectHandle ReadNullablePropertyObject(FProperty* P,const void* Memory,das::Context* C){
        // VM object getters expose destroyed references as null, including
        // nested/array/map-value getters. Queued captures still use the strict
        // value codec and their original-identity/lifetime checks.
        auto* Object=DasReferenceGet(P,Memory);
        if(Object && (!IsValid(Object) || (Cast<AActor>(Object) && CastChecked<AActor>(Object)->IsActorBeingDestroyed())))return {};
        const auto V=ReadNativePropertyValue(P,Memory,C);
        if(V.Kind!=int32(EDasMethodSignature::Object))C->throw_error("Expected object/interface property");
        return MakeObjectHandle(V.ObjectValue.Get(),DasReferencePropertyClass(P),C);
    }
    FDasObjectHandle ReadPropertyObject(FProperty* P,UObject* O,das::Context* C){const auto Access=ObjectPropertyAccess(P,C);if(!Access.NativeGetter && Access.Getter.IsNone())return ReadNullablePropertyObject(P,P->ContainerPtrToValuePtr<void>(O),C);const auto V=ReadOwnerProperty(P,O,C);return MakeObjectHandle(V.ObjectValue.Get(),DasReferencePropertyClass(P),C);}
    void WritePropertyObject(FProperty* P,UObject* O,const FDasObjectHandle& Input,das::Context* C){
        FDasScalarValue V;V.Kind=int32(EDasMethodSignature::Object);auto* Type=DasReferencePropertyClass(P);if(!Type)C->throw_error("Expected object/interface property");
        V.ObjectClass=Type->GetPathName();V.ObjectValue=Input.Token?ResolveObject(Input,C):nullptr;
        if(V.ObjectValue && !DasReferenceMatches(V.ObjectValue,Type))C->throw_error("object field class mismatch");
        WriteOwnerProperty(P,O,V,C);
    }
    UClass* ReadPropertyClass(FClassProperty* P,UObject* O,das::Context* C){return ReadOwnerProperty(P,O,C).ClassValue.Get();}
    void WritePropertyClass(FClassProperty* P,UObject* O,UClass* Class,das::Context* C){
        FDasScalarValue V;V.Kind=int32(EDasMethodSignature::Class);V.ObjectClass=P->MetaClass->GetPathName();V.ClassValue=Class;
        WriteOwnerProperty(P,O,V,C);
    }
    FDasObjectHandle GetSelfObject(const char* N,das::Context* C){auto* P=SelfObjectProperty(N,false,C);return ReadPropertyObject(P,ResolveSelf(C),C);}
    void SetSelfObject(const char* N,const FDasObjectHandle& V,das::Context* C){ResolveSelf(C);if(ActiveContext->bCollectingSubscriptions)C->throw_error("self writes are forbidden in bind_events");auto* P=SelfObjectProperty(N,true,C);WritePropertyObject(P,ResolveSelf(C),V,C);}
    bool ObjectsEqual(const FDasObjectHandle& Left,const FDasObjectHandle& Right,das::Context* C){RequireObjectContext(C);auto* A=TryObject(Left,C);auto* B=TryObject(Right,C);return A && B && A==B;}
    bool ObjectIsA(const FDasObjectHandle& Handle, const char* ClassPath, das::Context* Context)
    {
        UClass* Class = ResolveClass(ClassPath, Context);
        UObject* Object = TryObject(Handle, Context);
        return Object && Object->IsA(Class);
    }
    FDasObjectHandle ObjectCast(const FDasObjectHandle& Handle, const char* ClassPath, das::Context* Context)
    {
        UObject* Object = ResolveObject(Handle, Context);
        UClass* Class = ResolveClass(ClassPath, Context);
        if (!DasReferenceMatches(Object,Class)) Context->throw_error("object does not match requested class");
        return MakeObjectHandle(Object, Class, Context);
    }
    FDasObjectHandle ObjectCastNullable(const FDasObjectHandle& Handle, const char* ClassPath, das::Context* Context)
    {
        RequireObjectContext(Context);
        UClass* Class = ResolveClass(ClassPath, Context);
        if (!Handle.Token) return {};
        UObject* Object = ResolveObject(Handle, Context);
        if (!DasReferenceMatches(Object,Class)) Context->throw_error("object does not match requested class");
        return MakeObjectHandle(Object, Class, Context);
    }

    FDasObjectHandle ObjectProperty(const FDasObjectHandle& Handle, const char* Name, das::Context* Context)
    {
        UObject* Object = ResolveObject(Handle, Context);
        auto* Property = ResolveProperty<FObjectProperty>(Object, Name, false, Context);
        return MakeObjectHandle(Property->GetObjectPropertyValue_InContainer(Object), Property->PropertyClass, Context);
    }
UObject* ResolveAuthoredObject(const FDasObjectHandle& Handle,const char* Id,das::Context* C){auto* Object=ResolveObject(Handle,C);const UObject* CDO=Object->GetClass()->GetDefaultObject();const FString Actual=DasHostIdentity(CDO);bool Matches=false;if(Id)for(auto* Class=Object->GetClass();Class;Class=Class->GetSuperClass()){const auto* Default=Class->GetDefaultObject();const FString Identity=DasHostIdentity(Default);if(Identity==UTF8_TO_TCHAR(Id) && !Identity.IsEmpty()){Matches=true;break;}}if(!Matches)C->throw_error("authored object identity mismatch");return Object;}
    bool LiveHashObject(UObject* O){return IsValid(O) && (!O->IsA<AActor>() || !CastChecked<AActor>(O)->IsActorBeingDestroyed());}
    uint64 InternObjectKey(UObject* O,das::Context* C){
        RequireObjectContext(C);if(!O)return 0;if(!LiveHashObject(O))C->throw_error("destroyed object cannot be a hash key");
        const FObjectKey Identity(O);if(const auto* Token=ActiveContext->ObjectKeyTokens.Find(Identity))return *Token;
        if(ActiveContext->ObjectKeys.Num()>=65536)C->throw_error("object key token limit exceeded");static uint64 Next=1;const uint64 Token=Next++;ActiveContext->ObjectKeyTokens.Add(Identity,Token);ActiveContext->ObjectKeys.Add(Token,O);return Token;
    }
    UObject* ResolveObjectKey(uint64 Token,das::Context* C){RequireObjectContext(C);if(!Token)return nullptr;const auto* Entry=ActiveContext->ObjectKeys.Find(Token);auto* O=Entry?Entry->Get():nullptr;if(!LiveHashObject(O))C->throw_error("object key is destroyed, expired, forged or belongs to another context");return O;}
    uint64 CheckedObjectKey(uint64 Token,const char* Path,const char* Id,das::Context* C){
        auto* O=ResolveObjectKey(Token,C);auto* Class=Path?FindObject<UClass>(nullptr,UTF8_TO_TCHAR(Path)):nullptr;
        if(!Class || Class->HasAnyClassFlags(CLASS_Interface) || (O && !O->IsA(Class)))C->throw_error("object key class constraint mismatch");
        if(O && Id && *Id)ResolveAuthoredObject(MakeObjectHandle(O,Class,C),Id,C);return Token;
    }
    uint64 ObjectKeyFromReference(const FDasObjectHandle& H,const char* Path,const char* Id,das::Context* C){return CheckedObjectKey(InternObjectKey(H.Token?ResolveObject(H,C):nullptr,C),Path,Id,C);}
    FDasObjectHandle ObjectKeyReference(uint64 Token,const char* Path,const char* Id,das::Context* C){CheckedObjectKey(Token,Path,Id,C);return MakeObjectHandle(ResolveObjectKey(Token,C),FindObject<UClass>(nullptr,UTF8_TO_TCHAR(Path)),C);}
    bool ObjectKeyValid(uint64 Token,das::Context* C){RequireObjectContext(C);const auto* Entry=ActiveContext->ObjectKeys.Find(Token);return Entry && LiveHashObject(Entry->Get());}
    uint64 StructSetElementObjectKey(const FDasStructValue& H,const char* Path,int32 Index,das::Context* C){auto F=SnapshotKeyAt(H,Path,SnapshotSet(StructLeaf(ResolveStructValue(H,C),Path,C),ESnapshotKind::Object,C),Index,C);return InternObjectKey(ReadNativePropertyValue(F.Property,F.Memory,C).ObjectValue.Get(),C);}
    FDasStructValue StructSetWithObjectKey(const FDasStructValue& H,const char* Path,uint64 Token,bool Present,das::Context* C,das::LineInfoArg* At){UObject* O=ResolveObjectKey(Token,C);auto View=SnapshotSet(StructLeaf(ResolveStructValue(H,C),Path,C),ESnapshotKind::Object,C);if(O && !DasReferenceMatches(O,DasReferencePropertyClass(View.Property->ElementProp)))C->throw_error("snapshot object set class mismatch");TObjectPtr<UObject> Key=O;return SetSnapshotValue(H,Path,&Key,ESnapshotKind::Object,Present,C,At);}
    TObjectPtr<UObject> ObjectMapKey(const FDasStructValue& H,const char* Path,uint64 Token,das::Context* C){UObject* O=ResolveObjectKey(Token,C);const auto& V=ResolveStructValue(H,C);auto F=StructLeaf(V,Path,C);auto* P=CastField<FMapProperty>(F.Property);if(!P || !IsDasObjectHashProperty(P->KeyProp) || (O && !DasReferenceMatches(O,DasReferencePropertyClass(P->KeyProp))))C->throw_error("snapshot map object key class mismatch");return O;}
    uint64 StructMapObjectKey(const FDasStructValue& H,const char* Path,int32 Index,das::Context* C){auto F=SnapshotKeyAt(H,Path,SnapshotMap(StructLeaf(ResolveStructValue(H,C),Path,C),ESnapshotKind::Object,ESnapshotKind::Invalid,C),Index,C);return InternObjectKey(ReadNativePropertyValue(F.Property,F.Memory,C).ObjectValue.Get(),C);}
    int32 StructMapGetObjectKeyInt(const FDasStructValue& H,const char* Path,uint64 Token,das::Context* C){return StructScalarMapGet<FIntProperty>(H,Path,ObjectMapKey(H,Path,Token,C),ESnapshotKind::Object,C);}
    FDasStructValue StructMapWithObjectKeyInt(const FDasStructValue& H,const char* Path,uint64 Token,int32 Value,das::Context* C,das::LineInfoArg* At){return StructScalarMapWith<FIntProperty>(H,Path,ObjectMapKey(H,Path,Token,C),Value,ESnapshotKind::Object,C,At);}
    float StructMapGetObjectKeyFloat(const FDasStructValue& H,const char* Path,uint64 Token,das::Context* C){return StructScalarMapGet<FFloatProperty>(H,Path,ObjectMapKey(H,Path,Token,C),ESnapshotKind::Object,C);}
    FDasStructValue StructMapWithObjectKeyFloat(const FDasStructValue& H,const char* Path,uint64 Token,float Value,das::Context* C,das::LineInfoArg* At){return StructScalarMapWith<FFloatProperty>(H,Path,ObjectMapKey(H,Path,Token,C),Value,ESnapshotKind::Object,C,At);}
    double StructMapGetObjectKeyDouble(const FDasStructValue& H,const char* Path,uint64 Token,das::Context* C){return StructScalarMapGet<FDoubleProperty>(H,Path,ObjectMapKey(H,Path,Token,C),ESnapshotKind::Object,C);}
    FDasStructValue StructMapWithObjectKeyDouble(const FDasStructValue& H,const char* Path,uint64 Token,double Value,das::Context* C,das::LineInfoArg* At){return StructScalarMapWith<FDoubleProperty>(H,Path,ObjectMapKey(H,Path,Token,C),Value,ESnapshotKind::Object,C,At);}
    bool StructMapGetObjectKeyBool(const FDasStructValue& H,const char* Path,uint64 Token,das::Context* C){return StructScalarMapGet<FBoolProperty>(H,Path,ObjectMapKey(H,Path,Token,C),ESnapshotKind::Object,C);}
    FDasStructValue StructMapWithObjectKeyBool(const FDasStructValue& H,const char* Path,uint64 Token,bool Value,das::Context* C,das::LineInfoArg* At){return StructScalarMapWith<FBoolProperty>(H,Path,ObjectMapKey(H,Path,Token,C),Value,ESnapshotKind::Object,C,At);}
    char* StructMapGetObjectKeyString(const FDasStructValue& H,const char* Path,uint64 Token,das::Context* C){return CopyDSLString(StructScalarMapGet<FStrProperty>(H,Path,ObjectMapKey(H,Path,Token,C),ESnapshotKind::Object,C),C);}
    FDasStructValue StructMapWithObjectKeyString(const FDasStructValue& H,const char* Path,uint64 Token,const char* Value,das::Context* C,das::LineInfoArg* At){return StructScalarMapWith<FStrProperty>(H,Path,ObjectMapKey(H,Path,Token,C),FString(Value?UTF8_TO_TCHAR(Value):TEXT("")),ESnapshotKind::Object,C,At);}
    FDasNameValue StructMapGetObjectKeyName(const FDasStructValue& H,const char* Path,uint64 Token,das::Context* C){return MakeDSLName(StructScalarMapGet<FNameProperty>(H,Path,ObjectMapKey(H,Path,Token,C),ESnapshotKind::Object,C),C);}
    FDasStructValue StructMapWithObjectKeyName(const FDasStructValue& H,const char* Path,uint64 Token,FDasNameValue Value,das::Context* C,das::LineInfoArg* At){return StructScalarMapWith<FNameProperty>(H,Path,ObjectMapKey(H,Path,Token,C),ResolveDSLName(Value,C),ESnapshotKind::Object,C,At);}
    FDasEnumValue StructMapGetObjectKeyEnum(const FDasStructValue& H,const char* Path,uint64 Token,das::Context* C){return StructMapGetEnum(H,Path,ObjectMapKey(H,Path,Token,C),ESnapshotKind::Object,C);}
    FDasStructValue StructMapWithObjectKeyEnum(const FDasStructValue& H,const char* Path,uint64 Token,FDasEnumValue Value,das::Context* C,das::LineInfoArg* At){return StructMapWithEnum(H,Path,ObjectMapKey(H,Path,Token,C),Value,ESnapshotKind::Object,C,At);}
    FDasObjectHandle StructMapGetObjectKeyObject(const FDasStructValue& H,const char* Path,uint64 Token,das::Context* C){return StructMapGetObject(H,Path,ObjectMapKey(H,Path,Token,C),ESnapshotKind::Object,C);}
    FDasStructValue StructMapWithObjectKeyObject(const FDasStructValue& H,const char* Path,uint64 Token,const FDasObjectHandle& Value,das::Context* C,das::LineInfoArg* At){return StructMapWithObject(H,Path,ObjectMapKey(H,Path,Token,C),Value,ESnapshotKind::Object,C,At);}
    FDasStructValue StructMapGetObjectKeyStruct(const FDasStructValue& H,const char* Path,uint64 Token,das::Context* C,das::LineInfoArg* At){return StructMapGetStruct(H,Path,ObjectMapKey(H,Path,Token,C),ESnapshotKind::Object,C,At);}
    FDasStructValue StructMapWithObjectKeyStruct(const FDasStructValue& H,const char* Path,uint64 Token,const FDasStructValue& Value,das::Context* C,das::LineInfoArg* At){return StructMapWithStruct(H,Path,ObjectMapKey(H,Path,Token,C),Value,ESnapshotKind::Object,C,At);}
    uint8 EnumMapKey(const FDasStructValue& H,const char* Path,FDasEnumValue Token,das::Context* C){const auto& E=ResolveEnumValue(Token,C);const auto& V=ResolveStructValue(H,C);auto F=StructLeaf(V,Path,C);auto* P=CastField<FMapProperty>(F.Property);if(!P || DasEnumPropertyType(P->KeyProp)!=E.Type.Get() || P->KeyProp->GetElementSize()!=1 || E.Value<0 || E.Value>255)C->throw_error("snapshot map enum key identity/storage mismatch");return uint8(E.Value);}
    FDasEnumValue StructMapEnumKey(const FDasStructValue& H,const char* Path,int32 Index,das::Context* C){auto F=SnapshotKeyAt(H,Path,SnapshotMap(StructLeaf(ResolveStructValue(H,C),Path,C),ESnapshotKind::Enum,ESnapshotKind::Invalid,C),Index,C);auto* E=DasEnumPropertyType(F.Property);return MakeEnumValue(E->GetPathName(),ReadNativeEnumNumber(F.Property,F.Memory,C),C);}
    int32 StructMapGetEnumKeyInt(const FDasStructValue& H,const char* Path,FDasEnumValue Token,das::Context* C){return StructScalarMapGet<FIntProperty>(H,Path,EnumMapKey(H,Path,Token,C),ESnapshotKind::Enum,C);}
    FDasStructValue StructMapWithEnumKeyInt(const FDasStructValue& H,const char* Path,FDasEnumValue Token,int32 Value,das::Context* C,das::LineInfoArg* At){return StructScalarMapWith<FIntProperty>(H,Path,EnumMapKey(H,Path,Token,C),Value,ESnapshotKind::Enum,C,At);}
    float StructMapGetEnumKeyFloat(const FDasStructValue& H,const char* Path,FDasEnumValue Token,das::Context* C){return StructScalarMapGet<FFloatProperty>(H,Path,EnumMapKey(H,Path,Token,C),ESnapshotKind::Enum,C);}
    FDasStructValue StructMapWithEnumKeyFloat(const FDasStructValue& H,const char* Path,FDasEnumValue Token,float Value,das::Context* C,das::LineInfoArg* At){return StructScalarMapWith<FFloatProperty>(H,Path,EnumMapKey(H,Path,Token,C),Value,ESnapshotKind::Enum,C,At);}
    double StructMapGetEnumKeyDouble(const FDasStructValue& H,const char* Path,FDasEnumValue Token,das::Context* C){return StructScalarMapGet<FDoubleProperty>(H,Path,EnumMapKey(H,Path,Token,C),ESnapshotKind::Enum,C);}
    FDasStructValue StructMapWithEnumKeyDouble(const FDasStructValue& H,const char* Path,FDasEnumValue Token,double Value,das::Context* C,das::LineInfoArg* At){return StructScalarMapWith<FDoubleProperty>(H,Path,EnumMapKey(H,Path,Token,C),Value,ESnapshotKind::Enum,C,At);}
    bool StructMapGetEnumKeyBool(const FDasStructValue& H,const char* Path,FDasEnumValue Token,das::Context* C){return StructScalarMapGet<FBoolProperty>(H,Path,EnumMapKey(H,Path,Token,C),ESnapshotKind::Enum,C);}
    FDasStructValue StructMapWithEnumKeyBool(const FDasStructValue& H,const char* Path,FDasEnumValue Token,bool Value,das::Context* C,das::LineInfoArg* At){return StructScalarMapWith<FBoolProperty>(H,Path,EnumMapKey(H,Path,Token,C),Value,ESnapshotKind::Enum,C,At);}
    char* StructMapGetEnumKeyString(const FDasStructValue& H,const char* Path,FDasEnumValue Token,das::Context* C){return CopyDSLString(StructScalarMapGet<FStrProperty>(H,Path,EnumMapKey(H,Path,Token,C),ESnapshotKind::Enum,C),C);}
    FDasStructValue StructMapWithEnumKeyString(const FDasStructValue& H,const char* Path,FDasEnumValue Token,const char* Value,das::Context* C,das::LineInfoArg* At){return StructScalarMapWith<FStrProperty>(H,Path,EnumMapKey(H,Path,Token,C),FString(Value?UTF8_TO_TCHAR(Value):TEXT("")),ESnapshotKind::Enum,C,At);}
    FDasNameValue StructMapGetEnumKeyName(const FDasStructValue& H,const char* Path,FDasEnumValue Token,das::Context* C){return MakeDSLName(StructScalarMapGet<FNameProperty>(H,Path,EnumMapKey(H,Path,Token,C),ESnapshotKind::Enum,C),C);}
    FDasStructValue StructMapWithEnumKeyName(const FDasStructValue& H,const char* Path,FDasEnumValue Token,FDasNameValue Value,das::Context* C,das::LineInfoArg* At){return StructScalarMapWith<FNameProperty>(H,Path,EnumMapKey(H,Path,Token,C),ResolveDSLName(Value,C),ESnapshotKind::Enum,C,At);}
    FDasEnumValue StructMapGetEnumKeyEnum(const FDasStructValue& H,const char* Path,FDasEnumValue Token,das::Context* C){return StructMapGetEnum(H,Path,EnumMapKey(H,Path,Token,C),ESnapshotKind::Enum,C);}
    FDasStructValue StructMapWithEnumKeyEnum(const FDasStructValue& H,const char* Path,FDasEnumValue Token,FDasEnumValue Value,das::Context* C,das::LineInfoArg* At){return StructMapWithEnum(H,Path,EnumMapKey(H,Path,Token,C),Value,ESnapshotKind::Enum,C,At);}
    FDasObjectHandle StructMapGetEnumKeyObject(const FDasStructValue& H,const char* Path,FDasEnumValue Token,das::Context* C){return StructMapGetObject(H,Path,EnumMapKey(H,Path,Token,C),ESnapshotKind::Enum,C);}
    FDasStructValue StructMapWithEnumKeyObject(const FDasStructValue& H,const char* Path,FDasEnumValue Token,const FDasObjectHandle& Value,das::Context* C,das::LineInfoArg* At){return StructMapWithObject(H,Path,EnumMapKey(H,Path,Token,C),Value,ESnapshotKind::Enum,C,At);}
    FDasStructValue StructMapGetEnumKeyStruct(const FDasStructValue& H,const char* Path,FDasEnumValue Token,das::Context* C,das::LineInfoArg* At){return StructMapGetStruct(H,Path,EnumMapKey(H,Path,Token,C),ESnapshotKind::Enum,C,At);}
    FDasStructValue StructMapWithEnumKeyStruct(const FDasStructValue& H,const char* Path,FDasEnumValue Token,const FDasStructValue& Value,das::Context* C,das::LineInfoArg* At){return StructMapWithStruct(H,Path,EnumMapKey(H,Path,Token,C),Value,ESnapshotKind::Enum,C,At);}


FDasObjectHandle AuthoredObjectCheck(const FDasObjectHandle& H,const char* Id,das::Context* C){ResolveAuthoredObject(H,Id,C);return H;}
FDasObjectHandle AuthoredNullableCheck(const FDasObjectHandle& Handle,const char* Id,das::Context* Context){RequireObjectContext(Context);if(!Handle.Token)return {};ResolveAuthoredObject(Handle,Id,Context);return Handle;}
int32 AuthoredGetint(const FDasObjectHandle& H,const char* Id,const char* Name,das::Context* C){auto* Object=ResolveAuthoredObject(H,Id,C);return ReadPropertyScalar(ResolveProperty<FIntProperty>(Object,Name,false,C),Object,C);}
void AuthoredSetint(const FDasObjectHandle& H,const char* Id,const char* Name,int32 Value,das::Context* C){auto* Object=ResolveAuthoredObject(H,Id,C);if(ActiveContext->bCollectingSubscriptions)C->throw_error("object writes are forbidden in bind_events");WritePropertyScalar(ResolveProperty<FIntProperty>(Object,Name,true,C),Object,Value,C);}
float AuthoredGetfloat(const FDasObjectHandle& H,const char* Id,const char* Name,das::Context* C){auto* Object=ResolveAuthoredObject(H,Id,C);return ReadPropertyScalar(ResolveProperty<FFloatProperty>(Object,Name,false,C),Object,C);}
void AuthoredSetfloat(const FDasObjectHandle& H,const char* Id,const char* Name,float Value,das::Context* C){auto* Object=ResolveAuthoredObject(H,Id,C);if(ActiveContext->bCollectingSubscriptions)C->throw_error("object writes are forbidden in bind_events");WritePropertyScalar(ResolveProperty<FFloatProperty>(Object,Name,true,C),Object,Value,C);}
double AuthoredGetdouble(const FDasObjectHandle& H,const char* Id,const char* Name,das::Context* C){auto* Object=ResolveAuthoredObject(H,Id,C);return ReadPropertyScalar(ResolveProperty<FDoubleProperty>(Object,Name,false,C),Object,C);}
void AuthoredSetdouble(const FDasObjectHandle& H,const char* Id,const char* Name,double Value,das::Context* C){auto* Object=ResolveAuthoredObject(H,Id,C);if(ActiveContext->bCollectingSubscriptions)C->throw_error("object writes are forbidden in bind_events");WritePropertyScalar(ResolveProperty<FDoubleProperty>(Object,Name,true,C),Object,Value,C);}
bool AuthoredGetbool(const FDasObjectHandle& H,const char* Id,const char* Name,das::Context* C){auto* Object=ResolveAuthoredObject(H,Id,C);return ReadPropertyScalar(ResolveProperty<FBoolProperty>(Object,Name,false,C),Object,C);}
void AuthoredSetbool(const FDasObjectHandle& H,const char* Id,const char* Name,bool Value,das::Context* C){auto* Object=ResolveAuthoredObject(H,Id,C);if(ActiveContext->bCollectingSubscriptions)C->throw_error("object writes are forbidden in bind_events");WritePropertyScalar(ResolveProperty<FBoolProperty>(Object,Name,true,C),Object,Value,C);}
char* AuthoredGetstring(const FDasObjectHandle& H,const char* Id,const char* Name,das::Context* C){auto* O=ResolveAuthoredObject(H,Id,C);return CopyDSLString(ReadPropertyScalar(ResolveProperty<FStrProperty>(O,Name,false,C),O,C),C);}
void AuthoredSetstring(const FDasObjectHandle& H,const char* Id,const char* Name,const char* Value,das::Context* C){auto* O=ResolveAuthoredObject(H,Id,C);if(ActiveContext->bCollectingSubscriptions)C->throw_error("object writes are forbidden in bind_events");WritePropertyScalar(ResolveProperty<FStrProperty>(O,Name,true,C),O,Value?UTF8_TO_TCHAR(Value):TEXT(""),C);}
FDasNameValue AuthoredGetname(const FDasObjectHandle& H,const char* Id,const char* Name,das::Context* C){auto* O=ResolveAuthoredObject(H,Id,C);return MakeDSLName(ReadPropertyScalar(ResolveProperty<FNameProperty>(O,Name,false,C),O,C),C);}
void AuthoredSetname(const FDasObjectHandle& H,const char* Id,const char* Name,FDasNameValue Value,das::Context* C){auto* O=ResolveAuthoredObject(H,Id,C);if(ActiveContext->bCollectingSubscriptions)C->throw_error("object writes are forbidden in bind_events");WritePropertyScalar(ResolveProperty<FNameProperty>(O,Name,true,C),O,ResolveDSLName(Value,C),C);}


FDasStructValue AuthoredGetstruct(const FDasObjectHandle& H,const char* Id,const char* Name,das::Context* C,das::LineInfoArg* At){auto* O=ResolveAuthoredObject(H,Id,C);auto* P=ResolveProperty<FProperty>(O,Name,false,C);return ReadPropertySnapshot(P,P->ContainerPtrToValuePtr<void>(O),C,At);}
void AuthoredSetstruct(const FDasObjectHandle& H,const char* Id,const char* Name,const FDasStructValue& Value,das::Context* C){auto* O=ResolveAuthoredObject(H,Id,C);if(ActiveContext->bCollectingSubscriptions)C->throw_error("object writes are forbidden in bind_events");auto* P=ResolveProperty<FProperty>(O,Name,true,C);WritePropertySnapshot(P,P->ContainerPtrToValuePtr<void>(O),Value,C);}
FDasEnumValue AuthoredGetenum(const FDasObjectHandle& H,const char* Id,const char* Name,das::Context* C){auto* O=ResolveAuthoredObject(H,Id,C);auto* P=ResolveProperty<FProperty>(O,Name,false,C);return ReadPropertyEnum(P,O,C);}
void AuthoredSetenum(const FDasObjectHandle& H,const char* Id,const char* Name,FDasEnumValue Value,das::Context* C){auto* O=ResolveAuthoredObject(H,Id,C);if(ActiveContext->bCollectingSubscriptions)C->throw_error("object writes are forbidden in bind_events");auto* P=ResolveProperty<FProperty>(O,Name,true,C);WritePropertyEnum(P,O,Value,C);}
FDasObjectHandle AuthoredGetobject(const FDasObjectHandle& H,const char* Id,const char* Name,const char* Expected,das::Context* C){auto* O=ResolveAuthoredObject(H,Id,C);auto* P=ResolveProperty<FProperty>(O,Name,false,C);auto* Type=DasReferencePropertyClass(P);if(!Type)C->throw_error("Expected authored object/interface property");if(Expected && *Expected && Type->GetPathName()!=UTF8_TO_TCHAR(Expected))C->throw_error("Authored object property class identity mismatch");return ReadPropertyObject(P,O,C);}
void AuthoredSetobject(const FDasObjectHandle& H,const char* Id,const char* Name,const char* Expected,const FDasObjectHandle& Value,das::Context* C){auto* O=ResolveAuthoredObject(H,Id,C);if(ActiveContext->bCollectingSubscriptions)C->throw_error("object writes are forbidden in bind_events");auto* P=ResolveProperty<FProperty>(O,Name,true,C);auto* Type=DasReferencePropertyClass(P);if(!Type || (Expected && *Expected && Type->GetPathName()!=UTF8_TO_TCHAR(Expected)))C->throw_error("Authored object property class identity mismatch");WritePropertyObject(P,O,Value,C);}

    int32 ObjectGetInt(const FDasObjectHandle& Handle, const char* Name, das::Context* Context)
    {
        UObject* Object = ResolveObject(Handle, Context);
        return ReadPropertyScalar(ResolveProperty<FIntProperty>(Object, Name, false, Context),Object,Context);
    }
    void ObjectSetInt(const FDasObjectHandle& Handle, const char* Name, int32 Value, das::Context* Context)
    {
        UObject* Object = ResolveObject(Handle, Context);
        if (ActiveContext->bCollectingSubscriptions) Context->throw_error("object writes are forbidden in bind_events");
        WritePropertyScalar(ResolveProperty<FIntProperty>(Object, Name, true, Context),Object,Value,Context);
    }

    enum class ECallKind { None, Int, Float, Object };
    struct FCheckedCall { UFunction* Function; FProperty* Input = nullptr; FProperty* Output = nullptr; };
    bool MatchesCallProperty(FProperty* Property, ECallKind Kind)
    {
        if (!Property || Property->ArrayDim != 1) return false;
        if (Kind == ECallKind::Int) return CastField<FIntProperty>(Property) != nullptr;
        if (Kind == ECallKind::Float) return CastField<FFloatProperty>(Property) != nullptr;
        if (Kind == ECallKind::Object) return CastField<FObjectProperty>(Property) != nullptr && !CastField<FClassProperty>(Property);
        return false;
    }
    FCheckedCall CheckCall(UObject* Object, const char* Name, ECallKind Input, ECallKind Output, das::Context* Context)
    {
        if (ActiveContext->bCollectingSubscriptions) Context->throw_error("UFunction calls are forbidden in bind_events");
        if (!Name || !*Name || FCStringAnsi::Strlen(Name) >= NAME_SIZE) Context->throw_error("invalid function name");
        UFunction* Function = Object->FindFunction(FName(UTF8_TO_TCHAR(Name)));
        if (!Function || !Function->HasAnyFunctionFlags(FUNC_BlueprintCallable) ||
            Function->HasAnyFunctionFlags(FUNC_Static | FUNC_Net | FUNC_Delegate | FUNC_MulticastDelegate))
            Context->throw_error("expected a local BlueprintCallable instance UFunction");
        FCheckedCall Call{Function};
        int32 Inputs = 0;
        for (TFieldIterator<FProperty> It(Function); It; ++It)
        {
            FProperty* Property = *It;
            if (!Property->HasAnyPropertyFlags(CPF_Parm)) continue;
            if (Property->HasAnyPropertyFlags(CPF_ReturnParm)) Call.Output = Property;
            else
            {
                ++Inputs; Call.Input = Property;
                if (Property->HasAnyPropertyFlags(CPF_OutParm | CPF_ReferenceParm)) Context->throw_error("out/ref parameters are not supported");
            }
        }
        if (Inputs != (Input == ECallKind::None ? 0 : 1) ||
            (Input != ECallKind::None && !MatchesCallProperty(Call.Input, Input)) ||
            (Output == ECallKind::None ? Call.Output != nullptr : !MatchesCallProperty(Call.Output, Output)))
            Context->throw_error("UFunction signature does not match the requested call adapter");
        return Call;
    }
    int32 CallInt(const FDasObjectHandle& Handle, const char* Name, int32 Value, das::Context* Context)
    {
        UObject* Object = ResolveObject(Handle, Context);
        const auto Call = CheckCall(Object, Name, ECallKind::Int, ECallKind::Int, Context);
        FStructOnScope Frame(Call.Function);
        FDasScalarValue Native;Native.Kind=int32(EDasMethodSignature::Int);Native.IntValue=Value;WriteNativePropertyValue(Call.Input,Call.Input->ContainerPtrToValuePtr<void>(Frame.GetStructMemory()),Native,Context);
        TStrongObjectPtr<UObject> KeepAlive(Object);
        Object->ProcessEvent(Call.Function, Frame.GetStructMemory());
        return ReadNativePropertyValue(Call.Output,Call.Output->ContainerPtrToValuePtr<void>(Frame.GetStructMemory()),Context).IntValue;
    }
    UClass* CheckedInterface(const char* Path,das::Context* C){auto* I=ResolveClass(Path,C);if((!FString(Path).StartsWith(TEXT("/Script/")) && !FString(Path).StartsWith(TEXT("/Game/"))) || !I->HasAnyClassFlags(CLASS_Interface))C->throw_error("expected loaded native UInterface path");return I;}
    FDasObjectHandle CastInterface(const FDasObjectHandle& Handle,const char* Path,das::Context* C){auto* Interface=CheckedInterface(Path,C);if(!Handle.Token)return {};auto* Object=ResolveObject(Handle,C);if(!Object->GetClass()->ImplementsInterface(Interface))C->throw_error("object does not implement requested interface");return Handle;}
    bool ObjectImplementsInterface(const FDasObjectHandle& H,const char* Path,das::Context* C){auto* I=CheckedInterface(Path,C);auto* O=TryObject(H,C);return O && O->GetClass()->ImplementsInterface(I);}
    UFunction* CheckedInterfaceFunction(UObject* O,const char* Path,const char* Name,das::Context* C){auto* I=CheckedInterface(Path,C);if(!O->GetClass()->ImplementsInterface(I))C->throw_error("object does not implement requested interface");if(!Name || !*Name || FCStringAnsi::Strlen(Name)>=NAME_SIZE)C->throw_error("expected bounded interface method name");auto* Declared=I->FindFunctionByName(FName(UTF8_TO_TCHAR(Name)));auto* Actual=O->FindFunction(FName(UTF8_TO_TCHAR(Name)));if(!Declared || !Declared->HasAnyFunctionFlags(FUNC_BlueprintEvent) || !Actual)C->throw_error("reflected interface implementation is unavailable");if(!Actual->IsSignatureCompatibleWith(Declared))C->throw_error("interface dispatch signature mismatch");return Actual;}
    int32 InterfaceCallInt(const FDasObjectHandle& H,const char* Path,const char* Name,int32 Input,das::Context* C);
    char* InterfaceCallString(const FDasObjectHandle& H,const char* Path,const char* Name,das::Context* C);
    float CallFloat(const FDasObjectHandle& Handle, const char* Name, float Value, das::Context* Context)
    {
        UObject* Object = ResolveObject(Handle, Context);
        const auto Call = CheckCall(Object, Name, ECallKind::Float, ECallKind::Float, Context);
        FStructOnScope Frame(Call.Function);
        FDasScalarValue Native;Native.Kind=int32(EDasMethodSignature::Float);Native.FloatValue=Value;WriteNativePropertyValue(Call.Input,Call.Input->ContainerPtrToValuePtr<void>(Frame.GetStructMemory()),Native,Context);
        TStrongObjectPtr<UObject> KeepAlive(Object);
        Object->ProcessEvent(Call.Function, Frame.GetStructMemory());
        return ReadNativePropertyValue(Call.Output,Call.Output->ContainerPtrToValuePtr<void>(Frame.GetStructMemory()),Context).FloatValue;
    }
    void CallVoidInt(const FDasObjectHandle& Handle, const char* Name, int32 Value, das::Context* Context)
    {
        UObject* Object = ResolveObject(Handle, Context);
        const auto Call = CheckCall(Object, Name, ECallKind::Int, ECallKind::None, Context);
        FStructOnScope Frame(Call.Function);
        FDasScalarValue Native;Native.Kind=int32(EDasMethodSignature::Int);Native.IntValue=Value;WriteNativePropertyValue(Call.Input,Call.Input->ContainerPtrToValuePtr<void>(Frame.GetStructMemory()),Native,Context);
        TStrongObjectPtr<UObject> KeepAlive(Object);
        Object->ProcessEvent(Call.Function, Frame.GetStructMemory());
    }
    FDasObjectHandle CallObject(const FDasObjectHandle& Handle, const char* Name, das::Context* Context)
    {
        UObject* Object = ResolveObject(Handle, Context);
        const auto Call = CheckCall(Object, Name, ECallKind::None, ECallKind::Object, Context);
        FStructOnScope Frame(Call.Function);
        TStrongObjectPtr<UObject> KeepAlive(Object);
        Object->ProcessEvent(Call.Function, Frame.GetStructMemory());
        auto* Return = CastFieldChecked<FObjectProperty>(Call.Output);
        return ReadNullablePropertyObject(Return,Return->ContainerPtrToValuePtr<void>(Frame.GetStructMemory()),Context);
    }
    void CallVoidObject(const FDasObjectHandle& Handle, const char* Name, const FDasObjectHandle& Value, das::Context* Context)
    {
        UObject* Object = ResolveObject(Handle, Context);
        const auto Call = CheckCall(Object, Name, ECallKind::Object, ECallKind::None, Context);
        UObject* Argument = Value.Token == 0 ? nullptr : ResolveObject(Value, Context);
        auto* Property = CastFieldChecked<FObjectProperty>(Call.Input);
        if (Argument && !Argument->IsA(Property->PropertyClass)) Context->throw_error("object argument has an incompatible class");
        FStructOnScope Frame(Call.Function);
        FDasScalarValue Native;Native.Kind=int32(EDasMethodSignature::Object);Native.ObjectClass=Property->PropertyClass->GetPathName();Native.ObjectValue=Argument;WriteNativePropertyValue(Property,Property->ContainerPtrToValuePtr<void>(Frame.GetStructMemory()),Native,Context);
        TStrongObjectPtr<UObject> KeepAlive(Object);
        TStrongObjectPtr<UObject> KeepArgumentAlive(Argument);
        Object->ProcessEvent(Call.Function, Frame.GetStructMemory());
    }

    void ValidateConstraintCallInputs(FMarshalledCall&,das::Context*);
    void ValidateInputEventCallInputs(FMarshalledCall&,das::Context*);
    void CaptureConstraintCallOutput(FMarshalledCall&,das::Context*);
    #include "DasMarshalling.inl"
    // Legacy scalar adapters use the same checked frame and dispatch scope as
    // generated interface methods, including recursion and error propagation.
    int32 InterfaceCallInt(const FDasObjectHandle& H,const char* Path,const char* Name,int32 Input,das::Context* C){
        auto* Object=ResolveObject(H,C);CheckedInterfaceFunction(Object,Path,Name,C);const auto Shape=CheckCall(Object,Name,ECallKind::Int,ECallKind::Int,C);
        auto Call=BeginInterfaceCall(H,Path,Name,C);ON_SCOPE_EXIT{ActiveContext->Calls.Remove(Call.Token);};
        SetCallInt(Call,TCHAR_TO_UTF8(*Shape.Input->GetName()),Input,C);InvokeCall(Call,C);return GetCallScalar<FIntProperty>(Call,TCHAR_TO_UTF8(*Shape.Output->GetName()),C);
    }
    char* InterfaceCallString(const FDasObjectHandle& H,const char* Path,const char* Name,das::Context* C){
        auto* Object=ResolveObject(H,C);auto* Function=CheckedInterfaceFunction(Object,Path,Name,C);if(Function->NumParms!=1 || !CastField<FStrProperty>(Function->GetReturnProperty()))C->throw_error("interface string getter signature mismatch");
        auto Call=BeginInterfaceCall(H,Path,Name,C);ON_SCOPE_EXIT{ActiveContext->Calls.Remove(Call.Token);};InvokeCall(Call,C);return GetCallString(Call,TCHAR_TO_UTF8(*Function->GetReturnProperty()->GetName()),C);
    }

    #include "DasTextValues.inl"
    #include "DasAuthoredTextByte.inl"
    #include "DasTextByteContainers.inl"

// Cleanup is idempotent: a rejected BeginCall may not have produced a handle yet.
void ReleaseAuthoredCall(const FDasCallHandle& H,das::Context* C){if(C==ActiveContext && ActiveContext)ActiveContext->Calls.Remove(H.Token);}
FDasCallHandle BeginAuthoredCallSelected(const FDasObjectHandle& Target,const char* Id,const char* Name,const char* Wire,das::Context* Context,UFunction* Selected=nullptr){
 auto* Object=ResolveAuthoredObject(Target,Id,Context);FDasAuthoringModule Model;FString Error;if(!Wire || !FDasAuthoringModule::FromJson(UTF8_TO_TCHAR(Wire),Model,Error) || Model.Classes.Num()!=1 || Model.Classes[0].Id!=UTF8_TO_TCHAR(Id))Context->throw_error("invalid authored method contract");
 const auto* Method=Model.Classes[0].Methods.FindByPredicate([&](const auto& Item){return Name && Item.Name==UTF8_TO_TCHAR(Name);});if(!Method || !Method->bCallable || Method->bStatic)Context->throw_error("unknown callable authored method");
 auto Handle=BeginCallSelected(Target,Name,Context,Selected);auto& Call=ResolveCall(Handle,Context);

 auto Matches=[&](FProperty* Property,const FDasDeclaredType& Type){if(!Property || FString(CallKindName(FieldKind(Property,Context)))!=Type.Kind)return false;if(Type.Kind==TEXT("array") || Type.Kind==TEXT("map") || Type.Kind==TEXT("set")){auto* Carrier=DasContainerCarrier(Type);auto* Values=Carrier?Carrier->FindPropertyByName(TEXT("Values")):nullptr;return Values && Values->SameType(Property);}if(auto* D=CastField<FDelegateProperty>(Property)){TStrongObjectPtr<UFunction> S(MakeDasSingleDelegateSignature(Type.DelegateWire,Error));return S && D->SignatureFunction && S->IsSignatureCompatibleWith(D->SignatureFunction);}if(auto* Enum=CallEnumType(Property))return Enum->GetPathName()==Type.TypePath;if(auto* Struct=CastField<FStructProperty>(Property))return Struct->Struct->GetPathName()==Type.TypePath;if(auto* Class=CastField<FClassProperty>(Property))return Class->MetaClass->GetPathName()==Type.TypePath;if(auto* Class=DasReferencePropertyClass(Property))return Class->GetPathName()==Type.TypePath;return true;};
 if(Method->ReturnType.Kind==TEXT("void")?Call.Function->GetReturnProperty()!=nullptr:!Matches(Call.Function->GetReturnProperty(),Method->ReturnType))Context->throw_error("authored method return contract changed");
 int32 Index=0;for(TFieldIterator<FProperty> It(Call.Function.Get());It;++It){if(!It->HasAnyPropertyFlags(CPF_Parm) || It->HasAnyPropertyFlags(CPF_ReturnParm))continue;++Index;const auto* DeclaredParameter=Method->Parameters.FindByPredicate([&](const auto& P){return P.Name==It->GetName();});if(!DeclaredParameter)Context->throw_error("authored method parameter name changed");const auto& Param=*DeclaredParameter;const bool Out=It->HasAnyPropertyFlags(CPF_OutParm) && !It->HasAnyPropertyFlags(CPF_ConstParm);const FString Direction=Out?(It->HasAnyPropertyFlags(CPF_ReferenceParm)?TEXT("inout"):TEXT("out")):TEXT("in");const bool ContainerInput=Param.Direction==TEXT("in") && Direction==TEXT("inout") && (Param.Type.Kind==TEXT("array") || Param.Type.Kind==TEXT("map") || Param.Type.Kind==TEXT("set"));if(It->GetName()!=Param.Name || (Direction!=Param.Direction && !ContainerInput) || !Matches(*It,Param.Type))Context->throw_error(TCHAR_TO_UTF8(*FString::Printf(TEXT("authored method parameter contract changed: %s.%s direction=%s/%s kind=%s/%s matches=%d"),*Method->Name,*It->GetName(),*Direction,*Param.Direction,CallKindName(FieldKind(*It,Context)),*Param.Type.Kind,Matches(*It,Param.Type))));}
 if(Index!=Method->Parameters.Num())Context->throw_error("authored method parameter count changed");Call.bAuthored=true;return Handle;
}
void MethodArgname(const FDasCallHandle& H,const char* Path,FDasNameValue Value,das::Context* C){const auto Name=ResolveDSLName(Value,C);SetCallName(H,Path,TCHAR_TO_UTF8(*Name.ToString()),C);}
FDasNameValue MethodResultname(const FDasCallHandle& H,const char* Path,das::Context* C){return MakeDSLName(GetCallScalar<FNameProperty>(H,Path,C),C);}
void MethodArgenum(const FDasCallHandle& H,const char* Path,FDasEnumValue Value,das::Context* C){const auto& V=ResolveEnumValue(Value,C);auto& Call=ResolveCall(H,C);const auto Field=ResolveCallField(Call,Path,false,C);RequireCallEnum(Field.Property,TCHAR_TO_UTF8(*V.Type->GetPathName()),C);WriteCallEnum(Call,Field,V.Type.Get(),int64(V.Value),C);}
FDasEnumValue MethodResultenum(const FDasCallHandle& H,const char* Path,das::Context* C){auto& Call=ResolveCall(H,C);const auto Field=ResolveCallField(Call,Path,true,C);auto* Enum=CallEnumType(Field.Property);if(!Enum)C->throw_error("expected enum result");return MakeEnumValue(Enum->GetPathName(),int32(ReadCallEnum(Field,C)),C);}
// Whole struct snapshots avoid exposing UE addresses or retaining stale property pointers.
FCallField MethodStructField(FMarshalledCall& Call,const char* Path,bool Output,das::Context* C){
 if(Call.bFailed || Call.bExecuted!=Output || !Path)C->throw_error("invalid method snapshot access direction");auto* Prop=FindFProperty<FProperty>(Call.Function.Get(),FName(UTF8_TO_TCHAR(Path)));if(!Prop || !Prop->HasAnyPropertyFlags(CPF_Parm) || (!CastField<FStructProperty>(Prop) && !DasContainerCarrierForProperty(Prop)))C->throw_error("expected struct/container method parameter");const bool MutableOut=Prop->HasAnyPropertyFlags(CPF_OutParm) && !Prop->HasAnyPropertyFlags(CPF_ConstParm);const bool Return=Prop->HasAnyPropertyFlags(CPF_ReturnParm);if(Output?!(Return || MutableOut):(Return || (MutableOut && !Prop->HasAnyPropertyFlags(CPF_ReferenceParm))))C->throw_error("wrong snapshot parameter direction");return {Prop,Prop->ContainerPtrToValuePtr<void>(Call.Frame->GetStructMemory()),Prop->GetName().ToLower()};
}
FDasCallHandle BeginAuthoredCall(const FDasObjectHandle& T,const char* Id,const char* N,const char* W,das::Context* C){return BeginAuthoredCallSelected(T,Id,N,W,C);}
thread_local bool ExplicitParentTick=false;
thread_local UObject* ParentLifecycleTarget=nullptr;
FDasCallHandle BeginAuthoredSuperCall(const FDasObjectHandle& Target,const char* CurrentId,const char* ParentId,const char* Name,const char* Wire,das::Context* C){auto* Object=ResolveAuthoredObject(Target,CurrentId,C);auto Identity=[](UClass* Class){const auto* D=Class?Class->GetDefaultObject():nullptr;return DasHostIdentity(D);};UClass* Owner=nullptr;for(auto* Class=Object->GetClass();Class;Class=Class->GetSuperClass())if(Identity(Class)==UTF8_TO_TCHAR(CurrentId) && Identity(Class->GetSuperClass())!=UTF8_TO_TCHAR(CurrentId)){Owner=Class;break;}auto* Parent=Owner?Owner->GetSuperClass():nullptr;if(!Parent || Identity(Parent)!=UTF8_TO_TCHAR(ParentId))C->throw_error("Super parent identity changed");auto* Function=Parent->FindFunctionByName(FName(UTF8_TO_TCHAR(Name)));if(!Function)C->throw_error("Super method is missing");auto H=BeginAuthoredCallSelected(Target,ParentId,Name,Wire,C,Function);ResolveCall(H,C).FunctionScopeClass=Parent;return H;}
void InvokeParentLifecycle(const char* Id,int32 Kind,float Delta,int32 Reason,das::Context* C){RequireObjectContext(C);if(Kind<0 || Kind>4 || !FMath::IsFinite(Delta) || Delta<0)C->throw_error("invalid parent lifecycle call");if(Kind==3 && !ActiveContext->bInitializingObject)C->throw_error("Super.Initialize is only allowed during Initialize");if(Kind==4 && !ConstructingObject)C->throw_error("Super.ConstructionScript is only allowed during ConstructionScript");if(ConstructingObject && Kind!=4)C->throw_error("gameplay lifecycle is forbidden during ConstructionScript");auto* Object=ResolveSelf(C);auto Identity=[](UClass* Class){const auto* D=Class?Class->GetDefaultObject():nullptr;return DasHostIdentity(D);};UClass* Owner=nullptr;for(auto* Class=Object->GetClass();Class;Class=Class->GetSuperClass())if(Identity(Class)==UTF8_TO_TCHAR(Id) && Identity(Class->GetSuperClass())!=UTF8_TO_TCHAR(Id)){Owner=Class;break;}if(!Owner || !Owner->GetSuperClass())C->throw_error("authored lifecycle parent is missing");const auto* Default=Owner->GetSuperClass()->GetDefaultObject();const FString File=DasHostLifecycle(Default);if(File.IsEmpty())return;TGuardValue<bool> TickScope(ExplicitParentTick,Kind==1);TGuardValue<UObject*> ParentScope(ParentLifecycleTarget,Object);auto& R=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));const auto Result=R.RunLifecycle(R.ScriptPath(File),Kind==0?EDasCallback::BeginPlay:Kind==1?EDasCallback::Tick:Kind==2?EDasCallback::EndPlay:Kind==3?EDasCallback::Initialize:EDasCallback::Construction,Object,Delta,Reason);if(!Result.bSucceeded)C->throw_error(TCHAR_TO_UTF8(*Result.Error));}
void PinMethodSnapshot(FProperty* Property,void* Memory,FMarshalledCall& Call,const FString& Path,das::Context* C,int32& Budget,int32 Depth=0){
 if(Depth>16 || --Budget<0)C->throw_error("method snapshot traversal limit exceeded");
 if(CastField<FWeakObjectProperty>(Property))return;
 if(DasReferencePropertyClass(Property)){auto* Object=DasReferenceGet(Property,Memory);if(Object && (!IsValid(Object) || (Cast<AActor>(Object) && CastChecked<AActor>(Object)->IsActorBeingDestroyed())))C->throw_error("method snapshot object is expired");Call.ObjectPins.Add(Path,TStrongObjectPtr<UObject>(Object));}
 else if(auto* Struct=CastField<FStructProperty>(Property)){for(TFieldIterator<FProperty> It(Struct->Struct);It;++It)PinMethodSnapshot(*It,It->ContainerPtrToValuePtr<void>(Memory),Call,Path+TEXT(".")+It->GetName().ToLower(),C,Budget,Depth+1);}
 else if(auto* Array=CastField<FArrayProperty>(Property)){FScriptArrayHelper Values(Array,Memory);if(Values.Num()>MaxCallArrayElements)C->throw_error("method array size limit exceeded");for(int32 Index=0;Index<Values.Num();++Index)PinMethodSnapshot(Array->Inner,Values.GetRawPtr(Index),Call,Path+TEXT(".")+FString::FromInt(Index),C,Budget,Depth+1);}
 else if(auto* Map=CastField<FMapProperty>(Property)){FScriptMapHelper Values(Map,Memory);if(Values.Num()>MaxCallArrayElements)C->throw_error("method map size limit exceeded");for(int32 Index=0;Index<Values.GetMaxIndex();++Index)if(Values.IsValidIndex(Index)){PinMethodSnapshot(Map->KeyProp,Values.GetKeyPtr(Index),Call,Path+TEXT(".key")+FString::FromInt(Index),C,Budget,Depth+1);PinMethodSnapshot(Map->ValueProp,Values.GetValuePtr(Index),Call,Path+TEXT(".value")+FString::FromInt(Index),C,Budget,Depth+1);}}
 else if(auto* Set=CastField<FSetProperty>(Property)){FScriptSetHelper Values(Set,Memory);if(Values.Num()>MaxCallArrayElements)C->throw_error("method set size limit exceeded");for(int32 Index=0;Index<Values.GetMaxIndex();++Index)if(Values.IsValidIndex(Index))PinMethodSnapshot(Set->ElementProp,Values.GetElementPtr(Index),Call,Path+TEXT(".")+FString::FromInt(Index),C,Budget,Depth+1);}
}
void MethodArgstruct(const FDasCallHandle& H,const char* Path,const FDasStructValue& Value,das::Context* C){
 auto& Call=ResolveCall(H,C);const auto Field=MethodStructField(Call,Path,false,C);
 FString Error;if(!WriteDasPropertySnapshot(Field.Property,Field.Memory,ResolveStructValue(Value,C),Error))C->throw_error(TCHAR_TO_UTF8(*Error));
 Call.Assigned.Add(Field.Path);
 int32 Budget=1048576;PinMethodSnapshot(Field.Property,Field.Memory,Call,Field.Path,C,Budget);Call.WholeInputs.Add(Field.Path);
}
FDasStructValue MethodResultstruct(const FDasCallHandle& H,const char* Path,das::Context* C,das::LineInfoArg* At){
 auto& Call=ResolveCall(H,C);const auto Field=MethodStructField(Call,Path,true,C);
 return StoreStructValue(ReadCallValue(Field,C).StructValue,C,At);
}


// Math values use native copied storage in the same reflected call frame.
template<class T> FDasStructValue MathPack(const T& Value,das::Context* C,das::LineInfoArg* At){FInstancedStruct Snapshot;Snapshot.InitializeAs(TBaseStructure<T>::Get(),reinterpret_cast<const uint8*>(&Value));return StoreStructValue(MoveTemp(Snapshot),C,At);}
template<class T> T MathUnpack(const FDasStructValue& Value,das::Context* C){const auto& Snapshot=ResolveStructValue(Value,C);if(Snapshot.GetScriptStruct()!=TBaseStructure<T>::Get() || !MathSnapshotValid(Snapshot))C->throw_error("math property snapshot type/value mismatch");return Snapshot.Get<T>();}
FVector2D MakeVector2D(double X,double Y){return FVector2D(X,Y);}
FDasStructValue MathPackvector2d(const FVector2D& V,das::Context* C,das::LineInfoArg* At){return MathPack(V,C,At);}
FVector2D MathUnpackvector2d(const FDasStructValue& V,das::Context* C){return MathUnpack<FVector2D>(V,C);}
FDasStructValue MathPackvector(const FVector& V,das::Context* C,das::LineInfoArg* At){return MathPack(V,C,At);}
FDasStructValue MathPackrotator(const FRotator& V,das::Context* C,das::LineInfoArg* At){return MathPack(V,C,At);}
FDasStructValue MathPacktransform(const FTransform& V,das::Context* C,das::LineInfoArg* At){return MathPack(V,C,At);}
FVector MathUnpackvector(const FDasStructValue& V,das::Context* C){return MathUnpack<FVector>(V,C);}
FRotator MathUnpackrotator(const FDasStructValue& V,das::Context* C){return MathUnpack<FRotator>(V,C);}
FTransform MathUnpacktransform(const FDasStructValue& V,das::Context* C){return MathUnpack<FTransform>(V,C);}
bool MethodMathValid(const FVector2D& Value){return !Value.ContainsNaN();}
bool MethodMathValid(const FVector& Value){return !Value.ContainsNaN();}
bool MethodMathValid(const FRotator& Value){return !Value.ContainsNaN();}
bool MethodMathValid(const FTransform& Value){return !Value.ContainsNaN() && Value.IsRotationNormalized();}
// Concrete leaf assignments also cover aggregate writes to elements of arrays.
void MarkMathCallLeaves(FMarshalledCall& Call,FProperty* Property,const FString& Path){
 if(auto* Struct=CastField<FStructProperty>(Property)){for(TFieldIterator<FProperty> It(Struct->Struct);It;++It)MarkMathCallLeaves(Call,*It,Path+TEXT(".")+It->GetName().ToLower());}
 else Call.Assigned.Add(Path);
}
template<class T> void MethodArgMath(const FDasCallHandle& H,const char* Path,const T& Value,das::Context* C){auto& Call=ResolveCall(H,C);const auto Field=ResolveCallField(Call,Path,false,C,true);auto* Prop=CastField<FStructProperty>(Field.Property);if(!Prop || Prop->Struct!=TBaseStructure<T>::Get() || !MethodMathValid(Value))C->throw_error("method math input type/value mismatch");FDasScalarValue Native;Native.Kind=int32(EDasMethodSignature::Struct);Native.StructValue.InitializeAs(TBaseStructure<T>::Get(),reinterpret_cast<const uint8*>(&Value));WriteCallValue(Call,Field,Native,C);MarkMathCallLeaves(Call,Field.Property,Field.Path);}
template<class T> T MethodResultMath(const FDasCallHandle& H,const char* Path,das::Context* C){auto& Call=ResolveCall(H,C);const auto Field=ResolveCallField(Call,Path,true,C,true);auto* Prop=CastField<FStructProperty>(Field.Property);if(!Prop || Prop->Struct!=TBaseStructure<T>::Get())C->throw_error("method math result type mismatch");const auto Native=ReadCallValue(Field,C);return Native.StructValue.Get<T>();}
void MethodArgvector2d(const FDasCallHandle& H,const char* P,const FVector2D& V,das::Context* C){MethodArgMath(H,P,V,C);}
void MethodArgvector(const FDasCallHandle& H,const char* P,const FVector& V,das::Context* C){MethodArgMath(H,P,V,C);}
void MethodArgrotator(const FDasCallHandle& H,const char* P,const FRotator& V,das::Context* C){MethodArgMath(H,P,V,C);}
void MethodArgtransform(const FDasCallHandle& H,const char* P,const FTransform& V,das::Context* C){MethodArgMath(H,P,V,C);}
FVector2D MethodResultvector2d(const FDasCallHandle& H,const char* P,das::Context* C){return MethodResultMath<FVector2D>(H,P,C);}
FVector MethodResultvector(const FDasCallHandle& H,const char* P,das::Context* C){return MethodResultMath<FVector>(H,P,C);}
FRotator MethodResultrotator(const FDasCallHandle& H,const char* P,das::Context* C){return MethodResultMath<FRotator>(H,P,C);}
FTransform MethodResulttransform(const FDasCallHandle& H,const char* P,das::Context* C){return MethodResultMath<FTransform>(H,P,C);}
#include "DasDelegateValues.inl"


    FProperty* ScalarEventParameter(FMulticastDelegateProperty* P){if(!P || !P->SignatureFunction)return nullptr;for(TFieldIterator<FProperty> It(P->SignatureFunction);It;++It)if(It->HasAnyPropertyFlags(CPF_Parm) && !It->HasAnyPropertyFlags(CPF_ReturnParm))return *It;return nullptr;}

    TMap<TWeakObjectPtr<UActorComponent>,TWeakObjectPtr<UObject>> CreatedScriptScenes;
    USceneComponent* ScriptScene(const FDasObjectHandle& H,das::Context* C){auto* S=Cast<USceneComponent>(ResolveObject(H,C));if(!S || S->IsBeingDestroyed())C->throw_error("expected live SceneComponent");if(auto* A=S->GetOwner();A && A->IsActorBeingDestroyed() && !ActiveContext->bEnding)C->throw_error("SceneComponent owner is being destroyed");return S;}
    void SceneMutation(USceneComponent* S,das::Context* C){auto* Self=ResolveWorldContextObject(C);if(ActiveContext->bCollectingSubscriptions)C->throw_error("scene mutations are forbidden in bind_events");if(!S->GetWorld() || S->GetWorld()!=Self->GetWorld() || S->IsTemplate())C->throw_error("scene mutation requires component in script owner's world");}
    bool SceneValid(const FDasObjectHandle& H,das::Context* C){auto* S=Cast<USceneComponent>(TryObject(H,C));return S && !S->IsBeingDestroyed() && (!S->GetOwner() || !S->GetOwner()->IsActorBeingDestroyed());}
    FDasObjectHandle ActorOwner(const FDasObjectHandle& H,das::Context* C){auto* A=Cast<AActor>(ResolveObject(H,C));if(!A)C->throw_error("expected Actor for owner");return MakeObjectHandle(A->GetOwner(),AActor::StaticClass(),C);}
    FDasObjectHandle SceneActorRoot(const FDasObjectHandle& H,das::Context* C){auto* A=Cast<AActor>(ResolveObject(H,C));if(!A)C->throw_error("expected Actor for root component");return MakeObjectHandle(A->GetRootComponent(),USceneComponent::StaticClass(),C);}
    FDasObjectHandle SceneParent(const FDasObjectHandle& H,das::Context* C){return MakeObjectHandle(ScriptScene(H,C)->GetAttachParent(),USceneComponent::StaticClass(),C);}
    FTransform SceneRelative(const FDasObjectHandle& H,das::Context* C){return ScriptScene(H,C)->GetRelativeTransform();}
    FTransform SceneWorld(const FDasObjectHandle& H,das::Context* C){return ScriptScene(H,C)->GetComponentTransform();}
    FVector SceneRelativeTranslation(const FDasObjectHandle& H,das::Context* C){return ScriptScene(H,C)->GetRelativeLocation();}
    FRotator SceneRelativeRotation(const FDasObjectHandle& H,das::Context* C){return ScriptScene(H,C)->GetRelativeRotation();}
    FVector SceneRelativeScale(const FDasObjectHandle& H,das::Context* C){return ScriptScene(H,C)->GetRelativeScale3D();}
    void SceneSetRelative(const FDasObjectHandle& H,const FTransform& V,bool Teleport,das::Context* C){auto* S=ScriptScene(H,C);SceneMutation(S,C);if(!MethodMathValid(V))C->throw_error("invalid scene transform");S->SetRelativeTransform(V,false,nullptr,Teleport?ETeleportType::TeleportPhysics:ETeleportType::None);}
    void SceneSetWorld(const FDasObjectHandle& H,const FTransform& V,bool Teleport,das::Context* C){auto* S=ScriptScene(H,C);SceneMutation(S,C);if(!MethodMathValid(V))C->throw_error("invalid scene transform");S->SetWorldTransform(V,false,nullptr,Teleport?ETeleportType::TeleportPhysics:ETeleportType::None);}
    bool SceneAttach(const FDasObjectHandle& H,const FDasObjectHandle& Parent,const char* Socket,const char* Rule,bool Weld,das::Context* C){auto* Child=ScriptScene(H,C);auto* P=ScriptScene(Parent,C);SceneMutation(Child,C);SceneMutation(P,C);if(Child==P || P->IsAttachedTo(Child))C->throw_error("scene attachment would form a cycle");const FString SocketText=Socket?UTF8_TO_TCHAR(Socket):TEXT("");if(SocketText.Len()>=NAME_SIZE)C->throw_error("socket name too long");const FName SocketName=SocketText.IsEmpty()?NAME_None:FName(*SocketText);if(SocketName!=NAME_None && !P->DoesSocketExist(SocketName))C->throw_error("unknown parent socket");const FString R=Rule?UTF8_TO_TCHAR(Rule):TEXT("");FAttachmentTransformRules Rules=FAttachmentTransformRules::KeepRelativeTransform;if(R==TEXT("KeepRelative")){}else if(R==TEXT("KeepWorld"))Rules=FAttachmentTransformRules::KeepWorldTransform;else if(R==TEXT("SnapIncludingScale"))Rules=FAttachmentTransformRules::SnapToTargetIncludingScale;else if(R==TEXT("SnapNotIncludingScale"))Rules=FAttachmentTransformRules::SnapToTargetNotIncludingScale;else C->throw_error("unknown scene attachment rule");Rules.bWeldSimulatedBodies=Weld;return Child->AttachToComponent(P,Rules,SocketName);}
    void SceneDetach(const FDasObjectHandle& H,const char* Rule,das::Context* C){auto* S=ScriptScene(H,C);SceneMutation(S,C);const FString R=Rule?UTF8_TO_TCHAR(Rule):TEXT("");if(R==TEXT("KeepWorld"))S->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld,false));else if(R==TEXT("KeepRelative"))S->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepRelative,false));else C->throw_error("detach rule must be KeepWorld or KeepRelative");}
    FDasObjectHandle SceneCreate(const FDasObjectHandle& H,const char* Name,das::Context* C){auto* Self=ResolveWorldContextObject(C);if(ConstructingObject || ActiveContext->bCollectingSubscriptions || ActiveContext->bEnding)C->throw_error("cannot create scene components during bind_events/EndPlay");auto* Actor=Cast<AActor>(ResolveObject(H,C));if(!Actor || Actor->IsTemplate() || !Actor->GetWorld() || (!Actor->GetWorld()->IsGameWorld() && !ConstructingObject) || Actor->GetWorld()!=Self->GetWorld())C->throw_error("scene creation requires live game-world Actor in owner's world");const FString N=Name?UTF8_TO_TCHAR(Name):TEXT("");if(N.IsEmpty() || N.Len()>=NAME_SIZE || N.Equals(TEXT("None"),ESearchCase::IgnoreCase))C->throw_error("scene component name is required");for(TCHAR Ch:N)if(!FChar::IsAlnum(Ch) && Ch!='_')C->throw_error("scene component name supports letters/digits/underscore");if(FindObject<UObject>(Actor,*N))C->throw_error("scene component name already exists");for(auto I=CreatedScriptScenes.CreateIterator();I;++I)if(!I.Key().IsValid())I.RemoveCurrent();if(CreatedScriptScenes.Num()>=1024)C->throw_error("created scene component tracking limit");TStrongObjectPtr<AActor> Keep(Actor);auto* Component=NewObject<USceneComponent>(Actor,*N,RF_Transient);const bool Root=Actor->GetRootComponent()==nullptr;Actor->AddInstanceComponent(Component);if(Root)Actor->SetRootComponent(Component);else Component->SetupAttachment(Actor->GetRootComponent());Component->RegisterComponent();if(!Component->IsRegistered()){if(Root)Actor->SetRootComponent(nullptr);Actor->RemoveInstanceComponent(Component);Component->DestroyComponent();C->throw_error("scene component registration failed");}CreatedScriptScenes.Add(TWeakObjectPtr<UActorComponent>(Component),TWeakObjectPtr<UObject>(Self));return MakeObjectHandle(Component,USceneComponent::StaticClass(),C);}
    FDasObjectHandle SceneGet(const FDasObjectHandle& H,const char* Name,das::Context* C)
    {
        auto* Self=ResolveWorldContextObject(C);
        auto* Actor=Cast<AActor>(ResolveObject(H,C));
        if(!Actor || Actor->IsActorBeingDestroyed() || Actor->IsTemplate() || !Actor->GetWorld() || (!Actor->GetWorld()->IsGameWorld() && !ConstructingObject) || Actor->GetWorld()!=Self->GetWorld())C->throw_error("scene lookup requires live game-world Actor in owner's world");
        const FString N=Name?UTF8_TO_TCHAR(Name):TEXT("");
        if(N.IsEmpty() || N.Len()>=NAME_SIZE || N.Equals(TEXT("None"),ESearchCase::IgnoreCase))C->throw_error("scene component name is required");
        for(TCHAR Ch:N)if(!FChar::IsAlnum(Ch) && Ch!='_')C->throw_error("scene component name supports letters/digits/underscore");
        USceneComponent* Found=nullptr;
        for(auto* Component:Actor->GetComponents())
            if(Component && Component->GetFName()==FName(*N))
            {
                Found=Cast<USceneComponent>(Component);
                if(!Found || Found->IsBeingDestroyed())C->throw_error("named component is not a live SceneComponent");
                break;
            }
        return MakeObjectHandle(Found,USceneComponent::StaticClass(),C);
    }
    FDasObjectHandle SceneGetOrCreate(const FDasObjectHandle& H,const char* Name,das::Context* C)
    {
        auto Found=SceneGet(H,Name,C);
        if(TryObject(Found,C))return Found;
        return SceneCreate(H,Name,C);
    }
    FDasObjectHandle ComponentOperation(const FDasObjectHandle& H,UClass* Class,const char* Name,int Mode,const char* AuthoredId,das::Context* C)
    {
        auto* Self=ResolveWorldContextObject(C);
        auto* Actor=Cast<AActor>(ResolveObject(H,C));
        if(!Actor || Actor->IsTemplate() || Actor->IsActorBeingDestroyed() || !Actor->GetWorld() || (!Actor->GetWorld()->IsGameWorld() && !ConstructingObject) || Actor->GetWorld()!=Self->GetWorld())C->throw_error("component operation requires live Actor in owner's game world");
        const bool Authored=AuthoredId && *AuthoredId;
        if(!Class || !Class->IsChildOf(UActorComponent::StaticClass()) || Class->HasAnyClassFlags(CLASS_Abstract|CLASS_Deprecated|CLASS_NewerVersionExists))C->throw_error("expected loaded concrete ActorComponent class");
        if(Authored){
            const auto* Defaults=Cast<UDasScriptComponent>(Class->GetDefaultObject());
            if(!Defaults || Defaults->AuthoredTypeId!=UTF8_TO_TCHAR(AuthoredId))C->throw_error("authored component class identity mismatch");
        }
        const FString N=Name?UTF8_TO_TCHAR(Name):TEXT("");
        if(N.IsEmpty() || N.Len()>=NAME_SIZE || N.Equals(TEXT("None"),ESearchCase::IgnoreCase))C->throw_error("component name is required");
        for(TCHAR Ch:N)if(!FChar::IsAlnum(Ch) && Ch!='_')C->throw_error("component name supports letters/digits/underscore");
        if(Mode<0 || Mode>2)C->throw_error("invalid component operation");
        if(Mode!=1)for(auto* Component:Actor->GetComponents())if(Component && Component->GetFName()==FName(*N)){
            if(Component->IsBeingDestroyed() || !Component->IsA(Class))C->throw_error("named component has incompatible type or is being destroyed");
            return MakeObjectHandle(Component,Class,C);
        }
        if(Mode==0)return MakeObjectHandle(nullptr,Class,C);
        if(ConstructingObject || ActiveContext->bCollectingSubscriptions || ActiveContext->bEnding)C->throw_error("cannot create components during bind_events/EndPlay");
        if(FindObject<UObject>(Actor,*N))C->throw_error("component name already exists");
        for(auto I=CreatedScriptScenes.CreateIterator();I;++I)if(!I.Key().IsValid())I.RemoveCurrent();
        if(CreatedScriptScenes.Num()>=1024)C->throw_error("created component tracking limit");
        if(ComponentLifecycleDepth>=16)C->throw_error("component lifecycle nesting limit");
        TStrongObjectPtr<AActor> Keep(Actor);
        auto* Component=NewObject<UActorComponent>(Actor,Class,*N,RF_Transient);
        auto* Scene=Cast<USceneComponent>(Component);
        const bool Root=Scene && !Actor->GetRootComponent();
        Actor->AddInstanceComponent(Component);
        if(Scene){if(Root)Actor->SetRootComponent(Scene);else Scene->SetupAttachment(Actor->GetRootComponent());}
        TGuardValue<UDasScriptComponent*> LifecycleTarget(ComponentLifecycleTarget,Cast<UDasScriptComponent>(Component));
        TGuardValue<int32> LifecycleDepth(ComponentLifecycleDepth,ComponentLifecycleDepth+1);
        Component->RegisterComponent();
        if(!IsValid(Component) || Component->IsBeingDestroyed() || !Component->IsRegistered()){
            if(Root)Actor->SetRootComponent(nullptr);
            Actor->RemoveInstanceComponent(Component);Component->DestroyComponent();
            C->throw_error("component registration failed");
        }
        CreatedScriptScenes.Add(TWeakObjectPtr<UActorComponent>(Component),TWeakObjectPtr<UObject>(Self));
        return MakeObjectHandle(Component,Class,C);
    }
    UClass* CheckedClass(UClass* Class,const char* Base,const char* Id,das::Context* C)
    {
        const FString B=Base?UTF8_TO_TCHAR(Base):TEXT("");
        auto* Parent=FindObject<UClass>(nullptr,*B);
        if(!Parent || !IsValid(Class) || !Class->IsChildOf(Parent) || Class->HasAnyClassFlags(CLASS_Deprecated|CLASS_NewerVersionExists))C->throw_error("class reference is missing, abstract, obsolete or has incompatible base");
        if(Id && *Id){
            const auto* Defaults=Class->GetDefaultObject();
            const FString Actual=DasHostIdentity(Defaults);
            if(Actual!=UTF8_TO_TCHAR(Id))C->throw_error("class reference authored identity mismatch");
        }
        return Class;
    }
    FDasClassHandle ClassLoaded(const char* Path,const char* Base,const char* Id,das::Context* C)
    {
        RequireObjectContext(C);
        const FString P=Path?UTF8_TO_TCHAR(Path):TEXT("");
        if(P.IsEmpty() || P.Len()>4096)C->throw_error("class path is required");
        auto* Class=CheckedClass(FindObject<UClass>(nullptr,*P),Base,Id,C);
        return {MakeObjectHandle(Class,UClass::StaticClass(),C).Token};
    }
    FDasClassHandle CheckTypedClass(FDasClassHandle H,const char* Base,das::Context* C){RequireObjectContext(C);if(H.Token)CheckedClass(Cast<UClass>(ResolveObject(FDasObjectHandle{H.Token},C)),Base,"",C);return H;}
    bool ClassValid(FDasClassHandle H,das::Context* C)
    {
        auto* Class=Cast<UClass>(TryObject(FDasObjectHandle{H.Token},C));
        return Class && !Class->HasAnyClassFlags(CLASS_Deprecated|CLASS_NewerVersionExists);
    }
    char* ClassPath(FDasClassHandle H,das::Context* C)
    {
        auto* Class=Cast<UClass>(ResolveObject(FDasObjectHandle{H.Token},C));
        if(!Class)C->throw_error("expected class reference");
        return CopyDSLString(Class->GetPathName(),C);
    }
    FDasClassHandle GetSelfClass(const char* Name,das::Context* C)
    {
        auto* Self=ResolveSelf(C);auto* P=ResolveProperty<FClassProperty>(Self,Name,false,C);
        auto* Class=ReadPropertyClass(P,Self,C);
        if(!Class)return {};
        CheckedClass(Class,TCHAR_TO_UTF8(*P->MetaClass->GetPathName()),"",C);
        return {MakeObjectHandle(Class,UClass::StaticClass(),C).Token};
    }
    void SetSelfClass(const char* Name,FDasClassHandle Value,das::Context* C)
    {
        auto* Self=ResolveSelf(C);if(ActiveContext->bCollectingSubscriptions)C->throw_error("class writes are forbidden in bind_events");
        auto* P=ResolveProperty<FClassProperty>(Self,Name,true,C);
        UClass* Class=nullptr;
        if(Value.Token)Class=CheckedClass(Cast<UClass>(ResolveObject(FDasObjectHandle{Value.Token},C)),TCHAR_TO_UTF8(*P->MetaClass->GetPathName()),"",C);
        WritePropertyClass(P,Self,Class,C);
    }
    FDasClassHandle AuthoredGetclass(const FDasObjectHandle& H,const char* Id,const char* Name,das::Context* C)
    {
        auto* Object=ResolveAuthoredObject(H,Id,C);auto* P=ResolveProperty<FClassProperty>(Object,Name,false,C);
        auto* Class=ReadPropertyClass(P,Object,C);if(!Class)return {};
        CheckedClass(Class,TCHAR_TO_UTF8(*P->MetaClass->GetPathName()),"",C);
        return {MakeObjectHandle(Class,UClass::StaticClass(),C).Token};
    }
    void AuthoredSetclass(const FDasObjectHandle& H,const char* Id,const char* Name,FDasClassHandle V,das::Context* C)
    {
        auto* Object=ResolveAuthoredObject(H,Id,C);if(ActiveContext->bCollectingSubscriptions)C->throw_error("class writes are forbidden in bind_events");
        auto* P=ResolveProperty<FClassProperty>(Object,Name,true,C);UClass* Class=nullptr;
        if(V.Token)Class=CheckedClass(Cast<UClass>(ResolveObject(FDasObjectHandle{V.Token},C)),TCHAR_TO_UTF8(*P->MetaClass->GetPathName()),"",C);
        WritePropertyClass(P,Object,Class,C);
    }
    FDasObjectHandle NativeComponentOperation(const FDasObjectHandle& H,const char* Path,const char* Name,int Mode,const char* Id,das::Context* C)
    {
        const FString P=Path?UTF8_TO_TCHAR(Path):TEXT("");
        if((!Id || !*Id) && !P.StartsWith(TEXT("/Script/")))C->throw_error("expected native component class");
        return ComponentOperation(H,FindObject<UClass>(nullptr,*P),Name,Mode,Id,C);
    }
    FDasObjectHandle ClassComponentOperation(const FDasObjectHandle& H,FDasClassHandle Class,const char* Name,int Mode,const char* Base,const char* Id,das::Context* C)
    {
        auto* Actual=CheckedClass(Cast<UClass>(ResolveObject(FDasObjectHandle{Class.Token},C)),Base,Id,C);
        TStrongObjectPtr<UClass> Keep(Actual);
        return ComponentOperation(H,Actual,Name,Mode,Id,C);
    }
    FDasObjectHandle PathComponentOperation(const FDasObjectHandle& H,const char* Path,const char* Name,int Mode,const char* Base,const char* Id,das::Context* C)
    {
        const FString P=Path?UTF8_TO_TCHAR(Path):TEXT("");
        auto* Class=CheckedClass(FindObject<UClass>(nullptr,*P),Base,Id,C);
        TStrongObjectPtr<UClass> Keep(Class);
        return ComponentOperation(H,Class,Name,Mode,Id,C);
    }
    #include "DasActorSpawn.inl"
    #include "DasObjectQueries.inl"
    #include "DasCharacterControl.inl"
    #include "DasNativeGameplayRuntime.inl"
    FDasObjectHandle AuthoredComponentGet(const FDasObjectHandle& H,const char* Id,const char* Name,das::Context* C)
    {
        auto* Self=ResolveWorldContextObject(C);auto* Actor=Cast<AActor>(ResolveObject(H,C));
        if(!Actor || Actor->IsTemplate() || Actor->IsActorBeingDestroyed() || !Actor->GetWorld() || (!Actor->GetWorld()->IsGameWorld() && !ConstructingObject) || Actor->GetWorld()!=Self->GetWorld())C->throw_error("component lookup requires live Actor in owner's game world");
        const FString N=Name?UTF8_TO_TCHAR(Name):TEXT("");
        if(N.IsEmpty() || N.Len()>=NAME_SIZE || N.Equals(TEXT("None"),ESearchCase::IgnoreCase))C->throw_error("component name is required");
        for(TCHAR Ch:N)if(!FChar::IsAlnum(Ch) && Ch!='_')C->throw_error("component name supports letters/digits/underscore");
        for(auto* Component:Actor->GetComponents())if(Component && Component->GetFName()==FName(*N)){
            if(Component->IsBeingDestroyed() || !Component->IsA<UDasScriptComponent>())C->throw_error("named component is not a live authored component");
            auto Result=MakeObjectHandle(Component,UDasScriptComponent::StaticClass(),C);
            ResolveAuthoredObject(Result,Id,C);return Result;
        }
        return {};
    }
    bool NativeComponentDestroy(const FDasObjectHandle& H,das::Context* C)
    {
        auto* Component=Cast<UActorComponent>(ResolveObject(H,C));auto* Self=ResolveWorldContextObject(C);
        if(!Component || Component->IsBeingDestroyed() || Component->IsTemplate() || !Component->GetWorld() || Component->GetWorld()!=Self->GetWorld() || ConstructingObject || ActiveContext->bCollectingSubscriptions)C->throw_error("invalid component destruction context");
        const auto* Creator=CreatedScriptScenes.Find(TWeakObjectPtr<UActorComponent>(Component));
        if(!Creator || Creator->Get()!=Self)C->throw_error("only components created by this script owner may be destroyed");
        auto* Scene=Cast<USceneComponent>(Component);
        if(Scene && Scene->GetAttachChildren().Num())C->throw_error("detach children before destroying component");
        auto* Actor=Component->GetOwner();
        CreatedScriptScenes.Remove(TWeakObjectPtr<UActorComponent>(Component));
        if(Actor){if(Scene && Actor->GetRootComponent()==Scene)Actor->SetRootComponent(nullptr);Actor->RemoveInstanceComponent(Component);}
        TGuardValue<UDasScriptComponent*> LifecycleTarget(ComponentLifecycleTarget,Cast<UDasScriptComponent>(Component));
        Component->DestroyComponent();return true;
    }
    bool SceneDestroy(const FDasObjectHandle& H,das::Context* C){if(ConstructingObject)C->throw_error("component destruction is forbidden during ConstructionScript");auto* S=ScriptScene(H,C);SceneMutation(S,C);const auto* Creator=CreatedScriptScenes.Find(TWeakObjectPtr<UActorComponent>(S));if(!Creator || Creator->Get()!=ResolveWorldContextObject(C))C->throw_error("only scene components created by this script owner may be destroyed");if(S->GetAttachChildren().Num()!=0)C->throw_error("detach children before destroying created scene component");auto* Actor=S->GetOwner();CreatedScriptScenes.Remove(TWeakObjectPtr<UActorComponent>(S));if(Actor){if(Actor->GetRootComponent()==S)Actor->SetRootComponent(nullptr);Actor->RemoveInstanceComponent(S);}S->DestroyComponent();return true;}

    FMulticastDelegateProperty* ScalarEventProperty(UObject* Source,FName Name,EDasMethodSignature Kind){
        auto* P=FindFProperty<FMulticastDelegateProperty>(Source->GetClass(),Name);
        if(!P || !P->HasAnyPropertyFlags(CPF_BlueprintAssignable) || P->ArrayDim!=1 || !P->SignatureFunction)return nullptr;
        auto* Sig=P->SignatureFunction.Get();
        if(Kind==EDasMethodSignature::Void)return Sig->NumParms==0?P:nullptr;
        if(Sig->NumParms>=2 && Sig->NumParms<=16 && !Sig->GetReturnProperty())return Kind==EDasMethodSignature::Struct?P:nullptr;
        if(Sig->NumParms!=1)return nullptr;
        auto* Arg=ScalarEventParameter(P);const auto* Type=DasCallbackTypeForProperty(Arg);
        if(!Type || Type->Signature!=Kind || Arg->HasAnyPropertyFlags(CPF_ReturnParm))return nullptr;
        const bool Ref=Arg->HasAnyPropertyFlags(CPF_OutParm|CPF_ReferenceParm);
        if(Type->bDynamic)return !Ref || Arg->HasAnyPropertyFlags(CPF_ConstParm)?P:nullptr;
        return Ref?(Arg->HasAnyPropertyFlags(CPF_ConstParm) && !Type->ConstRefProxy.IsNone()?P:nullptr):(!Type->ValueProxy.IsNone()?P:nullptr);
    }

    void SubscribeObjectScalar(const FDasObjectHandle& Handle, const char* Event, const char* Handler, das::Context* Context,EDasMethodSignature Kind,UScriptStruct* ExpectedCarrier=nullptr)
    {
        RequireObjectContext(Context);
        if (!ActiveContext->bCollectingSubscriptions) Context->throw_error("subscriptions may only be declared in bind_events");
        if (!Event || !*Event || !Handler || !*Handler) Context->throw_error("event and handler are required");
        UObject* Source = ResolveObject(Handle, Context);
        if (!IsValid(Source) || (Source->IsA<AActor>() && CastChecked<AActor>(Source)->IsActorBeingDestroyed()))
            Context->throw_error("subscription source is missing or expired");
        const FString EventName(UTF8_TO_TCHAR(Event));
        if (EventName.Len() >= NAME_SIZE) Context->throw_error("event name is too long");
        const FName Name(*EventName);
        auto* EventProperty=ScalarEventProperty(Source,Name,Kind);FString PayloadPath;if(ExpectedCarrier){auto* Actual=EventProperty?(CastField<FStructProperty>(ScalarEventParameter(EventProperty))?CastFieldChecked<FStructProperty>(ScalarEventParameter(EventProperty))->Struct.Get():DasContainerCarrierForProperty(ScalarEventParameter(EventProperty))):nullptr;if(!Actual || Actual!=ExpectedCarrier)Context->throw_error("event array element type mismatch");PayloadPath=ExpectedCarrier->GetPathName();}
        if (!EventProperty) Context->throw_error(Kind==EDasMethodSignature::Int?"expected BlueprintAssignable multicast event with one int32 value":"BlueprintAssignable event signature differs from callback type");
        if(ExpectedCarrier){const auto* Capability=DasCallbackTypeForProperty(ScalarEventParameter(EventProperty));if(Capability && Capability->bDynamic){FString Error;TStrongObjectPtr<UDasIntDelegateProxy> Check(NewObject<UDasIntDelegateProxy>());if(Check->ConfigureDynamic(EventProperty->SignatureFunction,PayloadPath,Kind,Error).IsNone())Context->throw_error(TCHAR_TO_UTF8(*Error));}}
        for (const auto& Existing : ActiveContext->Subscriptions)
            if (Existing.Source.Get() == Source && Existing.Event == Name && Existing.Handler == UTF8_TO_TCHAR(Handler))
                Context->throw_error("duplicate subscription declaration");
        if(ActiveContext->Subscriptions.Num()>=256)Context->throw_error("subscription declaration limit exceeded");
        ActiveContext->Subscriptions.Add({Source, Name, UTF8_TO_TCHAR(Handler),Kind,PayloadPath});
    }

    void SubscribeObjectValue(const FDasObjectHandle& Handle,const char* Event,const char* Handler,const char* Path,das::Context* Context,EDasMethodSignature Kind){
        RequireObjectContext(Context);const FString Expected=Path?UTF8_TO_TCHAR(Path):TEXT("");FString Error;
        if(!ValidateDasCallbackTypePath(Kind==EDasMethodSignature::Class?TEXT("class"):Kind==EDasMethodSignature::Enum?TEXT("enum"):TEXT("struct"),Expected,Error))Context->throw_error(TCHAR_TO_UTF8(*Error));
        SubscribeObjectScalar(Handle,Event,Handler,Context,Kind);
        auto& Declaration=ActiveContext->Subscriptions.Last();auto* P=ScalarEventParameter(ScalarEventProperty(ResolveObject(Handle,Context),Declaration.Event,Kind));
        const FString Actual=FindDasCallbackFrame(Expected)?Expected:Kind==EDasMethodSignature::Class?CastFieldChecked<FClassProperty>(P)->MetaClass->GetPathName():Kind==EDasMethodSignature::Enum?DasEnumPropertyType(P)->GetPathName():DasCallbackSnapshotForProperty(P)->GetPathName();
        if(Actual!=Expected && !(Kind==EDasMethodSignature::Class && FindObject<UClass>(nullptr,*Actual)->IsChildOf(FindObject<UClass>(nullptr,*Expected)))){ActiveContext->Subscriptions.Pop();Context->throw_error("Event and callback require the same concrete enum/struct type");}
        TStrongObjectPtr<UDasIntDelegateProxy> ReceiverCheck(NewObject<UDasIntDelegateProxy>());
        auto* Signature=ScalarEventProperty(ResolveObject(Handle,Context),Declaration.Event,Kind)->SignatureFunction.Get();
        if(ReceiverCheck->ConfigureDynamic(Signature,Expected,Kind,Error).IsNone()){ActiveContext->Subscriptions.Pop();Context->throw_error(TCHAR_TO_UTF8(*Error));}
        Declaration.PayloadPath=Expected;
    }
    void SubscribeObjectClass(const FDasObjectHandle& H,const char* E,const char* F,const char* P,das::Context* C){SubscribeObjectValue(H,E,F,P,C,EDasMethodSignature::Class);}
    void SubscribeObjectEnum(const FDasObjectHandle& H,const char* E,const char* F,const char* P,das::Context* C){SubscribeObjectValue(H,E,F,P,C,EDasMethodSignature::Enum);}
    void SubscribeObjectStruct(const FDasObjectHandle& H,const char* E,const char* F,const char* P,das::Context* C){SubscribeObjectValue(H,E,F,P,C,EDasMethodSignature::Struct);}
    void SubscribeObjectByte(const FDasObjectHandle& H,const char* Event,const char* Handler,das::Context* C){SubscribeObjectScalar(H,Event,Handler,C,EDasMethodSignature::Byte);}
    void SubscribeObjectArrayFloat(const FDasObjectHandle& H,const char* Event,const char* Handler,das::Context* C){SubscribeObjectScalar(H,Event,Handler,C,EDasMethodSignature::Struct,FDasArrayFloatCarrier::StaticStruct());}
    void SubscribeObjectArrayBool(const FDasObjectHandle& H,const char* Event,const char* Handler,das::Context* C){SubscribeObjectScalar(H,Event,Handler,C,EDasMethodSignature::Struct,FDasArrayBoolCarrier::StaticStruct());}
    void SubscribeObjectArrayName(const FDasObjectHandle& H,const char* Event,const char* Handler,das::Context* C){SubscribeObjectScalar(H,Event,Handler,C,EDasMethodSignature::Struct,FDasArrayNameCarrier::StaticStruct());}
    void SubscribeObjectArrayByte(const FDasObjectHandle& H,const char* Event,const char* Handler,das::Context* C){SubscribeObjectScalar(H,Event,Handler,C,EDasMethodSignature::Struct,FDasArrayByteCarrier::StaticStruct());}
    void SubscribeObjectText(const FDasObjectHandle& H,const char* Event,const char* Handler,das::Context* C){SubscribeObjectScalar(H,Event,Handler,C,EDasMethodSignature::Text);}
    void SubscribeObjectArrayText(const FDasObjectHandle& H,const char* Event,const char* Handler,das::Context* C){SubscribeObjectScalar(H,Event,Handler,C,EDasMethodSignature::Struct,FDasArrayTextCarrier::StaticStruct());}
    void SubscribeObjectInt(const FDasObjectHandle& H,const char* Event,const char* Handler,das::Context* C){SubscribeObjectScalar(H,Event,Handler,C,EDasMethodSignature::Int);}
    void SubscribeObjectFloat(const FDasObjectHandle& H,const char* Event,const char* Handler,das::Context* C){SubscribeObjectScalar(H,Event,Handler,C,EDasMethodSignature::Float);}
    void SubscribeObjectBool(const FDasObjectHandle& H,const char* Event,const char* Handler,das::Context* C){SubscribeObjectScalar(H,Event,Handler,C,EDasMethodSignature::Bool);}
    void SubscribeObjectString(const FDasObjectHandle& H,const char* Event,const char* Handler,das::Context* C){SubscribeObjectScalar(H,Event,Handler,C,EDasMethodSignature::String);}
    void SubscribeObjectVoid(const FDasObjectHandle& H,const char* Event,const char* Handler,das::Context* C){SubscribeObjectScalar(H,Event,Handler,C,EDasMethodSignature::Void);}

    void SubscribeObjectDouble(const FDasObjectHandle& H,const char* Event,const char* Handler,das::Context* C){SubscribeObjectScalar(H,Event,Handler,C,EDasMethodSignature::Double);}
    void SubscribeObjectObject(const FDasObjectHandle& H,const char* Event,const char* Handler,das::Context* C){SubscribeObjectScalar(H,Event,Handler,C,EDasMethodSignature::Object);ActiveContext->Subscriptions.Last().PayloadPath=TEXT("/Script/CoreUObject.Object");}
    void SubscribeObjectTyped(const FDasObjectHandle& H,const char* Event,const char* Handler,const char* Path,das::Context* C){FString Error;const FString Expected=Path?UTF8_TO_TCHAR(Path):TEXT("");if(!ValidateDasCallbackTypePath(TEXT("object"),Expected,Error))C->throw_error(TCHAR_TO_UTF8(*Error));SubscribeObjectScalar(H,Event,Handler,C,EDasMethodSignature::Object);auto& D=ActiveContext->Subscriptions.Last();auto* Property=ScalarEventProperty(ResolveObject(H,C),D.Event,D.Kind);auto* Actual=DasReferencePropertyClass(ScalarEventParameter(Property));TStrongObjectPtr<UDasIntDelegateProxy> Probe(NewObject<UDasIntDelegateProxy>());if(!Actual || Probe->ConfigureDynamic(Property->SignatureFunction,Actual->GetPathName(),D.Kind,Error).IsNone()){ActiveContext->Subscriptions.Pop();C->throw_error(TCHAR_TO_UTF8(*Error));}D.PayloadPath=Expected;}
    void SubscribeObjectVector(const FDasObjectHandle& H,const char* Event,const char* Handler,das::Context* C){SubscribeObjectScalar(H,Event,Handler,C,EDasMethodSignature::Vector);}

    void SubscribeObjectRotator(const FDasObjectHandle& H,const char* Event,const char* Handler,das::Context* C){SubscribeObjectScalar(H,Event,Handler,C,EDasMethodSignature::Rotator);}
    void SubscribeObjectTransform(const FDasObjectHandle& H,const char* Event,const char* Handler,das::Context* C){SubscribeObjectScalar(H,Event,Handler,C,EDasMethodSignature::Transform);}

    void SubscribeObjectName(const FDasObjectHandle& H,const char* Event,const char* Handler,das::Context* C){SubscribeObjectScalar(H,Event,Handler,C,EDasMethodSignature::Name);}
    void SubscribeObjectArrayInt(const FDasObjectHandle& H,const char* Event,const char* Handler,das::Context* C){SubscribeObjectScalar(H,Event,Handler,C,EDasMethodSignature::Struct,FDasArrayIntCarrier::StaticStruct());}
    void SubscribeObjectArrayDouble(const FDasObjectHandle& H,const char* Event,const char* Handler,das::Context* C){SubscribeObjectScalar(H,Event,Handler,C,EDasMethodSignature::Struct,FDasArrayDoubleCarrier::StaticStruct());}
    void SubscribeObjectArrayString(const FDasObjectHandle& H,const char* Event,const char* Handler,das::Context* C){SubscribeObjectScalar(H,Event,Handler,C,EDasMethodSignature::Struct,FDasArrayStringCarrier::StaticStruct());}

    void SubscribeObjectVector2d(const FDasObjectHandle& H,const char* E,const char* F,das::Context* C){SubscribeObjectScalar(H,E,F,C,EDasMethodSignature::Struct,TBaseStructure<FVector2D>::Get());}
    FName EventProxyFunction(EDasMethodSignature K,FMulticastDelegateProperty* Property){
        auto* Arg=ScalarEventParameter(Property);
        const auto* Type=K==EDasMethodSignature::Void?FindDasCallbackType(TEXT("void")):DasCallbackTypeForProperty(Arg);
        if(!Type || Type->Signature!=K)return NAME_None;
        return Arg && Arg->HasAnyPropertyFlags(CPF_OutParm|CPF_ReferenceParm)?Type->ConstRefProxy:Type->ValueProxy;
    }
    bool VerifyEventHandler(das::SimFunction* F,das::ModuleGroup& G,EDasMethodSignature K){if(!F)return false;switch(K){case EDasMethodSignature::Class:return das::verifyCall<void,FDasClassHandle>(F->debugInfo,G);case EDasMethodSignature::Enum:return das::verifyCall<void,FDasEnumValue>(F->debugInfo,G);case EDasMethodSignature::Byte:return das::verifyCall<void,uint8_t>(F->debugInfo,G);case EDasMethodSignature::Text:return das::verifyCall<void,FDasTextValue>(F->debugInfo,G);case EDasMethodSignature::Name:return das::verifyCall<void,FDasNameValue>(F->debugInfo,G);case EDasMethodSignature::Struct:return das::verifyCall<void,FDasStructValue>(F->debugInfo,G);case EDasMethodSignature::Double:return das::verifyCall<void,double>(F->debugInfo,G);case EDasMethodSignature::Object:return das::verifyCall<void,FDasObjectHandle>(F->debugInfo,G);case EDasMethodSignature::Rotator:return das::verifyCall<void,FRotator>(F->debugInfo,G);case EDasMethodSignature::Transform:return das::verifyCall<void,FTransform>(F->debugInfo,G);case EDasMethodSignature::Vector:return das::verifyCall<void,FVector>(F->debugInfo,G);case EDasMethodSignature::Float:return das::verifyCall<void,float>(F->debugInfo,G);case EDasMethodSignature::Bool:return das::verifyCall<void,bool>(F->debugInfo,G);case EDasMethodSignature::String:return das::verifyCall<void,char*>(F->debugInfo,G);case EDasMethodSignature::Void:return das::verifyCall<void>(F->debugInfo,G);default:return das::verifyCall<void,int32>(F->debugInfo,G);}}

    void SubscribeInt(const char* SourceKind, const char* Event, const char* Handler, das::Context* Context)
    {
        RequireObjectContext(Context);
        if (!SourceKind) Context->throw_error("subscription source is required");
        UObject* Source = ResolveSelf(Context);
        const FString Kind(UTF8_TO_TCHAR(SourceKind));
        if (Kind == TEXT("peer"))
        {
            auto* Actor = Cast<ADasProbeActor>(Source);
            Source = Actor ? Actor->Peer.Get() : nullptr;
        }
        else if (Kind != TEXT("self")) Context->throw_error("subscription source must be self or peer");
        SubscribeObjectInt(MakeObjectHandle(Source, UObject::StaticClass(), Context), Event, Handler, Context);
    }

    void EmitSignal(int32 Value, das::Context* Context)
    {
        if (ConstructingObject || (ActiveContext && ActiveContext->bCollectingSubscriptions)) Context->throw_error("event emission is forbidden in bind_events/ConstructionScript");
        auto* Actor = Cast<ADasProbeActor>(ResolveSelf(Context));
        if (!Actor) Context->throw_error("signal emission requires a DasProbeActor");
        Actor->EmitDasSignal(Value);
    }

    void ScriptEnhancedAddContext(const FDasObjectHandle&,const FDasObjectHandle&,int32,das::Context*);
    bool ScriptEnhancedRemoveContext(const FDasObjectHandle&,const FDasObjectHandle&,das::Context*);
    bool ScriptEnhancedHasContext(const FDasObjectHandle&,const FDasObjectHandle&,das::Context*);
    int32 ScriptEnhancedClearContexts(das::Context*);
    void ScriptEnhancedBind(const FDasObjectHandle&,const char*,const char*,das::Context*);
    bool ScriptEnhancedUnbind(const FDasObjectHandle&,const char*,das::Context*);
    bool ScriptEnhancedAvailable(das::Context*);
    void ScriptInputAction(const char*,const char*,const char*,bool,bool,das::Context*);
    void ScriptInputAxis(const char*,const char*,bool,bool,das::Context*);
    bool ScriptInputAvailable(das::Context*);
    bool ScriptInputUnbindAction(const char*,const char*,das::Context*);
    bool ScriptInputUnbindAxis(const char*,das::Context*);
    int32 ScriptInputUnbindAll(das::Context*);
    void ScriptAssetLoad(const char*,const char*,const char*,const char*,das::Context*);
    void ScriptAssetLoadGeneric(const char*,const char*,const char*,das::Context*);
    bool ScriptAssetCancel(const char*,das::Context*);
    int32 ScriptAssetCancelAll(das::Context*);
    bool ScriptAssetPending(const char*,das::Context*);
    void ScriptLatentDelay(const char*,float,const char*,das::Context*);
    FDasObjectHandle ScriptAISelf(das::Context*);
    bool ScriptAIMove(const char*,const FDasObjectHandle&,const FDasObjectHandle&,const FVector&,bool,const char*,float,bool,bool,das::Context*);
    bool ScriptAICancel(const char*,das::Context*);
    bool ScriptAIActive(const char*,das::Context*);
    bool ScriptEffectPlay(const char*,const FDasObjectHandle&,const FDasObjectHandle&,const FTransform&,const char*,float,float,das::Context*);
    bool ScriptEffectStop(const char*,das::Context*);
    int32 ScriptEffectStopAll(das::Context*);
    bool ScriptEffectActive(const char*,das::Context*);
    float ScriptMontagePlay(const char*,const FDasObjectHandle&,const FDasObjectHandle&,const char*,float,float,FDasNameValue,das::Context*);
    bool ScriptMontageStop(const char*,float,das::Context*);
    bool ScriptMontageCancel(const char*,das::Context*);
    bool ScriptMontagePlaying(const char*,das::Context*);
    bool ScriptLatentCancel(const char*,das::Context*);
    int32 ScriptLatentCancelAll(das::Context*);
    bool ScriptLatentPending(const char*,das::Context*);
    void ScriptTimerStart(const char*,float,const char*,bool,das::Context*);
    void ScriptTimerStartVector2d(const char*,float,const char*,bool,const FVector2D&,das::Context*);
    void ScriptTimerNextTickVector2d(const char*,const char*,const FVector2D&,das::Context*);
    void ScriptTimerStartArray(const char*,float,const char*,bool,const FDasStructValue&,das::Context*);
    void ScriptTimerStartClass(const char*,float,const char*,bool,FDasClassHandle,const char*,das::Context*);
    void ScriptTimerNextTickClass(const char*,const char*,FDasClassHandle,const char*,das::Context*);
    void ScriptTimerStartObject(const char*,float,const char*,bool,const FDasObjectHandle&,const char*,das::Context*);
    void ScriptTimerNextTickObject(const char*,const char*,const FDasObjectHandle&,const char*,das::Context*);
    void ScriptTimerStartEnum(const char*,float,const char*,bool,FDasEnumValue,das::Context*);
    void ScriptTimerNextTickEnum(const char*,const char*,FDasEnumValue,das::Context*);
    void ScriptTimerStartByte(const char* N,float Seconds,const char* H,bool Repeat,uint8 Value,das::Context* C);
    void ScriptTimerNextTickByte(const char* N,const char* H,uint8 Value,das::Context* C);
    void ScriptTimerStartText(const char* N,float Seconds,const char* H,bool Repeat,FDasTextValue Value,das::Context* C);
    void ScriptTimerNextTickText(const char* N,const char* H,FDasTextValue Value,das::Context* C);
    void ScriptTimerStartInt(const char*,float,const char*,bool,int32,das::Context*);
    void ScriptTimerStartFloat(const char*,float,const char*,bool,float,das::Context*);
    void ScriptTimerStartBool(const char*,float,const char*,bool,bool,das::Context*);
    void ScriptTimerStartDouble(const char*,float,const char*,bool,double,das::Context*);
    void ScriptTimerStartString(const char*,float,const char*,bool,const char*,das::Context*);
    void ScriptTimerStartName(const char*,float,const char*,bool,FDasNameValue,das::Context*);
    void ScriptTimerStartVector(const char*,float,const char*,bool,const FVector&,das::Context*);
    void ScriptTimerStartRotator(const char*,float,const char*,bool,const FRotator&,das::Context*);
    void ScriptTimerStartTransform(const char*,float,const char*,bool,const FTransform&,das::Context*);
    void ScriptTimerNextTick(const char*,const char*,das::Context*);
    void ScriptTimerNextTickArray(const char*,const char*,const FDasStructValue&,das::Context*);
    bool ScriptTimerPaused(const char*,das::Context*);
    float ScriptTimerElapsed(const char*,das::Context*);
    int32 ScriptTimerCancelAll(das::Context*);
    void ScriptTimerNextTickInt(const char*,const char*,int32,das::Context*);
    void ScriptTimerNextTickFloat(const char*,const char*,float,das::Context*);
    void ScriptTimerNextTickBool(const char*,const char*,bool,das::Context*);
    void ScriptTimerNextTickDouble(const char*,const char*,double,das::Context*);
    void ScriptTimerNextTickString(const char*,const char*,const char*,das::Context*);
    void ScriptTimerNextTickName(const char*,const char*,FDasNameValue,das::Context*);
    void ScriptTimerNextTickVector(const char*,const char*,const FVector&,das::Context*);
    void ScriptTimerNextTickRotator(const char*,const char*,const FRotator&,das::Context*);
    void ScriptTimerNextTickTransform(const char*,const char*,const FTransform&,das::Context*);
    bool ScriptTimerCancel(const char*,das::Context*);
    bool ScriptTimerPause(const char*,das::Context*);
    bool ScriptTimerResume(const char*,das::Context*);
    bool ScriptTimerActive(const char*,das::Context*);
    float ScriptTimerRemaining(const char*,das::Context*);

    struct FObjectHandleAnnotation : das::ManagedStructureAnnotation<FDasObjectHandle, false>
    {
        explicit FObjectHandleAnnotation(das::ModuleLibrary& Library)
            : ManagedStructureAnnotation("UObjectHandle", Library, "FDasObjectHandle") {}
        virtual bool isLocal() const override { return true; }
        virtual bool canCopy() const override { return true; }
        virtual bool canMove() const override { return true; }
        virtual bool hasNonTrivialCtor() const override { return false; }
        virtual bool isPod() const override { return true; }
        virtual bool canBePlacedInContainer() const override { return true; }
    };

    struct FClassHandleAnnotation:das::ManagedStructureAnnotation<FDasClassHandle,false>{
        explicit FClassHandleAnnotation(das::ModuleLibrary& L):ManagedStructureAnnotation("UEClassHandle",L,"FDasClassHandle"){}
        virtual das::TypeDeclPtr makeValueType()const override{return new das::TypeDecl(das::Type::tUInt64,das::LineInfo());}
        virtual das::SimNode* simulateClone(das::Context& C,const das::LineInfo& At,das::SimNode* Left,das::SimNode* Right)const override{return C.code->makeNode<das::SimNode_Set<FDasClassHandle>>(At,Left,Right);}
        virtual bool isRefType()const override{return false;}
        virtual bool isLocal()const override{return true;}
        virtual bool canCopy()const override{return true;}
        virtual bool canMove()const override{return true;}
        virtual bool hasNonTrivialCtor()const override{return false;}
        virtual bool isPod()const override{return true;}
        virtual bool canBePlacedInContainer()const override{return true;}
    };
    struct FDSLEnumAnnotation:das::ManagedStructureAnnotation<FDasEnumValue,false>{FDSLEnumAnnotation(das::ModuleLibrary& L):ManagedStructureAnnotation("UEEnumValue",L,"FDasEnumValue"){}virtual das::TypeDeclPtr makeValueType()const override{static_assert(sizeof(FDasEnumValue)==sizeof(uint64));return new das::TypeDecl(das::Type::tUInt64,das::LineInfo());}virtual das::SimNode* simulateClone(das::Context& C,const das::LineInfo& At,das::SimNode* Left,das::SimNode* Right)const override{return C.code->makeNode<das::SimNode_Set<FDasEnumValue>>(At,Left,Right);}virtual bool isRefType()const override{return false;}virtual bool isLocal()const override{return true;}virtual bool canCopy()const override{return true;}virtual bool canMove()const override{return true;}virtual bool hasNonTrivialCtor()const override{return false;}virtual bool isPod()const override{return true;}};
    struct FDSLNameAnnotation:das::ManagedStructureAnnotation<FDasNameValue,false>{
        FDSLNameAnnotation(das::ModuleLibrary& L):ManagedStructureAnnotation("UENameValue",L,"FDasNameValue"){}
        // Reference-to-value conversion during table cloning/calls copies the owned token.
        virtual das::TypeDeclPtr makeValueType() const override {static_assert(sizeof(FDasNameValue)==sizeof(uint64));return new das::TypeDecl(das::Type::tUInt64,das::LineInfo());}
        virtual das::SimNode* simulateClone(das::Context& C,const das::LineInfo& At,das::SimNode* Left,das::SimNode* Right) const override {return C.code->makeNode<das::SimNode_Set<FDasNameValue>>(At,Left,Right);}
        virtual bool isRefType() const override{return false;}virtual bool isLocal() const override{return true;}
        virtual bool canCopy() const override{return true;}virtual bool canMove() const override{return true;}
        virtual bool hasNonTrivialCtor() const override{return false;}virtual bool isPod() const override{return true;}
    };
    struct FDSLTextAnnotation:das::ManagedStructureAnnotation<FDasTextValue,false>{
        FDSLTextAnnotation(das::ModuleLibrary& L):ManagedStructureAnnotation("UETextValue",L,"FDasTextValue"){}
        // Reference-to-value conversion during table cloning/calls copies the owned token.
        virtual das::TypeDeclPtr makeValueType() const override {static_assert(sizeof(FDasTextValue)==sizeof(uint64));return new das::TypeDecl(das::Type::tUInt64,das::LineInfo());}
        virtual das::SimNode* simulateClone(das::Context& C,const das::LineInfo& At,das::SimNode* Left,das::SimNode* Right) const override {return C.code->makeNode<das::SimNode_Set<FDasTextValue>>(At,Left,Right);}
        virtual bool isRefType() const override{return false;}virtual bool isLocal() const override{return true;}
        virtual bool canCopy() const override{return true;}virtual bool canMove() const override{return true;}
        virtual bool hasNonTrivialCtor() const override{return false;}virtual bool isPod() const override{return true;}
    };
    template<class T> struct FMathValueAnnotation:das::ManagedStructureAnnotation<T,false>{
        FMathValueAnnotation(const char* Name,das::ModuleLibrary& L,const char* Cpp):das::ManagedStructureAnnotation<T,false>(Name,L,Cpp){}
        virtual bool isLocal() const override{return true;}virtual bool canCopy() const override{return true;}virtual bool canMove() const override{return true;}virtual bool hasNonTrivialCtor() const override{return false;}
    };
    struct FDSLVector2DAnnotation:FMathValueAnnotation<FVector2D>{FDSLVector2DAnnotation(das::ModuleLibrary& L):FMathValueAnnotation("UEVector2D",L,"FVector2D"){addField<DAS_BIND_MANAGED_FIELD(X)>("x");addField<DAS_BIND_MANAGED_FIELD(Y)>("y");}};
    struct FDSLVectorAnnotation:FMathValueAnnotation<FVector>{FDSLVectorAnnotation(das::ModuleLibrary& L):FMathValueAnnotation("UEVector",L,"FVector"){addField<DAS_BIND_MANAGED_FIELD(X)>("x");addField<DAS_BIND_MANAGED_FIELD(Y)>("y");addField<DAS_BIND_MANAGED_FIELD(Z)>("z");}};
    struct FDSLRotatorAnnotation:FMathValueAnnotation<FRotator>{FDSLRotatorAnnotation(das::ModuleLibrary& L):FMathValueAnnotation("UERotator",L,"FRotator"){addField<DAS_BIND_MANAGED_FIELD(Pitch)>("pitch");addField<DAS_BIND_MANAGED_FIELD(Yaw)>("yaw");addField<DAS_BIND_MANAGED_FIELD(Roll)>("roll");}};
    struct FTraceHitAnnotation:FMathValueAnnotation<FDasTraceHit>{
        FTraceHitAnnotation(das::ModuleLibrary& L):FMathValueAnnotation("UETraceHit",L,"FDasTraceHit"){
            addField<DAS_BIND_MANAGED_FIELD(BlockingHit)>("blocking_hit");addField<DAS_BIND_MANAGED_FIELD(StartPenetrating)>("start_penetrating");
            addField<DAS_BIND_MANAGED_FIELD(Distance)>("distance");addField<DAS_BIND_MANAGED_FIELD(Time)>("time");
            addField<DAS_BIND_MANAGED_FIELD(Location)>("location");addField<DAS_BIND_MANAGED_FIELD(ImpactPoint)>("impact_point");
            addField<DAS_BIND_MANAGED_FIELD(Normal)>("normal");addField<DAS_BIND_MANAGED_FIELD(ImpactNormal)>("impact_normal");addField<DAS_BIND_MANAGED_FIELD(FaceIndex)>("face_index");
            addField<DAS_BIND_MANAGED_FIELD(TraceStart)>("trace_start");addField<DAS_BIND_MANAGED_FIELD(TraceEnd)>("trace_end");addField<DAS_BIND_MANAGED_FIELD(PenetrationDepth)>("penetration_depth");
            addField<DAS_BIND_MANAGED_FIELD(Item)>("item");addField<DAS_BIND_MANAGED_FIELD(MyItem)>("my_item");addField<DAS_BIND_MANAGED_FIELD(ElementIndex)>("element_index");
        }
    };
    struct FOverlapResultAnnotation:FMathValueAnnotation<FDasOverlapResult>{
        FOverlapResultAnnotation(das::ModuleLibrary& L):FMathValueAnnotation("UEOverlapResult",L,"FDasOverlapResult"){
            addField<DAS_BIND_MANAGED_FIELD(BlockingHit)>("blocking_hit");addField<DAS_BIND_MANAGED_FIELD(ItemIndex)>("item_index");addField<DAS_BIND_MANAGED_FIELD(MyItemIndex)>("my_item_index");
        }
    };
    struct FReflectedValueAnnotation:FMathValueAnnotation<FDasStructValue>{FReflectedValueAnnotation(das::ModuleLibrary& L):FMathValueAnnotation("UEStructValue",L,"FDasStructValue"){}virtual bool isPod()const override{return true;}};
    FDasCallHandle BeginCallbackFrame(const char*,das::Context*);
    void StartCallbackFrameTimer(const char*,float,const char*,bool,const FDasCallHandle&,das::Context*);
    void NextCallbackFrameTimer(const char*,const char*,const FDasCallHandle&,das::Context*);
    #include "DasSoftRuntime.inl"
    #include "DasDSL.inl"
    #include "DasGameplayServices.inl"
    #include "DasInputActionValue.inl"
    #include "DasInputEventsRuntime.inl"
    #include "DasHitResultTransport.inl"
    #include "DasConstraintTransport.inl"
    struct FHitResultValueAnnotation:FMathValueAnnotation<FDasHitResultValue>{
        FHitResultValueAnnotation(das::ModuleLibrary& L):FMathValueAnnotation("UEHitResult",L,"FDasHitResultValue"){}
        virtual bool isPod()const override{return true;}
    };

#include "DasAsyncPhysicsVMExterns.inl"

    class FUnrealFoundationModule final : public das::Module
    {
    public:
        FUnrealFoundationModule() : das::Module("ue_foundation")
        {
            addCommentReader(new FUECommentReader());
            addAnnotation(new FUENativeBaseAnnotation());addAnnotation(new FUEClassAnnotation());addAnnotation(new FUEAsyncPhysicsAnnotation());addAnnotation(new FUELibraryAnnotation());addAnnotation(new FUEEnumAnnotation());addAnnotation(new FUENativeEnumAnnotation());addAnnotation(new FUENativeStructAnnotation());addAnnotation(new FUEStructAnnotation());
            addAnnotation(new FUENativeInterfaceAnnotation());addAnnotation(new FUENativeInterfaceAnnotation("ue_interface_ref"));addAnnotation(new FUESourceValueBinding());addAnnotation(new FUEObjectTypeAnnotation());addAnnotation(new FUEAuthoredRefAnnotation());addTypeMacro(new FUEObjectTypeMacro());addTypeMacro(new FUEObjectKeyTypeMacro());addAnnotation(new FUEClassTypeAnnotation());addTypeMacro(new FUEClassTypeMacro());addAnnotation(new FUESoftAnnotation());addTypeMacro(new FUESoftTypeMacro(false));addTypeMacro(new FUESoftTypeMacro(true));
            for(const auto& Pair:{std::pair<const char*,int>{"ue_soft",0},{"ue_soft_value",1},{"ue_soft_get",2},{"ue_soft_load",3}}){auto* Macro=new FUESoftCallMacro(Pair.first,Pair.second);ownedCallMacros.push_back(das::unique_ptr<das::CallMacro>(Macro));Macro->module=this;addCallMacro(Macro->name,[Macro](const das::LineInfo& At)->das::ExprLooksLikeCall*{auto* E=new das::ExprCallMacro(At,Macro->name);E->macro=Macro;return E;});}
            {auto* Macro=new FUESoftUpcastMacro();ownedCallMacros.push_back(das::unique_ptr<das::CallMacro>(Macro));Macro->module=this;addCallMacro(Macro->name,[Macro](const das::LineInfo& At)->das::ExprLooksLikeCall*{auto* E=new das::ExprCallMacro(At,Macro->name);E->macro=Macro;return E;});}
            for(bool Raw:{false,true}){auto* Macro=new FUEClassCastFactory(Raw);ownedCallMacros.push_back(das::unique_ptr<das::CallMacro>(Macro));Macro->module=this;addCallMacro(Macro->name,[Macro](const das::LineInfo& At)->das::ExprLooksLikeCall*{auto* E=new das::ExprCallMacro(At,Macro->name);E->macro=Macro;return E;});}
            addAnnotation(new FUEDelegateAnnotation(false));addAnnotation(new FUEDelegateAnnotation(true));
            addAnnotation(new FUECallbackFrameAnnotation());
            addAnnotation(new FUEFunctionAnnotation());addAnnotation(new FUECallbackAnnotation(false));addAnnotation(new FUECallbackAnnotation(true));addAnnotation(new FUEInterfaceAnnotation());addAnnotation(new FUEImplementsAnnotation());addAnnotation(new FUEComponentAnnotation());
            {auto* Macro=new FUEBoundDelegateFactory();ownedCallMacros.push_back(das::unique_ptr<das::CallMacro>(Macro));Macro->module=this;addCallMacro(Macro->name,[Macro](const das::LineInfo& At)->das::ExprLooksLikeCall*{auto* E=new das::ExprCallMacro(At,Macro->name);E->macro=Macro;return E;});}
            for(bool Component:{false,true})for(bool All:{false,true}){auto* Macro=new FUEObjectQueryFactory(Component,All);ownedCallMacros.push_back(das::unique_ptr<das::CallMacro>(Macro));Macro->module=this;addCallMacro(Macro->name,[Macro](const das::LineInfo& At)->das::ExprLooksLikeCall*{auto* E=new das::ExprCallMacro(At,Macro->name);E->macro=Macro;return E;});}
            {auto* Macro=new FUEActorSpawnFactory();ownedCallMacros.push_back(das::unique_ptr<das::CallMacro>(Macro));Macro->module=this;addCallMacro(Macro->name,[Macro](const das::LineInfo& At)->das::ExprLooksLikeCall*{auto* E=new das::ExprCallMacro(At,Macro->name);E->macro=Macro;return E;});}
            {auto* Macro=new FUEClassLoadedFactory();ownedCallMacros.push_back(das::unique_ptr<das::CallMacro>(Macro));Macro->module=this;addCallMacro(Macro->name,[Macro](const das::LineInfo& At)->das::ExprLooksLikeCall*{auto* E=new das::ExprCallMacro(At,Macro->name);E->macro=Macro;return E;});}
            for(int Mode=0;Mode<3;++Mode){auto* Macro=new FUENativeComponentFactory(Mode);ownedCallMacros.push_back(das::unique_ptr<das::CallMacro>(Macro));Macro->module=this;addCallMacro(Macro->name,[Macro](const das::LineInfo& At)->das::ExprLooksLikeCall*{auto* E=new das::ExprCallMacro(At,Macro->name);E->macro=Macro;return E;});}
            for(int Mode=0;Mode<3;++Mode){auto* Macro=new FUEObjectKeyFactory(Mode);ownedCallMacros.push_back(das::unique_ptr<das::CallMacro>(Macro));Macro->module=this;addCallMacro(Macro->name,[Macro](const das::LineInfo& At)->das::ExprLooksLikeCall*{auto* E=new das::ExprCallMacro(At,Macro->name);E->macro=Macro;return E;});}
            inferMacros.push_back(das::unique_ptr<das::PassMacro>(new FUEPropertyPass()));for(bool Owner:{false,true}){auto* Macro=new FUEReferenceFactory(Owner);ownedCallMacros.push_back(das::unique_ptr<das::CallMacro>(Macro));Macro->module=this;addCallMacro(Macro->name,[Macro](const das::LineInfo& At)->das::ExprLooksLikeCall*{auto* E=new das::ExprCallMacro(At,Macro->name);E->macro=Macro;return E;});}
            das::ModuleLibrary Library(this);
            Library.addBuiltInModule();
            addAnnotation(new FPhysicsBodyTokenAnnotation(Library));
            AddPhysicsDataExtern<DAS_BIND_FUN(PhysicsBody)>(*this,Library,"ue_physics_body");
            AddPhysicsDataExtern<DAS_BIND_FUN(PhysicsAddForce)>(*this,Library,"ue_physics_add_force");
            AddUEExtern<DAS_BIND_FUN(AsyncPhysicsSetInput)>(*this,Library,"ue_async_physics_set_input",das::SideEffects::modifyExternal);
            AddUEExtern<DAS_BIND_FUN(AsyncPhysicsHasOutput)>(*this,Library,"ue_async_physics_has_output",das::SideEffects::modifyExternal);
            AddUEExtern<DAS_BIND_FUN(AsyncPhysicsOutput)>(*this,Library,"ue_async_physics_output",das::SideEffects::modifyExternal);
            AddUEExtern<DAS_BIND_FUN(AsyncPhysicsRunning)>(*this,Library,"ue_async_physics_running",das::SideEffects::modifyExternal);
            AddUEExtern<DAS_BIND_FUN(AsyncPhysicsError)>(*this,Library,"ue_async_physics_error",das::SideEffects::modifyExternal);
            AddPhysicsDataExtern<DAS_BIND_FUN(PhysicsInput)>(*this,Library,"ue_physics_input");
            AddPhysicsDataExtern<DAS_BIND_FUN(PhysicsOutput)>(*this,Library,"ue_physics_output");
            addAnnotation(new FDSLTextAnnotation(Library));addAnnotation(new FDSLNameAnnotation(Library));addAnnotation(new FDSLEnumAnnotation(Library));
            addAnnotation(new FReflectedValueAnnotation(Library));
            addAnnotation(new FDSLVector2DAnnotation(Library));addAnnotation(new FDSLVectorAnnotation(Library));addAnnotation(new FDSLRotatorAnnotation(Library));addAnnotation(new FMathValueAnnotation<FTransform>("UETransform",Library,"FTransform"));

            addAnnotation(new FObjectHandleAnnotation(Library));addAnnotation(new FClassHandleAnnotation(Library));
            addAnnotation(new FTraceHitAnnotation(Library));
            addAnnotation(new FCallHandleAnnotation(Library));
            addAnnotation(new TInputEventAnnotation<FDasInputEventValue>("UEInputEvent",Library,"FDasInputEventValue"));
            AddUEExtern<DAS_BIND_FUN(ArgumentInputEvent)>(*this,Library,"__ue_arg_inputevent",das::SideEffects::modifyExternal);
            AddUEExtern<DAS_BIND_FUN(ResultInputEvent),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_result_inputevent",das::SideEffects::modifyExternal)->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(InputFromInputEvent),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_unpack_inputevent",das::SideEffects::accessExternal);
            addAnnotation(new TInputEventAnnotation<FDasKeyEventValue>("UEKeyEvent",Library,"FDasKeyEventValue"));
            AddUEExtern<DAS_BIND_FUN(ArgumentKeyEvent)>(*this,Library,"__ue_arg_keyevent",das::SideEffects::modifyExternal);
            AddUEExtern<DAS_BIND_FUN(ResultKeyEvent),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_result_keyevent",das::SideEffects::modifyExternal)->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(InputFromKeyEvent),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_unpack_keyevent",das::SideEffects::accessExternal);
            addAnnotation(new TInputEventAnnotation<FDasAnalogInputEventValue>("UEAnalogInputEvent",Library,"FDasAnalogInputEventValue"));
            AddUEExtern<DAS_BIND_FUN(ArgumentAnalogInputEvent)>(*this,Library,"__ue_arg_analoginputevent",das::SideEffects::modifyExternal);
            AddUEExtern<DAS_BIND_FUN(ResultAnalogInputEvent),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_result_analoginputevent",das::SideEffects::modifyExternal)->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(InputFromAnalogInputEvent),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_unpack_analoginputevent",das::SideEffects::accessExternal);
            addAnnotation(new TInputEventAnnotation<FDasCharacterEventValue>("UECharacterEvent",Library,"FDasCharacterEventValue"));
            AddUEExtern<DAS_BIND_FUN(ArgumentCharacterEvent)>(*this,Library,"__ue_arg_characterevent",das::SideEffects::modifyExternal);
            AddUEExtern<DAS_BIND_FUN(ResultCharacterEvent),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_result_characterevent",das::SideEffects::modifyExternal)->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(InputFromCharacterEvent),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_unpack_characterevent",das::SideEffects::accessExternal);
            addAnnotation(new TInputEventAnnotation<FDasPointerEventValue>("UEPointerEvent",Library,"FDasPointerEventValue"));
            AddUEExtern<DAS_BIND_FUN(ArgumentPointerEvent)>(*this,Library,"__ue_arg_pointerevent",das::SideEffects::modifyExternal);
            AddUEExtern<DAS_BIND_FUN(ResultPointerEvent),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_result_pointerevent",das::SideEffects::modifyExternal)->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(InputFromPointerEvent),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_unpack_pointerevent",das::SideEffects::accessExternal);
            addAnnotation(new TInputEventAnnotation<FDasNavigationEventValue>("UENavigationEvent",Library,"FDasNavigationEventValue"));
            AddUEExtern<DAS_BIND_FUN(ArgumentNavigationEvent)>(*this,Library,"__ue_arg_navigationevent",das::SideEffects::modifyExternal);
            AddUEExtern<DAS_BIND_FUN(ResultNavigationEvent),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_result_navigationevent",das::SideEffects::modifyExternal)->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(InputFromNavigationEvent),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_unpack_navigationevent",das::SideEffects::accessExternal);
            addAnnotation(new FInputActionValueAnnotation(Library));
            AddUEExtern<DAS_BIND_FUN(PackInputValue),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_math_pack_input_value",das::SideEffects::modifyExternal,"PackInputValue")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(UnpackInputValue),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_math_unpack_input_value",das::SideEffects::accessExternal,"UnpackInputValue");
            AddUEExtern<DAS_BIND_FUN(MakeInputValue),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_input_make",das::SideEffects::none,"MakeInputValue");
            AddUEExtern<DAS_BIND_FUN(InputValueKind)>(*this,Library,"__ue_input_kind",das::SideEffects::none,"InputValueKind");
            AddUEExtern<DAS_BIND_FUN(InputValueBool)>(*this,Library,"ue_input_bool",das::SideEffects::none,"InputValueBool");
            AddUEExtern<DAS_BIND_FUN(InputValueAxis1D)>(*this,Library,"ue_input_axis1d",das::SideEffects::none,"InputValueAxis1D");
            AddUEExtern<DAS_BIND_FUN(InputValueAxis2D),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_input_axis2d",das::SideEffects::none,"InputValueAxis2D");
            AddUEExtern<DAS_BIND_FUN(InputValueAxis3D),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_input_axis3d",das::SideEffects::none,"InputValueAxis3D");
            AddUEExtern<DAS_BIND_FUN(ArgumentInputValue)>(*this,Library,"__ue_arg_input_value",das::SideEffects::modifyExternal,"ArgumentInputValue");
            AddUEExtern<DAS_BIND_FUN(ResultInputValue),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_result_input_value",das::SideEffects::accessExternal,"ResultInputValue");
            addAnnotation(new FHitResultValueAnnotation(Library));
            addAnnotation(new FMathValueAnnotation<FDasConstraintHandle>("UEConstraintHandle",Library,"FDasConstraintHandle"));
            AddUEExtern<DAS_BIND_FUN(ArgumentConstraint)>(*this,Library,"__ue_arg_constraint",das::SideEffects::modifyExternal,"ArgumentConstraint");
            AddUEExtern<DAS_BIND_FUN(ResultConstraint),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_result_constraint",das::SideEffects::modifyExternal,"ResultConstraint")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(ConstraintValid)>(*this,Library,"ue_constraint_valid",das::SideEffects::accessExternal,"ConstraintValid");
            AddUEExtern<DAS_BIND_FUN(ArgumentHitResult)>(*this,Library,"__ue_arg_hit_result",das::SideEffects::modifyExternal,"ArgumentHitResult");
            AddUEExtern<DAS_BIND_FUN(ResultHitResult),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_result_hit_result",das::SideEffects::modifyExternal,"ResultHitResult")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(EmptyHitResult),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_hit_result_empty",das::SideEffects::modifyExternal,"EmptyHitResult")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(ViewHitResult),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_hit_result_view",das::SideEffects::modifyExternal,"ViewHitResult");
            addAnnotation(new FOverlapResultAnnotation(Library));
            AddUEExtern<DAS_BIND_FUN(TraceMaterial),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_trace_material",das::SideEffects::accessExternal,"TraceMaterial");
            AddUEExtern<DAS_BIND_FUN(TracePhysicsOwner),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_trace_physics_owner",das::SideEffects::accessExternal,"TracePhysicsOwner");
            AddUEExtern<DAS_BIND_FUN(TraceBone)>(*this,Library,"__ue_trace_bone",das::SideEffects::accessExternal,"TraceBone")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(OverlapWorld)>(*this,Library,"__ue_overlap_world",das::SideEffects::modifyExternal,"OverlapWorld")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(OverlapReference),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_overlap_reference",das::SideEffects::accessExternal,"OverlapReference");
            AddUEExtern<DAS_BIND_FUN(TraceWorld),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_trace_world",das::SideEffects::modifyExternal,"TraceWorld");
            AddUEExtern<DAS_BIND_FUN(TraceActor),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_trace_actor",das::SideEffects::accessExternal,"TraceActor");
            AddUEExtern<DAS_BIND_FUN(TraceComponent),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_trace_component",das::SideEffects::accessExternal,"TraceComponent");
            AddUEExtern<DAS_BIND_FUN(ApplyGameplayDamage)>(*this,Library,"__ue_apply_damage",das::SideEffects::modifyExternal,"ApplyGameplayDamage");
            AddUEExtern<DAS_BIND_FUN(CreateGameplayWidget),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_widget_create",das::SideEffects::modifyExternal,"CreateGameplayWidget");
            AddUEExtern<DAS_BIND_FUN(GameplayWidgetVisible)>(*this,Library,"__ue_widget_visible",das::SideEffects::accessExternal,"GameplayWidgetVisible");
            AddUEExtern<DAS_BIND_FUN(ShowGameplayWidget)>(*this,Library,"__ue_widget_show",das::SideEffects::modifyExternal,"ShowGameplayWidget");
            AddUEExtern<DAS_BIND_FUN(RemoveGameplayWidget)>(*this,Library,"__ue_widget_remove",das::SideEffects::modifyExternal,"RemoveGameplayWidget");
            AddUEExtern<DAS_BIND_FUN(GameplayWidgetText)>(*this,Library,"__ue_widget_text",das::SideEffects::modifyExternal,"GameplayWidgetText");
            AddUEExtern<DAS_BIND_FUN(GameplayWidgetClick)>(*this,Library,"__ue_widget_click",das::SideEffects::modifyExternal,"GameplayWidgetClick");
            AddUEExtern<DAS_BIND_FUN(GameplayWidgetInput)>(*this,Library,"__ue_widget_input",das::SideEffects::modifyExternal,"GameplayWidgetInput");


            AddUEExtern<DAS_BIND_FUN(CharacterController),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_pawn_controller",das::SideEffects::modifyExternal,"CharacterController");
            AddUEExtern<DAS_BIND_FUN(CharacterControlsReady)>(*this,Library,"__ue_character_controls_ready",das::SideEffects::modifyExternal,"CharacterControlsReady");
            AddUEExtern<DAS_BIND_FUN(CharacterPossess)>(*this,Library,"__ue_pawn_possess",das::SideEffects::modifyExternal,"CharacterPossess");
            AddUEExtern<DAS_BIND_FUN(NativeReferenceGetint)>(*this,Library,"__ue_native_get_int",das::SideEffects::modifyExternal,"NativeReferenceGetint");
            AddUEExtern<DAS_BIND_FUN(NativeReferenceSetint)>(*this,Library,"__ue_native_set_int",das::SideEffects::modifyExternal,"NativeReferenceSetint");
            AddUEExtern<DAS_BIND_FUN(NativeReferenceGetfloat)>(*this,Library,"__ue_native_get_float",das::SideEffects::modifyExternal,"NativeReferenceGetfloat");
            AddUEExtern<DAS_BIND_FUN(NativeReferenceSetfloat)>(*this,Library,"__ue_native_set_float",das::SideEffects::modifyExternal,"NativeReferenceSetfloat");
            AddUEExtern<DAS_BIND_FUN(NativeReferenceGetdouble)>(*this,Library,"__ue_native_get_double",das::SideEffects::modifyExternal,"NativeReferenceGetdouble");
            AddUEExtern<DAS_BIND_FUN(NativeReferenceSetdouble)>(*this,Library,"__ue_native_set_double",das::SideEffects::modifyExternal,"NativeReferenceSetdouble");
            AddUEExtern<DAS_BIND_FUN(NativeReferenceGetbool)>(*this,Library,"__ue_native_get_bool",das::SideEffects::modifyExternal,"NativeReferenceGetbool");
            AddUEExtern<DAS_BIND_FUN(NativeReferenceSetbool)>(*this,Library,"__ue_native_set_bool",das::SideEffects::modifyExternal,"NativeReferenceSetbool");
            AddUEExtern<DAS_BIND_FUN(NativeReferenceGetstring)>(*this,Library,"__ue_native_get_string",das::SideEffects::modifyExternal,"NativeReferenceGetstring");
            AddUEExtern<DAS_BIND_FUN(NativeReferenceSetstring)>(*this,Library,"__ue_native_set_string",das::SideEffects::modifyExternal,"NativeReferenceSetstring");
            AddUEExtern<DAS_BIND_FUN(DSLTextCreate)>(*this,Library,"ue_text",das::SideEffects::modifyExternal,"DSLTextCreate");
            AddUEExtern<DAS_BIND_FUN(DSLTextInvariant)>(*this,Library,"ue_text_invariant",das::SideEffects::modifyExternal,"DSLTextInvariant");
            AddUEExtern<DAS_BIND_FUN(DSLTextToString)>(*this,Library,"ue_text_to_string",das::SideEffects::modifyExternal,"DSLTextToString");
            AddUEExtern<DAS_BIND_FUN(DSLTextNamespace)>(*this,Library,"ue_text_namespace",das::SideEffects::modifyExternal,"DSLTextNamespace");
            AddUEExtern<DAS_BIND_FUN(DSLTextKey)>(*this,Library,"ue_text_key",das::SideEffects::modifyExternal,"DSLTextKey");
            AddUEExtern<DAS_BIND_FUN(DSLTextIsInvariant)>(*this,Library,"ue_text_is_invariant",das::SideEffects::modifyExternal,"DSLTextIsInvariant");
            AddUEExtern<DAS_BIND_FUN(NativeReferenceGetbyte)>(*this,Library,"__ue_native_get_byte",das::SideEffects::modifyExternal,"NativeReferenceGetbyte");
            AddUEExtern<DAS_BIND_FUN(NativeReferenceSetbyte)>(*this,Library,"__ue_native_set_byte",das::SideEffects::modifyExternal,"NativeReferenceSetbyte");
            AddUEExtern<DAS_BIND_FUN(NativeReferenceGettext)>(*this,Library,"__ue_native_get_text",das::SideEffects::modifyExternal,"NativeReferenceGettext");
            AddUEExtern<DAS_BIND_FUN(NativeReferenceSettext)>(*this,Library,"__ue_native_set_text",das::SideEffects::modifyExternal,"NativeReferenceSettext");
            AddUEExtern<DAS_BIND_FUN(NativeReferenceGetenum)>(*this,Library,"__ue_native_get_enum",das::SideEffects::modifyExternal,"NativeReferenceGetenum");
            AddUEExtern<DAS_BIND_FUN(NativeReferenceSetenum)>(*this,Library,"__ue_native_set_enum",das::SideEffects::modifyExternal,"NativeReferenceSetenum");
            AddUEExtern<DAS_BIND_FUN(NativeReferenceGetname)>(*this,Library,"__ue_native_get_name",das::SideEffects::modifyExternal,"NativeReferenceGetname");
            AddUEExtern<DAS_BIND_FUN(NativeReferenceSetname)>(*this,Library,"__ue_native_set_name",das::SideEffects::modifyExternal,"NativeReferenceSetname");
            AddUEExtern<DAS_BIND_FUN(NativeReferenceGetclass)>(*this,Library,"__ue_native_get_class",das::SideEffects::modifyExternal,"NativeReferenceGetclass");
            AddUEExtern<DAS_BIND_FUN(NativeReferenceSetclass)>(*this,Library,"__ue_native_set_class",das::SideEffects::modifyExternal,"NativeReferenceSetclass");
            AddUEExtern<DAS_BIND_FUN(NativeReferenceGetobject),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_native_get_object",das::SideEffects::modifyExternal,"NativeReferenceGetobject");
            AddUEExtern<DAS_BIND_FUN(NativeReferenceSetobject)>(*this,Library,"__ue_native_set_object",das::SideEffects::modifyExternal,"NativeReferenceSetobject");
            AddUEExtern<DAS_BIND_FUN(NativeReferenceGetstruct),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_native_get_struct",das::SideEffects::modifyExternal,"NativeReferenceGetstruct")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(NativeReferenceSetstruct)>(*this,Library,"__ue_native_set_struct",das::SideEffects::modifyExternal,"NativeReferenceSetstruct");
            AddUEExtern<DAS_BIND_FUN(CharacterMove)>(*this,Library,"__ue_character_move",das::SideEffects::modifyExternal,"CharacterMove");
            AddUEExtern<DAS_BIND_FUN(CharacterLook)>(*this,Library,"__ue_character_look",das::SideEffects::modifyExternal,"CharacterLook");
            AddUEExtern<DAS_BIND_FUN(CharacterJump)>(*this,Library,"__ue_character_jump",das::SideEffects::modifyExternal,"CharacterJump");
            AddUEExtern<DAS_BIND_FUN(ScriptEnhancedAddContext)>(*this,Library,"__ue_enhanced_add_context",das::SideEffects::modifyExternal,"ScriptEnhancedAddContext");
            AddUEExtern<DAS_BIND_FUN(ScriptEnhancedRemoveContext)>(*this,Library,"__ue_enhanced_remove_context",das::SideEffects::modifyExternal,"ScriptEnhancedRemoveContext");
            AddUEExtern<DAS_BIND_FUN(ScriptEnhancedHasContext)>(*this,Library,"__ue_enhanced_has_context",das::SideEffects::modifyExternal,"ScriptEnhancedHasContext");
            AddUEExtern<DAS_BIND_FUN(ScriptEnhancedClearContexts)>(*this,Library,"ue_enhanced_clear_contexts",das::SideEffects::modifyExternal,"ScriptEnhancedClearContexts");
            AddUEExtern<DAS_BIND_FUN(ScriptEnhancedBind)>(*this,Library,"__ue_enhanced_bind",das::SideEffects::modifyExternal,"ScriptEnhancedBind");
            AddUEExtern<DAS_BIND_FUN(ScriptEnhancedUnbind)>(*this,Library,"__ue_enhanced_unbind",das::SideEffects::modifyExternal,"ScriptEnhancedUnbind");
            AddUEExtern<DAS_BIND_FUN(ScriptEnhancedAvailable)>(*this,Library,"ue_enhanced_is_available",das::SideEffects::modifyExternal,"ScriptEnhancedAvailable");
            AddUEExtern<DAS_BIND_FUN(ScriptInputAction)>(*this,Library,"ue_input_bind_action",das::SideEffects::modifyExternal,"ScriptInputAction");
            AddUEExtern<DAS_BIND_FUN(ScriptInputAxis)>(*this,Library,"ue_input_bind_axis",das::SideEffects::modifyExternal,"ScriptInputAxis");
            AddUEExtern<DAS_BIND_FUN(ScriptInputAvailable)>(*this,Library,"ue_input_is_available",das::SideEffects::modifyExternal,"ScriptInputAvailable");
            AddUEExtern<DAS_BIND_FUN(ScriptInputUnbindAction)>(*this,Library,"ue_input_unbind_action",das::SideEffects::modifyExternal,"ScriptInputUnbindAction");
            AddUEExtern<DAS_BIND_FUN(ScriptInputUnbindAxis)>(*this,Library,"ue_input_unbind_axis",das::SideEffects::modifyExternal,"ScriptInputUnbindAxis");
            AddUEExtern<DAS_BIND_FUN(ScriptInputUnbindAll)>(*this,Library,"ue_input_unbind_all",das::SideEffects::modifyExternal,"ScriptInputUnbindAll");
            AddUEExtern<DAS_BIND_FUN(ScriptAssetLoad)>(*this,Library,"__ue_asset_load_async",das::SideEffects::modifyExternal,"ScriptAssetLoad");
            AddUEExtern<DAS_BIND_FUN(SoftMake),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_soft_make",das::SideEffects::modifyExternal,"SoftMake");
            AddUEExtern<DAS_BIND_FUN(SoftUpcast),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_soft_upcast",das::SideEffects::modifyExternal,"SoftUpcast");
            AddUEExtern<DAS_BIND_FUN(SoftCheck),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_soft_check",das::SideEffects::modifyExternal,"SoftCheck");
            AddUEExtern<DAS_BIND_FUN(SoftGetObject),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_soft_get_object",das::SideEffects::modifyExternal,"SoftGetObject");
            AddUEExtern<DAS_BIND_FUN(SoftGetClass)>(*this,Library,"__ue_soft_get_class",das::SideEffects::modifyExternal,"SoftGetClass");
            AddUEExtern<DAS_BIND_FUN(SoftPath)>(*this,Library,"__ue_soft_path",das::SideEffects::modifyExternal,"SoftPath");
            AddUEExtern<DAS_BIND_FUN(SoftNull)>(*this,Library,"__ue_soft_null",das::SideEffects::modifyExternal,"SoftNull");
            AddUEExtern<DAS_BIND_FUN(SoftLoaded)>(*this,Library,"__ue_soft_loaded",das::SideEffects::modifyExternal,"SoftLoaded");
            AddUEExtern<DAS_BIND_FUN(ScriptSoftAssetLoad)>(*this,Library,"__ue_soft_load_async",das::SideEffects::modifyExternal,"ScriptSoftAssetLoad");
            AddUEExtern<DAS_BIND_FUN(ScriptAssetLoadGeneric)>(*this,Library,"ue_asset_load_async",das::SideEffects::modifyExternal,"ScriptAssetLoadGeneric");
            AddUEExtern<DAS_BIND_FUN(ScriptAssetCancel)>(*this,Library,"ue_asset_cancel",das::SideEffects::modifyExternal,"ScriptAssetCancel");
            AddUEExtern<DAS_BIND_FUN(ScriptAssetCancelAll)>(*this,Library,"ue_asset_cancel_all",das::SideEffects::modifyExternal,"ScriptAssetCancelAll");
            AddUEExtern<DAS_BIND_FUN(ScriptAssetPending)>(*this,Library,"ue_asset_is_pending",das::SideEffects::modifyExternal,"ScriptAssetPending");
            AddUEExtern<DAS_BIND_FUN(ScriptAISelf),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_ai_self",das::SideEffects::modifyExternal,"ScriptAISelf");
            AddUEExtern<DAS_BIND_FUN(ScriptAIMove)>(*this,Library,"__ue_ai_move",das::SideEffects::modifyExternal,"ScriptAIMove");
            AddUEExtern<DAS_BIND_FUN(ScriptAICancel)>(*this,Library,"__ue_ai_cancel",das::SideEffects::modifyExternal,"ScriptAICancel");
            AddUEExtern<DAS_BIND_FUN(ScriptAIActive)>(*this,Library,"__ue_ai_active",das::SideEffects::modifyExternal,"ScriptAIActive");
            AddUEExtern<DAS_BIND_FUN(ScriptEffectPlay)>(*this,Library,"__ue_effect_play",das::SideEffects::modifyExternal,"ScriptEffectPlay");
            AddUEExtern<DAS_BIND_FUN(ScriptEffectStop)>(*this,Library,"__ue_effect_stop",das::SideEffects::modifyExternal,"ScriptEffectStop");
            AddUEExtern<DAS_BIND_FUN(ScriptEffectStopAll)>(*this,Library,"__ue_effect_stop_all",das::SideEffects::modifyExternal,"ScriptEffectStopAll");
            AddUEExtern<DAS_BIND_FUN(ScriptEffectActive)>(*this,Library,"__ue_effect_active",das::SideEffects::modifyExternal,"ScriptEffectActive");
            AddUEExtern<DAS_BIND_FUN(ScriptMontagePlay)>(*this,Library,"__ue_montage_play",das::SideEffects::modifyExternal,"ScriptMontagePlay");
            AddUEExtern<DAS_BIND_FUN(ScriptMontageStop)>(*this,Library,"__ue_montage_stop",das::SideEffects::modifyExternal,"ScriptMontageStop");
            AddUEExtern<DAS_BIND_FUN(ScriptMontageCancel)>(*this,Library,"__ue_montage_cancel",das::SideEffects::modifyExternal,"ScriptMontageCancel");
            AddUEExtern<DAS_BIND_FUN(ScriptMontagePlaying)>(*this,Library,"__ue_montage_playing",das::SideEffects::modifyExternal,"ScriptMontagePlaying");
            AddUEExtern<DAS_BIND_FUN(ScriptLatentDelay)>(*this,Library,"ue_latent_delay",das::SideEffects::modifyExternal,"ScriptLatentDelay");
            AddUEExtern<DAS_BIND_FUN(ScriptLatentCancel)>(*this,Library,"ue_latent_cancel",das::SideEffects::modifyExternal,"ScriptLatentCancel");
            AddUEExtern<DAS_BIND_FUN(ScriptLatentCancelAll)>(*this,Library,"ue_latent_cancel_all",das::SideEffects::modifyExternal,"ScriptLatentCancelAll");
            AddUEExtern<DAS_BIND_FUN(ScriptLatentPending)>(*this,Library,"ue_latent_is_pending",das::SideEffects::modifyExternal,"ScriptLatentPending");
            AddUEExtern<DAS_BIND_FUN(ScriptTimerStart)>(*this,Library,"ue_timer_start",das::SideEffects::modifyExternal,"ScriptTimerStart");
            AddUEExtern<DAS_BIND_FUN(ScriptTimerStartVector2d)>(*this,Library,"ue_timer_start",das::SideEffects::modifyExternal,"ScriptTimerStartVector2d");
            AddUEExtern<DAS_BIND_FUN(ScriptTimerNextTickVector2d)>(*this,Library,"ue_timer_next_tick",das::SideEffects::modifyExternal,"ScriptTimerNextTickVector2d");
            AddUEExtern<DAS_BIND_FUN(ScriptTimerStartClass)>(*this,Library,"__ue_timer_start_class",das::SideEffects::modifyExternal,"ScriptTimerStartClass");
            AddUEExtern<DAS_BIND_FUN(ScriptTimerNextTickClass)>(*this,Library,"__ue_timer_next_tick_class",das::SideEffects::modifyExternal,"ScriptTimerNextTickClass");
            AddUEExtern<DAS_BIND_FUN(ScriptTimerStartObject)>(*this,Library,"__ue_timer_start_object",das::SideEffects::modifyExternal,"ScriptTimerStartObject");
            AddUEExtern<DAS_BIND_FUN(ScriptTimerNextTickObject)>(*this,Library,"__ue_timer_next_tick_object",das::SideEffects::modifyExternal,"ScriptTimerNextTickObject");
            AddUEExtern<DAS_BIND_FUN(ScriptTimerStartArray)>(*this,Library,"__ue_timer_start_array",das::SideEffects::modifyExternal,"ScriptTimerStartArray");
            AddUEExtern<DAS_BIND_FUN(ScriptTimerStartEnum)>(*this,Library,"ue_timer_start",das::SideEffects::modifyExternal,"ScriptTimerStartEnum");
            AddUEExtern<DAS_BIND_FUN(ScriptTimerNextTickEnum)>(*this,Library,"ue_timer_next_tick",das::SideEffects::modifyExternal,"ScriptTimerNextTickEnum");
            AddUEExtern<DAS_BIND_FUN(ScriptTimerStartByte)>(*this,Library,"ue_timer_start",das::SideEffects::modifyExternal,"ScriptTimerStartByte");
            AddUEExtern<DAS_BIND_FUN(ScriptTimerNextTickByte)>(*this,Library,"ue_timer_next_tick",das::SideEffects::modifyExternal,"ScriptTimerNextTickByte");
            AddUEExtern<DAS_BIND_FUN(ScriptTimerStartText)>(*this,Library,"ue_timer_start",das::SideEffects::modifyExternal,"ScriptTimerStartText");
            AddUEExtern<DAS_BIND_FUN(ScriptTimerNextTickText)>(*this,Library,"ue_timer_next_tick",das::SideEffects::modifyExternal,"ScriptTimerNextTickText");
            AddUEExtern<DAS_BIND_FUN(ScriptTimerStartInt)>(*this,Library,"ue_timer_start",das::SideEffects::modifyExternal,"ScriptTimerStartInt");
            AddUEExtern<DAS_BIND_FUN(ScriptTimerStartFloat)>(*this,Library,"ue_timer_start",das::SideEffects::modifyExternal,"ScriptTimerStartFloat");
            AddUEExtern<DAS_BIND_FUN(ScriptTimerStartBool)>(*this,Library,"ue_timer_start",das::SideEffects::modifyExternal,"ScriptTimerStartBool");
            AddUEExtern<DAS_BIND_FUN(ScriptTimerStartDouble)>(*this,Library,"ue_timer_start",das::SideEffects::modifyExternal,"ScriptTimerStartDouble");
            AddUEExtern<DAS_BIND_FUN(ScriptTimerStartString)>(*this,Library,"ue_timer_start",das::SideEffects::modifyExternal,"ScriptTimerStartString");
            AddUEExtern<DAS_BIND_FUN(ScriptTimerStartName)>(*this,Library,"ue_timer_start",das::SideEffects::modifyExternal,"ScriptTimerStartName");
            AddUEExtern<DAS_BIND_FUN(ScriptTimerStartVector)>(*this,Library,"ue_timer_start",das::SideEffects::modifyExternal,"ScriptTimerStartVector");
            AddUEExtern<DAS_BIND_FUN(ScriptTimerStartRotator)>(*this,Library,"ue_timer_start",das::SideEffects::modifyExternal,"ScriptTimerStartRotator");
            AddUEExtern<DAS_BIND_FUN(ScriptTimerStartTransform)>(*this,Library,"ue_timer_start",das::SideEffects::modifyExternal,"ScriptTimerStartTransform");
            AddUEExtern<DAS_BIND_FUN(ScriptTimerNextTick)>(*this,Library,"ue_timer_next_tick",das::SideEffects::modifyExternal,"ScriptTimerNextTick");
            AddUEExtern<DAS_BIND_FUN(ScriptTimerNextTickArray)>(*this,Library,"__ue_timer_next_tick_array",das::SideEffects::modifyExternal,"ScriptTimerNextTickArray");
            AddUEExtern<DAS_BIND_FUN(ScriptTimerPaused)>(*this,Library,"ue_timer_is_paused",das::SideEffects::modifyExternal,"ScriptTimerPaused");
            AddUEExtern<DAS_BIND_FUN(ScriptTimerElapsed)>(*this,Library,"ue_timer_elapsed",das::SideEffects::modifyExternal,"ScriptTimerElapsed");
            AddUEExtern<DAS_BIND_FUN(ScriptTimerCancelAll)>(*this,Library,"ue_timer_cancel_all",das::SideEffects::modifyExternal,"ScriptTimerCancelAll");
            AddUEExtern<DAS_BIND_FUN(ScriptTimerNextTickInt)>(*this,Library,"ue_timer_next_tick",das::SideEffects::modifyExternal,"ScriptTimerNextTickInt");
            AddUEExtern<DAS_BIND_FUN(ScriptTimerNextTickFloat)>(*this,Library,"ue_timer_next_tick",das::SideEffects::modifyExternal,"ScriptTimerNextTickFloat");
            AddUEExtern<DAS_BIND_FUN(ScriptTimerNextTickBool)>(*this,Library,"ue_timer_next_tick",das::SideEffects::modifyExternal,"ScriptTimerNextTickBool");
            AddUEExtern<DAS_BIND_FUN(ScriptTimerNextTickDouble)>(*this,Library,"ue_timer_next_tick",das::SideEffects::modifyExternal,"ScriptTimerNextTickDouble");
            AddUEExtern<DAS_BIND_FUN(ScriptTimerNextTickString)>(*this,Library,"ue_timer_next_tick",das::SideEffects::modifyExternal,"ScriptTimerNextTickString");
            AddUEExtern<DAS_BIND_FUN(ScriptTimerNextTickName)>(*this,Library,"ue_timer_next_tick",das::SideEffects::modifyExternal,"ScriptTimerNextTickName");
            AddUEExtern<DAS_BIND_FUN(ScriptTimerNextTickVector)>(*this,Library,"ue_timer_next_tick",das::SideEffects::modifyExternal,"ScriptTimerNextTickVector");
            AddUEExtern<DAS_BIND_FUN(ScriptTimerNextTickRotator)>(*this,Library,"ue_timer_next_tick",das::SideEffects::modifyExternal,"ScriptTimerNextTickRotator");
            AddUEExtern<DAS_BIND_FUN(ScriptTimerNextTickTransform)>(*this,Library,"ue_timer_next_tick",das::SideEffects::modifyExternal,"ScriptTimerNextTickTransform");
            AddUEExtern<DAS_BIND_FUN(ScriptTimerCancel)>(*this,Library,"ue_timer_cancel",das::SideEffects::modifyExternal,"ScriptTimerCancel");
            AddUEExtern<DAS_BIND_FUN(ScriptTimerPause)>(*this,Library,"ue_timer_pause",das::SideEffects::modifyExternal,"ScriptTimerPause");
            AddUEExtern<DAS_BIND_FUN(ScriptTimerResume)>(*this,Library,"ue_timer_resume",das::SideEffects::modifyExternal,"ScriptTimerResume");
            AddUEExtern<DAS_BIND_FUN(ScriptTimerActive)>(*this,Library,"ue_timer_is_active",das::SideEffects::modifyExternal,"ScriptTimerActive");
            AddUEExtern<DAS_BIND_FUN(ScriptTimerRemaining)>(*this,Library,"ue_timer_remaining",das::SideEffects::modifyExternal,"ScriptTimerRemaining");

            AddUEExtern<DAS_BIND_FUN(ObjectCastNullable), das::SimNode_ExtFuncCallAndCopyOrMove>(*this, Library, "ue_cast_nullable_object", das::SideEffects::modifyExternal, "ObjectCastNullable");

            AddUEExtern<DAS_BIND_FUN(BeginCallbackFrame),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_begin_callback_frame",das::SideEffects::modifyExternal,"BeginCallbackFrame");
            AddUEExtern<DAS_BIND_FUN(StartCallbackFrameTimer)>(*this,Library,"__ue_timer_start_frame",das::SideEffects::modifyExternal,"StartCallbackFrameTimer");
            AddUEExtern<DAS_BIND_FUN(NextCallbackFrameTimer)>(*this,Library,"__ue_timer_next_tick_frame",das::SideEffects::modifyExternal,"NextCallbackFrameTimer");
            AddUEExtern<DAS_BIND_FUN(BeginNativeGameplayCall),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_begin_native_gameplay_call",das::SideEffects::modifyExternal,"BeginNativeGameplayCall");
            AddUEExtern<DAS_BIND_FUN(ArgumentNativeDelegate)>(*this,Library,"__ue_arg_native_delegate",das::SideEffects::modifyExternal,"ArgumentNativeDelegate");
            AddUEExtern<DAS_BIND_FUN(DelegateGetField)>(*this,Library,"__ue_delegate_get_field",das::SideEffects::modifyExternal,"DelegateGetField");
            AddUEExtern<DAS_BIND_FUN(DelegateSetField)>(*this,Library,"__ue_delegate_set_field",das::SideEffects::modifyExternal,"DelegateSetField");
            AddUEExtern<DAS_BIND_FUN(DelegateCreate)>(*this,Library,"__ue_delegate_create",das::SideEffects::modifyExternal,"DelegateCreate");
            AddUEExtern<DAS_BIND_FUN(DelegateBind)>(*this,Library,"__ue_delegate_bind",das::SideEffects::modifyExternal,"DelegateBind");
            AddUEExtern<DAS_BIND_FUN(DelegateRemove)>(*this,Library,"__ue_delegate_remove",das::SideEffects::modifyExternal,"DelegateRemove");
            AddUEExtern<DAS_BIND_FUN(DelegateClear)>(*this,Library,"__ue_delegate_clear",das::SideEffects::modifyExternal,"DelegateClear");
            AddUEExtern<DAS_BIND_FUN(DelegateIsBound)>(*this,Library,"__ue_delegate_is_bound",das::SideEffects::modifyExternal,"DelegateIsBound");
            AddUEExtern<DAS_BIND_FUN(DelegateEventView)>(*this,Library,"__ue_delegate_event_view",das::SideEffects::modifyExternal,"DelegateEventView");
            AddUEExtern<DAS_BIND_FUN(BeginDelegateCall),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_delegate_begin_call",das::SideEffects::modifyExternal,"BeginDelegateCall");

            AddUEExtern<DAS_BIND_FUN(ExpectCallSignature)>(*this, Library, "ue_expect_signature", das::SideEffects::modifyExternal, "ExpectCallSignature");
            AddUEExtern<DAS_BIND_FUN(MethodArgdelegate)>(*this,Library,"__ue_method_arg_delegate",das::SideEffects::modifyExternal,"MethodArgdelegate");
            AddUEExtern<DAS_BIND_FUN(MethodResultdelegate)>(*this,Library,"__ue_method_result_delegate",das::SideEffects::modifyExternal,"MethodResultdelegate");
            AddUEExtern<DAS_BIND_FUN(GetCallEnum)>(*this, Library, "ue_result_enum", das::SideEffects::modifyExternal, "GetCallEnum");
            AddUEExtern<DAS_BIND_FUN(SetCallEnum)>(*this, Library, "ue_arg_enum", das::SideEffects::modifyExternal, "SetCallEnum");
            AddUEExtern<DAS_BIND_FUN(GetCallName)>(*this, Library, "ue_result_name", das::SideEffects::modifyExternal, "GetCallName");
            AddUEExtern<DAS_BIND_FUN(SetCallName)>(*this, Library, "ue_arg_name", das::SideEffects::modifyExternal, "SetCallName");
            AddUEExtern<DAS_BIND_FUN(SetCallClass)>(*this,Library,"ue_arg_class",das::SideEffects::modifyExternal,"SetCallClass");
            AddUEExtern<DAS_BIND_FUN(GetCallClass)>(*this,Library,"ue_result_class",das::SideEffects::modifyExternal,"GetCallClass");
            AddUEExtern<DAS_BIND_FUN(CheckTypedClass)>(*this,Library,"__ue_class_check",das::SideEffects::accessExternal,"CheckTypedClass");
            AddUEExtern<DAS_BIND_FUN(NullClass)>(*this,Library,"ue_class_null",das::SideEffects::none,"NullClass");
            AddUEExtern<DAS_BIND_FUN(SetCallObject)>(*this, Library, "ue_arg_object", das::SideEffects::modifyExternal, "SetCallObject");
            AddUEExtern<DAS_BIND_FUN(GetCallObject), das::SimNode_ExtFuncCallAndCopyOrMove>(*this, Library, "ue_result_object", das::SideEffects::modifyExternal, "GetCallObject");
            AddUEExtern<DAS_BIND_FUN(RequireCallContext)>(*this,Library,"ue_require_call_context",das::SideEffects::modifyExternal,"RequireCallContext");
            AddUEExtern<DAS_BIND_FUN(ExpectInterfaceSignature)>(*this,Library,"ue_expect_interface_signature",das::SideEffects::modifyExternal,"ExpectInterfaceSignature");
            AddUEExtern<DAS_BIND_FUN(BeginInterfaceCall),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_begin_interface_call",das::SideEffects::modifyExternal,"BeginInterfaceCall");

            AddUEExtern<DAS_BIND_FUN(MakeVector2D),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_vector2d",das::SideEffects::none,"MakeVector2D");
            AddUEExtern<DAS_BIND_FUN(MathPackvector2d),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_math_pack_vector2d",das::SideEffects::modifyExternal,"MathPackvector2d")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(MathUnpackvector2d),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_math_unpack_vector2d",das::SideEffects::modifyExternal,"MathUnpackvector2d");
            AddUEExtern<DAS_BIND_FUN(MathPackvector),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_math_pack_vector",das::SideEffects::modifyExternal,"MathPackvector")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(MathUnpackvector),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_math_unpack_vector",das::SideEffects::modifyExternal,"MathUnpackvector");
            AddUEExtern<DAS_BIND_FUN(MathPackrotator),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_math_pack_rotator",das::SideEffects::modifyExternal,"MathPackrotator")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(MathUnpackrotator),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_math_unpack_rotator",das::SideEffects::modifyExternal,"MathUnpackrotator");
            AddUEExtern<DAS_BIND_FUN(MathPacktransform),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_math_pack_transform",das::SideEffects::modifyExternal,"MathPacktransform")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(MathUnpacktransform),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_math_unpack_transform",das::SideEffects::modifyExternal,"MathUnpacktransform");
            AddUEExtern<DAS_BIND_FUN(MethodArgvector2d)>(*this,Library,"__ue_method_arg_vector2d",das::SideEffects::modifyExternal,"MethodArgvector2d");
            AddUEExtern<DAS_BIND_FUN(MethodResultvector2d),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_method_result_vector2d",das::SideEffects::modifyExternal,"MethodResultvector2d");
            AddUEExtern<DAS_BIND_FUN(MethodArgvector)>(*this,Library,"__ue_method_arg_vector",das::SideEffects::modifyExternal,"MethodArgvector");
            AddUEExtern<DAS_BIND_FUN(MethodResultvector),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_method_result_vector",das::SideEffects::modifyExternal,"MethodResultvector");
            AddUEExtern<DAS_BIND_FUN(MethodArgrotator)>(*this,Library,"__ue_method_arg_rotator",das::SideEffects::modifyExternal,"MethodArgrotator");
            AddUEExtern<DAS_BIND_FUN(MethodResultrotator),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_method_result_rotator",das::SideEffects::modifyExternal,"MethodResultrotator");
            AddUEExtern<DAS_BIND_FUN(MethodArgtransform)>(*this,Library,"__ue_method_arg_transform",das::SideEffects::modifyExternal,"MethodArgtransform");
            AddUEExtern<DAS_BIND_FUN(MethodResulttransform),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_method_result_transform",das::SideEffects::modifyExternal,"MethodResulttransform");
            AddUEExtern<DAS_BIND_FUN(ReleaseAuthoredCall)>(*this,Library,"__ue_release_authored_call",das::SideEffects::modifyExternal,"ReleaseAuthoredCall");
            AddUEExtern<DAS_BIND_FUN(InvokeParentLifecycle)>(*this,Library,"__ue_parent_lifecycle",das::SideEffects::modifyExternal,"InvokeParentLifecycle");
            AddUEExtern<DAS_BIND_FUN(BeginAuthoredSuperCall),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_begin_super_call",das::SideEffects::modifyExternal,"BeginAuthoredSuperCall");
            AddUEExtern<DAS_BIND_FUN(BeginAuthoredCall),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_begin_authored_call",das::SideEffects::modifyExternal,"BeginAuthoredCall");
            AddUEExtern<DAS_BIND_FUN(MethodArgtext)>(*this,Library,"__ue_method_arg_text",das::SideEffects::modifyExternal,"MethodArgtext");
            AddUEExtern<DAS_BIND_FUN(MethodResulttext)>(*this,Library,"__ue_method_result_text",das::SideEffects::modifyExternal,"MethodResulttext");
            AddUEExtern<DAS_BIND_FUN(SetCallByte)>(*this,Library,"ue_arg_byte",das::SideEffects::modifyExternal,"SetCallByte");
            AddUEExtern<DAS_BIND_FUN(GetCallByte)>(*this,Library,"ue_result_byte",das::SideEffects::modifyExternal,"GetCallByte");
            AddUEExtern<DAS_BIND_FUN(MethodArgname)>(*this,Library,"__ue_method_arg_name",das::SideEffects::modifyExternal,"MethodArgname");
            AddUEExtern<DAS_BIND_FUN(MethodResultname)>(*this,Library,"__ue_method_result_name",das::SideEffects::modifyExternal,"MethodResultname");
            AddUEExtern<DAS_BIND_FUN(MethodArgenum)>(*this,Library,"__ue_method_arg_enum",das::SideEffects::modifyExternal,"MethodArgenum");
            AddUEExtern<DAS_BIND_FUN(MethodResultenum)>(*this,Library,"__ue_method_result_enum",das::SideEffects::modifyExternal,"MethodResultenum");
            AddUEExtern<DAS_BIND_FUN(MethodArgstruct)>(*this,Library,"__ue_method_arg_struct",das::SideEffects::modifyExternal,"MethodArgstruct");
            AddUEExtern<DAS_BIND_FUN(MethodResultstruct),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_method_result_struct",das::SideEffects::modifyExternal,"MethodResultstruct")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(BeginStaticCall),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_begin_static_call",das::SideEffects::modifyExternal,"BeginStaticCall");
            AddUEExtern<DAS_BIND_FUN(BeginCall), das::SimNode_ExtFuncCallAndCopyOrMove>(*this, Library, "ue_begin_call", das::SideEffects::modifyExternal, "BeginCall");
            AddUEExtern<DAS_BIND_FUN(InvokeCall)>(*this, Library, "ue_invoke_call", das::SideEffects::modifyExternal, "InvokeCall");
            AddUEExtern<DAS_BIND_FUN(ReleaseCall)>(*this, Library, "ue_release_call", das::SideEffects::modifyExternal, "ReleaseCall");
            AddUEExtern<DAS_BIND_FUN(PrepareCallHash)>(*this, Library, "ue_arg_hash", das::SideEffects::modifyExternal, "PrepareCallHash");
            AddUEExtern<DAS_BIND_FUN(GetCallHashSize)>(*this, Library, "ue_result_hash_size", das::SideEffects::modifyExternal, "GetCallHashSize");
            AddUEExtern<DAS_BIND_FUN(PrepareCallArray)>(*this, Library, "ue_arg_array", das::SideEffects::modifyExternal, "PrepareCallArray");
            AddUEExtern<DAS_BIND_FUN(GetCallArraySize)>(*this, Library, "ue_result_array_size", das::SideEffects::modifyExternal, "GetCallArraySize");
            AddUEExtern<DAS_BIND_FUN(SetCallInt)>(*this, Library, "ue_arg_int", das::SideEffects::modifyExternal, "SetCallInt");
            AddUEExtern<DAS_BIND_FUN(GetCallInt)>(*this, Library, "ue_result_int", das::SideEffects::modifyExternal, "GetCallInt");
            AddUEExtern<DAS_BIND_FUN(SetCallFloat)>(*this, Library, "ue_arg_float", das::SideEffects::modifyExternal, "SetCallFloat");
            AddUEExtern<DAS_BIND_FUN(GetCallFloat)>(*this, Library, "ue_result_float", das::SideEffects::modifyExternal, "GetCallFloat");
            AddUEExtern<DAS_BIND_FUN(SetCallDouble)>(*this, Library, "ue_arg_double", das::SideEffects::modifyExternal, "SetCallDouble");
            AddUEExtern<DAS_BIND_FUN(GetCallDouble)>(*this, Library, "ue_result_double", das::SideEffects::modifyExternal, "GetCallDouble");
            AddUEExtern<DAS_BIND_FUN(SetCallBool)>(*this, Library, "ue_arg_bool", das::SideEffects::modifyExternal, "SetCallBool");
            AddUEExtern<DAS_BIND_FUN(GetCallBool)>(*this, Library, "ue_result_bool", das::SideEffects::modifyExternal, "GetCallBool");
            AddUEExtern<DAS_BIND_FUN(SetCallString)>(*this, Library, "ue_arg_string", das::SideEffects::modifyExternal, "SetCallString");
            AddUEExtern<DAS_BIND_FUN(GetCallString)>(*this, Library, "ue_result_string", das::SideEffects::modifyExternal, "GetCallString");

            AddUEExtern<DAS_BIND_FUN(ObjectValue),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_object_value",das::SideEffects::modifyExternal,"ObjectValue");
            AddUEExtern<DAS_BIND_FUN(GetSelfObject),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_get_self_object",das::SideEffects::modifyExternal,"GetSelfObject");
            AddUEExtern<DAS_BIND_FUN(SetSelfObject)>(*this,Library,"ue_set_self_object",das::SideEffects::modifyExternal,"SetSelfObject");
            AddUEExtern<DAS_BIND_FUN(EnumValue)>(*this,Library,"ue_enum_value",das::SideEffects::modifyExternal,"EnumValue");
            AddUEExtern<DAS_BIND_FUN(EnumFromNumber)>(*this,Library,"ue_enum_from_number",das::SideEffects::modifyExternal,"EnumFromNumber");
            AddUEExtern<DAS_BIND_FUN(StructEnumGetValue)>(*this,Library,"__ue_struct_get_enum_value",das::SideEffects::modifyExternal,"StructEnumGetValue");
            AddUEExtern<DAS_BIND_FUN(StructEnumWithValue),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_struct_with_enum_value",das::SideEffects::modifyExternal,"StructEnumWithValue")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(EnumNumber)>(*this,Library,"ue_enum_number",das::SideEffects::accessExternal,"EnumNumber");
            AddUEExtern<DAS_BIND_FUN(CheckedEnumNumber)>(*this,Library,"__ue_checked_enum_number",das::SideEffects::accessExternal,"CheckedEnumNumber");
            AddUEExtern<DAS_BIND_FUN(RequireStructType)>(*this,Library,"__ue_require_struct_type",das::SideEffects::accessExternal,"RequireStructType");
            AddUEExtern<DAS_BIND_FUN(GetSelfEnum)>(*this,Library,"ue_get_self_enum",das::SideEffects::modifyExternal,"GetSelfEnum");
            AddUEExtern<DAS_BIND_FUN(SetSelfEnum)>(*this,Library,"ue_set_self_enum",das::SideEffects::modifyExternal,"SetSelfEnum");
            AddUEExtern<DAS_BIND_FUN(LibraryScope)>(*this,Library,"__ue_library_scope",das::SideEffects::modifyExternal,"LibraryScope");
            AddUEExtern<DAS_BIND_FUN(NullObject), das::SimNode_ExtFuncCallAndCopyOrMove>(*this, Library, "ue_null_object", das::SideEffects::none, "NullObject");
            AddUEExtern<DAS_BIND_FUN(ObjectSelf), das::SimNode_ExtFuncCallAndCopyOrMove>(*this, Library, "ue_self_object", das::SideEffects::modifyExternal, "ObjectSelf");
            AddUEExtern<DAS_BIND_FUN(ComponentOwner),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_component_owner",das::SideEffects::accessExternal,"ComponentOwner");
            AddUEExtern<DAS_BIND_FUN(ObjectProperty), das::SimNode_ExtFuncCallAndCopyOrMove>(*this, Library, "ue_get_object", das::SideEffects::modifyExternal, "ObjectProperty");
            AddUEExtern<DAS_BIND_FUN(ObjectCast), das::SimNode_ExtFuncCallAndCopyOrMove>(*this, Library, "ue_cast_object", das::SideEffects::modifyExternal, "ObjectCast");
            AddUEExtern<DAS_BIND_FUN(ObjectValid)>(*this, Library, "ue_object_valid", das::SideEffects::accessExternal, "ObjectValid");
            AddUEExtern<DAS_BIND_FUN(ObjectsEqual)>(*this,Library,"__ue_same_object",das::SideEffects::accessExternal,"ObjectsEqual");
            AddUEExtern<DAS_BIND_FUN(ObjectIsA)>(*this, Library, "ue_object_is_a", das::SideEffects::accessExternal, "ObjectIsA");
            AddUEExtern<DAS_BIND_FUN(AuthoredNullableCheck),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_authored_nullable_check",das::SideEffects::accessExternal,"AuthoredNullableCheck");
            AddUEExtern<DAS_BIND_FUN(AuthoredObjectCheck),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_authored_object_check",das::SideEffects::accessExternal,"AuthoredObjectCheck");
AddUEExtern<DAS_BIND_FUN(AuthoredGetint)>(*this,Library,"__ue_authored_get_int",das::SideEffects::accessExternal,"AuthoredGetint");
AddUEExtern<DAS_BIND_FUN(AuthoredSetint)>(*this,Library,"__ue_authored_set_int",das::SideEffects::modifyExternal,"AuthoredSetint");
AddUEExtern<DAS_BIND_FUN(AuthoredGetfloat)>(*this,Library,"__ue_authored_get_float",das::SideEffects::accessExternal,"AuthoredGetfloat");
AddUEExtern<DAS_BIND_FUN(AuthoredSetfloat)>(*this,Library,"__ue_authored_set_float",das::SideEffects::modifyExternal,"AuthoredSetfloat");
AddUEExtern<DAS_BIND_FUN(AuthoredGetdouble)>(*this,Library,"__ue_authored_get_double",das::SideEffects::accessExternal,"AuthoredGetdouble");
AddUEExtern<DAS_BIND_FUN(AuthoredSetdouble)>(*this,Library,"__ue_authored_set_double",das::SideEffects::modifyExternal,"AuthoredSetdouble");
AddUEExtern<DAS_BIND_FUN(AuthoredGetbool)>(*this,Library,"__ue_authored_get_bool",das::SideEffects::accessExternal,"AuthoredGetbool");
AddUEExtern<DAS_BIND_FUN(AuthoredSetbool)>(*this,Library,"__ue_authored_set_bool",das::SideEffects::modifyExternal,"AuthoredSetbool");
AddUEExtern<DAS_BIND_FUN(AuthoredGetstring)>(*this,Library,"__ue_authored_get_string",das::SideEffects::modifyExternal,"AuthoredGetstring");
AddUEExtern<DAS_BIND_FUN(AuthoredSetstring)>(*this,Library,"__ue_authored_set_string",das::SideEffects::modifyExternal,"AuthoredSetstring");
AddUEExtern<DAS_BIND_FUN(AuthoredGetname)>(*this,Library,"__ue_authored_get_name",das::SideEffects::modifyExternal,"AuthoredGetname");
AddUEExtern<DAS_BIND_FUN(AuthoredSetname)>(*this,Library,"__ue_authored_set_name",das::SideEffects::modifyExternal,"AuthoredSetname");
            AddUEExtern<DAS_BIND_FUN(AuthoredGetenum)>(*this,Library,"__ue_authored_get_enum",das::SideEffects::modifyExternal,"AuthoredGetenum");
AddUEExtern<DAS_BIND_FUN(AuthoredSetenum)>(*this,Library,"__ue_authored_set_enum",das::SideEffects::modifyExternal,"AuthoredSetenum");
AddUEExtern<DAS_BIND_FUN(AuthoredGetstruct),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_authored_get_struct",das::SideEffects::modifyExternal,"AuthoredGetstruct")->needCallerStackFrame=true;
AddUEExtern<DAS_BIND_FUN(AuthoredSetstruct)>(*this,Library,"__ue_authored_set_struct",das::SideEffects::modifyExternal,"AuthoredSetstruct");
AddUEExtern<DAS_BIND_FUN(AuthoredGetobject),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_authored_get_object",das::SideEffects::modifyExternal,"AuthoredGetobject");
AddUEExtern<DAS_BIND_FUN(AuthoredSetobject)>(*this,Library,"__ue_authored_set_object",das::SideEffects::modifyExternal,"AuthoredSetobject");
            AddUEExtern<DAS_BIND_FUN(ObjectGetInt)>(*this, Library, "ue_object_get_int", das::SideEffects::accessExternal, "ObjectGetInt");
            AddUEExtern<DAS_BIND_FUN(ObjectSetInt)>(*this, Library, "ue_object_set_int", das::SideEffects::modifyExternal, "ObjectSetInt");
            AddUEExtern<DAS_BIND_FUN(CallInt)>(*this, Library, "ue_call_int", das::SideEffects::modifyExternal, "CallInt");
            AddUEExtern<DAS_BIND_FUN(CallFloat)>(*this, Library, "ue_call_float", das::SideEffects::modifyExternal, "CallFloat");
            AddUEExtern<DAS_BIND_FUN(CallVoidInt)>(*this, Library, "ue_call_void_int", das::SideEffects::modifyExternal, "CallVoidInt");
            AddUEExtern<DAS_BIND_FUN(CallObject), das::SimNode_ExtFuncCallAndCopyOrMove>(*this, Library, "ue_call_object", das::SideEffects::modifyExternal, "CallObject");
            AddUEExtern<DAS_BIND_FUN(CallVoidObject)>(*this, Library, "ue_call_void_object", das::SideEffects::modifyExternal, "CallVoidObject");
            AddUEExtern<DAS_BIND_FUN(SubscribeObjectDouble)>(*this,Library,"ue_subscribe_object_double",das::SideEffects::modifyExternal,"SubscribeObjectDouble");
            AddUEExtern<DAS_BIND_FUN(SubscribeObjectTyped)>(*this,Library,"ue_subscribe_object_object",das::SideEffects::modifyExternal,"SubscribeObjectTyped");
            AddUEExtern<DAS_BIND_FUN(SubscribeObjectObject)>(*this,Library,"ue_subscribe_object_object",das::SideEffects::modifyExternal,"SubscribeObjectObject");
            AddUEExtern<DAS_BIND_FUN(SubscribeObjectRotator)>(*this,Library,"ue_subscribe_object_rotator",das::SideEffects::modifyExternal,"SubscribeObjectRotator");
            AddUEExtern<DAS_BIND_FUN(SubscribeObjectTransform)>(*this,Library,"ue_subscribe_object_transform",das::SideEffects::modifyExternal,"SubscribeObjectTransform");
            AddUEExtern<DAS_BIND_FUN(SubscribeObjectName)>(*this,Library,"ue_subscribe_object_name",das::SideEffects::modifyExternal,"SubscribeObjectName");
            AddUEExtern<DAS_BIND_FUN(SubscribeObjectArrayInt)>(*this,Library,"ue_subscribe_object_array_int",das::SideEffects::modifyExternal,"SubscribeObjectArrayInt");
            AddUEExtern<DAS_BIND_FUN(SubscribeObjectArrayDouble)>(*this,Library,"ue_subscribe_object_array_double",das::SideEffects::modifyExternal,"SubscribeObjectArrayDouble");
            AddUEExtern<DAS_BIND_FUN(SubscribeObjectArrayString)>(*this,Library,"ue_subscribe_object_array_string",das::SideEffects::modifyExternal,"SubscribeObjectArrayString");
            AddUEExtern<DAS_BIND_FUN(SubscribeObjectVector2d)>(*this,Library,"ue_subscribe_object_vector2d",das::SideEffects::modifyExternal,"SubscribeObjectVector2d");
            AddUEExtern<DAS_BIND_FUN(SceneValid)>(*this,Library,"__ue_scene_valid",das::SideEffects::modifyExternal,"SceneValid");
            AddUEExtern<DAS_BIND_FUN(ActorOwner),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_actor_owner",das::SideEffects::modifyExternal,"ActorOwner");
            AddUEExtern<DAS_BIND_FUN(SceneActorRoot),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_scene_actor_root",das::SideEffects::modifyExternal,"SceneActorRoot");
            AddUEExtern<DAS_BIND_FUN(SceneParent),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_scene_parent",das::SideEffects::modifyExternal,"SceneParent");
            AddUEExtern<DAS_BIND_FUN(SceneRelative),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_scene_relative_transform",das::SideEffects::modifyExternal,"SceneRelative");
            AddUEExtern<DAS_BIND_FUN(SceneWorld),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_scene_world_transform",das::SideEffects::modifyExternal,"SceneWorld");
            AddUEExtern<DAS_BIND_FUN(SceneRelativeTranslation),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_scene_relative_translation",das::SideEffects::modifyExternal,"SceneRelativeTranslation");
            AddUEExtern<DAS_BIND_FUN(SceneRelativeRotation),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_scene_relative_rotation",das::SideEffects::modifyExternal,"SceneRelativeRotation");
            AddUEExtern<DAS_BIND_FUN(SceneRelativeScale),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_scene_relative_scale",das::SideEffects::modifyExternal,"SceneRelativeScale");
            AddUEExtern<DAS_BIND_FUN(SceneSetRelative)>(*this,Library,"__ue_scene_set_relative_transform",das::SideEffects::modifyExternal,"SceneSetRelative");
            AddUEExtern<DAS_BIND_FUN(SceneSetWorld)>(*this,Library,"__ue_scene_set_world_transform",das::SideEffects::modifyExternal,"SceneSetWorld");
            AddUEExtern<DAS_BIND_FUN(SceneAttach)>(*this,Library,"__ue_scene_attach",das::SideEffects::modifyExternal,"SceneAttach");
            AddUEExtern<DAS_BIND_FUN(SceneDetach)>(*this,Library,"__ue_scene_detach",das::SideEffects::modifyExternal,"SceneDetach");
            AddUEExtern<DAS_BIND_FUN(SceneCreate),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_scene_create",das::SideEffects::modifyExternal,"SceneCreate");
            AddUEExtern<DAS_BIND_FUN(SceneGet),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_scene_get",das::SideEffects::modifyExternal,"SceneGet");
            AddUEExtern<DAS_BIND_FUN(SceneGetOrCreate),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_scene_get_or_create",das::SideEffects::modifyExternal,"SceneGetOrCreate");
            AddUEExtern<DAS_BIND_FUN(ClassLoaded)>(*this,Library,"__ue_class_loaded",das::SideEffects::accessExternal,"ClassLoaded");
            AddUEExtern<DAS_BIND_FUN(AuthoredGetclass)>(*this,Library,"__ue_authored_get_class",das::SideEffects::accessExternal,"AuthoredGetclass");
            AddUEExtern<DAS_BIND_FUN(AuthoredSetclass)>(*this,Library,"__ue_authored_set_class",das::SideEffects::modifyExternal,"AuthoredSetclass");
            AddUEExtern<DAS_BIND_FUN(GetSelfClass)>(*this,Library,"ue_get_self_class",das::SideEffects::accessExternal,"GetSelfClass");
            AddUEExtern<DAS_BIND_FUN(SetSelfClass)>(*this,Library,"ue_set_self_class",das::SideEffects::modifyExternal,"SetSelfClass");
            AddUEExtern<DAS_BIND_FUN(ActorSpawn),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_actor_spawn",das::SideEffects::modifyExternal,"ActorSpawn");
            AddUEExtern<DAS_BIND_FUN(ActorDestroy)>(*this,Library,"ue_actor_destroy_handle",das::SideEffects::modifyExternal,"ActorDestroy");
            AddUEExtern<DAS_BIND_FUN(ObjectFindByType),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_find_by_type",das::SideEffects::accessExternal,"ObjectFindByType");
            AddUEExtern<DAS_BIND_FUN(ObjectQueryBegin)>(*this,Library,"__ue_query_begin",das::SideEffects::modifyExternal,"ObjectQueryBegin");
            AddUEExtern<DAS_BIND_FUN(ObjectQueryCount)>(*this,Library,"__ue_query_count",das::SideEffects::accessExternal,"ObjectQueryCount");
            AddUEExtern<DAS_BIND_FUN(ObjectQueryGet),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_query_get",das::SideEffects::modifyExternal,"ObjectQueryGet");
            AddUEExtern<DAS_BIND_FUN(ObjectQueryRelease)>(*this,Library,"__ue_query_release",das::SideEffects::modifyExternal,"ObjectQueryRelease");
            AddUEExtern<DAS_BIND_FUN(ClassValid)>(*this,Library,"ue_class_valid",das::SideEffects::accessExternal,"ClassValid");
            AddUEExtern<DAS_BIND_FUN(ClassPath)>(*this,Library,"ue_class_path",das::SideEffects::accessExternal,"ClassPath");
            AddUEExtern<DAS_BIND_FUN(PathComponentOperation),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_class_component_operation",das::SideEffects::modifyExternal,"PathComponentOperation");
            AddUEExtern<DAS_BIND_FUN(ClassComponentOperation),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_class_component_operation",das::SideEffects::modifyExternal,"ClassComponentOperation");
            AddUEExtern<DAS_BIND_FUN(NativeComponentOperation),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_native_component_operation",das::SideEffects::modifyExternal,"NativeComponentOperation");
            AddUEExtern<DAS_BIND_FUN(AuthoredComponentGet),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_authored_component_get",das::SideEffects::modifyExternal,"AuthoredComponentGet");
            AddUEExtern<DAS_BIND_FUN(NativeComponentDestroy)>(*this,Library,"ue_component_destroy_handle",das::SideEffects::modifyExternal,"NativeComponentDestroy");
            AddUEExtern<DAS_BIND_FUN(SceneDestroy)>(*this,Library,"__ue_scene_destroy",das::SideEffects::modifyExternal,"SceneDestroy");
            AddUEExtern<DAS_BIND_FUN(SubscribeObjectVector)>(*this,Library,"ue_subscribe_object_vector",das::SideEffects::modifyExternal,"SubscribeObjectVector");
            AddUEExtern<DAS_BIND_FUN(SubscribeObjectFloat)>(*this,Library,"ue_subscribe_object_float",das::SideEffects::modifyExternal,"SubscribeObjectFloat");
            AddUEExtern<DAS_BIND_FUN(SubscribeObjectBool)>(*this,Library,"ue_subscribe_object_bool",das::SideEffects::modifyExternal,"SubscribeObjectBool");
            AddUEExtern<DAS_BIND_FUN(SubscribeObjectString)>(*this,Library,"ue_subscribe_object_string",das::SideEffects::modifyExternal,"SubscribeObjectString");
            AddUEExtern<DAS_BIND_FUN(SubscribeObjectVoid)>(*this,Library,"ue_subscribe_object_void",das::SideEffects::modifyExternal,"SubscribeObjectVoid");
            AddUEExtern<DAS_BIND_FUN(SubscribeObjectByte)>(*this,Library,"ue_subscribe_object_byte",das::SideEffects::modifyExternal,"SubscribeObjectByte");
            AddUEExtern<DAS_BIND_FUN(SubscribeObjectText)>(*this,Library,"ue_subscribe_object_text",das::SideEffects::modifyExternal,"SubscribeObjectText");
            AddUEExtern<DAS_BIND_FUN(SubscribeObjectArrayFloat)>(*this,Library,"ue_subscribe_object_array_float",das::SideEffects::modifyExternal,"SubscribeObjectArrayFloat");
            AddUEExtern<DAS_BIND_FUN(SubscribeObjectArrayBool)>(*this,Library,"ue_subscribe_object_array_bool",das::SideEffects::modifyExternal,"SubscribeObjectArrayBool");
            AddUEExtern<DAS_BIND_FUN(SubscribeObjectArrayName)>(*this,Library,"ue_subscribe_object_array_name",das::SideEffects::modifyExternal,"SubscribeObjectArrayName");
            AddUEExtern<DAS_BIND_FUN(SubscribeObjectEnum)>(*this,Library,"ue_subscribe_object_enum",das::SideEffects::modifyExternal,"SubscribeObjectEnum");
            AddUEExtern<DAS_BIND_FUN(SubscribeObjectClass)>(*this,Library,"ue_subscribe_object_class",das::SideEffects::modifyExternal,"SubscribeObjectClass");
            AddUEExtern<DAS_BIND_FUN(SubscribeObjectStruct)>(*this,Library,"ue_subscribe_object_struct",das::SideEffects::modifyExternal,"SubscribeObjectStruct");
            AddUEExtern<DAS_BIND_FUN(SubscribeObjectArrayByte)>(*this,Library,"ue_subscribe_object_array_byte",das::SideEffects::modifyExternal,"SubscribeObjectArrayByte");
            AddUEExtern<DAS_BIND_FUN(SubscribeObjectArrayText)>(*this,Library,"ue_subscribe_object_array_text",das::SideEffects::modifyExternal,"SubscribeObjectArrayText");
            AddUEExtern<DAS_BIND_FUN(SubscribeObjectInt)>(*this, Library, "ue_subscribe_object_int", das::SideEffects::modifyExternal, "SubscribeObjectInt");

            AddUEExtern<DAS_BIND_FUN(SubscribeInt)>(*this, Library, "ue_subscribe_int", das::SideEffects::modifyExternal, "SubscribeInt");
            AddUEExtern<DAS_BIND_FUN(EmitSignal)>(*this, Library, "ue_emit_signal", das::SideEffects::modifyExternal, "EmitSignal");
            AddUEExtern<DAS_BIND_FUN(GetStateInt)>(*this, Library, "ue_state_get_int", das::SideEffects::modifyExternal, "GetStateInt");
            AddUEExtern<DAS_BIND_FUN(SetStateInt)>(*this, Library, "ue_state_set_int", das::SideEffects::modifyExternal, "SetStateInt");
            AddUEExtern<DAS_BIND_FUN(NativeAdd)>(*this, Library, "ue_add", das::SideEffects::none, "NativeAdd");
            AddUEExtern<DAS_BIND_FUN(DSLVector),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_vector",das::SideEffects::none,"DSLVector");
            AddUEExtern<DAS_BIND_FUN(DSLRotator),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_rotator",das::SideEffects::none,"DSLRotator");
            AddUEExtern<DAS_BIND_FUN(DSLTransform),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_transform",das::SideEffects::none,"DSLTransform");
            AddUEExtern<DAS_BIND_FUN(DSLIdentityTransform),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_transform_identity",das::SideEffects::none,"DSLIdentityTransform");
            AddUEExtern<DAS_BIND_FUN(DSLTransformPosition),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_transform_position",das::SideEffects::none,"DSLTransformPosition");
            AddUEExtern<DAS_BIND_FUN(DSLTranslation),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_transform_translation",das::SideEffects::none,"DSLTranslation");
            AddUEExtern<DAS_BIND_FUN(DSLScale),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_transform_scale",das::SideEffects::none,"DSLScale");
            AddUEExtern<DAS_BIND_FUN(DSLRotation),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_transform_rotation",das::SideEffects::none,"DSLRotation");
            AddUEExtern<DAS_BIND_FUN(ExplicitStructCollect)>(*this,Library,"ue_struct_collect",das::SideEffects::modifyExternal,"ExplicitStructCollect")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(StructValueCount)>(*this,Library,"ue_struct_value_count",das::SideEffects::accessExternal,"StructValueCount");
            AddUEExtern<DAS_BIND_FUN(EditStructValue),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_edit",das::SideEffects::modifyExternal,"EditStructValue")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(NewStructDefault),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_default",das::SideEffects::modifyExternal,"NewStructDefault")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(GetSelfStruct),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_get_self_struct",das::SideEffects::modifyExternal,"GetSelfStruct")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(SetSelfStruct)>(*this,Library,"ue_set_self_struct",das::SideEffects::modifyExternal,"SetSelfStruct");
            AddUEExtern<DAS_BIND_FUN(NewStructValue),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_value",das::SideEffects::modifyExternal,"NewStructValue")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(StructSetSize)>(*this,Library,"ue_struct_set_size",das::SideEffects::modifyExternal,"StructSetSize");
            AddUEExtern<DAS_BIND_FUN(StructSetElementInt)>(*this,Library,"ue_struct_set_element_int",das::SideEffects::modifyExternal,"StructSetElementInt");
            AddUEExtern<DAS_BIND_FUN(StructSetElementString)>(*this,Library,"ue_struct_set_element_string",das::SideEffects::modifyExternal,"StructSetElementString");
            AddUEExtern<DAS_BIND_FUN(StructSetContainsInt)>(*this,Library,"ue_struct_set_contains_int",das::SideEffects::modifyExternal,"StructSetContainsInt");
            AddUEExtern<DAS_BIND_FUN(StructSetContainsString)>(*this,Library,"ue_struct_set_contains_string",das::SideEffects::modifyExternal,"StructSetContainsString");
            AddUEExtern<DAS_BIND_FUN(StructSetWithInt),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_set_with_int",das::SideEffects::modifyExternal,"StructSetWithInt")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(StructSetWithString),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_set_with_string",das::SideEffects::modifyExternal,"StructSetWithString")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(StructMapGetIntEnum)>(*this,Library,"__ue_struct_map_get_int_enum",das::SideEffects::modifyExternal,"StructMapGetIntEnum");
            AddUEExtern<DAS_BIND_FUN(StructMapWithIntEnum),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_struct_map_with_int_enum",das::SideEffects::modifyExternal,"StructMapWithIntEnum")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(StructMapGetIntObject),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_struct_map_get_int_object",das::SideEffects::modifyExternal,"StructMapGetIntObject");
            AddUEExtern<DAS_BIND_FUN(StructMapWithIntObject),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_struct_map_with_int_object",das::SideEffects::modifyExternal,"StructMapWithIntObject")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(StructMapGetStringEnum)>(*this,Library,"__ue_struct_map_get_string_enum",das::SideEffects::modifyExternal,"StructMapGetStringEnum");
            AddUEExtern<DAS_BIND_FUN(StructMapWithStringEnum),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_struct_map_with_string_enum",das::SideEffects::modifyExternal,"StructMapWithStringEnum")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(StructMapGetStringObject),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_struct_map_get_string_object",das::SideEffects::modifyExternal,"StructMapGetStringObject");
            AddUEExtern<DAS_BIND_FUN(StructMapWithStringObject),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_struct_map_with_string_object",das::SideEffects::modifyExternal,"StructMapWithStringObject")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(StructMapGetIntStruct),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_struct_map_get_int_struct",das::SideEffects::modifyExternal,"StructMapGetIntStruct")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(StructMapWithIntStruct),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_struct_map_with_int_struct",das::SideEffects::modifyExternal,"StructMapWithIntStruct")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(StructMapGetStringStruct),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_struct_map_get_string_struct",das::SideEffects::modifyExternal,"StructMapGetStringStruct")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(StructMapWithStringStruct),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_struct_map_with_string_struct",das::SideEffects::modifyExternal,"StructMapWithStringStruct")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(StructMapGetIntFloat)>(*this,Library,"ue_struct_map_get_int_float",das::SideEffects::modifyExternal,"StructMapGetIntFloat");
            AddUEExtern<DAS_BIND_FUN(StructMapWithIntFloat),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_map_with_int_float",das::SideEffects::modifyExternal,"StructMapWithIntFloat")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(StructMapGetIntDouble)>(*this,Library,"ue_struct_map_get_int_double",das::SideEffects::modifyExternal,"StructMapGetIntDouble");
            AddUEExtern<DAS_BIND_FUN(StructMapWithIntDouble),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_map_with_int_double",das::SideEffects::modifyExternal,"StructMapWithIntDouble")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(StructMapGetIntBool)>(*this,Library,"ue_struct_map_get_int_bool",das::SideEffects::modifyExternal,"StructMapGetIntBool");
            AddUEExtern<DAS_BIND_FUN(StructMapWithIntBool),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_map_with_int_bool",das::SideEffects::modifyExternal,"StructMapWithIntBool")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(StructMapGetIntName)>(*this,Library,"ue_struct_map_get_int_name",das::SideEffects::modifyExternal,"StructMapGetIntName");
            AddUEExtern<DAS_BIND_FUN(StructMapWithIntName),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_map_with_int_name",das::SideEffects::modifyExternal,"StructMapWithIntName")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(StructMapGetStringFloat)>(*this,Library,"ue_struct_map_get_string_float",das::SideEffects::modifyExternal,"StructMapGetStringFloat");
            AddUEExtern<DAS_BIND_FUN(StructMapWithStringFloat),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_map_with_string_float",das::SideEffects::modifyExternal,"StructMapWithStringFloat")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(StructMapGetStringDouble)>(*this,Library,"ue_struct_map_get_string_double",das::SideEffects::modifyExternal,"StructMapGetStringDouble");
            AddUEExtern<DAS_BIND_FUN(StructMapWithStringDouble),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_map_with_string_double",das::SideEffects::modifyExternal,"StructMapWithStringDouble")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(StructMapGetStringBool)>(*this,Library,"ue_struct_map_get_string_bool",das::SideEffects::modifyExternal,"StructMapGetStringBool");
            AddUEExtern<DAS_BIND_FUN(StructMapWithStringBool),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_map_with_string_bool",das::SideEffects::modifyExternal,"StructMapWithStringBool")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(StructMapGetStringName)>(*this,Library,"ue_struct_map_get_string_name",das::SideEffects::modifyExternal,"StructMapGetStringName");
            AddUEExtern<DAS_BIND_FUN(StructMapWithStringName),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_map_with_string_name",das::SideEffects::modifyExternal,"StructMapWithStringName")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(PublishFrameInt)>(*this,Library,"ue_frame_out_int",das::SideEffects::modifyExternal,"PublishFrameInt");
            AddUEExtern<DAS_BIND_FUN(PublishFrameFloat)>(*this,Library,"ue_frame_out_float",das::SideEffects::modifyExternal,"PublishFrameFloat");
            AddUEExtern<DAS_BIND_FUN(PublishFrameBool)>(*this,Library,"ue_frame_out_bool",das::SideEffects::modifyExternal,"PublishFrameBool");
            AddUEExtern<DAS_BIND_FUN(PublishFrameString)>(*this,Library,"ue_frame_out_string",das::SideEffects::modifyExternal,"PublishFrameString");
            AddUEExtern<DAS_BIND_FUN(PublishFrameDouble)>(*this,Library,"ue_frame_out_double",das::SideEffects::modifyExternal,"PublishFrameDouble");
            AddUEExtern<DAS_BIND_FUN(AuthoredGetbyte)>(*this,Library,"__ue_authored_get_byte",das::SideEffects::modifyExternal,"AuthoredGetbyte");
            AddUEExtern<DAS_BIND_FUN(AuthoredSetbyte)>(*this,Library,"__ue_authored_set_byte",das::SideEffects::modifyExternal,"AuthoredSetbyte");
            AddUEExtern<DAS_BIND_FUN(AuthoredGettext)>(*this,Library,"__ue_authored_get_text",das::SideEffects::modifyExternal,"AuthoredGettext");
            AddUEExtern<DAS_BIND_FUN(AuthoredSettext)>(*this,Library,"__ue_authored_set_text",das::SideEffects::modifyExternal,"AuthoredSettext");
            AddUEExtern<DAS_BIND_FUN(GetSelfByte)>(*this,Library,"ue_get_self_byte",das::SideEffects::modifyExternal,"GetSelfByte");
            AddUEExtern<DAS_BIND_FUN(SetSelfByte)>(*this,Library,"ue_set_self_byte",das::SideEffects::modifyExternal,"SetSelfByte");
            AddUEExtern<DAS_BIND_FUN(GetSelfText)>(*this,Library,"ue_get_self_text",das::SideEffects::modifyExternal,"GetSelfText");
            AddUEExtern<DAS_BIND_FUN(SetSelfText)>(*this,Library,"ue_set_self_text",das::SideEffects::modifyExternal,"SetSelfText");
            AddUEExtern<DAS_BIND_FUN(StructMapKeyByte)>(*this,Library,"ue_struct_map_key_byte",das::SideEffects::modifyExternal,"StructMapKeyByte");
            AddUEExtern<DAS_BIND_FUN(StructSetElementByte)>(*this,Library,"ue_struct_set_element_byte",das::SideEffects::modifyExternal,"StructSetElementByte");
            AddUEExtern<DAS_BIND_FUN(StructSetWithByte),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_set_with_byte",das::SideEffects::modifyExternal,"StructSetWithByte")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(GetTextByteMapIntByte)>(*this,Library,"ue_struct_map_get_int_byte",das::SideEffects::modifyExternal,"GetTextByteMapIntByte");
            AddUEExtern<DAS_BIND_FUN(WithTextByteMapIntByte),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_map_with_int_byte",das::SideEffects::modifyExternal,"WithTextByteMapIntByte")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(GetTextByteMapIntText)>(*this,Library,"ue_struct_map_get_int_text",das::SideEffects::modifyExternal,"GetTextByteMapIntText");
            AddUEExtern<DAS_BIND_FUN(WithTextByteMapIntText),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_map_with_int_text",das::SideEffects::modifyExternal,"WithTextByteMapIntText")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(GetTextByteMapStringByte)>(*this,Library,"ue_struct_map_get_string_byte",das::SideEffects::modifyExternal,"GetTextByteMapStringByte");
            AddUEExtern<DAS_BIND_FUN(WithTextByteMapStringByte),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_map_with_string_byte",das::SideEffects::modifyExternal,"WithTextByteMapStringByte")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(GetTextByteMapStringText)>(*this,Library,"ue_struct_map_get_string_text",das::SideEffects::modifyExternal,"GetTextByteMapStringText");
            AddUEExtern<DAS_BIND_FUN(WithTextByteMapStringText),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_map_with_string_text",das::SideEffects::modifyExternal,"WithTextByteMapStringText")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(GetTextByteMapNameByte)>(*this,Library,"ue_struct_map_get_name_byte",das::SideEffects::modifyExternal,"GetTextByteMapNameByte");
            AddUEExtern<DAS_BIND_FUN(WithTextByteMapNameByte),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_map_with_name_byte",das::SideEffects::modifyExternal,"WithTextByteMapNameByte")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(GetTextByteMapNameText)>(*this,Library,"ue_struct_map_get_name_text",das::SideEffects::modifyExternal,"GetTextByteMapNameText");
            AddUEExtern<DAS_BIND_FUN(WithTextByteMapNameText),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_map_with_name_text",das::SideEffects::modifyExternal,"WithTextByteMapNameText")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(GetTextByteMapObjectByte)>(*this,Library,"__ue_struct_map_get_object_byte",das::SideEffects::modifyExternal,"GetTextByteMapObjectByte");
            AddUEExtern<DAS_BIND_FUN(WithTextByteMapObjectByte),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_struct_map_with_object_byte",das::SideEffects::modifyExternal,"WithTextByteMapObjectByte")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(GetTextByteMapObjectText)>(*this,Library,"__ue_struct_map_get_object_text",das::SideEffects::modifyExternal,"GetTextByteMapObjectText");
            AddUEExtern<DAS_BIND_FUN(WithTextByteMapObjectText),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_struct_map_with_object_text",das::SideEffects::modifyExternal,"WithTextByteMapObjectText")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(GetTextByteMapEnumByte)>(*this,Library,"__ue_struct_map_get_enum_byte",das::SideEffects::modifyExternal,"GetTextByteMapEnumByte");
            AddUEExtern<DAS_BIND_FUN(WithTextByteMapEnumByte),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_struct_map_with_enum_byte",das::SideEffects::modifyExternal,"WithTextByteMapEnumByte")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(GetTextByteMapEnumText)>(*this,Library,"__ue_struct_map_get_enum_text",das::SideEffects::modifyExternal,"GetTextByteMapEnumText");
            AddUEExtern<DAS_BIND_FUN(WithTextByteMapEnumText),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_struct_map_with_enum_text",das::SideEffects::modifyExternal,"WithTextByteMapEnumText")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(GetTextByteMapByteByte)>(*this,Library,"ue_struct_map_get_byte_byte",das::SideEffects::modifyExternal,"GetTextByteMapByteByte");
            AddUEExtern<DAS_BIND_FUN(WithTextByteMapByteByte),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_map_with_byte_byte",das::SideEffects::modifyExternal,"WithTextByteMapByteByte")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(GetTextByteMapByteText)>(*this,Library,"ue_struct_map_get_byte_text",das::SideEffects::modifyExternal,"GetTextByteMapByteText");
            AddUEExtern<DAS_BIND_FUN(WithTextByteMapByteText),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_map_with_byte_text",das::SideEffects::modifyExternal,"WithTextByteMapByteText")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(GetTextByteMapByteInt)>(*this,Library,"ue_struct_map_get_byte_int",das::SideEffects::modifyExternal,"GetTextByteMapByteInt");
            AddUEExtern<DAS_BIND_FUN(WithTextByteMapByteInt),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_map_with_byte_int",das::SideEffects::modifyExternal,"WithTextByteMapByteInt")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(GetTextByteMapByteFloat)>(*this,Library,"ue_struct_map_get_byte_float",das::SideEffects::modifyExternal,"GetTextByteMapByteFloat");
            AddUEExtern<DAS_BIND_FUN(WithTextByteMapByteFloat),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_map_with_byte_float",das::SideEffects::modifyExternal,"WithTextByteMapByteFloat")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(GetTextByteMapByteDouble)>(*this,Library,"ue_struct_map_get_byte_double",das::SideEffects::modifyExternal,"GetTextByteMapByteDouble");
            AddUEExtern<DAS_BIND_FUN(WithTextByteMapByteDouble),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_map_with_byte_double",das::SideEffects::modifyExternal,"WithTextByteMapByteDouble")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(GetTextByteMapByteBool)>(*this,Library,"ue_struct_map_get_byte_bool",das::SideEffects::modifyExternal,"GetTextByteMapByteBool");
            AddUEExtern<DAS_BIND_FUN(WithTextByteMapByteBool),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_map_with_byte_bool",das::SideEffects::modifyExternal,"WithTextByteMapByteBool")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(GetTextByteMapByteString)>(*this,Library,"ue_struct_map_get_byte_string",das::SideEffects::modifyExternal,"GetTextByteMapByteString");
            AddUEExtern<DAS_BIND_FUN(WithTextByteMapByteString),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_map_with_byte_string",das::SideEffects::modifyExternal,"WithTextByteMapByteString")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(GetTextByteMapByteName)>(*this,Library,"ue_struct_map_get_byte_name",das::SideEffects::modifyExternal,"GetTextByteMapByteName");
            AddUEExtern<DAS_BIND_FUN(WithTextByteMapByteName),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_map_with_byte_name",das::SideEffects::modifyExternal,"WithTextByteMapByteName")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(GetTextByteMapByteStruct),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_struct_map_get_byte_struct",das::SideEffects::modifyExternal,"GetTextByteMapByteStruct")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(WithTextByteMapByteStruct),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_struct_map_with_byte_struct",das::SideEffects::modifyExternal,"WithTextByteMapByteStruct")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(GetTextByteMapByteEnum)>(*this,Library,"__ue_struct_map_get_byte_enum",das::SideEffects::modifyExternal,"GetTextByteMapByteEnum");
            AddUEExtern<DAS_BIND_FUN(WithTextByteMapByteEnum),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_struct_map_with_byte_enum",das::SideEffects::modifyExternal,"WithTextByteMapByteEnum")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(GetTextByteMapByteObject),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_struct_map_get_byte_object",das::SideEffects::modifyExternal,"GetTextByteMapByteObject");
            AddUEExtern<DAS_BIND_FUN(WithTextByteMapByteObject),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_struct_map_with_byte_object",das::SideEffects::modifyExternal,"WithTextByteMapByteObject")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(StructGetbyte)>(*this,Library,"ue_struct_get_byte",das::SideEffects::modifyExternal,"StructGetbyte");
            AddUEExtern<DAS_BIND_FUN(StructGettext)>(*this,Library,"ue_struct_get_text",das::SideEffects::modifyExternal,"StructGettext");
            AddUEExtern<DAS_BIND_FUN(PublishFrameByte)>(*this,Library,"ue_frame_out_byte",das::SideEffects::modifyExternal,"PublishFrameByte");
            AddUEExtern<DAS_BIND_FUN(PublishFrameText)>(*this,Library,"ue_frame_out_text",das::SideEffects::modifyExternal,"PublishFrameText");
            AddUEExtern<DAS_BIND_FUN(StructWithbyte),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_with_byte",das::SideEffects::modifyExternal,"StructWithbyte")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(StructWithtext),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_with_text",das::SideEffects::modifyExternal,"StructWithtext")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(PublishFrameName)>(*this,Library,"ue_frame_out_name",das::SideEffects::modifyExternal,"PublishFrameName");
            AddUEExtern<DAS_BIND_FUN(PublishFrameObject)>(*this,Library,"ue_frame_out_object",das::SideEffects::modifyExternal,"PublishFrameObject");
            AddUEExtern<DAS_BIND_FUN(PublishFrameClass)>(*this,Library,"ue_frame_out_class",das::SideEffects::modifyExternal,"PublishFrameClass");
            AddUEExtern<DAS_BIND_FUN(PublishFrameEnum)>(*this,Library,"ue_frame_out_enum",das::SideEffects::modifyExternal,"PublishFrameEnum");
            AddUEExtern<DAS_BIND_FUN(PublishFrameVector)>(*this,Library,"ue_frame_out_vector",das::SideEffects::modifyExternal,"PublishFrameVector");
            AddUEExtern<DAS_BIND_FUN(PublishFrameRotator)>(*this,Library,"ue_frame_out_rotator",das::SideEffects::modifyExternal,"PublishFrameRotator");
            AddUEExtern<DAS_BIND_FUN(PublishFrameTransform)>(*this,Library,"ue_frame_out_transform",das::SideEffects::modifyExternal,"PublishFrameTransform");
            AddUEExtern<DAS_BIND_FUN(PublishFrameStruct)>(*this,Library,"ue_frame_out_struct",das::SideEffects::modifyExternal,"PublishFrameStruct");
            AddUEExtern<DAS_BIND_FUN(CastInterface),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_cast_interface",das::SideEffects::modifyExternal,"CastInterface");
            AddUEExtern<DAS_BIND_FUN(ObjectImplementsInterface)>(*this,Library,"ue_implements_interface",das::SideEffects::modifyExternal,"ObjectImplementsInterface");
            AddUEExtern<DAS_BIND_FUN(InterfaceCallInt)>(*this,Library,"ue_interface_call_int",das::SideEffects::modifyExternal,"InterfaceCallInt");
            AddUEExtern<DAS_BIND_FUN(InterfaceCallString)>(*this,Library,"ue_interface_call_string",das::SideEffects::modifyExternal,"InterfaceCallString");
            AddUEExtern<DAS_BIND_FUN(StructMapSize)>(*this,Library,"ue_struct_map_size",das::SideEffects::modifyExternal,"StructMapSize");
            AddUEExtern<DAS_BIND_FUN(StructMapKey)>(*this,Library,"ue_struct_map_key",das::SideEffects::modifyExternal,"StructMapKey");
            AddUEExtern<DAS_BIND_FUN(StructMapGetInt)>(*this,Library,"ue_struct_map_get_int",das::SideEffects::modifyExternal,"StructMapGetInt");
            AddUEExtern<DAS_BIND_FUN(StructMapWithInt),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_map_with_int",das::SideEffects::modifyExternal,"StructMapWithInt")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(ObjectKeyFromReference)>(*this,Library,"__ue_object_key_from_ref",das::SideEffects::modifyExternal,"ObjectKeyFromReference");
        AddUEExtern<DAS_BIND_FUN(CheckedObjectKey)>(*this,Library,"__ue_object_key_check",das::SideEffects::modifyExternal,"CheckedObjectKey");
        AddUEExtern<DAS_BIND_FUN(ObjectKeyReference),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_object_key_reference",das::SideEffects::modifyExternal,"ObjectKeyReference");
        AddUEExtern<DAS_BIND_FUN(ObjectKeyValid)>(*this,Library,"__ue_object_key_valid",das::SideEffects::modifyExternal,"ObjectKeyValid");
        AddUEExtern<DAS_BIND_FUN(StructSetElementObjectKey)>(*this,Library,"__ue_struct_set_element_object_key",das::SideEffects::modifyExternal,"StructSetElementObjectKey");
        AddUEExtern<DAS_BIND_FUN(StructSetWithObjectKey),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_struct_set_with_object_key",das::SideEffects::modifyExternal,"StructSetWithObjectKey")->needCallerStackFrame=true;
        AddUEExtern<DAS_BIND_FUN(StructMapObjectKey)>(*this,Library,"__ue_struct_map_key_object",das::SideEffects::modifyExternal,"StructMapObjectKey");
        AddUEExtern<DAS_BIND_FUN(StructMapGetObjectKeyInt)>(*this,Library,"__ue_struct_map_get_object_int",das::SideEffects::modifyExternal,"StructMapGetObjectKeyInt");
        AddUEExtern<DAS_BIND_FUN(StructMapWithObjectKeyInt),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_struct_map_with_object_int",das::SideEffects::modifyExternal,"StructMapWithObjectKeyInt")->needCallerStackFrame=true;
        AddUEExtern<DAS_BIND_FUN(StructMapGetObjectKeyFloat)>(*this,Library,"__ue_struct_map_get_object_float",das::SideEffects::modifyExternal,"StructMapGetObjectKeyFloat");
        AddUEExtern<DAS_BIND_FUN(StructMapWithObjectKeyFloat),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_struct_map_with_object_float",das::SideEffects::modifyExternal,"StructMapWithObjectKeyFloat")->needCallerStackFrame=true;
        AddUEExtern<DAS_BIND_FUN(StructMapGetObjectKeyDouble)>(*this,Library,"__ue_struct_map_get_object_double",das::SideEffects::modifyExternal,"StructMapGetObjectKeyDouble");
        AddUEExtern<DAS_BIND_FUN(StructMapWithObjectKeyDouble),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_struct_map_with_object_double",das::SideEffects::modifyExternal,"StructMapWithObjectKeyDouble")->needCallerStackFrame=true;
        AddUEExtern<DAS_BIND_FUN(StructMapGetObjectKeyBool)>(*this,Library,"__ue_struct_map_get_object_bool",das::SideEffects::modifyExternal,"StructMapGetObjectKeyBool");
        AddUEExtern<DAS_BIND_FUN(StructMapWithObjectKeyBool),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_struct_map_with_object_bool",das::SideEffects::modifyExternal,"StructMapWithObjectKeyBool")->needCallerStackFrame=true;
        AddUEExtern<DAS_BIND_FUN(StructMapGetObjectKeyString)>(*this,Library,"__ue_struct_map_get_object_string",das::SideEffects::modifyExternal,"StructMapGetObjectKeyString");
        AddUEExtern<DAS_BIND_FUN(StructMapWithObjectKeyString),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_struct_map_with_object_string",das::SideEffects::modifyExternal,"StructMapWithObjectKeyString")->needCallerStackFrame=true;
        AddUEExtern<DAS_BIND_FUN(StructMapGetObjectKeyName)>(*this,Library,"__ue_struct_map_get_object_name",das::SideEffects::modifyExternal,"StructMapGetObjectKeyName");
        AddUEExtern<DAS_BIND_FUN(StructMapWithObjectKeyName),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_struct_map_with_object_name",das::SideEffects::modifyExternal,"StructMapWithObjectKeyName")->needCallerStackFrame=true;
        AddUEExtern<DAS_BIND_FUN(StructMapGetObjectKeyEnum)>(*this,Library,"__ue_struct_map_get_object_enum",das::SideEffects::modifyExternal,"StructMapGetObjectKeyEnum");
        AddUEExtern<DAS_BIND_FUN(StructMapWithObjectKeyEnum),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_struct_map_with_object_enum",das::SideEffects::modifyExternal,"StructMapWithObjectKeyEnum")->needCallerStackFrame=true;
        AddUEExtern<DAS_BIND_FUN(StructMapGetObjectKeyObject),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_struct_map_get_object_object",das::SideEffects::modifyExternal,"StructMapGetObjectKeyObject");
        AddUEExtern<DAS_BIND_FUN(StructMapWithObjectKeyObject),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_struct_map_with_object_object",das::SideEffects::modifyExternal,"StructMapWithObjectKeyObject")->needCallerStackFrame=true;
        AddUEExtern<DAS_BIND_FUN(StructMapGetObjectKeyStruct),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_struct_map_get_object_struct",das::SideEffects::modifyExternal,"StructMapGetObjectKeyStruct")->needCallerStackFrame=true;
        AddUEExtern<DAS_BIND_FUN(StructMapWithObjectKeyStruct),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_struct_map_with_object_struct",das::SideEffects::modifyExternal,"StructMapWithObjectKeyStruct")->needCallerStackFrame=true;
        AddUEExtern<DAS_BIND_FUN(StructMapEnumKey)>(*this,Library,"__ue_struct_map_key_enum",das::SideEffects::modifyExternal,"StructMapEnumKey");
        AddUEExtern<DAS_BIND_FUN(StructMapGetEnumKeyInt)>(*this,Library,"__ue_struct_map_get_enum_int",das::SideEffects::modifyExternal,"StructMapGetEnumKeyInt");
        AddUEExtern<DAS_BIND_FUN(StructMapWithEnumKeyInt),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_struct_map_with_enum_int",das::SideEffects::modifyExternal,"StructMapWithEnumKeyInt")->needCallerStackFrame=true;
        AddUEExtern<DAS_BIND_FUN(StructMapGetEnumKeyFloat)>(*this,Library,"__ue_struct_map_get_enum_float",das::SideEffects::modifyExternal,"StructMapGetEnumKeyFloat");
        AddUEExtern<DAS_BIND_FUN(StructMapWithEnumKeyFloat),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_struct_map_with_enum_float",das::SideEffects::modifyExternal,"StructMapWithEnumKeyFloat")->needCallerStackFrame=true;
        AddUEExtern<DAS_BIND_FUN(StructMapGetEnumKeyDouble)>(*this,Library,"__ue_struct_map_get_enum_double",das::SideEffects::modifyExternal,"StructMapGetEnumKeyDouble");
        AddUEExtern<DAS_BIND_FUN(StructMapWithEnumKeyDouble),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_struct_map_with_enum_double",das::SideEffects::modifyExternal,"StructMapWithEnumKeyDouble")->needCallerStackFrame=true;
        AddUEExtern<DAS_BIND_FUN(StructMapGetEnumKeyBool)>(*this,Library,"__ue_struct_map_get_enum_bool",das::SideEffects::modifyExternal,"StructMapGetEnumKeyBool");
        AddUEExtern<DAS_BIND_FUN(StructMapWithEnumKeyBool),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_struct_map_with_enum_bool",das::SideEffects::modifyExternal,"StructMapWithEnumKeyBool")->needCallerStackFrame=true;
        AddUEExtern<DAS_BIND_FUN(StructMapGetEnumKeyString)>(*this,Library,"__ue_struct_map_get_enum_string",das::SideEffects::modifyExternal,"StructMapGetEnumKeyString");
        AddUEExtern<DAS_BIND_FUN(StructMapWithEnumKeyString),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_struct_map_with_enum_string",das::SideEffects::modifyExternal,"StructMapWithEnumKeyString")->needCallerStackFrame=true;
        AddUEExtern<DAS_BIND_FUN(StructMapGetEnumKeyName)>(*this,Library,"__ue_struct_map_get_enum_name",das::SideEffects::modifyExternal,"StructMapGetEnumKeyName");
        AddUEExtern<DAS_BIND_FUN(StructMapWithEnumKeyName),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_struct_map_with_enum_name",das::SideEffects::modifyExternal,"StructMapWithEnumKeyName")->needCallerStackFrame=true;
        AddUEExtern<DAS_BIND_FUN(StructMapGetEnumKeyEnum)>(*this,Library,"__ue_struct_map_get_enum_enum",das::SideEffects::modifyExternal,"StructMapGetEnumKeyEnum");
        AddUEExtern<DAS_BIND_FUN(StructMapWithEnumKeyEnum),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_struct_map_with_enum_enum",das::SideEffects::modifyExternal,"StructMapWithEnumKeyEnum")->needCallerStackFrame=true;
        AddUEExtern<DAS_BIND_FUN(StructMapGetEnumKeyObject),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_struct_map_get_enum_object",das::SideEffects::modifyExternal,"StructMapGetEnumKeyObject");
        AddUEExtern<DAS_BIND_FUN(StructMapWithEnumKeyObject),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_struct_map_with_enum_object",das::SideEffects::modifyExternal,"StructMapWithEnumKeyObject")->needCallerStackFrame=true;
        AddUEExtern<DAS_BIND_FUN(StructMapGetEnumKeyStruct),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_struct_map_get_enum_struct",das::SideEffects::modifyExternal,"StructMapGetEnumKeyStruct")->needCallerStackFrame=true;
        AddUEExtern<DAS_BIND_FUN(StructMapWithEnumKeyStruct),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_struct_map_with_enum_struct",das::SideEffects::modifyExternal,"StructMapWithEnumKeyStruct")->needCallerStackFrame=true;
        AddUEExtern<DAS_BIND_FUN(StructSetElementEnum)>(*this,Library,"__ue_struct_set_element_enum",das::SideEffects::modifyExternal,"StructSetElementEnum");
        AddUEExtern<DAS_BIND_FUN(StructSetWithEnum),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_struct_set_with_enum",das::SideEffects::modifyExternal,"StructSetWithEnum")->needCallerStackFrame=true;
        AddUEExtern<DAS_BIND_FUN(StructSetElementName)>(*this,Library,"ue_struct_set_element_name",das::SideEffects::modifyExternal,"StructSetElementName");
            AddUEExtern<DAS_BIND_FUN(StructSetWithName),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_set_with_name",das::SideEffects::modifyExternal,"StructSetWithName")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(StructMapNameKey)>(*this,Library,"ue_struct_map_key_name",das::SideEffects::modifyExternal,"StructMapNameKey");
            AddUEExtern<DAS_BIND_FUN(StructMapGetNameInt)>(*this,Library,"ue_struct_map_get_name_int",das::SideEffects::modifyExternal,"StructMapGetNameInt");
            AddUEExtern<DAS_BIND_FUN(StructMapWithNameInt),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_map_with_name_int",das::SideEffects::modifyExternal,"StructMapWithNameInt")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(StructMapGetNameFloat)>(*this,Library,"ue_struct_map_get_name_float",das::SideEffects::modifyExternal,"StructMapGetNameFloat");
            AddUEExtern<DAS_BIND_FUN(StructMapWithNameFloat),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_map_with_name_float",das::SideEffects::modifyExternal,"StructMapWithNameFloat")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(StructMapGetNameDouble)>(*this,Library,"ue_struct_map_get_name_double",das::SideEffects::modifyExternal,"StructMapGetNameDouble");
            AddUEExtern<DAS_BIND_FUN(StructMapWithNameDouble),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_map_with_name_double",das::SideEffects::modifyExternal,"StructMapWithNameDouble")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(StructMapGetNameBool)>(*this,Library,"ue_struct_map_get_name_bool",das::SideEffects::modifyExternal,"StructMapGetNameBool");
            AddUEExtern<DAS_BIND_FUN(StructMapWithNameBool),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_map_with_name_bool",das::SideEffects::modifyExternal,"StructMapWithNameBool")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(StructMapGetNameString)>(*this,Library,"ue_struct_map_get_name_string",das::SideEffects::modifyExternal,"StructMapGetNameString");
            AddUEExtern<DAS_BIND_FUN(StructMapWithNameString),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_map_with_name_string",das::SideEffects::modifyExternal,"StructMapWithNameString")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(StructMapGetNameName)>(*this,Library,"ue_struct_map_get_name_name",das::SideEffects::modifyExternal,"StructMapGetNameName");
            AddUEExtern<DAS_BIND_FUN(StructMapWithNameName),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_map_with_name_name",das::SideEffects::modifyExternal,"StructMapWithNameName")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(StructMapGetNameStruct),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_struct_map_get_name_struct",das::SideEffects::modifyExternal,"StructMapGetNameStruct")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(StructMapWithNameStruct),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_struct_map_with_name_struct",das::SideEffects::modifyExternal,"StructMapWithNameStruct")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(StructMapGetNameEnum)>(*this,Library,"__ue_struct_map_get_name_enum",das::SideEffects::modifyExternal,"StructMapGetNameEnum");
            AddUEExtern<DAS_BIND_FUN(StructMapWithNameEnum),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_struct_map_with_name_enum",das::SideEffects::modifyExternal,"StructMapWithNameEnum")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(StructMapGetNameObject),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_struct_map_get_name_object",das::SideEffects::modifyExternal,"StructMapGetNameObject");
            AddUEExtern<DAS_BIND_FUN(StructMapWithNameObject),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"__ue_struct_map_with_name_object",das::SideEffects::modifyExternal,"StructMapWithNameObject")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(StructMapStringKey)>(*this,Library,"ue_struct_map_key_string",das::SideEffects::modifyExternal,"StructMapStringKey");
            AddUEExtern<DAS_BIND_FUN(StructMapGetIntString)>(*this,Library,"ue_struct_map_get_int_string",das::SideEffects::modifyExternal,"StructMapGetIntString");
            AddUEExtern<DAS_BIND_FUN(StructMapWithIntString),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_map_with_int_string",das::SideEffects::modifyExternal,"StructMapWithIntString")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(StructMapGetStringInt)>(*this,Library,"ue_struct_map_get_string_int",das::SideEffects::modifyExternal,"StructMapGetStringInt");
            AddUEExtern<DAS_BIND_FUN(StructMapWithStringInt),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_map_with_string_int",das::SideEffects::modifyExternal,"StructMapWithStringInt")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(StructMapGetStringString)>(*this,Library,"ue_struct_map_get_string_string",das::SideEffects::modifyExternal,"StructMapGetStringString");
            AddUEExtern<DAS_BIND_FUN(StructMapWithStringString),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_map_with_string_string",das::SideEffects::modifyExternal,"StructMapWithStringString")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(StructArraySize)>(*this,Library,"ue_struct_array_size",das::SideEffects::modifyExternal,"StructArraySize");
            AddUEExtern<DAS_BIND_FUN(StructArrayResize),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_array_resize",das::SideEffects::modifyExternal,"StructArrayResize")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(StructNestedGet),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_nested_get",das::SideEffects::modifyExternal,"StructNestedGet")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(StructNestedWith),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_nested_with",das::SideEffects::modifyExternal,"StructNestedWith")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(StructGetInt)>(*this,Library,"ue_struct_get_int",das::SideEffects::modifyExternal,"StructGetInt");
            AddUEExtern<DAS_BIND_FUN(StructWithInt),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_with_int",das::SideEffects::modifyExternal,"StructWithInt")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(StructGetFloat)>(*this,Library,"ue_struct_get_float",das::SideEffects::modifyExternal,"StructGetFloat");
            AddUEExtern<DAS_BIND_FUN(StructWithFloat),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_with_float",das::SideEffects::modifyExternal,"StructWithFloat")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(StructGetDouble)>(*this,Library,"ue_struct_get_double",das::SideEffects::modifyExternal,"StructGetDouble");
            AddUEExtern<DAS_BIND_FUN(StructWithDouble),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_with_double",das::SideEffects::modifyExternal,"StructWithDouble")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(StructGetBool)>(*this,Library,"ue_struct_get_bool",das::SideEffects::modifyExternal,"StructGetBool");
            AddUEExtern<DAS_BIND_FUN(StructWithBool),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_with_bool",das::SideEffects::modifyExternal,"StructWithBool")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(StructGetString)>(*this,Library,"ue_struct_get_string",das::SideEffects::modifyExternal,"StructGetString");
            AddUEExtern<DAS_BIND_FUN(StructWithString),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_with_string",das::SideEffects::modifyExternal,"StructWithString")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(StructGetEnum)>(*this,Library,"ue_struct_get_enum",das::SideEffects::modifyExternal,"StructGetEnum");
            AddUEExtern<DAS_BIND_FUN(StructWithEnum),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_with_enum",das::SideEffects::modifyExternal,"StructWithEnum")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(StructGetClass)>(*this,Library,"__ue_struct_get_class",das::SideEffects::modifyExternal,"StructGetClass");
            AddUEExtern<DAS_BIND_FUN(StructGetObject),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_get_object",das::SideEffects::modifyExternal,"StructGetObject");
            AddUEExtern<DAS_BIND_FUN(StructWithObject),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_with_object",das::SideEffects::modifyExternal,"StructWithObject")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(StructGetName)>(*this,Library,"ue_struct_get_name",das::SideEffects::modifyExternal,"StructGetName");
            AddUEExtern<DAS_BIND_FUN(StructWithName),das::SimNode_ExtFuncCallAndCopyOrMove>(*this,Library,"ue_struct_with_name",das::SideEffects::modifyExternal,"StructWithName")->needCallerStackFrame=true;
            AddUEExtern<DAS_BIND_FUN(DSLNameFromString)>(*this,Library,"ue_name_value",das::SideEffects::modifyExternal,"DSLNameFromString");
            AddUEExtern<DAS_BIND_FUN(DSLNameToString)>(*this,Library,"ue_name_text",das::SideEffects::modifyExternal,"DSLNameToString");
            AddUEExtern<DAS_BIND_FUN(DSLNamesEqual)>(*this,Library,"ue_names_equal",das::SideEffects::accessExternal,"DSLNamesEqual");
            AddUEExtern<DAS_BIND_FUN(GetSelfDouble)>(*this,Library,"ue_get_self_double",das::SideEffects::accessExternal,"GetSelfDouble");
            AddUEExtern<DAS_BIND_FUN(SetSelfDouble)>(*this,Library,"ue_set_self_double",das::SideEffects::modifyExternal,"SetSelfDouble");
            AddUEExtern<DAS_BIND_FUN(GetSelfString)>(*this,Library,"ue_get_self_string",das::SideEffects::modifyExternal,"GetSelfString");
            AddUEExtern<DAS_BIND_FUN(SetSelfString)>(*this,Library,"ue_set_self_string",das::SideEffects::modifyExternal,"SetSelfString");
            AddUEExtern<DAS_BIND_FUN(GetSelfName)>(*this,Library,"ue_get_self_name",das::SideEffects::modifyExternal,"GetSelfName");
            AddUEExtern<DAS_BIND_FUN(SetSelfName)>(*this,Library,"ue_set_self_name",das::SideEffects::modifyExternal,"SetSelfName");
            AddUEExtern<DAS_BIND_FUN(GetSelfBool)>(*this, Library, "ue_get_self_bool", das::SideEffects::accessExternal, "GetSelfBool");
            AddUEExtern<DAS_BIND_FUN(SetSelfBool)>(*this, Library, "ue_set_self_bool", das::SideEffects::modifyExternal, "SetSelfBool");
            AddUEExtern<DAS_BIND_FUN(GetSelfInt)>(*this, Library, "ue_get_self_int", das::SideEffects::accessExternal, "GetSelfInt");
            AddUEExtern<DAS_BIND_FUN(GetSelfFloat)>(*this, Library, "ue_get_self_float", das::SideEffects::accessExternal, "GetSelfFloat");
            AddUEExtern<DAS_BIND_FUN(SetSelfInt)>(*this, Library, "ue_set_self_int", das::SideEffects::modifyExternal, "SetSelfInt");
            AddUEExtern<DAS_BIND_FUN(SetSelfFloat)>(*this, Library, "ue_set_self_float", das::SideEffects::modifyExternal, "SetSelfFloat");
            AddUEExtern<DAS_BIND_FUN(EmitType)>(*this, Library, "ue_emit_type", das::SideEffects::modifyExternal, "EmitType");
        }
    };

    FString Errors(const das::ProgramPtr& Program, das::TextWriter& Output)
    {
        if (Program)
            for (const auto& Error : Program->errors)
                Output << das::reportError(Error.at, Error.what, Error.extra, Error.fixme, Error.cerr);
        const auto Text = Output.str();
        return UTF8_TO_TCHAR(Text.c_str());
    }

    // Declaration order keeps compiler owners alive until the context is destroyed.
    struct FCompiledContext
    {
        FString DSLDeclaration;
        TSharedPtr<FDSLAsyncPhysicsState> Physics;
        bool bPhysicsOwner=false,bPhysicsForeignSession=false;
        das::ModuleGroup Modules;
        das::FileAccessPtr Access;
        das::ProgramPtr Program;
        TUniquePtr<FUnrealDasContext> Context;
        das::SimFunction* Function = nullptr;
        das::SimFunction* Initializer = nullptr;
        das::SimFunction* Construction = nullptr;
        das::SimFunction* BeginPlay = nullptr;
        das::SimFunction* Tick = nullptr;
        das::SimFunction* EndPlay = nullptr;
    };
    #include "DasAsyncPhysicsDSLRuntime.inl"

    struct FManagedSubscription
    {
        uint64 Id = 0;
        TWeakObjectPtr<UObject> Source;
        FName Event;
        FString Handler;
        EDasMethodSignature Kind=EDasMethodSignature::Int;
        FString PayloadPath;
        TStrongObjectPtr<UDasIntDelegateProxy> Proxy;FName ProxyFunction;
        int32 AsyncKind=0;FString AsyncName;TStrongObjectPtr<UDasAsyncAssetProxy> AsyncProxy;TSharedPtr<FStreamableHandle> StreamHandle;
        int32 LatentKind=0;FString LatentName;TWeakObjectPtr<UWorld> LatentWorld;TStrongObjectPtr<UDasLatentProxy> LatentProxy;
        bool InputConsume=true,InputPaused=false;int32 InputKind=0,InputHandle=INDEX_NONE;EInputEvent InputPhase=IE_Pressed;
        uint32 EnhancedHandle=0;ETriggerEvent EnhancedPhase=ETriggerEvent::Triggered;TStrongObjectPtr<UDasEnhancedInputProxy> EnhancedProxy;TStrongObjectPtr<UObject> InputAsset;
        FString AIMoveName;TStrongObjectPtr<UDasAIMoveProxy> AIMoveProxy;
        FString EffectName;TStrongObjectPtr<UDasEffectProxy> EffectProxy;
        FString MontageName;TStrongObjectPtr<UDasMontageProxy> MontageProxy;
        ~FManagedSubscription()
        {
            if(AIMoveProxy.IsValid()){AIMoveProxy->Deactivate();return;}
            if(EffectProxy.IsValid()){EffectProxy->Deactivate();return;}
            if(MontageProxy.IsValid()){MontageProxy->Deactivate();return;}
            if(AsyncKind){if(StreamHandle.IsValid()){if(!StreamHandle->HasLoadCompleted())StreamHandle->CancelHandle();else StreamHandle->ReleaseHandle();}return;}
            if(LatentKind){if(auto* W=LatentWorld.Get())if(LatentProxy.IsValid())W->GetLatentActionManager().RemoveActionsForObject(TWeakObjectPtr<UObject>(LatentProxy.Get()));return;}
            if(InputKind==3){if(auto* Input=Cast<UEnhancedInputComponent>(Source.Get()))Input->RemoveBindingByHandle(EnhancedHandle);return;}
            if(InputKind){if(auto* Input=Cast<UInputComponent>(Source.Get())){if(InputKind==1)Input->RemoveActionBindingForHandle(InputHandle);else Input->AxisBindings.RemoveAll([&](const FInputAxisBinding& B){return B.AxisDelegate.IsBoundToObject(Proxy.Get());});}return;}
            if (UObject* Object = Source.Get())
                if (auto* Property = ScalarEventProperty(Object,Event,Kind))
                {
                    FScriptDelegate Delegate;
                    Delegate.BindUFunction(Proxy.Get(),ProxyFunction);
                    Property->RemoveDelegate(Delegate, Object, Property->ContainerPtrToValuePtr<void>(Object));
                }
        }
    };

    struct FManagedInputContext {
        TWeakObjectPtr<UEnhancedInputLocalPlayerSubsystem> Subsystem;
        TStrongObjectPtr<UInputMappingContext> Context;
        int32 Priority=0;
        ~FManagedInputContext(){if(auto* S=Subsystem.Get()){int32 Actual=0;if(Context.IsValid() && S->HasMappingContext(Context.Get(),Actual) && Actual==Priority){FModifyContextOptions Options;Options.bNotifyUserSettings=false;S->RemoveMappingContext(Context.Get(),Options);}}}
    };

    struct FInputRecipe {int32 Kind=0;FName Name;FString Handler;EInputEvent Phase=IE_Pressed;ETriggerEvent Trigger=ETriggerEvent::Triggered;bool Consume=true,Paused=false;TStrongObjectPtr<UObject> Asset;};
    struct FInputContextRecipe {TWeakObjectPtr<UEnhancedInputLocalPlayerSubsystem> Subsystem;TStrongObjectPtr<UInputMappingContext> Context;int32 Priority=0;};

    struct FPersistentSession
    {
        TWeakObjectPtr<UObject> Owner;
        TWeakObjectPtr<UWorld> World;
        FString Path;
        FSourceSet AttemptedSources;
        TMap<FString,FDasCallContract> RequiredEntries;TSet<FString> NativeInputEntries,InputSnapshotEntries;
        FString ApprovedDSLDeclaration;
        bool bDefaultsUpdateApproved=false;FString AttemptedEntry;FDasCallContract AttemptedContract;
        TUniquePtr<FCompiledContext> Live;
        FString ReloadError;
        uint32 Revision = 0;
        bool bBeginDelivered = false;
        bool bEndDelivered = false;
        bool bReleaseRequested = false;
        TArray<TUniquePtr<FManagedSubscription>> Bindings;
        TArray<TUniquePtr<FManagedInputContext>> InputContexts;
        TArray<FInputRecipe> InputRecipes;TArray<FInputContextRecipe> ContextRecipes;bool bRestoringInput=false;double NextInputRestore=0;FString InputRestoreError;
#if WITH_EDITOR
        bool bReinstanceOwnerInScope=false,bReinstanceOwnerChanged=false,bReinstanceTickEnabled=false;
        FString ReinstanceLifecyclePath;
#endif
    };
    // Delivery belongs to the UObject lifetime, independently of VM/session replacement.
    enum class EInitializerDelivery:uint8 {Running,Complete,Failed};
    struct FInitializerDelivery {EInitializerDelivery State=EInitializerDelivery::Running;FString Error;};
    TMap<TWeakObjectPtr<UObject>,TMap<FString,FInitializerDelivery>> InitializerDeliveries;
    TArray<TUniquePtr<FPersistentSession>> Sessions;
#if WITH_EDITOR
    TArray<TUniquePtr<FPersistentSession>> PreservedSessions;
    bool bPreservingSessions=false;
#endif

    struct FManagedScriptTimer {
        uint64 Id=0;FString Name,Handler;FUnrealDasContext* Context=nullptr;
        TWeakObjectPtr<UObject> Owner;TWeakObjectPtr<UWorld> World;FTimerHandle Handle;bool Repeat=false;TSharedPtr<FDasCallbackPayload> Argument;
        ~FManagedScriptTimer(){if(auto* W=World.Get())W->GetTimerManager().ClearTimer(Handle);}
    };
    TMap<uint64,TUniquePtr<FManagedScriptTimer>> ScriptTimers;
    TArray<uint64> PendingScriptTimers;uint64 NextScriptTimer=1;bool bDrainingScriptTimers=false;
    void DrainEvents();
    void ClearSessionTimers(FPersistentSession& S){if(!S.Live)return;for(auto I=ScriptTimers.CreateIterator();I;++I)if(I.Value()->Context==S.Live->Context.Get())I.RemoveCurrent();}
    FPersistentSession* ManagedOwnerSession(das::Context* C){ResolveSelf(C);if(ConstructingObject)C->throw_error("managed callbacks are forbidden during ConstructionScript");if(ActiveContext->bCollectingSubscriptions)C->throw_error("managed callback operations are forbidden in bind_events");for(const auto& S:Sessions)if(S->Live && S->Live->Context.Get()==ActiveContext && !S->bReleaseRequested && !S->bEndDelivered)return S.Get();C->throw_error("managed callbacks require a live persistent owner session");return nullptr;}
    FString TimerName(const char* Name,das::Context* C){const FString N=Name?UTF8_TO_TCHAR(Name):TEXT("");if(N.IsEmpty() || N.Len()>128)C->throw_error("timer name requires 1..128 characters");return N;}
    FManagedScriptTimer* FindScriptTimer(FPersistentSession* S,const FString& Name){for(auto& P:ScriptTimers)if(P.Value->Context==S->Live->Context.Get() && P.Value->Name==Name)return P.Value.Get();return nullptr;}
    void QueueScriptTimer(uint64 Id){if(!ScriptTimers.Contains(Id))return;if(!PendingScriptTimers.Contains(Id)){if(PendingScriptTimers.Num()>=1024){UE_LOG(LogDasRuntime,Warning,TEXT("Script timer dispatch queue limit reached"));return;}PendingScriptTimers.Add(Id);}DrainEvents();}
    void ScriptTimerStartImpl(const char* Name,float Seconds,const char* Handler,bool Repeat,das::Context* C,FDasScalarValue* Argument,bool NextTick=false){
        auto* S=ManagedOwnerSession(C);if(ActiveContext->bEnding)C->throw_error("cannot start a timer during EndPlay");const FString N=TimerName(Name,C),Entry=Handler?UTF8_TO_TCHAR(Handler):TEXT("");if((!NextTick && (!FMath::IsFinite(Seconds) || Seconds<=0.f)) || Entry.IsEmpty() || Entry.Len()>=NAME_SIZE)C->throw_error("timer requires positive finite interval and handler");
        FDasCallContract Contract=FDasCallContract::Scalar(EDasMethodSignature::Void);
        if(Argument){FString Issue;if(!ValidateDasCallbackPayload(*Argument,EDasCallbackUsage::Timer,Issue))C->throw_error(TCHAR_TO_UTF8(*Issue));Contract.Inputs.Add(static_cast<EDasMethodSignature>(Argument->Kind));if((Argument->Kind==int32(EDasMethodSignature::Struct) || Argument->Kind==int32(EDasMethodSignature::Enum) || Argument->Kind==int32(EDasMethodSignature::Object) || Argument->Kind==int32(EDasMethodSignature::Class)))Contract.InputClasses.Add(Argument->ObjectClass);}
        if(!VerifyContract(S->Live->Program,Entry,Contract))C->throw_error("timer handler must return void and match captured argument signature");auto* W=S->World.Get();if(!W || W!=S->Owner->GetWorld())C->throw_error("timer requires owner's live world");
        auto* Previous=FindScriptTimer(S,N);int32 Count=0;for(const auto& P:ScriptTimers)if(P.Value->Context==ActiveContext)++Count;if(!Previous && (Count>=64 || ScriptTimers.Num()>=1024))C->throw_error("script timer count limit exceeded");
        if(Previous)ScriptTimers.Remove(Previous->Id);auto T=MakeUnique<FManagedScriptTimer>();T->Id=NextScriptTimer++;T->Name=N;T->Handler=Entry;T->Context=ActiveContext;T->Owner=S->Owner;T->World=W;T->Repeat=Repeat;if(Argument)T->Argument=MakeShared<FDasCallbackPayload>(MoveTemp(*Argument));const uint64 Id=T->Id;
        auto Delegate=FTimerDelegate::CreateLambda([Id](){QueueScriptTimer(Id);});if(NextTick)T->Handle=W->GetTimerManager().SetTimerForNextTick(Delegate);else W->GetTimerManager().SetTimer(T->Handle,Delegate,Seconds,Repeat);ScriptTimers.Add(Id,MoveTemp(T));S->RequiredEntries.Add(Entry,Contract);
    }
    FDasCallHandle BeginCallbackFrame(const char* Path,das::Context* C){
        RequireObjectContext(C);auto* Frame=FindDasCallbackFrame(Path?UTF8_TO_TCHAR(Path):TEXT(""));auto* Function=DasCallbackFrameBuilder(Frame);if(!Function)C->throw_error("unknown callback frame");
        if(ActiveContext->Calls.Num()>=256)C->throw_error("call frame limit reached");auto Call=MakeShared<FMarshalledCall>();Call->CallbackFramePath=Frame->GetPathName();Call->Target=ResolveSelf(C);Call->Function.Reset(Function);Call->Frame=MakeUnique<FStructOnScope>(Function);Call->Signature=DescribeCall(Function,C);
        const uint64 Token=NextCallHandle++;ActiveContext->Calls.Add(Token,MoveTemp(Call));return {Token};
    }
    void StartCallbackFrame(const char* N,float Seconds,const char* H,bool Repeat,const FDasCallHandle& Handle,bool Next,das::Context* C){
        auto& Call=ResolveCall(Handle,C);auto* Type=FindDasCallbackFrame(Call.CallbackFramePath);if(!Type || Call.bExecuted)C->throw_error("invalid callback builder");
        FDasScalarValue Value;Value.Kind=12;Value.ObjectClass=Type->GetPathName();Value.StructValue.InitializeAs(Type);
        for(TFieldIterator<FProperty> I(Type);I;++I){const FString Key=I->GetName().ToLower();if(!Call.Assigned.Contains(Key))C->throw_error("missing callback argument");auto* Source=Call.Function->FindPropertyByName(I->GetFName());FDasScalarValue Input;FString Error;
            if(!ReadDasPropertyValue(Source,Source->ContainerPtrToValuePtr<void>(Call.Frame->GetStructMemory()),Input,Error) || !WriteDasPropertyValue(*I,I->ContainerPtrToValuePtr<void>(Value.StructValue.GetMutableMemory()),Input,Error))C->throw_error(TCHAR_TO_UTF8(*Error));}
        ScriptTimerStartImpl(N,Seconds,H,Repeat,C,&Value,Next);Call.bExecuted=true;
    }
    void StartCallbackFrameTimer(const char* N,float Seconds,const char* H,bool Repeat,const FDasCallHandle& Handle,das::Context* C){StartCallbackFrame(N,Seconds,H,Repeat,Handle,false,C);}
    void NextCallbackFrameTimer(const char* N,const char* H,const FDasCallHandle& Handle,das::Context* C){StartCallbackFrame(N,0.f,H,false,Handle,true,C);}
    void ScriptTimerStart(const char* N,float Seconds,const char* H,bool Repeat,das::Context* C){ScriptTimerStartImpl(N,Seconds,H,Repeat,C,nullptr);}
    void ScriptTimerStartArray(const char* N,float Seconds,const char* H,bool Repeat,const FDasStructValue& Value,das::Context* C){FDasScalarValue V;V.Kind=12;V.StructValue=ResolveStructValue(Value,C);V.ObjectClass=V.StructValue.GetScriptStruct()->GetPathName();ScriptTimerStartImpl(N,Seconds,H,Repeat,C,&V);}
    FDasScalarValue ClassTimerValue(FDasClassHandle Value,const char* Path,das::Context* C){FDasScalarValue V;V.Kind=15;V.ObjectClass=Path?UTF8_TO_TCHAR(Path):TEXT("");if(Value.Token){V.ClassValue=Cast<UClass>(ResolveObject(FDasObjectHandle{Value.Token},C));if(!V.ClassValue)C->throw_error("expected class capability");}return V;}
    void ScriptTimerStartClass(const char* N,float Seconds,const char* H,bool Repeat,FDasClassHandle Value,const char* Path,das::Context* C){auto V=ClassTimerValue(Value,Path,C);ScriptTimerStartImpl(N,Seconds,H,Repeat,C,&V);}
    void ScriptTimerNextTickClass(const char* N,const char* H,FDasClassHandle Value,const char* Path,das::Context* C){auto V=ClassTimerValue(Value,Path,C);ScriptTimerStartImpl(N,0.f,H,false,C,&V,true);}
    void ScriptTimerStartObject(const char* N,float Seconds,const char* H,bool Repeat,const FDasObjectHandle& Value,const char* Path,das::Context* C){FDasScalarValue V;V.Kind=8;V.ObjectValue=Value.Token?ResolveObject(Value,C):nullptr;V.ObjectClass=Path?UTF8_TO_TCHAR(Path):TEXT("");ScriptTimerStartImpl(N,Seconds,H,Repeat,C,&V);}
    void ScriptTimerNextTickObject(const char* N,const char* H,const FDasObjectHandle& Value,const char* Path,das::Context* C){FDasScalarValue V;V.Kind=8;V.ObjectValue=Value.Token?ResolveObject(Value,C):nullptr;V.ObjectClass=Path?UTF8_TO_TCHAR(Path):TEXT("");ScriptTimerStartImpl(N,0.f,H,false,C,&V,true);}
    void ScriptTimerStartEnum(const char* N,float Seconds,const char* H,bool Repeat,FDasEnumValue Value,das::Context* C){const auto& E=ResolveEnumValue(Value,C);FDasScalarValue V;V.Kind=13;V.IntValue=E.Value;V.ObjectClass=E.Type->GetPathName();ScriptTimerStartImpl(N,Seconds,H,Repeat,C,&V);}
    void ScriptTimerNextTickEnum(const char* N,const char* H,FDasEnumValue Value,das::Context* C){const auto& E=ResolveEnumValue(Value,C);FDasScalarValue V;V.Kind=13;V.IntValue=E.Value;V.ObjectClass=E.Type->GetPathName();ScriptTimerStartImpl(N,0.f,H,false,C,&V,true);}
    void ScriptTimerStartByte(const char* N,float Seconds,const char* H,bool Repeat,uint8 Value,das::Context* C){FDasScalarValue V;V.Kind=17;V.ByteValue=Value;ScriptTimerStartImpl(N,Seconds,H,Repeat,C,&V);}
    void ScriptTimerNextTickByte(const char* N,const char* H,uint8 Value,das::Context* C){FDasScalarValue V;V.Kind=17;V.ByteValue=Value;ScriptTimerStartImpl(N,0.f,H,false,C,&V,true);}
    void ScriptTimerStartText(const char* N,float Seconds,const char* H,bool Repeat,FDasTextValue Value,das::Context* C){FDasScalarValue V;V.Kind=18;V.TextValue=ResolveDSLText(Value,C);ScriptTimerStartImpl(N,Seconds,H,Repeat,C,&V);}
    void ScriptTimerNextTickText(const char* N,const char* H,FDasTextValue Value,das::Context* C){FDasScalarValue V;V.Kind=18;V.TextValue=ResolveDSLText(Value,C);ScriptTimerStartImpl(N,0.f,H,false,C,&V,true);}
    void ScriptTimerStartInt(const char* N,float Seconds,const char* H,bool Repeat,int32 Value,das::Context* C){FDasScalarValue V;V.Kind=0;V.IntValue=Value;ScriptTimerStartImpl(N,Seconds,H,Repeat,C,&V);}
    void ScriptTimerStartFloat(const char* N,float Seconds,const char* H,bool Repeat,float Value,das::Context* C){FDasScalarValue V;V.Kind=1;V.FloatValue=Value;ScriptTimerStartImpl(N,Seconds,H,Repeat,C,&V);}
    void ScriptTimerStartBool(const char* N,float Seconds,const char* H,bool Repeat,bool Value,das::Context* C){FDasScalarValue V;V.Kind=2;V.BoolValue=Value;ScriptTimerStartImpl(N,Seconds,H,Repeat,C,&V);}
    void ScriptTimerStartDouble(const char* N,float Seconds,const char* H,bool Repeat,double Value,das::Context* C){FDasScalarValue V;V.Kind=6;V.DoubleValue=Value;ScriptTimerStartImpl(N,Seconds,H,Repeat,C,&V);}
    void ScriptTimerStartString(const char* N,float Seconds,const char* H,bool Repeat,const char* Value,das::Context* C){FDasScalarValue V;V.Kind=5;V.StringValue=Value?UTF8_TO_TCHAR(Value):TEXT("");ScriptTimerStartImpl(N,Seconds,H,Repeat,C,&V);}
    void ScriptTimerStartName(const char* N,float Seconds,const char* H,bool Repeat,FDasNameValue Value,das::Context* C){FDasScalarValue V;V.Kind=7;V.NameValue=ResolveDSLName(Value,C);ScriptTimerStartImpl(N,Seconds,H,Repeat,C,&V);}
    void ScriptTimerStartVector(const char* N,float Seconds,const char* H,bool Repeat,const FVector& Value,das::Context* C){FDasScalarValue V;V.Kind=9;V.VectorValue=Value;ScriptTimerStartImpl(N,Seconds,H,Repeat,C,&V);}
    void ScriptTimerStartRotator(const char* N,float Seconds,const char* H,bool Repeat,const FRotator& Value,das::Context* C){FDasScalarValue V;V.Kind=10;V.RotatorValue=Value;ScriptTimerStartImpl(N,Seconds,H,Repeat,C,&V);}
    void ScriptTimerStartTransform(const char* N,float Seconds,const char* H,bool Repeat,const FTransform& Value,das::Context* C){FDasScalarValue V;V.Kind=11;V.TransformValue=Value;ScriptTimerStartImpl(N,Seconds,H,Repeat,C,&V);}
    void ScriptTimerNextTick(const char* N,const char* H,das::Context* C){ScriptTimerStartImpl(N,0.f,H,false,C,nullptr,true);}
    void ScriptTimerNextTickArray(const char* N,const char* H,const FDasStructValue& Value,das::Context* C){FDasScalarValue V;V.Kind=12;V.StructValue=ResolveStructValue(Value,C);V.ObjectClass=V.StructValue.GetScriptStruct()->GetPathName();ScriptTimerStartImpl(N,0.f,H,false,C,&V,true);}
    void ScriptTimerNextTickInt(const char* N,const char* H,int32 Value,das::Context* C){FDasScalarValue V;V.Kind=0;V.IntValue=Value;ScriptTimerStartImpl(N,0.f,H,false,C,&V,true);}
    void ScriptTimerNextTickFloat(const char* N,const char* H,float Value,das::Context* C){FDasScalarValue V;V.Kind=1;V.FloatValue=Value;ScriptTimerStartImpl(N,0.f,H,false,C,&V,true);}
    void ScriptTimerNextTickBool(const char* N,const char* H,bool Value,das::Context* C){FDasScalarValue V;V.Kind=2;V.BoolValue=Value;ScriptTimerStartImpl(N,0.f,H,false,C,&V,true);}
    void ScriptTimerNextTickDouble(const char* N,const char* H,double Value,das::Context* C){FDasScalarValue V;V.Kind=6;V.DoubleValue=Value;ScriptTimerStartImpl(N,0.f,H,false,C,&V,true);}
    void ScriptTimerNextTickString(const char* N,const char* H,const char* Value,das::Context* C){FDasScalarValue V;V.Kind=5;V.StringValue=Value?UTF8_TO_TCHAR(Value):TEXT("");ScriptTimerStartImpl(N,0.f,H,false,C,&V,true);}
    void ScriptTimerNextTickName(const char* N,const char* H,FDasNameValue Value,das::Context* C){FDasScalarValue V;V.Kind=7;V.NameValue=ResolveDSLName(Value,C);ScriptTimerStartImpl(N,0.f,H,false,C,&V,true);}
    void ScriptTimerNextTickVector(const char* N,const char* H,const FVector& Value,das::Context* C){FDasScalarValue V;V.Kind=9;V.VectorValue=Value;ScriptTimerStartImpl(N,0.f,H,false,C,&V,true);}
    void ScriptTimerNextTickRotator(const char* N,const char* H,const FRotator& Value,das::Context* C){FDasScalarValue V;V.Kind=10;V.RotatorValue=Value;ScriptTimerStartImpl(N,0.f,H,false,C,&V,true);}
    void ScriptTimerNextTickTransform(const char* N,const char* H,const FTransform& Value,das::Context* C){FDasScalarValue V;V.Kind=11;V.TransformValue=Value;ScriptTimerStartImpl(N,0.f,H,false,C,&V,true);}
    bool ScriptTimerPaused(const char* Name,das::Context* C){auto* S=ManagedOwnerSession(C);auto* T=FindScriptTimer(S,TimerName(Name,C));return T && T->World.IsValid() && T->World->GetTimerManager().IsTimerPaused(T->Handle);}
    float ScriptTimerElapsed(const char* Name,das::Context* C){auto* S=ManagedOwnerSession(C);auto* T=FindScriptTimer(S,TimerName(Name,C));return T && T->World.IsValid()?T->World->GetTimerManager().GetTimerElapsed(T->Handle):-1.f;}
    int32 ScriptTimerCancelAll(das::Context* C){auto* S=ManagedOwnerSession(C);int32 Count=0;for(const auto& P:ScriptTimers)if(P.Value->Context==S->Live->Context.Get())++Count;ClearSessionTimers(*S);return Count;}
    void ScriptTimerStartVector2d(const char* N,float Seconds,const char* H,bool Repeat,const FVector2D& Value,das::Context* C){FDasScalarValue V;V.Kind=12;V.ObjectClass=TEXT("/Script/CoreUObject.Vector2D");V.StructValue.InitializeAs(TBaseStructure<FVector2D>::Get(),reinterpret_cast<const uint8*>(&Value));ScriptTimerStartImpl(N,Seconds,H,Repeat,C,&V);}
    void ScriptTimerNextTickVector2d(const char* N,const char* H,const FVector2D& Value,das::Context* C){FDasScalarValue V;V.Kind=12;V.ObjectClass=TEXT("/Script/CoreUObject.Vector2D");V.StructValue.InitializeAs(TBaseStructure<FVector2D>::Get(),reinterpret_cast<const uint8*>(&Value));ScriptTimerStartImpl(N,0.f,H,false,C,&V,true);}
    bool ScriptTimerCancel(const char* Name,das::Context* C){auto* S=ManagedOwnerSession(C);auto* T=FindScriptTimer(S,TimerName(Name,C));if(!T)return false;ScriptTimers.Remove(T->Id);return true;}
    bool ScriptTimerPause(const char* Name,das::Context* C){auto* S=ManagedOwnerSession(C);auto* T=FindScriptTimer(S,TimerName(Name,C));if(!T || !T->World.IsValid())return false;T->World->GetTimerManager().PauseTimer(T->Handle);return true;}
    bool ScriptTimerResume(const char* Name,das::Context* C){auto* S=ManagedOwnerSession(C);auto* T=FindScriptTimer(S,TimerName(Name,C));if(!T || !T->World.IsValid())return false;T->World->GetTimerManager().UnPauseTimer(T->Handle);return true;}
    bool ScriptTimerActive(const char* Name,das::Context* C){auto* S=ManagedOwnerSession(C);auto* T=FindScriptTimer(S,TimerName(Name,C));return T && T->World.IsValid() && T->World->GetTimerManager().IsTimerActive(T->Handle);}
    float ScriptTimerRemaining(const char* Name,das::Context* C){auto* S=ManagedOwnerSession(C);auto* T=FindScriptTimer(S,TimerName(Name,C));return T && T->World.IsValid()?T->World->GetTimerManager().GetTimerRemaining(T->Handle):-1.f;}
    void DrainScriptTimers(){
        if(ActiveContext || bDrainingScriptTimers || !IsInGameThread())return;
#if WITH_EDITOR
        if(bPreservingSessions)return;
#endif
        TGuardValue<bool> Guard(bDrainingScriptTimers,true);int32 Count=0;
        while(!PendingScriptTimers.IsEmpty() && Count++<1024){const uint64 Id=PendingScriptTimers[0];PendingScriptTimers.RemoveAt(0);auto* Slot=ScriptTimers.Find(Id);if(!Slot)continue;auto* T=Slot->Get();FPersistentSession* S=nullptr;for(const auto& Candidate:Sessions)if(Candidate->Live && Candidate->Live->Context.Get()==T->Context){S=Candidate.Get();break;}
            if(!S || S->bReleaseRequested || S->bEndDelivered || !T->Owner.IsValid() || !T->World.IsValid()){ScriptTimers.Remove(Id);continue;}if(auto* A=Cast<AActor>(T->Owner.Get());A && A->IsActorBeingDestroyed()){ScriptTimers.Remove(Id);continue;}
            const FString Path=S->Path,Handler=T->Handler;const TWeakObjectPtr<UObject> Owner=T->Owner;const auto Captured=T->Argument;FString CaptureIssue;if(Captured && (!Captured->IsAlive() || !ValidateDasCallbackPayload(Captured->GetValue(),EDasCallbackUsage::Timer,CaptureIssue))){ScriptTimers.Remove(Id);continue;}if(!T->Repeat)ScriptTimers.Remove(Id);
            auto& Runtime=FModuleManager::LoadModuleChecked<FDasRuntimeModule>(TEXT("DasRuntime"));TArray<FDasScalarValue> Args;if(Captured)Args.Add(Captured->GetValue());auto Result=Captured?Runtime.RunNamedFramePersistent(Path,Handler,Args,EDasMethodSignature::Void,Owner):Runtime.RunNamedVoidPersistent(Path,Handler,Owner);
            if(!Result.bSucceeded){ScriptTimers.Remove(Id);UE_LOG(LogDasRuntime,Warning,TEXT("Script timer %s failed: %s"),*Handler,*Result.Error);if(auto* Error=DasHostString(Owner.Get(),TEXT("LastDasError")))*Error=Result.Error;}
        }
        if(!PendingScriptTimers.IsEmpty()){UE_LOG(LogDasRuntime,Warning,TEXT("Script timer dispatch budget exceeded"));PendingScriptTimers.Reset();}
    }

    uint64 NextSubscriptionId = 1;
    struct FPendingIntEvent { uint64 Id; TSharedPtr<FDasCallbackPayload> Capture; };
    TArray<FPendingIntEvent> PendingEvents;
    bool bDrainingEvents = false;

    void RememberInput(FPersistentSession& S,const FManagedSubscription& B){if(S.bRestoringInput)return;FInputRecipe R;R.Kind=B.InputKind;R.Name=B.Event;R.Handler=B.Handler;R.Phase=B.InputPhase;R.Trigger=B.EnhancedPhase;R.Consume=B.InputConsume;R.Paused=B.InputPaused;R.Asset.Reset(B.InputAsset.Get());auto* Old=S.InputRecipes.FindByPredicate([&](const auto& X){return X.Kind==R.Kind && (R.Kind==3?X.Asset.Get()==R.Asset.Get() && X.Trigger==R.Trigger:X.Name==R.Name && (R.Kind==2 || X.Phase==R.Phase));});if(Old)*Old=R;else S.InputRecipes.Add(MoveTemp(R));S.NextInputRestore=0;}

    UInputComponent* ScriptOwnerInput(das::Context* C){auto* Self=ResolveSelf(C);auto* Actor=Cast<AActor>(Self);if(!Actor)if(auto* Component=Cast<UActorComponent>(Self))Actor=Component->GetOwner();return Actor && IsValid(Actor->InputComponent)?Actor->InputComponent.Get():nullptr;}
    FName ScriptInputName(const char* Name,das::Context* C){const FString N=Name?UTF8_TO_TCHAR(Name):TEXT("");if(N.IsEmpty() || N.Len()>=NAME_SIZE || N.Equals(TEXT("None"),ESearchCase::IgnoreCase))C->throw_error("input mapping name is required");return FName(*N);}
    EInputEvent ScriptInputPhase(const char* Phase,das::Context* C){const FString P=Phase?UTF8_TO_TCHAR(Phase):TEXT("");if(P==TEXT("Pressed"))return IE_Pressed;if(P==TEXT("Released"))return IE_Released;if(P==TEXT("Repeat"))return IE_Repeat;if(P==TEXT("DoubleClick"))return IE_DoubleClick;C->throw_error("input action phase must be Pressed/Released/Repeat/DoubleClick");return IE_Pressed;}
    void BindScriptInput(const char* Name,const char* Handler,int32 Kind,EInputEvent Phase,bool Consume,bool Paused,das::Context* C){
        auto* S=ManagedOwnerSession(C);if(ActiveContext->bEnding)C->throw_error("cannot bind input during EndPlay");auto* Input=ScriptOwnerInput(C);if(!Input)C->throw_error("owner has no InputComponent; enable input or complete possession before binding");const FName Mapping=ScriptInputName(Name,C);const FString Entry=Handler?UTF8_TO_TCHAR(Handler):TEXT("");FDasCallContract Contract=FDasCallContract::Scalar(EDasMethodSignature::Void);if(Kind==2)Contract.Inputs.Add(EDasMethodSignature::Float);if(!VerifyContract(S->Live->Program,Entry,Contract))C->throw_error("input requires matching exported void action/float axis handler");int32 Old=INDEX_NONE,Count=0;
        for(int32 I=0;I<S->Bindings.Num();++I){const auto& B=*S->Bindings[I];if(B.InputKind)++Count;if(B.InputKind==Kind && B.Event==Mapping && (Kind==2 || B.InputPhase==Phase))Old=I;}if(Old==INDEX_NONE && Count>=64)C->throw_error("input binding limit exceeded");if(Old!=INDEX_NONE)S->Bindings.RemoveAt(Old);
        auto B=MakeUnique<FManagedSubscription>();B->Id=NextSubscriptionId++;B->Source=Input;B->Event=Mapping;B->Handler=Entry;B->Kind=Kind==1?EDasMethodSignature::Void:EDasMethodSignature::Float;B->InputKind=Kind;B->InputPhase=Phase;B->InputConsume=Consume;B->InputPaused=Paused;B->Proxy.Reset(NewObject<UDasIntDelegateProxy>());B->Proxy->SubscriptionId=B->Id;
        if(Kind==1){FInputActionBinding Action(Mapping,Phase);Action.bConsumeInput=Consume;Action.bExecuteWhenPaused=Paused;Action.ActionDelegate.BindDelegate(B->Proxy.Get(),GET_FUNCTION_NAME_CHECKED(UDasIntDelegateProxy,ReceiveVoid));B->InputHandle=Input->AddActionBinding(MoveTemp(Action)).GetHandle();}
        else {FInputAxisBinding Axis(Mapping);Axis.bConsumeInput=Consume;Axis.bExecuteWhenPaused=Paused;Axis.AxisDelegate.BindDelegate(B->Proxy.Get(),GET_FUNCTION_NAME_CHECKED(UDasIntDelegateProxy,ReceiveFloat));Input->AxisBindings.Add(MoveTemp(Axis));}
        RememberInput(*S,*B);S->Bindings.Add(MoveTemp(B));S->RequiredEntries.Add(Entry,Contract);
    }
    void ScriptEnhancedAddContext(const FDasObjectHandle&,const FDasObjectHandle&,int32,das::Context*);
    bool ScriptEnhancedRemoveContext(const FDasObjectHandle&,const FDasObjectHandle&,das::Context*);
    bool ScriptEnhancedHasContext(const FDasObjectHandle&,const FDasObjectHandle&,das::Context*);
    int32 ScriptEnhancedClearContexts(das::Context*);
    void ScriptEnhancedBind(const FDasObjectHandle&,const char*,const char*,das::Context*);
    bool ScriptEnhancedUnbind(const FDasObjectHandle&,const char*,das::Context*);
    bool ScriptEnhancedAvailable(das::Context*);
    void ScriptInputAction(const char* Name,const char* Phase,const char* Handler,bool Consume,bool Paused,das::Context* C){BindScriptInput(Name,Handler,1,ScriptInputPhase(Phase,C),Consume,Paused,C);}
    void ScriptInputAxis(const char* Name,const char* Handler,bool Consume,bool Paused,das::Context* C){BindScriptInput(Name,Handler,2,IE_Pressed,Consume,Paused,C);}
    bool ScriptInputAvailable(das::Context* C){ManagedOwnerSession(C);return ScriptOwnerInput(C)!=nullptr;}
    bool ScriptInputUnbindAction(const char* Name,const char* Phase,das::Context* C){auto* S=ManagedOwnerSession(C);const FName N=ScriptInputName(Name,C);const auto P=ScriptInputPhase(Phase,C);const int32 Wanted=S->InputRecipes.RemoveAll([&](const auto& R){return R.Kind==1 && R.Name==N && R.Phase==P;});return S->Bindings.RemoveAll([&](const auto& B){return B->InputKind==1 && B->Event==N && B->InputPhase==P;})>0 || Wanted>0;}
    bool ScriptInputUnbindAxis(const char* Name,das::Context* C){auto* S=ManagedOwnerSession(C);const FName N=ScriptInputName(Name,C);const int32 Wanted=S->InputRecipes.RemoveAll([&](const auto& R){return R.Kind==2 && R.Name==N;});return S->Bindings.RemoveAll([&](const auto& B){return B->InputKind==2 && B->Event==N;})>0 || Wanted>0;}
    int32 ScriptInputUnbindAll(das::Context* C){auto* S=ManagedOwnerSession(C);const int32 Count=S->InputRecipes.Num();S->InputRecipes.Empty();S->Bindings.RemoveAll([](const auto& B){return B->InputKind!=0;});return Count;}


    ETriggerEvent ScriptEnhancedPhase(const char* Phase,das::Context* C){const FString P=Phase?UTF8_TO_TCHAR(Phase):TEXT("");if(P==TEXT("Triggered"))return ETriggerEvent::Triggered;if(P==TEXT("Started"))return ETriggerEvent::Started;if(P==TEXT("Ongoing"))return ETriggerEvent::Ongoing;if(P==TEXT("Canceled"))return ETriggerEvent::Canceled;if(P==TEXT("Completed"))return ETriggerEvent::Completed;C->throw_error("Enhanced Input trigger must be Triggered/Started/Ongoing/Canceled/Completed");return ETriggerEvent::None;}
    void ScriptEnhancedAddContext(const FDasObjectHandle&,const FDasObjectHandle&,int32,das::Context*);
    bool ScriptEnhancedRemoveContext(const FDasObjectHandle&,const FDasObjectHandle&,das::Context*);
    bool ScriptEnhancedHasContext(const FDasObjectHandle&,const FDasObjectHandle&,das::Context*);
    int32 ScriptEnhancedClearContexts(das::Context*);
    void ScriptEnhancedBind(const FDasObjectHandle& ActionHandle,const char* Trigger,const char* Handler,das::Context* C){
        auto* S=ManagedOwnerSession(C);if(ActiveContext->bEnding)C->throw_error("cannot bind Enhanced Input during EndPlay");auto* Input=Cast<UEnhancedInputComponent>(ScriptOwnerInput(C));if(!Input)C->throw_error("owner requires an existing EnhancedInputComponent");auto* Action=Cast<UInputAction>(ResolveObject(ActionHandle,C));if(!Action)C->throw_error("Enhanced Input requires an InputAction asset");const auto Phase=ScriptEnhancedPhase(Trigger,C);EDasMethodSignature Kind;
        switch(Action->ValueType){case EInputActionValueType::Boolean:Kind=EDasMethodSignature::Bool;break;case EInputActionValueType::Axis1D:Kind=EDasMethodSignature::Float;break;case EInputActionValueType::Axis2D:Kind=EDasMethodSignature::Struct;break;case EInputActionValueType::Axis3D:Kind=EDasMethodSignature::Vector;break;default:C->throw_error("unsupported Enhanced Input value shape");return;}
        const FString Entry=Handler?UTF8_TO_TCHAR(Handler):TEXT("");FDasCallContract Contract=FDasCallContract::Scalar(EDasMethodSignature::Void);Contract.Inputs.Add(EDasMethodSignature::Struct);Contract.InputClasses.Add(TEXT("/Script/EnhancedInput.InputActionValue"));const bool TypedValue=VerifyContract(S->Live->Program,Entry,Contract,true);if(TypedValue)Kind=EDasMethodSignature::Struct;else{Contract.Inputs.Reset();Contract.InputClasses.Reset();Contract.Inputs.Add(Kind);if(Kind==EDasMethodSignature::Struct)Contract.InputClasses.Add(TEXT("/Script/CoreUObject.Vector2D"));if(!VerifyContract(S->Live->Program,Entry,Contract))C->throw_error("Enhanced Input requires InputActionValue or a matching bool/float/Vector2D/Vector void handler");}int32 Old=INDEX_NONE,Count=0;for(int32 I=0;I<S->Bindings.Num();++I){const auto& B=*S->Bindings[I];if(B.InputKind)++Count;if(B.InputKind==3 && B.InputAsset.Get()==Action && B.EnhancedPhase==Phase)Old=I;}if(Old==INDEX_NONE && Count>=64)C->throw_error("input binding limit exceeded");if(Old!=INDEX_NONE)S->Bindings.RemoveAt(Old);
        auto B=MakeUnique<FManagedSubscription>();B->Id=NextSubscriptionId++;B->Source=Input;B->Handler=Entry;B->Kind=Kind;if(Kind==EDasMethodSignature::Struct)B->PayloadPath=TypedValue?TEXT("/Script/EnhancedInput.InputActionValue"):TEXT("/Script/CoreUObject.Vector2D");B->InputKind=3;B->EnhancedPhase=Phase;B->InputAsset.Reset(Action);B->EnhancedProxy.Reset(NewObject<UDasEnhancedInputProxy>());B->EnhancedProxy->SubscriptionId=B->Id;B->EnhancedProxy->PayloadKind=int32(Kind);B->EnhancedProxy->TypedValue=TypedValue;B->EnhancedProxy->Action=Action;if(TypedValue)S->NativeInputEntries.Add(Entry);B->EnhancedHandle=Input->BindAction(Action,Phase,B->EnhancedProxy.Get(),GET_FUNCTION_NAME_CHECKED(UDasEnhancedInputProxy,Receive)).GetHandle();RememberInput(*S,*B);S->Bindings.Add(MoveTemp(B));S->RequiredEntries.Add(Entry,Contract);
    }
    bool ScriptEnhancedUnbind(const FDasObjectHandle& H,const char* Trigger,das::Context* C){auto* S=ManagedOwnerSession(C);auto* Action=Cast<UInputAction>(ResolveObject(H,C));if(!Action)C->throw_error("Enhanced Input requires an InputAction asset");const auto Phase=ScriptEnhancedPhase(Trigger,C);const int32 Wanted=S->InputRecipes.RemoveAll([&](const auto& R){return R.Kind==3 && R.Asset.Get()==Action && R.Trigger==Phase;});return S->Bindings.RemoveAll([&](const auto& B){return B->InputKind==3 && B->InputAsset.Get()==Action && B->EnhancedPhase==Phase;})>0 || Wanted>0;}
    bool ScriptEnhancedAvailable(das::Context* C){ManagedOwnerSession(C);return Cast<UEnhancedInputComponent>(ScriptOwnerInput(C))!=nullptr;}


    UEnhancedInputLocalPlayerSubsystem* ScriptEnhancedSubsystem(const FDasObjectHandle& H,FPersistentSession* S,das::Context* C){auto* Player=Cast<APlayerController>(ResolveObject(H,C));if(!Player || Player->GetWorld()!=S->World.Get() || !Player->IsLocalController() || !Player->GetLocalPlayer())C->throw_error("mapping context requires explicit local PlayerController in owner's world");auto* Subsystem=ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(Player->GetLocalPlayer());if(!Subsystem || !Subsystem->GetPlayerInput())C->throw_error("Enhanced Input LocalPlayer subsystem/player input is not ready");return Subsystem;}
    UInputMappingContext* ScriptMappingContext(const FDasObjectHandle& H,das::Context* C){auto* Context=Cast<UInputMappingContext>(ResolveObject(H,C));if(!Context)C->throw_error("expected InputMappingContext asset");return Context;}
    void ScriptEnhancedAddContext(const FDasObjectHandle& Player,const FDasObjectHandle& Mapping,int32 Priority,das::Context* C){auto* S=ManagedOwnerSession(C);if(ActiveContext->bEnding)C->throw_error("cannot add mapping contexts during EndPlay");auto* Subsystem=ScriptEnhancedSubsystem(Player,S,C);auto* Context=ScriptMappingContext(Mapping,C);auto* Slot=S->InputContexts.FindByPredicate([&](const auto& L){return L->Subsystem.Get()==Subsystem && L->Context.Get()==Context;});int32 Actual=0;const bool Present=Subsystem->HasMappingContext(Context,Actual);
        if(!Slot && Present)C->throw_error("mapping context already installed outside this session; bind to it without acquiring ownership");if(Slot && Present && Actual!=(*Slot)->Priority)C->throw_error("owned mapping context priority changed externally; refusing to overwrite it");if(!Slot && S->InputContexts.Num()>=32)C->throw_error("mapping context count limit exceeded");
        auto New=MakeUnique<FManagedInputContext>();New->Subsystem=Subsystem;New->Context.Reset(Context);New->Priority=Priority;FModifyContextOptions Options;Options.bNotifyUserSettings=false;Subsystem->AddMappingContext(Context,Priority,Options);if(!Subsystem->HasMappingContext(Context,Actual) || Actual!=Priority)C->throw_error("mapping context installation/priority could not be confirmed");if(Slot){(*Slot)->Priority=Priority;New->Context.Reset();}else S->InputContexts.Add(MoveTemp(New));if(!S->bRestoringInput){FInputContextRecipe R;R.Subsystem=Subsystem;R.Context.Reset(Context);R.Priority=Priority;auto* Prior=S->ContextRecipes.FindByPredicate([&](const auto& X){return X.Subsystem.Get()==Subsystem && X.Context.Get()==Context;});if(Prior)*Prior=R;else S->ContextRecipes.Add(MoveTemp(R));}
    }
    bool ScriptEnhancedRemoveContext(const FDasObjectHandle& Player,const FDasObjectHandle& Mapping,das::Context* C){auto* S=ManagedOwnerSession(C);auto* Subsystem=ScriptEnhancedSubsystem(Player,S,C);auto* Context=ScriptMappingContext(Mapping,C);const int32 Wanted=S->ContextRecipes.RemoveAll([&](const auto& R){return R.Subsystem.Get()==Subsystem && R.Context.Get()==Context;});return S->InputContexts.RemoveAll([&](const auto& L){return L->Subsystem.Get()==Subsystem && L->Context.Get()==Context;})>0 || Wanted>0;}
    bool ScriptEnhancedHasContext(const FDasObjectHandle& Player,const FDasObjectHandle& Mapping,das::Context* C){auto* S=ManagedOwnerSession(C);auto* Subsystem=ScriptEnhancedSubsystem(Player,S,C);return Subsystem->HasMappingContext(ScriptMappingContext(Mapping,C));}
    int32 ScriptEnhancedClearContexts(das::Context* C){auto* S=ManagedOwnerSession(C);const int32 Count=S->ContextRecipes.Num();S->ContextRecipes.Empty();S->InputContexts.Empty();return Count;}


    void ReconcileScriptInput(UWorld* World){
        if(ActiveContext || !IsInGameThread())return;
#if WITH_EDITOR
        if(bPreservingSessions)return;
#endif
        const double Now=FPlatformTime::Seconds();for(auto& Owned:Sessions){auto& S=*Owned;if(!S.Live || S.bReleaseRequested || S.bEndDelivered || S.World.Get()!=World || !S.Owner.IsValid() || Now<S.NextInputRestore || (S.InputRecipes.IsEmpty() && S.ContextRecipes.IsEmpty()))continue;if(auto* A=Cast<AActor>(S.Owner.Get());A && A->IsActorBeingDestroyed())continue;
            auto& C=*S.Live->Context;FActiveContextScope Scope(C);TGuardValue<bool> Restoring(S.bRestoringInput,true);try{
                for(const auto& R:S.ContextRecipes){auto* Subsystem=R.Subsystem.Get();if(!Subsystem || !Subsystem->GetPlayerInput())continue;auto* Player=Cast<APlayerController>(Subsystem->GetPlayerInput()->GetOuter());if(!Player || Player->GetWorld()!=World)continue;const bool OwnedContext=S.InputContexts.ContainsByPredicate([&](const auto& L){return L->Subsystem.Get()==Subsystem && L->Context.Get()==R.Context.Get();});if(!OwnedContext)ScriptEnhancedAddContext(MakeObjectHandle(Player,APlayerController::StaticClass(),&C),MakeObjectHandle(R.Context.Get(),UInputMappingContext::StaticClass(),&C),R.Priority,&C);}
                auto* Input=ScriptOwnerInput(&C);S.Bindings.RemoveAll([&](const auto& B){return B->InputKind && B->Source.Get()!=Input;});if(Input){for(const auto& R:S.InputRecipes){const bool Bound=S.Bindings.ContainsByPredicate([&](const auto& B){return B->InputKind==R.Kind && B->Source.Get()==Input && (R.Kind==3?B->InputAsset.Get()==R.Asset.Get() && B->EnhancedPhase==R.Trigger:B->Event==R.Name && (R.Kind==2 || B->InputPhase==R.Phase));});if(Bound)continue;const FTCHARToUTF8 Name(*R.Name.ToString()),Handler(*R.Handler);if(R.Kind==1){const char* Phase=R.Phase==IE_Pressed?"Pressed":R.Phase==IE_Released?"Released":R.Phase==IE_Repeat?"Repeat":"DoubleClick";ScriptInputAction(Name.Get(),Phase,Handler.Get(),R.Consume,R.Paused,&C);}else if(R.Kind==2)ScriptInputAxis(Name.Get(),Handler.Get(),R.Consume,R.Paused,&C);else if(Cast<UEnhancedInputComponent>(Input)){const char* Phase=R.Trigger==ETriggerEvent::Triggered?"Triggered":R.Trigger==ETriggerEvent::Started?"Started":R.Trigger==ETriggerEvent::Ongoing?"Ongoing":R.Trigger==ETriggerEvent::Canceled?"Canceled":"Completed";ScriptEnhancedBind(MakeObjectHandle(R.Asset.Get(),UInputAction::StaticClass(),&C),Phase,Handler.Get(),&C);}}}S.InputRestoreError.Empty();
            }catch(const std::exception& E){const FString Error=UTF8_TO_TCHAR(E.what());if(S.InputRestoreError!=Error)UE_LOG(LogDasRuntime,Warning,TEXT("Input restore deferred: %s"),*Error);S.InputRestoreError=Error;S.NextInputRestore=Now+1.0;C.restart();}
        }
    }


    class FScriptDelayAction:public FPendingLatentAction {
        float Remaining;FName Function;int32 Linkage;FWeakObjectPtr Target;
    public:
        FScriptDelayAction(float Seconds,const FLatentActionInfo& Info):Remaining(Seconds),Function(Info.ExecutionFunction),Linkage(Info.Linkage),Target(Info.CallbackTarget){}
        virtual void UpdateOperation(FLatentResponse& Response)override{Remaining-=Response.ElapsedTime();Response.FinishAndTriggerIf(Remaining<=0.f,Function,Linkage,Target);}
    };
    void ScriptAssetLoad(const char*,const char*,const char*,const char*,das::Context*);
    void ScriptAssetLoadGeneric(const char*,const char*,const char*,das::Context*);
    bool ScriptAssetCancel(const char*,das::Context*);
    int32 ScriptAssetCancelAll(das::Context*);
    bool ScriptAssetPending(const char*,das::Context*);
    void ScriptLatentDelay(const char* Name,float Seconds,const char* Handler,das::Context* C){auto* S=ManagedOwnerSession(C);if(ActiveContext->bEnding)C->throw_error("cannot start latent work during EndPlay");auto* W=S->World.Get();if(!W || W!=S->Owner->GetWorld() || !W->IsGameWorld())C->throw_error("latent delay requires owner's live game world");const FString N=TimerName(Name,C),Entry=Handler?UTF8_TO_TCHAR(Handler):TEXT("");if(!FMath::IsFinite(Seconds) || Seconds<0.f)C->throw_error("latent delay duration must be finite and nonnegative");const auto Contract=FDasCallContract::Scalar(EDasMethodSignature::Void);if(!VerifyContract(S->Live->Program,Entry,Contract))C->throw_error("latent delay requires exported handler():void");int32 Count=0,Old=INDEX_NONE;for(int32 I=0;I<S->Bindings.Num();++I){const auto& B=*S->Bindings[I];if(B.LatentKind)++Count;if(B.LatentKind==1 && B.LatentName==N)Old=I;}if(Old==INDEX_NONE && Count>=64)C->throw_error("latent delay count limit exceeded");if(Old!=INDEX_NONE)S->Bindings.RemoveAt(Old);
        auto B=MakeUnique<FManagedSubscription>();B->Id=NextSubscriptionId++;B->Handler=Entry;B->Kind=EDasMethodSignature::Void;B->LatentKind=1;B->LatentName=N;B->LatentWorld=W;B->LatentProxy.Reset(NewObject<UDasLatentProxy>());B->LatentProxy->SubscriptionId=B->Id;B->Source=B->LatentProxy.Get();FLatentActionInfo Info;Info.CallbackTarget=B->LatentProxy.Get();Info.ExecutionFunction=GET_FUNCTION_NAME_CHECKED(UDasLatentProxy,Complete);Info.Linkage=0;Info.UUID=1;W->GetLatentActionManager().AddNewAction(B->LatentProxy.Get(),Info.UUID,new FScriptDelayAction(Seconds,Info));S->Bindings.Add(MoveTemp(B));S->RequiredEntries.Add(Entry,Contract);
    }
    bool ScriptLatentCancel(const char* Name,das::Context* C){auto* S=ManagedOwnerSession(C);const FString N=TimerName(Name,C);return S->Bindings.RemoveAll([&](const auto& B){return B->LatentKind==1 && B->LatentName==N;})>0;}
    int32 ScriptLatentCancelAll(das::Context* C){auto* S=ManagedOwnerSession(C);return S->Bindings.RemoveAll([](const auto& B){return B->LatentKind!=0;});}
    bool ScriptLatentPending(const char* Name,das::Context* C){auto* S=ManagedOwnerSession(C);const FString N=TimerName(Name,C);return S->Bindings.ContainsByPredicate([&](const auto& B){return B->LatentKind==1 && B->LatentName==N;});}


    void ScriptAssetLoadImpl(const char* Name,const char* Asset,const char* Handler,const char* Class,bool ClassReference,bool AllowNull,das::Context* C){auto* S=ManagedOwnerSession(C);if(ActiveContext->bEnding)C->throw_error("cannot start asset loading during EndPlay");if(!S->World.IsValid() || !S->World->IsGameWorld())C->throw_error("async asset loading requires owner's game world");const FString N=TimerName(Name,C),PathText=Asset?UTF8_TO_TCHAR(Asset):TEXT(""),Entry=Handler?UTF8_TO_TCHAR(Handler):TEXT(""),ClassPath=Class?UTF8_TO_TCHAR(Class):TEXT("");if((PathText.IsEmpty() && !AllowNull) || PathText.Len()>1024 || (!PathText.IsEmpty() && !FPackageName::IsValidObjectPath(PathText)))C->throw_error("async asset load requires a valid Unreal object path");auto* Expected=FindObject<UClass>(nullptr,*ClassPath);if(!Expected || !ClassPath.StartsWith(TEXT("/Script/")) || Expected->HasAnyClassFlags(CLASS_Interface))C->throw_error("async asset callback requires a loaded native object class");FDasCallContract Contract=FDasCallContract::Scalar(EDasMethodSignature::Void);Contract.Inputs.Add(ClassReference?EDasMethodSignature::Class:EDasMethodSignature::Object);Contract.InputClasses.Add(ClassPath);if(!VerifyContract(S->Live->Program,Entry,Contract))C->throw_error("async asset handler must return void with matching object input");int32 Old=INDEX_NONE,Count=0;for(int32 I=0;I<S->Bindings.Num();++I){const auto& B=*S->Bindings[I];if(B.AsyncKind)++Count;if(B.AsyncKind && B.AsyncName==N)Old=I;}if(Old==INDEX_NONE && Count>=64)C->throw_error("async asset request count limit exceeded");
        auto B=MakeUnique<FManagedSubscription>();B->Id=NextSubscriptionId++;B->Handler=Entry;B->Kind=ClassReference?EDasMethodSignature::Class:EDasMethodSignature::Object;B->PayloadPath=ClassPath;B->AsyncKind=1;B->AsyncName=N;B->AsyncProxy.Reset(NewObject<UDasAsyncAssetProxy>());B->AsyncProxy->SubscriptionId=B->Id;B->AsyncProxy->Path=FSoftObjectPath(PathText);B->AsyncProxy->ExpectedClass=ClassPath;B->AsyncProxy->ClassReference=ClassReference;B->Source=B->AsyncProxy.Get();const TWeakObjectPtr<UDasAsyncAssetProxy> Target(B->AsyncProxy.Get());
        if(!PathText.IsEmpty())B->StreamHandle=UAssetManager::GetStreamableManager().RequestAsyncLoad(B->AsyncProxy->Path,FStreamableDelegate::CreateLambda([Target](){if(auto* P=Target.Get())P->Complete();}),FStreamableManager::DefaultAsyncLoadPriority,false,true,TEXT("DasForUE:")+N);if(!PathText.IsEmpty() && !B->StreamHandle.IsValid())C->throw_error("async asset request could not be created");auto Handle=B->StreamHandle;if(Old!=INDEX_NONE)S->Bindings.RemoveAt(Old);S->Bindings.Add(MoveTemp(B));S->RequiredEntries.Add(Entry,Contract);if(Handle.IsValid())Handle->StartStalledHandle();else if(auto* Proxy=Target.Get())Proxy->Complete();
    }
    void ScriptAssetLoad(const char* N,const char* Path,const char* Handler,const char* Class,das::Context* C){ScriptAssetLoadImpl(N,Path,Handler,Class,false,false,C);}
    void ScriptSoftAssetLoad(const char* N,const FDasStructValue& H,const char* Handler,das::Context* C){const auto& V=SoftValue(H,C);auto* P=DasSoftProperty(V.GetScriptStruct()->GetPathName());auto* Class=CastField<FSoftClassProperty>(P);const FTCHARToUTF8 Path(*SoftAssetPath(V).ToString()),Base(*(Class?Class->MetaClass.Get():P->PropertyClass.Get())->GetPathName());ScriptAssetLoadImpl(N,Path.Get(),Handler,Base.Get(),Class!=nullptr,true,C);}
    void ScriptAssetLoadGeneric(const char* N,const char* Path,const char* Handler,das::Context* C){ScriptAssetLoad(N,Path,Handler,"/Script/CoreUObject.Object",C);}
    bool ScriptAssetCancel(const char* Name,das::Context* C){auto* S=ManagedOwnerSession(C);const FString N=TimerName(Name,C);return S->Bindings.RemoveAll([&](const auto& B){return B->AsyncKind && B->AsyncName==N;})>0;}
    int32 ScriptAssetCancelAll(das::Context* C){auto* S=ManagedOwnerSession(C);return S->Bindings.RemoveAll([](const auto& B){return B->AsyncKind!=0;});}
    bool ScriptAssetPending(const char* Name,das::Context* C){auto* S=ManagedOwnerSession(C);const FString N=TimerName(Name,C);return S->Bindings.ContainsByPredicate([&](const auto& B){return B->AsyncKind && B->AsyncName==N;});}

#include "DasMontage.inl"
#include "DasAI.inl"
#include "DasEffects.inl"

    void RevokeSession(FPersistentSession& Session)
    {
        UDasAsyncPhysicsBridge::RevokeOwner(Session.Owner.GetEvenIfUnreachable());
        ClearSessionTimers(Session);
        Session.InputRecipes.Empty();Session.ContextRecipes.Empty();
        Session.bReleaseRequested = true;
        Session.Bindings.Empty();
        Session.InputContexts.Empty();
    }
    void FlushReleasedSessions()
    {
        if (!ActiveContext)
            Sessions.RemoveAll([](const auto& Session) { return Session->bReleaseRequested; });
    }
    void DrainEvents();
    struct FDrainEventsOnExit { ~FDrainEventsOnExit() { DrainEvents(); } };

    void InstallSubscriptions(FPersistentSession& Session)
    {
        for (const auto& Declaration : Session.Live->Context->Subscriptions)
        {
            UObject* Source = Declaration.Source.Get();
            if (!IsValid(Source)) continue;
            auto* Property = ScalarEventProperty(Source,Declaration.Event,Declaration.Kind);
            if (!Property) continue;
            auto Binding = MakeUnique<FManagedSubscription>();
            Binding->Id = NextSubscriptionId++;
            Binding->Source = Source;
            Binding->Event = Declaration.Event;
            Binding->Handler = Declaration.Handler;Binding->Kind=Declaration.Kind;Binding->PayloadPath=Declaration.PayloadPath;
            Binding->Proxy.Reset(NewObject<UDasIntDelegateProxy>());
            Binding->Proxy->SubscriptionId = Binding->Id;
            FScriptDelegate Delegate;
            const auto* CallbackType=DasCallbackTypeForProperty(ScalarEventParameter(Property));
            if(FindDasCallbackFrame(Binding->PayloadPath) || (CallbackType && CallbackType->bDynamic)){FString Error;Binding->ProxyFunction=Binding->Proxy->ConfigureDynamic(Property->SignatureFunction,Binding->Kind==EDasMethodSignature::Object?DasReferencePropertyClass(ScalarEventParameter(Property))->GetPathName():Binding->PayloadPath,Binding->Kind,Error);if(Binding->ProxyFunction.IsNone()){UE_LOG(LogDasRuntime,Error,TEXT("Dynamic event receiver: %s"),*Error);continue;}}
            else Binding->ProxyFunction=EventProxyFunction(Binding->Kind,Property);
            Delegate.BindUFunction(Binding->Proxy.Get(),Binding->ProxyFunction);
            Property->AddDelegate(Delegate, Source, Property->ContainerPtrToValuePtr<void>(Source));
            Session.Bindings.Add(MoveTemp(Binding));
        }
    }

    void DrainEvents()
    {
        if (ConstructingObject || ActiveContext || bDrainingEvents || !IsInGameThread()) return;
        DrainScriptTimers();
#if WITH_EDITOR
        if(bPreservingSessions)return;
#endif
        TGuardValue<bool> Draining(bDrainingEvents, true);
        FlushReleasedSessions();
        int32 Index = 0;
        while (Index < PendingEvents.Num() && Index < 1024)
        {
            FlushReleasedSessions();
            const auto Event = PendingEvents[Index++];
            const auto& Payload=Event.Capture->GetValue();FString CaptureIssue;if(!Event.Capture->IsAlive() || !ValidateDasCallbackPayload(Payload,EDasCallbackUsage::Event,CaptureIssue))continue;
            FPersistentSession* Session = nullptr;
            FString Handler,EventObjectPath;
            for (const auto& Candidate : Sessions)
            {
                for (const auto& Binding : Candidate->Bindings)
                    if (Binding->Id == Event.Id && Binding->Source.IsValid() && int32(Binding->Kind)==Payload.Kind && ((Payload.Kind!=12 && Payload.Kind!=13 && Payload.Kind!=15) || Binding->PayloadPath==Payload.ObjectClass))
                    { Session = Candidate.Get(); Handler = Binding->Handler;EventObjectPath=Binding->PayloadPath; break; }
                if (Session) break;
            }
            if (!Session || !Session->Live || !Session->Owner.IsValid() || Session->bEndDelivered || Session->bReleaseRequested) continue;
            if (auto* Actor = Cast<AActor>(Session->Owner.Get()); Actor && Actor->IsActorBeingDestroyed()) continue;
            auto& Context = *Session->Live->Context;
            const auto* MontagePayload=Payload.Kind==int32(EDasMethodSignature::Struct)?Payload.StructValue.GetPtr<FDasMontageEvent>():nullptr;
            const bool MontageTerminal=MontagePayload && (MontagePayload->Kind==EDasMontageEventKind::Completed || MontagePayload->Kind==EDasMontageEventKind::Interrupted);
            Session->Bindings.RemoveAll([&](const auto& B){return B->Id==Event.Id && (B->LatentKind!=0 || B->AsyncKind!=0 || B->AIMoveProxy.IsValid() || B->EffectProxy.IsValid() || (B->MontageProxy.IsValid() && MontageTerminal));}); // Consume completion before user rearm.
            Context.restart();
            TGuardValue<int32> ExecutionScope(Context.ExecutionDepth,Context.ExecutionDepth+1);
            FActiveContextScope Scope(Context);
            FStructCollectOnExit CollectStructs{Context};
            FString Error;
            try
            {
                const FTCHARToUTF8 Name(*Handler);
                auto* Function = Context.findFunction(Name.Get());
                auto Declaration=Session->Live->Program->getThisModule()->findUniqueFunction(Name.Get());
                if (!Function) continue; // Candidate validation makes this unreachable for a current binding.
                FDasInputActionValue InputArg;FDasStructValue StructArg;FDasObjectHandle ObjectArg;FVector VectorArg=Payload.VectorValue;FRotator RotatorArg=Payload.RotatorValue;FTransform TransformArg=Payload.TransformValue;vec4f Args[1];if(Payload.Kind==15)Args[0]=das::cast<FDasClassHandle>::from({MakeObjectHandle(Payload.ClassValue.Get(),UClass::StaticClass(),&Context).Token});else if(Payload.Kind==13)Args[0]=das::cast<FDasEnumValue>::from(MakeEnumValue(Payload.ObjectClass,Payload.IntValue,&Context));else if(Payload.Kind==17)Args[0]=das::cast<uint8_t>::from(Payload.ByteValue);else if(Payload.Kind==18)Args[0]=das::cast<FDasTextValue>::from(MakeDSLText(Payload.TextValue,&Context));else if(Payload.Kind==7)Args[0]=das::cast<FDasNameValue>::from(MakeDSLName(Payload.NameValue,&Context));else if(Payload.Kind==12){if(Payload.ObjectClass==TEXT("/Script/EnhancedInput.InputActionValue") && Declaration && Declaration->arguments.size()==1 && Declaration->arguments[0]->type->annotation && Declaration->arguments[0]->type->annotation->name=="InputActionValue"){InputArg=SnapshotInputValue(Payload.StructValue.Get<FInputActionValue>(),&Context);Args[0]=das::cast<void*>::from(&InputArg);}else{StructArg=StoreStructValue(Payload.StructValue,&Context);Args[0]=das::cast<void*>::from(&StructArg);}}else if(Payload.Kind==6)Args[0]=das::cast<double>::from(Payload.DoubleValue);else if(Payload.Kind==8){auto* Expected=FindObject<UClass>(nullptr,*(EventObjectPath.IsEmpty()?FString(TEXT("/Script/CoreUObject.Object")):EventObjectPath));if(!Expected || !DasReferenceMatches(Payload.ObjectValue.Get(),Expected))continue;ObjectArg=MakeObjectHandle(Payload.ObjectValue.Get(),Expected,&Context);Args[0]=das::cast<void*>::from(&ObjectArg);}else if(Payload.Kind==9)Args[0]=das::cast<void*>::from(&VectorArg);else if(Payload.Kind==10)Args[0]=das::cast<void*>::from(&RotatorArg);else if(Payload.Kind==11)Args[0]=das::cast<void*>::from(&TransformArg);else if(Payload.Kind==0)Args[0]=das::cast<int32>::from(Payload.IntValue);else if(Payload.Kind==1)Args[0]=das::cast<float>::from(Payload.FloatValue);else if(Payload.Kind==2)Args[0]=das::cast<bool>::from(Payload.BoolValue);else if(Payload.Kind==5)Args[0]=das::cast<char*>::from(CopyDSLString(Payload.StringValue,&Context));
                Context.evalWithCatch(Function,Payload.Kind==3?nullptr:Args);
                if (const char* Exception = Context.getException()) Error = UTF8_TO_TCHAR(Exception);
            }
            catch (const std::exception& Exception) { Error = UTF8_TO_TCHAR(Exception.what()); }
            if (!Error.IsEmpty())
            {
                UE_LOG(LogDasRuntime, Display, TEXT("Event handler %s failed: %s"), *Handler, *Error);
                if(auto* HostError=DasHostString(Session->Owner.Get(),TEXT("LastDasError")))*HostError=Error;
            }
        }
        if (Index < PendingEvents.Num()) UE_LOG(LogDasRuntime, Display, TEXT("Event dispatch budget exceeded; remaining recursive events dropped."));
        PendingEvents.Reset();
        FlushReleasedSessions();
    }

    void PruneSessions()
    {
        for(auto It=DSLPhysicsOwners.CreateIterator();It;++It)if(!It.Key().IsValid() || !It.Value().IsValid())It.RemoveCurrent();
        for(auto It=InitializerDeliveries.CreateIterator();It;++It)if(!It.Key().IsValid())It.RemoveCurrent();
        for (const auto& Session : Sessions)
        {
            if (!Session->Owner.IsValid()) RevokeSession(*Session);
            else Session->Bindings.RemoveAll([](const auto& Binding) { return !Binding->Source.IsValid(); });
        }
        FlushReleasedSessions();
    }
}

void FDasRuntimeModule::StartupModule()
{
    FModuleManager::LoadModuleChecked<IModuleInterface>(TEXT("EnhancedInput"));
    Root = FPaths::ConvertRelativePathToFull(IPluginManager::Get().FindPlugin(TEXT("DasForUE"))->GetContentDir() / TEXT("Das"));
    const FTCHARToUTF8 Utf8Root(*Root);
    das::setDasRoot(das::string(Utf8Root.Get(), Utf8Root.Length()));
    NEED_ALL_DEFAULT_MODULES;
    das::Module::Initialize();
    new FUnrealFoundationModule();
    InputRestoreTickHandle=FWorldDelegates::OnWorldTickStart.AddLambda([](UWorld* World,ELevelTick,float){ReconcileScriptInput(World);});
    WorldCleanupHandle = FWorldDelegates::OnWorldCleanup.AddLambda([](UWorld* World, bool, bool)
    {
        UDasAsyncPhysicsBridge::RevokeWorld(World);
        for(auto It=InitializerDeliveries.CreateIterator();It;++It){auto* O=It.Key().Get();if(!O || O->GetWorld()==World)It.RemoveCurrent();}
        for (const auto& Session : Sessions)
            if (Session->World.Get() == World) RevokeSession(*Session);
#if WITH_EDITOR
        for (const auto& Session : PreservedSessions)
            if (Session->World.Get() == World) RevokeSession(*Session);
#endif
        FlushReleasedSessions();
    });
    PostGCHandle = FCoreUObjectDelegates::GetPostGarbageCollect().AddStatic(&PruneSessions);
#if WITH_EDITOR
    ObjectsReplacedHandle=FCoreUObjectDelegates::OnObjectsReplaced.AddLambda([](const FCoreUObjectDelegates::FReplacementObjectMap& Map){
        if(!IsInGameThread())return;
        for(const auto& Pair:Map)if(Pair.Key!=Pair.Value && IsValid(Pair.Value))if(auto* Old=InitializerDeliveries.Find(Pair.Key)){auto State=*Old;InitializerDeliveries.Remove(Pair.Key);InitializerDeliveries.Add(Pair.Value,MoveTemp(State));}
        for(auto& Session:PreservedSessions){
            if(UObject* const* New=Map.Find(Session->Owner.GetEvenIfUnreachable())){Session->Owner=*New;Session->World=*New?(*New)->GetWorld():nullptr;Session->bReinstanceOwnerChanged=true;}
            if(Session->Live){auto& C=*Session->Live->Context;if(UObject* const* New=Map.Find(C.Self.GetEvenIfUnreachable()))C.Self=*New;for(auto& Declaration:C.Subscriptions)if(UObject* const* New=Map.Find(Declaration.Source.GetEvenIfUnreachable()))Declaration.Source=*New;for(auto& Pair:C.DelegateValues){auto& V=Pair.Value;if(UObject* const* New=Map.Find(V.Source.GetEvenIfUnreachable()))V.Source=*New;auto Remap=[&](FScriptDelegate& D){if(UObject* const* New=Map.Find(const_cast<UObject*>(D.GetUObjectEvenIfUnreachable()))){const FName N=D.GetFunctionName();if(IsValid(*New))D.BindUFunction(*New,N);else D.Unbind();}};Remap(V.Single);for(auto& D:V.Listeners)Remap(D);}
            for(auto& Pair:C.ObjectHandles){auto& Entry=Pair.Value;if(UObject* const* New=Map.Find(Entry.ExpectedClass.GetEvenIfUnreachable()))Entry.ExpectedClass=Cast<UClass>(*New);if(UObject* const* New=Map.Find(Entry.Object.GetEvenIfUnreachable()))Entry.Object=*New;if(Entry.Object.IsValid() && (!Entry.ExpectedClass.IsValid() || !Entry.Object->IsA(Entry.ExpectedClass.Get())))Entry.Object.Reset();}}
        }
        for(const auto& Session:Sessions)if(Session->Live)for(auto& Pair:Session->Live->Context->ObjectHandles){auto& Entry=Pair.Value;
            if(UObject* const* New=Map.Find(Entry.ExpectedClass.GetEvenIfUnreachable()))Entry.ExpectedClass=Cast<UClass>(*New);
            if(UObject* const* New=Map.Find(Entry.Object.GetEvenIfUnreachable()))Entry.Object=*New;
            if(Entry.Object.IsValid() && (!Entry.ExpectedClass.IsValid() || !Entry.Object->IsA(Entry.ExpectedClass.Get())))Entry.Object.Reset();
        }
    });
#endif
    RuntimePreExitHandle=FCoreDelegates::OnPreExit.AddRaw(this,&FDasRuntimeModule::ShutdownModule);
    bInitialized = true;
    UE_LOG(LogDasRuntime, Display, TEXT("daScript initialized; content: %s"), *Root);
}

void FDasRuntimeModule::ShutdownModule()
{
    // Release rooted callback signatures while UObject storage is still valid.
    FCoreDelegates::OnPreExit.Remove(RuntimePreExitHandle);RuntimePreExitHandle.Reset();
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
        PreservedSessions.Empty();bPreservingSessions=false;
#endif
        ScriptTimers.Empty();PendingScriptTimers.Empty();
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

int32 FDasRuntimeModule::StructSnapshotCount(const UObject* Owner) const{int32 Count=0;for(const auto& S:Sessions)if(S->Live && (!Owner || S->Owner.Get()==Owner))Count+=S->Live->Context->StructValues.Num();return Count;}
uint64 FDasRuntimeModule::StructSnapshotsReclaimed(const UObject* Owner) const{uint64 Count=0;for(const auto& S:Sessions)if(S->Live && (!Owner || S->Owner.Get()==Owner))Count+=S->Live->Context->StructReclaimed;return Count;}

FString FDasRuntimeModule::ScriptPath(const FString& Name) const
{
    if(Name==FPaths::GetCleanFilename(Name)){
        const FString Project=FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir()/TEXT("Das/Scripts")/Name);
        if(IFileManager::Get().FileExists(*Project))return Project;
    }
    return Root / TEXT("Scripts") / Name;
}

bool FDasRuntimeModule::ReadAuthoredInterfaces(const FString& Path,FDasAuthoringModule& Out,FString& Error){
 Error.Reset();if(!IsInGameThread() || !bInitialized){Error=TEXT("Interface declaration compilation requires game-thread runtime");return false;}try{TGuardValue<bool> CollectOnly(CollectTypesOnly,true);das::TextWriter Text;das::ModuleGroup Group;auto Access=das::make_smart<FUnrealFileAccess>();Access->addFileSystem(new FUnrealFileSystem(),true,false);const FTCHARToUTF8 File(*Path);auto Program=DSLCompile(File.Get(),Access,Text,Group);if(!Program || Program->failed()){Error=Errors(Program,Text);return false;}FDasAuthoringModule Model;bool OK=true;Program->getThisModule()->structures.foreach([&](const das::StructurePtr& St){auto* A=DSLAnnotation(St->annotations,"ue_interface");if(!A || !OK)return;auto* J=A->arguments.find(DSLInterfaceKey,das::Type::tString);FDasAuthoringModule Part;if(!J || !FDasAuthoringModule::FromJson(DSLText(J->sValue),Part,Error)){OK=false;return;}if(!Model.Id.IsEmpty() && Model.Id!=Part.Id){Error=TEXT("Interface declarations must share module ID");OK=false;return;}Model.Id=Part.Id;Model.Classes.Append(Part.Classes);});if(!OK || !Model.Validate(Error))return false;Out=MoveTemp(Model);return true;}catch(const std::exception& X){Error=UTF8_TO_TCHAR(X.what());return false;}
}
bool FDasRuntimeModule::ReadReflectedTypes(const FString& Path,FDasReflectedTypes& Out,FString& Error,FString* SourceModule){
 Error.Reset();if(!IsInGameThread() || !bInitialized){Error=TEXT("Type declaration compilation requires initialized game-thread runtime");return false;}try{TGuardValue<bool> CollectOnly(CollectTypesOnly,true);das::TextWriter Text;das::ModuleGroup Group;auto Access=das::make_smart<FUnrealFileAccess>();Access->addFileSystem(new FUnrealFileSystem(),true,false);const FTCHARToUTF8 File(*Path);auto Program=DSLCompile(File.Get(),Access,Text,Group);if(!Program || Program->failed()){Error=Errors(Program,Text);return false;}FDasReflectedTypes Model;bool OK=true;auto Append=[&](const das::AnnotationList& List,const char* Name){const auto* A=DSLAnnotation(List,Name);if(!A || !OK)return;const auto* J=A->arguments.find(DSLTypesKey,das::Type::tString);FDasReflectedTypes Part;if(!J || !FDasReflectedTypes::FromJson(DSLText(J->sValue),Part,Error,true)){OK=false;return;}if(!Model.Id.IsEmpty() && Model.Id!=Part.Id){Error=TEXT("Reflected declarations must share module ID");OK=false;return;}Model.Id=Part.Id;Model.Enums.Append(Part.Enums);Model.Structs.Append(Part.Structs);};Program->getThisModule()->enumerations.foreach([&](const das::EnumerationPtr& E){Append(E->annotations,"ue_enum");});Program->getThisModule()->structures.foreach([&](const das::StructurePtr& S){Append(S->annotations,"ue_struct");});if(!OK || !Model.Validate(Error))return false;if(SourceModule){bool HasClasses=false;Program->getThisModule()->structures.foreach([&](const das::StructurePtr& S){HasClasses|=DSLAnnotation(S->annotations,"ue_class")!=nullptr || DSLAnnotation(S->annotations,"ue_interface")!=nullptr;});if(HasClasses){Error=TEXT("Catalog value imports require dedicated enum/struct modules without classes/interfaces");return false;}*SourceModule=DSLText(Program->getThisModule()->name);}Out=MoveTemp(Model);return true;}catch(const std::exception& X){Error=UTF8_TO_TCHAR(X.what());return false;}
}

namespace {
bool CollectDasCatalogModules(das::Program* Program,const FDasRuntimeModule& Runtime,FDasSourceCatalog& Out,FString& Error){
    bool OK=true;FDasSourceCatalog Discovered;
            Program->library.foreach([&](das::Module* Module){
                if(!OK)return false;FDasCatalogValueModule Values;Values.Module=DSLText(Module->name);bool HasClass=false;
                Module->structures.foreach([&](const das::StructurePtr& S){HasClass|=DSLAnnotation(S->annotations,"ue_class")!=nullptr;});
                auto Collect=[&](const das::AnnotationList& Annotations,const char* Kind,const das::LineInfo& At){
                    const auto* A=DSLAnnotation(Annotations,Kind);if(!A || !OK)return;const auto* Wire=A->arguments.find(DSLTypesKey,das::Type::tString);FDasReflectedTypes Part;
                    if(!Wire || !FDasReflectedTypes::FromJson(DSLText(Wire->sValue),Part,Error,true) || !At.fileInfo){if(Error.IsEmpty())Error=TEXT("Missing imported value type source/identity");OK=false;return;}
                    FString Source=FPaths::ConvertRelativePathToFull(DSLText(At.fileInfo->name));FPaths::NormalizeFilename(Source);FPaths::CollapseRelativeDirectories(Source);const FString File=FPaths::GetCleanFilename(Source);FString Expected=FPaths::ConvertRelativePathToFull(Runtime.ScriptPath(File));FPaths::NormalizeFilename(Expected);FPaths::CollapseRelativeDirectories(Expected);
                    if(!FDasSourceCatalog::LocalFile(File,TEXT("das")) || !Source.Equals(Expected,ESearchCase::IgnoreCase) || (!Values.Script.IsEmpty() && Values.Script!=File)){Error=TEXT("Imported authored values require one local Scripts source per module: ")+Source;OK=false;return;}Values.Script=File;
                    for(const auto& E:Part.Enums){FDasCatalogValueType T;T.Kind=TEXT("enum");T.Id=E.Id;T.Name=E.Name;Values.Types.Add(MoveTemp(T));}for(const auto& E:Part.Structs){FDasCatalogValueType T;T.Kind=TEXT("struct");T.Id=E.Id;T.Name=E.Name;Values.Types.Add(MoveTemp(T));}
                };
                Module->enumerations.foreach([&](const das::EnumerationPtr& E){Collect(E->annotations,"ue_enum",E->at);});Module->structures.foreach([&](const das::StructurePtr& S){Collect(S->annotations,"ue_struct",S->at);});
                auto SourceFor=[&](const das::LineInfo& At){if(!At.fileInfo){Error=TEXT("Missing catalog interface source");OK=false;return false;}FString Source=FPaths::ConvertRelativePathToFull(DSLText(At.fileInfo->name));FPaths::NormalizeFilename(Source);FPaths::CollapseRelativeDirectories(Source);const FString File=FPaths::GetCleanFilename(Source);FString Expected=FPaths::ConvertRelativePathToFull(Runtime.ScriptPath(File));FPaths::NormalizeFilename(Expected);FPaths::CollapseRelativeDirectories(Expected);if(!FDasSourceCatalog::LocalFile(File,TEXT("das")) || !Source.Equals(Expected,ESearchCase::IgnoreCase) || (!Values.Script.IsEmpty() && Values.Script!=File)){Error=TEXT("Catalog interface modules require a local Scripts source: ")+Source;OK=false;return false;}Values.Script=File;return true;};
                Module->structures.foreach([&](const das::StructurePtr& S){
                    if(!OK)return;
                    if(const auto* A=DSLAnnotation(S->annotations,"ue_interface")){const auto* Wire=A->arguments.find(DSLInterfaceKey,das::Type::tString);FDasAuthoringModule Model;if(!Wire || !FDasAuthoringModule::FromJson(DSLText(Wire->sValue),Model,Error) || !SourceFor(S->at)){OK=false;return;}for(const auto& C:Model.Classes){FDasCatalogValueType T;T.Kind=TEXT("interface");T.Id=C.Id;T.Name=C.Name;Values.Types.Add(MoveTemp(T));}}
                    if(const auto* A=DSLAnnotation(S->annotations,"ue_interface_ref")){if(!SourceFor(S->at))return;FDasCatalogValueType T;T.Kind=TEXT("interface_ref");T.Id=DSLString(A->arguments,"id");T.Name=DSLText(S->name);Values.Types.Add(MoveTemp(T));}
                });
                if(!Values.Types.IsEmpty()){if(HasClass){Error=TEXT("Automatic catalog imports require dedicated type/interface modules without ue_class: ")+Values.Module;OK=false;return false;}Discovered.ValueModules.Add(MoveTemp(Values));}return OK;
            },"*");
            if(!OK)return false;Discovered.ValueModules.Sort([](const auto& A,const auto& B){return A.Module<B.Module;});
    Out=MoveTemp(Discovered);return true;
}
}

bool FDasRuntimeModule::ReadAuthoringScript(const FString& Path,FDasAuthoringModule& Out,FString& Error,FDasSourceCatalog* ImportedValues)
{
    Error.Reset();if(!IsInGameThread() || !bInitialized){Error=TEXT("DSL compilation requires initialized runtime on game thread");return false;}
    try
    {
        das::TextWriter Output;das::ModuleGroup Group;auto Access=das::make_smart<FUnrealFileAccess>();Access->addFileSystem(new FUnrealFileSystem(),true,false);
        const FTCHARToUTF8 File(*Path);auto Program=DSLCompile(File.Get(),Access,Output,Group);
        if(!Program || Program->failed()){Error=Errors(Program,Output);return false;}
        FDasAuthoringModule Candidate;bool OK=true;
        Program->getThisModule()->structures.foreach([&](const das::StructurePtr& St)
        {
            const auto* A=DSLAnnotation(St->annotations,"ue_class");if(!A)A=DSLAnnotation(St->annotations,"ue_library");if(!A || !OK) return;
            const auto* Wire=A->arguments.find(DSLModelKey,das::Type::tString);FDasAuthoringModule Part;
            if(!Wire || !FDasAuthoringModule::FromJson(DSLText(Wire->sValue),Part,Error)){OK=false;return;}
            if(!Candidate.Id.IsEmpty() && Candidate.Id!=Part.Id){Error=TEXT("Declarations in a file must share module ID");OK=false;return;}
            Candidate.Id=Part.Id;for(const auto& C:Part.Classes){if(!Candidate.Classes.ContainsByPredicate([&](const auto& P){return P.Id==C.Id;}))Candidate.Classes.Add(C);}
        });
        if(!OK || !Candidate.Validate(Error)) return false;
        FDasSourceCatalog Discovered;if(ImportedValues && !CollectDasCatalogModules(Program.get(),*this,Discovered,Error))return false;
        if(ImportedValues)*ImportedValues=MoveTemp(Discovered);Out=MoveTemp(Candidate);return true;
    }
    catch(const std::exception& E){Error=UTF8_TO_TCHAR(E.what());return false;}
}

bool FDasRuntimeModule::ReadSourceCatalogImports(const FString& Path,FDasSourceCatalog& Out,FString& Error){
    Error.Reset();if(!IsInGameThread() || !bInitialized){Error=TEXT("Catalog import discovery requires initialized game-thread runtime");return false;}
    try{das::TextWriter Text;das::ModuleGroup Group;auto Access=das::make_smart<FUnrealFileAccess>();Access->addFileSystem(new FUnrealFileSystem(),true,false);const FTCHARToUTF8 File(*Path);auto Program=DSLCompile(File.Get(),Access,Text,Group);if(!Program || Program->failed()){Error=Errors(Program,Text);return false;}FDasSourceCatalog Candidate;if(!CollectDasCatalogModules(Program.get(),*this,Candidate,Error))return false;if(Candidate.ValueModules.IsEmpty()){Error=TEXT("No authored enum/struct/interface imports found");return false;}Out=MoveTemp(Candidate);return true;}catch(const std::exception& E){Error=UTF8_TO_TCHAR(E.what());return false;}
}

bool FDasRuntimeModule::ValidateIntMethod(const FString& Path,FString& Error,const FString& EntryPoint)
{return ValidateScalarMethod(Path,Error,EntryPoint,EDasMethodSignature::Int);}
bool FDasRuntimeModule::ValidateScalarMethod(const FString& Path,FString& Error,const FString& EntryPoint,EDasMethodSignature Signature)
{return ValidateFrameMethod(Path,Error,EntryPoint,FDasCallContract::Scalar(Signature));}
bool FDasRuntimeModule::ValidateFrameMethod(const FString& Path,FString& Error,const FString& EntryPoint,const FDasCallContract& Contract)
{
    Error.Reset();
    if (!IsInGameThread() || !bInitialized) { Error=TEXT("Method preflight requires initialized runtime on game thread");return false; }
    try
    {
        das::TextWriter Output;das::ModuleGroup Modules;
        auto Access=das::make_smart<FUnrealFileAccess>();Access->addFileSystem(new FUnrealFileSystem(),true,false);
        const FTCHARToUTF8 Utf8Path(*Path);das::CodeOfPolicies Policies;
        auto Program=DSLCompile(Utf8Path.Get(),Access,Output,Modules,Policies);
        if (!Program || Program->failed()) { Error=Errors(Program,Output);return false; }
        if(!VerifyContract(Program,EntryPoint,Contract)){Error=TEXT("Expected exported scalar frame signature by value: ")+EntryPoint;return false;}
        // No simulate/eval: runtime global initializers and the method body are not executed during preflight.
        return true;
    }
    catch (const std::exception& Exception) { Error=UTF8_TO_TCHAR(Exception.what());return false; }
}

FDasExecutionResult FDasRuntimeModule::RunFile(const FString& Path, int32 Input, bool bUseAot, TWeakObjectPtr<UObject> Self)
{
    TGuardValue<const FString*> LibraryContractScope(LibraryInvocationContract,FPaths::GetCleanFilename(Path)==LibraryInvocationScript?LibraryInvocationContract:nullptr);
    FDasExecutionResult Result;
    if(GetDasExecutionRole()==EDasExecutionRole::PhysicsPreSim || IsDasFrozenPhysicsWorld(Self.Get())){Result.Error=TEXT("gameplay VM entry is forbidden during PhysicsPreSim");return Result;}
    if (!IsInGameThread() || !bInitialized)
    {
        Result.Error = TEXT("daScript calls require an initialized runtime on the game thread.");
        return Result;
    }
#if WITH_EDITOR
    if(bPreservingSessions){Result.Error=TEXT("Runtime callbacks are suspended during class migration");return Result;}
#endif
    FDrainEventsOnExit DrainOnExit;
    try
    {
        das::TextWriter Output;
        das::ModuleGroup Modules;
        auto Access = das::make_smart<FUnrealFileAccess>();
        Access->addFileSystem(new FUnrealFileSystem(), true, false);
        const FTCHARToUTF8 Utf8Path(*Path);
        das::CodeOfPolicies Policies;
        Policies.aot = bUseAot;
        auto Program = DSLCompile(Utf8Path.Get(), Access, Output, Modules, Policies);
        if (!Program || Program->failed())
        {
            Result.Error = Errors(Program, Output);
            return Result;
        }
        FUnrealDasContext Context(Program->getContextStackSize());
        Context.Self = Self;
        FActiveContextScope ActiveScope(Context);
        if (!Program->simulate(Context, Output))
        {
            Result.Error = Errors(Program, Output);
            return Result;
        }
        auto Function = Context.findFunction("main");
        if (!Function || !das::verifyCall<int32, int32>(Function->debugInfo, Modules))
        {
            Result.Error = TEXT("Expected exported main(input: int): int.");
            return Result;
        }
        vec4f Arguments[] = { das::cast<int32>::from(Input) };
        const vec4f Value = Context.evalWithCatch(Function,Arguments);
        if (const char* Exception = Context.getException())
        {
            Result.Error = UTF8_TO_TCHAR(Exception);
            return Result;
        }
        Result.bUsedAot = Function->aot;
        Result.Value = das::cast<int32>::to(Value);
        Result.Types = MoveTemp(Context.Types);
        Result.bSucceeded = true;
        return Result;
    }
    catch (const std::exception& Exception)
    {
        Result.Error = UTF8_TO_TCHAR(Exception.what());
        return Result;
    }
}

void FDasRuntimeModule::DispatchIntEvent(uint64 SubscriptionId,int32 Value){FDasScalarValue V;V.Kind=0;V.IntValue=Value;DispatchScalarEvent(SubscriptionId,MoveTemp(V));}
namespace {
template<class ValueType>
void DispatchScalarEventImpl(uint64 SubscriptionId,ValueType&& Value,bool Initialized)
{
    if (!IsInGameThread() || !Initialized) return;
#if WITH_EDITOR
    if(bPreservingSessions)return; // Old proxy IDs are detached and must never enter a VM during migration.
#endif
    FString Issue;if(!ValidateDasCallbackPayload(Value,EDasCallbackUsage::Event,Issue))return;
    if(PendingEvents.Num()<1024)PendingEvents.Add({SubscriptionId,MakeShared<FDasCallbackPayload>(Forward<ValueType>(Value))});
    DrainEvents();
}
}
void FDasRuntimeModule::DispatchScalarEvent(uint64 SubscriptionId,const FDasScalarValue& Value){if(IsInGameThread())DispatchScalarEventImpl(SubscriptionId,Value,bInitialized);}
void FDasRuntimeModule::DispatchScalarEvent(uint64 SubscriptionId,FDasScalarValue&& Value){if(IsInGameThread())DispatchScalarEventImpl(SubscriptionId,MoveTemp(Value),bInitialized);}

int32 FDasRuntimeModule::ScriptTimerCount(const UObject* Owner) const {check(IsInGameThread());int32 Count=0;for(const auto& Timer:ScriptTimers)if(!Owner || Timer.Value->Owner.Get()==Owner)++Count;return Count;}

int32 FDasRuntimeModule::SubscriptionCount(const UObject* Owner) const
{
    check(IsInGameThread());
    int32 Count = 0;
    for (const auto& Session : Sessions)
        if (!Owner || Session->Owner.Get() == Owner) Count += Session->Bindings.Num();
    return Count;
}

void FDasRuntimeModule::ReleaseObject(UObject* Object)
{
    check(IsInGameThread());
    UDasAsyncPhysicsBridge::RevokeOwner(Object);
    for (const auto& Session : Sessions)
        if (Session->Owner.Get(true) == Object) RevokeSession(*Session);
    FlushReleasedSessions();
}

bool FDasRuntimeModule::IsAuthoredMethodDispatchActive()const{return IsInGameThread() && AuthoredDispatchDepth>0;}

bool FDasRuntimeModule::CanMigrateAuthoredTypes()const {
#if WITH_EDITOR
 return IsInGameThread() && bInitialized && !ActiveContext && !bDrainingEvents && !bPreservingSessions && PendingEvents.IsEmpty() && PendingScriptTimers.IsEmpty() && ScriptTimers.IsEmpty() && !bDrainingScriptTimers && PersistentContextCount()==0 && SubscriptionCount()==0;
#else
 return false;
#endif
}
int32 FDasRuntimeModule::PersistentContextCount(const UWorld* World) const
{
    check(IsInGameThread());
    int32 Count = 0;
    for (const auto& Session : Sessions)
        if (Session->Live && Session->Owner.IsValid() && (!World || Session->World.Get() == World)) ++Count;
    return Count;
}

FDasExecutionResult FDasRuntimeModule::DispatchInputEventSnapshots(const FString& Path,const FString& Entry,const TArray<FInstancedStruct>& Values,TWeakObjectPtr<UObject> Self){
 FDasExecutionResult Rejected;if(Values.IsEmpty() || Values.Num()>16){Rejected.Error=TEXT("input snapshot dispatch requires 1..16 detached values");return Rejected;}
 TArray<FDasScalarValue> Args;for(const auto& V:Values){FString Error;if(!ValidateDasInputEventSnapshot(V,Error)){Rejected.Error=Error;return Rejected;}FDasScalarValue A;A.Kind=12;A.ObjectClass=V.GetScriptStruct()->GetPathName();A.StructValue=V;Args.Add(MoveTemp(A));}
 TGuardValue<bool> Scope(InputSnapshotDispatch,true);return RunNamedFramePersistent(Path,Entry,Args,EDasMethodSignature::Int,Self);
}

FDasExecutionResult FDasRuntimeModule::RunNamedFramePersistent(const FString& Path,const FString& Entry,const TArray<FDasScalarValue>& Args,EDasMethodSignature ResultKind,TWeakObjectPtr<UObject> Self,const FString& ReturnClass)
{return RunCallback(Path,EDasCallback::Main,0,0.f,Self,Entry,ResultKind,&Args,ReturnClass);}
FDasExecutionResult FDasRuntimeModule::RunNamedFloatPersistent(const FString& Path,const FString& Entry,float Input,TWeakObjectPtr<UObject> Self)
{return RunCallback(Path,EDasCallback::Main,0,Input,Self,Entry,EDasMethodSignature::Float);}
FDasExecutionResult FDasRuntimeModule::RunNamedBoolPersistent(const FString& Path,const FString& Entry,bool Input,TWeakObjectPtr<UObject> Self)
{return RunCallback(Path,EDasCallback::Main,Input?1:0,0.f,Self,Entry,EDasMethodSignature::Bool);}
FDasExecutionResult FDasRuntimeModule::RunNamedVoidPersistent(const FString& Path,const FString& Entry,TWeakObjectPtr<UObject> Self)
{return RunCallback(Path,EDasCallback::Main,0,0.f,Self,Entry,EDasMethodSignature::Void);}

FDasExecutionResult FDasRuntimeModule::RunNamedPersistent(const FString& Path, const FString& EntryPoint, int32 Input, TWeakObjectPtr<UObject> Self)
{
    if (EntryPoint.IsEmpty() || EntryPoint.Len()>=NAME_SIZE || FChar::IsDigit(EntryPoint[0]))
    { FDasExecutionResult Result;Result.Error=TEXT("Invalid entry point name");return Result; }
    for (TCHAR Ch:EntryPoint) if (!FChar::IsAlnum(Ch) && Ch!='_')
    { FDasExecutionResult Result;Result.Error=TEXT("Invalid entry point name");return Result; }
    return RunCallback(Path,EDasCallback::Main,Input,0.f,Self,EntryPoint);
}

FDasExecutionResult FDasRuntimeModule::RunPersistent(const FString& Path, int32 Input, TWeakObjectPtr<UObject> Self)
{
    return RunCallback(Path, EDasCallback::Main, Input, 0.f, Self);
}

FDasExecutionResult FDasRuntimeModule::RunConstruction(const FString& Path,TWeakObjectPtr<UObject> Self)
{
    FDasExecutionResult Result;
    auto* Object=Self.Get();auto* Component=Cast<UDasScriptComponent>(Object);
    auto* Actor=Component?Component->GetOwner():Cast<AActor>(Object);
    const bool Begun=Component?Component->HasBegunPlay():(Actor && Actor->HasActorBegunPlay());
    if(!IsInGameThread() || !bInitialized || ConstructingObject || !IsValid(Object) || Object->IsTemplate() || !IsValid(Actor) || Actor->IsTemplate() || !Actor->GetWorld() || Actor->IsActorBeingDestroyed() || (Component && Component->IsBeingDestroyed()) || Begun) {
        Result.Error=TEXT("ConstructionScript requires a live Actor or ActorComponent before its BeginPlay and cannot recurse");return Result;
    }
#if WITH_EDITOR
    if(bPreservingSessions){Result.bSucceeded=true;return Result;}
#endif
    // Construction contexts are temporary: never reset or reuse gameplay VMs.
    auto SavedSessions=MoveTemp(Sessions);
    ON_SCOPE_EXIT { Sessions.Empty(); Sessions=MoveTemp(SavedSessions); };
    TGuardValue<bool> ConstructionScope(ConstructingObject,true);
#if WITH_EDITOR
    // Actor ProcessEvent otherwise skips Blueprint method graphs in editor worlds.
    FEditorScriptExecutionGuard ScriptExecution;
#endif
    return RunCallback(Path,EDasCallback::Construction,0,0.f,Self);
}

FDasExecutionResult FDasRuntimeModule::RunLifecycle(const FString& Path, EDasCallback Event, TWeakObjectPtr<UObject> Self, float DeltaSeconds, int32 EndReason)
{
    if(Event==EDasCallback::EndPlay && IsInGameThread() && GetDasExecutionRole()==EDasExecutionRole::Gameplay && !IsDasFrozenPhysicsWorld(Self.Get()))UDasAsyncPhysicsBridge::RevokeOwner(Self.Get());
    if (Event == EDasCallback::Main)
    {
        FDasExecutionResult Result;
        Result.Error = TEXT("Use RunPersistent for main.");
        return Result;
    }
    return RunCallback(Path, Event, EndReason, DeltaSeconds, Self);
}

FDasExecutionResult FDasRuntimeModule::RunCallback(const FString& Path, EDasCallback Event, int32 Input, float DeltaSeconds, TWeakObjectPtr<UObject> Self, const FString& EntryPoint, EDasMethodSignature Signature,const TArray<FDasScalarValue>* Frame,const FString& ReturnClass)
{
    TGuardValue<const FString*> LibraryContractScope(LibraryInvocationContract,FPaths::GetCleanFilename(Path)==LibraryInvocationScript?LibraryInvocationContract:nullptr);
    FDasExecutionResult Result;
    if(GetDasExecutionRole()==EDasExecutionRole::PhysicsPreSim || IsDasFrozenPhysicsWorld(Self.Get())){Result.Error=TEXT("gameplay VM entry is forbidden during PhysicsPreSim");return Result;}
    ON_SCOPE_EXIT{if(AuthoredDispatchFailure && !Result.bSucceeded)*AuthoredDispatchFailure=Result.Error;};
    if(Event==EDasCallback::Main)
    {
        bool Valid=!EntryPoint.IsEmpty() && EntryPoint.Len()<NAME_SIZE && !FChar::IsDigit(EntryPoint[0]);
        for(TCHAR Ch:EntryPoint) Valid &= FChar::IsAlnum(Ch) || Ch==TEXT('_');
        if(!Valid){Result.Error=TEXT("Invalid entry point name");return Result;}
    }
    const auto* LifecycleComponent=Cast<UDasScriptComponent>(Self.Get());
    const bool ActorLifecycle=ActorLifecycleTarget && (Self.Get()==ActorLifecycleTarget || (LifecycleComponent && LifecycleComponent->GetOwner()==ActorLifecycleTarget)) && (Event==EDasCallback::BeginPlay || Event==EDasCallback::EndPlay || Event==EDasCallback::Construction);
    const bool ComponentLifecycle=ComponentLifecycleTarget && Self.Get()==ComponentLifecycleTarget && (Event==EDasCallback::BeginPlay || Event==EDasCallback::EndPlay || Event==EDasCallback::Construction);
    const bool ParentLifecycle=ParentLifecycleTarget && Self.Get()==ParentLifecycleTarget && Event!=EDasCallback::Main;
    if (!IsInGameThread() || !bInitialized || (ActiveContext && !ActorLifecycle && !ComponentLifecycle && !ParentLifecycle && (AuthoredDispatchDepth==0 || Event!=EDasCallback::Main)))
    {
        Result.Error = TEXT("Persistent calls require an initialized runtime and authorized method dispatch on the game thread.");
        return Result;
    }
#if WITH_EDITOR
    if(bPreservingSessions){
        // Native teardown/reconstruction is not a gameplay lifecycle transition.
        Result.bSucceeded=Event!=EDasCallback::Main;
        if(!Result.bSucceeded)Result.Error=TEXT("Runtime callbacks are suspended during class migration");
        return Result;
    }
#endif
    if(Event==EDasCallback::Construction && !ConstructingObject){Result.Error=TEXT("Use RunConstruction for ConstructionScript");return Result;}
    // Nested authored UFunction calls share this construction's temporary session set.
    // They may evaluate methods, but must never deliver gameplay lifecycle callbacks.
    const bool ConstructionMethod=ConstructingObject && ActiveContext && AuthoredDispatchDepth>0 && Event==EDasCallback::Main;
    if(ConstructingObject && Event!=EDasCallback::Construction && !ConstructionMethod){Result.Error=TEXT("Gameplay callbacks are forbidden during ConstructionScript");return Result;}
    TArray<FDasScalarValue> NormalizedFrame,PendingOutputs;
    if(Frame){NormalizedFrame=*Frame;for(auto& V:NormalizedFrame){if(V.Direction<0 || V.Direction>2){Result.Error=TEXT("Invalid frame direction");return Result;}if(V.Direction==1)V=UDasFrameLibrary::DefaultValue(V.Kind,V.ObjectClass,1);}Frame=&NormalizedFrame;}
    FStructFrameRoots FrameRoots(NormalizedFrame);
    FDasCallContract Expected=FDasCallContract::Scalar(Signature);
    if(Frame){Expected.Inputs.Reset();if(Frame->Num()>16 || ((static_cast<uint8>(Signature)>13 && Signature!=EDasMethodSignature::Class && Signature!=EDasMethodSignature::Delegate && Signature!=EDasMethodSignature::Byte && Signature!=EDasMethodSignature::Text) || Signature==EDasMethodSignature::Frame)){Result.Error=TEXT("Invalid scalar frame");return Result;}for(const auto& V:*Frame){if(V.Kind<0 || (V.Kind>13 && V.Kind!=15 && V.Kind!=16 && V.Kind!=17 && V.Kind!=18) || V.Kind==3 || V.Kind==4){Result.Error=TEXT("Invalid scalar argument kind");return Result;}if(V.Kind==5)for(int32 I=0;I<V.StringValue.Len();++I)if(V.StringValue[I]==0){Result.Error=TEXT("embedded NUL is not supported in strings");return Result;}if((V.Kind==9 && V.VectorValue.ContainsNaN()) || (V.Kind==10 && V.RotatorValue.ContainsNaN()) || (V.Kind==11 && (V.TransformValue.ContainsNaN() || !V.TransformValue.IsRotationNormalized()))){Result.Error=TEXT("Invalid/non-finite math struct input");return Result;}Expected.Inputs.Add(static_cast<EDasMethodSignature>(V.Kind));}}
    TArray<TStrongObjectPtr<UObject>> ObjectPins;
    if(Frame){bool Directed=false;for(const auto& V:*Frame)Directed |= V.Direction!=0;if(Directed)for(const auto& V:*Frame)Expected.InputDirections.Add(V.Direction);}
    if(Frame){bool Objects=false;for(const auto& V:*Frame)Objects |= V.Kind==15 || V.Kind==8 || V.Kind==12 || V.Kind==13 || V.Kind==16;
        if(Objects)for(const auto& V:*Frame){const FString PathClass=(V.Kind==12 || V.Kind==13 || V.Kind==15 || V.Kind==16)?V.ObjectClass:V.Kind==8?(V.ObjectClass.IsEmpty()?TEXT("/Script/CoreUObject.Object"):V.ObjectClass):FString();Expected.InputClasses.Add(PathClass);if(V.Kind==15){if(!FrameClassMatches(V.ClassValue.Get(),PathClass)){Result.Error=TEXT("Class argument has wrong base or is obsolete");return Result;}if(V.ClassValue)ObjectPins.Emplace(V.ClassValue.Get());}if(V.Kind==8){UClass* Class=FindObject<UClass>(nullptr,*PathClass);UObject* Object=V.ObjectValue.Get();if(!Class || (Object && (!IsValid(Object) || !DasReferenceMatches(Object,Class) || (Object->IsA<AActor>() && CastChecked<AActor>(Object)->IsActorBeingDestroyed())))){Result.Error=TEXT("Object argument is expired or has wrong class");return Result;}if(Object)ObjectPins.Emplace(Object);}}
    }
    if(Frame)for(const auto& V:*Frame)if(V.Kind==12){FString E;if(!V.StructValue.IsValid() || V.StructValue.GetScriptStruct()->GetPathName()!=V.ObjectClass || !(DasIsInputEventType(V.StructValue.GetScriptStruct())?ValidateDasInputEventSnapshot(V.StructValue,E):ValidateDasStructSnapshot(V.StructValue,E))){Result.Error=TEXT("Invalid reflected struct input: ")+E;return Result;}}
    if(Signature==EDasMethodSignature::Delegate){Expected.ReturnClass=ReturnClass;FString E;TStrongObjectPtr<UFunction> Sig(MakeDasSingleDelegateSignature(ReturnClass,E));if(!Sig){Result.Error=E;return Result;}}
    if(Signature==EDasMethodSignature::Enum){Expected.ReturnClass=ReturnClass;FString E;if(!ResolveDasEnum(ReturnClass,E)){Result.Error=E;return Result;}}
    if(Frame)for(const auto& V:*Frame)if(V.Kind==13){FString E;if(!ValidateDasEnumNumber(V.ObjectClass,V.IntValue,E)){Result.Error=E;return Result;}}
    if(Signature==EDasMethodSignature::Struct){DasContainerProperty(ReturnClass);Expected.ReturnClass=ReturnClass;FString E;if(!ValidateDasValueStruct(ReturnClass,E)){Result.Error=E;return Result;}}
    if(Signature==EDasMethodSignature::Class){Expected.ReturnClass=ReturnClass;if(!FrameClassMatches(nullptr,ReturnClass)){Result.Error=TEXT("Class result requires a loaded native base");return Result;}}
    if(Signature==EDasMethodSignature::Object){Expected.ReturnClass=ReturnClass.IsEmpty()?TEXT("/Script/CoreUObject.Object"):ReturnClass;if(!FindObject<UClass>(nullptr,*Expected.ReturnClass)){Result.Error=TEXT("Object result class is not loaded");return Result;}}
    FDrainEventsOnExit DrainOnExit;
    UObject* Owner = Self.Get();
    if (!IsValid(Owner) || (Owner->IsA<AActor>() && CastChecked<AActor>(Owner)->IsActorBeingDestroyed() && Event != EDasCallback::EndPlay))
    {
        Result.Error = TEXT("self is missing or expired");
        return Result;
    }
    PruneSessions();
    FString FullPath = FPaths::ConvertRelativePathToFull(Path);
    FPaths::NormalizeFilename(FullPath);
    FPaths::CollapseRelativeDirectories(FullPath);
    if(Event==EDasCallback::Initialize && (!Owner->GetWorld() || !Owner->GetWorld()->IsGameWorld() || Owner->IsTemplate())){Result.Error=TEXT("Initialize requires a live game-world instance");return Result;}
    if(Event!=EDasCallback::EndPlay)if(const auto* Deliveries=InitializerDeliveries.Find(Self))for(const auto& Delivery:*Deliveries)if(Delivery.Value.State==EInitializerDelivery::Failed){Result.Error=Delivery.Value.Error;return Result;}
    FPersistentSession* Session = nullptr;
    for (const auto& Existing : Sessions)
        if (Existing->Owner == Self && Existing->Path == FullPath) { Session = Existing.Get(); break; }
    if (!Session && Event == EDasCallback::EndPlay)
    {
        Result.bSucceeded = true; // Never start a script during teardown.
        return Result;
    }
    if (!Session)
    {
        auto NewSession = MakeUnique<FPersistentSession>();
        NewSession->Owner = Self;
        NewSession->World = Owner->GetWorld();
        NewSession->Path = FullPath;
        Session = NewSession.Get();
        Sessions.Add(MoveTemp(NewSession));
    }
    if (Session->bEndDelivered)
    {
        Result.bSucceeded = Event == EDasCallback::EndPlay;
        if (!Result.bSucceeded) Result.Error = TEXT("Script lifecycle has already ended.");
        Result.Revision = Session->Revision;
        return Result;
    }
    bool bChanged = Session->AttemptedSources.IsEmpty() || (!Session->Live && (Session->AttemptedEntry!=EntryPoint || Session->AttemptedContract!=Expected));
    for (const auto& Source : Session->AttemptedSources)
        if (!Source.Value.Matches(Source.Key)) { bChanged = true; break; }
    if(Session->bReleaseRequested){Result.Error=TEXT("Script session was released");return Result;}
    const bool Reentering=Session->Live && Session->Live->Context->ExecutionDepth>0;
    if(Reentering && bChanged){Result.Error=TEXT("Cannot reload an executing script during reentrant dispatch");return Result;}
    if (bChanged && Event != EDasCallback::EndPlay)
    {
        // Rejected sources are remembered too: no compile storm for an unchanged broken file.
        Session->AttemptedEntry=EntryPoint;Session->AttemptedContract=Expected;
        Session->AttemptedSources.Reset();
        Session->AttemptedSources.Add(FullPath, FSourceSnapshot::Read(FullPath));
        FString CandidateError;
        auto Candidate = MakeUnique<FCompiledContext>();
        try
        {
            das::TextWriter Output;
            auto Access = das::make_smart<FUnrealFileAccess>();
            Access->addFileSystem(new FUnrealFileSystem(&Session->AttemptedSources), true, false);
            Candidate->Access = Access;
            const FTCHARToUTF8 Utf8Path(*FullPath);
            {TGuardValue<FSourceSet*> ReferenceDependencies(AuthoredReferenceSources,&Session->AttemptedSources);Candidate->Program = DSLCompile(Utf8Path.Get(), Access, Output, Candidate->Modules);}
            if (!Candidate->Program || Candidate->Program->failed()) CandidateError = Errors(Candidate->Program, Output);
            else if(!DSLValidateOwner(Candidate->Program,EntryPoint,Self.Get(),Candidate->DSLDeclaration,CandidateError)) {}
            else if(Session->Live && Session->Live->DSLDeclaration!=Candidate->DSLDeclaration && (Session->ApprovedDSLDeclaration.IsEmpty() || Session->ApprovedDSLDeclaration!=Candidate->DSLDeclaration) && !([&]{
                if(!Session->bDefaultsUpdateApproved)return false;
                FDasAuthoringModule Old,Next;FString Issue;
                return FDasAuthoringModule::FromJson(Session->Live->DSLDeclaration,Old,Issue) && FDasAuthoringModule::FromJson(Candidate->DSLDeclaration,Next,Issue) && FDasAuthoringModule::DefaultsOnly(Old,Next,Issue);
            })())
                CandidateError=TEXT("DSL declaration changed; apply/reimport the Blueprint before runtime reload");
            else
            {
                Candidate->Context = MakeUnique<FUnrealDasContext>(Candidate->Program->getContextStackSize());
                FActiveContextScope Scope(*Candidate->Context);
                // No self during initialization: rejected code cannot alter the live Actor.
                if (!Candidate->Program->simulate(*Candidate->Context, Output))
                {
                    CandidateError = Errors(Candidate->Program, Output);
                    if (CandidateError.IsEmpty()) CandidateError = TEXT("Candidate initialization failed.");
                }
                else
                {
                    Candidate->Function = Candidate->Context->findFunction("main");
                    Candidate->Initializer = Candidate->Context->findFunction("__ue_initialize");
                    Candidate->Construction = Candidate->Context->findFunction("__ue_construct");
                    Candidate->BeginPlay = Candidate->Context->findFunction("begin_play");
                    Candidate->Tick = Candidate->Context->findFunction("tick");
                    Candidate->EndPlay = Candidate->Context->findFunction("end_play");
                    const FTCHARToUTF8 EntryUtf8(*EntryPoint);
                    auto* Selected=Event==EDasCallback::Main?Candidate->Context->findFunction(EntryUtf8.Get()):Candidate->Function;
                    if ((Event==EDasCallback::Main && !Selected) || (Selected && !VerifyContract(Candidate->Program,Event==EDasCallback::Main?EntryPoint:TEXT("main"),Event==EDasCallback::Main?Expected:FDasCallContract::Scalar(EDasMethodSignature::Int))))
                        CandidateError=TEXT("Expected exported ")+EntryPoint+TEXT(" with requested scalar signature.");
                    else if (Candidate->Construction && !das::verifyCall<void>(Candidate->Construction->debugInfo,Candidate->Modules))
                        CandidateError=TEXT("Expected exported __ue_construct(): void.");
                    else if (Candidate->Initializer && !das::verifyCall<void>(Candidate->Initializer->debugInfo,Candidate->Modules))
                        CandidateError=TEXT("Expected exported __ue_initialize(): void.");
                    else if (Candidate->BeginPlay && !das::verifyCall<void>(Candidate->BeginPlay->debugInfo, Candidate->Modules))
                        CandidateError = TEXT("Expected exported begin_play(): void.");
                    else if (Candidate->Tick && !das::verifyCall<void, float>(Candidate->Tick->debugInfo, Candidate->Modules))
                        CandidateError = TEXT("Expected exported tick(delta_time: float): void.");
                    else if (Candidate->EndPlay && !das::verifyCall<void, int32>(Candidate->EndPlay->debugInfo, Candidate->Modules))
                        CandidateError = TEXT("Expected exported end_play(reason: int): void.");
                    if (CandidateError.IsEmpty())
                        for (const auto& Required:Session->RequiredEntries)
                        {
                            TGuardValue<bool> SnapshotContractScope(InputSnapshotDispatch,InputSnapshotDispatch || Session->InputSnapshotEntries.Contains(Required.Key));
                            const FTCHARToUTF8 RequiredUtf8(*Required.Key);
                            auto* RequiredFunction=Candidate->Context->findFunction(RequiredUtf8.Get());
                            if (!RequiredFunction || !VerifyContract(Candidate->Program,Required.Key,Required.Value,Session->NativeInputEntries.Contains(Required.Key)))
                            {CandidateError=TEXT("Previously used entry point signature changed: ")+Required.Key;break;}
                        }
                }
                if (CandidateError.IsEmpty())
                {
                    if (Session->Live) Candidate->Context->PersistentInts = Session->Live->Context->PersistentInts;
                    Candidate->Context->Self = Self;
                    auto* BindEvents = Candidate->Context->findFunction("bind_events");
                    if (BindEvents && !ConstructingObject)
                    {
                        if (!das::verifyCall<void>(BindEvents->debugInfo, Candidate->Modules))
                            CandidateError = TEXT("Expected exported bind_events(): void.");
                        else
                        {
                            TGuardValue<bool> Collect(Candidate->Context->bCollectingSubscriptions, true);
                            Candidate->Context->evalWithCatch(BindEvents, nullptr);
                        }
                    }
                    for (const auto& Declaration : Candidate->Context->Subscriptions)
                    {
                        const FTCHARToUTF8 Name(*Declaration.Handler);
                        auto* Handler = Candidate->Context->findFunction(Name.Get());
                        if (!VerifyEventHandler(Handler,Candidate->Modules,Declaration.Kind))
                            CandidateError = (Declaration.Kind==EDasMethodSignature::Int?TEXT("Expected exported event handler(value: int): void: "):TEXT("Expected event handler matching declared scalar signature: ")) + Declaration.Handler;
                    }
                }
                if (const char* Exception = Candidate->Context->getException()) CandidateError = UTF8_TO_TCHAR(Exception);
            }
        }
        catch (const std::exception& Exception) { CandidateError = UTF8_TO_TCHAR(Exception.what()); }
        if(CandidateError.IsEmpty() && Candidate->Context && Candidate->Program && !Candidate->Program->failed())CompileDSLPhysics(*Candidate,Session->Live.Get(),FullPath,Owner,Session->AttemptedSources,ParentLifecycle,Session->bBeginDelivered && DSLPhysicsOwnerReady(Owner),CandidateError);
        if (CandidateError.IsEmpty() && Candidate->Context && Candidate->Program && !Candidate->Program->failed())
        {
            ClearSessionTimers(*Session);
            Session->Bindings.Empty();Session->InputContexts.Empty(); // Revoke old IDs before destroying any old VM state.
            Candidate->Context->Self = Self;
            const uint64 NewPhysicsRevision=Candidate->Physics && Candidate->Physics->VM?Candidate->Physics->VM->BindingRevision():0;
            if(Session->Revision && !ParentLifecycle && !Candidate->bPhysicsForeignSession)UDasAsyncPhysicsBridge::RevokeOwnerExcept(Session->Owner.Get(),NewPhysicsRevision);
            Session->Live = MoveTemp(Candidate);
            if(Session->Live->bPhysicsOwner){if(Session->Live->Physics)DSLPhysicsOwners.Add(Self,Session->Live->Physics);else DSLPhysicsOwners.Remove(Self);}
            Session->ApprovedDSLDeclaration.Empty();Session->bDefaultsUpdateApproved=false;
            InstallSubscriptions(*Session);
            // Old physics callbacks were revoked before committing the staged replacement.
            ++Session->Revision;
            Session->ReloadError.Empty();
        }
        else
        {
            Session->ReloadError = CandidateError.IsEmpty() ? TEXT("Candidate initialization failed.") : CandidateError;
            UE_LOG(LogDasRuntime, Display, TEXT("Reload rejected for %s; keeping revision %u: %s"), *FullPath, Session->Revision, *Session->ReloadError);
        }
    }
    Result.Revision = Session->Revision;
    Result.ReloadError = Session->ReloadError;
    if (Event == EDasCallback::EndPlay && (!Session->Live || !Session->bBeginDelivered || Session->bEndDelivered))
    {
        ClearSessionTimers(*Session);
        Session->Bindings.Empty();Session->InputContexts.Empty();Session->InputRecipes.Empty();Session->ContextRecipes.Empty();
        Session->bEndDelivered = true;
        Result.bSucceeded = true;
        return Result;
    }
    if (!Session->Live) { Result.Error = Session->ReloadError; return Result; }
    if (Session->bEndDelivered)
    {
        Result.Error = TEXT("Script lifecycle has already ended.");
        return Result;
    }
    auto& Context = *Session->Live->Context;
    if(!Reentering){Context.restart();Context.Types.Reset();}
    TGuardValue<int32> ExecutionScope(Context.ExecutionDepth,Context.ExecutionDepth+1);
    FActiveContextScope Scope(Context);
    FStructCollectOnExit CollectStructs{Context};
    TGuardValue<bool> EndingScope(Context.bEnding, Event == EDasCallback::EndPlay);
    try
    {
        if(Event==EDasCallback::Construction){
            if(Session->Live->Construction)Context.evalWithCatch(Session->Live->Construction,nullptr);
            if(const char* Exception=Context.getException()){Result.Error=FString(TEXT("ConstructionScript failed: "))+UTF8_TO_TCHAR(Exception);return Result;}
            Result.bSucceeded=true;return Result;
        }
        if(ExplicitParentTick && Event==EDasCallback::Tick && Session->Live->BeginPlay && !Session->bBeginDelivered){Result.Error=TEXT("Call Super.BeginPlay before parent Tick");return Result;}
        // Parent BeginPlay does not implicitly opt back into an overridden initializer.
        const bool CanInitialize=Owner->GetWorld() && Owner->GetWorld()->IsGameWorld() && !Owner->IsTemplate();
        if(!CanInitialize && Session->Live->Initializer && (Event==EDasCallback::BeginPlay || Event==EDasCallback::Tick)){Result.Error=TEXT("Initialize requires a live game-world instance");return Result;}
        const bool NeedInitialize=Event==EDasCallback::Initialize || (CanInitialize && !ParentLifecycle && (Event==EDasCallback::BeginPlay || Event==EDasCallback::Tick));
        if(NeedInitialize){
            if(!Owner->GetWorld() || !Owner->GetWorld()->IsGameWorld() || Owner->IsTemplate()){Result.Error=TEXT("Initialize requires a live game-world instance");return Result;}
            const auto* ExistingOwner=InitializerDeliveries.Find(Self);const auto* Existing=ExistingOwner?ExistingOwner->Find(FullPath):nullptr;
            if(Existing && Existing->State==EInitializerDelivery::Running){Result.Error=TEXT("Recursive object initialization is forbidden");return Result;}
            if(!Existing){
                InitializerDeliveries.FindOrAdd(Self).Add(FullPath,FInitializerDelivery{});
                bool Initialized=false;
                ON_SCOPE_EXIT{if(!Initialized){auto& Delivery=InitializerDeliveries.FindOrAdd(Self).FindOrAdd(FullPath);Delivery.State=EInitializerDelivery::Failed;Delivery.Error=Result.Error.IsEmpty()?TEXT("Object initialization failed; recreate the instance"):Result.Error;for(const auto& S:Sessions)if(S->Owner==Self){ClearSessionTimers(*S);S->Bindings.Empty();S->InputContexts.Empty();S->InputRecipes.Empty();S->ContextRecipes.Empty();}}};
                TGuardValue<bool> Initializing(Context.bInitializingObject,true);
                if(Session->Live->Initializer)Context.evalWithCatch(Session->Live->Initializer,nullptr);
                if(const char* Exception=Context.getException()){Result.Error=FString(TEXT("Initialize failed: "))+UTF8_TO_TCHAR(Exception);return Result;}
                if(!IsValid(Owner) || (Cast<AActor>(Owner) && CastChecked<AActor>(Owner)->IsActorBeingDestroyed()) || (Cast<UActorComponent>(Owner) && CastChecked<UActorComponent>(Owner)->IsBeingDestroyed())){Result.Error=TEXT("Owner destroyed during Initialize");return Result;}
                InitializerDeliveries.FindOrAdd(Self).FindOrAdd(FullPath).State=EInitializerDelivery::Complete;Initialized=true;
            }
            if(Event==EDasCallback::Initialize){Result.bSucceeded=true;return Result;}
        }
        // Delivery flags belong to the session, so swapping code cannot replay BeginPlay.
        if ((Event == EDasCallback::BeginPlay || Event == EDasCallback::Tick) && !Session->bBeginDelivered)
        {
            Session->bBeginDelivered = true;
            if (Session->Live->BeginPlay) Context.evalWithCatch(Session->Live->BeginPlay, nullptr);
            if (const char* Exception = Context.getException())
            {
                Result.Error = UTF8_TO_TCHAR(Exception);
                if(Session->Live->Physics){Session->Live->Physics->Activated=true;Session->Live->Physics->ActivationError=TEXT("Async Physics registration blocked after failed BeginPlay: ")+Result.Error;}
                return Result;
            }
        }
        das::SimFunction* Function = nullptr;
        vec4f Arguments[] = { Event==EDasCallback::Tick || (Event==EDasCallback::Main && Signature==EDasMethodSignature::Float)?das::cast<float>::from(DeltaSeconds):Event==EDasCallback::Main && Signature==EDasMethodSignature::Bool?das::cast<bool>::from(Input!=0):das::cast<int32>::from(Input) };
        if (Event == EDasCallback::Main)
        {
            const FTCHARToUTF8 EntryUtf8(*EntryPoint);
            Function=Context.findFunction(EntryUtf8.Get());
            if (!Function || !VerifyContract(Session->Live->Program,EntryPoint,Expected))
            { Result.Error=TEXT("Expected exported ")+EntryPoint+TEXT(" with requested scalar signature.");return Result; }
            Session->RequiredEntries.Add(EntryPoint,Expected);if(InputSnapshotDispatch)Session->InputSnapshotEntries.Add(EntryPoint);
        }
        else if (Event == EDasCallback::Tick) Function = Session->Live->Tick;
        else if (Event == EDasCallback::EndPlay)
        {
            ClearSessionTimers(*Session);
            Session->bEndDelivered = true;
            Session->Bindings.Empty();Session->InputContexts.Empty();Session->InputRecipes.Empty();Session->ContextRecipes.Empty();
            Function = Session->Live->EndPlay;
        }
        if (Function)
        {
            das::vector<vec4f> Packed;TArray<FDasObjectHandle> ObjectArgs;if(Frame)ObjectArgs.Reserve(Frame->Num());FDasStructValue ReturnedStructToken;TArray<FDasStructValue> StructArgs;if(Frame)StructArgs.Reserve(Frame->Num());FDasObjectHandle ReturnedObject;TArray<FVector> VectorArgs;TArray<FRotator> RotatorArgs;TArray<FTransform> TransformArgs;if(Frame){VectorArgs.Reserve(Frame->Num());RotatorArgs.Reserve(Frame->Num());TransformArgs.Reserve(Frame->Num());}FVector ReturnedVector=FVector::ZeroVector;FRotator ReturnedRotator=FRotator::ZeroRotator;FTransform ReturnedTransform=FTransform::Identity;
            if(Frame) for(const auto& V:*Frame){
                if(V.Kind==17){Packed.push_back(das::cast<uint8_t>::from(V.ByteValue));}else if(V.Kind==18){Packed.push_back(das::cast<FDasTextValue>::from(MakeDSLText(V.TextValue,&Context)));}else if(V.Kind==15){Packed.push_back(das::cast<FDasClassHandle>::from(FDasClassHandle{V.ClassValue?MakeObjectHandle(V.ClassValue.Get(),UClass::StaticClass(),&Context).Token:0}));}else if(V.Kind==16){auto D=ResolveDelegateValue(0,TCHAR_TO_UTF8(*V.ObjectClass),false,&Context);D.Single=V.DelegateValue;Packed.push_back(das::cast<uint64>::from(StoreDelegateValue(MoveTemp(D),&Context)));}else if(V.Kind==0)Packed.push_back(das::cast<int32>::from(V.IntValue));else if(V.Kind==1)Packed.push_back(das::cast<float>::from(V.FloatValue));else if(V.Kind==2)Packed.push_back(das::cast<bool>::from(V.BoolValue));
                else if(V.Kind==5)Packed.push_back(das::cast<char*>::from(CopyDSLString(V.StringValue,&Context)));else if(V.Kind==6)Packed.push_back(das::cast<double>::from(V.DoubleValue));else if(V.Kind==13)Packed.push_back(das::cast<FDasEnumValue>::from(MakeEnumValue(V.ObjectClass,V.IntValue,&Context)));else if(V.Kind==7)Packed.push_back(das::cast<FDasNameValue>::from(MakeDSLName(V.NameValue,&Context)));else if(V.Kind==9){VectorArgs.Add(V.VectorValue);Packed.push_back(das::cast<void*>::from(&VectorArgs.Last()));}else if(V.Kind==10){RotatorArgs.Add(V.RotatorValue);Packed.push_back(das::cast<void*>::from(&RotatorArgs.Last()));}else if(V.Kind==12){StructArgs.Add(StoreStructValue(V.StructValue,&Context));Packed.push_back(das::cast<void*>::from(&StructArgs.Last()));}else if(V.Kind==11){TransformArgs.Add(V.TransformValue);Packed.push_back(das::cast<void*>::from(&TransformArgs.Last()));}else{const FString ClassPath=V.ObjectClass.IsEmpty()?TEXT("/Script/CoreUObject.Object"):V.ObjectClass;ObjectArgs.Add(MakeObjectHandle(V.ObjectValue.Get(),FindObject<UClass>(nullptr,*ClassPath),&Context));Packed.push_back(das::cast<void*>::from(&ObjectArgs.Last()));}
            }
            FOutputFrame OutFrame;OutFrame.Arguments=Frame;TGuardValue<FOutputFrame*> OutScope(ActiveOutputFrame,&OutFrame);
            const auto Value = Context.evalWithCatch(Function,Frame?(Packed.empty()?nullptr:Packed.data()):Event==EDasCallback::Main && Signature==EDasMethodSignature::Void?nullptr:Arguments,Signature==EDasMethodSignature::Struct?static_cast<void*>(&ReturnedStructToken):Signature==EDasMethodSignature::Object?static_cast<void*>(&ReturnedObject):Signature==EDasMethodSignature::Vector?static_cast<void*>(&ReturnedVector):Signature==EDasMethodSignature::Rotator?static_cast<void*>(&ReturnedRotator):Signature==EDasMethodSignature::Transform?static_cast<void*>(&ReturnedTransform):nullptr);
            if(Event==EDasCallback::Main){
                if(Signature==EDasMethodSignature::Byte && !Context.getException()){Result.ByteValue=das::cast<uint8_t>::to(Value);}else if(Signature==EDasMethodSignature::Text && !Context.getException()){Result.TextValue=ResolveDSLText(das::cast<FDasTextValue>::to(Value),&Context);}else if(Signature==EDasMethodSignature::Class && !Context.getException()){auto H=das::cast<FDasClassHandle>::to(Value);UObject* O=H.Token?TryObject(FDasObjectHandle{H.Token},&Context):nullptr;auto* K=Cast<UClass>(O);if((H.Token && !K) || !FrameClassMatches(K,Expected.ReturnClass)){Result.Error=TEXT("Class result is expired, belongs to another context or has wrong base");return Result;}Result.ObjectValue=K;}else if(Signature==EDasMethodSignature::Delegate && !Context.getException()){Result.DelegateValue=ResolveDelegateValue(das::cast<uint64>::to(Value),TCHAR_TO_UTF8(*Expected.ReturnClass),false,&Context).Single;}else if(Signature==EDasMethodSignature::Enum && !Context.getException()){const auto& E=ResolveEnumValue(das::cast<FDasEnumValue>::to(Value),&Context);if(E.Type->GetPathName()!=Expected.ReturnClass){Result.Error=TEXT("Enum result type mismatch");return Result;}Result.Value=E.Value;}else if(Signature==EDasMethodSignature::Struct && !Context.getException()){const auto& S=ResolveStructValue(ReturnedStructToken,&Context);if(S.GetScriptStruct()->GetPathName()!=Expected.ReturnClass){Result.Error=TEXT("Reflected struct result type mismatch");return Result;}Result.StructValue=S;}else if(Signature==EDasMethodSignature::Vector)Result.VectorValue=ReturnedVector;else if(Signature==EDasMethodSignature::Rotator)Result.RotatorValue=ReturnedRotator;else if(Signature==EDasMethodSignature::Transform)Result.TransformValue=ReturnedTransform;else if(Signature==EDasMethodSignature::Int) Result.Value=das::cast<int32>::to(Value);
                else if(Signature==EDasMethodSignature::Float) Result.FloatValue=das::cast<float>::to(Value);
                else if(Signature==EDasMethodSignature::Bool) Result.BoolValue=das::cast<bool>::to(Value);
                else if(!Context.getException() && Signature==EDasMethodSignature::String){const auto* Text=das::cast<char*>::to(Value);Result.StringValue=Text?UTF8_TO_TCHAR(Text):TEXT("");}
                else if(Signature==EDasMethodSignature::Double)Result.DoubleValue=das::cast<double>::to(Value);
                else if(!Context.getException() && Signature==EDasMethodSignature::Name)Result.NameValue=ResolveDSLName(das::cast<FDasNameValue>::to(Value),&Context);
                else if(!Context.getException() && Signature==EDasMethodSignature::Object){UObject* Object=ReturnedObject.Token?TryObject(ReturnedObject,&Context):nullptr;if(ReturnedObject.Token && !Object){Result.Error=TEXT("Object result is expired or belongs to another context");return Result;}UClass* Class=FindObject<UClass>(nullptr,*Expected.ReturnClass);if(Object && !DasReferenceMatches(Object,Class)){Result.Error=TEXT("Object result has wrong class");return Result;}Result.ObjectValue=Object;}
            }
            if(Frame && !Context.getException()){TArray<FDasScalarValue> Published=*Frame;for(int32 I=0;I<Frame->Num();++I)if((*Frame)[I].Direction){auto* Out=OutFrame.Values.Find(I);if(!Out){Result.Error=TEXT("Declared output slot not published");return Result;}Published[I]=*Out;}if(!Expected.InputDirections.IsEmpty())PendingOutputs=MoveTemp(Published);}
        }
        if((Signature==EDasMethodSignature::Vector && Result.VectorValue.ContainsNaN()) || (Signature==EDasMethodSignature::Rotator && Result.RotatorValue.ContainsNaN()) || (Signature==EDasMethodSignature::Transform && (Result.TransformValue.ContainsNaN() || !Result.TransformValue.IsRotationNormalized()))){Result.Error=TEXT("Invalid/non-finite math struct result");return Result;}
        if (const char* Exception = Context.getException()) Result.Error = UTF8_TO_TCHAR(Exception);
        else {
            if(!ParentLifecycle && (Event==EDasCallback::BeginPlay || Event==EDasCallback::Tick) && Session->Live->Physics && DSLPhysicsOwnerReady(Owner) && !ActivateDSLPhysics(*Session->Live->Physics,Owner,Result.Error))return Result;
            Result.Types = Context.Types; Result.bSucceeded = true;Result.OutputValues=MoveTemp(PendingOutputs);
        }
    }
    catch (const std::exception& Exception) { Result.Error = UTF8_TO_TCHAR(Exception.what()); }
    return Result;
}

IMPLEMENT_MODULE(FDasRuntimeModule, DasRuntime)

#include "DasBindingExport.inl"
#include "DasNativeGameplayPolicy.inl"
#include "DasNativeGameplayExport.inl"
#include "DasNativeGameplayCoverage.inl"

#if WITH_EDITOR
bool FDasRuntimeModule::BeginDefaultUpdate(){
    if(!bInitialized || !IsInGameThread() || ActiveContext || bDrainingEvents || bPreservingSessions || !PendingEvents.IsEmpty())return false;
    for(const auto& S:Sessions)if(S->Live && !S->Live->Context->Calls.IsEmpty())return false;
    UDasAsyncPhysicsBridge::RevokeAll();
    bPreservingSessions=true;return true; // Do not detach sessions, timers or subscriptions.
}
void FDasRuntimeModule::EndDefaultUpdate(UClass* RootClass,bool Applied){
    if(!bPreservingSessions || !IsInGameThread())return;
    if(Applied)for(const auto& S:Sessions)if(S->Owner.IsValid() && S->Owner->IsA(RootClass)){S->bDefaultsUpdateApproved=true;S->AttemptedSources.Reset();}
    bPreservingSessions=false;
}
bool FDasRuntimeModule::BeginPreserveContextsForReinstance(UClass* RootClass){
    if(!bInitialized || !IsInGameThread() || ActiveContext || bDrainingEvents || bPreservingSessions || !RootClass || !PendingEvents.IsEmpty())return false;
    PruneSessions();
    // Call frames are invocation-local and own reflected addresses. Require an
    // empty table at this idle boundary before any host layout can change.
    for(const auto& S:Sessions)if(S->Live && !S->Live->Context->Calls.IsEmpty())return false;
    UDasAsyncPhysicsBridge::RevokeAll();
    bPreservingSessions=true;
    // Suspend all contexts: an unaffected owner can subscribe to a replaced peer.
    PreservedSessions=MoveTemp(Sessions);
    for(auto& S:PreservedSessions){
        S->bReinstanceOwnerInScope=S->Owner.IsValid() && S->Owner->IsA(RootClass);
        S->bReinstanceOwnerChanged=false;S->ReinstanceLifecyclePath.Empty();
        auto RememberLifecycle=[&](auto* A){if(A){if(A->bLifecycleActive)S->ReinstanceLifecyclePath=A->ActiveLifecyclePath;S->bReinstanceTickEnabled=A->IsActorTickEnabled();}};
        RememberLifecycle(Cast<ADasProbeActor>(S->Owner.Get()));RememberLifecycle(Cast<ADasPawn>(S->Owner.Get()));RememberLifecycle(Cast<ADasCharacter>(S->Owner.Get()));
        ClearSessionTimers(*S);
        S->Bindings.Empty();S->InputContexts.Empty(); // Remove old proxies before Unreal copies delegate properties.
    }
    return true;
}
void FDasRuntimeModule::EndPreserveContextsForReinstance(const FString& ApprovedDeclaration){
    if(!IsInGameThread() || !bPreservingSessions)return;
    for(auto& Saved:PreservedSessions)if(Saved->Owner.IsValid() && !Saved->bReleaseRequested){
        if(auto* Actor=Cast<AActor>(Saved->Owner.Get());Actor && Actor->IsActorBeingDestroyed())continue;
        auto RestoreLifecycle=[&](auto* Actor){if(Actor && Saved->bReinstanceOwnerChanged && !Saved->ReinstanceLifecyclePath.IsEmpty() && !Saved->bEndDelivered){Actor->ActiveLifecyclePath=Saved->ReinstanceLifecyclePath;Actor->bLifecycleActive=true;Actor->SetActorTickEnabled(Saved->bReinstanceTickEnabled);}};
        RestoreLifecycle(Cast<ADasProbeActor>(Saved->Owner.Get()));RestoreLifecycle(Cast<ADasPawn>(Saved->Owner.Get()));RestoreLifecycle(Cast<ADasCharacter>(Saved->Owner.Get()));
        if(Saved->bReinstanceOwnerInScope)Saved->ApprovedDSLDeclaration=ApprovedDeclaration;
        if(Saved->bReinstanceOwnerInScope || Saved->bReinstanceOwnerChanged)Saved->AttemptedSources.Reset();
        if(Saved->Live && !Saved->bEndDelivered)InstallSubscriptions(*Saved);
        Saved->ReinstanceLifecyclePath.Empty();Sessions.Add(MoveTemp(Saved));
    }
    PreservedSessions.Empty();bPreservingSessions=false;
}
#endif

FDasExecutionResult FDasRuntimeModule::RunLibraryFrame(const FString& Contract,const FString& Script,const FString& Entry,const TArray<FDasScalarValue>& Arguments,EDasMethodSignature Kind,const FString& ReturnClass){
 FDasExecutionResult Failed;FDasAuthoringModule Model;FDasClassSchema Schema;
 if(!IsInGameThread() || !bInitialized || Script!=FPaths::GetCleanFilename(Script) || FPaths::GetExtension(Script)!=TEXT("das") || Contract.Len()>1048576 || !FDasAuthoringModule::FromJson(Contract,Model,Failed.Error) || Model.Classes.Num()!=1 || !Model.ToActorSchema(Model.Classes[0].Id,Schema,Failed.Error) || Schema.Parent!=TEXT("FunctionLibrary")){if(Failed.Error.IsEmpty())Failed.Error=TEXT("Invalid static library invocation contract");return Failed;}
 const auto* Method=Schema.Methods.FindByPredicate([&](const auto& M){return M.EntryPoint==Entry && M.ScriptFile==Script && M.ReturnKind==Kind && M.ReturnClass==ReturnClass;});
 if(!Method){Failed.Error=TEXT("Static library entry/result differs from saved declaration");return Failed;}
 UObject* Owner=GetMutableDefault<UDasBlueprintLibrary>();if(!Method->WorldContext.IsEmpty()){const int32 Index=Method->Parameters.IndexOfByPredicate([&](const auto& P){return P.Name==Method->WorldContext;});if(!Arguments.IsValidIndex(Index) || Arguments[Index].Kind!=8){Failed.Error=TEXT("Missing library WorldContext object");return Failed;}Owner=Arguments[Index].ObjectValue.Get();auto* World=IsValid(Owner) && GEngine?GEngine->GetWorldFromContextObject(Owner,EGetWorldErrorMode::ReturnNull):nullptr;if(!World || !World->IsGameWorld() || World->bIsTearingDown || Owner->IsTemplate() || Owner->GetWorld()!=World || (Cast<AActor>(Owner) && CastChecked<AActor>(Owner)->IsActorBeingDestroyed()) || (Cast<UActorComponent>(Owner) && CastChecked<UActorComponent>(Owner)->IsBeingDestroyed())){Failed.Error=TEXT("Library requires a live game-world WorldContext");return Failed;}}
 TGuardValue<FString> ScriptScope(LibraryInvocationScript,Script);TGuardValue<const FString*> Scope(LibraryInvocationContract,&Contract);
 return RunNamedFramePersistent(ScriptPath(Script),Entry,Arguments,Kind,Owner,ReturnClass);
}
#include "DasPerformanceProbe.inl"

#include "DasExecutionGuardProbe.inl"

#include "DasAsyncPhysicsVM.inl"
